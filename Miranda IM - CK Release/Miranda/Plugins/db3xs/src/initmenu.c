/*

Miranda IM: the free IM client for Microsoft* Windows*

Copyright 2000-2008 Miranda ICQ/IM project, 
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
*/

#include "commonheaders.h"

extern HANDLE hOnLoadHook;

void dbxIconsInit()
{
	SKINICONDESC sid = {0};
	TCHAR tszFile[MAX_PATH];
	GetModuleFileName(g_hInst, tszFile, MAX_PATH);

	sid.cbSize = sizeof(SKINICONDESC);
	sid.flags = SIDF_ALL_TCHAR;
	sid.ptszDefaultFile = tszFile;
	sid.ptszSection = LPGENT("Database");

	sid.pszName = "password";
	sid.ptszDescription = LPGENT("Change Password");
	sid.iDefaultIndex = -IDI_ICON1;
	CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);	
}

void xModifyMenu(HANDLE hMenu, TCHAR* name)
{
	CLISTMENUITEM mi = {0};

	mi.cbSize = sizeof(mi);
	mi.flags = CMIM_FLAGS | CMIF_TCHAR  | (name ? CMIM_NAME : 0);
	mi.ptszName = name;
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hMenu, (LPARAM)&mi);
}

int dbxMenusInit(WPARAM wParam, LPARAM lParam)
{
	CLISTMENUITEM mi = {0};

	dbxIconsInit();
	CreateServiceFunction("DB3XS/SetPassword",DB3XSSetPassword);
	mi.cbSize = sizeof(mi);
	mi.flags = CMIF_TCHAR;
	mi.position = -100000000;
	mi.ptszPopupName = LPGENT("Database");
	mi.hIcon = ( HICON )CallService( MS_SKIN2_GETICON, 0, (LPARAM)"password" );
	mi.ptszName = (g_secured) ? LPGENT("Change Password") : LPGENT("Set Password");
	mi.pszService = "DB3XS/SetPassword";

	hSetPwdMenu = (HANDLE)CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi);

	UnhookEvent(hOnLoadHook);
	hOnLoadHook = NULL;
	return 0;
}
