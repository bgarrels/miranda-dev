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
// Revision       : $Revision: 50 $
// Last change on : $Date: 2007-08-28 02:57:00 +0300 (Ð’Ñ‚, 28 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Handles packets from Buddy family
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"
#include "m_cluiframes.h"
static void handleUserOffline(BYTE* buf, WORD wPackLen);
static void handleUserOnline(BYTE* buf, WORD wPackLen, serverthread_info* info);
static void handleReplyBuddy(BYTE* buf, WORD wPackLen);
static void handleNotifyRejected(BYTE* buf, WORD wPackLen);

extern const capstr capAimIcon;
extern const char* cliSpamBot;
extern char* detectUserClient(HANDLE hContact, int nIsICQ, DWORD dwUin, WORD wUserClass, WORD wVersion, DWORD dwFT1, DWORD dwFT2, DWORD dwFT3, DWORD dwOnlineSince, BYTE bDirectFlag, DWORD dwDirectCookie, DWORD dwWebPort, BYTE* caps, WORD wLen, BYTE* bClientId, char* szClientBuf);


void handleBuddyFam(unsigned char* pBuffer, WORD wBufferLength, snac_header* pSnacHeader, serverthread_info *info)
{
    switch (pSnacHeader->wSubtype)
    {
    case ICQ_USER_ONLINE:
        handleUserOnline(pBuffer, wBufferLength, info);
        break;

    case ICQ_USER_OFFLINE:
        handleUserOffline(pBuffer, wBufferLength);
        break;

    case ICQ_USER_SRV_REPLYBUDDY:
        handleReplyBuddy(pBuffer, wBufferLength);
        break;

    case ICQ_USER_NOTIFY_REJECTED:
        handleNotifyRejected(pBuffer, wBufferLength);
        break;

    case ICQ_ERROR:
    {
        WORD wError;

        if (wBufferLength >= 2)
            unpackWord(&pBuffer, &wError);
        else
            wError = 0;

        LogFamilyError(ICQ_BUDDY_FAMILY, wError);
        break;
    }

    default:
        NetLog_Server("Warning: Ignoring SNAC(x%02x,x%02x) - Unknown SNAC (Flags: %u, Ref: %u)", ICQ_BUDDY_FAMILY, pSnacHeader->wSubtype, pSnacHeader->wFlags, pSnacHeader->dwRef);
        break;
    }
}



void extractMoodData(oscar_tlv_chain* pChain, char** pMood, int* cbMood)
{
    oscar_tlv* tlv = getTLV(pChain, 0x1D, 1);
    int len = 0;
    char* data;

    if (tlv)
    {
        len = tlv->wLen;
        data = (char*)tlv->pData;
    }

    while (len >= 4)
    {
        // parse online message items one by one
        WORD itemType = data[0] << 8 | data[1];
        BYTE itemLen = data[3];

        // just some validity check
        if (itemLen + 4 > len)
            itemLen = len - 4;

        if (itemType == 0x0E)
        {
            // mood data
            *pMood = data + 4;
            *cbMood = itemLen;
        }
        data += itemLen + 4;
        len -= itemLen + 4;
    }
}

int unpackSessionDataItem(oscar_tlv_chain *pChain, WORD wItemType, BYTE **ppItemData, WORD *pwItemSize, BYTE *pbItemFlags)
{
    oscar_tlv* tlv = getTLV(pChain, 0x1D, 1);
    int len = 0;
    BYTE* data;

    if (tlv)
    {
        len = tlv->wLen;
        data = tlv->pData;
    }

    while (len >= 4)
    {
        // parse session data items one by one
        WORD itemType;
        BYTE itemFlags;
        BYTE itemLen;

        unpackWord(&data, &itemType);
        unpackByte(&data, &itemFlags);
        unpackByte(&data, &itemLen);
        len -= 4;

        // just some validity check
        if (itemLen > len)
            itemLen = len;

        if (itemType == wItemType)
        {
            // found the requested item
            if (ppItemData)
                *ppItemData = data;
            if (pwItemSize)
                *pwItemSize = itemLen;
            if (pbItemFlags)
                *pbItemFlags = itemFlags;

            return 1; // Success
        }
        data += itemLen;
        len -= itemLen;
    }
    return 0;
}


//this code block must be not here...

static int CListMW_QipExtraIconsRebuild(WPARAM wParam, LPARAM lParam);
static void setContactQipExtraIcon(HANDLE hContact, int qipstatus);


