#ifndef __QUICK_REPLY_H__
#define __QUICK_REPLY_H__

/*
QuickReplies plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
Copyright (C) Copyright (C) 2010 Unsane

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

#define MIRANDA_VER 0x0A00

#include <windows.h>
#include <vector>
#include <string>

#include "tchar.h"

#include <newpluginapi.h>
#include <m_database.h>
#include <m_icolib.h>
#include <m_langpack.h>
#include <m_message.h>
#include <m_options.h>

#include "m_msg_buttonsbar.h"
#include "m_variables.h"

#include "../resource.h"
#include "../version.h"

#define MODULE_NAME				__INTERNAL_NAME
#define TEXT_LIMIT				2048
#define IDC_MESSAGE				1002
#define IDC_CHATMESSAGE			1009

#define MS_QUICKREPLIES_SERVICE	MODULE_NAME"/Service"

extern HINSTANCE hInstance;

extern BYTE iNumber;

extern HANDLE hOnOptInitialized;
extern HANDLE hOnButtonPressed;
extern HANDLE hQuickRepliesService;
extern HANDLE hOnModulesLoaded;
extern HANDLE hOnPreShutdown;

#ifdef _UNICODE
	typedef std::wstring tString;
#else
	typedef std::string tString;
#endif //_UNICODE

INT_PTR QuickRepliesService(WPARAM wParam, LPARAM lParam);

INT OnModulesLoaded(WPARAM wParam, LPARAM lParam);
INT OnOptInitialized(WPARAM wParam, LPARAM lParam);
INT OnButtonPressed(WPARAM wParam, LPARAM lParam);
INT OnPreShutdown(WPARAM wParam, LPARAM lParam);

#endif //__QUICK_REPLY_H__