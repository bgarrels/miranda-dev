#ifndef M_FOLDERS_DLG_HANDLERS_H
#define M_FOLDERS_DLG_HANDLERS_H

/*
Custom profile folders plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			 Copyright © 2005 Cristian Libotean

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

//#include "commonheaders.h"
#include "services.h"
#include "events.h"
#include <richedit.h>

INT_PTR CALLBACK DlgProcOpts(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgProcVariables(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int GetCurrentItemSelection(HWND hWnd);
int GetCurrentSectionSelection(HWND hWnd);
PFolderItem GetSelectedItem(HWND hWnd);
int GetCurrentSectionText(HWND hWnd, WCHAR *buffer, int count);
int GetCurrentItemText(HWND hWnd, WCHAR *buffer, int count);
//void GetEditText(HWND hWnd, char *buffer, int size);
void GetEditTextW(HWND hWnd, wchar_t *buffer, int size);
//void SetEditText(HWND hWnd, const char *buffer);
void SetEditTextW(HWND hWnd, const wchar_t *buffer);
void LoadRegisteredFolderSections(HWND hWnd);
void LoadRegisteredFolderItems(HWND hWnd);
void LoadItem(HWND hWnd, PFolderItem item);
void SaveItem(HWND hWnd, PFolderItem item, int bEnableApply = TRUE);
int ChangesNotSaved(HWND hWnd);
void RefreshPreview(HWND hWnd);

#endif //M_FOLDERS_DLG_HANDLERS_H