/*
Twitter protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Auhor		Copyright © 2009-2012 Jim Porter

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

#include "http.h"

#include <windows.h>
#include <newpluginapi.h>
#include <m_netlib.h>

std::string http::url_encode(const std::string &s)
{
	char *encoded = reinterpret_cast<char*>(CallService( MS_NETLIB_URLENCODE,
		0,reinterpret_cast<LPARAM>(s.c_str()) ));
	std::string ret = encoded;
	HeapFree(GetProcessHeap(),0,encoded);

	return ret;
}
