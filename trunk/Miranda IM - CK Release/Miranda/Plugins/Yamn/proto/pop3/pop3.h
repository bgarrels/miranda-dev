#ifndef __POP3_H
#define __POP3_H

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

#include "../../src/debug.h"
#include "../netlib.h"				//NetLib client

#define DOTLINE(s)	((((s)[-2]=='\r') || ((s)[-2]=='\n')) && ((s)[-1]=='.') && (((s)[0]=='\r') || ((s)[0]=='\n') || ((s)[0]=='\0')))	// be careful, it's different to ESR's pop3.c ;-)
#define ENDLINE(s)	(((s)[0]=='\r') || ((s)[0]=='\n'))	//endline
#define OKLINE(s)	(((s)[0]=='+') && (((s)[1]=='o') || ((s)[1]=='O')) && (((s)[2]=='k') || ((s)[2]=='K')))	// +OK
#define ERRLINE(s)	(((s)[0]=='-') && (((s)[1]=='e') || ((s)[1]=='E')) && (((s)[2]=='r') || ((s)[2]=='R')) && (((s)[3]=='r') || ((s)[3]=='R')))	// -ERR
#define ACKLINE(s)	(OKLINE(s) || ERRLINE(s))

#define	POP3_SEARCHDOT	1
#define	POP3_SEARCHACK	2
#define	POP3_SEARCHOK	3
#define	POP3_SEARCHERR	4
#define	POP3_SEARCHNL	5

#define POP3_FOK	1
#define POP3_FERR	2

class CPop3Client
{
public:
	CPop3Client(): NetClient(NULL), Stopped(FALSE) {}
	~CPop3Client() {if(NetClient!=NULL) delete NetClient;}

	char* Connect(const char* servername,const int port=110,BOOL UseSSL=FALSE, BOOL NoTLS=FALSE);
	char* RecvRest(char* prev,int mode,int size=65536);
	char* User(char* name);
	char* Pass(char* pw);
	char* APOP(char* name, char* pw, char* timestamp);
	char* Quit();
	char* Stat();
	char* List();
	char* Top(int nr, int lines=0);
	char* Uidl(int nr=0);
	char* Dele(int nr);
	char* Retr(int nr);

	unsigned char AckFlag;
	BOOL SSL;
	BOOL Stopped;

	DWORD POP3Error;
	class CNetClient *NetClient;	//here the network layout is defined (TCP or SSL+TCP etc.)
private:
	BOOL SearchFromEnd(char *end,int bs,int mode);
	BOOL SearchFromStart(char *end,int bs,int mode);
};

enum
{
	EPOP3_QUEUEALLOC=1,	//memory allocation
	EPOP3_STOPPED,		//stop account
	EPOP3_CONNECT,		//cannot connect to server
	EPOP3_RESTALLOC,	//cannot allocate memory for received data
	EPOP3_BADUSER,		//cannot login because USER command failed
	EPOP3_BADPASS,		//cannot login because PASS command failed
	EPOP3_APOP,		//server does not send timestamp for APOP auth
	EPOP3_STAT,
	EPOP3_LIST,
	EPOP3_UIDL,
};

#endif
