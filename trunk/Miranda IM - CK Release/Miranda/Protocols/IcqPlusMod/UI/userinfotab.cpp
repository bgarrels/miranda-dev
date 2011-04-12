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
// Revision       : $Revision: 43 $
// Last change on : $Date: 2007-08-20 01:51:06 +0300 (Пн, 20 авг 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Code for User details ICQ specific pages
//
// -----------------------------------------------------------------------------

#include "../icqoscar.h"

#include <windowsx.h>


extern WORD wListenPort;

extern char* calcMD5Hash(char* szFile);
extern char* MirandaVersionToString(char* szStr, int v, int m);

static INT_PTR CALLBACK IcqDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR CALLBACK IcqDlgProcClient(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static BYTE SetValue(HWND hwndDlg, int idCtrl, HANDLE hContact, char *szModule, char *szSetting, WORD type);


#define SVS_NORMAL        0
#define SVS_GENDER        1
#define SVS_ZEROISUNSPEC  2
#define SVS_IP            3
#define SVS_COUNTRY       4
#define SVS_MONTH         5
#define SVS_SIGNED        6
#define SVS_TIMEZONE      7
#define SVS_ICQVERSION    8
#define SVS_TIMESTAMP     9
#define SVS_STATUSID      10
#define SVS_ICQ_STATUSID	11

#define SVS_FLAGS         0xFF
#define SVSF_GRAYED       0x100
#define SVSF_NORMAL       0x200
#define AVATAR_HASH_FLASH 0x08

#ifdef _WIN64
#define GWL_WNDPROC         (-4)
#define GWL_HINSTANCE       (-6)
#define GWL_HWNDPARENT      (-8)
#define GWL_STYLE           (-16)
#define GWL_EXSTYLE         (-20)
#define GWL_USERDATA        (-21)
#define GWL_ID              (-12)
#endif

//static char szNotSpec[32];

int OnDetailsInit(WPARAM wParam, LPARAM lParam)
{
    static char szTmp[256];
    OPTIONSDIALOGPAGE odp = {0};

    if ((!IsICQContact((HANDLE)lParam)) && lParam)
        return 0;

    ZeroMemory(&odp,sizeof(odp));
    odp.cbSize = sizeof(odp);
    odp.flags = ODPF_USERINFOTAB;
    odp.hIcon = NULL;
    odp.hInstance = hInst;
    //odp.position = -1900000000;
    odp.pszTitle=ICQ_PROTOCOL_NAME;

    mir_snprintf( szTmp, sizeof( szTmp ), "%s", Translate( "Contact Info" ));
    odp.pszTemplate = MAKEINTRESOURCEA(IDD_INFO_ICQ);
    odp.pszTab=szTmp;
    odp.pfnDlgProc = IcqDlgProc;
    CallService(MS_USERINFO_ADDPAGE,wParam,(LPARAM)&odp);

    mir_snprintf( szTmp, sizeof( szTmp ), "%s", Translate( "Client Info" ));
    odp.pszTemplate = MAKEINTRESOURCEA(IDD_INFO_ICQ_CLIENT);
    odp.pszTab=szTmp;
    odp.pfnDlgProc = IcqDlgProcClient;
    CallService(MS_USERINFO_ADDPAGE,wParam,(LPARAM)&odp);

    //InitChangeDetails(wParam, lParam);
    if (!lParam)
    {
        mir_snprintf( szTmp, sizeof( szTmp ), "%s", Translate( "Details" ));
        odp.pszTemplate = MAKEINTRESOURCEA(IDD_INFO_CHANGEINFO);
        odp.pszTab=szTmp;
        odp.pfnDlgProc = ChangeInfoDlgProc;
        CallService(MS_USERINFO_ADDPAGE,wParam,(LPARAM)&odp);
    }

    return 0;
}

#define HM_DBCHANGE   (WM_USER+200)
#define HM_PROTOACK   (WM_USER+201)
#define TIMERID       1
#define TIMEOUT_IGNORE  2
//#define TIMEOUT_CAPS    3
#define TIMEOUT_IP      4
typedef struct infoflag_s
{
    char  *Descr;
    char  *szSetting;
    DWORD mask;
} infoflag;


struct UserData
{
    HANDLE hContact;
    DWORD dwUin;
    HANDLE hEventDbSettingChange;
    int nTimer;
    HANDLE hProtoAck;
    DWORD dwCookie;
};

/*static infoflag infoflags[] =
{
	{"Hidden",			NULL, WAS_FOUND},
	{"WebAware",		"WebAware", 0},
	{"Auth",			"AddAuth", 0},
	{"UTF",				NULL, CAPF_UTF},
	{"MTN",				NULL, CAPF_TYPING},
	{"Xtraz",			NULL, CAPF_XTRAZ},
	{"ServerRelay",		NULL, CAPF_SRV_RELAY},
	{"AIMFile",			NULL, CAPF_AIM_FILE},
	{"Push2Talk",		NULL, CAPF_PUSH2TALK},
	{"ICQLite",			NULL, CAPF_ICQ_LITE},
	{"RTF",				NULL, CAPF_RTF},
	{"XTrazChat",		NULL, CAPF_XTRAZ_CHAT},
	{"VoiceChat",		NULL, CAPF_VOICE_CHAT},
	{"ICQDevil",		NULL, CAPF_ICQ_DEVIL},
	{"DirectConnect",	NULL, CAPF_DIRECT},
};
*/
static INT_PTR CALLBACK IcqDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{

    struct UserData *dat=(struct UserData*)GetWindowLong(hwndDlg,GWL_USERDATA);

    switch (msg)
    {

    case WM_INITDIALOG:
        ICQTranslateDialog(hwndDlg);
        dat=(struct UserData*)malloc(sizeof(struct UserData));
        SetWindowLong(hwndDlg,GWL_USERDATA,(LONG)dat);

        dat->hEventDbSettingChange=HookEventMessage(ME_DB_CONTACT_SETTINGCHANGED,hwndDlg,HM_DBCHANGE);
        dat->hContact=(HANDLE)lParam;
        dat->nTimer=0;
        dat->dwUin=ICQGetContactSettingDword(dat->hContact, UNIQUEIDSETTING, 0);
        dat->hProtoAck=NULL;
        dat->dwCookie=0;
        if ((HANDLE)lParam == NULL)
        {

            ShowWindow(GetDlgItem(hwndDlg, IDC_FLAGS), SW_HIDE);
            ShowWindow(GetDlgItem(hwndDlg, IDC_FLAGSTEXT), SW_HIDE);
        }
        return TRUE;

    case WM_NOTIFY:
    {
        switch (((LPNMHDR)lParam)->idFrom)
        {

        case 0:
        {
            switch (((LPNMHDR)lParam)->code)
            {

            case PSN_INFOCHANGED:
            {
                HANDLE hContact = dat->hContact;
                WORD flag = SVSF_NORMAL;
                int was_found = 0;
                char szUin[20];
                char szStatus[64];

                mir_snprintf(szUin, 20, "%u", dat->dwUin);

                if (hContact == NULL)
                {
                    char str[MAX_PATH];
                    SetValue(hwndDlg, IDC_SYSTEMUPTIME, NULL, ICQ_PROTOCOL_NAME, "MemberTS", (WORD)((icqOnline)?SVS_TIMESTAMP|SVSF_NORMAL:SVS_TIMESTAMP|SVSF_GRAYED));
                    SetDlgItemTextUtf(hwndDlg, IDC_SUPTIME, ICQTranslateUtfStatic("Member since:", str, MAX_PATH));
                    _snprintf(szStatus, sizeof(szStatus), MirandaStatusToString(gnCurrentStatus));
                    SetDlgItemTextA(hwndDlg, IDC_STATUS, szStatus);
                    //			SetValue(hwndDlg, IDC_VERSION, NULL, gpszICQProtoName, "setVersion", SVS_ICQVERSION);

                }
                else
                {
//					WORD wStatus = ICQGetContactSettingWord(hContact, "Status", 0);
                    char str[256] = {0};
//					int i;

                    SetValue(hwndDlg, IDC_STATUS, hContact, ICQ_PROTOCOL_NAME, "Status", SVS_STATUSID);
                    SetValue(hwndDlg, IDC_ICQ_STATUS, hContact, ICQ_PROTOCOL_NAME, "ICQStatus", SVS_ICQ_STATUSID);

                    ShowWindow(GetDlgItem(hwndDlg,IDC_GETSTATUS),(gbASD)?SW_SHOW:SW_HIDE);
                    //ShowWindow(GetDlgItem(hwndDlg,IDC_GETCAPS),(wStatus != ID_STATUS_OFFLINE)?SW_SHOW:SW_HIDE);
                    ShowWindow(GetDlgItem(hwndDlg,IDC_RETRIEVE),SW_SHOW);
                    ShowWindow(GetDlgItem(hwndDlg,IDC_IGNORECHECK),SW_SHOW);

                    SetValue(hwndDlg, IDC_SYSTEMUPTIME, hContact, ICQ_PROTOCOL_NAME, "MemberTS", (WORD)((icqOnline)?SVS_TIMESTAMP|SVSF_NORMAL:SVS_TIMESTAMP|SVSF_GRAYED));
                    SetDlgItemTextUtf(hwndDlg, IDC_SUPTIME, ICQTranslateUtfStatic("Member since:", str, MAX_PATH));
                    /*
                    					if (ICQGetContactSettingByte(hContact, "ClientID", 0) == 1)
                    						SetValue(hwndDlg, IDC_SYSTEMUPTIME, hContact, NULL, "TickTS", SVS_TIMESTAMP);
                    					else
                    						SetValue(hwndDlg, IDC_SYSTEMUPTIME, hContact, gpszICQProtoName, "TickTS", SVS_TIMESTAMP);
                    */
                    // Subnets
                    //DBGetStaticString(hContact, gpszICQProtoName, "SubNets", str, sizeof(str));

                }

                //SetValue(hwndDlg, IDC_UIN, hContact, gpszICQProtoName, UNIQUEIDSETTING, 0);
                SetDlgItemTextA(hwndDlg, IDC_UIN, szUin);

                // Client version
                if (hContact == NULL)
                    SetValue(hwndDlg, IDC_MIRVER, hContact, ICQ_PROTOCOL_NAME, "MirVer", (WORD)(SVS_ZEROISUNSPEC|flag));
                else
                    SetValue(hwndDlg, IDC_MIRVER, NULL, ICQ_PROTOCOL_NAME, "MirVer", (WORD)(SVS_ZEROISUNSPEC|flag));

                // Proto version.
//				if (!SetValue(hwndDlg, IDC_VERSION, hContact, gpszICQProtoName, "Version", SVS_ZEROISUNSPEC|SVSF_NORMAL))
//					SetValue(hwndDlg, IDC_VERSION, hContact, gpszICQProtoName, "OldVersion", SVS_ZEROISUNSPEC|SVSF_GRAYED);
                // IP
                if (!SetValue(hwndDlg, IDC_IP, hContact, ICQ_PROTOCOL_NAME, "IP", SVS_IP|SVSF_NORMAL))
                    SetValue(hwndDlg, IDC_IP, hContact, ICQ_PROTOCOL_NAME, "OldIP", SVS_IP|SVSF_GRAYED);
                // Real IP
                if (!SetValue(hwndDlg, IDC_REALIP, hContact, ICQ_PROTOCOL_NAME, "RealIP", SVS_IP|SVSF_NORMAL))
                    SetValue(hwndDlg, IDC_REALIP, hContact, ICQ_PROTOCOL_NAME, "OldRealIP", SVS_IP|SVSF_GRAYED);
                // Port
                if (!SetValue(hwndDlg, IDC_PORT, hContact, ICQ_PROTOCOL_NAME, "UserPort", SVS_ZEROISUNSPEC|SVSF_NORMAL))
                    SetValue(hwndDlg, IDC_PORT, hContact, ICQ_PROTOCOL_NAME, "OldUserPort", SVS_ZEROISUNSPEC|SVSF_GRAYED);
                // Online since time
                if (!SetValue(hwndDlg, IDC_ONLINESINCE, hContact, ICQ_PROTOCOL_NAME, "LogonTS", SVS_TIMESTAMP|SVSF_NORMAL))
                    SetValue(hwndDlg, IDC_ONLINESINCE, hContact, ICQ_PROTOCOL_NAME, "OldLogonTS", SVS_TIMESTAMP|SVSF_GRAYED);
                // Idle time
                if (was_found)
                    SetValue(hwndDlg, IDC_IDLETIME, hContact, NULL, NULL, 0);
                else if (!SetValue(hwndDlg, IDC_IDLETIME, hContact, ICQ_PROTOCOL_NAME, "IdleTS", SVS_TIMESTAMP|SVSF_NORMAL))
                    SetValue(hwndDlg, IDC_IDLETIME, hContact, ICQ_PROTOCOL_NAME, "OldIdleTS", SVS_TIMESTAMP|SVSF_GRAYED);

            }
            break;
            }
        }
        break;
        }
    }
    break;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDCANCEL:
            SendMessage(GetParent(hwndDlg),msg,wParam,lParam);
            break;

        case IDC_GETSTATUS:
            if (icqOnline && (!invis_for(0,dat->hContact)))
                icq_GetUserStatus(dat->hContact, 1);
            break;

        case IDC_RETRIEVE:
            if (icqOnline && dat->dwUin &&(!invis_for(0,dat->hContact))) //not on invisible status, we don't want to be detected
            {
                int wMessageType = 0;
                switch(ICQGetContactSettingWord(dat->hContact, "Status",ID_STATUS_OFFLINE))
                {
                case ID_STATUS_OFFLINE:
                    break;
                case ID_STATUS_NA:
                    wMessageType = MTYPE_AUTONA;
                    break;
                case ID_STATUS_OCCUPIED:
                    wMessageType = MTYPE_AUTOBUSY;
                    break;
                case ID_STATUS_DND:
                    wMessageType = MTYPE_AUTODND;
                    break;
                case ID_STATUS_FREECHAT:
                    wMessageType = MTYPE_AUTOFFC;
                    break;
                case ID_STATUS_AWAY:
                default:
                    wMessageType = MTYPE_AUTOAWAY;
                    break;
                }
                icq_sendGetAwayMsgServ(NULL, dat->dwUin, wMessageType, (WORD)(ICQGetContactSettingWord(dat->hContact, "Version", 0)==9?9:ICQ_VERSION));
                EnableWindow(GetDlgItem(hwndDlg, IDC_RETRIEVE), FALSE);
                SetTimer(hwndDlg, TIMEOUT_IP, 10000, NULL);
            }
            break;
        case IDC_IGNORECHECK:
            if ((icqOnline &&ICQGetContactSettingWord(dat->hContact,  "Status", 0) != ID_STATUS_OFFLINE) && (!invis_for(0,dat->hContact)))   //we do not want to be detected

            {
                message_cookie_data* pCookieData;

                if (!dat->hProtoAck)
                    dat->hProtoAck = HookEventMessage(ME_PROTO_ACK, hwndDlg, HM_PROTOACK);

                SetDlgItemTextA(hwndDlg, IDC_IGNORECHECK, Translate("Checking..."));

                pCookieData = (message_cookie_data*)malloc(sizeof(message_cookie_data));
                pCookieData->bMessageType = MTYPE_PLAIN;
                pCookieData->nAckType = ACKTYPE_SERVER;
                dat->dwCookie = icq_SendChannel4Message(dat->dwUin, NULL,0 ,0, "", pCookieData);

                EnableWindow(GetDlgItem(hwndDlg, IDC_IGNORECHECK), FALSE);
                SetTimer(hwndDlg, TIMEOUT_IGNORE, 10000, NULL);
            }
            break;
        }
        break;
    case HM_DBCHANGE:
    {
        DBCONTACTWRITESETTING *cws=(DBCONTACTWRITESETTING*)lParam;

        if (!dat->nTimer &&
                (cws->szModule != NULL) &&
                (dat->hContact == (HANDLE)wParam) &&
                !strcmp(cws->szModule, ICQ_PROTOCOL_NAME))
        {
            dat->nTimer = 1;
            SetTimer(hwndDlg, TIMERID, 1000, NULL);
        }
    }
    break;

    case HM_PROTOACK:
    {
        ACKDATA *ack = (ACKDATA*)lParam;

        if (!ack || !ack->szModule ||
                ack->hProcess != (HANDLE)dat->dwCookie ||
                ack->hContact != dat->hContact ||
                strcmp(ack->szModule, ICQ_PROTOCOL_NAME))
            break;

        if (dat->hProtoAck)
        {
            UnhookEvent(dat->hProtoAck);
            dat->hProtoAck = NULL;
        }

        if (ack->result == ACKRESULT_SUCCESS)
        {
            SetDlgItemTextA(hwndDlg, IDC_IGNORECHECK, Translate("Msg Accepted!"));
            Netlib_Logf(ghServerNetlibUser, "User %u accepts your messages", dat->dwUin);
        }
        else if (ack->result == ACKRESULT_FAILED)
        {
            SetDlgItemTextA(hwndDlg, IDC_IGNORECHECK, Translate("Msg Ignored!"));
            Netlib_Logf(ghServerNetlibUser, "User %u ignores your messages", dat->dwUin);
        }

    }
    break;

    case WM_TIMER:
        if (wParam==TIMERID)
        {
            PSHNOTIFY pshn;
            pshn.hdr.code=PSN_INFOCHANGED;
            pshn.hdr.hwndFrom=hwndDlg;
            pshn.hdr.idFrom=0;
            pshn.lParam=(LPARAM)wParam;

            KillTimer(hwndDlg, TIMERID);
            dat->nTimer = 0;

            SendMessage(hwndDlg,WM_NOTIFY,0,(LPARAM)&pshn);
        }
        else if (wParam==TIMEOUT_IGNORE)
        {
            KillTimer(hwndDlg, TIMEOUT_IGNORE);
            EnableWindow(GetDlgItem(hwndDlg, IDC_IGNORECHECK), TRUE);
            SetDlgItemTextA(hwndDlg, IDC_IGNORECHECK, Translate("Ignore Check"));//set back
        }
        else if (wParam==TIMEOUT_IP)
        {
            KillTimer(hwndDlg, TIMEOUT_IP);
            EnableWindow(GetDlgItem(hwndDlg, IDC_RETRIEVE), TRUE);
        }
        break;

    case WM_DESTROY:
        if (dat->hEventDbSettingChange)
            UnhookEvent(dat->hEventDbSettingChange);
        if (dat->nTimer)
            KillTimer(hwndDlg, TIMERID);
        if (dat->hProtoAck)
            UnhookEvent(dat->hProtoAck);
        free(dat);
        break;
    }

    return FALSE;
}

