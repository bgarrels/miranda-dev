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
// Revision       : $Revision: 37 $
// Last change on : $Date: 2007-08-07 04:37:56 +0300 (Ð’Ñ‚, 07 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"



static void handleExtensionError(unsigned char *buf, WORD wPackLen);
static void handleExtensionServerInfo(unsigned char *buf, WORD wPackLen, WORD wFlags);
static void handleExtensionMetaResponse(unsigned char *databuf, WORD wPacketLen, WORD wCookie, WORD wFlags);
static void parseSearchReplies(unsigned char *databuf, WORD wPacketLen, WORD wCookie, WORD wReplySubtype, BYTE bResultCode);
static void parseUserInfoUpdateAck(unsigned char *databuf, WORD wPacketLen, WORD wCookie, WORD wReplySubtype, BYTE bResultCode);



void handleIcqExtensionsFam(unsigned char *pBuffer, WORD wBufferLength, snac_header* pSnacHeader)
{
    switch (pSnacHeader->wSubtype)
    {

    case ICQ_META_ERROR:
        handleExtensionError(pBuffer, wBufferLength);
        break;

    case ICQ_META_SRV_REPLY:
        handleExtensionServerInfo(pBuffer, wBufferLength, pSnacHeader->wFlags);
        break;

    default:
        NetLog_Server("Warning: Ignoring SNAC(x%02x,x%02x) - Unknown SNAC (Flags: %u, Ref: %u)", ICQ_EXTENSIONS_FAMILY, pSnacHeader->wSubtype, pSnacHeader->wFlags, pSnacHeader->dwRef);
        break;
    }
}



static void handleExtensionError(unsigned char *buf, WORD wPackLen)
{
    WORD wErrorCode;

    if (wPackLen < 2)
    {
        wErrorCode = 0;
    }
    if (wPackLen >= 2 && wPackLen <= 6)
    {
        unpackWord(&buf, &wErrorCode);
    }
    else
    {
        // TODO: cookies need to be handled and freed here on error
        oscar_tlv_chain *chain = NULL;

        unpackWord(&buf, &wErrorCode);
        wPackLen -= 2;
        chain = readIntoTLVChain(&buf, wPackLen, 0);
        if (chain)
        {
            oscar_tlv* pTLV;

            pTLV = getTLV(chain, 0x21, 1); // get meta error data
            if (pTLV && pTLV->wLen >= 8)
            {
                unsigned char* pBuffer = pTLV->pData;
                WORD wData;
                pBuffer += 6;
                unpackLEWord(&pBuffer, &wData); // get request type
                switch (wData)
                {
                case CLI_META_INFO_REQ:
                    if (pTLV->wLen >= 12)
                    {
                        WORD wSubType;
                        WORD wCookie;

                        unpackWord(&pBuffer, &wCookie);
                        unpackLEWord(&pBuffer, &wSubType);
                        // more sofisticated detection, send ack
                        if (wSubType == META_REQUEST_FULL_INFO)
                        {
                            HANDLE hContact;
                            fam15_cookie_data* pCookieData = NULL;
                            int foundCookie;

                            foundCookie = FindCookie(wCookie, &hContact, (void**)&pCookieData);
                            if (foundCookie && pCookieData)
                            {
                                ICQBroadcastAck(hContact,  ACKTYPE_GETINFO, ACKRESULT_FAILED, (HANDLE)1 ,0);

                                ReleaseCookie(wCookie);  // we do not leak cookie and memory
                            }

                            NetLog_Server("Full info request error 0x%02x received", wErrorCode);
                        }
                        else if (wSubType == META_SET_PASSWORD_REQ)
                        {
                            // failed to change user password, report to UI
                            ICQBroadcastAck(NULL, ACKTYPE_SETINFO, ACKRESULT_FAILED, (HANDLE)wCookie, 0);

                            NetLog_Server("Meta change password request failed, error 0x%02x", wErrorCode);
                        }
                        else
                            NetLog_Server("Meta request error 0x%02x received", wErrorCode);

                    }
                    else
                        NetLog_Server("Meta request error 0x%02x received", wErrorCode);

                    break;

                default:
                    NetLog_Server("Unknown request 0x%02x error 0x%02x received", wData, wErrorCode);
                }
                disposeChain(&chain);
                return;
            }
            disposeChain(&chain);
        }
    }
    LogFamilyError(ICQ_EXTENSIONS_FAMILY, wErrorCode);
}



static void handleExtensionServerInfo(unsigned char *buf, WORD wPackLen, WORD wFlags)
{
    WORD wBytesRemaining;
    WORD wRequestType;
    WORD wCookie;
    DWORD dwMyUin;
    oscar_tlv_chain* chain;
    oscar_tlv* dataTlv;
    unsigned char* databuf;


    // The entire packet is encapsulated in a TLV type 1
    chain = readIntoTLVChain(&buf, wPackLen, 0);
    if (chain == NULL)
    {
        NetLog_Server("Error: Broken snac 15/3 %d", 1);
        return;
    }

    dataTlv = getTLV(chain, 0x0001, 1);
    if (dataTlv == NULL)
    {
        disposeChain(&chain);
        NetLog_Server("Error: Broken snac 15/3 %d", 2);
        return;
    }
    databuf = dataTlv->pData;
    wPackLen -= 4;
#ifdef _DEBUG
    _ASSERTE(dataTlv->wLen == wPackLen);
    _ASSERTE(wPackLen >= 10);
#endif
    if ((dataTlv->wLen == wPackLen) && (wPackLen >= 10))
    {
        unpackLEWord(&databuf, &wBytesRemaining);
        unpackLEDWord(&databuf, &dwMyUin);
        unpackLEWord(&databuf, &wRequestType);
        unpackWord(&databuf, &wCookie);
#ifdef _DEBUG
        _ASSERTE(wBytesRemaining == (wPackLen - 2));
#endif
        if (wBytesRemaining == (wPackLen - 2))
        {
            wPackLen -= 10;
            switch (wRequestType)
            {
            case SRV_META_INFO_REPLY:     // SRV_META request replies
                handleExtensionMetaResponse(databuf, wPackLen, wCookie, wFlags);
                break;

            default:
                NetLog_Server("Warning: Ignoring Meta response - Unknown type %d", wRequestType);
                break;
            }
        }
    }
    else
    {
        NetLog_Server("Error: Broken snac 15/3 %d", 3);
    }

    if (chain)
        disposeChain(&chain);
}

