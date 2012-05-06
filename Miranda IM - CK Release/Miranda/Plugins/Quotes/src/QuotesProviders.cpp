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

#include "StdAfx.h"
#include "QuotesProviders.h"

#include "QuotesProviderDukasCopy.h"
#include "EconomicRateInfo.h"
#include "QuotesProviderGoogle.h"
#include "DBUtils.h"
#include "QuotesProviderGoogleFinance.h"
#include "QuotesProviderYahoo.h"

#define LAST_RUN_VERSION "LastRunVersion"

CQuotesProviders::CQuotesProviders()
{
	InitProviders();
}

CQuotesProviders::~CQuotesProviders()
{
	ClearProviders();
}

const CQuotesProviders::TQuotesProviders& CQuotesProviders::GetProviders()const
{
	return m_apProviders;
}

namespace
{
	template<class T>void create_provider(CQuotesProviders::TQuotesProviders& apProviders)
	{
		CQuotesProviders::TQuotesProviderPtr pProvider(new T);
		if(pProvider->Init())
		{
			apProviders.push_back(pProvider);
		}
	}
}

void CQuotesProviders::CreateProviders()
{
	create_provider<CQuotesProviderDukasCopy>(m_apProviders);
	create_provider<CQuotesProviderGoogle>(m_apProviders);
	create_provider<CQuotesProviderGoogleFinance>(m_apProviders);
	create_provider<CQuotesProviderYahoo>(m_apProviders);
}

void CQuotesProviders::ClearProviders()
{
	m_apProviders.clear();
}

namespace
{
	void convert_contact_settings(HANDLE hContact) 
	{
		WORD dwLogMode = DBGetContactSettingWord(hContact,QUOTES_PROTOCOL_NAME,DB_STR_QUOTE_LOG,static_cast<WORD>(lmDisabled));
		if((dwLogMode&lmInternalHistory) || (dwLogMode&lmExternalFile))
		{
			DBWriteContactSettingByte(hContact,QUOTES_PROTOCOL_NAME,DB_STR_CONTACT_SPEC_SETTINGS,1);
		}
	}
}
void CQuotesProviders::InitProviders()
{
	CreateProviders();

	const WORD nCurrentVersion = 17;
	WORD nVersion = DBGetContactSettingWord(NULL,QUOTES_MODULE_NAME,LAST_RUN_VERSION,1);

	for(HANDLE hContact = reinterpret_cast<HANDLE>(CallService(MS_DB_CONTACT_FINDFIRST,0,0));hContact;hContact = reinterpret_cast<HANDLE>(CallService(MS_DB_CONTACT_FINDNEXT,reinterpret_cast<WPARAM>(hContact),0)))
	{
		TQuotesProviderPtr pProvider = GetContactProviderPtr(hContact);
		if(pProvider)
		{
			pProvider->AddContact(hContact);
			if(nVersion < nCurrentVersion)
			{
				convert_contact_settings(hContact);
			}
		}
	}

	DBWriteContactSettingWord(NULL,QUOTES_MODULE_NAME,LAST_RUN_VERSION,nCurrentVersion);
}

CQuotesProviders::TQuotesProviderPtr CQuotesProviders::GetContactProviderPtr(HANDLE hContact)const
{
	char* szProto = reinterpret_cast<char*>(CallService(MS_PROTO_GETCONTACTBASEPROTO,
											reinterpret_cast<WPARAM>(hContact),0));
	if(NULL == szProto || 0 != ::_stricmp(szProto,QUOTES_PROTOCOL_NAME))
	{
		return TQuotesProviderPtr();
	}

	tstring sProvider = Quotes_DBGetStringT(hContact,QUOTES_MODULE_NAME,DB_STR_QUOTE_PROVIDER);
	if(true == sProvider.empty())
	{
		return TQuotesProviderPtr();
	}

	return FindProvider(sProvider);
}

CQuotesProviders::TQuotesProviderPtr CQuotesProviders::FindProvider(const tstring& rsName)const
{
	TQuotesProviderPtr pResult;
	for(TQuotesProviders::const_iterator i = m_apProviders.begin();i != m_apProviders.end();++i)
	{
		const TQuotesProviderPtr& pProvider = *i;
		const IQuotesProvider::CProviderInfo& rInfo = pProvider->GetInfo();
		if(0 == ::quotes_stricmp(rsName.c_str(),rInfo.m_sName.c_str()))
		{
			pResult = pProvider;
			break;
		}
	}

	return pResult;
}
