// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright � 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright � 2001,2002 Jon Keating, Richard Hughes
// Copyright � 2002,2003,2004 Martin �berg, Sam Kothari, Robert Rainwater
// Copyright � 2004,2005,2006,2007 Joe Kucera
// Copyright � 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
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
// Revision       : $Revision: 51 $
// Last change on : $Date: 2007-08-30 23:46:51 +0300 (Чт, 30 авг 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Handlers for Family 4 ICBM Messages
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"


static void handleReplyICBM(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwRef);
static void handleRecvServMsg(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwRef);
static void handleRecvServMsgType1(unsigned char *buf, WORD wLen, DWORD dwUin, char *szUID, DWORD dwMsgID1, DWORD dwMsgID2, DWORD dwRef);
static void handleRecvServMsgType2(unsigned char *buf, WORD wLen, DWORD dwUin, char *szUID, DWORD dwTS1, DWORD dwTS2);
static void handleRecvServMsgType4(unsigned char *buf, WORD wLen, DWORD dwUin, char *szUID, DWORD dwMsgID1, DWORD dwMsgID2, DWORD dwRef);
static void handleRecvServMsgError(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwRef);
static void handleRecvMsgResponse(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwRef);
static void handleServerAck(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwRef);
static void handleTypingNotification(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwRef);
static void handleMissedMsg(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwRef);
static void handleOffineMessagesReply(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwRef);
static void parseTLV2711(DWORD dwUin, HANDLE hContact, DWORD dwID1, DWORD dwID2, WORD wAckType, oscar_tlv* tlv);
static void parseServerGreeting(BYTE* pDataBuf, WORD wLen, WORD wMsgLen, DWORD dwUin, BYTE bFlags, WORD wStatus, WORD wCookie, WORD wAckType, DWORD dwID1, DWORD dwID2, WORD wVersion);
static void handleRecvServMsgContacts(unsigned char *buf, WORD wLen, DWORD dwUin, char *szUID, DWORD dwID1, DWORD dwID2, WORD wCommand);
static BYTE OfflineMessagesReceived = 0;


void handleMsgFam(unsigned char *pBuffer, WORD wBufferLength, snac_header* pSnacHeader)
{
    switch (pSnacHeader->wSubtype)
    {

    case ICQ_MSG_SRV_ERROR:          // SNAC(4, 0x01)
        handleRecvServMsgError(pBuffer, wBufferLength, pSnacHeader->wFlags, pSnacHeader->dwRef);
        break;

    case ICQ_MSG_SRV_RECV:           // SNAC(4, 0x07)
        handleRecvServMsg(pBuffer, wBufferLength, pSnacHeader->wFlags, pSnacHeader->dwRef);
        break;

    case ICQ_MSG_SRV_MISSED_MESSAGE: // SNAC(4, 0x0A)
        handleMissedMsg(pBuffer, wBufferLength, pSnacHeader->wFlags, pSnacHeader->dwRef);
        break;

    case ICQ_MSG_RESPONSE:           // SNAC(4, 0x0B)
        handleRecvMsgResponse(pBuffer, wBufferLength, pSnacHeader->wFlags, pSnacHeader->dwRef);
        break;

    case ICQ_MSG_SRV_ACK:            // SNAC(4, 0x0C) Server acknowledgements
        handleServerAck(pBuffer, wBufferLength, pSnacHeader->wFlags, pSnacHeader->dwRef);
        break;

    case ICQ_MSG_MTN:                // SNAC(4, 0x14) Typing notifications
        handleTypingNotification(pBuffer, wBufferLength, pSnacHeader->wFlags, pSnacHeader->dwRef);
        break;

    case ICQ_MSG_SRV_OFFLINE_REPLY:  // SNAC(4, 0x17) Offline Messages response
        handleOffineMessagesReply(pBuffer, wBufferLength, pSnacHeader->wFlags, pSnacHeader->dwRef);
        break;

    case ICQ_MSG_SRV_REPLYICBM:      // SNAC(4, 0x05) SRV_REPLYICBM
        handleReplyICBM(pBuffer, wBufferLength, pSnacHeader->wFlags, pSnacHeader->dwRef);
        break;

    default:
        NetLog_Server("Warning: Ignoring SNAC(x%02x,x%02x) - Unknown SNAC (Flags: %u, Ref: %u)", ICQ_MSG_FAMILY, pSnacHeader->wSubtype, pSnacHeader->wFlags, pSnacHeader->dwRef);
        break;
    }
}



static void setMsgChannelParams(WORD wChan, DWORD dwFlags)
{
    icq_packet packet;

    // Set message parameters for channel wChan (CLI_SET_ICBM_PARAMS)
    serverPacketInit(&packet, 26);
    packFNACHeader(&packet, ICQ_MSG_FAMILY, ICQ_MSG_CLI_SETPARAMS);
    packWord(&packet, wChan);               // Channel
    packDWord(&packet, dwFlags);            // Flags
    packWord(&packet, MAX_MESSAGESNACSIZE); // Max message snac size
    packWord(&packet, 0x03E7);              // Max sender warning level
    packWord(&packet, 0x03E7);              // Max receiver warning level
    packWord(&packet, CLIENTRATELIMIT);     // Minimum message interval in seconds
    packWord(&packet, 0x0000);              // Unknown
    sendServPacket(&packet);
}



static void handleReplyICBM(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwRef)
{
    // we don't care about the stuff, just change the params
    DWORD dwFlags = 0x00000303;

#ifdef DBG_CAPHTML
    dwFlags |= 0x00000400;
#endif
#ifdef DBG_CAPMTN
    dwFlags |= 0x00000008;
#endif
    // Set message parameters for all channels (imitate ICQ 6)
    setMsgChannelParams(0x0000, dwFlags);
}



static void handleRecvServMsg(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwRef)
{
    DWORD dwUin;
    DWORD dwID1;
    DWORD dwID2;
    WORD wTLVCount;
    WORD wMessageFormat;
    uid_str szUID;

    if (wLen < 11)
    {
        // just do some basic packet checking
        NetLog_Server("Error: Malformed message thru server");
        return;
    }

    // These two values are some kind of reference, we need to save
    // them to send file request responses for example
    unpackLEDWord(&buf, &dwID1); // TODO: msg cookies should be main
    wLen -= 4;
    unpackLEDWord(&buf, &dwID2);
    wLen -= 4;

    // The message type used:
    unpackWord(&buf, &wMessageFormat); //  0x0001: Simple message format
    wLen -= 2;                         //  0x0002: Advanced message format
    //  0x0004: 'New' message format
    // Sender UIN
    if (!unpackUID(&buf, &wLen, &dwUin, &szUID)) return;

    if (dwUin && IsOnSpammerList(dwUin))
    {
        NetLog_Server("Ignored Message from known Spammer");
        return;
    }

    if (wLen < 4)
    {
        // just do some basic packet checking
        NetLog_Server("Error: Malformed message thru server");
        return;
    }

    // Warning level?
    buf += 2;
    wLen -= 2;

    // Number of following TLVs, until msg-format dependant TLVs
    unpackWord(&buf, &wTLVCount);
    wLen -= 2;
    if (wTLVCount > 0)
    {
        // Save current buffer pointer so we can calculate
        // how much data we have left after the chain read.
        oscar_tlv_chain* chain = NULL;
        unsigned char* pBufStart = buf;

        chain = readIntoTLVChain(&buf, wLen, wTLVCount);

        // This chain contains info that is filled in by the server.
        // TLV(1): unknown
        // TLV(2): date: on since
        // TLV(3): date: on since
        // TLV(4): unknown, usually 0000. Not in file-req or auto-msg-req
        // TLV(6): sender's status
        // TLV(F): a time in seconds, unknown

        disposeChain(&chain);

        // Update wLen
        wLen = wLen - (buf - pBufStart);
    }


    // This is where the format specific data begins

    switch (wMessageFormat)
    {

    case 1: // Simple message format
        handleRecvServMsgType1(buf, wLen, dwUin, szUID, dwID1, dwID2, dwRef);
        break;

    case 2: // Encapsulated messages
        handleRecvServMsgType2(buf, wLen, dwUin, szUID, dwID1, dwID2);
        break;

    case 4: // Typed messages
        handleRecvServMsgType4(buf, wLen, dwUin, szUID, dwID1, dwID2, dwRef);
        break;

    default:
        NetLog_Server("Unknown format message thru server - Ref %u, Type: %u, UID: %s", dwRef, wMessageFormat, strUID(dwUin, szUID));
        break;

    }
}



static char* convertMsgToUserSpecificUtf(HANDLE hContact, const char* szMsg)
{
    WORD wCP = getSettingWord(hContact, "CodePage", gwAnsiCodepage);
    char* usMsg = NULL;

    if (wCP != CP_ACP)
        usMsg = mir_utf8encodecp(szMsg, wCP);

    return usMsg;
}



static void handleRecvServMsgType1(unsigned char *buf, WORD wLen, DWORD dwUin, char *szUID, DWORD dwMsgID1, DWORD dwMsgID2, DWORD dwRef) //TODO: rewrite
{
    WORD wTLVType;
    WORD wTLVLen;
    BYTE* pMsgTLV;
    HANDLE hContact;


    hContact = HContactFromUID(dwUin, szUID, 0);
    if (wLen < 4)
    {
        // just perform basic structure check
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        CheckContact(chk);
        NetLog_Server("Message (format %u) - Ignoring empty message", 1);
        return;
    }

    // Unpack the first TLV(2)
    unpackTypedTLV(buf, wLen, 2, &wTLVType, &wTLVLen, (char**)&pMsgTLV);
    NetLog_Server("Message (format %u) - UID: %s", 1, strUID(dwUin, szUID));

    // It must be TLV(2)
    if (wTLVType == 2)
    {
        BYTE *pDataBuf = pMsgTLV;
        oscar_tlv_chain* pChain;

        pChain = readIntoTLVChain(&pDataBuf, wTLVLen, 0);

        // TLV(2) contains yet another TLV chain with the following TLVs:
        //   TLV(1281): Capability
        //   TLV(257):  This TLV contains the actual message (can be fragmented)

        if (pChain)
        {
            oscar_tlv* pMessageTLV;
            oscar_tlv* pCapabilityTLV;
            WORD wMsgPart = 1;

            // Find the capability TLV
            pCapabilityTLV = getTLV(pChain, 0x0501, 1);
            if (pCapabilityTLV && (pCapabilityTLV->wLen > 0))
            {
                WORD wDataLen;
                BYTE *pDataBuf;

                wDataLen = pCapabilityTLV->wLen;
                pDataBuf = pCapabilityTLV->pData;

                if (wDataLen > 0)
                {
                    NetLog_Server("Message (format 1) - Message has %d caps.", wDataLen);
                }
            }
            else
            {
                NetLog_Server("Message (format 1) - No message cap.");
            }

            {
                // Parse the message parts, usually only one 0x0101 TLV containing the message,
                // but in some cases there can be more 0x0101 TLVs containing message parts in
                // different encodings (just like the new format of Offline Messages).
                DWORD dwRecvTime;
                char* szMsg = NULL;
                HANDLE hContact;
                CCSDATA ccs;
                PROTORECVEVENT pre = {0};
                int bAdded;

                hContact = HContactFromUID(dwUin, szUID, &bAdded);

                while (pMessageTLV = getTLV(pChain, 0x0101, wMsgPart))
                {
                    // Loop thru all message parts
                    if (pMessageTLV->wLen > 4)
                    {
                        WORD wMsgLen;
                        BYTE* pMsgBuf;
                        WORD wEncoding;
                        WORD wCodePage;
                        char* szMsgPart = NULL;
                        int bMsgPartUnicode = FALSE;

                        // The message begins with a encoding specification
                        // The first WORD is believed to have the following meaning:
                        //  0x00: US-ASCII
                        //  0x02: Unicode UCS-2 Big Endian encoding
                        //  0x03: local 8bit encoding
                        pMsgBuf = pMessageTLV->pData;
                        unpackWord(&pMsgBuf, &wEncoding);
                        unpackWord(&pMsgBuf, &wCodePage);

                        wMsgLen = pMessageTLV->wLen - 4;
                        NetLog_Server("Message (format 1) - Part %d: Encoding is 0x%X, page is 0x%X", wMsgPart, wEncoding, wCodePage);

                        switch (wEncoding)
                        {

                        case 2: // UCS-2
                        {
                            WCHAR* usMsgPart = (WCHAR*)icq_alloc_zero(wMsgLen + 2);

                            bMsgPartUnicode = TRUE;

                            unpackWideString(&pMsgBuf, usMsgPart, wMsgLen);
                            usMsgPart[wMsgLen/sizeof(WCHAR)] = 0;

                            szMsgPart = mir_utf8encodeW(usMsgPart);
                            mir_free(usMsgPart);

                            break;
                        }

                        case 0: // us-ascii
                        case 3: // ANSI
                        default:
                        {
                            // Copy the message text into a new proper string.
                            szMsgPart = (char *)icq_alloc_zero(wMsgLen + 1);
                            memcpy(szMsgPart, pMsgBuf, wMsgLen);
                            szMsgPart[wMsgLen] = '\0';

                            break;
                        }
                        }
                        // Check if the new part is compatible with the message
                        if (!pre.flags && bMsgPartUnicode)
                        {
                            // make the resulting message utf-8 encoded - need to append utf-8 encoded part
                            if (szMsg)
                            {
                                // not necessary to convert - appending first part, only set flags
                                char* szUtfMsg = mir_utf8encodecp(szMsg, getSettingWord(hContact, "CodePage", gwAnsiCodepage));

                                mir_free(szMsg);
                                szMsg = szUtfMsg;
                            }
                            pre.flags = PREF_UTF;
                        }
                        if (!bMsgPartUnicode && pre.flags == PREF_UTF)
                        {
                            // convert message part to utf-8 and append
                            char* szUtfPart = mir_utf8encodecp(szMsgPart, getSettingWord(hContact, "CodePage", gwAnsiCodepage));

                            mir_free(szMsgPart);
                            szMsgPart = szUtfPart;
                        }
                        // Append the new message part
                        {
                            char *tmp = (char*)icq_alloc_zero(strlennull(szMsg) + strlennull(szMsgPart) + 1);
                            if(szMsg)
                                strcat(tmp, szMsg);
                            strcat(tmp, szMsgPart);
                            mir_free(szMsg);
                            szMsg = (char*)icq_alloc_zero(strlennull(tmp) + 1);
                            strcpy(szMsg, tmp);
                            mir_free(tmp);
                        }
                        mir_free(szMsgPart);
                    }
                    wMsgPart++;
                }
                if (strlennull(szMsg))
                {
                    /// TODO: Add inteligent HTML detection & stripping - support for CAPF_HTML
                    if (!dwUin)
                    {
                        // strip HTML formating from AIM message
                        szMsg = EliminateHtml(szMsg, strlennull(szMsg));
                    }

                    if (!pre.flags && !IsUSASCII(szMsg, strlennull(szMsg)))
                    {
                        // message is Ansi and contains national characters, create Unicode part by codepage
                        char* usMsg = convertMsgToUserSpecificUtf(hContact, szMsg);

                        if (usMsg)
                        {
                            mir_free(szMsg);
                            szMsg = usMsg;
                            pre.flags = PREF_UTF;
                        }
                    }

                    dwRecvTime = (DWORD)time(NULL);

                    {
                        // Check if the message was received as offline
                        offline_message_cookie *cookie;

                        if (!(dwRef & 0x80000000) && FindCookie(dwRef, NULL, (void**)&cookie))
                        {
                            WORD wTimeTLVType, wTimeTLVLen;
                            BYTE *pTimeTLV;

                            cookie->nMessages++;

                            unpackTypedTLV(buf, wLen, 0x16, &wTimeTLVType, &wTimeTLVLen, (char**)&pTimeTLV);
                            if (pTimeTLV && wTimeTLVType == 0x16 && wTimeTLVLen == 4)
                            {
                                // found Offline timestamp
                                BYTE *pBuf = pTimeTLV;
                                unpackDWord(&pBuf, &dwRecvTime);
                                NetLog_Server("Message (format %u) - Offline timestamp is %s", 1, time2text(dwRecvTime));
                            }
                            mir_free(pTimeTLV);
                        }
                    }
                    // Create and send the message event
                    ccs.szProtoService = PSR_MESSAGE;
                    ccs.hContact = hContact;
                    ccs.wParam = 0;
                    ccs.lParam = (LPARAM)&pre;
                    pre.timestamp = dwRecvTime;
                    pre.szMessage = (char *)szMsg;
                    CallService(MS_PROTO_CHAINRECV, 0, (LPARAM)&ccs);

                    NetLog_Server("Message (format 1) received");

                    // Save tick value
                    setSettingDword(ccs.hContact, "TickTS", time(NULL) - (dwMsgID1/1000));
                }
                else
                {
                    NetLog_Server("Message (format %u) - Ignoring empty message", 1);
                }
                mir_free(szMsg);
            }

            // Free the chain memory
            disposeChain(&pChain);
        }
        else
        {
            NetLog_Server("Failed to read TLV chain in message (format 1)");
        }
    }
    else
    {
        NetLog_Server("Unsupported TLV (%u) in message (format %u)", wTLVType, 1);
    }
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        if(!OfflineMessagesReceived)
            chk.PSD=-1;
        CheckContact(chk);
    }
    mir_free(pMsgTLV);
}