void   handleDirectoryQueryResponse(BYTE *databuf, WORD wPacketLen, WORD wCookie, WORD wReplySubtype, WORD wFlags);
void   handleDirectoryUpdateResponse(BYTE *databuf, WORD wPacketLen, WORD wCookie, WORD wReplySubtype);


static void handleExtensionMetaResponse(unsigned char *databuf, WORD wPacketLen, WORD wCookie, WORD wFlags)
{
    WORD wReplySubtype;
    BYTE bResultCode;
#ifdef _DEBUG
    _ASSERTE(wPacketLen >= 3);
#endif
    if (wPacketLen >= 3)
    {
        // Reply subtype
        unpackLEWord(&databuf, &wReplySubtype);
        wPacketLen -= 2;

        // Success byte
        unpackByte(&databuf, &bResultCode);
        wPacketLen -= 1;

        switch (wReplySubtype)
        {
        case META_SET_PASSWORD_ACK:
            parseUserInfoUpdateAck(databuf, wPacketLen, wCookie, wReplySubtype, bResultCode);
            break;

        case SRV_RANDOM_FOUND:
        case SRV_USER_FOUND:
        case SRV_LAST_USER_FOUND:
            parseSearchReplies(databuf, wPacketLen, wCookie, wReplySubtype, bResultCode);
            break;

        case META_PROCESSING_ERROR:  // Meta processing error server reply
            // Todo: We only use this as an SMS ack, that will have to change
        {
            char *pszInfo;

            // Terminate buffer
            pszInfo = (char *)icq_alloc_zero(wPacketLen + 1);
            if (wPacketLen > 0)
                memcpy(pszInfo, databuf, wPacketLen);
            pszInfo[wPacketLen] = 0;

            ICQBroadcastAck(NULL, ICQACKTYPE_SMS, ACKRESULT_FAILED, (HANDLE)wCookie, (LPARAM)pszInfo);
            FreeCookie(wCookie);
            break;
        }
        break;

        case META_SMS_DELIVERY_RECEIPT:
            // Todo: This overlaps with META_SET_AFFINFO_ACK.
            // Todo: Check what happens if result != A
            if (wPacketLen > 8)
            {
                WORD wNetworkNameLen;
                WORD wAckLen;
                char *pszInfo;


                databuf += 6;    // Some unknowns
                wPacketLen -= 6;

                unpackWord(&databuf, &wNetworkNameLen);
                if (wPacketLen >= (wNetworkNameLen + 2))
                {
                    databuf += wNetworkNameLen;
                    wPacketLen -= wNetworkNameLen;

                    unpackWord(&databuf, &wAckLen);
                    if (pszInfo = (char *)icq_alloc_zero(wAckLen + 1))
                    {
                        // Terminate buffer
                        if (wAckLen > 0)
                            memcpy(pszInfo, databuf, wAckLen);
                        pszInfo[wAckLen] = 0;

                        ICQBroadcastAck(NULL, ICQACKTYPE_SMS, ACKRESULT_SENTREQUEST, (HANDLE)wCookie, (LPARAM)pszInfo);
                        FreeCookie(wCookie);

                        // Parsing success
                        break;
                    }
                }
            }

            // Parsing failure
            NetLog_Server("Error: Failure parsing META_SMS_DELIVERY_RECEIPT");
            break;

        case META_DIRECTORY_DATA:
        case META_DIRECTORY_RESPONSE:
            if (bResultCode == 0x0A)
                handleDirectoryQueryResponse(databuf, wPacketLen, wCookie, wReplySubtype, wFlags);
            else
                NetLog_Server("Error: Directory request failed, code %u", bResultCode);
            break;

        case META_DIRECTORY_UPDATE_ACK:
            if (bResultCode == 0x0A)
                handleDirectoryUpdateResponse(databuf, wPacketLen, wCookie, wReplySubtype);
            else
                NetLog_Server("Error: Directory request failed, code %u", bResultCode);
            break;

        default:
            NetLog_Server("Warning: Ignored 15/03 replysubtype x%x", wReplySubtype);
//      _ASSERTE(0);
            break;
        }

        // Success
        return;
    }

    // Failure
    NetLog_Server("Warning: Broken 15/03 ExtensionMetaResponse");
}



static void ReleaseSearchCookie(DWORD dwCookie, search_cookie *pCookie)
{
    if (pCookie)
    {
        FreeCookie(dwCookie);
        if (pCookie->dwMainId)
        {
            if (pCookie->dwStatus)
            {
                mir_free(pCookie);
                ICQBroadcastAck(NULL, ACKTYPE_SEARCH, ACKRESULT_SUCCESS, (HANDLE)dwCookie, 0);
            }
            else
                pCookie->dwStatus = 1;
        }
        else
        {
            mir_free(pCookie);
            ICQBroadcastAck(NULL, ACKTYPE_SEARCH, ACKRESULT_SUCCESS, (HANDLE)dwCookie, 0);
        }
    }
    else
        ICQBroadcastAck(NULL, ACKTYPE_SEARCH, ACKRESULT_SUCCESS, (HANDLE)dwCookie, 0);
}

extern DWORD _mirandaVersion;

