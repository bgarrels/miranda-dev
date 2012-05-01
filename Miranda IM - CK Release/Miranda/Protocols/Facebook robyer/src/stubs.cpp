/*
Facebook plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			(C) 2009-2011 Michal Zelinka
			(C) 2011-2012 Robert Pösel

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

HANDLE FacebookProto::AddToList(int flags, PROTOSEARCHRESULT* psr)
{
	return 0;
};

HANDLE FacebookProto::AddToListByEvent(int flags,int iContact,HANDLE hDbEvent)
{
	return 0;
};

int FacebookProto::Authorize(HANDLE hContact)
{
	return 0;
}

int FacebookProto::AuthDeny(HANDLE hContact,const PROTOCHAR *reason)
{
	return 0;
}

int FacebookProto::AuthRecv(HANDLE hContact,PROTORECVEVENT *)
{
	return 0;
}

int FacebookProto::AuthRequest(HANDLE hContact,const PROTOCHAR *message)
{
	return 0;
}

HANDLE FacebookProto::ChangeInfo(int type,void *info_data)
{
	MessageBoxA(0,"ChangeInfo","",0);
	return 0;
}

HANDLE FacebookProto::FileAllow(HANDLE hContact,HANDLE hTransfer,const PROTOCHAR *path)
{
	return 0;
}

int FacebookProto::FileCancel(HANDLE hContact,HANDLE hTransfer)
{
	return 0;
}

int FacebookProto::FileDeny(HANDLE hContact,HANDLE hTransfer,const PROTOCHAR *reason)
{
	return 0;
}

int FacebookProto::FileResume(HANDLE hTransfer,int *action,const PROTOCHAR **filename)
{
	return 0;
}

int FacebookProto::GetInfo( HANDLE hContact, int infoType )
{
	// TODO: Most probably some ProtoAck should be here instead
	return 1;
}

HANDLE FacebookProto::SearchBasic( const PROTOCHAR* id )
{
	return 0;
}

HANDLE FacebookProto::SearchByEmail( const PROTOCHAR* email )
{
	return 0;
}

HANDLE FacebookProto::SearchByName( const PROTOCHAR* nick, const PROTOCHAR* firstName, const PROTOCHAR* lastName )
{
	return 0;
}

HWND FacebookProto::SearchAdvanced(HWND owner)
{
	return 0;
}

HWND FacebookProto::CreateExtendedSearchUI(HWND owner)
{
	return 0;
}

int FacebookProto::RecvContacts(HANDLE hContact,PROTORECVEVENT *)
{
	return 0;
}

int FacebookProto::RecvFile(HANDLE hContact,PROTORECVFILET *)
{
	return 0;
}

int FacebookProto::RecvUrl(HANDLE hContact,PROTORECVEVENT *)
{
	return 0;
}

int FacebookProto::SendContacts(HANDLE hContact,int flags,int nContacts,HANDLE *hContactsList)
{
	return 0;
}

HANDLE FacebookProto::SendFile(HANDLE hContact,const PROTOCHAR *desc, PROTOCHAR **files)
{
	return 0;
}

int FacebookProto::SendUrl(HANDLE hContact,int flags,const char *url)
{
	return 0;
}

int FacebookProto::SetApparentMode(HANDLE hContact,int mode)
{
	return 0;
}

int FacebookProto::RecvAwayMsg(HANDLE hContact,int mode,PROTORECVEVENT *evt)
{
	return 0;
}

int FacebookProto::SendAwayMsg(HANDLE hContact,HANDLE hProcess,const char *msg)
{
	return 0;
}
