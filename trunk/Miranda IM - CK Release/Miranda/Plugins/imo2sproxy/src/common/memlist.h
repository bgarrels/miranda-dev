#ifndef __LIST_H__
#define __LIST_H__

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

#ifndef BOOL
#define BOOL int
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

struct _tagLIST;
typedef struct _tagLIST TYP_LIST;

TYP_LIST *List_Init(unsigned int uiCapacity);
void List_Exit(TYP_LIST *pstHandle);
BOOL List_Push(TYP_LIST *pstHandle, void *pItem);
void *List_Pop (TYP_LIST *pstHandle);
BOOL List_ReplaceElementAt(TYP_LIST *pstHandle, void *pItem, unsigned int uiPos);
BOOL List_InsertElementAt(TYP_LIST *pstHandle, void *pItem, unsigned int uiPos);
void *List_RemoveElementAt(TYP_LIST *pstHandle, unsigned int uiPos);
unsigned int List_Count(TYP_LIST *pstHandle);
void *List_ElementAt(TYP_LIST *pstHandle,unsigned int uiPos);
void *List_Top(TYP_LIST *pstHandle);
void List_FreeElements(TYP_LIST *pstHandle);
void List_Sort(TYP_LIST *pstHandle, int (*pFunc)(const void*,const void*));

#endif
