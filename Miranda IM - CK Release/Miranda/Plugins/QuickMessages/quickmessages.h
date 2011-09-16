/*
Quick Messages plugin for Miranda IM

Copyright (C) 2008 Danil Mozhar

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#define _CRT_SECURE_NO_DEPRECATE


#include <tchar.h>
#include <windows.h>
#include "resource.h"
#include <stdio.h>
#include <commctrl.h>
#include <Richedit.h>

#define MIRANDA_VER  0x0600
#include "..\..\include\m_plugins.h"
#include "..\..\include\newpluginapi.h"
#include "..\..\include\m_utils.h"
#include "..\..\include\m_system.h"
#include "..\..\include\m_clist.h"
#include "..\..\include\m_options.h"
#include "..\..\include\m_database.h"
#include "..\..\include\m_protomod.h"
#include "..\..\include\m_langpack.h"
#include "..\..\include\m_icolib.h"
#include "..\..\include\m_message.h"
#include "..\..\include\m_contacts.h"

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



