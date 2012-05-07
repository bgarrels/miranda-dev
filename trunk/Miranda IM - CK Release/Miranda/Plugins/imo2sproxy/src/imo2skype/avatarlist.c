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

Manages the avatars of your buddies
*/

#include <stdlib.h>
#include <string.h>
#include "memlist.h"
#include "avatarlist.h"

static void SetEntry(AVATARENTRY *pEntry, cJSON *pNick);

// -----------------------------------------------------------------------------
// Interface
// -----------------------------------------------------------------------------

TYP_LIST *AvatarList_Init(void)
{
	TYP_LIST *hList = List_Init(16);

	return hList;
}

// -----------------------------------------------------------------------------

void AvatarList_Exit(TYP_LIST *hList)
{
	AVATARENTRY *pEntry;

	while (pEntry=(AVATARENTRY*)List_Pop(hList))
	{
		AvatarList_FreeEntry(pEntry);
		free (pEntry);
	}
	List_Exit(hList);
}

// -----------------------------------------------------------------------------

BOOL AvatarList_Insert(TYP_LIST *hList, cJSON *pIcon)
{
	AVATARENTRY *pEntry;

	if (pEntry=AvatarList_Find (hList, cJSON_GetObjectItem(pIcon, "buid")->valuestring))
		AvatarList_FreeEntry (pEntry);
	else
	{
		if (!(pEntry = calloc (1, sizeof(AVATARENTRY)))) return FALSE;
		if (!List_Push(hList, pEntry)) return FALSE;
	}
	SetEntry(pEntry, pIcon);
	return TRUE;
}

// -----------------------------------------------------------------------------

BOOL AvatarList_Remove(TYP_LIST *hList, AVATARENTRY *pEntry)
{
	AVATARENTRY *pListEntry;
	int i, nCount;

	for (i=0, nCount=List_Count(hList); i<nCount; i++)
	{
		pListEntry = List_ElementAt (hList, i);
		if (pListEntry == pEntry) break;
	}
	if (i<nCount)
	{
		AvatarList_FreeEntry (pEntry);
		List_RemoveElementAt(hList, i);
		free (pEntry);
		return TRUE;
	}
	return FALSE;
}

// -----------------------------------------------------------------------------

AVATARENTRY *AvatarList_Find(TYP_LIST *hList, char *pszUser)
{
	int i, nCount;
	AVATARENTRY *pEntry;

	for (i=0, nCount=List_Count(hList); i<nCount; i++)
	{
		pEntry = List_ElementAt (hList, i);
		if (strcmp(pEntry->pszUser, pszUser) == 0)
			return pEntry;
	}
	return NULL;
}

// -----------------------------------------------------------------------------

void AvatarList_FreeEntry(AVATARENTRY *pEntry)
{
	if (pEntry->pszUser) free (pEntry->pszUser);
	if (pEntry->pszIcon) free (pEntry->pszIcon);
}

// -----------------------------------------------------------------------------
// Static
// -----------------------------------------------------------------------------

static void SetEntry(AVATARENTRY *pEntry, cJSON *pNick)
{
	pEntry->pszUser = strdup(cJSON_GetObjectItem(pNick, "buid")->valuestring);
	pEntry->pszIcon = strdup(cJSON_GetObjectItem(pNick, "icon")->valuestring);
}
