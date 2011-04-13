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
// Revision       : $Revision: 62 $
// Last change on : $Date: 2007-10-16 14:56:05 +0300 (Ð’Ñ‚, 16 Ð¾ÐºÑ‚ 2007) $
// Last change by : $Author: chaos.persei $
//
// DESCRIPTION:
//
//  Manages main server connection, low-level communication
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"


extern CRITICAL_SECTION connectionHandleMutex;
extern WORD wLocalSequence;
extern CRITICAL_SECTION localSeqMutex;
extern int icqGoingOnlineStatus;
HANDLE hServerConn;
WORD wListenPort;
WORD wLocalSequence;
static DWORD serverThreadId;
static HANDLE serverThreadHandle;
BOOL TryNextWorking;

int iBosSeq;


WORD sequences[] =
{
    5695, 23595, 23620, 23049, 0x2886, 0x2493, 23620, 23049, 2853, 17372, 1255, 1796, 1657, 13606, 1930, 23918, 31234, 30120, 0x1BEA, 0x5342, 0x30CC, 0x2294, 0x5697, 0x25FA, 0x3C26, 0x3303, 0x078A, 0x0FC5, 0x25D6, 0x26EE,0x7570, 0x7F33, 0x4E94, 0x07C9, 0x7339, 0x42A8
    //	0x39B1, 0x1F07
};

static int handleServerPackets(unsigned char* buf, int len, serverthread_info* info);

static unsigned __stdcall icq_TryNextServerThread(LPVOID lp)
{
    WORD CurrServ, SrvCount, Port;
    char DBModule[64], buf[1024], buf1[64], buf2[64];

    TryNextWorking = 1;

    WaitForSingleObject(serverThreadHandle, INFINITE);

    mir_snprintf(DBModule, 64, "%sSrvs", ICQ_PROTOCOL_NAME);

    SrvCount = DBGetContactSettingWord(0, DBModule, "SrvCount", 0);
    if(SrvCount == 0)
    {
        TryNextWorking = 0;
        return 0;
    }
    CurrServ = DBGetContactSettingWord(0, DBModule, "CurrServ", 0);

    if(CurrServ >= SrvCount)
    {
        DBWriteContactSettingWord(0, DBModule, "CurrServ", 0);
        icq_LogMessage(LOG_FATAL, "Server AutoChange: servers list end, exiting");
        TryNextWorking = 0;
        return 0;
    }


    CurrServ = CurrServ + 1;

    DBWriteContactSettingWord(0, DBModule, "CurrServ", CurrServ);

    mir_snprintf(buf1, 64, "server%luhost", CurrServ);
    mir_snprintf(buf2, 64, "server%luport", CurrServ);
    mir_snprintf(buf, 64, "server%luuse", CurrServ);

    if(!DBGetContactSettingByte(0, DBModule, buf, 1))
    {
        TryNextWorking = 0;
        TryNextServer();
        return 0;
    }

    Port = DBGetContactSettingWord(0, DBModule, buf2, DEFAULT_SERVER_PORT);


    DBWriteContactSettingWord(0, ICQ_PROTOCOL_NAME, "OscarPort", Port);
    setSettingString(NULL, "OscarServer", getSettingStringUtf(NULL, DBModule, buf1, 0));

    icq_LogMessage(LOG_NOTE, "Server AutoChange: connection problems with current server, switching to next");

    CIcqProto::IcqSetStatus((WPARAM)iIcqNewStatus, 0);

    TryNextWorking = 0;

    return 0;
}

void TryNextServer()
{
    if(TryNextWorking) return;

    ICQCreateThread(icq_TryNextServerThread, NULL);
}


