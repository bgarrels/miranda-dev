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

* Code borrowed for Skype plugin. Fixed to compile on Mingw by G.Feldman
* Original Copyright (c) 2003 Robert Rainwater
*/

#include "skype.h"

/* Gena01 - added some defined to fix compilation with mingw gcc */
/* __try/__finally taken from abiword patch found on the web */
#if 0
 #include <crtdbg.h>
#else
#define __try
#define __except(x) if (0) /* don't execute handler */
#define __finally

#define _try __try
#define _except __except
#define _finally __finally
#endif

#include <excpt.h> 

struct pthread_arg
{
    HANDLE hEvent;
    void (*threadcode) (void *);
    void *arg;
};

void pthread_r(struct pthread_arg *fa)
{
    void (*callercode) (void *) = fa->threadcode;
    void *arg = fa->arg;
    CallService(MS_SYSTEM_THREAD_PUSH, 0, 0);
    SetEvent(fa->hEvent);
    __try {
        callercode(arg);
    }
    __finally {
        CallService(MS_SYSTEM_THREAD_POP, 0, 0);
    }
}

unsigned long pthread_create(pThreadFunc parFunc, void *arg)
{
    unsigned long rc;
    struct pthread_arg fa;
    fa.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    fa.threadcode = parFunc;
    fa.arg = arg;
    rc = _beginthread((pThreadFunc) pthread_r, 0, &fa);
    if ((unsigned long) -1L != rc) {
        WaitForSingleObject(fa.hEvent, INFINITE);
    }
    CloseHandle(fa.hEvent);
    return rc;
}

