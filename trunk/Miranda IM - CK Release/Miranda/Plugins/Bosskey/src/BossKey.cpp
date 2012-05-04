/*
BossKey plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author 
			Copyright (C) 2002-2012 Sergey V. Gershovich a.k.a. Jazzy$

Copyright 2000-2012 Miranda IM project,
all portions of this codebase are copyrighted to the people
listed in contributors.txt.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

===============================================================================

File name      : $HeadURL: 
Revision       : $Revision: 
Last change on : $Date: 
Last change by : $Author:
$Id$		   : $Id$:

===============================================================================
*/

#include "../BossKey.h"
#include "m_updater.h"
#include "m_msg_buttonsbar.h"

/* Multiple instances support:
   when hotkey is hit, handler notifies all listen windows
// #pragma data_seg("Shared") // Shared data segment
   these must be shared, since they're called by the hook (The hook is global and called from the context of each process)
   HHOOK g_hKeyHook = NULL; // shared
   this isn't referenced by hook, but should be shared to keep a reference count of multiple instances
   WORD g_wRefCount = 0; // reference count. when this is 0 on init, the hook is created. when this is 0 on destruction, the hook is destroyed.
//#pragma data_seg() // end of shared data segment
*/

// unique to this DLL, not to be shared 
HINSTANCE g_hInstance;
PLUGINLINK *pluginLink;
MM_INTERFACE mmi;
CLIST_INTERFACE *pcli;
HANDLE g_hmGenMenuInit, g_hIcon, g_hMenuItem, g_hHideService, g_hIsHiddenService;
HANDLE g_hHooks[7];
HWINEVENTHOOK g_hWinHook;
HWND g_hListenWindow, hDlg, g_hDlgPass, hOldForegroundWindow;
HWND_ITEM *g_pMirWnds; // a pretty simple linked list
HMODULE hWTS, hDwmApi;
DWORD g_dwMirandaPID;
WORD g_wMask, g_wMaskAdv;
bool g_bWindowHidden, g_fKeyPressed, g_fPassRequested, g_TrayIcon;
char g_password[MAXPASSLEN + 1];
HKL oldLangID, oldLayout;
int protoCount;
PROTOCOLDESCRIPTOR **proto;
unsigned *oldStatus;
TCHAR **oldStatusMsg;
BYTE g_bOldSetting;
int hLangpack;

PWTSRegisterSessionNotification wtsRegisterSessionNotification ; 
PWTSUnRegisterSessionNotification wtsUnRegisterSessionNotification;
PFNDwmIsCompositionEnabled dwmIsCompositionEnabled;

void LanguageChanged(HWND hDlg);

static PLUGININFOEX pluginInfo = {
	sizeof(PLUGININFOEX),
	__PLUGIN_NAME,
	__VERSION_DWORD,
	__DESC,
	__AUTHORS,
	__EMAIL,
	__COPYRIGHTS,
	__FL __PLUGIN_ID_STR,
	UNICODE_AWARE,
	0,
#if defined( _UNICODE )
	//4fac353d-0a36-44a4-9064-6759c53ae782
	{0x4fac353d, 0x0a36, 0x44a4, { 0x90, 0x64, 0x67, 0x59, 0xc5, 0x3a, 0xe7, 0x82 }}
#else
	//0be17104-518f-411c-9bb7-7c1042a2ed02
	{0x0be17104, 0x518f, 0x411c, { 0x9b, 0xb7, 0x7c, 0x10, 0x42, 0xa2, 0xed, 0x02 }}
#endif
};

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	g_hInstance = hinstDLL;
	return(true);
}

extern "C" __declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
		if ( mirandaVersion < PLUGIN_MAKE_VERSION(0,9,0,0)) {
		MessageBox( NULL, _T("The BossKey+ cannot be loaded. It requires Miranda IM 0.9 or later."), _T("BossKey+"), MB_OK|MB_ICONWARNING|MB_SETFOREGROUND|MB_TOPMOST );
		return NULL;
	}
	pluginInfo.cbSize = sizeof(PLUGININFOEX);
	return &pluginInfo;
}


static const MUUID interfaces[] = { MIID_BOSSKEY, MIID_LAST };
extern "C" __declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}

static BOOL IsAeroMode()
{
	BOOL result;
	return dwmIsCompositionEnabled && (dwmIsCompositionEnabled(&result) == S_OK) && result;
}

