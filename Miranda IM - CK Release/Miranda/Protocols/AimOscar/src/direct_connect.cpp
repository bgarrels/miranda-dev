/*
AIM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			(C) 2008-2012 Boris Krasnovskiy
			(C) 2005-2006 Aaron Myles Landwehr

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

#include "../aim.h"

void __cdecl CAimProto::aim_dc_helper(void* param) //only called when we are initiating a direct connection with someone else
{
	file_transfer *ft = (file_transfer*)param;	

	sendBroadcast(ft->hContact, ACKTYPE_FILE, ACKRESULT_CONNECTED, ft, 0);

	NETLIBPACKETRECVER packetRecv = {0};
	packetRecv.cbSize = sizeof(packetRecv);
	packetRecv.dwTimeout = 350000;

	HANDLE hServerPacketRecver = (HANDLE) CallService(MS_NETLIB_CREATEPACKETRECVER, (WPARAM)ft->hConn, 2048 * 4);

	int result;
	if (ft->sending)//we are sending
		result = sending_file(ft, hServerPacketRecver, packetRecv);
	else 
		result = receiving_file(ft, hServerPacketRecver, packetRecv);

	Netlib_CloseHandle(hServerPacketRecver);
	Netlib_CloseHandle(ft->hConn);
	ft->hConn = NULL;

	if (result == 0)
	{
		sendBroadcast(ft->hContact, ACKTYPE_FILE, ACKRESULT_SUCCESS, ft, 0);
	}
	else
	{
		if (!ft->requester && result == 1 && !Miranda_Terminated())
		{
			ft->accepted = false;
			HANDLE hConn = aim_peer_connect(AIM_PROXY_SERVER, get_default_port());
			if (hConn) 
			{
				LOG("Connected to proxy ip because we want to use a proxy for the file transfer.");
				ft->requester = true;
				ft->hConn = hConn;
				ForkThread(&CAimProto::aim_proxy_helper, ft);
				return;
			}
		}
		aim_file_ad(hServerConn, seqno, ft->sn, ft->icbm_cookie, true, 0);
		sendBroadcast(ft->hContact, ACKTYPE_FILE, ACKRESULT_FAILED, ft, 0);
	}

	ft_list.remove_by_ft(ft);
}

void aim_direct_connection_initiated(HANDLE hNewConnection, DWORD dwRemoteIP, CAimProto* ppro)//for receiving stuff via dc
{
	 file_transfer *ft;

	char cip[20];
	ppro->LOG("Buddy connected from IP: %s", long_ip_to_char_ip(dwRemoteIP, cip));

	//okay someone connected to us or we initiated the connection- we need to figure out who they are and if they belong
	for (int i=21; --i; )
	{
		ft = ppro->ft_list.find_by_ip(dwRemoteIP);

		if (ft == NULL) ft = ppro->ft_list.find_suitable();
		if (ft || Miranda_Terminated()) break;
		Sleep(100);
	}

	if (ft)
	{
		ft->hConn = hNewConnection;
		ppro->aim_dc_helper(ft);
	}
	else 
		Netlib_CloseHandle(hNewConnection);
}
