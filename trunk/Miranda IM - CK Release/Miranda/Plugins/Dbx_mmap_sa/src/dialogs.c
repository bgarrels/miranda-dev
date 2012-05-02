/*
Miranda IM: the free IM client for Microsoft* Windows*

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

===============================================================================
*/

#include "commonheaders.h"
#include <m_utils.h>
#include <m_clist.h>
#include <m_options.h>
#include <m_icolib.h>

#include <shlobj.h>

#define MS_DB_CHANGEPASSWORD "DB/ChangePassword"

extern char encryptKey[255];
extern size_t encryptKeyLength;
extern HANDLE g_hInst;
HANDLE hSetPwdMenu;

extern BOOL gl_bUnicodeAwareCore;

INT_PTR CALLBACK DlgProcOptions(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL ShowDlgItem(HWND hwndDlg, int iIDCtrl, BOOL bShow)
{
	HWND hwndCtrl = GetDlgItem(hwndDlg, iIDCtrl);
	if (!hwndCtrl) return FALSE;

	// Avoid flickering
	if (bShow && IsWindowVisible(hwndCtrl))
		return TRUE;

	return ShowWindow(hwndCtrl, (bShow ? SW_SHOW : SW_HIDE));
}

BOOL EnableDlgItem(HWND hwndDlg, int iIDCtrl, BOOL bEnable)
{
	HWND hwndCtrl = GetDlgItem(hwndDlg, iIDCtrl);
	if (!hwndCtrl) return FALSE;

	// Avoid flickering
	if (IsWindowEnabled(hwndCtrl) == bEnable)
		return (bEnable == FALSE);

	return EnableWindow(hwndCtrl, bEnable);
}

BOOL IsDlgItemEnabled(HWND hwndDlg, int iIDCtrl)
{
	HWND hwndCtrl = GetDlgItem(hwndDlg, iIDCtrl);
	if (!hwndCtrl) return FALSE;
	return IsWindowEnabled(hwndCtrl);
}


static int OptionsInit(WPARAM wParam, LPARAM lParam)
{
	OPTIONSDIALOGPAGE odp;

	odp.cbSize						= 60;
	odp.position					= -790000000;
	odp.hInstance					= g_hInst;
	odp.pszTemplate					= MAKEINTRESOURCEA(IDD_OPTIONS);
	odp.flags						= ODPF_BOLDGROUPS;
	if (gl_bUnicodeAwareCore)
	{
		odp.ptszTitle				= TranslateT("Database Features");
		odp.ptszGroup				= TranslateT("Services");
		odp.flags				   |= ODPF_UNICODE;
	}
	else
	{
		odp.pszTitle				= Translate("Database Features");
		odp.pszGroup				= Translate("Services");
	}

	odp.nIDBottomSimpleControl		= 0;
	odp.pfnDlgProc					= DlgProcOptions;
	CallService( MS_OPT_ADDPAGE, wParam,( LPARAM )&odp );

	return 0;
}

INT_PTR ChangePassword(WPARAM wParam, LPARAM lParam)
{
	if(bEncoding){
		ChangePwd();
	}else{
		EncryptDB();
	}
	return 0;
}

void xModifyMenu(HANDLE hMenu,long flags,const TCHAR* name, HICON hIcon)
{
	CLISTMENUITEM menu;
	ZeroMemory(&menu,sizeof(menu));
	menu.cbSize = sizeof(menu);
	menu.flags = CMIM_FLAGS;
	menu.flags |= gl_bUnicodeAwareCore ? CMIF_TCHAR : 0;
	menu.flags |= name ? CMIM_NAME : 0;
	menu.flags |= hIcon ? CMIM_ICON : 0;
	menu.flags |= flags;
	menu.ptszName = (TCHAR*)name;
	menu.hIcon=hIcon;

	CallService(MS_CLIST_MODIFYMENUITEM,(WPARAM)hMenu,(LPARAM)&menu);
}

static int ModulesLoad(WPARAM wParam, LPARAM lParam)
{
	CLISTMENUITEM menu = {0};
	SKINICONDESC sid = {0};
	char szFile[MAX_PATH];
	//HANDLE hFirst;

	HookEvent(ME_OPT_INITIALISE, OptionsInit);

	// icolib init
	GetModuleFileNameA(g_hInst, szFile, MAX_PATH);

	sid.cbSize				= sizeof(SKINICONDESC);
	sid.pszDefaultFile		= szFile;

	if (gl_bUnicodeAwareCore)
	{
		sid.flags			= SIDF_TCHAR;
		sid.ptszSection		= LPGENT("Database");
		sid.ptszDescription = LPGENT("Database");
	}else
	{
		sid.pszSection		= LPGEN("Database");
		sid.pszDescription	= LPGEN("Database");
	}
	sid.pszName				= "database";
	sid.iDefaultIndex		= -IDI_ICON2;
	CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);

	if (gl_bUnicodeAwareCore)
		sid.ptszDescription = LPGENT("Change Password");
	else
		sid.pszDescription	= LPGEN("Change Password");

	sid.pszName				= "password";
	sid.iDefaultIndex		= -IDI_ICON3;
	CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);

	menu.cbSize = sizeof(menu);
	menu.flags = CMIM_ALL;

	menu.hIcon = (HICON)CallService(MS_SKIN2_GETICON, 0, (LPARAM)"database");

	// main menu item
	if (gl_bUnicodeAwareCore)
	{
		menu.flags|=CMIF_TCHAR;
		menu.ptszName = (bEncoding) ? LPGENT("Change password") : LPGENT("Set password");
		menu.ptszPopupName = LPGENT("Database");
	}
	else
	{
		menu.pszName = (bEncoding) ? LPGEN("Change password") : LPGEN("Set password");
		menu.pszPopupName = LPGEN("Database");
	}

	menu.pszService = MS_DB_CHANGEPASSWORD;
	menu.position = 500100000;

	hSetPwdMenu = (HANDLE)CallService(MS_CLIST_ADDMAINMENUITEM,0,(LPARAM)&menu);

	ZeroMemory(&menu,sizeof(menu));
	menu.cbSize	=	sizeof(menu);
	menu.flags	=	CMIM_ICON;
	menu.hIcon	=	(HICON)CallService(MS_SKIN2_GETICON, 0, (LPARAM)"password");
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hSetPwdMenu, (LPARAM)&menu);

	return 0;
}

