/*
NewsAggregator (formaly known as RSSReader) plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright (c) 2012 Mataes

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

struct _tag_iconList
{
	TCHAR*  szDescr;
	char*  szName;
	int    defIconID;
	HANDLE hIconLibItem;
}

static iconList[] =
{
	{	_T("Protocol icon"),	"main",			IDI_ICON		},
	{	_T("Check All Feeds"),	"checkall",		IDI_CHECKALL	},
	{	_T("Add Feed"),			"addfeed",		IDI_ADDFEED		},
	{	_T("Import Feeds"),		"importfeeds",	IDI_IMPORTFEEDS	},
	{	_T("Export Feeds"),		"exportfeeds",	IDI_EXPORTFEEDS	},
	{	_T("Check Feed"),		"checkfeed",	IDI_CHECKALL	},
};

VOID InitIcons()
{
	TCHAR szFile[MAX_PATH];
	char szSettingName[100];
	SKINICONDESC sid = {0};
	unsigned i;

	GetModuleFileName(hInst, szFile, MAX_PATH);

	sid.cbSize = sizeof(SKINICONDESC);
	sid.flags = SIDF_ALL_TCHAR;
	sid.ptszDefaultFile = szFile;
	sid.pszName = szSettingName;
	sid.ptszSection = _T(MODULE);

	for (i = 0; i < SIZEOF(iconList); i++) 
	{
		mir_snprintf(szSettingName, SIZEOF(szSettingName), "%s_%s", MODULE, iconList[i].szName);

		sid.ptszDescription = iconList[i].szDescr;
		sid.iDefaultIndex = -iconList[i].defIconID;
		iconList[i].hIconLibItem = ( HANDLE )CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);
	}	
}

HICON LoadIconEx(const char* name, BOOL big)
{
	char szSettingName[100];
	mir_snprintf(szSettingName, SIZEOF(szSettingName), "%s_%s", MODULE, name);
	return (HICON)CallService(MS_SKIN2_GETICON, big, (LPARAM)szSettingName);
}

HANDLE  GetIconHandle(const char* name)
{
	unsigned i;
	for (i=0; i < SIZEOF(iconList); i++)
		if (strcmp(iconList[i].szName, name) == 0)
			return iconList[i].hIconLibItem;
	return NULL;
}