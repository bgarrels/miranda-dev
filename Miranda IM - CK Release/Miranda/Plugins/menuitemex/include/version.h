/*
MenuEx plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
				Copyright (C) 2001-03 Heiko Schillinger
				Copyright (C) 2006-08 Baloo
				Copyright (C) 2009-10 Billy_Bons

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

#define __FILEVERSION_STRING        1,3,0,8
#define __VERSION_STRING            "1.3.0.8"
#define __VERSION_DWORD             0x01030008

#if defined (_WIN64)
#define __PLUGIN_NAME "MenuItemEx (x64) plugin for Miranda IM"
#define __PLUGIN_ID 4304
#define __PLUGIN_ID_STR "4304"
#elif  (_UNICODE)
#define __PLUGIN_NAME "MenuItemEx (Unicode) plugin for Miranda IM"
#define __PLUGIN_ID 4036
#define __PLUGIN_ID_STR "4036"
#endif

#define __WEB "http://addons.miranda-im.org/details.php?action=viewfile&id="

#define __DESC "Adds some useful options to a contacts menu."
#define __AUTHORS "Heiko Schillinger, Baloo, Billy_Bons"
#define __EMAIL "micron@nexgo.de, baloo@bk.ru, tatarinov.sergey@gmail.com"
#define __COPYRIGHTS "© 2001-03 Heiko Schillinger, © 2006-08 Baloo, © 2009-10 Billy_Bons"

#ifdef _UNICODE
// {b1902a52-9114-4d7e-ac2e-b3a52e01d574}
#define MIID_MENUEX		{0xb1902a52, 0x9114, 0x4d7e, { 0xac, 0x2e, 0xb3, 0xa5, 0x2e, 0x01, 0xd5, 0x74 }}
#else
// {0C9B8DDC-4894-4b28-ADF2-FAC071BBBD53}
#define MIID_MENUEX		{0xc9b8ddc, 0x4894, 0x4b28, { 0xad, 0xf2, 0xfa, 0xc0, 0x71, 0xbb, 0xbd, 0x53 }}
#endif
