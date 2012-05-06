#ifndef __DEBUG_H
#define __DEBUG_H

/*
YAMN plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2002-2004 majvan
			Copyright (C) 2005-2007 tweety y_b

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

//#define YAMN_VER_BETA
//#define YAMN_VER_BETA_CRASHONLY

#ifdef YAMN_DEBUG

//#pragma comment(lib, "th32.lib")

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0601 
#endif
#define VC_EXTRALEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <shlwapi.h>

//#define DEBUG_SYNCHRO				//debug synchro to a file
//#define DEBUG_COMM				//debug communiation to a file
//#define DEBUG_DECODE				//debug header decoding to a file
//#define DEBUG_DECODECODEPAGE		//add info about codepage used in conversion
//#define DEBUG_DECODEBASE64		//add info about base64 result
//#define DEBUG_DECODEQUOTED		//add info about quoted printable result
//#define DEBUG_FILEREAD			//debug file reading to message boxes
//#define DEBUG_FILEREADMESSAGES	//debug file reading messages to message boxes

void DebugLog(HANDLE,const char *fmt,...);
void DebugLogW(HANDLE File,const WCHAR *fmt,...);

#ifdef DEBUG_SYNCHRO
// Used for synchronization debug
extern HANDLE SynchroFile;
#endif

#ifdef DEBUG_COMM
// Used for communication debug
extern HANDLE CommFile;
#endif

#ifdef DEBUG_DECODE
// Used for decoding debug
extern HANDLE DecodeFile;
#endif

#if defined(DEBUG_FILEREAD) || defined(DEBUG_FILEREADMESSAGES)
DWORD ReadStringFromMemory(TCHAR **Parser,TCHAR *End,TCHAR **StoreTo,TCHAR *DebugString);
	#ifndef UNICODE
DWORD ReadStringFromMemoryW(TCHAR **Parser,TCHAR *End,TCHAR **StoreTo,TCHAR *DebugString);
	#else
#define ReadStringFromMemoryW	ReadStringFromMemory
	#endif
#endif

//#ifdef DEBUG_ACCOUNTS
//int GetAccounts();
//void WriteAccounts();
//#endif

#endif	//YAMN_DEBUG
#endif	//_DEBUG_H
