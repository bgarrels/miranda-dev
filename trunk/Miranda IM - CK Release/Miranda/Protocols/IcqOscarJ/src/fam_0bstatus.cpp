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

* Handles packets from Service family
*/

#include "icqoscar.h"


void CIcqProto::handleStatusFam(unsigned char *pBuffer, WORD wBufferLength, snac_header* pSnacHeader)
{
	switch (pSnacHeader->wSubtype) {

	case ICQ_STATS_MINREPORTINTERVAL:
		{
			WORD wInterval;
			unpackWord(&pBuffer, &wInterval);
			NetLog_Server("Server sent SNAC(x0B,x02) - SRV_SET_MINREPORTINTERVAL (Value: %u hours)", wInterval);
		}
		break;

	case ICQ_ERROR:
		{
			WORD wError;

			if (wBufferLength >= 2)
				unpackWord(&pBuffer, &wError);
			else 
				wError = 0;

			LogFamilyError(ICQ_STATS_FAMILY, wError);
			break;
		}

	default:
		NetLog_Server("Warning: Ignoring SNAC(x%02x,x%02x) - Unknown SNAC (Flags: %u, Ref: %u)", ICQ_STATS_FAMILY, pSnacHeader->wSubtype, pSnacHeader->wFlags, pSnacHeader->dwRef);
		break;
	}
}