static void handleRecvServMsgType2(unsigned char *buf, WORD wLen, DWORD dwUin, char *szUID, DWORD dwID1, DWORD dwID2)
{
    WORD wTLVType;
    WORD wTLVLen;
    char* pDataBuf = NULL;
    char* pBuf;
    HANDLE hContact = HContactFromUIN(dwUin, 0);


    if (wLen < 4)
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        chk.PSD=22;
        CheckContact(chk);
        NetLog_Server("Message (format %u) - Ignoring empty message", 2);
        return;
    }

    // Unpack the first TLV(5)
    unpackTypedTLV(buf, wLen, 5, &wTLVType, &wTLVLen, &pDataBuf);
    NetLog_Server("Message (format %u) - UID: %s", 2, strUID(dwUin, szUID));
    pBuf = pDataBuf;

    // It must be TLV(5)
    if (wTLVType == 5)
    {
        WORD wCommand;
        oscar_tlv_chain* chain;
        oscar_tlv* tlv;
        DWORD dwIP;
        DWORD dwExternalIP;
        WORD wPort;
        WORD wAckType;
        DWORD q1,q2,q3,q4;

        if (wTLVLen < 26)
        {
            // just check if all basic data is there
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            chk.PSD=22;
            CheckContact(chk);
            NetLog_Server("Message (format %u) - Ignoring empty message", 2);

            return;
        }

        unpackWord((BYTE**)&pDataBuf, &wCommand);
        wTLVLen -= 2;                             // Command 0x0000 - Normal message/file send request
#ifdef _DEBUG                                 //         0x0001 - Abort request
        NetLog_Server("Command is %u", wCommand); //         0x0002 - Acknowledge request
#endif

        // Some stuff we don't use
        pDataBuf += 8;  // dwID1 and dwID2 again
        wTLVLen -= 8;
        unpackDWord((BYTE**)&pDataBuf, &q1);
        unpackDWord((BYTE**)&pDataBuf, &q2);
        unpackDWord((BYTE**)&pDataBuf, &q3);
        unpackDWord((BYTE**)&pDataBuf, &q4); // Message Capability
        wTLVLen -= 16;

        if (CompareGUIDs(q1,q2,q3,q4, MCAP_SRV_RELAY_FMT))
        {
            // we surely have at least 4 bytes for TLV chain

            if (wCommand == 1)
            {
                CHECKCONTACT chk = {0};
                chk.hContact=hContact;
                chk.dwUin=dwUin;
                chk.PSD=22;
                CheckContact(chk);
                NetLog_Server("Cannot handle abort messages yet... :(");
                mir_free(pBuf);
                return;
            }

            if (wTLVLen < 4)
            {
                // just check if at least one tlv is there
                CHECKCONTACT chk = {0};
                chk.hContact=hContact;
                chk.dwUin=dwUin;
                chk.PSD=22;
                CheckContact(chk);
                NetLog_Server("Message (format %u) - Ignoring empty message", 2);
                mir_free(pBuf);
                return;
            }
            if (!dwUin)
            {
                // AIM cannot send this, just sanity
                NetLog_Server("Error: Malformed UIN in packet");
                mir_free(pBuf);
                return;
            }

            // This TLV chain may contain the following TLVs:
            // TLV(A): Acktype 0x0000 - normal message
            //                 0x0001 - file request / abort request
            //                 0x0002 - file ack
            // TLV(F): Unknown
            // TLV(3): External IP
            // TLV(5): DC port (not to use for filetransfers)
            // TLV(0x2711): The next message level

            chain = readIntoTLVChain((BYTE**)&pDataBuf, wTLVLen, 0);

            wAckType = getWordFromChain(chain, 0x0A, 1);

            // Update the saved DC info (if contact already exists)
            if (hContact != INVALID_HANDLE_VALUE)
            {
                if (dwExternalIP = getDWordFromChain(chain, 0x03, 1))
                    setSettingDword(hContact, "RealIP", dwExternalIP);
                if (dwIP = getDWordFromChain(chain, 0x04, 1))
                    setSettingDword(hContact, "IP", dwIP);
                if (wPort = getWordFromChain(chain, 0x05, 1))
                    setSettingWord(hContact, "UserPort", wPort);

                // Save tick value
                setSettingDword(hContact, "TickTS", time(NULL) - (dwID1/1000));
            }

            // Parse the next message level
            if (tlv = getTLV(chain, 0x2711, 1))
            {
                parseTLV2711(dwUin, hContact, dwID1, dwID2, wAckType, tlv);
            }
            else
            {
                NetLog_Server("Warning, no 0x2711 TLV in message (format 2)");
            }
            // Clean up
            disposeChain(&chain);
        }
        else if (CompareGUIDs(q1,q2,q3,q4, MCAP_REVERSE_DC_REQ))
        {
            // Handle reverse DC request
            if (wCommand == 1)
            {
                CHECKCONTACT chk = {0};
                chk.hContact=hContact;
                chk.dwUin=dwUin;
                chk.PSD=22;
                CheckContact(chk);

                NetLog_Server("Cannot handle abort messages yet... :(");
                mir_free(pBuf);
                return;
            }
            if (wTLVLen < 4)
            {
                // just check if at least one tlv is there
                CHECKCONTACT chk = {0};
                chk.hContact=hContact;
                chk.dwUin=dwUin;
                chk.PSD=22;
                CheckContact(chk);
                NetLog_Server("Message (format %u) - Ignoring empty message", 2);
                mir_free(pBuf);
                return;
            }
            if (!dwUin)
            {
                // AIM cannot send this, just sanity
                NetLog_Server("Error: Malformed UIN in packet");
                mir_free(pBuf);
                return;
            }
            chain = readIntoTLVChain((BYTE**)&pDataBuf, wTLVLen, 0);

            wAckType = getWordFromChain(chain, 0x0A, 1);
            // Parse the next message level
            if (tlv = getTLV(chain, 0x2711, 1))
            {
                if (tlv->wLen == 0x1B)
                {
                    char* buf = (char*)tlv->pData;
                    DWORD dwUin, dwIp, dwPort;
                    WORD wVersion;
                    BYTE bMode;
                    HANDLE hContact;

                    unpackLEDWord((BYTE**)&buf, &dwUin);

                    hContact = HContactFromUIN(dwUin, 0);
                    if (hContact == INVALID_HANDLE_VALUE)
                    {
                        NetLog_Server("Error: %s from unknown contact %u", "Reverse Connect Request", dwUin);
                    }
                    else
                    {
                        unpackDWord((BYTE**)&buf, &dwIp);
                        unpackLEDWord((BYTE**)&buf, &dwPort);
                        unpackByte((BYTE**)&buf, &bMode);
                        buf += 4; // unknown
                        if (dwPort)
                            buf += 4;  // port, again?
                        else
                            unpackLEDWord((BYTE**)&buf, &dwPort);
                        unpackLEWord((BYTE**)&buf, &wVersion);

                        setSettingDword(hContact, "IP", dwIp);
                        setSettingWord(hContact,  "UserPort", (WORD)dwPort);
                        setSettingByte(hContact,  "DCType", bMode);
                        setSettingWord(hContact,  "Version", wVersion);
                        if (wVersion>6)
                        {
                            reverse_cookie *pCookie = NULL;
                            pCookie = (reverse_cookie*)icq_alloc_zero(sizeof(reverse_cookie));

                            unpackLEDWord((BYTE**)&buf, (DWORD*)&pCookie->ft);
                            pCookie->pMessage.dwMsgID1 = dwID1;
                            pCookie->pMessage.dwMsgID2 = dwID2;

                            OpenDirectConnection(hContact, DIRECTCONN_REVERSE, (void*)pCookie);
                        }
                        else
                            NetLog_Server("Warning: Unsupported direct protocol version in %s", "Reverse Connect Request");
                    }
                }
                else
                {
                    NetLog_Server("Malformed %s", "Reverse Connect Request");
                }
            }
            else
            {
                NetLog_Server("Warning, no 0x2711 TLV in message (format 2)");
            }
            // Clean up
            disposeChain(&chain);
        }
        else if (CompareGUIDs(q1,q2,q3,q4, MCAP_FILE_TRANSFER))
        {
            // this is an OFT packet
            handleRecvServMsgOFT((BYTE*)pDataBuf, wTLVLen, dwUin, szUID, dwID1, dwID2, wCommand);
        }
        else if (CompareGUIDs(q1,q2,q3,q4, MCAP_CONTACTS))
        {
            // this is Contacts Transfer
            handleRecvServMsgContacts((BYTE*)pDataBuf, wTLVLen, dwUin, szUID, dwID1, dwID2, wCommand);
        }
        else // here should be detection of extra data streams (Xtraz)
        {
            NetLog_Server("Unknown Message Format Capability");
        }
    }
    else
    {
        NetLog_Server("Unsupported TLV (%u) in message (format %u)", wTLVType, 2);
    }
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        CheckContact(chk);
    }
    mir_free(pBuf);
}



