#ifndef __GUIDS_H
#define __GUIDS_H

/*
ICQ plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright © 2001-2004 Richard Hughes
			Copyright © 2002-2004 Martin Öberg, Sam Kothari, Robert Rainwater
			Copyright © 2004-2010 Joe Kucera, Bio
			Copyright © 2010-2012 Borkra, Georg Hazan

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

* Contains helper functions to handle oscar message GUIDs.
*/

typedef DWORD plugin_guid[4];

// Message Capability GUIDs
static const plugin_guid MCAP_SRV_RELAY_FMT    = {MCAP_SRV_RELAY_FMT_s};
static const plugin_guid MCAP_REVERSE_DC_REQ   = {MCAP_REVERSE_DC_REQ_s};
static const plugin_guid MCAP_FILE_TRANSFER    = {MCAP_FILE_TRANSFER_s};
static const plugin_guid MCAP_CONTACTS         = {MCAP_CONTACTS_s};

// Plugin GUIDs
static const plugin_guid PSIG_MESSAGE          = {PSIG_MESSAGE_s};
static const plugin_guid PSIG_INFO_PLUGIN      = {PSIG_INFO_PLUGIN_s};
static const plugin_guid PSIG_STATUS_PLUGIN    = {PSIG_STATUS_PLUGIN_s};

// Plugin Message GUIDs
static const plugin_guid PMSG_QUERY_INFO       = {PMSG_QUERY_INFO_s};
static const plugin_guid PMSG_QUERY_STATUS     = {PMSG_QUERY_STATUS_s};

// Message GUIDs
static const plugin_guid MGTYPE_MESSAGE        = {MGTYPE_MESSAGE_s};
static const plugin_guid MGTYPE_STATUSMSGEXT   = {MGTYPE_STATUSMSGEXT_s};
static const plugin_guid MGTYPE_FILE           = {MGTYPE_FILE_s};
static const plugin_guid MGTYPE_WEBURL         = {MGTYPE_WEBURL_s};
static const plugin_guid MGTYPE_CONTACTS       = {MGTYPE_CONTACTS_s};
static const plugin_guid MGTYPE_GREETING_CARD  = {MGTYPE_GREETING_CARD_s};
static const plugin_guid MGTYPE_CHAT           = {MGTYPE_CHAT_s};
static const plugin_guid MGTYPE_SMS_MESSAGE    = {MGTYPE_SMS_MESSAGE_s};
static const plugin_guid MGTYPE_XTRAZ_SCRIPT   = {MGTYPE_XTRAZ_SCRIPT_s};


// make GUID checks easy
static BOOL CompareGUIDs(DWORD q1,DWORD q2,DWORD q3,DWORD q4, const plugin_guid guid)
{
  return ((q1 == guid[0]) && (q2 == guid[1]) && (q3 == guid[2]) && (q4 == guid[3]))?TRUE:FALSE;
}


// pack entire GUID into icq packet
static __inline void packGUID(icq_packet *packet, const plugin_guid guid)
{
  packDWord(packet, guid[0]);
  packDWord(packet, guid[1]);
  packDWord(packet, guid[2]);
  packDWord(packet, guid[3]);
}


#endif /* __GUIDS_H */
