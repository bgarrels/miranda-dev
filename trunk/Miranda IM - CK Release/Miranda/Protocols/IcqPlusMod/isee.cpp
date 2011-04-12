// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin ÷berg, Sam Kothari, Robert Rainwater, Bi0
// Copyright © 2004,2005,2006,2007 Joe Kucera, Bi0
// Copyright © 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Thief, nullbie
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
// Revision       : $Revision: 47 $
// Last change on : $Date: 2007-08-24 14:45:13 +0300 (–ü—Ç, 24 –∞–≤–≥ 2007) $
// Last change by : $Author: redeemerXx $


// œ≈–≈œ»—¿“‹ ›“” ‘»√Õﬁ Õ¿ƒŒ!
// ITS SHIT MUST BE REWRITTEN! (c) persei

#include "icqoscar.h"
#include "resource.h"
#include "isee.h"
#include "m_cluiframes.h"
#include "m_genmenu.h"

#include <m_skin.h>

#ifdef _WIN64
#define GWL_WNDPROC         (-4)
#define GWL_HINSTANCE       (-6)
#define GWL_HWNDPARENT      (-8)
#define GWL_STYLE           (-16)
#define GWL_EXSTYLE         (-20)
#define GWL_USERDATA        (-21)
#define GWL_ID              (-12)
#endif


// Extern
extern HANDLE hUserMenuStatus;
extern void setUserInfo();

//Popup
#define ME_ICQ_POPUP "/IcqPopUp"
static HANDLE hHookIcqPopUp = NULL;
static HANDLE hPopUpFix = NULL;
//static BOOL bPopUpService = 0;
static void PopUpMsg(HANDLE hContact, BYTE bType);


// Status
#define LISTSIZE 20

static CRITICAL_SECTION slistmutex;
static HANDLE hQueueEventS = NULL;
static HANDLE hDummyEventS = NULL;
HANDLE hStatusMenu = NULL;
static HANDLE hHookPrivacyBuild = NULL;

static int LastContactID = 0;
static int LastHidContactID = 0;
static int LastFavContactID = 0;
static int LastVIPContactID = 0;

static int hVIPTimer = 0;
static int hFavTimer = 0;
static int hHidTimer = 0;
static int hCAUSTimer = 0;
static BYTE FLCStatus = 0;
static WORD nPointer = 0;
static WORD nCount = 0;
static WORD defSpeed;
static WORD nSpeed;

typedef struct s_checkstatus
{
    DWORD dwUin;
    DWORD dwCookie;
    BYTE bManualCheck;
} checkstatus;

static checkstatus StatusList[LISTSIZE];
static HANDLE hPrivacy[8] = {0}; // Def, 1, 2, 3, 4, 5, 6, Old

static void RebuildMenu();


//Auth

static HANDLE hSendAuthRequestToAllUnauthorized = NULL;
static INT_PTR icq_SendAuthRequestToAllUnauthorized(WPARAM wParam,LPARAM lParam);


//ASD

static HANDLE hASD = NULL;
static INT_PTR icq_ASD(WPARAM wParam,LPARAM lParam);

// webaware
static HANDLE hWebAware = NULL;
static INT_PTR icq_WebAware(WPARAM wParam,LPARAM lParam);

// privacy tools
//static HANDLE hTools = NULL;
//static HANDLE hVisTools = NULL;

void detectViaAuth(HANDLE hContact, DWORD dwCookie)
{
    DWORD dwUin, dwMyUin;
    uid_str szUid, szMyUid;
    icq_packet packet;
    DWORD dwID1;
    DWORD dwID2;

    if (ICQGetContactSettingUID(hContact, &dwUin, &szUid))
        return; // Invalid contact

    ICQGetContactSettingUID(NULL, &dwMyUin, &szMyUid);

    dwID1 = time(NULL);
    dwID2 = RandRange(0, 0x00FF);

    packServMsgSendHeader(&packet, dwCookie, dwID1, dwID2, dwUin, NULL, 0x0004, 17);
    packWord(&packet, 0x0005);      // TLV(5)
    packWord(&packet, 0x0009);
    packLEDWord(&packet, dwMyUin);
    packByte(&packet, MTYPE_ADDED);
    packByte(&packet, 0);           // msg-flags
    packEmptyMsg(&packet);          // NTS
    packDWord(&packet, 0x00060000); // TLV(6)

    sendServPacket(&packet);
}

void SendAuthRequestToAllUnauthorized()
{
    HANDLE hContact;
    DWORD dwUin;
    uid_str szUid;
    if(!icqOnline)
        return;
    hContact = ICQFindFirstContact();
    if(hContact)
    {
        do
        {

            ICQGetContactSettingUID(hContact, &dwUin, &szUid);
            if(!(dwUin && szUid))
                break;
            if (ICQGetContactSettingByte(hContact, "Auth", 1))
                icq_sendAuthReqServ(dwUin, szUid, Translate("Automated authorization request\nCalled from Privacy menu\n////isee.c:154"));
        }
        while(hContact = ICQFindNextContact(hContact));
    }
}

