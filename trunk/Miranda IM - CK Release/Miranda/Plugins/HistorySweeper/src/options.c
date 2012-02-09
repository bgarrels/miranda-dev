/*
Miranda IM History Sweeper plugin
Copyright (C) 2002-2003  Sergey V. Gershovich
Copyright (C) 2006-2009  Boris Krasnovskiy

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
*/

#include "historysweeper.h"

// Menu items
const char* menu_items[] = 
{
	LPGEN("Sweep system history"),
	LPGEN("Sweep whole history"),
	LPGEN("Sweep history from selected contacts")
};

// Time Stamps strings
static const char* time_stamp_strings[] = 
{
	LPGEN("All events"), 
	LPGEN("Older than 1 Day"), 
	LPGEN("Older than 3 Days"), 
	LPGEN("Older than 7 Days"), 
	LPGEN("Older than 2 Weeks (14 days)"),
	LPGEN("Older than 1 Month (30 days)"), 
	LPGEN("Older than 3 Months (90 days)"),
	LPGEN("Older than 6 Months (180 days)"),
	LPGEN("Older than 1 Year (364 days)")
};


struct
{
	char* szDescr;
	char* szName;
	int   defIconID;
}
static const iconList[] =
{
	{ LPGEN("Main"),             "main",    IDI_MAINICON     },
	{ LPGEN("History Enabled"),  "keep",    IDI_KEEP         },
	{ LPGEN("History Disabled"), "purge",   IDI_PURGE        },
	{ LPGEN("Custom Action"),    "custact", IDI_CUSTOMACTION },
};

static HANDLE hIconLibItem[SIZEOF(iconList)];

void InitIcons(void)
{
	int i;
	char szSettingName[100];
	SKINICONDESC sid = {0};

	TCHAR szFile[MAX_PATH];
	GetModuleFileName(hInst, szFile, SIZEOF(szFile));

	sid.cbSize = sizeof(SKINICONDESC);
	sid.ptszDefaultFile = szFile;
	sid.cx = sid.cy = 16;
	sid.pszName = szSettingName;
	sid.pszSection = ModuleName;
	sid.flags = SIDF_PATH_TCHAR;

	for (i = 0; i < SIZEOF(iconList); i++) 
	{
		mir_snprintf(szSettingName, SIZEOF(szSettingName), "%s_%s", ModuleName, iconList[i].szName);

		sid.pszDescription = (char*)iconList[i].szDescr;
		sid.iDefaultIndex = -iconList[i].defIconID;
		hIconLibItem[i] = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);
	}	
}

HICON LoadIconEx(const char* name)
{
	char szSettingName[100];
	mir_snprintf(szSettingName, sizeof(szSettingName), "%s_%s", ModuleName, name);
	return (HICON)CallService(MS_SKIN2_GETICON, 0, (LPARAM)szSettingName);
}

HANDLE GetIconHandle(const char* name)
{
	unsigned i;

	for (i=0; i < SIZEOF(iconList); i++)
		if (strcmp(iconList[i].szName, name) == 0)
			return hIconLibItem[i];

	return NULL;
}

void  ReleaseIconEx(const char* name)
{
	char szSettingName[100];
	mir_snprintf(szSettingName, sizeof(szSettingName), "%s_%s", ModuleName,name);
	CallService(MS_SKIN2_RELEASEICON, 0, (LPARAM)szSettingName);
}

HANDLE hAllContacts, hSystemHistory; 

void ResetListOptions(HWND hwndList)
{
	int i;

	SendMessage(hwndList, CLM_SETBKBITMAP, 0, 0);
	SendMessage(hwndList, CLM_SETBKCOLOR, GetSysColor(COLOR_WINDOW), 0);
	SendMessage(hwndList, CLM_SETGREYOUTFLAGS, 0, 0);
	SendMessage(hwndList, CLM_SETLEFTMARGIN, 4, 0);
	SendMessage(hwndList, CLM_SETINDENT, 10, 0);

	for (i=0; i<=FONTID_MAX; i++)
		SendMessage(hwndList, CLM_SETTEXTCOLOR, i, GetSysColor(COLOR_WINDOWTEXT));
}

