/* 
Copyright (C) 2012 Mataes

This is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this file; see the file license.txt.  If
not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  
*/

#include "common.h"

INT_PTR CALLBACK DlgProcAddFeedOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
		case WM_INITDIALOG:
		{
			SetWindowLongPtr(hwndDlg, GWLP_USERDATA, lParam);
			SetWindowText(hwndDlg, TranslateT("Add Feed"));
			SetDlgItemText(hwndDlg, IDC_FEEDURL, _T("http://"));
			SetDlgItemInt(hwndDlg, IDC_CHECKTIME, 60, false);
			SetDlgItemText(hwndDlg, IDC_TAGSEDIT, _T(TAGSDEFAULT));
			TranslateDialogDefault(hwndDlg);
			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDOK:
					{
						TCHAR str[MAX_PATH];
						if (!GetDlgItemText(hwndDlg, IDC_FEEDTITLE, str, SIZEOF(str)))
						{
							MessageBox(NULL, TranslateT("Enter Feed name"), TranslateT("Error"), MB_OK);
							break;
						}
						else if (!GetDlgItemText(hwndDlg, IDC_FEEDURL, str, SIZEOF(str)) || lstrcmp(str, _T("http://")) == 0)
						{
							MessageBox(NULL, TranslateT("Enter Feed URL"), TranslateT("Error"), MB_OK);
							break;
						}
						else if (!GetDlgItemInt(hwndDlg, IDC_CHECKTIME, false, false))
						{
							MessageBox(NULL, TranslateT("Enter checking interval"), TranslateT("Error"), MB_OK);
							break;
						}
						else if (!GetDlgItemText(hwndDlg, IDC_TAGSEDIT, str, SIZEOF(str)))
						{
							MessageBox(NULL, TranslateT("Enter message format"), TranslateT("Error"), MB_OK);
							break;
						}
						else
						{
							HANDLE hContact = (HANDLE) CallService(MS_DB_CONTACT_ADD, 0, 0);
							CallService(MS_PROTO_ADDTOCONTACT, (WPARAM)hContact, (LPARAM)MODULE);
							GetDlgItemText(hwndDlg, IDC_FEEDTITLE, str, SIZEOF(str));
							DBWriteContactSettingTString(hContact, MODULE, "Nick", str);
							HWND hwndList = (HWND)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
							GetDlgItemText(hwndDlg, IDC_FEEDURL, str, SIZEOF(str));
							DBWriteContactSettingTString(hContact, MODULE, "URL", str);
							DBWriteContactSettingByte(hContact, MODULE, "State", 1);
							DBWriteContactSettingDword(hContact, MODULE, "UpdateTime", GetDlgItemInt(hwndDlg, IDC_CHECKTIME, false, false));
							GetDlgItemText(hwndDlg, IDC_TAGSEDIT, str, SIZEOF(str));
							DBWriteContactSettingTString(hContact, MODULE, "MsgFormat", str);
							DBWriteContactSettingWord(hContact, MODULE, "Status", CallProtoService(MODULE, PS_GETSTATUS, 0, 0));
							if (IsDlgButtonChecked(hwndDlg, IDC_USEAUTH))
							{
								DBWriteContactSettingByte(hContact, MODULE, "UseAuth", 1);
								GetDlgItemText(hwndDlg, IDC_LOGIN, str, SIZEOF(str));
								DBWriteContactSettingTString(hContact, MODULE, "Login", str);
								GetDlgItemText(hwndDlg, IDC_PASSWORD, str, SIZEOF(str));
								DBWriteContactSettingTString(hContact, MODULE, "Password", str);
							}
							DeleteAllItems(hwndList);
							UpdateList(hwndList);
						}
					}

				case IDCANCEL:
					DestroyWindow(hwndDlg);
					break;

				case IDC_USEAUTH:
					{
						if (IsDlgButtonChecked(hwndDlg, IDC_USEAUTH))
						{
							EnableWindow(GetDlgItem(hwndDlg, IDC_LOGIN), TRUE);
							EnableWindow(GetDlgItem(hwndDlg, IDC_PASSWORD), TRUE);
						}
						else
						{
							EnableWindow(GetDlgItem(hwndDlg, IDC_LOGIN), FALSE);
							EnableWindow(GetDlgItem(hwndDlg, IDC_PASSWORD), FALSE);
						}
					}
					break;

				case IDC_TAGHELP:
					MessageBox(NULL, TranslateT(TAGSHELP), TranslateT("Feed Tag Help"), MB_OK);
					break;

				case IDC_RESET:
					if (MessageBox(NULL, TranslateT("Are you sure?"), TranslateT("Tags Mask Reset"), MB_YESNO | MB_ICONWARNING) == IDYES)
						SetDlgItemText(hwndDlg, IDC_TAGSEDIT, _T(TAGSDEFAULT));
					break;
			}
			break;
		}
		case WM_CLOSE:
		{
			DestroyWindow(hwndDlg);
			break;
		}
	}

	return FALSE;
}

