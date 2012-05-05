/*
PackUpdater plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C)	2010-2012 Mataes
							2007 ZERO_BiT

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

#include "common.h"

HWND hDlgDld = NULL;
INT UpdatesCount = 0;

void PopupAction(HWND hWnd, BYTE action)
{
	switch (action)
	{
		case PCA_CLOSEPOPUP: 
			break;
		case PCA_DONOTHING:
			return;
	}//end* switch
	PUDeletePopUp(hWnd);
}

static INT_PTR CALLBACK PopupDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case UM_POPUPACTION:
		{
			if (HIWORD(wParam) == BN_CLICKED)
			{
				LPMSGPOPUPDATA pmpd = (LPMSGPOPUPDATA)PUGetPluginData(hDlg);

				if (pmpd)
				{
					switch (LOWORD(wParam))
					{
					case IDYES:
						{
							if (IsWindow(pmpd->hDialog))
								EndDialog(pmpd->hDialog, LOWORD(wParam));
							PUDeletePopUp(hDlg);
							break;
						}
					case IDNO:
						{
							if (IsWindow(pmpd->hDialog))
								EndDialog(pmpd->hDialog, LOWORD(wParam));
							PUDeletePopUp(hDlg);
							break;
						}
					}
				}
			}
		}
		break;

		case UM_FREEPLUGINDATA:
		{
			LPMSGPOPUPDATA pmpd = (LPMSGPOPUPDATA)PUGetPluginData(hDlg);
			if (pmpd > 0)
				mir_free(pmpd);
			return TRUE; //TRUE or FALSE is the same, it gets ignored.
		}
		break;

		default:
		break;
	}
	return DefWindowProc(hDlg, uMsg, wParam, lParam);
}

static INT_PTR CALLBACK PopupDlgProc2(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND: 
		{
			PopupAction(hDlg, MyOptions.LeftClickAction);
			break; 
		}
		case WM_CONTEXTMENU: 
		{
			PopupAction(hDlg, MyOptions.RightClickAction);
			break;
		}
		case UM_FREEPLUGINDATA:
		{
			LPMSGPOPUPDATA pmpd = (LPMSGPOPUPDATA)PUGetPluginData(hDlg);
			if (pmpd > 0)
				mir_free(pmpd);
			return TRUE; //TRUE or FALSE is the same, it gets ignored.
		}
		break;
	}
	return DefWindowProc(hDlg, uMsg, wParam, lParam);
}

static VOID MakePopupAction(POPUPACTION &pa, INT id)
{
	pa.cbSize = sizeof(POPUPACTION);
	pa.flags = PAF_ENABLED;
	pa.wParam = MAKEWORD(id, BN_CLICKED);
	pa.lParam = 0;
	switch (id)
	{
	case IDYES:
		{
			pa.lchIcon = (HICON)CallService(MS_SKIN2_GETICON, 0, (LPARAM)"btn_ok");
			strncpy_s(pa.lpzTitle, MODNAME"/Yes", SIZEOF(pa.lpzTitle));
		}
		break;
	case IDNO:
		{
			pa.lchIcon = (HICON)CallService(MS_SKIN2_GETICON, 0, (LPARAM)"btn_cancel");
			strncpy_s(pa.lpzTitle, MODNAME"/No", SIZEOF(pa.lpzTitle));
		}
		break;
	}
}
		
VOID show_popup(HWND hDlg, LPCTSTR Title, LPCTSTR Text, int Number, int ActType)
{
	POPUPDATAT_V2 pd;
	LPMSGPOPUPDATA	pmpd;
	pmpd = (LPMSGPOPUPDATA)mir_alloc(sizeof(MSGPOPUPDATA));
	if (pmpd)
	{
		ZeroMemory(&pd, sizeof(pd));
		pd.cbSize = sizeof(POPUPDATAT_V2);
		pd.lchContact = NULL; //(HANDLE)wParam;
		pd.lchIcon = LoadSkinnedIcon(PopupsList[Number].Icon);
		lstrcpyn(pd.lptzText, Text, SIZEOF(pd.lptzText));
		lstrcpyn(pd.lptzContactName, Title, SIZEOF(pd.lptzContactName));
		switch (MyOptions.DefColors)
		{
			case byCOLOR_WINDOWS:
				pd.colorBack = GetSysColor(COLOR_BTNFACE);
				pd.colorText = GetSysColor(COLOR_WINDOWTEXT);
				break;
			case byCOLOR_OWN:
				pd.colorBack = PopupsList[Number].colorBack;
				pd.colorText = PopupsList[Number].colorText;
				break;
			case byCOLOR_POPUP:
				pd.colorBack = pd.colorText = 0;
				break;
		}//end* switch
		if (Number == 0 && ActType != 0)
			pd.PluginWindowProc = (WNDPROC)PopupDlgProc;
		else
			pd.PluginWindowProc = (WNDPROC)PopupDlgProc2;
		pd.PluginData = pmpd;
		if (Number == 0)
			pd.iSeconds = -1;
		else
			pd.iSeconds = MyOptions.Timeout;
		pd.hNotification = NULL;
		pd.lpActions = pmpd->pa;
	}
	pmpd->hDialog = hDlg;
	switch (ActType)
	{
		case 0:
			break;
		case 1:
		{
			MakePopupAction(pmpd->pa[pd.actionCount++], IDYES);
			MakePopupAction(pmpd->pa[pd.actionCount++], IDNO);
		}
		break;
	}

	CallService(MS_POPUP_ADDPOPUPT, (WPARAM) &pd, APF_NEWDATA);
}

INT_PTR CALLBACK DlgDownload(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
			SetWindowText(GetDlgItem(hDlg, IDC_LABEL), tszDialogMsg);
			SetWindowLongPtr(GetDlgItem(hDlg, IDC_PB), GWL_STYLE, GetWindowLongPtr(GetDlgItem(hDlg, IDC_PB), GWL_STYLE) | PBS_MARQUEE);
			SendMessage(GetDlgItem(hDlg, IDC_PB), PBM_SETMARQUEE, 1, 50);
			return TRUE;
	}//end* switch
	return FALSE;
}

INT_PTR CALLBACK DlgDownloadPop(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			Number = 3;
			show_popup(hDlg, Title, Text, Number, 0);
			return TRUE;
		} // end* WM_INITDIALOG:
	} // end* switch (uMsg)
	return FALSE;
}

static void __stdcall CreateDownloadDialog(void*)
{
	if (ServiceExists(MS_POPUP_ADDPOPUPEX) && DBGetContactSettingByte(NULL, "PopUp", "ModuleIsEnabled", 1) &&  DBGetContactSettingByte(NULL,MODNAME, "Popups3", DEFAULT_POPUP_ENABLED))
		hDlgDld = CreateDialog(hInst, MAKEINTRESOURCE(IDD_POPUPDUMMI), NULL, DlgDownloadPop);
	else if (DBGetContactSettingByte(NULL,MODNAME, "Popups3M", DEFAULT_MESSAGE_ENABLED))
	{
		lstrcpyn(tszDialogMsg, Text, SIZEOF(tszDialogMsg));
		hDlgDld = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DOWNLOAD), NULL, DlgDownload);
	}
}

static void __stdcall DestroyDownloadDialog(void*)
{
	DestroyWindow(hDlgDld);
}

void DlgDownloadProc()
{
	CallFunctionAsync(CreateDownloadDialog, 0);
	if (!DownloadFile(pFileUrl->tszDownloadURL, pFileUrl->tszDiskPath))
	{
		Title = TranslateT("Pack Updater");
		Text = TranslateT("An error occured while downloading the update.");
		if (ServiceExists(MS_POPUP_ADDPOPUPEX) && DBGetContactSettingByte(NULL, "PopUp", "ModuleIsEnabled", 1) &&  DBGetContactSettingByte(NULL, MODNAME, "Popups1", DEFAULT_POPUP_ENABLED))
		{
			Number = 1;
			show_popup(0, Title, Text, Number, 0);
		}
		else if (DBGetContactSettingByte(NULL, MODNAME, "Popups1M", DEFAULT_MESSAGE_ENABLED))
			MessageBox(NULL, Text, Title, MB_ICONSTOP);
	}
	CallFunctionAsync(DestroyDownloadDialog, 0);
}

INT_PTR CALLBACK DlgUpdate(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hwndList = GetDlgItem(hDlg, IDC_LIST_UPDATES);

	switch (message)
	{
		case WM_INITDIALOG:
			{
				TranslateDialogDefault( hDlg );
				SetWindowLongPtr(hDlg, GWLP_USERDATA, 0);
				SendMessage(hwndList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

				LVCOLUMN lvc = {0}; 
				// Initialize the LVCOLUMN structure.
				// The mask specifies that the format, width, text, and
				// subitem members of the structure are valid. 
				lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
				lvc.fmt = LVCFMT_LEFT;
	  
				lvc.iSubItem = 0;
				lvc.pszText = TranslateT("Component Name");	
				lvc.cx = 145;     // width of column in pixels
				ListView_InsertColumn(hwndList, 0, &lvc);

				lvc.iSubItem = 1;
				lvc.pszText = TranslateT("Current Version");	
				lvc.cx = 95;     // width of column in pixels
				ListView_InsertColumn(hwndList, 1, &lvc);

				lvc.iSubItem = 2;
				lvc.pszText = TranslateT("New Version");	
				lvc.cx = 82;     // width of column in pixels
				ListView_InsertColumn(hwndList, 2, &lvc);

				//enumerate plugins, fill in list
				//bool one_enabled = false;
				ListView_DeleteAllItems(hwndList);

				LVITEM lvI = {0};

				// Some code to create the list-view control.
				// Initialize LVITEM members that are common to all
				// items. 
				lvI.mask = LVIF_TEXT | LVIF_PARAM | LVIF_NORECOMPUTE;// | LVIF_IMAGE; 

				vector<FILEINFO> &todo = *(vector<FILEINFO> *)lParam;
				for (int i = 0; i < (int)todo.size(); ++i) 
				{
					lvI.mask = LVIF_TEXT | LVIF_PARAM;// | LVIF_IMAGE; 
					lvI.iSubItem = 0;
					lvI.lParam = (LPARAM)&todo[i];
					lvI.pszText = todo[i].tszDescr;
					lvI.iItem = i;
					ListView_InsertItem(hwndList, &lvI);

					lvI.mask = LVIF_TEXT;// | LVIF_IMAGE; 

					lvI.iSubItem = 1;
					lvI.pszText = todo[i].tszCurVer;
					ListView_SetItem(hwndList, &lvI);

					lvI.iSubItem = 2;
					lvI.pszText = todo[i].tszNewVer;
					ListView_SetItem(hwndList, &lvI);

					// remember whether the user has decided not to update this component with this particular new version
					/*char stored_setting[256];
					char* descr = mir_t2a(todo[i].tszDescr);
					mir_snprintf(stored_setting, 256, "DisabledVer%s", descr);
					mir_free(descr);
					DBVARIANT dbv;
					bool check = todo[i].enabled;
					if(!DBGetContactSettingTString(0, MODNAME, stored_setting, &dbv))
					{
						if(dbv.ptszVal && lstrcmp(dbv.ptszVal, todo[i].tszNewVer) == 0)
							check = false;
						else
							DBDeleteContactSetting(0, MODNAME, stored_setting);
						DBFreeVariant(&dbv);
					}
					one_enabled |= check;*/
					ListView_SetCheckState(hwndList, lvI.iItem, true);
					todo[i].enabled = true;
				}
				HWND hwOk = GetDlgItem(hDlg, IDOK);
				EnableWindow(hwOk, true/*one_enabled ? TRUE : FALSE*/);
				// do this after filling list - enables 'ITEMCHANGED' below
				SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
				Utils_RestoreWindowPositionNoSize(hDlg,0,MODNAME,"ConfirmWindow");
				//if (!IsUserAnAdmin())
					//SendDlgItemMessage(hDlg, IDOK, BCM_SETSHIELD, 0, TRUE);
				return TRUE;
			}
		case WM_NOTIFY:
			{
				if(((LPNMHDR) lParam)->hwndFrom == hwndList)
				{
					switch (((LPNMHDR) lParam)->code)
					{
						case LVN_ITEMCHANGED:
							{
								if(GetWindowLongPtr(hDlg, GWLP_USERDATA))
								{
									NMLISTVIEW *nmlv = (NMLISTVIEW *)lParam;

									LVITEM lvI = {0};
					
									lvI.iItem = nmlv->iItem;
									lvI.iSubItem = 0;
									lvI.mask = LVIF_PARAM;
									ListView_GetItem(hwndList, &lvI);

									vector<FILEINFO> &todo = *(vector<FILEINFO> *)GetWindowLongPtr(hDlg, GWLP_USERDATA);
									if((nmlv->uNewState ^ nmlv->uOldState) & LVIS_STATEIMAGEMASK)
									{
										todo[lvI.iItem].enabled = ListView_GetCheckState(hwndList, nmlv->iItem);

										/*char stored_setting[256];
										mir_snprintf(stored_setting, 256, "DisabledVer%s", ((UpdateInternal *)lvI.lParam)->update.szComponentName);

										if(((UpdateInternal *)lvI.lParam)->update_options.enabled)
											DBDeleteContactSetting(0, "Updater", stored_setting); // user has re-enabled update to this version - remove setting from db
										else
											DBWriteContactSettingString(0, "Updater", stored_setting, ((UpdateInternal *)lvI.lParam)->newVersion);
										//ListView_SetItem(hwndList, &lvI);
										*/

										bool enableOk = false;
										for(int i=0; i<(int)todo.size(); ++i)
										{
											if(todo[i].enabled)
											{
												enableOk = true;
												break;
											}
										}
										HWND hwOk = GetDlgItem(hDlg, IDOK);
										EnableWindow(hwOk, enableOk ? TRUE : FALSE);
									}
									if(nmlv->uNewState & LVIS_SELECTED)
									{
										if (lstrcmp(todo[lvI.iItem].tszInfoURL, _T("")))
											EnableWindow(GetDlgItem(hDlg, IDC_INFO), TRUE);
										else
											EnableWindow(GetDlgItem(hDlg, IDC_INFO), FALSE);
										SetDlgItemText(hDlg, IDC_MESSAGE, TranslateTS(todo[lvI.iItem].tszMessage));
										/*char stored_setting[256];
										char* descr = mir_t2a(todo[lvI.iItem].tszDescr);
										mir_snprintf(stored_setting, 256, "DisabledVer%s", descr);
										mir_free(descr);
										DBVARIANT dbv;
										if(!DBGetContactSettingTString(0, MODNAME, stored_setting, &dbv))
										{
											if(dbv.ptszVal && lstrcmp(dbv.ptszVal, todo[lvI.iItem].tszNewVer) == 0)
												CheckDlgButton(hDlg, IDC_DONOTREMIND, BST_CHECKED);
											else
												DBDeleteContactSetting(0, MODNAME, stored_setting);
											DBFreeVariant(&dbv);
										}
										else
											CheckDlgButton(hDlg, IDC_DONOTREMIND, BST_UNCHECKED);
										*/

									}
								}
								break;
							}
					}
				}
				break;
			}
		case WM_COMMAND:
			{
				if (HIWORD( wParam ) == BN_CLICKED)
				{
					switch(LOWORD(wParam))
					{
						case IDOK:
							{
								vector<FILEINFO> &todo = *(vector<FILEINFO> *)GetWindowLongPtr(hDlg, GWLP_USERDATA);
								ShowWindow(hDlg, SW_HIDE);
								TCHAR tszBuff[2048] = {0}, tszFileDest[MAX_PATH] = {0}, tszFilePathDest[MAX_PATH] = {0}, tszFilePathBack[MAX_PATH] = {0}, tszFileName[MAX_PATH] = {0};
								TCHAR* tszExt = NULL;
								char szKey[64] = {0};
								vector<int> arFileType;
								vector<tString> arFilePath;
								vector<tString> arFileName;
								vector<tString> arAdvFolder;
								vector<tString> arExt;
								STARTUPINFO si;
								PROCESS_INFORMATION pi;

								SetWindowLongPtr(hDlg, GWLP_USERDATA, 0);
								Utils_SaveWindowPosition(hDlg, NULL, MODNAME, "ConfirmWindow");
								/*DBWriteContactSettingByte(NULL, "Updater", "DefaultConfAll", IsDlgButtonChecked(hwndDlg, IDC_CHK_CONFALL) ? 1 : 0);
								DBWriteContactSettingByte(NULL, "Updater", "NoInstall", IsDlgButtonChecked(hwndDlg, IDC_CHK_NOINSTALL) ? 1 : 0);

								if(IsDlgButtonChecked(hDlg, IDC_CHK_CONFALL))
									EndDialog(hDlg, CD_CONFALL);
								else if(IsDlgButtonChecked(hDlg, IDC_CHK_NOINSTALL))
									EndDialog(hDlg, CD_NOINSTALL);
								else
								*/
								
								arFileType.clear();
								arFilePath.clear();
								arFileName.clear();
								arAdvFolder.clear();
								arExt.clear();
								for(int i=0; i<(int)todo.size(); ++i)
								{
									if(todo[i].enabled)
									{
										switch (todo[i].FileType)
											{
												case 1:
													mir_sntprintf(tszFileDest, SIZEOF(tszFileDest), _T("%s\\Pack"), tszRoot);
													CreateDirectory(tszFileDest, NULL);
													break;
												case 2:
													mir_sntprintf(tszFileDest, SIZEOF(tszFileDest), _T("%s\\Plugins"), tszRoot);
													CreateDirectory(tszFileDest, NULL);
													break;
												case 3:
													mir_sntprintf(tszFileDest, SIZEOF(tszFileDest), _T("%s\\Icons"), tszRoot);
													CreateDirectory(tszFileDest, NULL);
													break;
												case 4:
													mir_sntprintf(tszFileDest, SIZEOF(tszFileDest), _T("%s\\Others"), tszRoot);
													CreateDirectory(tszFileDest, NULL);
													break;
												case 5:
													mir_sntprintf(tszFileDest, SIZEOF(tszFileDest), _T("%s\\Others"), tszRoot);
													CreateDirectory(tszFileDest, NULL);
													break;
												default:
													lstrcpyn(tszFileDest, tszRoot, SIZEOF(tszFileDest));
													break;
											}//end * switch (todo[i].FileType)
										mir_sntprintf(tszBuff, SIZEOF(tszBuff), _T("%s\\Backups"), tszRoot);
										CreateDirectory(tszBuff, NULL);
										lstrcpyn(tszFileName, todo[i].File.tszDiskPath, SIZEOF(tszFileName));
										mir_sntprintf(todo[i].File.tszDiskPath, SIZEOF(todo[i].File.tszDiskPath), _T("%s\\%s"), tszFileDest, tszFileName);
										UpdatesCount++;

										tszExt = &todo[i].File.tszDownloadURL[lstrlen(todo[i].File.tszDownloadURL)-5];
										if (lstrcmp(tszExt, _T(".html")) == 0)
										{
											char* szUrl = mir_t2a(todo[i].File.tszDownloadURL);
											CallService(MS_UTILS_OPENURL, TRUE, (LPARAM)szUrl);
											mir_free(szUrl);
										}
										else
										{
											// download update
											pFileUrl = &todo[i].File;
											Title = TranslateT("Pack Updater");
											if (todo[i].FileType == 1)
												Text = TranslateT("Downloading pack updates...");
											else
												Text = TranslateT("Downloading update...");
											DlgDownloadProc();
											if (!DlgDld)
											{
												if (UpdatesCount)
													UpdatesCount--;
												continue;
											}
										}
										lstrcpyn(todo[i].tszCurVer, todo[i].tszNewVer, SIZEOF(todo[i].tszCurVer));
										mir_snprintf(szKey, SIZEOF(szKey), "File_%d_CurrentVersion", todo[i].FileNum);
										DBWriteContactSettingTString(NULL, MODNAME, szKey, todo[i].tszCurVer);
										arFileType.push_back(todo[i].FileType);
										arFilePath.push_back(todo[i].File.tszDiskPath);
										arFileName.push_back(tszFileName);
										arAdvFolder.push_back(todo[i].tszAdvFolder);
										arExt.push_back(tszExt);
										if (todo[i].FileType == 1)
											i = (int)todo.size();
									}
								}
										
								if (UpdatesCount > 1 && lstrcmp(arExt[0].c_str(), _T(".html")) != 0)
									lstrcpyn(tszBuff, TranslateT("Downloads complete. Start updating? All your data will be saved and Miranda IM will be closed."), SIZEOF(tszBuff));
								else if (UpdatesCount == 1 && lstrcmp(arExt[0].c_str(), _T(".html")) != 0)
									lstrcpyn(tszBuff, TranslateT("Download complete. Start updating? All your data will be saved and Miranda IM will be closed."), SIZEOF(tszBuff));
								if (UpdatesCount > 0 && lstrcmp(arExt[0].c_str(), _T(".html")) != 0)
								{
									INT rc = -1;
									Title = TranslateT("Pack Updater");
									Text = tszBuff;
									if (ServiceExists(MS_POPUP_ADDPOPUPEX)  && ServiceExists(MS_POPUP_REGISTERACTIONS) && DBGetContactSettingByte(NULL, "PopUp", "ModuleIsEnabled", 1) && DBGetContactSettingByte(NULL,MODNAME, "Popups0", DEFAULT_POPUP_ENABLED) && (DBGetContactSettingDword(NULL, "PopUp", "Actions", 0) & 1))
										rc = DialogBox(hInst, MAKEINTRESOURCE(IDD_POPUPDUMMI), NULL, DlgMsgPop);
									else
										rc = MessageBox(NULL, tszBuff, Title, MB_YESNO | MB_ICONQUESTION);
									if (rc == IDYES)
									{
										for (int i = 0; i < UpdatesCount; i++)
										{
											TCHAR* tszUtilRootPlug = NULL; 
											TCHAR* tszUtilRootIco = NULL;
											TCHAR* tszUtilRoot = NULL;
						
											switch (arFileType[i])
											{
												case 0:
													break;
												case 1:
													if (Reminder == 2)
														DBWriteContactSettingByte(NULL, MODNAME, "Reminder", 1);
													memset(&si, 0, sizeof(STARTUPINFO));
													memset(&pi, 0, sizeof(PROCESS_INFORMATION));
													si.cb = sizeof(STARTUPINFO);
													CreateProcess(arFilePath[i].c_str(), _T(""), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
													i = UpdatesCount;
													CallFunctionAsync(ExitMe, 0);
													break;
												case 2:
													tszUtilRootPlug = Utils_ReplaceVarsT(_T("%miranda_path%\\Plugins"));
													if (lstrcmp(arAdvFolder[i].c_str(), _T("")) == 0)
														mir_sntprintf(tszFilePathDest, SIZEOF(tszFilePathDest), _T("%s\\%s"), tszUtilRootPlug, arFileName[i].c_str());
													else
														mir_sntprintf(tszFilePathDest, SIZEOF(tszFilePathDest), _T("%s\\%s\\%s"), tszUtilRootPlug, arAdvFolder[i].c_str(), arFileName[i].c_str());
													mir_sntprintf(tszFilePathBack, SIZEOF(tszFilePathBack), _T("%s\\Backups\\%s"), tszRoot, arFileName[i].c_str());
													MoveFile(tszFilePathDest, tszFilePathBack);
													MoveFile(arFilePath[i].c_str(), tszFilePathDest);
													mir_free(tszUtilRootPlug);
													if (i == UpdatesCount - 1)
														CallFunctionAsync(RestartMe, 0);
													break;
												case 3:
													tszUtilRootIco = Utils_ReplaceVarsT(_T("%miranda_path%\\Icons"));
													if (lstrcmp(arAdvFolder[i].c_str(), _T("")) == 0)
														mir_sntprintf(tszFilePathDest, SIZEOF(tszFilePathDest), _T("%s\\%s"), tszUtilRootIco, arFileName[i].c_str());
													else
														mir_sntprintf(tszFilePathDest, SIZEOF(tszFilePathDest), _T("%s\\%s\\%s"), tszUtilRootIco, arAdvFolder[i].c_str(), arFileName[i].c_str());
													mir_sntprintf(tszFilePathBack, SIZEOF(tszFilePathBack), _T("%s\\Backups\\%s"), tszRoot, arFileName[i].c_str());
													MoveFile(tszFilePathDest, tszFilePathBack);
													MoveFile(arFilePath[i].c_str(), tszFilePathDest);
													mir_free(tszUtilRootIco);
													if (i == UpdatesCount - 1)
														CallFunctionAsync(RestartMe, 0);
													break;
												case 4:
													tszUtilRoot = Utils_ReplaceVarsT(_T("%miranda_path%"));
													if (lstrcmp(arAdvFolder[i].c_str(), _T("")) == 0)
														mir_sntprintf(tszFilePathDest, SIZEOF(tszFilePathDest), _T("%s\\%s"), tszUtilRoot, arFileName[i].c_str());
													else
														mir_sntprintf(tszFilePathDest, SIZEOF(tszFilePathDest), _T("%s\\%s\\%s"), tszUtilRoot, arAdvFolder[i].c_str(), arFileName[i].c_str());
													mir_sntprintf(tszFilePathBack, SIZEOF(tszFilePathBack), _T("%s\\Backups\\%s"), tszRoot, arFileName[i].c_str());
													MoveFile(tszFilePathDest, tszFilePathBack);
													MoveFile(arFilePath[i].c_str(), tszFilePathDest);
													mir_free(tszUtilRoot);
													if (i == UpdatesCount - 1)
														CallFunctionAsync(RestartMe, 0);
													break;
												case 5:
													tszUtilRoot = Utils_ReplaceVarsT(_T("%miranda_path%"));
													if (lstrcmp(arAdvFolder[i].c_str(), _T("")) == 0)
														mir_sntprintf(tszFilePathDest, SIZEOF(tszFilePathDest), _T("%s\\%s"), tszUtilRoot, arFileName[i].c_str());
													else
														mir_sntprintf(tszFilePathDest, SIZEOF(tszFilePathDest), _T("%s\\%s\\%s"), tszUtilRoot, arAdvFolder[i].c_str(), arFileName[i].c_str());
													mir_sntprintf(tszFilePathBack, SIZEOF(tszFilePathBack), _T("%s\\Backups\\%s"), tszRoot, arFileName[i].c_str());
													MoveFile(tszFilePathDest, tszFilePathBack);
													MoveFile(arFilePath[i].c_str(), tszFilePathDest);
													mir_free(tszUtilRoot);
													break;
											}//end* switch (arFileType[i])
										}//end* for (int i = 0; i < UpdatesCount; i++)
									}//end* if (IDYES == MessageBox(NULL, tszBuff, Title, MB_YESNO | MB_ICONQUESTION))
									else
									{//reminder for not installed pack update
										if (Reminder && (UpdatesCount == 1) && (arFileType[0] == 1))
											DBWriteContactSettingByte(NULL, MODNAME, "Reminder", 2);
										mir_sntprintf(tszBuff, SIZEOF(tszBuff), TranslateT("You have chosen not to install the pack update immediately.\nYou can install it manually from this location:\n\n%s"), arFilePath[0].c_str());
										Title = TranslateT("Pack Updater");
										Text = tszBuff;
										if (ServiceExists(MS_POPUP_ADDPOPUPEX) && DBGetContactSettingByte(NULL, "PopUp", "ModuleIsEnabled", 1) &&  DBGetContactSettingByte(NULL, MODNAME, "Popups2", DEFAULT_POPUP_ENABLED))
										{
											Number = 2;
											show_popup(0, Title, Text, Number, 0);
										}
										else if (DBGetContactSettingByte(NULL, MODNAME, "Popups2M", DEFAULT_MESSAGE_ENABLED))
											MessageBox(NULL, Text, Title, MB_ICONINFORMATION);
									}
								}//end* if (UpdatesCount > 0)

								EndDialog(hDlg, IDOK);
								return TRUE;
							}
						case IDCANCEL:
							{
								SetWindowLongPtr(hDlg, GWLP_USERDATA, 0);
								Utils_SaveWindowPosition(hDlg, NULL, MODNAME, "ConfirmWindow");
								EndDialog(hDlg, IDCANCEL);
								return TRUE;
							}
						case IDC_INFO:
							{
								int sel = ListView_GetSelectionMark(hwndList);
								vector<FILEINFO> &todo = *(vector<FILEINFO> *)GetWindowLongPtr(hDlg, GWLP_USERDATA);
								char* szUrl = mir_t2a(todo[sel].tszInfoURL);
								CallService(MS_UTILS_OPENURL, TRUE, (LPARAM)szUrl);
								mir_free(szUrl);
								break;
							}
					}
				}
				break;
			}
	}
	return FALSE;
}

INT_PTR CALLBACK DlgMsgPop(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			Number = 0;
			show_popup(hDlg, Title, Text, Number, 1);
			return TRUE;
		} // end* WM_INITDIALOG:
	} // end* switch (uMsg)
	ShowWindow(hDlg, SW_HIDE);
	return FALSE;
}