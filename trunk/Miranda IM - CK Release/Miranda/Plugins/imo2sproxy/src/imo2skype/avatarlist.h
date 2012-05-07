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

#include "cJSON.h"
#include "memlist.h"

typedef struct
{
	char *pszUser;
	char *pszIcon;
} AVATARENTRY;

TYP_LIST *AvatarList_Init(void);
void AvatarList_Exit(TYP_LIST *hList);

BOOL AvatarList_Insert(TYP_LIST *hList, cJSON *pIcon);
BOOL AvatarList_Remove(TYP_LIST *hList, AVATARENTRY *pEntry);
AVATARENTRY *AvatarList_Find(TYP_LIST *hList, char *pszUser);
void AvatarList_FreeEntry(AVATARENTRY *pEntry);