INT_PTR CALLBACK DlgStdInProc(HWND hDlg, UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	static DWORD dwOldIcon = 0;
	HICON hIcon = 0;
	
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			g_hDlgPass = hDlg;
			hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_DLGPASSWD));
			dwOldIcon = SetClassLongPtr(hDlg, GCLP_HICON, (LONG)hIcon); // set alt+tab icon
			SendDlgItemMessage(hDlg,IDC_EDIT1,EM_LIMITTEXT,MAXPASSLEN,0);

			if (IsAeroMode())
			{
				SetWindowLongPtr(hDlg, GWL_STYLE, GetWindowLongPtr(hDlg, GWL_STYLE) | WS_DLGFRAME | WS_SYSMENU);
				
				RECT rect;
				GetClientRect(hDlg, &rect);
				SetWindowPos(hDlg, 0, 0, 0, rect.right, rect.bottom +
					GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXSIZEFRAME), 
					SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER);
			}
			SendMessage(GetDlgItem(hDlg, IDC_HEADERBAR), WM_SETICON, 0, (LPARAM)hIcon);
			SetWindowText(GetDlgItem(hDlg, IDC_HEADERBAR), _T("Miranda IM is locked.\nEnter password to unlock it."));

			TranslateDialogDefault(hDlg);
			oldLangID = 0;
			SetTimer(hDlg,1,200,NULL);

			oldLayout = GetKeyboardLayout(0);		
			if (MAKELCID((WORD)oldLayout & 0xffffffff,  SORT_DEFAULT) != (LCID)0x00000409) 
				ActivateKeyboardLayout((HKL)0x00000409, 0);
			LanguageChanged(hDlg);
			return TRUE;
		}

	case WM_CTLCOLORSTATIC:
		{
			if (GetWindowLongPtr((HWND)lParam, GWLP_ID) == IDC_LANG)
			{
				SetTextColor((HDC)wParam, GetSysColor(COLOR_HIGHLIGHTTEXT));
				SetBkMode((HDC)wParam, TRANSPARENT);
				return (INT_PTR)GetSysColorBrush(COLOR_HIGHLIGHT);
			}
			return FALSE;
		}

	case WM_COMMAND:
		{
			UINT uid = LOWORD(wParam);
			if(uid == IDOK){
				char password[MAXPASSLEN + 1] = {0};
				int passlen = GetDlgItemTextA(hDlg,IDC_EDIT1,password,MAXPASSLEN+1);

				if (passlen == 0)
				{
					SetWindowText(GetDlgItem(hDlg, IDC_HEADERBAR), TranslateT("Miranda IM is locked.\nEnter password to unlock it."));
					SendMessage(GetDlgItem(hDlg, IDC_HEADERBAR), WM_NCPAINT, 0, 0);
				}
				else if (lstrcmpA(password, g_password))
				{
					SetWindowText(GetDlgItem(hDlg, IDC_HEADERBAR), TranslateT("Password is not correct!\nPlease, enter correct password."));
					SendMessage(GetDlgItem(hDlg, IDC_HEADERBAR), WM_NCPAINT, 0, 0);
					SetDlgItemTextA(hDlg, IDC_EDIT1, "");
				}
				else EndDialog(hDlg,IDOK);

			}else if (uid == IDCANCEL)
				EndDialog(hDlg,IDCANCEL);
		}

	case WM_TIMER:
		{
			LanguageChanged(hDlg);
			return FALSE;
		}
	case WM_DESTROY:
		{
			KillTimer(hDlg, 1);
			if (GetKeyboardLayout(0) != oldLayout) 
				ActivateKeyboardLayout(oldLayout, 0);
			SetClassLongPtr(hDlg, GCLP_HICON, (long)dwOldIcon);
			DestroyIcon(hIcon);
			return FALSE;
		}

	}return FALSE;
}

static void LanguageChanged(HWND hDlg)
{
	HKL LangID = GetKeyboardLayout(0);
	char Lang[3] = {0};
	if (LangID != oldLangID)
	{
		oldLangID = LangID;
		GetLocaleInfoA(MAKELCID(((WORD)LangID & 0xffffffff),  SORT_DEFAULT),  LOCALE_SABBREVLANGNAME, Lang, 2);
		Lang[0] = toupper(Lang[0]);
		Lang[1] = tolower(Lang[1]);
		SetDlgItemTextA(hDlg, IDC_LANG, Lang);
	}
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd,LPARAM lParam)
{
	DWORD dwWndPID;
	GetWindowThreadProcessId(hWnd,&dwWndPID);

	if ((g_dwMirandaPID == dwWndPID) && hWnd != g_hDlgPass /* && (GetWindowLongPtr(hWnd,GWLP_HWNDPARENT) == NULL)*/ && (IsWindowVisible(hWnd)))
	{
		char szTemp[32];
		GetClassNameA(hWnd,szTemp,32);

		if (lstrcmpA(szTemp,"MirandaThumbsWnd") == 0) // hide floating contacts
		{
			CallService("FloatingContacts/MainHideAllThumbs",0,0);
			g_bOldSetting |= OLD_FLTCONT;
		}
		else
		if (lstrcmpA(szTemp,"PopupWnd2") == 0 || lstrcmpA(szTemp,"YAPPWinClass") == 0) // destroy opened popups
			SendMessage(hWnd, UM_DESTROYPOPUP,0,0);
//		else 
//		if (lstrcmpA(szTemp,"#32770") == 0)
//			SendMessage(hWnd, WM_CLOSE,0,0);
		else
		{
			HWND_ITEM *node = new HWND_ITEM;
			node->hWnd = hWnd;
			// add to list
			node->next = g_pMirWnds;
			g_pMirWnds = node;
			ShowWindow(hWnd,SW_HIDE);
		}			
	}
	return(true);
}

