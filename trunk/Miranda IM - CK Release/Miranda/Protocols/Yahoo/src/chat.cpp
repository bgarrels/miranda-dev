/*
Yahoo protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Gennady Feldman (aka Gena01) 
			Laurent Marechal (aka Peorth)

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

#include "../yahoo.h"
#include <m_protosvc.h>
#include <m_langpack.h>
#include <m_message.h>

/* Conference handlers */
void ext_yahoo_got_conf_invite(int id, const char *me, const char *who, const char *room, const char *msg, YList *members)
{
	char z[1024];
	_snprintf(z, sizeof(z), Translate("[miranda] Got conference invite to room: %s with msg: %s"), room ?room:"", msg ?msg:"");
	LOG(("[ext_yahoo_got_conf_invite] %s", z));
	GETPROTOBYID( id )->ext_got_im("me", who, 0, z, 0, 0, 1, -1);
	
	yahoo_conference_decline(id, NULL, members, room, Translate("I am sorry, but i can't join your conference since this feature is not currently implemented in my client."));
}

void ext_yahoo_conf_userdecline(int id, const char *me, const char *who, const char *room, const char *msg)
{
}

void ext_yahoo_conf_userjoin(int id, const char *me, const char *who, const char *room)
{
}

void ext_yahoo_conf_userleave(int id, const char *me, const char *who, const char *room)
{
}

void ext_yahoo_conf_message(int id, const char *me, const char *who, const char *room, const char *msg, int utf8)
{
}

/* chat handlers */
void ext_yahoo_chat_cat_xml(int id, const char *xml) 
{
}

void ext_yahoo_chat_join(int id, const char *me, const char *room, const char * topic, YList *members, int fd)
{
}

void ext_yahoo_chat_userjoin(int id, const char *me, const char *room, struct yahoo_chat_member *who)
{
}

void ext_yahoo_chat_userleave(int id, const char *me, const char *room, const char *who)
{
}
void ext_yahoo_chat_message(int id, const char *me, const char *who, const char *room, const char *msg, int msgtype, int utf8)
{
}

void ext_yahoo_chat_yahoologout(int id, const char *me)
{ 
	LOG(("got chat logout"));
}
void ext_yahoo_chat_yahooerror(int id, const char *me)
{ 
	LOG(("got chat error"));
}
