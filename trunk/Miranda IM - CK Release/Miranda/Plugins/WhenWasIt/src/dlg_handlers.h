/*
WhenWasIt (birthday reminder) plugin for Miranda IM

Copyright � 2006 Cristian Libotean

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

#ifndef M_WWI_DIALOG_HANDLERS_H
#define M_WWI_DIALOG_HANDLERS_H

#include "commonheaders.h"

#define FOREGROUND_COLOR RGB(0, 0, 0)
#define BACKGROUND_COLOR RGB(255, 255, 255)

#define POPUP_TIMEOUT 10

#define CHECK_INTERVAL 12

#define DAYS_TO_NOTIFY 9
#define DAYS_TO_NOTIFY_AFTER 0

#define CLIST_ICON 4

#define WWIM_UPDATE_BIRTHDAY (WM_USER + 101)
#define WWIM_ADD_UPCOMING_BIRTHDAY (WM_USER + 102)

INT_PTR CALLBACK DlgProcOptions(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgProcBirthdays(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgProcAddBirthday(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgProcUpcoming(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgProcPopup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif //M_WWI_DIALOG_HANDLERS_H