TCHAR* GetDefStatusMsg(unsigned uStatus, const char* szProto)
{
	TCHAR *ret = (TCHAR *)CallService ( MS_AWAYMSG_GETSTATUSMSGT, (WPARAM)uStatus, (LPARAM)szProto );
	if ( (int)ret == CALLSERVICE_NOTFOUND ) 
	{
		char* tmp = ( char* )CallService( MS_AWAYMSG_GETSTATUSMSG, (WPARAM)uStatus, (LPARAM)szProto );
		ret = mir_a2t( tmp );
		mir_free( tmp ); 
	}
	return ret;
}

void SetStatus(const char* szProto, unsigned status, TCHAR *tszAwayMsg)
{
	if ( tszAwayMsg && CallProtoService( szProto, PS_GETCAPS, PFLAGNUM_1, 0 ) & PF1_MODEMSGSEND )
	{
		if ( CallProtoService( szProto, PS_SETAWAYMSGT, status, (LPARAM) tszAwayMsg ) == CALLSERVICE_NOTFOUND )
		{
			char *szAwayMsg = mir_t2a(tszAwayMsg);
			CallProtoService( szProto, PS_SETAWAYMSG, status, (LPARAM) szAwayMsg );
			mir_free(szAwayMsg);
		}
	}
	CallProtoService( szProto, PS_SETSTATUS, status, 0 );
}

static int ChangeAllProtoStatuses(unsigned statusMode, TCHAR *msg)
{
	for (int i=0; i < protoCount; i++)
	{
		unsigned status = CallProtoService(proto[i]->szName,PS_GETSTATUS,0,0);
		if (
			(proto[i]->type == PROTOTYPE_PROTOCOL)
			&&
			(g_wMask & OPT_ONLINEONLY) ? // check "Change only if current status is Online" option 
			((status == ID_STATUS_ONLINE) || (status == ID_STATUS_FREECHAT)) // process only "online" and "free for chat"
			: 
			((status > ID_STATUS_OFFLINE) && (status < ID_STATUS_IDLE) && (status != ID_STATUS_INVISIBLE)) // process all existing statuses except for "invisible" & "offline"
			) 
		{
			if (g_wMask & OPT_SETONLINEBACK){ // need to save old statuses & status messages
				oldStatus[i] = status;

				char svc[256];
				mir_snprintf(svc, 256, "%s%s", proto[i]->szName, PS_GETMYAWAYMSG);
				if (ServiceExists (svc))
				{
					if (ServiceExists (MS_AWAYMSG_GETSTATUSMSGT)) // if core can support unicode status message
						oldStatusMsg[i] = (TCHAR *)CallService (svc, 0, SGMA_TCHAR);
					else
					{
						char *tmp = (char *)CallService (svc, 0, 0);
						oldStatusMsg[i] = mir_a2t(tmp);
						mir_free(tmp);
					}
				}
				else
					oldStatusMsg[i] = GetDefStatusMsg(status, proto[i]->szName);
			}
			SetStatus(proto[i]->szName, statusMode, msg);
		}
	}
	return 0;
}

static int BackAllProtoStatuses(void)
{
	for (int i=0; i < protoCount; i++)
	{
		if ( oldStatus[i] )
		{
			SetStatus(proto[i]->szName, oldStatus[i], oldStatusMsg[i]); 
			if (oldStatusMsg[i])
			{
				mir_free(oldStatusMsg[i]);
				oldStatusMsg[i] = 0;
			}
			oldStatus[i] = 0;
		}
	}
	return 0;
}

static void CreateTrayIcon(bool create)
{
	NOTIFYICONDATA nim;
	DBVARIANT dbVar;
	if (!DBGetContactSettingTString(NULL,MOD_NAME,"ToolTipText",&dbVar))
	{
		mir_sntprintf(nim.szTip, 64, _T("%s"), dbVar.ptszVal);
		DBFreeVariant(&dbVar);
	}
	else
		lstrcpy(nim.szTip, _T("Miranda IM"));

	nim.cbSize = sizeof(nim);
	nim.hWnd = g_hListenWindow;
	nim.uID = 100;
	nim.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nim.hIcon = ( HICON )CallService( MS_SKIN2_GETICON, 0, (LPARAM)"hidemim" );
	nim.uCallbackMessage = WM_USER + 24;
	Shell_NotifyIcon(create ? NIM_ADD : NIM_DELETE, &nim);
	g_TrayIcon = create;
}