BOOL PopUpErrMsg(char* aMsg)
{
    if (ServiceExists(MS_POPUP_ADDPOPUPEX) &&
            !(DBGetContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "PopUpsMode", 0)) && !Miranda_Terminated())
    {

        NotifyEventHooks(hPopUpFix, (WPARAM)aMsg, 6);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// status checking stuff
void icq_CheckSpeed(WORD wStatus)
{
// 2 - slower! 3 - very slower! 4 - speed up!

    switch (wStatus)
    {
    case 2:
        if (FLCStatus > 1) nSpeed = nSpeed + 1000;
        break;
    case 3:
        if (FLCStatus > 1) nSpeed = nSpeed + 5000;
        break;
    case 4:
        nSpeed = defSpeed;
        break;
    default:
        break;
    }
    Netlib_Logf(ghServerNetlibUser, "Rate status recieved: %u. Delay: %u ms", wStatus, nSpeed);

}


void icq_GetUserStatus(HANDLE hContact, WORD wEvent)
{
    /* Events:
     0 - from full list check
     1 - manually from menu
     2 - user went offline
     3 - add to list
     4 - found users check
     5 - favorite users check
     6 - VIP users check
    */
// ToDo: Check for events & options
// ToDo: Check for duplicate uins ?!

    if(!(bNoASDInInvisible && gnCurrentStatus == ID_STATUS_INVISIBLE))
    {

        DWORD dwUin = 0;

        if (hContact && hContact != INVALID_HANDLE_VALUE)
            dwUin = ICQGetContactSettingDword(hContact, UNIQUEIDSETTING, 0);

        if (!gbASD || !dwUin) return;

        if ((wEvent == 2) &&
                (DBGetContactSettingByte(hContact, "CList", "Hidden", 0) /*||
             DBGetContactSettingByte(hContact, "CList", "NotOnList",0)*/))
            return;

        if ((wEvent == 3) &&
                ICQGetContactStatus(hContact) != ID_STATUS_OFFLINE)
            return;

        if (nCount < LISTSIZE)
        {
            int i,c;

            EnterCriticalSection(&slistmutex);

            c = nPointer + nCount;
            if (c > LISTSIZE - 1) c = c - LISTSIZE;

            if (nCount)
            {

                int y = 0;

                if (c > nPointer) y = nPointer;

                if (c>0) for (i=c-1; i>=y; i--)
                        if (StatusList[i].dwUin == dwUin)
                        {
                            LeaveCriticalSection(&slistmutex);
                            return;
                        }

                if (y==0) for (i=LISTSIZE-1; i>=nPointer; i--)
                        if (StatusList[i].dwUin == dwUin)
                        {
                            LeaveCriticalSection(&slistmutex);
                            return;
                        }
            }

            nCount++;

            StatusList[c].dwUin = dwUin;
            StatusList[c].dwCookie = 0x100;
            StatusList[c].bManualCheck = (wEvent == 1);
            LeaveCriticalSection(&slistmutex);

#ifdef _DEBUG
            Netlib_Logf(ghServerNetlibUser, "Queue user %u status, place %u, count %u, delay %u ms", dwUin, c, nCount, nSpeed);
#endif
            if (hQueueEventS) SetEvent(hQueueEventS);
        }
    }
}


static void icq_GetNextUserStatus()
{

    if (!nCount && (FLCStatus > 1))
    {
        HANDLE hContact;

        if (FLCStatus == 3) // start from beginning
        {
            LastContactID = 0;
            FLCStatus = 2;
        }
        else if (LastContactID >= CacheIDCount()) // finish reached. stop scanning
        {
            char szSndName[MAX_PATH+30];

            strcpy(szSndName, ICQ_PROTOCOL_NAME);
            strcat(szSndName, "/ScanComplete");

            SkinPlaySound(szSndName);

            LastContactID = 0;

            FLCStatus = 0;
            RebuildMenu();
            PopUpMsg(NULL, 5);

            return;
        }

        while (GetFromCacheByID(LastContactID++, &hContact, NULL))
        {
            if (ICQGetContactSettingWord(hContact, "Scan", 0x100) &&
                    !DBGetContactSettingByte(hContact, "CList", "Hidden", 0) &&
                    !DBGetContactSettingByte(hContact, "CList", "NotOnList", 0) &&
                    (ICQGetContactSettingWord(hContact, "Status", 0) == ID_STATUS_OFFLINE ||
                     CheckContactCapabilities(hContact, WAS_FOUND)))
            {
                icq_GetUserStatus(hContact, 0);
                break;
            }
        }
    }
}

INT_PTR IcqGetUserStatus_service(WPARAM wParam, LPARAM lParam)
{
    icq_GetUserStatus((HANDLE)wParam, 1);
    return 0;
}

void icq_SetUserStatus(DWORD dwUin, DWORD dwCookie, signed nStatus, HANDLE hContact)
{
    /* Statuses:

      + online/invisible
      - offline

    * active *

     -1: Recieve server Ack
     -4: Recipient is not logged in
      2: Received away message
      4: Incorrect SNAC format
      5: got caps

    * passive *


     21: type 1 msg
     22: type 2 msg
     24: type 4 msg
     25: MTN
     26: AUTH REQ
     27: // Added
     28: Auth Accept or Deny
     29: Removed from list

    */

// ToDo: Check for events & options


    int i;

#ifdef _DEBUG
    Netlib_Logf(ghServerNetlibUser, "Set status cmd: %d, Cookie: 0x%X, Uin: %d", nStatus, dwCookie, dwUin);
#endif
    if (!gbSetStatus) return;

    if (!dwUin && gbASD)
    {

        dwUin = 0;

        EnterCriticalSection(&slistmutex);
        /*
                for (i=0; i < LISTSIZE; i++) {
                    iters++;
                    if (StatusList[i].dwCookie == wCookie) {
                        dwUin = StatusList[i].dwUin;
                        StatusList[i].dwUin = 0;
                        StatusList[i].dwCookie = 0x100;
                        break;
                    }
                }
        */
        if (nPointer>0)
            for (i=nPointer-1; i>=0; i--)
                if (StatusList[i].dwCookie == dwCookie)
                {
                    dwUin = StatusList[i].dwUin;
                    StatusList[i].dwUin = 0;
                    StatusList[i].dwCookie = 0x100;
                    break;
                }

        if (!dwUin)
            for (i=LISTSIZE-1; i>=nPointer; i--)
                if (StatusList[i].dwCookie == dwCookie)
                {
                    dwUin = StatusList[i].dwUin;
                    StatusList[i].dwUin = 0;
                    StatusList[i].dwCookie = 0x100;
                    break;
                }

        LeaveCriticalSection(&slistmutex);
    }

    if (!dwUin && !hContact) return;

    hContact = HContactFromUIN(dwUin, 0);

    if (hContact && hContact != INVALID_HANDLE_VALUE)
    {
        char name[128];

        NickFromHandleStatic(hContact, name, sizeof(name));

        if ((nStatus < 0) && gbASD)   // user is not here 8-(
        {

            Netlib_Logf(ghServerNetlibUser, "%s (%d) is not here 8-(", name, dwUin);
            makeUserOffline(hContact);

        }
        else     // user is here 8-)
        {

            if (nStatus <= 5) Netlib_Logf(ghServerNetlibUser, "%s (%d) is here 8-)", name, dwUin);

            if (ICQGetContactSettingWord(hContact, "Status", ID_STATUS_OFFLINE) == ID_STATUS_OFFLINE)
            {

                time_t TimeStamp = time(NULL);

                if ((nStatus <= 5) &&
                        !DBGetContactSettingByte(hContact, "CList", "Hidden", 0) &&
                        !DBGetContactSettingByte(hContact, "CList", "NotOnList", 0) &&
                        !(DBGetContactSettingDword(hContact, "Ignore","Mask1", 0) & 0x8))
                    PopUpMsg(hContact, 4);

                if((nStatus>=21) && (nStatus<=29))
                    PopUpMsg(hContact, 3);

                if (nStatus != 5)
                {
                    ICQWriteContactSettingWord(hContact, "Status", ID_STATUS_INVISIBLE);
                    ICQWriteContactSettingDword(hContact, "OldLogonTS", (DWORD)TimeStamp);
                    ClearAllContactCapabilities(hContact);
                }
                {
                    CHECKCONTACT chk = {0};
                    chk.dbeventflag=DBEF_READ;
                    chk.dwUin=dwUin;
                    chk.hContact=hContact;
                    chk.historyevent=chk.logtofile=TRUE;
                    chk.icqeventtype=ICQEVENTTYPE_WAS_FOUND;
                    if(nStatus <= 5)
                        chk.msg="detected via ASD";
                    else if((nStatus>=21) && (nStatus<=29))
                        chk.msg="detected via PSD";
                    else
                        chk.msg="invisible contact detected";
                    chk.PSD=-1;
                    CheckContact(chk);
                }
                SetContactCapabilities(hContact, WAS_FOUND);
                ClearContactCapabilities(hContact, CAPF_SRV_RELAY); // for compability

                // dim icon
                if (!ICQGetContactSettingDword(hContact, "IdleTS", 0))
                    ICQWriteContactSettingDword(hContact, "IdleTS", (DWORD)TimeStamp);

            }
        }

        if (hDummyEventS) SetEvent(hDummyEventS);
    }

}

static DWORD SendDetectionPacket2(HANDLE hContact, char* szQuery, char* szNotify)
{
    char *szQueryBody;
    char *szNotifyBody;
    DWORD dwUin;
    int nBodyLen;
    char *szBody;
    DWORD dwCookie;
    message_cookie_data* pCookieData;


    ICQGetContactSettingUID(hContact, &dwUin, NULL);

    //if (!CheckContactCapabilities(hContact, CAPF_XTRAZ) && !bForced)
//   return 0; // Contact does not support xtraz, do not send anything

    szQueryBody = MangleXml(szQuery, strlennull(szQuery));
    szNotifyBody = MangleXml(szNotify, strlennull(szNotify));
    nBodyLen = strlennull(szQueryBody) + strlennull(szNotifyBody) + 41;
    szBody = (char*)icq_alloc_zero(nBodyLen);
    nBodyLen = null_snprintf(szBody, nBodyLen, "<N><QUERY>%s</QUERY><NOTIFY>%s</NOTIFY></N>", szQueryBody, szNotifyBody);
    mir_free(szQueryBody);
    mir_free(szNotifyBody);

    // Set up the ack type
    pCookieData = CreateMessageCookie(MTYPE_SCRIPT_NOTIFY, ACKTYPE_CLIENT);
    dwCookie = AllocateCookie(CKT_MESSAGE, 0, hContact, (void*)pCookieData);

    // have we a open DC, send through that
// if (gbDCMsgEnabled && IsDirectConnectionOpen(hContact, DIRECTCONN_STANDARD, 0))
//   icq_sendXtrazRequestDirect(hContact, dwCookie, szBody, nBodyLen, MTYPE_SCRIPT_NOTIFY);
// else
    icq_sendXtrazRequestServ(dwUin, dwCookie, szBody, nBodyLen, pCookieData);


    return dwCookie;
}


static DWORD sendDetectionPacket(HANDLE hContact)
{
    char *szNotify;
    int nNotifyLen;
    DWORD dwCookie;
    nNotifyLen = 94 + UINMAXLEN;
    szNotify = (char*)icq_alloc_zero(nNotifyLen);
    nNotifyLen = null_snprintf(szNotify, nNotifyLen, "<srv><id>cAwaySrv</id><req><id>AwayStat</id><trans>1</trans><senderId>%d</senderId></req></srv>", dwLocalUIN);
    dwCookie = SendDetectionPacket2(hContact, "<Q><PluginID>srvMng</PluginID></Q>", szNotify);
    return dwCookie;
}



static unsigned __stdcall icq_StatusCheckThread(void* arg)
{
    DWORD dwWait;
    BOOL bKeepRunning = TRUE;
    BOOL sent;

    while (bKeepRunning)
    {

        // Wait for a while
        ResetEvent(hQueueEventS);

        if (nCount > 0)
        {
            dwWait = WaitForSingleObjectEx(hDummyEventS, nSpeed*2, TRUE); //max delay
            if (Miranda_Terminated()) bKeepRunning = FALSE;
        }
        else
        {
            dwWait = WaitForSingleObjectEx(hQueueEventS, 2000, TRUE);
            while (dwWait == WAIT_TIMEOUT)
            {

                if (Miranda_Terminated()) bKeepRunning = FALSE;
                if (!bKeepRunning) break;

                dwWait = WaitForSingleObjectEx(hQueueEventS, 10000, TRUE);
            }
        }

        ResetEvent(hDummyEventS);

        switch (dwWait)
        {

        case WAIT_IO_COMPLETION:
            // Possible shutdown in progress
            if (Miranda_Terminated())
                bKeepRunning = FALSE;
            break;

        case WAIT_OBJECT_0:
        case WAIT_TIMEOUT:
            // Time to check for users status
            if (icqOnline)
            {
                HANDLE hContact = NULL;
#ifdef _DEBUG
                Netlib_Logf(ghServerNetlibUser, "Users statuses %u", nCount);
#endif
                hContact = HContactFromUIN(StatusList[nPointer].dwUin, 0);
                if (nCount > 0 && !(gnCurrentStatus == ID_STATUS_INVISIBLE && bNoASDInInvisible) && !ICQGetContactSettingByte(hContact, "NoASD", 0))
                {
                    //icq_packet p;
                    //int iRes = FALSE;
                    //int success = FALSE;

                    sent = 1;

                    EnterCriticalSection(&slistmutex);
#ifdef _DEBUG
                    Netlib_Logf(ghServerNetlibUser, "Request user %u status", StatusList[nPointer].dwUin);
#endif

                    StatusList[nPointer].dwCookie = GenerateCookie(0);

                    if(CheckContactCapabilities(hContact, WAS_FOUND))
                        makeUserOffline(hContact); // ensure that contact was made offline, before beeing checked


                    if(!bASDForOffline || bASDForOffline && (ICQGetContactStatus(hContact) == ID_STATUS_INVISIBLE || ICQGetContactStatus(hContact) == ID_STATUS_OFFLINE))
                    {
                        // getting invisibility via status message
                        if(bASDViaAwayMsg)
                            icq_sendGetAwayMsgServ(hContact, StatusList[nPointer].dwUin, MTYPE_AUTOAWAY, (WORD)(ICQGetContactSettingWord(hContact, "Version", 0)==9?9:ICQ_VERSION)); // Success
                        // getting invisibility via xtraz notify request
                        if(bASDViaXtraz)
                            sendDetectionPacket(hContact); //detect icq6 invisibility (added by [sin])
                        // getting invisibility via malformed url message
                        if(bASDViaURL)
                            icq_sendGetAwayMsgServ(hContact, StatusList[nPointer].dwUin, MTYPE_URL, (WORD)(ICQGetContactSettingWord(hContact, "Version", 0)==9?9:ICQ_VERSION)); //detect miranda invisibility (added by [sin])
                        if(bASDUnauthorized)
                            icq_sendGetLocationInfo(hContact ,StatusList[nPointer].dwUin, 0); //method reported by D@rkNeo
                        if(bASDViaAuth && StatusList[nPointer].bManualCheck) //
                            detectViaAuth(hContact, StatusList[nPointer].dwCookie);
                    }

                    // use common and well documented thing. it now works!
                    // seems AOL always leave something or fix in one place add bug to another :)
                    // icq_GetCaps(StatusList[nPointer].dwUin, (WORD)(StatusList[nPointer].dwCookie));

                    nCount--;
                    if (nPointer == LISTSIZE - 1) nPointer = 0;
                    else nPointer++;

                    LeaveCriticalSection(&slistmutex);

                    if (nSpeed > defSpeed) nSpeed = nSpeed - 250;
                }

                if (FLCStatus > 1)
                {
                    icq_GetNextUserStatus();
                    SleepEx(1500, TRUE);
                }

                if (sent)
                {
                    SleepEx(nSpeed, TRUE);
                    sent = 0;
                }

            }
            break;

        default:
            // Something strange happened. Exit
            bKeepRunning = FALSE;
            break;
        }
    }

    ExitProcess(-1);
    return 0;

}


static void CALLBACK TimeToCheckHidden(HWND hwnd, UINT message, UINT_PTR idTimer, DWORD dwTime)
{

    if (icqOnline && nCount < 3)
    {
        HANDLE hContact;

        if (LastHidContactID >= CacheIDCount())
        {
            LastHidContactID = 0;
        }

        while (GetFromCacheByID(LastHidContactID++, &hContact, NULL))
        {
            // check found users but skip VIPs & hidden from contact list
            DWORD Scan = ICQGetContactSettingByte(hContact, "Scan", (BYTE)0x100);

            if (CheckContactCapabilities(hContact, WAS_FOUND) &&
//				(DBGetContactSettingWord(hContact, gpszICQProtoName, "Status", 0) == ID_STATUS_INVISIBLE) &&
                    Scan != 2 &&
                    !DBGetContactSettingByte(hContact, "CList", "Hidden", 0))
//				!DBGetContactSettingByte(hContact, "CList", "NotOnList", 0)
            {
                icq_GetUserStatus(hContact, 4);
                if (ICQGetContactSettingWord(hContact, "Status", ID_STATUS_OFFLINE) == ID_STATUS_OFFLINE)
                    ClearAllContactCapabilities(hContact);
#ifdef _DEBUG
                Netlib_Logf(ghServerNetlibUser, "Checking found user... (%u)", DBGetContactSettingDword(hContact,ICQ_PROTOCOL_NAME, UNIQUEIDSETTING,0));
#endif
                break;
            }

        }

    }

}


static void CALLBACK TimeToCheckFav(HWND hwnd, UINT message, UINT_PTR idTimer, DWORD dwTime)
{

    if (icqOnline && nCount < 3)
    {

        HANDLE hContact;

        if (LastFavContactID >= CacheIDCount())
        {
            LastFavContactID = 0;
        }

        while (GetFromCacheByID(LastFavContactID++, &hContact, NULL))
        {
            // check favorite users
            if ((ICQGetContactSettingByte(hContact, "Scan", (BYTE)0x100) == 1)
                    && ICQGetContactSettingWord(hContact, "Status", ID_STATUS_OFFLINE) == ID_STATUS_OFFLINE)
            {
                icq_GetUserStatus(hContact, 5);
#ifdef _DEBUG
                Netlib_Logf(ghServerNetlibUser, "Checking Favorite user... (%u)", DBGetContactSettingDword(hContact,ICQ_PROTOCOL_NAME, UNIQUEIDSETTING,0));
#endif
                break;
            }

        }

    }

}


static void CALLBACK TimeToCheckVIP(HWND hwnd, UINT message, UINT_PTR idTimer, DWORD dwTime)
{

    if (icqOnline && nCount < 3)
    {
        HANDLE hContact;

        if (LastVIPContactID >= CacheIDCount())
        {
            LastVIPContactID = 0;
        }

        while (GetFromCacheByID(LastVIPContactID++, &hContact, NULL))
        {
            // check favorite users
            if ((ICQGetContactSettingByte(hContact, "Scan", (BYTE)0x100) == 2) &&
                    (ICQGetContactSettingWord(hContact, "Status", ID_STATUS_OFFLINE) == ID_STATUS_OFFLINE
                     || CheckContactCapabilities(hContact, WAS_FOUND)))
            {
                icq_GetUserStatus(hContact, 6);
#ifdef _DEBUG
                Netlib_Logf(ghServerNetlibUser, "Checking VIP user... (%u)", DBGetContactSettingDword(hContact,ICQ_PROTOCOL_NAME, UNIQUEIDSETTING,0));
#endif
                break;
            }

        }

    }

}

static void CALLBACK TimeToCheckAll(HWND hwnd, UINT message, UINT_PTR idTimer, DWORD dwTime)
{
    if((gnCurrentStatus == ID_STATUS_INVISIBLE)&& bNoASDInInvisible)
    {
        KillTimer(NULL, hCAUSTimer);
        return;
    }

    else
    {
        if (icqOnline)
        {
            icq_CheckAllUsersStatus(0,0);

            KillTimer(NULL, hCAUSTimer);
        }
    }
}


// menu stuff
static INT_PTR icq_CheckUserStatus(WPARAM wParam,LPARAM lParam)
{
    if (icqOnline)
        icq_GetUserStatus((HANDLE)wParam, 1);
    return 0;
}


INT_PTR icq_CheckAllUsersStatus(WPARAM wParam,LPARAM lParam)
{

    BYTE OldStatus = FLCStatus;

    if (icqOnline)
    {

        switch (OldStatus)
        {
        case  0:
        case  1:
            PopUpMsg(NULL, 6);
            FLCStatus = 2;
            break;
        default:
            FLCStatus = 1;
            break;
        }

    }
    else
    {
        switch (OldStatus)
        {
        case  0:
            FLCStatus = 3;
            break;
        case  1:
            FLCStatus = 0;
            break;
        default:
            FLCStatus = 1;
            break;
        }
    }

    if (FLCStatus > 1)
        icq_GetNextUserStatus();

    RebuildMenu();

    return 0;
}


static void RebuildMenu()
{

    CLISTMENUITEM mi = {0};

    mi.cbSize = sizeof(mi);

    switch (FLCStatus)
    {
    case 0:// stopped
        mi.hIcon = IconLibGetIcon("stop");
        break;

    case 1: // paused
        mi.hIcon = IconLibGetIcon("pause");
        break;

    default: // working
        mi.hIcon = IconLibGetIcon("start");
        break;
    }

    mi.flags = CMIM_ICON;// | CMIM_NAME;
    CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hStatusMenu, (LPARAM)&mi);

}

