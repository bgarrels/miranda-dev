#ifndef __debug_h__
#define __debug_h__

/*
Splash Screen Plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
Copyright (C) 2004-2007 nullbie
Copyright (C) 2005-2007 Thief

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

#define PlugName "SplashScreen"
extern TCHAR szLogFile[MAX_PATH];

/*
 * output a notification message.
 * may accept a hContact to include the contacts nickname in the notification message...
 * the actual message is using printf() rules for formatting and passing the arguments...
 *
 */

int inline _DebugPopup(HANDLE hContact, TCHAR *fmt, ...)
{
	POPUPDATAT ppd;
	va_list va;
	TCHAR debug[1024];

	va_start(va, fmt);
	mir_sntprintf(debug, SIZEOF(debug), fmt, va);
    
	if(CallService(MS_POPUP_QUERY, PUQS_GETSTATUS, 0) == 1)
	{
		ZeroMemory((void *)&ppd, sizeof(ppd));
		ppd.lchContact = hContact;
		ppd.lchIcon = LoadSkinnedIcon(SKINICON_OTHER_MIRANDA);
		if(hContact != 0)
			_tcsncpy_s(ppd.lptzContactName, (TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)hContact, GCDNF_TCHAR), MAX_CONTACTNAME);
		else
			_tcsncpy_s(ppd.lptzContactName, _T(PlugName), MAX_CONTACTNAME);
		_tcsncpy_s(ppd.lptzText, debug, MAX_SECONDLINE - 20);
		ppd.colorText = RGB(255,255,255);
		ppd.colorBack = RGB(255,0,0);
		CallService(MS_POPUP_ADDPOPUP, (WPARAM)&ppd, 0);
	}
	return 0;
}

/*
 * initialize logfile
 */

int inline initLog()
{
	fclose(_tfopen(szLogFile, _T("w")));
	return 0;
}

/*
 * log timestamp
 */

void inline logTimeStamp()
{
	FILE *f = _tfopen(szLogFile, _T("a"));
	_ftprintf(f, _T("Time:\t\t\t\t%s\n"), _T(__TIME__));
	fclose(f);
}

/*
 * logging func
 */

void inline logMessage(TCHAR *func, TCHAR *msg)
{
	FILE *f = _tfopen(szLogFile, _T("a"));
	_ftprintf(f, _T("%s:\t\t%s\n"), func, msg);
	fclose(f);
}

#endif // __debug_h__