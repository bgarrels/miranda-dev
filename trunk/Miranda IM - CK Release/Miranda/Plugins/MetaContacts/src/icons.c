/*
MetaContacts plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2004 Universite Louis PASTEUR, STRASBOURG.
			Copyright (C) 2005-2008 Scott Ellis

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

* Functions for the <b>'Add To'</b> Dialog.
* Contains all the functions and all the structures needed to display and control
* the <b>'Add To'</b> Dialog.
*/

#include "metacontacts.h"

HANDLE hIcoLibIconsChanged = NULL;


typedef struct {
	char* szDescr;
	char* szName;
	int   defIconID;
} IconStruct;

static IconStruct iconList[] = {
	{ "Toggle Off",              "mc_off",      IDI_MCMENUOFF     },
	{ "Toggle On",               "mc_on",       IDI_MCMENU        },
	{ "Convert to MetaContact",  "mc_convert",  IDI_MCCONVERT     },
	{ "Add to Existing",         "mc_add",      IDI_MCADD         },
	{ "Edit",                    "mc_edit",     IDI_MCEDIT        },
	{ "Set to Default",          "mc_default",  IDI_MCSETDEFAULT  },
	{ "Remove",                  "mc_remove",   IDI_MCREMOVE      },
};


HICON LoadIconEx(IconIndex i) {
	HICON hIcon;

	if (hIcoLibIconsChanged)
		hIcon = (HICON)CallService(MS_SKIN2_GETICON, 0, (LPARAM)iconList[(int)i].szName);
	else
		hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(iconList[(int)i].defIconID), IMAGE_ICON, 0, 0, 0);

	return hIcon;
}


void ReleaseIconEx(HICON hIcon) {
	if (hIcoLibIconsChanged)
		CallService(MS_SKIN2_RELEASEICON, (WPARAM)hIcon, 0);
	else
		DestroyIcon(hIcon);
}

int ReloadIcons(WPARAM wParam, LPARAM lParam) {
	// fix menu icons
	CLISTMENUITEM menu = {0};

	menu.cbSize = sizeof(menu);
	menu.flags = CMIM_ICON;

	menu.hIcon = LoadIconEx(Meta_IsEnabled() ? I_MENUOFF : I_MENU);
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hMenuOnOff, (LPARAM)&menu);
	ReleaseIconEx(menu.hIcon);

	menu.hIcon = LoadIconEx(I_CONVERT);
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hMenuConvert, (LPARAM)&menu);
	ReleaseIconEx(menu.hIcon);
	
	menu.hIcon = LoadIconEx(I_ADD);
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hMenuAdd, (LPARAM)&menu);
	ReleaseIconEx(menu.hIcon);
	
	menu.hIcon = LoadIconEx(I_EDIT);
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hMenuEdit, (LPARAM)&menu);
	ReleaseIconEx(menu.hIcon);

	menu.hIcon = LoadIconEx(I_SETDEFAULT);
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hMenuDefault, (LPARAM)&menu);
	ReleaseIconEx(menu.hIcon);

	menu.hIcon = LoadIconEx(I_REMOVE);
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hMenuDelete, (LPARAM)&menu);
	ReleaseIconEx(menu.hIcon);

	return 0;
}

void InitIcons(void)  {
	SKINICONDESC sid = {0};
	char path[MAX_PATH];
	int i;
	
	sid.cbSize = sizeof(SKINICONDESC);
	sid.pszSection = META_PROTO;
	sid.pszDefaultFile = path;
	GetModuleFileName(hInstance, path, sizeof(path));

	for (i = 0; i < sizeof(iconList) / sizeof(IconStruct); ++i)
	{
		sid.pszDescription = Translate(iconList[i].szDescr);
		sid.pszName = iconList[i].szName;
		sid.iDefaultIndex = -iconList[i].defIconID;
		CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);
	}

	hIcoLibIconsChanged = HookEvent(ME_SKIN2_ICONSCHANGED, ReloadIcons);

	ReloadIcons(0, 0); 
}

void DeinitIcons(void) {
	if (hIcoLibIconsChanged) UnhookEvent(hIcoLibIconsChanged);
}