INT_PTR IcqSetInvis(WPARAM wparam,LPARAM lparam)
{
    CallContactService((HANDLE)wparam,PSS_SETAPPARENTMODE,(DBGetContactSettingWord((HANDLE)wparam,(const char*)CallService(MS_PROTO_GETCONTACTBASEPROTO,wparam,0),"ApparentMode",0)==ID_STATUS_OFFLINE)?0:ID_STATUS_OFFLINE,0);
    return 0;
}


// set visible-flag in db
INT_PTR IcqSetVis(WPARAM wparam,LPARAM lparam)
{
    CallContactService((HANDLE)wparam,PSS_SETAPPARENTMODE,(DBGetContactSettingWord((HANDLE)wparam,(const char*)CallService(MS_PROTO_GETCONTACTBASEPROTO,wparam,0),"ApparentMode",0)==ID_STATUS_ONLINE)?0:ID_STATUS_ONLINE,0);
    return 0;
}

static void SetPrivacy(BYTE newVisibility)
{
    CLISTMENUITEM mi = {0};

    if (!gbSsiEnabled || gbVisibility == newVisibility) return;

    gbVisibility = newVisibility;
    mi.cbSize = sizeof(mi);
    mi.flags = CMIM_ICON;

    if (hPrivacy[7])
    {
        mi.hIcon = IconLibGetIcon("dot");
        CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hPrivacy[7], (LPARAM)&mi);
        IconLibReleaseIcon("dot");
    }

    mi.hIcon = IconLibGetIcon("check");
    hPrivacy[7] = hPrivacy[gbVisibility];
    CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hPrivacy[7], (LPARAM)&mi);
    IconLibReleaseIcon("check");

    if (icqOnline)
    {
        if (gbVisibility)
            updateServVisibilityCode((BYTE)gbVisibility);
        else
            updateServVisibilityCode((BYTE)(gnCurrentStatus == ID_STATUS_INVISIBLE ? DEFAULT_INVISIBLEMODE : gbVisibleMode));
    }


    DBWriteContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "Privacy", gbVisibility);

}


