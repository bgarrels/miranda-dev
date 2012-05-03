#ifndef _YAHOO_HTTP_GATEWAY_H_
#define _YAHOO_HTTP_GATEWAY_H_

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

#define HTTP_PROXY_VERSION  0x0443

int YAHOO_httpGatewayInit(HANDLE hConn, NETLIBOPENCONNECTION *nloc, NETLIBHTTPREQUEST *nlhr);
int YAHOO_httpGatewayWrapSend(HANDLE hConn, PBYTE buf, int len, int flags, MIRANDASERVICE pfnNetlibSend);
PBYTE YAHOO_httpGatewayUnwrapRecv(NETLIBHTTPREQUEST *nlhr, PBYTE buf, int bufLen, int *outBufLen, void *(*NetlibRealloc)(void *, size_t));

#endif 

