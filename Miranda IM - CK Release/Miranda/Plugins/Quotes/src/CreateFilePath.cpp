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

#include "StdAfx.h"
#include "CreateFilePath.h"

#include <sstream>
#include "ModuleInfo.h"

namespace
{
	TCHAR replace_invalid_symbol(TCHAR chr)
	{
		TCHAR InvaliSymbols[] = {_T('\\'),_T('/'),_T(':'),_T('*'),_T('?'),_T('"'),_T('<'),_T('>'),_T('|')};
		for(int i = 0; i < sizeof(InvaliSymbols)/sizeof(InvaliSymbols[0]);++i)
		{
			if(chr == InvaliSymbols[i])
			{
				return _T('_');
			}
		}

		return chr;
	}

	void prepare_name(tstring& rsName)
	{
		std::transform(rsName.begin(),rsName.end(),rsName.begin(),boost::bind(replace_invalid_symbol,_1));
	}
}

tstring CreateFilePath(const tstring& rsName)
{
	TCHAR szPath[_MAX_PATH];
	::GetModuleFileName(CModuleInfo::GetModuleHandle(),szPath,_MAX_PATH);

	TCHAR* p = _tcsrchr(szPath,_T('\\'));
	if(p)
	{
		*p = 0;
	}

	tstring s(rsName);
	prepare_name(s);
	tostringstream o;
	o << szPath << _T("\\Quotes\\") << s;
	return o.str();
}