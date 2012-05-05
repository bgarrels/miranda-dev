/*
Show Contact Gender plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			 Copyright (C) 2006-2011 by Thief

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

#include "commonheaders.h"

extern HINSTANCE g_hInst;
extern int onExtraImageApplying(WPARAM wParam, LPARAM lParam);
extern int onExtraImageListRebuild(WPARAM wParam, LPARAM lParam);
extern IconExtraColumn g_IECClear;
extern int clistIcon;
extern HANDLE g_hExtraIcon;
extern byte bContactMenuItems, bEnableClistIcon, bDrawNoGenderIcon;
const TCHAR *szAdvancedIcons[] = {LPGENT("Email"), LPGENT("Protocol"), LPGENT("SMS"), LPGENT("Advanced 1"), LPGENT("Advanced 2"), LPGENT("Web"), LPGENT("Client"), LPGENT("Advanced 3"), LPGENT("Advanced 4"), LPGENT("Advanced 5")};
const int cAdvancedIcons = sizeof(szAdvancedIcons) / sizeof(szAdvancedIcons[0]);

INT_PTR CALLBACK DlgProcOptions(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int bInitializing = 0;
	switch (msg)
	{
		case WM_INITDIALOG:
		{ 
			TranslateDialogDefault(hwndDlg);
			bInitializing = 1;
			if (g_hExtraIcon == NULL)
			{
				SendMessage(GetDlgItem(hwndDlg, IDC_ADVICON), CB_ADDSTRING, 0, (LPARAM) TranslateT("Disabled"));
				for (int i = 0; i < cAdvancedIcons; i++)
				{
					SendMessage(GetDlgItem(hwndDlg, IDC_ADVICON), CB_ADDSTRING, 0, (LPARAM) TranslateTS(szAdvancedIcons[i]));
				}
				
				if (bEnableClistIcon)
					SendMessage(GetDlgItem(hwndDlg, IDC_ADVICON), CB_SETCURSEL, clistIcon, 0);
				else
					SendMessage(GetDlgItem(hwndDlg, IDC_ADVICON), CB_SETCURSEL, 0, 0);
			}
			else
			{
				ShowWindow(GetDlgItem(hwndDlg, IDC_USE_L), SW_HIDE);
				ShowWindow(GetDlgItem(hwndDlg, IDC_ADVICON), SW_HIDE);
				ShowWindow(GetDlgItem(hwndDlg, IDC_SLOT_L), SW_HIDE);
				ShowWindow(GetDlgItem(hwndDlg, IDC_NOTE_L), SW_HIDE);
			}
			CheckDlgButton(hwndDlg, IDC_MENUITEMS, DBGetContactSettingByte(NULL, MODULENAME, "MenuItems", 1) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_DRAWUNDEFICON, bDrawNoGenderIcon ? BST_CHECKED : BST_UNCHECKED);
			
			bInitializing = 0;
			return TRUE;
		}
		
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) {
				case IDC_ADVICON:
				{
					if ((HIWORD(wParam) == EN_CHANGE) && (!bInitializing) || (HIWORD(wParam) == CBN_SELENDOK))
					{
						SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
					}
				}
				break;
				case IDC_MENUITEMS:
				case IDC_DRAWUNDEFICON:
					SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
				break;
			}
		}
		break;
		
		case WM_NOTIFY:
		{
			if (((LPNMHDR)lParam)->idFrom == 0)
			switch (((LPNMHDR)lParam)->code)
			{
				case PSN_APPLY:
				{			   
					HANDLE hContact;

					bDrawNoGenderIcon = IsDlgButtonChecked(hwndDlg, IDC_DRAWUNDEFICON);
					
					if (g_hExtraIcon == NULL)
					{
						clistIcon = SendMessage(GetDlgItem(hwndDlg, IDC_ADVICON), CB_GETCURSEL, 0, 0);
						DBWriteContactSettingByte(NULL, MODULENAME, "AdvancedIcon", clistIcon);

						if (bEnableClistIcon)
						{
							hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
							while (hContact)
							{         
								CallService(MS_CLIST_EXTRA_SET_ICON, (WPARAM)hContact, (LPARAM)&g_IECClear);
								hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0);
							}
						}

						bEnableClistIcon = (clistIcon != 0);
						
						if (bEnableClistIcon)
						{					
							g_IECClear.ColumnType = clistIcon;
							onExtraImageListRebuild(0,0);
						}
					}

					if (g_hExtraIcon != NULL || bEnableClistIcon)
					{					
						hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
						while (hContact)
						{         
							onExtraImageApplying((WPARAM)hContact,0);
							hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0);
						}
					}
					
					DBWriteContactSettingByte(NULL, MODULENAME, "ClistIcon", bEnableClistIcon);
					DBWriteContactSettingByte(NULL, MODULENAME, "NoGenderIcon", bDrawNoGenderIcon);
					DBWriteContactSettingByte(NULL, MODULENAME, "MenuItems", IsDlgButtonChecked(hwndDlg, IDC_MENUITEMS)); 
				}
				return TRUE;
			}
		}
		
		case WM_DESTROY:
		break;
	}
	return FALSE;
}

int onOptInitialise(WPARAM wParam, LPARAM lParam)
{
	OPTIONSDIALOGPAGE odp;
	ZeroMemory(&odp, sizeof(odp));
	odp.cbSize = sizeof(odp);
	odp.position = 0;
	odp.hInstance = g_hInst;
	odp.ptszGroup = LPGENT("Plugins");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_GENDER_OPT);
	odp.ptszTitle = LPGENT("Gender");
	odp.pfnDlgProc = DlgProcOptions;
	odp.flags = ODPF_BOLDGROUPS|ODPF_TCHAR;
	CallService(MS_OPT_ADDPAGE, wParam, (LPARAM) &odp);
	
	return 0;
}
