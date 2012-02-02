#ifndef _COMMON_INC
#define _COMMON_INC

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define _CRT_SECURE_NO_DEPRECATE

#if defined( UNICODE ) && !defined( _UNICODE )
#define _UNICODE
#endif

#define MIRANDA_VER    0x0900
#define MIRANDA_CUSTOM_LP

#include <tchar.h>

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>

#include <win2k.h>
#include <newpluginapi.h>
#include <m_system.h>
#include <m_database.h>
#include <m_langpack.h>
#include <m_options.h>
#include <m_hotkeys.h>
#include <m_utils.h>
#include <m_skin.h>
#include <m_genmenu.h>
#include <m_icolib.h>
#include <m_protocols.h>
#include "m_actman.h"
#include "m_msg_buttonsbar.h"
#include "m_toolbar.h"
#include "m_toptoolbar.h"
#include "m_variables.h"
#include "m_updater.h"

extern HINSTANCE hInst;
extern PLUGINLINK *pluginLink;

#ifndef MIID_USEACTIONS
#define MIID_USEACTIONS { 0xA63473F7, 0xF8BF, 0x46E7, { 0x9D, 0xF0, 0x4, 0x2D, 0xBE, 0x4C, 0xF5, 0x79 } }
#endif

#endif
