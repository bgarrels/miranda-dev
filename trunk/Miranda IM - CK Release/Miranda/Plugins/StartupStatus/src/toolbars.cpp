/*
Statup Status plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) Copyright 2003-2006 P. Boon

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

#include "commonstatus.h"
#include "../startupstatus.h"
#include "../resource.h"
#include <commctrl.h>

#include <m_icolib.h>

#define MAX_MMITEMS		6

static HANDLE hTtbDown = 0, hTtbUp = 0;

static HANDLE* ttbButtons = NULL;
static int ttbButtonCount = 0;

/////////////////////////////////////////////////////////////////////////////////////////

int RemoveTopToolbarButtons()
{
	int profileCount, i, orgButtonCount;
	
	profileCount = CallService(MS_SS_GETPROFILECOUNT, 0, 0);
	orgButtonCount = ttbButtonCount;
	for (i=0;i<orgButtonCount;i++)
		if (CallService(MS_TTB_REMOVEBUTTON, (WPARAM)ttbButtons[i], 0) != 1)
			ttbButtonCount -= 1;

	if (ttbButtonCount == 0) {
		free(ttbButtons);
		ttbButtons = NULL;
	}
	else ttbButtons = ( HANDLE* )realloc(ttbButtons, profileCount*sizeof(HANDLE));

	return 0;
}

int CreateTopToolbarButtons(WPARAM wParam, LPARAM lParam)
{
	TTBButton ttb;
	int i, profileCount;
	DBVARIANT dbv;
	char setting[80];
	
	profileCount = CallService(MS_SS_GETPROFILECOUNT, 0, 0);
	ttbButtons = ( HANDLE* )realloc(ttbButtons, profileCount*sizeof(HANDLE));
	ZeroMemory(&ttb,sizeof(ttb));
	ZeroMemory(&dbv,sizeof(dbv));
	ttb.cbSize = sizeof(ttb);
	ttb.dwFlags = TTBBF_VISIBLE|TTBBF_SHOWTOOLTIP;
	ttb.hbBitmapDown = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_TTBDOWN));
	ttb.hbBitmapUp = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_TTBUP));
	ttb.pszServiceDown = ttb.pszServiceUp = MS_SS_LOADANDSETPROFILE;
	for (i=0;i<profileCount;i++) {
		char profileName[128];
		INT_PTR ttbAddResult = -1;
		
		_snprintf(setting, sizeof(setting), "%d_%s", i, SETTING_CREATETTBBUTTON);
		if (!DBGetContactSettingByte(NULL, MODULENAME, setting, FALSE)) continue;
		_snprintf(setting, sizeof(setting), "%d_%s", i, SETTING_PROFILENAME);
		if (DBGetContactSetting(NULL, MODULENAME, setting, &dbv)) continue;
		strncpy(profileName, dbv.pszVal, sizeof(profileName)-1);
		ttb.name = profileName;
		ttb.wParamDown = ttb.wParamUp = i;
		ttbAddResult = CallService(MS_TTB_ADDBUTTON, (WPARAM)&ttb, 0);
		if (ttbAddResult != -1) {
			ttbButtons[ttbButtonCount] = (HANDLE)ttbAddResult;
			// this tooltip makes miranda crash if changed > 3 times or so
			CallService(MS_TTB_SETBUTTONOPTIONS,MAKEWPARAM(TTBO_TIPNAME,ttbButtons[ttbButtonCount]), (LPARAM)profileName);
			DBFreeVariant(&dbv);
			ttbButtonCount += 1;
		}
	}
	if (ttbButtonCount > 0)
		ttbButtons = ( HANDLE* )realloc(ttbButtons, ttbButtonCount*sizeof(HANDLE));
	else {
		free(ttbButtons);
		ttbButtons = NULL;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

int RemoveToolbarButtons()
{
	int profileCount, i, orgButtonCount;
	
	profileCount = CallService(MS_SS_GETPROFILECOUNT, 0, 0);
	orgButtonCount = ttbButtonCount;
	for ( i=0; i < orgButtonCount; i++ )
		if ( !CallService( MS_TB_REMOVEBUTTON, (WPARAM)ttbButtons[i], 0 ))
			ttbButtonCount --;

	if ( ttbButtonCount == 0 ) {
		free( ttbButtons );
		ttbButtons = NULL;
	}
	else ttbButtons = ( HANDLE* )realloc(ttbButtons, profileCount*sizeof(HANDLE));

	return 0;
}

int CreateToolbarButtons(WPARAM wParam, LPARAM lParam)
{
	char setting[80];
	
	int profileCount = CallService(MS_SS_GETPROFILECOUNT, 0, 0);
	ttbButtons = ( HANDLE* )realloc(ttbButtons, profileCount*sizeof(HANDLE));

	TBButton ttb = { 0 };
	ttb.cbSize = sizeof(ttb);
	ttb.tbbFlags = TBBF_VISIBLE | TBBF_SHOWTOOLTIP;
	ttb.pszServiceName = MS_SS_LOADANDSETPROFILE;
	for (int i=0; i < profileCount; i++ ) {
		char profileName[128];
		INT_PTR ttbAddResult = -1;
		
		_snprintf(setting, sizeof(setting), "%d_%s", i, SETTING_CREATETTBBUTTON);
		if (!DBGetContactSettingByte(NULL, MODULENAME, setting, FALSE))
			continue;

		DBVARIANT dbv;
		_snprintf(setting, sizeof(setting), "%d_%s", i, SETTING_PROFILENAME);
		if (DBGetContactSetting(NULL, MODULENAME, setting, &dbv))
			continue;

		strncpy(profileName, dbv.pszVal, sizeof(profileName)-1);
		ttb.pszButtonID = "LaunchProfile";
		ttb.pszButtonName = profileName;
		ttb.pszTooltipUp = ttb.pszTooltipDn = profileName;
		ttb.hPrimaryIconHandle = hTtbUp;
		ttb.hSecondaryIconHandle = hTtbDown;
		ttb.defPos = 200;
		ttb.lParam = i;
		ttbAddResult = CallService( MS_TB_ADDBUTTON, 0, (LPARAM)&ttb );
		if (ttbAddResult != -1) {
			ttbButtons[ttbButtonCount] = (HANDLE)ttbAddResult;
			ttbButtonCount += 1;
		}
		DBFreeVariant( &dbv );
	}
	if (ttbButtonCount > 0)
		ttbButtons = ( HANDLE* )realloc(ttbButtons, ttbButtonCount*sizeof(HANDLE));
	else {
		free(ttbButtons);
		ttbButtons = NULL;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

void RegisterButtons()
{
	TCHAR szFile[MAX_PATH];
	GetModuleFileName(hInst, szFile, SIZEOF(szFile));

	SKINICONDESC sid = {0};
	sid.cbSize = sizeof(SKINICONDESC);
	sid.ptszDefaultFile = szFile;
	sid.pszSection = "StartupStatus";
	sid.pszName = "StartupStatus/TtbDown";
	sid.pszDescription = "Pressed toolbar icon";
	sid.iDefaultIndex = -IDI_TTBDOWN;
	sid.flags = SIDF_PATH_TCHAR;
	hTtbDown = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);

	sid.pszName = "StartupStatus/TtbUp";
	sid.pszDescription = "Released toolbar icon";
	sid.iDefaultIndex = -IDI_TTBUP;
	hTtbUp = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);
}