static int CListMW_QipExtraIconsRebuild(WPARAM wParam, LPARAM lParam)
{
    BYTE i;
    extern HICON GetQipStatusIcon(int bStatus, UINT flags);

    if (gbQipStatusEnabled && ServiceExists(MS_CLIST_EXTRA_ADD_ICON))
    {
        for (i = 0; i < SIZEOF(hQIPStatusIcons); i++)
        {
            hQIPStatusIcons[i] = (HANDLE)CallService(MS_CLIST_EXTRA_ADD_ICON, (WPARAM)GetQipStatusIcon(i + 1, LR_SHARED), 0);
        }
    }

    return 0;
}


static int CListMW_QipExtraIconsApply(WPARAM wParam, LPARAM lParam)
{
    if (gbQipStatusEnabled && ServiceExists(MS_CLIST_EXTRA_SET_ICON))
    {
        if (IsICQContact((HANDLE)wParam))
        {
            // only apply icons to our contacts, do not mess others
            setContactQipExtraIcon((HANDLE)wParam, GetQipStatusID((HANDLE)wParam));
        }
    }
    return 0;
}



static int CListMW_QipBuildStatusItems(WPARAM wParam, LPARAM lParam)
{
    InitQipStatusItems();
    return 0;
}

void InitQipStatusEvents()
{
    if (!hHookQStatusBuild)
        if (bQipStatusMenu = ServiceExists(MS_CLIST_ADDSTATUSMENUITEM))
            hHookQStatusBuild = HookEvent(ME_CLIST_PREBUILDSTATUSMENU, CListMW_QipBuildStatusItems);

    if (!hHookQipIconsRebuild)
        hHookQipIconsRebuild = HookEvent(ME_CLIST_EXTRA_LIST_REBUILD, CListMW_QipExtraIconsRebuild);

    if (!hHookQipIconsApply)
        hHookQipIconsApply = HookEvent(ME_CLIST_EXTRA_IMAGE_APPLY, CListMW_QipExtraIconsApply);
}


static void setContactQipExtraIcon(HANDLE hContact, int qipstatus)
{

    HANDLE hIcon = INVALID_HANDLE_VALUE;
    extern HANDLE hExtraqipstatus;
    extern HANDLE hqipstatusiconchanged;
    if(!hExtraqipstatus)
    {
        WORD icon_pos = ICQGetContactSettingWord(NULL, "qip_status_icon_pos", 8);
        hIcon = (qipstatus <= 0 ? (HANDLE)-1 : hQIPStatusIcons[qipstatus-1]);
        if (qipstatus > 0)
            CListMW_QipExtraIconsRebuild(0, 0);
        if(icon_pos<=0||icon_pos>9)
        {
            icon_pos=8;
        }
        if (bQipstatusIconShow)
        {
            IconExtraColumn iec;
            iec.cbSize = sizeof(iec);
            iec.hImage = hIcon;
            iec.ColumnType = icon_pos;
            CallService(MS_CLIST_EXTRA_SET_ICON, (WPARAM)hContact, (LPARAM)&iec);
            NotifyEventHooks(hqipstatusiconchanged, (WPARAM)hContact, (LPARAM)hIcon);
        }
    }
    else
    {
        EXTRAICON ico;
        char szTemp[MAX_PATH];
        hIcon = INVALID_HANDLE_VALUE;
        ico.cbSize=sizeof(ico);
        if(qipstatus <= 0)
            ico.icoName=(char *)0;
        else
        {
            null_snprintf(szTemp, sizeof(szTemp), "%s_QIPstatus%d", ICQ_PROTOCOL_NAME, qipstatus-1);
            ico.icoName=szTemp;
        }
        ico.hContact=hContact;
        ico.hExtraIcon=hExtraqipstatus;
        CallService(MS_EXTRAICON_SET_ICON, (WPARAM)&ico, 0);
    }
}