INT_PTR CALLBACK DlgProcChangeFeedOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
		case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);
			ItemInfo &SelItem = *(ItemInfo*)lParam;
			ItemInfo *nSelItem = new ItemInfo(SelItem);
			SetWindowText(hwndDlg, TranslateT("Change Feed"));

			HANDLE hContact= (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
			while (hContact != NULL) 
			{
				if(IsMyContact(hContact)) 
				{
					DBVARIANT dbVar = {0};
					DBGetContactSettingTString(hContact, MODULE, "Nick", &dbVar);
					if (lstrcmp(dbVar.ptszVal, NULL) == 0)
						DBFreeVariant(&dbVar);
					else if (lstrcmp(dbVar.ptszVal, SelItem.nick) == 0)
					{
						DBGetContactSettingTString(hContact, MODULE, "URL", &dbVar);
						if (lstrcmp(dbVar.ptszVal, NULL) == 0)
							DBFreeVariant(&dbVar);
						else if (lstrcmp(dbVar.ptszVal, SelItem.url) == 0)
						{
							nSelItem->hContact = hContact;
							SetWindowLongPtr(hwndDlg, GWLP_USERDATA, (LONG)nSelItem);
							SetDlgItemText(hwndDlg, IDC_FEEDURL, SelItem.url);
							SetDlgItemText(hwndDlg, IDC_FEEDTITLE, SelItem.nick);
							SetDlgItemInt(hwndDlg, IDC_CHECKTIME, DBGetContactSettingDword(hContact, MODULE, "UpdateTime", 0), false);
							DBGetContactSettingTString(hContact, MODULE, "MsgFormat", &dbVar);
							if (lstrcmp(dbVar.ptszVal, NULL) == 0)
								DBFreeVariant(&dbVar);
							else
								SetDlgItemText(hwndDlg, IDC_TAGSEDIT, dbVar.ptszVal);
							if (DBGetContactSettingByte(hContact, MODULE, "UseAuth", 0))
							{
								CheckDlgButton(hwndDlg, IDC_USEAUTH, BST_CHECKED);
								EnableWindow(GetDlgItem(hwndDlg, IDC_LOGIN), TRUE);
								EnableWindow(GetDlgItem(hwndDlg, IDC_PASSWORD), TRUE);
								DBGetContactSettingTString(hContact, MODULE, "Login", &dbVar);
								if (lstrcmp(dbVar.ptszVal, NULL) == 0)
									DBFreeVariant(&dbVar);
								else
									SetDlgItemText(hwndDlg, IDC_LOGIN, dbVar.ptszVal);
								DBGetContactSettingTString(hContact, MODULE, "Password", &dbVar);
								if (lstrcmp(dbVar.ptszVal, NULL) == 0)
									DBFreeVariant(&dbVar);
								else
									SetDlgItemText(hwndDlg, IDC_PASSWORD, dbVar.ptszVal);
							}
							break;
						}
					}
				}
				hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0);
			}
			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDOK:
					{
						ItemInfo *SelItem = (ItemInfo*)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
						TCHAR str[MAX_PATH];
						if (!GetDlgItemText(hwndDlg, IDC_FEEDTITLE, str, SIZEOF(str)))
						{
							MessageBox(NULL, TranslateT("Enter Feed name"), TranslateT("Error"), MB_OK);
							break;
						}
						else if (!GetDlgItemText(hwndDlg, IDC_FEEDURL, str, SIZEOF(str)) || lstrcmp(str, _T("http://")) == 0)
						{
							MessageBox(NULL, TranslateT("Enter Feed URL"), TranslateT("Error"), MB_OK);
							break;
						}
						else if (!GetDlgItemInt(hwndDlg, IDC_CHECKTIME, false, false))
						{
							MessageBox(NULL, TranslateT("Enter checking interval"), TranslateT("Error"), MB_OK);
							break;
						}
						else if (!GetDlgItemText(hwndDlg, IDC_TAGSEDIT, str, SIZEOF(str)))
						{
							MessageBox(NULL, TranslateT("Enter message format"), TranslateT("Error"), MB_OK);
							break;
						}
						else
						{
							GetDlgItemText(hwndDlg, IDC_FEEDURL, str, SIZEOF(str));
							DBWriteContactSettingTString(SelItem->hContact, MODULE, "URL", str);
							GetDlgItemText(hwndDlg, IDC_FEEDTITLE, str, SIZEOF(str));
							DBWriteContactSettingTString(SelItem->hContact, MODULE, "Nick", str);
							DBWriteContactSettingDword(SelItem->hContact, MODULE, "UpdateTime", GetDlgItemInt(hwndDlg, IDC_CHECKTIME, false, false));
							GetDlgItemText(hwndDlg, IDC_TAGSEDIT, str, SIZEOF(str));
							DBWriteContactSettingTString(SelItem->hContact, MODULE, "MsgFormat", str);
							if (IsDlgButtonChecked(hwndDlg, IDC_USEAUTH))
							{
								DBWriteContactSettingByte(SelItem->hContact, MODULE, "UseAuth", 1);
								GetDlgItemText(hwndDlg, IDC_LOGIN, str, SIZEOF(str));
								DBWriteContactSettingTString(SelItem->hContact, MODULE, "Login", str);
								GetDlgItemText(hwndDlg, IDC_PASSWORD, str, SIZEOF(str));
								DBWriteContactSettingTString(SelItem->hContact, MODULE, "Password", str);
							}
							DeleteAllItems(SelItem->hwndList);
							UpdateList(SelItem->hwndList);
						}
					}

				case IDCANCEL:
					DestroyWindow(hwndDlg);
					break;

				case IDC_USEAUTH:
					{
						if (IsDlgButtonChecked(hwndDlg, IDC_USEAUTH))
						{
							EnableWindow(GetDlgItem(hwndDlg, IDC_LOGIN), TRUE);
							EnableWindow(GetDlgItem(hwndDlg, IDC_PASSWORD), TRUE);
						}
						else
						{
							EnableWindow(GetDlgItem(hwndDlg, IDC_LOGIN), FALSE);
							EnableWindow(GetDlgItem(hwndDlg, IDC_PASSWORD), FALSE);
						}
						break;
					}

				case IDC_TAGHELP:
					MessageBox(NULL, TranslateT(TAGSHELP), TranslateT("Feed Tag Help"), MB_OK);
					break;

				case IDC_RESET:
					if (MessageBox(NULL, TranslateT("Are you sure?"), TranslateT("Tags Mask Reset"), MB_YESNO | MB_ICONWARNING) == IDYES)
						SetDlgItemText(hwndDlg, IDC_TAGSEDIT, _T(TAGSDEFAULT));
					break;
			}
			break;
		}

		case WM_CLOSE:
		{
			DestroyWindow(hwndDlg);
			break;
		}

		case WM_DESTROY:
		{
			ItemInfo *SelItem = (ItemInfo*)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
			delete SelItem;
			break;
		}
	}

	return FALSE;
}

