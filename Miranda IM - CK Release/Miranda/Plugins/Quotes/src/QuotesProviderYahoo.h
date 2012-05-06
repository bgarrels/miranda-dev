#ifndef __E927F394_5452_458E_AF48_71E44F9EE793_QuotesProviderYahoo_h__
#define __E927F394_5452_458E_AF48_71E44F9EE793_QuotesProviderYahoo_h__

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


#define DB_STR_YAHOO_OPEN_VALUE "OpenQuotePrice"
#define DB_STR_YAHOO_DAY_HIGH "DayHigh"
#define DB_STR_YAHOO_DAY_LOW "DayLow"
#define DB_STR_YAHOO_PREVIOUS_CLOSE "PreviousClose"
#define DB_STR_YAHOO_CHANGE "Change"

class CQuotesProviderYahoo : public CQuotesProviderFinance
{
private:
	virtual void RefreshQuotes(TContracts& anContacts);
	virtual void Accept(CQuotesProviderVisitor& visitor)const;
};

#endif //__E927F394_5452_458E_AF48_71E44F9EE793_QuotesProviderYahoo_h__
