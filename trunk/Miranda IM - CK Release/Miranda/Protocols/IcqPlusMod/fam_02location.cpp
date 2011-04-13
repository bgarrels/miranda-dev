// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin Öberg, Sam Kothari, Robert Rainwater
// Copyright © 2004,2005,2006,2007 Joe Kucera
// Copyright © 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// -----------------------------------------------------------------------------
//
// File name      : $Source$
// Revision       : $Revision: 36 $
// Last change on : $Date: 2007-08-05 03:45:10 +0300 (Ð’Ñ, 05 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"


static void handleLocationUserInfoReply(BYTE* buf, WORD wLen, DWORD dwCookie);

const char* cliSpamBot;
extern char* detectUserClient(HANDLE hContact, int nIsICQ, DWORD dwUin, WORD wUserClass, WORD wVersion, DWORD dwFT1, DWORD dwFT2, DWORD dwFT3, DWORD dwOnlineSince, BYTE bDirectFlag, DWORD dwDirectCookie, DWORD dwWebPort, BYTE* caps, WORD wLen, BYTE* bClientId, char* szClientBuf);


void handleLocationFam(unsigned char *pBuffer, WORD wBufferLength, snac_header* pSnacHeader)
{
    switch (pSnacHeader->wSubtype)
    {

    case ICQ_LOCATION_RIGHTS_REPLY: // Reply to CLI_REQLOCATION
        NetLog_Server("Server sent SNAC(x02,x03) - SRV_LOCATION_RIGHTS_REPLY");
        break;

    case ICQ_LOCATION_USR_INFO_REPLY: // AIM user info reply
        handleLocationUserInfoReply(pBuffer, wBufferLength, pSnacHeader->dwRef);
        break;

    case ICQ_ERROR:
    {
        WORD wError;
        HANDLE hCookieContact;
        fam15_cookie_data *pCookieData;


        if (wBufferLength >= 2)
            unpackWord(&pBuffer, &wError);
        else
            wError = 0;

        if (wError == 4)
        {
            if (FindCookie(pSnacHeader->dwRef, &hCookieContact, (void**)&pCookieData))
                if(hCookieContact && (hCookieContact != INVALID_HANDLE_VALUE))
                    if(pCookieData)
                        if(!getContactUin(hCookieContact) && pCookieData->bRequestType == REQUESTTYPE_PROFILE)
                        {
                            BroadcastAck(hCookieContact, ACKTYPE_GETINFO, ACKRESULT_FAILED, (HANDLE)1 ,0);

                            ReleaseCookie(pSnacHeader->dwRef);
                        }
        }

        LogFamilyError(ICQ_LOCATION_FAMILY, wError);
        break;
    }

    default:
        NetLog_Server("Warning: Ignoring SNAC(x%02x,x%02x) - Unknown SNAC (Flags: %u, Ref: %u)", ICQ_LOCATION_FAMILY, pSnacHeader->wSubtype, pSnacHeader->wFlags, pSnacHeader->dwRef);
        break;
    }
}



static char* AimApplyEncoding(char* pszStr, const char* pszEncoding)
{
    // decode encoding to ANSI only
    if (pszStr && pszEncoding)
    {
        const char *szEnc = strstr(pszEncoding, "charset=");

        if (szEnc)
        {
            // decode custom encoding to Utf-8
            char* szStr = ApplyEncoding(pszStr, szEnc + 9);
            // decode utf-8 to ansi
            char *szRes = NULL;

            mir_free(pszStr);
            szRes = mir_utf8decodeA(szStr);
            mir_free(szStr);

            return szRes;
        }
    }
    return pszStr;
}

extern void extractMoodData(oscar_tlv_chain* pChain, char** pMood, int* cbMood);

