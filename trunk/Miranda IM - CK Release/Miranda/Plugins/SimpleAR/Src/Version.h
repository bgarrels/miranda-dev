/*
SimpleAR (based on J. Lawler BaseProtocol) plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2004 Daniel Savi (dss@brturbo.com)
			Copyright (C) 2010-2012 Stark Wong, Mataes, Mikel-Ard-Ri

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

#define __MAJOR_VERSION				2
#define __MINOR_VERSION				0
#define __RELEASE_NUM				2
#define __BUILD_NUM					4

#define __FILEVERSION_STRING		__MAJOR_VERSION,__MINOR_VERSION,__RELEASE_NUM,__BUILD_NUM
#define __FILEVERSION_DOTS			__MAJOR_VERSION.__MINOR_VERSION.__RELEASE_NUM.__BUILD_NUM

#define __STRINGIFY_IMPL(x)			#x
#define __STRINGIFY(x)				__STRINGIFY_IMPL(x)
#define __VERSION_STRING			__STRINGIFY(__FILEVERSION_DOTS)

#ifdef _UNICODE
#if defined(WIN64) || defined(_WIN64)
	#define __PLUGIN_NAME "SimpleAR (Unicode x64)"
#else
	#define __PLUGIN_NAME "SimpleAR (AlfaMaR)"
#endif
#else
	#define __PLUGIN_NAME "SimpleAR"
#endif
#define __INTERNAL_NAME				"Simple Auto Replier"
#define __FILENAME					"SimpleAR.dll"
#define __DESCRIPTION 				"Simple Auto Replier."
#define __AUTHOR					"Stark Wong, Mataes, Mikel-Ard-Ri"
#define __AUTHOREMAIL				"mikelardri@gmail.com"
#define __AUTHORWEB					"https://miranda-dev.googlecode.com/svn/trunk/Miranda%20IM%20-%20CK%20Release/Miranda/Plugins/SimpleAR"
#define __COPYRIGHT					"© 2012"
