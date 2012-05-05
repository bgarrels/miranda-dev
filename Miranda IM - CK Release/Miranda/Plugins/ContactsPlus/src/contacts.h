/*
Contacts+ for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright © 2002 Dominus Procellarum 
			Copyright © 2004-2008 Joe Kucera

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

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS

#include <windows.h>
#include <commctrl.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <crtdbg.h>
#include <malloc.h>
#include <time.h>


#define MIRANDA_VER   0x0A00

#include "newpluginapi.h"
#include "m_system.h"
#include "m_protocols.h"
#include "m_protosvc.h"
#include "m_database.h"
#include "m_utils.h"
#include "m_langpack.h"
#include "m_skin.h"
#include "m_clist.h"
#include "m_clc.h"
#include "m_clui.h"
#include "m_addcontact.h"
#include "m_history.h"
#include "m_userinfo.h"
#include "m_button.h"
#include "m_contacts.h"
#include "m_message.h"
#include "statusmodes.h"
#include "win2k.h"

#include "resource.h"

#include "m_updater.h"

#include "utils.h"
#include "send.h"
#include "receive.h"

#define MODULENAME "SendReceiveContacts"


#define MS_CONTACTS_SEND "ContactsTransfer/SendContacts"
#define MS_CONTACTS_RECEIVE "ContactsTransfer/ReceiveContacts"

// Global Variables
extern int g_UnicodeCore;
extern int g_NewProtoAPI;
extern int g_SendAckSupported;
extern int g_Utf8EventsSupported;
