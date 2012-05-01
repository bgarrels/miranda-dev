#ifndef WINDOWS_H
#define WINDOWS_H

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

#ifndef CFM_BACKCOLOR
#define CFM_BACKCOLOR 0x04000000
#endif
#ifndef CFE_AUTOBACKCOLOR
#define CFE_AUTOBACKCOLOR CFM_BACKCOLOR
#endif

struct invite_chat_param
{
	char* id;
	CAimProto* ppro;
	
	invite_chat_param(const char* idt, CAimProto* prt)
	{ id = mir_strdup(idt); ppro = prt; }
};

struct invite_chat_req_param
{
	chatnav_param* cnp;
	CAimProto* ppro;
	char* message;
	char* name;
	char* icbm_cookie;
	
	invite_chat_req_param(chatnav_param* cnpt, CAimProto* prt, char* msg, char* nm, char* icki)
	{ cnp = cnpt; ppro = prt; message = mir_strdup(msg); name = mir_strdup(nm); 
	  icbm_cookie = (char*)mir_alloc(8); memcpy(icbm_cookie, icki, 8); }

	~invite_chat_req_param()
	{ mir_free(message); mir_free(name); mir_free(icbm_cookie); }
};

INT_PTR CALLBACK instant_idle_dialog(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK join_chat_dialog(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK invite_to_chat_dialog(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK chat_request_dialog(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void CALLBACK chat_request_cb(PVOID dwParam);

#endif