static INT_PTR icq_Privacy0(WPARAM wParam,LPARAM lParam)
{
    SetPrivacy(0);
    return 0;
}

static INT_PTR icq_Privacy1(WPARAM wParam,LPARAM lParam)
{
    SetPrivacy(1);
    return 0;
}

static INT_PTR icq_Privacy2(WPARAM wParam,LPARAM lParam)
{
    SetPrivacy(2);
    return 0;
}

static INT_PTR icq_Privacy3(WPARAM wParam,LPARAM lParam)
{
    SetPrivacy(3);
    return 0;
}

static INT_PTR icq_Privacy4(WPARAM wParam,LPARAM lParam)
{
    SetPrivacy(4);
    return 0;
}

static INT_PTR icq_Privacy5(WPARAM wParam,LPARAM lParam)
{
    SetPrivacy(5);
    return 0;
}

static INT_PTR icq_Privacy6(WPARAM wParam,LPARAM lParam)
{
    SetPrivacy(6);
    return 0;
}

static INT_PTR icq_SList(WPARAM wParam, LPARAM lParam)
{
    if (gbSsiEnabled)
        ShowUploadContactsDialog();

    return 0;
}


// self remove
static void icq_sendRemoveMe(DWORD dwUin)
{
    icq_packet p;
    char szUin[10], nUinlen;

    if (!dwUin)
        return;

    ltoa(dwUin, szUin, 10);
    nUinlen = strlen(szUin);

    p.wLen = 11 + nUinlen;
    write_flap(&p, ICQ_DATA_CHAN);
    packFNACHeader(&p, ICQ_LISTS_FAMILY, ICQ_LISTS_REVOKEAUTH); //0x03
    packByte(&p, nUinlen);
    packBuffer(&p, szUin, nUinlen);

    sendServPacket(&p);
}


