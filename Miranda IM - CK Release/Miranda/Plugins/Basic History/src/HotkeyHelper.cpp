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

#include "StdAfx.h"
#include "HotkeyHelper.h"

LRESULT CALLBACK HotkeySubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void RegisterHotkeyControl(HWND control)
{
	WNDPROC oldProc = (WNDPROC)SetWindowLongPtr(control, GWLP_WNDPROC, (LONG_PTR) HotkeySubclassProc);
	SetWindowLongPtr(control, GWLP_USERDATA, (LONG_PTR) oldProc);
}

void UnregisterHotkeyControl(HWND control)
{
	WNDPROC oldProc = (WNDPROC)GetWindowLongPtr(control, GWLP_USERDATA);
	if(oldProc != NULL)
	{
		SetWindowLongPtr(control, GWLP_WNDPROC, (LONG_PTR) oldProc);
		SetWindowLongPtr(control, GWLP_USERDATA, NULL);
	}
}

static bool isPresed = false;

LRESULT CALLBACK HotkeySubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC oldProc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg) {
		case WM_CHAR:
		case WM_SYSCHAR:
		case WM_UNICHAR:
		case WM_DEADCHAR:
		case WM_SYSDEADCHAR:
			if(isPresed)
				return 0;
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			if(isPresed)
			{
				isPresed = false;
				return 0;
			}
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			{
				isPresed = false;
				HWND hwndParent = GetParent(hwnd);
				MSGFILTER filter;
				filter.msg = msg;
				filter.lParam = lParam;
				filter.wParam = wParam;
				filter.nmhdr.hwndFrom = hwnd;
				filter.nmhdr.code = EN_MSGFILTER;
				filter.nmhdr.idFrom = GetDlgCtrlID(hwnd);
				if(SendMessage(hwndParent, WM_NOTIFY, NULL, (LPARAM)&filter))
				{
					isPresed = true;
					return 0;
				}

				LRESULT res = CallWindowProc(oldProc, hwnd, msg, wParam, lParam);
				filter.nmhdr.code = CLN_MYSELCHANGED;
				SendMessage(hwndParent, WM_NOTIFY, NULL, (LPARAM)&filter);
				return res;
			}
			break;
		case WM_LBUTTONDOWN:
			{
				HWND hwndParent = GetParent(hwnd);
				MSGFILTER filter;
				filter.msg = msg;
				filter.lParam = lParam;
				filter.wParam = wParam;
				filter.nmhdr.hwndFrom = hwnd;
				filter.nmhdr.code = CLN_MYSELCHANGED;
				filter.nmhdr.idFrom = GetDlgCtrlID(hwnd);

				LRESULT res = CallWindowProc(oldProc, hwnd, msg, wParam, lParam);
				SendMessage(hwndParent, WM_NOTIFY, NULL, (LPARAM)&filter);
				return res;
			}
			break;
	}
	return CallWindowProc(oldProc, hwnd, msg, wParam, lParam);
}