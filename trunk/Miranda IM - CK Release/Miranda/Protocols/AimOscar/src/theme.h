#ifndef THEME_H
#define THEME_H

/*
AIM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			(C) 2008-2012 Boris Krasnovskiy
			(C) 2005-2006 Aaron Myles Landwehr

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

extern HMODULE themeAPIHandle;
extern HANDLE  (WINAPI *MyOpenThemeData)(HWND,LPCWSTR);
extern HRESULT (WINAPI *MyCloseThemeData)(HANDLE);
extern HRESULT (WINAPI *MyDrawThemeBackground)(HANDLE,HDC,int,int,const RECT *,const RECT *);

void InitThemeSupport(void);
void InitIcons(void);
void InitExtraIcons(void);

void DestroyExtraIcons(void);

HICON  LoadIconEx(const char* name, bool big = false);
HANDLE GetIconHandle(const char* name);
void   ReleaseIconEx(const char* name, bool big = false);
void   WindowSetIcon(HWND hWnd, const char* name);
void   WindowFreeIcon(HWND hWnd);

void   add_AT_icons(CAimProto* ppro);
void   remove_AT_icons(CAimProto* ppro);
void   add_ES_icons(CAimProto* ppro);
void   remove_ES_icons(CAimProto* ppro);

void set_contact_icon(CAimProto* ppro, HANDLE hContact);

#endif