static unsigned __stdcall icq_serverThread(void* p)
{
    serverthread_start_info* infoParam = (serverthread_start_info*)p;
    serverthread_info info = {0};

    info.isLoginServer = 1;
    info.wAuthKeyLen = infoParam->wPassLen;
    strncpy((char*)info.szAuthKey, infoParam->szPass, info.wAuthKeyLen);
    // store server port
    info.wServerPort = infoParam->nloc.wPort;

    srand(time(NULL));

    ResetSettingsOnConnect();

    // Connect to the login server
    NetLog_Server("Authenticating to server");
    {
        NETLIBOPENCONNECTION nloc = infoParam->nloc;

        hServerConn = NetLib_OpenConnection(ghServerNetlibUser, NULL, &nloc);
        if (hServerConn)
        {
            if(m_bSecureConnection)
            {
#ifdef _DEBUG
                NetLog_Server("(%d) Starting SSL negotiation", CallService(MS_NETLIB_GETSOCKET, (WPARAM)hServerConn, 0));
#endif
                if(!CallService(MS_NETLIB_STARTSSL, (WPARAM)hServerConn, 0))
                {
                    CallService(MS_NETLIB_CLOSEHANDLE, (WPARAM)hServerConn, 0);
                    hServerConn = NULL;
                }
            }
        }
        mir_free((char*)nloc.szHost);
    }
    mir_free(infoParam);


    // Login error
    if (hServerConn == NULL)
    {
        DWORD dwError = GetLastError();

        hServerConn = NULL;

        SetCurrentStatus(ID_STATUS_OFFLINE);

        if(bServerAutoChange)
            TryNextServer();
        else
            icq_LogUsingErrorCode(LOG_ERROR, dwError, "Unable to connect to ICQ login server");

        return 0;
    }


    // Initialize direct connection ports
    {
        DWORD dwInternalIP;
        BYTE bConstInternalIP = getSettingByte(NULL, "ConstRealIP", 0);

        info.hDirectBoundPort = NetLib_BindPort(icq_newConnectionReceived, NULL, &wListenPort, &dwInternalIP);
        if (!info.hDirectBoundPort)
        {
            icq_LogUsingErrorCode(LOG_WARNING, GetLastError(), "Miranda was unable to allocate a port to listen for direct peer-to-peer connections between clients. You will be able to use most of the ICQ network without problems but you may be unable to send or receive files.\n\nIf you have a firewall this may be blocking Miranda, in which case you should configure your firewall to leave some ports open and tell Miranda which ports to use in M->Options->ICQ->Network.");
            wListenPort = 0;
            if (!bConstInternalIP) deleteSetting(NULL, "RealIP");
        }
        else if (!bConstInternalIP)
            setSettingDword(NULL, "RealIP", dwInternalIP);

//	ICQWriteContactSettingDword(NULL, "OldRealIP", nlb.dwInternalIP);
        setSettingWord(NULL, "UserPort", wListenPort);
        setSettingWord(NULL, "OldUserPort", wListenPort);
    }


    // This is the "infinite" loop that receives the packets from the ICQ server
    {
        int recvResult;
        NETLIBPACKETRECVER packetRecv = {0};

        info.hPacketRecver = (HANDLE)CallService(MS_NETLIB_CREATEPACKETRECVER, (WPARAM)hServerConn, 0x2400);
        packetRecv.cbSize = sizeof(packetRecv);
        packetRecv.dwTimeout = INFINITE;
        while(hServerConn)
        {
            if (info.bReinitRecver)
            {
                // we reconnected, reinit struct
                info.bReinitRecver = 0;
                ZeroMemory(&packetRecv, sizeof(packetRecv));
                packetRecv.cbSize = sizeof(packetRecv);
                packetRecv.dwTimeout = INFINITE;
            }

            recvResult = CallService(MS_NETLIB_GETMOREPACKETS,(WPARAM)info.hPacketRecver, (LPARAM)&packetRecv);

            if (recvResult == 0)
            {
                NetLog_Server("Clean closure of server socket");
                break;
            }

            if (recvResult == SOCKET_ERROR)
            {
                NetLog_Server("Abortive closure of server socket, error: %d", GetLastError());
                break;
            }

            // Deal with the packet
            packetRecv.bytesUsed = handleServerPackets(packetRecv.buffer, packetRecv.bytesAvailable, &info);
        }

        // Close the packet receiver (connection may still be open)
        NetLib_SafeCloseHandle(&info.hPacketRecver);

        // Close DC port
        NetLib_SafeCloseHandle(&info.hDirectBoundPort);
    }

    // signal keep-alive thread to stop
    StopKeepAlive(&info);

    // disable auto info-update thread
    icq_EnableUserLookup(FALSE);

    // Time to shutdown
    icq_serverDisconnect(FALSE);
    if (gnCurrentStatus != ID_STATUS_OFFLINE && icqGoingOnlineStatus != ID_STATUS_OFFLINE)
    {
        if (!info.bLoggedIn)
        {
            if(bServerAutoChange)
                TryNextServer();
            else
                icq_LogMessage(LOG_FATAL, "Connection failed.\nLogin sequence failed for unknown reason.\nTry again later.");
            m_bConnectionLost = TRUE;
        }

        SetCurrentStatus(ID_STATUS_OFFLINE);
    }

    // Close all open DC connections
    CloseContactDirectConns(NULL);

    // Close avatar connection if any
    StopAvatarThread();

    // Offline all contacts
    {
        HANDLE hContact;

        hContact= FindFirstContact();

        while (hContact)
        {
            DWORD dwUIN;
            uid_str szUID;

            if (!getContactUid(hContact, &dwUIN, &szUID))
            {
                if (getContactStatus(hContact) != ID_STATUS_OFFLINE)
                {
                    setSettingWord(hContact, "Status", ID_STATUS_OFFLINE);

                    handleXStatusCaps(hContact, NULL, 0, NULL, 0);
                }
            }

            hContact = FindNextContact(hContact);
        }
    }
    setSettingDword(NULL, "LogonTS", 0); // clear logon time

    FlushServerIDs();         // clear server IDs list
    FlushPendingOperations(); // clear pending operations list
    FlushGroupRenames();      // clear group rename in progress list
    ratesRelease(&gRates);
    gRates = NULL;

    NetLog_Server("%s thread ended.", "Server");

    return 0;
}



