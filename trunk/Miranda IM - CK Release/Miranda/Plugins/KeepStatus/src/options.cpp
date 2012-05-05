/*
AdvancedAutoAway plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2003-2006 P. Boon

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

#include "commonstatus.h"
#include "keepstatus.h"
#include "../resource.h"

static BOOL (WINAPI *pfnEnableThemeDialogTexture)(HANDLE, DWORD) = 0;

// Thanks to TioDuke for cleaning up the ListView handling
#ifndef ListView_GetCheckState
#define ListView_GetCheckState(w,i) ((((UINT)(SNDMSG((w),LVM_GETITEMSTATE,(WPARAM)(i),LVIS_STATEIMAGEMASK)))>>12)-1)
#endif
#ifndef ListView_SetCheckState
#define ListView_SetCheckState(w,i,f) ListView_SetItemState(w,i,INDEXTOSTATEIMAGEMASK((f)+1),LVIS_STATEIMAGEMASK)
#endif

// prototypes
INT_PTR CALLBACK OptDlgProc(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam);
INT_PTR CALLBACK PopupOptDlgProc(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam);

static INT_PTR CALLBACK DlgProcKSBasicOpts(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg) {
	case WM_INITDIALOG:
		TranslateDialogDefault(hwndDlg);
		{
			LVCOLUMN lvCol;
			LVITEM lvItem;
			HWND hList;
			int i;
			DBVARIANT dbv;
			
			SetDlgItemInt(hwndDlg, IDC_MAXRETRIES, DBGetContactSettingByte(NULL,MODULENAME,SETTING_MAXRETRIES,DEFAULT_MAXRETRIES),FALSE);
			SetDlgItemInt(hwndDlg, IDC_INITDELAY, DBGetContactSettingDword(NULL,MODULENAME,SETTING_INITDELAY,DEFAULT_INITDELAY),FALSE);
			CheckDlgButton(hwndDlg, IDC_CHECKCONNECTION, DBGetContactSettingByte(NULL, MODULENAME, SETTING_CHECKCONNECTION,1)?BST_CHECKED:BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_SHOWCONNECTIONPOPUPS, (DBGetContactSettingByte(NULL, MODULENAME, SETTING_SHOWCONNECTIONPOPUPS,FALSE)&&ServiceExists(MS_POPUP_SHOWMESSAGE))?BST_CHECKED:BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_CHKINET, DBGetContactSettingByte(NULL, MODULENAME, SETTING_CHKINET, FALSE)?BST_CHECKED:BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_CONTCHECK, DBGetContactSettingByte(NULL, MODULENAME, SETTING_CONTCHECK, FALSE)?BST_CHECKED:BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_BYPING, DBGetContactSettingByte(NULL, MODULENAME, SETTING_BYPING, FALSE)?BST_CHECKED:BST_UNCHECKED);
			if (!DBGetContactSetting(NULL, MODULENAME, SETTING_PINGHOST, &dbv))
				SetDlgItemTextA(hwndDlg, IDC_PINGHOST, dbv.pszVal);
			// proto list
			hList = GetDlgItem(hwndDlg,IDC_PROTOCOLLIST);
			ListView_SetExtendedListViewStyleEx(hList, LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES, LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
			memset(&lvCol,0,sizeof(lvCol));
			lvCol.mask = LVCF_WIDTH|LVCF_TEXT;
			lvCol.pszText=TranslateT("Protocol");
			lvCol.cx = 118;
			ListView_InsertColumn(hList, 0, &lvCol);
			// fill the list
			memset(&lvItem,0,sizeof(lvItem));
			lvItem.mask=LVIF_TEXT|LVIF_PARAM;
			lvItem.iItem=0;
			lvItem.iSubItem=0;

			int count;
			PROTOACCOUNT** protos;
			ProtoEnumAccounts( &count, &protos );

			for(i=0;i<count;i++) {
				if ( !IsSuitableProto( protos[i] ))
					continue;

				lvItem.pszText = protos[i]->tszAccountName;
				lvItem.lParam = (LPARAM)protos[i]->szModuleName;
				ListView_InsertItem(hList,&lvItem);

				char dbSetting[128];
				_snprintf(dbSetting, sizeof(dbSetting), "%s_enabled", protos[i]->szModuleName);
				ListView_SetCheckState(hList, lvItem.iItem, DBGetContactSettingByte(NULL, MODULENAME, dbSetting, TRUE));
				lvItem.iItem++;
			}
			EnableWindow(GetDlgItem(hwndDlg, IDC_MAXRETRIES), IsDlgButtonChecked(hwndDlg, IDC_CHECKCONNECTION));
			EnableWindow(GetDlgItem(hwndDlg, IDC_SHOWCONNECTIONPOPUPS), ServiceExists(MS_POPUP_SHOWMESSAGE)&&IsDlgButtonChecked(hwndDlg, IDC_CHECKCONNECTION));
			EnableWindow(GetDlgItem(hwndDlg, IDC_INITDELAY), IsDlgButtonChecked(hwndDlg, IDC_CHECKCONNECTION));
			EnableWindow(GetDlgItem(hwndDlg, IDC_PROTOCOLLIST), IsDlgButtonChecked(hwndDlg, IDC_CHECKCONNECTION));
			EnableWindow(GetDlgItem(hwndDlg, IDC_CHKINET), IsDlgButtonChecked(hwndDlg, IDC_CHECKCONNECTION));
			EnableWindow(GetDlgItem(hwndDlg, IDC_CONTCHECK), IsDlgButtonChecked(hwndDlg, IDC_CHECKCONNECTION));
			EnableWindow(GetDlgItem(hwndDlg, IDC_BYPING), (IsDlgButtonChecked(hwndDlg, IDC_CONTCHECK)&&IsDlgButtonChecked(hwndDlg, IDC_CHECKCONNECTION))?TRUE:FALSE);
			EnableWindow(GetDlgItem(hwndDlg, IDC_PINGHOST), (IsDlgButtonChecked(hwndDlg, IDC_CONTCHECK))&&(IsDlgButtonChecked(hwndDlg, IDC_BYPING)&&IsDlgButtonChecked(hwndDlg, IDC_CHECKCONNECTION))?TRUE:FALSE);
		}
		break;

	case WM_COMMAND:
		if ( ((HIWORD(wParam) == EN_CHANGE) || (HIWORD(wParam) == BN_CLICKED)) && ((HWND)lParam == GetFocus()) )
			SendMessage(GetParent(hwndDlg),PSM_CHANGED,0,0);
		// something changed
		switch (LOWORD(wParam)) {
		case IDC_CHECKCONNECTION:
		case IDC_CONTCHECK:
		case IDC_BYPING:
			EnableWindow(GetDlgItem(hwndDlg, IDC_MAXRETRIES), SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
			EnableWindow(GetDlgItem(hwndDlg, IDC_SHOWCONNECTIONPOPUPS), ServiceExists(MS_POPUP_SHOWMESSAGE)&&SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
			EnableWindow(GetDlgItem(hwndDlg, IDC_INITDELAY), SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
			EnableWindow(GetDlgItem(hwndDlg, IDC_PROTOCOLLIST), SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
			EnableWindow(GetDlgItem(hwndDlg, IDC_CHKINET), SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
			EnableWindow(GetDlgItem(hwndDlg, IDC_CONTCHECK), SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
			EnableWindow(GetDlgItem(hwndDlg, IDC_BYPING), (IsDlgButtonChecked(hwndDlg, IDC_CONTCHECK)&&SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0))?TRUE:FALSE);
			EnableWindow(GetDlgItem(hwndDlg, IDC_PINGHOST), (IsDlgButtonChecked(hwndDlg, IDC_CONTCHECK))&&(IsDlgButtonChecked(hwndDlg, IDC_BYPING)&&SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0))?TRUE:FALSE);
			break;
		}
		break;
		
	case WM_NOTIFY:
		if (((NMHDR*)lParam)->idFrom == IDC_PROTOCOLLIST) {
			switch(((NMHDR*)lParam)->code) {
			case LVN_ITEMCHANGED:
				{
					NMLISTVIEW *nmlv = (NMLISTVIEW *)lParam;
					if (IsWindowVisible(GetDlgItem(hwndDlg,IDC_PROTOCOLLIST)) && ((nmlv->uNewState^nmlv->uOldState)&LVIS_STATEIMAGEMASK))
						SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
				}
				break;
		}	}

		if (((LPNMHDR)lParam)->code == PSN_APPLY) {
			int i;
			LVITEM lvItem;
			HWND hList;

			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_MAXRETRIES, (BYTE)GetDlgItemInt(hwndDlg,IDC_MAXRETRIES, NULL, FALSE));
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_CHECKCONNECTION, (BYTE)SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_SHOWCONNECTIONPOPUPS, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_SHOWCONNECTIONPOPUPS));
			DBWriteContactSettingDword(NULL, MODULENAME, SETTING_INITDELAY, (DWORD)GetDlgItemInt(hwndDlg,IDC_INITDELAY, NULL, FALSE));
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_CHKINET, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_CHKINET));
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_CONTCHECK, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_CONTCHECK));
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_BYPING, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_BYPING));
			if (IsDlgButtonChecked(hwndDlg, IDC_BYPING)) {
				int len;
				char *host;

				len = SendDlgItemMessage(hwndDlg, IDC_PINGHOST, WM_GETTEXTLENGTH, 0, 0);
				if (len > 0) {
					host = ( char* )malloc(len+1);
					if (host != NULL) {
						memset(host, '\0', len+1);
						GetDlgItemTextA(hwndDlg, IDC_PINGHOST, host, len+1);
						DBWriteContactSettingString(NULL, MODULENAME, SETTING_PINGHOST, host);
					}
				}
			}
			hList = GetDlgItem(hwndDlg,IDC_PROTOCOLLIST);
			memset(&lvItem,0,sizeof(lvItem));
			lvItem.mask=LVIF_PARAM;
			for (i=0;i<ListView_GetItemCount(hList);i++) {
				lvItem.iItem=i;
				lvItem.iSubItem=0;
				ListView_GetItem(hList, &lvItem);

				char dbSetting[128];
				_snprintf(dbSetting, sizeof(dbSetting), "%s_enabled", (char *)lvItem.lParam);
				DBWriteContactSettingByte(NULL, MODULENAME, dbSetting, (BYTE)ListView_GetCheckState(hList, lvItem.iItem));
			}
		}
		break;
	}
	
	return 0;
}

static INT_PTR CALLBACK DlgProcKSAdvOpts(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg) {
	case WM_INITDIALOG: {
		TranslateDialogDefault(hwndDlg);
		SetDlgItemInt(hwndDlg, IDC_MAXDELAY, DBGetContactSettingDword(NULL,MODULENAME,SETTING_MAXDELAY,DEFAULT_MAXDELAY),FALSE);
		SetDlgItemInt(hwndDlg, IDC_MAXCONNECTINGTIME, DBGetContactSettingDword(NULL,MODULENAME,SETTING_MAXCONNECTINGTIME,0),FALSE);
		CheckDlgButton(hwndDlg, IDC_INCREASEEXPONENTIAL, DBGetContactSettingByte(NULL, MODULENAME, SETTING_INCREASEEXPONENTIAL,FALSE)?BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_CNCOTHERLOC, (DBGetContactSettingByte(NULL, MODULENAME, SETTING_CNCOTHERLOC, FALSE)&&(CallService(MS_SYSTEM_GETVERSION,0,0) >= 0x00040000))?BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_LOGINERR, DBGetContactSettingByte(NULL, MODULENAME, SETTING_LOGINERR, LOGINERR_NOTHING)==LOGINERR_NOTHING?FALSE:TRUE);
		CheckDlgButton(hwndDlg, IDC_CHECKAPMRESUME, (DBGetContactSettingByte(NULL, MODULENAME, SETTING_CHECKAPMRESUME, 1)&&(CallService(MS_SYSTEM_GETVERSION,0,0) >= 0x00040000))?BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_FIRSTOFFLINE, (DBGetContactSettingByte(NULL, MODULENAME, SETTING_FIRSTOFFLINE, FALSE)));
		CheckDlgButton(hwndDlg, IDC_NOLOCKED, (DBGetContactSettingByte(NULL, MODULENAME, SETTING_NOLOCKED, FALSE)));
		SetDlgItemInt(hwndDlg, IDC_LOGINERR_DELAY, DBGetContactSettingDword(NULL, MODULENAME, SETTING_LOGINERR_DELAY, DEFAULT_MAXDELAY), FALSE);
		SetDlgItemInt(hwndDlg, IDC_PINGCOUNT, DBGetContactSettingWord(NULL, MODULENAME, SETTING_PINGCOUNT, DEFAULT_PINGCOUNT), FALSE);
		SetDlgItemInt(hwndDlg, IDC_CNTDELAY, DBGetContactSettingDword(NULL, MODULENAME, SETTING_CNTDELAY, CHECKCONTIN_DELAY), FALSE);
		switch (DBGetContactSettingByte(NULL, MODULENAME, SETTING_LOGINERR, LOGINERR_CANCEL)) {
			case LOGINERR_SETDELAY:
				CheckRadioButton(hwndDlg, IDC_LOGINERR_CANCEL, IDC_LOGINERR_SETDELAY, IDC_LOGINERR_SETDELAY);
				break;
			default:
			case LOGINERR_CANCEL:
				CheckRadioButton(hwndDlg, IDC_LOGINERR_CANCEL, IDC_LOGINERR_SETDELAY, IDC_LOGINERR_CANCEL);
				break;
		}
		SendMessage(hwndDlg, KS_ENABLEITEMS, 0, 0);
		break;
						}
	case WM_COMMAND:
		if ( ((HIWORD(wParam) == EN_CHANGE) || (HIWORD(wParam) == BN_CLICKED)) && ((HWND)lParam == GetFocus()) )
			SendMessage(GetParent(hwndDlg),PSM_CHANGED,0,0);
		// something changed
		switch (LOWORD(wParam)) {
		case IDC_INCREASEEXPONENTIAL:
		case IDC_LOGINERR:
			SendMessage(hwndDlg, KS_ENABLEITEMS, 0, 0);
			break;
			
		case IDC_LOGINERR_CANCEL:
		case IDC_LOGINERR_SETDELAY:
			CheckRadioButton(hwndDlg, IDC_LOGINERR_CANCEL, IDC_LOGINERR_SETDELAY, LOWORD(wParam));
			EnableWindow(GetDlgItem(hwndDlg, IDC_LOGINERR_DELAY), IsDlgButtonChecked(hwndDlg, IDC_LOGINERR_SETDELAY)&&IsDlgButtonChecked(hwndDlg, IDC_LOGINERR)&&SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
			break;
		}
		break;
		
	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->code == PSN_APPLY) {
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_INCREASEEXPONENTIAL, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_INCREASEEXPONENTIAL));
			DBWriteContactSettingDword(NULL, MODULENAME, SETTING_MAXDELAY, (DWORD)GetDlgItemInt(hwndDlg,IDC_MAXDELAY, NULL, FALSE));
			DBWriteContactSettingDword(NULL, MODULENAME, SETTING_MAXCONNECTINGTIME, (DWORD)GetDlgItemInt(hwndDlg,IDC_MAXCONNECTINGTIME, NULL, FALSE));
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_FIRSTOFFLINE, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_FIRSTOFFLINE));
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_NOLOCKED, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_NOLOCKED));
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_CNCOTHERLOC, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_CNCOTHERLOC));
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_LOGINERR, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_LOGINERR));
			if (IsDlgButtonChecked(hwndDlg, IDC_LOGINERR)) {
				if (IsDlgButtonChecked(hwndDlg, IDC_LOGINERR_SETDELAY)) {
					DBWriteContactSettingByte(NULL, MODULENAME, SETTING_LOGINERR, LOGINERR_SETDELAY);
					DBWriteContactSettingDword(NULL, MODULENAME, SETTING_LOGINERR_DELAY, GetDlgItemInt(hwndDlg, IDC_LOGINERR_DELAY, NULL, FALSE));
				}
				else DBWriteContactSettingByte(NULL, MODULENAME, SETTING_LOGINERR, LOGINERR_CANCEL);
			}
			else DBWriteContactSettingByte(NULL, MODULENAME, SETTING_LOGINERR, LOGINERR_NOTHING);

			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_CHECKAPMRESUME, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_CHECKAPMRESUME));
			DBWriteContactSettingWord(NULL, MODULENAME, SETTING_PINGCOUNT, (WORD)GetDlgItemInt(hwndDlg,IDC_PINGCOUNT, NULL, FALSE));
			DBWriteContactSettingDword(NULL, MODULENAME, SETTING_CNTDELAY, (DWORD)GetDlgItemInt(hwndDlg,IDC_CNTDELAY, NULL, FALSE)==0?CHECKCONTIN_DELAY:GetDlgItemInt(hwndDlg,IDC_CNTDELAY, NULL, FALSE));
		}
		break;

	case KS_ENABLEITEMS:
		EnableWindow(GetDlgItem(hwndDlg, IDC_INCREASEEXPONENTIAL), SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
		EnableWindow(GetDlgItem(hwndDlg, IDC_MAXCONNECTINGTIME), SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
		EnableWindow(GetDlgItem(hwndDlg, IDC_CNCOTHERLOC), SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0)&&(CallService(MS_SYSTEM_GETVERSION,0,0) >= 0x00040000));
		EnableWindow(GetDlgItem(hwndDlg, IDC_LOGINERR), SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
		EnableWindow(GetDlgItem(hwndDlg, IDC_LOGINERR_SETDELAY), IsDlgButtonChecked(hwndDlg, IDC_LOGINERR)&&SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
		EnableWindow(GetDlgItem(hwndDlg, IDC_LOGINERR_DELAY), IsDlgButtonChecked(hwndDlg, IDC_LOGINERR_SETDELAY)&&IsDlgButtonChecked(hwndDlg, IDC_LOGINERR)&&SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
		EnableWindow(GetDlgItem(hwndDlg, IDC_LOGINERR_CANCEL), IsDlgButtonChecked(hwndDlg, IDC_LOGINERR)&&SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
		EnableWindow(GetDlgItem(hwndDlg, IDC_MAXDELAY), (IsDlgButtonChecked(hwndDlg, IDC_INCREASEEXPONENTIAL)&&SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0))?TRUE:FALSE);
		EnableWindow(GetDlgItem(hwndDlg, IDC_CHECKAPMRESUME), SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0)&&(CallService(MS_SYSTEM_GETVERSION,0,0) >= 0x00040000));
		EnableWindow(GetDlgItem(hwndDlg, IDC_FIRSTOFFLINE), SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
		EnableWindow(GetDlgItem(hwndDlg, IDC_NOLOCKED), SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CHECKCONNECTION, 0));
		EnableWindow(GetDlgItem(hwndDlg, IDC_PINGCOUNT), SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_BYPING, 0));
		EnableWindow(GetDlgItem(hwndDlg, IDC_CNTDELAY), SendMessage(GetParent(hwndDlg), KS_ISENABLED, (WPARAM)IDC_CONTCHECK, 0));
		break;

	case WM_SHOWWINDOW:
		if (wParam)
			SendMessage(hwndDlg, KS_ENABLEITEMS, 0, 0);

		break;
	}
	
	return 0;
}

static INT_PTR CALLBACK DlgProcKsTabs(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hBasicTab;

	switch(msg) {
	case WM_INITDIALOG:
		{
			int tabCount;
			HWND hTab, hShow, hPage;
			TCITEM tci;
			RECT rcTabs, rcOptions, rcPage;

			TranslateDialogDefault(hwndDlg);

			// set tabs
			tabCount = 0;
			hTab = GetDlgItem(hwndDlg, IDC_TABS);
			GetWindowRect(hTab, &rcTabs);
			GetWindowRect(hwndDlg, &rcOptions);
			ZeroMemory(&tci, sizeof(TCITEM));

			// basic tab
			tci.mask = TCIF_TEXT|TCIF_PARAM;
			tci.pszText = TranslateT("Basic");
			hShow = hBasicTab = hPage = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_OPT_KS_BASIC), hwndDlg, DlgProcKSBasicOpts, (LPARAM)GetParent(hwndDlg));
			if (pfnEnableThemeDialogTexture)
				pfnEnableThemeDialogTexture(hPage, ETDT_ENABLETAB);

			tci.lParam = (LPARAM)hPage;
			GetClientRect(hPage, &rcPage);
			MoveWindow(hPage, (rcTabs.left - rcOptions.left) + ((rcTabs.right-rcTabs.left)-(rcPage.right-rcPage.left))/2, 10 + (rcTabs.top - rcOptions.top) + ((rcTabs.bottom-rcTabs.top)-(rcPage.bottom-rcPage.top))/2, rcPage.right-rcPage.left, rcPage.bottom-rcPage.top, TRUE);
			ShowWindow(hPage, SW_HIDE);
			TabCtrl_InsertItem(hTab, tabCount++, &tci);

			// advanced tab
			tci.mask = TCIF_TEXT|TCIF_PARAM;
			tci.pszText = TranslateT("Advanced");
			hPage = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_OPT_KS_ADV), hwndDlg, DlgProcKSAdvOpts, (LPARAM)GetParent(hwndDlg));
			if (pfnEnableThemeDialogTexture) {
				pfnEnableThemeDialogTexture(hPage, ETDT_ENABLETAB);
			}
			tci.lParam = (LPARAM)hPage;
			GetClientRect(hPage, &rcPage);
			MoveWindow(hPage, (rcTabs.left - rcOptions.left) + ((rcTabs.right-rcTabs.left)-(rcPage.right-rcPage.left))/2, 10 + (rcTabs.top - rcOptions.top) + ((rcTabs.bottom-rcTabs.top)-(rcPage.bottom-rcPage.top))/2, rcPage.right-rcPage.left, rcPage.bottom-rcPage.top, TRUE);
			ShowWindow(hPage, SW_HIDE);
			TabCtrl_InsertItem(hTab, tabCount++, &tci);
			ShowWindow(hShow, SW_SHOW);			
		}
		break;

	case KS_ISENABLED:
		SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, (LPARAM)IsDlgButtonChecked(hBasicTab, wParam));
		return TRUE;

	case PSM_CHANGED:
		SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
		break;

	case WM_NOTIFY:
		if ( (((NMHDR*)lParam)->idFrom == IDC_TABS) ) {
			if (((NMHDR*)lParam)->code == TCN_SELCHANGING) {
				TCITEM tci;

				tci.mask = TCIF_PARAM;
				TabCtrl_GetItem(GetDlgItem(hwndDlg, IDC_TABS), TabCtrl_GetCurSel(GetDlgItem(hwndDlg, IDC_TABS)), &tci);
				ShowWindow((HWND)tci.lParam, SW_HIDE);
			}
			else if (((NMHDR*)lParam)->code == TCN_SELCHANGE) {
				TCITEM tci;

				tci.mask = TCIF_PARAM;
				TabCtrl_GetItem(GetDlgItem(hwndDlg, IDC_TABS), TabCtrl_GetCurSel(GetDlgItem(hwndDlg, IDC_TABS)), &tci);
				ShowWindow((HWND)tci.lParam, SW_SHOW);
			}
		}
		if (((LPNMHDR)lParam)->code == PSN_APPLY) {
			TCITEM tci;
			 int i, count;
			 
			tci.mask = TCIF_PARAM;
			count = TabCtrl_GetItemCount(GetDlgItem(hwndDlg, IDC_TABS));
			for (i=0;i<count;i++) {
				TabCtrl_GetItem(GetDlgItem(hwndDlg,IDC_TABS), i, &tci);
				SendMessage((HWND)tci.lParam, WM_NOTIFY, 0, lParam);
			}
			// let main reload options
			LoadMainOptions();
		}
		break;
	}

	return FALSE;
}

INT_PTR CALLBACK PopupOptDlgProc(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
	static bool bFreeze = false;

	switch(msg) {
	case WM_INITDIALOG:
		TranslateDialogDefault(hwndDlg);
		
		bFreeze = true;

		// left action
		switch ( DBGetContactSettingByte( NULL, MODULENAME, SETTING_POPUP_LEFTCLICK, POPUP_ACT_NOTHING )) {
		case POPUP_ACT_CLOSEPOPUP:
			CheckDlgButton(hwndDlg, IDC_LCLOSE, BST_CHECKED);
			break;
			
		case POPUP_ACT_CANCEL:
			CheckDlgButton(hwndDlg, IDC_LCANCEL, BST_CHECKED);
			break;
			
		case POPUP_ACT_NOTHING:
		default:
			CheckDlgButton(hwndDlg, IDC_LNOTHING, BST_CHECKED);
			break;
		}
		// right action
		switch ( DBGetContactSettingByte( NULL, MODULENAME, SETTING_POPUP_RIGHTCLICK, POPUP_ACT_CLOSEPOPUP )) {
		case POPUP_ACT_CLOSEPOPUP:
			CheckDlgButton(hwndDlg, IDC_RCLOSE, BST_CHECKED);
			break;
			
		case POPUP_ACT_CANCEL:
			CheckDlgButton(hwndDlg, IDC_RCANCEL, BST_CHECKED);
			break;
			
		case POPUP_ACT_NOTHING:
		default:
			CheckDlgButton(hwndDlg, IDC_RNOTHING, BST_CHECKED);
			break;
		}
		// delay
		EnableWindow(GetDlgItem(hwndDlg, IDC_DELAYCUSTOM), ServiceExists(MS_POPUP_ADDPOPUPEX));
		EnableWindow(GetDlgItem(hwndDlg, IDC_DELAYFROMPU), ServiceExists(MS_POPUP_ADDPOPUPEX));
		EnableWindow(GetDlgItem(hwndDlg, IDC_DELAYPERMANENT), ServiceExists(MS_POPUP_ADDPOPUPEX));
		EnableWindow(GetDlgItem(hwndDlg, IDC_DELAY), ServiceExists(MS_POPUP_ADDPOPUPEX));
		switch (DBGetContactSettingByte(NULL, MODULENAME, SETTING_POPUP_DELAYTYPE, POPUP_DELAYFROMPU)) {
		case POPUP_DELAYCUSTOM:
			CheckDlgButton(hwndDlg, IDC_DELAYCUSTOM, BST_CHECKED);
			EnableWindow(GetDlgItem(hwndDlg, IDC_DELAY), ServiceExists(MS_POPUP_ADDPOPUPEX));
			break;

		case POPUP_DELAYPERMANENT:
			CheckDlgButton(hwndDlg, IDC_DELAYPERMANENT, BST_CHECKED);
			EnableWindow(GetDlgItem(hwndDlg, IDC_DELAY), FALSE);
			break;

		case POPUP_DELAYFROMPU:
		default:
			CheckDlgButton(hwndDlg, IDC_DELAYFROMPU, BST_CHECKED);
			EnableWindow(GetDlgItem(hwndDlg, IDC_DELAY), FALSE);
			break;
		}
		// delay
		SetDlgItemInt(hwndDlg,IDC_DELAY, DBGetContactSettingDword(NULL, MODULENAME, SETTING_POPUP_TIMEOUT, 0), FALSE);
		// back color
		SendDlgItemMessage(hwndDlg,IDC_BGCOLOR,CPM_SETCOLOUR,0,DBGetContactSettingDword(NULL, MODULENAME, SETTING_POPUP_BACKCOLOR, 0xAAAAAA));
		// text
		SendDlgItemMessage(hwndDlg,IDC_TEXTCOLOR,CPM_SETCOLOUR,0,DBGetContactSettingDword(NULL, MODULENAME, SETTING_POPUP_TEXTCOLOR, 0x0000CC));
		// wincolors
		CheckDlgButton(hwndDlg, IDC_WINCOLORS, DBGetContactSettingByte(NULL, MODULENAME, SETTING_POPUP_USEWINCOLORS, 0));
		// defaultcolors
		CheckDlgButton(hwndDlg, IDC_DEFAULTCOLORS, ( (DBGetContactSettingByte(NULL, MODULENAME, SETTING_POPUP_USEDEFCOLORS, 1)) && (!IsDlgButtonChecked(hwndDlg, IDC_WINCOLORS)) ));
		EnableWindow(GetDlgItem(hwndDlg, IDC_BGCOLOR), ((!IsDlgButtonChecked(hwndDlg, IDC_WINCOLORS)) && (!IsDlgButtonChecked(hwndDlg, IDC_DEFAULTCOLORS))));
		EnableWindow(GetDlgItem(hwndDlg, IDC_TEXTCOLOR), ((!IsDlgButtonChecked(hwndDlg, IDC_WINCOLORS)) && (!IsDlgButtonChecked(hwndDlg, IDC_DEFAULTCOLORS))));
		EnableWindow(GetDlgItem(hwndDlg, IDC_DEFAULTCOLORS), (!IsDlgButtonChecked(hwndDlg, IDC_WINCOLORS)));
		EnableWindow(GetDlgItem(hwndDlg, IDC_WINCOLORS), (!IsDlgButtonChecked(hwndDlg, IDC_DEFAULTCOLORS)));
		// popup types
		CheckDlgButton(hwndDlg, IDC_PUCONNLOST, DBGetContactSettingByte(NULL, MODULENAME, SETTING_PUCONNLOST, TRUE)?BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_PUOTHER, DBGetContactSettingByte(NULL, MODULENAME, SETTING_PUOTHER, TRUE)?BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_PUCONNRETRY, DBGetContactSettingByte(NULL, MODULENAME, SETTING_PUCONNRETRY, TRUE)?BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_PURESULT, DBGetContactSettingByte(NULL, MODULENAME, SETTING_PURESULT, TRUE)?BST_CHECKED:BST_UNCHECKED);
		CheckDlgButton(hwndDlg, IDC_PUSHOWEXTRA, DBGetContactSettingByte(NULL, MODULENAME, SETTING_PUSHOWEXTRA, TRUE)?BST_CHECKED:BST_UNCHECKED);
		bFreeze = false;
		break;
		
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_WINCOLORS:
		case IDC_DEFAULTCOLORS:
			EnableWindow(GetDlgItem(hwndDlg, IDC_BGCOLOR), ((!IsDlgButtonChecked(hwndDlg, IDC_WINCOLORS)) && (!IsDlgButtonChecked(hwndDlg, IDC_DEFAULTCOLORS))));
			EnableWindow(GetDlgItem(hwndDlg, IDC_TEXTCOLOR), ((!IsDlgButtonChecked(hwndDlg, IDC_WINCOLORS)) && (!IsDlgButtonChecked(hwndDlg, IDC_DEFAULTCOLORS))));
			EnableWindow(GetDlgItem(hwndDlg, IDC_DEFAULTCOLORS), (!IsDlgButtonChecked(hwndDlg, IDC_WINCOLORS)));
			EnableWindow(GetDlgItem(hwndDlg, IDC_WINCOLORS), (!IsDlgButtonChecked(hwndDlg, IDC_DEFAULTCOLORS)));
			SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			break;
			
		case IDC_DELAYFROMPU:
		case IDC_DELAYPERMANENT:
			EnableWindow(GetDlgItem(hwndDlg, IDC_DELAY), FALSE);
			SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			break;

		case IDC_DELAYCUSTOM:
			EnableWindow(GetDlgItem(hwndDlg, IDC_DELAY), TRUE);
			SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			break;

		case IDC_BGCOLOR:
		case IDC_TEXTCOLOR:
		case IDC_DELAY:
		case IDC_LNOTHING:
		case IDC_LCLOSE:
		case IDC_LCANCEL:
		case IDC_RNOTHING:
		case IDC_RCLOSE:
		case IDC_RCANCEL:
		case IDC_PUCONNLOST:
		case IDC_PUOTHER:
		case IDC_PUCONNRETRY:
		case IDC_PURESULT:
		case IDC_PUSHOWEXTRA:
			if ( !bFreeze )
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			break;
			
		case IDC_PREV:
			{
				POPUPDATAEX ppd = { NULL };
				
				ppd.lchContact = NULL;
				ppd.lchIcon = (HICON)CallService(MS_SKIN_LOADICON, (WPARAM)SKINICON_STATUS_OFFLINE, 0);
				strcpy( ppd.lpzContactName, Translate("KeepStatus") );
				strcpy( ppd.lpzText, Translate("You broke the Internet!") );
				if (IsDlgButtonChecked(hwndDlg, IDC_WINCOLORS))
					ppd.colorBack = GetSysColor(COLOR_BTNFACE);
				else if (IsDlgButtonChecked(hwndDlg, IDC_DEFAULTCOLORS))
					ppd.colorBack = (COLORREF)NULL;
				else
					ppd.colorBack = SendDlgItemMessage(hwndDlg,IDC_BGCOLOR,CPM_GETCOLOUR,0,0);
				if (IsDlgButtonChecked(hwndDlg, IDC_WINCOLORS))
					ppd.colorText = GetSysColor(COLOR_WINDOWTEXT);
				else if (IsDlgButtonChecked(hwndDlg, IDC_DEFAULTCOLORS))
					ppd.colorText = (COLORREF)NULL;
				else
					ppd.colorText = SendDlgItemMessage(hwndDlg,IDC_TEXTCOLOR,CPM_GETCOLOUR,0,0);
				ppd.PluginWindowProc = ( WNDPROC )PopupDlgProc;
				ppd.PluginData = NULL;
				if (!ServiceExists(MS_POPUP_ADDPOPUPEX))
					CallService(MS_POPUP_ADDPOPUP, (WPARAM)&ppd, 0);
				else {
					if (IsDlgButtonChecked(hwndDlg, IDC_DELAYFROMPU))
						ppd.iSeconds = 0;
					else if (IsDlgButtonChecked(hwndDlg, IDC_DELAYCUSTOM))
						ppd.iSeconds = GetDlgItemInt(hwndDlg, IDC_DELAY, NULL, FALSE);
					else if (IsDlgButtonChecked(hwndDlg, IDC_DELAYPERMANENT))
						ppd.iSeconds = -1;
					CallService(MS_POPUP_ADDPOPUPEX, (WPARAM)&ppd, 0);
				}
			}
			break;
		}
		break;
		
	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->code == PSN_APPLY) {
			// left action
			if (IsDlgButtonChecked(hwndDlg, IDC_LNOTHING))
				DBWriteContactSettingByte(NULL, MODULENAME, SETTING_POPUP_LEFTCLICK, POPUP_ACT_NOTHING);
			else if (IsDlgButtonChecked(hwndDlg, IDC_LCLOSE))
				DBWriteContactSettingByte(NULL, MODULENAME, SETTING_POPUP_LEFTCLICK, POPUP_ACT_CLOSEPOPUP);
			else if (IsDlgButtonChecked(hwndDlg, IDC_LCANCEL))
				DBWriteContactSettingByte(NULL, MODULENAME, SETTING_POPUP_LEFTCLICK, POPUP_ACT_CANCEL);
			// right action
			if (IsDlgButtonChecked(hwndDlg, IDC_RNOTHING))
				DBWriteContactSettingByte(NULL, MODULENAME, SETTING_POPUP_RIGHTCLICK, POPUP_ACT_NOTHING);
			else if (IsDlgButtonChecked(hwndDlg, IDC_RCLOSE))
				DBWriteContactSettingByte(NULL, MODULENAME, SETTING_POPUP_RIGHTCLICK, POPUP_ACT_CLOSEPOPUP);
			else if (IsDlgButtonChecked(hwndDlg, IDC_RCANCEL))
				DBWriteContactSettingByte(NULL, MODULENAME, SETTING_POPUP_RIGHTCLICK, POPUP_ACT_CANCEL);
			// delay
			if (IsDlgButtonChecked(hwndDlg, IDC_DELAYFROMPU))
				DBWriteContactSettingByte(NULL, MODULENAME, SETTING_POPUP_DELAYTYPE, POPUP_DELAYFROMPU);
			else if (IsDlgButtonChecked(hwndDlg, IDC_DELAYCUSTOM))
				DBWriteContactSettingByte(NULL, MODULENAME, SETTING_POPUP_DELAYTYPE, POPUP_DELAYCUSTOM);
			else if (IsDlgButtonChecked(hwndDlg, IDC_DELAYPERMANENT))
				DBWriteContactSettingByte(NULL, MODULENAME, SETTING_POPUP_DELAYTYPE, POPUP_DELAYPERMANENT);
			// delay
			DBWriteContactSettingDword(NULL, MODULENAME, SETTING_POPUP_TIMEOUT, GetDlgItemInt(hwndDlg,IDC_DELAY, NULL, FALSE));
			// back color
			DBWriteContactSettingDword(NULL, MODULENAME, SETTING_POPUP_BACKCOLOR, SendDlgItemMessage(hwndDlg,IDC_BGCOLOR,CPM_GETCOLOUR,0,0));
			// text color
			DBWriteContactSettingDword(NULL, MODULENAME, SETTING_POPUP_TEXTCOLOR, SendDlgItemMessage(hwndDlg,IDC_TEXTCOLOR,CPM_GETCOLOUR,0,0));
			// use win
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_POPUP_USEWINCOLORS, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_WINCOLORS));
			// use def
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_POPUP_USEDEFCOLORS, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_DEFAULTCOLORS));
			// store types
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_PUCONNLOST, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_PUCONNLOST));
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_PUOTHER, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_PUOTHER));
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_PUCONNRETRY, (BYTE)IsDlgButtonChecked(hwndDlg, IDC_PUCONNRETRY));
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_PURESULT,  (BYTE)IsDlgButtonChecked(hwndDlg, IDC_PURESULT));
			DBWriteContactSettingByte(NULL, MODULENAME, SETTING_PUSHOWEXTRA,  (BYTE)IsDlgButtonChecked(hwndDlg, IDC_PUSHOWEXTRA));
		}
		break;
	}
	
	return 0;
}

int OptionsInit(WPARAM wparam,LPARAM lparam)
{	
	if ( IsWinVerXPPlus()) {
		HMODULE hUxTheme = GetModuleHandle(_T("uxtheme.dll"));
		if (hUxTheme)
			pfnEnableThemeDialogTexture = (BOOL (WINAPI *)(HANDLE, DWORD))GetProcAddress(hUxTheme, "EnableThemeDialogTexture");
	}

	OPTIONSDIALOGPAGE odp = { 0 };
	odp.cbSize    = sizeof(odp);
	odp.hInstance = hInst;
	odp.pszGroup  = LPGEN("Status");
	odp.pszTitle  = LPGEN("KeepStatus");
	odp.flags = ODPF_BOLDGROUPS;

	odp.pszTemplate	= MAKEINTRESOURCEA(IDD_OPT_TABS);
	odp.pfnDlgProc	= DlgProcKsTabs;
	CallService(MS_OPT_ADDPAGE,wparam,(LPARAM)&odp);

	if ( ServiceExists( MS_POPUP_ADDPOPUP )) {
		ZeroMemory(&odp,sizeof(odp));
		odp.cbSize = sizeof(odp);
		odp.position = 150000000;
		odp.pszGroup = LPGEN("PopUps");
		odp.groupPosition = 910000000;
		odp.hInstance = hInst;
		odp.pszTemplate = MAKEINTRESOURCEA(IDD_PUOPT_KEEPSTATUS);
		odp.pszTitle = LPGEN("KeepStatus");
		odp.pfnDlgProc = PopupOptDlgProc;
		odp.flags = ODPF_BOLDGROUPS;
		CallService(MS_OPT_ADDPAGE,wparam,(LPARAM)&odp);
	}

	return 0;
}
