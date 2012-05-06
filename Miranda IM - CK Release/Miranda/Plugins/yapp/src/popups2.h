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

* The following ifdef block is the standard way of creating macros which make exporting 
* from a DLL simpler. All files within this DLL are compiled with the POPUPS2_EXPORTS
* symbol defined on the command line. this symbol should not be defined on any project
* that uses this DLL. This way any other project whose source files include this file see 
* POPUPS2_API functions as being imported from a DLL, whereas this DLL sees symbols
* defined with this macro as being exported.
*/


#ifdef POPUPS2_EXPORTS
#define POPUPS2_API __declspec(dllexport)
#else
#define POPUPS2_API __declspec(dllimport)
#endif

