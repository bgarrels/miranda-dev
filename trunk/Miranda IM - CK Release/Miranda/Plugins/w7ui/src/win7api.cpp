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

#include <windows.h>

//extern "C" const CLSID CLSID_TaskbarList              = {0x56fdf344,0xfd6d,0x11d0,{0x95,0x8a,0x00,0x60,0x97,0xc9,0xa0,0x90}}; // 56fdf344-fd6d-11d0-958a-006097c9a090;
//extern "C" const IID IID_ITaskbarList                 = {0x56FDF342,0xFD6D,0x11d0,{0x95,0x8A,0x00,0x60,0x97,0xC9,0xA0,0x90}}; // 56FDF342-FD6D-11d0-958A-006097C9A090;
//extern "C" const IID IID_ITaskbarList2                = {0x602D4995,0xB13A,0x429b,{0xA6,0x6E,0x19,0x35,0xE4,0x4F,0x43,0x17}}; // 602D4995-B13A-429b-A66E-1935E44F4317;
//extern "C" const IID IID_ICustomDestinationList         = {0x6332debf,0x87b5,0x4670,{0x90,0xc0,0x5e,0x57,0xb4,0x08,0xa4,0x9e}}; // 6332debf-87b5-4670-90c0-5e57b408a49e
//extern "C" const IID IID_IObjectArray                   = {0x92CA9DCD,0x5622,0x4bba,{0xA8,0x05,0x5E,0x9F,0x54,0x1B,0xD8,0xC9}}; // 92CA9DCD-5622-4bba-A805-5E9F541BD8C9
//extern "C" const IID IID_IObjectCollection              = {0x5632b1a4,0xe38a,0x400a,{0x92,0x8a,0xd4,0xcd,0x63,0x23,0x02,0x95}}; // 5632b1a4-e38a-400a-928a-d4cd63230295
//extern "C" const IID IID_ITaskbarList3                  = {0xea1afb91,0x9e28,0x4b86,{0x90,0xe9,0x9e,0x9f,0x8a,0x5e,0xef,0xaf}}; // ea1afb91-9e28-4b86-90e9-9e9f8a5eefaf
extern "C" const CLSID CLSID_CustomDestinationList      = {0x77f10cf0,0x3db5,0x4966,{0xb5,0x20,0xb7,0xc5,0x4f,0xd3,0x5e,0xd6}};
//extern "C" const CLSID CLSID_EnumerableObjectCollection = {0x2d3468c1,0x36a7,0x43b6,{0xac,0x24,0xd3,0xf0,0x2f,0xd9,0x60,0x7a}};

HRESULT (WINAPI *dwmInvalidateIconicBitmaps)(HWND) =
	(HRESULT (WINAPI *)(HWND))GetProcAddress(LoadLibraryA("dwmapi.dll"), "DwmInvalidateIconicBitmaps");
HRESULT (WINAPI *dwmSetIconicThumbnail)(HWND, HBITMAP, DWORD) =
	(HRESULT (WINAPI *)(HWND, HBITMAP, DWORD))GetProcAddress(LoadLibraryA("dwmapi.dll"), "DwmSetIconicThumbnail");
HRESULT (WINAPI *dwmSetIconicLivePreviewBitmap)(HWND, HBITMAP, LPPOINT, DWORD) =
	(HRESULT (WINAPI *)(HWND, HBITMAP, LPPOINT, DWORD))GetProcAddress(LoadLibraryA("dwmapi.dll"), "DwmSetIconicLivePreviewBitmap");

HANDLE (STDAPICALLTYPE *openThemeData)(HWND, LPCWSTR) =
	(HANDLE (STDAPICALLTYPE *)(HWND, LPCWSTR))GetProcAddress(LoadLibraryA("uxtheme.dll"), "OpenThemeData");
HRESULT (STDAPICALLTYPE *drawThemeTextEx)(HANDLE, HDC, int, int, LPCWSTR, int, DWORD, LPRECT, const struct _DTTOPTS *) =
	(HRESULT (STDAPICALLTYPE *)(HANDLE, HDC, int, int, LPCWSTR, int, DWORD, LPRECT, const struct _DTTOPTS *))GetProcAddress(LoadLibraryA("uxtheme.dll"), "DrawThemeTextEx");
HRESULT (STDAPICALLTYPE *closeThemeData)(HANDLE) =
	(HRESULT (STDAPICALLTYPE *)(HANDLE))GetProcAddress(LoadLibraryA("uxtheme.dll"), "CloseThemeData");
