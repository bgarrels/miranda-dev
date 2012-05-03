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

#ifdef HTTP_GATEWAY

int YAHOO_httpGatewayInit(HANDLE hConn, NETLIBOPENCONNECTION *nloc, NETLIBHTTPREQUEST *nlhr)
{
	NETLIBHTTPPROXYINFO nlhpi;

	DebugLog("YAHOO_httpGatewayInit!!!");
	
	ZeroMemory(&nlhpi, sizeof(nlhpi) );
	nlhpi.cbSize = sizeof(nlhpi);
	nlhpi.szHttpPostUrl = "http://shttp.msg.yahoo.com/notify/";
	
	//CallService( MS_NETLIB_SETPOLLINGTIMEOUT, (WPARAM) hConn, 15 );
	
	return CallService(MS_NETLIB_SETHTTPPROXYINFO, (WPARAM)hConn, (LPARAM)&nlhpi);
}

int YAHOO_httpGatewayWrapSend(HANDLE hConn, PBYTE buf, int len, int flags, MIRANDASERVICE pfnNetlibSend)
{
	DebugLog("YAHOO_httpGatewayWrapSend!!! Len: %d", len);

	if (len == 0 && m_id > 0) { // we need to send something!!!
		int n;
		char *z = yahoo_webmessenger_idle_packet(m_id, &n);
		int ret = 0;
		
		if (z != NULL) {
			DebugLog("YAHOO_httpGatewayWrapSend!!! Got Len: %d", n);
			NETLIBBUFFER tBuf = { ( char* )z, n, flags };
			ret = pfnNetlibSend(( LPARAM )hConn, (WPARAM) &tBuf );
			FREE(z);
		} else {
			DebugLog("YAHOO_httpGatewayWrapSend!!! GOT NULL???");
		}
		
		return ret;
	} else {
		NETLIBBUFFER tBuf = { ( char* )buf, len, flags };
		
		return pfnNetlibSend(( LPARAM )hConn, (WPARAM) &tBuf );
	}
}

PBYTE YAHOO_httpGatewayUnwrapRecv(NETLIBHTTPREQUEST *nlhr, PBYTE buf, int len, int *outBufLen, void *(*NetlibRealloc)(void *, size_t))
{
	DebugLog("YAHOO_httpGatewayUnwrapRecv!!! Len: %d", len);

	DebugLog("Got headers: %d", nlhr->headersCount);
	/* we need to get the first 4 bytes! */
	if (len < 4) 
		return NULL;

	ylad->rpkts = buf[0] + buf[1] *256;
	DebugLog("Got packets: %d", ylad->rpkts);
	
	if (len == 4){
		*outBufLen = 0;
		return buf;
	} else  if ( (buf[4] == 'Y') && (buf[5] == 'M') && (buf[6] == 'S') && (buf[7] == 'G') ) {
		MoveMemory( buf, buf + 4, len - 4);
		*outBufLen = len-4;// we take off 4 bytes from the beginning
		 
		return buf;                 
	} else
		return NULL; /* Break connection, something went wrong! */
	 
}

#endif