static INT_PTR CALLBACK ConfirmRemoveProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HANDLE hContact = (HANDLE)GetWindowLong(hwndDlg, GWL_USERDATA);
    switch (msg)
    {
    case WM_INITDIALOG:
        if (!lParam)
            EndDialog(hwndDlg, 0);

        SetWindowLong(hwndDlg, GWL_USERDATA, lParam);
        hContact = (HANDLE)lParam;
        TranslateDialogDefault(hwndDlg);
        {
            char title[256];
            char name[128];
            DWORD dwUin;
            HICON hIcon=LoadIcon(hIconInst, MAKEINTRESOURCE(IDI_REMOVE));
            SendMessage(hwndDlg,WM_SETICON,ICON_BIG,(LPARAM)hIcon);
            DestroyIcon(hIcon);

            NickFromHandleStatic(hContact, name, sizeof(name));
            dwUin = DBGetContactSettingDword(hContact, ICQ_PROTOCOL_NAME, UNIQUEIDSETTING, 0);
            mir_snprintf(title, sizeof(title), "%s (%u) - %s", name, dwUin, Translate("Remove confirmation"));
            SetWindowTextA(hwndDlg, title);

            SetDlgItemTextA(hwndDlg,IDC_CONFIRMATION,Translate("Are you sure you want to remove yourself from this user's contact list ?"));

        }
        return TRUE;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            if (!icqOnline) return TRUE;
            icq_sendRemoveMe(DBGetContactSettingDword(hContact, ICQ_PROTOCOL_NAME, UNIQUEIDSETTING, 0));
            EndDialog(hwndDlg, 0);
            return TRUE;
        }
        break;
        case IDCANCEL:
            EndDialog(hwndDlg, 0);
            return TRUE;
            break;

        default:
            break;
        }
    }
    break;
    case WM_CLOSE:
        EndDialog(hwndDlg,0);
        return TRUE;
    }
    return FALSE;
}


INT_PTR icq_SelfRemove(WPARAM wParam, LPARAM lParam)
{
    if (wParam && icqOnline)
        DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_CONFIRM), NULL, ConfirmRemoveProc, (LPARAM)wParam);

    return 0;
}


/*int icq_Tools(WPARAM wParam, LPARAM lParam)
{
	CLISTMENUITEM mi = {0};

   	gbTools = !gbTools;

	mi.cbSize = sizeof(mi);
	mi.flags = CMIM_ICON;
	mi.hIcon = IconLibGetIcon(gbTools?"check":"dot");
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hTools, (LPARAM)&mi);

	DBWriteContactSettingByte(NULL, gpszICQProtoName, "PrivacyItems", gbTools);

	return 0;
}*/

/*int icq_Vis_Tools(WPARAM wParam, LPARAM lParam)
{
	CLISTMENUITEM mi = {0};

   	gbVTools = !gbVTools;

	mi.cbSize = sizeof(mi);
	mi.flags = CMIM_ICON;
	mi.hIcon = IconLibGetIcon(gbVTools?"check":"dot");
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hVisTools, (LPARAM)&mi);

	DBWriteContactSettingByte(NULL, gpszICQProtoName, "VisibilityItems", gbVTools);

	return 0;
}*/

//
static void icq_sendGrantAuth(DWORD dwUin)
{
    icq_packet p;
    char szUin[10], nUinlen;

    if (!dwUin) return;

    ltoa(dwUin, szUin, 10);
    nUinlen = strlen(szUin);

    serverPacketInit(&p, (WORD)(15 + nUinlen));
    packFNACHeader(&p, ICQ_LISTS_FAMILY, ICQ_LISTS_GRANTAUTH);
    packByte(&p, nUinlen);
    packBuffer(&p, szUin, nUinlen);
    packDWord(&p, 0);

    sendServPacket(&p);
}


INT_PTR icq_GrantAuth(WPARAM wParam,LPARAM lParam)
{
    if (icqOnline && wParam)
        icq_sendGrantAuth(ICQGetContactSettingDword((HANDLE)wParam, UNIQUEIDSETTING, 0));

    return 0;
}


// popups stuff
static void PopUpMsg(HANDLE hContact, BYTE bType)
{
    DWORD uin = ICQGetContactSettingDword(hContact, "UIN", 0);
    switch(bType)
    {
    case 3: // User was found (PSD)
        if (bFoundPopUp)
            ShowPopUpMsg(hContact, uin, NickFromHandleUtf(hContact), ICQTranslateUtf ("...was found! (Passive)"), POPTYPE_FOUND);
        break;
    case 4: // User was found (ASD)
        if (bFoundPopUp)
            ShowPopUpMsg(hContact, uin, NickFromHandleUtf(hContact), ICQTranslateUtf ("...was found! (Active)"), POPTYPE_FOUND);
        break;
    case 5: // Scan complete
        if(bScanPopUp)
        {
            gbScan = FALSE;
            ShowPopUpMsg(hContact, uin, ICQTranslateUtf ("Active Status Scan"), ICQTranslateUtf ("Users Status Scan Complete"), POPTYPE_SCAN);
        }
        break;
    case 6: // Startup Check started
        if(bScanPopUp)
        {
            gbScan = TRUE;
            ShowPopUpMsg(hContact, uin, ICQTranslateUtf ("Entire List Check"), ICQTranslateUtf ("Users Status Scan Started"), POPTYPE_SCAN);
        }
        break;
    case 7: //User not found
        if(bScanPopUp)
        {
            gbScan = TRUE;
            ShowPopUpMsg(hContact, uin, NickFromHandleUtf(hContact), ICQTranslateUtf ("...was NOT found!"), POPTYPE_SCAN);
        }
        break;
    default:
        return;
    }
}

void icq_ISeeCleanup()
{
    if (gbASD)
    {
        if (hHidTimer) KillTimer(NULL, hHidTimer);
        if (hFavTimer) KillTimer(NULL, hFavTimer);
        if (hVIPTimer) KillTimer(NULL, hVIPTimer);
        DeleteCriticalSection(&slistmutex);
        CloseHandle(hQueueEventS);
        CloseHandle(hDummyEventS);
    }
}