int UnloadOptions()
{
	OleUninitialize();
	return 0;
}


int InitDialogs()
{
	OleInitialize(0);
	CreateServiceFunction(MS_DB_CHANGEPASSWORD, ChangePassword);
	HookEvent(ME_SYSTEM_MODULESLOADED, ModulesLoad);

	return 0;
}

extern Cryptor* CryptoEngine;

extern int ModulesCount;
extern CryptoModule* Modules[100];

//ugly, i know
#undef LVM_SETITEMTEXT
#define  LVM_SETITEMTEXT        LVM_SETITEMTEXTA

int ImageList_AddIcon_IconLibLoaded(HIMAGELIST hIml, char* name)
{
	HICON hIcon = (HICON)CallService(MS_SKIN2_GETICON, (WPARAM)NULL, (LPARAM)name);
	int res = ImageList_AddIcon(hIml, hIcon);
	return res;
}

INT_PTR CALLBACK DlgProcOptions(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hwndList = GetDlgItem(hwndDlg, IDC_MODULES);
	LVCOLUMN col;
	LVITEM item;
	int i, iRow, iIndex;
	NMLISTVIEW * hdr = (NMLISTVIEW *) lParam;
	WORD uid;
	HIMAGELIST hIml;

	switch ( msg ) {
		case WM_INITDIALOG:
			hIml = ImageList_Create(16, 16, ILC_MASK | (IsWinVerXPPlus()? ILC_COLOR32 : ILC_COLOR16), 2, 0);
			TranslateDialogDefault( hwndDlg );

			ImageList_AddIcon_IconLibLoaded( hIml, "core_main_29" );
			ImageList_AddIcon_IconLibLoaded( hIml, "core_main_30" );
			ListView_SetImageList( hwndList, hIml, LVSIL_SMALL );

			col.pszText = NULL;
			col.mask = LVCF_TEXT | LVCF_WIDTH;
			col.fmt = LVCFMT_LEFT;
			col.cx = 50;
			ListView_InsertColumn(hwndList, 1, &col);

			col.pszText = TranslateT("Dll");
			col.mask = LVCF_TEXT | LVCF_WIDTH;
			col.fmt = LVCFMT_LEFT;
			col.cx = 1000;
			ListView_InsertColumn(hwndList, 2, &col);

			col.pszText = TranslateT("Name");
			col.cx = 1000;
			ListView_InsertColumn(hwndList, 3, &col);

			col.pszText = TranslateT("Version");
			col.cx = 1000;
			ListView_InsertColumn(hwndList, 4, &col);

			ListView_SetExtendedListViewStyleEx(hwndList, 0, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES);

			uid = DBGetContactSettingWord(NULL, "SecureMMAP", "CryptoModule", 0);

			for(i = 0; i < ModulesCount; i++){
				char buf[100];

				item.mask = LVIF_TEXT;
				item.iItem = i;
				item.iSubItem = 0;
				item.pszText = NULL;
				iRow = ListView_InsertItem(hwndList, &item);

				ListView_SetItemText(hwndList, iRow, 1, (LPWSTR)Modules[i]->dllname);
				ListView_SetItemText(hwndList, iRow, 2, (LPWSTR)Modules[i]->cryptor->Name);
				mir_snprintf(buf,SIZEOF(buf),"%d.%d.%d.%d", HIBYTE(HIWORD(Modules[i]->cryptor->Version)), LOBYTE(HIWORD(Modules[i]->cryptor->Version)), HIBYTE(LOWORD(Modules[i]->cryptor->Version)), LOBYTE(LOWORD(Modules[i]->cryptor->Version)));
				ListView_SetItemText(hwndList, iRow, 3, (LPWSTR)buf);

				if(uid == Modules[i]->cryptor->uid && bEncoding)
					ListView_SetCheckState(hwndList, i, 1);

				item.mask = LVIF_IMAGE;
				item.iItem = iRow;
				item.iSubItem = 0;
				item.iImage = ( CryptoEngine == Modules[i]->cryptor && bEncoding ) ? 0 : 1;
				ListView_SetItem( hwndList, &item );
			}

			ListView_SetColumnWidth(hwndList, 0, LVSCW_AUTOSIZE);
			ListView_SetColumnWidth(hwndList, 1, LVSCW_AUTOSIZE);
			ListView_SetColumnWidth(hwndList, 2, LVSCW_AUTOSIZE);
			ListView_SetColumnWidth(hwndList, 3, LVSCW_AUTOSIZE);
			return TRUE;

		case WM_COMMAND:
			if ( HIWORD(wParam) == STN_CLICKED ) {
				switch (LOWORD(wParam)) {
					case IDC_EMAIL:
					case IDC_SITE:
					{
						char buf[512];
						char * p = &buf[7];
						lstrcpyA(buf,"mailto:");
						if ( GetWindowTextA(GetDlgItem(hwndDlg, LOWORD(wParam)), p, SIZEOF(buf) - 7) ) {
							CallService(MS_UTILS_OPENURL,0,(LPARAM) (LOWORD(wParam)==IDC_EMAIL ? buf : p) );
						}
						break;
			}	}	}
			break;

		case WM_NOTIFY:
			if ( hdr && hdr->hdr.code == LVN_ITEMCHANGED && IsWindowVisible(hdr->hdr.hwndFrom) && hdr->iItem != (-1) ) {
				iIndex = hdr->iItem;
				if(hdr->uNewState & 0x2000){
					for(i = 0; i < ModulesCount; i++){
						if(i != iIndex) ListView_SetCheckState(hwndList, i, 0);
					}
					SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
					break;
				}
				if(hdr->uNewState & 0x1000){
					SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
					break;
				}
				if(hdr->uNewState & LVIS_SELECTED){
					SetDlgItemTextA(hwndDlg, IDC_AUTHOR, Modules[iIndex]->cryptor->Author);
					{
						TCHAR* info_t = mir_a2t((char*)(Modules[iIndex]->cryptor->Info));
						SetDlgItemText(hwndDlg, IDC_INFO, TranslateTS(info_t));
						mir_free(info_t);
					}
					SetDlgItemTextA(hwndDlg, IDC_SITE, Modules[iIndex]->cryptor->Site);
					SetDlgItemTextA(hwndDlg, IDC_EMAIL, Modules[iIndex]->cryptor->Email);
					SetDlgItemTextA(hwndDlg, IDC_ENC, Modules[iIndex]->cryptor->Name);
					SetDlgItemInt(hwndDlg, IDC_UID, Modules[iIndex]->cryptor->uid, 0);
				} else {
					SetDlgItemTextA(hwndDlg, IDC_AUTHOR, "");
					SetDlgItemTextA(hwndDlg, IDC_INFO, "");
					SetDlgItemTextA(hwndDlg, IDC_SITE, "");
					SetDlgItemTextA(hwndDlg, IDC_EMAIL, "");
					SetDlgItemTextA(hwndDlg, IDC_ENC, "");
					SetDlgItemTextA(hwndDlg, IDC_UID, "");
				}

				break;
			}
			if (((LPNMHDR)lParam)->code == PSN_APPLY ) {
				int alg = -1;
				for(i = 0; i < ModulesCount; i++){
					if(ListView_GetCheckState(hwndList, i)){
						alg = i;
						break;
					}
				}

				if(alg > -1){
					if(!bEncoding){
						DBWriteContactSettingWord(NULL, "SecureMMAP", "CryptoModule", Modules[alg]->cryptor->uid);
						EncryptDB();
					}else{
						if(Modules[alg]->cryptor->uid != DBGetContactSettingWord(NULL, "SecureMMAP", "CryptoModule", -1)){
							DBWriteContactSettingWord(NULL, "SecureMMAP", "CryptoModule", Modules[alg]->cryptor->uid);
							RecryptDB();
						}
					}
				}else{
					if(bEncoding){
						DecryptDB();
					}
				}

				uid = DBGetContactSettingWord(NULL, "SecureMMAP", "CryptoModule", 0);

				for(i = 0; i < ModulesCount; i++){
					if(uid == Modules[i]->cryptor->uid && bEncoding)
						ListView_SetCheckState(hwndList, i, 1);

					item.mask = LVIF_IMAGE;
					item.iItem = i;
					item.iSubItem = 0;
					item.iImage = ( CryptoEngine == Modules[i]->cryptor && bEncoding ) ? 0 : 1;

					ListView_SetItem( hwndList, &item );
				}

				return TRUE;

			}
			break;
	}

	return FALSE;
}
UINT oldLangID = 0;
void LanguageChanged(HWND hDlg)
{
	UINT LangID = (UINT)GetKeyboardLayout(0);
	char Lang[3] = {0};
	if (LangID != oldLangID)
	{
		oldLangID = LangID;
		GetLocaleInfoA(MAKELCID((LangID & 0xffffffff),  SORT_DEFAULT),  LOCALE_SABBREVLANGNAME, Lang, 2);
		Lang[0] = toupper(Lang[0]);
		Lang[1] = tolower(Lang[1]);
		SetDlgItemTextA(hDlg, IDC_LANG, Lang);
	}
}

