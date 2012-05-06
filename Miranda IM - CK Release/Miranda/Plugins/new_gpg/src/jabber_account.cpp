/*
New_gpg plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author 
			Copyright (C) 2010-2011 sss

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

#include "commonheaders.h"

void JabberAccount::setAccountName(TCHAR *Name)
{
	AccountName = Name;
}
void JabberAccount::setAccountNumber(int Number)
{
	AccountNumber = Number;
}
void JabberAccount::setJabberInterface(IJabberInterface *JIf)
{
	JabberInterface = JIf;
}
void JabberAccount::setSendHandler(HJHANDLER hHandler)
{
	hSendHandler = hHandler;
}
void JabberAccount::setPrescenseHandler(HJHANDLER hHandler)
{
	hPrescenseHandler = hHandler;
}
void JabberAccount::setMessageHandler(HJHANDLER hHandler)
{
	hMessageHandler = hHandler;
}


TCHAR *JabberAccount::getAccountName()
{
	return AccountName;
}
int JabberAccount::getAccountNumber()
{
	return AccountNumber;
}
IJabberInterface *JabberAccount::getJabberInterface()
{
	return JabberInterface;
}
HJHANDLER JabberAccount::getSendHandler()
{
	return hSendHandler;
}
HJHANDLER JabberAccount::getPrescenseHandler()
{
	return hPrescenseHandler;
}

HJHANDLER JabberAccount::getMessageHandler()
{
	return hMessageHandler;
}


JabberAccount::JabberAccount()
{
	AccountName = NULL;
	hSendHandler = INVALID_HANDLE_VALUE;
	hPrescenseHandler = INVALID_HANDLE_VALUE;
	hMessageHandler = INVALID_HANDLE_VALUE;
	AccountNumber = -1;
	JabberInterface = NULL;
}
JabberAccount::~JabberAccount()
{
	if(AccountName)
		mir_free(AccountName);
}