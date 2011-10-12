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
#include "Searcher.h"

class HistoryWindow : public SearchContext
{
private:
	HistoryWindow(HANDLE _hContact);
	void Initialise();
	void Destroy();
	void SplitterMoved(HWND splitter, LONG pos);
	static INT_PTR CALLBACK DlgProcHistory(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK SplitterSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static int HistoryDlgResizer(HWND, LPARAM, UTILRESIZECONTROL *urc);
	static void FillHistoryThread(void* param);
	static void Close(HistoryWindow* historyWindow);
	static void ChangeToFreeWindow(HistoryWindow* historyWindow);
	void ReplaceIcons(HWND hwndDlg, int selStart, BOOL isSent);
	void AddGroup(DBEVENTINFO *dbei);
	void InitNames();
	void EnableWindows(BOOL enable);
	void ReloadContacts();
	bool DoHotkey(UINT msg, LPARAM lParam, WPARAM wParam, int window);
	void RestorePos();
	void SavePos(bool all);
	void FindToolbarClicked(LPNMTOOLBAR lpnmTB);
	void ConfigToolbarClicked(LPNMTOOLBAR lpnmTB);
	void DeleteToolbarClicked(LPNMTOOLBAR lpnmTB);
	void Delete(int what);
	void ContactChanged(bool sync = false);
	void GroupImagesChanged();
	void FormatQuote(std::wstring& quote, const MessageData& md, const std::wstring& msg);

	static std::map<HANDLE, HistoryWindow*> windows;
	static std::vector<HistoryWindow*> freeWindows;
	HANDLE hContact;
	bool isDestroyed;
	TCHAR contactName[256];
	TCHAR myName[256];
	LONG splitterY;
	LONG splitterOrgY;
	LONG splitterX;
	LONG splitterOrgX;
	HICON inIco, outIco, plusIco, minusIco, findNextIco, findPrevIco, configIco, deleteIco;
	WNDPROC OldSplitterProc;
	bool isContactList;
	LONG listOryginalPos;
	bool isLoading;
	Searcher searcher;
	bool isGroupImages;
	HIMAGELIST himlSmall, himlNone;
public:
	~HistoryWindow();
	static void Deinit();
	static void Open(HANDLE hContact);
	static void Close(HANDLE hContact);
	static int FontsChanged(WPARAM wParam, LPARAM lParam);
	static void OptionsGroupChanged();
	static void OptionsSearchingChanged();
	void Show();
	void Focus();

	// SearchContext interface
	virtual void SelectEventGroup(int sel);
};

