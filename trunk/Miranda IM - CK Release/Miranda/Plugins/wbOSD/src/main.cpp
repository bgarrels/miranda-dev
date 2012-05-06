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
#include "buildnumber.h"

HINSTANCE hI;
MM_INTERFACE mmi;

HWND hwnd=0;
HANDLE hservosda;
PLUGINLINK *pluginLink;
HANDLE hHookedNewEvent, hHookedInit, hProtoAck, hContactSettingChanged, hHookContactStatusChanged, hContactStatusChanged, hpluginShutDown;
HINSTANCE hUser32;
BOOL (WINAPI*pSetLayeredWindowAttributes)(HWND, COLORREF, BYTE, DWORD);

void logmsg2(char *str);
int MainInit(WPARAM wparam,LPARAM lparam);

/////////////////////////////////////////////////////////////////////////////////////////

extern "C" int __declspec(dllexport)Load(PLUGINLINK *link)
{
	pluginLink=link;
	mir_getMMI( &mmi );

	logmsg("Load");
	pSetLayeredWindowAttributes=0;

#ifndef FORCE_9XDRAWING
	hUser32=LoadLibraryA("user32.dll");
#else
	hUser32=0;
#endif

	if (hUser32) {
		pSetLayeredWindowAttributes=(BOOL(WINAPI*)(HWND, COLORREF, BYTE, DWORD))GetProcAddress(hUser32, "SetLayeredWindowAttributes");
		if (!pSetLayeredWindowAttributes) {
			FreeLibrary(hUser32);
			hUser32=0;
		}
	}

	hHookedInit = HookEvent(ME_SYSTEM_MODULESLOADED, MainInit);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport) int Unload(void)
{
	logmsg("Unload");
	UnhookEvent(hpluginShutDown);
	UnhookEvent(hProtoAck);
	UnhookEvent(hContactSettingChanged);
	UnhookEvent(hContactStatusChanged);
	UnhookEvent(hHookedNewEvent);
	UnhookEvent(hHookedInit);

	if (hUser32)
		FreeLibrary(hUser32);
	pSetLayeredWindowAttributes=0;

	DestroyServiceFunction(hservosda);
	DestroyHookableEvent(hHookContactStatusChanged);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

static PLUGININFO pluginInfo={
	sizeof(PLUGININFO),
	#ifdef UNICODE
		"WannaBe OSD (Unicode)",
	#else
		"WannaBe OSD",
	#endif
	__VERSION_DWORD,
	"Show new message/status change info using onscreen display",
	"Andrej Krutak",
	"andree182 {at} gmail (dot) com",
	"©2005-2012 Andrej Krutak",
	"http://urtax.ms.mff.cuni.cz/~andree/",
	UNICODE_AWARE,
	0
};

extern "C" __declspec(dllexport) PLUGININFO* MirandaPluginInfo(DWORD mirandaVersion)
{
	return &pluginInfo;
}

/////////////////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hinst,DWORD fdwReason,LPVOID lpvReserved)
{
	hI = hinst;
	return 1;
}