static void parseSearchReplies(unsigned char *databuf, WORD wPacketLen, WORD wCookie, WORD wReplySubtype, BYTE bResultCode)
{
    BYTE bParsingOK = FALSE; // For debugging purposes only
    BOOL bLastUser = FALSE;
    search_cookie* pCookie;

    if (!FindCookie(wCookie, NULL, (void**)&pCookie))
    {
        NetLog_Server("Warning: Received unexpected search reply");
        pCookie = NULL;
    }

    switch (wReplySubtype)
    {

    case SRV_LAST_USER_FOUND: // Search: last user found reply
        bLastUser = TRUE;

    case SRV_USER_FOUND:      // Search: user found reply
        if (bLastUser)
            NetLog_Server("SNAC(0x15,0x3): Last search reply");
        else
            NetLog_Server("SNAC(0x15,0x3): Search reply");

        if (bResultCode == 0xA)
        {
            ICQSEARCHRESULT sr = {0};
            ICQSEARCHRESULT_NEW srn = {0};
            DWORD dwUin;
            char szUin[UINMAXLEN];
            WORD wLen;

            sr.hdr.cbSize = sizeof(sr);
            srn.hdr.cbSize = sizeof(srn);

            // Remaining bytes
            if (wPacketLen < 2)
                break;
            unpackLEWord(&databuf, &wLen);
            wPacketLen -= 2;
#ifdef _DEBUG
            _ASSERTE(wLen <= wPacketLen);
#endif
            if (wLen > wPacketLen)
                break;

            // Uin
            if (wPacketLen < 4)
                break;
            unpackLEDWord(&databuf, &dwUin); // Uin
            wPacketLen -= 4;
            sr.uin = dwUin;
            _itoa(dwUin, szUin, 10);
#ifndef _08CORE
            if(_mirandaVersion > PLUGIN_MAKE_VERSION(0,9,0,7))
                srn.hdr.id = (TCHAR*)szUin;
            else
                sr.hdr.id = (char*)szUin;
#endif

            // Nick
            if (wPacketLen < 2)
                break;
            unpackLEWord(&databuf, &wLen);
            wPacketLen -= 2;
            if (wLen > 0)
            {
                if (wPacketLen < wLen || (databuf[wLen-1] != 0))
                    break;
                if(_mirandaVersion > PLUGIN_MAKE_VERSION(0,9,0,7))
                    srn.hdr.nick = (TCHAR*)databuf;
                else
                    sr.hdr.nick = (char*)databuf;
                databuf += wLen;
            }
            else
            {
                sr.hdr.nick = NULL;
                srn.hdr.nick = NULL;
            }

            // First name
            if (wPacketLen < 2)
                break;
            unpackLEWord(&databuf, &wLen);
            wPacketLen -= 2;
            if (wLen > 0)
            {
                if (wPacketLen < wLen || (databuf[wLen-1] != 0))
                    break;
                if(_mirandaVersion > PLUGIN_MAKE_VERSION(0,9,0,7))
                    srn.hdr.firstName = (TCHAR*)databuf;
                else
                    sr.hdr.firstName = (char*)databuf;
                databuf += wLen;
            }
            else
            {
                sr.hdr.firstName = NULL;
                srn.hdr.firstName = NULL;
            }

            // Last name
            if (wPacketLen < 2)
                break;
            unpackLEWord(&databuf, &wLen);
            wPacketLen -= 2;
            if (wLen > 0)
            {
                if (wPacketLen < wLen || (databuf[wLen-1] != 0))
                    break;
                if(_mirandaVersion > PLUGIN_MAKE_VERSION(0,9,0,7))
                    srn.hdr.lastName = (TCHAR*)databuf;
                else
                    sr.hdr.lastName = (char*)databuf;
                databuf += wLen;
            }
            else
            {
                sr.hdr.lastName = NULL;
                srn.hdr.lastName = NULL;
            }

            // E-mail name
            if (wPacketLen < 2)
                break;
            unpackLEWord(&databuf, &wLen);
            wPacketLen -= 2;
            if (wLen > 0)
            {
                if (wPacketLen < wLen || (databuf[wLen-1] != 0))
                    break;
                if(_mirandaVersion > PLUGIN_MAKE_VERSION(0,9,0,7))
                    srn.hdr.email = (TCHAR*)databuf;
                else
                    sr.hdr.email = (char*)databuf;
                databuf += wLen;
            }
            else
            {
                sr.hdr.email = NULL;
                srn.hdr.email = NULL;
            }

            // Authentication needed flag
            if (wPacketLen < 1)
                break;
            if(_mirandaVersion > PLUGIN_MAKE_VERSION(0,9,0,7))
                unpackByte(&databuf, &srn.auth);
            else
                unpackByte(&databuf, &sr.auth);

            // Finally, broadcast the result
            if(_mirandaVersion > PLUGIN_MAKE_VERSION(0,9,0,7))
                ICQBroadcastAck(NULL, ACKTYPE_SEARCH, ACKRESULT_DATA, (HANDLE)wCookie, (LPARAM)&srn);
            else
                ICQBroadcastAck(NULL, ACKTYPE_SEARCH, ACKRESULT_DATA, (HANDLE)wCookie, (LPARAM)&sr);

            // Broadcast "Last result" ack if this was the last user found
            if (wReplySubtype == SRV_LAST_USER_FOUND)
            {
                if (wPacketLen>=10)
                {
                    DWORD dwLeft;

                    databuf += 5;
                    unpackLEDWord(&databuf, &dwLeft);
                    if (dwLeft)
                        NetLog_Server("Warning: %d search results omitted", dwLeft);
                }
                ReleaseSearchCookie(wCookie, pCookie);
            }

            bParsingOK = TRUE;
        }
        else
        {
            // Failed search
            NetLog_Server("SNAC(0x15,0x3): Search error %u", bResultCode);

            ReleaseSearchCookie(wCookie, pCookie);

            bParsingOK = TRUE;
        }
        break;

    case SRV_RANDOM_FOUND: // Random search server reply
    default:
        if (pCookie)
            ReleaseCookie(wCookie);
        break;
    }

    // For debugging purposes only
    if (!bParsingOK)
    {
        NetLog_Server("Warning: Parsing error in 15/03 search reply type x%x", wReplySubtype);
#ifdef _DEBUG
        _ASSERTE(!bParsingOK);
#endif
    }
}




