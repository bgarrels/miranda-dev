#ifndef __8821d334_afac_439e_9a81_76318e1ac4ef_IconLib_h__
#define __8821d334_afac_439e_9a81_76318e1ac4ef_IconLib_h__

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

#define ICON_STR_MAIN "main"
#define ICON_STR_QUOTE_UP "quote_up"
#define ICON_STR_QUOTE_DOWN "quote_down"
#define ICON_STR_QUOTE_NOT_CHANGED "quote_not_changed"
#define ICON_STR_SECTION "quote_section"
#define ICON_STR_QUOTE "quote"
#define ICON_STR_CURRENCY_CONVERTER "currency_converter"
#define ICON_STR_REFRESH "refresh"
#define ICON_STR_IMPORT "import"
#define ICON_STR_EXPORT "export"
#define ICON_STR_SWAP "swap"

void Quotes_IconsInit();
HICON Quotes_LoadIconEx(const char* name,bool bBig = false);
HANDLE Quotes_GetIconHandle(int iconId);
std::string Quotes_MakeIconName(const char* name);

#endif //__8821d334_afac_439e_9a81_76318e1ac4ef_IconLib_h__
