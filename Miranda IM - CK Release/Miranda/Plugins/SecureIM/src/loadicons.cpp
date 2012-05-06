/*
SecureIM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 	2003 Johell
							2005-2009 Baloo

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


HINSTANCE LoadIconsPack(const char* szIconsPack)
{
	HINSTANCE hNewIconInst = NULL;
	WORD i;

	hNewIconInst = LoadLibrary(szIconsPack);

	if (hNewIconInst != NULL)
	{
		for(i=ID_FIRSTICON; i<=ID_LASTICON; i++)
			if (LoadIcon(hNewIconInst, MAKEINTRESOURCE(i)) == NULL)
			{
				FreeLibrary(hNewIconInst);
				hNewIconInst = NULL;
				break;
			}
	}
	return hNewIconInst;
}



int ReloadIcons(WPARAM wParam, LPARAM lParam)
{
	HICON hIcon;
	for (int i=0; icons[i].key; i++) {
		hIcon = (HICON)CallService(MS_SKIN2_GETICON, 0, (LPARAM)icons[i].name);
		if(icons[i].tbl == TBL_IEC)
			g_hIEC[icons[i].idx]=hIcon;
		else
		if(icons[i].tbl == TBL_ICO)
			g_hICO[icons[i].idx]=hIcon;
		else
		if(icons[i].tbl == TBL_POP)
			g_hPOP[icons[i].idx]=hIcon;
	}

	return 0;
}


void InitIcons(void)
{
	HINSTANCE hNewIconInst = NULL;

	if( g_hFolders ) {
		LPSTR pathname = (LPSTR) alloca(MAX_PATH);
		FoldersGetCustomPathEx(g_hFolders, pathname, MAX_PATH, "icons\\", "secureim_icons.dll");
		if (hNewIconInst == NULL)
			hNewIconInst = LoadIconsPack(pathname);
	}

	if (hNewIconInst == NULL)
		hNewIconInst = LoadIconsPack("icons\\secureim_icons.dll");

	if (hNewIconInst == NULL)
		hNewIconInst = LoadIconsPack("plugins\\secureim_icons.dll");

	if (hNewIconInst == NULL)
		g_hIconInst = g_hInst;
	else
		g_hIconInst = hNewIconInst;


	SKINICONDESC sid;
	memset(&sid,0,sizeof(sid));

	sid.cbSize = sizeof(SKINICONDESC);
	sid.pszSection = "SecureIM";

	HICON hIcon;
	for (int i=0; icons[i].key; i++) {
		if(ServiceExists(MS_SKIN2_ADDICON)) {
			sid.pszSection = icons[i].section;
			sid.pszName = icons[i].name;
			sid.pszDescription = icons[i].text;
			sid.pszDefaultFile = "secureim_icons.dll";
			sid.iDefaultIndex = icons[i].key;
			sid.hDefaultIcon = (HICON)LoadImage(g_hIconInst, MAKEINTRESOURCE(icons[i].key), IMAGE_ICON, 16, 16, LR_SHARED);
			CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);
			hIcon = (HICON)CallService(MS_SKIN2_GETICON, 0, (LPARAM)icons[i].name);
		}
		else {
			hIcon = (HICON)LoadImage(g_hIconInst, MAKEINTRESOURCE(icons[i].key), IMAGE_ICON, 16, 16, LR_SHARED);
		}
		if(icons[i].tbl == TBL_IEC)
			g_hIEC[icons[i].idx]=hIcon;
		else
		if(icons[i].tbl == TBL_ICO)
			g_hICO[icons[i].idx]=hIcon;
		else
		if(icons[i].tbl == TBL_POP)
			g_hPOP[icons[i].idx]=hIcon;
	}

	if(ServiceExists(MS_SKIN2_ADDICON)) {
		AddHookFunction(ME_SKIN2_ICONSCHANGED, ReloadIcons);
	}
}

// EOF
