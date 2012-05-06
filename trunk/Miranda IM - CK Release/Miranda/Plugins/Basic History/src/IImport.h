/*
Basic History plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2011-2012 Krzysztof Kral

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

class IImport
{
protected:
	std::istream* stream;
public:
	enum ImportType
	{
		Binary,
		Dat
	};

	struct ExternalMessage
	{
		DWORD timestamp;
		WORD eventType;
		WORD flags;
		std::wstring message;
	};

	void SetStream(std::istream *str)
	{
		stream = str;
	}

	virtual const TCHAR* GetExt() = 0;
	virtual int IsContactInFile(const std::vector<HANDLE>& contacts) = 0;
	virtual bool GetEventList(std::vector<ExternalMessage>& eventList) = 0;

	virtual ~IImport()
	{
	}
};
