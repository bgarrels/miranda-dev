/*
Sessions Management plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2007-2008 MaD_CLuSTeR

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

#define __MAJOR_VERSION				0
#define __MINOR_VERSION				1
#define __RELEASE_NUM				3
#define __BUILD_NUM					0

#define __FILEVERSION_STRING		__MAJOR_VERSION,__MINOR_VERSION,__RELEASE_NUM,__BUILD_NUM
#define __FILEVERSION_DOTS			__MAJOR_VERSION.__MINOR_VERSION.__RELEASE_NUM.__BUILD_NUM

#define __STRINGIFY_IMPL(x)			#x
#define __STRINGIFY(x)				__STRINGIFY_IMPL(x)
#define __VERSION_STRING			__STRINGIFY(__FILEVERSION_DOTS)

#ifdef _UNICODE
#if defined(WIN64) || defined(_WIN64)
	#define __PLUGIN_NAME "Sessions (Unicode x64)"
#else
	#define __PLUGIN_NAME "Sessions (Unicode)"
#endif

#endif
#define __INTERNAL_NAME				"Sessions"
#define __FILENAME					"Sessions.dll"
#define __DESCRIPTION 				"Sessions Manager for Miranda IM."
#define __AUTHOR					"MaD_CLuSTeR"
#define __AUTHOREMAIL				"daniok@yandex.ru"
#define __AUTHORWEB					"http://miranda.radicaled.ru/index.php?plugin=2"
#define __COPYRIGHT					"© 2007-2012 Danil Mozhar"
