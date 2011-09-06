/*
Traffic Counter plugin for Miranda IM 
Copyright 2007-2011 Mironych.

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

#include "commonheaders.h"

/*-------------------------------------------------------------------------------------------------------------------*/
//GLOBAL
/*-------------------------------------------------------------------------------------------------------------------*/
HINSTANCE hInst;
PLUGINLINK *pluginLink;

int hLangpack = 0; // ��������� ����������������� ��������.

TCHAR* TRAFFIC_COUNTER_WINDOW_CLASS = _T("TrafficCounterWnd");

/*-------------------------------------------------------------------------------------------------------------------*/
//TRAFFIC COUNTER
/*-------------------------------------------------------------------------------------------------------------------*/
OPTTREE_OPTION *pOptions; // ����� ���� ��������� ������ opttree.c ����� ��������� �� ������ �����.
WORD notify_send_size = 0;
WORD notify_recv_size = 0;
//
// ���� ������ � ����
COLORREF Traffic_BkColor,Traffic_FontColor;

//notify
int Traffic_PopupBkColor;
int Traffic_PopupFontColor;
char Traffic_Notify_time_value;
short int Traffic_Notify_size_value;
char Traffic_PopupTimeoutDefault;
char Traffic_PopupTimeoutValue;

unsigned short int Traffic_LineHeight;
//
TCHAR Traffic_CounterFormat[512];
TCHAR Traffic_TooltipFormat[512];
//
HANDLE Traffic_FrameID = NULL;

char Traffic_AdditionSpace;

HANDLE h_OptInit;
HANDLE h_ModulesLoaded;
HANDLE h_SystemShutDown;
HANDLE h_OnRecv, h_OnSend;
HANDLE h_OnAccListChange;

HFONT Traffic_h_font = NULL;
HMENU TrafficPopupMenu = NULL;

/*-------------------------------------------------------------------------------------------------------------------*/
//TIME COUNTER
/*-------------------------------------------------------------------------------------------------------------------*/
static HANDLE h_AckHook;
BYTE online_count = 0;

/*-------------------------------------------------------------------------------------------------------------------*/
//font service support
/*-------------------------------------------------------------------------------------------------------------------*/
int TrafficFontHeight = 0;
FontIDT TrafficFontID;
ColourIDT TrafficBackgroundColorID;

//---------------------------------------------------------------------------------------------
// ��� ToolTip
//---------------------------------------------------------------------------------------------
BOOL TooltipShowing;
POINT TooltipPosition;

// ��������������� ���������� ����� ��������� �������� �������� ����
BOOL UseKeyColor;
COLORREF KeyColor;

#ifdef UNICODE
// {82181510-5DFA-49d7-B469-33871E2AE8B5}
#define MIID_TRAFFICCOUNTER {0x82181510, 0x5dfa, 0x49d7, { 0xb4, 0x69, 0x33, 0x87, 0x1e, 0x2a, 0xe8, 0xb5}}
#else
// {EDC2B29C-30F5-4b0c-9F3D-27F504B907B6}
#define MIID_TRAFFICCOUNTER {0xedc2b29c, 0x30f5, 0x4b0c, { 0x9f, 0x3d, 0x27, 0xf5, 0x4, 0xb9, 0x7, 0xb6}}
#endif

PLUGININFOEX pluginInfoEx = 
{
	sizeof(PLUGININFOEX),
	"Traffic Counter",
	PLUGIN_MAKE_VERSION(0, 1, 1, 7),
	"Adding traffic and time counters",
	"Ghost, Mironych",
	"",
	"� 2002-2006 Ghost, � 2007-2011 Mironych ["__DATE__" "__TIME__"]",
	"",
	UNICODE_AWARE,
	0,
	MIID_TRAFFICCOUNTER
};

__declspec(dllexport) PLUGININFOEX *MirandaPluginInfoEx(DWORD mirandaVersion)
{
	if (PLUGIN_MAKE_VERSION(0, 8, 0, 11) > (mirandaVer = mirandaVersion))
        return NULL;
    return &pluginInfoEx;
}

static const MUUID interfaces[] = {MIID_TRAFFICCOUNTER, MIID_LAST};

__declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			hInst = hinstDLL;
			DisableThreadLibraryCalls(hInst);
			break;

		case DLL_PROCESS_DETACH:
			PostMessage(TrafficHwnd,WM_USER+697,0,666);
			//
			break;
	}
	//
	return TRUE;
}

int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink = link;

	if (mirandaVer < PLUGIN_MAKE_VERSION(0, 8, 0, 11)) return -1;
	
	// set the memory manager
	memoryManagerInterface.cbSize = sizeof(struct MM_INTERFACE);
	CallService(MS_SYSTEM_GET_MMI,0,(LPARAM)&memoryManagerInterface);
	
	// �������� ���������� ��������� ������.
	mir_getLP(&pluginInfoEx);
	
	h_OptInit = HookEvent(ME_OPT_INITIALISE,TrafficCounterOptInitialise);
	h_ModulesLoaded = HookEvent(ME_SYSTEM_MODULESLOADED,TrafficCounterModulesLoaded);
	h_AckHook = HookEvent(ME_PROTO_ACK,ProtocolAckHook);
	h_OnAccListChange = HookEvent(ME_PROTO_ACCLISTCHANGED, OnAccountsListChange);
	h_SystemShutDown = HookEvent(ME_SYSTEM_SHUTDOWN,TrafficCounterShutdown);
	
	return 0;
}

int __declspec(dllexport) Unload(void)
{
	return 1;
}

int TrafficCounterShutdown(WPARAM wParam,LPARAM lParam)
{
	SaveSettings(0);

	if (Traffic_h_font) 
		DeleteObject(Traffic_h_font);
	if( (ServiceExists(MS_CLIST_FRAMES_REMOVEFRAME)) && (!Traffic_FrameID) )
		CallService(MS_CLIST_FRAMES_REMOVEFRAME, (WPARAM)Traffic_FrameID, 0);

	DestroyServiceFunction(MenuCommand_TrafficShowHide);
	
	if (TrafficHwnd)
		DestroyWindow(TrafficHwnd);

	UnhookEvent(h_OnAccListChange);
	UnhookEvent(h_AckHook);
	UnhookEvent(h_ModulesLoaded);
	UnhookEvent(h_OnRecv);
	UnhookEvent(h_OnSend);
	UnhookEvent(h_OptInit);
	UnhookEvent(h_SystemShutDown);

	DestroyProtocolList();

	if (TrafficPopupMenu) DestroyMenu(TrafficPopupMenu);

	return 0;
}

