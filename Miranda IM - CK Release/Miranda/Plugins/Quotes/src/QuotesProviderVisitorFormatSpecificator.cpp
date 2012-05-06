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
#include "QuotesProviderVisitorFormatSpecificator.h"

CQuotesProviderVisitorFormatSpecificator::CQuotesProviderVisitorFormatSpecificator()
{
}

CQuotesProviderVisitorFormatSpecificator::~CQuotesProviderVisitorFormatSpecificator()
{
}

void CQuotesProviderVisitorFormatSpecificator::Visit(const CQuotesProviderDukasCopy&/* rProvider*/)
{
	m_aSpecificators.push_back(CFormatSpecificator(_T("%s"),TranslateT("Quote Symbol")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%d"),TranslateT("Quote Name")));
}

void CQuotesProviderVisitorFormatSpecificator::Visit(const CQuotesProviderGoogle&/* rProvider*/)
{
	m_aSpecificators.push_back(CFormatSpecificator(_T("%F"),TranslateT("From Currency Full Name")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%f"),TranslateT("From Currency Short Name")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%I"),TranslateT("Into Currency Full Name")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%i"),TranslateT("Into Currency Short Name")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%s"),TranslateT("Short notation for \"%f/%i\"")));
}

void CQuotesProviderVisitorFormatSpecificator::Visit(const CQuotesProviderBase&/* rProvider*/)
{
	m_aSpecificators.push_back(CFormatSpecificator(_T("%S"),TranslateT("Source of Information")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%r"),TranslateT("Rate Value")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%p"),TranslateT("Previous Rate Value")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%X"),TranslateT("Fetch Time")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%x"),TranslateT("Fetch Date")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%t"),TranslateT("Fetch Time and Date")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("\\%"),TranslateT("Percentage Character (%)")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("\\t"),TranslateT("Tabulation")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("\\\\"),TranslateT("Left slash (\\)")));
}

void CQuotesProviderVisitorFormatSpecificator::Visit(const CQuotesProviderGoogleFinance&/* rProvider*/)
{
	m_aSpecificators.push_back(CFormatSpecificator(_T("%s"),TranslateT("Quote Symbol")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%n"),TranslateT("Quote Name")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%o"),TranslateT("Open Price")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%d"),TranslateT("Percent Change to After Hours")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%y"),TranslateT("Percent Change to Yesterday Close")));
}

const CQuotesProviderVisitorFormatSpecificator::TFormatSpecificators& CQuotesProviderVisitorFormatSpecificator::GetSpecificators()const
{
	return m_aSpecificators;
}

void CQuotesProviderVisitorFormatSpecificator::Visit(const CQuotesProviderYahoo& rProvider)
{
	m_aSpecificators.push_back(CFormatSpecificator(_T("%s"),TranslateT("Quote Symbol")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%n"),TranslateT("Quote Name")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%o"),TranslateT("Open Price")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%h"),TranslateT("Day's High")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%g"),TranslateT("Day's Low")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%P"),TranslateT("Previous Close")));
	m_aSpecificators.push_back(CFormatSpecificator(_T("%c"),TranslateT("Change")));
}
