/*
SimpleAR (based on J. Lawler BaseProtocol) plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2004 Daniel Savi (dss@brturbo.com)
			Copyright (C) 2010-2012 Stark Wong, Mataes, Mikel-Ard-Ri

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

//need some work after that ...
#define _CRT_SECURE_NO_DEPRECATE

#define MIRANDA_VER    0x0A00

// Windows Header Files
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <time.h>
#include <stddef.h>
#include <process.h>
#include <string.h>
#include <winsock.h>
#include <string>
#include <winbase.h>

//Miranda SDK headers
#include "newpluginapi.h"
#include "m_clist.h"
#include "m_clui.h"
#include "m_skin.h"
#include "m_langpack.h"
#include "m_protomod.h"
#include "m_database.h"
#include "m_system.h"
#include "m_protocols.h"
#include "m_userinfo.h"
#include "m_options.h"
#include "m_protosvc.h"
#include "m_utils.h"
#include "m_ignore.h"
#include "m_clc.h"
#include <m_variables.h>

#include "Resource.h"
#include "Version.h"

#define protocolname		"SimpleAutoReplier" 

#define KEY_ENABLED			"Enabled"
#define KEY_HEADING			"Heading"
#define KEY_REPEATINTERVAL	"RepeatInterval"

//General
extern HINSTANCE hinstance;
extern BOOL fEnabled;
extern INT interval;
extern TCHAR* ptszDefaultMsg[6];

extern INT OptInit(WPARAM wParam,LPARAM lParam);
