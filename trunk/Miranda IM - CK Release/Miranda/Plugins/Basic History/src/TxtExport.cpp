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

#include "StdAfx.h"
#include "TxtExport.h"
#define EXP_FILE (*stream)

TxtExport::~TxtExport()
{
}

void TxtExport::WriteHeader(const std::wstring &fileName, const std::wstring &filterName, const std::wstring &myName, const std::wstring &myId, const std::wstring &name1, const std::wstring &proto1, const std::wstring &id1, const std::string& baseProto1, const std::wstring& encoding)
{
	TCHAR* start = TranslateT("###");
	EXP_FILE << start << "\n" << start << _T(" ") << TranslateT("History Log") << _T("\n");
	EXP_FILE << start << _T(" ") << myName;
	if(proto1.length() || myId.length())
	{
		EXP_FILE << _T(" (") << proto1 << _T(": ") << myId << _T(") - ");
	}
	else
	{
		EXP_FILE << _T(" - ");
	}

	EXP_FILE << name1;
	if(proto1.length() || id1.length())
	{
		EXP_FILE << _T(" (") << proto1 << _T(": ") << id1 << _T(")\n");
	}
	else
	{
		EXP_FILE << _T("\n");
	}

	EXP_FILE << start << _T(" ") << TranslateT("Filter:") << _T(" ") << filterName << _T("\n") << start << _T("\n");
}

void TxtExport::WriteFooter()
{
}

void TxtExport::WriteGroup(bool isMe, const std::wstring &time, const std::wstring &user, const std::wstring &eventText)
{
}

void TxtExport::WriteMessage(bool isMe, const std::wstring &longDate, const std::wstring &shortDate, const std::wstring &user, const std::wstring &message, const DBEVENTINFO& dbei)
{
	EXP_FILE << "\n[" << longDate << "] " << user << ":\n" << message << "\n";
}
