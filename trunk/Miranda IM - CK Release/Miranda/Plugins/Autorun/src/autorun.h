/*
Autorun plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author 
			Copyright (C) 2002-2012 Sergey V. Gershovich a.k.a. Jazzy$

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

#define IDD_OPT_AUTORUN 101
#define IDC_AUTORUN 102

#define SUB_KEY "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define ModuleName "Autorun"

// Plugin UUID for New plugin loader
// req. 0.7.18+ core
// {EB0465E2-CEEE-11DB-83EF-C1BF55D89593}

#define MIID_AUTORUN  {0xeb0465e2, 0xceee, 0x11db, { 0x83, 0xef, 0xc1, 0xbf, 0x55, 0xd8, 0x95, 0x93}}

HKEY ROOT_KEY = HKEY_CURRENT_USER;

