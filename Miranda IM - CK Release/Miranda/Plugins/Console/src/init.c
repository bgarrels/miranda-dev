/*
Console plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) Bio

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

HINSTANCE hInst;
DWORD mirandaVer;


static const PLUGININFOEX pluginInfo =
{
	sizeof(PLUGININFOEX),
#if defined(_WIN64)
	"Console x64",
#elif defined(UNICODE) | defined(_UNICODE)
	"Console Unicode",
#endif
	__PLUGIN_NAME,
	__VERSION_DWORD,
	__PLUGIN_LONGDESC,
	__PLUGIN_AUTHOR,
	__PLUGIN_EMAIL,
	__PLUGIN_RIGHTS,
	__PLUGIN_URL,
#if defined(UNICODE) | defined(_UNICODE)
	UNICODE_AWARE,	
	0,
	MIID_CONSOLE
};


// entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	hInst = hinstDLL;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

__declspec(dllexport) const PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
	mirandaVer = mirandaVersion;
	return &pluginInfo;
}

// The following function is required to be implemented if you are using MirandaPluginInfoEx.
// we implement service mode interface
// returns plugin's interfaces information
static const MUUID interfaces[] = {MIID_LOGWINDOW, MIID_LAST};
__declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}

////////////////////////////////////////////////////////////////////////////////
// plugin's entry point
int __declspec(dllexport) int Load(PLUGINLINK *link)
{
	pluginLink = link;

	if (mirandaVer < PLUGIN_MAKE_VERSION(0,8,0,3))
		return -1;
	else
	{
		InitCommonControls();
		InitConsole();
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////
// plugin's exit point
int __declspec(dllexport) int Unload(void)
{
	LOG(( "Unload" ));

	Netlib_CloseHandle( g_hNetlibUser );
	DestroyHookableEvent( hStateChangedEvent );
	ShutdownConsole();
	return 0;
}