int TrafficCounterModulesLoaded(WPARAM wParam,LPARAM lParam)
{
	DBVARIANT dbv;

	CreateProtocolList();

	// ������ �����
	unOptions.Flags = DBGetContactSettingDword(NULL, TRAFFIC_SETTINGS_GROUP, SETTINGS_WHAT_DRAW, 0x0882);
	Stat_SelAcc = DBGetContactSettingWord(NULL, TRAFFIC_SETTINGS_GROUP, SETTINGS_STAT_ACC_OPT, 0x01);

	//settings for notification
	Traffic_PopupBkColor = DBGetContactSettingDword(NULL,TRAFFIC_SETTINGS_GROUP,SETTINGS_POPUP_BKCOLOR,RGB(200,255,200));
	Traffic_PopupFontColor = DBGetContactSettingDword(NULL,TRAFFIC_SETTINGS_GROUP,SETTINGS_POPUP_FONTCOLOR,RGB(0,0,0));
	//
	Traffic_Notify_time_value = DBGetContactSettingByte(NULL,TRAFFIC_SETTINGS_GROUP,SETTINGS_POPUP_NOTIFY_TIME_VALUE,10);
	//
	Traffic_Notify_size_value = DBGetContactSettingWord(NULL,TRAFFIC_SETTINGS_GROUP,SETTINGS_POPUP_NOTIFY_SIZE_VALUE,100);
	//
	//popup timeout
	Traffic_PopupTimeoutDefault = DBGetContactSettingByte(NULL,TRAFFIC_SETTINGS_GROUP,SETTINGS_POPUP_TIMEOUT_DEFAULT,1);
	Traffic_PopupTimeoutValue = DBGetContactSettingByte(NULL,TRAFFIC_SETTINGS_GROUP,SETTINGS_POPUP_TIMEOUT_VALUE,5);
	
	// ������ �������� ��� ������� ��������� ���������
	if (DBGetContactSettingTString(NULL, TRAFFIC_SETTINGS_GROUP, SETTINGS_COUNTER_FORMAT, &dbv) == 0)
	{
		if(lstrlen(dbv.ptszVal) > 0)
			lstrcpyn(Traffic_CounterFormat, dbv.ptszVal, SIZEOF(Traffic_CounterFormat));
		//
		DBFreeVariant(&dbv);
	}
	else //defaults here
	{
		_tcscpy(Traffic_CounterFormat, _T("{I4}\x0D\x0A\x0A\
{R65}?tc_GetTraffic(%extratext%,now,sent,d)\x0D\x0A\x0A\
{R115}?tc_GetTraffic(%extratext%,now,recieved,d)\x0D\x0A\x0A\
{R165}?tc_GetTraffic(%extratext%,total,both,d)\x0D\x0A\x0A\
{L180}?if3(?tc_GetTime(%extratext%,now,hh:mm:ss),)\x0D\x0A\x0A\
{L230}?if3(?tc_GetTime(%extratext%,total,d hh:mm),)"));
	}

	// ������ ����������� ���������
	if (DBGetContactSettingTString(NULL, TRAFFIC_SETTINGS_GROUP, SETTINGS_TOOLTIP_FORMAT, &dbv) == 0)
	{
		if(lstrlen(dbv.ptszVal) > 0)
			lstrcpyn(Traffic_TooltipFormat, dbv.ptszVal, SIZEOF(Traffic_TooltipFormat));
		//
		DBFreeVariant(&dbv);
	}
	else //defaults here
	{
		_tcscpy(Traffic_TooltipFormat, _T("Traffic Counter"));
	}

	Traffic_AdditionSpace = DBGetContactSettingByte(NULL, TRAFFIC_SETTINGS_GROUP, SETTINGS_ADDITION_SPACE, 0);

	// ������� ������� �������
	OverallInfo.Total.Timer = DBGetContactSettingDword(NULL, TRAFFIC_SETTINGS_GROUP, SETTINGS_TOTAL_ONLINE_TIME, 0);

	//register traffic font
	TrafficFontID.cbSize = sizeof(FontIDT);
	_tcscpy(TrafficFontID.group, LPGENT("Traffic counter"));
	_tcscpy(TrafficFontID.name, LPGENT("Font"));
	strcpy(TrafficFontID.dbSettingsGroup, TRAFFIC_SETTINGS_GROUP);
	strcpy(TrafficFontID.prefix, "Font");
	TrafficFontID.flags = FIDF_DEFAULTVALID | FIDF_SAVEPOINTSIZE;
	TrafficFontID.deffontsettings.charset = DEFAULT_CHARSET;
	TrafficFontID.deffontsettings.colour = GetSysColor(COLOR_BTNTEXT);
	TrafficFontID.deffontsettings.size = 12;
	TrafficFontID.deffontsettings.style = 0;
	_tcscpy(TrafficFontID.deffontsettings.szFace, _T("Arial"));
	TrafficFontID.order = 0;
	CallService(MS_FONT_REGISTERT, (WPARAM)&TrafficFontID, 0);

	// ������������ ���� ����
	TrafficBackgroundColorID.cbSize = sizeof(ColourIDT);
	_tcscpy(TrafficBackgroundColorID.group, LPGENT("Traffic counter"));
	_tcscpy(TrafficBackgroundColorID.name, LPGENT("Font"));
	strcpy(TrafficBackgroundColorID.dbSettingsGroup, TRAFFIC_SETTINGS_GROUP);
	strcpy(TrafficBackgroundColorID.setting, "FontBkColor");
	TrafficBackgroundColorID.defcolour = GetSysColor(COLOR_BTNFACE);
	CallService(MS_COLOUR_REGISTERT, (WPARAM)&TrafficBackgroundColorID, 0);
	HookEvent(ME_FONT_RELOAD, UpdateFonts);

	// ��������� ��������� ������� Variables
	RegisterVariablesTokens();

	CreateServiceFunction("TrafficCounter/ShowHide", MenuCommand_TrafficShowHide);
	// ������������ ������� ������� ��� ������/������� ������
	{
		HOTKEYDESC hkd = {0};
		hkd.cbSize = sizeof(hkd);
		hkd.DefHotKey = HOTKEYCODE(HOTKEYF_CONTROL|HOTKEYF_SHIFT,'T');
		hkd.pszSection = "Traffic Counter";
		hkd.pszDescription = "Show/Hide frame";
		hkd.pszName = "TC_Show_Hide";
		hkd.pszService = "TrafficCounter/ShowHide";
		CallService(MS_HOTKEY_REGISTER, 0, (LPARAM)&hkd);
	}
	
	CreateTrafficWindow((HWND)CallService(MS_CLUI_GETHWND, 0, 0));
	UpdateFonts(0, 0);	//Load and create fonts here
	
	// �������� ����
	UseKeyColor = DBGetContactSettingByte(NULL, "ModernSettings", "UseKeyColor", 1);
	KeyColor = DBGetContactSettingDword(NULL, "ModernSettings", "KeyColor", 0);

	// ������������ ����������� ������� Netlib
	h_OnRecv = HookEvent(ME_NETLIB_FASTRECV, TrafficRecv);
	h_OnSend = HookEvent(ME_NETLIB_FASTSEND, TrafficSend);

	return 0;
}

static BOOL CALLBACK DlgProcPopupsTraffic(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
			TranslateDialogDefault(hwndDlg);
			//
			CheckDlgButton(hwndDlg,IDC_NOTIFYSIZE,(unOptions.NotifyBySize) ? BST_CHECKED : BST_UNCHECKED);
			SetDlgItemInt(hwndDlg,IDC_ENOTIFYSIZE,Traffic_Notify_size_value,0);
			SendDlgItemMessage(hwndDlg,IDC_ENOTIFYSIZE,EM_LIMITTEXT,4,0);
			if (!unOptions.NotifyBySize) EnableWindow(GetDlgItem(hwndDlg,IDC_ENOTIFYSIZE),0);
			CheckDlgButton(hwndDlg,IDC_NOTIFYTIME,(unOptions.NotifyByTime) ? BST_CHECKED : BST_UNCHECKED);
			SetDlgItemInt(hwndDlg,IDC_ENOTIFYTIME,Traffic_Notify_time_value,0);
			SendDlgItemMessage(hwndDlg,IDC_ENOTIFYTIME,EM_LIMITTEXT,2,0);
			if (!unOptions.NotifyByTime) EnableWindow(GetDlgItem(hwndDlg,IDC_ENOTIFYTIME),0);
			//colors
			SendDlgItemMessage(hwndDlg,IDC_COLOR1,CPM_SETDEFAULTCOLOUR,0,RGB(200,255,200));
			SendDlgItemMessage(hwndDlg,IDC_COLOR1,CPM_SETCOLOUR,0,Traffic_PopupBkColor);
			SendDlgItemMessage(hwndDlg,IDC_COLOR2,CPM_SETDEFAULTCOLOUR,0,RGB(0,0,0));
			SendDlgItemMessage(hwndDlg,IDC_COLOR2,CPM_SETCOLOUR,0,Traffic_PopupFontColor);
			//timeout
			CheckDlgButton(hwndDlg,IDC_RADIO_FROMPOPUP,(Traffic_PopupTimeoutDefault != 0) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hwndDlg,IDC_RADIO_CUSTOM,(Traffic_PopupTimeoutDefault == 0) ? BST_CHECKED : BST_UNCHECKED);
			EnableWindow(GetDlgItem(hwndDlg,IDC_POPUP_TIMEOUT),Traffic_PopupTimeoutDefault == 0);
			SetDlgItemInt(hwndDlg,IDC_POPUP_TIMEOUT,Traffic_PopupTimeoutValue,0);
			SendDlgItemMessage(hwndDlg,IDC_POPUP_TIMEOUT,EM_LIMITTEXT,2,0);

			return 0;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_NOTIFYSIZE:
					EnableWindow(GetDlgItem(hwndDlg,IDC_ENOTIFYSIZE),IsDlgButtonChecked(hwndDlg,IDC_NOTIFYSIZE));
					break;

				case IDC_ENOTIFYSIZE:
					if (HIWORD(wParam)!=EN_CHANGE || (HWND)lParam!=GetFocus()) return 0;
					break;

				case IDC_NOTIFYTIME:
					EnableWindow(GetDlgItem(hwndDlg,IDC_ENOTIFYTIME),IsDlgButtonChecked(hwndDlg,IDC_NOTIFYTIME));
					break;

				case IDC_ENOTIFYTIME:
					if (HIWORD(wParam)!=EN_CHANGE || (HWND)lParam!=GetFocus()) return 0;
					break;

				case IDC_RESETCOLORS:
					SendDlgItemMessage(hwndDlg,IDC_COLOR1,CPM_SETDEFAULTCOLOUR,0,RGB(200,255,200));
					SendDlgItemMessage(hwndDlg,IDC_COLOR1,CPM_SETCOLOUR,0,RGB(200,255,200));
					SendDlgItemMessage(hwndDlg,IDC_COLOR2,CPM_SETDEFAULTCOLOUR,0,RGB(0,0,0));
					SendDlgItemMessage(hwndDlg,IDC_COLOR2,CPM_SETCOLOUR,0,RGB(0,0,0));
					break;

				case IDC_RADIO_FROMPOPUP:
				case IDC_RADIO_CUSTOM:
					EnableWindow(GetDlgItem(hwndDlg,IDC_POPUP_TIMEOUT),IsDlgButtonChecked(hwndDlg,IDC_RADIO_CUSTOM));
					break;

				case IDC_TEST:
					NotifyOnRecv();
					NotifyOnSend();
					return 0;

			}
			SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			return 0;

		case WM_NOTIFY:
			switch(((LPNMHDR)lParam)->idFrom)
			{
				case 0:
					if (((LPNMHDR)lParam)->code == PSN_APPLY ) {
						unOptions.NotifyBySize = IsDlgButtonChecked(hwndDlg,IDC_NOTIFYSIZE);
						Traffic_Notify_size_value = GetDlgItemInt(hwndDlg,IDC_ENOTIFYSIZE,NULL,0);
						unOptions.NotifyByTime = IsDlgButtonChecked(hwndDlg,IDC_NOTIFYTIME);
						Traffic_Notify_time_value = GetDlgItemInt(hwndDlg,IDC_ENOTIFYTIME,NULL,0);
						//
						Traffic_PopupBkColor = SendDlgItemMessage(hwndDlg,IDC_COLOR1,CPM_GETCOLOUR,0,0);
						Traffic_PopupFontColor = SendDlgItemMessage(hwndDlg,IDC_COLOR2,CPM_GETCOLOUR,0,0);
						//
						Traffic_PopupTimeoutDefault = IsDlgButtonChecked(hwndDlg,IDC_RADIO_FROMPOPUP);
						if (Traffic_PopupTimeoutDefault == 0) Traffic_PopupTimeoutValue = GetDlgItemInt(hwndDlg,IDC_POPUP_TIMEOUT,NULL,0);
						//
						UpdateNotifyTimer();
						return TRUE;
					}
					break;
			}
			break;
	}
	return 0;
}

