#ifndef __MMI_H__
#define __MMI_H__

/*
SecureIM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 	2003 Johell
							2005-2009 Baloo

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

char *m_wwstrcat(LPCSTR,LPCSTR);
char *m_awstrcat(LPCSTR,LPCSTR);
char *m_aastrcat(LPCSTR,LPCSTR);
char *m_ustrcat(LPCSTR,LPCSTR);
LPSTR to_hex(PBYTE,int);

LPSTR utf8_to_miranda(LPCSTR,DWORD&);
LPSTR miranda_to_utf8(LPCSTR,DWORD);

void *operator new(size_t sz);
void operator delete(void *p);
void *operator new[](size_t size);
void operator delete[](void * p);

#define SAFE_INIT(t,p)			t p=NULL;
#define SAFE_FREE(p)			safe_free((void **)&(p));
#define SAFE_DELETE(p)			safe_delete((void **)&(p));

void __fastcall safe_free(void** p);
void __fastcall safe_delete(void** p);

#endif