INT_PTR CALLBACK UpdateNotifyOptsProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hwndList = GetDlgItem(hwndDlg, IDC_FEEDLIST);
	switch (msg)
	{
	case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);
			SetWindowLongPtr(hwndDlg, GWLP_USERDATA, 0);
			CreateList(hwndList);
			UpdateList(hwndList);
			return TRUE;
		}

	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) 
		{
			case IDC_ADD:
				{
					CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_ADDFEED), hwndDlg, DlgProcAddFeedOpts, (LPARAM)hwndList);
				}
				return FALSE;
			case IDC_CHANGE:
				{
					ItemInfo SelItem = {0};
					int sel = ListView_GetSelectionMark(hwndList);
					ListView_GetItemText(hwndList, sel, 0, SelItem.nick, MAX_PATH);								
					ListView_GetItemText(hwndList, sel, 1, SelItem.url, MAX_PATH);
					SelItem.hwndList = hwndList;
					SelItem.SelNumber = sel;
					CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_ADDFEED), hwndDlg, DlgProcChangeFeedOpts, (LPARAM)&SelItem);
				}
				return FALSE;
			case IDC_REMOVE:
				{
					if (MessageBox(NULL, TranslateT("Are you sure?"), TranslateT("Contact deleting"), MB_YESNO | MB_ICONWARNING) == IDYES)
					{
						TCHAR nick[MAX_PATH], url[MAX_PATH];
						int sel = ListView_GetSelectionMark(hwndList);
						ListView_GetItemText(hwndList, sel, 0, nick, MAX_PATH);								
						ListView_GetItemText(hwndList, sel, 1, url, MAX_PATH);								

						HANDLE hContact= (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
						while (hContact != NULL) 
						{
							if(IsMyContact(hContact)) 
							{
								DBVARIANT dbVar = {0};
								DBGetContactSettingTString(hContact, MODULE, "Nick", &dbVar);
								if (lstrcmp(dbVar.ptszVal, NULL) == 0)
									DBFreeVariant(&dbVar);
								else if (lstrcmp(dbVar.ptszVal, nick) == 0)
								{
									DBGetContactSettingTString(hContact, MODULE, "URL", &dbVar);
									if (lstrcmp(dbVar.ptszVal, NULL) == 0)
										DBFreeVariant(&dbVar);
									else if (lstrcmp(dbVar.ptszVal, url) == 0)
									{
										CallService(MS_DB_CONTACT_DELETE, (WPARAM)hContact, 0);
										ListView_DeleteItem(hwndList, sel);
										break;
									}
								}
							}
							hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0);
						}
					}
					return FALSE;
				}
		}
		break;
	}
	case WM_NOTIFY:
		{
			NMHDR *hdr = (NMHDR *)lParam;
			switch (hdr->code)
			{
				case PSN_APPLY:
					{
						HANDLE hContact= (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
						int i = 0;
						while (hContact != NULL) 
						{
							if(IsMyContact(hContact)) 
							{
								DBWriteContactSettingByte(hContact, MODULE, "State", ListView_GetCheckState(hwndList, i));
								i += 1;
							}
							hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0);
						}
						break;
					}

				case NM_DBLCLK:
					{
						ItemInfo SelItem = {0};
						int sel = ListView_GetHotItem(hwndList);
						if (sel != -1)
						{
							ListView_GetItemText(hwndList, sel, 0, SelItem.nick, MAX_PATH);								
							ListView_GetItemText(hwndList, sel, 1, SelItem.url, MAX_PATH);								
							SelItem.hwndList = hwndList;
							SelItem.SelNumber = sel;
							CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_ADDFEED), hwndDlg, DlgProcChangeFeedOpts, (LPARAM)&SelItem);
						}
						break;
					}

			case LVN_ITEMCHANGED:
				{
					NMLISTVIEW *nmlv = (NMLISTVIEW *)lParam;
					if(((nmlv->uNewState ^ nmlv->uOldState) & LVIS_STATEIMAGEMASK) && !UpdateListFlag)
					{
						SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
					}
					break;
				}
			}
		}
	}//end* switch (msg)
	return FALSE;
}

INT OptInit(WPARAM wParam, LPARAM lParam)
{
 	OPTIONSDIALOGPAGE odp = {0};

	ZeroMemory(&odp, sizeof(odp));
	odp.cbSize = sizeof(odp);
    odp.position = 100000000;
    odp.hInstance = hInst;
    odp.flags = ODPF_TCHAR | ODPF_BOLDGROUPS;
    odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPTIONS);
    odp.ptszGroup = _T("Network");
    odp.ptszTitle = _T("News Aggregator");
    odp.pfnDlgProc = UpdateNotifyOptsProc;
    CallService(MS_OPT_ADDPAGE, wParam, (LPARAM)&odp);
    return 0;
}