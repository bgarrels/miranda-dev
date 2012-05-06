#ifndef __STDHDR_H__
#define __STDHDR_H__

/*
FltContacts plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Authors
Copyright (C) Ranger (fonts and colors settings)
Copyright (C) Iavor Vajarov ( ivajarov@code.bg )


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

* This file is a modification of clcfiledrop.h originally  written by Richard Hughes.
*/

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define MIRANDA_VER    0x0A00
#define _WIN32_WINNT 0x0600

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


#endif	// #ifndef __STDHDR_H__
		// End Of File stdhdr.h