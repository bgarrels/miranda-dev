/*
Skype protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright © 2008-2012 leecher, tweety, jls17 

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

#include <time.h>

// ----------------------------------------------------------------------
// Stolen from *nix sys/queue.h
// ----------------------------------------------------------------------
/*
 * Tail queue definitions.
 */
#define TAILQ_HEAD(name, type)                                          \
struct name {                                                           \
        struct type *tqh_first; /* first element */                     \
        struct type **tqh_last; /* addr of last next element */         \
}

#define TAILQ_ENTRY(type)                                               \
struct {                                                                \
        struct type *tqe_next;  /* next element */                      \
        struct type **tqe_prev; /* address of previous next element */  \
}


/*
 * Tail queue functions.
 */
#define TAILQ_INIT(head) {                                              \
        (head)->tqh_first = NULL;                                       \
        (head)->tqh_last = &(head)->tqh_first;                          \
}

#define TAILQ_INSERT_TAIL(head, elm, field) {                           \
        (elm)->field.tqe_next = NULL;                                   \
        (elm)->field.tqe_prev = (head)->tqh_last;                       \
        *(head)->tqh_last = (elm);                                      \
        (head)->tqh_last = &(elm)->field.tqe_next;                      \
}

#define TAILQ_REMOVE(head, elm, field) {                                \
        if (((elm)->field.tqe_next) != NULL)                            \
                (elm)->field.tqe_next->field.tqe_prev =                 \
                    (elm)->field.tqe_prev;                              \
        else                                                            \
                (head)->tqh_last = (elm)->field.tqe_prev;               \
        *(elm)->field.tqe_prev = (elm)->field.tqe_next;                 \
}
// ----------------------------------------------------------------------

struct MsgQueue {
	TAILQ_ENTRY(MsgQueue) l;
	char *message;
	time_t tAdded;
	time_t tReceived;
};
typedef struct
{
	TAILQ_HEAD(tag_msgq, MsgQueue) l;
	CRITICAL_SECTION cs;
} TYP_MSGQ;

void MsgQ_Init(TYP_MSGQ *q);
void MsgQ_Exit(TYP_MSGQ *q);
BOOL MsgQ_Add(TYP_MSGQ *q, char *msg);
char *MsgQ_RemoveMsg(TYP_MSGQ *q, struct MsgQueue *ptr);
char *MsgQ_Get(TYP_MSGQ *q);
int MsgQ_CollectGarbage(TYP_MSGQ *q, time_t age);
