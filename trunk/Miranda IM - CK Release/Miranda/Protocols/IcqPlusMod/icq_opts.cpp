// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright � 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright � 2001,2002 Jon Keating, Richard Hughes
// Copyright � 2002,2003,2004 Martin �berg, Sam Kothari, Robert Rainwater
// Copyright � 2004,2005,2006,2007 Joe Kucera
// Copyright � 2006,2007,2008 [sss], chaos.persei, [sin], Faith Healer, Thief, nullbie
// Copyright � 2008 [sss], chaos.persei, nullbie, baloo, jarvis
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
// Revision       : $Revision: 64 $
// Last change on : $Date: 2007-10-16 17:39:11 +0300 (Вт, 16 окт 2007) $
// Last change by : $Author: chaos.persei $
//
// DESCRIPTION:
//
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"

#include <win2k.h>
//#include <uxtheme.h>

#ifndef TTM_TRACKACTIVATE
#define TTM_TRACKACTIVATE       (WM_USER + 17)  // wParam = TRUE/FALSE start end  lparam = LPTOOLINFO
#endif
#define TTS_BALLOON             0x40
#define TTF_TRACK               0x0020
#define TTF_TRANSPARENT         0x0100
#ifndef TTM_TRACKPOSITION
#define TTM_TRACKPOSITION       (WM_USER + 18)  // lParam = dwPos
#endif
#ifndef TTM_SETTITLEA
#define TTM_SETTITLEA           (WM_USER + 32)  // wParam = TTI_*, lParam = char* szTitle
#endif

#ifdef _WIN64
#define GWL_WNDPROC         (-4)
#define GWL_HINSTANCE       (-6)
#define GWL_HWNDPARENT      (-8)
#define GWL_STYLE           (-16)
#define GWL_EXSTYLE         (-20)
#define GWL_USERDATA        (-21)
#define GWL_ID              (-12)
#endif



static INT_PTR CALLBACK DlgProcIcqOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR CALLBACK DlgProcIcqContactsOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR CALLBACK DlgProcIcqFeaturesOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR CALLBACK DlgProcIcqFeatures2Opts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR CALLBACK DlgProcIcqPrivacyOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR CALLBACK DlgProcIcqEventLogOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR CALLBACK DlgProcIcqClientIDOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR CALLBACK DlgProcIcqNewUINOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR CALLBACK DlgProcIcqPopupOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR CALLBACK DlgProcIcqPopupOpts2(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR CALLBACK DlgProcIcqASDOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

extern WORD wCurrentQIPStatus;

extern BOOL bXstatusIconShow;

extern BOOL bQipstatusIconShow;//added
extern void ShowSrvListDialog(HWND hwndCaller);
extern void ShowCapsListDialog( HWND hwndCaller );
extern void setUserInfo();

static const char* szLogLevelDescr[] = {"Display all problems", "Display problems causing possible loss of data", "Display explanations for disconnection", "Display problems requiring user intervention", "Do not display problems"};

static BOOL (WINAPI *pfnEnableThemeDialogTexture)(HANDLE, DWORD) = 0;

static HWND hwndRegUin = NULL;
static HWND hwndPopUpsOpts = NULL;
static HWND hwndASDOpts = NULL;

extern char szHttpUserAgent[255];

int IcqOptInit(WPARAM wParam,LPARAM lParam)
{
    OPTIONSDIALOGPAGE odp = { 0 };


    odp.cbSize = sizeof(odp);
//	odp.position = 100000000;
    odp.hInstance = hInst;
    odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_ICQ);
    odp.pszTitle = ICQ_PROTOCOL_NAME;
    odp.pszGroup = LPGEN("Network");
    odp.pszTab = LPGEN("Account");
//	odp.groupPosition = 910000000;
    odp.flags=ODPF_BOLDGROUPS;
    odp.pfnDlgProc = DlgProcIcqOpts;
    CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);


    ZeroMemory(&odp,sizeof(odp));
    odp.cbSize = sizeof(odp);
    odp.hInstance = hInst;
    odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_ICQCONTACTS);
    odp.pszTitle = ICQ_PROTOCOL_NAME;
    odp.pszGroup = LPGEN("Network");
    odp.pszTab = LPGEN("Contacts");
    odp.flags=ODPF_BOLDGROUPS;
    odp.pfnDlgProc = DlgProcIcqContactsOpts;
    CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);


    ZeroMemory(&odp,sizeof(odp));
    odp.cbSize = sizeof(odp);
//	odp.position = 100000000;
    odp.hInstance = hInst;
    odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_ICQFEATURES);
    odp.pszTitle = ICQ_PROTOCOL_NAME;
    odp.pszGroup = LPGEN("Network");
    odp.pszTab = LPGEN("Main Features");
//	odp.groupPosition = 910000000;
    odp.flags=ODPF_BOLDGROUPS | ODPF_EXPERTONLY;
    odp.pfnDlgProc = DlgProcIcqFeaturesOpts;
    CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);


    ZeroMemory(&odp,sizeof(odp));
    odp.cbSize = sizeof(odp);
//	odp.position = 100000000;
    odp.hInstance = hInst;
    odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_ICQFEATURES2);
    odp.pszTitle = ICQ_PROTOCOL_NAME;
    odp.pszGroup = LPGEN("Network");
    odp.pszTab = LPGEN("Advanced Features");
//	odp.groupPosition = 910000000;
    odp.flags=ODPF_BOLDGROUPS | ODPF_EXPERTONLY;
    odp.pfnDlgProc = DlgProcIcqFeatures2Opts;
    CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);


    ZeroMemory(&odp,sizeof(odp));
    odp.cbSize = sizeof(odp);
    odp.hInstance = hInst;
    odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_ICQPRIVACY);
    odp.pszTitle = ICQ_PROTOCOL_NAME;
    odp.pszGroup = LPGEN("Network");
    odp.pszTab = LPGEN("Privacy");
    odp.flags=ODPF_BOLDGROUPS;
    odp.pfnDlgProc = DlgProcIcqPrivacyOpts;
    CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);


    ZeroMemory(&odp,sizeof(odp));
    odp.cbSize = sizeof(odp);
    odp.hInstance = hInst;
    odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_ICQ_EVENTS);
    odp.pszTitle = ICQ_PROTOCOL_NAME;
    odp.pszGroup = LPGEN("Network");
    odp.pszTab = LPGEN("Log");
    odp.flags=ODPF_BOLDGROUPS;
    odp.pfnDlgProc = DlgProcIcqEventLogOpts;
    CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);


    ZeroMemory(&odp,sizeof(odp));
    odp.cbSize = sizeof(odp);
    odp.hInstance = hInst;
    odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_ICQ_CLIENT_ID);
    odp.pszTitle = ICQ_PROTOCOL_NAME;
    odp.pszGroup = LPGEN("Network");
    odp.pszTab = LPGEN("Client ID");
    odp.flags=ODPF_BOLDGROUPS | ODPF_EXPERTONLY;
    odp.pfnDlgProc = DlgProcIcqClientIDOpts;
    CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);

    if (bPopUpService)
    {
        ZeroMemory(&odp,sizeof(odp));
        odp.cbSize = sizeof(odp);
        odp.hInstance = hInst;
        odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_POPUPS);
        odp.pszTitle = ICQ_PROTOCOL_NAME;
        odp.pszGroup = LPGEN("PopUps");
        odp.flags=ODPF_BOLDGROUPS;
        odp.pfnDlgProc = DlgProcIcqPopupOpts;
        CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);
    }

    return 0;
}


static void LoadDBCheckState(HWND hwndDlg, int idCtrl, const char* szSetting, BYTE bDef)
{
    CheckDlgButton(hwndDlg, idCtrl, getSettingByte(NULL, szSetting, bDef));
}



static void StoreDBCheckState(HWND hwndDlg, int idCtrl, const char* szSetting)
{
    setSettingByte(NULL, szSetting, (BYTE)IsDlgButtonChecked(hwndDlg, idCtrl));
}



static void OptDlgChanged(HWND hwndDlg)
{
    SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
}


// standalone option pages
static void IcqPasswordBox_Init(HWND hwndDlg, int idCtrl);

char HttpUserAgents [][255] =
{
    "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; SV1; .NET CLR 1.1.4322)",
    "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0)",
    "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.1; Trident/4.0; MRA 5.3 (build 02552); SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; Tablet PC 2.0; InfoPath.2; .NET CLR 1.1.4322)",
    "Mozilla/5.0 (Windows; U; Windows NT 5.1; ru; rv:1.8.1.9) Gecko/20071025 Firefox/2.0.0.9",
    "Opera/9.27 (Windows NT 5.1; U; en)",
    "Opera/8.01 (J2ME/MIDP; Opera Mini/3.0.6306/1528; nb; U; ssr)",
    "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.1.2) Gecko/20070221 SeaMonkey/1.1.1",
    "Mozilla/5.0 (compatible; Konqueror/3.5; Linux 2.6.21-rc1; x86_64; cs, en_US) KHTML/3.5.6 (like Gecko)",
    "Lynx/2.8.4rel.1 libwww-FM/2.14",
    "Mozilla/4.08 [en] (WinNT; U ;Nav)"
};

static INT_PTR CALLBACK DlgProcIcqOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static char DBModule[64], buf[64];
    WORD SrvCount;
    int i, iIndex;
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        static char pszPwd[16];
        static char szServer[MAX_PATH];

        ICQTranslateDialog(hwndDlg);

        SetDlgItemInt(hwndDlg, IDC_ICQNUM, getContactUin(NULL), FALSE);

        if (!getSettingStringStatic(NULL, "Password", pszPwd, sizeof(pszPwd)))
        {
            CallService(MS_DB_CRYPT_DECODESTRING, strlennull(pszPwd) + 1, (LPARAM)pszPwd);

            //bit of a security hole here, since it's easy to extract a password from an edit box
            SetDlgItemTextA(hwndDlg, IDC_PASSWORD, pszPwd);
        }

        SendDlgItemMessageA(hwndDlg, IDC_ICQSERVER, CB_RESETCONTENT, 0, 0);
        mir_snprintf(DBModule, 64, "%sSrvs", ICQ_PROTOCOL_NAME);
        SrvCount = DBGetContactSettingWord(0, DBModule, "SrvCount", 0);

        for(i = 1; i <= SrvCount; i++)
        {
            mir_snprintf(buf, 64, "server%luhost", i);
            SendDlgItemMessageA(hwndDlg, IDC_ICQSERVER, CB_ADDSTRING, 0, (LPARAM)getSettingStringUtf(NULL, DBModule, buf, 0));
        }

        SendDlgItemMessageA(hwndDlg, IDC_HTTPUSERAGENT, CB_RESETCONTENT, 0, 0);
        for(i = 0; i < SIZEOF(HttpUserAgents); i++)
        {
            SendDlgItemMessageA(hwndDlg, IDC_HTTPUSERAGENT, CB_ADDSTRING, 0, (LPARAM)HttpUserAgents[i]);
        }

        if (!getSettingStringStatic(NULL, "OscarServer", szServer, MAX_PATH))
        {
            SetDlgItemTextA(hwndDlg, IDC_ICQSERVER, szServer);
        }
        else
        {
            SetDlgItemTextA(hwndDlg, IDC_ICQSERVER, IsDlgButtonChecked(hwndDlg, IDC_SSL) ? DEFAULT_SERVER_HOST_SSL : DEFAULT_SERVER_HOST);
        }
        CheckDlgButton(hwndDlg, IDC_AUTOCHANGE, 0);
        EnableWindow(GetDlgItem(hwndDlg, IDC_AUTOCHANGE), 0); // not used anymore
        EnableWindow(GetDlgItem(hwndDlg, IDC_SERVERBOX), 0); // not used anymore


        SetDlgItemTextA(hwndDlg, IDC_HTTPUSERAGENT, szHttpUserAgent);

        SetDlgItemInt(hwndDlg, IDC_ICQPORT, getSettingWord(NULL, "OscarPort", DEFAULT_SERVER_PORT), FALSE);
        LoadDBCheckState(hwndDlg, IDC_KEEPALIVE, "KeepAlive", 1);
        LoadDBCheckState(hwndDlg, IDC_SECURE, "SecureLogin", DEFAULT_SECURE_LOGIN);
        LoadDBCheckState(hwndDlg, IDC_SSL, "SecureConnection", DEFAULT_SECURE_CONNECTION);
        SendDlgItemMessage(hwndDlg, IDC_LOGLEVEL, TBM_SETRANGE, FALSE, MAKELONG(0, 4));
        SendDlgItemMessage(hwndDlg, IDC_LOGLEVEL, TBM_SETPOS, TRUE, 4-getSettingByte(NULL, "ShowLogLevel", LOG_WARNING));
        SetDlgItemTextUtf(hwndDlg, IDC_LEVELDESCR, ICQTranslateUtfStatic(szLogLevelDescr[4-SendDlgItemMessage(hwndDlg, IDC_LOGLEVEL, TBM_GETPOS, 0, 0)], szServer, MAX_PATH));
        ShowWindow(GetDlgItem(hwndDlg, IDC_RECONNECTREQD), SW_HIDE);
        LoadDBCheckState(hwndDlg, IDC_NOERRMULTI, "IgnoreMultiErrorBox", 0);
        LoadDBCheckState(hwndDlg, IDC_AUTOCHANGE, "ServerAutoChange", 1);

        IcqPasswordBox_Init(hwndDlg, IDC_PASSWORD);

        return TRUE;
    }	// case WM_INITDIALOG

    case WM_HSCROLL:
    {
        char str[MAX_PATH];

        SetDlgItemTextUtf(hwndDlg, IDC_LEVELDESCR, ICQTranslateUtfStatic(szLogLevelDescr[4-SendDlgItemMessage(hwndDlg, IDC_LOGLEVEL,TBM_GETPOS, 0, 0)], str, MAX_PATH));
        OptDlgChanged(hwndDlg);
    }
    break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {

        case IDC_PASSWORD:
            if ((HIWORD(wParam) == EN_CHANGE))
            {
                if (GetWindowTextLength(GetDlgItem(hwndDlg, IDC_PASSWORD)) > 8)
                    SendDlgItemMessage(hwndDlg, IDC_PASSWORD, WM_APP, (WPARAM)Translate("Warning"), (LPARAM)Translate("ICQ servers do not accept passwords longer than 8 characters."));
                else
                    SendDlgItemMessage(hwndDlg, IDC_PASSWORD, WM_APP, 0, 0);
            }
            break;

        case IDC_REGUIN:
            ShowRegUinDialog();
            return TRUE;

        case IDC_LOOKUPLINK:
            CallService(MS_UTILS_OPENURL, 1, (LPARAM)URL_FORGOT_PASSWORD);
            return TRUE;

//      case IDC_SERVERBOX:
//		  ShowSrvListDialog(hwndDlg);
//		  return TRUE;

        case IDC_ICQSERVER:
            switch(HIWORD(wParam))
            {
            case CBN_SELCHANGE:

                iIndex = (int)SendDlgItemMessage(hwndDlg, IDC_ICQSERVER, CB_GETCURSEL, 0, 0);
                if(iIndex < 0) return FALSE;
                mir_snprintf(DBModule, 64, "%sSrvs", ICQ_PROTOCOL_NAME);
                mir_snprintf(buf, 64, "server%luport", iIndex + 1);
                DBWriteContactSettingWord(0, DBModule, "CurrServ", (WORD)(iIndex + 1));
                SetDlgItemInt(hwndDlg, IDC_ICQPORT, DBGetContactSettingWord(0, DBModule, buf, DEFAULT_SERVER_PORT), 0);
                OptDlgChanged(hwndDlg);
                break;
            }
            break;

        case IDC_SSL: //need to write something better here
            SetDlgItemTextA(hwndDlg, IDC_ICQSERVER, IsDlgButtonChecked(hwndDlg, IDC_SSL) ? DEFAULT_SERVER_HOST_SSL : DEFAULT_SERVER_HOST);
            SetDlgItemInt(hwndDlg, IDC_ICQPORT, IsDlgButtonChecked(hwndDlg, IDC_SSL)?DEFAULT_SERVER_PORT_SSL:DEFAULT_SERVER_PORT, 0);
            OptDlgChanged(hwndDlg);
            break;

        case IDC_ICQNUM:
        case IDC_ICQPORT:
            if (HIWORD(wParam) == EN_CHANGE)
                if ((HWND)lParam == GetFocus())
                    OptDlgChanged(hwndDlg);
            break;
        }	// switch (LOWORD(wParam))

        if (icqOnline && LOWORD(wParam) != IDC_NOERRMULTI)
        {
            char szClass[80];

            GetClassNameA((HWND)lParam, szClass, sizeof(szClass));

            if (stricmp(szClass, "EDIT") || HIWORD(wParam) == EN_CHANGE)
                ShowWindow(GetDlgItem(hwndDlg, IDC_RECONNECTREQD), SW_SHOW);
        }
        /*
        if ((LOWORD(wParam)==IDC_ICQNUM || LOWORD(wParam)==IDC_PASSWORD || LOWORD(wParam)==IDC_ICQSERVER || LOWORD(wParam)==IDC_ICQPORT) &&
          (HIWORD(wParam)!=EN_CHANGE || (HWND)lParam!=GetFocus()))
        {
          return 0;
        }
        */

        break;
    }

    case WM_NOTIFY:
    {
        switch (((LPNMHDR)lParam)->code)
        {

        case PSN_APPLY:
        {
            char str[64];

            setSettingDword(NULL, UNIQUEIDSETTING, (DWORD)GetDlgItemInt(hwndDlg, IDC_ICQNUM, NULL, FALSE));
            GetDlgItemTextA(hwndDlg, IDC_PASSWORD, str, 9);
            if (strlennull(str))
            {
                strcpy(gpszPassword, str);
                gbRememberPwd = TRUE;
            }
            else
            {
                gbRememberPwd = getSettingByte(NULL, "RememberPass", 0);
            }
            CallService(MS_DB_CRYPT_ENCODESTRING, sizeof(gpszPassword), (LPARAM)str);
            setSettingString(NULL, "Password", str);
            GetDlgItemTextA(hwndDlg,IDC_ICQSERVER, str, sizeof(str));
            setSettingString(NULL, "OscarServer", str);
            setSettingWord(NULL, "OscarPort", (WORD)GetDlgItemInt(hwndDlg, IDC_ICQPORT, NULL, FALSE));
            StoreDBCheckState(hwndDlg, IDC_KEEPALIVE, "KeepAlive");
            StoreDBCheckState(hwndDlg, IDC_SECURE, "SecureLogin");
            StoreDBCheckState(hwndDlg, IDC_SSL, "SecureConnection");
            m_bSecureConnection = getSettingByte(NULL, "SecureConnection", 0);
            StoreDBCheckState(hwndDlg, IDC_NOERRMULTI, "IgnoreMultiErrorBox");
            StoreDBCheckState(hwndDlg, IDC_AUTOCHANGE, "ServerAutoChange");
            bServerAutoChange = getSettingByte(NULL,"ServerAutoChange",1);
            setSettingByte(NULL, "ShowLogLevel", (BYTE)(4-SendDlgItemMessage(hwndDlg, IDC_LOGLEVEL, TBM_GETPOS, 0, 0)));
            GetDlgItemTextA(hwndDlg, IDC_HTTPUSERAGENT, szHttpUserAgent, sizeof(szHttpUserAgent));
            setSettingString(NULL, "HttpUserAgent", szHttpUserAgent);
            return TRUE;
        }
        }
    }
    break;
    }

    return FALSE;
}



