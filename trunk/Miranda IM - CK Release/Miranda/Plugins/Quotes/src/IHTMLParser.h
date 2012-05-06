#ifndef __98ad6d6d_2a27_43fd_bf3e_c18416a45e54_IHTMLParser_h__
#define __98ad6d6d_2a27_43fd_bf3e_c18416a45e54_IHTMLParser_h__

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

class IHTMLNode
{
public:
	typedef boost::shared_ptr<IHTMLNode> THTMLNodePtr;

	enum EType
	{
		Table = 1,
		TableRow,
		TableColumn
	};

public:
	IHTMLNode(){}
	virtual ~IHTMLNode(){}

	virtual size_t GetChildCount()const = 0;
	virtual THTMLNodePtr GetChildPtr(size_t nIndex) = 0;
	virtual bool Is(EType nType)const = 0;

	virtual THTMLNodePtr GetElementByID(const tstring& rsID)const = 0;
	
	virtual tstring GetAttribute(const tstring& rsAttrName)const = 0;
	virtual tstring GetText()const = 0;
};

class IHTMLParser
{
public:
	typedef IHTMLNode::THTMLNodePtr THTMLNodePtr;
public:
	IHTMLParser(){}
	virtual ~IHTMLParser(){}

	virtual THTMLNodePtr ParseString(const tstring& rsHTML) = 0;
};

#endif //__98ad6d6d_2a27_43fd_bf3e_c18416a45e54_IHTMLParser_h__