static void parseTLV2711(DWORD dwUin, HANDLE hContact, DWORD dwID1, DWORD dwID2, WORD wAckType, oscar_tlv* tlv)
{
    BYTE* pDataBuf;
    WORD wId;
    WORD wLen;

    pDataBuf = tlv->pData;
    wLen = tlv->wLen;



    if (wLen < 2)
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        chk.PSD=22;
        CheckContact(chk);
        NetLog_Server("Message (format %u) - Ignoring empty message", 2);
        return;
    }

    unpackLEWord(&pDataBuf, &wId); // Incorrect identification, but working
    wLen -= 2;

    // Only 0x1B are real messages
    if (wId == 0x001B)
    {
        WORD wVersion;
        WORD wCookie;
        WORD wMsgLen;
        BYTE bMsgType;
        BYTE bFlags;
        DWORD dwGuid1,dwGuid2,dwGuid3,dwGuid4;

        if (wLen < 31)
        {
            // just check if we have data to work with
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            chk.PSD=22;
            CheckContact(chk);
            NetLog_Server("Message (format %u) - Ignoring empty message", 2);
            return;
        }

        unpackLEWord(&pDataBuf, &wVersion);
        wLen -= 2;

        if (hContact != INVALID_HANDLE_VALUE)
            setSettingWord(hContact, "Version", wVersion);

        unpackDWord(&pDataBuf, &dwGuid1); // plugin type GUID
        unpackDWord(&pDataBuf, &dwGuid2);
        unpackDWord(&pDataBuf, &dwGuid3);
        unpackDWord(&pDataBuf, &dwGuid4);
        wLen -= 16;

        // Skip lots of unused stuff
        pDataBuf += 9;
        wLen -= 9;

        unpackLEWord(&pDataBuf, &wId);
        wLen -= 2;

        unpackLEWord(&pDataBuf, &wCookie);
        wLen -= 2;

        if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, PSIG_MESSAGE))
        {
            // is this a normal message ?
            WORD wPritority;
            WORD wStatus;

            if (wLen < 20)
            {
                // check if there is everything that should be there
                CHECKCONTACT chk = {0};
                chk.hContact=hContact;
                chk.dwUin=dwUin;
                chk.PSD=22;
                CheckContact(chk);
                NetLog_Server("Message (format %u) - Ignoring empty message", 2);
                return;
            }

            pDataBuf += 12;  /* all zeroes */
            wLen -= 12;
            unpackByte(&pDataBuf, &bMsgType);
            wLen -= 1;
            unpackByte(&pDataBuf, &bFlags);
            wLen -= 1;

            // Status
            unpackLEWord(&pDataBuf, &wStatus);
            wLen -= 2;

            // Priority
            unpackLEWord(&pDataBuf, &wPritority);
            wLen -= 2;
            NetLog_Server("Priority: %u", wPritority);

            // Message
            unpackLEWord(&pDataBuf, &wMsgLen);
            wLen -= 2;

            // HANDLERS
            switch (bMsgType)
            {
                // File messages, handled by the file module
            case MTYPE_FILEREQ:
            {
                char* szMsg;

                szMsg = (char *)icq_alloc_zero(wMsgLen + 1);
                memcpy(szMsg, pDataBuf, wMsgLen);
                szMsg[wMsgLen] = '\0';
                pDataBuf += wMsgLen;
                wLen -= wMsgLen;

                if (wAckType == 0 || wAckType == 1)
                {
                    // File requests 7
                    handleFileRequest(pDataBuf, wLen, dwUin, wCookie, dwID1, dwID2, szMsg, 7, FALSE);
                }
                else if (wAckType == 2)
                {
                    // File reply 7
                    handleFileAck(pDataBuf, wLen, dwUin, wCookie, wStatus, szMsg);
                }
                else
                {
                    NetLog_Server("Ignored strange file message");
                }

                break;
            }

            // Chat messages, handled by the chat module
            case MTYPE_CHAT:
            {
                // TODO: this type is deprecated
                break;
            }

            // Plugin messages, need further parsing
            case MTYPE_PLUGIN:
            {
                parseServerGreeting(pDataBuf, wLen, wMsgLen, dwUin, bFlags, wStatus, wCookie, wAckType, dwID1, dwID2, wVersion);
                break;
            }

            // Everything else
            default:
            {
                message_ack_params pMsgAck = {0};

                pMsgAck.bType = MAT_SERVER_ADVANCED;
                pMsgAck.dwUin = dwUin;
                pMsgAck.dwMsgID1 = dwID1;
                pMsgAck.dwMsgID2 = dwID2;
                pMsgAck.wCookie = wCookie;
                pMsgAck.msgType = bMsgType;
                pMsgAck.bFlags = bFlags;
                handleMessageTypes(dwUin, time(NULL), dwID1, dwID2, wCookie, wVersion, bMsgType, bFlags, wAckType, tlv->wLen - 53, wMsgLen, (char*)pDataBuf, 0, &pMsgAck);
                break;
            }
            }
        }
        else if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, PSIG_INFO_PLUGIN))
        {
            // info manager plugin - obsolete
            BYTE bLevel;

            pDataBuf += 16;  /* unused stuff */
            wLen -= 16;
            unpackByte(&pDataBuf, &bMsgType);
            wLen -= 1;

            pDataBuf += 3; // unknown
            wLen -= 3;
            unpackByte(&pDataBuf, &bLevel);
            if (bLevel != 0 || wLen < 16)
            {
                CHECKCONTACT chk = {0};
                chk.hContact=hContact;
                chk.dwUin=dwUin;
                chk.PSD=22;
                CheckContact(chk);
                NetLog_Server("Invalid %s Manager Plugin message from %u", "Info", dwUin);
                return;
            }
            unpackDWord(&pDataBuf, &dwGuid1); // plugin request GUID
            unpackDWord(&pDataBuf, &dwGuid2);
            unpackDWord(&pDataBuf, &dwGuid3);
            unpackDWord(&pDataBuf, &dwGuid4);
            wLen -= 16;

            if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, PMSG_QUERY_INFO))
            {
                NetLog_Server("User %u requests our %s plugin list. NOT SUPPORTED", dwUin, "info");
            }
            else
                NetLog_Server("Unknown %s Manager message from %u", "Info", dwUin);
        }
        else if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, PSIG_STATUS_PLUGIN))
        {
            // status manager plugin - obsolete
            BYTE bLevel;

            pDataBuf += 16;  /* unused stuff */
            wLen -= 16;
            unpackByte(&pDataBuf, &bMsgType);
            wLen -= 1;

            pDataBuf += 3; // unknown
            wLen -= 3;
            unpackByte(&pDataBuf, &bLevel);
            if (bLevel != 0 || wLen < 16)
            {
                CHECKCONTACT chk = {0};
                chk.hContact=hContact;
                chk.dwUin=dwUin;
                chk.PSD=22;
                CheckContact(chk);
                NetLog_Server("Invalid %s Manager Plugin message from %u", "Status", dwUin);
                return;
            }
            unpackDWord(&pDataBuf, &dwGuid1); // plugin request GUID
            unpackDWord(&pDataBuf, &dwGuid2);
            unpackDWord(&pDataBuf, &dwGuid3);
            unpackDWord(&pDataBuf, &dwGuid4);
            wLen -= 16;

            if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, PMSG_QUERY_STATUS))
            {
                NetLog_Server("User %u requests our %s plugin list. NOT SUPPORTED", dwUin, "status");
            }
            else
                NetLog_Server("Unknown %s Manager message from %u", "Status", dwUin);
        }
        else
            NetLog_Server("Unknown signature (%08x-%08x-%08x-%08x) in message (format 2)", dwGuid1, dwGuid2, dwGuid3, dwGuid4);
    }
    else
    {
        NetLog_Server("Unknown wId1 (%u) in message (format 2)", wId);
    }
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        CheckContact(chk);
    }
}



void parseServerGreeting(BYTE* pDataBuf, WORD wLen, WORD wMsgLen, DWORD dwUin, BYTE bFlags, WORD wStatus, WORD wCookie, WORD wAckType, DWORD dwID1, DWORD dwID2, WORD wVersion)
{
    DWORD dwLengthToEnd;
    DWORD dwDataLen;
    int typeId;
    WORD wFunction;

    NetLog_Server("Parsing Greeting message through server");

    pDataBuf += wMsgLen;   // Message
    wLen -= wMsgLen;

    // Message plugin identification
    if (!unpackPluginTypeId(&pDataBuf, &wLen, &typeId, &wFunction, FALSE)) return;

    if (wLen > 8)
    {
        // Length of remaining data
        unpackLEDWord(&pDataBuf, &dwLengthToEnd);

        // Length of message
        unpackLEDWord(&pDataBuf, &dwDataLen);
        wLen -= 8;

        if (dwDataLen > wLen)
            dwDataLen = wLen;

        if (typeId == MTYPE_FILEREQ && wAckType == 2)
        {
            char* szMsg;

            NetLog_Server("This is file ack");
            szMsg = (char *)icq_alloc_zero(dwDataLen + 1);
            memcpy(szMsg, pDataBuf, dwDataLen);
            szMsg[dwDataLen] = '\0';
            pDataBuf += dwDataLen;
            wLen -= (WORD)dwDataLen;

            handleFileAck(pDataBuf, wLen, dwUin, wCookie, wStatus, szMsg);
        }
        else if (typeId == MTYPE_FILEREQ && wAckType == 1)
        {
            char* szMsg;

            NetLog_Server("This is a file request");
            szMsg = (char *)icq_alloc_zero(dwDataLen + 1);
            memcpy(szMsg, pDataBuf, dwDataLen);
            szMsg[dwDataLen] = '\0';
            pDataBuf += dwDataLen;
            wLen -= (WORD)dwDataLen;

            handleFileRequest(pDataBuf, wLen, dwUin, wCookie, dwID1, dwID2, szMsg, 8, FALSE);
        }
        else if (typeId == MTYPE_CHAT && wAckType == 1)
        {
            // TODO: this is deprecated
            char* szMsg;

            NetLog_Server("This is a chat request");
            szMsg = (char *)icq_alloc_zero(dwDataLen + 1);
            memcpy(szMsg, pDataBuf, dwDataLen);
            szMsg[dwDataLen] = '\0';
            pDataBuf += dwDataLen;
            wLen -= (WORD)dwDataLen;

            //    handleChatRequest(pDataBuf, wLen, dwUin, wCookie, dwID1, dwID2, szMsg, 8);
        }
        else if (typeId == MTYPE_STATUSMSGEXT && wFunction >= 1 && wFunction <= 5)
        {
            // handle ICQ6 status message request
//      handleMessageTypes(dwUin, time(NULL), dwID1, dwID2, wCookie, wVersion, 0xE7 + wFunction, bFlags, wAckType, dwLengthToEnd, 0, pDataBuf, MTF_PLUGIN, NULL);
            int nMsgType = 0;
            switch (wFunction)
            {
            case 1: // Away
                if (gnCurrentStatus == ID_STATUS_ONLINE || gnCurrentStatus == ID_STATUS_INVISIBLE)
                    nMsgType = MTYPE_AUTOONLINE;
                else if (gnCurrentStatus == ID_STATUS_AWAY)
                    nMsgType = MTYPE_AUTOAWAY;
                else if (gnCurrentStatus == ID_STATUS_FREECHAT)
                    nMsgType = MTYPE_AUTOFFC;
                break;
            case 2: // Busy
                if (gnCurrentStatus == ID_STATUS_OCCUPIED)
                    nMsgType = MTYPE_AUTOBUSY;
                else if (gnCurrentStatus == ID_STATUS_DND)
                    nMsgType = MTYPE_AUTODND;
                break;
            case 3: // N/A
                if (gnCurrentStatus == ID_STATUS_NA)
                    nMsgType = MTYPE_AUTONA;
            }
            handleMessageTypes(dwUin, time(NULL), dwID1, dwID2, wCookie, wVersion, nMsgType, bFlags, wAckType, dwLengthToEnd, 0, (char*)pDataBuf, MTF_PLUGIN, NULL);
        }
        else if (typeId)
        {
            message_ack_params pMsgAck = {0};

            pMsgAck.bType = MAT_SERVER_ADVANCED;
            pMsgAck.dwUin = dwUin;
            pMsgAck.dwMsgID1 = dwID1;
            pMsgAck.dwMsgID2 = dwID2;
            pMsgAck.wCookie = wCookie;
            pMsgAck.msgType = typeId;
            pMsgAck.bFlags = bFlags;
            handleMessageTypes(dwUin, time(NULL), dwID1, dwID2, wCookie, wVersion, typeId, bFlags, wAckType, dwLengthToEnd, (WORD)dwDataLen, (char*)pDataBuf, MTF_PLUGIN, &pMsgAck);
        }
        else
        {
            NetLog_Server("Unsupported plugin message type %d", typeId);
        }
    }
}



static void handleRecvServMsgContacts(unsigned char *buf, WORD wLen, DWORD dwUin, char *szUID, DWORD dwID1, DWORD dwID2, WORD wCommand)
{
    HANDLE hContact = HContactFromUID(dwUin, szUID, NULL);

    if (wCommand == 0)
    {
        // received contacts
        oscar_tlv_chain *chain;
        WORD wAckType;

        if (wLen < 4)
        {
            // just check if at least one tlv is there
            NetLog_Server("Message (format %u) - Ignoring empty contacts message", 2);
            return;
        }
        chain = readIntoTLVChain(&buf, wLen, 0);

        wAckType = getWordFromChain(chain, 0x0A, 1);

        if (wAckType == 1)
        {
            // it is really message containing contacts, parse them
            oscar_tlv *tlvUins, *tlvNames;
            ICQSEARCHRESULT **contacts = NULL;
            int nContacts = 0x10, iContact = 0;
            WORD wContactsGroup = 0;
            int i, valid;
            char* pBuffer;
            int nLen;

            tlvUins = getTLV(chain, 0x2711, 1);
            tlvNames = getTLV(chain, 0x2712, 1);

            if (!tlvUins || tlvUins->wLen < 4)
            {
                NetLog_Server("Malformed '%s' message", "contacts");
                disposeChain(&chain);
                return;
            }
            valid = 1;
            contacts = (ICQSEARCHRESULT**)icq_alloc_zero(nContacts * sizeof(ICQSEARCHRESULT*));
            pBuffer = (char*)tlvUins->pData;
            nLen = tlvUins->wLen;

            while (nLen > 2)
            {
                // parse UIDs
                if (!wContactsGroup)
                {
                    WORD wGroupLen;

                    unpackWord((BYTE**)&pBuffer, &wGroupLen);
                    nLen -= 2;
                    if (nLen >= wGroupLen + 2)
                    {
                        pBuffer += wGroupLen;
                        unpackWord((BYTE**)&pBuffer, &wContactsGroup);
                        nLen -= wGroupLen + 2;
                    }
                    else
                        break;
                }
                else
                {
                    // group parsed, UIDs waiting
                    WORD wUidLen;

                    unpackWord((BYTE**)&pBuffer, &wUidLen);
                    nLen -= 2;
                    if (nLen >= wUidLen)
                    {
                        if (iContact >= nContacts)
                        {
                            // the list is too small, resize it
                            nContacts += 0x10;
                            contacts = (ICQSEARCHRESULT**)mir_realloc(contacts, nContacts * sizeof(ICQSEARCHRESULT*));
                        }
                        contacts[iContact] = (ICQSEARCHRESULT*)icq_alloc_zero(sizeof(ICQSEARCHRESULT));
                        contacts[iContact]->hdr.cbSize = sizeof(ICQSEARCHRESULT);
                        contacts[iContact]->hdr.nick = null_strdup("");
                        contacts[iContact]->uid = (char*)icq_alloc_zero(wUidLen + 1);
                        unpackString((BYTE**)&pBuffer, contacts[iContact]->uid, wUidLen);
                        nLen -= wUidLen;

                        if (IsStringUIN(contacts[iContact]->uid))
                        {
                            // icq contact
                            contacts[iContact]->uin = atoi(contacts[iContact]->uid);
                            if (contacts[iContact]->uin == 0)
                                valid = 0;

                            mir_free(contacts[iContact]->uid);
                        }
                        else
                        {
                            // aim contact
                            if (!strlennull(contacts[iContact]->uid))
                                valid = 0;
                        }
                        iContact++;
                    }
                    else
                    {
                        if (wContactsGroup) valid = 0;
                        break;
                    }

                    wContactsGroup--;
                }
            }
            if (!iContact || !valid)
            {
                NetLog_Server("Malformed '%s' message", "contacts");
                disposeChain(&chain);
                for (i = 0; i < iContact; i++)
                {
                    mir_free(contacts[i]->uid);
                    mir_free(contacts[i]->hdr.nick);
                    mir_free(contacts[i]);
                }
                mir_free(&contacts);
                return;
            }
            nContacts = iContact;
            if (tlvNames && tlvNames->wLen >= 4)
            {
                // parse names, if available
                pBuffer = (char*)tlvNames->pData;
                nLen = tlvNames->wLen;
                iContact = 0;

                while (nLen > 2)
                {
                    // parse Names
                    if (!wContactsGroup)
                    {
                        WORD wGroupLen;

                        unpackWord((BYTE**)&pBuffer, &wGroupLen);
                        nLen -= 2;
                        if (nLen >= wGroupLen + 2)
                        {
                            pBuffer += wGroupLen;
                            unpackWord((BYTE**)&pBuffer, &wContactsGroup);
                            nLen -= wGroupLen + 2;
                        }
                        else
                            break;
                    }
                    else
                    {
                        // group parsed, Names waiting
                        WORD wNickLen;

                        unpackWord((BYTE**)&pBuffer, &wNickLen);
                        nLen -= 2;
                        if (nLen >= wNickLen)
                        {
                            WORD wNickTLV, wNickTLVLen;
                            char* pNick = NULL;

                            unpackTypedTLV((BYTE*)pBuffer, wNickLen, 0x01, &wNickTLV, &wNickTLVLen, &pNick);
                            if (wNickTLV == 0x01)
                            {
                                mir_free(contacts[iContact]->hdr.nick);
                                contacts[iContact]->hdr.nick = pNick;
                            }
                            else
                                mir_free(pNick);
                            pBuffer += wNickLen;
                            nLen -= wNickLen;

                            iContact++;
                            if (iContact >= nContacts) break;
                        }
                        else
                            break;

                        wContactsGroup--;
                    }
                }
            }

            if (!valid)
            {
                NetLog_Server("Malformed '%s' message", "contacts");
            }
            else
            {
                int bAdded;
                CCSDATA ccs;
                PROTORECVEVENT pre = {0};

                hContact = HContactFromUID(dwUin, szUID, &bAdded);

                // ack the message
                icq_sendContactsAck(dwUin, szUID, dwID1, dwID2);

                ccs.szProtoService = PSR_CONTACTS;
                ccs.hContact = hContact;
                ccs.wParam = 0;
                ccs.lParam = (LPARAM)&pre;
                pre.timestamp = (DWORD)time(NULL);
                pre.szMessage = (char *)contacts;
                pre.lParam = nContacts;
                pre.flags = PREF_UTF;
                CallService(MS_PROTO_CHAINRECV, 0, (LPARAM)&ccs);
            }

            for (i = 0; i < iContact; i++)
            {
                mir_free(contacts[i]->uid);
                mir_free(contacts[i]->hdr.nick);
                mir_free(contacts[i]);
            }
            mir_free(&contacts);
        }
        else
            NetLog_Server("Error: Received unknown contacts message, ignoring.");
        // Clean up
        disposeChain(&chain);
    }
    else if (wCommand == 1)
    {
        NetLog_Server("Cannot handle abort messages yet... :(");
        return;
    }
    else if (wCommand == 2)
    {
        // acknowledgement
        DWORD dwCookie;
        HANDLE hCookieContact;

        if (FindMessageCookie(dwID1, dwID2, &dwCookie, &hCookieContact, NULL))
        {
            if (hCookieContact != hContact)
                NetLog_Server("Warning: Ack Contact does not match Cookie Contact(0x%x != 0x%x)", hContact, hCookieContact);

            ICQBroadcastAck(hContact, ACKTYPE_CONTACTS, ACKRESULT_SUCCESS, (HANDLE)dwCookie, 0);

            ReleaseCookie(dwCookie);
        }
        else
            NetLog_Server("Warning: Unexpected Contact Transfer ack from %s", strUID(dwUin, szUID));
    }
}



