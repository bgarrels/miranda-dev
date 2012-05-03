/*
Skype protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright © 2008-2012 leecher, tweety, jls17 

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

//#define DEBUG_RELEASE 1
#ifdef DEBUG_RELEASE
  #define _DEBUG 1
#endif

#ifdef _DEBUG
	void init_debug(void);
	void end_debug (void);
	void do_log(const char *pszFormat, ...);
	#define DEBUG_OUT(a) OUTPUT(a)
	#define TRACE(a) OutputDebugString(a)
	#define TRACEA(a) OutputDebugStringA(a)
	#define TRACEW(a) OutputDebugStringW(a)
	#define LOG(a) do_log a
#else
	#define DEBUG_OUT(a) 
	#define LOG(a)
	#define TRACE(a)
	#define TRACEA(a)
	#define TRACEW(a)
#endif

