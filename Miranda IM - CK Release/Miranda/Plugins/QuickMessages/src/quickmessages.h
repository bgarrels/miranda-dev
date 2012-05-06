/*
Quick Messages plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2008 Danil Mozhar


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

#define _CRT_SECURE_NO_DEPRECATE
#define MIRANDA_VER    0x0A00

#include <tchar.h>
#include <windows.h>
#include "resource.h"
#include <stdio.h>
#include <commctrl.h>
#include <Richedit.h>

//Miranda IM
#include "m_plugins.h"
#include "newpluginapi.h"
#include "m_utils.h"
#include "m_system.h"
#include "m_clist.h"
#include "m_options.h"
#include "m_database.h"
#include "m_protomod.h"
#include "m_langpack.h"
#include "m_icolib.h"
#include "m_message.h"
#include "m_contacts.h"

//ExternalAPI
#include "Utils.h"
#include "m_msg_buttonsbar.h"
#include "m_updater.h"

#pragma optimize("gsy",on)

#define MODULENAME "Quick Messages"
#define PLGNAME "QuickMessages"

extern struct LIST_INTERFACE li;
extern HINSTANCE hinstance;
extern HANDLE hIcon;
extern ListData* ButtonsList[100];
extern SortedList* QuickList;
extern BOOL g_bRClickAuto;
extern BOOL g_bLClickAuto;
extern BOOL g_bQuickMenu;
extern int g_iButtonsCount;

//#define MIIM_STRING	0x00000040



int AddIcon(HICON icon, char *name, char *description);
int OptionsInit(WPARAM,LPARAM);


#define IDC_MESSAGE	        1002
#define IDC_CHATMESSAGE         1009

#define SIZEOF(X)(sizeof(X)/sizeof(X[0]))

#ifdef _UNICODE
#define QMESSAGES_NAME "quickmessages_u"
#else
#define QMESSAGES_NAME "quickmessages_a"
#endif 

#define QMESSAGES_VERSION_URL "http://miranda.radicaled.ru/public/updater/quickmessages.txt"

#define QMESSAGES_CHAGELOG_URL "http://miranda.radicaled.ru/public/quickmessages/changelog_en.txt"

#define QMESSAGES_UPDATE_URL "http://miranda.radicaled.ru/public/quickmessages/"QMESSAGES_NAME".zip"

#define QMESSAGES_UPDATE_URL "http://miranda.radicaled.ru/public/quickmessages/"QMESSAGES_NAME".zip"

#define QMESSAGES_VERSION_PREFIX "QuickMessages "