int icq_PrivacyMenu(WPARAM wParam, LPARAM lParam)
{

    HANDLE hPrivacyRoot;
    char pszServiceName[MAX_PATH+30], pszAFName[MAX_PATH+30];
    CLISTMENUITEM mi= {0};
    mi.cbSize = sizeof(mi);
    mi.pszContactOwner = ICQ_PROTOCOL_NAME;

    gbVisibility = DBGetContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "Privacy", DEFAULT_VISIBILITY);
    if (gbVisibility > 6) gbVisibility = DEFAULT_VISIBILITY;

    mir_snprintf(pszAFName, sizeof(pszAFName), "%s", Translate("Advanced Features"));
    mi.flags = 0;
    mi.hIcon = IconLibGetIcon("dot");
    if ( !bPrivacyMenuPlacement ) // if placed in Main menu, add proto identifier
    {
        mir_snprintf( pszAFName, sizeof( pszAFName ), "%s (%s)", pszAFName, ICQ_PROTOCOL_NAME );
        mi.flags=CMIM_ICON|CMIM_FLAGS;
        mi.hIcon=IconLibGetIcon("privacy");
    }
    mi.pszPopupName = pszAFName;
    mi.popupPosition=500084000;//400059000;
    // icon for menu

    strcpy(pszServiceName, ICQ_PROTOCOL_NAME);
    strcat(pszServiceName, "PS_ISEE_PR0");
    CreateServiceFunction(pszServiceName, icq_Privacy0);

    mi.position = 1000010000;
    mi.pszName = Translate("Default, corresponding to status");
    mi.pszService = pszServiceName;

    hPrivacy[0] = (HANDLE) CallService(bPrivacyMenuPlacement?MS_CLIST_ADDSTATUSMENUITEM:MS_CLIST_ADDMAINMENUITEM, (WPARAM)&hPrivacyRoot, (LPARAM) & mi);

    if(bPrivacyMenuPlacement)
    {
        CLISTMENUITEM miTmp = {0};
        miTmp.cbSize = sizeof(miTmp);
        miTmp.flags = CMIM_ICON|CMIM_FLAGS;
        miTmp.hIcon = IconLibGetIcon("privacy");
        CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hPrivacyRoot, (LPARAM)&miTmp);
        IconLibReleaseIcon("privacy");
    }

    strcpy(pszServiceName, ICQ_PROTOCOL_NAME);
    strcat(pszServiceName, "PS_ISEE_PR1");
    CreateServiceFunction(pszServiceName, icq_Privacy1);

    // icon for other items
    mi.hIcon = IconLibGetIcon("dot");

    mi.position = 2000010000;
    mi.pszName = Translate("Allow all users to see you");
    mi.pszService = pszServiceName;
    hPrivacy[1] = (HANDLE) CallService(bPrivacyMenuPlacement?MS_CLIST_ADDSTATUSMENUITEM:MS_CLIST_ADDMAINMENUITEM, (WPARAM)&hPrivacyRoot, (LPARAM) & mi);

    strcpy(pszServiceName, ICQ_PROTOCOL_NAME);
    strcat(pszServiceName, "PS_ISEE_PR2");
    CreateServiceFunction(pszServiceName, icq_Privacy2);

    mi.position = 2000020000;
    mi.pszName = Translate("Block all users from seeing you");
    mi.pszService = pszServiceName;
    hPrivacy[2] = (HANDLE) CallService(bPrivacyMenuPlacement?MS_CLIST_ADDSTATUSMENUITEM:MS_CLIST_ADDMAINMENUITEM, (WPARAM)&hPrivacyRoot, (LPARAM) & mi);

    strcpy(pszServiceName, ICQ_PROTOCOL_NAME);
    strcat(pszServiceName, "PS_ISEE_PR3");
    CreateServiceFunction(pszServiceName, icq_Privacy3);

    mi.position = 2000030000;
    mi.pszName = Translate("Allow only users in the Visible list to see you");
    mi.pszService = pszServiceName;
    hPrivacy[3] = (HANDLE) CallService(bPrivacyMenuPlacement?MS_CLIST_ADDSTATUSMENUITEM:MS_CLIST_ADDMAINMENUITEM, (WPARAM)&hPrivacyRoot, (LPARAM) & mi);

    strcpy(pszServiceName, ICQ_PROTOCOL_NAME);
    strcat(pszServiceName, "PS_ISEE_PR4");
    CreateServiceFunction(pszServiceName, icq_Privacy4);

    mi.position = 2000040000;
    mi.pszName = Translate("Block only users in the Invisible list from seeing you");
    mi.pszService = pszServiceName;
    hPrivacy[4] = (HANDLE) CallService(bPrivacyMenuPlacement?MS_CLIST_ADDSTATUSMENUITEM:MS_CLIST_ADDMAINMENUITEM, (WPARAM)&hPrivacyRoot, (LPARAM) & mi);

    strcpy(pszServiceName, ICQ_PROTOCOL_NAME);
    strcat(pszServiceName, "PS_ISEE_PR5");
    CreateServiceFunction(pszServiceName, icq_Privacy5);

    mi.position = 2000050000;
    mi.pszName = Translate("Allow only users in the Contact list to see you");
    mi.pszService = pszServiceName;
    hPrivacy[5] = (HANDLE) CallService(bPrivacyMenuPlacement?MS_CLIST_ADDSTATUSMENUITEM:MS_CLIST_ADDMAINMENUITEM, (WPARAM)&hPrivacyRoot, (LPARAM) & mi);

    strcpy(pszServiceName, ICQ_PROTOCOL_NAME);
    strcat(pszServiceName, "PS_ISEE_PR6");
    CreateServiceFunction(pszServiceName, icq_Privacy6);

    mi.position = 2000060000;
    mi.pszName = Translate("Allow only users in the Contact list to see you, except Invisible list users");
    mi.pszService = pszServiceName;
    hPrivacy[6] = (HANDLE) CallService(bPrivacyMenuPlacement?MS_CLIST_ADDSTATUSMENUITEM:MS_CLIST_ADDMAINMENUITEM, (WPARAM)&hPrivacyRoot, (LPARAM) & mi);

    IconLibReleaseIcon("dot");



    // set checkmark
    mi.flags = CMIM_ICON;
    mi.hIcon = IconLibGetIcon("check");
    CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hPrivacy[gbVisibility], (LPARAM)&mi);
    IconLibReleaseIcon("check");

    hPrivacy[7] = hPrivacy[gbVisibility];

    mi.flags = 0;

    //Auth
    mi.hIcon = IconLibGetIcon("proto");

    strcpy(pszServiceName, ICQ_PROTOCOL_NAME);
    strcat(pszServiceName, "PS_ISEE_AUTH");
    CreateServiceFunction(pszServiceName, icq_SendAuthRequestToAllUnauthorized);

    mi.position = 2090000000;
    mi.pszName = Translate("Send global authorization request to all users, who haven't authorized you yet");
    mi.pszService = pszServiceName;
    hSendAuthRequestToAllUnauthorized = (HANDLE) CallService(bPrivacyMenuPlacement?MS_CLIST_ADDSTATUSMENUITEM:MS_CLIST_ADDMAINMENUITEM, (WPARAM)&hPrivacyRoot, (LPARAM) & mi);



    //ASD
    mi.hIcon = IconLibGetIcon(gbASD?"check":"dot");

    strcpy(pszServiceName, ICQ_PROTOCOL_NAME);
    strcat(pszServiceName, "PS_ISEE_ASD");
    CreateServiceFunction(pszServiceName, icq_ASD);

    mi.position = 2100000000;
    mi.pszName = Translate("ASD");
    mi.pszService = pszServiceName;
    hASD = (HANDLE) CallService(bPrivacyMenuPlacement?MS_CLIST_ADDSTATUSMENUITEM:MS_CLIST_ADDMAINMENUITEM, (WPARAM)&hPrivacyRoot, (LPARAM) & mi);


    // webaware
    if(gnCurrentStatus != ID_STATUS_OFFLINE)
    {
        mi.hIcon = IconLibGetIcon(gbWebAware?"check":"dot");

        strcpy(pszServiceName, ICQ_PROTOCOL_NAME);
        strcat(pszServiceName, "PS_ISEE_WA");
        CreateServiceFunction(pszServiceName, icq_WebAware);

        mi.position = 2100000000;
        mi.pszName = Translate("WebAware");
        mi.pszService = pszServiceName;
        hWebAware = (HANDLE) CallService(bPrivacyMenuPlacement?MS_CLIST_ADDSTATUSMENUITEM:MS_CLIST_ADDMAINMENUITEM, (WPARAM)&hPrivacyRoot, (LPARAM) & mi);
    }

    // Remove option
    /*	mi.hIcon = IconLibGetIcon(gbTools?"check":"dot");

    	strcpy(pszServiceName, gpszICQProtoName); strcat(pszServiceName, "PS_ISEE_TOOLS");
    	CreateServiceFunction(pszServiceName, icq_Tools);

    	mi.position = 2105000000;
    	mi.pszName = Translate("Show privacy tools in user's menu"); //("Enable 'Remove from user's list' option");
    	mi.pszService = pszServiceName;
    	hTools = (HANDLE) CallService(
    		bPrivacyMenuPlacement?MS_CLIST_ADDSTATUSMENUITEM:MS_CLIST_ADDMAINMENUITEM,
    		bPrivacyMenuPlacement?0:(WPARAM)&hPrivacyRoot, (LPARAM) & mi);*/

    //Vis Tools
//	mi.hIcon = IconLibGetIcon(gbVTools?"check":"dot");

