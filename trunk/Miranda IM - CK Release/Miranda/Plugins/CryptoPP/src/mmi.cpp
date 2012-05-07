/*
Crypto++ plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
Copyright (C) 2006-2009 Baloo

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

#include "commonheaders.h"

/*
void m_check(void *ptr, const char *module) {
    if(ptr==NULL) {
    	char buffer[128];
    	strcpy(buffer,module); strcat(buffer,": NULL pointer detected !");
	MessageBoxA(0,buffer,szModuleName,MB_OK|MB_ICONSTOP);
	__asm{ int 3 };
	exit(1);
    }
}


void *m_alloc(size_t size) {
	void *ptr;
	ptr = malloc(size);
	m_check(ptr,"m_alloc");
   	ZeroMemory(ptr,size);
	return ptr;
}


void m_free(void *ptr) {
//    m_check(ptr,"m_free");
    if(ptr) {
	free(ptr);
    }
}


void *m_realloc(void *ptr,size_t size) {
    r = realloc(ptr,size);
    m_check(ptr,"m_realloc");
	return ptr;
}


#ifndef _DEBUG
void *operator new(size_t size) {
	return malloc(size);
}
#endif


void operator delete(void *p) {
	free(p);
}


void *operator new[](size_t size) {
	return operator new(size);
}


void operator delete[](void *p) {
	operator delete(p);
}


char *m_strdup(const char *str) {
    if(str==NULL) return NULL;
    int len = (int)strlen(str)+1;
	char *dup = (char*) m_alloc(len);
	MoveMemory((void*)dup,(void*)str,len);
	return dup;
}
*/

void __fastcall safe_free(void** p)
{
  if (*p) {
    free(*p);
    *p = NULL;
  }
}


void __fastcall safe_delete(void** p)
{
  if (*p) {
    delete(*p);
    *p = NULL;
  }
}


// EOF
