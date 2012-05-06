/*
Google Extension plugin for 
Miranda IM: the free IM client for Microsoft* Windows*
and the Jabber plugin.

Authors
			Copyright (C) 2011-2012	bems@jabber.org
									George Hazan (ghazan@jabber.ru)

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

#include "stdafx.h"
#include "notifications.h"
#include "options.h"
#include "popups.h"

HINSTANCE hInst = 0;

DWORD itlsSettings = TLS_OUT_OF_INDEXES;
DWORD itlsRecursion = TLS_OUT_OF_INDEXES;
DWORD itlsPopupHook = TLS_OUT_OF_INDEXES;

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	hInst = hinstDLL;

	switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            if (((itlsSettings = TlsAlloc()) == TLS_OUT_OF_INDEXES) ||
				((itlsRecursion = TlsAlloc()) == TLS_OUT_OF_INDEXES) ||
				((itlsPopupHook = TlsAlloc()) == TLS_OUT_OF_INDEXES))
                return FALSE;
			break;

		case DLL_THREAD_ATTACH:
			TlsSetValue(itlsPopupHook,
				(PVOID)SetWindowsHookEx(WH_CALLWNDPROCRET, PopupHookProc, NULL, GetCurrentThreadId()));
			break;

		case DLL_THREAD_DETACH:
			UnhookWindowsHookEx((HHOOK)TlsGetValue(itlsPopupHook));
			break;

        case DLL_PROCESS_DETACH:
            TlsFree(itlsSettings);
			TlsFree(itlsRecursion);
			TlsFree(itlsPopupHook);
            break;
    }

	return TRUE;
}