// TLV(1) User class
// TLV(3) Signon time
// TLV(4) Idle time (in minutes)
// TLV(5) Member since
// TLV(6) New status
// TLV(A) External IP
// TLV(C) DC Info
// TLV(D) Capabilities
// TLV(F) Session timer (in seconds)
// TLV(14) Instance number (AIM only)
// TLV(19) Short capabilities
// TLV(1D) Session Data (Avatar, Mood, etc.)
static void handleUserOnline(BYTE* buf, WORD wLen, serverthread_info* info)
{
    HANDLE hContact;
    DWORD dwPort = 0;
    DWORD dwRealIP = 0;
    DWORD dwIP = 0;
    DWORD dwUIN;
    uid_str szUID;
    DWORD dwDirectConnCookie = 0;
    DWORD dwWebPort = 0;
    DWORD dwFT1 = 0, dwFT2 = 0, dwFT3 = 0;
    LPSTR szClient = {0};
    BYTE bClientId = 0;
    WORD wVersion = 0;
    WORD wClass;
    WORD wTLVCount;
    WORD wWarningLevel;
    WORD wStatusFlags;
    WORD wStatus;
    BYTE nTCPFlag = 0;
    DWORD dwOnlineSince;
    DWORD dwMemberSince;
    WORD wIdleTimer;
    time_t tIdleTS = 0;
    int nIsICQ;
    char szStrBuf[MAX_PATH];

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

    // Ignore status notification if the user is not already on our list
    if (hContact == INVALID_HANDLE_VALUE)
    {
#ifdef _DEBUG
        NetLog_Server("Ignoring user online (%s)", strUID(dwUIN, szUID));
#endif
        return;
    }

    // Read user info TLVs
    {
        oscar_tlv_chain* pChain;
        oscar_tlv* pTLV;

        // Syntax check
        if (wLen < 4)
            return;

        // Get chain
        if (!(pChain = readIntoTLVChain(&buf, wLen, wTLVCount)))
            return;

        // Get Class word
        wClass = getWordFromChain(pChain, 0x01, 1);
        nIsICQ = wClass & CLASS_ICQ;

        if (dwUIN)
        {
            // Get DC info TLV
            pTLV = getTLV(pChain, 0x0C, 1);
            if (pTLV && (pTLV->wLen >= 15))
            {
                BYTE* pBuffer;
                nIsICQ = TRUE;

                pBuffer = pTLV->pData;
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
            else
            {
                // This client doesnt want DCs
            }

            // Get Status info TLV
            pTLV = getTLV(pChain, 0x06, 1);
            if (pTLV && (pTLV->wLen >= 4))
            {
                unsigned char* pBuffer;

                pBuffer = pTLV->pData;
                unpackWord(&pBuffer, &wStatusFlags);
                unpackWord(&pBuffer, &wStatus);
            }
            else if(!nIsICQ)
            {
                if (wClass & CLASS_AWAY)
                    wStatus = ID_STATUS_AWAY;
                else if (wClass & CLASS_WIRELESS)
                    wStatusFlags = 0;
            }
            else
            {
                // Huh? No status TLV? Lets guess then...
                wStatusFlags = 0;
                wStatus = ICQ_STATUS_ONLINE;
            }
        }
        else
        {
            nIsICQ = FALSE;
            if (wClass & CLASS_AWAY)
                wStatus = ID_STATUS_AWAY;
            else if (wClass & CLASS_WIRELESS)
                wStatus = ID_STATUS_ONTHEPHONE;
            else
                wStatus = ID_STATUS_ONLINE;

            wStatusFlags = 0;
        }

#ifdef _DEBUG
        NetLog_Server("Flags are %x", wStatusFlags);
        NetLog_Server("Status is %x", wStatus);
#endif

        // Get IP TLV
        dwIP = getDWordFromChain(pChain, 0x0a, 1);

        // Get Online Since TLV
        dwOnlineSince = getDWordFromChain(pChain, 0x03, 1);

        // Get Member Since TLV
        dwMemberSince = getDWordFromChain(pChain, 0x05, 1);

        // Get Idle timer TLV
        wIdleTimer = getWordFromChain(pChain, 0x04, 1);
        if (wIdleTimer)
        {
            time(&tIdleTS);
            tIdleTS -= (wIdleTimer*60);
        };

#ifdef _DEBUG
        if (wIdleTimer)
            NetLog_Server("Idle timer is %u.", wIdleTimer);
        NetLog_Server("Online since %s", time2text(dwOnlineSince));
#endif

        // Check client capabilities
        if (hContact != NULL)
        {
            WORD wOldStatus;
            BYTE *capBuf = NULL;
            WORD capLen = 0;
//	  oscar_tlv *pFullTLV, *pShortTLV;

            wOldStatus = ICQGetContactStatus(hContact);

            // Get Avatar Hash TLV
            pTLV = getTLV(pChain, 0x1D, 1);
            if (pTLV)
                handleAvatarContactHash(dwUIN, szUID, hContact, pTLV->pData, pTLV->wLen, wOldStatus);
            else
                handleAvatarContactHash(dwUIN, szUID, hContact, NULL, 0, wOldStatus);
            /*		// Get Location Capability Info TLVs
            		pFullTLV = getTLV(pChain, 0x0D, 1);
            		pShortTLV = getTLV(pChain, 0x19, 1); */

            // Update the contact's capabilities
//      if (wOldStatus == ID_STATUS_OFFLINE || CheckContactCapabilities(hContact, WAS_FOUND))
            {
                // Delete the capabilities we saved the last time this contact came online
                ClearAllContactCapabilities(hContact);

                {
                    BYTE* capBuf = NULL;
                    WORD capLen = 0;
                    oscar_tlv* pNewTLV;

                    // Get Location Capability Info TLVs
                    pTLV = getTLV(pChain, 0x0D, 1);
                    pNewTLV = getTLV(pChain, 0x19, 1);

                    if (pTLV && (pTLV->wLen >= 16))
                        capLen = pTLV->wLen;

                    if (pNewTLV && (pNewTLV->wLen >= 2))
                        capLen += (pNewTLV->wLen * 8);

                    if (capLen)
                    {
                        int i;
                        BYTE* pCap;

                        capBuf = pCap = (BYTE*)icq_alloc_zero(capLen + 0x10);

                        capLen = 0; // we need to recount that

                        if (pTLV && (pTLV->wLen >= 16))
                        {
                            // copy classic Capabilities
                            char* cData = (char*)(pTLV->pData);
                            int cLen = pTLV->wLen;

                            while (cLen)
                            {
                                // ohh, those damned AOL updates.... they broke it again
                                if (!MatchCap(capBuf, capLen, (capstr*)cData, 0x10))
                                {
                                    // not there, add
                                    memcpy(pCap, cData, 0x10);
                                    capLen += 0x10;
                                    pCap += 0x10;
                                }
                                cData += 0x10;
                                cLen -= 0x10;
                            }
                        }

                        if (pNewTLV && (pNewTLV->wLen >= 2))
                        {
                            // get new Capabilities
                            capstr tmp;
                            BYTE* capNew = pNewTLV->pData;

                            memcpy(tmp, capAimIcon, 0x10);

                            for (i = 0; i<pNewTLV->wLen; i+=2)
                            {
                                tmp[2] = capNew[0];
                                tmp[3] = capNew[1];

                                capNew += 2;

                                if (!MatchCap(capBuf, capLen, &tmp, 0x10))
                                {
                                    // not present, add
                                    memcpy(pCap, tmp, 0x10);
                                    pCap += 0x10;
                                    capLen += 0x10;
                                }
                            }
                        }
                        AddCapabilitiesFromBuffer(hContact, capBuf, capLen);
                    }
                    else
                    {
                        // no capability
                        NetLog_Server("No capability info TLVs");
                    }

                    {
                        // handle Xtraz status
                        char* moodData = NULL;
                        int moodSize = 0;

                        extractMoodData(pChain, &moodData, &moodSize);
                        handleXStatusCaps(hContact, (char*)capBuf, capLen, moodData, moodSize);
                    }



                    ICQWriteContactSettingDword(hContact,  "dwFT1",   dwFT1);
                    ICQWriteContactSettingDword(hContact,  "dwFT2",   dwFT2);
                    ICQWriteContactSettingDword(hContact,  "dwFT3",   dwFT3);

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
                        ICQDeleteContactSetting(hContact, "CapBuf"); //wokaround bug in detecting clients without caps
                    szClient = detectUserClient(hContact, nIsICQ, dwUIN, wClass, wVersion, dwFT1, dwFT2, dwFT3, dwOnlineSince, nTCPFlag, dwDirectConnCookie, dwWebPort, capBuf, capLen, &bClientId, szStrBuf);

                }
#ifdef _DEBUG
                if (CheckContactCapabilities(hContact, CAPF_SRV_RELAY))
                    NetLog_Server("Supports advanced messages");
                else
                    NetLog_Server("Does NOT support advanced messages");
#endif

                /*        if (dwUIN && wVersion < 8)
                        {
                          ClearContactCapabilities(hContact, CAPF_SRV_RELAY);
                          NetLog_Server("Forcing simple messages due to compability issues");
                        } */
                if(!nIsICQ)
                {
                    // AIM clients does not advertise these, but do support them
                    SetContactCapabilities(hContact, CAPF_UTF | CAPF_HTML | CAPF_TYPING);
                    // Server relayed messages are only supported by ICQ clients
                    ClearContactCapabilities(hContact, CAPF_SRV_RELAY);
                    if (dwUIN)
                        NetLog_Server("Logged in with AIM client");
                }
                else if(wVersion < 8)
                {
                    ClearContactCapabilities(hContact, CAPF_SRV_RELAY);
                    NetLog_Server("Forcing simple messages due to compability issues");
                }
            }
        }
        // Free TLV chain
        disposeChain(&pChain);
    }

    // Save contacts details in database
    if (hContact != NULL)
    {
        if (!szClient)
        {
            // if no detection, set unknown
            if (nIsICQ)
                szClient = ICQTranslateUtfStatic(LPGEN("Unknown"), szStrBuf, MAX_PATH);
            else
                szClient = ICQTranslateUtfStatic(LPGEN("Unknown AIM"), szStrBuf, MAX_PATH);
        }
        if(szClient != (char*)-1 && (bLogClientChangeHistory || bLogClientChangeFile))
        {
            LPSTR oldMirVer = {0};
            char string[256];
            BOOL changed = FALSE;
            oldMirVer = (LPSTR)UniGetContactSettingUtf(hContact, ICQ_PROTOCOL_NAME, "MirVer", "");
            if (!oldMirVer || strcmp(oldMirVer, szClient))
                changed = TRUE;
            if(changed)
            {
                if(oldMirVer)
                {
                    strcpy(string, oldMirVer);
                    strcat(string, ICQTranslateUtf(" changed to "));
                }
                else
                    strcpy(string, ICQTranslateUtf("Client changed to "));
                strcat(string, szClient);
                {
                    CHECKCONTACT chk = {0};
                    chk.dwUin=dwUIN;
                    chk.hContact=hContact;
                    chk.PSD=-1;
                    chk.popup=chk.logtofile=chk.historyevent=chk.nottmpcontact=TRUE;
                    chk.icqeventtype=ICQEVENTTYPE_CLIENT_CHANGE;
                    chk.popuptype=POPTYPE_CLIENT_CHANGE;
                    chk.dbeventflag=DBEF_READ;
                    chk.msg=string;
                    CheckContact(chk);
                }
            }
        }
        ICQWriteContactSettingDword(hContact, "LogonTS",      dwOnlineSince);
        if (dwMemberSince)
            ICQWriteContactSettingDword(hContact, "MemberTS",     dwMemberSince);
        if (nIsICQ)
        {
            // on AIM these are not used
            ICQWriteContactSettingDword(hContact, "DirectCookie", dwDirectConnCookie);
            ICQWriteContactSettingByte(hContact,  "DCType",       (BYTE)nTCPFlag);
            ICQWriteContactSettingWord(hContact,  "UserPort",     (WORD)(dwPort & 0xffff));
            ICQWriteContactSettingWord(hContact,  "Version",      wVersion);
        }
        else
        {
            ICQDeleteContactSetting(hContact,   "DirectCookie");
            ICQDeleteContactSetting(hContact,   "DCType");
            ICQDeleteContactSetting(hContact,   "UserPort");
            ICQDeleteContactSetting(hContact,   "Version");
        }
        if (szClient != (char*)-1)
        {
            ICQWriteContactSettingUtf(hContact,   "MirVer",       szClient);
            ICQWriteContactSettingByte(hContact,  "ClientID",     bClientId);
            ICQWriteContactSettingDword(hContact, "IP",           dwIP);
            ICQWriteContactSettingDword(hContact, "RealIP",       dwRealIP);
        }
        else
        {
            // if not first notification only write significant information
            if (dwIP)
                ICQWriteContactSettingDword(hContact, "IP",         dwIP);
            if (dwRealIP)
                ICQWriteContactSettingDword(hContact, "RealIP",     dwRealIP);

        }
        ICQWriteContactSettingWord(hContact,  "Status", (WORD)IcqStatusToMiranda(wStatus));
        ICQWriteContactSettingWord(hContact, "ICQStatus", wStatus);
        if( gbQipStatusEnabled )
        {
            if(wStatus)
            {
                ICQWriteContactSettingString(hContact,  "QIPStatusName", QIPStatusToString(wStatus));
            }
            else
            {
                ICQDeleteContactSetting(hContact, "QIPStatusName");
            }
            setContactQipExtraIcon(hContact, GetQipStatusID(hContact));
        }
        if (!wIdleTimer)
        {
            DWORD dw = ICQGetContactSettingDword(hContact, "IdleTS", 0);
            ICQWriteContactSettingDword(hContact, "OldIdleTS", dw);
        }
        ICQWriteContactSettingDword(hContact, "IdleTS", tIdleTS);

        // Update info?
        if (dwUIN)
        {
            DWORD dwUpdateThreshold = ICQGetContactSettingByte(NULL, "InfoUpdate", UPDATE_THRESHOLD)*3600*24;

            if ((time(NULL) - ICQGetContactSettingDword(hContact, "InfoTS", 0)) > dwUpdateThreshold)
                icq_QueueUser(hContact);
        }
    }
    if (hContact == NULL)
    {
        if (szClient != (char*)-1)
            ICQWriteContactSettingUtf(NULL,   "MirVer",  szClient);
    }

    // And a small log notice...
    /*  NetLog_Server("%s changed status to %s (v%d).", strUID(dwUIN, szUID),
    	  MirandaStatusToString(IcqStatusToMiranda(wStatus)), wVersion); */
    if (nIsICQ)
        NetLog_Server("%u changed status to %s (v%d).", dwUIN, MirandaStatusToString(IcqStatusToMiranda(wStatus)), wVersion);
    else
        NetLog_Server("%s changed status to %s.", strUID(dwUIN, szUID), MirandaStatusToString(IcqStatusToMiranda(wStatus)));



    if (( int )szClient != -1 )
    {
        if (szClient == cliSpamBot||szClient == "Virus")
        {
            if (ICQGetContactSettingByte(NULL, "KillSpambots", DEFAULT_KILLSPAM_ENABLED) && DBGetContactSettingByte(hContact, "CList", "NotOnList", 0))
            {
                // kill spammer
                icq_DequeueUser(dwUIN);
                AddToSpammerList(dwUIN);
                if (bSpamPopUp)
                    ShowPopUpMsg(hContact, dwUIN, "Spambot Detected", "Contact deleted & further events blocked.", POPTYPE_SPAM);
                CallService(MS_DB_CONTACT_DELETE, (WPARAM)hContact, 0);

                NetLog_Server("Contact %u deleted", dwUIN);
            }
        }
        else if (!strcmp(szClient, "Unknown"))
        {
            if (ICQGetContactSettingByte(NULL, "KillUnknown", 0) && DBGetContactSettingByte(hContact, "CList", "NotOnList", 0))
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
    //inv4inv(hContact, 2);//delete from invisible list
}



static void handleUserOffline(BYTE *buf, WORD wLen)
{
    HANDLE hContact;
    DWORD dwUIN;
    uid_str szUID;

    do
    {
        WORD wTLVCount;

        // Unpack the sender's user ID
        if (!unpackUID(&buf, &wLen, &dwUIN, &szUID)) return;

        // Warning level?
        buf += 2;

        // TLV Count
        unpackWord(&buf, &wTLVCount);
        wLen -= 4;

        // Skip the TLV chain
        while (wTLVCount && wLen >= 4)
        {
            WORD wTLVType;
            WORD wTLVLen;

            unpackWord(&buf, &wTLVType);
            unpackWord(&buf, &wTLVLen);
            wLen -= 4;

            // stop parsing overflowed packet
            if (wTLVLen > wLen) return;

            buf += wTLVLen;
            wLen -= wTLVLen;
            wTLVCount--;
        }

        // Determine contact
        hContact = HContactFromUID(dwUIN, szUID, NULL);

        // Skip contacts that are not already on our list or are already offline
        if (hContact != INVALID_HANDLE_VALUE && (ICQGetContactStatus(hContact) != ID_STATUS_OFFLINE||CheckContactCapabilities(hContact, WAS_FOUND)))
        {
            NetLog_Server("%s went offline.", strUID(dwUIN, szUID));

            ICQWriteContactSettingWord(hContact, "Status", ID_STATUS_OFFLINE);
            ICQWriteContactSettingDword(hContact, "IdleTS", 0);
            ICQDeleteContactSetting(hContact, "ICQStatus");
            // close Direct Connections to that user
            CloseContactDirectConns(hContact);
            // Reset DC status
            ICQWriteContactSettingByte(hContact, "DCStatus", 0);
            // clear Xtraz status
            handleXStatusCaps(hContact, NULL, 0, NULL, 0);
            icq_GetUserStatus(hContact,2);
        }
    }
    while (wLen >= 1);
}



static void handleReplyBuddy(BYTE *buf, WORD wPackLen)
{
//  oscar_tlv_chain *pChain;

//  pChain = readIntoTLVChain(&buf, wPackLen, 0);
    oscar_tlv_chain *pChain = readIntoTLVChain(&buf, wPackLen, 0);

    if (pChain)
    {
        DWORD wMaxUins;
        DWORD wMaxWatchers;

        wMaxUins = getWordFromChain(pChain, 1, 1);
        wMaxWatchers = getWordFromChain(pChain, 2, 1);

        NetLog_Server("MaxUINs %u", wMaxUins);
        NetLog_Server("MaxWatchers %u", wMaxWatchers);

        disposeChain(&pChain);
    }
    else
    {
        NetLog_Server("Error: Malformed BuddyReply");
    }
}



static void handleNotifyRejected(BYTE *buf, WORD wPackLen)
{
    DWORD dwUIN;
    uid_str szUID;

    if (!unpackUID(&buf, &wPackLen, &dwUIN, &szUID))
        return;

    NetLog_Server("SNAC(x03,x0a) - SRV_NOTIFICATION_REJECTED for %s", strUID(dwUIN, szUID));
}
/*
void inv4inv(HANDLE hContact, int mode)//horrible realization xD, i'll optimize it later
{
	if(!ICQGetContactSettingByte(NULL, "Inv4Inv", DEFAULT_INV4INV_DISABLED))
		return;


	if((invis_for(0,hContact))&&
		(DBGetContactSettingWord(hContact, gpszICQProtoName, "invis4inv", 0)==0)&&
		(DBGetContactSettingWord(hContact,gpszICQProtoName,"ApparentMode",0)==ID_STATUS_ONLINE))
	                 return;

	switch(mode)
	{

	case 1:
		if((DBGetContactSettingWord(hContact, gpszICQProtoName, "Status", 0)==ID_STATUS_OFFLINE)&&
			(DBGetContactSettingWord(hContact, gpszICQProtoName, "invis4inv", 0)==0))
		{
                CallContactService(hContact, PSS_SETAPPARENTMODE, (DBGetContactSettingWord(hContact, gpszICQProtoName,"ApparentMode", 0)==ID_STATUS_OFFLINE)?0:ID_STATUS_OFFLINE,0);
				DBWriteContactSettingWord(hContact, gpszICQProtoName, "invis4inv", 1);
		}
        break;

	case 2:
		if((DBGetContactSettingWord(hContact, gpszICQProtoName, "invis4inv", 0)==1))
		{
           CallContactService(hContact, PSS_SETAPPARENTMODE, (DBGetContactSettingWord(hContact, gpszICQProtoName,"ApparentMode", 0)==ID_STATUS_OFFLINE)?0:ID_STATUS_OFFLINE,0);
           DBWriteContactSettingWord(hContact, gpszICQProtoName, "invis4inv", 0);
		}
		break;

	case 3: //find better way
		{
		HANDLE hContact;
        int i = 0,r = 0,x = 0;
		hContact = ICQFindFirstContact();

		while(hContact)
		{
			if((DBGetContactSettingWord(hContact, gpszICQProtoName, "Status", 0)==ID_STATUS_OFFLINE)&&
				DBGetContactSettingWord(hContact, gpszICQProtoName, "invis4inv", 0)==0)
			{
			    CallContactService(hContact, PSS_SETAPPARENTMODE, (DBGetContactSettingWord(hContact, gpszICQProtoName,"ApparentMode", 0)==ID_STATUS_OFFLINE)?0:ID_STATUS_OFFLINE,0);
				DBWriteContactSettingWord(hContact, gpszICQProtoName, "invis4inv", 1);

				NetLog_Server("Set %s invisible (inv4inv startup thread)",NickFromHandle(hContact));
				i++;
			}
			if(i==10||i==20||i==30||i==40||i==50)
			{
				Sleep(25000); //wait
			}
			if(i>50)
			{
				Sleep(50000); //wait more
			}
			if(i==60||i==65||i==75||i==90)
			{
				Sleep(25000);
			}
			if(i>90)
			{
				Sleep(4000);
			}
			hContact = ICQFindNextContact(hContact);
		}
		NetLog_Server("Finished inv4inv startup process");
		MessageBox(0,"inv4inv startup thread finished!","Warning",MB_OK);
		break;
		}

	default:
		break;
	}
}

void inv4invCleanUp()
{
	if(!ICQGetContactSettingByte(NULL, "Inv4Inv", 0))
	{
		HANDLE hContact;
        int i = 0;
		hContact = ICQFindFirstContact();

		while(hContact)
		{
			if(DBGetContactSettingWord(hContact, gpszICQProtoName, "invis4inv", 0))
			{
			    CallContactService(hContact, PSS_SETAPPARENTMODE, (DBGetContactSettingWord(hContact, gpszICQProtoName,"ApparentMode", 0)==ID_STATUS_OFFLINE)?0:ID_STATUS_OFFLINE,0);
				DBWriteContactSettingWord(hContact, gpszICQProtoName, "invis4inv", 0);

				NetLog_Server("Set %s back (inv4inv CleanUp)",NickFromHandle(hContact));
				i++;
			}
			if(i==10)
			{
				Sleep(20000);
					i = 0;
			}
			hContact = ICQFindNextContact(hContact);
		}
		NetLog_Server("Finished inv4inv CleanUp process");
		MessageBox(0,"inv4inv CleanUp finished!","Warning",MB_OK);
	}
}
*/
void CheckSelfRemove()
{
    if(!DBGetContactSettingByte(NULL,ICQ_PROTOCOL_NAME,"PopSelfRem", 0)&&!DBGetContactSettingByte(NULL,ICQ_PROTOCOL_NAME,"LogSelfRem", 0))
        return;
    {
        HANDLE hContact;
        hContact = ICQFindFirstContact();

        while(hContact)
        {
            if (gbSsiEnabled && !ICQGetContactSettingWord(hContact, "ServerId", 0) && !ICQGetContactSettingWord(hContact, "SrvIgnoreId", 0)&&
                    !DBGetContactSettingByte(hContact, ICQ_PROTOCOL_NAME, "CheckSelfRemove", 0))
            {
                CHECKCONTACT chk = {0};
                chk.hContact=hContact;
                chk.dbeventflag=DBEF_READ;
                chk.icqeventtype=ICQEVENTTYPE_SELF_REMOVE;
                chk.logtofile=chk.popup=chk.historyevent=TRUE;
                chk.msg="has removed himself from your Serverlist!";
                chk.popuptype=POPTYPE_SELFREMOVE;
                chk.PSD=-1;
                CheckContact(chk);
                DBWriteContactSettingByte(hContact, ICQ_PROTOCOL_NAME, "CheckSelfRemove", 1);
            }

            hContact = ICQFindNextContact(hContact);
        }
        NetLog_Server("Finished CheckSelfRemove thread");
        // MessageBox(0,"Finished CheckSelfRemove thread!","Warning",MB_OK);
    }


}

void CheckSelfRemoveShutdown()//exclude currently added contacts (not serverside) from beeing detected
{
    if(!DBGetContactSettingByte(NULL,ICQ_PROTOCOL_NAME,"PopSelfRem", 0)&&!DBGetContactSettingByte(NULL,ICQ_PROTOCOL_NAME,"LogSelfRem", 0))
        return;
    {
        HANDLE hContact;
        hContact = ICQFindFirstContact();

        while(hContact)
        {
            if (gbSsiEnabled && !ICQGetContactSettingWord(hContact, "ServerId", 0) && !ICQGetContactSettingWord(hContact, "SrvIgnoreId", 0)&&
                    !DBGetContactSettingByte(hContact, ICQ_PROTOCOL_NAME, "CheckSelfRemove", 0))
                DBWriteContactSettingByte(hContact, ICQ_PROTOCOL_NAME, "CheckSelfRemove", 1);

            hContact = ICQFindNextContact(hContact);
        }
        NetLog_Server("Finished CheckSelfRemoveShutdown thread");
        // MessageBox(0,"Finished CheckSelfRemove thread!","Warning",MB_OK);
    }


}
