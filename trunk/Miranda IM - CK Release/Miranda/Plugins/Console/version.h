/*

Miranda IM: the free IM client for Microsoft* Windows*

Copyright 2000-2008 Miranda ICQ/IM project,
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
*/


#define __FILEVERSION_STRING 0,0,7,2
#define __VERSION_STRING     "0.0.7.2"
#define __VERSION_DWORD      0x00000702




#define __PLUGIN_DESC        "Console log window for Miranda 0.8.0.3+"
#define __PLUGIN_LONGDESC    __PLUGIN_DESC ". Compiled by Bio. Enjoy! 8-)          [ Built "__DATE__"  "__TIME__" ]"
#define __PLUGIN_AUTHOR      "Bio"
#define __PLUGIN_EMAIL       "bio@msx.ru"
#define __PLUGIN_RIGHTS      "� 2005-2009 Bio"
#define __PLUGIN_FILE        "Console.dll"
#define __PLUGIN_SHORTNAME   "Console"


#ifdef _UNICODE
	// {23D4F302-D513-45b7-9027-445F29557311}
	#define MIID_CONSOLE { 0x23d4f302, 0xd513, 0x45b7, { 0x90, 0x27, 0x44, 0x5f, 0x29, 0x55, 0x73, 0x11 }}
	#define __PLUGIN_NAME __PLUGIN_SHORTNAME " (Unicode)"
	#define __PLUGIN_URL "http://addons.miranda-im.org/details.php?action=viewfile&id=3791"
#endif