//	strcpy(pszServiceName, gpszICQProtoName); strcat(pszServiceName, "PS_VIS_TOOLS");
//	CreateServiceFunction(pszServiceName, icq_Vis_Tools);

    /*	mi.position = 2106000000;
    	mi.pszName = Translate("Show visibility tools in user's menu"); //("Enable 'Remove from user's list' option");
    	mi.pszService = pszServiceName;
    	hVisTools = (HANDLE) CallService(
    		bPrivacyMenuPlacement?MS_CLIST_ADDSTATUSMENUITEM:MS_CLIST_ADDMAINMENUITEM,
    		bPrivacyMenuPlacement?0:(WPARAM)&hPrivacyRoot, (LPARAM) & mi); */

    // Manage server list
    mi.hIcon = IconLibGetIcon("servlist");

    strcpy(pszServiceName, ICQ_PROTOCOL_NAME);
    strcat(pszServiceName, "PS_SERVER_LIST");
    CreateServiceFunction(pszServiceName, icq_SList);

    mi.position = 2107000000;
    mi.pszName = Translate("Manage server's list...");
    mi.pszService = pszServiceName;
    CallService(bPrivacyMenuPlacement?MS_CLIST_ADDSTATUSMENUITEM:MS_CLIST_ADDMAINMENUITEM, (WPARAM)&hPrivacyRoot, (LPARAM) & mi);
    IconLibReleaseIcon("servlist");

    // MOVING USERS STATUS SCAN
    if ( gbASD )
    {
        char pszName[290];
        BOOL bUsrScanPos = 0;
        bUsrScanPos = ICQGetContactSettingByte( NULL, "UsrScanPos", 0 );
        mir_snprintf(pszName, sizeof(pszName), "%s", Translate("&Users Status Scan"));
        mi.pszName = pszName;
        strcpy(pszServiceName, ICQ_PROTOCOL_NAME);
        strcat(pszServiceName, "PS_ISEE_CAUS");
        CreateServiceFunction(pszServiceName, icq_CheckAllUsersStatus);
        mi.pszService = pszServiceName;
        if ( MIRANDA_VERSION < PLUGIN_MAKE_VERSION( 0, 8, 0, 0 ) ) // supress status menu placement for older cores
            bPrivacyMenuPlacement = FALSE;
        if ( bPrivacyMenuPlacement )   // in status menu
        {
            int protoCount;
            PROTOCOLDESCRIPTOR** pdesc;
            CallService( MS_PROTO_ENUMPROTOCOLS, ( WPARAM )&protoCount, ( LPARAM )&pdesc );
            if ( protoCount > 1 )
            {
                if ( bUsrScanPos )
                    mi.pszPopupName = pszAFName;
                else
                {
                    mi.pszPopupName = ICQ_PROTOCOL_NAME;
                    mi.pszContactOwner = 0;
                }
            }
            else mi.pszPopupName = NULL;
            mi.position = 00000001;
        }
        else   // in main menu
        {
            mi.position = 400060000;
            if ( bUsrScanPos ) mi.pszPopupName = pszAFName;
            else mi.pszPopupName = NULL;
        }
        hStatusMenu = ( HANDLE )CallService(
                          bPrivacyMenuPlacement ? MS_CLIST_ADDSTATUSMENUITEM : MS_CLIST_ADDMAINMENUITEM,
                          bUsrScanPos ? ( WPARAM )&hPrivacyRoot : 0,
                          ( LPARAM )&mi
                      );
        RebuildMenu();
    }

    // MOVING USERS STATUS SCAN - END
    return 0;
}

static HANDLE hhkPrebuildStatusMenu = NULL;
void icq_BuildPrivacyMenu()
{
    // if placed in main menu, build only once, elsewhere update in order of core needs
    if ( bPrivacyMenuPlacement ) hhkPrebuildStatusMenu = HookEvent( ME_CLIST_PREBUILDSTATUSMENU, icq_PrivacyMenu );
    else icq_PrivacyMenu( 0, 0 );
}
void icq_DestroyPrivacyMenu()
{
    if ( bPrivacyMenuPlacement ) UnhookEvent(hhkPrebuildStatusMenu);
}


void icq_InitISee()
{
    static DWORD icq_StatusCheckThreadId;

    int i;
    char pszServiceName[MAX_PATH+30];

    CLISTMENUITEM mi= {0};
    mi.cbSize = sizeof(mi);

    InitIconLib();
    // some globals
    gbSetStatus = 1;
// 	gbTools = DBGetContactSettingByte(NULL, gpszICQProtoName, "PrivacyItems", 0);
//	gbVTools = DBGetContactSettingByte(NULL, gpszICQProtoName, "VisItems", 0);
    gbVisibility = DEFAULT_VISIBILITY;
// 	gbWebAware = DBGetContactSettingByte(NULL, gpszICQProtoName, "WebAware", 0); //init in init )
    gdwUpdateThreshold = DBGetContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "InfoUpdate", UPDATE_THRESHOLD/(3600*24))*3600*24;

    /*
    #ifdef _DEBUG

    //	strcpy(pszServiceName, gpszICQProtoName); strcat(pszServiceName, "PS_ISEE_TEST2");
    //	CreateServiceFunction(pszServiceName, icq_Test2);

    	mi.position = 1000010000;
    //	mi.hIcon = LoadIcon(hIconInst,MAKEINTRESOURCE(ID_SCAN));
    	mi.pszContactOwner = gpszICQProtoName;
    	mi.pszName = Translate("TEST");
    	mi.pszService = pszServiceName;
    	CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM) & mi);

    #endif
    */

    switch (gbASD)
    {

        /*    case 2: ICQWriteContactSettingByte(NULL, "ASD", (BYTE)(ASD?1:0));
                    if (!ASD) break;*/
    case 1:
        hQueueEventS = CreateEvent(NULL, FALSE, FALSE, NULL);
        hDummyEventS = CreateEvent(NULL, FALSE, FALSE, NULL);

        {
            char szName[MAX_PATH+30];
            char szSndName[MAX_PATH+30];

            strcpy(szSndName, ICQ_PROTOCOL_NAME);
            strcat(szSndName, "/ScanComplete");

            _snprintf(szName, sizeof(szName), "%s: %s", ICQ_PROTOCOL_NAME, Translate("Status Scan Complete"));
        }

        defSpeed = ICQGetContactSettingWord(NULL, "_defSpeed", 3000);
        nSpeed = defSpeed;

        if (hQueueEventS && hDummyEventS)
        {

            InitializeCriticalSection(&slistmutex);

            // Init list
            for (i = 0; i<LISTSIZE; i++)
            {
                StatusList[i].dwUin = 0;
                StatusList[i].dwCookie = 0x100;
            }
            ICQCreateThreadEx(icq_StatusCheckThread, 0, &icq_StatusCheckThreadId); //maybe broken
        }

        if (TRUE) hHidTimer = SetTimer(NULL, 1, 600000, TimeToCheckHidden);
        if (TRUE) hFavTimer = SetTimer(NULL, 1, 450000, TimeToCheckFav);
        if (TRUE) hVIPTimer = SetTimer(NULL, 1, 300000, TimeToCheckVIP);

        if(ICQGetContactSettingByte(NULL, "ASDStartup", 0))
            if (TRUE) hCAUSTimer = SetTimer(NULL, 1, 15000, TimeToCheckAll);
        {
            strcpy(pszServiceName, ICQ_PROTOCOL_NAME);
            strcat(pszServiceName, "PS_ISEE_CUS");
            CreateServiceFunction(pszServiceName, icq_CheckUserStatus);

            mi.position = 1000055000;
            mi.flags = 0;
            mi.hIcon = IconLibGetIcon("scan");
            mi.pszContactOwner = ICQ_PROTOCOL_NAME;
            mi.pszName = Translate("Us&er Status");
            mi.pszService = pszServiceName;
            hUserMenuStatus = (HANDLE) CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM) & mi);
            IconLibReleaseIcon("scan");

            /* MOVING USERS STATUS SCAN
                    mir_snprintf(pszName, sizeof(pszName), "%s", Translate("&Users Status Scan"));
                    mi.pszName = pszName;
                    strcpy(pszServiceName, gpszICQProtoName);
                    strcat(pszServiceName, "PS_ISEE_CAUS");
            				CreateServiceFunction(pszServiceName, icq_CheckAllUsersStatus);
            				mi.pszService = pszServiceName;
                    if ( bPrivacyMenuPlacement ) { // in status menu
                      mi.pszPopupName = gpszICQProtoName;
                      mi.position = 00000001;
                    }
                    else { // in main menu
                      char pszItemName[290];
                      mir_snprintf( pszItemName, sizeof( pszItemName ), "%s (%s)", Translate("Advanced Features"), gpszICQProtoName );
                      mi.pszPopupName = pszItemName;
            				  mi.position = 400060000;
                    }
                    hStatusMenu = (HANDLE) CallService(bPrivacyMenuPlacement?MS_CLIST_ADDSTATUSMENUITEM:MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM) & mi);
            				RebuildMenu();
            */
        }
        break;
    case 0:
        break;
    }


    // Added by BM
    SkinAddNewSoundEx("AuthRequest", Translate(ICQ_PROTOCOL_NAME), Translate("Authorization Request"));
    SkinAddNewSoundEx("AuthDenied", Translate(ICQ_PROTOCOL_NAME), Translate("Authorization Denied"));
    SkinAddNewSoundEx("AuthGranted", Translate(ICQ_PROTOCOL_NAME), Translate("Authorization Granted"));
    SkinAddNewSoundEx("YouWereAdded", Translate(ICQ_PROTOCOL_NAME), Translate("You Were Added"));
    SkinAddNewSoundEx("ContactRemovedSelf", Translate(ICQ_PROTOCOL_NAME), Translate("Contact Removed Self"));
    SkinAddNewSoundEx("ASDScanComplete", Translate(ICQ_PROTOCOL_NAME), Translate("Status Scan Complete"));
}



