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

WNDPROC g_pOldProc;

LRESULT CALLBACK MyEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		SendMessage(GetParent(GetParent(hwnd)), PSM_CHANGED, 0, 0);
		break;
	}
	return CallWindowProc (g_pOldProc, hwnd, message, wParam, lParam);
}

INT_PTR CALLBACK UpdateNotifyOptsProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WORD i = 0;
	char str[20] = {0};
	switch (msg)
	{
	case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);
			CheckDlgButton(hwndDlg, IDC_UPDATEONSTARTUP, (int)UpdateOnStartup);
			CheckDlgButton(hwndDlg, IDC_ONLYONCEADAY, (int)OnlyOnceADay);
			EnableWindow(GetDlgItem(hwndDlg, IDC_ONLYONCEADAY), UpdateOnStartup);
			CheckDlgButton(hwndDlg, IDC_UPDATEONPERIOD, (int)UpdateOnPeriod);
			EnableWindow(GetDlgItem(hwndDlg, IDC_PERIOD), UpdateOnPeriod);
			EnableWindow(GetDlgItem(hwndDlg, IDC_PERIODSPIN), UpdateOnPeriod);
			EnableWindow(GetDlgItem(hwndDlg, IDC_PERIODMEASURE), UpdateOnPeriod);

			SendDlgItemMessage(hwndDlg, IDC_PERIODSPIN, UDM_SETRANGE, 0, MAKELONG(99, 1));
			SendDlgItemMessage(hwndDlg, IDC_PERIODSPIN, UDM_SETPOS, 0, (LPARAM)Period);

			Edit_LimitText(GetDlgItem(hwndDlg, IDC_PERIOD), 2);
			g_pOldProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(hwndDlg, IDC_PERIOD), GWLP_WNDPROC, (LONG)MyEditProc);

			ComboBox_InsertString(GetDlgItem(hwndDlg, IDC_PERIODMEASURE), 0, TranslateT("hours"));
			ComboBox_InsertString(GetDlgItem(hwndDlg, IDC_PERIODMEASURE), 1, TranslateT("days"));
			ComboBox_SetCurSel(GetDlgItem(hwndDlg, IDC_PERIODMEASURE), PeriodMeasure);

			CheckDlgButton(hwndDlg, IDC_REMINDER, (int)Reminder);
			if (ServiceExists(MS_POPUP_ADDPOPUP))
			{
				ShowWindow(GetDlgItem(hwndDlg, IDC_NOTIFY2), SW_HIDE);
				ShowWindow(GetDlgItem(hwndDlg, IDC_MSG_BOXES2), SW_HIDE);
				ShowWindow(GetDlgItem(hwndDlg, IDC_ERRORS2), SW_HIDE);
				ShowWindow(GetDlgItem(hwndDlg, IDC_INFO_MESSAGES2), SW_HIDE);
				ShowWindow(GetDlgItem(hwndDlg, IDC_PROGR_DLG2), SW_HIDE);
			}
			else
			{
				for (i = 1; i < POPUPS; i++) 
				{
					mir_snprintf(str, SIZEOF(str), "Popups%dM", i);
					CheckDlgButton(hwndDlg, (i+1029), (DBGetContactSettingByte(NULL, MODNAME, str, DEFAULT_MESSAGE_ENABLED)) ? BST_CHECKED: BST_UNCHECKED);
				}
			}
			return TRUE;
		}

	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) 
		{
		case IDC_UPDATEONSTARTUP:
			{
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
				EnableWindow(GetDlgItem(hwndDlg, IDC_ONLYONCEADAY), IsDlgButtonChecked(hwndDlg, IDC_UPDATEONSTARTUP));
			}
			break;
		case IDC_ONLYONCEADAY:
			SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			break;
		case IDC_UPDATEONPERIOD:
			{
				BOOL value = IsDlgButtonChecked(hwndDlg, IDC_UPDATEONPERIOD);
				EnableWindow(GetDlgItem(hwndDlg, IDC_PERIOD), value);
				EnableWindow(GetDlgItem(hwndDlg, IDC_PERIODSPIN), value);
				EnableWindow(GetDlgItem(hwndDlg, IDC_PERIODMEASURE), value);
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			}
			break;
		case IDC_PERIODMEASURE:
			if (HIWORD(wParam) == CBN_SELCHANGE)
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			break;
		case IDC_REMINDER:
			SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			break;
		case IDC_LINK_HOTKEY:
			{
				OPENOPTIONSDIALOG ood = {0};
				ood.cbSize = sizeof(ood);
				ood.pszGroup = "Customize";
				ood.pszPage = "Hotkeys";
				CallService( MS_OPT_OPENOPTIONS, 0, (LPARAM)&ood );
				return (true);
			} 
		case IDC_MSG_BOXES2:
		case IDC_ERRORS2:
		case IDC_INFO_MESSAGES2:
		case IDC_PROGR_DLG2:
			if ((HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == EN_CHANGE) && (HWND)lParam == GetFocus())
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			break;
		}
		break;
	}
	case WM_NOTIFY:
		{
			NMHDR *hdr = (NMHDR *)lParam;
			if(hdr && hdr->code == UDN_DELTAPOS)
			{
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			}
			if (hdr && hdr->code == PSN_APPLY) 
			{
				UpdateOnStartup = IsDlgButtonChecked(hwndDlg, IDC_UPDATEONSTARTUP);
				OnlyOnceADay = IsDlgButtonChecked(hwndDlg, IDC_ONLYONCEADAY);

				UpdateOnPeriod = IsDlgButtonChecked(hwndDlg, IDC_UPDATEONPERIOD);
				
				char buffer[3] = {0};
				Edit_GetText(GetDlgItem(hwndDlg, IDC_PERIOD), (LPWSTR)&buffer, 2);
				Period = atoi(buffer); 

				PeriodMeasure = ComboBox_GetCurSel(GetDlgItem(hwndDlg, IDC_PERIODMEASURE));
				
				InitTimer();

				DBWriteContactSettingByte(NULL, MODNAME, "UpdateOnStartup", UpdateOnStartup);
				DBWriteContactSettingByte(NULL, MODNAME, "OnlyOnceADay", OnlyOnceADay);
				DBWriteContactSettingByte(NULL, MODNAME, "UpdateOnPeriod", UpdateOnPeriod);
				DBWriteContactSettingDword(NULL, MODNAME, "Period", Period);
				DBWriteContactSettingByte(NULL, MODNAME, "PeriodMeasure", PeriodMeasure);
				Reminder = IsDlgButtonChecked(hwndDlg, IDC_REMINDER);
				DBWriteContactSettingByte(NULL, MODNAME, "Reminder", Reminder);
				if (!ServiceExists(MS_POPUP_ADDPOPUP))
				{
					for (i = 1; i < POPUPS; i++) 
					{
						mir_snprintf(str, SIZEOF(str), "Popups%dM", i);
						DBWriteContactSettingByte(NULL, MODNAME, str, (BYTE)(IsDlgButtonChecked(hwndDlg, (i+1029))));
					}
				}
			}
			break;
		}
	}//end* switch (msg)
	return FALSE;
}

