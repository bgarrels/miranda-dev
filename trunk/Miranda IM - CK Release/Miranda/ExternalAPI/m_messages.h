#ifndef __MESSAGES_H
#define __MESSAGES_H

/*
Miranda IM: the free IM client for Microsoft* Windows*

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

===============================================================================
*/

//#include "list.h"

// structure for chained list of handles (window handles, account handles, whatever)
struct WndHandles
{
	HANDLE Handle;

	struct WndHandles *Next;
};

#define WM_YAMN		WM_APP+0x2800		//(0xA800 in fact)
enum
{
	WM_YAMN_CHANGEHOTKEY=WM_YAMN,
	WM_YAMN_CHANGETIME,

//ChangeStatus message
//WPARAM- (HACCOUNT) Account whose status is changed
//LPARAM- new status of account
	WM_YAMN_CHANGESTATUS,

//StopAccount message
//WPARAM- (HACCOUNT) Account, which should stop its work and finish immidiatelly
	WM_YAMN_STOPACCOUNT,

//Account content changed
	WM_YAMN_CHANGECONTENT,

	WM_YAMN_UPDATEMAILS,

	WM_YAMN_NOTIFYICON,

	WM_YAMN_CHANGESTATUSOPTION,

	WM_YAMN_SHOWSELECTED,
};

#endif