static const UINT icqPrivacyControls[]= {IDC_DCALLOW_ANY, IDC_DCALLOW_CLIST, IDC_DCALLOW_AUTH, IDC_ADD_ANY, IDC_ADD_AUTH, IDC_WEBAWARE, IDC_PUBLISHPRIMARY, IDC_STATIC_DC1, IDC_STATIC_DC2, IDC_STATIC_CLIST};
static INT_PTR CALLBACK DlgProcIcqPrivacyOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {

    case WM_INITDIALOG:
    {
        int nDcType;
        int nAddAuth;
        BYTE bData;

        nDcType = getSettingByte(NULL, "DCType", 0);
        nAddAuth = getSettingByte(NULL, "Auth", 1);

        ICQTranslateDialog(hwndDlg);
        if (!icqOnline)
        {
            icq_EnableMultipleControls(hwndDlg, icqPrivacyControls, sizeof(icqPrivacyControls)/sizeof(icqPrivacyControls[0]), FALSE);
            ShowWindow(GetDlgItem(hwndDlg, IDC_STATIC_NOTONLINE), SW_SHOW);
        }
        else
        {
            ShowWindow(GetDlgItem(hwndDlg, IDC_STATIC_NOTONLINE), SW_HIDE);
        }
        CheckDlgButton(hwndDlg, IDC_DCALLOW_ANY, (nDcType == 0));
        CheckDlgButton(hwndDlg, IDC_DCALLOW_CLIST, (nDcType == 1));
        CheckDlgButton(hwndDlg, IDC_DCALLOW_AUTH, (nDcType == 2));
        CheckDlgButton(hwndDlg, IDC_ADD_ANY, (nAddAuth == 0));
        CheckDlgButton(hwndDlg, IDC_ADD_AUTH, (nAddAuth == 1));
        LoadDBCheckState(hwndDlg, IDC_AUTH_IGNORE, "AuthIgnore", 0);
        LoadDBCheckState(hwndDlg, IDC_WEBAWARE, "WebAware", 0);
        LoadDBCheckState(hwndDlg, IDC_PUBLISHPRIMARY, "PublishPrimaryEmail", 0);
        LoadDBCheckState(hwndDlg, IDC_NOSTATUSREPLY, "NoStatusReply", 0);
        LoadDBCheckState(hwndDlg, IDC_STATUSMSG_CLIST, "StatusMsgReplyCList", 0);
        LoadDBCheckState(hwndDlg, IDC_STATUSMSG_VISIBLE, "StatusMsgReplyVisible", 0);
        if (!getSettingByte(NULL, "StatusMsgReplyCList", 0))
            EnableDlgItem(hwndDlg, IDC_STATUSMSG_VISIBLE, FALSE);
        bData = DBGetContactSettingByte(NULL,ICQ_PROTOCOL_NAME,"NoStatusReply", 0);
        CheckDlgButton(hwndDlg, IDC_NOSTATUSREPLY, bData);

        return TRUE;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_DCALLOW_ANY:
        case IDC_DCALLOW_CLIST:
        case IDC_DCALLOW_AUTH:
        case IDC_ADD_ANY:
        case IDC_ADD_AUTH:
        case IDC_WEBAWARE:
        case IDC_AUTH_IGNORE:
        case IDC_PUBLISHPRIMARY:
        case IDC_NOSTATUSREPLY:
            if (IsDlgButtonChecked(hwndDlg, IDC_NOSTATUSREPLY))
                icq_sendSetAimAwayMsgServ("");//set aim message to nothing
        case IDC_STATUSMSG_VISIBLE:
            if ((HWND)lParam != GetFocus())  return 0;
            break;
        case IDC_STATUSMSG_CLIST:
            if (IsDlgButtonChecked(hwndDlg, IDC_STATUSMSG_CLIST))
            {
                EnableDlgItem(hwndDlg, IDC_STATUSMSG_VISIBLE, TRUE);
                LoadDBCheckState(hwndDlg, IDC_STATUSMSG_VISIBLE, "StatusMsgReplyVisible", 0);
            }
            else
            {
                EnableDlgItem(hwndDlg, IDC_STATUSMSG_VISIBLE, FALSE);
                CheckDlgButton(hwndDlg, IDC_STATUSMSG_VISIBLE, FALSE);
            }
            break;
        default:
            return 0;
        }
        OptDlgChanged(hwndDlg);
        break;

    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case PSN_APPLY:

            StoreDBCheckState(hwndDlg, IDC_WEBAWARE, "WebAware");
            StoreDBCheckState(hwndDlg, IDC_PUBLISHPRIMARY, "PublishPrimaryEmail");
            StoreDBCheckState(hwndDlg, IDC_NOSTATUSREPLY, "NoStatusReply");
            StoreDBCheckState(hwndDlg, IDC_STATUSMSG_CLIST, "StatusMsgReplyCList");
            StoreDBCheckState(hwndDlg, IDC_STATUSMSG_VISIBLE, "StatusMsgReplyVisible");
            if (IsDlgButtonChecked(hwndDlg, IDC_DCALLOW_AUTH))
                setSettingByte(NULL, "DCType", 2);
            else if (IsDlgButtonChecked(hwndDlg, IDC_DCALLOW_CLIST))
                setSettingByte(NULL, "DCType", 1);
            else
                setSettingByte(NULL, "DCType", 0);
            StoreDBCheckState(hwndDlg, IDC_ADD_AUTH, "Auth");
            StoreDBCheckState(hwndDlg, IDC_AUTH_IGNORE, "AuthIgnore");
            bAuthIgnore = getSettingByte(NULL, "AuthIgnore", 0);
            bNoStatusReply = getSettingByte(NULL, "NoStatusReply", 0);
            if (icqOnline)
            {
                PBYTE buf=NULL;
                int buflen=0;

                ppackTLVLNTSBytefromDB(&buf, &buflen, "e-mail", (BYTE)!getSettingByte(NULL, "PublishPrimaryEmail", 0), TLV_EMAIL);
                ppackTLVLNTSBytefromDB(&buf, &buflen, "e-mail0", 0, TLV_EMAIL);
                ppackTLVLNTSBytefromDB(&buf, &buflen, "e-mail1", 0, TLV_EMAIL);

                ppackTLVByte(&buf, &buflen, (BYTE)!getSettingByte(NULL, "Auth", 1), TLV_AUTH, 1);

                ppackTLVByte(&buf, &buflen, (BYTE)getSettingByte(NULL, "WebAware", 0), TLV_WEBAWARE, 1);

                icq_changeUserDirectoryInfoServ(buf, (WORD)buflen, DIRECTORYREQUEST_UPDATEPRIVACY);

                mir_free(buf);

                // Send a status packet to notify the server about the webaware setting
                {
                    WORD wStatus;

                    wStatus = MirandaStatusToIcq(gnCurrentStatus);

                    if (gnCurrentStatus == ID_STATUS_INVISIBLE)
                    {
                        if (gbSsiEnabled)
                            updateServVisibilityCode(3);
                        icq_setstatus(wStatus, FALSE);
                        // Tell who is on our visible list
                        icq_sendEntireVisInvisList(0);
                    }
                    else
                    {
                        icq_setstatus(wStatus, FALSE);
                        if (gbSsiEnabled)
                            updateServVisibilityCode(4);
                        // Tell who is on our invisible list
                        icq_sendEntireVisInvisList(1);
                    }
                }
            }
            return TRUE;
        }
        break;
    }

    return FALSE;
}


