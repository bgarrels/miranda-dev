/*
Smart Auto Replier (SAR) for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2004 - 2012 by Volodymyr M. Shcherbyna <volodymyr@shcherbyna.com>

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

#include "stdafx.h"
#include "modemesshandler.h"

/// TODO: add comments and change char* to TCHAR* !!!

/// ctor
CModeMessHandler::CModeMessHandler(void)
{
}

/// dtor
CModeMessHandler::~CModeMessHandler(void)
{
}

TCHAR * CModeMessHandler::StatusModeToDbSetting(int status, TCHAR *suffix)
{
    LPTSTR prefix;
	static TCHAR str[64] = {0};

	switch(status) {
		case ID_STATUS_AWAY: prefix = TEXT("Away");	break;
		case ID_STATUS_NA: prefix = TEXT("Na");	break;
		case ID_STATUS_DND: prefix = TEXT("Dnd"); break;
		case ID_STATUS_OCCUPIED: prefix = TEXT("Occupied"); break;
		case ID_STATUS_FREECHAT: prefix = TEXT("FreeChat"); break;
		case ID_STATUS_ONLINE: prefix = TEXT("On"); break;
		case ID_STATUS_OFFLINE: prefix = TEXT("Off"); break;
		case ID_STATUS_INVISIBLE: prefix = TEXT("Inv"); break;
		case ID_STATUS_ONTHEPHONE: prefix = TEXT("Otp"); break;
		case ID_STATUS_OUTTOLUNCH: prefix = TEXT("Otl"); break;
		default: 
			return NULL;
	}
	lstrcpy(str, prefix); 
	_tcscat(str, suffix);

	return str;
}

LPTSTR CModeMessHandler::GetDefaultMessage(INT status)
{
	switch (status)
	{
		case ID_STATUS_AWAY:	return TranslateT("I've been away since %time%.");
		case ID_STATUS_NA:		return TranslateT("Give it up, I'm not in!");
		case ID_STATUS_OCCUPIED: return TranslateT("Not right now.");
		case ID_STATUS_DND: return TranslateT("Give a guy some peace, would ya?");
		case ID_STATUS_FREECHAT: return TranslateT("Well, I would talk to you if Miranda IM supported chat");
		case ID_STATUS_ONLINE: return TranslateT("Yep, I'm here.");
		case ID_STATUS_OFFLINE: return TranslateT("Nope, not here.");
		case ID_STATUS_INVISIBLE: return TranslateT("I'm hiding from the mafia.");
		case ID_STATUS_ONTHEPHONE: return TranslateT("That'll be the phone.");
		case ID_STATUS_OUTTOLUNCH: return TranslateT("Mmm...food.");
	}
	return NULL;
}

/// returnes mode message
/// looks like ret. val should be free(), but
/// read notes in the place where this func is used
LPTSTR CModeMessHandler::GetModeMessage(INT statusMode)
{
	/*DBVARIANT dbv = {0};
	INT nretVal = {0};	/// suppose failure..

	nretVal = DBGetContactSetting(NULL, TCHAR("SRAway"), StatusModeToDbSetting(statusMode, TCHAR("Msg")), &dbv);
	if (nretVal) /// unable to get away msg...
	{
		dbv.pszVal = _tcsdup(GetDefaultMessage(statusMode));
		if (dbv.pszVal == NULL) /// maybe will occure ;)
		{
			DBGetContactSetting(NULL, TCHAR("SRAway"), StatusModeToDbSetting(statusMode, TCHAR("Default")), &dbv);
		}
	}

	int i;
	TCHAR substituteStr[128] = {0};
	for(i = 0; dbv.pszVal[i]; i++)
	{
		if (dbv.pszVal[i] != '%')
			continue;
		if (!_tcsncicmp((TCHAR*)dbv.pszVal+i, TEXT("%time%"), 6))
			GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, NULL, NULL, substituteStr, sizeof(substituteStr) / sizeof (TCHAR));
		else if (!_tcsncicmp((TCHAR*)dbv.pszVal+i, TEXT("%date%"), 6))
			GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, NULL, NULL, substituteStr, sizeof(substituteStr) / sizeof(TCHAR));
		else
			continue;
		if (_tcslen(substituteStr) > 6) 
			dbv.pszVal = ((TCHAR*))realloc((TCHAR*)dbv.pszVal, _tcslen((TCHAR*)dbv.pszVal) + 1 + _tcslen(substituteStr) - 6);
		MoveMemory(dbv.pszVal + i + _tcslen(substituteStr), dbv.pszVal + i + 6, _tcslen(dbv.pszVal) - i - 5);
		CopyMemory(dbv.pszVal + i, substituteStr, _tcslen(substituteStr));
	}*/

	//return dbv.pszVal;

	return NULL;
}