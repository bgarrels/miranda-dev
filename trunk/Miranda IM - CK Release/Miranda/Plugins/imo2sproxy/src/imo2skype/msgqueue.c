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

* Sort on insert, do a binary search instead of iterating list.
*/

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "msgqueue.h"
#include "buddylist.h"

static volatile unsigned int m_uMsgNr=0;
static void FreeEntry(void *pEntry);

// -----------------------------------------------------------------------------
// Interface
// -----------------------------------------------------------------------------

TYP_LIST *MsgQueue_Init(void)
{
	TYP_LIST *hList = List_Init(16);

	return hList;
}

// -----------------------------------------------------------------------------

void MsgQueue_Exit(TYP_LIST *hList)
{
	Queue_Exit(hList, FreeEntry);
}

// -----------------------------------------------------------------------------

MSGENTRY *MsgQueue_Insert(TYP_LIST *hList, cJSON *pNick)
{
	MSGENTRY *pEntry;

	if (!(pEntry = Queue_InsertEntry(hList, sizeof(MSGENTRY), ++m_uMsgNr, 
		FreeEntry))) return NULL;
	pEntry->pszUser = strdup(cJSON_GetObjectItem(pNick, "buid")->valuestring);
	pEntry->pszAlias = strdup(cJSON_GetObjectItem(pNick, "alias")->valuestring);
	pEntry->pszMessage = strdup(cJSON_GetObjectItem(pNick, "msg")->valuestring);
	pEntry->timestamp = cJSON_GetObjectItem(pNick, "timestamp")->valueint;
	strcpy (pEntry->szStatus, "RECEIVED");
	return pEntry;
}

// -----------------------------------------------------------------------------

MSGENTRY *MsgQueue_AddReflect(TYP_LIST *hList, cJSON *pNick, TYP_LIST *hBuddyList)
{
	MSGENTRY *pEntry;
	cJSON *pVal;

	if (!(pEntry = Queue_InsertEntry(hList, sizeof(MSGENTRY), ++m_uMsgNr,
		FreeEntry))) return NULL;
	pEntry->pszUser = strdup(cJSON_GetObjectItem(pNick, "buid")->valuestring);

	// Usually no alias in Reflection, so query buddy list, if available
	if (pVal = cJSON_GetObjectItem(pNick, "alias"))
		pEntry->pszAlias = strdup(pVal->valuestring);
	else
	{
		NICKENTRY *pBuddy = hBuddyList?BuddyList_Find (hBuddyList, pEntry->pszUser):NULL;
		pEntry->pszAlias = strdup (pBuddy?pBuddy->pszAlias:pEntry->pszUser);
	}

	pEntry->pszMessage = strdup(cJSON_GetObjectItem(pNick, "msg")->valuestring);
	pEntry->timestamp = cJSON_GetObjectItem(pNick, "timestamp")->valueint;
	strcpy (pEntry->szStatus, "SENT");
	return pEntry;
}

// -----------------------------------------------------------------------------

MSGENTRY *MsgQueue_AddSent(TYP_LIST *hList, char *pszUser, char *pszAlias, char *pszMessage, unsigned int *puMsgId)
{
	MSGENTRY *pEntry;

	if (!(pEntry = Queue_InsertEntry(hList, sizeof(MSGENTRY), ++m_uMsgNr,
		FreeEntry))) return NULL;
	pEntry->pszUser = strdup(pszUser);
	pEntry->pszAlias = strdup(pszAlias);
	pEntry->pszMessage = strdup(pszMessage);
	time (&pEntry->timestamp);
	strcpy (pEntry->szStatus, "SENDING");
	if (puMsgId) *puMsgId = pEntry->hdr.uMsgNr;
	return pEntry;
}

// -----------------------------------------------------------------------------

BOOL MsgQueue_Remove(TYP_LIST *hList, unsigned int uMsgNr)
{
	return Queue_Remove(hList, uMsgNr, FreeEntry);
}

// -----------------------------------------------------------------------------

MSGENTRY *MsgQueue_Find(TYP_LIST *hList, unsigned int uMsgNr)
{
	return (MSGENTRY*)Queue_Find(hList, uMsgNr);
}

// -----------------------------------------------------------------------------
// Static
// -----------------------------------------------------------------------------

static void FreeEntry(void *pPEntry)
{
	MSGENTRY *pEntry = (MSGENTRY*)pPEntry;
	
	if (pEntry->pszAlias) free (pEntry->pszAlias);
	free (pEntry->pszUser);
	free (pEntry->pszMessage);
}

