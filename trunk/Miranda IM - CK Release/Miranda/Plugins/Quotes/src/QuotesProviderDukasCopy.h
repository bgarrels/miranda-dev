#ifndef __93121758_68c7_4836_b571_da84dfe82b84_QuotesProviderDukasCopy_h__
#define __93121758_68c7_4836_b571_da84dfe82b84_QuotesProviderDukasCopy_h__

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

#include "quotesproviderbase.h"
#include <vector>

#define DB_STR_REFRESH_RATE_TYPE "RefreshRateType"
#define DB_STR_REFRESH_RATE_VALUE "RefreshRateValue"
#define DB_STR_DC_DISPLAY_NAME_FORMAT "DC_DisplayNameFormat"
// #define DB_STR_DC_LOG_FILE_FORMAT "DC_LogFileFormat"
// #define DB_STR_DC_HISTORY_FORMAT "DC_HistoryFormat"

class CQuotesProviderDukasCopy : public CQuotesProviderBase
{
public:
	CQuotesProviderDukasCopy();
	~CQuotesProviderDukasCopy();

	bool WatchForQuote(const CQuote& rQuote,bool bWatch);
	bool IsQuoteWatched(const CQuote& rQuote)const;

	HANDLE GetContactByQuoteID(const tstring& rsQuoteID)const;
// #ifdef CHART_IMPLEMENT
// 	bool Chart(HANDLE hContact,const tstring& url)const;
// #endif

private:
	//IQuotesProvider implementation
	virtual void RefreshQuotes(TContracts& anContacts);
	virtual void ShowPropertyPage(WPARAM wp,OPTIONSDIALOGPAGE& odp);
	virtual void Accept(CQuotesProviderVisitor& visitor)const;

private:
	tstring BuildHTTPURL()const;
	
};

#endif //__93121758_68c7_4836_b571_da84dfe82b84_QuotesProviderDukasCopy_h__
