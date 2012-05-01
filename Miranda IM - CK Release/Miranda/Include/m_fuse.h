#ifdef _ALPHA_FUSE_
#ifndef _M_FUSE_
#define _M_FUSE_

/*
Miranda IM: the free IM client for Microsoft* Windows*

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

===============================================================================
*/

#define CALLSERVICE_NOTFOUND      ((int)0x80000000)
#define MAXMODULELABELLENGTH 64
typedef int (*MIRANDAHOOK)(WPARAM,LPARAM);
typedef INT_PTR (*MIRANDASERVICE)(WPARAM,LPARAM);

#define FUSE_INIT	0			// core started, Param=**FUSE_LINK
#define FUSE_DEINIT 1			// core stopped
#define FUSE_DEFMOD	3			// LoadDefaultModules() return code, Param=*int
#define FUSE_DEATH  4			// DestroyingModularEngine() just got called

typedef struct {
	int cbSize;
	HANDLE (*CreateHookableEvent)(const char *);
	int (*DestroyHookableEvent)(HANDLE);
	int (*NotifyEventHooks)(HANDLE,WPARAM,LPARAM);
	HANDLE (*HookEvent)(const char *,MIRANDAHOOK);
	HANDLE (*HookEventMessage)(const char *,HWND,UINT);
	int (*UnhookEvent)(HANDLE);
	HANDLE (*CreateServiceFunction)(const char *,MIRANDASERVICE);
	HANDLE (*CreateTransientServiceFunction)(const char *,MIRANDASERVICE);
	int (*DestroyServiceFunction)(HANDLE);
	int (*CallService)(const char *,WPARAM,LPARAM);
	int (*ServiceExists)(const char *);					//v0.1.0.1+
	int (*CallServiceSync)(const char*,WPARAM,LPARAM); //v0.1.2.2+
} FUSE_LINK;

#endif
#endif