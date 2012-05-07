#ifndef __QUEUE_H__
#define __QUEUE_H__

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
*/

#include "memlist.h"

typedef struct
{
	unsigned int uMsgNr;
} QUEUEHDR;

void Queue_Exit(TYP_LIST *hList, void (*fpFree)(void *pEntry));
void* Queue_InsertEntry (TYP_LIST *hList, unsigned int cbSize, unsigned int uMsgNr,
                         void (*fpFree)(void *pEntry));
BOOL Queue_Remove(TYP_LIST *hList, unsigned int uMsgNr, void (*fpFree)(void *pEntry));
void *Queue_Find(TYP_LIST *hList, unsigned int uMsgNr);

#endif
