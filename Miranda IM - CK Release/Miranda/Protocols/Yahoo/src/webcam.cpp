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

#include <time.h>
#include <sys/stat.h>
#include <malloc.h>
#include <io.h>

/*
 * Miranda IM headers
 */
#include "../yahoo.h"
#include <m_protosvc.h>
#include <m_langpack.h>
#include <m_message.h>

/* WEBCAM callbacks */
void ext_yahoo_got_webcam_image(int id, const char *who,
		const unsigned char *image, unsigned int image_size, unsigned int real_size,
		unsigned int timestamp)
{
	LOG(("ext_yahoo_got_webcam_image"));
}

void ext_yahoo_webcam_viewer(int id, const char *who, int connect)
{
	LOG(("ext_yahoo_webcam_viewer"));
}

void ext_yahoo_webcam_closed(int id, const char *who, int reason)
{
	LOG(("ext_yahoo_webcam_closed"));
}

void ext_yahoo_webcam_data_request(int id, int send)
{
	LOG(("ext_yahoo_webcam_data_request"));
}

void ext_yahoo_webcam_invite(int id, const char *me, const char *from)
{
	LOG(("ext_yahoo_webcam_invite"));

	GETPROTOBYID( id )->ext_got_im(me, from, 0, Translate("[miranda] Got webcam invite. (not currently supported)"), 0, 0, 0, -1);
}

void ext_yahoo_webcam_invite_reply(int id, const char *me, const char *from, int accept)
{
	LOG(("ext_yahoo_webcam_invite_reply"));
}
