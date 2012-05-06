/*
Google Extension plugin for 
Miranda IM: the free IM client for Microsoft* Windows*
and the Jabber plugin.

Authors
			Copyright (C) 2011-2012	bems@jabber.org
									George Hazan (ghazan@jabber.ru)

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
#include "resources.h"

static const LPSTR LAST_MAIL_TIME_FROM_JID = SHORT_PLUGIN_NAME ".LastMailTimeFromJid";
static const LPSTR LAST_THREAD_ID_FROM_JID = SHORT_PLUGIN_NAME ".LastThreadIdFromJid";

LPTSTR ReadJidSetting(LPCSTR name, LPCTSTR jid);
void WriteJidSetting(LPCSTR name, LPCTSTR jid, LPCTSTR setting);
char *WtoA(LPCTSTR W);
void RenewPseudocontactHandles();