static const UINT icqPopupsControls[] = {IDC_POPUPS_LOG_ENABLED,IDC_PREVIEW,IDC_USEWINCOLORS,IDC_USESYSICONS,IDC_POPUP_LOG0_TIMEOUT,IDC_POPUP_LOG1_TIMEOUT,IDC_POPUP_LOG2_TIMEOUT,IDC_POPUP_LOG3_TIMEOUT,IDC_POPUP_LOG4_TIMEOUT,IDC_POPUP_LOG5_TIMEOUT,IDC_POPUP_SPAM_TIMEOUT,IDC_POPUP_UNKNOWN_TIMEOUT,IDC_POPUP_LOG6_TIMEOUT,IDC_POPUP_FOR_NOTONLIST,IDC_POPUP_FOR_IGNORED,IDC_POPUP_FOR_HIDDEN,IDC_UINPOPUP};
static const UINT icqPopupColorControls[] =
{
    IDC_POPUP_LOG0_TEXTCOLOR,IDC_POPUP_LOG1_TEXTCOLOR,IDC_POPUP_LOG2_TEXTCOLOR,IDC_POPUP_LOG3_TEXTCOLOR,IDC_POPUP_LOG4_TEXTCOLOR,IDC_POPUP_LOG5_TEXTCOLOR,IDC_POPUP_SPAM_TEXTCOLOR,IDC_POPUP_UNKNOWN_TEXTCOLOR,IDC_POPUP_LOG6_TEXTCOLOR,IDC_POPUP_CLIENT_CHANGE_TEXTCOLOR,IDC_POPUP_REMOVE_HIMSELF_TEXTCOLOR,IDC_POPUP_INFO_REQUEST_TEXTCOLOR,IDC_POPUP_IGNORECHECK_TEXTCOLOR,IDC_POPUP_AUTH_TEXTCOLOR,
    IDC_POPUP_LOG0_BACKCOLOR,IDC_POPUP_LOG1_BACKCOLOR,IDC_POPUP_LOG2_BACKCOLOR,IDC_POPUP_LOG3_BACKCOLOR,IDC_POPUP_LOG4_BACKCOLOR,IDC_POPUP_LOG5_BACKCOLOR,IDC_POPUP_SPAM_BACKCOLOR,IDC_POPUP_UNKNOWN_BACKCOLOR,IDC_POPUP_LOG6_BACKCOLOR,IDC_POPUP_CLIENT_CHANGE_BACKCOLOR,IDC_POPUP_REMOVE_HIMSELF_BACKCOLOR,IDC_POPUP_INFO_REQUEST_BACKCOLOR,IDC_POPUP_IGNORECHECK_BACKCOLOR,IDC_POPUP_AUTH_BACKCOLOR
};
void ShowPopUpsOpts(void);
static INT_PTR CALLBACK DlgProcIcqPopupOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    BYTE bEnabled;
    switch (msg)
    {
    case WM_INITDIALOG:
        ICQTranslateDialog(hwndDlg);
        LoadDBCheckState(hwndDlg, IDC_POPUP_FOR_NOTONLIST, "PopUpForNotOnList", 0);
        LoadDBCheckState(hwndDlg, IDC_POPUP_FOR_IGNORED, "PopUpForIgnored", 0);
        LoadDBCheckState(hwndDlg, IDC_POPUP_FOR_HIDDEN, "PopUpForHidden", 0);
        LoadDBCheckState(hwndDlg, IDC_POPCLOSEWND, "CloseWindow", 0);
        LoadDBCheckState(hwndDlg, IDC_UINPOPUP, "UinPopup", 0);
        LoadDBCheckState(hwndDlg, IDC_READXSTPOPUP, "ReadXStatusPopUp", 0);
        CheckDlgButton(hwndDlg, IDC_POPUPS_LOG_ENABLED, getSettingByte(NULL,"PopupsLogEnabled",1));
        bEnabled = getSettingByte(NULL,"PopupsWinColors",0);
        CheckDlgButton(hwndDlg, IDC_USEWINCOLORS, bEnabled);
        CheckDlgButton(hwndDlg, IDC_USESYSICONS, getSettingByte(NULL,"PopupsSysIcons",0));
        bEnabled = getSettingByte(NULL,"PopupsEnabled",1);
        CheckDlgButton(hwndDlg, IDC_POPUPS_ENABLED, bEnabled);
        icq_EnableMultipleControls(hwndDlg, icqPopupsControls, SIZEOF(icqPopupsControls), bEnabled);
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_PREVIEW:
        {
            extern BOOL bXUpdaterPopUp;
            if (getSettingByte(NULL,"PopupsLogEnabled",1))
            {
                ShowPopUpMsg(NULL, 0, "Popup Title", "Sample Note",    LOG_NOTE);
                ShowPopUpMsg(NULL, 0, "Popup Title", "Sample Warning", LOG_WARNING);
                ShowPopUpMsg(NULL, 0, "Popup Title", "Sample Error",   LOG_ERROR);
                ShowPopUpMsg(NULL, 0, "Popup Title", "Sample Fatal",   LOG_FATAL);
            }
            if(bFoundPopUp)
                ShowPopUpMsg(NULL, 0, "Contact", "Sample ...was found!",   POPTYPE_FOUND);
            if(bScanPopUp)
            {
                gbScan = TRUE;
                ShowPopUpMsg(NULL, 0, "Entire List Check", "Sample Users Status Scan Started",   POPTYPE_SCAN);

                gbScan = FALSE;
                ShowPopUpMsg(NULL, 0, "Active Status Scan", "Sample Users Status Scan Complete",   POPTYPE_SCAN);
            }
            if(bSpamPopUp)
                ShowPopUpMsg(NULL, 0, "Popup Title", "Sample Spambot Found", POPTYPE_SPAM);
            if(bUnknownPopUp)
                ShowPopUpMsg(NULL, 0, "Popup Title", "Sample Unknown Found", POPTYPE_UNKNOWN);
            if(bVisPopUp)
                ShowPopUpMsg(NULL, 0, "Popup Title", "Sample Visibility event", POPTYPE_VIS);
            if(bClientChangePopUp)
                ShowPopUpMsg(NULL, 0, "Popup Title", "Sample ClientChange event", POPTYPE_CLIENT_CHANGE);
            if(bInfoRequestPopUp)
                ShowPopUpMsg(NULL, 0, "Popup Title", "Sample InfoRequest event", POPTYPE_INFO_REQUEST);
            if(bIgnoreCheckPop)
                ShowPopUpMsg(NULL, 0, "Popup Title", "Check Your Ignore State", POPTYPE_IGNORE_CHECK);
            if(bPopSelfRem)
                ShowPopUpMsg(NULL, 0, "Popup Title", "Removed Himself", POPTYPE_SELFREMOVE);
            if(bAuthPopUp)
                ShowPopUpMsg(NULL, 0, "Popup Title", "Sample Auth PopUp", POPTYPE_AUTH);
            if(bXUpdaterPopUp)
                ShowPopUpMsg(NULL, 0, "Popup Title", "Sample XStatus Updater PopUp", POPTYPE_XUPDATER);
        }
        return FALSE;

        case IDC_POPUPS_ENABLED:
            bEnabled = IsDlgButtonChecked(hwndDlg,IDC_POPUPS_ENABLED);
            icq_EnableMultipleControls(hwndDlg, icqPopupsControls, SIZEOF(icqPopupsControls), bEnabled);
            break;
        case IDC_USEWINCOLORS:
            bEnabled = IsDlgButtonChecked(hwndDlg,IDC_POPUPS_ENABLED);
            icq_EnableMultipleControls(hwndDlg, icqPopupColorControls, SIZEOF(icqPopupColorControls), bEnabled & !IsDlgButtonChecked(hwndDlg,IDC_USEWINCOLORS));
            break;
        case IDC_POPUPS_OPTIONS:
            ShowPopUpsOpts();
            return TRUE;
        }
        OptDlgChanged(hwndDlg);
        break;
    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case PSN_APPLY:
            setSettingByte(NULL,"PopupsEnabled",(BYTE)IsDlgButtonChecked(hwndDlg,IDC_POPUPS_ENABLED));
            setSettingByte(NULL,"PopupsLogEnabled",(BYTE)IsDlgButtonChecked(hwndDlg,IDC_POPUPS_LOG_ENABLED));
            setSettingByte(NULL,"PopupsWinColors",(BYTE)IsDlgButtonChecked(hwndDlg,IDC_USEWINCOLORS));
            setSettingByte(NULL,"PopupsSysIcons",(BYTE)IsDlgButtonChecked(hwndDlg,IDC_USESYSICONS));
            StoreDBCheckState(hwndDlg, IDC_POPUP_FOR_NOTONLIST , "PopUpForNotOnList");
            bPopUpForNotOnList = getSettingByte(NULL, "PopUpForNotOnList", 0);
            StoreDBCheckState(hwndDlg, IDC_POPUP_FOR_IGNORED, "PopUpForIgnored");
            bPopupsForIgnored = getSettingByte(NULL, "PopUpForIgnored", 0);
            StoreDBCheckState(hwndDlg, IDC_POPUP_FOR_HIDDEN, "PopUpForHidden");
            bPopupsForHidden = getSettingByte(NULL, "PopUpForHidden", 0);
            StoreDBCheckState(hwndDlg, IDC_POPCLOSEWND, "CloseWindow");
            bCloseWindowPopUp = getSettingByte(NULL, "CloseWindow", 0);
            StoreDBCheckState(hwndDlg, IDC_UINPOPUP , "UinPopup");
            bUinPopup = getSettingByte(NULL, "UinPopup", 0);
            StoreDBCheckState(hwndDlg, IDC_READXSTPOPUP, "ReadXStatusPopUp");
            bReadXStatusPopUp = getSettingByte(NULL, "ReadXStatusPopUp", 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}




