#ifndef __OPTIONS_H
#define __OPTIONS_H

/*
YAMN plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2002-2004 majvan
			Copyright (C) 2005-2007 tweety y_b

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

#define M_SHOWACTUAL	0
#define M_SHOWDEFAULT	1


//Enables account in options
BOOL DlgEnableAccount(HWND hDlg,WPARAM wParam,LPARAM lParam);

//Sets dialog controls to match current account
BOOL DlgShowAccount(HWND hDlg,WPARAM wParam,LPARAM lParam);

//Sets colors to match colors of actual account 
BOOL DlgShowAccountColors(HWND hDlg,WPARAM wParam,LPARAM lParam);

//Sets dialog item text
BOOL DlgSetItemText(HWND hDlg,WPARAM wParam,LPARAM lParam);

//Sets dialog item text in Unicode
BOOL DlgSetItemTextW(HWND hDlg,WPARAM wParam,LPARAM lParam);

//Options dialog procedure
INT_PTR CALLBACK DlgProcPOP3AccOpt(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

//Options dialog procedure
BOOL CALLBACK DlgProcPOP3AccStatusOpt(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

//Options dialog procedure
INT_PTR CALLBACK DlgProcYAMNOpt(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

//Options dialog procedure
INT_PTR CALLBACK DlgProcPOP3AccPopup(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

//Initializes POP3 options for Miranda
int POP3OptInit(WPARAM wParam,LPARAM lParam);

#endif
