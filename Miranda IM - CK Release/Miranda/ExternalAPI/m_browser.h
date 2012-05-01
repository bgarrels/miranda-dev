#ifndef __MAILBROWSER_H
#define __MAILBROWSER_H

/*
Miranda IM: the free IM client for Microsoft* Windows*

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

===============================================================================
*/

#include "m_account.h"
#include "debug.h"

typedef struct MailBrowserWinParam
{
#define YAMN_MAILBROWSERVERSION	1
	HANDLE ThreadRunningEV;
	HACCOUNT account;
	DWORD nflags;			//flags YAMN_ACC_??? when new mails
	DWORD nnflags;			//flags YAMN_ACC_??? when no new mails
	void *Param;
} YAMN_MAILBROWSERPARAM,*PYAMN_MAILBROWSERPARAM;

typedef struct MailShowMsgWinParam
{
	HANDLE ThreadRunningEV;
	HACCOUNT account;
	HYAMNMAIL mail;
} YAMN_MAILSHOWPARAM, *PYAMN_MAILSHOWPARAM;

typedef struct NoNewMailParam
{
#define YAMN_NONEWMAILVERSION	1
	HANDLE ThreadRunningEV;
	HACCOUNT account;
	DWORD flags;
	void *Param;
} YAMN_NONEWMAILPARAM,*PYAMN_NONEWMAILPARAM;

typedef struct BadConnectionParam
{
#define YAMN_BADCONNECTIONVERSION	1
	HANDLE ThreadRunningEV;
	HACCOUNT account;
	UINT_PTR errcode;
	void *Param;
} YAMN_BADCONNECTIONPARAM,*PYAMN_BADCONNECTIONPARAM;

#endif
