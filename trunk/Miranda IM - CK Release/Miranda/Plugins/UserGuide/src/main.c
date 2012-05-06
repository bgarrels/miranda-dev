/*
UserGuide plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2009 Mikhail Yuriev

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

struct MM_INTERFACE mmi;
HINSTANCE hInst;
PLUGINLINK *pluginLink;
HANDLE hModulesLoaded;

PLUGININFOEX pluginInfo={
	sizeof(PLUGININFOEX),
	"User Guide Plugin (AlfaMaR)",
	PLUGIN_MAKE_VERSION(0,0,0,1),
	"This plug-in adds the main menu item used to view miranda-im pack user guide",
	"Yasnovidyashii",
	"yasnovidyashii@gmail.com",
	"© 2009-2012 Mikhail Yuriev",
	"http://miranda-im.org/",
	0,
	0,
	MIID_USERGUIDE
};

static int ShowGuideFile(WPARAM wParam,LPARAM lParam)
{
	DBVARIANT dbv = {0};
	int iRes;

	LPCSTR pszEmptySting="";
	LPSTR pszDirName, pszDirNameEx, pszFileName,pszDivider;

	REPLACEVARSDATA dat = {0};
	dat.cbSize = sizeof( dat );
	dat.dwFlags = 0;
	
	pszDirName=(LPSTR)mir_alloc(250*sizeof(CHAR));
	pszFileName=(LPSTR)mir_alloc(250*sizeof(CHAR));

	iRes = DBGetContactSettingString(NULL,"UserGuide","PathToHelpFile",&dbv);
	
	if (iRes!=0)
	{
			strcpy(pszDirName, "%miranda_path%\\Plugins");
			strcpy(pszFileName, "UserGuide.chm");			
	}
	else
			if(strcmp((dbv.pszVal),pszEmptySting)==0)
			{
				strcpy(pszDirName, "%miranda_path%\\Plugins");
				strcpy(pszFileName, "UserGuide.chm");			
				mir_free(dbv.pszVal);
			}
			else 
			{
				pszDivider = strrchr(dbv.pszVal, '\\');
				if (pszDivider == NULL)
				{	
					pszDirName = "";
					strncpy(pszFileName, dbv.pszVal, strlen(dbv.pszVal) );
				}
				else
				{
					strncpy(pszFileName, pszDivider+1, strlen(dbv.pszVal)-strlen(pszDivider)-1);
					pszFileName[strlen(dbv.pszVal)-strlen(pszDivider)-1] = 0;
					strncpy(pszDirName, dbv.pszVal, pszDivider-dbv.pszVal);
					pszDirName[pszDivider-dbv.pszVal] = 0;
				}
				mir_free(dbv.pszVal);
			}
	if (ServiceExists(MS_UTILS_REPLACEVARS))
		pszDirNameEx = (char *) CallService(MS_UTILS_REPLACEVARS,(WPARAM)pszDirName,(LPARAM)&dat);
	else
		pszDirNameEx = mir_strdup(pszDirName);

	ShellExecuteA(NULL,"open",pszFileName,NULL,pszDirNameEx,SW_SHOW);
	mir_free(pszDirName);
	mir_free(pszFileName);
	mir_free(pszDirNameEx);
	return 0;
}

int ModulesLoaded(WPARAM wParam,LPARAM lParam)
{
	CLISTMENUITEM mi;

	CreateServiceFunction("UserGuide/ShowGuide",ShowGuideFile);
	ZeroMemory(&mi,sizeof(mi));
	mi.cbSize=sizeof(mi);
	mi.position=500000;
	mi.flags=0;
	mi.hIcon=LoadSkinnedIcon(SKINICON_OTHER_HELP);
	mi.pszName=LPGEN("User Guide");
	mi.pszService="UserGuide/ShowGuide";
	CallService(MS_CLIST_ADDMAINMENUITEM,0,(LPARAM)&mi);
	
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	hInst=hinstDLL;
	return TRUE;
}


__declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
	return &pluginInfo;
}

static const MUUID interfaces[] = {MIID_USERGUIDE, MIID_LAST};
__declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}

int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink=link;
	mir_getMMI(&mmi);
	hModulesLoaded = HookEvent(ME_SYSTEM_MODULESLOADED,ModulesLoaded);
	return 0;
}

int __declspec(dllexport) Unload(void)
{
	UnhookEvent(hModulesLoaded);
	return 0;
}