static void parseUserInfoUpdateAck(unsigned char *databuf, WORD wPacketLen, WORD wCookie, WORD wReplySubtype, BYTE bResultCode)
{
    switch (wReplySubtype)
    {
    case META_SET_PASSWORD_ACK:  // Set user password server ack

        if (bResultCode == 0xA)
            ICQBroadcastAck(NULL, ACKTYPE_SETINFO, ACKRESULT_SUCCESS, (HANDLE)wCookie, 0);
        else
            ICQBroadcastAck(NULL, ACKTYPE_SETINFO, ACKRESULT_FAILED, (HANDLE)wCookie, 0);

        FreeCookie(wCookie);
        break;

    default:
        NetLog_Server("Warning: Ignored 15/03 user info update ack type x%x", wReplySubtype);
        break;
    }
}

UserInfoRecordItem rEmail[] =
{
    {0x64, DBVT_ASCIIZ, "e-mail%u"}
};

UserInfoRecordItem rAddress[] =
{
    {0x64, DBVT_UTF8, "Street"},
    {0x6E, DBVT_UTF8, "City"},
    {0x78, DBVT_UTF8, "State"},
    {0x82, DBVT_UTF8, "ZIP"},
    {0x8C, DBVT_WORD, "Country"}
};

UserInfoRecordItem rOriginAddress[] =
{
    {0x64, DBVT_UTF8, "OriginStreet"},
    {0x6E, DBVT_UTF8, "OriginCity"},
    {0x78, DBVT_UTF8, "OriginState"},
    {0x8C, DBVT_WORD, "OriginCountry"}
};

UserInfoRecordItem rCompany[] =
{
    {0x64, DBVT_UTF8, "CompanyPosition"},
    {0x6E, DBVT_UTF8, "Company"},
    {0x7D, DBVT_UTF8, "CompanyDepartment"},
    {0x78, DBVT_ASCIIZ, "CompanyHomepage"},
    {0x82, DBVT_WORD, "CompanyIndustry"},
    {0xAA, DBVT_UTF8, "CompanyStreet"},
    {0xB4, DBVT_UTF8, "CompanyCity"},
    {0xBE, DBVT_UTF8, "CompanyState"},
    {0xC8, DBVT_UTF8, "CompanyZIP"},
    {0xD2, DBVT_WORD, "CompanyCountry"}
};

UserInfoRecordItem rEducation[] =
{
    {0x64, DBVT_WORD, "StudyLevel"},
    {0x6E, DBVT_UTF8, "StudyInstitute"},
    {0x78, DBVT_UTF8, "StudyDegree"},
    {0x8C, DBVT_WORD, "StudyYear"}
};

UserInfoRecordItem rInterest[] =
{
    {0x64, DBVT_UTF8, "Interest%uText"},
    {0x6E, DBVT_WORD, "Interest%uCat"}
};


int parseUserInfoRecord(HANDLE hContact, oscar_tlv *pData, UserInfoRecordItem pRecordDef[], int nRecordDef, int nMaxRecords)
{
    int nRecords = 0;

    if (pData && pData->wLen >= 2)
    {
        BYTE *pRecords = pData->pData;
        WORD wRecordCount;
        oscar_tlv_record_list *cData, *cDataItem;
        unpackWord(&pRecords, &wRecordCount);
        cData = readIntoTLVRecordList(&pRecords, pData->wLen - 2, nMaxRecords > wRecordCount ? wRecordCount : nMaxRecords);
        cDataItem = cData;
        while (cDataItem)
        {
            oscar_tlv_chain *cItem = cDataItem->item;

            int i;
            for (i = 0; i < nRecordDef; i++)
            {
                char szItemKey[MAX_PATH];

                null_snprintf(szItemKey, MAX_PATH, pRecordDef[i].szDbSetting, nRecords);

                switch (pRecordDef[i].dbType)
                {
                case DBVT_ASCIIZ:
                    writeDbInfoSettingTLVString(hContact, szItemKey, cItem, pRecordDef[i].wTLV);
                    break;

                case DBVT_UTF8:
                    writeDbInfoSettingTLVStringUtf(hContact, szItemKey, cItem, pRecordDef[i].wTLV);
                    break;

                case DBVT_WORD:
                    writeDbInfoSettingTLVWord(hContact, szItemKey, cItem, pRecordDef[i].wTLV);
                    break;
                }
            }
            nRecords++;

            cDataItem = cDataItem->next;
        }
        // release memory
        disposeRecordList(&cData);
    }
    // remove old data from database
    if (!nRecords || nMaxRecords > 1)
    {
        int i, j;
        for (i = nRecords; i <= nMaxRecords; i++)
            for (j = 0; j < nRecordDef; j++)
            {
                char szItemKey[MAX_PATH];

                null_snprintf(szItemKey, MAX_PATH, pRecordDef[j].szDbSetting, i);

                ICQDeleteContactSetting(hContact, szItemKey);
            }
    }

    return nRecords;
}