INT_PTR CALLBACK DlgPopUpOpts(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	WORD i = 0;
	char str[20] = {0}, str2[20] = {0};

	switch (msg)
	{
	case WM_INITDIALOG:
	{
		TranslateDialogDefault(hdlg);
		//Colors
		if (MyOptions.DefColors == byCOLOR_OWN)
		{
			CheckDlgButton(hdlg, IDC_USEOWNCOLORS, BST_CHECKED);
			CheckDlgButton(hdlg, IDC_USEPOPUPCOLORS, BST_UNCHECKED);
			CheckDlgButton(hdlg, IDC_USEWINCOLORS, BST_UNCHECKED);
		}
		if (MyOptions.DefColors == byCOLOR_WINDOWS) 
		{
			CheckDlgButton(hdlg, IDC_USEOWNCOLORS, BST_UNCHECKED);
			CheckDlgButton(hdlg, IDC_USEPOPUPCOLORS, BST_UNCHECKED);
			CheckDlgButton(hdlg, IDC_USEWINCOLORS, BST_CHECKED);
		}
		if (MyOptions.DefColors == byCOLOR_POPUP) 
		{
			CheckDlgButton(hdlg, IDC_USEOWNCOLORS, BST_UNCHECKED);
			CheckDlgButton(hdlg, IDC_USEPOPUPCOLORS, BST_CHECKED);
			CheckDlgButton(hdlg, IDC_USEWINCOLORS, BST_UNCHECKED);
		}
		SendDlgItemMessage(hdlg, (42071), CPM_SETCOLOUR, 0, PopupsList[0].colorBack);
		SendDlgItemMessage(hdlg, (41071), CPM_SETCOLOUR, 0, PopupsList[0].colorText);
		for (i = 1; i < POPUPS; i++) 
		{
			SendDlgItemMessage(hdlg, (i+42071), CPM_SETCOLOUR, 0, PopupsList[i].colorBack);
			SendDlgItemMessage(hdlg, (i+41071), CPM_SETCOLOUR, 0, PopupsList[i].colorText);
			EnableWindow(GetDlgItem(hdlg, (i+42071)), (MyOptions.DefColors == byCOLOR_OWN));
			EnableWindow(GetDlgItem(hdlg, (i+41071)), (MyOptions.DefColors == byCOLOR_OWN));
		}
		//Timeout
		SendDlgItemMessage(hdlg, IDC_TIMEOUT_VALUE, EM_LIMITTEXT, 4, 0);
		SendDlgItemMessage(hdlg, IDC_TIMEOUT_VALUE_SPIN, UDM_SETRANGE32, -1, 9999);	
		SetDlgItemInt(hdlg, IDC_TIMEOUT_VALUE, MyOptions.Timeout, TRUE);
		//Mouse actions
		for (i = 0; i < SIZEOF(PopupActions); i++)
		{
			SendMessage(GetDlgItem(hdlg, IDC_LC), CB_SETITEMDATA, SendMessage(GetDlgItem(hdlg, IDC_LC), CB_ADDSTRING, 0, (LPARAM)TranslateTS(PopupActions[i].Text)), PopupActions[i].Action);
			SendMessage(GetDlgItem(hdlg, IDC_RC), CB_SETITEMDATA, SendMessage(GetDlgItem(hdlg, IDC_RC), CB_ADDSTRING, 0, (LPARAM)TranslateTS(PopupActions[i].Text)), PopupActions[i].Action);
		}
		SendDlgItemMessage(hdlg, IDC_LC, CB_SETCURSEL, MyOptions.LeftClickAction, 0);
		SendDlgItemMessage(hdlg, IDC_RC, CB_SETCURSEL, MyOptions.RightClickAction, 0);
		//Popups nitified
		for (i = 0; i < POPUPS; i++) 
		{
			mir_snprintf(str, SIZEOF(str), "Popups%d", i);
			mir_snprintf(str2, SIZEOF(str2), "Popups%dM", i);
			CheckDlgButton(hdlg, (i+40071), (DBGetContactSettingByte(NULL, MODNAME, str, DEFAULT_POPUP_ENABLED)) ? BST_CHECKED: BST_UNCHECKED);
			CheckDlgButton(hdlg, (i+1024), (DBGetContactSettingByte(NULL, MODNAME, str2, DEFAULT_MESSAGE_ENABLED)) ? BST_CHECKED: BST_UNCHECKED);
			if (IsDlgButtonChecked(hdlg, (i+40071)))
				EnableWindow(GetDlgItem(hdlg, (i+1024)), FALSE);
			else if (i > 0)
				EnableWindow(GetDlgItem(hdlg, (i+1024)), TRUE);
		}
		if (!(DBGetContactSettingDword(NULL, "PopUp", "Actions", 0) & 1)  || !ServiceExists(MS_POPUP_REGISTERACTIONS))
		{
			EnableWindow(GetDlgItem(hdlg, (40071)), FALSE);
			EnableWindow(GetDlgItem(hdlg, (41071)), FALSE);
			EnableWindow(GetDlgItem(hdlg, (42071)), FALSE);
		}
		else
		{
			EnableWindow(GetDlgItem(hdlg, (40071)), TRUE);
			EnableWindow(GetDlgItem(hdlg, (41071)), (MyOptions.DefColors == byCOLOR_OWN));
			EnableWindow(GetDlgItem(hdlg, (42071)), (MyOptions.DefColors == byCOLOR_OWN));
		}
		return TRUE;
	}
	case WM_SHOWWINDOW:
		if (!(DBGetContactSettingDword(NULL, "PopUp", "Actions", 0) & 1)  || !ServiceExists(MS_POPUP_REGISTERACTIONS))
		{
			EnableWindow(GetDlgItem(hdlg, (40071)), FALSE);
			EnableWindow(GetDlgItem(hdlg, (41071)), FALSE);
			EnableWindow(GetDlgItem(hdlg, (42071)), FALSE);
		}
		else
		{
			EnableWindow(GetDlgItem(hdlg, (40071)), TRUE);
			EnableWindow(GetDlgItem(hdlg, (41071)), (MyOptions.DefColors == byCOLOR_OWN));
			EnableWindow(GetDlgItem(hdlg, (42071)), (MyOptions.DefColors == byCOLOR_OWN));
		}
		return TRUE;
	case WM_COMMAND:
	{
		WORD idCtrl = LOWORD(wParam), wNotifyCode = HIWORD(wParam);
		if (wNotifyCode == CPN_COLOURCHANGED) 
		{
			if(idCtrl > 40070)
			{
				//It's a color picker change. idCtrl is the control id.
				COLORREF color = SendDlgItemMessage(hdlg, idCtrl, CPM_GETCOLOUR, 0, 0);
				int ctlID = idCtrl;
				if ((ctlID > 41070) && (ctlID < 42070)) //It's 41071 or above => Text color.
					PopupsList[ctlID-41071].colorText = color;
				else if (ctlID > 42070)//Background color.
					PopupsList[ctlID-42071].colorBack = color;
				SendMessage(GetParent(hdlg), PSM_CHANGED, 0, 0);
				return TRUE;
			}
		}
		if (wNotifyCode == CBN_SELCHANGE)
		{ 	
			if (idCtrl == IDC_LC)
			{
				MyOptions.LeftClickAction = (BYTE)SendDlgItemMessage(hdlg, IDC_LC, CB_GETCURSEL, 0, 0);
			}
			else if(idCtrl == IDC_RC)
			{
				MyOptions.RightClickAction = (BYTE)SendDlgItemMessage(hdlg, IDC_RC, CB_GETCURSEL, 0, 0);
			}
			SendMessage(GetParent(hdlg), PSM_CHANGED, 0, 0);
			return TRUE;
		}
		switch(idCtrl)
		{
			case IDC_USEOWNCOLORS: 
			{
				BOOL bEnableOthers = FALSE;
				if (wNotifyCode != BN_CLICKED) 
					break;
				MyOptions.DefColors = byCOLOR_OWN;
				bEnableOthers = TRUE;
				if (!(DBGetContactSettingDword(NULL, "PopUp", "Actions", 0) & 1)  || !ServiceExists(MS_POPUP_REGISTERACTIONS))
				{
					EnableWindow(GetDlgItem(hdlg, (41071)), FALSE);
					EnableWindow(GetDlgItem(hdlg, (42071)), FALSE);
				}
				else
				{
					EnableWindow(GetDlgItem(hdlg, (41071)), bEnableOthers);
					EnableWindow(GetDlgItem(hdlg, (42071)), bEnableOthers);
				}
				for (i = 1; i < POPUPS; i++) 
				{
					EnableWindow(GetDlgItem(hdlg, (i+42071)), bEnableOthers); //Background
					EnableWindow(GetDlgItem(hdlg, (i+41071)), bEnableOthers); //Text
				}
				SendMessage(GetParent(hdlg), PSM_CHANGED, 0, 0);
				break;
			}
			case IDC_USEWINCOLORS: 
			{
				BOOL bEnableOthers = FALSE;
				if (wNotifyCode != BN_CLICKED) 
					break;
				//Use Windows colors
				MyOptions.DefColors = byCOLOR_WINDOWS;
				bEnableOthers = FALSE;
				for (i = 0; i < POPUPS; i++) 
				{
					EnableWindow(GetDlgItem(hdlg, (i+42071)), bEnableOthers); //Background
					EnableWindow(GetDlgItem(hdlg, (i+41071)), bEnableOthers); //Text
				}
				SendMessage(GetParent(hdlg), PSM_CHANGED, 0, 0);
				break;
			}
			case IDC_USEPOPUPCOLORS:
			{
				BOOL bEnableOthers = FALSE;
				if (wNotifyCode != BN_CLICKED)
					break;
				//Use Popup colors
				MyOptions.DefColors = byCOLOR_POPUP;
				bEnableOthers = FALSE;
				for (i = 0; i < POPUPS; i++) 
				{
					EnableWindow(GetDlgItem(hdlg, (i+42071)), bEnableOthers); //Background
					EnableWindow(GetDlgItem(hdlg, (i+41071)), bEnableOthers); //Text
				}
				SendMessage(GetParent(hdlg), PSM_CHANGED, 0, 0);
				break;
			}
			case IDC_PREVIEW: 
			{//Declarations and initializations
				Title = TranslateT("Pack Updater");
				Text = TranslateT("Test");
				for (int i = 0; i < POPUPS; i++) 
				{
					if ((!IsDlgButtonChecked(hdlg, (i+40071))) || (!IsWindowEnabled(GetDlgItem(hdlg, (i+40071)))))
						continue;
					show_popup(0, Title, Text, i, 0);
				}
				break;
			}
			case IDC_TIMEOUT_VALUE:
			case IDC_MSG_BOXES:
			case IDC_ERRORS:
			{
				if (!IsDlgButtonChecked(hdlg, IDC_ERRORS))
					EnableWindow(GetDlgItem(hdlg, IDC_ERRORS_MSG), TRUE);
				else
					EnableWindow(GetDlgItem(hdlg, IDC_ERRORS_MSG), FALSE);
				if ((HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == EN_CHANGE) && (HWND)lParam == GetFocus())
					SendMessage(GetParent(hdlg), PSM_CHANGED, 0, 0);
				break;
			}
			case IDC_INFO_MESSAGES:
			{
				if (!IsDlgButtonChecked(hdlg, IDC_INFO_MESSAGES))
					EnableWindow(GetDlgItem(hdlg, IDC_INFO_MESSAGES_MSG), TRUE);
				else
					EnableWindow(GetDlgItem(hdlg, IDC_INFO_MESSAGES_MSG), FALSE);
				if ((HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == EN_CHANGE) && (HWND)lParam == GetFocus())
					SendMessage(GetParent(hdlg), PSM_CHANGED, 0, 0);
				break;
			}
			case IDC_PROGR_DLG:
			{
				if (!IsDlgButtonChecked(hdlg, IDC_PROGR_DLG))
					EnableWindow(GetDlgItem(hdlg, IDC_PROGR_DLG_MSG), TRUE);
				else
					EnableWindow(GetDlgItem(hdlg, IDC_PROGR_DLG_MSG), FALSE);
				if ((HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == EN_CHANGE) && (HWND)lParam == GetFocus())
					SendMessage(GetParent(hdlg), PSM_CHANGED, 0, 0);
				break;
			}
			case IDC_MSG_BOXES_MSG:
			case IDC_ERRORS_MSG:
			case IDC_INFO_MESSAGES_MSG:
			case IDC_PROGR_DLG_MSG:
				if ((HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == EN_CHANGE) && (HWND)lParam == GetFocus())
					SendMessage(GetParent(hdlg), PSM_CHANGED, 0, 0);
			break;
		}//end* switch(idCtrl)
		break;
	}//end* case WM_COMMAND:
	case WM_NOTIFY: 
		switch (((LPNMHDR)lParam)->code) 
		{
			case PSN_RESET:
			{
				//Restore the options stored in memory.
				LoadOptions();
				InitPopupList();
				return TRUE;
			}
			case PSN_APPLY: 
			{
				//Text color
				char szSetting[20] = {0};
				DWORD ctlColor = 0;
				for (i = 0; i <= POPUPS-1; i++) 
				{
					ctlColor = SendDlgItemMessage(hdlg, (i+42071), CPM_GETCOLOUR, 0, 0);
					PopupsList[i].colorBack = ctlColor;
					mir_snprintf(szSetting, SIZEOF(szSetting), "Popups%iBg", i);
					DBWriteContactSettingDword(NULL, MODNAME, szSetting, ctlColor);
					ctlColor = SendDlgItemMessage(hdlg, (i+41071), CPM_GETCOLOUR, 0, 0);
					PopupsList[i].colorText = ctlColor;
					mir_snprintf(szSetting, SIZEOF(szSetting), "Popups%iTx", i);
					DBWriteContactSettingDword(NULL, MODNAME, szSetting, ctlColor);
				}
				//Colors
				DBWriteContactSettingByte(NULL, MODNAME, "DefColors", MyOptions.DefColors);
				//Timeout
				MyOptions.Timeout = GetDlgItemInt(hdlg, IDC_TIMEOUT_VALUE, 0, TRUE);
				DBWriteContactSettingDword(NULL, MODNAME, "Timeout", MyOptions.Timeout);
				//Left mouse click
				DBWriteContactSettingByte(NULL, MODNAME, "LeftClickAction", MyOptions.LeftClickAction);
				//Right mouse click
				DBWriteContactSettingByte(NULL, MODNAME, "RightClickAction", MyOptions.RightClickAction);
				//Notified popups
				for (i = 0; i < POPUPS; i++) 
				{
					mir_snprintf(str, SIZEOF(str), "Popups%d", i);
					DBWriteContactSettingByte(NULL, MODNAME, str, (BYTE)(IsDlgButtonChecked(hdlg, (i+40071))));
					mir_snprintf(str2, SIZEOF(str2), "Popups%dM", i);
					DBWriteContactSettingByte(NULL, MODNAME, str2, (BYTE)(IsDlgButtonChecked(hdlg, (i+1024))));
				}
				return TRUE;
			} //case PSN_APPLY
		} // switch code
	break; //End WM_NOTIFY
	} //switch message
return FALSE;
}

int OptInit(WPARAM wParam, LPARAM lParam)
{
 	OPTIONSDIALOGPAGE odp = {0};

	ZeroMemory(&odp, sizeof(odp));
	odp.cbSize = sizeof(odp);
    odp.position = 100000000;
    odp.hInstance = hInst;
    odp.flags = ODPF_TCHAR | ODPF_BOLDGROUPS;
    odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_UPDATENOTIFY);
    odp.ptszGroup = _T("Events");
    odp.ptszTitle = _T("Pack Updater");
    odp.pfnDlgProc = UpdateNotifyOptsProc;
    CallService(MS_OPT_ADDPAGE, wParam, (LPARAM)&odp);
	if (ServiceExists(MS_POPUP_ADDPOPUP))
	{
		odp.pszTemplate = MAKEINTRESOURCEA(IDD_POPUP);
		odp.ptszGroup = _T("PopUps");
		odp.ptszTitle = _T("Pack Updater");
		odp.pfnDlgProc = DlgPopUpOpts;
		CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);
	}
    return 0;
}