void icq_serverDisconnect(BOOL bBlock)
{
    EnterCriticalSection(&connectionHandleMutex);

    if (hServerConn)
    {
        int sck = CallService(MS_NETLIB_GETSOCKET, (WPARAM)hServerConn, (LPARAM)0);
        if (sck!=INVALID_SOCKET) shutdown(sck, 2); // close gracefully
        NetLib_CloseConnection(&hServerConn, TRUE);
        LeaveCriticalSection(&connectionHandleMutex);

        // Not called from network thread?
        if (bBlock && GetCurrentThreadId() != serverThreadId)
        {
            while (WaitForSingleObjectEx(serverThreadHandle, INFINITE, TRUE) != WAIT_OBJECT_0);
            CloseHandle(serverThreadHandle);
        }
        else
            CloseHandle(serverThreadHandle);
    }
    else
        LeaveCriticalSection(&connectionHandleMutex);
}



static int handleServerPackets(unsigned char* buf, int len, serverthread_info* info)
{
    BYTE channel;
    WORD sequence;
    WORD datalen;
    int bytesUsed = 0;

    while (len > 0)
    {
        if (info->bReinitRecver)
            break;

        // All FLAPS begin with 0x2a
        if (*buf++ != FLAP_MARKER)
            break;

        if (len < 6)
            break;

        unpackByte(&buf, &channel);
        unpackWord(&buf, &sequence);
        unpackWord(&buf, &datalen);

        if (len < 6 + datalen)
            break;


#ifdef _DEBUG
        NetLog_Server("Server FLAP: Channel %u, Seq %u, Length %u bytes", channel, sequence, datalen);
#endif

        switch (channel)
        {
        case ICQ_LOGIN_CHAN:
            handleLoginChannel(buf, datalen, info);
            break;

        case ICQ_DATA_CHAN:
            handleDataChannel(buf, datalen, info);
            break;

        case ICQ_ERROR_CHAN:
            handleErrorChannel(buf, datalen);
            break;

        case ICQ_CLOSE_CHAN:
            handleCloseChannel(buf, datalen, info);
            break; // we need this for walking thru proxy

        case ICQ_PING_CHAN:
            handlePingChannel(buf, datalen);
            break;

        default:
            NetLog_Server("Warning: Unhandled %s FLAP Channel: Channel %u, Seq %u, Length %u bytes", "Server", channel, sequence, datalen);
            break;
        }

        /* Increase pointers so we can check for more FLAPs */
        len -= (datalen + 6);
        bytesUsed += (datalen + 6);
        if (len > 0)	// NOTE: To satisfy BC's "dangling pointer" error
            buf += datalen;
    }

    return bytesUsed;
}



