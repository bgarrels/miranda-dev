#ifndef MODERNOPT_H__
#define MODERNOPT_H__ 1

/*
Modernopt plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2009 Victor Pavlychko
			Copyright (C) 2010-2012 George Hazan

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

#include "m_descbutton.h"
#include "m_iconheader.h"
#include "m_modernopt.h"

struct ModernOptMainPageInfo
{
	int		idcButton;
	BOOL	bShowTab;
	int		iIcon;
	TCHAR	*lpzTitle;
	TCHAR	*lpzDescription;
	int		iPageType;
	BOOL	bShow;
};

extern struct ModernOptMainPageInfo g_ModernOptPages[MODERNOPT_PAGE_COUNT];

extern HMODULE hInst;

INT_PTR CALLBACK ModernOptHome_DlgProc(HWND hwndDlg, UINT  msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ModernOptSelector_DlgProc(HWND hwndDlg, UINT  msg, WPARAM wParam, LPARAM lParam);

void ModernOptIgnore_AddItem(MODERNOPTOBJECT *obj);

#endif // MODERNOPT_H__
