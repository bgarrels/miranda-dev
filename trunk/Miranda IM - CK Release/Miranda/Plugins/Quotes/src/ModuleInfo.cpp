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
#include "ModuleInfo.h"
#include "QuotesProviders.h"
#include "HTMLParserMS.h"
#include "LightMutex.h"
#include "WinCtrlHelper.h"
#include "EconomicRateInfo.h"
#include "XMLEngineMI.h"

namespace
{
	HINSTANCE g_hInstance = NULL;
	CModuleInfo::TXMLEnginePtr g_pXMLEngine;
	CModuleInfo::THTMLEnginePtr g_pHTMLEngine;
	CLightMutex g_lmParsers;
}

CModuleInfo::CModuleInfo() 
			: m_bExtendedStatusInfo(1 == DBGetContactSettingByte(NULL,QUOTES_MODULE_NAME,"ExtendedStatus",false))
{
}

CModuleInfo::~CModuleInfo()
{
}

CModuleInfo& CModuleInfo::GetInstance()
{
	static CModuleInfo mi;
	return mi;
}

HANDLE CModuleInfo::GetWindowList(const std::string& rsKey,bool bAllocateIfNonExist /*= true*/)
{
	HANDLE hResult = NULL;
	THandles::const_iterator i = m_ahWindowLists.find(rsKey);
	if(i != m_ahWindowLists.end())
	{
		hResult = i->second;
	}
	else if(bAllocateIfNonExist)
	{
		hResult = reinterpret_cast<HANDLE>(CallService(MS_UTILS_ALLOCWINDOWLIST,0,0));
		if(hResult)
		{
			m_ahWindowLists.insert(std::make_pair(rsKey,hResult));
		}
	}

	return hResult;
}	

void CModuleInfo::OnMirandaShutdown()
{
	BOOST_FOREACH(THandles::value_type p,m_ahWindowLists)
	{
		WindowList_Broadcast(p.second,WM_CLOSE,0,0);
	}
}

void CModuleInfo::SetModuleHandle(HINSTANCE hInstance)
{
	assert(NULL == g_hInstance);
	assert(NULL != hInstance);

	g_hInstance = hInstance;
}

HINSTANCE CModuleInfo::GetModuleHandle()
{
	assert(NULL != g_hInstance);
	return g_hInstance;
}

CModuleInfo::TQuotesProvidersPtr CModuleInfo::GetQuoteProvidersPtr()
{
	static TQuotesProvidersPtr pProviders(new CQuotesProviders);
	return pProviders;
}

CModuleInfo::TXMLEnginePtr CModuleInfo::GetXMLEnginePtr()
{
	if(!g_pXMLEngine)
	{
		CGuard<CLightMutex> cs(g_lmParsers);
		if(!g_pXMLEngine)
		{
			mir_getXI(&xi);
			g_pXMLEngine = TXMLEnginePtr(new CXMLEngineMI);
		}
	}

	return g_pXMLEngine;
}

// void CModuleInfo::SetXMLEnginePtr(TXMLEnginePtr pEngine)
// {
// 	g_pXMLEngine = pEngine;
// }

CModuleInfo::THTMLEnginePtr CModuleInfo::GetHTMLEngine()
{
	if(!g_pHTMLEngine)
	{
		CGuard<CLightMutex> cs(g_lmParsers);
		if(!g_pHTMLEngine)
		{
			g_pHTMLEngine = THTMLEnginePtr(new CHTMLEngineMS);
		}
	}

	return g_pHTMLEngine;
}

void CModuleInfo::SetHTMLEngine(THTMLEnginePtr pEngine)
{
	g_pHTMLEngine = pEngine;
}

bool CModuleInfo::Verify()
{
	INITCOMMONCONTROLSEX icc = {0};
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_WIN95_CLASSES|ICC_LINK_CLASS;
	if(FALSE == ::InitCommonControlsEx(&icc))
	{
		return false;
	}

	if(!GetXMLEnginePtr())
	{
		Quotes_MessageBox(NULL,TranslateT("Miranda could not load Quotes plugin. XML parser is missing."),MB_OK|MB_ICONERROR);
		return false;
	}

	if(!g_pHTMLEngine && (false == CHTMLParserMS::IsInstalled()))
	{
		Quotes_MessageBox(NULL,
			TranslateT("Miranda could not load Quotes plugin. Microsoft HTML parser is missing."),
			MB_YESNO|MB_ICONQUESTION);
		return false;
	}

	return true;
}

bool CModuleInfo::GetExtendedStatusFlag()const
{
	return m_bExtendedStatusInfo;
}
