#ifndef __95A13A68_0DF0_43FA_B6C1_81D83AED59AA_QuotesProviderFinance_h__
#define __95A13A68_0DF0_43FA_B6C1_81D83AED59AA_QuotesProviderFinance_h__

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

#include "QuotesProviderBase.h"

class CQuotesProviderFinance : public CQuotesProviderBase
{
public:
	typedef std::vector<CQuotesProviderBase::CQuote> TQuotes;

public:
	void GetWatchedQuotes(TQuotes& raQuotes)const;
	bool WatchForQuote(const CQuote& rQuote,bool bWatch);
	HANDLE GetContactByQuoteID(const tstring& rsQuoteID)const;

protected:
	virtual void ShowPropertyPage(WPARAM wp,OPTIONSDIALOGPAGE& odp);
	virtual void Accept(CQuotesProviderVisitor& visitor)const;
};

#endif //__95A13A68_0DF0_43FA_B6C1_81D83AED59AA_QuotesProviderFinance_h__