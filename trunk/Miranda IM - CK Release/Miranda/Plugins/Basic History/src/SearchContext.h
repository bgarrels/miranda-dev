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
#include "EventList.h"

class SearchContext : public EventList
{
public:
	HWND editWindow;
	HWND findWindow;
	HWND toolbarWindow;
	HWND listWindow;
	int selected;

	virtual void SelectEventGroup(int sel) = 0;

	struct MessageData
	{
		MessageData(const std::wstring& _description, int _startPos, int _endPos, bool _isMe, DWORD _timestamp)
			:description(_description)
		{
			startPos = _startPos;
			endPos = _endPos;
			isMe = _isMe;
			timestamp = _timestamp;
		}

		std::wstring description;
		int startPos;
		int endPos;
		bool isMe;
		DWORD timestamp;
	};

	std::vector<MessageData> currentGroup;
};

