#ifndef _MESSAGE_PUMP_INC
#define _MESSAGE_PUMP_INC

/*
TipperYM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
            Copyright (C) 2006-2009 Scott Ellis
            Copyright (C) 2007-2011 Jan Holub

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

* Tipper is internally unicode and requires unicows.dll to function correctly on
  Windows 95/98/ME so you'll find a lot of wchar_t stuff in here.
*/

#define  WAIT_TIMER_INTERVAL	500

#define MUM_CREATEPOPUP			(WM_USER + 0x011)
#define MUM_DELETEPOPUP			(WM_USER + 0x012)
#define MUM_GOTSTATUS			(WM_USER + 0x013)
#define MUM_GOTAVATAR			(WM_USER + 0x014)
#define MUM_GOTXSTATUS			(WM_USER + 0x015)

extern BOOL (WINAPI *MySetLayeredWindowAttributes)(HWND,COLORREF,BYTE,DWORD);
extern BOOL (WINAPI *MyUpdateLayeredWindow)(HWND hwnd, HDC hdcDST, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);
extern BOOL (WINAPI *MyAnimateWindow)(HWND hWnd,DWORD dwTime,DWORD dwFlags);
extern HMONITOR (WINAPI *MyMonitorFromPoint)(POINT, DWORD);
extern BOOL (WINAPI *MyGetMonitorInfo)(HMONITOR, LPMONITORINFO);
 
#define DWM_BB_ENABLE					0x00000001
#define DWM_BB_BLURREGION				0x00000002
#define DWM_BB_TRANSITIONONMAXIMIZED	0x00000004
struct DWM_BLURBEHIND
{
	DWORD dwFlags;
	BOOL fEnable;
	HRGN hRgnBlur;
	BOOL fTransitionOnMaximized;
};

extern HRESULT (WINAPI *MyDwmEnableBlurBehindWindow)(HWND hWnd, DWM_BLURBEHIND *pBlurBehind); 

void InitMessagePump();
void DeinitMessagePump();

INT_PTR ShowTip(WPARAM wParam, LPARAM lParam);
INT_PTR ShowTipW(WPARAM wParam, LPARAM lParam);
INT_PTR HideTip(WPARAM wParam, LPARAM lParam);

int ShowTipHook(WPARAM wParam, LPARAM lParam);
int HideTipHook(WPARAM wParam, LPARAM lParam);

int FramesShowSBTip(WPARAM wParam, LPARAM lParam);
int FramesHideSBTip(WPARAM wParam, LPARAM lParam);

int ProtoAck(WPARAM wParam, LPARAM lParam);
int AvatarChanged(WPARAM wParam, LPARAM lParam);

BOOL MyDestroyWindow(HWND hwnd);
void PostMPMessage(UINT msg, WPARAM, LPARAM);


#endif
