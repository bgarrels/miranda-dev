/*
Skype protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright © 2008-2012 leecher, tweety, jls17,
						Laurent Marechal (aka Peorth)
						Gennady Feldman (aka Gena01) 

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

* Implements a simple message queue for send and receive queue.
* We could use memlist.c, but it's not efficient  enough for this purpose
* (would always memmove on removing first element), therefore it's implemented as tail queue.
*/

#include "skype.h"
#include "msgq.h"
#include "debug.h"


void MsgQ_Init(TYP_MSGQ *q)
{
	TAILQ_INIT(&q->l);
	InitializeCriticalSection (&q->cs);
}

void MsgQ_Exit(TYP_MSGQ *q)
{
	struct MsgQueue *ptr;

	EnterCriticalSection(&q->cs);
	while (ptr=q->l.tqh_first)
		free(MsgQ_RemoveMsg(q, ptr));
	LeaveCriticalSection(&q->cs);
	DeleteCriticalSection (&q->cs);
}

BOOL MsgQ_Add(TYP_MSGQ *q, char *msg)
{
	struct MsgQueue *ptr;

	if (!(ptr=(struct MsgQueue*)malloc(sizeof(struct MsgQueue))))
		return FALSE;
	ptr->message = _strdup(msg); // Don't forget to free!
	ptr->tAdded = SkypeTime(NULL);
	SkypeTime(&ptr->tReceived);
	EnterCriticalSection(&q->cs);
	TAILQ_INSERT_TAIL(&q->l, ptr, l);
	//LOG (("MsgQ_Add (%s) @%lu/%ld", msg, ptr->tReceived, ptr->tAdded));
	LeaveCriticalSection(&q->cs);
	return TRUE;
}

char *MsgQ_RemoveMsg(TYP_MSGQ *q, struct MsgQueue *ptr)
{
	char *msg;

	if (!ptr) return NULL;
	TAILQ_REMOVE(&q->l, ptr, l);
	msg=ptr->message;
	free(ptr);
	return msg;
}

char *MsgQ_Get(TYP_MSGQ *q)
{
	char *msg;

	EnterCriticalSection(&q->cs);
	msg=MsgQ_RemoveMsg(q, q->l.tqh_first);
	LeaveCriticalSection(&q->cs);
	return msg;
}

int MsgQ_CollectGarbage(TYP_MSGQ *q, time_t age)
{
	struct MsgQueue *ptr;
	int i=0;

	EnterCriticalSection(&q->cs);
	ptr=q->l.tqh_first;
	while (ptr)
	{
		if (ptr->tAdded && SkypeTime(NULL)-ptr->tAdded>age)
		{
			struct MsgQueue *ptr_;
			LOG(("GarbageCollector throwing out message: %s", ptr->message));
			ptr_=ptr;
			ptr=ptr->l.tqe_next;
			free(MsgQ_RemoveMsg(q, ptr_));
			i++;
			continue;
		}
		ptr=ptr->l.tqe_next;
	}
	LeaveCriticalSection(&q->cs);
	return i;
}
