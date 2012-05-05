/*
Database Editor++ for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2003-2011 Bio, Jonathan Gordon

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

#include "headers.h"

// thread stuff
struct FORK_ARG {
	HANDLE hEvent;
	void (__cdecl *threadcode)(void*);
	unsigned (__stdcall *threadcodeex)(void*);
	void *arg;
};

void __cdecl forkthread_r(void *param) 
{    
   struct FORK_ARG *fa=(struct FORK_ARG*)param; 
   void (*callercode)(void*)=fa->threadcode; 
   void *arg=fa->arg; 

   CallService(MS_SYSTEM_THREAD_PUSH,0,0); 

   SetEvent(fa->hEvent); 

   __try { 
      callercode(arg); 
   } __finally { 
      CallService(MS_SYSTEM_THREAD_POP,0,0); 
   } 

   return; 
} 

unsigned long forkthread (   void (__cdecl *threadcode)(void*),unsigned long stacksize,void *arg) 
{ 
   unsigned long rc; 
   struct FORK_ARG fa; 

   fa.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL); 
   fa.threadcode=threadcode; 
   fa.arg=arg; 

   rc=_beginthread(forkthread_r,stacksize,&fa); 

   if ((unsigned long)-1L != rc) { 
      WaitForSingleObject(fa.hEvent,INFINITE); 
   } 
   CloseHandle(fa.hEvent); 

   return rc; 
}
