#ifndef __c0e48a95_b3f0_4227_8adc_455e265f3c14_QuotesProviderGoogle_h__
#define __c0e48a95_b3f0_4227_8adc_455e265f3c14_QuotesProviderGoogle_h__

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

#include "quotesproviderBase.h"

#define DB_STR_FROM_ID "FromID"
#define DB_STR_TO_ID "ToID"
#define DB_STR_FROM_DESCRIPTION "FromDesc"
#define DB_STR_TO_DESCRIPTION "ToDesc"
#define DB_STR_GOOGLE_REFRESH_RATE_TYPE "Google_RefreshRateType"
#define DB_STR_GOOGLE_REFRESH_RATE_VALUE "Google_RefreshRateValue"
#define DB_STR_GOOGLE_DISPLAY_NAME_FORMAT "Google_DspNameFrmt"
// #define DB_STR_GOOGLE_LOG_FILE_FORMAT "Google_LogFileFormat"
// #define DB_STR_GOOGLE_HISTORY_FORMAT "Google_HistoryFormat"

class CQuotesProviderGoogle : public CQuotesProviderBase
{
public:
	struct CRateInfo
	{
		CQuotesProviderBase::CQuote m_from;
		CQuotesProviderBase::CQuote m_to;
	};
public:
	CQuotesProviderGoogle();
	~CQuotesProviderGoogle();

	bool WatchForRate(const CRateInfo& ri,bool bWatch);
	size_t GetWatchedRateCount()const;
	bool GetWatchedRateInfo(size_t nIndex,CRateInfo& rRateInfo);

	HANDLE GetContactByID(const tstring& rsFromID,const tstring& rsToID)const;

	double Convert(double dAmount,const CQuote& from,const CQuote& to)const;

private:
	virtual void RefreshQuotes(TContracts& anContacts);
	virtual void ShowPropertyPage(WPARAM wp,OPTIONSDIALOGPAGE& odp);
	virtual void Accept(CQuotesProviderVisitor& visitor)const;
};

#endif //__c0e48a95_b3f0_4227_8adc_455e265f3c14_QuotesProviderGoogle_h__
