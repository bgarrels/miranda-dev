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
#include "Locale.h"

const std::locale GetSystemLocale()
{
	return std::locale("");
}

namespace
{
	tstring get_int_registry_value(LPCTSTR pszValueName)
	{
		tstring sResult;
		HKEY hKey = NULL;
		LONG lResult = ::RegOpenKeyEx(HKEY_CURRENT_USER,
			_T("Control Panel\\International"),0,KEY_QUERY_VALUE,&hKey);
		if((ERROR_SUCCESS == lResult) && (NULL != hKey))
		{
			DWORD dwType = 0;
			DWORD dwSize = 0;
			lResult = ::RegQueryValueEx(hKey,pszValueName,nullptr,&dwType,nullptr,&dwSize);
			if((ERROR_SUCCESS == lResult) && ((REG_SZ == dwType) || (REG_EXPAND_SZ == dwType)))
			{
				std::vector<TCHAR> aBuffer(dwSize);
				lResult = ::RegQueryValueEx(hKey,pszValueName,nullptr,nullptr,reinterpret_cast<LPBYTE>(&*aBuffer.begin()),&dwSize);
				if(ERROR_SUCCESS == lResult)
				{
					std::copy(aBuffer.begin(),aBuffer.end(),std::back_inserter(sResult));
				}
			}
		}

		if(NULL != hKey)
		{
			lResult = ::RegCloseKey(hKey);
			assert(ERROR_SUCCESS == lResult);
		}

		return sResult;
	}

	tstring date_win_2_boost(const tstring& sFrmt)
	{
		tstring sResult(_T("%d.%m.%y"));
		if(sFrmt == _T("dd/MM/yy"))
		{
			sResult = _T("%d/%m/%y");
		}
		else if(sFrmt == _T("yyyy-MM-dd"))
		{
			sResult = _T("%y-%m-%d");
		}
		return sResult;
	}

	tstring time_win_2_boost(const tstring& sFrmt)
	{
		tstring sResult = _T("%H:%M:%S");
		if(sFrmt == _T("H:mm") || sFrmt == _T("HH:mm"))
		{
			sResult = _T("%H:%M");
		}
		return sResult;
	}
}

tstring Quotes_GetDateFormat(bool bShort)
{
	return date_win_2_boost(get_int_registry_value(bShort ? _T("sShortDate") : _T("sLongDate")));
}

tstring Quotes_GetTimeFormat(bool bShort)
{
	return time_win_2_boost(get_int_registry_value(bShort ? _T("sShortTime") : _T("sTimeFormat")));
}