static void RestoreOldSettings(void)
{
	if (g_bOldSetting & OLD_POPUP)
		CallService(MS_POPUP_QUERY, PUQS_ENABLEPOPUPS, 0);

	if (g_bOldSetting & OLD_SOUND)
		DBWriteContactSettingByte(NULL,"Skin","UseSound", 1);

	if (g_bOldSetting & OLD_FLTCONT) // show Floating contacts if needed
	{
		if(ServiceExists("FloatingContacts/MainHideAllThumbs"))
			CallService("FloatingContacts/MainHideAllThumbs",0,0);
		else
			DBWriteContactSettingByte(NULL,"FloatingContacts","HideAll", 0);
	}
	g_bOldSetting = 0;
}

LRESULT CALLBACK ListenWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_WTSSESSION_CHANGE:
		{
			if (wParam == WTS_SESSION_LOCK && g_wMaskAdv & OPT_HIDEIFLOCK && !g_bWindowHidden) // Windows locked
				PostMessage(hWnd,WM_USER + 40, 0, 0);
			return 0;
		}break;

		case WM_USER + 24:
		{
			if (lParam == WM_LBUTTONDBLCLK)
				PostMessage(hWnd,WM_USER + 52, 0, 0);
			return 0;
		}break;

		case WM_USER+40: // hide
		{
			if (g_bWindowHidden || g_fOptionsOpen) // already hidden or in options, no hiding
				break;

			if (ServiceExists(MS_TRIGGER_REGISTERTRIGGER))
				BossKeyEvent(g_bWindowHidden, 0);

			DWORD dwWndPID; // remember foreground window
			HWND hForegroundWnd = GetForegroundWindow();
			GetWindowThreadProcessId(hForegroundWnd,&dwWndPID);
			if (g_dwMirandaPID == dwWndPID)
				hOldForegroundWindow = hForegroundWnd;

			EnumWindows(EnumWindowsProc, 0);

			if (g_wMask & OPT_CHANGESTATUS) // is this even needed?
			{
				BYTE bReqMode = DBGetContactSettingByte(NULL,MOD_NAME,"stattype",4);
				unsigned uMode = (STATUS_ARR_TO_ID[bReqMode]);
				DBVARIANT dbVar;
				if (g_wMask & OPT_USEDEFMSG || DBGetContactSettingTString(NULL,MOD_NAME,"statmsg",&dbVar))
				{
					TCHAR *ptszDefMsg = GetDefStatusMsg(uMode, 0);
					ChangeAllProtoStatuses(uMode, ptszDefMsg);
					if(ptszDefMsg)
						mir_free(ptszDefMsg);
				}
				else
				{
					if (ServiceExists(MS_VARS_FORMATSTRING)) 
					{
						FORMATINFO fi;
						TCHAR *ptszParsed;
						
						ZeroMemory(&fi, sizeof(fi));
						fi.cbSize = sizeof(fi);
						fi.flags = FIF_TCHAR;
						fi.tszFormat = dbVar.ptszVal;
						ptszParsed = (TCHAR*)CallService(MS_VARS_FORMATSTRING, (WPARAM)&fi, 0);
						ChangeAllProtoStatuses(uMode, ptszParsed);
						if (ptszParsed)
							CallService(MS_VARS_FREEMEMORY, (WPARAM)ptszParsed, 0);
					}else
						ChangeAllProtoStatuses(uMode, dbVar.ptszVal);
				DBFreeVariant(&dbVar);
				}
			}

			pcli->pfnTrayIconDestroy (pcli->hwndContactList);
			pcli->pfnUninitTray();

			if (g_wMask & OPT_TRAYICON)
				CreateTrayIcon(true);

			// disable popups
			if (CallService(MS_POPUP_QUERY, PUQS_GETSTATUS, 0) == 1)
			{
				// save current
				g_bOldSetting |= OLD_POPUP;
				CallService(MS_POPUP_QUERY, PUQS_DISABLEPOPUPS, 0);
			}

			// disable sounds
			if ((g_wMask & OPT_DISABLESNDS) && DBGetContactSettingByte(NULL,"Skin","UseSound",1))
			{
				// save current
				g_bOldSetting |= OLD_SOUND;
				DBWriteContactSettingByte(NULL,"Skin","UseSound",0);
			}

			g_bWindowHidden = true;

			g_bOldSetting |= OLD_WASHIDDEN;
			DBWriteContactSettingByte(NULL, MOD_NAME, "OldSetting", g_bOldSetting);
			return(0);
		} break;
		case WM_USER+52: // back
		{
			if (!g_bWindowHidden || g_fPassRequested) 
				break;

			if (g_wMask & OPT_REQPASS){  //password request
				DBVARIANT dbVar = {0};
				if(!DBGetContactSettingString(NULL,MOD_NAME,"password",&dbVar))
				{
					g_fPassRequested = true;

					strncpy(g_password, dbVar.pszVal, MAXPASSLEN);
					DBFreeVariant(&dbVar);
					CallService( MS_DB_CRYPT_DECODESTRING, MAXPASSLEN+1, ( LPARAM )g_password );
		
					int res = DialogBox(g_hInstance,(MAKEINTRESOURCE(IDD_PASSDIALOGNEW)),GetForegroundWindow(),(DLGPROC)DlgStdInProc);

					g_fPassRequested = false;
					if(res != IDOK) return 0;
				}
			}

			if (ServiceExists(MS_TRIGGER_REGISTERTRIGGER))
				BossKeyEvent(g_bWindowHidden, 0);

			if (g_wMask & OPT_CHANGESTATUS && g_wMask & OPT_SETONLINEBACK) // set back to some status
			{
				BackAllProtoStatuses();
			}

			HWND_ITEM *pCurWnd = g_pMirWnds;
			while (pCurWnd != NULL)
			{
				HWND_ITEM *pNextWnd = pCurWnd->next;
				char szTemp[32];
				GetClassNameA(pCurWnd->hWnd,szTemp,32);

				if (IsWindow(pCurWnd->hWnd) && lstrcmpA(szTemp,"SysShadow") != 0) // precaution
				{
					ShowWindow(pCurWnd->hWnd, SW_SHOW);
				}

				delete pCurWnd; // bye-bye
				pCurWnd = pNextWnd; // traverse to next item
			}
			g_pMirWnds = NULL;

			if (hOldForegroundWindow)
			{
				SetForegroundWindow(hOldForegroundWindow);
				hOldForegroundWindow = NULL;
			}

			RestoreOldSettings();

			if (g_TrayIcon) CreateTrayIcon(false);

			pcli->pfnInitTray();
			pcli->pfnTrayIconInit(pcli->hwndContactList); //this restores the icons without memory leaks :)

			// force a redraw
			// should prevent drawing problems
			InvalidateRect(pcli->hwndContactList,NULL,true);
			UpdateWindow(pcli->hwndContactList);

			PostMessage(hWnd, WM_MOUSEMOVE, 0, (LPARAM)MAKELONG(2, 2)); // reset core's IDLE
			g_bWindowHidden = false;

			DBWriteContactSettingByte(NULL, MOD_NAME, "OldSetting", 0);
			return(0);
		} break;
		default:break;
	}
	return(DefWindowProc(hWnd,uMsg,wParam,lParam));
}

