/*
Author Pescuma

Miranda IM: the free IM client for Microsoft* Windows*

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

===============================================================================
*/

#pragma once

#ifndef __LOG_H__
# define __LOG_H__

#ifdef _DEBUG_LOG

void Log(const char *file,int line,const char *fmt,...);
#define logg()  Log(__FILE__,__LINE__,"")
#define log0(s)  Log(__FILE__,__LINE__,s)
#define log1(s,a)  Log(__FILE__,__LINE__,s,a)
#define log2(s,a,b)  Log(__FILE__,__LINE__,s,a,b)
#define log3(s,a,b,c)  Log(__FILE__,__LINE__,s,a,b,c)
#define log4(s,a,b,c,d)  Log(__FILE__,__LINE__,s,a,b,c,d)

#else

#define logg()
#define log0(s)
#define log1(s,a)
#define log2(s,a,b)
#define log3(s,a,b,c)
#define log4(s,a,b,c,d)

#endif

#endif // __LOG_H__