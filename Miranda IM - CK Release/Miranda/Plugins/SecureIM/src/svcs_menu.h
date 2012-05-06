#ifndef __SVCS_MENU_H__
#define __SVCS_MENU_H__

/*
SecureIM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 	2003 Johell
							2005-2009 Baloo

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

#include <windows.h>

INT_PTR __cdecl Service_IsContactSecured(WPARAM,LPARAM);

INT_PTR __cdecl Service_CreateIM(WPARAM,LPARAM);
INT_PTR __cdecl Service_DisableIM(WPARAM,LPARAM);

INT_PTR __cdecl Service_StatusDis(WPARAM,LPARAM);
INT_PTR __cdecl Service_StatusEna(WPARAM,LPARAM);
INT_PTR __cdecl Service_StatusTry(WPARAM,LPARAM);

INT_PTR __cdecl Service_PGPdelKey(WPARAM,LPARAM);
INT_PTR __cdecl Service_PGPsetKey(WPARAM,LPARAM);

INT_PTR __cdecl Service_GPGdelKey(WPARAM,LPARAM);
INT_PTR __cdecl Service_GPGsetKey(WPARAM,LPARAM);

INT_PTR __cdecl Service_ModeNative(WPARAM,LPARAM);
INT_PTR __cdecl Service_ModePGP(WPARAM,LPARAM);
INT_PTR __cdecl Service_ModeGPG(WPARAM,LPARAM);
INT_PTR __cdecl Service_ModeRSAAES(WPARAM,LPARAM);

#endif
