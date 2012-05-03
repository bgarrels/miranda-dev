/*
Omegle protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright © 2011-2012 Robert Pösel

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

#define HTTP_CODE_OK					200
#define HTTP_CODE_MOVED_PERMANENTLY		301
#define HTTP_CODE_FOUND					302
#define HTTP_CODE_FORBIDDEN				403
#define HTTP_CODE_NOT_FOUND				404
#define HTTP_CODE_REQUEST_TIMEOUT		408
#define HTTP_CODE_INTERNAL_SERVER_ERROR	500
#define HTTP_CODE_NOT_IMPLEMENTED		501
#define HTTP_CODE_BAD_GATEWAY			502
#define HTTP_CODE_SERVICE_UNAVAILABLE   503

#define HTTP_CODE_FAKE_DISCONNECTED 0
#define HTTP_CODE_FAKE_ERROR        1

namespace http
{
	struct response
	{
		response() : code(0) {}
		int code;
		std::map< std::string, std::string > headers;
		std::string data;
	};
}
