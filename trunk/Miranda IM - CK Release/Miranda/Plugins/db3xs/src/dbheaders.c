/*
db3xs (secure) database driver for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright (C) sje, Piotr Pawluczuk, a0x, Billy_Bons

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
#include "database.h"
#include "SecureDB.h"

extern struct DBHeader dbHeader;
extern HANDLE hDbFile;

struct DBSignature {
  char name[15];
  BYTE eof;
};
struct DBSignature dbSignature={"Miranda ICQ DB",0x1A};
struct DBSignature dbSecSignature={"Miranda SEC DB",0x1A};

//the cache has not been loaded when these functions are used

int CreateDbHeaders(HANDLE hFile)
{
	struct DBContact user;
	DWORD bytesWritten;

	CopyMemory(dbHeader.signature,&dbSignature,sizeof(dbHeader.signature));
	dbHeader.version=DB_THIS_VERSION;
	dbHeader.ofsFileEnd=sizeof(struct DBHeader);
	dbHeader.slackSpace=0;
	dbHeader.contactCount=0;
	dbHeader.ofsFirstContact=0;
	dbHeader.ofsFirstModuleName=0;
	dbHeader.ofsUser=0;
	//create user
	dbHeader.ofsUser=dbHeader.ofsFileEnd;
	dbHeader.ofsFileEnd+=sizeof(struct DBContact);
	SetFilePointer(hFile,0,NULL,FILE_BEGIN);
	EncWriteFile(hFile,&dbHeader,sizeof(dbHeader),&bytesWritten,NULL);
	user.signature=DBCONTACT_SIGNATURE;
	user.ofsNext=0;
	user.ofsFirstSettings=0;
	user.eventCount=0;
	user.ofsFirstEvent=user.ofsLastEvent=0;
	SetFilePointer(hFile,dbHeader.ofsUser,NULL,FILE_BEGIN);
	EncWriteFile(hFile,&user,sizeof(struct DBContact),&bytesWritten,NULL);
	FlushFileBuffers(hFile);
	return 0;
}

int CheckDbHeaders(struct DBHeader * hdr, int* secure)
{
	if(!memcmp(hdr->signature,&dbSecSignature,sizeof(hdr->signature)))
		*secure = 1;
	else {
		if(memcmp(hdr->signature,&dbSignature,sizeof(hdr->signature))) return 1;
		if(hdr->version!=DB_THIS_VERSION) return 2;
		*secure = 0;
	}
	if(hdr->ofsUser==0) return 3;
	return 0;
}

int InitialiseDbHeaders(void)
{
	return 0;
}