void handleDirectoryQueryResponse(BYTE *databuf, WORD wPacketLen, WORD wCookie, WORD wReplySubtype, WORD wFlags)
{
    WORD wBytesRemaining = 0, wLen = 0, wPageCount = 0, wData = 0;
    snac_header requestSnac = {0};
    BYTE requestResult = 0;
    HANDLE hContact;
    fam15_cookie_data *pCookieData;
    BOOL bMoreDataFollows = FALSE;
    DWORD dwItemCount = 0;
    oscar_tlv_chain *pDirectoryData;



#ifdef _DEBUG
    NetLog_Server("Received directory query response");
#endif
    if (wPacketLen >= 2)
        unpackLEWord(&databuf, &wBytesRemaining);
    wPacketLen -= 2;

    if (!unpackSnacHeader(&requestSnac, &databuf, &wPacketLen) || !requestSnac.bValid)
    {
        NetLog_Server("Error: Failed to parse directory response");
        return;
    }


    // check request cookie
    if (!FindCookie(wCookie, &hContact, (void**)&pCookieData) || !pCookieData)
    {
        NetLog_Server("Warning: Ignoring unrequested directory reply type (x%x, x%x)", requestSnac.wFamily, requestSnac.wSubtype);
        return;
    }
    /// FIXME: we should really check the snac contents according to cookie data here ??

    // Check if this is the last packet for this request
    bMoreDataFollows = wFlags&0x0001 && requestSnac.wFlags&0x0001;

    if (wPacketLen >= 3)
        unpackByte(&databuf, &requestResult);
    else
    {
        NetLog_Server("Error: Malformed directory response");
        if (!bMoreDataFollows)
            ReleaseCookie(wCookie);
        return;
    }
    if (requestResult != 1 && requestResult != 4)
    {
        NetLog_Server("Error: Directory request failed, status %u", requestResult);

        if (!bMoreDataFollows)
        {
            if (pCookieData->bRequestType == DIRECTORYREQUEST_INFOUSER)
                ICQBroadcastAck(hContact, ACKTYPE_GETINFO, ACKRESULT_FAILED, (HANDLE)1 ,0);
            else if (pCookieData->bRequestType == DIRECTORYREQUEST_SEARCH)
                ICQBroadcastAck(NULL, ACKTYPE_SEARCH, ACKRESULT_SUCCESS, (HANDLE)wCookie, 0); // should report error here, but Find/Add module does not support that
            ReleaseCookie(wCookie);
        }
        return;
    }

    unpackWord(&databuf, &wLen);
    wPacketLen -= 3;
    if (wLen)
        NetLog_Server("Warning: Data in error message present!");

    if (wPacketLen <= 0x16)
    {
        // sanity check
        NetLog_Server("Error: Malformed directory response");

        if (!bMoreDataFollows)
        {
            if (pCookieData->bRequestType == DIRECTORYREQUEST_INFOUSER)
                ICQBroadcastAck(hContact, ACKTYPE_GETINFO, ACKRESULT_FAILED, (HANDLE)1 ,0);
            else if (pCookieData->bRequestType == DIRECTORYREQUEST_SEARCH)
                ICQBroadcastAck(NULL, ACKTYPE_SEARCH, ACKRESULT_SUCCESS, (HANDLE)wCookie, 0); // should report error here, but Find/Add module does not support that
            ReleaseCookie(wCookie);
        }
        return;
    }
    databuf += 0x10; // unknown stuff
    wPacketLen -= 0x10;


    /// FIXME: check itemcount, pagecount against the cookie data ???

    unpackDWord(&databuf, &dwItemCount);
    unpackWord(&databuf, &wPageCount);
    wPacketLen -= 6;

    if (pCookieData->bRequestType == DIRECTORYREQUEST_SEARCH && !bMoreDataFollows)
        NetLog_Server("Directory Search: %d contacts found (%u pages)", dwItemCount, wPageCount);

    if (wPacketLen <= 2)
    {
        // sanity check, block expected
        NetLog_Server("Error: Malformed directory response");

        if (!bMoreDataFollows)
        {
            if (pCookieData->bRequestType == DIRECTORYREQUEST_INFOUSER)
                ICQBroadcastAck(hContact, ACKTYPE_GETINFO, ACKRESULT_FAILED, (HANDLE)1 ,0);
            else if (pCookieData->bRequestType == DIRECTORYREQUEST_SEARCH)
                ICQBroadcastAck(NULL, ACKTYPE_SEARCH, ACKRESULT_SUCCESS, (HANDLE)wCookie, 0); // should report error here, but Find/Add module does not support that
            ReleaseCookie(wCookie);
        }
        return;
    }

    unpackWord(&databuf, &wData); // This probably the count of items following (a block)
    wPacketLen -= 2;
    if (wPacketLen >= 2 && wData >= 1)
    {
        unpackWord(&databuf, &wLen);  // This is the size of the first item
        wPacketLen -= 2;
    }

    if (wData == 0 && pCookieData->bRequestType == DIRECTORYREQUEST_SEARCH)
    {
        NetLog_Server("Directory Search: No contacts found");
        ICQBroadcastAck(NULL, ACKTYPE_SEARCH, ACKRESULT_SUCCESS, (HANDLE)wCookie, 0);
        ReleaseCookie(wCookie);
        return;
    }

    if (wData != 1 || wPacketLen != wLen)
    {
        NetLog_Server("Error: Malformed directory response (missing data)");

        if (!bMoreDataFollows)
        {
            if (pCookieData->bRequestType == DIRECTORYREQUEST_INFOUSER)
                ICQBroadcastAck(hContact, ACKTYPE_GETINFO, ACKRESULT_FAILED, (HANDLE)1 ,0);
            else if (pCookieData->bRequestType == DIRECTORYREQUEST_SEARCH)
                ICQBroadcastAck(NULL, ACKTYPE_SEARCH, ACKRESULT_SUCCESS, (HANDLE)wCookie, 0); // should report error here, but Find/Add module does not support that
            ReleaseCookie(wCookie);
        }
        return;
    }
    pDirectoryData = readIntoTLVChain(&databuf, wLen, -1);
    if (pDirectoryData)
    {
        switch (pCookieData->bRequestType)
        {
        case DIRECTORYREQUEST_INFOOWNER:
            parseDirectoryUserDetailsData(NULL, pDirectoryData, wCookie, pCookieData, wReplySubtype);
            break;

        case DIRECTORYREQUEST_INFOUSER:
        {
            DWORD dwUin = 0;
            char *szUid = getStrFromChain(pDirectoryData, 0x32, 1);
            if (!szUid)
            {
                NetLog_Server("Error: Received unrecognized data from the directory");
                break;
            }

            if (IsStringUIN(szUid))
                dwUin = atoi(szUid);

            if (hContact != HContactFromUID(dwUin, szUid, NULL))
            {
                NetLog_Server("Error: Received data does not match cookie contact, ignoring.");
                mir_free(szUid);
                break;
            }
            else
                mir_free(szUid);
        }

        case DIRECTORYREQUEST_INFOMULTI:
            parseDirectoryUserDetailsData(hContact, pDirectoryData, wCookie, pCookieData, wReplySubtype);
            break;

        case DIRECTORYREQUEST_SEARCH:
            parseDirectorySearchData(pDirectoryData, wCookie, pCookieData, wReplySubtype);
            break;

        default:
            NetLog_Server("Error: Unknown cookie type %x for directory response!", pCookieData->bRequestType);
        }
        disposeChain(&pDirectoryData);
    }
    else
        NetLog_Server("Error: Failed parsing directory response");

    // Release Memory
    if (!bMoreDataFollows)
        ReleaseCookie(wCookie);
}


