/*
Crypto++ plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
Copyright (C) 2006-2009 Baloo

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

#include "commonheaders.h"


// dllmain
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID) {
	g_hInst = hInst;
	if( dwReason == DLL_PROCESS_ATTACH ) {
		{
		char temp[MAX_PATH];
		GetTempPath(sizeof(temp),temp);
		GetLongPathName(temp,TEMP,sizeof(TEMP));
		TEMP_SIZE = strlen(TEMP);
		if(TEMP[TEMP_SIZE-1]=='\\') {
			TEMP_SIZE--;
			TEMP[TEMP_SIZE]='\0';
		}
		}
		InitializeCriticalSection(&localQueueMutex);
		InitializeCriticalSection(&localContextMutex);
#ifdef _DEBUG
		isVista = 1;
#else
		isVista = ( (DWORD)(LOBYTE(LOWORD(GetVersion()))) == 6 );
#endif
	}
	//DLL_THREAD_ATTACH
	return TRUE;
}


PLUGININFO *MirandaPluginInfo(DWORD mirandaVersion) {
	return &pluginInfo;
}


PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
	return &pluginInfoEx;
}


MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}


int onModulesLoaded(WPARAM wParam,LPARAM lParam) {
    // updater plugin support
#if defined(_DEBUG) || defined(NETLIB_LOG)
	InitNetlib();
#endif
	if(ServiceExists(MS_UPDATE_REGISTERFL)) {
		CallService(MS_UPDATE_REGISTERFL, (WPARAM)2669, (LPARAM)&pluginInfo);
	}
	return 0;
}


int Load(PLUGINLINK *link) {

	pluginLink = link;
	DisableThreadLibraryCalls(g_hInst);

	// get memoryManagerInterface address
	mir_getMMI( &mmi );

	// register plugin module
	PROTOCOLDESCRIPTOR pd;
	memset(&pd,0,sizeof(pd));
	pd.cbSize = sizeof(pd);
	pd.szName = (char*)szModuleName;
	pd.type = PROTOTYPE_ENCRYPTION;
	CallService(MS_PROTO_REGISTERMODULE, 0, (LPARAM)&pd);

	// hook events
	HookEvent(ME_SYSTEM_MODULESLOADED,onModulesLoaded);
	return 0;
}


int Unload() {
	return 0;
}


// EOF
