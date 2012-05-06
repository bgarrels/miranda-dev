/*
Miranda Text Control for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
				Copyright (C) 2005 Victor Pavlychko (nullbie@gmail.com),
							  2010 Merlin_de

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

#include "headers.h"

HANDLE htuDefault = 0;

TextUser *textUserFirst=0;
TextUser *textUserLast=0;

void LoadTextUsers()
{
	htuDefault = MTI_TextUserAdd("Text Controls", MTEXT_FANCY_MASK);
}

void UnloadTextUsers()
{
	while (textUserFirst)
	{
		delete [] textUserFirst->name;
		TextUser *next = textUserFirst->next;
		delete [] textUserFirst;
		textUserFirst = next;
	}
}

HANDLE DLL_CALLCONV 
MTI_TextUserAdd(const char *userTitle, DWORD options)
{
	TextUser *textUserNew = new TextUser;
	textUserNew->name = new char [lstrlenA(userTitle)+1];
	lstrcpyA(textUserNew->name, userTitle);
	textUserNew->options =
		(DBGetContactSettingDword(0, MODULNAME, userTitle, options)&MTEXT_FANCY_MASK) | (textUserNew->options&MTEXT_SYSTEM_MASK);
	DBWriteContactSettingDword(0, MODULNAME, userTitle, textUserNew->options);
	textUserNew->prev = textUserLast;
	textUserNew->next = 0;
	if (textUserLast)
	{
		textUserLast->next = textUserNew;
		textUserLast = textUserNew;
	} else
	{
		textUserFirst = textUserLast = textUserNew;
	}
	return (HANDLE)textUserNew;
}

DWORD TextUserGetOptions(HANDLE userHandle)
{
	if (!userHandle) return 0;
	return ((TextUser *)userHandle)->options;
}

void TextUserSetOptions(HANDLE userHandle, DWORD options)
{
	if (!userHandle) return;
	((TextUser *)userHandle)->options = options;
}

void TextUsersSave()
{
	for (TextUser *textUser = textUserFirst; textUser; textUser = textUser->next)
		DBWriteContactSettingDword(0, MODULNAME, textUser->name, textUser->options);
}

void TextUsersReset()
{
	for (TextUser *textUser = textUserFirst; textUser; textUser = textUser->next)
		textUser->options =
			(DBGetContactSettingDword(0, MODULNAME, textUser->name, 0)&MTEXT_FANCY_MASK) | (textUser->options&MTEXT_SYSTEM_MASK);
}
