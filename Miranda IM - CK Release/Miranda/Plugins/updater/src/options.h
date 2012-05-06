#ifndef _OPTIONS_INC
#define _OPTIONS_INC

/*
Updater plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	2005-2006 Scott Ellis
							2009-2012 Boris Krasnovskiy

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

#include "services.h"
#include "icons.h"

typedef enum {VR_MAJOR, VR_MINOR, VR_RELEASE, VR_BUILD} VersionRequirement;

typedef struct Options_tag {
	bool check_on_startup;
	bool check_once;
	bool check_daily;
	bool no_conf_idle;
	bool backup;
	TCHAR temp_folder[MAX_PATH];
	TCHAR backup_folder[MAX_PATH];
	TCHAR data_folder[MAX_PATH];
	bool use_xml_backend;
	bool start_offline;
	bool popup_notify;
	bool set_colours;
	bool restart_menu_item;
	bool update_and_exit_menu_item;
	COLORREF bkCol;
	COLORREF textCol;
	VersionRequirement ver_req;
	bool save_zips;
	bool no_unzip;
	bool auto_dll_only;
	TCHAR zip_folder[MAX_PATH];
} Options;

extern Options options;

extern HWND hwndOptions;

#define WMU_CHECKING		(WM_USER + 41)
#define WMU_DONECHECKING	(WM_USER + 42)


int OptInit(WPARAM wParam, LPARAM lParam);

void LoadOptions();
void SaveOptions();

// do this after icolib support has been handled
void InitOptionsMenuItems();

#endif
