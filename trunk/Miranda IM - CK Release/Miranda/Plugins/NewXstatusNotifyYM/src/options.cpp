/*
Name_day plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
Copyright (c) 2001-2004 Luca Santarelli
Copyright (c) 2005-2007 Vasilich
Copyright (c) 2007-2011 yaho
Copyright (c) 2011-2012 Mataes

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
#include "indsnd.h"
#include "options.h"
#include "popup.h"
#include "xstatus.h"
#include "utils.h"

OPTIONS opt = {0};
TEMPLATES templates = {0};
BOOL UpdateListFlag = FALSE;
SortedList *ProtoTamplates; 
int LastItem = 0;

extern HINSTANCE hInst;
extern HANDLE hEnableDisableMenu;
extern STATUS StatusList[STATUS_COUNT];

INT_PTR CALLBACK DlgProcAutoDisableOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void LoadTemplates()
{	
	DBGetStringDefault(0, MODULE, "TPopupDelimiter", templates.PopupDelimiter, SIZEOF(templates.PopupDelimiter), DEFAULT_POPUP_DELIMITER);
	DBGetStringDefault(0, MODULE, "TPopupChange", templates.PopupNewXstatus, SIZEOF(templates.PopupNewXstatus), DEFAULT_POPUP_NEW);
	DBGetStringDefault(0, MODULE, "TPopupChangeMsg", templates.PopupNewMsg, SIZEOF(templates.PopupNewMsg), DEFAULT_POPUP_CHANGEMSG);
	DBGetStringDefault(0, MODULE, "TPopupRemoval", templates.PopupRemove, SIZEOF(templates.PopupRemove), DEFAULT_POPUP_REMOVE);
	
	DBGetStringDefault(0, MODULE, "TLogDelimiter", templates.LogDelimiter, SIZEOF(templates.LogDelimiter), DEFAULT_LOG_DELIMITER);
	DBGetStringDefault(0, MODULE, "TLogChange", templates.LogNewXstatus, SIZEOF(templates.LogNewXstatus), DEFAULT_LOG_NEW);
	DBGetStringDefault(0, MODULE, "TLogChangeMsg", templates.LogNewMsg, SIZEOF(templates.LogNewMsg), DEFAULT_LOG_CHANGEMSG);
	DBGetStringDefault(0, MODULE, "TLogRemoval", templates.LogRemove, SIZEOF(templates.LogRemove), DEFAULT_LOG_REMOVE);
	DBGetStringDefault(0, MODULE, "TLogOpening", templates.LogOpening, SIZEOF(templates.LogOpening), DEFAULT_LOG_OPENING);
	templates.PopupFlags = DBGetContactSettingByte(0, MODULE, "TPopupFlags", NOTIFY_NEW_XSTATUS | NOTIFY_NEW_MESSAGE);
	templates.LogFlags = DBGetContactSettingByte(0, MODULE, "TLogFlags", NOTIFY_NEW_XSTATUS | NOTIFY_NEW_MESSAGE | NOTIFY_OPENING_ML);
}

void LoadOptions() 
{
	// IDD_OPT_POPUP
	opt.Colors = DBGetContactSettingByte(0,MODULE, "Colors", DEFAULT_COLORS);
	opt.ShowGroup = DBGetContactSettingByte(0, MODULE, "ShowGroup", 0);
	opt.ShowStatus = DBGetContactSettingByte(0, MODULE, "ShowStatus", 1);
	opt.UseAlternativeText = DBGetContactSettingByte(0, MODULE, "UseAlternativeText", 0);
	opt.ShowPreviousStatus = DBGetContactSettingByte(0, MODULE, "ShowPreviousStatus", 0);
	opt.ReadAwayMsg = DBGetContactSettingByte(0, MODULE, "ReadAwayMsg", 0);
	opt.PopupTimeout = DBGetContactSettingDword(0, MODULE, "PopupTimeout", 0);
	opt.LeftClickAction= DBGetContactSettingByte(0, MODULE, "LeftClickAction", 5);
	opt.RightClickAction = DBGetContactSettingByte(0, MODULE, "RightClickAction", 1);
	// IDD_OPT_XPOPUP
	opt.PDisableForMusic = DBGetContactSettingByte(0, MODULE, "PDisableForMusic", 1);
	opt.PTruncateMsg = DBGetContactSettingByte(0, MODULE, "PTruncateMsg", 0);
	opt.PMsgLen = DBGetContactSettingDword(0, MODULE, "PMsgLen", 64);
	// IDD_OPT_GENERAL
	opt.FromOffline = DBGetContactSettingByte(0, MODULE, "FromOffline", 1);
	opt.AutoDisable = DBGetContactSettingByte(0, MODULE, "AutoDisable", 1);
	opt.HiddenContactsToo = DBGetContactSettingByte(0, MODULE, "HiddenContactsToo", 1);
	opt.UseIndSnd = DBGetContactSettingByte(0, MODULE, "UseIndSounds", 1);
	opt.BlinkIcon = DBGetContactSettingByte(0, MODULE, "BlinkIcon", 0);
	opt.BlinkIcon_Status = DBGetContactSettingByte(0, MODULE, "BlinkIcon_Status", 0);
	opt.Log = DBGetContactSettingByte(0, MODULE, "Log", 0);
	DBGetStringDefault(0, MODULE, "LogFilePath", opt.LogFilePath, MAX_PATH, _T(""));	
	// IDD_AUTODISABLE
	opt.OnlyGlobalChanges = DBGetContactSettingByte(0, MODULE, "OnlyGlobalChanges", 1);
	opt.DisablePopupGlobally = DBGetContactSettingByte(0, MODULE, "DisablePopupGlobally", 1);
	opt.DisableSoundGlobally = DBGetContactSettingByte(0, MODULE, "DisableSoundGlobally", 1);
	// IDD_OPT_XLOG
	opt.EnableLogging = DBGetContactSettingByte(0, MODULE, "EnableLogging", 0);
	opt.PreventIdentical = DBGetContactSettingByte(0, MODULE, "PreventIdentical", 1);
	opt.KeepInHistory = DBGetContactSettingByte(0, MODULE, "KeepInHistory", 0);
	opt.LDisableForMusic = DBGetContactSettingByte(0, MODULE, "LDisableForMusic", 1);
	opt.LTruncateMsg = DBGetContactSettingByte(0, MODULE, "LTruncateMsg", 0);
	opt.LMsgLen = DBGetContactSettingDword(0, MODULE, "LMsgLen", 128);
	//IDD_OPT_SMPOPUP
	opt.IgnoreEmpty = DBGetContactSettingByte(0, MODULE, "IgnoreEmpty", 1);
	opt.PopupOnConnect = DBGetContactSettingByte(0, MODULE, "PopupOnConnect", 0);
	// OTHER
	opt.TempDisabled = DBGetContactSettingByte(0, MODULE, "TempDisable", 0);

	LoadTemplates();

	return;
}

void SaveTemplates()
{
	DBWriteContactSettingTString(0, MODULE, "TPopupDelimiter", templates.PopupDelimiter);
	DBWriteContactSettingTString(0, MODULE, "TPopupChange", templates.PopupNewXstatus);
	DBWriteContactSettingTString(0, MODULE, "TPopupChangeMsg", templates.PopupNewMsg);
	DBWriteContactSettingTString(0, MODULE, "TPopupRemoval", templates.PopupRemove);
	DBWriteContactSettingTString(0, MODULE, "TLogDelimiter", templates.LogDelimiter);
	DBWriteContactSettingTString(0, MODULE, "TLogChange", templates.LogNewXstatus);
	DBWriteContactSettingTString(0, MODULE, "TLogChangeMsg", templates.LogNewMsg);
	DBWriteContactSettingTString(0, MODULE, "TLogRemoval", templates.LogRemove);
	DBWriteContactSettingTString(0, MODULE, "TLogOpening", templates.LogOpening);
	DBWriteContactSettingByte(0, MODULE, "TPopupFlags", templates.PopupFlags);
	DBWriteContactSettingByte(0, MODULE, "TLogFlags", templates.LogFlags);
	for (int i = 0; i < ProtoTamplates->realCount; i++)
	{
		PROTOTAMPLATE *prototamplate = (PROTOTAMPLATE *)ProtoTamplates->items[i];
		TCHAR str[MAX_PATH];
		mir_sntprintf(str, SIZEOF(str), _T("%s_TSMChange"), prototamplate->ProtoName);
		char *szstr = mir_t2a(str);
		DBWriteContactSettingTString(0, MODULE, szstr, prototamplate->ProtoTamplate);
	}
}

void SaveOptions() 
{
	// IDD_OPT_POPUP
	DBWriteContactSettingByte(0, MODULE, "Colors", opt.Colors);
	DBWriteContactSettingByte(0, MODULE, "ShowGroup", opt.ShowGroup);
	DBWriteContactSettingByte(0, MODULE, "ShowStatus", opt.ShowStatus);
	DBWriteContactSettingByte(0, MODULE, "UseAlternativeText", opt.UseAlternativeText);
	DBWriteContactSettingByte(0, MODULE, "ShowPreviousStatus", opt.ShowPreviousStatus);
	DBWriteContactSettingByte(0, MODULE, "ReadAwayMsg", opt.ReadAwayMsg);
	DBWriteContactSettingDword(0, MODULE, "PopupTimeout", opt.PopupTimeout);
	DBWriteContactSettingByte(0, MODULE, "LeftClickAction", opt.LeftClickAction);
	DBWriteContactSettingByte(0, MODULE, "RightClickAction", opt.RightClickAction);
	// IDD_OPT_XPOPUP
	DBWriteContactSettingByte(0, MODULE, "PDisableForMusic", opt.PDisableForMusic);
	DBWriteContactSettingByte(0, MODULE, "PTruncateMsg", opt.PTruncateMsg);
	DBWriteContactSettingDword(0, MODULE, "PMsgLen", opt.PMsgLen);
	// IDD_OPT_GENERAL
	DBWriteContactSettingByte(0, MODULE, "FromOffline", opt.FromOffline);
	DBWriteContactSettingByte(0, MODULE, "AutoDisable", opt.AutoDisable);
	DBWriteContactSettingByte(0, MODULE, "HiddenContactsToo", opt.HiddenContactsToo);
	DBWriteContactSettingByte(0, MODULE, "UseIndSounds", opt.UseIndSnd);
	DBWriteContactSettingByte(0, MODULE, "BlinkIcon", opt.BlinkIcon);
	DBWriteContactSettingByte(0, MODULE, "BlinkIcon_Status", opt.BlinkIcon_Status);
	DBWriteContactSettingByte(0, MODULE, "Log", opt.Log);
	DBWriteContactSettingTString(0, MODULE, "LogFilePath", opt.LogFilePath);
	// IDD_AUTODISABLE
	DBWriteContactSettingByte(0, MODULE, "OnlyGlobalChanges", opt.OnlyGlobalChanges);
	DBWriteContactSettingByte(0, MODULE, "DisablePopupGlobally", opt.DisablePopupGlobally);
	DBWriteContactSettingByte(0, MODULE, "DisableSoundGlobally", opt.DisableSoundGlobally);
	// IDD_OPT_XLOG
	DBWriteContactSettingByte(0, MODULE, "EnableLogging", opt.EnableLogging);
	DBWriteContactSettingByte(0, MODULE, "PreventIdentical", opt.PreventIdentical);
	DBWriteContactSettingByte(0, MODULE, "KeepInHistory", opt.KeepInHistory);
	DBWriteContactSettingByte(0, MODULE, "LDisableForMusic", opt.LDisableForMusic);
	DBWriteContactSettingByte(0, MODULE, "LTruncateMsg", opt.LTruncateMsg);
	DBWriteContactSettingDword(0, MODULE, "LMsgLen", opt.LMsgLen);	
	//IDD_OPT_SMPOPUP
	DBWriteContactSettingByte(0, MODULE, "IgnoreEmpty", opt.IgnoreEmpty);
	DBWriteContactSettingByte(0, MODULE, "PopupOnConnect", opt.PopupOnConnect);	
}

INT_PTR CALLBACK DlgProcGeneralOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	switch (msg) 	
	{
		case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);

			CheckDlgButton(hwndDlg, IDC_AUTODISABLE, opt.AutoDisable);
			EnableWindow(GetDlgItem(hwndDlg, IDC_CONFIGUREAUTODISABLE), opt.AutoDisable);

			CheckDlgButton(hwndDlg, IDC_HIDDENCONTACTSTOO, opt.HiddenContactsToo);
			CheckDlgButton(hwndDlg, IDC_USEINDIVSOUNDS, opt.UseIndSnd);
			CheckDlgButton(hwndDlg, IDC_BLINKICON, opt.BlinkIcon);
			CheckDlgButton(hwndDlg, IDC_BLINKICON_STATUS, opt.BlinkIcon_Status);
			EnableWindow(GetDlgItem(hwndDlg, IDC_BLINKICON_STATUS), opt.BlinkIcon);

			CheckDlgButton(hwndDlg, IDC_LOG, opt.Log);
			SetDlgItemText(hwndDlg, IDC_LOGFILE, opt.LogFilePath);
			EnableWindow(GetDlgItem(hwndDlg, IDC_LOGFILE), opt.Log);
			EnableWindow(GetDlgItem(hwndDlg, IDC_BT_CHOOSELOGFILE), opt.Log);
			EnableWindow(GetDlgItem(hwndDlg, IDC_BT_VIEWLOG), opt.Log);

			//Statuses notified
			char status[8];
			for (int i = ID_STATUS_MIN; i <= ID_STATUS_MAX2; i++) 
			{
				wsprintfA(status, "%d", i);
				CheckDlgButton(hwndDlg, i, DBGetContactSettingByte(0, MODULE, status, 1));
			}
			CheckDlgButton(hwndDlg, IDC_CHK_FROMOFFLINE, opt.FromOffline);

			return TRUE;
		}
		case WM_COMMAND:
		{
			switch(LOWORD(wParam)) 
			{
				case IDC_CONFIGUREAUTODISABLE:
					CreateDialog(hInst, MAKEINTRESOURCE(IDD_AUTODISABLE), hwndDlg, DlgProcAutoDisableOpts);
					return FALSE;
				case IDC_AUTODISABLE:
					EnableWindow(GetDlgItem(hwndDlg, IDC_CONFIGUREAUTODISABLE), IsDlgButtonChecked(hwndDlg, IDC_AUTODISABLE));
					break;
				case IDC_BLINKICON:
					EnableWindow(GetDlgItem(hwndDlg, IDC_BLINKICON_STATUS), IsDlgButtonChecked(hwndDlg, IDC_BLINKICON));
					break;
				case IDC_BT_VIEWLOG:
					ShowLog(opt.LogFilePath);
					return FALSE;
				case IDC_BT_CHOOSELOGFILE:
				{
					TCHAR buff[MAX_PATH];
					OPENFILENAME ofn = {0};

					GetDlgItemText(hwndDlg, IDC_LOGFILE, buff, MAX_PATH);

					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.lpstrFile = buff;
					ofn.nMaxFile = MAX_PATH;
					ofn.hwndOwner = hwndDlg;
					TCHAR filter[MAX_PATH];
					mir_sntprintf(filter, SIZEOF(filter), _T("%s (*.*)%c*.*%c%s (*.log)%c*.log%c%s (*.txt)%c*.txt%c"), TranslateT("All Files"), 0, 0, TranslateT("Log"), 0, 0, TranslateT("Text"), 0, 0);
					ofn.lpstrFilter = filter;
					ofn.nFilterIndex = 2;
					ofn.lpstrInitialDir = buff;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
					ofn.lpstrDefExt = _T("log");
					if (GetSaveFileName(&ofn))
						SetDlgItemText(hwndDlg, IDC_LOGFILE, buff);
											   
					break;
				}
				case IDC_LOG:
					EnableWindow(GetDlgItem(hwndDlg, IDC_LOGFILE), IsDlgButtonChecked(hwndDlg, IDC_LOG));
					EnableWindow(GetDlgItem(hwndDlg, IDC_BT_CHOOSELOGFILE), IsDlgButtonChecked(hwndDlg, IDC_LOG));
					EnableWindow(GetDlgItem(hwndDlg, IDC_BT_VIEWLOG), IsDlgButtonChecked(hwndDlg, IDC_LOG));
					break;
				case IDC_HYPERLINK:
				{
					OPENOPTIONSDIALOG ood = {0};
					ood.cbSize = sizeof(ood);
					ood.pszGroup = "Customize";
					ood.pszPage = "Sounds";
					CallService(MS_OPT_OPENOPTIONS, 0, (LPARAM)&ood);
					return FALSE;
				}
			}

			if (HIWORD(wParam) == BN_CLICKED || (HIWORD(wParam) == EN_CHANGE && (HWND)lParam == GetFocus()))
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);

			break;
		}
		case WM_NOTIFY:
		{
			if (((LPNMHDR)lParam)->code == PSN_APPLY) 
			{
				opt.AutoDisable = IsDlgButtonChecked(hwndDlg, IDC_AUTODISABLE);
				opt.BlinkIcon = IsDlgButtonChecked(hwndDlg, IDC_BLINKICON);
				opt.BlinkIcon_Status = IsDlgButtonChecked(hwndDlg, IDC_BLINKICON_STATUS);
				opt.HiddenContactsToo = IsDlgButtonChecked(hwndDlg, IDC_HIDDENCONTACTSTOO);
				opt.UseIndSnd = IsDlgButtonChecked(hwndDlg, IDC_USEINDIVSOUNDS);

				opt.Log = IsDlgButtonChecked(hwndDlg, IDC_LOG);
				GetDlgItemText(hwndDlg, IDC_LOGFILE, opt.LogFilePath, SIZEOF(opt.LogFilePath));

				//Notified statuses
				char status[8];
				for (int i = ID_STATUS_MIN; i <= ID_STATUS_MAX2; i++) 
				{
					wsprintfA(status, "%d", i);
					DBWriteContactSettingByte(NULL, MODULE, status, (BYTE)IsDlgButtonChecked(hwndDlg, i));
				}
				opt.FromOffline = IsDlgButtonChecked(hwndDlg, IDC_CHK_FROMOFFLINE);

				SaveOptions();
				return TRUE;
			}
		}
	}

	return FALSE;
}

void EnablePopupTextControls(HWND hwndDlg, BOOL bEnable)
{
	EnableWindow(GetDlgItem(hwndDlg, IDC_SHOWALTDESCS), bEnable);
	EnableWindow(GetDlgItem(hwndDlg, IDC_SHOWPREVIOUSSTATUS), bEnable);
	//EnableWindow(GetDlgItem(hwndDlg, IDC_READAWAYMSG), bEnable);
}

INT_PTR CALLBACK DlgProcPopUpOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	switch (msg) 
	{
		case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);

			CheckDlgButton(hwndDlg, IDC_USEOWNCOLORS, (opt.Colors == POPUP_COLOR_OWN) ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_USEPOPUPCOLORS, (opt.Colors == POPUP_COLOR_POPUP) ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_USEWINCOLORS, (opt.Colors == POPUP_COLOR_WINDOWS) ? 1 : 0);

			for (int i = ID_STATUS_MIN; i <= ID_STATUS_MAX2 - 1; i++) 
			{
				SendDlgItemMessage(hwndDlg, (i+2000), CPM_SETCOLOUR, 0, StatusList[Index(i)].colorBack);
				SendDlgItemMessage(hwndDlg, (i+1000), CPM_SETCOLOUR, 0, StatusList[Index(i)].colorText);
				EnableWindow(GetDlgItem(hwndDlg, (i+2000)), (opt.Colors == POPUP_COLOR_OWN));
				EnableWindow(GetDlgItem(hwndDlg, (i+1000)), (opt.Colors == POPUP_COLOR_OWN));
			}

			CheckDlgButton(hwndDlg, IDC_SHOWSTATUS, opt.ShowStatus);
			CheckDlgButton(hwndDlg, IDC_SHOWALTDESCS, opt.UseAlternativeText);
			CheckDlgButton(hwndDlg, IDC_READAWAYMSG, opt.ReadAwayMsg);
			CheckDlgButton(hwndDlg, IDC_SHOWPREVIOUSSTATUS, opt.ShowPreviousStatus);
			CheckDlgButton(hwndDlg, IDC_SHOWGROUP, opt.ShowGroup);

			SendDlgItemMessage(hwndDlg, IDC_TIMEOUT_VALUE, EM_LIMITTEXT, 3, 0);
			SendDlgItemMessage(hwndDlg, IDC_TIMEOUT_VALUE_SPIN, UDM_SETRANGE32, -1, 999);	
			SetDlgItemInt(hwndDlg, IDC_TIMEOUT_VALUE, opt.PopupTimeout, TRUE);

			//Mouse actions
			for (int i = 0; i < SIZEOF(PopupActions); i++)
			{
				SendMessage(GetDlgItem(hwndDlg, IDC_STATUS_LC), CB_SETITEMDATA, SendMessage(GetDlgItem(hwndDlg, IDC_STATUS_LC), CB_ADDSTRING, 0, (LPARAM)TranslateTS(PopupActions[i].Text)), PopupActions[i].Action);
				SendMessage(GetDlgItem(hwndDlg, IDC_STATUS_RC), CB_SETITEMDATA, SendMessage(GetDlgItem(hwndDlg, IDC_STATUS_RC), CB_ADDSTRING, 0, (LPARAM)TranslateTS(PopupActions[i].Text)), PopupActions[i].Action);
			}

			SendDlgItemMessage(hwndDlg, IDC_STATUS_LC, CB_SETCURSEL, opt.LeftClickAction, 0);
			SendDlgItemMessage(hwndDlg, IDC_STATUS_RC, CB_SETCURSEL, opt.RightClickAction, 0);

			EnablePopupTextControls(hwndDlg, opt.ShowStatus);

			return TRUE;
		}
		case WM_COMMAND: 
		{
			WORD idCtrl = LOWORD(wParam);
			if (HIWORD(wParam) == CPN_COLOURCHANGED) 
			{
				if(idCtrl > 40070)
				{
					COLORREF colour = SendDlgItemMessage(hwndDlg, idCtrl, CPM_GETCOLOUR, 0, 0);
					if ((idCtrl > 41070) && (idCtrl < 42070)) //Text colour
						StatusList[Index(idCtrl - 1000)].colorText = colour;
					else //Background colour
						StatusList[Index(idCtrl - 2000)].colorBack = colour;
					SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
					return TRUE;
				}
			}

			if (HIWORD(wParam) == BN_CLICKED)
			{
				switch(idCtrl)
				{
					case IDC_USEOWNCOLORS: 
					case IDC_USEWINCOLORS:
					case IDC_USEPOPUPCOLORS:
					{
						for (int i = ID_STATUS_MIN; i <= ID_STATUS_MAX2 - 1; i++)
						{
							EnableWindow(GetDlgItem(hwndDlg, (i+2000)), idCtrl == IDC_USEOWNCOLORS); //Background
							EnableWindow(GetDlgItem(hwndDlg, (i+1000)), idCtrl == IDC_USEOWNCOLORS); //Text
						}

						break;
					}
					case IDC_SHOWSTATUS:
					{
						EnablePopupTextControls(hwndDlg, IsDlgButtonChecked(hwndDlg, IDC_SHOWSTATUS));
						break;
					}
					case IDC_PREVIEW: 
					{
						POPUPDATAT ppd = {0};
						ppd.iSeconds = opt.PopupTimeout;

						for (int i = ID_STATUS_MIN; i <= (ID_STATUS_MAX+1); i++) 
						{					
							WORD status = i <= ID_STATUS_MAX2 ? i : ID_STATUS_MIN;
							ppd.lchIcon = LoadSkinnedIcon(StatusList[Index(status)].icon);

							_tcscpy(ppd.lptzContactName, (TCHAR *)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)NULL, GSMDF_TCHAR));
							_tcscpy(ppd.lptzText, _T(""));

							if (opt.ShowStatus)
							{
								if (opt.UseAlternativeText == 1)
									_tcscpy(ppd.lptzText, StatusList[Index(status)].lpzUStatusText);
								else
									_tcscpy(ppd.lptzText, StatusList[Index(status)].lpzStandardText);
							
								if (opt.ShowPreviousStatus)
								{
									TCHAR buff[MAX_STATUSTEXT];
									mir_sntprintf(buff, SIZEOF(buff), TranslateTS(STRING_SHOWPREVIOUSSTATUS), StatusList[Index(status)].lpzStandardText);
									mir_sntprintf(ppd.lptzText, SIZEOF(ppd.lptzText), _T("%s %s"), ppd.lptzText, buff);
								}
							}

							if (opt.ReadAwayMsg)
							{
								if (ppd.lptzText[0]) _tcscat(ppd.lptzText, _T("\n"));
								_tcscat(ppd.lptzText, TranslateT("This is status message"));
							}

							switch (opt.Colors)
							{
								case POPUP_COLOR_WINDOWS:
									ppd.colorBack = GetSysColor(COLOR_BTNFACE);
									ppd.colorText = GetSysColor(COLOR_WINDOWTEXT);
									break;
								case POPUP_COLOR_OWN:
									ppd.colorBack = StatusList[Index(status)].colorBack;
									ppd.colorText = StatusList[Index(status)].colorText;
									break;
								case POPUP_COLOR_POPUP:
									ppd.colorBack = ppd.colorText = 0;
									break;
							}
				
							PUAddPopUpT(&ppd);
						}

						return FALSE;
					}
				}
			}

			if ((HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == CBN_SELCHANGE || HIWORD(wParam) == EN_CHANGE) && (HWND)lParam == GetFocus())
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);

			break;
		}
		case WM_NOTIFY: 
		{
			if (((LPNMHDR)lParam)->code == PSN_APPLY) 
			{
				char str[8];
				DWORD ctlColour = 0;
				for (int i = ID_STATUS_MIN; i <= ID_STATUS_MAX2 - 1; i++) 
				{
					ctlColour = SendDlgItemMessage(hwndDlg, (i+2000), CPM_GETCOLOUR, 0, 0);
					StatusList[Index(i)].colorBack = SendDlgItemMessage(hwndDlg, (i+2000), CPM_GETCOLOUR, 0, 0);
					wsprintfA(str, "%ibg", i);
					DBWriteContactSettingDword(0, MODULE, str, ctlColour);

					ctlColour = SendDlgItemMessage(hwndDlg, (i+1000), CPM_GETCOLOUR, 0, 0);
					StatusList[Index(i)].colorText = ctlColour;
					wsprintfA(str, "%itx", i);
					DBWriteContactSettingDword(0, MODULE, str, ctlColour);
				}

				if (IsDlgButtonChecked(hwndDlg, IDC_USEOWNCOLORS))
					opt.Colors = POPUP_COLOR_OWN;
				else if (IsDlgButtonChecked(hwndDlg, IDC_USEPOPUPCOLORS))
					opt.Colors = POPUP_COLOR_POPUP;
				else
					opt.Colors = POPUP_COLOR_WINDOWS;

				opt.ShowStatus = IsDlgButtonChecked(hwndDlg, IDC_SHOWSTATUS);
				opt.UseAlternativeText = IsDlgButtonChecked(hwndDlg, IDC_SHOWALTDESCS);
				opt.ReadAwayMsg = IsDlgButtonChecked(hwndDlg, IDC_READAWAYMSG);
				opt.ShowPreviousStatus = IsDlgButtonChecked(hwndDlg, IDC_SHOWPREVIOUSSTATUS);
				opt.ShowGroup = IsDlgButtonChecked(hwndDlg, IDC_SHOWGROUP);
				opt.PopupTimeout = GetDlgItemInt(hwndDlg, IDC_TIMEOUT_VALUE, 0, TRUE);
				opt.LeftClickAction = (BYTE)SendDlgItemMessage(hwndDlg, IDC_STATUS_LC, CB_GETCURSEL, 0, 0);
				opt.RightClickAction = (BYTE)SendDlgItemMessage(hwndDlg, IDC_STATUS_RC, CB_GETCURSEL, 0, 0);

				SaveOptions();
				return TRUE;
			}

			break;
		}
	} 

	return FALSE;
}

INT_PTR CALLBACK DlgProcAutoDisableOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	switch (msg) 
	{
		case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);
			CheckDlgButton(hwndDlg, IDC_CHK_PGLOBAL, opt.DisablePopupGlobally);
			CheckDlgButton(hwndDlg, IDC_CHK_SGLOBAL, opt.DisableSoundGlobally);
			CheckDlgButton(hwndDlg, IDC_CHK_ONLYGLOBAL, opt.OnlyGlobalChanges);

			char str[8];
			for (int i = ID_STATUS_MIN; i <= ID_STATUS_MAX; i++) 
			{
				wsprintfA(str, "p%d", i);
				CheckDlgButton(hwndDlg, i, DBGetContactSettingByte(0, MODULE, str, 0));
			}

			for (int i = ID_STATUS_MIN; i <= ID_STATUS_MAX; i++) 
			{
				wsprintfA(str, "s%d", i);
				CheckDlgButton(hwndDlg, (i+2000),DBGetContactSettingByte(NULL, MODULE, str, 0));
			}

			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_OK:
					char str[8];
					opt.DisablePopupGlobally = IsDlgButtonChecked(hwndDlg, IDC_CHK_PGLOBAL);
					opt.DisableSoundGlobally = IsDlgButtonChecked(hwndDlg, IDC_CHK_SGLOBAL);
					opt.OnlyGlobalChanges = IsDlgButtonChecked(hwndDlg, IDC_CHK_ONLYGLOBAL);

					for (int i = ID_STATUS_MIN; i <= ID_STATUS_MAX; i++)
					{
						wsprintfA(str, "p%d", i);
						DBWriteContactSettingByte(NULL, MODULE, str, IsDlgButtonChecked(hwndDlg, i));
					}
					for (int i = ID_STATUS_MIN; i <= ID_STATUS_MAX; i++) 
					{
						wsprintfA(str, "s%d", i);
						DBWriteContactSettingByte(NULL, MODULE, str, IsDlgButtonChecked(hwndDlg, i+2000));
					}

					SaveOptions();
					//Fall through
				case IDC_CANCEL:
					DestroyWindow(hwndDlg);
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

int ResetTemplatesToDefault(HWND hwndDlg)
{
	int result = MessageBox(hwndDlg, 
		TranslateT("Do you want to reset all templates to default?"), 
		TranslateT("Reset templates"), 
		MB_ICONQUESTION | MB_YESNO);
	if (result == IDYES)
	{
		SetDlgItemText(hwndDlg, IDC_ED_TDELIMITER, DEFAULT_POPUP_DELIMITER);
		SetDlgItemText(hwndDlg, IDC_ED_TCHANGE, DEFAULT_POPUP_NEW);
		SetDlgItemText(hwndDlg, IDC_ED_TCHANGEMSG, DEFAULT_POPUP_CHANGEMSG);
		SetDlgItemText(hwndDlg, IDC_ED_TREMOVE, DEFAULT_POPUP_REMOVE);
		SetDlgItemText(hwndDlg, IDC_POPUPTEXT, TranslateT(DEFAULT_POPUP_STATUSMESSAGE));
		CheckDlgButton(hwndDlg, IDC_CHK_XSTATUSCHANGE, 1);
		CheckDlgButton(hwndDlg, IDC_CHK_MSGCHANGE, 1);
		CheckDlgButton(hwndDlg, IDC_CHK_REMOVE, 1);
		EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TCHANGE), TRUE);
		EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TCHANGEMSG), TRUE);
		EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TREMOVE), TRUE);
	}

	return result;
}

INT_PTR CALLBACK DlgProcXPopupOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);

			SendDlgItemMessage(hwndDlg, IDC_ED_MSGLEN, EM_LIMITTEXT, 3, 0);
			SendDlgItemMessage(hwndDlg, IDC_UD_MSGLEN, UDM_SETRANGE, 0, MAKELONG(999, 1));

			CheckDlgButton(hwndDlg, IDC_CHK_DISABLEMUSIC, opt.PDisableForMusic);
			CheckDlgButton(hwndDlg, IDC_CHK_CUTMSG, opt.PTruncateMsg);
			SetDlgItemInt(hwndDlg, IDC_ED_MSGLEN, opt.PMsgLen, FALSE);

			// Templates
			CheckDlgButton(hwndDlg, IDC_CHK_XSTATUSCHANGE, templates.PopupFlags & NOTIFY_NEW_XSTATUS ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_CHK_MSGCHANGE, templates.PopupFlags & NOTIFY_NEW_MESSAGE ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_CHK_REMOVE, templates.PopupFlags & NOTIFY_REMOVE ? 1 : 0);

			SetDlgItemText(hwndDlg, IDC_ED_TDELIMITER, templates.PopupDelimiter);
			SetDlgItemText(hwndDlg, IDC_ED_TCHANGE, templates.PopupNewXstatus);
			SetDlgItemText(hwndDlg, IDC_ED_TCHANGEMSG, templates.PopupNewMsg);
			SetDlgItemText(hwndDlg, IDC_ED_TREMOVE, templates.PopupRemove);

			EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TCHANGE), templates.PopupFlags & NOTIFY_NEW_XSTATUS);
			EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TCHANGEMSG), templates.PopupFlags & NOTIFY_NEW_MESSAGE);
			EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TREMOVE), templates.PopupFlags & NOTIFY_REMOVE);

			// Buttons
			SendDlgItemMessage(hwndDlg, IDC_BT_VARIABLES, BUTTONADDTOOLTIP, (WPARAM)TranslateT("Show available variables"), BATF_TCHAR);
			HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_VARIABLES));
			SendDlgItemMessage(hwndDlg, IDC_BT_VARIABLES, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			DestroyIcon(hIcon);

			SendDlgItemMessage(hwndDlg, IDC_BT_RESET, BUTTONADDTOOLTIP, (WPARAM)TranslateT("Reset all templates to default"), BATF_TCHAR);
			hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_RESET));
			SendDlgItemMessage(hwndDlg, IDC_BT_RESET, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			DestroyIcon(hIcon);

			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (HIWORD(wParam))
			{
				case BN_CLICKED:
				{
					switch (LOWORD(wParam))
					{
						case IDC_CHK_CUTMSG:
							EnableWindow(GetDlgItem(hwndDlg, IDC_ED_MSGLEN), IsDlgButtonChecked(hwndDlg, IDC_CHK_CUTMSG));
							break;
						case IDC_CHK_XSTATUSCHANGE:
							EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TCHANGE), IsDlgButtonChecked(hwndDlg, IDC_CHK_XSTATUSCHANGE));
							break;
						case IDC_CHK_MSGCHANGE:
							EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TCHANGEMSG), IsDlgButtonChecked(hwndDlg, IDC_CHK_MSGCHANGE));
							break;
						case IDC_CHK_REMOVE:
							EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TREMOVE), IsDlgButtonChecked(hwndDlg, IDC_CHK_REMOVE));
							break;
						case IDC_BT_VARIABLES:
							MessageBox(hwndDlg, VARIABLES_HELP_TEXT, TranslateT("Variables"), MB_OK | MB_ICONINFORMATION);
							break;				
						case IDC_BT_RESET:
							if (ResetTemplatesToDefault(hwndDlg) == IDYES) break;
							else return FALSE;
					}

					if (LOWORD(wParam) != IDC_BT_VARIABLES)
						SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);

					break;
				} 
				case EN_CHANGE:
				{
					if ((HWND)lParam == GetFocus())
						SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
					break;
				}

			}

			return TRUE;
		}
		case WM_NOTIFY:
		{
			if (((LPNMHDR)lParam)->code == PSN_APPLY ) 
			{
				opt.PDisableForMusic = IsDlgButtonChecked(hwndDlg, IDC_CHK_DISABLEMUSIC);
				opt.PTruncateMsg = IsDlgButtonChecked(hwndDlg, IDC_CHK_CUTMSG);
				opt.PMsgLen = GetDlgItemInt(hwndDlg, IDC_ED_MSGLEN, 0, FALSE);

				templates.PopupFlags = 0;
				templates.PopupFlags |= (IsDlgButtonChecked(hwndDlg, IDC_CHK_XSTATUSCHANGE) ? NOTIFY_NEW_XSTATUS : 0) |
										(IsDlgButtonChecked(hwndDlg, IDC_CHK_MSGCHANGE) ? NOTIFY_NEW_MESSAGE : 0) |
										(IsDlgButtonChecked(hwndDlg, IDC_CHK_REMOVE) ? NOTIFY_REMOVE : 0);

				// Templates
				GetDlgItemText(hwndDlg, IDC_ED_TDELIMITER, templates.PopupDelimiter, SIZEOF(templates.PopupDelimiter));
				GetDlgItemText(hwndDlg, IDC_ED_TCHANGE, templates.PopupNewXstatus, SIZEOF(templates.PopupNewXstatus));
				GetDlgItemText(hwndDlg, IDC_ED_TCHANGEMSG, templates.PopupNewMsg, SIZEOF(templates.PopupNewMsg));
				GetDlgItemText(hwndDlg, IDC_ED_TREMOVE, templates.PopupRemove, SIZEOF(templates.PopupRemove));

				// Save options to db
				SaveOptions();
				SaveTemplates();
			}

			return TRUE;
		}
	}

	return FALSE;
}

bool IsSuitableProto( PROTOACCOUNT* pa )
{
	if ( pa == NULL )
		return false;

	if ( pa->bDynDisabled || !pa->bIsEnabled )
		return false;

	if ( CallProtoService( pa->szProtoName, PS_GETCAPS, PFLAGNUM_2, 0 ) == 0 )
		return false;

	return true;
}

INT_PTR CALLBACK DlgProcSMPopupOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);
			CheckDlgButton(hwndDlg, IDC_ONCONNECT, opt.PopupOnConnect);
			CheckDlgButton(hwndDlg, IDC_PUIGNOREREMOVE, opt.IgnoreEmpty);

			// Buttons
			SendDlgItemMessage(hwndDlg, IDC_BT_VARIABLES, BUTTONADDTOOLTIP, (WPARAM)TranslateT("Show available variables"), BATF_TCHAR);
			HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_VARIABLES));
			SendDlgItemMessage(hwndDlg, IDC_BT_VARIABLES, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			DestroyIcon(hIcon);

			SendDlgItemMessage(hwndDlg, IDC_BT_RESET, BUTTONADDTOOLTIP, (WPARAM)TranslateT("Reset all templates to default"), BATF_TCHAR);
			hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_RESET));
			SendDlgItemMessage(hwndDlg, IDC_BT_RESET, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			DestroyIcon(hIcon);

			// proto list
			HWND hList = GetDlgItem(hwndDlg, IDC_PROTOCOLLIST);
			SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
			LVCOLUMN lvCol = {0};
			lvCol.mask = LVCF_WIDTH|LVCF_TEXT;
			lvCol.pszText=TranslateT("Protocol");
			lvCol.cx = 118;
			ListView_InsertColumn(hList, 0, &lvCol);
			// fill the list
			LVITEM lvItem = {0};
			lvItem.mask=LVIF_TEXT|LVIF_PARAM;
			lvItem.iItem=0;
			lvItem.iSubItem=0;

			int count;
			PROTOACCOUNT** protos;
			ProtoEnumAccounts( &count, &protos );
			ProtoTamplates = li.List_Create(0, 10);

			for(int i=0;i<count;i++) {
				if ( !IsSuitableProto( protos[i] ))
					continue;

				UpdateListFlag = TRUE;
				lvItem.pszText = protos[i]->tszAccountName;
				lvItem.lParam = (LPARAM)protos[i]->szModuleName;
				PROTOTAMPLATE *prototamplate = (PROTOTAMPLATE *)mir_alloc(sizeof(PROTOTAMPLATE));
				prototamplate->ProtoName = protos[i]->tszAccountName;
				TCHAR protoname[MAX_PATH] = {0};
				mir_sntprintf(protoname, SIZEOF(protoname), _T("%s_TSMChange"), protos[i]->tszAccountName);
				char *szprotoname = mir_t2a(protoname);
				DBVARIANT dbVar = {0};
				DBGetContactSettingTString(NULL, MODULE, szprotoname, &dbVar);
				if (lstrcmp(dbVar.ptszVal, NULL) == 0)
				{
					DBFreeVariant(&dbVar);
					_tcsncpy(prototamplate->ProtoTamplate, TranslateT(DEFAULT_POPUP_STATUSMESSAGE), SIZEOF(prototamplate->ProtoTamplate));
				}
				else
				{
					_tcsncpy(prototamplate->ProtoTamplate, dbVar.ptszVal, SIZEOF(prototamplate->ProtoTamplate));
				}
				mir_free(szprotoname);
				ListView_InsertItem(hList,&lvItem);
				li.List_Insert(ProtoTamplates, prototamplate, ProtoTamplates->realCount);

				char dbSetting[128];
				mir_snprintf(dbSetting, SIZEOF(dbSetting), "%s_enabled", protos[i]->szModuleName);
				ListView_SetCheckState(hList, lvItem.iItem, DBGetContactSettingByte(NULL, MODULE, dbSetting, TRUE));
				lvItem.iItem++;
			}
			UpdateListFlag = FALSE;
			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (HIWORD(wParam))
			{
				case BN_CLICKED:
				{
					switch (LOWORD(wParam))
					{
						case IDC_BT_VARIABLES:
							MessageBox(0, VARIABLES_SM_HELP_TEXT, TranslateT("Variables"), MB_OK | MB_ICONINFORMATION);
							break;				

						case IDC_BT_RESET:
							if (ResetTemplatesToDefault(hwndDlg) == IDYES) break;
							else return FALSE;
					}

					if (LOWORD(wParam) != IDC_BT_VARIABLES)
						SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);

					break;
				} 
				case EN_CHANGE:
				{
					if ((HWND)lParam == GetFocus())
						SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
					break;
				}

			}
			return TRUE;
		}
		case WM_NOTIFY:
		{
			if (((NMHDR*)lParam)->idFrom == IDC_PROTOCOLLIST) {
				switch(((NMHDR*)lParam)->code) {
				case LVN_ITEMCHANGED:
					{
						NMLISTVIEW *nmlv = (NMLISTVIEW *)lParam;
						if (nmlv->uNewState == 3 && !UpdateListFlag)
						{
							HWND hList = GetDlgItem(hwndDlg,IDC_PROTOCOLLIST);
							PROTOTAMPLATE *prototamplate;
							if (ListView_GetHotItem(hList) != ListView_GetSelectionMark(hList))
							{
								prototamplate = (PROTOTAMPLATE *)ProtoTamplates->items[ListView_GetSelectionMark(hList)];
								GetDlgItemText(hwndDlg, IDC_POPUPTEXT, prototamplate->ProtoTamplate, MAX_PATH);
								li.List_Remove(ProtoTamplates, ListView_GetSelectionMark(hList));
								li.List_Insert(ProtoTamplates, prototamplate, ListView_GetSelectionMark(hList));

							}
							LastItem = ListView_GetHotItem(hList);
							prototamplate = (PROTOTAMPLATE *)ProtoTamplates->items[LastItem];
							SetDlgItemText(hwndDlg, IDC_POPUPTEXT, prototamplate->ProtoTamplate);
						}
						if ((nmlv->uNewState^nmlv->uOldState)&LVIS_STATEIMAGEMASK && !UpdateListFlag)
							SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
					}
					break;
			}	}
			if (((LPNMHDR)lParam)->code == PSN_APPLY ) 
			{

				opt.IgnoreEmpty = IsDlgButtonChecked(hwndDlg, IDC_PUIGNOREREMOVE);
				opt.PopupOnConnect = IsDlgButtonChecked(hwndDlg, IDC_ONCONNECT);

				// Templates
				PROTOTAMPLATE *prototamplate = (PROTOTAMPLATE *)ProtoTamplates->items[LastItem];
				GetDlgItemText(hwndDlg, IDC_POPUPTEXT, prototamplate->ProtoTamplate, MAX_PATH);
				li.List_Remove(ProtoTamplates, LastItem);
				li.List_Insert(ProtoTamplates, prototamplate, LastItem);

				// Save options to db
				SaveOptions();
				SaveTemplates();
				HWND hList = GetDlgItem(hwndDlg,IDC_PROTOCOLLIST);
				LVITEM lvItem = {0};
				lvItem.mask=LVIF_PARAM;
				for (int i=0;i<ListView_GetItemCount(hList);i++) {
					lvItem.iItem=i;
					lvItem.iSubItem=0;
					ListView_GetItem(hList, &lvItem);

					char dbSetting[128];
					mir_snprintf(dbSetting, SIZEOF(dbSetting), "%s_enabled", (char *)lvItem.lParam);
					DBWriteContactSettingByte(NULL, MODULE, dbSetting, (BYTE)ListView_GetCheckState(hList, lvItem.iItem));
				}
			}

			return TRUE;
		}

		case WM_DESTROY:
		{
			li.List_Destroy(ProtoTamplates);
			break;
		}

	}

	return FALSE;
}

void EnableLogControls(HWND hwndDlg, BOOL state)
{
	EnableWindow(GetDlgItem(hwndDlg, IDC_CHK_SAVETOHISTORY), state);
	EnableWindow(GetDlgItem(hwndDlg, IDC_CHK_PREVENTIDENTICAL), state);
	EnableWindow(GetDlgItem(hwndDlg, IDC_CHK_DISABLEMUSIC), state);
	EnableWindow(GetDlgItem(hwndDlg, IDC_CHK_CUTMSG), state);
	EnableWindow(GetDlgItem(hwndDlg, IDC_ED_MSGLEN), state & IsDlgButtonChecked(hwndDlg, IDC_CHK_CUTMSG));
	EnableWindow(GetDlgItem(hwndDlg, IDC_CHK_XSTATUSCHANGE), state);
	EnableWindow(GetDlgItem(hwndDlg, IDC_CHK_MSGCHANGE), state);
	EnableWindow(GetDlgItem(hwndDlg, IDC_CHK_REMOVE), state);
	EnableWindow(GetDlgItem(hwndDlg, IDC_CHK_OPENING), state);
	EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TDELIMITER), state);
	EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TCHANGE), state & IsDlgButtonChecked(hwndDlg, IDC_CHK_XSTATUSCHANGE));
	EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TCHANGEMSG), state & IsDlgButtonChecked(hwndDlg, IDC_CHK_MSGCHANGE));
	EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TREMOVE), state & IsDlgButtonChecked(hwndDlg, IDC_CHK_REMOVE));
	EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TOPENING), state & IsDlgButtonChecked(hwndDlg, IDC_CHK_OPENING));
	EnableWindow(GetDlgItem(hwndDlg, IDC_BT_VARIABLES), state);
	EnableWindow(GetDlgItem(hwndDlg, IDC_BT_RESET), state);	
}

INT_PTR CALLBACK DlgProcXLogOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);

			CheckDlgButton(hwndDlg, IDC_CHK_LOGGING, opt.EnableLogging);
			CheckDlgButton(hwndDlg, IDC_CHK_SAVETOHISTORY, opt.KeepInHistory);
			CheckDlgButton(hwndDlg, IDC_CHK_PREVENTIDENTICAL, opt.PreventIdentical);
			CheckDlgButton(hwndDlg, IDC_CHK_DISABLEMUSIC, opt.LDisableForMusic);
			CheckDlgButton(hwndDlg, IDC_CHK_CUTMSG, opt.LTruncateMsg);

			SendDlgItemMessage(hwndDlg, IDC_ED_MSGLEN, EM_LIMITTEXT, 3, 0);
			SendDlgItemMessage(hwndDlg, IDC_UD_MSGLEN, UDM_SETRANGE, 0, MAKELONG(999, 1));
			SetDlgItemInt(hwndDlg, IDC_ED_MSGLEN, opt.LMsgLen, FALSE);

			//Templates
			CheckDlgButton(hwndDlg, IDC_CHK_XSTATUSCHANGE, (templates.LogFlags & NOTIFY_NEW_XSTATUS) ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_CHK_MSGCHANGE, (templates.LogFlags & NOTIFY_NEW_MESSAGE) ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_CHK_REMOVE, (templates.LogFlags & NOTIFY_REMOVE) ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_CHK_OPENING, (templates.LogFlags & NOTIFY_OPENING_ML) ? 1 : 0);

			SetDlgItemText(hwndDlg, IDC_ED_TDELIMITER, templates.LogDelimiter);
			SetDlgItemText(hwndDlg, IDC_ED_TCHANGE, templates.LogNewXstatus);
			SetDlgItemText(hwndDlg, IDC_ED_TCHANGEMSG, templates.LogNewMsg);
			SetDlgItemText(hwndDlg, IDC_ED_TREMOVE, templates.LogRemove);
			SetDlgItemText(hwndDlg, IDC_ED_TOPENING, templates.LogOpening);

			// Buttons
			SendDlgItemMessage(hwndDlg, IDC_BT_VARIABLES, BUTTONADDTOOLTIP, (WPARAM)TranslateT("Show available variables"), BATF_TCHAR);
			HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_VARIABLES));
			SendDlgItemMessage(hwndDlg, IDC_BT_VARIABLES, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			DestroyIcon(hIcon);

			SendDlgItemMessage(hwndDlg, IDC_BT_RESET, BUTTONADDTOOLTIP, (WPARAM)TranslateT("Reset all templates to default"), BATF_TCHAR);
			hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_RESET));
			SendDlgItemMessage(hwndDlg, IDC_BT_RESET, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			DestroyIcon(hIcon);
			
			EnableLogControls(hwndDlg, opt.EnableLogging);

			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (HIWORD(wParam))
			{
				case BN_CLICKED:
				{
					switch (LOWORD(wParam))
					{
						case IDC_CHK_LOGGING:
							EnableLogControls(hwndDlg, IsDlgButtonChecked(hwndDlg, IDC_CHK_LOGGING) ? TRUE : FALSE);
							break;
						case IDC_CHK_CUTMSG:
							EnableWindow(GetDlgItem(hwndDlg, IDC_ED_MSGLEN), IsDlgButtonChecked(hwndDlg, IDC_CHK_CUTMSG));
							break;
						case IDC_CHK_XSTATUSCHANGE:
							EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TCHANGE), IsDlgButtonChecked(hwndDlg, IDC_CHK_XSTATUSCHANGE));
							break;
						case IDC_CHK_MSGCHANGE:
							EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TCHANGEMSG), IsDlgButtonChecked(hwndDlg, IDC_CHK_MSGCHANGE));
							break;
						case IDC_CHK_REMOVE:
							EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TREMOVE), IsDlgButtonChecked(hwndDlg, IDC_CHK_REMOVE));
							break;
						case IDC_CHK_OPENING:
							EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TOPENING), IsDlgButtonChecked(hwndDlg, IDC_CHK_OPENING));
							break;
						case IDC_BT_VARIABLES:
							MessageBox(hwndDlg, VARIABLES_HELP_TEXT, TranslateT("Variables"), MB_OK | MB_ICONINFORMATION);
							break;
						case IDC_BT_RESET:
							if (ResetTemplatesToDefault(hwndDlg) == IDYES)
							{
								SetDlgItemText(hwndDlg, IDC_ED_TOPENING, DEFAULT_LOG_OPENING);
								CheckDlgButton(hwndDlg, IDC_CHK_OPENING, 1);
								EnableWindow(GetDlgItem(hwndDlg, IDC_ED_TOPENING), TRUE);
							}
							else 
							{
								return FALSE;
							}

							break;
					}
					
					if (LOWORD(wParam) != IDC_BT_VARIABLES)
						SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
					
					break;
				}
				
				case EN_CHANGE:
				{
					if ((HWND)lParam == GetFocus())
						SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
					break;
				}
			} 
			return TRUE;
		}
		case WM_NOTIFY:
		{
			if (((LPNMHDR)lParam)->code == PSN_APPLY ) 
			{
				opt.EnableLogging = IsDlgButtonChecked(hwndDlg, IDC_CHK_LOGGING);
				opt.KeepInHistory = IsDlgButtonChecked(hwndDlg, IDC_CHK_SAVETOHISTORY);
				opt.PreventIdentical = IsDlgButtonChecked(hwndDlg, IDC_CHK_PREVENTIDENTICAL);
				opt.LDisableForMusic = IsDlgButtonChecked(hwndDlg, IDC_CHK_DISABLEMUSIC);
				opt.LTruncateMsg = IsDlgButtonChecked(hwndDlg, IDC_CHK_CUTMSG);
				opt.LMsgLen = GetDlgItemInt(hwndDlg, IDC_ED_MSGLEN, 0, FALSE);

				templates.LogFlags = 0;
				templates.LogFlags |= (IsDlgButtonChecked(hwndDlg, IDC_CHK_XSTATUSCHANGE) ? NOTIFY_NEW_XSTATUS : 0) |
									  (IsDlgButtonChecked(hwndDlg, IDC_CHK_MSGCHANGE) ? NOTIFY_NEW_MESSAGE : 0) |
									  (IsDlgButtonChecked(hwndDlg, IDC_CHK_REMOVE) ? NOTIFY_REMOVE : 0) |
									  (IsDlgButtonChecked(hwndDlg, IDC_CHK_OPENING) ? NOTIFY_OPENING_ML : 0);
	
				// Templates
				GetDlgItemText(hwndDlg, IDC_ED_TDELIMITER, templates.LogDelimiter, SIZEOF(templates.LogDelimiter));
				GetDlgItemText(hwndDlg, IDC_ED_TCHANGE, templates.LogNewXstatus, SIZEOF(templates.LogNewXstatus));
				GetDlgItemText(hwndDlg, IDC_ED_TCHANGEMSG, templates.LogNewMsg, SIZEOF(templates.LogNewMsg));
				GetDlgItemText(hwndDlg, IDC_ED_TREMOVE, templates.LogRemove, SIZEOF(templates.LogRemove));
				GetDlgItemText(hwndDlg, IDC_ED_TOPENING, templates.LogOpening, SIZEOF(templates.LogOpening));

				SaveOptions();
				SaveTemplates();
			}
			return TRUE;
		}
	}

	return FALSE;
}

int OptionsInitialize(WPARAM wParam, LPARAM lParam) 
{
	OPTIONSDIALOGPAGE odp = {0};

	odp.cbSize = sizeof(odp);
	odp.position = -100000000;
	odp.hInstance = hInst;
	odp.flags = ODPF_BOLDGROUPS | ODPF_TCHAR;
	odp.ptszTitle = LPGENT("Status Notify");
	odp.ptszGroup = LPGENT("Status");
	odp.ptszTab	= LPGENT("General");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_GENERAL);
	odp.pfnDlgProc = DlgProcGeneralOpts;
	CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);

	odp.ptszTab	= LPGENT("Filtering");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_FILTERS);
	odp.pfnDlgProc = DlgProcFiltering;
	CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);

	odp.ptszTab	= LPGENT("Message log");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_XLOG);
	odp.pfnDlgProc = DlgProcXLogOpts;	
	CallService(MS_OPT_ADDPAGE, wParam, (LPARAM)&odp);

	if (ServiceExists(MS_POPUP_ADDPOPUP)) 
	{
		odp.ptszTitle = LPGENT("Status Notify");
		odp.ptszGroup = LPGENT("Popups");
		odp.ptszTab = LPGENT("General");
		odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_POPUP);
		odp.pfnDlgProc = DlgProcPopUpOpts;
		CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);

		odp.ptszTab = LPGENT("Extra status");
		odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_XPOPUP);
		odp.pfnDlgProc = DlgProcXPopupOpts;	
		CallService(MS_OPT_ADDPAGE, wParam, (LPARAM)&odp);

		odp.ptszTab = LPGENT("Status message");
		odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_SMPOPUP);
		odp.pfnDlgProc = DlgProcSMPopupOpts;	
		CallService(MS_OPT_ADDPAGE, wParam, (LPARAM)&odp);
	}

	return 0;
}