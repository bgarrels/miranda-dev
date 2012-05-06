#ifndef win7api_h__
#define win7api_h__

/*
W7ui plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2009 nullbie, persei

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

#define WM_DWMSENDICONICTHUMBNAIL           0x0323
#define WM_DWMSENDICONICLIVEPREVIEWBITMAP   0x0326

#define DWMWA_HAS_ICONIC_BITMAP 10

#define DWM_SIT_DISPLAYFRAME 0x00000001

enum TBATFLAG
{	TBATF_USEMDITHUMBNAIL	= 0x1,
	TBATF_USEMDILIVEPREVIEW	= 0x2
};

#define THBN_CLICKED 0x1800

extern HRESULT (WINAPI *dwmInvalidateIconicBitmaps)(HWND);
extern HRESULT (WINAPI *dwmSetIconicThumbnail)(HWND, HBITMAP, DWORD);
extern HRESULT (WINAPI *dwmSetIconicLivePreviewBitmap)(HWND, HBITMAP, LPPOINT, DWORD);

extern HANDLE (STDAPICALLTYPE *openThemeData)(HWND, LPCWSTR);
extern HRESULT (STDAPICALLTYPE *drawThemeTextEx)(HANDLE, HDC, int, int, LPCWSTR, int, DWORD, LPRECT, const struct _DTTOPTS *);
extern HRESULT (STDAPICALLTYPE *closeThemeData)(HANDLE);

extern "C" const IID IID_ICustomDestinationList;
extern "C" const IID IID_IObjectArray;
extern "C" const IID IID_IObjectCollection;
extern "C" const IID IID_ITaskbarList3;
extern "C" const CLSID CLSID_CustomDestinationList;
extern "C" const CLSID CLSID_EnumerableObjectCollection;

#include "win7api_IObjectArray.h"
#include "win7api_IObjectCollection.h"
#include "win7api_ICustomDestinationList.h"
//#include "win7api_ITaskbarList3.h"

#endif // win7api_h__
