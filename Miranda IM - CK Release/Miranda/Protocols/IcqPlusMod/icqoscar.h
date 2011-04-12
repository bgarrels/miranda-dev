// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin Öberg, Sam Kothari, Robert Rainwater
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
// Revision       : $Revision: 51 $
// Last change on : $Date: 2007-08-30 23:46:51 +0300 (Ð§Ñ‚, 30 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
// Includes all header files that should be precompiled to speed up compilation.
//
// -----------------------------------------------------------------------------

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
//disable the deprecated warnings for the crt functions.

//#define _USE_32BIT_TIME_T
#if defined(_MSC_VER)
#pragma warning(disable: 4996)
#endif

#define MIRANDA_VER 0x0700
#define ICQ_MODNAME "Plus"
#define _WIN32_IE 0x0500


#include "m_stdhdr.h"

// Windows includes
#include <windows.h>
#include <commctrl.h>
//#include <shlwapi.h>

// Standard includes
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <direct.h>
#include <fcntl.h>
#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include <process.h>
#include <malloc.h>

#include <list>
#include <string>

#include "changeinfo/changeinfo.h"
// Miranda IM SDK includes
#include <newpluginapi.h> // This must be included first
#include <m_awaymsg.h>
#include <m_button.h>
#include <m_clc.h>
#include <m_clist.h>
#include <m_clui.h>
#include <m_database.h>
#include <m_langpack.h>
#include <m_message.h>
#include <m_netlib.h>
#include <m_protocols.h>
#include <m_protomod.h>
#include <m_protosvc.h>
#include <m_options.h>
#include <m_system.h>
#include <m_userinfo.h>
#include <m_utils.h>
#include <m_idle.h>
#include <m_popup.h>
#include <m_clistint.h>
#include <m_ignore.h>
#include <m_avatars.h>
#include <m_skin.h>
#include <m_system.h>
#include <win2k.h>
//other plugins api
#include "m_fingerprint.h"
#include "m_assocmgr.h"
#include "m_extraicons.h"

// Project resources
#include "resource.h"

// ICQ plugin includes
#include "isee.h"
#include "globals.h"
#include "icq_db.h"
#include "i18n.h"
#include "cookies.h"
#include "icq_packet.h"
#include "icq_direct.h"
#include "oscar_filetransfer.h"
#include "icq_server.h"
#include "icqosc_svcs.h"
#include "icq_xtraz.h"
#include "icq_opts.h"
#include "icq_servlist.h"
#include "icq_http.h"
#include "icq_fieldnames.h"
#include "icq_constants.h"
#include "icq_infoupdate.h"
#include "icq_avatar.h"
#include "guids.h"
#include "capabilities.h"
#include "init.h"
#include "stdpackets.h"
#include "directpackets.h"
#include "tlv.h"
#include "families.h"
#include "utilities.h"
#include "m_icq.h"
#include "m_icqplus.h"
#include "icq_advsearch.h"
#include "icq_uploadui.h"
#include "log.h"
#include "channels.h"
#include "icq_reguin.h"
#include "UI/askauthentication.h"
#include "UI/userinfotab.h"
#include "UI/loginpassword.h"

#include "icq_rates.h"

#include "icq_popups.h"
#include "iconlib.h"

#include "icq_tzers.h"
#include "icq_qipstatus.h"

#if defined(_MSC_VER) && _MSC_VER < 1300

#define TTS_BALLOON             0x40
#define TTM_SETTITLE            (WM_USER + 32)

#endif

extern struct UTF8_INTERFACE utfi;

#ifndef CDSIZEOF_STRUCT
#define CDSIZEOF_STRUCT(structname, member)  (((int)((LPBYTE)(&((structname*)0)->member) - ((LPBYTE)((structname*)0)))) + sizeof(((structname*)0)->member))
#endif

// :TODO: This should not be here :p
void icq_FirstRunCheck(void);


__inline static int Netlib_MyCloseHandle(HANDLE h)
{
    FreeGatewayIndex(h);
    return CallService(MS_NETLIB_CLOSEHANDLE,(WPARAM)h,0);
}

//////////////////////////////////////////////////////////////////////////
#include "IcqCore.h"

extern std::list<CIcqProto*> g_Instances;
extern CIcqProto gProtocol;
