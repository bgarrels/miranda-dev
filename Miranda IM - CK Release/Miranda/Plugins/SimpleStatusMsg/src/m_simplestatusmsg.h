#ifndef M_SIMPLESTATUSMSG_H__
#define M_SIMPLESTATUSMSG_H__

/*
Simple Status Message plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2006-2011 Bartosz 'Dezeath' Bia³ek, 
			Copyright (C) 2005 Harven

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

// Change global status mode/message
// wParam = (int)new status, 0 if for current
// lParam = (LPARAM)(TCHAR *)status message
#define MS_SIMPLESTATUSMSG_SETSTATUS "SimpleStatusMsg/SetStatus"

// Bring up the status message dialog
// wParam = 0
// lParam = (LPARAM)(char *)protocol name, NULL if for all protocols
#define MS_SIMPLESTATUSMSG_SHOWDIALOG "SimpleStatusMsg/ShowDialog"

// Force a change of status mode/message. The status message dialog will
// appear depending on settings
// wParam = (int)new status
// lParam = (LPARAM)(char *)protocol name, NULL if for all protocols
// Returns 1 when changed without showing the status message dialog
#define MS_SIMPLESTATUSMSG_CHANGESTATUSMSG "SimpleStatusMsg/ChangeStatusMessage"

// Copy the status message of a contact to clipboard
// wParam = (WPARAM)(HANDLE)hContact
// lParam = 0
#define MS_SIMPLESTATUSMSG_COPYMSG  "SimpleStatusMsg/CopyMsg"

// Go to URL in status message of a contact
// wParam = (WPARAM)(HANDLE)hContact
// lParam = 0
#define MS_SIMPLESTATUSMSG_GOTOURLMSG  "SimpleStatusMsg/GoToURLMsg"

#endif // M_SIMPLESTATUSMSG_H__
