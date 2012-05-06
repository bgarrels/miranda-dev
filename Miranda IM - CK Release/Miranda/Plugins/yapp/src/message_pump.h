#ifndef _MESSAGE_PUMP_INC
#define _MESSAGE_PUMP_INC

/*
YAPP for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
            Copyright (C) 2005-2006 Scott Ellis

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

extern unsigned message_pump_thread_id;
void PostMPMessage(UINT msg, WPARAM, LPARAM);

#define MUM_CREATEPOPUP					(WM_USER + 0x011)
#define MUM_DELETEPOPUP					(WM_USER + 0x012)

#define MUM_NMUPDATE					(WM_USER + 0x013)
#define MUM_NMREMOVE					(WM_USER + 0x014)
#define MUM_NMAVATAR					(WM_USER + 0x015)

// given a popup data pointer, and a handle to an event, this function
// will post a message to the message queue which will set the hwnd value
// and then set the event...so create an event, call this function and then wait on the event
// when the event is signalled, the hwnd will be valid
void FindWindow(PopupData *pd, HANDLE hEvent, HWND *hwnd);

void InitMessagePump();
void DeinitMessagePump();

#endif
