#ifndef _SECURE_DB__
#define _SECURE_DB__

/*
db3xs (secure) database driver for
Miranda IM: the free IM client for Microsoft* Windows*

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

#include <m_clist.h>
#include <m_icolib.h>

typedef HRESULT (WINAPI *PFNDwmIsCompositionEnabled)(BOOL *);

int EncReadFile(HANDLE hFile,void* data,unsigned int toread,unsigned int* read,void* ov);
int EncWriteFile(HANDLE hFile,void* data,unsigned int towrite,unsigned int* written,void* ov);
int EncGetPassword(void* dbh,const char* dbase);
INT_PTR DB3XSSetPassword(WPARAM wParam, LPARAM lParam);
int DB3XSRemovePassword(WPARAM wParam, LPARAM lParam);
int dbxOnLoad();
int dbxMenusInit(WPARAM wParam, LPARAM lParam);
void xModifyMenu(HANDLE hMenu, TCHAR* name);
void LanguageChanged(hDlg);
HMODULE hDwmApi;
PFNDwmIsCompositionEnabled dwmIsCompositionEnabled;

extern int g_secured;
extern HINSTANCE g_hInst;
extern HANDLE hSetPwdMenu;

#endif //_SECURE_DB__
