#ifndef __MIR_MEMORY_H__
#define __MIR_MEMORY_H__

/*
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2009-2010 Ricardo Pescuma Domenecci

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



static BOOL mir_is_unicode()
{
	static int is_unicode = -1;
	if (is_unicode == -1)
	{
		char ver[1024];
		CallService(MS_SYSTEM_GETVERSIONTEXT, (WPARAM) sizeof(ver), (LPARAM) ver);
		is_unicode = (strstr(ver, "Unicode") != NULL ? 1 : 0);
	}
	return is_unicode;
}


static void * mir_alloc0(size_t size) 
{
	void * ptr = mir_alloc(size);

	if (ptr != NULL)
		memset(ptr, 0, size);

	return ptr;
}

static int strcmpnull(char *str1, char *str2)
{
	if ( str1 == NULL && str2 == NULL )
		return 0;
	if ( str1 != NULL && str2 == NULL )
		return 1;
	if ( str1 == NULL && str2 != NULL )
		return -1;

	return strcmp(str1, str2);
}

static int strcmpnullW(WCHAR *str1, WCHAR *str2)
{
	if ( str1 == NULL && str2 == NULL )
		return 0;
	if ( str1 != NULL && str2 == NULL )
		return 1;
	if ( str1 == NULL && str2 != NULL )
		return -1;

	return lstrcmpW(str1, str2);
}


#ifdef UNICODE

#define CHECK_VERSION(_NAME_)																\
	if (!mir_is_unicode())																	\
	{																						\
		MessageBox(NULL, _T("Your Miranda is ansi. You have to install ansi ") _T(_NAME_),	\
						_T(_NAME_), MB_OK | MB_ICONERROR);									\
		return -1;																			\
	}

# define lstrcmpnull strcmpnullW

#define INPLACE_CHAR_TO_TCHAR(_new_var_, _size_, _old_var_)									\
	TCHAR _new_var_[_size_];																\
	MultiByteToWideChar(CP_ACP, 0, _old_var_, -1, _new_var_, _size_)


#define INPLACE_TCHAR_TO_CHAR(_new_var_, _size_, _old_var_)									\
	char _new_var_[_size_];																	\
	WideCharToMultiByte(CP_ACP, 0, _old_var_, -1, _new_var_, _size_, NULL, NULL);

#else

#define CHECK_VERSION(_NAME_)																\
	if (mir_is_unicode())																	\
	{																						\
		MessageBox(NULL, _T("Your Miranda is unicode. You have to install unicode ") _T(_NAME_),	\
						_T(_NAME_), MB_OK | MB_ICONERROR);									\
		return -1;																			\
	}

# define lstrcmpnull strcmpnull

#define INPLACE_CHAR_TO_TCHAR(_new_var_, _size_, _old_var_)									\
	TCHAR *_new_var_ = _old_var_

#define INPLACE_TCHAR_TO_CHAR(_new_var_, _size_, _old_var_)									\
	char *_new_var_ = _old_var_;

#endif



// Free memory and set to NULL
#define MIR_FREE(_x_) if (_x_ != NULL) { mir_free(_x_); _x_ = NULL; }



#endif // __MIR_MEMORY_H__
