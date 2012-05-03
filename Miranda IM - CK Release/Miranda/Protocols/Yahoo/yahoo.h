#ifndef _YAHOO_YAHOO_H_
#define _YAHOO_YAHOO_H_

/*
Yahoo protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Gennady Feldman (aka Gena01) 
			Laurent Marechal (aka Peorth)

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

#define MIRANDA_VER 0x0A00

extern "C"
{
	extern struct MD5_INTERFACE  md5i;
	extern struct tagPLUGINLINK* pluginLink;
};

#ifdef _MSC_VER
	#define snprintf  _snprintf
#endif

#include <m_stdhdr.h>

#include <sys/stat.h>
#include <io.h>
#include <time.h>

#include <windows.h>

/* 
 * Yahoo Services
 */
#define USE_STRUCT_CALLBACKS

#include "libyahoo2/yahoo2.h"
#include "libyahoo2/yahoo2_callbacks.h"
#include "libyahoo2/yahoo_util.h"

extern "C"
{
	#include <newpluginapi.h>
	#include <m_system.h>
};

#include <m_system_cpp.h>
#include <m_database.h>
#include <m_protomod.h>
#include <m_netlib.h>
#include <m_clist.h>
#include <m_clistint.h>
#include <m_protosvc.h>
#include <m_protoint.h>
#include <m_langpack.h>
#include <win2k.h>

//=======================================================
//	Definitions
//=======================================================
// Build is a cvs build
//
// If defined, the build will add cvs info to the plugin info
#define YAHOO_CVSBUILD

#define YAHOO_LOGINSERVER					"LoginServer"
#define YAHOO_LOGINPORT						"LoginPort"
#define YAHOO_LOGINID						"yahoo_id"
#define YAHOO_PASSWORD						"Password"
#define YAHOO_PWTOKEN						"PWToken"
#define YAHOO_CHECKMAIL						"CheckMail"
#define YAHOO_CUSTSTATDB					"CustomStat"
#define YAHOO_DEFAULT_PORT					5050
#define YAHOO_DEFAULT_LOGIN_SERVER			"mcs.msg.yahoo.com"	
#define YAHOO_DEFAULT_JAPAN_LOGIN_SERVER	"cs.yahoo.co.jp"	
#define YAHOO_CUSTOM_STATUS					99

#define YAHOO_DEBUGLOG DebugLog

extern int do_yahoo_debug;

#define LOG(x) if(do_yahoo_debug) { YAHOO_DEBUGLOG("%s:%d: ", __FILE__, __LINE__); \
	YAHOO_DEBUGLOG x; \
	YAHOO_DEBUGLOG(" ");}

#define YAHOO_SET_CUST_STAT			"/SetCustomStatCommand" 
#define YAHOO_EDIT_MY_PROFILE		"/YahooEditMyProfileCommand"
#define YAHOO_SHOW_PROFILE			"/YahooShowProfileCommand"
#define YAHOO_SHOW_MY_PROFILE		"/YahooShowMyProfileCommand"
#define YAHOO_YAHOO_MAIL			"/YahooGotoMailboxCommand"
#define YAHOO_REFRESH				"/YahooRefreshCommand"
#define YAHOO_AB					"/YahooAddressBook"
#define YAHOO_CALENDAR				"/YahooCalendar"
#define YAHOO_SEND_NUDGE			"/SendNudge"
#define YAHOO_GETUNREAD_EMAILCOUNT	"/GetUnreadEmailCount"

#define STYLE_DEFAULTBGCOLOUR     RGB(173,206,247)

#define LocalEventUnhook(hook)	if(hook) UnhookEvent(hook)
#define NEWSTR_ALLOCA(A) (A==NULL)?NULL:strcpy((char*)alloca(strlen(A)+1),A)

struct _conn {
	unsigned int tag;
	int id;
	int fd;
	yahoo_input_condition cond;
	void *data;
	int remove;
};

#include "proto.h"

//=======================================================
//	Defines
//=======================================================
extern HINSTANCE		hInstance;

#ifdef HTTP_GATEWAY
extern int 				iHTTPGateway;
#endif

INT_PTR __stdcall YAHOO_CallService( const char* szSvcName, WPARAM wParam, LPARAM lParam );

#define YAHOO_hasnotification() ServiceExists(MS_CLIST_SYSTRAY_NOTIFY)

int YAHOO_util_dbsettingchanged(WPARAM wParam, LPARAM lParam);

//Services.c
int SetStatus(WPARAM wParam,LPARAM lParam);
int GetStatus(WPARAM wParam,LPARAM lParam);

yahoo_status miranda_to_yahoo(int myyahooStatus);

void register_callbacks();
char* YAHOO_GetContactName(HANDLE hContact);

#ifdef __GNUC__
	int DebugLog( const char *fmt, ... ) __attribute__ ((format (printf, 1, 2)));
#else
	int DebugLog( const char *fmt, ... );
#endif

void SetButtonCheck(HWND hwndDlg, int CtrlID, BOOL bCheck);

char * yahoo_status_code(enum yahoo_status s);
void yahoo_callback(struct _conn *c, yahoo_input_condition cond);

CYahooProto* __fastcall getProtoById( int id );
#define GETPROTOBYID(A) CYahooProto* ppro = getProtoById(A); if ( ppro ) ppro

#endif
