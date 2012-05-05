/*
Name_day plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2005 Tibor Szabo

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
#include "newpluginapi.h"

#include "name_day_core.h"

static	name_day_core_t		name_day_core;

HINSTANCE hInst;
PLUGINLINK *pluginLink;

/**
 * @brief Plugin info.
 *
 */
PLUGININFO pluginInfo={
	sizeof(PLUGININFO),
	"Name Day Plugin",
	PLUGIN_MAKE_VERSION(0, 0, 0, 7),
	"The Name Day plugin.",
	"Tibor Szabo",
	"tibor.szabo@gmail.com",
	"© 2005-2012 Tibor Szabo",
	"",
	0,		//not transient
	0		//doesn't replace anything built-in
};

/**
 * @brief DllMain
 *
 */
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	hInst = hinstDLL;
	return TRUE;
}


/**
 * @brief Plugin Info
 *
 */
extern "C" __declspec(dllexport) PLUGININFO* MirandaPluginInfo(DWORD mirandaVersion)
{
	return &pluginInfo;
}

/**
 * @brief Load
 *
 */
extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink = link;

	//create_menu();

	//check_contacts();

	name_day_core.create_menu();
	name_day_core.perform_name_day_test();
	
	return 0;
}

/**
 * @brief Unload
 *
 */
extern "C" int __declspec(dllexport) Unload(void)
{
	return 0;
}