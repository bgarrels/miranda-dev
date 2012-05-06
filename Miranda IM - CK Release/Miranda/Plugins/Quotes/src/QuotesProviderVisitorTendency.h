#ifndef __AD721194_E9944366_9CF1_0307460EF32F_QuotesProviderVisitorTendency_h__
#define __AD721194_E9944366_9CF1_0307460EF32F_QuotesProviderVisitorTendency_h__

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

class CQuotesProviderVisitorTendency : public CQuotesProviderVisitor
{
public:
	CQuotesProviderVisitorTendency(HANDLE hContact,TCHAR chr);
	
	bool IsValid()const{return m_bValid;}
	double GetResult()const{return m_dResult;}

private:
	virtual void Visit(const CQuotesProviderBase& rProvider);
	virtual void Visit(const CQuotesProviderGoogleFinance& rProvider);
	virtual void Visit(const CQuotesProviderYahoo& rProvider);

private:
	void GetValue(LPCSTR pszDbKeyName);

private:
	HANDLE m_hContact;
	TCHAR m_chr;
	bool m_bValid;
	double m_dResult;
};

#endif //__AD721194_E9944366_9CF1_0307460EF32F_QuotesProviderVisitorTendency_h__
