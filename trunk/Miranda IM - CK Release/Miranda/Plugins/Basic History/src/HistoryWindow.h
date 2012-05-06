/*
Basic History plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2011-2012 Krzysztof Kral

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

#pragma once
#include "Searcher.h"

class HistoryWindow : public SearchContext
{
private:
	HistoryWindow(HANDLE _hContact);
	void Initialise();
	void Destroy();
	void SplitterMoved(HWND splitter, LONG pos, bool screenPos);
	static INT_PTR CALLBACK DlgProcHistory(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK SplitterSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static int HistoryDlgResizer(HWND, LPARAM, UTILRESIZECONTROL *urc);
	static void FillHistoryThread(void* param);
	static void Close(HistoryWindow* historyWindow);
	static void ChangeToFreeWindow(HistoryWindow* historyWindow);
	void ReplaceIcons(HWND hwndDlg, int selStart, BOOL isSent);
	void EnableWindows(BOOL enable);
	void ReloadContacts();
	bool DoHotkey(UINT msg, LPARAM lParam, WPARAM wParam, int window);
	void RestorePos();
	void SavePos(bool all);
	void FindToolbarClicked(LPNMTOOLBAR lpnmTB);
	void ConfigToolbarClicked(LPNMTOOLBAR lpnmTB);
	void DeleteToolbarClicked(LPNMTOOLBAR lpnmTB);
	void Delete(int what);
	bool ContactChanged(bool sync = false);
	void GroupImagesChanged();
	void FormatQuote(std::wstring& quote, const MessageData& md, const std::wstring& msg);
	void FontsChanged();
	void ReloadMainOptions();
	void DoImport(IImport::ImportType type);

	static std::map<HANDLE, HistoryWindow*> windows;
	static std::vector<HistoryWindow*> freeWindows;
	bool isDestroyed;
	LONG splitterY;
	LONG splitterOrgY;
	LONG splitterX;
	LONG splitterOrgX;
	HICON *eventIcoms;
	int allIconNumber;
	HICON plusIco, minusIco, findNextIco, findPrevIco, configIco, deleteIco;
	WNDPROC OldSplitterProc;
	bool isContactList;
	LONG listOryginalPos;
	bool isLoading;
	Searcher searcher;
	bool isGroupImages;
	HIMAGELIST himlSmall, himlNone;
	HBRUSH bkBrush;
protected:
	virtual void AddGroup(bool isMe, const std::wstring &time, const std::wstring &user, const std::wstring &eventText, int ico);
public:
	~HistoryWindow();
	static void Deinit();
	static void Open(HANDLE hContact);
	static void Close(HANDLE hContact);
	static void RebuildEvents(HANDLE hContact);
	static bool IsInList(HWND hWnd);
	static int FontsChanged(WPARAM wParam, LPARAM lParam);
	static INT_PTR DeleteAllUserHistory(WPARAM wParam, LPARAM lParam);
	static void OptionsMainChanged();
	static void OptionsGroupChanged();
	static void OptionsSearchingChanged();
	void Show();
	void Focus();

	// SearchContext interface
	virtual void SelectEventGroup(int sel);
};

