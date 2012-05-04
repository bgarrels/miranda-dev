/*
Nudge plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	francois.mean@skynet.be
							Sylvain.gougouzian@gmail.com
							wishmaster51@googlemail.com

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

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT              
#define _WIN32_WINNT 0x0501	
#endif						

#ifndef _WIN32_WINDOWS	
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE		
#define _WIN32_IE 0x0600
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


// Windows Header Files:
#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stddef.h>
#include <process.h>

#define MIRANDA_VER 0x0A00
#include <newpluginapi.h>	//CallService,UnHookEvent
#include <m_utils.h>		//window broadcasting
#include <m_clist.h>			
#include <m_langpack.h>	
#include <m_system.h>	
#include <m_popup.h>	
#include <m_clui.h>
#include <m_message.h>
#include <m_contacts.h>
#include <m_protocols.h>
#include <m_protomod.h>
#include <m_options.h>
#include <m_skin.h>
#include <m_database.h>
#include <m_protosvc.h>

#include "../resource.h"
#include "m_nudge.h"

// ExternalAPI
#include "m_trigger.h"
#include "m_metacontacts.h"
#include "m_updater.h"	
#include <m_icolib.h>	


void InitOptions();
void UninitOptions();

int Preview();

HANDLE Nudge_GethContact(HANDLE);