static INT_PTR CALLBACK DlgProcIcqPopupOpts2(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    extern BOOL bXUpdaterPopUp;
    switch (msg)
    {
    case WM_INITDIALOG:
        ICQTranslateDialog(hwndDlg);
        LoadDBCheckState(hwndDlg, IDC_SPAM_POPUP_ENABLE, "SpamPopUpEnabled", 0);
        LoadDBCheckState(hwndDlg, IDC_UNKNOWN_POPUP_ENABLE, "UnknownPopUpEnabled", 0);
        LoadDBCheckState(hwndDlg, IDC_WAS_FOUND_POPUP_ENABLE, "FoundPopUpEnabled", 1);
        LoadDBCheckState(hwndDlg, IDC_SCAN_POPUP_ENABLE, "ScanPopUpEnabled", 1);
        LoadDBCheckState(hwndDlg, IDC_VIS_POPUP_ENABLE, "VisPopUpEnabled", 1);
        LoadDBCheckState(hwndDlg, IDC_CLIENT_CHANGE_POPUP, "ClientChangePopup", 0);
        LoadDBCheckState(hwndDlg, IDC_POPSELFREM, "PopSelfRem", 1);
        LoadDBCheckState(hwndDlg, IDC_IGNCHECKPOP, "IgnoreCheckPop", 1);
        LoadDBCheckState(hwndDlg, IDC_INFO_REQUEST_POPUP, "InfoRequestPopUp", 0);
        LoadDBCheckState(hwndDlg, IDC_POPAUTH, "AuthPopUp", 0);
        LoadDBCheckState(hwndDlg, IDC_POPXUPDATER, "XUpdaterPopUp", 0);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_LOG0_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"Popups0TextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_LOG0_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"Popups0BackColor",RGB(0,0,0)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_LOG0_TIMEOUT, getSettingDword(NULL,"Popups0Timeout",0),FALSE);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_LOG1_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"Popups1TextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_LOG1_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"Popups1BackColor",RGB(0,0,0)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_LOG1_TIMEOUT, getSettingDword(NULL,"Popups1Timeout",0),FALSE);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_LOG2_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"Popups2TextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_LOG2_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"Popups2BackColor",RGB(0,0,0)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_LOG2_TIMEOUT, getSettingDword(NULL,"Popups2Timeout",0),FALSE);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_LOG3_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"Popups3TextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_LOG3_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"Popups3BackColor",RGB(0,0,0)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_LOG3_TIMEOUT, getSettingDword(NULL,"Popups3Timeout",0),FALSE);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_LOG4_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsFoundTextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_LOG4_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsFoundBackColor",RGB(0,0,0)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_LOG4_TIMEOUT, getSettingDword(NULL,"PopupsFoundTimeout",0),FALSE);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_LOG5_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsFinishedTextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_LOG5_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsFinishedBackColor",RGB(0,0,0)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_LOG5_TIMEOUT, getSettingDword(NULL,"PopupsFinishedTimeout",0),FALSE);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_SPAM_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsSpamTextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_SPAM_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsSpamBackColor",RGB(0,0,255)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_SPAM_TIMEOUT, getSettingDword(NULL,"PopupsSpamTimeout",0),FALSE);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_UNKNOWN_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsUnknownTextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_UNKNOWN_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsUnknownBackColor",RGB(0,0,0)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_UNKNOWN_TIMEOUT, getSettingDword(NULL,"PopupsUnknownTimeout",0),FALSE);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_LOG6_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsVisTextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_LOG6_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsVisBackColor",RGB(0,0,0)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_LOG6_TIMEOUT, getSettingDword(NULL,"PopupsVisTimeout",0),FALSE);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_CLIENT_CHANGE_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsClientChangeTextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_CLIENT_CHANGE_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsClientChangeBackColor",RGB(0,0,0)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_CLIENT_CHANGE_TIMEOUT, getSettingDword(NULL,"PopupsClientChangeTimeout",0),FALSE);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_INFO_REQUEST_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsInfoRequestTextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_INFO_REQUEST_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsInfoRequestBackColor",RGB(0,0,0)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_INFO_REQUEST_TIMEOUT, getSettingDword(NULL,"PopupsInfoRequestTimeout",0),FALSE);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_IGNORECHECK_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsIgnoreCheckTextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_IGNORECHECK_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsIgnoreCheckBackColor",RGB(0,0,0)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_IGNORECHECK_TIMEOUT, getSettingDword(NULL,"PopupsIgnoreCheckTimeout",0),FALSE);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_REMOVE_HIMSELF_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsRemoveHimselfTextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_REMOVE_HIMSELF_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsRemoveHimselfBackColor",RGB(0,0,0)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_REMOVE_HIMSELF_TIMEOUT, getSettingDword(NULL,"PopupsRemoveHimselfTimeout",0),FALSE);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_AUTH_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsAuthTextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_AUTH_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsAuthBackColor",RGB(0,0,0)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_AUTH_TIMEOUT, getSettingDword(NULL,"PopupsAuthTimeout",0),FALSE);
        SendDlgItemMessage(hwndDlg, IDC_POPUP_XUPDATER_TEXTCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsXUpdaterTextColor",RGB(255,255,255)));
        SendDlgItemMessage(hwndDlg, IDC_POPUP_XUPDATER_BACKCOLOR, CPM_SETCOLOUR, 0, getSettingDword(NULL,"PopupsXUpdaterBackColor",RGB(0,0,0)));
        SetDlgItemInt(hwndDlg, IDC_POPUP_XUPDATER_TIMEOUT, getSettingDword(NULL,"PopupsXUpdaterTimeout",0),FALSE);
        icq_EnableMultipleControls(hwndDlg, icqPopupColorControls, SIZEOF(icqPopupColorControls), getSettingByte(NULL, "PopupsWinColors", 0)-1);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_OK:
            setSettingDword(NULL,"Popups0TextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_LOG0_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"Popups0BackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_LOG0_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"Popups0Timeout",GetDlgItemInt(hwndDlg, IDC_POPUP_LOG0_TIMEOUT, NULL, FALSE));
            setSettingDword(NULL,"Popups1TextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_LOG1_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"Popups1BackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_LOG1_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"Popups1Timeout",GetDlgItemInt(hwndDlg, IDC_POPUP_LOG1_TIMEOUT, NULL, FALSE));
            setSettingDword(NULL,"Popups2TextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_LOG2_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"Popups2BackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_LOG2_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"Popups2Timeout",GetDlgItemInt(hwndDlg, IDC_POPUP_LOG2_TIMEOUT, NULL, FALSE));
            setSettingDword(NULL,"Popups3TextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_LOG3_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"Popups3BackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_LOG3_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"Popups3Timeout",GetDlgItemInt(hwndDlg, IDC_POPUP_LOG3_TIMEOUT, NULL, FALSE));
            setSettingDword(NULL,"PopupsFoundTextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_LOG4_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsFoundBackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_LOG4_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsFoundTimeout",GetDlgItemInt(hwndDlg, IDC_POPUP_LOG4_TIMEOUT, NULL, FALSE));
            setSettingDword(NULL,"PopupsFinishedTextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_LOG5_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsFinishedBackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_LOG5_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsFinishedTimeout",GetDlgItemInt(hwndDlg, IDC_POPUP_LOG5_TIMEOUT, NULL, FALSE));
            setSettingDword(NULL,"PopupsSpamTextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_SPAM_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsSpamBackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_SPAM_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsSpamTimeout",GetDlgItemInt(hwndDlg, IDC_POPUP_SPAM_TIMEOUT, NULL, FALSE));
            setSettingDword(NULL,"PopupsUnknownTextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_UNKNOWN_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsUnknownBackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_UNKNOWN_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsUnknownTimeout",GetDlgItemInt(hwndDlg, IDC_POPUP_UNKNOWN_TIMEOUT, NULL, FALSE));
            setSettingDword(NULL,"PopupsVisTextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_LOG6_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsVisBackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_LOG6_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsVisTimeout",GetDlgItemInt(hwndDlg, IDC_POPUP_LOG6_TIMEOUT, NULL, FALSE));
            setSettingDword(NULL,"PopupsClientChangeTextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_CLIENT_CHANGE_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsClientChangeBackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_CLIENT_CHANGE_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsClientChangeTimeout",GetDlgItemInt(hwndDlg, IDC_POPUP_CLIENT_CHANGE_TIMEOUT, NULL, FALSE));
            setSettingDword(NULL,"PopupsInfoRequestTextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_INFO_REQUEST_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsInfoRequestBackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_INFO_REQUEST_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsInfoRequestTimeout",GetDlgItemInt(hwndDlg, IDC_POPUP_INFO_REQUEST_TIMEOUT, NULL, FALSE));
            setSettingDword(NULL,"PopupsIgnoreCheckTextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_IGNORECHECK_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsIgnoreCheckBackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_IGNORECHECK_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsIgnoreCheckTimeout",GetDlgItemInt(hwndDlg, IDC_POPUP_IGNORECHECK_TIMEOUT, NULL, FALSE));
            setSettingDword(NULL,"PopupsRemoveHimselfTextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_REMOVE_HIMSELF_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsRemoveHimselfBackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_REMOVE_HIMSELF_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsRemoveHimselfTimeout",GetDlgItemInt(hwndDlg, IDC_POPUP_REMOVE_HIMSELF_TIMEOUT, NULL, FALSE));
            setSettingDword(NULL,"PopupsAuthTextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_AUTH_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsAuthBackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_AUTH_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsAuthTimeout",GetDlgItemInt(hwndDlg, IDC_POPUP_AUTH_TIMEOUT, NULL, FALSE));
            setSettingDword(NULL,"PopupsXUpdaterTextColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_XUPDATER_TEXTCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsXUpdaterBackColor",SendDlgItemMessage(hwndDlg,IDC_POPUP_XUPDATER_BACKCOLOR,CPM_GETCOLOUR,0,0));
            setSettingDword(NULL,"PopupsXUpdaterTimeout",GetDlgItemInt(hwndDlg, IDC_POPUP_XUPDATER_TIMEOUT, NULL, FALSE));
            StoreDBCheckState(hwndDlg, IDC_SPAM_POPUP_ENABLE , "SpamPopUpEnabled");
            bSpamPopUp = getSettingByte(NULL, "SpamPopUpEnabled", 0);
            StoreDBCheckState(hwndDlg, IDC_UNKNOWN_POPUP_ENABLE , "UnknownPopUpEnabled");
            bUnknownPopUp = getSettingByte(NULL, "UnknownPopUpEnabled", 0);
            StoreDBCheckState(hwndDlg, IDC_WAS_FOUND_POPUP_ENABLE , "FoundPopUpEnabled");
            bFoundPopUp = getSettingByte(NULL, "FoundPopUpEnabled", 1);
            StoreDBCheckState(hwndDlg, IDC_SCAN_POPUP_ENABLE , "ScanPopUpEnabled");
            bScanPopUp = getSettingByte(NULL, "ScanPopUpEnabled", 1);
            StoreDBCheckState(hwndDlg, IDC_VIS_POPUP_ENABLE , "VisPopUpEnabled");
            bVisPopUp = getSettingByte(NULL, "VisPopUpEnabled", 1);
            StoreDBCheckState(hwndDlg, IDC_CLIENT_CHANGE_POPUP , "ClientChangePopup");
            bClientChangePopUp = getSettingByte(NULL, "ClientChangePopup", 0);
            StoreDBCheckState(hwndDlg, IDC_IGNCHECKPOP , "IgnoreCheckPop");
            bIgnoreCheckPop = getSettingByte(NULL, "IgnoreCheckPop", 1);
            StoreDBCheckState(hwndDlg, IDC_POPSELFREM , "PopSelfRem");
            bPopSelfRem = getSettingByte(NULL, "PopSelfRem", 1);
            StoreDBCheckState(hwndDlg, IDC_INFO_REQUEST_POPUP , "InfoRequestPopUp");
            bInfoRequestPopUp = getSettingByte(NULL, "InfoRequestPopUp", 0);
            StoreDBCheckState(hwndDlg, IDC_POPAUTH , "AuthPopUp");
            bAuthPopUp = getSettingByte(NULL, "AuthPopUp", 0);
            StoreDBCheckState(hwndDlg, IDC_POPXUPDATER , "XUpdaterPopUp");
            bXUpdaterPopUp = getSettingByte(NULL, "XUpdaterPopUp", 0);
            DestroyWindow(hwndDlg);
        }
        break;
    case WM_NOTIFY:
        //switch (((LPNMHDR)lParam)->code)
        //{
        // break;
        //}
        break;
    case WM_CLOSE:
        DestroyWindow(hwndDlg);
        break;
    case WM_DESTROY:
        hwndPopUpsOpts = NULL;
        break;
    }
    return FALSE;
}
void ShowASDOpts(void);
static INT_PTR CALLBACK DlgProcIcqASDOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_INITDIALOG:
        ICQTranslateDialog(hwndDlg);
        LoadDBCheckState(hwndDlg, IDC_NOASD, "NoASDInInvisible", 1);
        LoadDBCheckState(hwndDlg, IDC_ASDSTARTUP, "ASDStartup", 0);
        LoadDBCheckState(hwndDlg, IDC_ASD_FOR_OFFLINE, "ASDForOffline", 1);
        LoadDBCheckState(hwndDlg, IDC_DETECT_VIA_STATUS_MESSAGE, "bASDViaAwayMsg", 0);
        LoadDBCheckState(hwndDlg, IDC_DETECT_VIA_XTRAZ, "bASDViaXtraz", 0);
        LoadDBCheckState(hwndDlg, IDC_DETECT_VIA_URL, "bASDViaURL", 0);
        LoadDBCheckState(hwndDlg, IDC_DETECT_UNAUTHORIZED, "bASDUnauthorized", 0);
        LoadDBCheckState(hwndDlg, IDC_DETECT_VIA_AUTH, "bASDViaAuth", 0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_OK:
            StoreDBCheckState(hwndDlg, IDC_NOASD , "NoASDInInvisible");
            bNoASDInInvisible = getSettingByte(NULL, "NoASDInInvisible", 1);
            StoreDBCheckState(hwndDlg, IDC_ASD_FOR_OFFLINE , "ASDForOffline");
            bASDForOffline = getSettingByte(NULL, "ASDForOffline", 1);
            StoreDBCheckState(hwndDlg, IDC_ASDSTARTUP , "ASDStartup");
            StoreDBCheckState(hwndDlg, IDC_DETECT_VIA_STATUS_MESSAGE, "bASDViaAwayMsg");
            bASDViaAwayMsg = getSettingByte(NULL, "bASDViaAwayMsg", 0);
            StoreDBCheckState(hwndDlg, IDC_DETECT_VIA_XTRAZ, "bASDViaXtraz");
            bASDViaXtraz = getSettingByte(NULL, "bASDViaXtraz", 0);
            StoreDBCheckState(hwndDlg, IDC_DETECT_VIA_URL, "bASDViaURL");
            bASDViaURL = getSettingByte(NULL, "bASDViaURL", 0);
            StoreDBCheckState(hwndDlg, IDC_DETECT_UNAUTHORIZED, "bASDUnauthorized");
            bASDUnauthorized = getSettingByte(NULL, "bASDUnauthorized", 0);
            StoreDBCheckState(hwndDlg, IDC_DETECT_VIA_AUTH, "bASDViaAuth");
            bASDViaAuth = getSettingByte(NULL, "bASDViaAuth", 0);
            DestroyWindow(hwndDlg);
            break;
        }
        break;
    case WM_NOTIFY:
        //switch (((LPNMHDR)lParam)->code)
        //{
        // break;
        //}
        break;
    case WM_CLOSE:
        DestroyWindow(hwndDlg);
        break;
    case WM_DESTROY:
        hwndASDOpts = NULL;
        break;
    }
    return FALSE;
}


static HWND hCpCombo;

struct CPTABLE
{
    WORD cpId;
    char *cpName;
};


struct CPTABLE cpTable[] =
{
    {  874,  "Thai" },
    {  932,  "Japanese" },
    {  936,  "Simplified Chinese" },
    {  949,  "Korean" },
    {  950,  "Traditional Chinese" },
    {  1250,  "Central European" },
    {  1251,  "Cyrillic" },
    {  1252,  "Latin I" },
    {  1253,  "Greek" },
    {  1254,  "Turkish" },
    {  1255,  "Hebrew" },
    {  1256,  "Arabic" },
    {  1257,  "Baltic" },
    {  1258,  "Vietnamese" },
    {  1361,  "Korean (Johab)" },
    {   -1, NULL}
};

static BOOL CALLBACK FillCpCombo(LPSTR str)
{
    int i;
    UINT cp;

    cp = atoi(str);
    for (i=0; cpTable[i].cpName != NULL && cpTable[i].cpId!=cp; i++);
    if (cpTable[i].cpName != NULL)
    {
        ComboBoxAddStringUtf(hCpCombo, cpTable[i].cpName, cpTable[i].cpId);
    }
    return TRUE;
}


