#ifndef __HISTORYSWEEPER_H__
#define __HISTORYSWEEPER_H__

/*
Historysweeper plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2002-2003  Sergey V. Gershovich
			Copyright (C) 2006-2009  Boris Krasnovskiy


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

#include <m_stdhdr.h>

#include <windows.h>
#include <commctrl.h>

#include <win2k.h>
#include <newpluginapi.h>
#include <m_system.h>
#include <m_clc.h>
#include <m_clist.h>
#include <m_clui.h>
#include <m_database.h>
#include <m_langpack.h>
#include <m_options.h>
#include <m_message.h>
#include <m_icolib.h>
#include <m_clistint.h>
#include <m_skin.h>

#include "resource.h"

// Plugin name
#define ModuleName "History Sweeper"

// Plugin UUID for New plugin loader
// req. 0.7.18+ core
// {4AA5EACC-CE9D-11DB-B131-5E9956D89593}
#define MIID_HISTORYSWEEPER {0x4aa5eacc, 0xce9d, 0x11db, { 0xb1, 0x31, 0x5e, 0x99, 0x56, 0xd8, 0x95, 0x93 }}

// Services
#define MS_HS_SWEEPCONTACTHISTORY      "HistorySweeper/SweepContactHistory"
#define MS_HS_SWEEPHISTORYFROMMAINMENU "HistorySweeper/SweepHistoryFromMainMenu"
#define MS_HS_NOCONTACTHISTORY         "HistorySweeper/NoContactHistory"

// main.c
extern HINSTANCE hInst;
void ModifyMainMenu(void);

// options.c
extern const char* menu_items[];
int HSOptInitialise(WPARAM wParam, LPARAM lParam);

void InitIcons(void);
HICON LoadIconEx(const char* name);
HANDLE GetIconHandle(const char* name);
void  ReleaseIconEx(const char* name);


// historysweeper.c
void SSAction(void);

INT_PTR MainMenuAction(WPARAM wParam, LPARAM lParam);
INT_PTR ContactMenuAction(WPARAM wParam, LPARAM lParam);
int OnWindowEvent(WPARAM wParam, LPARAM lParam);

#endif
