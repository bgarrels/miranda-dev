/*
Jabber Protocol plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright (C) 2002-2004		Santithorn Bunchua
			Copyright (C) 2007			Maxim Mluhov, Victor Pavlychko, Artem Shpynov, Michael Stepura
			Copyright (C) 2008-09		Dmitriy Chervov
			Copyright (C) 2005-2012		George Hazan, Borkra

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

#include "../../include/m_version.h"

#define __FILEVERSION_STRING        MIRANDA_VERSION_FILEVERSION
#define __VERSION_STRING            MIRANDA_VERSION_STRING
#define __VERSION_DWORD             MIRANDA_VERSION_DWORD
#if defined (_UNICODE)
	#define __PLUGIN_NAME		    "Jabber Protocol (Unicode) Release"
#else
	#define __PLUGIN_NAME		    "Jabber Protocol Release"
#endif
