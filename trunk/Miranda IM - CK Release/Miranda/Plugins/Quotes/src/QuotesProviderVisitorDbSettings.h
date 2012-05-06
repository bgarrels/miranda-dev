#ifndef __97cd432a_1119_4803_a96f_0abc1cc2653f_QuotesProviderVisitorDbSettings_h__
#define __97cd432a_1119_4803_a96f_0abc1cc2653f_QuotesProviderVisitorDbSettings_h__

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

class CQuotesProviderVisitorDbSettings : public CQuotesProviderVisitor
{
public:
	CQuotesProviderVisitorDbSettings();
	~CQuotesProviderVisitorDbSettings();

private:
	virtual void Visit(const CQuotesProviderBase& rProvider);
	virtual void Visit(const CQuotesProviderDukasCopy& rProvider);
	virtual void Visit(const CQuotesProviderGoogle& rProvider);
	virtual void Visit(const CQuotesProviderGoogleFinance& rProvider);
	virtual void Visit(const CQuotesProviderYahoo& rProvider);
public:
	LPCSTR m_pszDbRefreshRateType;
	LPCSTR m_pszDbRefreshRateValue;
	LPCSTR m_pszDbDisplayNameFormat;	
	LPCTSTR m_pszDefDisplayFormat;
	LPCTSTR m_pszDefLogFileFormat;
	LPCTSTR m_pszDefHistoryFormat;
	LPCTSTR m_pszXMLIniFileName;
	LPCSTR m_pszDbStatusMsgFormat;
	LPCTSTR m_pszDefStatusMsgFormat;
	LPCTSTR m_pszDefPopupFormat;
	LPCSTR m_pszDbTendencyFormat;
	LPCTSTR m_pszDefTendencyFormat;

	//global settings
	LPCSTR m_pszDbLogMode;
	LPCSTR m_pszDbHistoryFormat;
	LPCSTR m_pszDbHistoryCondition;
	LPCSTR m_pszDbLogFile;
	LPCSTR m_pszDbLogFormat;
	LPCSTR m_pszDbLogCondition;
	LPCSTR m_pszDbPopupFormat;
	LPCSTR m_pszDbPopupCondition;
	LPCSTR m_pszDbPopupColourMode;
	LPCSTR m_pszDbPopupBkColour;
	LPCSTR m_pszDbPopupTextColour;
	LPCSTR m_pszDbPopupDelayMode;
	LPCSTR m_pszDbPopupDelayTimeout;
	LPCSTR m_pszDbPopupHistoryFlag;
};

#endif //__97cd432a_1119_4803_a96f_0abc1cc2653f_QuotesProviderVisitorDbSettings_h__
