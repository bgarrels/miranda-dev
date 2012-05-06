#ifndef __textusers_h__
#define __textusers_h__

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

#define MTEXT_GUI_MASK (MTEXT_BIDI_MASK|MTEXT_FANCY_SMILEYS|MTEXT_FANCY_BBCODES|MTEXT_FANCY_MATHMOD)

struct TextUser
{
	char *name;
	DWORD options;
	TextUser *prev, *next;
};

extern TextUser *textUserFirst;
extern TextUser *textUserLast;

void LoadTextUsers();
void UnloadTextUsers();

extern HANDLE htuDefault;

DLL_API HANDLE	DLL_CALLCONV	MTI_TextUserAdd(const char *userTitle, DWORD options);

DWORD TextUserGetOptions(HANDLE userHandle);
void TextUserSetOptions(HANDLE userHandle, DWORD options);
void TextUsersSave();
void TextUsersReset();

#endif // __textusers_h__