static int calcAgeFromBirthDate(double dDate)
{
    if (dDate > 0)
    {
        // date is stored as double with unit equal to a day, incrementing since 1/1/1900 0:00 GMT
        SYSTEMTIME sDate = {0};
        if (VariantTimeToSystemTime(dDate + 2, &sDate))
        {
            int nAge = 0;
            SYSTEMTIME sToday = {0};

            GetLocalTime(&sToday);

            nAge = sToday.wYear - sDate.wYear;

            if (sToday.wMonth < sDate.wMonth || (sToday.wMonth == sDate.wMonth && sToday.wDay < sDate.wDay))
                nAge--;

            return nAge;
        }
    }
    return 0;
}


void parseDirectoryUserDetailsData(HANDLE hContact, oscar_tlv_chain *cDetails, DWORD dwCookie, fam15_cookie_data *pCookieData, WORD wReplySubType)
{
    oscar_tlv *pTLV;
    WORD wRecordCount;

    if (pCookieData->bRequestType == DIRECTORYREQUEST_INFOMULTI && !hContact)
    {
        DWORD dwUin = 0;
        char *szUid = getStrFromChain(cDetails, 0x32, 1);

        if (!szUid)
        {
            NetLog_Server("Error: Received unrecognized data from the directory");
            return;
        }

        if (IsStringUIN(szUid))
            dwUin = atoi(szUid);

        hContact = HContactFromUID(dwUin, szUid, NULL);
        if (hContact == INVALID_HANDLE_VALUE)
        {
            NetLog_Server("Error: Received details for unknown contact \"%s\"", szUid);
            mir_free(szUid);
            return;
        }
#ifdef _DEBUG
        else
            NetLog_Server("Received user info for %s from directory", szUid);
#endif
        mir_free(szUid);
    }
#ifdef _DEBUG
    else
    {
        char *szUid = getStrFromChain(cDetails, 0x32, 1);

        if (!hContact)
            NetLog_Server("Received owner user info from directory");
        else
            NetLog_Server("Received user info for %s from directory", szUid);
        mir_free(szUid);
    }
#endif

    pTLV = getTLV(cDetails, 0x50, 1);
    if (pTLV && pTLV->wLen > 0)
        writeDbInfoSettingTLVString(hContact, "e-mail",  cDetails, 0x50); // Verified e-mail
    else
        writeDbInfoSettingTLVString(hContact, "e-mail",  cDetails, 0x55); // Pending e-mail

    writeDbInfoSettingTLVStringUtf(hContact, "FirstName", cDetails, 0x64);
    writeDbInfoSettingTLVStringUtf(hContact, "LastName",  cDetails, 0x6E);
    writeDbInfoSettingTLVStringUtf(hContact, "Nick",      cDetails, 0x78);
    // Home Address
    parseUserInfoRecord(hContact, getTLV(cDetails, 0x96, 1), rAddress, SIZEOF(rAddress), 1);
    // Origin Address
    parseUserInfoRecord(hContact, getTLV(cDetails, 0xA0, 1), rOriginAddress, SIZEOF(rOriginAddress), 1);
    // Phones
    pTLV = getTLV(cDetails, 0xC8, 1);
    if (pTLV && pTLV->wLen >= 2)
    {
        BYTE *pRecords = pTLV->pData;
        oscar_tlv_record_list *cPhones;
        unpackWord(&pRecords, &wRecordCount);
        cPhones = readIntoTLVRecordList(&pRecords, pTLV->wLen - 2, wRecordCount);
        if (cPhones)
        {
            oscar_tlv_chain *cPhone;
            cPhone = getRecordByTLV(cPhones, 0x6E, 1);
            writeDbInfoSettingTLVString(hContact, "Phone", cPhone, 0x64);
            cPhone = getRecordByTLV(cPhones, 0x6E, 2);
            writeDbInfoSettingTLVString(hContact, "CompanyPhone", cPhone, 0x64);
            cPhone = getRecordByTLV(cPhones, 0x6E, 3);
            writeDbInfoSettingTLVString(hContact, "Cellular", cPhone, 0x64);
            cPhone = getRecordByTLV(cPhones, 0x6E, 4);
            writeDbInfoSettingTLVString(hContact, "Fax", cPhone, 0x64);
            cPhone = getRecordByTLV(cPhones, 0x6E, 5);
            writeDbInfoSettingTLVString(hContact, "CompanyFax", cPhone, 0x64);

            disposeRecordList(&cPhones);
        }
        else
        {
            // Remove old data when phones not available
            ICQDeleteContactSetting(hContact, "Phone");
            ICQDeleteContactSetting(hContact, "CompanyPhone");
            ICQDeleteContactSetting(hContact, "Cellular");
            ICQDeleteContactSetting(hContact, "Fax");
            ICQDeleteContactSetting(hContact, "CompanyFax");
        }
    }
    else
    {
        // Remove old data when phones not available
        ICQDeleteContactSetting(hContact, "Phone");
        ICQDeleteContactSetting(hContact, "CompanyPhone");
        ICQDeleteContactSetting(hContact, "Cellular");
        ICQDeleteContactSetting(hContact, "Fax");
        ICQDeleteContactSetting(hContact, "CompanyFax");
    }
    // Emails
    parseUserInfoRecord(hContact, getTLV(cDetails, 0x8C, 1), rEmail, SIZEOF(rEmail), 4);

    writeDbInfoSettingTLVByte(hContact, "Timezone", cDetails, 0x17C);
    // Company
    parseUserInfoRecord(hContact, getTLV(cDetails, 0x118, 1), rCompany, SIZEOF(rCompany), 1);
    // Education
    parseUserInfoRecord(hContact, getTLV(cDetails, 0x10E, 1), rEducation, SIZEOF(rEducation), 1);

    switch (getNumberFromChain(cDetails, 0x82, 1))
    {
    case 1:
        ICQGetContactSettingByte(hContact, "Gender", 'F');
        break;
    case 2:
        ICQGetContactSettingByte(hContact, "Gender", 'M');
        break;
    default:
        ICQDeleteContactSetting(hContact, "Gender");
    }

    writeDbInfoSettingTLVString(hContact, "Homepage", cDetails, 0xFA);
    writeDbInfoSettingTLVDate(hContact, "BirthYear", "BirthMonth", "BirthDay", cDetails, 0x1A4);

    writeDbInfoSettingTLVByte(hContact, "Language1", cDetails, 0xAA);
    writeDbInfoSettingTLVByte(hContact, "Language2", cDetails, 0xB4);
    writeDbInfoSettingTLVByte(hContact, "Language3", cDetails, 0xBE);

    writeDbInfoSettingTLVByte(hContact, "MaritalStatus", cDetails, 0x12C);
    // Interests
    parseUserInfoRecord(hContact, getTLV(cDetails, 0x122, 1), rInterest, SIZEOF(rInterest), 4);

    writeDbInfoSettingTLVStringUtf(hContact, "About", cDetails, 0x186);

    if (hContact)
        writeDbInfoSettingTLVStringUtf(hContact, DBSETTING_STATUS_NOTE, cDetails, 0x226);
    else
    {
        // Owner contact needs special processing, in the database is current status note for the client
        // We just received the last status note set on directory, if it differs call SetStatusNote() to
        // ensure the directory will be updated (it should be in process anyway)
//		char *szClientStatusNote = ICQGetContactSettingUtf(hContact, DBSETTING_STATUS_NOTE, NULL);
        char *szDirectoryStatusNote = getStrFromChain(cDetails, 0x226, 1);


        /*		if (strcmpnull(szClientStatusNote, szDirectoryStatusNote))
        			SetStatusNote(szClientStatusNote, 1000, TRUE);*/

        // Release memory
        mir_free(szDirectoryStatusNote);
//		mir_freeszClientStatusNote);
    }

    writeDbInfoSettingTLVByte(hContact, "PrivacyLevel", cDetails, 0x1F9);

    if (!hContact)
    {
        ICQWriteContactSettingByte(hContact, "Auth", !getByteFromChain(cDetails, 0x19A, 1));
        writeDbInfoSettingTLVByte(hContact, "WebAware", cDetails, 0x212);
        writeDbInfoSettingTLVByte(hContact, "AllowSpam", cDetails, 0x1EA);
    }

    writeDbInfoSettingTLVWord(hContact, "InfoCP", cDetails, 0x1C2);

    if (hContact)
    {
        // Handle deprecated setting (Age & Birthdate are not separate fields anymore)
        int nAge = calcAgeFromBirthDate(getDoubleFromChain(cDetails, 0x1A4, 1));

        if (nAge)
            ICQWriteContactSettingWord(hContact, "Age", nAge);
        else
            ICQDeleteContactSetting(hContact, "Age");
    }
    else // we do not need to calculate age for owner
        ICQDeleteContactSetting(hContact, "Age");

    {
        // Save user info last update time and privacy token
        double dInfoTime;
        BYTE pbEmptyMetaToken[0x10] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        int bHasMetaToken = FALSE;

        // Check if the details arrived with privacy token!
        if ((pTLV = getTLV(cDetails, 0x3C, 1)) && pTLV->wLen == 0x10 && memcmp(pTLV->pData, pbEmptyMetaToken, 0x10))
            bHasMetaToken = TRUE;

        // !Important, we need to save the MDir server-item time - it can be newer than the one from the directory
        if ((dInfoTime = ICQGetContactSettingDouble(hContact, DBSETTING_METAINFO_TIME, 0)) > 0)
            ICQWriteContactSettingDouble(hContact, DBSETTING_METAINFO_SAVED, dInfoTime);
        else if (bHasMetaToken || !hContact)
            writeDbInfoSettingTLVDouble(hContact, DBSETTING_METAINFO_SAVED, cDetails, 0x1CC);
        else
            ICQWriteContactSettingDword(hContact, DBSETTING_METAINFO_SAVED, time(NULL));
    }

    if (wReplySubType == META_DIRECTORY_RESPONSE)
        if (pCookieData->bRequestType == DIRECTORYREQUEST_INFOUSER)
            ICQBroadcastAck(hContact, ACKTYPE_GETINFO, ACKRESULT_SUCCESS, (HANDLE)1 ,0);

    // Remove user from info update queue. Removing is fast so we always call this
    // even if it is likely that the user is not queued at all.
    if (hContact)
        icq_DequeueUser(ICQGetContactSettingUIN(hContact));
}


