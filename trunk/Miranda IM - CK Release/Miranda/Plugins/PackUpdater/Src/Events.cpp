/*
PackUpdater plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	2010-2012 Mataes
							2007 ZERO_BiT

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

#include "common.h"

HANDLE Timer;
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

	if(AllowUpdateOnStartup())
		DoCheck(UpdateOnStartup, (int)CheckThread);

	Timer = CreateWaitableTimer(NULL, FALSE, NULL);
	InitTimer();

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
	if (lParam)
		file_op.fFlags |= FOF_NOCONFIRMATION;
	SHFileOperation(&file_op);
	return 0;
}

INT OnPreShutdown(WPARAM wParam, LPARAM lParam)
{
	CancelWaitableTimer(Timer);
	CloseHandle(Timer);

	UnhookEvent(hOptHook);
	UnhookEvent(hOnPreShutdown);
	return 0;
}