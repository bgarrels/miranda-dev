#ifndef _EXTERN_INC
#define _EXTERN_INC

/*
Updater plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	2005-2006 Scott Ellis
							2009-2012 Boris Krasnovskiy

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

#include "options.h"


#define ROOT_FILES_FOLDER		_T("root_files")

// returns 1 if any error, 0 if shutdown is imminent
int ExternProcess(bool restart);

// move all files in src_folder to backup_folder - put replaced files in backup folder
// if a file in src_folder is a directory, copy it's contents to the same dir in the root folder and
// set that dir as the new root (so that dirs in dirs go in the right place)
void MoveFiles(HANDLE hLogFile, TCHAR *src_folder, TCHAR *dst_folder, TCHAR *backup_folder, TCHAR *root_folder);


void CALLBACK ExternalUpdate(HWND hwnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow);

#endif
