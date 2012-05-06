/*
Google Extension plugin for 
Miranda IM: the free IM client for Microsoft* Windows*
and the Jabber plugin.

Authors
			Copyright (C) 2011-2012	bems@jabber.org
									George Hazan (ghazan@jabber.ru)

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

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_WARNINGS

#if defined( UNICODE ) && !defined( _UNICODE )
	#define _UNICODE
#endif

#include <assert.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>
#include <tchar.h>

// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <PrSht.h>
#include <shellapi.h>

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

#include <winsock.h>

// Miranda plugin API
#include <newpluginapi.h>
#include <m_database.h>

#pragma warning(push)
#pragma warning(disable:4996)

#include <m_system.h>
#include <m_popup.h>
#include <m_jabber.h>
#include <m_langpack.h>
#include <m_protosvc.h>
#include <m_protomod.h>
#include <m_options.h>
#include <m_utils.h>
#include <m_clist.h>
#include <m_avatars.h>
#include <m_netlib.h>
#pragma warning(pop)

#if _MSC_VER < 1400
	#define _tstoi64 _ttoi
	#define _tcstoui64(A,B,C) _ttoi(A)
#endif