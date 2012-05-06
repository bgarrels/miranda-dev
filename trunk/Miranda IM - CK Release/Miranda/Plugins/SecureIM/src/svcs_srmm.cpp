/*
SecureIM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 	2003 Johell
							2005-2009 Baloo

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

#include "commonheaders.h"


int __cdecl onWindowEvent(WPARAM wParam, LPARAM lParam) {

	MessageWindowEventData *mwd = (MessageWindowEventData *)lParam;
	if(mwd->uType == MSG_WINDOW_EVT_OPEN || mwd->uType == MSG_WINDOW_EVT_OPENING) {
		ShowStatusIcon(mwd->hContact);
	}
	return 0;
}


int __cdecl onIconPressed(WPARAM wParam, LPARAM lParam) {
	HANDLE hContact = (HANDLE)wParam;
	if( isProtoMetaContacts(hContact) )
		hContact = getMostOnline(hContact); // возьмем тот, через который пойдет сообщение

	StatusIconClickData *sicd = (StatusIconClickData *)lParam;
	if( strcmp(sicd->szModule, szModuleName) != 0 ||
		!isSecureProtocol(hContact) ) return 0; // not our event

	BOOL isPGP = isContactPGP(hContact);
	BOOL isGPG = isContactGPG(hContact);
	BOOL isSecured = isContactSecured(hContact)&SECURED;
	BOOL isChat = isChatRoom(hContact);

	if( !isPGP && !isGPG && !isChat ) {
		if(isSecured)	Service_DisableIM(wParam,0);
		else		Service_CreateIM(wParam,0);
	}

	return 0;
}


// EOF
