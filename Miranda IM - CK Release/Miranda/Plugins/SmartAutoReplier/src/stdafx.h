/*
Smart Auto Replier (SAR) for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2004 - 2012 by Volodymyr M. Shcherbyna <volodymyr@shcherbyna.com>

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

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600

#define WIN32_LEAN_AND_MEAN
#define _MERGE_RDATA_

/// windows and crt headers (win32 api)
#include <windows.h>
#include <string>
#include <prsht.h>
#include <crtdbg.h>
#include "../resource.h"
#include <tchar.h>

#include <commctrl.h>

/// shell's headers... (windows shell)
#include <shlwapi.h>
#include <shellapi.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")

/// miranda's headers...
#define MIRANDA_CUSTOM_LP
#include <newpluginapi.h>
#include <m_system.h>
#include <m_protomod.h>
#include <m_protosvc.h>
#include <m_clist.h>
#include <m_skin.h>
#include <m_database.h>
#include <m_options.h>
#include <m_langpack.h>

/// plugin's specific headers...
#include "messageshandler.h"	/// messages handler
#include "settingshandler.h"	/// settings handler
#include "GUI/optionsDlg.h"		/// options handler
#include "comlogging.h"			/// common loggin helper macroses
#include "crushlog.h"			/// crush log handler

#define REFRESH_OPTS_MSG	WM_USER + 666

void OnRefreshSettings(void);