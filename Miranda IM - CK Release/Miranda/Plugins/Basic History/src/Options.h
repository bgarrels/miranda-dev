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

#pragma once

struct FilterOptions
{
	FilterOptions()
		:onlyIncomming(false),
		onlyOutgoing(false)
	{
	}

	FilterOptions(const std::wstring& _name)
		:name(_name),
		onlyIncomming(false),
		onlyOutgoing(false)
	{
	}

	std::wstring name;
	std::vector<int> events;
	bool onlyIncomming;
	bool onlyOutgoing;
};

class Options
{
private:
	std::vector<FilterOptions> customFiltersTemp;
public:
	Options();
	~Options();
	static int InitOptions(WPARAM wParam, LPARAM lParam);

	static INT_PTR CALLBACK DlgProcOptsMain(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK DlgProcOptsGroupList(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK DlgProcOptsMessages(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK DlgProcOptsSearching(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK DlgProcOptsExport(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

	static Options *instance;
	void Save();
	void Load();

	bool showContacts, showContactGroups, groupNewOnTop, groupShowEvents, groupShowTime, groupShowName, groupShowMessage;
	bool messagesNewOnTop, messagesShowDate, messagesShowSec, messagesShowName, messagesShowEvents, messagesUseSmileys;
	bool searchForInList, searchForInMess, searchMatchCase, searchMatchWhole, searchOnlyIn, searchOnlyOut, searchOnlyGroup;
	int groupMessageLen, groupTime, groupMessagesNumber;

	std::vector<FilterOptions> customFilters;
	int defFilter;
	unsigned int codepageTxt, codepageHtml1, codepageHtml2;
	std::wstring encodingTxt, encodingHtml1, encodingHtml2;
	bool exportHtml1ShowDate, exportHtml2ShowDate, exportHtml2UseSmileys;
	std::wstring extCssHtml2;

	enum Fonts
	{
		OutTimestamp = 0,
		InTimestamp,
		OutName,
		InName,
		OutMessages,
		InMessages,
		GroupList,
	};

	enum Colors
	{
		OutBackground = 0,
		InBackground,
		GroupListBackground,
		WindowBackground,
	};

	COLORREF GetFont(Fonts fontId, PLOGFONT font);
	COLORREF GetColor(Colors colorId);
};

