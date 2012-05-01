/*
GaduGadu plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			(C) 2001-2002 Wojtek Kaniewski <wojtekka@irc.pl>
			(C)	Dawid Jarosz <dawjar@poczta.onet.pl>
			(C) 2003-2009 Adam Strzelecki <ono+miranda@java.pl>
			(C) 2009-2012 Bartosz Bia³ek

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

#ifdef _WIN32
#include "win32.h"

int sockpipe(SOCKET filedes[2])
{
	SOCKET sock;
	struct sockaddr_in sin;
	unsigned int len = sizeof(sin);

	filedes[0] = filedes[1] = INVALID_SOCKET;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		return -1;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(0);
	sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if (bind(sock, (SOCKADDR *)&sin, len) == SOCKET_ERROR ||
		listen(sock, 1) == SOCKET_ERROR ||
		getsockname(sock, (SOCKADDR *)&sin, &len) == SOCKET_ERROR) {
		closesocket(sock);
		return -1;
	}

	if ((filedes[1] = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET ||
		connect(filedes[1], (SOCKADDR *)&sin, len) == SOCKET_ERROR) {
		closesocket(sock);
		return -1;
	}

	if ((filedes[0] = accept(sock, (SOCKADDR *)&sin, &len)) == INVALID_SOCKET) {
		closesocket(filedes[1]);
		filedes[1] = INVALID_SOCKET;
		closesocket(sock);
		return -1;
	}

	closesocket(sock);
	return 0;
}

#endif /* _WIN32 */