static const UINT icqUnicodeControls[] = {IDC_UTFALL,IDC_UTFSTATIC,IDC_UTFCODEPAGE};
static const UINT icqDCMsgControls[] = {IDC_DCPASSIVE};
static const UINT icqDCIconsControls[] = {IDC_DC_ICON_POS,IDC_DC_ICON_POS_TEXT};
static const UINT icqXStatusControls[] = {IDC_QIP_STATUS,IDC_QIP_STATUS_ICON_POS,IDC_QIP_STATUS_POS_TEXT,IDC_QIPSTATUS_SHOW,IDC_XSTNONSTD,IDC_XSTUPDATERATE,IDC_XSTUPDATE,IDC_XSTATUSAUTO,IDC_XSTATUSRESET,IDC_FORCEXSTATUS,IDC_XSTATUS_ICON_POS,IDC_XSTATUS_SHOW,IDC_REPLACEXTEXT,IDC_ICON_POS_TEXT};// modified
static const UINT icqAimControls[] = {IDC_AIMENABLE};
static INT_PTR CALLBACK DlgProcIcqFeaturesOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        BYTE bData;
        int sCodePage;
        int i;

        ICQTranslateDialog(hwndDlg);
        bData = getSettingByte(NULL, "UtfEnabled", DEFAULT_UTF_ENABLED);
        CheckDlgButton(hwndDlg, IDC_UTFENABLE, bData?TRUE:FALSE);
        CheckDlgButton(hwndDlg, IDC_UTFALL, bData==2?TRUE:FALSE);
        icq_EnableMultipleControls(hwndDlg, icqUnicodeControls, sizeof(icqUnicodeControls)/sizeof(icqUnicodeControls[0]), bData?TRUE:FALSE);
        LoadDBCheckState(hwndDlg, IDC_XSTUPDATE, "UpdateXStatus", 1);
        SetDlgItemInt(hwndDlg, IDC_XSTUPDATERATE, getSettingDword(NULL, "XStatusUpdatePeriod", 15), 0);
        EnableDlgItem(hwndDlg, IDC_XSTUPDATERATE, IsDlgButtonChecked(hwndDlg, IDC_XSTUPDATE));
        LoadDBCheckState(hwndDlg, IDC_TEMPVISIBLE, "TempVisListEnabled",DEFAULT_TEMPVIS_ENABLED);
        LoadDBCheckState(hwndDlg, IDC_SLOWSEND, "SlowSend", DEFAULT_SLOWSEND);
        LoadDBCheckState(hwndDlg, IDC_ONLYSERVERACKS, "OnlyServerAcks", DEFAULT_ONLYSERVERACKS);
        bData = getSettingByte(NULL, "DirectMessaging", DEFAULT_DCMSG_ENABLED);
        LoadDBCheckState(hwndDlg, IDC_DCICON, "ShowDCIcon", 1);
        CheckDlgButton(hwndDlg, IDC_DCENABLE, bData?TRUE:FALSE);
        CheckDlgButton(hwndDlg, IDC_DCPASSIVE, bData==1?TRUE:FALSE);
        icq_EnableMultipleControls(hwndDlg, icqDCMsgControls, sizeof(icqDCMsgControls)/sizeof(icqDCMsgControls[0]), bData?TRUE:FALSE);
        bData = getSettingByte(NULL, "XStatusEnabled", DEFAULT_XSTATUS_ENABLED);
        CheckDlgButton(hwndDlg, IDC_XSTATUSENABLE, bData);
        icq_EnableMultipleControls(hwndDlg, icqXStatusControls, sizeof(icqXStatusControls)/sizeof(icqXStatusControls[0]), bData);
        LoadDBCheckState(hwndDlg, IDC_XSTATUSAUTO, "XStatusAuto", DEFAULT_XSTATUS_AUTO);
        LoadDBCheckState(hwndDlg, IDC_XSTATUSRESET, "XStatusReset", DEFAULT_XSTATUS_RESET);
        LoadDBCheckState(hwndDlg, IDC_XSTATUS_SHOW, "XStatusIconShow", 1);
        LoadDBCheckState(hwndDlg, IDC_QIPSTATUS_SHOW, "QipStatusShow", 0);
        LoadDBCheckState(hwndDlg, IDC_QIP_STATUS, "QipStatusEnable", 0);
        LoadDBCheckState(hwndDlg, IDC_FORCEXSTATUS, "ForceXstatus", 0);
        LoadDBCheckState(hwndDlg, IDC_KILLSPAMBOTS, "KillSpambots", DEFAULT_KILLSPAM_ENABLED);
        LoadDBCheckState(hwndDlg, IDC_KILLUNKNOWN, "KillUnknown", 0);
        LoadDBCheckState(hwndDlg, IDC_AIMENABLE, "AimEnabled", DEFAULT_AIM_ENABLED);
        LoadDBCheckState(hwndDlg, IDC_RTF, "RTF", 0);
        LoadDBCheckState(hwndDlg, IDC_REPLACEXTEXT, "ShowMyXText", 1);
        ShowWindow(GetDlgItem(hwndDlg, IDC_DCICON), gbExtraIcons?SW_HIDE:SW_SHOW);
        ShowWindow(GetDlgItem(hwndDlg, IDC_DC_ICON_POS), gbExtraIcons?SW_HIDE:SW_SHOW);
        ShowWindow(GetDlgItem(hwndDlg, IDC_DC_ICON_POS_TEXT), gbExtraIcons?SW_HIDE:SW_SHOW);
        ShowWindow(GetDlgItem(hwndDlg, IDC_QIPSTATUS_SHOW), gbExtraIcons?SW_HIDE:SW_SHOW);
        ShowWindow(GetDlgItem(hwndDlg, IDC_QIP_STATUS_ICON_POS), gbExtraIcons?SW_HIDE:SW_SHOW);
        ShowWindow(GetDlgItem(hwndDlg, IDC_QIP_STATUS_POS_TEXT), gbExtraIcons?SW_HIDE:SW_SHOW);
        ShowWindow(GetDlgItem(hwndDlg, IDC_XSTATUS_SHOW), gbExtraIcons?SW_HIDE:SW_SHOW);
        ShowWindow(GetDlgItem(hwndDlg, IDC_ICON_POS_TEXT), gbExtraIcons?SW_HIDE:SW_SHOW);
        ShowWindow(GetDlgItem(hwndDlg, IDC_XSTATUS_ICON_POS), gbExtraIcons?SW_HIDE:SW_SHOW);
        ShowWindow(GetDlgItem(hwndDlg, IDC_DC_ICON_POS_TEXT), gbExtraIcons?SW_HIDE:SW_SHOW);
        icq_EnableMultipleControls(hwndDlg, icqAimControls, sizeof(icqAimControls)/sizeof(icqAimControls[0]), icqOnline?FALSE:TRUE);
        {
            char* CIdComboBox[] =
            {
                "Email","Protocol","SMS","Advanced 1","Advanced 2","Web","Client","VisMode","Advanced 3","Advanced 4"
            };
            int i;
            for (i=0; i<sizeof(CIdComboBox)/sizeof(CIdComboBox[0]); i++)
            {
                ComboBoxAddStringUtf(GetDlgItem(hwndDlg, IDC_DC_ICON_POS), CIdComboBox[i],i);
                ComboBoxAddStringUtf(GetDlgItem(hwndDlg, IDC_XSTATUS_ICON_POS), CIdComboBox[i],i);
                ComboBoxAddStringUtf(GetDlgItem(hwndDlg, IDC_QIP_STATUS_ICON_POS), CIdComboBox[i],i);
            }
        }
        SendDlgItemMessage(hwndDlg, IDC_DC_ICON_POS, CB_SETCURSEL, (DBGetContactSettingWord(NULL,ICQ_PROTOCOL_NAME,"dc_icon_pos", 5))-1, 0);
        SendDlgItemMessage(hwndDlg, IDC_XSTATUS_ICON_POS, CB_SETCURSEL, (DBGetContactSettingWord(NULL,ICQ_PROTOCOL_NAME,"xstatus_icon_pos", 4))-1, 0);
        SendDlgItemMessage(hwndDlg, IDC_QIP_STATUS_ICON_POS, CB_SETCURSEL, (DBGetContactSettingWord(NULL,ICQ_PROTOCOL_NAME,"qip_status_icon_pos", 9))-1, 0);//added
        hCpCombo = GetDlgItem(hwndDlg, IDC_UTFCODEPAGE);
        sCodePage = getSettingWord(NULL, "AnsiCodePage", CP_ACP);
        ComboBoxAddStringUtf(GetDlgItem(hwndDlg, IDC_UTFCODEPAGE), "System default codepage", 0);
        EnumSystemCodePagesA(FillCpCombo, CP_INSTALLED);
        if(sCodePage == 0)
            SendDlgItemMessage(hwndDlg, IDC_UTFCODEPAGE, CB_SETCURSEL, (WPARAM)0, 0);
        else
        {
            for (i = 0; i < SendDlgItemMessage(hwndDlg, IDC_UTFCODEPAGE, CB_GETCOUNT, 0, 0); i++)
            {
                if (SendDlgItemMessage(hwndDlg, IDC_UTFCODEPAGE, CB_GETITEMDATA, (WPARAM)i, 0) == sCodePage)
                {
                    SendDlgItemMessage(hwndDlg, IDC_UTFCODEPAGE, CB_SETCURSEL, (WPARAM)i, 0);
                    break;
                }
            }
        }
        LoadDBCheckState(hwndDlg, IDC_XSTNONSTD, "NonStandartXstatus", 1);
        LoadDBCheckState(hwndDlg, IDC_QIP_STATUS, "QipStatusEnable", 1);
    }
    break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_UTFENABLE:
            icq_EnableMultipleControls(hwndDlg, icqUnicodeControls, sizeof(icqUnicodeControls)/sizeof(icqUnicodeControls[0]), IsDlgButtonChecked(hwndDlg, IDC_UTFENABLE));
            OptDlgChanged(hwndDlg);
            break;
        case IDC_DCENABLE:
            icq_EnableMultipleControls(hwndDlg, icqDCMsgControls, sizeof(icqDCMsgControls)/sizeof(icqDCMsgControls[0]), IsDlgButtonChecked(hwndDlg, IDC_DCENABLE));
            OptDlgChanged(hwndDlg);
            break;
        case IDC_XSTATUSENABLE:
            icq_EnableMultipleControls(hwndDlg, icqXStatusControls, sizeof(icqXStatusControls)/sizeof(icqXStatusControls[0]), IsDlgButtonChecked(hwndDlg, IDC_XSTATUSENABLE));
            OptDlgChanged(hwndDlg);
            break;
        case IDC_DCICON:
            icq_EnableMultipleControls(hwndDlg, icqDCIconsControls, sizeof(icqDCIconsControls)/sizeof(icqDCIconsControls[0]), IsDlgButtonChecked(hwndDlg, IDC_DCICON));
            OptDlgChanged(hwndDlg);
            break;
        case IDC_XSTUPDATE:
            EnableDlgItem(hwndDlg, IDC_XSTUPDATERATE, IsDlgButtonChecked(hwndDlg, IDC_XSTUPDATE));
            OptDlgChanged(hwndDlg);
            break;
        case IDC_UTFALL:
        case IDC_RTF:
        case IDC_UTFCODEPAGE:
        case IDC_TEMPVISIBLE:
        case IDC_SLOWSEND:
        case IDC_ONLYSERVERACKS:
        case IDC_DCPASSIVE:
        case IDC_XSTATUSAUTO:
        case IDC_QIP_STATUS_ICON_POS:
        case IDC_XSTATUS_ICON_POS:
        case IDC_DC_ICON_POS:
        case IDC_XSTATUS_SHOW:
        case IDC_QIPSTATUS_SHOW:
        case IDC_XSTATUSRESET:
        case IDC_KILLUNKNOWN:
        case IDC_KILLSPAMBOTS:
        case IDC_AIMENABLE:
        case IDC_REPLACEXTEXT:
        case IDC_FORCEXSTATUS:
        case IDC_XSTNONSTD:
        case IDC_QIP_STATUS:
            OptDlgChanged(hwndDlg);
            break;
        case IDC_XSTUPDATERATE:
            if(GetDlgItemInt(hwndDlg, IDC_XSTUPDATERATE, 0, 0)!=getSettingDword(NULL, "XStatusUpdatePeriod", 0))
                OptDlgChanged(hwndDlg);
            break;
        default:
            return 0;
        }
        break;
    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case PSN_APPLY:
        {
            int i = SendDlgItemMessage(hwndDlg, IDC_DC_ICON_POS, CB_GETCURSEL, 0, 0)+1;
            DBWriteContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "dc_icon_pos", (WORD)i);
        }
        {
            int i = SendDlgItemMessage(hwndDlg, IDC_XSTATUS_ICON_POS, CB_GETCURSEL, 0, 0)+1;
            DBWriteContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "xstatus_icon_pos", (WORD)i);
        }
        {
            int i = SendDlgItemMessage(hwndDlg, IDC_QIP_STATUS_ICON_POS, CB_GETCURSEL, 0, 0)+1;
            DBWriteContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "qip_status_icon_pos", (WORD)i);
        }
        if( DBGetContactSettingByte(NULL,ICQ_PROTOCOL_NAME,"RTF", 0) != (BYTE)IsDlgButtonChecked(hwndDlg,IDC_RTF))
        {
            MessageBox(0,TranslateW("To enable RTF text reciving you must reconnect your Miranda after option is enabled"),TranslateW("Warning"),MB_OK);
            setSettingByte(NULL,"RTF",(BYTE)IsDlgButtonChecked(hwndDlg,IDC_RTF));
        }
        if (IsDlgButtonChecked(hwndDlg, IDC_UTFENABLE))
            gbUtfEnabled = IsDlgButtonChecked(hwndDlg, IDC_UTFALL)?2:1;
        else
            gbUtfEnabled = 0;
        {
            int i = SendDlgItemMessage(hwndDlg, IDC_UTFCODEPAGE, CB_GETCURSEL, 0, 0);
            gwAnsiCodepage = (WORD)SendDlgItemMessage(hwndDlg, IDC_UTFCODEPAGE, CB_GETITEMDATA, (WPARAM)i, 0);
            setSettingWord(NULL, "AnsiCodePage", gwAnsiCodepage);
        }
        setSettingByte(NULL, "UtfEnabled", gbUtfEnabled);
        gbTempVisListEnabled = (BYTE)IsDlgButtonChecked(hwndDlg, IDC_TEMPVISIBLE);
        setSettingByte(NULL, "TempVisListEnabled", gbTempVisListEnabled);
        StoreDBCheckState(hwndDlg, IDC_SLOWSEND, "SlowSend");
        StoreDBCheckState(hwndDlg, IDC_ONLYSERVERACKS, "OnlyServerAcks");
        if (IsDlgButtonChecked(hwndDlg, IDC_DCENABLE))
            gbDCMsgEnabled = IsDlgButtonChecked(hwndDlg, IDC_DCPASSIVE)?1:2;
        else
            gbDCMsgEnabled = 0;
        setSettingByte(NULL, "DirectMessaging", gbDCMsgEnabled);
        gbXStatusEnabled = (BYTE)IsDlgButtonChecked(hwndDlg, IDC_XSTATUSENABLE);
        setSettingByte(NULL, "XStatusEnabled", gbXStatusEnabled);
        StoreDBCheckState(hwndDlg, IDC_DCICON, "ShowDCIcon");
        StoreDBCheckState(hwndDlg, IDC_XSTATUSAUTO, "XStatusAuto");
        StoreDBCheckState(hwndDlg, IDC_XSTATUSRESET, "XStatusReset");
        StoreDBCheckState(hwndDlg, IDC_FORCEXSTATUS , "ForceXstatus");
        StoreDBCheckState(hwndDlg, IDC_XSTATUS_SHOW, "XStatusIconShow");
        bXstatusIconShow = getSettingByte(NULL, "XStatusIconShow", 1);
        StoreDBCheckState(hwndDlg, IDC_KILLSPAMBOTS , "KillSpambots");
        StoreDBCheckState(hwndDlg, IDC_KILLUNKNOWN , "KillUnknown");
        StoreDBCheckState(hwndDlg, IDC_AIMENABLE, "AimEnabled");
        StoreDBCheckState(hwndDlg, IDC_RTF, "RTF");
        StoreDBCheckState(hwndDlg, IDC_REPLACEXTEXT, "ShowMyXText");
        {
            DWORD dwRate;
            StoreDBCheckState(hwndDlg, IDC_XSTUPDATE, "UpdateXStatus");
            dwRate = GetDlgItemInt(hwndDlg, IDC_XSTUPDATERATE, 0, 0);
            if(dwRate < 1 || dwRate > 60)
                dwRate = 15;
            SetDlgItemInt(hwndDlg, IDC_XSTUPDATERATE, dwRate, 0);
            setSettingDword(NULL, "XStatusUpdatePeriod", dwRate);
        }
        StoreDBCheckState(hwndDlg, IDC_XSTNONSTD, "NonStandartXstatus");
        StoreDBCheckState(hwndDlg, IDC_QIP_STATUS, "QipStatusEnable");
        gbQipStatusEnabled = getSettingByte(NULL, "QipStatusEnable", 0);
        StoreDBCheckState(hwndDlg, IDC_QIPSTATUS_SHOW, "QipStatusShow");
        bQipstatusIconShow = getSettingByte(NULL, "QipStatusShow", 0);
        return TRUE;
        }
        break;
    }
    return FALSE;
}
static const UINT icqASDControls[] = {IDC_NOASD, IDC_CHECKITEM, IDC_ASDSTARTUP, IDC_ASDOPTIONS};
static const UINT icqPSDControls[] = {IDC_NOPSD_FOR_HIDDEN};
static INT_PTR CALLBACK DlgProcIcqFeatures2Opts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    extern BOOL bShowAuth;
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        BYTE bData;

        ICQTranslateDialog(hwndDlg);
//			LoadDBCheckState(hwndDlg, IDC_STEALTHRQST, "StealthRequest", 0);
        LoadDBCheckState(hwndDlg, IDC_PSD, "PSD", 0);
        LoadDBCheckState(hwndDlg, IDC_SHOW_AUTH, "ShowAuth", 0);
//			LoadDBCheckState(hwndDlg, IDC_INV4INV, "Inv4Inv", DEFAULT_INV4INV_DISABLED);
//			LoadDBCheckState(hwndDlg, IDC_LOGSELFREM, "LogSelfRem", 0);
//			LoadDBCheckState(hwndDlg, IDC_IGNCHECKLOG, "IgnoreCheckLog", 0);
        LoadDBCheckState(hwndDlg, IDC_NOPSD_FOR_HIDDEN, "NoPSDForHidden", 1);
        bData = DBGetContactSettingByte(NULL,ICQ_PROTOCOL_NAME,"ASD", 0);
        CheckDlgButton(hwndDlg, IDC_ASD, bData);
