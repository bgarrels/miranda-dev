#ifndef __85dbfa97_919b_4776_919c_7410a1c3d787_HTMLEngine_h__
#define __85dbfa97_919b_4776_919c_7410a1c3d787_HTMLEngine_h__

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

* HTML Parser
*/

class IHTMLParser;

class IHTMLEngine
{
public:
	typedef boost::shared_ptr<IHTMLParser> THTMLParserPtr;

public:
	IHTMLEngine(void){}
	virtual ~IHTMLEngine(){}

	virtual THTMLParserPtr GetParserPtr()const = 0;
};

#endif //__85dbfa97_919b_4776_919c_7410a1c3d787_HTMLEngine_h__
