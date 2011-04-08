/* 
Copyright (C) 2010 Mataes

This is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this file; see the file license.txt.  If
not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  
*/

#include "common.h"

BOOL Silent;

int ModulesLoaded(WPARAM wParam, LPARAM lParam)
{
	UnhookEvent(hLoadHook);
	Silent = true;
	HOTKEYDESC hkd = {0};
	hkd.cbSize = sizeof(hkd);
	hkd.dwFlags = HKD_TCHAR;
	hkd.pszName = "Check for pack updates";
	hkd.ptszDescription = _T("Check for pack updates");
	hkd.ptszSection = _T("Pack Updater");
	hkd.pszService = MODNAME"/CheckUpdates";
	hkd.DefHotKey = HOTKEYCODE(HOTKEYF_CONTROL, VK_F10) | HKF_MIRANDA_LOCAL;
	hkd.lParam = FALSE;
	CallService(MS_HOTKEY_REGISTER, 0, (LPARAM)&hkd);

	DoCheck(AutoUpdate, (int)CheckThread);
	return 0;
}

INT_PTR MenuCommand(WPARAM wParam,LPARAM lParam)
{
	Silent = false;
	DoCheck(1, (int)CheckThread);
	return 0;
}

INT_PTR EmptyFolder(WPARAM wParam,LPARAM lParam)
{
    SHFILEOPSTRUCT file_op = {
        NULL,
        FO_DELETE,
        tszRoot,
        _T(""),
        FOF_NOERRORUI |
        FOF_SILENT,
        false,
        0,
        _T("") };
    SHFileOperation(&file_op);
	return 0;
}

INT OnPreShutdown(WPARAM wParam, LPARAM lParam)
{
	UnhookEvent(hOptHook);
	UnhookEvent(hOnPreShutdown);
	return 0;
}