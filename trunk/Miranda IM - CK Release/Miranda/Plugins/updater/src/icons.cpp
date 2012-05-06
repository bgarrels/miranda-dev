/*
Updater plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	2005-2006 Scott Ellis
							2009-2012 Boris Krasnovskiy

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
#include "icons.h"

typedef struct
{
	char* szDescr;
	char* szName;
	int   defIconID;
} IconStruct;

static const IconStruct iconList[] =
{
	{ "Check for Plugin Updates",	"updater_check",      IDI_MAINMENU       },
	{ "Restart",					"updater_restart",    IDI_RESTART        },
	{ "Update and Exit",			"updater_checkexit",  IDI_UPDATEANDEXIT  },
};

HANDLE hIcolibIcon[SIZEOF(iconList)];


HICON LoadIconEx(IconIndex i, bool big)
{
	return (HICON)CallService(MS_SKIN2_GETICONBYHANDLE, big, (LPARAM)hIcolibIcon[(int)i]);
} 

HANDLE GetIconHandle(IconIndex i)
{
	return hIcolibIcon[(int)i];
}

void ReleaseIconEx(HICON hIcon)
{
	CallService(MS_SKIN2_RELEASEICON, (WPARAM)hIcon, 0);
}


void InitIcons(void) 
{
	SKINICONDESC sid = {0};
	TCHAR path[MAX_PATH];
	int i;
	
	sid.cbSize = sizeof(SKINICONDESC);
	sid.pszSection = MODULE;
	sid.flags = SIDF_PATH_TCHAR;
	sid.ptszDefaultFile = path;
	GetModuleFileName(hInst, path, sizeof(path));

	for (i = 0; i < SIZEOF(iconList); ++i)
	{
		sid.pszDescription = iconList[i].szDescr;
		sid.pszName = iconList[i].szName;
		sid.iDefaultIndex = -iconList[i].defIconID;
		hIcolibIcon[i] = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);
	}
}