static void handleRecvServMsgType4(unsigned char *buf, WORD wLen, DWORD dwUin, char *szUID, DWORD dwMsgID1, DWORD dwMsgID2, DWORD dwRef)
{
    WORD wTLVType;
    WORD wTLVLen;
    BYTE* pDataBuf;
    DWORD dwUin2;
    HANDLE hContact;

    hContact = HContactFromUIN(dwUin, NULL);


    if (wLen < 2)
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        chk.PSD=24;
        CheckContact(chk);
        NetLog_Server("Message (format %u) - Ignoring empty message", 4);
        return;
    }

    // Unpack the first TLV(5)
    unpackTypedTLV(buf, wLen, 5, &wTLVType, &wTLVLen, (char**)&pDataBuf);
    NetLog_Server("Message (format %u) - UID: %s", 4, strUID(dwUin, szUID));

    // It must be TLV(5)
    if (wTLVType == 5)
    {
        BYTE bMsgType;
        BYTE bFlags;
        BYTE* pmsg = pDataBuf;
        WORD wMsgLen;

        unpackLEDWord(&pmsg, &dwUin2);

        if (dwUin2 == dwUin)
        {
            unpackByte(&pmsg, &bMsgType);
            unpackByte(&pmsg, &bFlags);
            unpackLEWord(&pmsg, &wMsgLen);

            if (bMsgType == 0 && wMsgLen == 1)
            {
                CHECKCONTACT chk = {0};
                chk.hContact=hContact;
                chk.dwUin=dwUin;
                chk.historyevent=chk.logtofile=chk.popup=TRUE;
                chk.msg="has checked your ignore list";
                chk.icqeventtype=ICQEVENTTYPE_IGNORECHECK_STATUS;
                chk.popuptype=POPTYPE_IGNORE_CHECK;
                chk.dbeventflag=DBEF_READ;
                CheckContact(chk);
                NetLog_Server("User %u probably checks his ignore state.", dwUin);
            }
            else
            {
                offline_message_cookie *cookie;
                DWORD dwRecvTime = (DWORD)time(NULL);

                if (!(dwRef & 0x80000000) && FindCookie(dwRef, NULL, (void**)&cookie))
                {
                    WORD wTimeTLVType, wTimeTLVLen;
                    BYTE *pTimeTLV;

                    cookie->nMessages++;

                    unpackTypedTLV(buf, wLen, 0x16, &wTimeTLVType, &wTimeTLVLen, (char**)&pTimeTLV);
                    if (pTimeTLV && wTimeTLVType == 0x16 && wTimeTLVLen == 4)
                    {
                        // found Offline timestamp
                        BYTE *pBuf = pTimeTLV;
                        unpackDWord(&pBuf, &dwRecvTime);
                        NetLog_Server("Message (format %u) - Offline timestamp is %s", 4, time2text(dwRecvTime));
                    }
                    mir_free(pTimeTLV);
                }

                if (bMsgType == MTYPE_PLUGIN)
                {
                    WORD wLen = wTLVLen - 8;
                    int typeId;

                    NetLog_Server("Parsing Greeting message through server");

                    pmsg += wMsgLen;
                    wLen -= wMsgLen;

                    if (unpackPluginTypeId(&pmsg, &wLen, &typeId, NULL, FALSE) && wLen > 8)
                    {
                        DWORD dwLengthToEnd;
                        DWORD dwDataLen;

                        // Length of remaining data
                        unpackLEDWord(&pmsg, &dwLengthToEnd);

                        // Length of message
                        unpackLEDWord(&pmsg, &dwDataLen);
                        wLen -= 8;

                        if (dwDataLen > wLen)
                            dwDataLen = wLen;

                        if (typeId)
                            handleMessageTypes(dwUin, dwRecvTime, dwMsgID1, dwMsgID2, 0, 0, typeId, bFlags, 0, dwLengthToEnd, (WORD)dwDataLen, (char*)pmsg, MTF_PLUGIN, NULL);
                        else
                        {
                            NetLog_Server("Unsupported plugin message type %d", typeId);
                        }
                    }
                }
                else
                    handleMessageTypes(dwUin, dwRecvTime, dwMsgID1, dwMsgID2, 0, 0, bMsgType, bFlags, 0, wTLVLen - 8, wMsgLen, (char*)pmsg, 0, NULL);
            }
        }
        else
        {
            NetLog_Server("Ignoring spoofed TYPE4 message thru server from %d", dwUin);
        }
    }
    else
    {
        NetLog_Server("Unsupported TLV (%u) in message (format %u)", wTLVType, 4);
    }
    mir_free(pDataBuf);
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        if(OfflineMessagesReceived)
            chk.PSD=22;
        else
            chk.PSD=-1;
        CheckContact(chk);
    }
}



//
// Helper functions
//

static int TypeGUIDToTypeId(DWORD dwGuid1, DWORD dwGuid2, DWORD dwGuid3, DWORD dwGuid4, WORD wType)
{
    int nTypeID = MTYPE_UNKNOWN;

    if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, MGTYPE_STATUSMSGEXT))
    {
        nTypeID = MTYPE_STATUSMSGEXT;
    }
    else if (wType==MGTYPE_UNDEFINED)
    {
        if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, PSIG_MESSAGE))
        {
            // icq6 message ack
            nTypeID = MTYPE_PLAIN;
        }
    }
    else if (wType==MGTYPE_STANDARD_SEND)
    {
        if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, MGTYPE_WEBURL))
        {
            nTypeID = MTYPE_URL;
        }
        else if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, MGTYPE_CONTACTS))
        {
            nTypeID = MTYPE_CONTACTS;
        }
        else if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, MGTYPE_CHAT))
        {
            nTypeID = MTYPE_CHAT;
        }
        else if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, MGTYPE_FILE))
        {
            nTypeID = MTYPE_FILEREQ;
        }
        else if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, MGTYPE_GREETING_CARD))
        {
            nTypeID = MTYPE_GREETINGCARD;
        }
        else if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, MGTYPE_MESSAGE))
        {
            nTypeID = MTYPE_MESSAGE;
        }
        else if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, MGTYPE_SMS_MESSAGE))
        {
            nTypeID = MTYPE_SMS_MESSAGE;
        }
        else if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, MGTYPE_TZER_MESSAGE))
        {
            nTypeID = MTYPE_TZER_MESSAGE;
        }
    }
    else if (wType==MGTYPE_CONTACTS_REQUEST)
    {
        if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, MGTYPE_CONTACTS))
        {
            nTypeID = MTYPE_REQUESTCONTACTS;
        }
        else if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, MGTYPE_XTRAZ_SCRIPT))
        {
            nTypeID = MTYPE_SCRIPT_DATA;
        }
    }
    else if (CompareGUIDs(dwGuid1, dwGuid2, dwGuid3, dwGuid4, MGTYPE_XTRAZ_SCRIPT))
    {
        if (wType==MGTYPE_SCRIPT_INVITATION)
        {
            nTypeID = MTYPE_SCRIPT_INVITATION;
        }
        else if (wType==MGTYPE_SCRIPT_NOTIFY)
        {
            nTypeID = MTYPE_SCRIPT_NOTIFY;
        }
    }

    return nTypeID;
}



int unpackPluginTypeId(BYTE** pBuffer, WORD* pwLen, int *pTypeId, WORD *pFunctionId, BOOL bThruDC)
{
    WORD wLen = *pwLen;
    WORD wInfoLen;
    DWORD dwPluginNameLen;
    DWORD q1,q2,q3,q4;
    WORD qt;
    char* szPluginName;
    int typeId;

    if (wLen < 24)
        return 0; // Failure

    unpackLEWord(pBuffer, &wInfoLen);

    unpackDWord(pBuffer, &q1); // get data GUID & function id
    unpackDWord(pBuffer, &q2);
    unpackDWord(pBuffer, &q3);
    unpackDWord(pBuffer, &q4);
    unpackLEWord(pBuffer, &qt);
    wLen -= 20;

    if (pFunctionId) *pFunctionId = qt;

    unpackLEDWord(pBuffer, &dwPluginNameLen);
    wLen -= 4;

    if (dwPluginNameLen > wLen)
    {
        // check for malformed plugin name
        dwPluginNameLen = wLen;
        NetLog_Uni(bThruDC, "Warning: malformed size of plugin name.");
    }
    szPluginName = (char *)icq_alloc_zero(dwPluginNameLen + 1);
    memcpy(szPluginName, *pBuffer, dwPluginNameLen);
    szPluginName[dwPluginNameLen] = '\0';
    wLen -= (WORD)dwPluginNameLen;

    *pBuffer += dwPluginNameLen;

    typeId = TypeGUIDToTypeId(q1, q2, q3, q4, qt);
    if (!typeId)
        NetLog_Uni(bThruDC, "Error: Unknown type {%08x-%08x-%08x-%08x:%04x}: %s", q1,q2,q3,q4,qt, szPluginName);

    if (wInfoLen >= 22 + dwPluginNameLen)
    {
        // sanity checking
        wInfoLen -= (WORD)(22 + dwPluginNameLen);

        // check if enough data is available - skip remaining bytes of info block
        if (wLen >= wInfoLen)
        {
            *pBuffer += wInfoLen;
            wLen -= wInfoLen;
        }
    }

    *pwLen = wLen;
    *pTypeId = typeId;

    return 1; // Success
}



int getPluginTypeIdLen(int nTypeID)
{
    switch (nTypeID)
    {
    case MTYPE_SCRIPT_NOTIFY:
        return 0x51;

    case MTYPE_FILEREQ:
        return 0x2B;

    case MTYPE_TZER_MESSAGE:
        return 0x34;

    case MTYPE_AUTOONLINE:
    case MTYPE_AUTOAWAY:
    case MTYPE_AUTOBUSY:
    case MTYPE_AUTODND:
    case MTYPE_AUTOFFC:
        return 0x3C;
    case MTYPE_AUTONA:
        return 0x3B;



    default:
        return 0;
    }
}