static int MsgWinOpening(WPARAM wParam, LPARAM lParam) // hiding new message windows
{
	if (g_bWindowHidden)
	EnumWindows(EnumWindowsProc,0);
	return 0;
}

VOID CALLBACK WinEventProc(HWINEVENTHOOK g_hWinHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
	if (g_bWindowHidden && idObject == OBJID_WINDOW && (event == EVENT_OBJECT_CREATE || event == EVENT_OBJECT_SHOW) && (IsWindowVisible(hwnd)) )
	{
		if(hwnd == pcli->hwndContactList)
			ShowWindow(hwnd,SW_HIDE);
		else
			EnumWindows(EnumWindowsProc,0);
	}
}

INT_PTR BossKeyHideMiranda(WPARAM wParam, LPARAM lParam) // for service :)
{
	PostMessage(g_hListenWindow, WM_USER + ((g_bWindowHidden) ? (52) : (40)), 0, 0);
	return 0;
}

static TCHAR *HokeyVkToName(WORD vkKey)
{
	static TCHAR buf[32] = {0};
	DWORD code = MapVirtualKey(vkKey, 0) << 16;

	switch (vkKey)
	{
	case 0:
	case VK_CONTROL:
	case VK_SHIFT:
	case VK_MENU:
	case VK_LWIN:
	case VK_RWIN:
	case VK_PAUSE:
	case VK_CANCEL:
	case VK_CAPITAL:
		return _T("");

	case VK_DIVIDE:
	case VK_INSERT:
	case VK_HOME:
	case VK_PRIOR:
	case VK_DELETE:
	case VK_END:
	case VK_NEXT:
	case VK_LEFT:
	case VK_RIGHT:
	case VK_UP:
	case VK_DOWN:
	case VK_NUMLOCK:
		code |= (1UL << 24);
	}

	GetKeyNameText(code, buf, SIZEOF(buf));
	return buf;
}