void handleLocationUserInfoReply(BYTE* buf, WORD wLen, DWORD dwCookie)
{
    HANDLE hContact;
    DWORD dwUIN;
    uid_str szUID;
    WORD wTLVCount;
    WORD wWarningLevel;
    HANDLE hCookieContact;
    WORD status;
    WORD wStatusFlags;

    cookie_message_data *pCookieData;

    // Unpack the sender's user ID
    if (!unpackUID(&buf, &wLen, &dwUIN, &szUID)) return;

    // Syntax check
    if (wLen < 4)
        return;

    // Warning level?
    unpackWord(&buf, &wWarningLevel);
    wLen -= 2;

    // TLV count
    unpackWord(&buf, &wTLVCount);
    wLen -= 2;

    // Determine contact
    hContact = HContactFromUID(dwUIN, szUID, NULL);

    // Ignore away status if the user is not already on our list
    if (hContact == INVALID_HANDLE_VALUE)
    {
#ifdef _DEBUG
        NetLog_Server("Ignoring away reply (%s)", strUID(dwUIN, szUID));
#endif
        return;
    }

    setSettingWord(hContact, "AOLWarningLevel", wWarningLevel); //maybe we display it somewhere later

    if (!FindCookie(dwCookie, &hCookieContact, (void**)&pCookieData))
    {
        NetLog_Server("Error: Received unexpected away reply from %s", strUID(dwUIN, szUID));
        return;
    }

    if (hContact != hCookieContact)
    {
        NetLog_Server("Error: Away reply Contact does not match Cookie Contact(0x%x != 0x%x)", hContact, hCookieContact);

        ReleaseCookie(dwCookie); // This could be a bad idea, but I think it is safe
        return;
    }

    switch (GetCookieType(dwCookie))
    {
    case CKT_FAMILYSPECIAL:
    {
        ReleaseCookie(dwCookie);

        // Read user info TLVs
        {
            oscar_tlv_chain* pChain;
            oscar_tlv* pTLV;
            BYTE *tmp;
            char *szMsg = NULL;

            // Syntax check
            if (wLen < 4)
                return;

            tmp = buf;
            // Get general chain
            if (!(pChain = readIntoTLVChain(&buf, wLen, wTLVCount)))
                return;

            disposeChain(&pChain);

            wLen -= (buf - tmp);

            // Get extra chain
            if (pChain = readIntoTLVChain(&buf, wLen, 2))
            {
                char* szEncoding = NULL;

                // Get Profile encoding TLV
                pTLV = getTLV(pChain, 0x01, 1);
                if (pTLV && (pTLV->wLen >= 1))
                {
                    szEncoding = (char*)icq_alloc_zero(pTLV->wLen + 1);
                    memcpy(szEncoding, pTLV->pData, pTLV->wLen);
                    szEncoding[pTLV->wLen] = '\0';
                }
                // Get Profile info TLV
                pTLV = getTLV(pChain, 0x02, 1);
                if (pTLV && (pTLV->wLen >= 1))
                {
                    szMsg = (char*)icq_alloc_zero(pTLV->wLen + 2);
                    memcpy(szMsg, pTLV->pData, pTLV->wLen);
                    szMsg[pTLV->wLen] = '\0';
                    szMsg[pTLV->wLen + 1] = '\0';
                    szMsg = AimApplyEncoding(szMsg, szEncoding);
                    szMsg = EliminateHtml(szMsg, pTLV->wLen);
                }
                // Free TLV chain
                disposeChain(&pChain);
            }

            setSettingString(hContact, "About", szMsg);
            BroadcastAck(hContact, ACKTYPE_GETINFO, ACKRESULT_SUCCESS, (HANDLE)1 ,0);

            mir_free(szMsg);
        }
        break;
    }

    case CKT_CHECKSPAMBOT:
    {
        ReleaseCookie(dwCookie);

        // Read user info TLVs
        {
            oscar_tlv_chain* pChain;
            oscar_tlv* pTLV;
            BYTE *tmp;
            WORD wClass;
            WORD wVersion = 0;
            DWORD dwFT1 = 0, dwFT2 = 0, dwFT3 = 0;
            DWORD dwOnlineSince;
            BYTE nTCPFlag = 0;
            DWORD dwDirectConnCookie = 0;
            DWORD dwWebPort = 0;
            BYTE* capBuf = NULL;
            WORD capLen = 0;
            char szStrBuf[MAX_PATH];
            BYTE bClientId = 0;
            char *szClient;
            int nIsICQ;
            DWORD dwMemberSince;
            WORD wIdleTimer;
            DWORD dwIP = 0;
            DWORD dwRealIP = 0;
            DWORD dwPort = 0;
            time_t tIdleTS = 0;


            // Syntax check
            if (wLen < 4)
                return;

            tmp = buf;
            // Get general chain
            if (!(pChain = readIntoTLVChain(&buf, wLen, wTLVCount)))
                return;

            // Get Class word
            wClass = getWordFromChain(pChain, 0x01, 1);
            nIsICQ = wClass & CLASS_ICQ;

            if (dwUIN)
            {
                // Get DC info TLV
                pTLV = getTLV(pChain, 0x0C, 1);
                SetContactCapabilities(hContact, WAS_FOUND); //mark contact for asd
                if (pTLV && (pTLV->wLen >= 15))
                {
                    BYTE* pBuffer;

                    pBuffer = pTLV->pData;
                    nIsICQ = TRUE;
                    unpackDWord(&pBuffer, &dwRealIP);
                    unpackDWord(&pBuffer, &dwPort);
                    unpackByte(&pBuffer,  &nTCPFlag);
                    unpackWord(&pBuffer,  &wVersion);
                    unpackDWord(&pBuffer, &dwDirectConnCookie);
                    unpackDWord(&pBuffer, &dwWebPort); // Web front port
                    pBuffer += 4; // Client features

                    // Get faked time signatures, used to identify clients
                    if (pTLV->wLen >= 0x23)
                    {
                        unpackDWord(&pBuffer, &dwFT1);
                        unpackDWord(&pBuffer, &dwFT2);
                        unpackDWord(&pBuffer, &dwFT3);
                    }
                }
            }
            else
                nIsICQ = FALSE;

            // Get Online Since TLV
            dwOnlineSince = getDWordFromChain(pChain, 0x03, 1);

            disposeChain(&pChain);

            wLen -= (buf - tmp);

            // Get extra chain
            if (pChain = readIntoTLVChain(&buf, wLen, 2))
            {
                pTLV = getTLV(pChain, 0x05, 1);
                if (pTLV && (pTLV->wLen > 0))
                {
                    capBuf = pTLV->pData;
                    capLen = pTLV->wLen;
                }
            }
            szClient = detectUserClient(hContact, nIsICQ, dwUIN, wClass, wVersion, dwFT1, dwFT2, dwFT3, dwOnlineSince, nTCPFlag, dwDirectConnCookie, dwWebPort, capBuf, capLen, &bClientId, szStrBuf);


            if (szClient && ( int )szClient != -1 )
            {
                if (szClient == cliSpamBot||szClient == "Virus")
                {
                    if (DBGetContactSettingByte(hContact, "CList", "NotOnList", 0)&& getSettingByte(NULL, "KillSpambots", DEFAULT_KILLSPAM_ENABLED))
                    {
                        // kill spammer
                        icq_DequeueUser(dwUIN);
                        AddToSpammerList(dwUIN);
                        if (bSpamPopUp)
                            ShowPopUpMsg(hContact, dwUIN, "Spambot Detected", "Contact deleted & further events blocked.", POPTYPE_SPAM);
                        CallService(MS_DB_CONTACT_DELETE, (WPARAM)hContact, 0);
                        NetLog_Server("Contact %s deleted", strUID(dwUIN, szUID));
                    }
                    disposeChain(&pChain);
                    break;
                }
                else if (!strcmp(szClient, "Unknown"))
                {
                    if (getSettingByte(NULL, "KillUnknown", 0) && DBGetContactSettingByte(hContact, "CList", "NotOnList", 0))
                    {
                        icq_DequeueUser(dwUIN);
                        AddToSpammerList(dwUIN);
                        if (bUnknownPopUp)
                            ShowPopUpMsg(hContact, dwUIN, "Unknown Detected", "Contact deleted & further events blocked.", POPTYPE_UNKNOWN);
                        CallService(MS_DB_CONTACT_DELETE, (WPARAM)hContact, 0);
                        NetLog_Server("Contact %u deleted", dwUIN);
                    }
                }
            }

            // Get IP TLV
            dwIP = getDWordFromChain(pChain, 0x0a, 1);

            // Get Member Since TLV
            dwMemberSince = getDWordFromChain(pChain, 0x05, 1);

            // Get Idle timer TLV
            wIdleTimer = getWordFromChain(pChain, 0x04, 1);
            if (wIdleTimer)
            {
                time(&tIdleTS);
                tIdleTS -= (wIdleTimer*60);
            };


            if (pTLV && (pTLV->wLen >= 4))
            {
                unsigned char* pBuffer;
                pBuffer = pTLV->pData;
                unpackWord(&pBuffer, &wStatusFlags);
                unpackWord(&pBuffer, &status);
            }

            {
                // handle Xtraz status
                char* moodData = NULL;
                int moodSize = 0;

                int unpackSessionDataItem(oscar_tlv_chain *pChain, WORD wItemType, BYTE **ppItemData, WORD *pwItemSize, BYTE *pbItemFlags);
//            extractMoodData(pChain, &moodData, &moodSize);
                unpackSessionDataItem(pChain, 0x0E, (BYTE**)&moodData, (WORD*)&moodSize, NULL);
                handleXStatusCaps(hContact, (char*)capBuf, capLen, moodData, moodSize);
            }

            if(dwFT1)
                setSettingDword(hContact,  "dwFT1",   dwFT1);
            if(dwFT2)
                setSettingDword(hContact,  "dwFT2",   dwFT2);
            if(dwFT3)
                setSettingDword(hContact,  "dwFT3",   dwFT3);
            if(capBuf)
            {
                // store client capabilities
                DBCONTACTWRITESETTING dbcws;
                dbcws.value.type = DBVT_BLOB;
                dbcws.value.cpbVal = capLen;
                dbcws.value.pbVal = capBuf;
                dbcws.szModule = ICQ_PROTOCOL_NAME;
                dbcws.szSetting = "CapBuf";
                CallService(MS_DB_CONTACT_WRITESETTING, (WPARAM)hContact, (LPARAM)&dbcws);
            }
            else
                deleteSetting(hContact, "CapBuf"); //workaround bug in detecting clients without caps
            if (pTLV && (pTLV->wLen >= 16))
            {
                // handle Xtraz status
                char* moodData = NULL;
                int moodSize = 0;

                extractMoodData(pChain, &moodData, &moodSize);
                handleXStatusCaps(hContact, (char*)(pTLV->pData), pTLV->wLen, moodData, moodSize);
            }
            setSettingDword(hContact, "LogonTS",      dwOnlineSince);
            if (dwMemberSince)
                setSettingDword(hContact, "MemberTS",     dwMemberSince);
            if(dwUIN)
            {
                setSettingDword(hContact, "DirectCookie", dwDirectConnCookie);
                setSettingByte(hContact,  "DCType",       (BYTE)nTCPFlag);
                setSettingWord(hContact,  "UserPort",     (WORD)(dwPort & 0xffff));
                if(wVersion)
                    setSettingWord(hContact,  "Version",      wVersion);
            }
            if (szClient != (char*)-1)
            {
                if(szClient != "Pocket Web 1&1")
                    setSettingStringUtf(hContact,   "MirVer",  szClient);
                setSettingByte(hContact,  "ClientID",     bClientId);
                setSettingDword(hContact, "IP",           dwIP);
                setSettingDword(hContact, "RealIP",       dwRealIP);
            }
            if((status && szClient) && getSettingWord(hContact, "Status", 0) == ID_STATUS_OFFLINE)
            {
                setSettingWord(hContact,  "Status", (WORD)IcqStatusToMiranda(status));
                NetLog_Server("%s changed status to %s (v%d).", strUID(dwUIN, szUID),
                              MirandaStatusToString(IcqStatusToMiranda(status)), wVersion);
                setSettingWord(hContact, "ICQStatus", status);
                {
                    CHECKCONTACT chk = {0};
                    chk.dbeventflag=DBEF_READ;
                    chk.dwUin=dwUIN;
                    chk.hContact=hContact;
                    chk.historyevent=chk.logtofile=TRUE;
                    chk.icqeventtype=ICQEVENTTYPE_WAS_FOUND;
                    chk.msg="detected via ASD";
                    chk.PSD=-1;
                    CheckContact(chk);
                }
            }
            if (!wIdleTimer)
            {
                DWORD dw = getSettingDword(hContact, "IdleTS", 0);
                setSettingDword(hContact, "OldIdleTS", dw);
            }
            setSettingDword(hContact, "IdleTS", tIdleTS);


            // Free TLV chain
            disposeChain(&pChain);


        }
        break;
    }

    default: // away message
    {
        status = AwayMsgTypeToStatus(pCookieData->nAckType);
        if (status == ID_STATUS_OFFLINE)
        {
            NetLog_Server("SNAC(2.6) Ignoring unknown status message from %s", strUID(dwUIN, szUID));

            ReleaseCookie(dwCookie);
            return;
        }

        ReleaseCookie(dwCookie);

        // Read user info TLVs
        {
            oscar_tlv_chain* pChain;
            oscar_tlv* pTLV;
            BYTE *tmp;
            char *szMsg = NULL;
            CCSDATA ccs;
            PROTORECVEVENT pre;

            // Syntax check
            if (wLen < 4)
                return;

            tmp = buf;
            // Get general chain
            if (!(pChain = readIntoTLVChain(&buf, wLen, wTLVCount)))
                return;

            disposeChain(&pChain);

            wLen -= (buf - tmp);

            // Get extra chain
            if (pChain = readIntoTLVChain(&buf, wLen, 2))
            {
                char* szEncoding = NULL;

                // Get Away encoding TLV
                pTLV = getTLV(pChain, 0x03, 1);
                if (pTLV && (pTLV->wLen >= 1))
                {
                    szEncoding = (char*)icq_alloc_zero(pTLV->wLen + 1);
                    memcpy(szEncoding, pTLV->pData, pTLV->wLen);
                    szEncoding[pTLV->wLen] = '\0';
                }
                // Get Away info TLV
                pTLV = getTLV(pChain, 0x04, 1);
                if (pTLV && (pTLV->wLen >= 1))
                {
                    szMsg = (char*)icq_alloc_zero(pTLV->wLen + 2);
                    memcpy(szMsg, pTLV->pData, pTLV->wLen);
                    szMsg[pTLV->wLen] = '\0';
                    szMsg[pTLV->wLen + 1] = '\0';
                    szMsg = AimApplyEncoding(szMsg, szEncoding);
                    szMsg = EliminateHtml(szMsg, pTLV->wLen);
                }
                // Free TLV chain
                disposeChain(&pChain);
            }

            ccs.szProtoService = PSR_AWAYMSG;
            ccs.hContact = hContact;
            ccs.wParam = status;
            ccs.lParam = (LPARAM)&pre;
            pre.flags = 0;
            pre.szMessage = szMsg?szMsg:"";
            pre.timestamp = time(NULL);
            pre.lParam = dwCookie;

            CallService(MS_PROTO_CHAINRECV,0,(LPARAM)&ccs);

            mir_free(szMsg);
        }
        break;
    }
    }
}