void packPluginTypeId(icq_packet *packet, int nTypeID)
{
    switch (nTypeID)
    {
    case MTYPE_SCRIPT_NOTIFY:
        packLEWord(packet, 0x04f);                // Length

        packGUID(packet, MGTYPE_XTRAZ_SCRIPT);    // Message Type GUID
        packLEWord(packet, MGTYPE_SCRIPT_NOTIFY); // Function ID
        packLEDWord(packet, 0x002a);              // Request type string
        packBuffer(packet, "Script Plug-in: Remote Notification Arrive", 0x002a);

        packDWord(packet, 0x00000100);            // Unknown binary stuff
        packDWord(packet, 0x00000000);
        packDWord(packet, 0x00000000);
        packWord(packet, 0x0000);
        packByte(packet, 0x00);

        break;

    case MTYPE_FILEREQ:
        packLEWord(packet, 0x029);     // Length

        packGUID(packet, MGTYPE_FILE); // Message Type GUID
        packWord(packet, 0x0000);      // Unknown
        packLEDWord(packet, 0x0004);   // Request type string
        packBuffer(packet, "File", 0x0004);

        packDWord(packet, 0x00000100); // More unknown binary stuff
        packDWord(packet, 0x00010000);
        packDWord(packet, 0x00000000);
        packWord(packet, 0x0000);
        packByte(packet, 0x00);

        break;

    case MTYPE_TZER_MESSAGE:
        packLEWord(packet, 0x32);
        packGUID(packet, MGTYPE_TZER_MESSAGE );
        packWord(packet, 0x0000);
        packLEDWord(packet, 0x000D);
        packBuffer(packet, "T-Zer Message", 0x000D);
        packDWord(packet, 0x00000000);
        packDWord(packet, 0x00000000);
        packDWord(packet, 0x00000000);
        packWord(packet, 0x0000);
        packByte(packet, 0x00);

        break;

    case MTYPE_AUTOONLINE:
    case MTYPE_AUTOAWAY:
    case MTYPE_AUTOFFC:
        packLEWord(packet, 0x03A);                // Length

        packGUID(packet, MGTYPE_STATUSMSGEXT);    // Message Type GUID
        packLEWord(packet, 1);                    // Function ID
        packLEDWord(packet, 0x13);                // Request type string
        packBuffer(packet, "Away Status Message", 0x13);

        packDWord(packet, 0x01000000);            // Unknown binary stuff
        packDWord(packet, 0x00000000);
        packDWord(packet, 0x00000000);
        packDWord(packet, 0x00000000);
        packByte(packet, 0x00);

        break;
    case MTYPE_AUTOBUSY:
    case MTYPE_AUTODND:
        packLEWord(packet, 0x03A);                // Length

        packGUID(packet, MGTYPE_STATUSMSGEXT);    // Message Type GUID

        packLEWord(packet, 2);                    // Function ID
        packLEDWord(packet, 0x13);                // Request type string
        packBuffer(packet, "Busy Status Message", 0x13);

        packDWord(packet, 0x02000000);            // Unknown binary stuff
        packDWord(packet, 0x00000000);
        packDWord(packet, 0x00000000);
        packDWord(packet, 0x00000000);
        packByte(packet, 0x00);

        break;

    case MTYPE_AUTONA:
        packLEWord(packet, 0x039);                // Length

        packGUID(packet, MGTYPE_STATUSMSGEXT);    // Message Type GUID

        packLEWord(packet, 3);                    // Function ID
        packLEDWord(packet, 0x12);                // Request type string
        packBuffer(packet, "N/A Status Message", 0x12);

        packDWord(packet, 0x03000000);            // Unknown binary stuff
        packDWord(packet, 0x00000000);
        packDWord(packet, 0x00000000);
        packDWord(packet, 0x00000000);
        packByte(packet, 0x00);

        break;

    default:
        return;
    }
}



void handleStatusMsgReply(const char* szPrefix, HANDLE hContact, DWORD dwUin, WORD wVersion, int bMsgType, WORD wCookie, const char* szMsg, int nMsgFlags)
{
    CCSDATA ccs;
    PROTORECVEVENT pre = {0};
    int status;
    char* pszMsg;


    if (hContact == INVALID_HANDLE_VALUE)
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        CheckContact(chk);
        NetLog_Server("%sIgnoring status message from unknown contact %u", szPrefix, dwUin);
        return;
    }

    status = AwayMsgTypeToStatus(bMsgType);
    if (status == ID_STATUS_OFFLINE)
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        CheckContact(chk);
        NetLog_Server("%sIgnoring unknown status message from %u", szPrefix, dwUin);
        return;
    }

    pszMsg = null_strdup((char*)szMsg);

    if (wVersion == 9|| (nMsgFlags & MTF_PLUGIN) && wVersion == 10)
    {
        // it is probably UTF-8 status reply
        pszMsg = detect_decode_utf8(pszMsg);
    }

    ccs.szProtoService = PSR_AWAYMSG;
    ccs.hContact = hContact;
    ccs.wParam = status;
    ccs.lParam = (LPARAM)&pre;
    pre.szMessage = pszMsg;
    pre.timestamp = time(NULL);
    pre.lParam = wCookie;

    CallService(MS_PROTO_CHAINRECV,0,(LPARAM)&ccs);

    mir_free(pszMsg);
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        CheckContact(chk);
    }
}



static HANDLE handleMessageAck(DWORD dwUin, WORD wCookie, WORD wVersion, int type, WORD wMsgLen, PBYTE buf, BYTE bFlags, int nMsgFlags)
{
    if (bFlags == 3)
    {
        HANDLE hContact;
        HANDLE hCookieContact;
        message_cookie_data* pCookieData = NULL;

        hContact = HContactFromUIN(dwUin, NULL);


        // Added by BM, modifed by icqj plus team ))
        {
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            chk.PSD=2;
            CheckContact(chk);
        }

        if (!FindCookie(wCookie, &hCookieContact, (void**)&pCookieData))
        {
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            CheckContact(chk);
            NetLog_Server("%sIgnoring unrequested status message from %u", "handleMessageAck: ", dwUin);

            ReleaseCookie(wCookie);
            return INVALID_HANDLE_VALUE;
        }

        if (hContact != hCookieContact)
        {
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            CheckContact(chk);
            NetLog_Server("%sAck Contact does not match Cookie Contact(0x%x != 0x%x)", "handleMessageAck: ", hContact, hCookieContact);

            ReleaseCookie(wCookie);
            return INVALID_HANDLE_VALUE;
        }
        ReleaseCookie(wCookie);

        handleStatusMsgReply("handleMessageAck: ", hContact, dwUin, wVersion, type, wCookie, (char*)buf, nMsgFlags);
        {
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            CheckContact(chk);
        }
    }
    else
    {
        // Should not happen
        NetLog_Server("%sIgnored type %u ack message (this should not happen)", "handleMessageAck: ", type);
    }

    return INVALID_HANDLE_VALUE;
}



/* this function send all acks from handleMessageTypes */
static void sendMessageTypesAck(HANDLE hContact, int bUnicode, message_ack_params *pArgs)
{
    if (pArgs)
    {
        if ((pArgs->msgType == MTYPE_PLAIN && !CallService(MS_IGNORE_ISIGNORED, (WPARAM)hContact, IGNOREEVENT_MESSAGE))
                || (pArgs->msgType == MTYPE_URL && !CallService(MS_IGNORE_ISIGNORED, (WPARAM)hContact, IGNOREEVENT_URL))
                || pArgs->msgType == MTYPE_CONTACTS)
        {
            if (pArgs->bType == MAT_SERVER_ADVANCED)
            {
                // Only ack message packets
                icq_sendAdvancedMsgAck(pArgs->dwUin, pArgs->dwMsgID1, pArgs->dwMsgID2, pArgs->wCookie, (BYTE)pArgs->msgType, pArgs->bFlags);
            }
            else if (pArgs->bType == MAT_DIRECT)
            {
                // Send acknowledgement
                icq_sendDirectMsgAck(pArgs->pDC, pArgs->wCookie, (BYTE)pArgs->msgType, pArgs->bFlags, bUnicode ? CAP_UTF8MSGS : NULL);
            }
        }
    }
}