static void SetContactIcons(HANDLE hItem, HWND hwndList, int save)
{
	int st;
	const char *mdl = "CList";
	const char *stt = "SweepHistory";
	HANDLE hContact = hItem;

	if (hItem == hAllContacts || hItem == hSystemHistory)
	{
		mdl = ModuleName;
		stt = (hItem == hSystemHistory) ? "SweepSHistory" : "SweepHistory";
		hContact = NULL;
	}

	if (save)
	{
		st = SendMessage(hwndList, CLM_GETEXTRAIMAGE, (WPARAM)hItem, 0);
		if (st > 0)
			DBWriteContactSettingByte(hContact, mdl, stt, (BYTE)st);
		else
			DBDeleteContactSetting(hContact, mdl, stt);
	}
	else
	{
		st = DBGetContactSettingByte(hContact, mdl, stt, 0);
		SendMessage(hwndList, CLM_SETEXTRAIMAGE, (WPARAM)hItem, MAKELPARAM(0, st));
	}
}

static void SetAllContactIcons(HANDLE hItem, HWND hwndList, int save)
{
	HANDLE hItemT;

	hItemT = hItem ? hItem : (HANDLE)SendMessage(hwndList, CLM_GETNEXTITEM, CLGN_ROOT, 0);
	while (hItemT) 
	{
		if (IsHContactContact(hItemT))
			SetContactIcons(hItemT, hwndList, save);

		hItemT = (HANDLE)SendMessage(hwndList, CLM_GETNEXTITEM, CLGN_NEXTCONTACT, (LPARAM)hItemT);
	}

	hItemT = hItem ? hItem : (HANDLE)SendMessage(hwndList, CLM_GETNEXTITEM, CLGN_ROOT, 0);
	while (hItemT) 
	{
		HANDLE hItemTT = (HANDLE)SendMessage(hwndList, CLM_GETNEXTITEM, CLGN_CHILD, (LPARAM)hItemT);
		if (hItemTT) SetAllContactIcons(hItemTT, hwndList, save);
		hItemT = (HANDLE)SendMessage(hwndList, CLM_GETNEXTITEM, CLGN_NEXTGROUP, (LPARAM)hItemT);
	}

	if (hItem == NULL)
	{
		SetContactIcons(hSystemHistory, hwndList, save);
		SetContactIcons(hAllContacts, hwndList, save);
	}
}

