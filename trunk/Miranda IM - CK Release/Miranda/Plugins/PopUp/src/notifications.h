#ifndef __notifications_h__
#define __notifications_h__

/*
Popup Plus plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	2002 Luca Santarelli
							2004-2007 Victor Pavlychko
							2010-2012 Merlin_de, MPK

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

struct POPUPTREEDATA
{
	int cbSize;
	DWORD signature;
	LPTSTR pszTreeRoot;
	LPTSTR pszDescription;
	BYTE typ;
	union {
		POPUPNOTIFICATION	notification;
		POPUPCLASS			pupClass;
	};
	BYTE enabled;
	DWORD disableWhen;
	int timeoutValue;
	char leftAction[MAXMODULELABELLENGTH];
	char rightAction[MAXMODULELABELLENGTH];
	COLORREF colorBack;			//realy needet ??
	COLORREF colorText;			//realy needet ??
};

extern LIST<POPUPTREEDATA> gTreeData;
extern HANDLE g_hntfError, g_hntfWarning, g_hntfNotification;

int TreeDataSortFunc(const POPUPTREEDATA *p1, const POPUPTREEDATA *p2);
HANDLE FindTreeData(LPTSTR group, LPTSTR name, BYTE typ);
void UnloadTreeData();

void LoadNotifications();
HANDLE RegisterNotification(POPUPNOTIFICATION *notification);

void FillNotificationData(POPUPDATA2 *ppd, DWORD *disableWhen);
bool PerformAction(HANDLE hNotification, HWND hwnd, UINT message, WPARAM wparal, LPARAM lparam);
bool IsValidNotification(HANDLE hNotification);

void LoadNotificationSettings(POPUPTREEDATA *ptd, char* szModul);
void SaveNotificationSettings(POPUPTREEDATA *ptd, char* szModul);

#endif // __notifications_h__
