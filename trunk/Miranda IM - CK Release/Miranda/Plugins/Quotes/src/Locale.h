#ifndef __11f7afd0_5a66_4029_8bf3_e3c66346b349_Locale_h_
#define __11f7afd0_5a66_4029_8bf3_e3c66346b349_Locale_h_

/*
Quotes (Forex) plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
Copyright (C) Dioksin

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

// std::string GetLocaleInfoString(LCTYPE LCType,LCID Locale = LOCALE_USER_DEFAULT);
const std::locale GetSystemLocale();
tstring Quotes_GetDateFormat(bool bShort);
tstring Quotes_GetTimeFormat(bool bShort);

#endif //__11f7afd0_5a66_4029_8bf3_e3c66346b349_Locale_h_
