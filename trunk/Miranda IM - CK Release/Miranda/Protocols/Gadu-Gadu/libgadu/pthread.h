/*
GaduGadu plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			(C) 2001-2002 Wojtek Kaniewski <wojtekka@irc.pl>
			(C)	Dawid Jarosz <dawjar@poczta.onet.pl>
			(C) 2003-2009 Adam Strzelecki <ono+miranda@java.pl>
			(C) 2009-2012 Bartosz Bia³ek
			(C) 2008-2010 Robert Rainwater

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

#ifndef PTHREAD_H
#define PTHREAD_H

#include <windows.h>

// Minipthread code from Miranda IM source
typedef struct
{
	HANDLE hThread;
	DWORD dwThreadId;
}
pthread_t;

typedef int pthread_attr_t;
typedef CRITICAL_SECTION pthread_mutex_t;

/* create thread */
int pthread_create(pthread_t *tid, const pthread_attr_t *attr, void *(__stdcall *thread_start) (void *), void *param);
/* wait for thread termination */
int pthread_join(pthread_t *tid, void **value_ptr);
/* detach a thread */
int pthread_detach(pthread_t *tid);
/* get calling thread's ID */
pthread_t *pthread_self(void);
/* cancel execution of a thread */
int pthread_cancel(pthread_t *thread);
/* terminate thread */
void pthread_exit(void *value_ptr);

#define pthread_mutex_init(pmutex, pattr)	 InitializeCriticalSection(pmutex)
#define pthread_mutex_destroy(pmutex)		 DeleteCriticalSection(pmutex)
#define pthread_mutex_lock(pmutex)			 EnterCriticalSection(pmutex)
#define pthread_mutex_unlock(pmutex)		 LeaveCriticalSection(pmutex)

#endif
