#ifndef _SECURE_DB__
#define _SECURE_DB__

/*
SecureDB for 
Miranda IM: the free IM client for Microsoft* Windows*

Authors
            Copyright (C) 2005 Piotr Pawluczuk (piotrek@piopawlu.net)
            Copyright (C) 2001-2005  Richard Hughes

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

int EncReadFile(HANDLE hFile,void* data,unsigned long toread,unsigned long* read,void* ov);
int EncWriteFile(HANDLE hFile,void* data,unsigned long towrite,unsigned long* written,void* ov);
int EncGetPassword(void* dbh,const char* dbase);
void LanguageChanged(HWND hDlg);
//int EncInitMenus(WPARAM wParam, LPARAM lParam);
//int EncOnLoad();

#endif //_SECURE_DB__