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

#pragma once


#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#if defined( UNICODE ) && !defined( _UNICODE )
#define _UNICODE
#endif

#include <m_stdhdr.h>

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <commctrl.h>
#include <time.h>

#define MIRANDA_VER    0x0A00

#include <newpluginapi.h>
#include <statusmodes.h>
#include <m_database.h>
#include <m_langpack.h>
#include <m_options.h>
#include <m_system.h>
#include <m_system_cpp.h>
#include <m_idle.h>
#include <m_skin.h>
#include <m_clui.h>
#include <m_clist.h>
#include <m_utils.h>
#include <m_fontservice.h>
#include <m_avatars.h>
#include <m_popup.h>
#include <win2k.h>

#include "m_updater.h"

#include <m_notify.h>

#define MODULE			"YAPP"

extern HMODULE hInst;
extern PLUGINLINK *pluginLink;

extern HFONT hFontFirstLine, hFontSecondLine, hFontTime;
extern COLORREF	colFirstLine, colSecondLine, colBg, colTime, colBorder, colSidebar, colTitleUnderline;

extern MNOTIFYLINK *notifyLink;

// work around a bug in neweventnotify, possibly httpserver
// ignore the address passed to the 'get plugin data' service
extern bool ignore_gpd_passed_addy;
 
// win32 defines for mingw version of windows headers :(
#ifndef LVM_SORTITEMSEX
#define LVM_SORTITEMSEX          (LVM_FIRST + 81)

typedef int (CALLBACK *PFNLVCOMPARE)(LPARAM, LPARAM, LPARAM);

#define ListView_SortItemsEx(hwndLV, _pfnCompare, _lPrm) \
  (BOOL)SendMessage((hwndLV), LVM_SORTITEMSEX, (WPARAM)(LPARAM)(_lPrm), (LPARAM)(PFNLVCOMPARE)(_pfnCompare))
#endif


typedef struct {
	int cbSize;
	int flags; // OR of PDF_* flags below
	HANDLE hContact;
	HICON hIcon;
	union {
		char *pszTitle;
		TCHAR *ptzTitle;
		wchar_t *pwzTitle;
	};
	union {
		char *pszText;
		TCHAR *ptzText;
		wchar_t *pwzText;
	};
	COLORREF colorBack;		// if colorBack and colorText are equal, defaults will be used
	COLORREF colorText;
	WNDPROC windowProc;		// optional custom window procedure
	int timeout;			// -1 == infinite, 0 == default, otherwise timeout in seconds
	void *opaque;
} PopupData;

#define PDF_UNICODE		0x0001
#ifdef _UNICODE
#define PDF_TCHAR		PDF_UNICODE
#else
#define PDF_TCHAR		0
#endif

// windowProc messages
#define PM_INIT			(WM_USER + 0x0202)				// message sent to your windowProc after the window has been initialized
#define PM_DIENOTIFY	(WM_USER + 0x0200)				// message sent to your windowProc just before the window is destroyed (can be used e.g. to free your opaque data)

#define PM_DESTROY		(WM_USER + 0x0201)				// send to the popup hWnd (use PostMessage generally, or SendMessage inside your windowProc) to kill it

void ShowPopup(PopupData &pd_in);