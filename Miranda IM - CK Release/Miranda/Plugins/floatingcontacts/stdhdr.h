
#ifndef __STDHDR_H__
#define __STDHDR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define MIRANDA_VER 0x0800
#define _WIN32_WINNT 0x0501


/////////////////////////////////////////////////////////////////////////////
//

// Suppress 'deprecated' warnings for nonsecure entities
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#define STRICT

#include <windows.h>
#include <stdio.h>
#include <assert.h>

#pragma warning ( disable : 4201 ) //nonstandard extension used : nameless struct/union
#include <commctrl.h>

#include <win2k.h>
#include <newpluginapi.h>
#include <m_system.h>
#include <m_database.h>
#include <m_langpack.h>
#include <m_button.h>
#include <m_clist.h>
#include <m_clc.h>
#include <m_clui.h>
#include <m_options.h>
#include <m_protosvc.h>
#include <m_utils.h>
#include <m_skin.h>
#include <m_contacts.h>
#include <m_userinfo.h>
#include <m_history.h>
#include <m_addcontact.h>
#include <m_message.h>
#include <m_file.h>
#include <m_icolib.h>
#include <m_fontservice.h>
#include <m_timezones.h>
#include <m_modernopt.h>

/////////////////////////////////////////////////////////////////////////////

#endif	// #ifndef __STDHDR_H__

/////////////////////////////////////////////////////////////////////////////
// End Of File stdhdr.h
