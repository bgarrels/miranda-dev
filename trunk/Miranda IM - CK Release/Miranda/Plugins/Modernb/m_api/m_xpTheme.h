/*
Author Artem Shpynov aka FYR

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

/* Wrapper for XP theme */
typedef void * XPTHANDLE;
typedef HANDLE HTHEME; 

XPTHANDLE	xpt_AddThemeHandle(HWND hwnd, LPCWSTR className);
void		xpt_FreeThemeHandle(XPTHANDLE xptHandle);
void		xpt_FreeThemeForWindow(HWND hwnd);
BOOL		xpt_IsValidHandle(XPTHANDLE xptHandle);
HRESULT		xpt_DrawThemeBackground(XPTHANDLE xptHandle, HDC hdc, int type, int state, const RECT * sizeRect, const RECT * clipRect);
HRESULT		xpt_DrawThemeParentBackground(HWND hWnd, HDC hdc, const RECT * sizeRect);
HRESULT		xpt_DrawThemeText(XPTHANDLE xptHandle, HDC hdc, int type, int state, LPCTSTR lpStr, int len, DWORD flag1, DWORD flag2, const RECT * textRect);
BOOL		xpt_IsThemeBackgroundPartiallyTransparent(XPTHANDLE xptHandle, int type,  int state);
HRESULT		xpt_DrawTheme(XPTHANDLE xptHandle, HWND hwnd, HDC hdc, int type, int state, const RECT *sizeRect, const RECT * clipRect);
BOOL		xpt_IsThemed(XPTHANDLE xptHandle);
BOOL		xpt_EnableThemeDialogTexture(HWND hwnd, DWORD flags);

// next will be called only from one place
HRESULT XPThemesLoadModule();
void		XPThemesUnloadModule();
void		xpt_OnWM_THEMECHANGED();


//#include <tmschema.h>
//TODO: ADD HERE other definitions of part and states from	<tmschema.h>
//WINDOW 
#define WP_CAPTION			1
#define WP_SMALLCAPTION     2
#define WP_SMALLMINCAPTION  4
#define WP_SMALLMAXCAPTION	6

#define CS_ACTIVE			1
#define CS_INACTIVE			2