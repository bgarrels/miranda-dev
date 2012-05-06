#ifndef __STARTUPSTATUSHEADER
#define __STARTUPSTATUSHEADER

/*
AdvancedAutoAway Plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) Copyright 2003-2006 P. Boon

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

#include <windows.h>
#include <wininet.h>
#include <shlobj.h>
#include <m_options.h>
#include <m_clui.h>
#include <m_toptoolbar.h>
#include <m_toolbar.h>
#include "version.h"
#define MODULENAME						"StartupStatus"
#include "db_helpers.h"

struct TSSSetting : public PROTOCOLSETTINGEX
{
	TSSSetting( PROTOACCOUNT* pa );
	TSSSetting( int profile, PROTOACCOUNT* pa );
	~TSSSetting();

	__inline void* operator new( size_t size ) {	return calloc( 1, size ); }
	__inline void operator delete( void* p ) { free( p ); }
};

typedef OBJLIST<TSSSetting> TSettingsList;

typedef struct {
	int profile;
	char *szProto;
	char *msg;
} PROFILECE;

typedef struct {
	char *szName;
	TSettingsList* ps;
	BOOL showDialog;
	BOOL createTtb;
	BOOL createMmi;
	BOOL inSubMenu;
	BOOL regHotkey;
	WORD hotKey;
} PROFILEOPTIONS;

typedef struct {
	ATOM id;
	int profile;
} HKINFO;

#define UM_REINITPROFILES	WM_USER + 1
#define UM_SETPROFILE		WM_USER + 2
#define UM_SETPROTOCOL		WM_USER + 3
#define UM_SETSTATUSMSG		WM_USER + 4
#define UM_ADDPROFILE		WM_USER + 5
#define UM_DELPROFILE		WM_USER + 6
#define UM_REINITDOCKED		WM_USER + 7
#define UM_REINITWINSTATE	WM_USER + 8
#define UM_REINITWINSIZE	WM_USER + 9

#define CLUIINTM_REDRAW (WM_USER+100)

#define MODULE_CLIST					"CList"
#define MODULE_CLUI						"CLUI"
#define SETTING_STATUS					"Status"

#define SETTING_SETWINSTATE				"SetState"
#define SETTING_WINSTATE				"State"

#define SETTING_SETDOCKED				"SetDocked"
#define SETTING_DOCKED					"Docked"

#define SETTING_SHOWDIALOG				"ShowDialog"
#define SETTING_OFFLINECLOSE			"OfflineOnClose"
#define SETTING_SETPROFILE				"SetStatusOnStartup"
#define SETTING_AUTODIAL				"AutoDial"
#define SETTING_AUTOHANGUP				"AutoHangup"

#define SETTING_TOOLWINDOW				"ToolWindow"

#define SETTING_OVERRIDE				"AllowOverride"

#define SETTING_SETWINLOCATION			"SetWinLoc"
#define SETTING_XPOS					"x"
#define SETTING_YPOS					"y"

#define SETTING_SETWINSIZE				"SetWinSize"
#define SETTING_WIDTH					"Width"
#define SETTING_HEIGHT					"Height"
#define SETTING_AUTOSIZE				"AutoSize"

#define SETTING_PROFILECOUNT			"ProfileCount"
#define SETTING_DEFAULTPROFILE			"DefaultProfile"
#define SETTING_PROFILENAME				"ProfileName"
#define SETTING_CREATETTBBUTTON			"CreateTTBButton"
#define SETTING_PROFILE_STSMSG			"StatusMsg"
#define SETTING_SHOWCONFIRMDIALOG		"profile_ShowDialog"
#define SETTING_CREATEMMITEM			"CreateMMItem"
#define SETTING_INSUBMENU				"InSubMenu"
#define SETTING_REGHOTKEY				"RegHotKey"
#define SETTING_HOTKEY					"HotKey"
#define SETTING_PROFILENO				"ProfileNo"

#define SETTING_SETPROFILEDELAY			"SetStatusDelay"
#define SETTING_DLGTIMEOUT				"DialogTimeout"

#define SHORTCUT_DESC					"Miranda IM"
#define SHORTCUT_FILENAME				"\\Miranda IM.lnk"

#define DOCKED_NONE						0
#define DOCKED_LEFT						1
#define DOCKED_RIGHT					2

#ifndef MS_CLIST_ADDSTATUSMENUITEM
#define MS_CLIST_ADDSTATUSMENUITEM		"CList/AddStatusMenuItem"
#endif
#define MS_SS_MENUSETPROFILEPREFIX		"StartupStatus/SetProfile_"

// options
int OptionsInit(WPARAM wparam,LPARAM lparam);

// startupstatus
int LoadMainOptions();

int CompareSettings( const TSSSetting* p1, const TSSSetting* p2 );

TSettingsList* GetCurrentProtoSettings();

// profile
int GetProfile(int profileID, TSettingsList& arSettings );
char *GetStatusMessage(int profile, char *szProto);

INT_PTR LoadAndSetProfile(WPARAM wParam, LPARAM lParam);
INT_PTR GetProfileCount(WPARAM wParam, LPARAM lParam);
INT_PTR GetProfileName(WPARAM wParam, LPARAM lParam);

int RemoveTopToolbarButtons();
int CreateTopToolbarButtons(WPARAM wParam, LPARAM lParam);

int RemoveToolbarButtons();
int CreateToolbarButtons(WPARAM wParam, LPARAM lParam);
void RegisterButtons();

int LoadProfileModule();
int InitProfileModule();
int ReinitProfileModule();
int DeinitProfilesModule();

#endif //__STARTUPSTATUSHEADER