static TCHAR *GetBossKeyText(void)
{
	WORD wHotKey = DBGetContactSettingWord(NULL,"SkinHotKeys","Hide/Show Miranda",HOTKEYCODE(HOTKEYF_CONTROL, VK_F12));

	BYTE key = LOBYTE(wHotKey);
	BYTE shift = HIBYTE(wHotKey);
	static TCHAR buf[128] = {0};

	mir_sntprintf(buf, SIZEOF(buf), _T("%s%s%s%s%s"),
		(shift & HOTKEYF_CONTROL)	? _T("Ctrl + ")		: _T(""),
		(shift & HOTKEYF_SHIFT)		? _T("Shift + ")	: _T(""),
		(shift & HOTKEYF_ALT)		? _T("Alt + ")		: _T(""),
		(shift & HOTKEYF_EXT)		? _T("Win + ")		: _T(""),
		HokeyVkToName(key));

	return buf;
}

static int IcoLibInit (void) // Icolib support
{
	SKINICONDESC sid = {0};
	TCHAR tszFile[MAX_PATH];
	GetModuleFileName(g_hInstance, tszFile, MAX_PATH);

	sid.cbSize = sizeof(SKINICONDESC);
	sid.flags = SIDF_ALL_TCHAR;
	sid.ptszDefaultFile = tszFile;
	sid.cx = sid.cy = 16;
	sid.ptszSection = _T("BossKey");

	sid.pszName = "hidemim";
	sid.ptszDescription = _T("Hide Miranda IM");
	sid.iDefaultIndex = -IDI_DLGPASSWD;
	g_hIcon = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);

	return 0;
}

static int GenMenuInit(WPARAM wParam, LPARAM lParam) // Modify menu item text before to show the main menu
{
	if (g_hMenuItem)
	{
		CLISTMENUITEM mi = {0};
		TCHAR buf[128] = {0};
		mir_sntprintf(buf, SIZEOF(buf), _T("%s [%s]"), TranslateT("Hide"), GetBossKeyText());

		mi.cbSize = sizeof(mi);
		mi.flags = CMIM_FLAGS | CMIF_TCHAR  | CMIM_NAME;
		mi.ptszName = buf;

		CallService(MS_CLIST_MODIFYMENUITEM,(WPARAM)g_hMenuItem,(LPARAM)&mi);
	}
	return 0;
}

void BossKeyMenuItemInit(void) // Add menu item
{
	CLISTMENUITEM mi = {0};

	mi.cbSize = sizeof(mi);
	mi.flags = CMIF_TCHAR;
	mi.position = 2000100000;
	mi.pszPopupName = 0;
	mi.hIcon = ( HICON )CallService( MS_SKIN2_GETICON, 0, (LPARAM)"hidemim" );
	mi.ptszName = _T("Hide");
	mi.pszService = MS_BOSSKEY_HIDE;

	g_hMenuItem = (HANDLE)CallService(MS_CLIST_ADDMAINMENUITEM,0,(LPARAM)&mi);

	g_hmGenMenuInit = HookEvent(ME_CLIST_PREBUILDMAINMENU, GenMenuInit);
}

void BossKeyMenuItemUnInit(void) // Remove menu item
{
	CallService(MS_CLIST_REMOVEMAINMENUITEM, (WPARAM)g_hMenuItem, 0);
	g_hMenuItem = 0;
	if(g_hmGenMenuInit) 
		UnhookEvent(g_hmGenMenuInit);
	g_hmGenMenuInit = 0;
}

void RegisterCoreHotKeys (void)
{
	HOTKEYDESC hotkey = {0};
	hotkey.cbSize = sizeof(HOTKEYDESC);
	hotkey.pszName = "Hide/Show Miranda";
	hotkey.pszDescription = LPGEN("Hide/Show Miranda");
	hotkey.pszSection = "BossKey";
	hotkey.pszService = MS_BOSSKEY_HIDE;
	hotkey.DefHotKey = HOTKEYCODE(HOTKEYF_CONTROL, VK_F12);

	CallService(MS_HOTKEY_REGISTER, 0, (LPARAM)&hotkey);
}

static int ModernToolbarInit(WPARAM, LPARAM) // Modern toolbar support
{
	TBButton button = {0};
	button.cbSize = sizeof(button);
	button.pszServiceName = MS_BOSSKEY_HIDE;
	button.pszButtonID = MOD_NAME;
	button.pszTooltipUp = button.pszTooltipDn = button.pszButtonName = "Hide Miranda IM";
	button.defPos = 1099;
	button.tbbFlags = TBBF_DISABLED|TBBF_SHOWTOOLTIP;
	button.hPrimaryIconHandle = button.hSecondaryIconHandle = g_hIcon;
	CallService(MS_TB_ADDBUTTON, 0, (LPARAM)&button);
	return 0;
}

// Tabsrmm toolbar support
static int TabsrmmButtonPressed(WPARAM wParam, LPARAM lParam)
{
	CustomButtonClickData *cbcd=(CustomButtonClickData *)lParam;

	if(!strcmp(cbcd->pszModule, MOD_NAME))
		BossKeyHideMiranda(0, 0);

	return 0;
}