static BOOL CALLBACK DlgProcTCOptions(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WORD i, j, l;
	BOOL result;
	static BYTE Initialized = 0;
	static WORD optionCount = 0;
	static OPTTREE_OPTION options[] =
	{
		{0,	LPGENT("Display/") LPGENT("Icon"),
				OPTTREE_CHECK,	OPTTREE_INVISIBLE,	NULL,	"DrawProtoIcon"},
		{0,	LPGENT("Display/") LPGENT("Account name"),
				OPTTREE_CHECK,	OPTTREE_INVISIBLE,	NULL,	"DrawProtoName"},
		{0,	LPGENT("Display/") LPGENT("Current traffic"),
				OPTTREE_CHECK,	OPTTREE_INVISIBLE,	NULL,	"DrawCurrentTraffic"},
		{0,	LPGENT("Display/") LPGENT("Total traffic"),
				OPTTREE_CHECK,	OPTTREE_INVISIBLE,	NULL,	"DrawTotalTraffic"},
		{0,	LPGENT("Display/") LPGENT("Current online"),
				OPTTREE_CHECK,	OPTTREE_INVISIBLE,	NULL,	"DrawCurrentTime"},
		{0,	LPGENT("Display/") LPGENT("Total online"),
				OPTTREE_CHECK,	OPTTREE_INVISIBLE,	NULL,	"DrawTotalTime"},
		{0,	LPGENT("General/") LPGENT("Draw frame as skin element"),
				OPTTREE_CHECK,	1,	NULL,	"DrawFrmAsSkin"},
		{0,	LPGENT("General/") LPGENT("Show tooltip in traffic window"),
				OPTTREE_CHECK,	1,	NULL,	"ShowTooltip"},
		{0,	LPGENT("General/") LPGENT("\"Toggle traffic counter\" in main menu"),
				OPTTREE_CHECK,	1,	NULL,	"ShowMainMenuItem"},
		// ����������� ����� ��� �������� � ������� ���������
		// �������� 16 ������� ������� � 16 ��������
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	NULL, OPTTREE_CHECK, OPTTREE_INVISIBLE,	NULL, NULL},
		{0,	LPGENT("Visible accounts/") LPGENT("Summary traffic for visible accounts"),
				OPTTREE_CHECK,	1,	NULL,	"ShowSummary"},
		{0,	LPGENT("Visible accounts/") LPGENT("Overall traffic"),
				OPTTREE_CHECK,	1,	NULL,	"ShowOverall"},
	};

	if (!Initialized)
	{
		pOptions = options;
		optionCount = SIZEOF(options);
		// ���� ��� Variables, ���������� ������� ��� ������� ������ ���������
		if (!ServiceExists(MS_VARS_FORMATSTRING))
		{
			for (i = 0; i < 8; i++)	options[i].dwFlag = 1;
		}

		// ������ ��� ��������� ��������� ��������� � ����� ������.
		for (i = j = 0; (j < NumberOfAccounts) && (i < optionCount) ; i++)
			if ((options[i].dwFlag & OPTTREE_INVISIBLE) && !options[i].szSettingName)
			{
				options[i].szSettingName = (char*)mir_alloc(1 + strlen(ProtoList[j].name));
				strcpy(options[i].szSettingName, ProtoList[j].name);
				l = 20 + _tcslen(ProtoList[j].tszAccountName);
				options[i].szOptionName = (TCHAR*)mir_alloc(sizeof(TCHAR) * l);
				mir_sntprintf(options[i].szOptionName,
								l,
								_T("Visible accounts/%s"),
								ProtoList[j].tszAccountName);
				options[i].dwFlag =   (ProtoList[j++].Enabled ? 1 : OPTTREE_INVISIBLE)
									| OPTTREE_NOTRANSLATE;
			}
		Initialized = 1;
	}

	if (OptTree_ProcessMessage(hwndDlg, msg, wParam, lParam, &result, IDC_APPEARANCEOPTIONS, options, optionCount))
		return result;

	switch (msg)
	{	
		case WM_INITDIALOG:
			TranslateDialogDefault(hwndDlg);
			
			SendDlgItemMessage(hwndDlg, IDC_SPIN1, UDM_SETRANGE, 0, MAKELONG(15, 0));
			SendDlgItemMessage(hwndDlg, IDC_SPIN1, UDM_SETPOS, 0, GetDlgItemInt(hwndDlg, IDC_EDIT_SPACE, NULL, FALSE));

			//show/hide button
			SetDlgItemText(hwndDlg,IDC_BSHOWHIDE,(IsWindowVisible(TrafficHwnd) != 0)? TranslateT("Hide now") : TranslateT("Show now"));

			// ������ ������� ��� ���������
			EnableWindow(GetDlgItem(hwndDlg,IDC_EDIT_COUNTER_FORMAT),ServiceExists(MS_VARS_FORMATSTRING));
			SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_COUNTER_FORMAT), Traffic_CounterFormat);

			// ������ ����������� ���������
			EnableWindow(GetDlgItem(hwndDlg,IDC_EDIT_TOOLTIP_FORMAT),
				ServiceExists("mToolTip/ShowTipW") || ServiceExists("mToolTip/ShowTip"));
			SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_TOOLTIP_FORMAT), Traffic_TooltipFormat);

			// Display traffic for current...
			SendDlgItemMessage(hwndDlg, IDC_COMBO_AUTO_CLEAR, CB_INSERTSTRING, -1, (LPARAM)TranslateT("Day"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_AUTO_CLEAR, CB_INSERTSTRING, -1, (LPARAM)TranslateT("Week"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_AUTO_CLEAR, CB_INSERTSTRING, -1, (LPARAM)TranslateT("Month"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_AUTO_CLEAR, CB_INSERTSTRING, -1, (LPARAM)TranslateT("Year"));
			SendDlgItemMessage(hwndDlg, IDC_COMBO_AUTO_CLEAR, CB_SETCURSEL, unOptions.PeriodForShow, 0);
			
			// �������� ����� ��������
			SetDlgItemInt(hwndDlg, IDC_EDIT_SPACE, Traffic_AdditionSpace, 0);
			SendDlgItemMessage(hwndDlg, IDC_EDIT_SPACE, EM_LIMITTEXT, 2, 0);

			// Appearance
			OptTree_SetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, unOptions.DrawProtoIcon, "DrawProtoIcon");
			OptTree_SetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, unOptions.DrawProtoName, "DrawProtoName");
			OptTree_SetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, unOptions.DrawCurrentTraffic, "DrawCurrentTraffic");
			OptTree_SetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, unOptions.DrawTotalTraffic, "DrawTotalTraffic");
			OptTree_SetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, unOptions.DrawCurrentTimeCounter, "DrawCurrentTime");
			OptTree_SetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, unOptions.DrawTotalTimeCounter, "DrawTotalTime");
			OptTree_SetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, unOptions.DrawFrmAsSkin, "DrawFrmAsSkin");
			OptTree_SetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, unOptions.ShowSummary , "ShowSummary");
			OptTree_SetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, unOptions.ShowTooltip, "ShowTooltip");
			OptTree_SetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, unOptions.ShowMainMenuItem, "ShowMainMenuItem");
			
			// ��������� ��������� ����������
			for (i = 0; i < NumberOfAccounts; i++)
			{
				char buffer[32];
				strcpy(buffer, ProtoList[i].name);
				OptTree_SetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, ProtoList[i].Visible, buffer);
			}
			OptTree_SetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, unOptions.ShowOverall, "ShowOverall");

			EnableWindow(GetDlgItem(GetParent(hwndDlg),IDC_APPLY),FALSE);
			return 0;
			
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_BSHOWHIDE:
					MenuCommand_TrafficShowHide(0, 0);
					SetDlgItemText(hwndDlg,IDC_BSHOWHIDE,IsWindowVisible(TrafficHwnd)? TranslateT("Hide now") : TranslateT("Show now"));
					return 0;
			
				case IDC_EDIT_SPACE:
				case IDC_EDIT_COUNTER_FORMAT:
				case IDC_EDIT_OVERALL_COUNTER_FORMAT:
				case IDC_EDIT_TOOLTIP_FORMAT:
					if (HIWORD(wParam) == EN_CHANGE)
						SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
					break;

				case IDC_COMBO_AUTO_CLEAR:
					if (HIWORD(wParam) == CBN_SELCHANGE)
						SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
					break;
			}
			break;
		}

		case WM_NOTIFY:
		{
			LPNMHDR lpnmhdr = (LPNMHDR)lParam;

			switch (lpnmhdr->code)
			{
				case PSN_APPLY:
				{
					unOptions.PeriodForShow = (char)SendDlgItemMessage(hwndDlg,IDC_COMBO_AUTO_CLEAR,CB_GETCURSEL,0,0);
							
					// �������� ����� ��������
					Traffic_AdditionSpace = GetDlgItemInt(hwndDlg, IDC_EDIT_SPACE, NULL, 0);
					// ��������� Appearance
					unOptions.DrawProtoIcon = OptTree_GetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, "DrawProtoIcon");
					unOptions.DrawProtoName = OptTree_GetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, "DrawProtoName");
					unOptions.DrawCurrentTraffic = OptTree_GetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, "DrawCurrentTraffic");
					unOptions.DrawTotalTraffic = OptTree_GetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, "DrawTotalTraffic");
					unOptions.DrawTotalTimeCounter = OptTree_GetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, "DrawTotalTime");
					unOptions.DrawCurrentTimeCounter = OptTree_GetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, "DrawCurrentTime");
					unOptions.DrawTotalTimeCounter = OptTree_GetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, "DrawTotalTime");
					unOptions.DrawFrmAsSkin = OptTree_GetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, "DrawFrmAsSkin");
					unOptions.ShowSummary = OptTree_GetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, "ShowSummary");
					unOptions.ShowTooltip = OptTree_GetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, "ShowTooltip");
					unOptions.ShowMainMenuItem = OptTree_GetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, "ShowMainMenuItem");

					// ��������� �������� ����������
					for (i = 0; i < NumberOfAccounts; i++)
					{
						char buffer[32];
						strcpy(buffer, ProtoList[i].name);
						ProtoList[i].Visible = (BYTE)OptTree_GetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, buffer);
					}
					unOptions.ShowOverall = OptTree_GetOptions(hwndDlg, IDC_APPEARANCEOPTIONS, options, optionCount, "ShowOverall");

					// ������ ���������
					GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_COUNTER_FORMAT), Traffic_CounterFormat, 512);
					// ������ ����������� ���������
					GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_TOOLTIP_FORMAT), Traffic_TooltipFormat, 512);

					// �������� ����
					UseKeyColor = DBGetContactSettingByte(NULL, "ModernSettings", "UseKeyColor", 1);
					KeyColor = DBGetContactSettingDword(NULL, "ModernSettings", "KeyColor", 0);

					// �������������� �����
					UpdateTrafficWindowSize();
							
					SaveSettings(0);
					//
					return TRUE;
				} // case PSN_APPLY
			} // switch
		}

		case WM_DESTROY:
			return FALSE;
	}
	return 0;
}

