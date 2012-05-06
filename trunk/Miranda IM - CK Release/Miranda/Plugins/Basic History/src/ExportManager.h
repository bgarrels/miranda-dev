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
#include "IExport.h"
#include "IImport.h"
class ExportManager : public EventList
{
private:
	IExport* exp;
	std::wstring file;
	HWND hwnd;
	bool oldOnTop;
protected:
	virtual void AddGroup(bool isMe, const std::wstring &time, const std::wstring &user, const std::wstring &eventText, int ico);
public:
	ExportManager(HWND _hwnd, HANDLE _hContact, int filter);

	void SetAutoExport(const std::wstring _file, int _deltaTime, DWORD _now)
	{
		file = _file;
		deltaTime = _deltaTime;
		now = _now;
	}

	void SetAutoImport(const std::wstring _file)
	{
		file = _file;
	}

	std::wstring GetFileName()
	{
		return file;
	}

	bool Export(IExport::ExportType type);
	void SetDeleteWithoutExportEvents(int _deltaTime, DWORD _now);
	void DeleteExportedEvents();
	int Import(IImport::ImportType type, const std::vector<HANDLE>& contacts);
	bool Import(IImport::ImportType type, std::vector<IImport::ExternalMessage>& eventList, std::wstring* err = NULL);
	static const TCHAR* GetExt(IImport::ImportType type);
};