static int TabsrmmButtonsInit(WPARAM wParam, LPARAM lParam)
{
	BBButton bbd = {0};
 
	bbd.cbSize = sizeof(BBButton);
	bbd.pszModuleName = MOD_NAME;
	bbd.dwDefPos = 5000;
	bbd.ptszTooltip = _T("Hide Miranda IM");
	bbd.bbbFlags = BBBF_ISRSIDEBUTTON | BBBF_CANBEHIDDEN;
	bbd.hIcon = g_hIcon;
	CallService (MS_BB_ADDBUTTON, 0, (LPARAM)&bbd);

	return 0;
} 

static TCHAR *VariablesBossKey(ARGUMENTSINFO *ai) {
	if (ai->cbSize < sizeof(ARGUMENTSINFO))	return NULL;
	if (ai->argc != 1) return NULL;

	ai->flags |= AIF_DONTPARSE;

	return GetBossKeyText();
}

static int EnumProtos(WPARAM wParam, LPARAM lParam)
{
	CallService(MS_PROTO_ENUMPROTOCOLS,(WPARAM)&protoCount,(LPARAM)&proto);

	delete[] oldStatus;
	delete[] oldStatusMsg;

	oldStatus = new unsigned[protoCount];
	oldStatusMsg = new TCHAR* [protoCount];
	for (int i = 0; i < protoCount; i++)
	{
		oldStatus[i] = 0;
		oldStatusMsg[i] = 0;
	}
	return 0;
}

int MirandaLoaded(WPARAM wParam,LPARAM lParam)
{
	g_wMask = DBGetContactSettingWord(NULL,MOD_NAME,"optsmask",DEFAULTSETTING);

	RegisterCoreHotKeys();

#ifndef _UNICODE
	if (IsWinVer2000Plus())
#endif
		g_hWinHook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_SHOW, 
						NULL, WinEventProc, GetCurrentProcessId(), 0, 0);

	g_hHooks[0] = HookEvent(ME_OPT_INITIALISE,OptsDlgInit);
	g_hHooks[1] = HookEvent(ME_MSG_WINDOWEVENT,MsgWinOpening);
	g_hHooks[2] = HookEvent(ME_PROTO_ACCLISTCHANGED, EnumProtos);
	g_hHooks[3] = HookEvent(ME_MSG_TOOLBARLOADED, TabsrmmButtonsInit);
	if (g_hHooks[3])
		g_hHooks[4] = HookEvent(ME_MSG_BUTTONPRESSED, TabsrmmButtonPressed);

	pcli = (CLIST_INTERFACE *)CallService(MS_CLIST_RETRIEVE_INTERFACE, 0, (LPARAM)g_hInstance);
#if defined _DEBUG
	if (pcli == 0)
	{
		MessageBox(NULL,_T("Can't get CLIST_INTERFACE!"),_T("BossKey+"),MB_ICONERROR);
		return 1;
	}else 
#endif
		GetWindowThreadProcessId(pcli->hwndContactList,&g_dwMirandaPID);

	// let's create our secret window
	// this is a cheap, cheap hack...
	// needed because of the hook, that gives problems
	// if the calls aren't made from this context, they won't work
	// using the window is a workaround to make this process do its work :)
	// see notes
	WNDCLASS winclass = {0};

	winclass.lpfnWndProc = ListenWndProc;
	winclass.hInstance = g_hInstance;
	//winclass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	//winclass.hCursor = LoadCursor(NULL,IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszClassName = BOSSKEY_LISTEN_INFO;

	if (RegisterClass(&winclass))
	{
		hWTS = LoadLibrary(_T("wtsapi32.dll"));

		g_hListenWindow = CreateWindow(BOSSKEY_LISTEN_INFO,BOSSKEY_LISTEN_INFO,WS_POPUP,0,0,5,5,pcli->hwndContactList,NULL,g_hInstance,NULL);
		
		if (hWTS)
		{
			wtsRegisterSessionNotification = (PWTSRegisterSessionNotification)GetProcAddress(hWTS, "WTSRegisterSessionNotification");
			if (wtsRegisterSessionNotification)
			{
				wtsUnRegisterSessionNotification = (PWTSUnRegisterSessionNotification)GetProcAddress(hWTS, "WTSUnRegisterSessionNotification");
				wtsRegisterSessionNotification(g_hListenWindow, 0);
			}
		}
	}

	if (IsWinVerVistaPlus())
	{
		hDwmApi = LoadLibrary(_T("dwmapi.dll"));
		if (hDwmApi)
		{
			dwmIsCompositionEnabled = (PFNDwmIsCompositionEnabled)GetProcAddress(hDwmApi,"DwmIsCompositionEnabled");
		}
	}
	if (g_wMaskAdv & OPT_MENUITEM)
		BossKeyMenuItemInit();