int TrafficCounterOptInitialise(WPARAM wParam,LPARAM lParam)
{
	OPTIONSDIALOGPAGE odp={0};
	// Main options page
	odp.cbSize=sizeof(odp);
	odp.position=900000000;
	odp.groupPosition = 1;
	odp.hInstance=hInst;
	odp.pszTemplate=MAKEINTRESOURCEA(IDD_OPT_TRAFFIC);
	odp.pszGroup = LPGEN("Services");
	odp.pszTab = LPGEN("Options");
	odp.pszTitle = LPGEN("Traffic counter");
	odp.pfnDlgProc=DlgProcTCOptions;
	odp.flags=ODPF_BOLDGROUPS;
	odp.expertOnlyControls=0;
	odp.nExpertOnlyControls=0;
	CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);

	// Statistics options page
	odp.pszTemplate=MAKEINTRESOURCEA(IDD_OPT_STATS);
	odp.pszTab = LPGEN("Statistics");
	odp.pfnDlgProc = DlgProcOptStatistics;
	CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);
	
	// Popups option page
	if (ServiceExists(MS_POPUP_ADDPOPUP))
	{
		odp.groupPosition = 100;
		odp.pszTemplate=MAKEINTRESOURCEA(IDD_OPT_TRAFFIC_POPUPS);
		odp.pszGroup=LPGEN("PopUps");
		odp.pszTitle=LPGEN("Traffic counter");
		odp.pfnDlgProc=DlgProcPopupsTraffic;
		odp.flags=ODPF_BOLDGROUPS;
		CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);
	}

	return 0;
}

void SaveSettings(BYTE OnlyCnt)
{
	unsigned short int i;

	// ��������� ������� ������� �������
	DBWriteContactSettingDword(NULL, TRAFFIC_SETTINGS_GROUP, SETTINGS_TOTAL_ONLINE_TIME, OverallInfo.Total.Timer);

	if (OnlyCnt) return;

	// ��� ������� ��������� ��������� �����
	for (i = 0; i < NumberOfAccounts; i++)
	{
		if (!ProtoList[i].name) continue;
		DBWriteContactSettingByte(NULL, ProtoList[i].name, SETTINGS_PROTO_FLAGS, ProtoList[i].Flags);
	}

	//settings for notification
	DBWriteContactSettingDword(NULL,TRAFFIC_SETTINGS_GROUP,SETTINGS_POPUP_BKCOLOR,Traffic_PopupBkColor);
	DBWriteContactSettingDword(NULL,TRAFFIC_SETTINGS_GROUP,SETTINGS_POPUP_FONTCOLOR,Traffic_PopupFontColor);
	//
	DBWriteContactSettingByte(NULL,TRAFFIC_SETTINGS_GROUP,SETTINGS_POPUP_NOTIFY_TIME_VALUE,Traffic_Notify_time_value);
	//
	DBWriteContactSettingWord(NULL,TRAFFIC_SETTINGS_GROUP,SETTINGS_POPUP_NOTIFY_SIZE_VALUE,Traffic_Notify_size_value);
	//
	//popup timeout
	DBWriteContactSettingByte(NULL,TRAFFIC_SETTINGS_GROUP,SETTINGS_POPUP_TIMEOUT_DEFAULT,Traffic_PopupTimeoutDefault);
	DBWriteContactSettingByte(NULL,TRAFFIC_SETTINGS_GROUP,SETTINGS_POPUP_TIMEOUT_VALUE,Traffic_PopupTimeoutValue);
	//
	// ������ ���������
	DBWriteContactSettingTString(NULL, TRAFFIC_SETTINGS_GROUP, SETTINGS_COUNTER_FORMAT, Traffic_CounterFormat);

	DBWriteContactSettingTString(NULL, TRAFFIC_SETTINGS_GROUP, SETTINGS_TOOLTIP_FORMAT, Traffic_TooltipFormat);

	DBWriteContactSettingByte(NULL, TRAFFIC_SETTINGS_GROUP, SETTINGS_ADDITION_SPACE, Traffic_AdditionSpace);
	// ��������� �����
	DBWriteContactSettingDword(NULL, TRAFFIC_SETTINGS_GROUP, SETTINGS_WHAT_DRAW, unOptions.Flags);
	DBWriteContactSettingWord(NULL, TRAFFIC_SETTINGS_GROUP, SETTINGS_STAT_ACC_OPT, Stat_SelAcc);
}

/*--------------------------------------------------------------------------------------------*/
int TrafficRecv(WPARAM wParam,LPARAM lParam)
{	
	NETLIBNOTIFY *nln = (NETLIBNOTIFY*)wParam;
	NETLIBUSER *nlu = (NETLIBUSER*)lParam;
	int i;
		
	if (nln->result > 0)
		for (i = 0; i < NumberOfAccounts; i++)
			if (!strcmp(ProtoList[i].name, nlu->szSettingsModule)) 
				InterlockedExchangeAdd(&ProtoList[i].AllStatistics[ProtoList[i].NumberOfRecords-1].Incoming, nln->result);
	return 0;
}

int TrafficSend(WPARAM wParam,LPARAM lParam)
{  
	NETLIBNOTIFY *nln = (NETLIBNOTIFY*)wParam;
	NETLIBUSER *nlu = (NETLIBUSER*)lParam;
	int i;
	
	if (nln->result > 0)
		for (i = 0; i < NumberOfAccounts; i++)
			if (!strcmp(ProtoList[i].name, nlu->szSettingsModule))
				InterlockedExchangeAdd(&ProtoList[i].AllStatistics[ProtoList[i].NumberOfRecords-1].Outgoing, nln->result);
	return 0;
}

int TrafficCounter_PaintCallbackProc(HWND hWnd, HDC hDC, RECT * rcPaint, HRGN rgn, DWORD dFlags, void * CallBackData)
{
    return TrafficCounter_Draw(hWnd,hDC);   
}

int TrafficCounter_Draw(HWND hwnd, HDC hDC)
{
  if (hwnd==(HWND)-1) return 0;
  if (GetParent(hwnd) == (HWND)CallService(MS_CLUI_GETHWND, 0, 0))
	  return PaintTrafficCounterWindow(hwnd, hDC);
  else
    InvalidateRect(hwnd,NULL,FALSE);
  return 0;
}

static void TC_AlphaText(HDC hDC, LPCTSTR lpString, RECT* lpRect, UINT format, BYTE ClistModernPresent)
{
	int nCount = lstrlen( lpString );

	if (ClistModernPresent)
		AlphaText(hDC, lpString, nCount, lpRect, format, Traffic_FontColor);
	else 
		DrawText( hDC, lpString, nCount, lpRect, format );
}

static void TC_DrawIconEx( HDC hdc,int xLeft,int yTop,HICON hIcon, HBRUSH hbrFlickerFreeDraw, BYTE ClistModernPresent)
{
	if (ClistModernPresent)
		mod_DrawIconEx_helper( hdc, xLeft, yTop, hIcon, 16, 16, 0, hbrFlickerFreeDraw, DI_NORMAL );
	else 
		DrawIconEx( hdc, xLeft, yTop, hIcon, 16, 16, 0, hbrFlickerFreeDraw, DI_NORMAL );
}

