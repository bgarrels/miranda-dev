/*
Twitter protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Auhor		Copyright � 2009-2012 Jim Porter

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

#pragma once

#define MIRANDA_VER 0x0A00
#define MIRANDA_CUSTOM_LP

#include <string>
using std::string;
using std::wstring;
#include <map>
using std::map;
#include <vector>
using std::vector;
#include <list>
using std::list;
#include <algorithm>
using std::min;

#include <windows.h>

#include "resource.h"

#pragma warning(push)
#	pragma warning(disable:4312)
#include <newpluginapi.h>
#include <m_avatars.h>
#include <m_button.h>
#include <m_chat.h>
#include <m_clc.h>
#include <m_clist.h>
#include <m_clistint.h>
#include <m_clui.h>
//#include "m_cluiframes.h"
#include <m_database.h>
#include <m_history.h>
#include <m_idle.h>
#include <m_langpack.h>
#include <m_message.h>
#include <m_netlib.h>
#include <m_options.h>
#include <m_popup.h>
#include <m_protocols.h>
#include <m_protomod.h>
#include <m_protosvc.h>
#include <m_skin.h>
#include <statusmodes.h>
#include <m_system.h>
#include <m_userinfo.h>
#include <m_addcontact.h>
#include <m_icolib.h>
#include <m_utils.h>
#include <m_system_cpp.h>
#include <m_hotkeys.h>
#include <win2k.h>
#pragma warning(pop)

extern HINSTANCE g_hInstance;

#define TWITTER_KEY_NICK		   "Nick"  // we need one called Nick for the chat thingo to work
#define TWITTER_KEY_UN             "Username"
#define TWITTER_KEY_PASS           "Password"
#define TWITTER_KEY_OAUTH_PIN	   "OAuthPIN"
#define TWITTER_KEY_OAUTH_TOK	   "OAuthToken"
#define TWITTER_KEY_OAUTH_TOK_SECRET "OAuthTokenSecret"
#define TWITTER_KEY_OAUTH_ACCESS_TOK "OAuthAccessToken"
#define TWITTER_KEY_OAUTH_ACCESS_TOK_SECRET "OAuthAccessTokenSecret"
#define TWITTER_KEY_BASEURL        "BaseURL"
#define TWITTER_KEY_CHATFEED       "ChatFeed"
#define TWITTER_KEY_POLLRATE       "PollRate"
#define TWITTER_KEY_GROUP		   "DefaultGroup"

#define TWITTER_KEY_POPUP_SHOW     "Popup/Show"
#define TWITTER_KEY_POPUP_SIGNON   "Popup/Signon"
#define TWITTER_KEY_POPUP_COLBACK  "Popup/ColorBack"
#define TWITTER_KEY_POPUP_COLTEXT  "Popup/ColorText"
#define TWITTER_KEY_POPUP_TIMEOUT  "Popup/Timeout"

#define TWITTER_KEY_TWEET_TO_MSG   "TweetToMsg"

#define TWITTER_KEY_SINCEID        "SinceID"
#define TWITTER_KEY_DMSINCEID      "DMSinceID"
#define TWITTER_KEY_NEW            "NewAcc"

#define TWITTER_KEY_AV_URL         "AvatarURL"

#define TWITTER_DB_EVENT_TYPE_TWEET 2718

#define WM_SETREPLY   WM_APP+10
