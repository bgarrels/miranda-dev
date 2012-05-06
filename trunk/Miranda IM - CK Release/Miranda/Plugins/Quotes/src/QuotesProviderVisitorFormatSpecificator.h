#ifndef __00c159f3_525a_41e9_8fc5_00464b6fafa3_QuotesProviderVisitorFormatSpecificator_h__
#define __00c159f3_525a_41e9_8fc5_00464b6fafa3_QuotesProviderVisitorFormatSpecificator_h__

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

#include "quotesprovidervisitor.h"

class CQuotesProviderVisitorFormatSpecificator : public CQuotesProviderVisitor
{
public:
	struct CFormatSpecificator
	{
		CFormatSpecificator(const tstring& rsSymbol = _T(""),const tstring& rsDec = _T(""))
			: m_sSymbol(rsSymbol),m_sDesc(rsDec){}

		tstring m_sSymbol;
		tstring m_sDesc;
	};
	typedef std::vector<CFormatSpecificator> TFormatSpecificators;

public:
	CQuotesProviderVisitorFormatSpecificator();
	~CQuotesProviderVisitorFormatSpecificator();

	const TFormatSpecificators& GetSpecificators()const;

private:
	virtual void Visit(const CQuotesProviderDukasCopy& rProvider);
	virtual void Visit(const CQuotesProviderGoogle& rProvider);
	virtual void Visit(const CQuotesProviderBase& rProvider);
	virtual void Visit(const CQuotesProviderGoogleFinance& rProvider);
	virtual void Visit(const CQuotesProviderYahoo& rProvider);

private:
	TFormatSpecificators m_aSpecificators;
};

#endif//__00c159f3_525a_41e9_8fc5_00464b6fafa3_QuotesProviderVisitorFormatSpecificator_h__
