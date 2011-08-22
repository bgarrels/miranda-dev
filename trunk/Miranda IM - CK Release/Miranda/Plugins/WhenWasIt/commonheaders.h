/*
WhenWasIt (birthday reminder) plugin for Miranda IM

Copyright � 2006-2011 Cristian Libotean

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
*/

#ifndef M_WWI_COMMONHEADERS_H
#define M_WWI_COMMONHEADERS_H

#define MIRANDA_VER    0x0900
#define MIRANDA_CUSTOM_LP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <math.h>

#include "resource.h"
#include "version.h"

#include "utils.h"
#include "date_utils.h"
#include "services.h"
//#include "events.h"
#include "hooked_events.h"
#include "icons.h"
#include "notifiers.h"
//#include "birthdays.h"
#include "win2k.h"
#include "newpluginapi.h"
#include "m_database.h"
#include "m_system.h"
#include "m_skin.h"
#include "m_options.h"
#include "m_clist.h"
#include "m_langpack.h"
#include "m_history.h"
#include "m_contacts.h"
#include "m_popup.h"
#include "m_fontservice.h"
#include "m_utils.h"
#include "m_icolib.h"
#include "m_cluiframes.h"
#include "m_clui.h"
#include "m_protosvc.h"
#include "m_message.h"

#include "m_toptoolbar.h"
#include "m_metacontacts.h"
#include "m_updater.h"
#include "m_extraicons.h"

extern char ModuleName[];
extern HINSTANCE hInstance;
extern HWND hBirthdaysDlg;
extern HWND hUpcomingDlg;
extern HANDLE hAddBirthdayWndsList;

struct CommonData{
	DWORD foreground;
	DWORD background;
	int checkInterval;
	int daysInAdvance;
	int popupTimeout;
	int popupTimeoutToday;
	int clistIcon;
	int bUsePopups;
	int bUseClistIcon;
	int bUseDialog;
	int bIgnoreSubcontacts;
	int cShowAgeMode;
	int bNoBirthdaysPopup;
	int cSoundNearDays;
	int cDefaultModule;
	int lPopupClick;
	int rPopupClick;
	int bOncePerDay;
	int cDlgTimeout;
	int notifyFor;
	int daysAfter;
	int bOpenInBackground;
};

extern CommonData commonData;

#endif //M_WWI_COMMONHEADERS_H