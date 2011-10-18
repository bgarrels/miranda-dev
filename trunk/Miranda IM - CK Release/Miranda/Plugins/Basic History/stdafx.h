// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#ifndef _WIN64
#define _USE_32BIT_TIME_T
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <richedit.h>
#include <time.h>
#include <commctrl.h>
#include <commdlg.h>
#include <tom.h>
#include <richole.h>
#include <Shlobj.h>

#include <map>
#include <vector>
#include <queue>
#include <locale>

#define MIRANDA_VER		0x0900
#define MIRANDA_CUSTOM_LP

#include <newpluginapi.h>
#include <m_langpack.h>
#include <m_system.h>
#include <m_clist.h>
#include <m_skin.h>
#include <m_history.h>
#include <m_database.h>
#include <m_icolib.h>
#include <m_timezones.h>
#include <m_contacts.h>
#include <m_protocols.h>
#include <m_button.h>
#include <m_options.h>
#include <m_fontservice.h>
#include <m_hotkeys.h>
#include <m_message.h>
#include <m_protosvc.h>
#include <m_icq.h>

#include <win2k.h>

#include "sdk/m_updater.h"
#include "sdk/m_smileyadd.h"
#include "sdk/m_toolbar.h"

#define HISTORY_HK_FIND 100
#define HISTORY_HK_FINDNEXT 101
#define HISTORY_HK_FINDPREV 102
#define HISTORY_HK_MATCHCASE 103
#define HISTORY_HK_MATCHWHOLE 104
#define HISTORY_HK_SHOWCONTACTS 105
#define HISTORY_HK_ONLYIN 106
#define HISTORY_HK_ONLYOUT 107
#define HISTORY_HK_DELETE 108
#define HISTORY_HK_ONLYGROUP 109

#define EVENTTYPE_STATUSCHANGE          25368
#define EVENTTYPE_SMTPSIMPLE            2350