void LoadSettings(HWND hwndDlg)
{
	int i;
	CLCINFOITEM cii = {0};

	HWND hwndList = GetDlgItem(hwndDlg, IDC_LIST);

	cii.cbSize = sizeof(cii);
	cii.flags = CLCIIF_GROUPFONT;

	cii.pszText = TranslateT("*** All contacts ***");
	hAllContacts = (HANDLE)SendMessage(hwndList, CLM_ADDINFOITEM, 0, (LPARAM)&cii);

	cii.pszText = TranslateT("*** System History ***");
	hSystemHistory = (HANDLE)SendMessage(hwndList, CLM_ADDINFOITEM, 0, (LPARAM)&cii);

	SetAllContactIcons(NULL, hwndList, 0);

	SendDlgItemMessage(hwndDlg, IDC_MMITEM, CB_RESETCONTENT, 0, 0);
	for (i = 0; i < SIZEOF(menu_items); i++) 
	{
		TCHAR* ptszMenuItem = (TCHAR*)CallService(MS_LANGPACK_PCHARTOTCHAR, 0, (LPARAM)menu_items[i]);
		SendDlgItemMessage(hwndDlg, IDC_MMITEM, CB_ADDSTRING, 0, (LPARAM)ptszMenuItem);
		mir_free(ptszMenuItem);
	}

	SendDlgItemMessage(hwndDlg, IDC_MMOLDER, CB_RESETCONTENT, 0, 0);
	SendDlgItemMessage(hwndDlg, IDC_SSOLDER, CB_RESETCONTENT, 0, 0);

	for (i = 0; i < SIZEOF(time_stamp_strings); i++) 
	{
		TCHAR* ptszTimeStr = (TCHAR*)CallService(MS_LANGPACK_PCHARTOTCHAR, 0, (LPARAM)time_stamp_strings[i]);
		SendDlgItemMessage(hwndDlg, IDC_MMOLDER, CB_ADDSTRING, 0, (LPARAM)ptszTimeStr);
		SendDlgItemMessage(hwndDlg, IDC_SSOLDER, CB_ADDSTRING, 0, (LPARAM)ptszTimeStr);
		mir_free(ptszTimeStr);
	}
	
	SendDlgItemMessage(hwndDlg, IDC_MMITEM, CB_SETCURSEL, DBGetContactSettingByte(NULL, ModuleName, "MainMenuAction", 0), 0);
	SendDlgItemMessage(hwndDlg, IDC_MMOLDER, CB_SETCURSEL, DBGetContactSettingByte(NULL, ModuleName, "MainMenuOlder", 0), 0);
	SendDlgItemMessage(hwndDlg, IDC_SSOLDER, CB_SETCURSEL, DBGetContactSettingByte(NULL, ModuleName, "StartupShutdownOlder", 0), 0);
	
	CheckDlgButton(hwndDlg, IDC_UNSAFEMODE, DBGetContactSettingByte(NULL, ModuleName, "UnsafeMode", 0));
	CheckDlgButton(hwndDlg, IDC_SWEEPONCLOSE, DBGetContactSettingByte(NULL, ModuleName, "SweepOnClose", 0));
}


void SaveSettings(HWND hwndDlg)
{
	HWND listItem = GetDlgItem(hwndDlg, IDC_LIST);

	SetAllContactIcons(NULL, listItem, 1);
	
	DBWriteContactSettingByte(NULL, ModuleName, "MainMenuAction", (BYTE)SendDlgItemMessage(hwndDlg, IDC_MMITEM, CB_GETCURSEL, 0, 0));
	DBWriteContactSettingByte(NULL, ModuleName, "MainMenuOlder", (BYTE)SendDlgItemMessage(hwndDlg, IDC_MMOLDER, CB_GETCURSEL, 0, 0));
	DBWriteContactSettingByte(NULL, ModuleName, "StartupShutdownOlder", (BYTE)SendDlgItemMessage(hwndDlg, IDC_SSOLDER, CB_GETCURSEL, 0, 0));
	DBWriteContactSettingByte(NULL, ModuleName, "UnsafeMode", (BYTE)IsDlgButtonChecked(hwndDlg, IDC_UNSAFEMODE));
	DBWriteContactSettingByte(NULL, ModuleName, "SweepOnClose", (BYTE)IsDlgButtonChecked(hwndDlg, IDC_SWEEPONCLOSE));

	ModifyMainMenu();
}