static INT_PTR CALLBACK IcqDlgProcClient(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{

    struct UserData *dat=(struct UserData*)GetWindowLong(hwndDlg,GWL_USERDATA);

    switch (msg)
    {

    case WM_INITDIALOG:
        ICQTranslateDialog(hwndDlg);
        dat=(struct UserData*)malloc(sizeof(struct UserData));	// FIXME: memory leak
        SetWindowLong(hwndDlg,GWL_USERDATA,(LONG)dat);

        dat->hEventDbSettingChange=HookEventMessage(ME_DB_CONTACT_SETTINGCHANGED,hwndDlg,HM_DBCHANGE);
        dat->hContact=(HANDLE)lParam;
        dat->nTimer=0;
        dat->dwUin=ICQGetContactSettingDword(dat->hContact, UNIQUEIDSETTING, 0);
        dat->hProtoAck=NULL;
        dat->dwCookie=0;
        return TRUE;

    case WM_MEASUREITEM:
    {
        LPMEASUREITEMSTRUCT lpmis;
        lpmis = (LPMEASUREITEMSTRUCT) lParam;
        if (lpmis->itemID == -1)
            return FALSE;
        lpmis->itemHeight = 18;
        return TRUE;
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
        HWND hwndList;
        ICQ_CAPINFO *info;

        if (lpdis->itemID == -1)
            return FALSE;
        hwndList = GetDlgItem(hwndDlg, lpdis->CtlID);
        info = (ICQ_CAPINFO *)ListBox_GetItemData(hwndList, lpdis->itemID);
        if (info)
        {
            if (lpdis->itemState&ODS_SELECTED)
            {
                HBRUSH hbr = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
                FillRect(lpdis->hDC, &lpdis->rcItem, hbr);
                SetTextColor(lpdis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
                DeleteObject(hbr);
            }
            else
            {
                HBRUSH hbr = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
                FillRect(lpdis->hDC, &lpdis->rcItem, hbr);
                SetTextColor(lpdis->hDC, GetSysColor(COLOR_WINDOWTEXT));
                DeleteObject(hbr);
            }

            {
                SIZE sz;
                SetBkMode(lpdis->hDC, TRANSPARENT);
                GetTextExtentPoint32A(lpdis->hDC, info->name, lstrlenA(info->name), &sz);
                TextOutA(lpdis->hDC, lpdis->rcItem.left+3+16+1,
                         (lpdis->rcItem.top+lpdis->rcItem.bottom-sz.cy)/2,
                         info->name, lstrlenA(info->name));
            }
        }
        else
        {
            char str[256];
            HBRUSH hbr = CreateSolidBrush(RGB(0x60, 0x60, 0x60));
            FillRect(lpdis->hDC, &lpdis->rcItem, hbr);
            SetTextColor(lpdis->hDC, RGB(0xff, 0xff, 0xff));
            DeleteObject(hbr);

            ListBox_GetText(hwndList, lpdis->itemID, str);
            if (!lstrcmpA(str, "01."))
            {
                lstrcpyA(str, Translate("Universal Protocol Features"));
            }
            else if (!lstrcmpA(str, "02."))
            {
                lstrcpyA(str, Translate("Miranda Features"));
            }
            else if (!lstrcmpA(str, "03."))
            {
                lstrcpyA(str, Translate("Miranda Features (unknown)"));
            }
            else if (!lstrcmpA(str, "04."))
            {
                lstrcpyA(str, Translate("Unknown Flags (decoded)"));
            }
            else if (!lstrcmpA(str, "05."))
            {
                lstrcpyA(str, Translate("Unknown Flags (other)"));
            }
            else
            {
                if (lpdis->itemState&ODS_SELECTED)
                {
                    HBRUSH hbr = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
                    FillRect(lpdis->hDC, &lpdis->rcItem, hbr);
                    SetTextColor(lpdis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
                    DeleteObject(hbr);
                }
                else
                {
                    HBRUSH hbr = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
                    FillRect(lpdis->hDC, &lpdis->rcItem, hbr);
                    SetTextColor(lpdis->hDC, GetSysColor(COLOR_WINDOWTEXT));
                    DeleteObject(hbr);
                }
            }

            {
                SIZE sz;
                SetBkMode(lpdis->hDC, TRANSPARENT);
                GetTextExtentPoint32A(lpdis->hDC, str, lstrlenA(str), &sz);
                TextOutA(lpdis->hDC, lpdis->rcItem.left+3,
                         (lpdis->rcItem.top+lpdis->rcItem.bottom-sz.cy)/2,
                         str, lstrlenA(str));
            }
        }
        return TRUE;
    }
    case WM_DELETEITEM:
    {
        DELETEITEMSTRUCT *lpdis = (DELETEITEMSTRUCT *)lParam;
        ICQ_CAPINFO *info = (ICQ_CAPINFO *)ListBox_GetItemData(lpdis->hwndItem, lpdis->itemID);
        if (info) free(info);
        return TRUE;
    }

    case WM_VKEYTOITEM:
    {
        char ch = (char)LOWORD(wParam);
        if (((ch == 'C') || (ch == VK_INSERT)) && (GetKeyState(VK_CONTROL)&0x80))
        {
            int i, count = ListBox_GetCount((HWND)lParam);
            int bufsize = 1; // for the \0
            char *buf, *bufptr;
            HGLOBAL hMem;
            for (i = 0; i < count; ++i)
            {
                ICQ_CAPINFO *info;
                if (!ListBox_GetSel((HWND)lParam, i)) continue;
                info = (ICQ_CAPINFO *)ListBox_GetItemData((HWND)lParam, i);
                if (!info) continue;
                bufsize += lstrlenA(info->caps) + 5 + lstrlenA(info->name) + 2;
            }
            hMem = GlobalAlloc(GMEM_MOVEABLE, bufsize);
            bufptr = buf = (char *)GlobalLock(hMem);
            for (i = 0; i < count; ++i)
            {
                ICQ_CAPINFO *info;
                if (!ListBox_GetSel((HWND)lParam, i)) continue;
                info = (ICQ_CAPINFO *)ListBox_GetItemData((HWND)lParam, i);
                if (!info) continue;
                bufptr += mir_snprintf(bufptr, bufsize-(bufptr-buf), "%s, // %s\r\n", info->caps, info->name);
            }
            GlobalUnlock(hMem);
            OpenClipboard(hwndDlg);
            EmptyClipboard();
            SetClipboardData(CF_TEXT, hMem);
            CloseClipboard();
            return (BOOL)(-2);
        }
        else if ((ch == 'A') && (GetKeyState(VK_CONTROL)&0x80))
        {
            SendMessage((HWND)lParam, LB_SELITEMRANGE, TRUE, MAKELPARAM(0, ListBox_GetCount((HWND)lParam)));
            return (BOOL)(-2);
        }
        return (BOOL)(-1);
    }

    case WM_NOTIFY:
    {
        switch (((LPNMHDR)lParam)->idFrom)
        {

        case 0:
        {
            switch (((LPNMHDR)lParam)->code)
            {

            case PSN_INFOCHANGED:
            {
                HANDLE hContact = dat->hContact;
                WORD flag = SVSF_NORMAL;
                int was_found = 0;
//				int i;
                DBVARIANT dbv;
                DBCONTACTGETSETTING dbcgs;
                dbcgs.szModule = ICQ_PROTOCOL_NAME;
                dbcgs.szSetting = "CapBuf";
                dbcgs.pValue = &dbv;
                CallService(MS_DB_CONTACT_GETSETTING, (WPARAM)hContact, (LPARAM)&dbcgs);

                // Set flags
                SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_RESETCONTENT, 0, 0);

                if ((dbv.type == DBVT_BLOB) && (dbv.cpbVal))
                {
                    int bufsize = dbv.cpbVal;
                    int capsize = 0x10;
                    BYTE *buf = dbv.pbVal;
                    SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_SETITEMDATA,
                                       SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_ADDSTRING, 0, (LPARAM)"01."), 0);
                    SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_SETITEMDATA,
                                       SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_ADDSTRING, 0, (LPARAM)"02."), 0);
                    SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_SETITEMDATA,
                                       SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_ADDSTRING, 0, (LPARAM)"03."), 0);
                    SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_SETITEMDATA,
                                       SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_ADDSTRING, 0, (LPARAM)"04."), 0);
                    SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_SETITEMDATA,
                                       SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_ADDSTRING, 0, (LPARAM)"05."), 0);
                    while (bufsize>0) // search the buffer for a capability
                    {
                        ICQ_CAPINFO *info = (ICQ_CAPINFO *)malloc(sizeof(ICQ_CAPINFO));
                        char *capName = GetCapabilityName(buf, info);
                        SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_SETITEMDATA,
                                           SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_ADDSTRING, 0, (LPARAM)capName), (LPARAM)info);
                        buf += 0x10;
                        bufsize -= 0x10;
                    }
                }/* else
				{
					for (i=0; i < sizeof(infoflags)/sizeof(infoflags[0]); i++)
					{
						if ((infoflags[i].szSetting && ICQGetContactSettingByte(hContact, infoflags[i].szSetting,0))
							|| (infoflags[i].szSetting == NULL && CheckContactCapabilities(hContact, infoflags[i].mask)))
						{
							SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_SETITEMDATA,
								SendDlgItemMessage(hwndDlg, IDC_CLIENTCAPS, LB_ADDSTRING, 0, (LPARAM)Translate(infoflags[i].Descr)), 0);
						}
					}
				}*/
                CallService(MS_DB_CONTACT_FREEVARIANT, 0, (LPARAM)&dbv);

                // Gray flags if miranda offline