/* this function also processes direct packets, so it should be bulletproof */
/* pMsg points to the beginning of the message */
BOOL handleMessageTypes(DWORD dwUin, DWORD dwTimestamp, DWORD dwMsgID, DWORD dwMsgID2, WORD wCookie, WORD wVersion, int type, int flags, WORD wAckType, DWORD dwDataLen, WORD wMsgLen, char *pMsg, int nMsgFlags, message_ack_params *pAckParams)
{
    BOOL res = TRUE;
    char* szMsg;
    char* pszMsgField[2*MAX_CONTACTSSEND+1];
    char* pszMsg;
    int nMsgFields;
    HANDLE hContact = INVALID_HANDLE_VALUE;
    BOOL bThruDC = (nMsgFlags & MTF_DIRECT) == MTF_DIRECT;
    int bAdded;

    BOOL bRdName = FALSE;
    DWORD dwNameLen = 0;
    char * szName = 0;
    char * szName2 = 0;
    BOOL bRdUrl = FALSE;
    DWORD dwUrlLen = 0;
    char * szUrl = 0;
    char * szUrl2 = 0;
    char * szStr = 0;
    int i = 0;
    CCSDATA ccs;
    PROTORECVEVENT pre;




    if (dwDataLen < wMsgLen)
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        CheckContact(chk);
        NetLog_Uni(bThruDC, "Ignoring overflowed message");
        res = FALSE;
        return res;
    }

    if (wAckType == 2)
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        CheckContact(chk);
        handleMessageAck(dwUin, wCookie, wVersion, type, wMsgLen, (BYTE*)pMsg, (BYTE)flags, nMsgFlags);
        res = FALSE;
        return res;
    }

    szMsg = (char *)icq_alloc_zero(wMsgLen + 1);
    if (wMsgLen > 0)
    {
        memcpy(szMsg, pMsg, wMsgLen);
        pMsg += wMsgLen;
        dwDataLen -= wMsgLen;
    }
    szMsg[wMsgLen] = '\0';


    pszMsgField[0] = szMsg;
    nMsgFields = 0;
    if (type == MTYPE_URL || type == MTYPE_AUTHREQ || type == MTYPE_ADDED || type == MTYPE_CONTACTS || type == MTYPE_EEXPRESS || type == MTYPE_WWP)
    {
        for (pszMsg=szMsg, nMsgFields=1; *pszMsg; pszMsg++)
        {
            if ((unsigned char)*pszMsg == 0xFE)
            {
                *pszMsg = '\0';
                pszMsgField[nMsgFields++] = pszMsg + 1;
                if (nMsgFields >= SIZEOF(pszMsgField))
                    break;
            }
        }
    }


    switch (type)
    {
        //  hContact = HContactFromUIN(dwUin, &bAdded);
    case MTYPE_TZER_MESSAGE://szMsg dwDataLen/ need parsing
        for (i = 0; i < wMsgLen; i++)
        {
            if (bRdName == TRUE)
            {
                if (szMsg[i] == '"')
                {
                    if (szName)
                    {
                        szName = (char*)realloc(szName, dwNameLen + 1);
                        szName[dwNameLen] = 0;
                    }

                    bRdName = FALSE;
                }
                else
                {
                    dwNameLen++;
                    if (dwNameLen == 1)
                    {
                        szName = (char *) malloc(dwNameLen);
                    }
                    else
                    {
                        szName = (char*)realloc(szName, dwNameLen);
                    }
                    szName[dwNameLen - 1] = szMsg[i];
                }
            }

            if (bRdUrl == TRUE)
            {
                if (szMsg[i] == '"')
                {
                    if (szUrl)
                    {
                        szUrl = (char*)realloc(szUrl, dwUrlLen + 1);
                        szUrl[dwUrlLen] = 0;
                    }

                    bRdUrl = FALSE;
                }
                else
                {
                    dwUrlLen++;
                    if (dwUrlLen == 1)
                    {
                        szUrl = (char *) malloc(dwUrlLen);
                    }
                    else
                    {
                        szUrl = (char*)realloc(szUrl, dwUrlLen);
                    }
                    szUrl[dwUrlLen - 1] = szMsg[i];
                }
            }


            if (i > 5)
            {
                if (szMsg[i] == '"' && szMsg[i - 1] == '=' && szMsg[i - 2] == 'e' && szMsg[i - 3] == 'm' && szMsg[i - 4] == 'a' && szMsg[i - 5] == 'n')
                {
                    bRdName = TRUE;
                }
            }

            if (i > 4)
            {
                if (szMsg[i] == '"' && szMsg[i - 1] == '=' && szMsg[i - 2] == 'l' && szMsg[i - 3] == 'r' && szMsg[i - 4] == 'u')
                {
                    bRdUrl = TRUE;
                }
            }
        }

        szName2 = DemangleXml(szName, dwNameLen);
        free(szName);

        szUrl2 = DemangleXml(szUrl, dwUrlLen);
        free(szUrl);

        szStr = (char *) malloc(17 + dwNameLen + dwUrlLen + 1);
        sprintf(szStr, "tZer received: %s\r\n%s", szName2, szUrl2);


        ccs.szProtoService = PSR_MESSAGE;
        ccs.hContact = HContactFromUIN(dwUin, &bAdded);
        ccs.wParam = 0;
        ccs.lParam = (LPARAM)&pre;
        pre.flags = 0;
        pre.timestamp = time(NULL);
        pre.szMessage = szStr;
        pre.lParam = 0;

        CallService(MS_PROTO_CHAINRECV, 0, (LPARAM)&ccs);

        free(szName2);
        free(szUrl2);
        free(szStr);
        break;

    case MTYPE_PLAIN:    /* plain message */
    {
        CCSDATA ccs;
        PROTORECVEVENT pre = {0};

        // Check if this message is marked as UTF8 encoded
        if (dwDataLen > 12)
        {
            DWORD dwGuidLen = 0;
            int bDoubleMsg = 0;

            if (bThruDC)
            {
                DWORD dwExtraLen = *(DWORD*)pMsg;

                if (dwExtraLen < dwDataLen && !strncmp(szMsg, "{\\rtf", 5))
                {
                    // it is icq5 sending us crap, get real message from it
                    WCHAR* usMsg = (WCHAR*)icq_alloc_zero((dwExtraLen + 1)*sizeof(WCHAR));
                    // make sure it is null-terminated
                    wcsncpy(usMsg, (WCHAR*)(pMsg + 4), dwExtraLen);
                    usMsg[dwExtraLen] = '\0';
                    mir_free(szMsg);
                    szMsg = mir_utf8encodeW(usMsg);

                    if (!IsUnicodeAscii(usMsg, dwExtraLen))
                        pre.flags = PREF_UTF; // only mark real non-ascii messages as unicode

                    bDoubleMsg = 1;
                }
            }

            if (!bDoubleMsg)
            {
                dwGuidLen = *(DWORD*)(pMsg+8);
                dwDataLen -= 12;
                pMsg += 12;
            }

            while ((dwGuidLen >= 38) && (dwDataLen >= dwGuidLen))
            {
                if (!strncmp(pMsg, CAP_UTF8MSGS, 38))
                {
                    // Found UTF8 cap, convert message to ansi
                    pre.flags = PREF_UTF;
                    break;
                }
                else if (!strncmp(pMsg, CAP_RTFMSGS, 38))
                {
                    // Found RichText cap
                    NetLog_Uni(bThruDC, "Warning: User %u sends us RichText.", dwUin);
                    break;
                }

                dwGuidLen -= 38;
                dwDataLen -= 38;
                pMsg += 38;
            }
        }

        hContact = HContactFromUIN(dwUin, &bAdded);
        sendMessageTypesAck(hContact, pre.flags & PREF_UTF, pAckParams);

        if (!pre.flags && !IsUSASCII(szMsg, strlennull(szMsg)))
        {
            // message is Ansi and contains national characters, create Unicode part by codepage
            char* usMsg = convertMsgToUserSpecificUtf(hContact, szMsg);

            if (usMsg)
            {
                mir_free(szMsg);
                szMsg = usMsg;
                pre.flags = PREF_UTF;
            }
        }

        ccs.szProtoService = PSR_MESSAGE;
        ccs.hContact = hContact;
        ccs.wParam = 0;
        ccs.lParam = (LPARAM)&pre;
        pre.timestamp = dwTimestamp;
        pre.szMessage = (char *)szMsg;

        CallService(MS_PROTO_CHAINRECV, 0, (LPARAM)&ccs);
    }
    break;

    case MTYPE_URL:
    {
        CCSDATA ccs;
        PROTORECVEVENT pre = {0};
        char *szBlob, *szTitle, *szDataDescr, *szDataUrl;


        if (nMsgFields < 2)
        {
            HANDLE hContact = HContactFromUIN(dwUin, NULL);
            {
                CHECKCONTACT chk = {0};
                chk.hContact=hContact;
                chk.dwUin=dwUin;
                CheckContact(chk);
            }
            NetLog_Uni(bThruDC, "Malformed '%s' message (maybe checking for invisible status...)", "URL");
            res = FALSE;
            break;
        }

        hContact = HContactFromUIN(dwUin, &bAdded);
        sendMessageTypesAck(hContact, 0, pAckParams);

        szTitle = ICQTranslateUtf("Incoming URL:");
        szDataDescr = mir_utf8encode(pszMsgField[0]);
        szDataUrl = mir_utf8encode(pszMsgField[1]);
        szBlob = (char *)icq_alloc_zero(strlennull(szTitle) + strlennull(szDataDescr) + strlennull(szDataUrl) + 8);
        strcpy(szBlob, szTitle);
        strcat(szBlob, " ");
        strcat(szBlob, szDataDescr); // Description
        strcat(szBlob, "\r\n");
        strcat(szBlob, szDataUrl); // URL
        mir_free(szTitle);
        mir_free(szDataDescr);
        mir_free(szDataUrl);

        ccs.szProtoService = PSR_MESSAGE;
        ccs.hContact = hContact;
        ccs.wParam = 0;
        ccs.lParam = (LPARAM)&pre;
        pre.timestamp = dwTimestamp;
        pre.szMessage = (char *)szBlob;
        pre.flags = PREF_UTF;

        CallService(MS_PROTO_CHAINRECV, 0, (LPARAM)&ccs);

        mir_free(szBlob);
    }
    break;

    case MTYPE_AUTHREQ:       /* auth request */
        /* format: nick FE first FE last FE email FE unk-char FE msg 00 */
    {
        CCSDATA ccs;
        PROTORECVEVENT pre = {0};
        char* szBlob;
        char* pCurBlob;


        if (nMsgFields < 6)
        {
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            chk.PSD=24;
            CheckContact(chk);
            NetLog_Server("Malformed '%s' message", "auth req");
            res = FALSE;
            break;
        }

        ccs.szProtoService=PSR_AUTH;
        ccs.hContact=hContact=HContactFromUIN(dwUin, &bAdded);
        ccs.wParam=0;
        ccs.lParam=(LPARAM)&pre;
        pre.timestamp=dwTimestamp;
        pre.lParam=sizeof(DWORD)+sizeof(HANDLE)+strlennull(pszMsgField[0])+strlennull(pszMsgField[1])+strlennull(pszMsgField[2])+strlennull(pszMsgField[3])+strlennull(pszMsgField[5])+5;

        /*blob is: uin(DWORD), hcontact(HANDLE), nick(ASCIIZ), first(ASCIIZ), last(ASCIIZ), email(ASCIIZ), reason(ASCIIZ)*/
        pCurBlob=szBlob=(char *)icq_alloc_zero(pre.lParam);
        memcpy(pCurBlob,&dwUin,sizeof(DWORD));
        pCurBlob+=sizeof(DWORD);
        memcpy(pCurBlob,&hContact,sizeof(HANDLE));
        pCurBlob+=sizeof(HANDLE);
        strcpy((char *)pCurBlob,pszMsgField[0]);
        pCurBlob+=strlennull((char *)pCurBlob)+1;
        strcpy((char *)pCurBlob,pszMsgField[1]);
        pCurBlob+=strlennull((char *)pCurBlob)+1;
        strcpy((char *)pCurBlob,pszMsgField[2]);
        pCurBlob+=strlennull((char *)pCurBlob)+1;
        strcpy((char *)pCurBlob,pszMsgField[3]);
        pCurBlob+=strlennull((char *)pCurBlob)+1;
        strcpy((char *)pCurBlob,pszMsgField[5]);
        pre.szMessage=(char *)szBlob;

        CallService(MS_PROTO_CHAINRECV,0,(LPARAM)&ccs);
    }
    break;

    case MTYPE_ADDED:       /* 'you were added' */
        /* format: nick FE first FE last FE email 00 */
    {
        DWORD cbBlob;
        PBYTE pBlob, pCurBlob;

        if (nMsgFields < 4)
        {
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            chk.PSD=24;
            CheckContact(chk);
            NetLog_Server("Malformed '%s' message", "you were added");
            res = FALSE;
            break;
        }

        hContact = HContactFromUIN(dwUin, &bAdded);

        /*blob is: uin(DWORD), hcontact(HANDLE), nick(ASCIIZ), first(ASCIIZ), last(ASCIIZ), email(ASCIIZ) */
        cbBlob=sizeof(DWORD)+sizeof(HANDLE)+strlennull(pszMsgField[0])+strlennull(pszMsgField[1])+strlennull(pszMsgField[2])+strlennull(pszMsgField[3])+4;
        pCurBlob=pBlob=(PBYTE)icq_alloc_zero(cbBlob);
        memcpy(pCurBlob,&dwUin,sizeof(DWORD));
        pCurBlob+=sizeof(DWORD);
        memcpy(pCurBlob,&hContact,sizeof(HANDLE));
        pCurBlob+=sizeof(HANDLE);
        strcpy((char *)pCurBlob,pszMsgField[0]);
        pCurBlob+=strlennull((char *)pCurBlob)+1;
        strcpy((char *)pCurBlob,pszMsgField[1]);
        pCurBlob+=strlennull((char *)pCurBlob)+1;
        strcpy((char *)pCurBlob,pszMsgField[2]);
        pCurBlob+=strlennull((char *)pCurBlob)+1;
        strcpy((char *)pCurBlob,pszMsgField[3]);

        AddEvent(NULL, EVENTTYPE_ADDED, dwTimestamp, 0, cbBlob, pBlob);
    }
    break;

    case MTYPE_CONTACTS:
    {
        CCSDATA ccs;
        PROTORECVEVENT pre = {0};
        ICQSEARCHRESULT** isrList = NULL;
        char* pszNContactsEnd;
        int nContacts;
        int i;
        int valid;


        if (nMsgFields < 3
                || (nContacts = strtol(pszMsgField[0], &pszNContactsEnd, 10)) == 0
                || pszNContactsEnd - pszMsgField[0] != (int)strlennull(pszMsgField[0])
                || nMsgFields < nContacts * 2 + 1)
        {
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            CheckContact(chk);
            NetLog_Uni(bThruDC, "Malformed '%s' message", "contacts");
            res = FALSE;
            break;
        }

        valid = 1;
        isrList = (ICQSEARCHRESULT**)icq_alloc_zero(nContacts * sizeof(ICQSEARCHRESULT*));
        for (i = 0; i < nContacts; i++)
        {
            isrList[i] = (ICQSEARCHRESULT*)icq_alloc_zero(sizeof(ICQSEARCHRESULT));
            isrList[i]->hdr.cbSize = sizeof(ICQSEARCHRESULT);
            if (IsStringUIN(pszMsgField[1 + i * 2]))
            {
                // icq contact
                isrList[i]->uin = atoi(pszMsgField[1 + i * 2]);
                if (isrList[i]->uin == 0)
                    valid = 0;
            }
            else
            {
                // aim contact
                isrList[i]->uid = pszMsgField[1 + i * 2];
                if (!strlennull(isrList[i]->uid))
                    valid = 0;
            }
            isrList[i]->hdr.nick = pszMsgField[2 + i * 2];
        }

        if (!valid)
        {
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            chk.PSD=24;
            CheckContact(chk);
            NetLog_Uni(bThruDC, "Malformed '%s' message", "contacts");
            res = FALSE;
        }
        else
        {
            hContact = HContactFromUIN(dwUin, &bAdded);
            sendMessageTypesAck(hContact, 0, pAckParams);

            ccs.szProtoService = PSR_CONTACTS;
            ccs.hContact = hContact;
            ccs.wParam = 0;
            ccs.lParam = (LPARAM)&pre;
            pre.timestamp = dwTimestamp;
            pre.szMessage = (char *)isrList;
            pre.lParam = nContacts;
            CallService(MS_PROTO_CHAINRECV, 0, (LPARAM)&ccs);
        }

        for (i = 0; i < nContacts; i++)
            mir_free(isrList[i]);
    }
    break;

    case MTYPE_PLUGIN: // FIXME: this should be removed - it is never called
        hContact = NULL;

        switch(dwUin)
        {
        case 1111:    /* icqmail 'you've got mail' - not processed */
            res = FALSE;
            break;
        default:
            res = FALSE;
            break;
        }
        break;

    case MTYPE_SMS_MESSAGE:
        /* it's a SMS message from a mobile - broadcast to SMS plugin */
        if (dwUin != 1002)
        {
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            chk.PSD=24;
            CheckContact(chk);
            NetLog_Uni(bThruDC, "Malformed '%s' message", "SMS Mobile");
            break;
        }
        NetLog_Server("Received SMS Mobile message");

        ICQBroadcastAck(NULL, ICQACKTYPE_SMS, ACKRESULT_SUCCESS, NULL, (LPARAM)szMsg);
        break;

    case MTYPE_STATUSMSGEXT:
        /* it's either extended StatusMsg reply from icq2003b or a IcqWebMessage */
        if (dwUin == 1003)
        {
            NetLog_Server("Received ICQWebMessage - NOT SUPPORTED");
        }
        break;

    case MTYPE_WWP:
        /* format: fromname FE FE FE fromemail FE unknownbyte FE 'Sender IP: xxx.xxx.xxx.xxx' 0D 0A body */
    {
        DWORD cbBlob;
        PBYTE pBlob, pCurBlob;

        if (nMsgFields < 6)
        {
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            chk.PSD=24;
            CheckContact(chk);
            NetLog_Server("Malformed '%s' message", "web pager");
            res = FALSE;
            break;
        }

        /*blob is: body(ASCIIZ), name(ASCIIZ), email(ASCIIZ) */
        cbBlob=strlennull(pszMsgField[0])+strlennull(pszMsgField[3])+strlennull(pszMsgField[5])+3;
        pCurBlob=pBlob=(PBYTE)icq_alloc_zero(cbBlob);
        strcpy((char *)pCurBlob,pszMsgField[5]);
        pCurBlob+=strlennull((char *)pCurBlob)+1;
        strcpy((char *)pCurBlob,pszMsgField[0]);
        pCurBlob+=strlennull((char *)pCurBlob)+1;
        strcpy((char *)pCurBlob,pszMsgField[3]);

        AddEvent(NULL, ICQEVENTTYPE_WEBPAGER, dwTimestamp, 0, cbBlob, pBlob);
    }
    break;

    case MTYPE_EEXPRESS:
        /* format: fromname FE FE FE fromemail FE unknownbyte FE body */
    {
        DWORD cbBlob;
        PBYTE pBlob, pCurBlob;

        if (nMsgFields < 6)
        {
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            chk.PSD=24;
            CheckContact(chk);
            NetLog_Server("Malformed '%s' message", "e-mail express");
            res = FALSE;
            break;
        }

        /*blob is: body(ASCIIZ), name(ASCIIZ), email(ASCIIZ) */
        cbBlob=strlennull(pszMsgField[0])+strlennull(pszMsgField[3])+strlennull(pszMsgField[5])+3;
        pCurBlob=pBlob=(PBYTE)icq_alloc_zero(cbBlob);
        strcpy((char *)pCurBlob,pszMsgField[5]);
        pCurBlob+=strlennull((char *)pCurBlob)+1;
        strcpy((char *)pCurBlob,pszMsgField[0]);
        pCurBlob+=strlennull((char *)pCurBlob)+1;
        strcpy((char *)pCurBlob,pszMsgField[3]);

        AddEvent(NULL, ICQEVENTTYPE_EMAILEXPRESS, dwTimestamp, 0, cbBlob, pBlob);
    }
    break;

    case MTYPE_REQUESTCONTACTS:
        /* it's a contacts-request */
        NetLog_Uni(bThruDC, "Received %s from %u", "Request for Contacts", dwUin);
        break;

    case MTYPE_GREETINGCARD:
        /* it's a greeting card */
        NetLog_Uni(bThruDC, "Received %s from %u", "Greeting Card", dwUin);
        break;

    case MTYPE_SCRIPT_NOTIFY:
        /* it's a xtraz notify request */
        NetLog_Uni(bThruDC, "Received %s from %u", "Xtraz Notify Request", dwUin);
        handleXtrazNotify(dwUin, dwMsgID, dwMsgID2, wCookie, szMsg, wMsgLen, bThruDC);
        break;

    case MTYPE_SCRIPT_INVITATION:
        /* it's a xtraz invitation to session */
        NetLog_Uni(bThruDC, "Received %s from %u", "Xtraz Invitation", dwUin);
        handleXtrazInvitation(dwUin, dwMsgID, dwMsgID2, wCookie, szMsg, wMsgLen, bThruDC);
        break;

    case MTYPE_SCRIPT_DATA:
        /* it's a xtraz data packet */
        NetLog_Uni(bThruDC, "Received %s from %u", "Xtraz data packet", dwUin);
        handleXtrazData(dwUin, dwMsgID, dwMsgID2, wCookie, szMsg, wMsgLen, bThruDC);
        break;

    case MTYPE_AUTOONLINE:
    case MTYPE_AUTOAWAY:
    case MTYPE_AUTOBUSY:
    case MTYPE_AUTONA:
    case MTYPE_AUTODND:
    case MTYPE_AUTOFFC:
    {
        char** szMsg = gProtocol.MirandaStatusToAwayMsg(AwayMsgTypeToStatus(type));

        if (szMsg)
        {
            rate_record rr = {0};

            rr.bType = RIT_AWAYMSG_RESPONSE;
            rr.dwUin = dwUin;
            rr.dwMid1 = dwMsgID;
            rr.dwMid2 = dwMsgID2;
            rr.wCookie = wCookie;
            rr.wVersion = wVersion;
            rr.msgType = type;
            rr.nRequestType = 0x102;
            EnterCriticalSection(&ratesMutex);
            rr.wGroup = ratesGroupFromSNAC(gRates, ICQ_MSG_FAMILY, ICQ_MSG_RESPONSE);
            LeaveCriticalSection(&ratesMutex);

            if (!handleRateItem(&rr, TRUE))
                icq_sendAwayMsgReplyServ(dwUin, dwMsgID, dwMsgID2, wCookie, wVersion, (BYTE)type, (const char**)szMsg);
        }

        break;
    }

    case MTYPE_FILEREQ: // Never happens
    default:
        if(wMsgLen == 0)
        {
            HANDLE hContact = HContactFromUIN(dwUin,NULL);
            {
                CHECKCONTACT chk = {0};
                chk.hContact=hContact;
                chk.dwUin=dwUin;
                chk.popup=chk.logtofile=chk.historyevent=TRUE;
                chk.icqeventtype=ICQEVENTTYPE_IGNORECHECK_STATUS;
                chk.dbeventflag=DBEF_READ;
                chk.popuptype=POPTYPE_IGNORE_CHECK;
                chk.msg="has checked your ignore list";
                CheckContact(chk);
            }
        }

        NetLog_Uni(bThruDC, "Unprocessed message type %d", type);
        {
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            CheckContact(chk);
        }
        res = FALSE;
        break;

    }
    if(!bExcludePSD)
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        CheckContact(chk);
    }

    mir_free(szMsg);

    return res;
}



