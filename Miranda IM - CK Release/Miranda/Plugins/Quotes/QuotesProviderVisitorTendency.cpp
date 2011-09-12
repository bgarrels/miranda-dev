#include "stdafx.h"
#include "QuotesProviderVisitorTendency.h"
#include "DBUtils.h"
#include "QuotesProviderGoogleFinance.h"
#include "EconomicRateInfo.h"

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

void CQuotesProviderVisitorTendency::GetValue(LPCSTR pszDbKeyName)
{
	m_bValid = Quotes_DBReadDouble(m_hContact,QUOTES_MODULE_NAME,pszDbKeyName,m_dResult);
}