//				SetWindowLong(GetDlgItem(hwndDlg, IDC_FLAGSTEXT), GWL_USERDATA, (!icqOnline && str[0] != 0));

                // Client version
                SetValue(hwndDlg, IDC_MIRVER, hContact, ICQ_PROTOCOL_NAME, "MirVer", (WORD)(SVS_ZEROISUNSPEC|flag));

                if (ServiceExists(MS_FP_GETCLIENTICON))
                {
//					DBVARIANT dbv;
//					DBCONTACTGETSETTING dbcgs;
                    dbcgs.szModule = ICQ_PROTOCOL_NAME;
                    dbcgs.pValue = &dbv;
                    dbcgs.szSetting = "MirVer";
                    CallService(MS_DB_CONTACT_GETSETTING, (WPARAM)hContact, (LPARAM)&dbcgs);
                    if (dbv.type == DBVT_ASCIIZ)
                    {
                        SendDlgItemMessage(hwndDlg, IDC_CLIENTICON, STM_SETICON,
                                           (WPARAM)CallService(MS_FP_GETCLIENTICON, (WPARAM)dbv.pszVal, 1), 0);
                    }
                    else
                    {
                        SendDlgItemMessage(hwndDlg, IDC_CLIENTICON, STM_SETICON,
                                           (WPARAM)CallService(MS_FP_GETCLIENTICON, (WPARAM)"[Undetected client]", 1), 0);
                    }
                    CallService(MS_DB_CONTACT_FREEVARIANT, 0, (LPARAM)&dbv);
                }




                // Proto version.
                if (hContact != NULL)
                {
                    if (!SetValue(hwndDlg, IDC_VERSION, hContact, ICQ_PROTOCOL_NAME, "Version", SVS_ZEROISUNSPEC|SVSF_NORMAL))
                        SetValue(hwndDlg, IDC_VERSION, hContact, ICQ_PROTOCOL_NAME, "OldVersion", SVS_ZEROISUNSPEC|SVSF_GRAYED);
                }
                else
                    SetValue(hwndDlg, IDC_VERSION, NULL, ICQ_PROTOCOL_NAME, "setVersion", SVS_ZEROISUNSPEC|SVSF_NORMAL);
            }
            break;
            }
        }
        break;
        }
    }
    break;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDCANCEL:
            SendMessage(GetParent(hwndDlg),msg,wParam,lParam);
            break;
        }
        break;

    case HM_DBCHANGE:
    {
        DBCONTACTWRITESETTING *cws=(DBCONTACTWRITESETTING*)lParam;

        if (!dat->nTimer &&
                (cws->szModule != NULL) &&
                (dat->hContact == (HANDLE)wParam) &&
                !strcmp(cws->szModule, ICQ_PROTOCOL_NAME))
        {
            dat->nTimer = 1;
            SetTimer(hwndDlg, TIMERID, 1000, NULL);
        }
    }
    break;

    case WM_TIMER:
        if (wParam==TIMERID)
        {
            PSHNOTIFY pshn;
            pshn.hdr.code=PSN_INFOCHANGED;
            pshn.hdr.hwndFrom=hwndDlg;
            pshn.hdr.idFrom=0;
            pshn.lParam=(LPARAM)wParam;

            KillTimer(hwndDlg, TIMERID);
            dat->nTimer = 0;

            SendMessage(hwndDlg,WM_NOTIFY,0,(LPARAM)&pshn);
        }
        break;

    case WM_DESTROY:
        if (dat->hEventDbSettingChange)
            UnhookEvent(dat->hEventDbSettingChange);
        if (dat->nTimer)
            KillTimer(hwndDlg, TIMERID);
        free(dat);
        break;
    }

    return FALSE;
}




















