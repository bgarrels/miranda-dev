/*
Basic History plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2011-2012 Krzysztof Kral

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

#ifndef _WIN64
#define _USE_32BIT_TIME_T
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <richedit.h>
#include <time.h>
#include <commctrl.h>
#include <commdlg.h>
#include <tom.h>
#include <richole.h>
#include <Shlobj.h>
#include <shellapi.h>

#include <map>
#include <hash_set>
#include <vector>
#include <queue>
#include <locale>
#include <string>
#include <fstream>

#define MIRANDA_VER		0x0900
#define MIRANDA_CUSTOM_LP

#include <newpluginapi.h>
#include <m_langpack.h>
#include <m_system.h>
#include <m_clist.h>
#include <m_skin.h>
#include <m_history.h>
#include <m_database.h>
#include <m_icolib.h>
#include <m_timezones.h>
#include <m_contacts.h>
#include <m_protocols.h>
#include <m_button.h>
#include <m_options.h>
#include <m_fontservice.h>
#include <m_hotkeys.h>
#include <m_message.h>
#include <m_protosvc.h>
#include <m_icq.h>
#include <m_clc.h>
#include <m_utils.h>

#include <win2k.h>

//External Api
#include "m_updater.h"
#include "m_smileyadd.h"
#include "m_toolbar.h"
#include "m_metacontacts.h"

#define HISTORY_HK_FIND 100
#define HISTORY_HK_FINDNEXT 101
#define HISTORY_HK_FINDPREV 102
#define HISTORY_HK_MATCHCASE 103
#define HISTORY_HK_MATCHWHOLE 104
#define HISTORY_HK_SHOWCONTACTS 105
#define HISTORY_HK_ONLYIN 106
#define HISTORY_HK_ONLYOUT 107
#define HISTORY_HK_DELETE 108
#define HISTORY_HK_ONLYGROUP 109
#define HISTORY_HK_EXRHTML 110
#define HISTORY_HK_EXPHTML 111
#define HISTORY_HK_EXTXT 112
#define HISTORY_HK_EXBIN 113
#define HISTORY_HK_IMPBIN 114
#define HISTORY_HK_EXDAT 115
#define HISTORY_HK_IMPDAT 116

#define EVENTTYPE_STATUSCHANGE          25368
#define EVENTTYPE_SMTPSIMPLE            2350