static void handleRecvMsgResponse(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwRef)
{
    DWORD dwUin;
    uid_str szUid;
    DWORD dwCookie;
    WORD wMessageFormat;
    WORD wStatus;
    WORD bMsgType = 0;
    BYTE bFlags;
    WORD wLength;
    HANDLE hContact;
    HANDLE hCookieContact;
    DWORD dwMsgID1, dwMsgID2;
    WORD wVersion = 0;
    message_cookie_data* pCookieData = NULL;


    unpackLEDWord(&buf, &dwMsgID1);  // Message ID
    unpackLEDWord(&buf, &dwMsgID2);
    wLen -= 8;

    unpackWord(&buf, &wMessageFormat);
    wLen -= 2;
    if (wMessageFormat != 2)
    {
        NetLog_Server("SNAC(4.B) Unknown type");
        return;
    }

    if (!unpackUID(&buf, &wLen, &dwUin, &szUid)) return;

    hContact = HContactFromUID(dwUin, szUid, NULL);

    buf += 2;   // 3. unknown
    wLen -= 2;

    if (!FindMessageCookie(dwMsgID1, dwMsgID2, &dwCookie, &hCookieContact, &pCookieData))
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        chk.PSD=2;
        CheckContact(chk);
        NetLog_Server("SNAC(4.B) Received an ack that I did not ask for from (%u)", dwUin);
        return;
    }

    if (IsValidOscarTransfer(pCookieData))
    {
        // it is OFT response
        handleRecvServResponseOFT(buf, wLen, dwUin, szUid, pCookieData);
        return;
    }

    if (!dwUin)
    {
        // AIM cannot send this - just sanity
        NetLog_Server("Error: Invalid UID in message response.");
        return;
    }

    // Length of sub chunk?
    if (wLen >= 2)
    {
        unpackLEWord(&buf, &wLength);
        wLen -= 2;
    }
    else
        wLength = 0;

    if (wLength == 0x1b && pCookieData->bMessageType != MTYPE_REVERSE_REQUEST)
    {
        // this can be v8 greeting message reply
        WORD wCookie;

        unpackLEWord(&buf, &wVersion);
        buf += 27;  /* unknowns from the msg we sent */
        wLen -= 29;

        // Message sequence (SEQ2)
        unpackLEWord(&buf, &wCookie);
        wLen -= 2;

        // Unknown (12 bytes)
        buf += 12;
        wLen -= 12;

        // Message type
        unpackByte(&buf, (BYTE*)&bMsgType);
        unpackByte(&buf, &bFlags);
        wLen -= 2;

        // Status
        unpackLEWord(&buf, &wStatus);
        wLen -= 2;

        // Priority?
        buf += 2;
        wLen -= 2;

        if (!FindCookie(wCookie, &hCookieContact, (void**)&pCookieData))
        {
            // use old reliable method
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            chk.PSD=2;
            CheckContact(chk);
            NetLog_Server("Warning: Invalid cookie in %s from (%u)", "message response", dwUin);

            if (pCookieData->bMessageType != MTYPE_AUTOAWAY && bFlags == 3)
            {
                // most probably a broken ack of some kind (e.g. from R&Q), try to fix that
                bMsgType = pCookieData->bMessageType;
                bFlags = 0;

                NetLog_Server("Warning: Invalid message type in %s from (%u)", "message response", dwUin);
            }
        }
        else if (bMsgType != MTYPE_PLUGIN && pCookieData->bMessageType != MTYPE_AUTOAWAY)
        {
            // just because some clients break it...
            CHECKCONTACT chk= {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            CheckContact(chk);
            dwCookie = wCookie;

            if (bMsgType != pCookieData->bMessageType)
                NetLog_Server("Warning: Invalid message type in %s from (%u)", "message response", dwUin);

            bMsgType = pCookieData->bMessageType;
        }
        else if (pCookieData->bMessageType == MTYPE_AUTOAWAY && bMsgType != MTYPE_PLUGIN)
        {
            if (bMsgType != pCookieData->nAckType)
                NetLog_Server("Warning: Invalid message type in %s from (%u)", "message response", dwUin);
        }
    }
    else
    {
        bMsgType = pCookieData->bMessageType;
        bFlags = 0;
    }

    if (hCookieContact != hContact)
    {
        NetLog_Server("SNAC(4.B) Ack Contact does not match Cookie Contact(0x%x != 0x%x)", hContact, hCookieContact);

        ReleaseCookie(dwCookie); // This could be a bad idea, but I think it is safe
        return;
    }

    if (bFlags == 3)     // A status message reply
    {
        // Added by BM, modified by icqj plus team
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        CheckContact(chk);

        handleStatusMsgReply("SNAC(4.B) ", hContact, dwUin, wVersion, bMsgType, (WORD)dwCookie, (char*)(buf + 2),0);
    }
    else
    {
        // An ack of some kind
        int ackType;


        if (hContact == NULL || hContact == INVALID_HANDLE_VALUE)
        {
            NetLog_Server("SNAC(4.B) Message from unknown contact (%u)", dwUin);

            ReleaseCookie(dwCookie); // This could be a bad idea, but I think it is safe
            return;
        }

        switch (bMsgType)
        {

        case MTYPE_FILEREQ:
        {
            char* szMsg;
            WORD wMsgLen;

            // Message length
            unpackLEWord(&buf, &wMsgLen);
            wLen -= 2;
            szMsg = (char *)icq_alloc_zero(wMsgLen + 1);
            szMsg[wMsgLen] = '\0';
            if (wMsgLen > 0)
            {
                memcpy(szMsg, buf, wMsgLen);
                buf += wMsgLen;
                wLen -= wMsgLen;
            }
            handleFileAck(buf, wLen, dwUin, dwCookie, wStatus, szMsg);
            // No success protoack will be sent here, since all file requests
            // will have been 'sent' when the server returns its ack
            return;
        }

        case MTYPE_PLUGIN:
        {
            WORD wMsgLen;
            DWORD dwLengthToEnd;
            DWORD dwDataLen;
            int typeId;
            WORD wFunctionId;


            if (wLength != 0x1B)
            {
                NetLog_Server("Invalid Greeting %s", "message response");

                ReleaseCookie(dwCookie);
                return;
            }

            NetLog_Server("Parsing Greeting %s", "message response");

            // Message
            unpackLEWord(&buf, &wMsgLen);
            wLen -= 2;
            buf += wMsgLen;
            wLen -= wMsgLen;

            // This packet is malformed. Possibly a file accept from Miranda IM 0.1.2.1
            if (wLen < 20) return;

            if (!unpackPluginTypeId(&buf, &wLen, &typeId, &wFunctionId, FALSE)) return;

            if (wLen < 4)
            {
                NetLog_Server("Error: Invalid greeting %s", "message response");

                ReleaseCookie(dwCookie);
                return;
            }

            // Length of remaining data
            unpackLEDWord(&buf, &dwLengthToEnd);
            wLen -= 4;

            if (wLen >= 4 && dwLengthToEnd > 0)
                unpackLEDWord(&buf, &dwDataLen); // Length of message
            else
                dwDataLen = 0;


            switch (typeId)
            {
            case MTYPE_PLAIN:
                if (pCookieData && pCookieData->bMessageType == MTYPE_AUTOAWAY && dwLengthToEnd >= 4)
                {
                    // ICQ 6 invented this
                    char *szMsg;

                    szMsg = (char*)icq_alloc_zero(dwDataLen + 1);
                    if (dwDataLen > 0)
                        memcpy(szMsg, buf, dwDataLen);
                    szMsg[dwDataLen] = '\0';
                    handleStatusMsgReply("SNAC(4.B) ", hContact, dwUin, wVersion, pCookieData->nAckType, (WORD)dwCookie, szMsg, 0);

                    break;
                }
                else
                    ackType = ACKTYPE_MESSAGE;
                break;

            case MTYPE_URL:
                ackType = ACKTYPE_URL;
                break;

            case MTYPE_CONTACTS:
                ackType = ACKTYPE_CONTACTS;
                break;

            case MTYPE_FILEREQ:
            {
                char* szMsg;

                NetLog_Server("This is file ack");
                szMsg = (char *)icq_alloc_zero(dwDataLen + 1);
                if (dwDataLen > 0)
                    memcpy(szMsg, buf, dwDataLen);
                szMsg[dwDataLen] = '\0';
                buf += dwDataLen;
                wLen -= (WORD)dwDataLen;

                handleFileAck(buf, wLen, dwUin, dwCookie, wStatus, szMsg);
                // No success protoack will be sent here, since all file requests
                // will have been 'sent' when the server returns its ack
            }
            return;

            case MTYPE_SCRIPT_NOTIFY:
            {
                char *szMsg;

                szMsg = (char*)icq_alloc_zero(dwDataLen + 1);
                if (dwDataLen > 0)
                    memcpy(szMsg, buf, dwDataLen);
                szMsg[dwDataLen] = '\0';

                handleXtrazNotifyResponse(dwUin, hContact, (WORD)dwCookie, szMsg, dwDataLen);

                ReleaseCookie(dwCookie);
            }
            return;

            case MTYPE_STATUSMSGEXT:
            {
                // handle Away Message response (ICQ 6)
                char *szMsg;

                szMsg = (char*)icq_alloc_zero(dwDataLen + 1);
                if (dwDataLen > 0)
                    memcpy(szMsg, buf, dwDataLen);
                szMsg[dwDataLen] = '\0';
                szMsg = EliminateHtml(szMsg, dwDataLen);

                handleStatusMsgReply("SNAC(4.B) ", hContact, dwUin, wVersion, pCookieData->nAckType, (WORD)dwCookie, szMsg, MTF_PLUGIN);

                mir_free(szMsg);
            }
            break;

            default:
                NetLog_Server("Error: Unknown plugin message response, type %d.", typeId);
                return;
            }
        }
        break;

        case MTYPE_PLAIN:
            ackType = ACKTYPE_MESSAGE;
            break;

        case MTYPE_URL:
            ackType = ACKTYPE_URL;
            break;

        case MTYPE_AUTHOK:
        case MTYPE_AUTHDENY:
            ackType = ACKTYPE_AUTHREQ;
            break;

        case MTYPE_ADDED:
            ackType = ACKTYPE_ADDED;
            break;

        case MTYPE_CONTACTS:
            ackType = ACKTYPE_CONTACTS;
            break;

        case MTYPE_REVERSE_REQUEST:
        {
            reverse_cookie *pReverse = (reverse_cookie*)pCookieData;

            if (pReverse->ft)
            {
                filetransfer *ft = (filetransfer*)pReverse->ft;

                ICQBroadcastAck(ft->hContact, ACKTYPE_FILE, ACKRESULT_FAILED, ft, 0);
            }
            NetLog_Server("Reverse Connect request failed");
            // Set DC status to failed
            setSettingByte(hContact, "DCStatus", 2);
        }
        break;

        case MTYPE_CHAT:
        default:
            NetLog_Server("SNAC(4.B) Unknown message type (%u) in switch", bMsgType);
            return;
        }

        if (bMsgType != MTYPE_REVERSE_REQUEST && ((ackType == MTYPE_PLAIN && pCookieData && (pCookieData->nAckType == ACKTYPE_CLIENT)) ||
                ackType != MTYPE_PLAIN))
        {
            ICQBroadcastAck(hContact, ackType, ACKRESULT_SUCCESS, (HANDLE)(WORD)dwCookie, 0);
        }
    }

    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        CheckContact(chk);
    }

    ReleaseCookie(dwCookie);
}


