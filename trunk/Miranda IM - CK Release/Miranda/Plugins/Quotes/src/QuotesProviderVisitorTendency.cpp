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

#include "stdafx.h"
#include "QuotesProviderVisitorTendency.h"
#include "DBUtils.h"
#include "QuotesProviderGoogleFinance.h"
#include "EconomicRateInfo.h"
#include "QuotesProviderYahoo.h"

CQuotesProviderVisitorTendency::CQuotesProviderVisitorTendency(HANDLE hContact,TCHAR chr)
	: m_hContact(hContact),m_chr(chr),m_bValid(false),m_dResult(0.0)
{
}

void CQuotesProviderVisitorTendency::Visit(const CQuotesProviderBase& rProvider)
{
	switch(m_chr)
	{
	case _T('r'):
	case _T('R'):
		GetValue(DB_STR_QUOTE_CURR_VALUE);
		break;
	case _T('p'):
		GetValue(DB_STR_QUOTE_PREV_VALUE);
		break;
	}
}

void CQuotesProviderVisitorTendency::Visit(const CQuotesProviderGoogleFinance& rProvider)
{
	switch(m_chr)
	{
	case _T('o'):
		GetValue(DB_STR_GOOGLE_FINANCE_OPEN_VALUE);
		break;
	case _T('d'):
		GetValue(DB_STR_GOOGLE_FINANCE_DIFF);
		break;
	case _T('y'):
		GetValue(DB_STR_GOOGLE_FINANCE_PERCENT_CHANGE_TO_YERSTERDAY_CLOSE);
		break;
	}
}

void CQuotesProviderVisitorTendency::Visit(const CQuotesProviderYahoo& rProvider)
{
	switch(m_chr)
	{
	case _T('o'):
		GetValue(DB_STR_YAHOO_OPEN_VALUE);
		break;
	case _T('h'):
		GetValue(DB_STR_YAHOO_DAY_HIGH);
		break;
	case _T('P'):
		GetValue(DB_STR_YAHOO_PREVIOUS_CLOSE);
		break;
	case _T('c'):
		GetValue(DB_STR_YAHOO_CHANGE);
		break;
	case _T('g'):
		GetValue(DB_STR_YAHOO_DAY_LOW);
		break;
	}

}


void CQuotesProviderVisitorTendency::GetValue(LPCSTR pszDbKeyName)
{
	m_bValid = Quotes_DBReadDouble(m_hContact,QUOTES_MODULE_NAME,pszDbKeyName,m_dResult);
}
