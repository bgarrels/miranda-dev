/*
Basic History plugin
Copyright (C) 2011 Krzysztof Kral

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "StdAfx.h"
#include "Options.h"
#include "resource.h"

extern HINSTANCE hInst;
extern bool g_SmileyAddAvail;

#define MODULE				"BasicHistory"

Options *Options::instance;

struct EventNamesType
{
	int id;
	TCHAR* name;
} EventNames[] = 
{
	EVENTTYPE_MESSAGE, LPGENT("Message"),
	EVENTTYPE_FILE, LPGENT("File transfer"),
	EVENTTYPE_URL, LPGENT("Link"),
	EVENTTYPE_STATUSCHANGE, LPGENT("Status change"),
	EVENTTYPE_AUTHREQUEST, LPGENT("Authorization request"),
	EVENTTYPE_ADDED, LPGENT("You were added"),
	EVENTTYPE_CONTACTS, LPGENT("Contacts recieved"),
	EVENTTYPE_SMTPSIMPLE, LPGENT("SMTP Simple Email"),
	ICQEVENTTYPE_SMS, LPGENT("SMS message")
};

Options::Options()
{
	showContacts = false;
	groupNewOnTop = true;
	groupShowEvents = true;
	groupShowTime = true;
	groupShowName = false;
	groupShowMessage = true;
	groupMessageLen = 43;
	groupTime = 5;
	groupMessagesNumber = 100;
	messagesNewOnTop = false;
	messagesShowDate = false;
	messagesShowSec = false;
	messagesShowName = true;
	messagesShowEvents = false;
	messagesUseSmileys = true;
	searchForInList = true;
	searchForInMess = true;
	searchMatchCase = false;
	searchMatchWhole = false;
	searchOnlyIn = false;
	searchOnlyOut = false;
	searchOnlyGroup = false;
	defFilter = 0;
}

Options::~Options()
{
}

int Options::InitOptions(WPARAM wParam, LPARAM lParam)
{
	OPTIONSDIALOGPAGE odp = {0};

	odp.cbSize = sizeof(odp);
	odp.position = 100000000;
	odp.hInstance = hInst;
	odp.flags = ODPF_BOLDGROUPS | ODPF_TCHAR;
	odp.ptszTitle = LPGENT("History");

	odp.ptszTab = LPGENT("Main");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_MAIN);
	odp.pfnDlgProc = Options::DlgProcOptsMain;
	CallService(MS_OPT_ADDPAGE, wParam, (LPARAM)&odp);

	odp.ptszTab = LPGENT("Group list");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_GROUPLIST);
	odp.pfnDlgProc = Options::DlgProcOptsGroupList;
	CallService(MS_OPT_ADDPAGE, wParam, (LPARAM)&odp);

	odp.ptszTab = LPGENT("Messages");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_MESSAGES);
	odp.pfnDlgProc = Options::DlgProcOptsMessages;
	CallService(MS_OPT_ADDPAGE, wParam, (LPARAM)&odp);

	odp.ptszTab = LPGENT("Searching");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_SEARCHING);
	odp.pfnDlgProc = Options::DlgProcOptsSearching;
	CallService(MS_OPT_ADDPAGE, wParam, (LPARAM)&odp);

	return 0;
}

struct FontOptionsList {
	TCHAR*   szDescr;
	COLORREF defColour;
	TCHAR*   szDefFace;
	BYTE     defStyle;
	char     defSize;
	TCHAR*   szBackgroundName;
};

struct ColorOptionsList {
	TCHAR*		tszName;
	COLORREF 	def;
};

struct HotkeyOptionsList {
	const char *pszName;
	const TCHAR *ptszDescription;
	const TCHAR *ptszSection;
	const char *pszService;
	WORD DefHotKey;
	LPARAM lParam;
};

static FontOptionsList g_FontOptionsList[] = {
	{LPGENT(">> Outgoing timestamp"), RGB(0, 0, 0), _T("MS Shell Dlg 2"), DBFONTF_BOLD, -11, LPGENT("Outgoing background")},
	{LPGENT("<< Incoming timestamp"), RGB(0, 0, 0), _T("MS Shell Dlg 2"), DBFONTF_BOLD, -11, LPGENT("Incoming background")},
	{LPGENT(">> Outgoing name"), RGB(100,100,100), _T("MS Shell Dlg 2"), DBFONTF_BOLD, -11, LPGENT("Outgoing background")},
	{LPGENT("<< Incoming name"), RGB(90,160,90), _T("MS Shell Dlg 2"), DBFONTF_BOLD, -11, LPGENT("Incoming background")},
	{LPGENT(">> Outgoing messages"), RGB(0, 0, 0), _T("MS Shell Dlg 2"), 0, -11, LPGENT("Outgoing background")},
	{LPGENT("<< Incoming messages"), RGB(0, 0, 0), _T("MS Shell Dlg 2"), 0, -11, LPGENT("Incoming background")},
};

static ColorOptionsList g_ColorOptionsList[] = {
	LPGENT("Outgoing background"), RGB(245,245,255),
	LPGENT("Incoming background"), RGB(245,255,245),
};

static HotkeyOptionsList g_HotkeyOptionsList[] = {
	{ "basichistory_hot_showall", LPGENT("Open global history"), LPGENT("History"), MS_HISTORY_SHOWCONTACTHISTORY, HOTKEYCODE(HOTKEYF_CONTROL|HOTKEYF_SHIFT, 'H') | HKF_MIRANDA_LOCAL, 0 },
	{ "basichistory_hot_find", LPGENT("Find"), LPGENT("History"), 0, HOTKEYCODE(HOTKEYF_CONTROL, 'F'), HISTORY_HK_FIND },
	{ "basichistory_hot_findnext", LPGENT("Find Next"), LPGENT("History"), 0, VK_F3, HISTORY_HK_FINDNEXT },
	{ "basichistory_hot_findprev", LPGENT("Find Previous"), LPGENT("History"), 0, VK_F2, HISTORY_HK_FINDPREV },
	{ "basichistory_hot_matchcase", LPGENT("Switch Match Case"), LPGENT("History"), 0, 0, HISTORY_HK_MATCHCASE },
	{ "basichistory_hot_matchwhole", LPGENT("Switch Match Whole Word"), LPGENT("History"), 0, 0, HISTORY_HK_MATCHWHOLE },
	{ "basichistory_hot_showcontacts", LPGENT("Show/Hide Contacts"), LPGENT("History"), 0, 0, HISTORY_HK_SHOWCONTACTS },
	{ "basichistory_hot_onlyin", LPGENT("Switch Only Incomming Messages"), LPGENT("History"), 0, 0, HISTORY_HK_ONLYIN },
	{ "basichistory_hot_onlyout", LPGENT("Switch Only Outgoing Messages"), LPGENT("History"), 0, 0, HISTORY_HK_ONLYOUT },
	{ "basichistory_hot_onlygroup", LPGENT("Switch Only Selected Group"), LPGENT("History"), 0, 0, HISTORY_HK_ONLYGROUP },
	{ "basichistory_hot_delete", LPGENT("Delete"), LPGENT("History"), 0, VK_DELETE, HISTORY_HK_DELETE },
};

const int g_fontsSize = SIZEOF(g_FontOptionsList);

const int g_colorsSize = SIZEOF(g_ColorOptionsList);

const int g_hotkeysSize = SIZEOF(g_HotkeyOptionsList);

void Options::Load()
{
	FontIDT fid = {0};
	ColourIDT cid = {0};
	HOTKEYDESC hid = {0};
	fid.cbSize = sizeof(FontIDT);
	cid.cbSize = sizeof(ColourIDT);
	hid.cbSize = sizeof(HOTKEYDESC);
	strncpy(fid.dbSettingsGroup, "BasicHistory_Fonts", SIZEOF(fid.dbSettingsGroup));
	_tcsncpy(fid.backgroundGroup, _T("History"), SIZEOF(fid.backgroundGroup));
	_tcsncpy(fid.group, LPGENT("History"), SIZEOF(fid.group));
	fid.flags = FIDF_DEFAULTVALID | FIDF_ALLOWEFFECTS | FIDF_CLASSGENERAL;
	for(int i = 0; i < g_fontsSize; ++i)
	{
		fid.order = i;
		_tcsncpy(fid.deffontsettings.szFace, g_FontOptionsList[i].szDefFace, LF_FACESIZE);
		fid.deffontsettings.size = g_FontOptionsList[i].defSize; 
		fid.deffontsettings.colour = g_FontOptionsList[i].defColour;
		fid.deffontsettings.style = g_FontOptionsList[i].defStyle;
		fid.deffontsettings.charset = DEFAULT_CHARSET;
		sprintf_s(fid.prefix, SIZEOF(fid.prefix), "Font%d", i);
		_tcsncpy(fid.name, g_FontOptionsList[i].szDescr, SIZEOF(fid.name));
		_tcsncpy(fid.backgroundName, g_FontOptionsList[i].szBackgroundName, SIZEOF(fid.name));
		CallService(MS_FONT_REGISTERT, (WPARAM)&fid, 0);
	}
	
	strncpy(cid.dbSettingsGroup, "BasicHistory_Fonts", SIZEOF(fid.dbSettingsGroup));
	_tcsncpy(cid.group, LPGENT("History"), SIZEOF(fid.group));
	for(int i = 0; i < g_colorsSize; ++i)
	{
		_tcsncpy(cid.name, g_ColorOptionsList[i].tszName, SIZEOF(cid.name));
		sprintf_s(cid.setting, SIZEOF(cid.setting), "Color%d", i);
		cid.order = i;
		cid.defcolour = g_ColorOptionsList[i].def;
		CallService(MS_COLOUR_REGISTERT, (WPARAM)&cid, 0);
	}

	hid.dwFlags = HKD_TCHAR;
	for(int i = 0; i < g_hotkeysSize; ++i)
	{
		hid.pszName = g_HotkeyOptionsList[i].pszName;
		hid.ptszDescription = g_HotkeyOptionsList[i].ptszDescription;
		hid.ptszSection = g_HotkeyOptionsList[i].ptszSection;
		hid.pszService = g_HotkeyOptionsList[i].pszService;
		hid.DefHotKey = g_HotkeyOptionsList[i].DefHotKey;
		hid.lParam = g_HotkeyOptionsList[i].lParam;
		CallService(MS_HOTKEY_REGISTER, 0, (LPARAM)&hid);
	}

	showContacts = DBGetContactSettingByte(0, MODULE, "showContacts", 0) ? true : false;
	groupNewOnTop = DBGetContactSettingByte(0, MODULE, "groupNewOnTop", 1) ? true : false;
	groupShowEvents = DBGetContactSettingByte(0, MODULE, "groupShowEvents", 1) ? true : false;
	groupShowTime = DBGetContactSettingByte(0, MODULE, "groupShowTime", 1) ? true : false;
	groupShowName = DBGetContactSettingByte(0, MODULE, "groupShowName", 0) ? true : false;
	groupShowMessage = DBGetContactSettingByte(0, MODULE, "groupShowMessage", 1) ? true : false;
	groupMessageLen = DBGetContactSettingDword(0, MODULE, "groupMessageLen", 43);
	if(groupMessageLen < 5) groupMessageLen = 5;
	groupTime = DBGetContactSettingDword(0, MODULE, "groupTime", 5);
	if(groupTime < 1) groupTime = 1;
	groupMessagesNumber = DBGetContactSettingDword(0, MODULE, "groupMessagesNumber", 100);
	if(groupMessagesNumber < 1) groupMessagesNumber = 1;
	messagesNewOnTop = DBGetContactSettingByte(0, MODULE, "messagesNewOnTop", 0) ? true : false;
	messagesShowDate = DBGetContactSettingByte(0, MODULE, "messagesShowDate", 0) ? true : false;
	messagesShowSec = DBGetContactSettingByte(0, MODULE, "messagesShowSec", 0) ? true : false;
	messagesShowName = DBGetContactSettingByte(0, MODULE, "messagesShowName", 1) ? true : false;
	messagesShowEvents = DBGetContactSettingByte(0, MODULE, "messagesShowEvents", 0) ? true : false;
	messagesUseSmileys = DBGetContactSettingByte(0, MODULE, "messagesUseSmileys", 1) ? true : false;
	searchForInList = DBGetContactSettingByte(0, MODULE, "searchForInList", 1) ? true : false;
	searchForInMess = DBGetContactSettingByte(0, MODULE, "searchForInMess", 1) ? true : false;
	searchMatchCase = DBGetContactSettingByte(0, MODULE, "searchMatchCase", 0) ? true : false;
	searchMatchWhole = DBGetContactSettingByte(0, MODULE, "searchMatchWhole", 0) ? true : false;
	searchOnlyIn = DBGetContactSettingByte(0, MODULE, "searchOnlyIn", 0) ? true : false;
	searchOnlyOut = DBGetContactSettingByte(0, MODULE, "searchOnlyOut", 0) ? true : false;
	searchOnlyGroup = DBGetContactSettingByte(0, MODULE, "searchOnlyGroup", 0) ? true : false;

	defFilter = DBGetContactSettingByte(0, MODULE, "defFilter", defFilter);
	int filtersCount = DBGetContactSettingDword(0, MODULE, "customFiltersCount", 0);
	for(int i = 0; i < filtersCount; ++i)
	{
		char buf[256];
		FilterOptions fo;
		sprintf_s(buf, "filterName_%d", i);
		DBVARIANT nameV;
		if(!DBGetContactSettingWString(0, MODULE, buf, &nameV))
		{
			fo.name = nameV.pwszVal;
			DBFreeVariant(&nameV);
		}
		else break;
		sprintf_s(buf, "filterInOut_%d", i);
		int inOut = DBGetContactSettingByte(0, MODULE, buf, 0);
		if(inOut == 1)
			fo.onlyIncomming = true;
		else if(inOut == 2)
			fo.onlyOutgoing = true;
		sprintf_s(buf, "filterEvents_%d", i);
		DBVARIANT eventsV;
		if(!DBGetContactSettingString(0, MODULE, buf, &eventsV))
		{
			int k = 0;
			char* id = eventsV.pszVal;
			while(eventsV.pszVal[k])
			{
				if(eventsV.pszVal[k] == ';')
				{
					eventsV.pszVal[k] = 0;
					fo.events.push_back(strtol(id, NULL, 16));
					id = eventsV.pszVal + k + 1;
				}

				++k;
			}
			DBFreeVariant(&eventsV);
		}
		else break;
		
		customFilters.insert(customFilters.end(), fo);
	}

	if(defFilter > 1)
	{
		defFilter = 0;
		
		DBVARIANT defFilterStrV;
		if(!DBGetContactSettingWString(0, MODULE, "defFilterStr", &defFilterStrV))
		{
			std::wstring filterName = defFilterStrV.pwszVal;
			for(int i = 0; i < customFilters.size(); ++i)
			{
				if(filterName == customFilters[i].name)
				{
					defFilter = i + 2;
					break;
				}
			}
			DBFreeVariant(&defFilterStrV);
		}
	}
}

COLORREF Options::GetFont(Fonts fontId, PLOGFONT font)
{
	FontIDT fid = {0};
	fid.cbSize = sizeof(FontIDT);
	_tcsncpy(fid.group, LPGENT("History"), SIZEOF(fid.group));
	_tcsncpy(fid.name, g_FontOptionsList[fontId].szDescr, SIZEOF(fid.name));
	return (COLORREF)CallService(MS_FONT_GETT, (WPARAM)&fid, (LPARAM)font);
}

COLORREF Options::GetColor(Colors colorId)
{
	ColourIDT cid = {0};
	cid.cbSize = sizeof(ColourIDT);
	_tcsncpy(cid.group, LPGENT("History"), SIZEOF(cid.group));
	_tcsncpy(cid.name, g_ColorOptionsList[colorId].tszName, SIZEOF(cid.name));
	return (COLORREF)CallService(MS_COLOUR_GETT, (WPARAM)&cid, NULL);
}

void Options::Save()
{
	DBWriteContactSettingByte(0, MODULE, "showContacts", showContacts ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "groupNewOnTop", groupNewOnTop ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "groupShowEvents", groupShowEvents ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "groupShowTime", groupShowTime ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "groupShowName", groupShowName ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "groupShowMessage", groupShowMessage ? 1 : 0);
	if(groupMessageLen < 5) groupMessageLen = 5;
	DBWriteContactSettingDword(0, MODULE, "groupMessageLen", groupMessageLen);
	if(groupTime < 1) groupTime = 1;
	DBWriteContactSettingDword(0, MODULE, "groupTime", groupTime);
	if(groupMessagesNumber < 1) groupMessagesNumber = 1;
	DBWriteContactSettingDword(0, MODULE, "groupMessagesNumber", groupMessagesNumber);
	DBWriteContactSettingByte(0, MODULE, "messagesNewOnTop", messagesNewOnTop ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "messagesShowDate", messagesShowDate ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "messagesShowSec", messagesShowSec ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "messagesShowName", messagesShowName ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "messagesShowEvents", messagesShowEvents ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "messagesUseSmileys", messagesUseSmileys ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "searchForInList", searchForInList ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "searchForInMess", searchForInMess ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "searchMatchCase", searchMatchCase ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "searchMatchWhole", searchMatchWhole ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "searchOnlyIn", searchOnlyIn ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "searchOnlyOut", searchOnlyOut ? 1 : 0);
	DBWriteContactSettingByte(0, MODULE, "searchOnlyGroup", searchOnlyGroup ? 1 : 0);
	if(defFilter < 0 || defFilter - 2 >= customFilters.size()) defFilter = 0;
	DBWriteContactSettingByte(0, MODULE, "defFilter", defFilter < 2 ? defFilter : 2);
	if(defFilter >= 2)
		DBWriteContactSettingWString(0, MODULE, "defFilterStr", customFilters[defFilter - 2].name.c_str());
	DBWriteContactSettingDword(0, MODULE, "customFiltersCount", customFilters.size());
	for(int i = 0 ; i < customFilters.size(); ++i)
	{
		char buf[256];
		sprintf_s(buf, "filterName_%d", i);
		DBWriteContactSettingWString(0, MODULE, buf, customFilters[i].name.c_str());
		sprintf_s(buf, "filterInOut_%d", i);
		DBWriteContactSettingByte(0, MODULE, buf, customFilters[i].onlyIncomming ? 1 : (customFilters[i].onlyOutgoing ? 2 : 0));
		std::string events;
		for(std::vector<int>::iterator it = customFilters[i].events.begin(); it != customFilters[i].events.end(); ++it)
		{
			_itoa_s(*it, buf, 16);
			events += buf;
			events += ";";
		}

		sprintf_s(buf, "filterEvents_%d", i);
		DBWriteContactSettingString(0, MODULE, buf, events.c_str());
	}
}

void OptionsGroupChanged();
void OptionsMessageChanged();
void OptionsSearchingChanged();

void SetEventCB(HWND hwndCB, int eventId)
{
	int cpCount = SIZEOF(EventNames);
	int selCpIdx = -1;
	for(int i = 0; i < cpCount; ++i)
	{
		if(EventNames[i].id == eventId)
			selCpIdx = i;
	}

	if(selCpIdx == -1)
	{
		TCHAR buf[24];
		_stprintf_s(buf, 24, _T("%d"), eventId);
		ComboBox_SetText(hwndCB, buf);	
	}
	else
	{
		ComboBox_SetCurSel(hwndCB, selCpIdx);	
	}
}

int GetEventCB(HWND hwndCB, bool errorReport, int &eventId)
{
	int selCpIdx = ComboBox_GetCurSel(hwndCB);
	if(selCpIdx < 0)
	{
		TCHAR text[24];
		ComboBox_GetText(hwndCB, text, 24);
		TCHAR * stopOn = NULL;
		long cp = _tcstol(text, &stopOn, 10);
		if(errorReport && (stopOn == text || *stopOn != '\0' || cp < 0))
		{
			MessageBox(GetParent(hwndCB), TranslateT("Invalid event number"), TranslateT("Error"), MB_OK | MB_ICONERROR);
			SetFocus(hwndCB);
			return -1;
		}

		eventId = cp;
	}
	else if(selCpIdx > 1)
		eventId = EventNames[selCpIdx - 2].id;
	else
		return selCpIdx + 1;
	
	return 0;
}

void ClearLB(HWND hwndLB)
{
	while(ListBox_GetCount(hwndLB) > 0)
		ListBox_DeleteString(hwndLB, 0);
}

void ReloadEventLB(HWND hwndLB, const FilterOptions &sel)
{
	while(ListBox_GetCount(hwndLB) > 0)
		ListBox_DeleteString(hwndLB, 0);
	if(sel.onlyIncomming && !sel.onlyOutgoing)
	{
		ListBox_AddString(hwndLB, TranslateT("Incoming events"));
	}
	else if(sel.onlyOutgoing && !sel.onlyIncomming)
	{
		ListBox_AddString(hwndLB, TranslateT("Outgoing events"));
	}

	for(std::vector<int>::const_iterator it = sel.events.begin(); it != sel.events.end(); ++it)
	{
		int cpCount = SIZEOF(EventNames);
		int selCpIdx = -1;
		for(int i = 0; i < cpCount; ++i)
		{
			if(EventNames[i].id == *it)
				selCpIdx = i;
		}
		if(selCpIdx == -1)
		{
			TCHAR buf[24];
			_stprintf_s(buf, 24, _T("%d"), *it);
			ListBox_AddString(hwndLB, buf);	
		}
		else
		{
			ListBox_AddString(hwndLB, TranslateTS(EventNames[selCpIdx].name));	
		}
	}
}

INT_PTR CALLBACK Options::DlgProcOptsMain(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) 
	{
		case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);
			CheckDlgButton(hwndDlg, IDC_SHOWCONTACTS, instance->showContacts ? 1 : 0);
			HWND events = GetDlgItem(hwndDlg, IDC_EVENT);
			HWND defFilter = GetDlgItem(hwndDlg, IDC_DEFFILTER);
				HWND listFilter = GetDlgItem(hwndDlg, IDC_LIST_FILTERS);
			ComboBox_AddString(events, TranslateT("Incoming events"));
			ComboBox_AddString(events, TranslateT("Outgoing events"));
			for(int i = 0 ; i < SIZEOF(EventNames); ++i)
			{
				ComboBox_AddString(events, TranslateTS(EventNames[i].name));
			}

			ComboBox_AddString(defFilter, TranslateT("Default history events"));
			ComboBox_AddString(defFilter, TranslateT("All events"));
			Edit_LimitText(GetDlgItem(hwndDlg, IDC_FILTER_NAME), 20);
			ComboBox_LimitText(events, 20);

			instance->customFiltersTemp.clear();
			instance->customFiltersTemp.insert(instance->customFiltersTemp.begin(), instance->customFilters.begin(), instance->customFilters.end());
			for(std::vector<FilterOptions>::iterator it = instance->customFiltersTemp.begin(); it != instance->customFiltersTemp.end(); ++it)
			{
				ComboBox_AddString(defFilter, it->name.c_str());
				ListBox_AddString(listFilter, it->name.c_str());
			}
			ComboBox_SetCurSel(defFilter, instance->defFilter);
			EnableWindow(GetDlgItem(hwndDlg, IDC_DELETE_FILTER), FALSE);
			EnableWindow(GetDlgItem(hwndDlg, IDC_EVENT), FALSE);
			EnableWindow(GetDlgItem(hwndDlg, IDC_ADD_EVENT), FALSE);
			EnableWindow(GetDlgItem(hwndDlg, IDC_DELETE_EVENT), FALSE);
			return TRUE;
		}
		case WM_COMMAND:
		{
			if (HIWORD(wParam) == BN_CLICKED)
			{
				HWND listFilter = GetDlgItem(hwndDlg, IDC_LIST_FILTERS);
				HWND listEvents = GetDlgItem(hwndDlg, IDC_LIST_EVENTS);
				HWND nameFilter = GetDlgItem(hwndDlg, IDC_FILTER_NAME);
				HWND defFilter = GetDlgItem(hwndDlg, IDC_DEFFILTER);
				HWND eventCB = GetDlgItem(hwndDlg, IDC_EVENT);
				switch(LOWORD(wParam))
				{
				case IDC_ADD_FILTER:
					{
						TCHAR name[24];
						Edit_GetText(nameFilter, name, 24);
						if(name[0] == 0)
						{
							MessageBox(hwndDlg, TranslateT("Enter filter name"), TranslateT("Error"), MB_ICONERROR);
							return TRUE;
						}
						
						FilterOptions fo(name);
						for(std::vector<FilterOptions>::iterator it = instance->customFiltersTemp.begin(); it != instance->customFiltersTemp.end(); ++it)
						{
							if(it->name == fo.name)
							{
								MessageBox(hwndDlg, TranslateT("Filter name exists"), TranslateT("Error"), MB_ICONERROR);
								return TRUE;
							}
						}

						instance->customFiltersTemp.insert(instance->customFiltersTemp.end(), fo);
						int i = ListBox_AddString(listFilter, name);
						ListBox_SetCurSel(listFilter, i);
						ComboBox_AddString(defFilter, name);
						name[0] = 0;
						Edit_SetText(nameFilter, name);
						ReloadEventLB(listEvents, instance->customFiltersTemp[i]);
						EnableWindow(GetDlgItem(hwndDlg, IDC_DELETE_FILTER), TRUE);
						EnableWindow(GetDlgItem(hwndDlg, IDC_EVENT), TRUE);
						EnableWindow(GetDlgItem(hwndDlg, IDC_ADD_EVENT), TRUE);
						EnableWindow(GetDlgItem(hwndDlg, IDC_DELETE_EVENT), FALSE);
					}
					break;
				case IDC_DELETE_FILTER:
					{
						int sel = ListBox_GetCurSel(listFilter);
						if(sel < 0)
							return TRUE;
						for(int i = sel; i < instance->customFiltersTemp.size() - 1; ++i)
						{
							instance->customFiltersTemp[i] = instance->customFiltersTemp[i + 1];
						}

						instance->customFiltersTemp.resize(instance->customFiltersTemp.size() - 1);
						ListBox_DeleteString(listFilter, sel);
						ComboBox_DeleteString(defFilter, sel + 2);
						if(ComboBox_GetCurSel(defFilter) < 0)
						{
							ComboBox_SetCurSel(defFilter, 0);
						}
						
						ClearLB(listEvents);
						EnableWindow(GetDlgItem(hwndDlg, IDC_DELETE_FILTER), FALSE);
						EnableWindow(GetDlgItem(hwndDlg, IDC_EVENT), FALSE);
						EnableWindow(GetDlgItem(hwndDlg, IDC_ADD_EVENT), FALSE);
						EnableWindow(GetDlgItem(hwndDlg, IDC_DELETE_EVENT), FALSE);
					}
					break;
				case IDC_ADD_EVENT:
					{
						int sel = ListBox_GetCurSel(listFilter);
						if(sel < 0)
							return TRUE;
						int eventId;
						int selCB = GetEventCB(eventCB, true, eventId);
						if(selCB < 0)
							return TRUE;
						if(selCB == 1)
						{
							if(instance->customFiltersTemp[sel].onlyIncomming)
							{
								MessageBox(hwndDlg, TranslateT("Event already exists"), TranslateT("Error"), MB_ICONERROR);
								return TRUE;
							}

							if(instance->customFiltersTemp[sel].onlyOutgoing)
								instance->customFiltersTemp[sel].onlyOutgoing = false;
							else
								instance->customFiltersTemp[sel].onlyIncomming = true;
						}
						else if(selCB == 2)
						{
							if(instance->customFiltersTemp[sel].onlyOutgoing)
							{
								MessageBox(hwndDlg, TranslateT("Event already exists"), TranslateT("Error"), MB_ICONERROR);
								return TRUE;
							}
							
							if(instance->customFiltersTemp[sel].onlyIncomming)
								instance->customFiltersTemp[sel].onlyIncomming = false;
							else
								instance->customFiltersTemp[sel].onlyOutgoing = true;
						}
						else
						{
							if(std::find(instance->customFiltersTemp[sel].events.begin(), instance->customFiltersTemp[sel].events.end(), eventId) != instance->customFiltersTemp[sel].events.end())
							{
								MessageBox(hwndDlg, TranslateT("Event already exists"), TranslateT("Error"), MB_ICONERROR);
								return TRUE;
							}

							instance->customFiltersTemp[sel].events.push_back(eventId);
						}

						ReloadEventLB(listEvents, instance->customFiltersTemp[sel]);
						ComboBox_SetCurSel(eventCB, -1);
						EnableWindow(GetDlgItem(hwndDlg, IDC_DELETE_EVENT), FALSE);
					}
					break;
				case IDC_DELETE_EVENT:
					{
						int sel = ListBox_GetCurSel(listFilter);
						if(sel < 0)
							return TRUE;
						int eventSel = ListBox_GetCurSel(listEvents);
						if(eventSel < 0)
							return TRUE;
						int stId = 0;
						if(instance->customFiltersTemp[sel].onlyIncomming || instance->customFiltersTemp[sel].onlyOutgoing)
							++stId;
						if(eventSel >= stId)
						{
							--eventSel;
							for(int i = eventSel; i < instance->customFiltersTemp[sel].events.size() - 1; ++i)
							{
								instance->customFiltersTemp[sel].events[i] = instance->customFiltersTemp[sel].events[i + 1];
							}

							instance->customFiltersTemp[sel].events.resize(instance->customFiltersTemp[sel].events.size() - 1);
						}
						else
						{
							instance->customFiltersTemp[sel].onlyIncomming = false;
							instance->customFiltersTemp[sel].onlyOutgoing = false;
						}

						ReloadEventLB(listEvents, instance->customFiltersTemp[sel]);
						EnableWindow(GetDlgItem(hwndDlg, IDC_DELETE_EVENT), FALSE);
					}
					break;
				}
			}
			else if(HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == IDC_LIST_FILTERS)
			{
				HWND listFilter = GetDlgItem(hwndDlg, IDC_LIST_FILTERS);
				HWND listEvents = GetDlgItem(hwndDlg, IDC_LIST_EVENTS);
				int sel = ListBox_GetCurSel(listFilter);
				if(sel < 0)
					ClearLB(listEvents);
				else
					ReloadEventLB(listEvents, instance->customFiltersTemp[sel]);
				EnableWindow(GetDlgItem(hwndDlg, IDC_DELETE_FILTER), sel >= 0);
				EnableWindow(GetDlgItem(hwndDlg, IDC_EVENT), sel >= 0);
				EnableWindow(GetDlgItem(hwndDlg, IDC_ADD_EVENT), sel >= 0);
				EnableWindow(GetDlgItem(hwndDlg, IDC_DELETE_EVENT), FALSE);
			}
			else if(HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == IDC_LIST_EVENTS)
			{
				HWND listEvents = GetDlgItem(hwndDlg, IDC_LIST_EVENTS);
				int sel = ListBox_GetCurSel(listEvents);
				EnableWindow(GetDlgItem(hwndDlg, IDC_DELETE_EVENT), sel >= 0);
			}

			if (HIWORD(wParam) == BN_CLICKED || (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == IDC_DEFFILTER))
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			return TRUE;
		}
		case WM_NOTIFY:
		{
			if(((LPNMHDR)lParam)->code == PSN_APPLY) 
			{
				instance->showContacts = IsDlgButtonChecked(hwndDlg, IDC_SHOWCONTACTS) ? true : false;
				instance->defFilter = ComboBox_GetCurSel(GetDlgItem(hwndDlg, IDC_DEFFILTER));
				instance->customFilters.clear();
				instance->customFilters.insert(instance->customFilters.begin(), instance->customFiltersTemp.begin(), instance->customFiltersTemp.end());
				Options::instance->Save();
				OptionsGroupChanged();
			}
			return TRUE;
		}
	}

	return FALSE;
}

class OptsData
{
public:
	OptsData()
	{
		init = false;
	}

	bool init;
};

INT_PTR CALLBACK Options::DlgProcOptsGroupList(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) 
	{
		case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);
			OptsData* optsData = new OptsData();
			SetWindowLongPtr(hwndDlg, GWLP_USERDATA, (LONG_PTR)optsData);

			CheckDlgButton(hwndDlg, IDC_NEWONTOP, instance->groupNewOnTop ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_SHOWEVENTS, instance->groupShowEvents ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_SHOWTIME, instance->groupShowTime ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_SHOWNAME, instance->groupShowName ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_SHOWMESSAGE, instance->groupShowMessage ? 1 : 0);
			Edit_LimitText(GetDlgItem(hwndDlg, IDC_MESSAGELEN), 4);
			SetDlgItemInt(hwndDlg, IDC_MESSAGELEN, instance->groupMessageLen, FALSE);
			SendMessage(hwndDlg, WM_COMMAND, MAKEWPARAM(IDC_SHOWMESSAGE, BN_CLICKED), NULL);
			Edit_LimitText(GetDlgItem(hwndDlg, IDC_GROUPTIME), 2);
			Edit_LimitText(GetDlgItem(hwndDlg, IDC_LIMITMESSAGES), 3);
			SetDlgItemInt(hwndDlg, IDC_GROUPTIME, instance->groupTime, FALSE);
			SetDlgItemInt(hwndDlg, IDC_LIMITMESSAGES, instance->groupMessagesNumber, FALSE);

			optsData->init = true;
			return TRUE;
		}
		case WM_COMMAND:
		{
			OptsData* optsData = (OptsData*)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
			if(HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDC_SHOWMESSAGE)
			{
				if(IsDlgButtonChecked(hwndDlg, IDC_SHOWMESSAGE))
				{
					EnableWindow(GetDlgItem(hwndDlg, IDC_MESSAGELEN), TRUE);
					EnableWindow(GetDlgItem(hwndDlg, IDC_MESSAGELEN_DESC), TRUE);
				}
				else
				{
					EnableWindow(GetDlgItem(hwndDlg, IDC_MESSAGELEN), FALSE);
					EnableWindow(GetDlgItem(hwndDlg, IDC_MESSAGELEN_DESC), FALSE);
				}
			}
			if (optsData->init && (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == EN_CHANGE))
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			return TRUE;
		}
		case WM_NOTIFY:
		{
			if(((LPNMHDR)lParam)->code == PSN_APPLY) 
			{
				instance->groupNewOnTop = IsDlgButtonChecked(hwndDlg, IDC_NEWONTOP) ? true : false;
				instance->groupShowEvents = IsDlgButtonChecked(hwndDlg, IDC_SHOWEVENTS) ? true : false;
				instance->groupShowTime = IsDlgButtonChecked(hwndDlg, IDC_SHOWTIME) ? true : false;
				instance->groupShowName = IsDlgButtonChecked(hwndDlg, IDC_SHOWNAME) ? true : false;
				instance->groupShowMessage = IsDlgButtonChecked(hwndDlg, IDC_SHOWMESSAGE) ? true : false;
				BOOL success;
				instance->groupMessageLen = GetDlgItemInt(hwndDlg, IDC_MESSAGELEN, &success, FALSE);
				instance->groupTime = GetDlgItemInt(hwndDlg, IDC_GROUPTIME, &success, FALSE);
				instance->groupMessagesNumber = GetDlgItemInt(hwndDlg, IDC_LIMITMESSAGES, &success, FALSE);
				
				Options::instance->Save();
				OptionsGroupChanged();
			}
			return TRUE;
		}
	}

	return FALSE;
}

INT_PTR CALLBACK Options::DlgProcOptsMessages(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) 
	{
		case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);

			CheckDlgButton(hwndDlg, IDC_NEWONTOP, instance->messagesNewOnTop ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_SHOWDATE, instance->messagesShowDate ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_SHOWSECOND, instance->messagesShowSec ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_SHOWNAME, instance->messagesShowName ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_SHOWEVENTS, instance->messagesShowEvents ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_SHOWSMILEYS, instance->messagesUseSmileys ? 1 : 0);
			if(!g_SmileyAddAvail)
				EnableWindow(GetDlgItem(hwndDlg, IDC_SHOWSMILEYS), FALSE);
			return TRUE;
		}
		case WM_COMMAND:
		{
			if (HIWORD(wParam) == BN_CLICKED)
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			return TRUE;
		}
		case WM_NOTIFY:
		{
			if(((LPNMHDR)lParam)->code == PSN_APPLY) 
			{
				instance->messagesNewOnTop = IsDlgButtonChecked(hwndDlg, IDC_NEWONTOP) ? true : false;
				instance->messagesShowDate = IsDlgButtonChecked(hwndDlg, IDC_SHOWDATE) ? true : false;
				instance->messagesShowSec = IsDlgButtonChecked(hwndDlg, IDC_SHOWSECOND) ? true : false;
				instance->messagesShowName = IsDlgButtonChecked(hwndDlg, IDC_SHOWNAME) ? true : false;
				instance->messagesShowEvents = IsDlgButtonChecked(hwndDlg, IDC_SHOWEVENTS) ? true : false;
				instance->messagesUseSmileys = IsDlgButtonChecked(hwndDlg, IDC_SHOWSMILEYS) ? true : false;
				
				Options::instance->Save();
				OptionsMessageChanged();
			}
			return TRUE;
		}
	}

	return FALSE;
}

INT_PTR CALLBACK Options::DlgProcOptsSearching(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) 
	{
		case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);

			CheckDlgButton(hwndDlg, IDC_FORLIST, instance->searchForInList ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_FORMES, instance->searchForInMess ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_MATCHCASE, instance->searchMatchCase ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_MATCHWHOLE, instance->searchMatchWhole ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_ONLYIN, instance->searchOnlyIn ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_ONLYOUT, instance->searchOnlyOut ? 1 : 0);
			CheckDlgButton(hwndDlg, IDC_ONLYGROUP, instance->searchOnlyGroup ? 1 : 0);
			return TRUE;
		}
		case WM_COMMAND:
		{
			if(HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDC_ONLYIN)
			{
				if(IsDlgButtonChecked(hwndDlg, IDC_ONLYIN) && IsDlgButtonChecked(hwndDlg, IDC_ONLYOUT))
				{
					CheckDlgButton(hwndDlg, IDC_ONLYOUT, 0);
				}
			}
			else if(HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDC_ONLYOUT)
			{
				if(IsDlgButtonChecked(hwndDlg, IDC_ONLYOUT) && IsDlgButtonChecked(hwndDlg, IDC_ONLYIN))
				{
					CheckDlgButton(hwndDlg, IDC_ONLYIN, 0);
				}
			}

			if (HIWORD(wParam) == BN_CLICKED)
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			return TRUE;
		}
		case WM_NOTIFY:
		{
			if(((LPNMHDR)lParam)->code == PSN_APPLY) 
			{
				instance->searchForInList = IsDlgButtonChecked(hwndDlg, IDC_FORLIST) ? true : false;
				instance->searchForInMess = IsDlgButtonChecked(hwndDlg, IDC_FORMES) ? true : false;
				instance->searchMatchCase = IsDlgButtonChecked(hwndDlg, IDC_MATCHCASE) ? true : false;
				instance->searchMatchWhole = IsDlgButtonChecked(hwndDlg, IDC_MATCHWHOLE) ? true : false;
				instance->searchOnlyIn = IsDlgButtonChecked(hwndDlg, IDC_ONLYIN) ? true : false;
				instance->searchOnlyOut = IsDlgButtonChecked(hwndDlg, IDC_ONLYOUT) ? true : false;
				instance->searchOnlyGroup = IsDlgButtonChecked(hwndDlg, IDC_ONLYGROUP) ? true : false;
				
				Options::instance->Save();
				OptionsSearchingChanged();
			}
			return TRUE;
		}
	}

	return FALSE;
}
