/*
db3xs (secure) database driver for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright (C) sje, Piotr Pawluczuk, a0x, Billy_Bons

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

#define __FILE_VERSION 0,9,1,3
#define __FILE_VERSION_STR "0, 9, 1, 3"
#define __FILE_VERSION_DWORD 0x00090103

#if defined ( _WIN64 )
#define __MODULE_NAME "Miranda Secure database driver (x64)"
#elif defined ( _UNICODE )
#define __MODULE_NAME "Miranda Secure database driver (Unicode)"
#endif

#define __PLUGIN_NAME "Miranda Secure database driver"
#define __DESC "Provides Miranda database support: global settings, contacts, history, settings per contact."
#define __AUTHORS "Miranda-IM project, Billy_Bons"
#define __EMAIL "tatarinov.sergey@gmail.com"
#define __COPYRIGHTS "(C) 2000-2012 Miranda IM project, sje, Piotr Pawluczuk, a0x, Billy_Bons"
