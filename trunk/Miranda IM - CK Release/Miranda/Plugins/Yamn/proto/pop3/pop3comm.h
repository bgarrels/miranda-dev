#ifndef __POP3COMM_H
#define __POP3COMM_H

/*
YAMN plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2002-2004 majvan
			Copyright (C) 2005-2007 tweety y_b

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

#include <windows.h>
#include "pop3.h"

#include "m_protoplugin.h"
//We can use synchro.h because this is internal plugin. If you use external plugin,
//and you want to use SO for your plugin, you can use YAMN's SO.
//All you need is to include synchro.h and use YAMN's exported synchronization functions.
#include "m_synchro.h"

//For mail exported functions defintions
#include "../../mails/m_mails.h"

#include "../../src/debug.h"
					
#define	POP3_FILEVERSION	1	//Version of aditional information stored in book file

typedef struct CPOP3Account: public CAccount
{
// We can use SCOUNTER structure, because this is internal plugin.
// This SO is used to determine if any POP3 account is in "write access" mode
	static PSCOUNTER AccountWriterSO;

// It is usefull to have client structure in account. With this structure we have access to account's socket.
// This is related to InternetQueries and UseInternetFree
// This member should be synchronized with UseInternetFree
	class CPop3Client Client;

// This member is usefull for MIME headers. It is default codepage, if no other codepage found
	WORD CP;				//access only through AccountAccessSO

// In this memeber last error code is stored
	DWORD SystemError;			//access through UseInternetFree

// We use only counter from this object and it is # of threads waiting to work on internet.
// We use event UseInternet to access critical sections.
// It is usefull in 2 ways: we have mutual exclusion that only one thread works with account on internet.
// Thread, which has done its work with account on internet can close socket, but it is not needed, when any other
// thread wants to work (e.g. we have deleted mails, but when deleting, another thread wants to check new mail, so
// we delete all needed mails and check if there's thread that wants to work. If yes, we do not need to quit session,
// we leave socket open, and leave internet. Another thread then start checking and does not connect, does not send
// user and password... because socket is open- it continues)
	PSCOUNTER InternetQueries;
	HANDLE UseInternetFree;

	CPOP3Account();
	~CPOP3Account();

} POP3ACCOUNT,*HPOP3ACCOUNT;

typedef struct POP3LayeredError
{
	BOOL SSL;
	DWORD AppError;
	DWORD POP3Error;
	DWORD NetError;
	DWORD SystemError;
} POP3_ERRORCODE,*PPOP3_ERRORCODE;

struct YAMNExportedFcns
{
	YAMN_SETPROTOCOLPLUGINFCNIMPORTFCN	SetProtocolPluginFcnImportFcn;
	YAMN_WAITTOWRITEFCN	WaitToWriteFcn;
	YAMN_WRITEDONEFCN	WriteDoneFcn;
	YAMN_WAITTOREADFCN	WaitToReadFcn;
	YAMN_READDONEFCN	ReadDoneFcn;
	YAMN_SCMANAGEFCN	SCGetNumberFcn;
	YAMN_SCMANAGEFCN	SCIncFcn;
	YAMN_SCMANAGEFCN	SCDecFcn;
	YAMN_SETSTATUSFCN	SetStatusFcn;
	YAMN_GETSTATUSFCN	GetStatusFcn;
};

struct MailExportedFcns
{
	YAMN_SYNCHROMIMEMSGSFCN	SynchroMessagesFcn;
	YAMN_TRANSLATEHEADERFCN	TranslateHeaderFcn;
	YAMN_APPENDQUEUEFCN	AppendQueueFcn;
	YAMN_DELETEMIMEQUEUEFCN	DeleteMessagesToEndFcn;
	YAMN_DELETEMIMEMESSAGEFCN	DeleteMessageFromQueueFcn;
	YAMN_FINDMIMEMESSAGEFCN	FindMessageByIDFcn;
	YAMN_CREATENEWDELETEQUEUEFCN	CreateNewDeleteQueueFcn;
};

enum
{
	EACC_QUEUEALLOC=1,	//memory allocation
	EACC_STOPPED,		//stop account
};

#define	NO_MAIL_FOR_DELETE	1

#define	POP3_DELETEFROMCHECK	1

#endif
