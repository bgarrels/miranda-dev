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

* Queue for incoming calls
*/

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "callqueue.h"

static volatile unsigned int m_uMsgNr=0;
static void FreeEntry(void *pPEntry);

// -----------------------------------------------------------------------------
// Interface
// -----------------------------------------------------------------------------

TYP_LIST *CallQueue_Init(void)
{
	TYP_LIST *hList = List_Init(16);

	return hList;
}

// -----------------------------------------------------------------------------

void CallQueue_Exit(TYP_LIST *hList)
{
	Queue_Exit (hList, FreeEntry);
}

// -----------------------------------------------------------------------------

CALLENTRY *CallQueue_Insert(TYP_LIST *hList, cJSON *pNick, int iDirection)
{
	CALLENTRY *pEntry;
	cJSON *pStream, *pVal, *pPipe;

	if (!(pEntry = Queue_InsertEntry(hList, sizeof(CALLENTRY), ++m_uMsgNr,
		FreeEntry))) return NULL;
	pEntry->pszUser = strdup(cJSON_GetObjectItem(pNick, "buid")->valuestring);
	time (&pEntry->timestamp);
	strcpy (pEntry->szStatus, "RINGING");

	if (pStream = cJSON_GetObjectItem(pNick, "send_stream"))
	{
		strcpy (pEntry->szSendStream, pStream->valuestring);
		if (pStream = cJSON_GetObjectItem(pNick, "recv_stream"))
			strcpy (pEntry->szRecvStream, pStream->valuestring);
	}
	else
	{
		// Copy pipe to Call object
		if (pPipe = cJSON_GetObjectItem(pNick, "pipe"))
		{
			if (pVal = cJSON_GetObjectItem(pPipe, "ip"))
				strncpy (pEntry->szIP, pVal->valuestring, sizeof(pEntry->szIP));
			if (pVal = cJSON_GetObjectItem(pPipe, "conv"))
				strncpy (pEntry->szConv, pVal->valuestring, sizeof(pEntry->szConv));
			if (pVal = cJSON_GetObjectItem(pPipe, "role"))
				pEntry->iRole = pVal->valueint;
		}
	}
	pEntry->iDirection = iDirection;
	return pEntry;
}

// -----------------------------------------------------------------------------

BOOL CallQueue_Remove(TYP_LIST *hList, unsigned int uMsgNr)
{
	return Queue_Remove (hList, uMsgNr, FreeEntry);
}

// -----------------------------------------------------------------------------

CALLENTRY *CallQueue_Find(TYP_LIST *hList, unsigned int uMsgNr)
{
	return (CALLENTRY*)Queue_Find(hList, uMsgNr);
}

// -----------------------------------------------------------------------------
// Static
// -----------------------------------------------------------------------------

static void FreeEntry(void *pPEntry)
{
	CALLENTRY *pEntry = (CALLENTRY*)pPEntry;

	free (pEntry->pszUser);
}