void parseDirectorySearchData(oscar_tlv_chain *cDetails, DWORD dwCookie, fam15_cookie_data *pCookieData, WORD wReplySubType)
{
    ICQSEARCHRESULT isr = {0};
    char *szUid = getStrFromChain(cDetails, 0x32, 1); // User ID
    oscar_tlv *pTLV;
    char *szData = NULL;


#ifdef _DEBUG
    NetLog_Server("Directory Search: Found user %s", szUid);
#endif
    isr.hdr.cbSize = sizeof(ICQSEARCHRESULT);
#ifndef _08CORE
    isr.hdr.flags = PSR_TCHAR;
    isr.hdr.id = (char*)mir_utf8encode(szUid);
#endif

    if (IsStringUIN(szUid))
        isr.uin = atoi(szUid);
    else
        isr.uin = 0;

    mir_free(szUid);

    pTLV = getTLV(cDetails, 0x50, 1);


    if (pTLV && pTLV->wLen > 0)
        szData = getStrFromChain(cDetails, 0x50, 1); // Verified e-mail
    else
        szData = getStrFromChain(cDetails, 0x55, 1); // Pending e-mail
    if (strlennull(szData))
        isr.hdr.email = (char*)mir_utf8encode(szData);
    mir_free(szData);

    szData = getStrFromChain(cDetails, 0x64, 1); // First Name
    if (strlennull(szData))
        isr.hdr.firstName = (char*)mir_utf8encode(szData);
    mir_free(szData);

    szData = getStrFromChain(cDetails, 0x6E, 1); // Last Name
    if (strlennull(szData))
        isr.hdr.lastName = (char*)mir_utf8encode(szData);
    mir_free(szData);

    szData = getStrFromChain(cDetails, 0x78, 1); // Nick
    if (strlennull(szData))
        isr.hdr.nick = (char*)mir_utf8encode(szData);
    mir_free(szData);

    switch (getNumberFromChain(cDetails, 0x82, 1)) // Gender
    {
    case 1:
        isr.gender = 'F';
        break;
    case 2:
        isr.gender = 'M';
        break;
    }

    pTLV = getTLV(cDetails, 0x96, 1);
    if (pTLV && pTLV->wLen >= 4)
    {
        BYTE *buf = pTLV->pData;
        oscar_tlv_chain *chain = readIntoTLVChain(&buf, pTLV->wLen, 0);
        if (chain)
            isr.country = getDWordFromChain(chain, 0x8C, 1); // Home Country
        disposeChain(&chain);
    }

    isr.auth = !getByteFromChain(cDetails, 0x19A, 1); // Require Authorization
    isr.maritalStatus = getNumberFromChain(cDetails, 0x12C, 1); // Marital Status

    // calculate Age if Birthdate is available
    isr.age = calcAgeFromBirthDate(getDoubleFromChain(cDetails, 0x1A4, 1));

    // Finally, broadcast the result
    ICQBroadcastAck(NULL, ACKTYPE_SEARCH, ACKRESULT_DATA, (HANDLE)dwCookie, (LPARAM)&isr);

    // Release memory
#ifndef _08CORE
    mir_free(isr.hdr.id);
#endif
    mir_free(isr.hdr.nick);
    mir_free(isr.hdr.firstName);
    mir_free(isr.hdr.lastName);
    mir_free(isr.hdr.email);

    // Search is over, broadcast final ack
    if (wReplySubType == META_DIRECTORY_RESPONSE)
        ICQBroadcastAck(NULL, ACKTYPE_SEARCH, ACKRESULT_SUCCESS, (HANDLE)dwCookie, 0);
}


