/*
Modernopt plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2009 Victor Pavlychko
			Copyright (C) 2010-2012 George Hazan

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

PLUGINLINK *pluginLink;
HINSTANCE hInst;
MM_INTERFACE mmi;
LIST_INTERFACE li;

#if defined( _UNICODE )
	// {621f886b-a7f6-457f-9d62-8ee84c275993}
	#define MIID_MODERNOPTS { 0x621f886b, 0xa7f6, 0x457f, { 0x9d, 0x62, 0x8e, 0xe8, 0x4c, 0x27, 0x59, 0x93 }}
#else
	// {0a8e1133-d12d-4103-a912-39 96 3b 49 b3 5f}
	#define MIID_MODERNOPTS { 0x0a8e1133, 0xd12d, 0x4103, { 0xa9, 0x12, 0x39, 0x96, 0x3b, 0x49, 0xb3, 0x5f }}
#endif

PLUGININFOEX pluginInfoEx = {
	sizeof(PLUGININFOEX),
	"Support for new options design",
	PLUGIN_MAKE_VERSION(0,1,0,0),
	"Support for new options design.",
	"Victor Pavlychko, George Hazan",
	"ghazan@miranda-im.org",
	"© 2009-2012 Victor Pavlychko, George Hazan",
	"http://www.miranda-im.org",
	UNICODE_AWARE,
	0,
	MIID_MODERNOPTS
};

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	hInst = hinstDLL;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// MirandaPluginInfoEx - returns an information about a plugin

extern "C" __declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
	if (mirandaVersion < PLUGIN_MAKE_VERSION(0, 8, 0, 0))
		return NULL;

	return &pluginInfoEx;
}

/////////////////////////////////////////////////////////////////////////////////////////
// MirandaPluginInterfaces - returns the protocol interface to the core

static const MUUID interfaces[] = { MIID_MODERNOPTS, MIID_LAST };

extern "C" __declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Performs a primary set of actions upon plugin loading

int LoadModernOptsModule();

extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink = link;
	mir_getMMI( &mmi );
	mir_getLI( &li );

	LoadModernOptsModule();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Unload a plugin

extern "C" int __declspec(dllexport) Unload(void)
{
	return 0;
}