//			icq_EnableMultipleControls(hwndDlg, icqASDControls, sizeof(icqASDControls)/sizeof(icqASDControls[0]), bData?TRUE:FALSE);
//			bData = DBGetContactSettingByte(NULL,gpszICQProtoName,"Inv4Inv", DEFAULT_INV4INV_DISABLED);
//		    CheckDlgButton(hwndDlg, IDC_INV4INV, bData);
//			bData = DBGetContactSettingByte(NULL,gpszICQProtoName,"ASDStartup", 0);
//		    CheckDlgButton(hwndDlg, IDC_ASDSTARTUP, bData);
        LoadDBCheckState(hwndDlg, IDC_TZER, "tZer", 0);
//            icq_EnableMultipleControls(hwndDlg, icqIncognitoControls, sizeof(icqIncognitoControls)/sizeof(icqIncognitoControls[0]), bStealthRequest?TRUE:FALSE);
        CheckDlgButton(hwndDlg, IDC_INCUSER, (bIncognitoGlobal == 0));
        CheckDlgButton(hwndDlg, IDC_INCGLOBAL, (bIncognitoGlobal == 1));
        if ( MIRANDA_VERSION > PLUGIN_MAKE_VERSION( 0, 8, 0, 0 ) )
        {
            CheckDlgButton(hwndDlg, IDC_INMAINMENU, (bPrivacyMenuPlacement == 0));
            CheckDlgButton(hwndDlg, IDC_INSTATUSMENU, (bPrivacyMenuPlacement == 1));
        }
        else
        {
            CheckDlgButton(hwndDlg, IDC_INMAINMENU, 1);
            EnableWindow( GetDlgItem( hwndDlg, IDC_INMAINMENU ), 0 );
            EnableWindow( GetDlgItem( hwndDlg, IDC_INSTATUSMENU ), 0 );
        }
        LoadDBCheckState( hwndDlg, IDC_USRCHKPOS, "UsrScanPos", 0 );
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_ASD:
            icq_EnableMultipleControls(hwndDlg, icqASDControls, sizeof(icqASDControls)/sizeof(icqASDControls[0]), IsDlgButtonChecked(hwndDlg, IDC_ASD));
            OptDlgChanged(hwndDlg);
            break;
        case IDC_PSD:
            icq_EnableMultipleControls(hwndDlg, icqPSDControls, sizeof(icqPSDControls)/sizeof(icqPSDControls[0]), IsDlgButtonChecked(hwndDlg, IDC_PSD));
            OptDlgChanged(hwndDlg);
            break;
//		  case IDC_STEALTHRQST:
//		  icq_EnableMultipleControls(hwndDlg, icqIncognitoControls, sizeof(icqIncognitoControls)/sizeof(icqIncognitoControls[0]), IsDlgButtonChecked(hwndDlg, IDC_STEALTHRQST));
        case IDC_ASDOPTIONS:
            ShowASDOpts();
            OptDlgChanged(hwndDlg);
            break;
        case IDC_NOPSD_FOR_HIDDEN:
        case IDC_INCUSER:
        case IDC_INCGLOBAL:
        case IDC_INMAINMENU:
        case IDC_INSTATUSMENU:
        case IDC_TZER:
        case IDC_SHOW_AUTH:
        case IDC_USRCHKPOS:
            OptDlgChanged(hwndDlg);
            break;
        default:
            return 0;
        }
        break;
    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case PSN_APPLY:
            if( gbASD != (BYTE)IsDlgButtonChecked(hwndDlg,IDC_ASD))
            {
                setSettingByte(NULL,"ASD",(BYTE)IsDlgButtonChecked(hwndDlg,IDC_ASD));
                gbASD = (BYTE)IsDlgButtonChecked(hwndDlg,IDC_ASD);
                gbASD?icq_InitISee():icq_ISeeCleanup();
                //here we need remove menu item from ASD
            }
            /*			if( DBGetContactSettingByte(NULL,gpszICQProtoName,"Inv4Inv", 0) != (BYTE)IsDlgButtonChecked(hwndDlg,IDC_INV4INV))
            		    {
            			  MessageBox(0,"To enable/disable Inv4Inv you must restart your Miranda","Warning",MB_OK);
            			  ICQWriteContactSettingByte(NULL,"Inv4Inv",(BYTE)IsDlgButtonChecked(hwndDlg,IDC_INV4INV));
            		    }*/
//		    StoreDBCheckState(hwndDlg, IDC_STEALTHRQST , "StealthRequest");
//			if(ICQGetContactSettingByte(NULL, "StealthRequest", 0) == 1)
//				bStealthRequest = TRUE;
//			else
//				bStealthRequest = FALSE;
            StoreDBCheckState(hwndDlg, IDC_SHOW_AUTH, "ShowAuth");
            bShowAuth = getSettingByte(NULL, "ShowAuth", 0);
            StoreDBCheckState(hwndDlg, IDC_PSD , "PSD");
            bPSD = getSettingByte(NULL, "PSD", 1);
            StoreDBCheckState(hwndDlg, IDC_NOPSD_FOR_HIDDEN, "NoPSDForHidden");
            bNoPSDForHidden = getSettingByte(NULL, "NoPSDForHidden", 1);
//			StoreDBCheckState(hwndDlg, IDC_INV4INV , "Inv4Inv");
            StoreDBCheckState(hwndDlg, IDC_INCGLOBAL , "IncognitoGlobal");
            StoreDBCheckState(hwndDlg, IDC_INSTATUSMENU , "PrivacyPlacement");
            StoreDBCheckState( hwndDlg, IDC_USRCHKPOS , "UsrScanPos" );
            bIncognitoGlobal = getSettingByte(NULL, "IncognitoGlobal", 0);
            bPrivacyMenuPlacement = getSettingByte(NULL, "PrivacyPlacement", 1);
            StoreDBCheckState(hwndDlg, IDC_TZER, "tZer");
            gbTzerEnabled = getSettingByte(NULL,"tZer",0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

static INT_PTR CALLBACK DlgProcIcqEventLogOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    extern BOOL bHcontactHistory;
    switch (msg)
    {
    case WM_INITDIALOG:
        ICQTranslateDialog(hwndDlg);

        LoadDBCheckState(hwndDlg, IDC_LOG_REMOVE_FILE, "LogSelfRemoveFile", 0);
        LoadDBCheckState(hwndDlg, IDC_LOG_IGNORECHECK_FILE, "LogIgnoreCheckFile", 0);
        LoadDBCheckState(hwndDlg, IDC_LOG_CHECKSTATUS_FILE, "LogStatusCheckFile", 0);
        LoadDBCheckState(hwndDlg, IDC_LOG_ASD_FILE, "LogASDFile", 0);
        LoadDBCheckState(hwndDlg, IDC_LOG_CLIENTCHANGE_FILE, "LogClientChangeFile", 0);
        LoadDBCheckState(hwndDlg, IDC_LOG_AUTH_FILE, "LogAuthFile", 0);
        LoadDBCheckState(hwndDlg, IDC_LOG_REQUEST_FILE, "LogRequestFile", 0);
        LoadDBCheckState(hwndDlg, IDC_LOG_READXSTATUS_FILE, "LogReadXStatusFile", 0);

        LoadDBCheckState(hwndDlg, IDC_LOG_REMOVE_HISTORY, "LogSelfRemoveHistory", 0);
        LoadDBCheckState(hwndDlg, IDC_LOG_IGNORECHECK_HISTORY, "LogIgnoreCheckHistory", 0);
        LoadDBCheckState(hwndDlg, IDC_LOG_CHECKSTATUS_HISTORY, "LogStatusCheckHistory", 0);
        LoadDBCheckState(hwndDlg, IDC_LOG_ASD_HISTORY, "LogASDHistory", 0);
        LoadDBCheckState(hwndDlg, IDC_LOG_CLIENTCHANGE_HISTORY, "LogClientChangeHistory", 0);
        LoadDBCheckState(hwndDlg, IDC_LOG_AUTH_HISTORY, "LogAuthHistory", 0);
        LoadDBCheckState(hwndDlg, IDC_LOG_REQUEST_HISTORY, "LogRequestHistory", 0);
        LoadDBCheckState(hwndDlg, IDC_LOG_READXSTATUS_HISTORY, "LogReadXStatusHistory", 0);

        LoadDBCheckState(hwndDlg, IDC_LOG_TO_HCONTACT_HISTORY, "LogToHcontact", 0);


        SetDlgItemTextA(hwndDlg, IDC_FILEPATH, getSettingStringUtf(NULL, ICQ_PROTOCOL_NAME, "EventsLog", "EventsLog.txt"));

        SendMessage(GetDlgItem(hwndDlg, IDC_BROWSE), BUTTONSETASFLATBTN, 0, 0);
        SendMessage(GetDlgItem(hwndDlg, IDC_BROWSE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadSkinnedIcon(SKINICON_EVENT_FILE));

        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BROWSE:
        {
            TCHAR str[MAX_PATH+2];
            OPENFILENAME ofn= {0};
            TCHAR filter[512],*pfilter;
            GetWindowText(GetWindow((HWND)lParam,GW_HWNDPREV),str,SIZEOF(str));
            ofn.lStructSize=CDSIZEOF_STRUCT(OPENFILENAME,lpTemplateName);
            ofn.hwndOwner=hwndDlg;
            ofn.Flags=OFN_HIDEREADONLY;
            if (LOWORD(wParam)==IDC_BROWSE)
            {
                ofn.lpstrTitle=TranslateT("Select where log file will be created");
            }
            else
            {
                ofn.Flags|=OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
                ofn.lpstrTitle=TranslateT("Select program to be run");
            }
            _tcscpy(filter,TranslateT("All Files"));
            _tcscat(filter,_T(" (*)"));
            pfilter=filter+_tcslen(filter)+1;
            _tcscpy(pfilter,_T("*"));
            pfilter=pfilter+_tcslen(pfilter)+1;
            *pfilter='\0';
            ofn.lpstrFilter=filter;
            ofn.lpstrFile=str;
            ofn.nMaxFile=SIZEOF(str)-2;
            ofn.nMaxFileTitle=MAX_PATH;
            if (LOWORD(wParam)==IDC_BROWSE)
            {
                if(!GetSaveFileName(&ofn)) return 1;
            }
            else
            {
                if(!GetOpenFileName(&ofn)) return 1;
            }
            SetWindowText(GetWindow((HWND)lParam,GW_HWNDPREV),str);
            OptDlgChanged(hwndDlg);
        }
        break;
        case IDC_LOG_REMOVE_FILE:
        case IDC_LOG_IGNORECHECK_FILE:
        case IDC_LOG_CHECKSTATUS_FILE:
        case IDC_LOG_CLIENTCHANGE_FILE:
        case IDC_LOG_AUTH_FILE:
        case IDC_LOG_REQUEST_FILE:
        case IDC_LOG_ASD_FILE:
        case IDC_LOG_READXSTATUS_FILE:
        case IDC_LOG_REMOVE_HISTORY:
        case IDC_LOG_IGNORECHECK_HISTORY:
        case IDC_LOG_CHECKSTATUS_HISTORY:
        case IDC_LOG_CLIENTCHANGE_HISTORY:
        case IDC_LOG_AUTH_HISTORY:
        case IDC_LOG_REQUEST_HISTORY:
        case IDC_LOG_ASD_HISTORY:
        case IDC_LOG_TO_HCONTACT_HISTORY:
        case IDC_LOG_READXSTATUS_HISTORY:
            OptDlgChanged(hwndDlg);
            break;
        case IDC_FILEPATH:
        {
            char i[1024];
            GetDlgItemTextA(hwndDlg, IDC_FILEPATH, i, sizeof(i));
            if(strstr(getSettingStringUtf(NULL, ICQ_PROTOCOL_NAME, "EventsLog", "EventsLog.txt"),i))
                OptDlgChanged(hwndDlg);
        }
        break;
        default:
            return 0;
        }
        break;
    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case PSN_APPLY:
            StoreDBCheckState(hwndDlg, IDC_LOG_REMOVE_FILE, "LogSelfRemoveFile");
            bLogSelfRemoveFile = getSettingByte(NULL, "LogSelfRemoveFile", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_IGNORECHECK_FILE, "LogIgnoreCheckFile");
            bLogIgnoreCheckFile= getSettingByte(NULL, "LogIgnoreCheckFile", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_CHECKSTATUS_FILE, "LogStatusCheckFile");
            bLogStatusCheckFile = getSettingByte(NULL, "LogStatusCheckFile", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_ASD_FILE, "LogASDFile");
            bLogASDFile = getSettingByte(NULL, "LogASDFile", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_CLIENTCHANGE_FILE, "LogClientChangeFile");
            bLogClientChangeFile = getSettingByte(NULL, "LogClientChangeFile", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_AUTH_FILE, "LogAuthFile");
            bLogAuthFile = getSettingByte(NULL, "LogAuthFile", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_REQUEST_FILE, "LogRequestFile");
            bLogInfoRequestFile = getSettingByte(NULL, "LogRequestFile", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_READXSTATUS_FILE, "LogReadXStatusFile");
            bLogReadXStatusFile = getSettingByte(NULL, "LogReadXStatusFile", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_REMOVE_HISTORY, "LogSelfRemoveHistory");
            bLogSelfRemoveHistory = getSettingByte(NULL, "LogSelfRemoveHistory", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_IGNORECHECK_HISTORY, "LogIgnoreCheckHistory");
            bLogIgnoreCheckHistory= getSettingByte(NULL, "LogIgnoreCheckHistory", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_CHECKSTATUS_HISTORY, "LogStatusCheckHistory");
            bLogStatusCheckHistory = getSettingByte(NULL, "LogStatusCheckHistory", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_ASD_HISTORY, "LogASDHistory");
            bLogASDHistory = getSettingByte(NULL, "LogASDHistory", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_CLIENTCHANGE_HISTORY, "LogClientChangeHistory");
            bLogClientChangeHistory = getSettingByte(NULL, "LogClientChangeHistory", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_AUTH_HISTORY, "LogAuthHistory");
            bLogAuthHistory = getSettingByte(NULL, "LogAuthHistory", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_REQUEST_HISTORY, "LogRequestHistory");
            bLogInfoRequestHistory = getSettingByte(NULL, "LogRequestHistory", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_READXSTATUS_HISTORY, "LogReadXStatusHistory");
            bLogReadXStatusHistory = getSettingByte(NULL, "LogReadXStatusHistory", 0);
            StoreDBCheckState(hwndDlg, IDC_LOG_TO_HCONTACT_HISTORY, "LogToHcontact");
            bHcontactHistory = getSettingByte(NULL, "LogToHcontact", 0);
            {
                char i[1024];
                GetDlgItemTextA(hwndDlg, IDC_FILEPATH, i, sizeof(i) );
                setSettingStringUtf(NULL, ICQ_PROTOCOL_NAME, "EventsLog", i);
            }
            return TRUE;
        }
        break;
    }
    return FALSE;
}

