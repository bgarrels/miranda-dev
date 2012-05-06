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
#include "DBUtils.h"

std::string Quotes_DBGetStringA(HANDLE hContact,const char* szModule,const char* szSetting,const char* pszDefValue /*= NULL*/)
{
	std::string sResult;
	char* pszSymbol = DBGetString(hContact,szModule,szSetting);
	if(NULL != pszSymbol)
	{
		sResult = pszSymbol;
		mir_free(pszSymbol);
	}
	else if(NULL != pszDefValue)
	{
		sResult = pszDefValue;
	}

	return sResult;
}

std::wstring Quotes_DBGetStringW(HANDLE hContact,const char* szModule,const char* szSetting,const wchar_t* pszDefValue/* = NULL*/)
{
	std::wstring sResult;
	wchar_t* pszSymbol = DBGetStringW(hContact,szModule,szSetting);
	if(NULL != pszSymbol)
	{
		sResult = pszSymbol;
		mir_free(pszSymbol);
	}
	else if(NULL != pszDefValue)
	{
		sResult = pszDefValue;
	}

	return sResult;
}

bool Quotes_DBWriteDouble(HANDLE hContact,const char* szModule,const char* szSetting,double dValue)
{
	DBCONTACTWRITESETTING cws = {0};

	cws.szModule = szModule;
	cws.szSetting = szSetting;
	cws.value.type = DBVT_BLOB;
	cws.value.cpbVal = sizeof(dValue);
	cws.value.pbVal = reinterpret_cast<BYTE*>(&dValue);
	return 0 == CallService(MS_DB_CONTACT_WRITESETTING,reinterpret_cast<WPARAM>(hContact),reinterpret_cast<LPARAM>(&cws));
}

bool Quotes_DBReadDouble(HANDLE hContact,const char* szModule,const char* szSetting,double& rdValue)
{
	DBVARIANT dbv = {0};
	DBCONTACTGETSETTING cgs;
	cgs.szModule=szModule;
	cgs.szSetting=szSetting;
	cgs.pValue = &dbv;
	dbv.type = DBVT_BLOB;

	bool bResult = ((0 == CallService(MS_DB_CONTACT_GETSETTING,(WPARAM)hContact,(LPARAM)&cgs))
		&& (DBVT_BLOB == dbv.type));

	if(bResult)
	{
		rdValue = *reinterpret_cast<double*>(dbv.pbVal);
	}

	DBFreeVariant(&dbv);
	return bResult;
}

