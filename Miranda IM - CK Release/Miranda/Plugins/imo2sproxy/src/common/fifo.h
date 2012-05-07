#ifndef __FIFO_H__
#define __FIFO_H__

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

struct _tagFIFO;
typedef struct _tagFIFO TYP_FIFO;

#ifndef BOOL
#define BOOL int
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

TYP_FIFO *Fifo_Init(unsigned int uiCapacity);
void Fifo_Exit(TYP_FIFO *pstHandle);
char *Fifo_AllocBuffer(TYP_FIFO *pstHandle, unsigned int uiPCount);
BOOL Fifo_Add(TYP_FIFO *pstHandle, char *acPBytes, unsigned int uiPCount);
BOOL Fifo_AddString(TYP_FIFO *pstHandle, char *pszString);
char *Fifo_Get (TYP_FIFO *pstHandle, unsigned int *uiPCount);
unsigned int Fifo_Count (TYP_FIFO *pstHandle);
void Fifo_Reset (TYP_FIFO *hFifo);

#endif