void handleDirectoryUpdateResponse(BYTE *databuf, WORD wPacketLen, WORD wCookie, WORD wReplySubtype)
{
    WORD wBytesRemaining = 0, wLen = 0;
    snac_header requestSnac = {0};
    BYTE requestResult;
    HANDLE hContact;
    fam15_cookie_data *pCookieData;

#ifdef _DEBUG
    NetLog_Server("Received directory update response");
#endif
    if (wPacketLen >= 2)
        unpackLEWord(&databuf, &wBytesRemaining);
    wPacketLen -= 2;

    if (!unpackSnacHeader(&requestSnac, &databuf, &wPacketLen) || !requestSnac.bValid)
    {
        NetLog_Server("Error: Failed to parse directory response");
        return;
    }

    // check request cookie
    if (!FindCookie(wCookie, &hContact, (void**)&pCookieData) || !pCookieData)
    {
        NetLog_Server("Warning: Ignoring unrequested directory reply type (x%x, x%x)", requestSnac.wFamily, requestSnac.wSubtype);
        return;
    }
    /// FIXME: we should really check the snac contents according to cookie data here ??

    if (wPacketLen >= 3)
        unpackByte(&databuf, &requestResult);
    else
    {
        NetLog_Server("Error: Malformed directory response");
        ReleaseCookie(wCookie);
        return;
    }
    if (requestResult != 1 && requestResult != 4)
    {
        NetLog_Server("Error: Directory request failed, status %u", requestResult);

        if (pCookieData->bRequestType == DIRECTORYREQUEST_UPDATEOWNER)
            ICQBroadcastAck(NULL, ACKTYPE_SETINFO, ACKRESULT_FAILED, (HANDLE)wCookie, 0);

        ReleaseCookie(wCookie);
        return;
    }

    unpackWord(&databuf, &wLen);
    wPacketLen -= 3;
    if (wLen)
        NetLog_Server("Warning: Data in error message present!");

    if (pCookieData->bRequestType == DIRECTORYREQUEST_UPDATEOWNER)
        ICQBroadcastAck(NULL, ACKTYPE_SETINFO, ACKRESULT_SUCCESS, (HANDLE)wCookie, 0);
    if (wPacketLen == 0x18)
    {
        DWORD64 qwMetaTime;
        BYTE pbEmptyMetaToken[0x10] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

        unpackQWord(&databuf, &qwMetaTime);
        ICQWriteContactSettingBlob(NULL, DBSETTING_METAINFO_TIME, (BYTE*)&qwMetaTime, 8);

        if (memcmp(databuf, pbEmptyMetaToken, 0x10))
            ICQWriteContactSettingBlob(NULL, DBSETTING_METAINFO_TOKEN, databuf, 0x10);
    }
    ReleaseCookie(wCookie);
}