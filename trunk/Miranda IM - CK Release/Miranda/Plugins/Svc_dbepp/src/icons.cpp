/*
Database Editor++ for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2003-2011 Bio, Jonathan Gordon

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

* Icons for the services
*/

#include "headers.h"

HIMAGELIST himl;

void addIcons(TCHAR* szModuleFileName)
{
	SKINICONDESC sid={0};
	char name[32];
	sid.cbSize = sizeof(sid);
	sid.ptszSection = _T(modFullname);
	sid.ptszDefaultFile = szModuleFileName;
	sid.flags = SIDF_ALL_TCHAR;

	// closed known module
	sid.ptszDescription = LPGENT("Closed Known Module");
	mir_snprintf(name, SIZEOF(name), "DBE++_%d", ICO_KNOWN);
	sid.pszName = name;
	sid.iDefaultIndex = -ICO_KNOWN;
	CallService(MS_SKIN2_ADDICON,0,(LPARAM)&sid);

	// open known module
	sid.ptszDescription = LPGENT("Open Known Module");
	mir_snprintf(name, SIZEOF(name), "DBE++_%d", ICO_KNOWNOPEN);
	sid.pszName = name;
	sid.iDefaultIndex = -ICO_KNOWNOPEN;
	CallService(MS_SKIN2_ADDICON,0,(LPARAM)&sid);

	// closed unknown module
	sid.ptszDescription = LPGENT("Closed Unknown Module");
	mir_snprintf(name, SIZEOF(name), "DBE++_%d", ICO_UNKNOWN);
	sid.pszName = name;
	sid.iDefaultIndex = -ICO_UNKNOWN;
	CallService(MS_SKIN2_ADDICON,0,(LPARAM)&sid);

	// open unknown module
	sid.ptszDescription = LPGENT("Open Unknown Module");
	mir_snprintf(name, SIZEOF(name), "DBE++_%d", ICO_UNKNOWNOPEN);
	sid.pszName = name;
	sid.iDefaultIndex = -ICO_UNKNOWNOPEN;
	CallService(MS_SKIN2_ADDICON,0,(LPARAM)&sid);

	// settings contact
	sid.ptszDescription = LPGENT("Settings");
	mir_snprintf(name, SIZEOF(name), "DBE++_%d", ICO_SETTINGS);
	sid.pszName = name;
	sid.iDefaultIndex = -ICO_SETTINGS;
	CallService(MS_SKIN2_ADDICON,0,(LPARAM)&sid);

	// contact group
	sid.ptszDescription = LPGENT("Contacts Group");
	mir_snprintf(name, SIZEOF(name), "DBE++_%d", ICO_CONTACTS);
	sid.pszName = name;
	sid.iDefaultIndex = -ICO_CONTACTS;
	CallService(MS_SKIN2_ADDICON,0,(LPARAM)&sid);

	// unknwon contact
	sid.ptszDescription = LPGENT("Unknown Contact");
	mir_snprintf(name, SIZEOF(name), "DBE++_%d", ICO_OFFLINE);
	sid.pszName = name;
	sid.iDefaultIndex = -ICO_OFFLINE;
	CallService(MS_SKIN2_ADDICON,0,(LPARAM)&sid);

	// known contact
	sid.ptszDescription = LPGENT("Known Contact");
	mir_snprintf(name, SIZEOF(name), "DBE++_%d", ICO_ONLINE);
	sid.pszName = name;
	sid.iDefaultIndex = -ICO_ONLINE;
	CallService(MS_SKIN2_ADDICON,0,(LPARAM)&sid);
}

HICON LoadSkinnedDBEIcon(int icon)
{
	HICON hIcon = 0;
	if (UsingIconManager)
	{
		char name[32];
		mir_snprintf(name, SIZEOF(name), "DBE++_%d", icon);

		hIcon = (HICON)CallService(MS_SKIN2_GETICON,0,(LPARAM)name);
	}
	if (!hIcon)
		return LoadIcon(hInst, MAKEINTRESOURCE(icon));
	else
		return hIcon;
}


int AddIconToList(HIMAGELIST hil, HICON hIcon)
{

	if (!hIcon || !hil) return 0;

	ImageList_AddIcon(hil, hIcon);

	return 1;

}


static PROTOCOLDESCRIPTOR **protocols = NULL;
static int protoCount = 0;
static int shift = 0;


void AddProtoIconsToList(HIMAGELIST hil, int newshift)
{
    HICON hIcon;
	int i;
	shift = newshift;

	CallService(MS_PROTO_ENUMPROTOCOLS,(WPARAM)&protoCount,(LPARAM)&protocols);

	for(i = 0 ;i < protoCount; i++)
	{
		if (protocols[i]->type != PROTOTYPE_PROTOCOL)
			continue;

		if (hIcon=LoadSkinnedProtoIcon(protocols[i]->szName, ID_STATUS_ONLINE))
			AddIconToList(hil, hIcon);
		else
			AddIconToList(himl, LoadSkinnedDBEIcon(ICO_ONLINE));
	}
}


int GetProtoIcon(char *szProto)
{
    int result = DEF_ICON;
	int i, n = 0;

    if (!protoCount || !protocols || !szProto) return result;

	for(i = 0 ;i < protoCount; i++)
	{
		if (protocols[i]->type != PROTOTYPE_PROTOCOL)
			continue;

		if (!mir_strcmp(protocols[i]->szName, szProto))
		{
			result = n + shift;
			break;
		}

		n++;

	}

	return result;
}


BOOL IsProtocolLoaded(char* pszProtocolName)
{
/*
    if (pszProtocolName && pszProtocolName[0])
    {
		int res = CallService(MS_PROTO_ISPROTOCOLLOADED, 0, (LPARAM)pszProtocolName);

		if (res != CALLSERVICE_NOTFOUND && res)
			return TRUE;
	}
*/
	int i;

    if (protoCount)
		for(i = 0 ;i < protoCount; i++)
		{
			if (protocols[i]->type != PROTOTYPE_PROTOCOL)
				continue;

			if (!mir_strcmp(protocols[i]->szName, pszProtocolName))
				return TRUE;
		}

	return FALSE;
}