static BYTE SetValue(HWND hwndDlg, int idCtrl, HANDLE hContact, char* szModule, char* szSetting, WORD type)
{
    DBVARIANT dbv = {0};
    char str[MAX_PATH];
    char* pstr = NULL;
    int unspecified = 0;
    int special = type & SVS_FLAGS;
    int bUtf = 0, bDbv = 0, bAlloc = 0;

    dbv.type = DBVT_DELETED;

    if ((hContact == NULL) && ((int)szModule<0x100))
    {
        dbv.type = (BYTE)szModule;

        switch((int)szModule)
        {
        case DBVT_BYTE:
            dbv.cVal = (BYTE)szSetting;
            break;
        case DBVT_WORD:
            dbv.wVal = (WORD)szSetting;
            break;
        case DBVT_DWORD:
            dbv.dVal = (DWORD)szSetting;
            break;
        case DBVT_ASCIIZ:
            dbv.pszVal = pstr = szSetting;
            break;
        default:
            unspecified = 1;
            dbv.type = DBVT_DELETED;
        }
    }
    else
    {
        if (szModule == NULL)
            unspecified = 1;
        else
        {
            unspecified = DBGetContactSetting(hContact, szModule, szSetting, &dbv);
            bDbv = 1;
        }
    }

    if (!unspecified)
    {
        switch (dbv.type)
        {

        case DBVT_BYTE:
            unspecified = (special == SVS_ZEROISUNSPEC && dbv.bVal == 0);
            pstr = itoa(special == SVS_SIGNED ? dbv.cVal:dbv.bVal, str, 10);
            break;

        case DBVT_WORD:
            if (special == SVS_ICQVERSION)
            {
                if (dbv.wVal != 0)
                {
                    char szExtra[80];

                    null_snprintf(str, 250, "%d", dbv.wVal);
                    pstr = str;

                    if (hContact && IsDirectConnectionOpen(hContact, DIRECTCONN_STANDARD, 1))
                    {
                        ICQTranslateUtfStatic(" (DC Established)", szExtra, 80);
                        strcat(str, szExtra);
                        bUtf = 1;
                    }
                }
                else
                    unspecified = 1;
            }
            else if (special == SVS_STATUSID)
            {
                char* pXName;
                char* pszStatus;
                BYTE bXStatus = ICQGetContactXStatus(hContact);

                pszStatus = MirandaStatusToStringUtf(dbv.wVal);
                if (bXStatus)
                {
                    pXName = ICQGetContactSettingUtf(hContact, DBSETTING_XSTATUSNAME, "");
                    if (!strlennull(pXName))
                    {
                        // give default name
                        pXName = ICQTranslateUtf(nameXStatus[bXStatus-1]);
                    }
                    null_snprintf(str, sizeof(str), "%s (%s)", pszStatus, pXName);
                    mir_free(pXName);
                }
                else
                    null_snprintf(str, sizeof(str), pszStatus);

                bUtf = 1;
                mir_free(pszStatus);
                pstr = str;
                unspecified = 0;
            }
            else if (special == SVS_ICQ_STATUSID)
            {
                switch (dbv.wVal)
                {
                case ICQ_STATUS_DEPRESS:
                    pstr = Translate("Depress");
                    break;
                case ICQ_STATUS_EVIL:
                    pstr = Translate("Evil");
                    break;
                case ICQ_STATUS_LUNCH:
                    pstr = Translate("Lunch");
                    break;
                case ICQ_STATUS_WORK:
                    pstr = Translate("Work");
                    break;
                case ICQ_STATUS_HOME:
                    pstr = Translate("Home");
                    break;
                default:
                    pstr = Translate("none");
                }
            }
            else
            {
                unspecified = (special == SVS_ZEROISUNSPEC && dbv.wVal == 0);
                pstr = itoa(special == SVS_SIGNED ? dbv.sVal:dbv.wVal, str, 10);
            }
            break;

        case DBVT_DWORD:
            unspecified = (special == SVS_ZEROISUNSPEC && dbv.dVal == 0);
            if (special == SVS_IP)
            {
                struct in_addr ia;
                ia.S_un.S_addr = htonl(dbv.dVal);
                pstr = inet_ntoa(ia);
                if (dbv.dVal == 0)
                    unspecified=1;
            }
            else if (special == SVS_TIMESTAMP)
            {
                if (dbv.dVal == 0)
                    unspecified = 1;
                else
                {
                    pstr = time2text(dbv.dVal);
                }
            }
            else
                pstr = itoa(special == SVS_SIGNED ? dbv.lVal:dbv.dVal, str, 10);
            break;

        case DBVT_ASCIIZ:
        case DBVT_WCHAR:
            unspecified = (special == SVS_ZEROISUNSPEC && dbv.pszVal[0] == '\0');
            if (!unspecified && pstr != szSetting)
            {
                pstr = UniGetContactSettingUtf(hContact, szModule, szSetting, NULL);
                bUtf = 1;
                bAlloc = 1;
            }
            if (idCtrl == IDC_UIN)
                SetDlgItemTextUtf(hwndDlg, IDC_UINSTATIC, ICQTranslateUtfStatic("ScreenName:", str, MAX_PATH));
            break;

        default:
            pstr = str;
            strcpy(str,"???");
            break;
        }
    }

    EnableDlgItem(hwndDlg, idCtrl, !unspecified);
    if (unspecified)
        SetDlgItemTextUtf(hwndDlg, idCtrl, ICQTranslateUtfStatic("<not specified>", str, MAX_PATH));
    else if (bUtf)
        SetDlgItemTextUtf(hwndDlg, idCtrl, pstr);
    else
        SetDlgItemTextA(hwndDlg, idCtrl, pstr);

    if (bDbv)
        ICQFreeVariant(&dbv);

    if (bAlloc)
        mir_free(pstr);

    if (type > SVS_FLAGS)
    {
        EnableWindow(GetDlgItem(hwndDlg, idCtrl), !(type & SVSF_GRAYED));
    }
    return !unspecified;
}