// Updater support	
	if(ServiceExists(MS_UPDATE_REGISTERFL))
		CallService(MS_UPDATE_REGISTERFL, (WPARAM)__PLUGIN_ID, (LPARAM)&pluginInfo);

	if (ServiceExists(MS_TRIGGER_REGISTERTRIGGER))
		RegisterTrigger();

// Register token for variables plugin
	if (ServiceExists(MS_VARS_REGISTERTOKEN))
	{
		TOKENREGISTER tr = {0};
		tr.cbSize = sizeof(TOKENREGISTER);
		tr.memType = TR_MEM_OWNER;
		tr.flags = TRF_FIELD | TRF_TCHAR | TRF_PARSEFUNC;

		tr.tszTokenString = _T("bosskeyname");
		tr.parseFunctionT = VariablesBossKey;
		tr.szHelpText = LPGEN("BossKey\tget the BossKey name");
		CallService(MS_VARS_REGISTERTOKEN, 0, (LPARAM) &tr);
	}

	EnumProtos(0, 0);
	InitIdleTimer();

	if (g_bOldSetting && !(g_wMaskAdv & OPT_RESTORE)) // Restore settings if Miranda was crushed or killed in hidden mode and "Restore hiding on startup after failure" option is disabled
		RestoreOldSettings();

	if (g_wMaskAdv & OPT_HIDEONSTART || 
		(g_wMaskAdv & OPT_RESTORE && g_bOldSetting))
		BossKeyHideMiranda(0, 0);

	return(0);
}



extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink = link;
	mir_getMMI( &mmi );
	mir_getLP(&pluginInfo);

	g_wMaskAdv = DBGetContactSettingWord(NULL,MOD_NAME,"optsmaskadv",0);
	g_bOldSetting = DBGetContactSettingByte(NULL, MOD_NAME, "OldSetting", 0);

	if ((g_bOldSetting & OLD_POPUP) && !(g_wMaskAdv & OPT_RESTORE)) // Restore popup settings if Miranda was crushed or killed in hidden mode and "Restore hiding on startup after failure" option is disabled
	{
		if (DBGetContactSettingByte(NULL, "PopUp", "ModuleIsEnabled", 1) == 0)
			DBWriteContactSettingByte(NULL, "PopUp", "ModuleIsEnabled", 1);
		if (DBGetContactSettingByte(NULL, "YAPP", "Enabled", 1) == 0)
			DBWriteContactSettingByte(NULL, "YAPP", "Enabled", 1);
	}
	if (g_wMaskAdv & OPT_HIDEONSTART && DBGetContactSettingByte(NULL, "PopUp", "ModuleIsEnabled", 0)) // hack for disabling popup on startup if "Hide Miranda on startup" is enabled
	{
		g_bOldSetting |= OLD_POPUP;
		DBWriteContactSettingByte(NULL, "PopUp", "ModuleIsEnabled", 0);
	}

	IcoLibInit();
	g_hHooks[5] = HookEvent(ME_SYSTEM_MODULESLOADED,MirandaLoaded);
	g_hHideService = CreateServiceFunction(MS_BOSSKEY_HIDE,BossKeyHideMiranda); // Create service
	g_hHooks[6] = HookEvent(ME_TB_MODULELOADED, ModernToolbarInit); // Toolbar hook
	return 0;
}



extern "C" int __declspec(dllexport) Unload(void)
{
	UninitIdleTimer();
	// nice boys and girls always unhook events :)
	for (int i = 0; i < SIZEOF(g_hHooks); i++)
	{
		if (g_hHooks[i])
			UnhookEvent(g_hHooks[i]);
	}

	if(g_hmGenMenuInit) 
		UnhookEvent(g_hmGenMenuInit);

	if (g_hWinHook != 0)
		UnhookWinEvent(g_hWinHook);

	DestroyServiceFunction(g_hHideService);
	if (g_hIsHiddenService)
		DestroyServiceFunction(g_hIsHiddenService);

	if (g_hListenWindow)
	{
		if (wtsUnRegisterSessionNotification)
			wtsUnRegisterSessionNotification(g_hListenWindow);
		DestroyWindow(g_hListenWindow);
	}

	if (hWTS)
		FreeLibrary(hWTS);
	if (hDwmApi)
		FreeLibrary(hDwmApi);

	// free all sessions
	HWND_ITEM *pTemp = g_pMirWnds;
	while (pTemp != NULL)
	{
		HWND_ITEM *pNext = pTemp->next;
		delete pTemp;
		pTemp = pNext;
	}
	g_pMirWnds = NULL; // safety

	// free memory
	delete[] oldStatus;
	delete[] oldStatusMsg;

	return(0);
}
