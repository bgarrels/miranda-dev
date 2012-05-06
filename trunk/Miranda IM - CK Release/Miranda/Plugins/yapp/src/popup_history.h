#ifndef __popup_history_h__
#define __popup_history_h__

/*
YAPP for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
            Copyright (C) 2005-2006 Scott Ellis

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

#include "m_ieview.h" //need this for special renderers

/****HISTORY ++ STUFF ***/
//there's no include file for h++ yet
#ifndef MS_HPP_EG_WINDOW
#define MS_HPP_EG_WINDOW "History++/ExtGrid/NewWindow"	
#endif

#ifndef MS_HPP_EG_EVENT
#define MS_HPP_EG_EVENT "History++/ExtGrid/Event"
#endif

#ifndef MS_HPP_EG_NAVIGATE
#define MS_HPP_EG_NAVIGATE "History++/ExtGrid/Navigate"
#endif

#ifndef MS_HPP_EG_OPTIONSCHANGED
#define MS_HPP_EG_OPTIONSCHANGED "History++/ExtGrid/OptionsChanged"
#endif
/************************/


#define HISTORY_SIZE 200 //number of popup history items

#define PHDF_UNICODE 1 

#define POPUPMENU_TITLE     100
#define POPUPMENU_MESSAGE   101
#define POPUPMENU_TIMESTAMP 102

#define RENDER_DEFAULT   0x00000
#define RENDER_HISTORYPP 0x00001
#define RENDER_IEVIEW    0x00002

struct PopupHistoryData{
	DWORD flags; //PHDF_* flags
	union{
		char *message;
		wchar_t *messageW;
		TCHAR *messageT;
	};
	union{
		char *title;
		wchar_t *titleW;
		TCHAR *titleT;
	};
	time_t timestamp;
};

class PopupHistoryList{
	private:
		PopupHistoryData *historyData; //historyData[0] - oldest, historyData[size - 1] - newest
		int count;
		int size;
		int renderer;
		
		void DeleteData(int index);
		void AddItem(PopupHistoryData item); //adds a PopupHistoryData item
		void RemoveItem(int index);
		
	public:
		PopupHistoryList(int renderer = RENDER_DEFAULT);
		~PopupHistoryList();
		
		int GetRenderer() { return renderer; }
		void SetRenderer(int newRenderer) { renderer = newRenderer; }
		
		void Add(char *title, char *message, time_t timestamp);
		void Add(wchar_t *title, wchar_t *message, time_t timestamp);
		
		PopupHistoryData *Get(int index);
		
		void Clear();
		int Count() { return count; }
		int Size() { return size; }
};

/*
Shows a history with the last popups.
Useful if you've missed a popup when it appeared.
wParam - 0
lParam - 0
*/
#define MS_POPUP_SHOWHISTORY "PopUp/ShowHistory"

extern PopupHistoryList lstPopupHistory; //defined in main.cpp
extern HWND hHistoryWindow; //the history window
extern HICON hiPopupHistory;
void RefreshPopupHistory(HWND hWnd, int renderer);

INT_PTR CALLBACK DlgProcHistLst(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif //__popup_history_h__
