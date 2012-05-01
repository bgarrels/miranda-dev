#ifndef __ICQOSC_SVCS_H
#define __ICQOSC_SVCS_H

/*
ICQ plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright � 2001-2004 Richard Hughes
			Copyright � 2002-2004 Martin �berg, Sam Kothari, Robert Rainwater
			Copyright � 2004-2010 Joe Kucera, Bio
			Copyright � 2010-2012 Borkra, Georg Hazan

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

* Eventmessage

* Most of the protocol constants follow the naming conventions of the
* Oscar documentation at http://iserverd.khstu.ru/oscar/index.html
* BIG THANKS to Alexandr for maintaining this site and to everyone
* in the ICQ devel community who have helped to collect the data.
*/

#define ICQ_DB_GETEVENTTEXT_MISSEDMESSAGE "ICQ/GetEventTextMissedMessage"

INT_PTR icq_getEventTextMissedMessage(WPARAM wParam, LPARAM lParam);


#endif /* __ICQOSC_SVCS_H */
