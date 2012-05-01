#ifndef M_TICKER_H
#define M_TICKER_H

/*
Miranda IM: the free IM client for Microsoft* Windows*

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

===============================================================================
*/

/* TICKER_ADD_STRING
wParam = (WPARAM)(*TICKERDATA)TickerDataAddress (see below)
lParam = 0... not used..
returns (int)TickerID on success or 0 if it fails
*/
#define TICKER_ADD_STRING "Ticker/AddString"

/* TICKER_ADD_SIMPLESTRING
wParam = (char*) szMessage // the text to display
lParam = (char*) szTooltip // the Items tooltip (can be NULL)
this service function will use default values for the rest of the TICKERDATA values 
returns (int)TickerID on success or 0 if it fails
*/
#define TICKER_ADD_SIMPLESTRING "Ticker/AddSimpleString"

/* TICKER_ADD_FROMPOPUPPLUGIN
This should only be called from the popup plugin before (or after.. makes no difference)
the popup is created and the user wants to display the ticker
wParam = (WPARAM)(*POPUPDATA)PopUpDataAddress
lParam = 0
returns (int)TickerID on success or 0 if it fails
*/
#define TICKER_ADD_FROMPOPUPPLUGIN "Ticker/AddFromPopupPlugin"


typedef struct  {
	char* szMessage;		// the message you want to display,		will be copied to another mem address by me, so u can free your copy
	char* szTooltip;		// a message to display in the items tooltip, can be NULL  will be copied to another mem address by me, so u can free your copy
	COLORREF crForeground;	// the foreground colour.. can be NULL
	COLORREF crBackground;	// the background colour.. can be NULL
	WNDPROC PluginWindowProc; // for the window proc.. can be NULL  -->- these are more for popup compatability but someone might find it usefull
	HANDLE hContact;		// can be NULL                          -/
	void * PluginData;		// other plugins might want this...    -/
} TICKERDATA, *LPTICKERDATA;


#endif