BOOL id = FALSE;
static const UINT icqVersionControls[] = {IDC_VERSION_CHECKBOX,IDC_SET_VERSION};
static INT_PTR CALLBACK DlgProcIcqClientIDOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static BYTE bIdChanged = FALSE;

    switch (msg)
    {
    case WM_INITDIALOG:
    {
        ICQTranslateDialog(hwndDlg);
        LoadDBCheckState(hwndDlg, IDC_HIDEID, "Hide ID", 1);
        LoadDBCheckState(hwndDlg, IDC_CUSTOM_CAP, "customcap", 1);
        LoadDBCheckState(hwndDlg,	IDC_VERSION_CHECKBOX, "CurrentVer", 0);
        id = FALSE;
        {
            char* CIdComboBox[] =
            {
                "MirandaIM","unknown","QIP2005","YSM","pyICQ","&RQ","Jimm","Trillian",
                "Licq","Kopete","ICQ for MAC","Miranda IM v6.6.6","ICQ 5.1 (Rambler)","ICQ 5.1","ICQ 5 (abv)",
                "ICQ 5 netvigator","Sim/MacOS X","Sim/Win32","Centericq","libicq2k","mChat","stICQ","KXicq2",
                "QIP PDA (Windows)","QIP Mobile (Symbian)","ICQ 2002","ICQ 6.5","ICQ for Pocket PC",
                "Anastasia","Virus","alicq","mICQ","StrICQ","vICQ","IM2","GAIM","ICQ99","WebICQ","SmartICQ",
                "IM+","uIM","TICQClient","IC@","PreludeICQ","Qnext","ICQ Lite","QIP Infium","JICQ",
                "SpamBot","MIP","Trillian Astra","R&Q","NanoICQ","IMadering","MirandaMobile",
                "D[i]Chat","LocID","BayanICQ","Core Pager"
            };
            int i;
            HWND hCombo = GetDlgItem(hwndDlg, IDC_CHANGEID);
            SendMessage(hCombo, CB_RESETCONTENT, 0, 0);
            for (i=0; i<sizeof(CIdComboBox)/sizeof(CIdComboBox[0]); i++)
                ComboBoxAddStringUtf(hCombo, CIdComboBox[i],i);
        }
        // Miranda IM ICQ mods
        {
            char* MIMIdComboBox[] =
            {
                "original (Joe@Whale)", "BM Mod", "S7&SSS Mod",
                "S!N Mod", "Plus Mod", "eternity/PlusPlus++"
            };
            int i;
            HWND hCombo = GetDlgItem(hwndDlg, IDC_ENIFMIRCBX);
            SendMessage(hCombo, CB_RESETCONTENT, 0, 0);
            for (i=0; i<sizeof(MIMIdComboBox)/sizeof(MIMIdComboBox[0]); i++)
                ComboBoxAddStringUtf(hCombo, MIMIdComboBox[i],i);
        }
        SendDlgItemMessage(hwndDlg, IDC_ENIFMIRCBX, CB_SETCURSEL, (DBGetContactSettingWord(NULL,ICQ_PROTOCOL_NAME,"CurrentICQModID",0)),0);
        if(DBGetContactSettingWord(NULL,ICQ_PROTOCOL_NAME,"CurrentID",0))
            SetDlgItemText(hwndDlg, IDC_FAKEWARNING, TranslateT("Your client ID is changed!\nChanging client id can result in serious messaging problems!\n\nIf you experience such problems, set your client ID back to \"Miranda\""));
        else
            bIdChanged = TRUE;

        SendDlgItemMessage(hwndDlg, IDC_CHANGEID, CB_SETCURSEL, (DBGetContactSettingWord(NULL,ICQ_PROTOCOL_NAME,"CurrentID",0)),0);
        EnableWindow(GetDlgItem(hwndDlg, IDC_SET_VERSION), IsDlgButtonChecked(hwndDlg, IDC_VERSION_CHECKBOX));
        EnableWindow(GetDlgItem(hwndDlg, IDC_SET_VERSION_RANGE), IsDlgButtonChecked(hwndDlg, IDC_VERSION_CHECKBOX));
        SetDlgItemInt(hwndDlg, IDC_SET_VERSION, (DBGetContactSettingWord(NULL,ICQ_PROTOCOL_NAME,"setVersion", 0)), FALSE );
        {
            char tmp[MAXMODULELABELLENGTH];
            mir_snprintf(tmp, MAXMODULELABELLENGTH, "%sCaps", ICQ_PROTOCOL_NAME);
            SetDlgItemTextA(hwndDlg, IDC_CUSTCAPEDIT, getSettingStringUtf(NULL,tmp,"capability", 0));
        }
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_CHANGEID:
        {
            int curID;
            curID = SendDlgItemMessage(hwndDlg, IDC_CHANGEID, CB_GETCURSEL, 0, 0);
            EnableWindow( GetDlgItem( hwndDlg, IDC_ENIFMIR ), curID == 0 );
            EnableWindow( GetDlgItem( hwndDlg, IDC_ENIFMIRCBX ), curID == 0 );
        }
        OptDlgChanged(hwndDlg);
        break;
        case IDC_CAPSBOX:
            ShowCapsListDialog( hwndDlg );
            return TRUE;
        case IDC_SET_VERSION:
        case IDC_VERSION_CHECKBOX:
        {
            BOOL ticked = IsDlgButtonChecked( hwndDlg, IDC_VERSION_CHECKBOX );
            EnableWindow( GetDlgItem( hwndDlg, IDC_SET_VERSION ), ticked );
            EnableWindow( GetDlgItem( hwndDlg, IDC_SET_VERSION_RANGE ), ticked );
        }
        OptDlgChanged(hwndDlg);
        break;
        case IDC_CUSTOM_CAP:
        {
            BOOL ticked = IsDlgButtonChecked( hwndDlg, IDC_CUSTOM_CAP );
            EnableWindow( GetDlgItem( hwndDlg, IDC_CAPSBOX ), ticked );
        }
        OptDlgChanged(hwndDlg);
        break;
        case IDC_HIDEID:
        case IDC_ENIFMIRCBX:
            OptDlgChanged(hwndDlg);
            break;
        default:
            return 0;
        }
        break;
    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case PSN_APPLY:
        {
            int i, fakeId, fakeICQModID;
            fakeId = SendDlgItemMessage(hwndDlg, IDC_CHANGEID, CB_GETCURSEL, 0, 0);
            if(DBGetContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "CurrentID", 0) != fakeId)
            {
                id = TRUE;
                DBWriteContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "CurrentID", (WORD)fakeId);
            }
            fakeICQModID = SendDlgItemMessage(hwndDlg, IDC_ENIFMIRCBX, CB_GETCURSEL, 0, 0);
            if(DBGetContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "CurrentICQModID", 0) != fakeICQModID)
            {
                id = TRUE;
                DBWriteContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "CurrentICQModID", (WORD)fakeICQModID);
            }
            StoreDBCheckState(hwndDlg, IDC_VERSION_CHECKBOX, "CurrentVer");
            gbVerEnabled = DBGetContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "CurrentVer", 0);
            i = GetDlgItemInt(hwndDlg, IDC_SET_VERSION, 0, 0);
            if(DBGetContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "setVersion", 0) != i)
            {
                id = TRUE;
                DBWriteContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "setVersion", (WORD)i);
            }
            i = IsDlgButtonChecked(hwndDlg, IDC_HIDEID);
            if(DBGetContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "Hide ID", 0) != i)
            {
                id = TRUE;
                DBWriteContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "Hide ID", i);
            }
            if(DBGetContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "Hide ID", 0))
                gbHideIdEnabled = 1;
            else
                gbHideIdEnabled = 0;

            i = IsDlgButtonChecked(hwndDlg, IDC_CUSTOM_CAP);
            if(DBGetContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "customcap", 0) != i)
            {
                id = TRUE;
                DBWriteContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "customcap", i);
                gbCustomCapEnabled=i;
            }
            if (id)
            {
                setUserInfo();
                {
                    extern int icqGoingOnlineStatus;
                    icq_packet packet;
                    WORD wStatus;
                    DWORD dwFT1;
                    DWORD dwFT2;
                    DWORD dwFT3;
                    int nPort = getSettingWord(NULL, "UserPort", 0);
                    DWORD dwDirectCookie = rand() ^ (rand() << 16);
                    // Get status
                    wStatus = MirandaStatusToIcq(icqGoingOnlineStatus);
                    wStatus = MirandaStatusToIcq(icqGoingOnlineStatus);
                    serverPacketInit(&packet, 71);
                    packFNACHeader(&packet, ICQ_SERVICE_FAMILY, ICQ_CLIENT_SET_STATUS);
                    packDWord(&packet, 0x00060004);             // TLV 6: Status mode and security flags
                    packWord(&packet, GetMyStatusFlags());      // Status flags
                    packWord(&packet, wStatus);                 // Status
                    packTLVWord(&packet, 0x0008, 0x0000);       // TLV 8: Error code
                    packDWord(&packet, 0x000c0025);             // TLV C: Direct connection info
                    packDWord(&packet, getSettingDword(NULL, "RealIP", 0));
                    packDWord(&packet, nPort);
                    packByte(&packet, DC_TYPE);                 // TCP/FLAG firewall settings
                    packWord(&packet, (WORD)GetProtoVersion());
                    packDWord(&packet, dwDirectCookie);         // DC Cookie
                    packDWord(&packet, WEBFRONTPORT);           // Web front port
                    packDWord(&packet, CLIENTFEATURES);         // Client features
                    SetTimeStamps(&dwFT1, &dwFT2, &dwFT3);
                    packDWord(&packet, dwFT1);
                    packDWord(&packet, dwFT2);
                    packDWord(&packet, dwFT3);
                    packWord(&packet, 0x0000);                  // Unknown
                    packTLVWord(&packet, 0x001F, 0x0000);
                    sendServPacket(&packet);
                }
            }
            if(fakeId != 0)
                SetDlgItemText(hwndDlg, IDC_FAKEWARNING, TranslateT("Your client ID is changed!\nChanging client id can result in serious messaging problems!\n\nIf you experience such problems, set your client ID back to \"Miranda\""));
            else
                SetDlgItemText(hwndDlg, IDC_FAKEWARNING, _T(""));
            if(bIdChanged && (fakeId != 0))
            {
                MessageBox(0,TranslateT("Changing client ID can result in serious messaging problems!"),TranslateT("Warning"),MB_OK);
                bIdChanged = FALSE;
            }
        }
        // custom capability edit control
        /*			{
        				char cap[24];
        				char tmp[MAXMODULELABELLENGTH];
        				mir_snprintf(tmp, MAXMODULELABELLENGTH, "%sCaps", gpszICQProtoName);
        				GetDlgItemText(hwndDlg, IDC_CUSTCAPEDIT, cap, sizeof(cap));
        				DBWriteContactSettingStringUtf(NULL, tmp, "capability", cap);
        			}*/
        return TRUE;
        }
        break;
    }
    return FALSE;
}