extern BOOL wrongPass;

BOOL CALLBACK DlgStdInProc(HWND hDlg, UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HICON hIcon = 0;
	TCHAR tszHeaderTxt[256];
	char szHeaderTxt[256];
	TCHAR* tszDbName;
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hwndCtrl;
//			if(pluginLink && ServiceExists(MS_LANGPACK_TRANSLATEDIALOG))
			TranslateDialogDefault(hDlg);

			hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON2));
			SendMessage(GetDlgItem(hDlg, IDC_HEADERBAR), WM_SETICON, 0, (LPARAM)hIcon);

			if(!wrongPass)
			{
				if (gl_bUnicodeAwareCore)
				{
					tszDbName = mir_a2t((char*)lParam);
					mir_sntprintf(tszHeaderTxt, SIZEOF(tszHeaderTxt), _T("%s\n%s"), TranslateT("Please type in your password for"), tszDbName);
					SetWindowText(GetDlgItem(hDlg, IDC_HEADERBAR), tszHeaderTxt);
					mir_free(tszDbName);
				}
				else
				{
					mir_snprintf(szHeaderTxt, SIZEOF(szHeaderTxt), "%s\n%s", Translate("Please type in your password for"), lParam);
					SetWindowTextA(GetDlgItem(hDlg, IDC_HEADERBAR), szHeaderTxt);
				}
			}
			else
			{
				if (wrongPass > 2)
				{
					hwndCtrl = GetDlgItem(hDlg, IDC_USERPASS);
					EnableWindow(hwndCtrl, FALSE);
					hwndCtrl = GetDlgItem(hDlg, IDOK);
					EnableWindow(hwndCtrl, FALSE);
					if (gl_bUnicodeAwareCore)
						SetWindowText(GetDlgItem(hDlg, IDC_HEADERBAR), TranslateT("Too many errors!"));
					else
						SetWindowTextA(GetDlgItem(hDlg, IDC_HEADERBAR), Translate("Too many errors!"));
				}
				else
				{
					if (gl_bUnicodeAwareCore)
						SetWindowText(GetDlgItem(hDlg, IDC_HEADERBAR), TranslateT("Password is not correct!"));
					else
						SetWindowTextA(GetDlgItem(hDlg, IDC_HEADERBAR), Translate("Password is not correct!"));
				}
			}
			oldLangID = 0;
			SetTimer(hDlg,1,200,NULL);
			LanguageChanged(hDlg);
			return TRUE;
		}

	case WM_CTLCOLORSTATIC:
		{
			if ((HWND)lParam == GetDlgItem(hDlg, IDC_LANG))
			{
				SetTextColor((HDC)wParam, GetSysColor(COLOR_HIGHLIGHTTEXT));
				SetBkMode((HDC)wParam, TRANSPARENT);
				return (BOOL)GetSysColorBrush(COLOR_HIGHLIGHT);
			}

			return FALSE;
		}

	case WM_COMMAND:
		{
			UINT uid = LOWORD(wParam);

			if(uid == IDOK){
				if(!GetWindowLongPtr(hDlg,GWLP_USERDATA))
				{
					encryptKeyLength = GetDlgItemTextA(hDlg, IDC_USERPASS, encryptKey, 254);
					EndDialog(hDlg,IDOK);
				}else{

				}
			}else if(uid == IDCANCEL){
				EndDialog(hDlg,IDCANCEL);
			}
		}
	case WM_TIMER:
		{
			LanguageChanged(hDlg);
			return FALSE;
		}
	case WM_DESTROY:
		{
			KillTimer(hDlg, 1);
			DestroyIcon(hIcon);
		}
	}

	return FALSE;
}

