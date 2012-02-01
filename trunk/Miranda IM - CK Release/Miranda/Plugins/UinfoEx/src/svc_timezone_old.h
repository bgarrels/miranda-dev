/*
UserinfoEx plugin for Miranda IM

Copyright:
� 2006-2010 DeathAxe, Yasnovidyashii, Merlin, K. Romanov, Kreol

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

===============================================================================

File name      : $HeadURL: http://userinfoex.googlecode.com/svn/trunk/svc_timezone_old.h $
Revision       : $Revision: 194 $
Last change on : $Date: 2010-09-20 15:57:18 +0400 (Пн, 20 сен 2010) $
Last change by : $Author: ing.u.horn $

===============================================================================
*/
#ifndef _SVC_TIMEZONE_H_OLD
#define _SVC_TIMEZONE_H_OLD

#include "svc_timezone.h"
#define TZINDEX_UNSPECIFIED	 -100

/**
 * This structure is used by GetTimeZoneInformationByIndex to retrieve
 * timezone information from windows' registry
 **/
struct TZ_MAP 
{
	DWORD	Index;
	LPTSTR Name;
};

/**
 * This structure is an element of the CTzManager.
 * It holds information about a timezone, which are required
 * to display the correct time of a contact which is not
 * in the same timezone as the owner contact.
 **/
struct CTimeZone : public REG_TZI_FORMAT
{
	LPTSTR	ptszName;
	LPTSTR	ptszDisplay;
	DWORD	dwIndex;			//old, only supportet in win9x

	/**
	 * This is the default constructure, which resets
	 * all attributes to NULL.
	 **/
	CTimeZone();

	/**
	 * The default construcor's task ist to clear out
	 * all pieces of the used memory.
	 **/
	~CTimeZone();

	/**
	 * This method can be used to basically convert a Windows
	 * timezone to the format, known by miranda.
	 *
	 * @warning		This operation does not work vice versa in
	 *				all cases, as there are sometimes more then
	 *				one Windows timezones with the same Bias.
	 **/
	BYTE ToMirandaTimezone() const;

	/**
	 * This operator translates the content of this object to
	 * a TIME_ZONE_INFORMATION structure as it is required by 
	 * several windows functions.
	 **/
	operator TIME_ZONE_INFORMATION() const;
};

typedef INT_PTR (*PEnumNamesProc)(CTimeZone* pTimeZone, INT index, LPARAM lParam);

CTimeZone*	GetContactTimeZone(HANDLE hContact);
CTimeZone*	GetContactTimeZone(HANDLE hContact, LPCSTR pszProto);
WORD		GetContactTimeZoneCtrl(HANDLE hContact, LPCSTR pszProto, CTimeZone** pTimeZone);
LPCTSTR		GetContactTimeZoneDisplayName(HANDLE hContact);
INT_PTR		EnumTimeZones(PEnumNamesProc enumProc, LPARAM lParam);

INT_PTR		GetContactTimeZoneInformation_old(WPARAM wParam, LPARAM lParam);
INT_PTR		GetContactLocalTime_old(WPARAM wParam, LPARAM lParam);

VOID		SvcTimezoneSyncWithWindows();
VOID		SvcTimezoneLoadModule_old();

#endif /* _SVC_TIMEZONE_H_OLD */