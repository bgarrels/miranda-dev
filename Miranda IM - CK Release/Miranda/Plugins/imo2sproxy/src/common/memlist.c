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
#include <string.h>
#include <stdlib.h>

struct _tagLIST
{
    unsigned int    uiCount;
    unsigned int    uiCapacity;
    void            **apStorage;
};

// -----------------------------------------------------------------------------
// Interface
// -----------------------------------------------------------------------------

TYP_LIST *List_Init(unsigned int uiCapacity)
{
    TYP_LIST *pstHandle;

    pstHandle = (TYP_LIST *)malloc(sizeof(TYP_LIST));
    
    if (!pstHandle) return NULL;
    pstHandle->uiCount = 0;
    pstHandle->uiCapacity = uiCapacity;
    if (uiCapacity == 0)
        pstHandle->apStorage = NULL;
    else
    {
        pstHandle->apStorage = (void **)malloc(sizeof(void *)*uiCapacity);
        if (!pstHandle->apStorage)
	{
		free(pstHandle);
		return NULL;
	}
    }
    return pstHandle;
}

// -----------------------------------------------------------------------------

void List_Exit(TYP_LIST *pstHandle)
{
    if (pstHandle->apStorage)
        free (pstHandle->apStorage);
    free (pstHandle);
}

// -----------------------------------------------------------------------------

BOOL List_Push(TYP_LIST *pstHandle, void *pItem)
{
    return List_InsertElementAt(pstHandle, pItem,pstHandle->uiCount);
}

// -----------------------------------------------------------------------------

void *List_Pop (TYP_LIST *pstHandle)
{
	if (pstHandle->uiCount)
		return List_RemoveElementAt(pstHandle ,pstHandle->uiCount-1);
	else return NULL;
}

// -----------------------------------------------------------------------------

BOOL List_ReplaceElementAt(TYP_LIST *pstHandle, void *pItem, unsigned int uiPos)
{
	if (uiPos >= pstHandle->uiCount) return NULL;
	pstHandle->apStorage[uiPos]=pItem;
	return TRUE;
}

// -----------------------------------------------------------------------------

BOOL List_InsertElementAt(TYP_LIST *pstHandle, void *pItem, unsigned int uiPos)
{
    unsigned int    uiStep;
    void            **apNewStorage;

    if (uiPos > pstHandle->uiCount)
        uiPos = pstHandle->uiCount;

    if (pstHandle->uiCount >= pstHandle->uiCapacity)
    {
        uiStep = pstHandle->uiCount*2;
        if (uiStep < 8) uiStep = 8;

	if (!pstHandle->apStorage)
		apNewStorage = (void **)malloc(sizeof(void *)*uiStep);
	else
		apNewStorage = realloc (pstHandle->apStorage, sizeof(void *)*uiStep);
	if (!apNewStorage) return FALSE;
	pstHandle->apStorage = apNewStorage;
	pstHandle->uiCapacity = uiStep;
   }

   if (uiPos<pstHandle->uiCount)
	memmove(&pstHandle->apStorage[uiPos+1], &pstHandle->apStorage[uiPos], (pstHandle->uiCount-uiPos)*sizeof(void*));
    pstHandle->apStorage[uiPos] = pItem;
    pstHandle->uiCount++;
    return TRUE;
}

// -----------------------------------------------------------------------------

void *List_RemoveElementAt(TYP_LIST *pstHandle, unsigned int uiPos)
{
    void            *pRet;

    pRet = pstHandle->apStorage[uiPos];
    if (uiPos<pstHandle->uiCount-1)
        memmove (&pstHandle->apStorage[uiPos], &pstHandle->apStorage[uiPos+1], (pstHandle->uiCount-uiPos-1)*sizeof(void*));
    pstHandle->uiCount--;
    return pRet;
}

// -----------------------------------------------------------------------------

unsigned int List_Count(TYP_LIST *pstHandle)
{
	return pstHandle->uiCount;
}

// -----------------------------------------------------------------------------

void *List_ElementAt(TYP_LIST *pstHandle,unsigned int uiPos)
{
    if (uiPos >= pstHandle->uiCount) return NULL;
    return pstHandle->apStorage[uiPos];
}

// -----------------------------------------------------------------------------

void *List_Top(TYP_LIST *pstHandle)
{
	if (pstHandle->uiCount)
		return List_ElementAt (pstHandle, pstHandle->uiCount-1);
	else return NULL;
}

// -----------------------------------------------------------------------------

void List_Sort(TYP_LIST *pstHandle, int (*pFunc)(const void*,const void*))
{
	qsort(pstHandle->apStorage,pstHandle->uiCount,sizeof(void *),pFunc);
}

// -----------------------------------------------------------------------------

void List_FreeElements(TYP_LIST *pstHandle)
{
	void *pEntry;

	while (pEntry = List_Pop(pstHandle))
		free (pEntry);
}

// -----------------------------------------------------------------------------
