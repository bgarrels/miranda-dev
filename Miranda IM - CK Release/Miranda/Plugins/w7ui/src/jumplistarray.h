#ifndef jumplistarray_h__
#define jumplistarray_h__

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

class CJumpListArray
{
private:
	IObjectCollection *m_pObjects;

public:
	CJumpListArray();
	~CJumpListArray();

	void AddItem(char *mir_icon, TCHAR *title, TCHAR *path, TCHAR *args);
	void AddItem(int skinicon, TCHAR *title, TCHAR *path, TCHAR *args);
	void AddItem(char *proto, int skinicon, TCHAR *title, TCHAR *path, TCHAR *args);
	
	IObjectArray *GetArray();

private:
	void AddItemImpl(TCHAR *icon, int iIcon, TCHAR *title, TCHAR *path, TCHAR *args);

	bool LoadMirandaIcon(char *mir_icon, TCHAR *icon, int *id);
	bool LoadMirandaIcon(int skinicon, TCHAR *icon, int *id);
	bool LoadMirandaIcon(char *proto, int status, TCHAR *icon, int *id);

	static IShellLink *NewShellLink(TCHAR *icon, int iIcon, TCHAR *title, TCHAR *path, TCHAR *args);
};

#endif // jumplistarray_h__
