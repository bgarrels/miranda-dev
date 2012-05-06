#ifndef __SVCS_RSA_H__
#define __SVCS_RSA_H__

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

#include <windows.h>

extern pRSA_EXPORT exp;
extern RSA_IMPORT imp;
extern BOOL rsa_4096;

int __cdecl rsa_inject(HANDLE,LPCSTR);
int __cdecl rsa_check_pub(HANDLE,PBYTE,int,PBYTE,int);
void __cdecl rsa_notify(HANDLE,int);

unsigned __stdcall sttGenerateRSA(LPVOID);
BYTE loadRSAkey(pUinKey);
void createRSAcntx(pUinKey);
void resetRSAcntx(pUinKey);
void deleteRSAcntx(pUinKey);

#endif
