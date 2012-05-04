/*
Authors Richard Hughes, Roland Rabien & Tristan Van de Vreede

Miranda IM: the free IM client for Microsoft* Windows*

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

===============================================================================
*/

#include <commonheaders.h>

HANDLE hOnCntMenuBuild;
HANDLE hPriorityItem = 0, hFloatingItem = 0;

static int OnContactMenuBuild(WPARAM wParam,LPARAM lParam)
{
	CLISTMENUITEM mi;
	BYTE bSetting;

	// Menu Item - Floating Contact
	bSetting = cfg::getByte("CList", "flt_enabled", 0);
	if (bSetting && !hFloatingItem)
	{
		memset(&mi,0,sizeof(mi));
		mi.cbSize=sizeof(mi);
		mi.position=200000;
		mi.pszPopupName=(char *)-1;
		mi.pszService="CList/SetContactFloating";
		mi.pszName=LPGEN("&Floating Contact");
		if (pcli)
		{
			if (SendMessage(pcli->hwndContactTree, CLM_QUERYFLOATINGCONTACT, wParam, 0))
				mi.flags=CMIF_CHECKED;
		}
		hFloatingItem = (HANDLE)CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);
	}
	else if (!bSetting && hFloatingItem)
	{
		CallService(MS_CLIST_REMOVECONTACTMENUITEM, (WPARAM)hFloatingItem, 0);
		hFloatingItem = 0;
	}
	else
	{
		memset(&mi,0,sizeof(mi));
		mi.cbSize=sizeof(mi);
		mi.flags=CMIM_FLAGS;
		if (pcli)
		{
			if (SendMessage(pcli->hwndContactTree, CLM_QUERYFLOATINGCONTACT, wParam, 0))
				mi.flags |= CMIF_CHECKED;
		}
		CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hFloatingItem, (LPARAM)&mi);
	}
	return 0;
}

int MTG_OnmodulesLoad(WPARAM wParam,LPARAM lParam)
{
	hOnCntMenuBuild=HookEvent(ME_CLIST_PREBUILDCONTACTMENU,OnContactMenuBuild);
	return 0;
}

int UnloadMoveToGroup(void)
{
	if (hOnCntMenuBuild)
		UnhookEvent(hOnCntMenuBuild);

	return 0;
}
