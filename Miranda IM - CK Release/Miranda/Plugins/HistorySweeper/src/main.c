/*
Historysweeper plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2002-2003  Sergey V. Gershovich
			Copyright (C) 2006-2009  Boris Krasnovskiy


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

#include "historysweeper.h"
#include "../version.h"

HINSTANCE hInst;
PLUGINLINK *pluginLink;
int hLangpack; 

struct MM_INTERFACE mmi;

static HANDLE hHooks[5];
static HANDLE hServices[3];

static HANDLE hMainMenuHandle;
static HANDLE hContactMenuHandle;
static HANDLE hNoHistoryMenuHandle;

const char* iconsel[] = { "keep", "custact", "purge" };

static PLUGININFOEX pluginInfoEx =
{ 
	// about plugin
	sizeof(PLUGININFOEX),
#if defined(_WIN64)
	"History Sweeper+ x64",
#elif defined(UNICODE) | defined(_UNICODE)
	"History Sweeper+ Unicode",
#else
	"History Sweeper+",
#endif
	__VERSION_DWORD,
	"This plugin can sweep system history, history from all contacts or only from chosen; also it may sweep history older than certain time; and may do it at Miranda IM startup/shutdown.",
	"Sergey V. Gershovich a.k.a. Jazzy$, Boris Krasnovskiy",
	"borkra@miranda-im.org",
	"Copyright © 2002-2003 Sergey V. Gershovich a.k.a. Jazzy$, 2006-2009 Boris Krasnovskiy",
	"http://addons.miranda-im.org/details.php?action=viewfile&id=3291",
#if defined(UNICODE) | defined(_UNICODE)
	UNICODE_AWARE,		//not transient
#else
	0,
#endif
	0,
	MIID_HISTORYSWEEPER
};

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	// plugin entry point
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		hInst = hinstDLL;
		DisableThreadLibraryCalls(hinstDLL);
	}
	return TRUE;
}


//==========================================================================
static int RebuildContactMenu(WPARAM wParam,LPARAM lParam)
{
	CLISTMENUITEM cli = {0}; 
	TCHAR menu_title[255];

	int count = CallService(MS_DB_EVENT_GETCOUNT, wParam, 0); 
	
	if (count != 0)
	{
		mir_sntprintf(menu_title, SIZEOF(menu_title), TranslateT("Sweep history [%d]"), count);
		cli.flags = CMIM_NAME | CMIF_TCHAR | CMIM_FLAGS;
		cli.ptszName = menu_title;
	} 
	else 
		cli.flags = CMIF_HIDDEN | CMIM_FLAGS;

	cli.cbSize = sizeof(cli);

	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hContactMenuHandle, (LPARAM)&cli);

	cli.flags = CMIM_ICON;

	count = DBGetContactSettingByte((HANDLE)wParam, "CList", "SweepHistory", 0);
	cli.icolibItem = GetIconHandle(iconsel[count]);

	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hNoHistoryMenuHandle, (LPARAM)&cli);

	return 0;
}

void ModifyMainMenu(void)
{
	CLISTMENUITEM mi = {0};
	
	mi.cbSize  = sizeof(mi);
	mi.flags   = CMIM_NAME;
	mi.pszName = (char*)menu_items[DBGetContactSettingByte(0, ModuleName, "MainMenuAction", 0)];
	
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hMainMenuHandle, (LPARAM)&mi);
}

INT_PTR NoHistoryAction(WPARAM wParam, LPARAM lParam)
{
	int nh = (DBGetContactSettingByte((HANDLE)wParam, "CList", "SweepHistory", 0) + 1) % 3;
	DBWriteContactSettingByte((HANDLE)wParam, "CList", "SweepHistory", (BYTE)nh);
	return 0;
}

int OnIconPressed(WPARAM wParam, LPARAM lParam) 
{
	int nh;
	HANDLE hContact = (HANDLE)wParam;
	StatusIconClickData *sicd = (StatusIconClickData *)lParam;

	if (sicd->cbSize < (int)sizeof(StatusIconClickData))
		return 0;

	if (sicd->flags & MBCF_RIGHTBUTTON) return 0;
	if (strcmp(sicd->szModule, ModuleName)) return 0; 

	nh = DBGetContactSettingByte((HANDLE)wParam, "CList", "SweepHistory", 0) ? 0 : 2;
	DBWriteContactSettingByte((HANDLE)wParam, "CList", "SweepHistory", (BYTE)nh);

	{
		StatusIconData sid = {0};
		sid.cbSize = sizeof(sid);
		sid.szModule = ModuleName;
		sid.flags = nh ? MBF_DISABLED : 0;
		CallService(MS_MSG_MODIFYICON, (WPARAM)hContact, (LPARAM)&sid);
	}

	return 0;
}


int OnModulesLoaded(WPARAM wParam, LPARAM lParam) 
{
	StatusIconData sid = {0};

	sid.cbSize = sizeof(sid);
	sid.szModule = ModuleName;
	sid.hIcon = LoadIconEx("keep");
	sid.hIconDisabled = LoadIconEx("purge");
	sid.szTooltip = Translate("History Enabled");
	CallService(MS_MSG_ADDICON, 0, (LPARAM)&sid);
		
	hHooks[3] = HookEvent(ME_MSG_WINDOWEVENT, OnWindowEvent);
	hHooks[4] = HookEvent(ME_MSG_ICONPRESSED, OnIconPressed);

	return 0;
}

//==========================================================================
__declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{// Tell Miranda IM about plugin
	return &pluginInfoEx;
}

static const MUUID interfaces[] = {MIID_HISTORYSWEEPER, MIID_LAST};
__declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}

int __declspec(dllexport) Load(PLUGINLINK *link)
{
	char temp[80];

	// on load: create services, hook events, add menu items, sweep history if need
	pluginLink = link;

	mir_getMMI(&mmi);

	CallService(MS_SYSTEM_GETVERSIONTEXT, (WPARAM)SIZEOF(temp), (LPARAM)temp);

#ifdef _UNICODE
	if (strstr(temp, "Unicode") == NULL)
	{
		MessageBox(NULL, TranslateT("Please update History Sweeper to ANSI Version"), TranslateT(ModuleName), MB_OK | MB_ICONERROR);
		return 1;
	}
#else
	if (strstr(temp, "Unicode") != NULL)
	{
		MessageBox(NULL, Translate("Please update History Sweeper to Unicode Version"), TranslateT(ModuleName), MB_OK | MB_ICONERROR);
		return 1;
	}
#endif


	hHooks[0] = HookEvent(ME_SYSTEM_MODULESLOADED, OnModulesLoaded);
	hHooks[1] = HookEvent(ME_CLIST_PREBUILDCONTACTMENU, RebuildContactMenu);
	hHooks[2] = HookEvent(ME_OPT_INITIALISE, HSOptInitialise);
	
	hServices[0] = CreateServiceFunction(MS_HS_SWEEPCONTACTHISTORY, ContactMenuAction);
	hServices[1] = CreateServiceFunction(MS_HS_SWEEPHISTORYFROMMAINMENU, MainMenuAction);
	hServices[2] = CreateServiceFunction(MS_HS_NOCONTACTHISTORY, NoHistoryAction);

	InitIcons();

	{
		CLISTMENUITEM mi = {0};
		mi.cbSize = sizeof(mi);
		mi.flags = CMIF_ICONFROMICOLIB;

		//add contact menu
		mi.position = 1000099000;
		mi.pszName = "Sweep history";
		mi.pszService = MS_HS_SWEEPCONTACTHISTORY;
		mi.icolibItem = GetIconHandle("main");
		hContactMenuHandle = (HANDLE)CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);

		mi.pszName = "Change History Policy";
		mi.pszService = MS_HS_NOCONTACTHISTORY;
		mi.icolibItem = GetIconHandle("purge");
		hNoHistoryMenuHandle = (HANDLE)CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);

		//add main menu item 
		mi.position = 500070000;
		mi.pszName = (char*)menu_items[DBGetContactSettingByte(NULL, ModuleName, "MainMenuAction", 0)];
		mi.pszService = MS_HS_SWEEPHISTORYFROMMAINMENU;
		mi.icolibItem = GetIconHandle("main");
		hMainMenuHandle = (HANDLE)CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi);
	}

	return 0;
}

int __declspec(dllexport) Unload(void)
{ 
	//on unload: unhook hooked events, destroy created services
	unsigned i;

	// Do Shutdown sweep if necessary
	SSAction();

	for (i = 0; i < SIZEOF(hServices); ++i)
		DestroyServiceFunction(hServices[i]);

	for (i = 0; i < SIZEOF(hHooks); ++i)
		UnhookEvent(hHooks[i]);

	return 0;
}
