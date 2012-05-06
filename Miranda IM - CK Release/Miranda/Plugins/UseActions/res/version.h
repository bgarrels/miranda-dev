/*
UseActions plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2009-2012 2012 Igor Abakumov

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


// plugin version part
#define __MAJOR_VERSION				0
#define __MINOR_VERSION				0
#define __RELEASE_NUM				0
#define __BUILD_NUM					4
// plugin SVN Revision number
//#define __REV_NUM					5678

// minimal MirandaIM version number, with which this plugin supposed to work
#define __PROD_MAJOR_VERSION		0
#define __PROD_MINOR_VERSION		7
#define __PROD_RELEASE_NUM			13
#define __PROD_BUILD_NUM			0
// if your plugin works only with Miranda core beginning from specific revision, you can include this information in Product Version resource
//#define __PROD_REV_NUM				1234

// stuff that will be used in PluginInfo section and in Version resource
#if defined (_UNICODE)
#define __PLUGIN_NAME				"UseActions (Unicode)"
#else
#define __PLUGIN_NAME				"UseActions"
#endif
#define __FILENAME					"UseActions.dll"
#define __DESC						"This plugin makes use of actions, serviced by Actions Manager plugin (ActMan by Awkward)."
#define __AUTHOR					"Igor Abakumov"
#define __AUTHOREMAIL				"abakumoff@mail.ru"
#define __AUTHORWEB					"http://vasilich.org/blog"
#define __COPYRIGHT					"© 2009-2012 Igor Abakumov"

// other stuff for Version resource
#define __STRINGIFY_IMPL(x)			#x
#define __STRINGIFY(x)				__STRINGIFY_IMPL(x)

#define __FILEVERSION_STRING        __MAJOR_VERSION,__MINOR_VERSION,__RELEASE_NUM,__BUILD_NUM
#define __FILEVERSION_STRING_DOTS	__MAJOR_VERSION.__MINOR_VERSION.__RELEASE_NUM.__BUILD_NUM
#ifdef _UNICODE
#ifdef __REV_NUM
#define __VERSION_STRING			__STRINGIFY(__FILEVERSION_STRING_DOTS rev. __REV_NUM Unicode)
#else
#define __VERSION_STRING			__STRINGIFY(__FILEVERSION_STRING_DOTS Unicode)
#endif
#else
#ifdef __REV_NUM
#define __VERSION_STRING			__STRINGIFY(__FILEVERSION_STRING_DOTS rev. __REV_NUM)
#else
#define __VERSION_STRING			__STRINGIFY(__FILEVERSION_STRING_DOTS)
#endif
#endif

#define __PRODVERSION_STRING        __PROD_MAJOR_VERSION,__PROD_MINOR_VERSION,__PROD_RELEASE_NUM,__PROD_BUILD_NUM
#define __PRODVERSION_STRING_DOTS	__PROD_MAJOR_VERSION.__PROD_MINOR_VERSION.__PROD_RELEASE_NUM.__PROD_BUILD_NUM
#ifdef __PROD_REV_NUM
#define __PROD_VERSION_STRING		__STRINGIFY(__PRODVERSION_STRING_DOTS rev. __PROD_REV_NUM)
#else
#define __PROD_VERSION_STRING		__STRINGIFY(__PRODVERSION_STRING_DOTS+)
#endif
