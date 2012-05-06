#ifndef jumplistbuilder_h__
#define jumplistbuilder_h__

/*
W7ui plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2009 nullbie, persei

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

class CJumpListBuilder
{
public:
	static void Load()
	{
		m_instance = new CJumpListBuilder;
	}

	static void Unload()
	{
		delete m_instance;
	}

	static void Rebuild()
	{
		m_instance->BuildJumpList();
	}

private:
	static CJumpListBuilder *m_instance;

	LIST<TCHAR> *m_lists;
	ICustomDestinationList *m_pList;
	CJumpListArray *m_pCurrentList;
	TCHAR m_cmdBuf[MAX_PATH * 2];

	HANDLE m_hBuildCategories;
	HANDLE m_hBuildItems;

	CJumpListBuilder();
	~CJumpListBuilder();
	void BuildJumpList();

	int __cdecl Rebuild(WPARAM wParam, LPARAM lParam);
	int __cdecl AddCategory(WPARAM wParam, LPARAM lParam);
	int __cdecl AddItem(WPARAM wParam, LPARAM lParam);

	void BuildCategory(TCHAR *category);

	TCHAR *BuildJumpListCommand(TCHAR *prefix, TCHAR *argument);
	HANDLE CreateSvc(char *svc, int (__cdecl CJumpListBuilder::*fn)(WPARAM, LPARAM));
};

#endif // jumplistbuilder_h__