BOOL CALLBACK DlgStdNewPass(HWND hDlg, UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HICON hIcon = 0;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			//if(pluginLink && ServiceExists(MS_LANGPACK_TRANSLATEDIALOG))
			TranslateDialogDefault(hDlg);

			hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON2));
			SendMessage(GetDlgItem(hDlg, IDC_HEADERBAR), WM_SETICON, 0, (LPARAM)hIcon);

			if (gl_bUnicodeAwareCore)
				SetWindowText(GetDlgItem(hDlg, IDC_HEADERBAR), TranslateT("Please enter your new password"));
			else
				SetWindowTextA(GetDlgItem(hDlg, IDC_HEADERBAR), Translate("Please enter your new password"));

			oldLangID = 0;
			SetTimer(hDlg,1,200,NULL);
			LanguageChanged(hDlg);

			return TRUE;
		}

	case WM_CTLCOLORSTATIC:
		{
			if ((HWND)lParam == GetDlgItem(hDlg, IDC_LANG))
			{
				SetTextColor((HDC)wParam, GetSysColor(COLOR_HIGHLIGHTTEXT));
				SetBkMode((HDC)wParam, TRANSPARENT);
				return (BOOL)GetSysColorBrush(COLOR_HIGHLIGHT);
			}

			return FALSE;
		}

	case WM_COMMAND:
		{
			UINT uid = LOWORD(wParam);

			if(uid == IDOK){
				if(!GetWindowLongPtr(hDlg,GWLP_USERDATA))
				{
					char pass1[255], pass2[255];
					if(GetDlgItemTextA(hDlg, IDC_USERPASS1, pass1, 254) < 3){
						if (gl_bUnicodeAwareCore)
							SetWindowText(GetDlgItem(hDlg, IDC_HEADERBAR), TranslateT("Password is too short!"));
						else
							SetWindowTextA(GetDlgItem(hDlg, IDC_HEADERBAR), Translate("Password is too short!"));
						SendMessage(GetDlgItem(hDlg, IDC_HEADERBAR), WM_NCPAINT, 0, 0);
						SetDlgItemTextA(hDlg,IDC_USERPASS1,"");
						SetDlgItemTextA(hDlg,IDC_USERPASS2,"");
					}else{

						GetDlgItemTextA(hDlg, IDC_USERPASS2, pass2, 254);
						if(!strcmp(pass1, pass2)){
							encryptKeyLength = strlen(pass1);
							strcpy(encryptKey, pass1);
							EndDialog(hDlg,IDOK);
						}else{
							if (gl_bUnicodeAwareCore)
								SetWindowText(GetDlgItem(hDlg, IDC_HEADERBAR), TranslateT("Passwords do not match!"));
							else
								SetWindowTextA(GetDlgItem(hDlg, IDC_HEADERBAR), Translate("Passwords do not match!"));
							SendMessage(GetDlgItem(hDlg, IDC_HEADERBAR), WM_NCPAINT, 0, 0);
							SetDlgItemTextA(hDlg,IDC_USERPASS1,"");
							SetDlgItemTextA(hDlg,IDC_USERPASS2,"");
						}
					}
				}else{

				}
			}else if(uid == IDCANCEL){
				EndDialog(hDlg,IDCANCEL);
			}
		}
	case WM_TIMER:
		{
			LanguageChanged(hDlg);
			return FALSE;
		}
	case WM_DESTROY:
		{
			KillTimer(hDlg, 1);
			DestroyIcon(hIcon);
			return FALSE;
		}
	}
	return FALSE;
}

