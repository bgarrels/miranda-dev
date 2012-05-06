#ifndef __VERSION_H_INCLUDED
#define __VERSION_H_INCLUDED

/*
YAPP for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
            Copyright (C) 2005-2006 Scott Ellis

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
#define __MINOR_VERSION				5
#define __RELEASE_NUM				0
#define __BUILD_NUM					10

#define __FILEVERSION_STRING		__MAJOR_VERSION,__MINOR_VERSION,__RELEASE_NUM,__BUILD_NUM
#define __FILEVERSION_STRING_DOTS	__MAJOR_VERSION.__MINOR_VERSION.__RELEASE_NUM.__BUILD_NUM
#define __STRINGIFY(x)			#x
#define __VERSION_STRING		__STRINGIFY(__FILEVERSION_STRING_DOTS)

#define __DESC				"Yet Another Popup Plugin - Provides popup notification window services (unicode and ansi) to other plugins"
#define __AUTHOR			"Scott Ellis"
#define __AUTHOREMAIL		"mail@scottellis.com.au"
#define __COPYRIGHT			"© 2005-2012 Scott Ellis"
#define __AUTHORWEB			"http://www.scottellis.com.au"

#ifdef _WIN64
#define __PLUGIN_NAME			"YAPP x64"
#else
#define __PLUGIN_NAME			"YAPP"
#endif

#define __FILENAME			"yapp.dll"

#endif //__VERSION_H_INCLUDED