void sendServPacket(icq_packet* pPacket)
{
    // make sure to have the connection handle
    EnterCriticalSection(&connectionHandleMutex);

    if (hServerConn)
    {
        int nRetries;
        int nSendResult;
        // This critsec makes sure that the sequence order doesn't get screwed up
        EnterCriticalSection(&localSeqMutex);



        // :IMPORTANT:
        // The FLAP sequence must be a WORD. When it reaches 0xFFFF it should wrap to
        // 0x0000, otherwise we'll get kicked by server.
        wLocalSequence++;

        // Pack sequence number
        pPacket->pData[2] = ((wLocalSequence & 0xff00) >> 8);
        pPacket->pData[3] = (wLocalSequence & 0x00ff);

        for (nRetries = 3; nRetries >= 0; nRetries--)
        {
            nSendResult = Netlib_Send(hServerConn, (const char *)pPacket->pData, pPacket->wLen, 0);

            if (nSendResult != SOCKET_ERROR)
                break;

            Sleep(1000);
        }
        LeaveCriticalSection(&localSeqMutex);
        LeaveCriticalSection(&connectionHandleMutex);

        // Rates management
        EnterCriticalSection(&ratesMutex);
        ratesPacketSent(gRates, pPacket);
        LeaveCriticalSection(&ratesMutex);

        // Send error
        if (nSendResult == SOCKET_ERROR)
        {
            icq_LogUsingErrorCode(LOG_ERROR, GetLastError(), "Your connection with the ICQ server was abortively closed");
            icq_serverDisconnect(FALSE);

            if (gnCurrentStatus != ID_STATUS_OFFLINE)
            {
                SetCurrentStatus(ID_STATUS_OFFLINE);
            }
        }
    }
    else
    {
        LeaveCriticalSection(&connectionHandleMutex);
        NetLog_Server("Error: Failed to send packet (no connection)");
    }


    mir_free(pPacket->pData);
}



typedef struct icq_packet_async_s
{
    icq_packet packet;

} icq_packet_async;

static unsigned __stdcall sendPacketAsyncThread(void* p)
{
    icq_packet_async* pArgs = (icq_packet_async*)p;
    sendServPacket(&pArgs->packet);

    mir_free(pArgs);
    return 0;
}


void sendServPacketAsync(icq_packet *packet)
{
    icq_packet_async *pArgs = NULL;

    pArgs = (icq_packet_async*)icq_alloc_zero(sizeof(icq_packet_async)); // This will be freed in the new thread
    memcpy(&pArgs->packet, packet, sizeof(icq_packet));

    ICQCreateThread(sendPacketAsyncThread, pArgs);
}



int IsServerOverRate(WORD wFamily, WORD wCommand, int nLevel)
{
    WORD wGroup;
    int result = FALSE;

    EnterCriticalSection(&ratesMutex);
    wGroup = ratesGroupFromSNAC(gRates, wFamily, wCommand);

    // check if the rate is not over specified level
    if (ratesNextRateLevel(gRates, wGroup) < ratesGetLimitLevel(gRates, wGroup, nLevel))
        result = TRUE;

    LeaveCriticalSection(&ratesMutex);

    return result;
}



void icq_login(const char* szPassword)
{
    DBVARIANT dbvServer = {DBVT_DELETED};
    char szServer[MAX_PATH];
    serverthread_start_info* stsi = NULL;
    DWORD dwUin;
    //MessageBox(0, szPassword, 0, MB_OK);
    if (bImageRequested)
    {
        icq_serverDisconnect(0);
        icq_regNewUin = 0;
        bImageRequested = 0;
        EnableWindow(GetDlgItem(hwndRegImageDialog, IDC_REGISTER), FALSE);
    }

    dwUin = getContactUin(NULL);
    stsi = (serverthread_start_info*)icq_alloc_zero(sizeof(serverthread_start_info));

    // Server host name
    if (getSettingStringStatic(NULL, "OscarServer", szServer, MAX_PATH))
        stsi->nloc.szHost = null_strdup(DEFAULT_SERVER_HOST);
    else
        stsi->nloc.szHost = null_strdup(szServer);

    // Server port
    stsi->nloc.wPort = (WORD)getSettingWord(NULL, "OscarPort", DEFAULT_SERVER_PORT);
    if (stsi->nloc.wPort == 0)
        stsi->nloc.wPort = RandRange(1024, 65535);

    // User password
    stsi->wPassLen = strlennull(szPassword);
    if (stsi->wPassLen > 9) stsi->wPassLen = 9;
    strncpy(stsi->szPass, szPassword, stsi->wPassLen);
    stsi->szPass[stsi->wPassLen] = '\0';

    // Randomize sequence
// wLocalSequence = sequences[RandRange(0, (sizeof sequences) / (sizeof sequences[0]))] - 1;
// iBosSeq = RandRange(0, (sizeof sequences) / (sizeof sequences[0]));
    wLocalSequence = generate_flap_sequence();


    dwLocalUIN = dwUin;

    serverThreadHandle = ICQCreateThreadEx(icq_serverThread, stsi, &serverThreadId);
}

BYTE m_bConnectionLost;
BOOL bImageRequested;