static const UINT icqContactsControls[] = {IDC_ADDSERVER,IDC_SERVERADD,IDC_LOADFROMSERVER,IDC_SAVETOSERVER,IDC_UPLOADNOW,IDC_FORCEREFRESH};
static const UINT icqAvatarControls[] = {IDC_AUTOLOADAVATARS,IDC_BIGGERAVATARS,IDC_STRICTAVATARCHECK};
static const UINT icqAddTempControls[] = {IDC_ADDTEMP,IDC_TMP_CONTACTS_GROUP,IDC_TMPREQAUTH,IDC_TMPSNDADDED};
static INT_PTR CALLBACK DlgProcIcqContactsOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        ICQTranslateDialog(hwndDlg);
        LoadDBCheckState(hwndDlg, IDC_ENABLE, "UseServerCList", DEFAULT_SS_ENABLED);
        LoadDBCheckState(hwndDlg, IDC_ADDSERVER, "ServerAddRemove", DEFAULT_SS_ADDSERVER);
        LoadDBCheckState(hwndDlg, IDC_SERVERADD, "ServerAddClist", 0);
        LoadDBCheckState(hwndDlg, IDC_LOADFROMSERVER, "LoadServerDetails", DEFAULT_SS_LOAD);
        LoadDBCheckState(hwndDlg, IDC_SAVETOSERVER, "StoreServerDetails", DEFAULT_SS_STORE);
        LoadDBCheckState(hwndDlg, IDC_ENABLEAVATARS, "AvatarsEnabled", DEFAULT_AVATARS_ENABLED);
        LoadDBCheckState(hwndDlg, IDC_AUTOLOADAVATARS, "AvatarsAutoLoad", DEFAULT_LOAD_AVATARS);
        LoadDBCheckState(hwndDlg, IDC_BIGGERAVATARS, "AvatarsAllowBigger", DEFAULT_BIGGER_AVATARS);
        LoadDBCheckState(hwndDlg, IDC_STRICTAVATARCHECK, "StrictAvatarCheck", DEFAULT_AVATARS_CHECK);

        icq_EnableMultipleControls(hwndDlg, icqContactsControls, sizeof(icqContactsControls)/sizeof(icqContactsControls[0]),
                                   getSettingByte(NULL, "UseServerCList", DEFAULT_SS_ENABLED)?TRUE:FALSE);
        icq_EnableMultipleControls(hwndDlg, icqAvatarControls, sizeof(icqAvatarControls)/sizeof(icqAvatarControls[0]),
                                   getSettingByte(NULL, "AvatarsEnabled", DEFAULT_AVATARS_ENABLED)?TRUE:FALSE);
        CheckDlgButton(hwndDlg, IDC_DELETE_TMP_CONTACTS, (bTmpContacts == 0));
        CheckDlgButton(hwndDlg, IDC_ADD_TMP_CONTACTS, (bTmpContacts == 1));
        SetDlgItemTextA(hwndDlg, IDC_TMP_CONTACTS_GROUP, getSettingStringUtf(NULL,ICQ_PROTOCOL_NAME,"TmpContactsGroup", ""));
        LoadDBCheckState(hwndDlg, IDC_ADDTEMP, "AddTemp", 0);
        LoadDBCheckState(hwndDlg, IDC_TMPREQAUTH, "TmpReqAuth", 1);
        LoadDBCheckState(hwndDlg, IDC_TMPSNDADDED, "TmpSndAdded", 1);
        LoadDBCheckState(hwndDlg, IDC_REMOVE_TEMP, "RemoveTmpContacts", 0);
        icq_EnableMultipleControls(hwndDlg, icqAddTempControls, sizeof(icqAddTempControls)/sizeof(icqAddTempControls[0]), bTmpContacts?TRUE:FALSE);

        if (icqOnline)
        {
            ShowWindow(GetDlgItem(hwndDlg, IDC_OFFLINETOENABLE), SW_SHOW);
            EnableDlgItem(hwndDlg, IDC_ENABLE, FALSE);
            EnableDlgItem(hwndDlg, IDC_ENABLEAVATARS, FALSE);
        }
        else
        {
            EnableDlgItem(hwndDlg, IDC_UPLOADNOW, FALSE);
        }
        if (!DBGetContactSettingDword(NULL,ICQ_PROTOCOL_NAME,"SrvLastUpdate",0) &&
                !DBGetContactSettingWord(NULL,ICQ_PROTOCOL_NAME,"SrvRecordCount",0))
            EnableWindow(GetDlgItem(hwndDlg, IDC_FORCEREFRESH), FALSE);

        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_UPLOADNOW:
            ShowUploadContactsDialog();
            break;
        case IDC_FORCEREFRESH:
            DBWriteContactSettingDword(NULL,ICQ_PROTOCOL_NAME,"SrvLastUpdate",0);
            DBWriteContactSettingWord(NULL,ICQ_PROTOCOL_NAME,"SrvRecordCount",0);
            EnableWindow(GetDlgItem(hwndDlg, IDC_FORCEREFRESH), FALSE);
            OptDlgChanged(hwndDlg);
            break;
        case IDC_ENABLE:
            icq_EnableMultipleControls(hwndDlg, icqContactsControls, sizeof(icqContactsControls)/sizeof(icqContactsControls[0]), IsDlgButtonChecked(hwndDlg, IDC_ENABLE));
            if (icqOnline)
                ShowWindow(GetDlgItem(hwndDlg, IDC_RECONNECTREQD), SW_SHOW);
            else
                EnableDlgItem(hwndDlg, IDC_UPLOADNOW, FALSE);
            OptDlgChanged(hwndDlg);
            break;
        case IDC_ENABLEAVATARS:
            icq_EnableMultipleControls(hwndDlg, icqAvatarControls, sizeof(icqAvatarControls)/sizeof(icqAvatarControls[0]), IsDlgButtonChecked(hwndDlg, IDC_ENABLEAVATARS));
            OptDlgChanged(hwndDlg);
            break;
        case IDC_ADD_TMP_CONTACTS:
            icq_EnableMultipleControls(hwndDlg, icqAddTempControls, sizeof(icqAddTempControls)/sizeof(icqAddTempControls[0]), IsDlgButtonChecked(hwndDlg, IDC_ADD_TMP_CONTACTS));
            OptDlgChanged(hwndDlg);
            break;
        case IDC_DELETE_TMP_CONTACTS:
            icq_EnableMultipleControls(hwndDlg, icqAddTempControls, sizeof(icqAddTempControls)/sizeof(icqAddTempControls[0]), IsDlgButtonChecked(hwndDlg, IDC_ADD_TMP_CONTACTS));
            OptDlgChanged(hwndDlg);
            break;
        case IDC_ADDSERVER:
        case IDC_LOADFROMSERVER:
        case IDC_SAVETOSERVER:
        case IDC_ADDTEMP:
        case IDC_TMPSNDADDED:
        case IDC_TMPREQAUTH:
        case IDC_REMOVE_TEMP:
        case IDC_TMP_CONTACTS_GROUP:
        case IDC_STRICTAVATARCHECK:
        case IDC_AUTOLOADAVATARS:
        case IDC_BIGGERAVATARS:
            OptDlgChanged(hwndDlg);
            break;
        default:
            return 0;
        }
        break;
    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case PSN_APPLY:
            StoreDBCheckState(hwndDlg, IDC_ENABLE, "UseServerCList");
            StoreDBCheckState(hwndDlg, IDC_ADDSERVER, "ServerAddRemove");
            StoreDBCheckState(hwndDlg, IDC_SERVERADD, "ServerAddClist");
            StoreDBCheckState(hwndDlg, IDC_LOADFROMSERVER, "LoadServerDetails");
            StoreDBCheckState(hwndDlg, IDC_SAVETOSERVER, "StoreServerDetails");
            StoreDBCheckState(hwndDlg, IDC_ENABLEAVATARS, "AvatarsEnabled");
            StoreDBCheckState(hwndDlg, IDC_AUTOLOADAVATARS, "AvatarsAutoLoad");
            StoreDBCheckState(hwndDlg, IDC_BIGGERAVATARS, "AvatarsAllowBigger");
            StoreDBCheckState(hwndDlg, IDC_STRICTAVATARCHECK, "StrictAvatarCheck");
            StoreDBCheckState(hwndDlg, IDC_REMOVE_TEMP, "RemoveTmpContacts");
            if (IsDlgButtonChecked(hwndDlg, IDC_DELETE_TMP_CONTACTS))
            {
                setSettingByte(NULL, "TempContacts", 0);
                bTmpContacts = 0;
            }
            else if (IsDlgButtonChecked(hwndDlg, IDC_ADD_TMP_CONTACTS))
            {
                setSettingByte(NULL, "TempContacts", 1);
                bTmpContacts = 1;
            }
            {
                static char NewTmpGroupName[128] = {0},
                                                   CurrentTmpGroupName[128] = {0};
                GetDlgItemTextA(hwndDlg, IDC_TMP_CONTACTS_GROUP, NewTmpGroupName, sizeof(NewTmpGroupName));
                strcpy(CurrentTmpGroupName, getSettingStringUtf(NULL, ICQ_PROTOCOL_NAME, "TmpContactsGroup", "0"));
                if(strcmp(CurrentTmpGroupName, NewTmpGroupName) != 0)
                {
                    int GroupNumber = 0;
                    BYTE GroupExist = 0;
                    char szNumber[32] = {0}, szValue[96] = {0};
                    extern int CreateCListGroup(const char* szGroupName);
                    strcpy(szNumber, "0");
                    while(strcmp(getSettingStringUtf(NULL, "CListGroups", szNumber, "0"), "0") != 0)
                    {
#if defined(_MSC_VER) && _MSC_VER >= 1300
                        _itoa_s(GroupNumber, szNumber, sizeof(szNumber), 10);
#else
                        _itoa(GroupNumber, szNumber, 10);
#endif
                        strcpy(szValue, getSettingStringUtf(NULL, "CListGroups", szNumber, "0"));
                        if(strcmp(NewTmpGroupName, szValue + 1) == 0)
                        {
                            GroupExist = 1;
                            break;
                        }
                        GroupNumber++;
                    }
                    setSettingStringUtf(NULL,ICQ_PROTOCOL_NAME, "TmpContactsGroup", NewTmpGroupName);
                    TmpGroupName = getSettingStringUtf(NULL,ICQ_PROTOCOL_NAME,"TmpContactsGroup", Translate("General"));
                    if(!GroupExist)
                        CreateCListGroup(TmpGroupName);
                }
            }
            StoreDBCheckState(hwndDlg, IDC_ADDTEMP, "AddTemp");
            bAddTemp = getSettingByte(NULL, "AddTemp", 0);
            StoreDBCheckState(hwndDlg, IDC_TMPREQAUTH, "TmpReqAuth");
            bTmpAuthRequet = getSettingByte(NULL, "TmpReqAuth", 1);
            StoreDBCheckState(hwndDlg, IDC_TMPSNDADDED, "TmpSndAdded");
            bTmpSendAdded = getSettingByte(NULL, "TmpSndAdded", 1);
            return TRUE;
        }
        break;
    }
    return FALSE;
}



static INT_PTR CALLBACK DlgProcIcqNewUINOpts(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        ICQTranslateDialog(hwndDlg);
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_LOOKUPLINK:
            CallService(MS_UTILS_OPENURL, 1, (LPARAM)URL_FORGOT_PASSWORD);
            return TRUE;

        case IDC_NEWUINLINK:
            CallService(MS_UTILS_OPENURL, 1, (LPARAM)URL_REGISTER_UIN);
            return TRUE;

        case IDC_PICTURE:
            if(icqOnline)
            {
                if (MessageBox(0,TranslateT("If you continue, you will lose current connection with a server.\nContinue?"),TranslateT("Warning"),MB_YESNO) == IDYES)
                    icq_requestRegImage(hwndDlg);
            }
            else
                icq_requestRegImage(hwndDlg);
            break;
        case IDC_REGISTER:
        {
            char password[128];
            char regimage[128];
            GetDlgItemTextA(hwndDlg, IDC_NEWUIN_PASS, password, sizeof(password));
            GetDlgItemTextA(hwndDlg, IDC_PICTURE_CONTENT, regimage, sizeof(regimage));
            icq_registerNewUin(password, regimage);
        }
        break;
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwndDlg);
        break;

    case WM_DESTROY:
        hwndRegUin = NULL;
        break;
    }
    return FALSE;
}


void ShowRegUinDialog(void)
{
    if (hwndRegUin == NULL)
    {
        hwndRegUin = CreateDialogUtf(hInst, MAKEINTRESOURCEA(IDD_ICQ_REG_UIN), NULL, DlgProcIcqNewUINOpts);
    }
    SetForegroundWindow(hwndRegUin);
}

void ShowPopUpsOpts(void)
{
    if (hwndPopUpsOpts == NULL)
    {
        hwndPopUpsOpts = CreateDialogUtf(hInst, MAKEINTRESOURCEA(IDD_OPT_POPUPS2), NULL, DlgProcIcqPopupOpts2);
    }
    SetForegroundWindow(hwndPopUpsOpts);
}
void ShowASDOpts(void)
{
    if (hwndASDOpts == NULL)
    {
        hwndASDOpts = CreateDialogUtf(hInst, MAKEINTRESOURCEA(IDD_OPT_ASD), NULL, DlgProcIcqASDOpts);
    }
    SetForegroundWindow(hwndASDOpts);
}



// password input box subclassing
typedef struct
{
    WNDPROC oldWndProc;
    HWND hwndTip;
} TPasswordBoxData;

static void IcqPasswordBox_Destroy(HWND hwndDlg, int idCtrl);

static LRESULT CALLBACK IcqPasswordBox_WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    WNDPROC oldWndProc;
    TPasswordBoxData *dat = (TPasswordBoxData *)GetWindowLong(hwnd, GWL_USERDATA);
    if (!dat) return DefWindowProc(hwnd, msg, wParam, lParam);
    oldWndProc = dat->oldWndProc;

    switch(msg)
    {
    case WM_APP:
    {
        if (dat->hwndTip)
        {
            SendMessage(dat->hwndTip, TTM_TRACKACTIVATE, FALSE, 0);
            DestroyWindow(dat->hwndTip);
            dat->hwndTip = 0;
        }

        if (wParam && lParam)
        {
            TOOLINFO ti = {0};
            RECT rc;

            dat->hwndTip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP|TTS_NOPREFIX|TTS_BALLOON, 0, 0, 0, 0, hwnd, NULL, hInst, 0);

            ti.cbSize = sizeof(ti);
            ti.lpszText = (TCHAR *)lParam;
            ti.hinst = hInst;
            ti.hwnd = hwnd;
            ti.uFlags = TTF_TRACK|TTF_IDISHWND|TTF_TRANSPARENT;
            ti.uId = (UINT_PTR)hwnd;
            SendMessage(dat->hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);

            GetWindowRect(hwnd, &rc);
            SendMessage(dat->hwndTip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(rc.left+20, rc.bottom));
            SendMessage(dat->hwndTip, TTM_SETTITLE, 1, (LPARAM)wParam);
            SendMessage(dat->hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
        }

        break;
    }

    case WM_KILLFOCUS:
    {
        if (dat->hwndTip)
        {
            SendMessage(dat->hwndTip, TTM_TRACKACTIVATE, FALSE, 0);
            DestroyWindow(dat->hwndTip);
            dat->hwndTip = 0;
        }

        break;
    }

    case WM_DESTROY:
    {
        IcqPasswordBox_Destroy(GetParent(hwnd), GetWindowLong(hwnd, GWL_ID));
        break;
    }
    }

    return CallWindowProcUtf(oldWndProc, hwnd, msg, wParam, lParam);
}

static void IcqPasswordBox_Init(HWND hwndDlg, int idCtrl)
{
    HWND hwnd = GetDlgItem(hwndDlg, idCtrl);

    if(GetWindowLong(hwnd, GWL_USERDATA)) return;

    if (IsWinVer2000Plus())
    {
        TPasswordBoxData *dat = (TPasswordBoxData *)icq_alloc_zero(sizeof(TPasswordBoxData));
        dat->hwndTip = NULL;
        SetWindowLong(hwnd, GWL_USERDATA, (LONG)dat);
        dat->oldWndProc = (WNDPROC)SetWindowLongUtf(hwnd, GWL_WNDPROC, (LONG)IcqPasswordBox_WndProc);
    }
}

static void IcqPasswordBox_Destroy(HWND hwndDlg, int idCtrl)
{
    HWND hwnd = GetDlgItem(hwndDlg, idCtrl);
    TPasswordBoxData *dat = (TPasswordBoxData *)GetWindowLong(hwnd, GWL_USERDATA);
    SetWindowLongUtf(hwnd, GWL_WNDPROC, (LONG)dat->oldWndProc);
    SetWindowLong(hwnd, GWL_USERDATA, 0);
    if (dat->hwndTip)
    {
        SendMessage(dat->hwndTip, TTM_TRACKACTIVATE, FALSE, 0);
        DestroyWindow(dat->hwndTip);
        dat->hwndTip = 0;
    }
    mir_free(dat);
}
