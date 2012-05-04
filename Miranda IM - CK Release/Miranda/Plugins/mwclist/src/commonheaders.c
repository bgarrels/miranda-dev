/*
Author Artem Shpynov aka FYR

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

#include "commonheaders.h"

#define SAFE_PTR(a) a?(IsBadReadPtr(a,1)?a=NULL:a):a

int __cdecl MyStrCmp (const char *a, const char *b)
{
	SAFE_PTR(a);
	SAFE_PTR(b);
	if (!(a&&b)) return a!=b;
	return (strcmp(a,b));
}

DWORD exceptFunction(LPEXCEPTION_POINTERS EP)
{
    //printf("1 ");                     // printed first
	char buf[4096];


	sprintf(buf,"\r\nExceptCode: %x\r\nExceptFlags: %x\r\nExceptAddress: %p\r\n",
		EP->ExceptionRecord->ExceptionCode,
		EP->ExceptionRecord->ExceptionFlags,
		EP->ExceptionRecord->ExceptionAddress
		);
	OutputDebugStringA(buf);
	MessageBoxA(0,buf,"clist_mw Exception",0);


	return EXCEPTION_EXECUTE_HANDLER;
}
