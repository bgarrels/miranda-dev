#ifndef FILE_H
#define FILE_H

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

struct file_transfer
{
	HANDLE hContact;
	char* sn;

	char icbm_cookie[8];

	HANDLE hConn;
	HANDLE hResumeEvent;

	char* file;
	char* message;

	PROTOFILETRANSFERSTATUS pfts;

	unsigned long cf;

	//below is for when receiving only
	unsigned long local_ip;
	unsigned long verified_ip;
	unsigned long proxy_ip;
	unsigned short port;
	unsigned short max_ver;

	unsigned short req_num;

	bool peer_force_proxy;
	bool me_force_proxy;
	bool sending;
	bool accepted;
	bool requester;
	bool success;

	file_transfer(HANDLE hCont, char* nick, char* cookie);
	~file_transfer();
};

struct ft_list_type : OBJLIST <file_transfer> 
{
	ft_list_type();

	file_transfer* find_by_handle(HANDLE hContact);
	file_transfer* find_by_cookie(char* cookie, HANDLE hContact);
	file_transfer* find_by_ip(unsigned long ip);
	file_transfer* find_suitable(void);

	bool find_by_ft(file_transfer *ft);

	void remove_by_ft(file_transfer *ft);
};


#endif
