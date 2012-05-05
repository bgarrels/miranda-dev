/*
UserinfoEx plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2006-2010 DeathAxe, Yasnovidyashii, Merlin_de, K. Romanov, Kreol

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

#define __MAJOR_VERSION   0
#define __MINOR_VERSION   8
#define __RELEASE_NUM     4	// due to beta builders
#define __BUILD_NUM       2	// due to beta builders

#define __STRINGIFY_(x) #x
#define __STRINGIFY(x)  __STRINGIFY_(x)

#define __FILEVERSION_STRING      __MAJOR_VERSION,__MINOR_VERSION,__RELEASE_NUM,__BUILD_NUM
#define __FILEVERSION_STRING_DOTS __MAJOR_VERSION.__MINOR_VERSION.__RELEASE_NUM.__BUILD_NUM

#define __VERSION_STRING  __STRINGIFY(__FILEVERSION_STRING_DOTS)
#define __VERSION_DWORD   PLUGIN_MAKE_VERSION(__MAJOR_VERSION, __MINOR_VERSION, __RELEASE_NUM, __BUILD_NUM)

#define __SHORT_DESC  "Extended UserInfo module for Miranda-IM. Provides interface to edit all contact information."
#define __DESC        "Gives extended ability to edit information about your contacts locally. "\
                      "It does not matter what information your contact gives about himself. "\
                      "If you know more you can add more."
#define __AUTHOR      "DeathAxe, Yasnovidyashii, Merlin, K. Romanov, Kreol"
#define __AUTHOREMAIL "deathaxe@web.de"
#define __COPYRIGHT   "� 2006-2012 DeathAxe, Yasnovidyashii, Merlin, K. Romanov, Kreol"
#define __AUTHORWEB   "http://addons.miranda-im.org/details.php?action=viewfile&id=" __STRINGIFY(__UPDATER_DOWNLOAD_ID)

#ifdef _WIN64
 #ifdef _UNICODE
  #define __UPDATER_DOWNLOAD_ID 3333
  #define __PLUGIN_DISPLAY_NAME "UserInfoEx x64 (Unicode)"
  #define __PLUGIN_FILENAME     "uinfoexw.dll"
 #endif
#else
 #ifdef _UNICODE
  #define __UPDATER_DOWNLOAD_ID 2537
  #define __PLUGIN_DISPLAY_NAME	"UserInfoEx (Unicode)"	//ensure plugin shortName matches file listing! <title>UserInfoEx (Unicode)</title>
  #define __PLUGIN_FILENAME     "uinfoexw.dll"
 #endif
#endif
