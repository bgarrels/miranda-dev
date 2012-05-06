#ifndef __ac71e133_786c_41a7_ab07_625b76ff2a8c_QuotesProvider_h__
#define __ac71e133_786c_41a7_ab07_625b76ff2a8c_QuotesProvider_h__

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

#pragma once

#include <boost\noncopyable.hpp>
#include <string>

class CQuotesProviderVisitor;

class IQuotesProvider : private boost::noncopyable
{
public:
	struct CProviderInfo 
	{		
		tstring m_sName;
		tstring m_sURL;

	};

public:
	IQuotesProvider(){}
	virtual ~IQuotesProvider(){}

	virtual bool Init() = 0;
	virtual const CProviderInfo& GetInfo()const = 0;

	virtual void AddContact(HANDLE hContact) = 0;
	virtual void DeleteContact(HANDLE hContact) = 0;

	virtual void ShowPropertyPage(WPARAM wp,OPTIONSDIALOGPAGE& odp) = 0;
	virtual void RefreshAll() = 0;
 	virtual void RefreshContact(HANDLE hContact) = 0;
	virtual void SetContactExtraIcon(HANDLE hContact)const = 0;

	virtual void Run() = 0;

	virtual void Accept(CQuotesProviderVisitor& visitor)const = 0;
};

#endif //__ac71e133_786c_41a7_ab07_625b76ff2a8c_QuotesProvider_h__
