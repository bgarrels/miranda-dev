/*
Yahoo protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Gennady Feldman (aka Gena01) 
			Laurent Marechal (aka Peorth)

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

#include "../yahoo.h"
#include <m_langpack.h>
#include <win2k.h>
#include "m_icolib.h"

#include "resource.h"

struct
{
	const char* szDescr;
	const char* szName;
	int   defIconID;
}
static iconList[] = {
	{	LPGEN("Main"),         "yahoo",      IDI_YAHOO      },
	{	LPGEN("Mail"),         "mail",       IDI_INBOX      },
	{	LPGEN("Profile"),      "profile",    IDI_PROFILE    },
	{	LPGEN("Refresh"),      "refresh",    IDI_REFRESH    },
	{	LPGEN("Address Book"), "yab",        IDI_YAB        },
	{	LPGEN("Set Status"),   "set_status", IDI_SET_STATUS },
	{	LPGEN("Calendar"),     "calendar",   IDI_CALENDAR   }
};

HANDLE hIconLibItem[SIZEOF(iconList)];

void CYahooProto::IconsInit( void )
{
	TCHAR szFile[MAX_PATH];
	char szSectionName[100];
	
	mir_snprintf(szSectionName, sizeof(szSectionName), "%s/%s", LPGEN("Protocols"), LPGEN("YAHOO"));
	GetModuleFileName(hInstance, szFile, SIZEOF(szFile));

	SKINICONDESC sid = {0};
	sid.cbSize = sizeof(SKINICONDESC);
	sid.ptszDefaultFile = szFile;
	sid.pszSection = szSectionName;
	sid.flags = SIDF_PATH_TCHAR;

	for ( int i = 0; i < SIZEOF(iconList); i++ ) {
		char szSettingName[100];
		mir_snprintf( szSettingName, sizeof( szSettingName ), "YAHOO_%s", iconList[i].szName );
		
		sid.pszName = szSettingName;
		sid.pszDescription = (char* )iconList[i].szDescr;
		sid.iDefaultIndex = -iconList[i].defIconID;
		hIconLibItem[i] = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);
	}
}

HICON CYahooProto::LoadIconEx( const char* name, bool big )
{
	char szSettingName[100];
	
	mir_snprintf( szSettingName, sizeof( szSettingName ), "YAHOO_%s", name );
	
	return ( HICON )CallService( MS_SKIN2_GETICON, big, (LPARAM)szSettingName );
}

HANDLE CYahooProto::GetIconHandle(int iconId)
{
	for (unsigned i=0; i < SIZEOF(iconList); i++)
		if (iconList[i].defIconID == iconId)
			return hIconLibItem[i];

	return NULL;
}

void CYahooProto::ReleaseIconEx(const char* name, bool big)
{
	char szSettingName[100];
	mir_snprintf(szSettingName, sizeof(szSettingName), "YAHOO_%s", name);
	YAHOO_CallService(big ? MS_SKIN2_RELEASEICONBIG : MS_SKIN2_RELEASEICON, 0, (LPARAM)szSettingName);
}
