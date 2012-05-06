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
#include "Log.h"
#include "LightMutex.h"
#include "EconomicRateInfo.h"
#include "CreateFilePath.h"

namespace
{
	CLightMutex g_Mutex;

	tstring get_log_file_name()
	{
		return CreateFilePath(_T("Quotes.log"));
	}

	bool is_log_enabled()
	{
#ifdef _DEBUG
		return true;
#else
		return (1 == DBGetContactSettingByte(NULL,QUOTES_PROTOCOL_NAME,DB_STR_ENABLE_LOG,false));
#endif
	}

	void do_log(const tstring& rsFileName,ESeverity nSeverity,const tstring& rsMsg)
	{
		CGuard<CLightMutex> guard(g_Mutex);
		tofstream file(rsFileName.c_str(),std::ios::ate|std::ios::app);
		if(file.good())
		{
			TCHAR szTime[20];
// 			TCHAR sz[10000+1];
			_tstrtime_s(szTime);
			file << szTime << _T(" ================================>\n") << rsMsg << _T("\n\n");
			
// 			size_t cBytes = rsMsg.size();
// 			const TCHAR* p = rsMsg.c_str();
// 			for(size_t c = 0;c < cBytes;c += 10000,p+=10000)
// 			{
// 				_tcsncpy_s(sz,p,10000);
// 				file << sz;
// 			}
// 			
// 			file << "\n\n";
		}
	}
}

void LogIt(ESeverity nSeverity,const tstring& rsMsg)
{
	if(is_log_enabled())
	{
		tstring sFileName = get_log_file_name();
		do_log(sFileName,nSeverity,rsMsg);
	}
}
