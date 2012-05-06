#ifndef __COMMONS_H__
#define __COMMONS_H__

/*
MyDetails plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2005 Ricardo Pescuma Domenecci

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

#if defined(__DEBUG__) || defined(_DEBUG) || defined(DEBUG)
#define DEBUGMODE // Debug Mode
#endif

#define _CRT_SECURE_NO_DEPRECATE

#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <win2k.h>
#include <commctrl.h>
#include <stdio.h>
#include <vector>

//Miranda IM stuff
#define MIRANDA_VER     0x0A00
#include <newpluginapi.h>
#include <m_clist.h>
#include <m_skin.h>
#include <m_system.h>
#include <m_protocols.h>
#include <m_protosvc.h>
#include <m_database.h>
#include <m_utils.h>
#include <m_langpack.h>
#include <m_awaymsg.h>
#include <m_contacts.h>
#include <m_options.h>
#include <m_clui.h>
#include <m_clc.h>
#include <m_proto_listeningto.h>
#include <m_listeningto.h>

//ExternalAPI
#include <m_NewAwaySys.h>
#include <m_updater.h>
#include <m_fontservice.h>
#include <m_variables.h>
#include <m_avatars.h>
#include <m_statusplugins.h>
#include <m_ersatz.h>
#include <m_icq.h>
#include <m_icolib.h>
#include "m_cluiframes.h"
#include "m_simpleaway.h"

#include <richedit.h>
#include <m_smileyadd.h>

#include <io.h>

#include "m_skins_cpp.h"

#include "../resource.h"


#define MODULE_NAME "MyDetails"

#define SETTING_FRAME_VISIBLE "FrameVisible"
#define SETTING_DEFAULT_NICK "DefaultNick"


extern HINSTANCE hInst;
extern PLUGINLINK *pluginLink;

extern long nickname_dialog_open;
extern long status_msg_dialog_open;

extern SkinDialog *dialog;


#include "m_mydetails.h"
#include "data.h"
#include "options.h"
#include "frame.h"

//Pescuma stuff
#include "mir_smileys.h"
#include "mir_memory.h"
#include "mir_options.h"
#include "mir_icons.h"
#include "tstring.h"


#define PS_SETMYAVATAR "/SetMyAvatar"
#define PS_GETMYAVATAR "/GetMyAvatar"
#define PS_GETMYAVATARMAXSIZE "/GetMyAvatarMaxSize"
#define PS_GETUNREADEMAILCOUNT "/GetUnreadEmailCount"

#define PS_SETMYNICKNAME "/SetNickname"

#define PS_GETMYNICKNAMEMAXLENGTH "/GetMyNicknameMaxLength"

#define WAYD_UNICODE 1        // return Unicode texts
#if defined( _UNICODE )
	#define WAYD_TCHAR WAYD_UNICODE
#else
	#define WAYD_TCHAR 0
#endif

// Get the max length that a WAYD message can have
// wParam=(WPARAM)0
// lParam=(LPARAM)0
// Returns the max length
#define PS_GET_MY_WAYD_MAXLENGTH "/GetMyWAYDMaxLength"

// Get the WAYD message for the user
// wParam=(WPARAM)WAYD_xxx
// lParam=(LPARAM)0
// Returns the text or NULL if there is none. Remember to mir_free the return value.
#define PS_GET_MY_WAYD "/GetMyWAYD"

// Sets the WAYD message for the user
// wParam=(WPARAM)WAYD_xxx
// lParam=(LPARAM)(WCHAR * or char *)The text to set
// Returns 0 on success, nonzero on failure
#define PS_SET_MY_WAYD "/SetMyWAYD"

#define MAX_REGS(_A_) ( sizeof(_A_) / sizeof(_A_[0]) )


// See if a protocol service exists
__inline static int ProtoServiceExists(const char *szModule,const char *szService)
{
	char str[MAXMODULELABELLENGTH];
	strcpy(str,szModule);
	strcat(str,szService);
	return ServiceExists(str);
}



#endif // __COMMONS_H__
