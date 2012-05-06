#ifndef __d0f22b66_3135_4bbe_bee5_a31ea631ce58_ModuleInfo__
#define __d0f22b66_3135_4bbe_bee5_a31ea631ce58_ModuleInfo__

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

class CQuotesProviders;
class IXMLEngine;
class IHTMLEngine;
// class IHTMLParser;

class CModuleInfo
{
public:
	typedef boost::shared_ptr<CQuotesProviders> TQuotesProvidersPtr;
	typedef boost::shared_ptr<IXMLEngine> TXMLEnginePtr;
	typedef boost::shared_ptr<IHTMLEngine> THTMLEnginePtr;

private:
	CModuleInfo();
	~CModuleInfo(void);

public:
	static CModuleInfo& GetInstance();

	void OnMirandaShutdown();
	HANDLE GetWindowList(const std::string& rsKey,bool bAllocateIfNonExist = true);
	bool GetExtendedStatusFlag()const;

	static void SetModuleHandle(HINSTANCE hInstance);
	static HINSTANCE GetModuleHandle();

	static bool Verify();

	static TQuotesProvidersPtr GetQuoteProvidersPtr();

	static TXMLEnginePtr GetXMLEnginePtr();
// 	static void SetXMLEnginePtr(TXMLEnginePtr pEngine);

	static THTMLEnginePtr GetHTMLEngine();
	static void SetHTMLEngine(THTMLEnginePtr pEngine);

private:
	typedef std::map<std::string,HANDLE> THandles;
	THandles m_ahWindowLists;
	bool m_bExtendedStatusInfo;
};

#endif //__d0f22b66_3135_4bbe_bee5_a31ea631ce58_ModuleInfo__
