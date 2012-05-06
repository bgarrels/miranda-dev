/*
Splash Screen Plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
Copyright (C) 2004-2007 nullbie
Copyright (C) 2005-2007 Thief

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
#define __RELEASE_NUM				2
#define __BUILD_NUM					3

#define __FILEVERSION_STRING		__MAJOR_VERSION,__MINOR_VERSION,__RELEASE_NUM,__BUILD_NUM
#define __FILEVERSION_DOTS			__MAJOR_VERSION.__MINOR_VERSION.__RELEASE_NUM.__BUILD_NUM

#define __STRINGIFY_IMPL(x)			#x
#define __STRINGIFY(x)				__STRINGIFY_IMPL(x)
#define __VERSION_STRING			__STRINGIFY(__FILEVERSION_DOTS)

#ifdef _UNICODE
#if defined(WIN64) || defined(_WIN64)
	#define __PLUGIN_NAME "Splash Screen (Unicode x64)"
#else
	#define __PLUGIN_NAME "Splash Screen (Unicode)"
#endif

#endif
#define __INTERNAL_NAME				"AdvSplashScreen"
#define __FILENAME					"AdvSplashScreen.dll"
#define __DESCRIPTION 				"Shows a splash at Miranda IM startup."
#define __AUTHOR					"nullbie, Thief"
#define __AUTHOREMAIL				"thief@miranda.im"
#define __AUTHORWEB					"http://addons.miranda-im.org/details.php?id=2624"
#define __COPYRIGHT					"© 2004-2012 Victor Pavlychko, 2005-2011 Alexander Turyak"
/* C64CC8E0-CF03-474A-8B11-8BD4565CCF04 */
#define MIID_SPLASHSCREEN {0xc64cc8e0, 0xcf03, 0x474a, {0x8b, 0x11, 0x8b, 0xd4, 0x56, 0x5c, 0xcf, 0x04}}
