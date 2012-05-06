#ifndef __3c99e3f7_ecd9_4d9b_8f86_fe293c5fc8e6_HTMLParserMS_h__
#define __3c99e3f7_ecd9_4d9b_8f86_fe293c5fc8e6_HTMLParserMS_h__

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

#include "ihtmlparser.h"
#include "LightMutex.h"
#include "IHTMLEngine.h"

class CHTMLParserMS : public IHTMLParser
{
public:
	CHTMLParserMS();
	~CHTMLParserMS();

	virtual THTMLNodePtr ParseString(const tstring& rsHTML);

	static bool IsInstalled();

private:
	bool m_bCallUninit;
	CComPtr<IHTMLDocument2> m_pDoc;
	CComPtr<IMarkupServices>  m_pMS;
	CComPtr<IMarkupPointer>  m_pMkStart;
	CComPtr<IMarkupPointer>  m_pMkFinish;
	mutable CLightMutex m_cs;
};

class CHTMLEngineMS : public IHTMLEngine
{
public:
	CHTMLEngineMS();
	~CHTMLEngineMS();

	virtual THTMLParserPtr GetParserPtr()const;
};

#endif //__3c99e3f7_ecd9_4d9b_8f86_fe293c5fc8e6_HTMLParserMS_h__
