/*
Wannabe OSD plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2005 Andrej Krutak

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

#include "wbOSD.h"

#define WORKAROUND_9X		//transparent window support on 9x workaround...
							//#define FORCE_9XDRAWING

TCHAR szClassName[]	= _T("wbOSD");
const static osdmsg defstr={_T(""), 0, RGB(0, 0, 0), 0, 0};

int DrawMe(HWND hwnd, TCHAR *string, COLORREF color)
{
	HDC hdc;
	PAINTSTRUCT	ps;
	RECT rect, rect2;
	HGDIOBJ oo;
	UINT talign=0;
	HBRUSH bkb;
	int sxo, syo;
	HFONT fh;
	plgsettings plgs;

	logmsg("DrawMe");
	if (!string) string=_T("bullshit");
	
	loadDBSettings(&plgs);
	fh=CreateFontIndirectA(&(plgs.lf));

	hdc=BeginPaint(hwnd, &ps);
	SetBkMode(hdc, TRANSPARENT);

	GetClientRect(hwnd, &rect);
	bkb=CreateSolidBrush(plgs.bkclr);

#ifdef WORKAROUND_9X
	//workaround for 9x, needs to get fixed...
	if (!pSetLayeredWindowAttributes && DBGetContactSettingByte(NULL,THIS_MODULE, "transparent", DEFAULT_TRANPARENT)) {
		HDC shdc;

		logmsg("DrawMe::w9x");

		shdc=CreateDC(TEXT("DISPLAY"), 0, 0, 0);
		GetWindowRect(hwnd, &rect2);
		BitBlt(hdc, 0, 0, rect2.right-rect2.left, rect2.bottom-rect2.top, shdc, rect2.left, rect2.top, SRCCOPY);
		DeleteDC(shdc);
	} else
#endif
		FillRect(hdc, &rect, bkb);

	DeleteObject(bkb);

	oo=SelectObject(hdc, fh);

//	rect2.left=0;
//	rect2.top=0;
//	DrawText(hdc, string, -1, &rect2, DT_SINGLELINE|DT_CALCRECT);
	rect2 = rect;
	DrawText(hdc, string, -1, &rect2, DT_WORDBREAK|DT_CALCRECT);

	if (plgs.align>=1 && plgs.align<=3)
		rect.top=0;
	if (plgs.align>=4 && plgs.align<=6)
		rect.top=(rect.bottom-rect2.bottom)/2;
	if (plgs.align>=7 && plgs.align<=9)
		rect.top=rect.bottom-rect2.bottom;

	if (((plgs.align-1)%3)==0)
		rect.left=0;
	else if (((plgs.align-2)%3)==0)
		rect.left=(rect.right-rect2.right)/2;
	else if (((plgs.align-3)%3)==0)
		rect.left=rect.right-rect2.right;

	rect.bottom=rect.top+rect2.bottom;
	rect.right=rect.left+rect2.right;

	//draw shadow
	if (plgs.showShadow) {
		logmsg("DrawMe::showShadow");
		if (plgs.salign>=1 && plgs.salign<=3) syo=-plgs.distance;
		if (plgs.salign>=4 && plgs.salign<=6) syo=0;
		if (plgs.salign>=7 && plgs.salign<=9) syo=plgs.distance;

		if (((plgs.salign-1)%3)==0) sxo=-plgs.distance;
		else if (((plgs.salign-2)%3)==0) sxo=0;
		else if (((plgs.salign-3)%3)==0) sxo=plgs.distance;

		SetTextColor(hdc, plgs.clr_shadow);
		if (plgs.altShadow==0) {
			rect2=rect;
			OffsetRect(&rect, sxo, syo);

			DrawText(hdc, string, -1, &rect2, DT_WORDBREAK|talign);
		} else {
			rect2=rect;
			rect2.left+=plgs.distance;
			DrawText(hdc, string, -1, &rect2, DT_WORDBREAK|talign);

			rect2=rect;
			rect2.left-=plgs.distance;
			DrawText(hdc, string, -1, &rect2, DT_WORDBREAK|talign);

			rect2=rect;
			rect2.top-=plgs.distance;
			DrawText(hdc, string, -1, &rect2, DT_WORDBREAK|talign);

			rect2=rect;
			rect2.top+=plgs.distance;
			DrawText(hdc, string, -1, &rect2, DT_WORDBREAK|talign);

			OffsetRect(&rect, sxo/2, syo/2);
		}
	}

	//draw text
	SetTextColor(hdc, color);
	DrawText(hdc, string, -1, &rect, DT_WORDBREAK);
	
	SelectObject(hdc, oo);
	DeleteObject(fh);
	EndPaint(hwnd, &ps);

	return 0;
}

LRESULT	CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	osdmsg* ms;

	switch (message) {
	case WM_CREATE:
		logmsg("WindowProcedure::CREATE");
		SetWindowLong(hwnd, GWL_USERDATA, (LONG)&defstr);
		return 0;

	case WM_DESTROY:
		logmsg("WindowProcedure::DESTROY");
		return 0;

	case WM_PAINT:
		logmsg("WindowProcedure::PAINT");

		ms=(osdmsg*)GetWindowLong(hwnd, GWL_USERDATA);
		if (ms)
			return DrawMe(hwnd, ms->text, ms->color);
		
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_NCRBUTTONDOWN:
		logmsg("WindowProcedure::NCRBUTTONDOWN");

		ms = (osdmsg*)GetWindowLong(hwnd, GWL_USERDATA);
		if ( ms ) {
			if ( ms->callback) ms->callback(ms->param);
			SendMessage(hwnd, WM_USER+3, 0, 0);
		}
		return 0;

	case WM_TIMER:
		logmsg("WindowProcedure::TIMER");
		SendMessage(hwnd, WM_USER+3, wParam, 0);
		return 0;

	case WM_USER+1: //draw text ((char *)string, (int) timeout
		logmsg("WindowProcedure::USER+1");

		ms=(osdmsg*)mir_alloc(sizeof(osdmsg));
		ms->text = mir_tstrdup((TCHAR *)wParam );
		if ( lParam == 0 )
			lParam = DBGetContactSettingDword( NULL,THIS_MODULE, "timeout", DEFAULT_TIMEOUT );
		ms->timeout = lParam;
		ms->callback = 0;
		ms->color = DBGetContactSettingDword(NULL,THIS_MODULE, "clr_msg", DEFAULT_CLRMSG);
		ms->param = 0;
		SendMessage(hwnd, WM_USER+4, (WPARAM)ms, 0);
		mir_free(ms->text);
		mir_free(ms);
		return 0;

	case WM_USER+2: //show
		logmsg("WindowProcedure::USER+2");
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
		return 0;

	case WM_USER+3: //hide
		ms = ( osdmsg* )GetWindowLong(hwnd, GWL_USERDATA);
		logmsg("WindowProcedure::USER+3");
		if ( !ms )
			return 0;

		logmsg("WindowProcedure::USER+3/om");
		KillTimer(hwnd, (UINT)ms);
		mir_free(ms->text);
		mir_free(ms);
		SetWindowLong(hwnd, GWL_USERDATA, 0);
		ShowWindow(hwnd, SW_HIDE);
		return 0;

	case WM_USER+4:
		logmsg("WindowProcedure::USER+4");

		ms = (osdmsg*)GetWindowLong(hwnd, GWL_USERDATA);
		if ( ms != 0 ) {
			logmsg("WindowProcedure::USER+4/old");
			KillTimer(hwnd, (UINT)ms);
			mir_free(ms->text);
			mir_free(ms);
			SetWindowLong(hwnd, GWL_USERDATA, 0);
			if (!pSetLayeredWindowAttributes) {
				logmsg("WindowProcedure::USER+4/old+9x");
				ShowWindow(hwnd, SW_HIDE);
				Sleep(50);
		}	}

		ms =(osdmsg*)mir_alloc(sizeof(osdmsg));
		memcpy(ms, (osdmsg*)wParam, sizeof(osdmsg));
		ms->text = mir_tstrdup( ms->text );

		SetWindowLong(hwnd, GWL_USERDATA, (LONG)ms);
		SetTimer(hwnd, (UINT)ms, (int)ms->timeout, 0);
		InvalidateRect(hwnd, 0, TRUE);
		SendMessage(hwnd, WM_USER+2, 0, 0);
		return 0;

	case WM_NCHITTEST:
		{
			RECT rect;
			GetWindowRect(hwnd, &rect);
			
			logmsg("WindowProcedure::NCHITTEST");

			if (LOWORD(lParam)>=(rect.left+5) && LOWORD(lParam)<=(rect.right-5) && 
				HIWORD(lParam)>=(rect.top+5) && HIWORD(lParam)<=(rect.bottom-5))
				return HTCAPTION;
			return DefWindowProc (hwnd,	message, wParam, lParam);
		}
	//here will be the doubleclick => open-message-window solution ;-)
	//case WM_NCLBUTTONDBLCLK:
	//	CallService(MS_MSG_SENDMESSAGE, wparam,(LPARAM)&odp);
	//	return 0;
	default:
		return DefWindowProc (hwnd,	message, wParam, lParam);
	}

	return 0;
}

int OSDAnnounce(WPARAM wParam, LPARAM lParam)
{
	logmsg("OSDAnnounce");
	return SendMessage(hwnd, WM_USER+1, wParam, lParam);
}

int pluginShutDown(WPARAM wparam,LPARAM lparam)
{
	logmsg("pluginShutDown");
	if (hwnd) {
		logmsg("pluginShutDown/hwnd");
		SendMessage(hwnd, WM_USER+3, 0, 0);
		DestroyWindow(hwnd);
		hwnd=0;
	}
	return 0;
}

int MainInit(WPARAM wparam,LPARAM lparam)
{
	logmsg("MainInit");
	HookEvent(ME_OPT_INITIALISE,OptionsInit);
	
	WNDCLASSEX wincl;
	wincl.hInstance	= hI;
	wincl.lpszClassName	= szClassName;
	wincl.lpfnWndProc =	WindowProcedure;
	wincl.style	= CS_DBLCLKS;
	wincl.cbSize = sizeof (WNDCLASSEX);

	wincl.hIcon	= LoadIcon (NULL, IDI_APPLICATION);
	wincl.hIconSm =	LoadIcon (NULL,	IDI_APPLICATION);
	wincl.hCursor =	LoadCursor (NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground	= 0;

	if ( !RegisterClassEx( &wincl ))
		return 0;

	hwnd = CreateWindowEx((pSetLayeredWindowAttributes?WS_EX_LAYERED:0)|WS_EX_TOOLWINDOW, szClassName, _T("WannaBeOSD"),
		(pSetLayeredWindowAttributes?0:WS_CLIPSIBLINGS) | WS_POPUP,
		DBGetContactSettingDword(NULL,THIS_MODULE, "winxpos", DEFAULT_WINXPOS),
		DBGetContactSettingDword(NULL,THIS_MODULE, "winypos", DEFAULT_WINYPOS),
		DBGetContactSettingDword(NULL,THIS_MODULE, "winx", DEFAULT_WINX),
		DBGetContactSettingDword(NULL,THIS_MODULE, "winy", DEFAULT_WINY),
		HWND_DESKTOP, NULL, hI, NULL);

	SetWindowLong(hwnd, GWL_USERDATA, 0);

	if (pSetLayeredWindowAttributes)
		pSetLayeredWindowAttributes(hwnd, DBGetContactSettingDword(NULL,THIS_MODULE, "bkclr", DEFAULT_BKCLR), DBGetContactSettingByte(NULL,THIS_MODULE, "alpha", DEFAULT_ALPHA), (DBGetContactSettingByte(NULL,THIS_MODULE, "transparent", DEFAULT_TRANPARENT)?LWA_COLORKEY:0)|LWA_ALPHA);

	hservosda=CreateServiceFunction("OSD/Announce", OSDAnnounce);
	
#if !defined(UNICODE_SERVICE_ONLY)
	hHookedNewEvent = HookEvent(ME_DB_EVENT_ADDED, HookedNewEvent);
	
	// try to create ME_STATUSCHANGE_CONTACTSTATUSCHANGED event... I hope it fails when newstatusnotify or equal creates it before ;-)
	
	hContactStatusChanged = HookEvent(ME_STATUSCHANGE_CONTACTSTATUSCHANGED,ContactStatusChanged);
	if (!hContactStatusChanged) {
		hHookContactStatusChanged = CreateHookableEvent(ME_STATUSCHANGE_CONTACTSTATUSCHANGED);
		hContactSettingChanged = HookEvent(ME_DB_CONTACT_SETTINGCHANGED,ContactSettingChanged);

		hContactStatusChanged = HookEvent(ME_STATUSCHANGE_CONTACTSTATUSCHANGED,ContactStatusChanged);
	}
	hProtoAck=HookEvent(ME_PROTO_ACK,ProtoAck);
#endif

	hpluginShutDown=HookEvent(ME_SYSTEM_SHUTDOWN,pluginShutDown);

	return 0;
}
