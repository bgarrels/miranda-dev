#ifndef __RTFCONV_H__
#define __RTFCONV_H__

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

#ifndef _INTPTR_T_DEFINED
#define intptr_t int
#endif

#define CONVMODE_USE_SYSTEM_TABLE         0x800000    /* Use OS's table only */

typedef intptr_t (WINAPI *RTFCONVSTRING) ( const void *pSrcBuffer, void *pDstBuffer,
    int nSrcCodePage, int nDstCodePage, unsigned long dwFlags,
    size_t nMaxLen );

extern RTFCONVSTRING pRtfconvString;

BOOL load_rtfconv ();
void free_rtfconv ();
void rtfconvA(LPCSTR rtf, LPWSTR plain);
void rtfconvW(LPCWSTR rtf, LPWSTR plain);

#endif