// A response to a CLI_SENDMSG
static void handleRecvServMsgError(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwSequence)
{
    WORD wError;
    char* pszErrorMessage;
    HANDLE hContact;
    message_cookie_data* pCookieData = NULL;
    int nMessageType;
// BOOL isHidden;

    if (wLen < 2)
        return;

    if (FindCookie((WORD)dwSequence, &hContact, (void**)&pCookieData))
    {
        // all packet cookies from msg family has command 0 in the queue
        DWORD dwUin;
        uid_str szUid;

        getContactUid(hContact, &dwUin, &szUid);

        // Error code
        unpackWord(&buf, &wError);

        if (wError == 9 && pCookieData->bMessageType == MTYPE_AUTOAWAY)
        {
            // we failed to request away message the normal way, try it AIM way
            icq_packet packet;

            serverPacketInit(&packet, (WORD)(13 + getUINLen(dwUin)));
            packFNACHeaderFull(&packet, ICQ_LOCATION_FAMILY, ICQ_LOCATION_REQ_USER_INFO, 0, (WORD)dwSequence);
            packWord(&packet, 0x03);
            packUIN(&packet, dwUin);

            sendServPacket(&packet);

            return;
        }

        //isHidden = CheckContactCapabilities(hContact, WAS_FOUND);

        // Not all of these are actually used in family 4
        // This will be moved into a special error handling function later
        switch (wError)
        {

        case 0x0002:     // Server rate limit exceeded
            pszErrorMessage = ICQTranslate("You are sending too fast. Wait a while and try again.\r\nSNAC(4.1) Error x02");
            break;

        case 0x0003:     // Client rate limit exceeded
            pszErrorMessage = ICQTranslate("You are sending too fast. Wait a while and try again.\r\nSNAC(4.1) Error x03");
            break;

        case 0x0004:     // Recipient is not logged in (resend in a offline message)
            if (/*isHidden || */pCookieData->bMessageType == MTYPE_PLAIN)
            {
                if (((message_cookie_data_ex*)pCookieData)->isOffline)
                {
                    // offline failed - most probably to AIM contact
                    pszErrorMessage = ICQTranslate("The contact does not support receiving offline messages.");
                    break;
                }
                // TODO: this needs better solution
                setSettingWord(hContact, "Status", ID_STATUS_OFFLINE);
            }
            pszErrorMessage = ICQTranslate("The user has logged off. Select 'Retry' to send an offline message.\r\nSNAC(4.1) Error x04");
            break;

        case 0x0005:     // Requested service unavailable
            pszErrorMessage = ICQTranslate("The messaging service is temporarily unavailable. Wait a while and try again.\r\nSNAC(4.1) Error x05");
            break;

        case 0x0009:     // Not supported by client (resend in a simpler format)
            pszErrorMessage = ICQTranslate("The receiving client does not support this type of message.\r\nSNAC(4.1) Error x09");
            break;

        case 0x000A:     // Refused by client
            pszErrorMessage = ICQTranslate("You sent too long message. The receiving client does not support it.\r\nSNAC(4.1) Error x0A");
            break;

        case 0x000E:     // Incorrect SNAC format
            pszErrorMessage = ICQTranslate("The SNAC format was rejected by the server.\nSNAC(4.1) Error x0E");
            break;

        case 0x0013:     // User temporarily unavailable
            pszErrorMessage = ICQTranslate("The user is temporarily unavailable. Wait a while and try again.\r\nSNAC(4.1) Error x13");
            break;

        case 0x0001:     // Invalid SNAC header
        case 0x0006:     // Requested service not defined
        case 0x0007:     // You sent obsolete SNAC
        case 0x0008:     // Not supported by server
        case 0x000B:     // Reply too big
        case 0x000C:     // Responses lost
        case 0x000D:     // Request denied
        case 0x000F:     // Insufficient rights
        case 0x0010:     // In local permit/deny (recipient blocked)
        case 0x0011:     // Sender too evil
        case 0x0012:     // Receiver too evil
        case 0x0014:     // No match
        case 0x0015:     // List overflow
        case 0x0016:     // Request ambiguous
        case 0x0017:     // Server queue full
        case 0x0018:     // Not while on AOL
        default:
            if (pszErrorMessage = (char*)icq_alloc_zero(256))
                null_snprintf(pszErrorMessage, 256, ICQTranslate("SNAC(4.1) SENDMSG Error (x%02x)"), wError);
            break;
        }


        switch (pCookieData->bMessageType)
        {

        case MTYPE_PLAIN:
            nMessageType = ACKTYPE_MESSAGE;
            break;

        case MTYPE_CHAT:
            nMessageType = ACKTYPE_CHAT;
            break;

        case MTYPE_FILEREQ:
            nMessageType = ACKTYPE_FILE;
            break;

        case MTYPE_URL:
            nMessageType = ACKTYPE_URL;
            break;

        case MTYPE_CONTACTS:
            nMessageType = ACKTYPE_CONTACTS;
            break;

        default:
            nMessageType = -1;
            break;
        }

        if (nMessageType != -1)
        {
            ICQBroadcastAck(hContact, nMessageType, ACKRESULT_FAILED, (HANDLE)(WORD)dwSequence, (LPARAM)pszErrorMessage);
        }
        else
        {
            NetLog_Server("Error: Message delivery to %u failed: %s", dwUin, pszErrorMessage);
        }

        FreeCookie((WORD)dwSequence);

        if (pCookieData->bMessageType != MTYPE_FILEREQ)
            mir_free(pCookieData);
    }
    else
    {
        WORD wError;

        unpackWord(&buf, &wError);
        icq_SetUserStatus(0, (WORD)dwSequence, 1, 0);
        LogFamilyError(ICQ_MSG_FAMILY, wError);
    }
}


static void handleServerAck(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwSequence)
{
    DWORD dwUin;
    uid_str szUID;
    WORD wChannel;
    HANDLE hContact;
    message_cookie_data* pCookieData;


    if (wLen < 13)
    {
        NetLog_Server("Ignoring SNAC(4,C) Packet to short");
        return;
    }

    buf += 8; // Skip first 8 bytes
    wLen -= 8;

    // Message channel
    unpackWord(&buf, &wChannel);
    wLen -= 2;

    // Sender
    if (!unpackUID(&buf, &wLen, &dwUin, &szUID)) return;

    hContact = HContactFromUID(dwUin, szUID, NULL);

    if (FindCookie((WORD)dwSequence, NULL, (void**)&pCookieData))
    {
        // If the user requested a full ack, the
        // server ack should be ignored here.
        if (pCookieData && (pCookieData->nAckType == ACKTYPE_SERVER))
        {
            if ((hContact != NULL) && (hContact != INVALID_HANDLE_VALUE))
            {
                int ackType;
                int ackRes = ACKRESULT_SUCCESS;

                switch (pCookieData->bMessageType)
                {
                case MTYPE_PLAIN:
                    ackType = ACKTYPE_MESSAGE;
                    break;

                case MTYPE_CONTACTS:
                    ackType = ACKTYPE_CONTACTS;
                    break;

                case MTYPE_URL:
                    ackType = ACKTYPE_URL;
                    break;

                case MTYPE_FILEREQ:
                    ackType = ACKTYPE_FILE;
                    ackRes = ACKRESULT_SENTREQUEST;
                    // Note 1: We are not allowed to free the cookie here because it
                    // contains the filetransfer struct that we will need later
                    // Note 2: The cookiedata is NOT a message_cookie_data*, it is a
                    // filetransfer*. IMPORTANT! (it's one of those silly things)
                    break;

                default:
                    ackType = -1;
                    NetLog_Server("Error: Unknown message type %d in ack", pCookieData->bMessageType);
                    break;
                }
                if (ackType != -1)
                    ICQBroadcastAck(hContact, ackType, ackRes, (HANDLE)(WORD)dwSequence, 0);

                if (pCookieData->bMessageType != MTYPE_FILEREQ)
                    mir_free(pCookieData); // this could be a bad idea, but I think it is safe
            }
            FreeCookie((WORD)dwSequence);
        }
        else if (pCookieData && (pCookieData->nAckType == ACKTYPE_CLIENT))
        {
            NetLog_Server("Received a server ack, waiting for client ack.");
        }
        else
        {
            NetLog_Server("Ignored a server ack I did not ask for");
        }
    } //else
    //icq_SetUserStatus(0,(WORD)dwSequence, -1, 0); not necessary anymore

    return;
}



static void handleMissedMsg(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwRef)
{
    DWORD dwUin;
    WORD wChannel;
    WORD wWarningLevel;
    WORD wCount;
    WORD wError;
    WORD wTLVCount;
    char* pszErrorMsg;
    oscar_tlv_chain* pChain;


    if (wLen < 14)
        return; // Too short

    // Message channel?
    unpackWord(&buf, &wChannel);
    wLen -= 2;

    // Sender
    if (!unpackUID(&buf, &wLen, &dwUin, NULL)) return;

    if (wLen < 8)
        return; // Too short

    // Warning level?
    unpackWord(&buf, &wWarningLevel);
    wLen -= 2;

    // TLV count
    unpackWord(&buf, &wTLVCount);
    wLen -= 2;

    // Read past user info TLVs
    pChain = readIntoTLVChain(&buf, (WORD)(wLen-4), wTLVCount);

    if (pChain)
        disposeChain(&pChain);

    if (wLen < 4)
        return; // Too short


    // Number of missed messages
    unpackWord(&buf, &wCount);
    wLen -= 2;

    // Error code
    unpackWord(&buf, &wError);
    wLen -= 2;

    {
        // offline retrieval process in progress, note that we received missed message notification
        offline_message_cookie *cookie;

        if (FindCookieByType(CKT_OFFLINEMESSAGE, NULL, NULL, (void**)&cookie))
            cookie->nMissed++;
    }


    switch (wError)
    {

    case 0:
        pszErrorMsg = "** This message was blocked by the ICQ server ** The message was invalid.";
        break;

    case 1:
        pszErrorMsg = "** This message was blocked by the ICQ server ** The message was too long.";
        break;

    case 2:
        pszErrorMsg = "** This message was blocked by the ICQ server ** The sender has flooded the server.";
        break;

    case 4:
        pszErrorMsg = "** This message was blocked by the ICQ server ** You are too evil.";
        break;

    default:
        // 3 = Sender too evil (sender warn level > your max_msg_sevil)
        return;
        break;
    }

    pszErrorMsg = ICQTranslateUtf(pszErrorMsg);

    // Create message to notify user
    {
        CCSDATA ccs;
        PROTORECVEVENT pre = {0};
        int bAdded;

        ccs.szProtoService = PSR_MESSAGE;
        ccs.hContact = HContactFromUIN(dwUin, &bAdded);
        ccs.wParam = 0;
        ccs.lParam = (LPARAM)&pre;
        pre.timestamp = time(NULL);
        pre.szMessage = pszErrorMsg;
        pre.flags = PREF_UTF;

        CallService(MS_PROTO_CHAINRECV, 0, (LPARAM)&ccs);
    }
    mir_free(pszErrorMsg);
}



static void handleOffineMessagesReply(unsigned char *buf, WORD wLen, WORD wFlags, DWORD dwRef)
{
    offline_message_cookie *cookie;
    icq_packet packet;

    if (FindCookie(dwRef, NULL, (void**)&cookie))
    {
        NetLog_Server("End of offline msgs, %u received", cookie->nMessages);
        if (cookie->nMissed)
        {
            // NASTY WORKAROUND!!
            // The ICQ server has a bug that causes offline messages to be received again and again when some
            // missed message notification is present (most probably it is not processed correctly and causes
            // the server to fail the purging process); try to purge them using the old offline messages
            // protocol.  2008/05/21
            NetLog_Server("Warning: Received %u missed message notifications, trying to fix the server.", cookie->nMissed);


            // This will delete the messages stored on server
            serverPacketInit(&packet, 24);
            packFNACHeader(&packet, ICQ_EXTENSIONS_FAMILY, ICQ_META_CLI_REQ);
            packWord(&packet, 1);             // TLV Type
            packWord(&packet, 10);            // TLV Length
            packLEWord(&packet, 8);           // Data length
            packLEDWord(&packet, dwLocalUIN); // My UIN
            packLEWord(&packet, CLI_DELETE_OFFLINE_MSGS_REQ); // Ack offline msgs
            packLEWord(&packet, 0x0000);      // Request sequence number (we dont use this for now)

            // Send it
            sendServPacket(&packet);
        }

        ReleaseCookie(dwRef);
    }
    else
        NetLog_Server("Error: Received unexpected end of offline msgs.");
    OfflineMessagesReceived=1;
}



static void handleTypingNotification(unsigned char* buf, WORD wLen, WORD wFlags, DWORD dwRef)
{
    DWORD dwUin;
    uid_str szUID;
    WORD wChannel;
    WORD wNotification;
    HANDLE hContact;

    if (wLen < 14)
    {
        NetLog_Server("Ignoring SNAC(4.x11) Packet to short");
        return;
    }

#ifndef DBG_CAPMTN
    {
        NetLog_Server("Ignoring unexpected typing notification");
        return;
    }
#endif

    // The message ID, unused?
    buf += 8;
    wLen -= 8;

    // Message channel, unused?
    unpackWord(&buf, &wChannel);
    wLen -= 2;

    // Sender
    if (!unpackUID(&buf, &wLen, &dwUin, &szUID)) return;

    hContact = HContactFromUID(dwUin, szUID, NULL);

    if (hContact == INVALID_HANDLE_VALUE) return;



    // Typing notification code
    unpackWord(&buf, &wNotification);
    wLen -= 2;

    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        chk.PSD=25;
        CheckContact(chk);
    }
    SetContactCapabilities(hContact, CAPF_TYPING);

    // Notify user
    switch (wNotification)
    {

    case MTN_FINISHED:
    case MTN_TYPED:
        CallService(MS_PROTO_CONTACTISTYPING, (WPARAM)hContact, (LPARAM)PROTOTYPE_CONTACTTYPING_OFF);
        NetLog_Server("%s has stopped typing (ch %u).", strUID(dwUin, szUID), wChannel);
        break;

    case MTN_BEGUN:
        CallService(MS_PROTO_CONTACTISTYPING, (WPARAM)hContact, (LPARAM)60);
        NetLog_Server("%s is typing a message (ch %u).", strUID(dwUin, szUID), wChannel);
        break;

    case MTN_WINDOW_CLOSED:
    {
        if(bCloseWindowPopUp)
        {
            char szFormat[MAX_PATH];
            char szMsg[MAX_PATH];
            char *nick = NickFromHandleUtf(hContact);
            null_snprintf(szMsg, MAX_PATH, ICQTranslateUtfStatic("Contact \"%s\" has closed the message window.", szFormat, MAX_PATH), nick);
            ShowPopUpMsg(hContact, dwUin, ICQTranslateUtfStatic("ICQ Note", szFormat, MAX_PATH), szMsg, LOG_NOTE);
            mir_free(nick);
        }
        NetLog_Server("%s has closed the message window.", strUID(dwUin, szUID));
    }
    break;

    default:
        NetLog_Server("Unknown typing notification from %s, type %u (ch %u)", strUID(dwUin, szUID), wNotification, wChannel);
        break;
    }

    return;
}



void sendTypingNotification(HANDLE hContact, WORD wMTNCode)
{
    icq_packet p;
    BYTE byUinlen;
    DWORD dwUin;
    uid_str szUID;
#ifdef _DEBUG
    _ASSERTE((wMTNCode == MTN_FINISHED) || (wMTNCode == MTN_TYPED) || (wMTNCode == MTN_BEGUN));
#endif

    if (getContactUid(hContact, &dwUin, &szUID))
        return; // Invalid contact

    byUinlen = getUIDLen(dwUin, szUID);

    serverPacketInit(&p, (WORD)(23 + byUinlen));
    packFNACHeader(&p, ICQ_MSG_FAMILY, ICQ_MSG_MTN);
    packLEDWord(&p, 0x0000);          // Msg ID
    packLEDWord(&p, 0x0000);          // Msg ID
    packWord(&p, 0x01);               // Channel
    packUID(&p, dwUin, szUID);        // User ID
    packWord(&p, wMTNCode);           // Notification type

    sendServPacketAsync(&p);
}