INT_PTR CALLBACK DlgProcHSOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		TranslateDialogDefault(hwndDlg);
		{
			HIMAGELIST hIml = ImageList_Create(
				GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
				ILC_MASK | (IsWinVerXPPlus() ? ILC_COLOR32 : ILC_COLOR16 ), 2, 2 );

			HICON hIcon = LoadSkinnedIcon(SKINICON_OTHER_SMALLDOT);
			ImageList_AddIcon(hIml, hIcon);
			CallService(MS_SKIN2_RELEASEICON, (WPARAM)hIcon, 0);

			hIcon =  LoadIconEx("custact");
			ImageList_AddIcon(hIml, hIcon);
			SendDlgItemMessage(hwndDlg, IDC_CUSTACT_ICO, STM_SETICON, (WPARAM)hIcon, 0);

			hIcon =  LoadIconEx("purge");
			ImageList_AddIcon(hIml, hIcon);
			SendDlgItemMessage(hwndDlg, IDC_NOH_ICO, STM_SETICON, (WPARAM)hIcon, 0);

			SendDlgItemMessage(hwndDlg, IDC_LIST, CLM_SETEXTRAIMAGELIST, 0, (LPARAM)hIml);
			SendDlgItemMessage(hwndDlg, IDC_LIST, CLM_SETEXTRACOLUMNS, 1, 0);
		}
		ResetListOptions(GetDlgItem(hwndDlg, IDC_LIST));
		LoadSettings(hwndDlg);
		return TRUE;

	case WM_DESTROY:
		{
			HIMAGELIST hIml = (HIMAGELIST)SendDlgItemMessage(hwndDlg, IDC_LIST, CLM_GETEXTRAIMAGELIST, 0, 0);
			ImageList_Destroy(hIml);
			ReleaseIconEx("purge");
			ReleaseIconEx("custact");
		}
		break;

	case WM_SETFOCUS: 
		SetFocus(GetDlgItem(hwndDlg,IDC_LIST));
		break;

	case WM_COMMAND:
		SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0); // Send message to activate "Apply" button
		return TRUE;

	case WM_NOTIFY:
		{
			NMCLISTCONTROL* nmc = (NMCLISTCONTROL*)lParam;
			if ( nmc->hdr.idFrom == 0 && nmc->hdr.code == (unsigned)PSN_APPLY )
			{
				SaveSettings(hwndDlg);
			}
			else if (nmc->hdr.idFrom == IDC_LIST)
			{
				switch (nmc->hdr.code) 
				{
//              case CLN_CONTACTMOVED:
				case CLN_NEWCONTACT:
					if ((nmc->flags & (CLNF_ISGROUP | CLNF_ISINFO)) == 0) 
						SetContactIcons(nmc->hItem, nmc->hdr.hwndFrom, 0);
					break;

				case CLN_LISTREBUILT:
					SetAllContactIcons(NULL, nmc->hdr.hwndFrom, 0);
					break;

				case CLN_OPTIONSCHANGED:
					ResetListOptions(nmc->hdr.hwndFrom);
					break;

				case NM_CLICK:
					{
						HANDLE hItem;
						DWORD hitFlags;
						int iImage;

						if ( nmc->iColumn == -1 )
							break;

						// Find clicked item
						hItem = (HANDLE)SendMessage(nmc->hdr.hwndFrom, CLM_HITTEST, (WPARAM)&hitFlags, MAKELPARAM(nmc->pt.x,nmc->pt.y));
						// Nothing was clicked
						if (hItem == NULL || !(IsHContactContact(hItem) || IsHContactInfo(hItem)))
							break;

						// It was not our extended icon
						if (!(hitFlags & CLCHT_ONITEMEXTRA))
							break;

						iImage = SendMessage(nmc->hdr.hwndFrom, CLM_GETEXTRAIMAGE, (WPARAM)hItem, MAKELPARAM(nmc->iColumn, 0));
						SendMessage(nmc->hdr.hwndFrom, CLM_SETEXTRAIMAGE, (WPARAM)hItem, MAKELPARAM(nmc->iColumn, (iImage + 1) % 3));

						// Activate Apply button
						SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
					}
					break;
				} 
			}
		}
		break;
	}
	return FALSE;
}

int HSOptInitialise(WPARAM wParam,LPARAM lParam)
{
	OPTIONSDIALOGPAGE odp = {0};

	odp.cbSize = sizeof(odp);
	odp.position = 100100000;
	odp.hInstance = hInst;
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_HISTORYSWEEPER);
	odp.pszTitle = ModuleName;
	odp.pszGroup = "History";
	odp.pfnDlgProc = DlgProcHSOpts;
	odp.flags = ODPF_BOLDGROUPS;	

	CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);

	return 0;
}