int PaintTrafficCounterWindow(HWND hwnd, HDC hDC)
{
	RECT        rect, rect2;
	HFONT		old_font;
	TCHAR		*buf = NULL;
	int			i, dx, height, width;
	HBRUSH		b, t;
	HDC			hdc;
	HBITMAP		hbmp, oldbmp;
	BITMAPINFO  RGB32BitsBITMAPINFO = {0};
	BLENDFUNCTION aga = {AC_SRC_OVER, 0, 0xFF, AC_SRC_ALPHA};
	DWORD SummarySession, SummaryTotal;

	BYTE ClistModernPresent = (GetModuleHandleA("clist_modern.dll") || GetModuleHandleA("clist_modern_dora.dll"))
			&& !DBGetContactSettingByte(NULL, "ModernData", "DisableEngine", 0)
			&& DBGetContactSettingByte(NULL, "ModernData", "EnableLayering", 1);

	GetClientRect (hwnd, &rect);
	height = rect.bottom - rect.top;
	width = rect.right - rect.left;

	// ���� �������� ����������.
	hdc = CreateCompatibleDC(hDC);
	//
	RGB32BitsBITMAPINFO.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	RGB32BitsBITMAPINFO.bmiHeader.biWidth = width;
	RGB32BitsBITMAPINFO.bmiHeader.biHeight = height;
	RGB32BitsBITMAPINFO.bmiHeader.biPlanes = 1;
	RGB32BitsBITMAPINFO.bmiHeader.biBitCount = 32;
	RGB32BitsBITMAPINFO.bmiHeader.biCompression = BI_RGB;
	hbmp = CreateDIBSection(NULL, 
		&RGB32BitsBITMAPINFO, 
		DIB_RGB_COLORS,
		NULL,
		NULL, 0);
	oldbmp = (HBITMAP)SelectObject(hdc, hbmp);

	b = CreateSolidBrush(Traffic_BkColor);
	t = CreateSolidBrush(KeyColor);
	
	if (   ClistModernPresent
		&& unOptions.DrawFrmAsSkin)
	{
		SKINDRAWREQUEST rq;

		rq.hDC = hdc;
		rq.rcDestRect = rect;
		rq.rcClipRect = rect;  
		strncpy(rq.szObjectID, "Main,ID=EventArea", sizeof(rq.szObjectID));
		if (CallService(MS_SKIN_DRAWGLYPH, (WPARAM)&rq, 0))
		{
			strncpy(rq.szObjectID, "Main,ID=StatusBar", sizeof(rq.szObjectID));
			CallService(MS_SKIN_DRAWGLYPH, (WPARAM)&rq, 0);
		}
	}
	else
	{
		SetTextColor(hdc, Traffic_FontColor);
		SetBkColor(hdc, Traffic_BkColor);
		FillRect(hdc, &rect, b);
		AlphaBlend(hdc, 0, 0, width, height, hdc, 0, 0, width, height, aga);
	}

	old_font = (HFONT)SelectObject(hdc, Traffic_h_font);

	// ������������ ������� ���������
	rect.top += 2;
	rect.left += 3;
	rect.bottom -= 2;
	rect.right -= 3;

//-------------------
// ���� ��� ������� Variables, ������ ���������.
//-------------------
	if (!ServiceExists(MS_VARS_FORMATSTRING))
	{
		SummarySession = SummaryTotal = 0;
		// ��� ������� ��������
		for (i = 0; i < NumberOfAccounts; i++)
		{
			// ������ ���� ��������� ��� �����������.
			if (ProtoList[i].Visible && ProtoList[i].Enabled)
			{
				dx = 0;
				// ���������� ������ ��������.
				if (unOptions.DrawProtoIcon)
				{
					TC_DrawIconEx(hdc, rect.left, rect.top,
						LoadSkinnedProtoIcon(ProtoList[i].name, CallProtoService(ProtoList[i].name,PS_GETSTATUS,0,0)), b, ClistModernPresent);
					dx = 19;
				}
				// ���������� ��� ��������
				if (unOptions.DrawProtoName)
				{
					rect.left += dx;
					TC_AlphaText(hdc, ProtoList[i].tszAccountName, &rect, DT_SINGLELINE | DT_LEFT | DT_TOP, ClistModernPresent);
					rect.left -= dx;
				}
				// ��������� ������� �������� �� ������� ����.
				// ����� �����.
				if (unOptions.DrawTotalTimeCounter)
				{
					TCHAR bu[32];
				
					GetDurationFormatM(ProtoList[i].Total.Timer, _T("h:mm:ss"), bu, 32);
					TC_AlphaText(hdc, bu, &rect, DT_SINGLELINE | DT_RIGHT | DT_TOP, ClistModernPresent);
				}
				// ������� �����.
				if (unOptions.DrawCurrentTimeCounter)
				{
					TCHAR bu[32];

					GetDurationFormatM(ProtoList[i].Session.Timer, _T("h:mm:ss"), bu, 32);
					rect.right -= 50;
					TC_AlphaText(hdc, bu, &rect, DT_SINGLELINE | DT_RIGHT | DT_TOP, ClistModernPresent);
					rect.right += 50;
				}
				// ���������� ����� ������.
				if (unOptions.DrawTotalTraffic)
				{
					TCHAR bu[32];

					GetFormattedTraffic(ProtoList[i].TotalSentTraffic + ProtoList[i].TotalRecvTraffic, 3, bu, 32);
					rect.right -= 100;
					TC_AlphaText(hdc, bu, &rect, DT_SINGLELINE | DT_RIGHT | DT_TOP, ClistModernPresent);
					rect.right += 100;

					if (ProtoList[i].Visible)
						SummaryTotal += ProtoList[i].TotalRecvTraffic + ProtoList[i].TotalSentTraffic;
				}
				// ���������� ������� ������.
				if (unOptions.DrawCurrentTraffic)
				{
					TCHAR bu[32];

					GetFormattedTraffic(ProtoList[i].CurrentRecvTraffic + ProtoList[i].CurrentSentTraffic, 3, bu, 32);
					rect.right -= 150;
					TC_AlphaText(hdc, bu, &rect, DT_SINGLELINE | DT_RIGHT | DT_TOP, ClistModernPresent);
					rect.right += 150;

					if (ProtoList[i].Visible)
						SummarySession += ProtoList[i].CurrentRecvTraffic + ProtoList[i].CurrentSentTraffic;
				}

				rect.top += Traffic_LineHeight + Traffic_AdditionSpace;
			}
		}
		// ������ ��������� ������ ��������� ���������.
		if (unOptions.ShowSummary)
		{
			// ���������� ������.
			dx = 0;
			if (unOptions.DrawProtoIcon)
			{
				TC_DrawIconEx(hdc, rect.left, rect.top,
					LoadSkinnedIcon(SKINICON_OTHER_MIRANDA), b, ClistModernPresent);
				dx = 19;
			}
			// ������� �����
			// ���������� ���
			if (unOptions.DrawProtoName)
			{
				TCHAR *bu = mir_a2t("Summary");

				rect.left += dx;
				TC_AlphaText(hdc, bu, &rect, DT_SINGLELINE | DT_LEFT | DT_TOP, ClistModernPresent);
				rect.left -= dx;
				mir_free(bu);
			}
			// ����� �����.
			if (unOptions.DrawTotalTimeCounter)
			{
				TCHAR bu[32];
			
				GetDurationFormatM(OverallInfo.Total.Timer, _T("h:mm:ss"), bu, 32);
				TC_AlphaText(hdc, bu, &rect, DT_SINGLELINE | DT_RIGHT | DT_TOP, ClistModernPresent);
			}
			// ������� �����.
			if (unOptions.DrawCurrentTimeCounter)
			{
				TCHAR bu[32];

				GetDurationFormatM(OverallInfo.Session.Timer, _T("h:mm:ss"), bu, 32);
				rect.right -= 50;
				TC_AlphaText(hdc, bu, &rect, DT_SINGLELINE | DT_RIGHT | DT_TOP, ClistModernPresent);
				rect.right += 50;
			}
			// ���������� ����� ������.
			if (unOptions.DrawTotalTraffic)
			{
				TCHAR bu[32];

				GetFormattedTraffic(SummaryTotal, 3, bu, 32);
				rect.right -= 100;
				TC_AlphaText(hdc, bu, &rect, DT_SINGLELINE | DT_RIGHT | DT_TOP, ClistModernPresent);
				rect.right += 100;
			}
			// ���������� ������� ������.
			if (unOptions.DrawCurrentTraffic)
			{
				TCHAR bu[32];

				GetFormattedTraffic(SummarySession, 3, bu, 32);
				rect.right -= 150;
				TC_AlphaText(hdc, bu, &rect, DT_SINGLELINE | DT_RIGHT | DT_TOP, ClistModernPresent);
				rect.right += 150;
			}
		}
		rect.top += Traffic_LineHeight + Traffic_AdditionSpace;
		// ������ �������� ������.
		if (unOptions.ShowOverall)
		{
			// ���������� ������.
			dx = 0;
			if (unOptions.DrawProtoIcon)
			{
				TC_DrawIconEx(hdc, rect.left, rect.top,
					LoadSkinnedIcon(SKINICON_OTHER_MIRANDA), b, ClistModernPresent);
				dx = 19;
			}
			// ������� �����
			// ���������� ���
			if (unOptions.DrawProtoName)
			{
				TCHAR *bu = mir_a2t("Overall");

				rect.left += dx;
				TC_AlphaText(hdc, bu, &rect, DT_SINGLELINE | DT_LEFT | DT_TOP, ClistModernPresent);
				rect.left -= dx;
				mir_free(bu);
			}
			// ��������� ������� �������� �� ������� ����.
			// ���������� ����� ������.
			if (unOptions.DrawTotalTraffic)
			{
				TCHAR bu[32];

				GetFormattedTraffic(OverallInfo.TotalSentTraffic + OverallInfo.TotalRecvTraffic, 3, bu, 32);
				rect.right -= 100;
				TC_AlphaText(hdc, bu, &rect, DT_SINGLELINE | DT_RIGHT | DT_TOP, ClistModernPresent);
				rect.right += 100;
			}
			// ���������� ������� ������.
			if (unOptions.DrawCurrentTraffic)
			{
				TCHAR bu[32];

				GetFormattedTraffic(OverallInfo.CurrentRecvTraffic + OverallInfo.CurrentSentTraffic, 3, bu, 32);
				rect.right -= 150;
				TC_AlphaText(hdc, bu, &rect, DT_SINGLELINE | DT_RIGHT | DT_TOP, ClistModernPresent);
				rect.right += 150;
			}
		}
	}
	else
//-------------
// ���� ���� Variables - ������ ��-������
//-------------
	{
		RowItemInfo *ItemsList;
		WORD ItemsNumber, RowsNumber;
		TCHAR **ExtraText;
		HICON *ahIcon;

		// ������� ������ ����� ��� Variables � ������.
		ExtraText = (TCHAR**)mir_alloc(sizeof(TCHAR*));
		ahIcon = (HICON*)mir_alloc(sizeof(HICON));
		RowsNumber = 0;
		// ���� �� ���������.
		for (i = 0; i < NumberOfAccounts; i++)
		{
			if (ProtoList[i].Visible && ProtoList[i].Enabled)
			{
				ExtraText = (TCHAR**)mir_realloc(ExtraText, sizeof(TCHAR*) * (RowsNumber + 1));
				ahIcon = (HICON*)mir_realloc(ahIcon, sizeof(HICON) * (RowsNumber + 1));

				ExtraText[RowsNumber] = mir_a2t(ProtoList[i].name);
				ahIcon[RowsNumber++] = LoadSkinnedProtoIcon(ProtoList[i].name, CallProtoService(ProtoList[i].name, PS_GETSTATUS, 0, 0));
			}
		}
		// ��� 2 ������ ��������.
		if (unOptions.ShowSummary)
		{
			ExtraText = (TCHAR**)mir_realloc(ExtraText, sizeof(TCHAR*) * (RowsNumber + 1));
			ahIcon = (HICON*)mir_realloc(ahIcon, sizeof(HICON) * (RowsNumber + 1));

			ExtraText[RowsNumber] = mir_a2t("summary");
			ahIcon[RowsNumber++] = LoadSkinnedIcon(SKINICON_OTHER_MIRANDA);
		}
		if (unOptions.ShowOverall)
		{
			ExtraText = (TCHAR**)mir_realloc(ExtraText, sizeof(TCHAR*) * (RowsNumber + 1));
			ahIcon = (HICON*)mir_realloc(ahIcon, sizeof(HICON) * (RowsNumber + 1));

			ExtraText[RowsNumber] = mir_a2t("overall");
			ahIcon[RowsNumber++] = LoadSkinnedIcon(SKINICON_OTHER_MIRANDA);
		}

		// ������ ���� ������� ��� ������� �� ��������� ����������
		for (i = 0; i < RowsNumber; i++)
		{
			buf = variables_parse(Traffic_CounterFormat, ExtraText[i], NULL);
			if (ItemsNumber = GetRowItems(buf, &ItemsList))
			{
				// ������ �����.
				for (dx = 0; dx < ItemsNumber; dx++)
				{
					// ������ ����� �������������� ��� ���������.
					memcpy(&rect2, &rect, sizeof(RECT));
					rect2.bottom = rect2.top + Traffic_LineHeight;
					// ��� �� ���?
					switch (ItemsList[dx].Alignment)
					{
						case 'I':
							TC_DrawIconEx(hdc, ItemsList[dx].Interval, rect2.top, ahIcon[i], b, ClistModernPresent);
							break;
						case 'i':
							TC_DrawIconEx(hdc, width - 16 - ItemsList[dx].Interval, rect2.top, ahIcon[i], b, ClistModernPresent);
							break;
						case 'L':
							rect2.left = ItemsList[dx].Interval;
							TC_AlphaText(hdc, ItemsList[dx].String, &rect2, DT_SINGLELINE | DT_LEFT | DT_VCENTER, ClistModernPresent);
							break;
						case 'R':
							rect2.right = width - ItemsList[dx].Interval;
							TC_AlphaText(hdc, ItemsList[dx].String, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER, ClistModernPresent);
							break;
						default:
							continue;
					}
					
				}

				// ����� ���������� ������.
				for (; ItemsNumber--; )
				{
					mir_free(ItemsList[ItemsNumber].String);
				}
				mir_free(ItemsList);
				dx = CallService(MS_VARS_FREEMEMORY, (WPARAM)(void*)buf, 0);
				rect.top += Traffic_LineHeight + Traffic_AdditionSpace;
			}
		}

		// ������� ������ �����.
		for (; RowsNumber--; )
			mir_free(ExtraText[RowsNumber]);
		mir_free(ExtraText);
	}

	DeleteObject(b);
	DeleteObject(t);
	SelectObject(hdc, old_font);

	if (ClistModernPresent)
	{
		AlphaBlend(hDC, 0, 0, width, height, hdc, 0, 0, width, height, aga);
	}
	else
	{
		BitBlt(hDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
	}

	SelectObject(hdc, oldbmp);
	DeleteObject(hbmp);
	DeleteDC(hdc);

	return 0;
}

void ProtocolIsOnLine(int num)
{
	DWORD CurrentTimeMs;

	if (ProtoList[num].State) return;

	online_count++;
	CurrentTimeMs = GetTickCount();

	ProtoList[num].State = 1;
	ProtoList[num].Session.TimeAtStart = CurrentTimeMs;
	ProtoList[num].Total.TimeAtStart = CurrentTimeMs
		- 1000 * ProtoList[num].AllStatistics[ProtoList[num].NumberOfRecords - 1].Time;
	ProtoList[num].Session.Timer = 0;

	if (online_count == 1) // ���� �� ���� ������� ������� � ������ - ��������� ������� ��������� �����.
	{
		OverallInfo.Session.TimeAtStart = CurrentTimeMs - 1000 * OverallInfo.Session.Timer;
		OverallInfo.Total.TimeAtStart = CurrentTimeMs - 1000 * OverallInfo.Total.Timer;
	}
}

void ProtocolIsOffLine(int num)
{
	if (!ProtoList[num].State) return;

	online_count--;
	ProtoList[num].State = 0;
}

static POINT ptMouse = {0};

LRESULT CALLBACK TrafficCounterWndProc_MW(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	POINT p;
	int i;
	//
	switch (msg)
    {
		case (WM_USER+697):
			if (lParam == 666)
				DestroyWindow(hwnd);
			break;

		case WM_PAINT:
			{
				if (  !DBGetContactSettingByte(NULL, "ModernData", "DisableEngine", 0)
					&& DBGetContactSettingByte(NULL, "ModernData", "EnableLayering", 1)
					&& ServiceExists(MS_SKINENG_INVALIDATEFRAMEIMAGE))
					CallService(MS_SKINENG_INVALIDATEFRAMEIMAGE, (WPARAM)TrafficHwnd, 0);
				else
				{
					HDC hdc;
					hdc = GetDC(hwnd);
					PaintTrafficCounterWindow(hwnd, hdc);
					ReleaseDC(hwnd, hdc);
				}
			}
			break;

		case WM_ERASEBKGND:
			return 1;

		case WM_LBUTTONDOWN :
			if (DBGetContactSettingByte(NULL,"CLUI","ClientAreaDrag",SETTING_CLIENTDRAG_DEFAULT))
			{
				ClientToScreen(GetParent(hwnd),&p);
				return SendMessage(GetParent(hwnd), WM_SYSCOMMAND, SC_MOVE|HTCAPTION,MAKELPARAM(p.x,p.y));
			}
			break;

		case WM_RBUTTONDOWN :
			p.x = GET_X_LPARAM(lParam);
			p.y = GET_Y_LPARAM(lParam);
			ClientToScreen(hwnd,&p);
			TrackPopupMenu(TrafficPopupMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,p.x,p.y,0,hwnd,NULL);
			break;

		case WM_COMMAND :
			switch (wParam)
			{
				case POPUPMENU_HIDE:
					MenuCommand_TrafficShowHide(0, 0);
					break;

				case POPUPMENU_CLEAR_NOW:
					for (i = 0; i < NumberOfAccounts; i++)
					{
						ProtoList[i].StartIncoming =
							ProtoList[i].AllStatistics[ProtoList[i].NumberOfRecords - 1].Incoming;
						ProtoList[i].StartOutgoing =
							ProtoList[i].AllStatistics[ProtoList[i].NumberOfRecords - 1].Outgoing;
						ProtoList[i].Session.TimeAtStart = GetTickCount();
					}
					OverallInfo.CurrentRecvTraffic = OverallInfo.CurrentSentTraffic = 0;
			}
			break;

		case WM_SETCURSOR:
			{
				POINT pt;

				GetCursorPos(&pt);
				if ( (abs(pt.x - ptMouse.x) < 20)
			      && (abs(pt.y - ptMouse.y) < 20) )
					return 1;

				if (TooltipShowing)
				{
					KillTimer(TrafficHwnd, TIMER_TOOLTIP);
					CallService("mToolTip/HideTip", 0, 0);
					TooltipShowing = FALSE;
				}
				KillTimer(TrafficHwnd, TIMER_TOOLTIP);
				SetTimer(TrafficHwnd, TIMER_TOOLTIP, CallService(MS_CLC_GETINFOTIPHOVERTIME, 0, 0), 0);
				break;
			}

		case WM_TIMER :
			switch(wParam)
			{
				case TIMER_NOTIFY_TICK:
					NotifyOnRecv();
					NotifyOnSend();
					break;

				case TIMER_REDRAW: // ����������� ��� � ����������.
					{
						DWORD CurrentTimeMs;
						SYSTEMTIME stNow;

						SaveSettings(1);
						// ����� ��������� ���������� ������� �������� ��������.
						OverallInfo.CurrentRecvTraffic =
							OverallInfo.CurrentSentTraffic =
							OverallInfo.TotalRecvTraffic =
							OverallInfo.TotalSentTraffic = 0;

						CurrentTimeMs = GetTickCount();

						for (i = 0; i < NumberOfAccounts; i++)
						{
							if (ProtoList[i].State)
							{
								ProtoList[i].AllStatistics[ProtoList[i].NumberOfRecords-1].Time =
									(CurrentTimeMs - ProtoList[i].Total.TimeAtStart) / 1000;
								ProtoList[i].Session.Timer =
									(CurrentTimeMs - ProtoList[i].Session.TimeAtStart) / 1000;
							}

							Stat_CheckStatistics(i);

							{	// ����� �� ��������� ���������� ����������� �������� ���� �������� � �������.
								DWORD Sum1, Sum2;
								unsigned long int j;

								// �������� ��� ������� ������.
								for (Sum1 = Sum2 = 0, j = ProtoList[i].StartIndex; j < ProtoList[i].NumberOfRecords; j++)
								{
									Sum1 += ProtoList[i].AllStatistics[j].Incoming;
									Sum2 += ProtoList[i].AllStatistics[j].Outgoing;
								}
								ProtoList[i].CurrentRecvTraffic = Sum1 - ProtoList[i].StartIncoming;
								ProtoList[i].CurrentSentTraffic = Sum2 - ProtoList[i].StartOutgoing;
								OverallInfo.CurrentRecvTraffic += ProtoList[i].CurrentRecvTraffic;
								OverallInfo.CurrentSentTraffic += ProtoList[i].CurrentSentTraffic;
								// �������� ��� ���������� �������.
								ProtoList[i].TotalRecvTraffic =
									Stat_GetItemValue(1 << i,
													  unOptions.PeriodForShow + 1,
													  Stat_GetRecordsNumber(i, unOptions.PeriodForShow + 1) - 1,
													  1);
								ProtoList[i].TotalSentTraffic =
									Stat_GetItemValue(1 << i,
													  unOptions.PeriodForShow + 1,
													  Stat_GetRecordsNumber(i, unOptions.PeriodForShow + 1) - 1,
													  2);
								ProtoList[i].Total.Timer =
									Stat_GetItemValue(1 << i,
													  unOptions.PeriodForShow + 1,
													  Stat_GetRecordsNumber(i, unOptions.PeriodForShow + 1) - 1,
													  4);
								OverallInfo.TotalRecvTraffic += ProtoList[i].TotalRecvTraffic;
								OverallInfo.TotalSentTraffic += ProtoList[i].TotalSentTraffic;
							}
						}
						if (!((OverallInfo.CurrentRecvTraffic >> 10) % Traffic_Notify_size_value)
							&& unOptions.NotifyBySize
							&& notify_recv_size != OverallInfo.CurrentRecvTraffic >> 10 )
						{
							NotifyOnRecv();
						}
						if (!((OverallInfo.CurrentSentTraffic >> 10) % Traffic_Notify_size_value)
							&& unOptions.NotifyBySize
							&& notify_send_size != OverallInfo.CurrentSentTraffic >> 10)
						{
							NotifyOnSend();
						}

						// �������� ���������� �������.
						GetLocalTime(&stNow);

						// �� ���� �� �������� ����� �������?
						if (   (unOptions.PeriodForShow == 0
								&& stNow.wHour == 0
								&& stNow.wMinute == 0
								&& stNow.wSecond == 0)
							|| (unOptions.PeriodForShow == 1
								&& DayOfWeek(stNow.wDay, stNow.wMonth,	stNow.wYear) == 1
								&& stNow.wHour == 0
								&& stNow.wMinute == 0
								&& stNow.wSecond == 0)
							|| (unOptions.PeriodForShow == 2
								&& stNow.wDay == 1
								&& stNow.wHour == 0
								&& stNow.wMinute == 0
								&& stNow.wSecond == 0)
							|| (unOptions.PeriodForShow == 3
								&& stNow.wMonth == 1
								&& stNow.wDay == 1
								&& stNow.wHour == 0
								&& stNow.wMinute == 0
								&& stNow.wSecond == 0) )
							OverallInfo.Total.TimeAtStart = CurrentTimeMs;

						if (online_count > 0)
						{
							OverallInfo.Session.Timer =
								(CurrentTimeMs - OverallInfo.Session.TimeAtStart) / 1000;
							OverallInfo.Total.Timer =
								(CurrentTimeMs - OverallInfo.Total.TimeAtStart) / 1000;
						}

						CallService(MS_CLIST_FRAMES_UPDATEFRAME, (WPARAM)Traffic_FrameID, FU_FMREDRAW);
					}
					break;

				case TIMER_TOOLTIP:
					{
						TCHAR *TooltipText;
						CLCINFOTIP ti = {0};
						RECT rt;

						GetCursorPos(&TooltipPosition);
						if (!TooltipShowing && unOptions.ShowTooltip )
						{
							KillTimer(TrafficHwnd, TIMER_TOOLTIP);
							ScreenToClient(TrafficHwnd, &TooltipPosition);
							GetClientRect(TrafficHwnd, &rt);
							if (PtInRect(&rt, TooltipPosition))
							{
								GetCursorPos(&ptMouse);
								ti.rcItem.left   = TooltipPosition.x - 10;
								ti.rcItem.right  = TooltipPosition.x + 10;
								ti.rcItem.top    = TooltipPosition.y - 10;
								ti.rcItem.bottom = TooltipPosition.y + 10;
								ti.cbSize = sizeof( ti );
								TooltipText = variables_parsedup(Traffic_TooltipFormat, NULL, NULL);
#ifdef _UNICODE
								CallService("mToolTip/ShowTipW", (WPARAM)TooltipText, (LPARAM)&ti);
#else
								CallService("mToolTip/ShowTip", (WPARAM)TooltipText, (LPARAM)&ti);
#endif
								TooltipShowing = TRUE;
								free(TooltipText);
							}
						}
					}
					break;
			}
			break;

		default:
			return DefWindowProc(hwnd,msg,wParam,lParam); 
	}
	//
	return DefWindowProc(hwnd,msg,wParam,lParam); 
}

void CreateTrafficWindow(HWND hCluiWnd)
{
	WNDCLASSEX wcx = {0};
	CLISTFrame f;

	wcx.cbSize			=	sizeof( WNDCLASSEX );
	wcx.style			=	0;
	wcx.lpfnWndProc		=	TrafficCounterWndProc_MW;
	wcx.cbClsExtra		=	0;
	wcx.cbWndExtra		=	0;
	wcx.hInstance		=	hInst;
	wcx.hIcon			=	NULL;
	wcx.hCursor			=	LoadCursor(hInst,IDC_ARROW);
	wcx.hbrBackground	=	0;
	wcx.lpszMenuName	=	NULL;
	wcx.lpszClassName	=	TRAFFIC_COUNTER_WINDOW_CLASS;
	wcx.hIconSm			=	NULL;
	RegisterClassEx( &wcx );
	TrafficHwnd = CreateWindowEx(WS_EX_TOOLWINDOW, TRAFFIC_COUNTER_WINDOW_CLASS,
								TRAFFIC_COUNTER_WINDOW_CLASS,
								WS_CHILDWINDOW | WS_CLIPCHILDREN,
								0, 0, 0, 0, hCluiWnd, NULL, hInst, NULL);

	if ( ServiceExists(MS_CLIST_FRAMES_ADDFRAME) )
	{
		// ��������� ��������� �����
		ZeroMemory( &f, sizeof(CLISTFrame) );
		f.align = alBottom;
		f.cbSize = sizeof(CLISTFrame);
		f.height = TrafficWindowHeight();
		f.Flags= unOptions.FrameIsVisible | F_LOCKED | F_NOBORDER | F_NO_SUBCONTAINER;
		f.hWnd = TrafficHwnd;
		f.TBname = ("Traffic counter");
		f.name = ("Traffic counter");
		// ������ �����
		Traffic_FrameID = (HANDLE)CallService(MS_CLIST_FRAMES_ADDFRAME,(WPARAM)&f,(LPARAM)0);
		CallService(MS_SKINENG_REGISTERPAINTSUB,(WPARAM)f.hWnd,(LPARAM)TrafficCounter_PaintCallbackProc);

		//create popup menu
		CreateTrafficMenu();
	}

	// ������ �������.
	SetTimer(TrafficHwnd, TIMER_REDRAW, 500, NULL);
	UpdateNotifyTimer();
}

int MenuCommand_TrafficShowHide(WPARAM wParam,LPARAM lParam)
{
	unOptions.FrameIsVisible = !unOptions.FrameIsVisible;
	if (Traffic_FrameID == NULL)
		ShowWindow(TrafficHwnd, unOptions.FrameIsVisible ? SW_SHOW : SW_HIDE);
	else
		CallService(MS_CLIST_FRAMES_SHFRAME, (WPARAM)Traffic_FrameID, 0);
	DBWriteContactSettingDword(NULL, TRAFFIC_SETTINGS_GROUP, SETTINGS_WHAT_DRAW, unOptions.Flags);
	//
	return 0;
}

void CreateTrafficMenu(void)
{
	CLISTMENUITEM mi;

	if (unOptions.ShowMainMenuItem)
	{
		ZeroMemory(&mi,sizeof(mi));
		mi.cbSize = sizeof(mi);
		mi.position = -0x7FFFFFFF;
		mi.flags = 0;
		mi.hIcon = NULL;
		mi.pszName = Translate("Toggle traffic counter");
		mi.pszService="TrafficCounter/ShowHide";
		CallService(MS_CLIST_ADDMAINMENUITEM,0,(LPARAM)&mi);
	}
	//create popup menu here
	TrafficPopupMenu = CreatePopupMenu();
	//
	if (TrafficPopupMenu == NULL) return;
	//
	AppendMenu(TrafficPopupMenu,MF_STRING,POPUPMENU_HIDE,TranslateT("Hide traffic window"));
	AppendMenu(TrafficPopupMenu,MF_STRING,POPUPMENU_CLEAR_NOW,TranslateT("Clear the current (Now:) value"));
}

/*-------------------------------------------------------------------------------------------------------------------*/
void UpdateNotifyTimer(void)
{
	if (!ServiceExists(MS_POPUP_ADDPOPUP)) return;

	if (Traffic_Notify_time_value && unOptions.NotifyByTime)
		SetTimer(TrafficHwnd, TIMER_NOTIFY_TICK, Traffic_Notify_time_value * 1000 * 60, NULL);
	else
		KillTimer(TrafficHwnd, TIMER_NOTIFY_TICK);
}

void NotifyOnSend(void)
{
	POPUPDATAT ppd;

	ZeroMemory(&ppd, sizeof(ppd));
	ppd.lchContact = NULL;
	ppd.lchIcon = LoadSkinnedIcon(SKINICON_EVENT_MESSAGE);
	_tcscpy(ppd.lptzContactName, TranslateT("Traffic counter notification"));
	//
	_stprintf(ppd.lptzText, TranslateT("%d kilobytes sent"),
		notify_send_size = OverallInfo.CurrentSentTraffic >> 10);
	//
	ppd.colorBack = Traffic_PopupBkColor;
	ppd.colorText = Traffic_PopupFontColor;
	ppd.PluginWindowProc = NULL;
	if (Traffic_PopupTimeoutDefault) ppd.iSeconds = 0;
		else ppd.iSeconds = Traffic_PopupTimeoutValue;
	CallService(MS_POPUP_ADDPOPUPT, (WPARAM)&ppd, 0); 
}

void NotifyOnRecv(void)
{
	POPUPDATAT ppd;

	ZeroMemory(&ppd, sizeof(ppd));
	ppd.lchContact = NULL;
	ppd.lchIcon = LoadSkinnedIcon(SKINICON_EVENT_MESSAGE);
	_tcscpy(ppd.lptzContactName, TranslateT("Traffic counter notification"));
	//
	_stprintf(ppd.lptzText,TranslateT("%d kilobytes received"),
		notify_recv_size = OverallInfo.CurrentRecvTraffic >> 10);
	//
	ppd.colorBack = Traffic_PopupBkColor;
	ppd.colorText = Traffic_PopupFontColor;
	ppd.PluginWindowProc = NULL;
	if (Traffic_PopupTimeoutDefault) ppd.iSeconds = 0;
		else ppd.iSeconds = Traffic_PopupTimeoutValue;
	CallService(MS_POPUP_ADDPOPUPT, (WPARAM)&ppd, 0); 
}

void CreateProtocolList(void)
{
	int i;
	PROTOACCOUNT **acc;
	//
	CallService(MS_PROTO_ENUMACCOUNTS, (WPARAM)(int*)&NumberOfAccounts, (LPARAM)(PROTOACCOUNT***)&acc);
	//
	ProtoList = (PROTOLIST*)mir_alloc(sizeof(PROTOLIST)*(NumberOfAccounts));
	//
	for (i = 0; i < NumberOfAccounts; i++)
	{
		ProtoList[i].name = (char*)mir_alloc(strlen(acc[i]->szModuleName) + 1);
		strcpy(ProtoList[i].name, acc[i]->szModuleName);
		ProtoList[i].tszAccountName = (TCHAR*)mir_alloc(sizeof(TCHAR) * (1 + _tcslen(acc[i]->tszAccountName)));
		_tcscpy(ProtoList[i].tszAccountName, acc[i]->tszAccountName);
		//
		ProtoList[i].Flags = DBGetContactSettingByte(NULL, ProtoList[i].name, SETTINGS_PROTO_FLAGS, 3);
		ProtoList[i].CurrentRecvTraffic = 
			ProtoList[i].CurrentSentTraffic = 
			ProtoList[i].Session.Timer = 0;
		//
		ProtoList[i].Enabled = acc[i]->bIsEnabled;
		ProtoList[i].State = 0;

		Stat_ReadFile(i);
		ProtoList[i].StartIndex = ProtoList[i].NumberOfRecords - 1;
		ProtoList[i].StartIncoming = ProtoList[i].AllStatistics[ProtoList[i].StartIndex].Incoming;
		ProtoList[i].StartOutgoing = ProtoList[i].AllStatistics[ProtoList[i].StartIndex].Outgoing;
	} // ���� �� ���������
	// ��������� �������� ��� ��������� ����������.
	OverallInfo.Session.Timer =	OverallInfo.Total.Timer = 0;
}

void DestroyProtocolList(void)
{
	int i;
	//
	for (i = 0; i < NumberOfAccounts; i++)
	{
		Stat_CheckStatistics(i);
		CloseHandle(ProtoList[i].hFile);
		mir_free(ProtoList[i].name);
		mir_free(ProtoList[i].AllStatistics);
	}
	//
	mir_free(ProtoList);
}

int ProtocolAckHook(WPARAM wParam,LPARAM lParam)
{
	ACKDATA* pAck=(ACKDATA*)lParam;
	WORD i;

	if (ACKTYPE_STATUS == pAck->type) 
	{
		if(ID_STATUS_OFFLINE == pAck->lParam) 
		{
			for (i = 0; i < NumberOfAccounts; i++)
			{
				if (!ProtoList[i].name) continue;
				if (!strcmp(ProtoList[i].name, pAck->szModule))
				{
					ProtocolIsOffLine(i);
					break;
				}
			}
		}
		else
		{
			if ((pAck->lParam >= ID_STATUS_ONLINE) && (pAck->lParam <= ID_STATUS_OUTTOLUNCH))
			{
				for (i = 0; i < NumberOfAccounts; i++)
					if (!strcmp(ProtoList[i].name, pAck->szModule))
					{
						ProtocolIsOnLine(i);
						break;
					}
			}
		}
	}
	return 0;
}

int UpdateFonts(WPARAM wParam, LPARAM lParam)
{
	LOGFONT logfont;
	//if no font service
	if (!ServiceExists(MS_FONT_GETT)) return 0;
	//update traffic font
	if (Traffic_h_font) DeleteObject(Traffic_h_font);
	Traffic_FontColor = CallService(MS_FONT_GETT, (WPARAM)&TrafficFontID, (LPARAM)&logfont);
	Traffic_h_font = CreateFontIndirect(&logfont);
	//
	TrafficFontHeight = abs(logfont.lfHeight) + 1;
	Traffic_BkColor = CallService(MS_COLOUR_GETT, (WPARAM)&TrafficBackgroundColorID, 0);

	// �������� ����
	UseKeyColor = DBGetContactSettingByte(NULL, "ModernSettings", "UseKeyColor", 1);
	KeyColor = DBGetContactSettingDword(NULL, "ModernSettings", "KeyColor", 0);

	UpdateTrafficWindowSize();
	return 0;
}

void UpdateTrafficWindowSize(void)
{
	if (Traffic_FrameID != NULL)
	{
		CallService(MS_CLIST_FRAMES_SETFRAMEOPTIONS, MAKEWPARAM(FO_HEIGHT, Traffic_FrameID), TrafficWindowHeight());
	}
}

unsigned short int TrafficWindowHeight(void)
{
	BYTE HeightLineTime = (unOptions.DrawProtoIcon) ? 16 : TrafficFontHeight,
		 i, ActProto;
	WORD MaxWndHeight;

	for (i = 0, ActProto = unOptions.ShowOverall + unOptions.ShowSummary; i < NumberOfAccounts; i++)
		ActProto += ProtoList[i].Visible && ProtoList[i].Enabled;

	// ������ ������ ������� 16 �������� (��� ������).
	Traffic_LineHeight = TrafficFontHeight > 16 ? TrafficFontHeight: 16;

	// ������ ������ ����� ���������� �����.
	MaxWndHeight = ActProto * Traffic_LineHeight
		+ Traffic_AdditionSpace * (ActProto - 1)
		+ 4;

	return (MaxWndHeight < TrafficFontHeight) ? 0 : MaxWndHeight;
}

// ������� ������ ��������� � ProtoList ��� ���������� ���������
int OnAccountsListChange(WPARAM wParam, LPARAM lParam)
{
	BYTE i;
	PROTOACCOUNT *acc = (PROTOACCOUNT*)lParam;
	
	switch (wParam)
	{
		case PRAC_ADDED:
		case PRAC_REMOVED:
			DestroyProtocolList();
			CreateProtocolList();
			break;
		case PRAC_CHANGED:
		case PRAC_CHECKED:
			for (i = 0; i < NumberOfAccounts; i++)
				if (!strcmp(acc->szModuleName, ProtoList[i].name))
					ProtoList[i].Enabled = acc->bIsEnabled;
			break;
	}
	UpdateTrafficWindowSize();
	return 0;
}

