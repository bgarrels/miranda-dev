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
#include "ignore.h"

const YList* CYahooProto::GetIgnoreList(void)
{
	if (m_id < 1)
		return NULL;
	
	return yahoo_get_ignorelist(m_id);
}

void CYahooProto::IgnoreBuddy(const char *buddy, int ignore)
{
	if (m_id < 1)
		return;
	
	yahoo_ignore_buddy(m_id, buddy, ignore);
	//yahoo_get_list(m_id);
}


int CYahooProto::BuddyIgnored(const char *who)
{
	const YList *l = GetIgnoreList();
	while (l != NULL) {
		struct yahoo_buddy *b = (struct yahoo_buddy *) l->data;
			
		if (lstrcmpiA(b->id, who) == 0) {
			//LOG(("User '%s' on our Ignore List. Dropping Message.", who));
			return 1;
		}
		l = l->next;
	}

	return 0;
}

void CYahooProto::ext_got_ignore(YList * igns)
{	
	YList *l = igns;
	
	LOG(("[ext_yahoo_got_ignore] Got Ignore List")); 
	
	while (l != NULL) {
		struct yahoo_buddy *b = (struct yahoo_buddy *) l->data;

		YAHOO_DEBUGLOG("[ext_yahoo_got_ignore] Buddy: %s", b->id );
		
		l = l->next;
	}
	
	YAHOO_DEBUGLOG("[ext_yahoo_got_ignore] End Of Ignore List"); 
}

