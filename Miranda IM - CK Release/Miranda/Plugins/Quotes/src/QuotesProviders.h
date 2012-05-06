#ifndef __148306d1_da2a_43df_b1ad_0cdc8ef8a79e_QuotesProviders_h__
#define __148306d1_da2a_43df_b1ad_0cdc8ef8a79e_QuotesProviders_h__

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

#include <boost\shared_ptr.hpp>
#include <vector>

class IQuotesProvider;

class CQuotesProviders
{
public:
	typedef boost::shared_ptr<IQuotesProvider> TQuotesProviderPtr;
	typedef std::vector<TQuotesProviderPtr> TQuotesProviders;

public:
	CQuotesProviders();
	~CQuotesProviders();

	TQuotesProviderPtr FindProvider(const tstring& rsName)const;
	TQuotesProviderPtr GetContactProviderPtr(HANDLE hContact)const;
	const TQuotesProviders& GetProviders()const;

private:
	void InitProviders();
	void CreateProviders();
	void ClearProviders();

private:
	TQuotesProviders m_apProviders;
};

#endif //__148306d1_da2a_43df_b1ad_0cdc8ef8a79e_QuotesProviders_h__
