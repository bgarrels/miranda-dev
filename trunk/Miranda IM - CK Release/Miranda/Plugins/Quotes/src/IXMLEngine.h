#ifndef __f88e20d7_5e65_40fb_a7b5_7c7af1ee1c78_IXMLEngine_h__
#define __f88e20d7_5e65_40fb_a7b5_7c7af1ee1c78_IXMLEngine_h__

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

class IXMLNode
{
public:
	typedef boost::shared_ptr<IXMLNode> TXMLNodePtr;

public:
	IXMLNode(){}
	virtual ~IXMLNode(){}

	virtual size_t GetChildCount()const = 0;
	virtual TXMLNodePtr GetChildNode(size_t nIndex)const = 0;
	
	virtual tstring GetText()const = 0;
	virtual tstring GetName()const = 0;

	virtual bool AddChild(const TXMLNodePtr& pNode) = 0;
	virtual bool AddAttribute(const tstring& rsName,const tstring& rsValue) = 0;
	virtual tstring GetAttributeValue(const tstring& rsAttrName) = 0;
	virtual void Write(tostream& o)const = 0;
};

inline tostream& operator<<(tostream& o,const IXMLNode& node) 
{
	node.Write(o);
	return o;
}

class IXMLEngine
{
public:
	IXMLEngine(){}

	virtual ~IXMLEngine(){}

	virtual IXMLNode::TXMLNodePtr LoadFile(const tstring& rsFileName)const = 0;
	virtual bool SaveFile(const tstring& rsFileName,const IXMLNode::TXMLNodePtr& pNode)const = 0;
	virtual IXMLNode::TXMLNodePtr CreateNode(const tstring& rsName,const tstring& rsText)const = 0;
};

#endif //__f88e20d7_5e65_40fb_a7b5_7c7af1ee1c78_IXMLEngine_h__