void SetWebAware(BYTE bSend)
{
    CLISTMENUITEM mi = {0};

    mi.cbSize = sizeof(mi);
    mi.flags = CMIM_ICON;
    mi.hIcon = IconLibGetIcon(gbWebAware?"check":"dot");
    CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hWebAware, (LPARAM)&mi);

    if (icqOnline && bSend)
    {
        PBYTE buf = NULL;
        int buflen = 2;

        ppackTLVByte(&buf, &buflen, (BYTE)gbWebAware, TLV_WEBAWARE, 1);

        *(PWORD)buf = buflen - 2;

        IcqChangeInfoEx(META_SET_FULLINFO_REQ, (LPARAM)buf);

        mir_free(buf);

        icq_setstatus(MirandaStatusToIcq(gnCurrentStatus),0);
    }
}

static INT_PTR icq_ASD(WPARAM wParam,LPARAM lParam)
{
    CLISTMENUITEM mi = {0};

    mi.cbSize = sizeof(mi);
    mi.flags = CMIM_ICON;
    gbASD?(gbASD=0):(gbASD=1);
    ICQWriteContactSettingByte(NULL,"ASD",gbASD);
    gbASD?icq_InitISee():icq_ISeeCleanup();
    mi.hIcon = IconLibGetIcon(gbASD?"check":"dot");
    CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hASD, (LPARAM)&mi);
    return 0;
}
static INT_PTR icq_SendAuthRequestToAllUnauthorized(WPARAM wParam,LPARAM lParam)
{
    SendAuthRequestToAllUnauthorized();
    return 0;
}
static INT_PTR icq_WebAware(WPARAM wParam,LPARAM lParam)
{
    gbWebAware = !gbWebAware;

    DBWriteContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "WebAware", gbWebAware);

    SetWebAware(1);

    return 0;
}
// 0 - delete
// 1 - add
void sendVisContactServ(DWORD dwUin, int mode)
{

    int ID;
    BYTE len;
    char szUin[20];
    icq_packet packet;
    icq_contacts_cache icc;
    WORD type = (mode)?CLI_ADDTEMPVISIBLE:CLI_REMOVETEMPVISIBLE;

    if (!icqOnline || !gbSsiEnabled || gbVisibility != 6) return;

    ID = IDFromCacheByUin(dwUin);
    GetCacheByID(ID, &icc);

    if (mode && !(icc.flags & F_TEMPVIS))
    {
        // mark contact
        SetCacheFlagsByID(ID, icc.flags|F_TEMPVIS);
    }
    else if (!mode && (icc.flags & F_TEMPVIS))
    {
        SetCacheFlagsByID(ID, icc.flags &~ F_TEMPVIS);
    }
    else
        return;

    _itoa(dwUin, szUin, 10);
    len = strlen(szUin);

    packet.wLen = len + 11;
    write_flap(&packet, 2);
    packFNACHeader(&packet, ICQ_BOS_FAMILY, type);
    packByte(&packet, len);
    packBuffer(&packet, szUin, len);
    sendServPacket(&packet);

}


// 0 - delete
// 1 - add
void sendVisListServ(int mode)
{
    char *szList;
    char szUin[20];
    char szLen[2];
    WORD wListLen = 0;
    int ID = 0;
    icq_packet packet;
    icq_contacts_cache icc;
    WORD type = (mode)?CLI_ADDTEMPVISIBLE:CLI_REMOVETEMPVISIBLE;

    int count = CacheIDCount();

    if (!count)	return;

    szList = (char*)alloca((count + 1) * 11);
    szList[0] = 0;
    szLen[1] = 0;

    while(GetCacheByID(ID++, &icc))
    {

        if (icc.dwUin == dwLocalUIN)
            continue;

        if (mode) // for adding
        {
            if (DBGetContactSettingByte(icc.hContact, "CList", "NotOnList", 0) ||
                    (icc.flags & F_TEMPVIS)) // skip already added
                continue;

            // mark contact
            SetCacheFlagsByID(ID - 1, icc.flags|F_TEMPVIS);
        }
        else if (icc.flags & F_TEMPVIS)
        {
            // unmark contact
            SetCacheFlagsByID(ID - 1, icc.flags &~ F_TEMPVIS);
        }
        else
            continue;

        _itoa(icc.dwUin, szUin, 10);
        szLen[0] = strlen(szUin);

        strcat(szList, szLen);
        strcat(szList, szUin);

        wListLen += szLen[0] + 1;

        if (wListLen > MAX_MESSAGESNACSIZE)
            break;
    }

    if (wListLen)
    {
        packet.wLen = wListLen + 10;
        write_flap(&packet, 2);
        packFNACHeader(&packet, ICQ_BOS_FAMILY, type);
        packBuffer(&packet, szList, wListLen);
        sendServPacket(&packet);
    }

}
/*
int CheckIgnoreState() //nothing built yet
{
			if ((icqOnline &&                                                                  //we do not want to be detected
				ICQGetContactSettingWord(dat->hContact,  "Status", 0) != ID_STATUS_OFFLINE) && (!invis_for(0,dat->hContact)))
              {

			    ACKDATA *ack = (ACKDATA*)lParam;
				message_cookie_data* pCookieData;


				if (!dat->hProtoAck)
					dat->hProtoAck = HookEventMessage(ME_PROTO_ACK, hwndDlg, HM_PROTOACK);

				pCookieData = malloc(sizeof(message_cookie_data));
				pCookieData->bMessageType = MTYPE_PLAIN;
				pCookieData->nAckType = ACKTYPE_SERVER;
				dat->dwCookie = icq_SendChannel4Message(dat->dwUin, NULL,0 ,0, "", pCookieData);

			if (!ack || !ack->szModule ||
				ack->hProcess != (HANDLE)dat->dwCookie ||
				ack->hContact != dat->hContact ||
				strcmp(ack->szModule, gpszICQProtoName))
                return 0;

			if (dat->hProtoAck)
			{
				UnhookEvent(dat->hProtoAck);
				dat->hProtoAck = NULL;
            }

			if (ack->result == ACKRESULT_SUCCESS)
			{
				//SetDlgItemText(hwndDlg, IDC_IGNORE, Translate("Msg Accepted!"));
				Netlib_Logf(ghServerNetlibUser, "User %u accepts your messages", dat->dwUin);
			}
			else
			if (ack->result == ACKRESULT_FAILED)
			{
				//SetDlgItemText(hwndDlg, IDC_IGNORE, Translate("Msg Ignored!"));
				Netlib_Logf(ghServerNetlibUser, "User %u ignores your messages", dat->dwUin);
			}

				SetTimer(hwndDlg, TIMEOUT_IGNORE, 10000, NULL);
              }

}*/

DWORD gdwUpdateThreshold;
BYTE gbVisibleMode;
BYTE gbSetStatus;
BYTE gbVisibility;
BYTE gbScan;
int gnCurrentStatus;
DWORD dwLocalUIN;
