#ifndef __FLTCONT_H__
#define __FLTCONT_H__

/*
FltContacts plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Authors
Copyright (C) Ranger (fonts and colors settings)
Copyright (C) Iavor Vajarov ( ivajarov@code.bg )


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

* This file is a modification of clcfiledrop.h originally  written by Richard Hughes.
*/

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#define FLT_FONTID_CONTACTS    0
#define FLT_FONTID_INVIS       1
#define FLT_FONTID_OFFLINE     2
#define FLT_FONTID_OFFINVIS    3
#define FLT_FONTID_NOTONLIST   4

#define FLT_FONTIDS 5

#define DBFONTF_BOLD       1
#define DBFONTF_ITALIC     2
#define DBFONTF_UNDERLINE  4

/////////////////////////////////////////////////////////////////////////////
#define FLT_DEFAULT_DRAWBORDER			TRUE
#define FLT_DEFAULT_LTEDGESCOLOR		GetSysColor(COLOR_3DHILIGHT)
#define FLT_DEFAULT_RBEDGESCOLOR		GetSysColor(COLOR_3DDKSHADOW)
#define FLT_DEFAULT_BKGNDCOLOR			GetSysColor(COLOR_3DFACE)
#define FLT_DEFAULT_BKGNDUSEBITMAP		FALSE
#define FLT_DEFAULT_BKGNDBITMAPOPT		CLB_STRETCH

/////////////////////////////////////////////////////////////////////////////
extern char szModuleDB[];

extern HINSTANCE	hInst;
extern BOOL			bNT;
extern BOOL			bHideOffline;
extern BOOL			bHideAll;
extern BOOL			bHideWhenFullscreen;
extern BOOL			bMoveTogether;
extern BOOL			bFixedWidth;
extern int			nThumbWidth;
extern BYTE			thumbAlpha;

extern BOOL (WINAPI *pSetLayeredWindowAttributes)(HWND,COLORREF,BYTE,DWORD);

/////////////////////////////////////////////////////////////////////////////
void
GetFontSetting
	( IN BOOL bFltContacts
	, IN int nFontId
	, IN LOGFONT* lf
	, IN COLORREF* colour
	);

void
ApplyOptionsChanges();

void
OnStatusChanged();

void
SetThumbsOpacity
	( IN BYTE btAlpha
	);

int
OnOptionsInitialize
	( IN WPARAM wParam
	, IN LPARAM lParam
	);

/////////////////////////////////////////////////////////////////////////////
#endif	// #ifndef __FLTCONT_H__
		// End Of File fltcont.h
