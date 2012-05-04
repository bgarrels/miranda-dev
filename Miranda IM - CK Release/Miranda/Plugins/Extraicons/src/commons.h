#ifndef __COMMONS_H__
# define __COMMONS_H__

/*
Extraicons service plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			 Copyright (C) 2009-2012 Ricardo Pescuma Domenecci

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

#ifdef UNICODE
#error "Unicode not needed by this plugin"
#endif

#define _WIN32_IE 0x0600
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <time.h>
#include <commctrl.h>

#include <map>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

// Miranda headers and version
#define MIRANDA_VER    0x0A00

#include <newpluginapi.h>
#include <m_system.h>
#include <m_protocols.h>
#include <m_protosvc.h>
#include <m_clui.h>
#include <m_clist.h>
#include <m_cluiframes.h>
#include <m_contacts.h>
#include <m_langpack.h>
#include <m_database.h>
#include <m_options.h>
#include <m_utils.h>
#include <m_updater.h>
#include <m_metacontacts.h>
#include <m_icolib.h>
#include <m_skin.h>
#include <m_userinfo.h>

#include "mir_icons.h"

#include "resource.h"
#include "m_extraicons.h"

#include "ExtraIcon.h"
#include "ExtraIconGroup.h"
#include "CallbackExtraIcon.h"
#include "IcolibExtraIcon.h"
#include "usedIcons.h"
#include "DefaultExtraIcons.h"
#include "options.h"

#define MODULE_NAME		"ExtraIcons"

// Global Variables
extern HINSTANCE hInst;
extern PLUGINLINK *pluginLink;

#define MAX_REGS(_A_)	( sizeof(_A_) / sizeof(_A_[0]) )
#define FREE(_m_)		if (_m_ != NULL) { free(_m_); _m_ = NULL; }

#define ICON_SIZE 16

extern vector<HANDLE> hHooks;

extern vector<BaseExtraIcon*> registeredExtraIcons;
extern vector<ExtraIcon*> extraIconsByHandle;
extern vector<ExtraIcon*> extraIconsBySlot;
void RebuildListsBasedOnGroups(vector<ExtraIconGroup *> &groups);
ExtraIcon * GetExtraIconBySlot(int slot);

int GetNumberOfSlots();
int ConvertToClistSlot(int slot);

int Clist_SetExtraIcon(HANDLE hContact, int slot, HANDLE hImage);

static inline BOOL IsEmpty(const char *str)
{
	return str == NULL || str[0] == 0;
}

static inline int MIN(int a, int b)
{
	if (a <= b)
		return a;
	return b;
}

static inline int MAX(int a, int b)
{
	if (a >= b)
		return a;
	return b;
}

#endif // __COMMONS_H__
