/*
Skype protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright © 2008-2012 leecher, tweety, jls17,
						Laurent Marechal (aka Peorth)
						Gennady Feldman (aka Gena01) 

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

// System includes
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <process.h>
#include <time.h>
#include "resource.h"
#include "skype.h"

#pragma warning (push)
#pragma warning (disable: 4100) // unreferenced formal parameter

// Miranda database access
#include "newpluginapi.h"
#include "m_database.h"
#pragma warning (pop)

typedef struct
{
	TCHAR FullName[256];
	char HomePhone[256];
	char OfficePhone[256];
	char HomePage[256];
	TCHAR City[256];
	TCHAR Province[256];
	BYTE Sex;
	SYSTEMTIME Birthday;
} SkypeProfile;

void SkypeProfile_Load(SkypeProfile *pstProf);
void SkypeProfile_Save(SkypeProfile *pstProf);
void SkypeProfile_Free(SkypeProfile *pstProf);
void SkypeProfile_LoadFromSkype(SkypeProfile *pstProf);
void SkypeProfile_SaveToSkype(SkypeProfile *pstProf);
