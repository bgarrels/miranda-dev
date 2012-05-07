/*
Skype lugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
Copyright (C) 2009-2012 leecher

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

* Communication layer for imo.im Skype 
*/

#include "cJSON.h"

struct _tagIMOSKYPE;
typedef struct _tagIMOSKYPE IMOSKYPE;
// 0 	- 	Message received, you may delete the cJSON object now
// 1	-	Don't delete cJSON object, callback will take care of freeing
typedef int(*IMOSTATCB)(cJSON *pMsg, void *pUser);

IMOSKYPE *ImoSkype_Init(IMOSTATCB StatusCb, void *pUser);
void ImoSkype_Exit(IMOSKYPE *hSkype);
void ImoSkype_CancelPolling(IMOSKYPE *hSkype);
char *ImoSkype_GetLastError(IMOSKYPE *hSkype);
char *ImoSkype_GetUserHandle(IMOSKYPE *hSkype);

int ImoSkype_Login(IMOSKYPE *hSkype, char *pszUser, char *pszPass);
int ImoSkype_Logout(IMOSKYPE *hSkype);
int ImoSkype_Poll(IMOSKYPE *hSkype);
int ImoSkype_Typing(IMOSKYPE *hSkype, char *pszBuddy, char *pszStatus);
int ImoSkype_SetStatus(IMOSKYPE *hSkype, char *pszStatus, char *pszStatusMsg);
int ImoSkype_SendMessage(IMOSKYPE *hSkype, char *pszBuddy, char *pszMessage);
int ImoSkype_AddBuddy(IMOSKYPE *hSkype, char *pszBuddy);
int ImoSkype_DelBuddy(IMOSKYPE *hSkype, char *pszBuddy, char *pszGroup);
int ImoSkype_BlockBuddy(IMOSKYPE *hSkype, char *pszBuddy);
int ImoSkype_UnblockBuddy(IMOSKYPE *hSkype, char *pszBuddy);
int ImoSkype_ChangeAlias(IMOSKYPE *hSkype, char *pszBuddy, char *pszNewAlias);
int ImoSkype_StartVoiceCall(IMOSKYPE *hSkype, char *pszBuddy);
int ImoSkype_KeepAlive(IMOSKYPE *hSkype);
int ImoSkype_Ping(IMOSKYPE *hSkype);
char *ImoSkype_GetAvatar(IMOSKYPE *hSkype, char *pszID, unsigned int *pdwLength);