char* newPass;

BOOL CALLBACK DlgChangePass(HWND hDlg, UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HICON hIcon = 0;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			//if(pluginLink && ServiceExists(MS_LANGPACK_TRANSLATEDIALOG))
			TranslateDialogDefault(hDlg);

			hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON2));
			SendMessage(GetDlgItem(hDlg, IDC_HEADERBAR), WM_SETICON, 0, (LPARAM)hIcon);
			if (gl_bUnicodeAwareCore)
				SetWindowText(GetDlgItem(hDlg, IDC_HEADERBAR), TranslateT("Change password"));
			else
				SetWindowTextA(GetDlgItem(hDlg, IDC_HEADERBAR), Translate("Change password"));

			newPass = (char*)lParam;
			oldLangID = 0;
			SetTimer(hDlg,1,200,NULL);
			LanguageChanged(hDlg);

			return TRUE;
		}

	case WM_CTLCOLORSTATIC:
		{
			if ((HWND)lParam == GetDlgItem(hDlg, IDC_LANG))
			{
				SetTextColor((HDC)wParam, GetSysColor(COLOR_HIGHLIGHTTEXT));
				SetBkMode((HDC)wParam, TRANSPARENT);
				return (BOOL)GetSysColorBrush(COLOR_HIGHLIGHT);
			}

			return FALSE;
		}

	case WM_COMMAND:
		{
			UINT uid = LOWORD(wParam);

			if(uid == IDOK){
					char pass1[255], pass2[255], oldpass[255];
					GetDlgItemTextA(hDlg, IDC_OLDPASS, oldpass, 254);
					if(strcmp(oldpass, encryptKey)){
						if (gl_bUnicodeAwareCore)
							SetWindowText(GetDlgItem(hDlg, IDC_HEADERBAR), TranslateT("Wrong password!"));
						else
							SetWindowTextA(GetDlgItem(hDlg, IDC_HEADERBAR), Translate("Wrong password!"));
						SendMessage(GetDlgItem(hDlg, IDC_HEADERBAR), WM_NCPAINT, 0, 0);
						break;
					}
					if(GetDlgItemTextA(hDlg, IDC_NEWPASS1, pass1, 254) < 3){
						if (gl_bUnicodeAwareCore)
							SetWindowText(GetDlgItem(hDlg, IDC_HEADERBAR), TranslateT("Password is too short!"));
						else
							SetWindowTextA(GetDlgItem(hDlg, IDC_HEADERBAR), Translate("Password is too short!"));
						SendMessage(GetDlgItem(hDlg, IDC_HEADERBAR), WM_NCPAINT, 0, 0);

					}else{
						GetDlgItemTextA(hDlg, IDC_NEWPASS2, pass2, 254);
						if(!strcmp(pass1, pass2)){
							strcpy(newPass, pass1);
							EndDialog(hDlg,IDOK);
						}else{
							if (gl_bUnicodeAwareCore)
								SetWindowText(GetDlgItem(hDlg, IDC_HEADERBAR), TranslateT("Passwords do not match!"));
							else
								SetWindowTextA(GetDlgItem(hDlg, IDC_HEADERBAR), Translate("Passwords do not match!"));
							SendMessage(GetDlgItem(hDlg, IDC_HEADERBAR), WM_NCPAINT, 0, 0);
						}
					}
			}else if(uid == IDCANCEL){
				EndDialog(hDlg,IDCANCEL);
			}else if(uid == IDREMOVE){
				char oldpass[255];
				GetDlgItemTextA(hDlg, IDC_OLDPASS, oldpass, 254);
				if(strcmp(oldpass, encryptKey)){
					if (gl_bUnicodeAwareCore)
						SetWindowText(GetDlgItem(hDlg, IDC_HEADERBAR), TranslateT("Wrong password!"));
					else
						SetWindowTextA(GetDlgItem(hDlg, IDC_HEADERBAR), Translate("Wrong password!"));
					SendMessage(GetDlgItem(hDlg, IDC_HEADERBAR), WM_NCPAINT, 0, 0);
					break;
				}
				EndDialog(hDlg, IDREMOVE);
			}
		}
	case WM_TIMER:
		{
			LanguageChanged(hDlg);
			return FALSE;
		}
	case WM_DESTROY:
		{
			KillTimer(hDlg, 1);
			return FALSE;
		}
	}
	return FALSE;
}
