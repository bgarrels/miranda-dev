#ifndef __FBB8ABCA_315E_4ace_B2EB_51E03AD6F8D1_Base64_h__
#define __FBB8ABCA_315E_4ace_B2EB_51E03AD6F8D1_Base64_h__

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

#pragma once

namespace base64
{
	bool encode(const BYTE* in, size_t inlen,std::vector<char>& out);
	bool decode(const char* in, size_t inlen,std::vector<BYTE>& out);
}

#endif //__FBB8ABCA_315E_4ace_B2EB_51E03AD6F8D1_Base64_h__
