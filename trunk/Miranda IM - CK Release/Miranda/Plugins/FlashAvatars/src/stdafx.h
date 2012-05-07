/*
Flash Avatar support plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	2001-2007 Big Muscle
							2001-2006 Jacek Sieka


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

#pragma once
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define BZ_NO_STDIO

#ifdef _WIN32
# define _WIN32_WINNT 0x0600
# define _WIN32_IE	0x0601
#endif

#if defined(UNICODE) && !defined(_UNICODE)
# define _UNICODE
#endif

#ifdef _DEBUG
# define _STLP_DEBUG 1
#endif

#define _STLP_DONT_USE_SHORT_STRING_OPTIM 1	// Uses small string buffer, so it saves memory for a lot of strings
#define _STLP_USE_PTR_SPECIALIZATIONS 1		// Reduces some code bloat
#define _STLP_USE_TEMPLATE_EXPRESSION 1		// Speeds up string concatenation
#define _STLP_NO_ANACHRONISMS 1

#include <windows.h>
#include <stdio.h>


#ifdef _DEBUG

inline void _cdecl debugTrace(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	
	char buf[512];
	
	_vsnprintf(buf, sizeof(buf), format, args);
	OutputDebugStringA(buf);
	va_end(args);
}

# define debug debugTrace
#define assert(exp) \
do { if (!(exp)) { \
	debug("Assertion hit in %s(%d): " #exp "\n", __FILE__, __LINE__); \
	if(1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, #exp)) \
_CrtDbgBreak(); } } while(false)

#else
# define debug
# define assert
#endif


