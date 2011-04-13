// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin ÷berg, Sam Kothari, Robert Rainwater
// Copyright © 2004,2005,2006,2007 Joe Kucera
// Copyright © 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// -----------------------------------------------------------------------------
//
// File name      : $Source$
// Revision       : $Revision: 48 $
// Last change on : $Date: 2007-08-26 16:12:47 +0300 (–í—Å, 26 –∞–≤–≥ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Contains global variables declarations.
//
// -----------------------------------------------------------------------------

#ifndef __GLOBALS_H
#define __GLOBALS_H

typedef char uid_str[MAX_PATH];

// Defaults
#define DEFAULT_QUICK_LOGIN         1
#define DEFAULT_DCTYPE              2 //auth req
#define DEFAULT_VISIBILITY          0
#define DEFAULT_OEM_CONV            0
#define DEFAULT_RENAME_SERVER_NICKS 1
#define DEFAULT_UPLOAD_DELAY        2
#define DEFAULT_PRIVACY_ENABLED     1
#define DEFAULT_SHOW_IDLE_TIME      1
#define DEFAULT_EXTSEARCH_ENABLED   1
#define DEFAULT_TOOLS               0
#define DEFAULT_VISIBILITY_TOOLS    0
#define DEFAULT_EXT_STATUS          0
#define DEFAULT_VISIBLEMODE         4
#define DEFAULT_INVISIBLEMODE       3

// from init.c
extern HINSTANCE hInst;
//extern IcqIconHandle hStaticIcons[];

//extern char gpszICQProtoName[MAX_PATH];
//extern char* ICQ_PROTOCOL_NAME;

extern HANDLE ghServerNetlibUser;
extern HANDLE ghDirectNetlibUser;

// from init.h
extern BYTE gbGatewayMode;
extern BYTE gbSecureLogin;
extern BYTE m_bAimEnabled;
extern BYTE m_bUtfEnabled;
extern WORD m_wAnsiCodepage;
extern BYTE m_bDCMsgEnabled;
extern BYTE m_bTempVisListEnabled;
extern BYTE m_bSsiEnabled;
extern BYTE m_bAvatarsEnabled;
extern BYTE gbQipStatusEnabled;
extern BYTE gbXStatusEnabled;
extern BYTE gbTzerEnabled;
extern DWORD MIRANDA_VERSION;

// from icqosc_svcs.c
extern int gnCurrentStatus;
extern DWORD dwLocalUIN;

extern char gpszPassword[16];
extern BYTE gbRememberPwd;

extern BYTE gbUnicodeAPI_dep;
extern BYTE gbUnicodeCore_dep;

// Globals
extern BYTE gbShowIdle;
extern BYTE gbVisibility;
extern BYTE gbMtnEnabled;
extern BYTE gbAdvSearch;
extern BYTE gbWebAware;
extern BYTE gbExtStatus;
extern BYTE gbRcvUnicode;
extern BYTE gbSendUnicode;
extern BYTE gbUnicodeDecode;
extern BYTE gbVisibleMode;
extern BYTE gwVersion;
extern BYTE gbQuickLogin;
extern BYTE gbSavePass;
extern BYTE gbUseServerNicks;
extern BYTE gbRenameServerNicks;
extern BYTE gbServerAddRemove;
extern BYTE gbSlowSend;
extern BYTE gbSetStatus;
extern BYTE gbCustomCapEnabled;
extern BYTE gbHideIdEnabled;
extern BYTE gbRTFEnabled;
extern BYTE gbVerEnabled;
extern BYTE gbScan;


extern DWORD gdwFP1;
extern DWORD gdwFP2;
extern DWORD gdwFP3;

extern DWORD gdwUpdateThreshold;

// from fam_04message.c
typedef struct icq_mode_messages_s
{
    char* szOffline;
    char* szOnline;
    char* szAway;
    char* szNa;
    char* szDnd;
    char* szOccupied;
    char* szFfc;
} icq_mode_messages;

extern struct LIST_INTERFACE li;
extern SortedList *lstCustomCaps;

extern const int moodXStatus[];


#endif /* __GLOBALS_H */
