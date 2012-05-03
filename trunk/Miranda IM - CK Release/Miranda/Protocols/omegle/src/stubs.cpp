/*
Omegle protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright � 2011-2012 Robert P�sel

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

#include "common.h"

HANDLE OmegleProto::AddToList(int flags, PROTOSEARCHRESULT* psr)
{
	return 0;
};

HANDLE OmegleProto::AddToListByEvent(int flags,int iContact,HANDLE hDbEvent)
{
	return 0;
};

int OmegleProto::Authorize(HANDLE hContact)
{
	return 0;
}

int OmegleProto::AuthDeny(HANDLE hContact,const PROTOCHAR *reason)
{
	return 0;
}

int OmegleProto::AuthRecv(HANDLE hContact,PROTORECVEVENT *)
{
	return 0;
}

int OmegleProto::AuthRequest(HANDLE hContact,const PROTOCHAR *message)
{
	return 0;
}

HANDLE OmegleProto::ChangeInfo(int type,void *info_data)
{
	return 0;
}

HANDLE OmegleProto::FileAllow(HANDLE hContact,HANDLE hTransfer,const PROTOCHAR *path)
{
	return 0;
}

int OmegleProto::FileCancel(HANDLE hContact,HANDLE hTransfer)
{
	return 0;
}

int OmegleProto::FileDeny(HANDLE hContact,HANDLE hTransfer,const PROTOCHAR *reason)
{
	return 0;
}

int OmegleProto::FileResume(HANDLE hTransfer,int *action,const PROTOCHAR **filename)
{
	return 0;
}

int OmegleProto::GetInfo( HANDLE hContact, int infoType )
{
	return 1;
}

HANDLE OmegleProto::SearchBasic( const PROTOCHAR* id )
{
	return 0;
}

HANDLE OmegleProto::SearchByEmail( const PROTOCHAR* email )
{
	return 0;
}

HANDLE OmegleProto::SearchByName( const PROTOCHAR* nick, const PROTOCHAR* firstName, const PROTOCHAR* lastName )
{
	return 0;
}

HWND OmegleProto::SearchAdvanced(HWND owner)
{
	return 0;
}

HWND OmegleProto::CreateExtendedSearchUI(HWND owner)
{
	return 0;
}

int OmegleProto::RecvContacts(HANDLE hContact,PROTORECVEVENT *)
{
	return 0;
}

int OmegleProto::RecvFile(HANDLE hContact,PROTORECVFILET *)
{
	return 0;
}

int OmegleProto::RecvUrl(HANDLE hContact,PROTORECVEVENT *)
{
	return 0;
}

int OmegleProto::SendContacts(HANDLE hContact,int flags,int nContacts,HANDLE *hContactsList)
{
	return 0;
}

HANDLE OmegleProto::SendFile(HANDLE hContact,const PROTOCHAR *desc, PROTOCHAR **files)
{
	return 0;
}

int OmegleProto::SendUrl(HANDLE hContact,int flags,const char *url)
{
	return 0;
}

int OmegleProto::SetApparentMode(HANDLE hContact,int mode)
{
	return 0;
}

int OmegleProto::RecvAwayMsg(HANDLE hContact,int mode,PROTORECVEVENT *evt)
{
	return 0;
}

int OmegleProto::SendAwayMsg(HANDLE hContact,HANDLE hProcess,const char *msg)
{
	return 0;
}

int OmegleProto::UserIsTyping(HANDLE hContact, int type)
{
	return 0;
}

int OmegleProto::SetAwayMsg(int iStatus, const PROTOCHAR* msg)
{
	return 0;
}

HANDLE OmegleProto::GetAwayMsg( HANDLE hContact )
{
	return 0;
}

int OmegleProto::RecvMsg(HANDLE hContact, PROTORECVEVENT *pre)
{
	return 0;
}
