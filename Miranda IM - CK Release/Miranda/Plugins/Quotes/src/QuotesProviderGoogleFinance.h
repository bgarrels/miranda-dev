#ifndef __89D3CC58_7DED_484f_AA25_62BDBB57E18B_QuotesProvider_Google_Finance_h__
#define __89D3CC58_7DED_484f_AA25_62BDBB57E18B_QuotesProvider_Google_Finance_h__

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

#include "QuotesProviderFinance.h"

#define DB_STR_GOOGLE_FINANCE_OPEN_VALUE "OpenQuotePrice"
#define DB_STR_GOOGLE_FINANCE_DIFF "DifferentFromStartOfDay"
#define DB_STR_GOOGLE_FINANCE_PERCENT_CHANGE_TO_YERSTERDAY_CLOSE "PercentChangeToYersterdayClose"

// #define DB_STR_GOOGLE_FINANCE_COMP_NAME "CompanyName"


class CQuotesProviderGoogleFinance : public CQuotesProviderFinance
{

public:
	CQuotesProviderGoogleFinance();
	~CQuotesProviderGoogleFinance();

private:
	virtual void RefreshQuotes(TContracts& anContacts);
	virtual void Accept(CQuotesProviderVisitor& visitor)const;
};

#endif //__89D3CC58_7DED_484f_AA25_62BDBB57E18B_QuotesProvider_Google_Finance_h__
