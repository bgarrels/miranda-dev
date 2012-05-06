/*
FltContacts plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Authors
Copyright (C) Ranger (fonts and colors settings)
Copyright (C) Iavor Vajarov (ivajarov@code.bg)
Copyright (C) 2008 Kosh&chka


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

#define BUILD_NUM 2
#define BUILD_NUM_STR  "3"
#define REVISION  "$Revision$"

#define COREVERSION_NUM 1, 0, 3,
#define COREVERSION_NUM_STR  "1, 0, 3"

#define MINIMAL_COREVERSION 0, 8, 0, 0
#define MINIMAL_COREVERSION_STR "0, 8, 0, 0"

#ifdef UNICODE
#define UNICODE_AWARE_STR " (Unicode)"
#endif

#define FILE_VERSION	COREVERSION_NUM BUILD_NUM
#define FILE_VERSION_STR COREVERSION_NUM_STR  UNICODE_AWARE_STR " build " BUILD_NUM_STR	" " REVISION 

#define PRODUCT_VERSION	FILE_VERSION
#define PRODUCT_VERSION_STR	FILE_VERSION_STR

#define __PLUGIN_NAME				"Floating Contacts" UNICODE_AWARE_STR
#define __FILENAME					"FltContacts.dll"
#define __DESC						"Floating Contacts plugin for Miranda IM."
#define __AUTHOR					"Iavor Vajarov, Kosh&chka, Victor Pavlychko"
#define __AUTHOREMAIL				"ell-6@ya.ru"
#define __AUTHORWEB					"http://www.miranda-im.org/"
#define __COPYRIGHT					"© 2002-2004 I. Vajarov (ivajarov@code.bg), © 2008 Kosh&chka, V. Pavlychko"
