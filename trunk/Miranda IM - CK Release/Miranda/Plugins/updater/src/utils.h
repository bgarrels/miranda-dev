#ifndef _UTILS_INC
#define _UTILS_INC

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

#include "xmldata.h"

bool VersionFromString(const char *szVer, DWORD *pdwVer);
int CheckForFileID(char *update_url, char *version_url, char *name);
bool CreatePath(const TCHAR *path);
bool IsAdminRequired(void);
void GetRootDir(TCHAR *szPath);

// must 'free' return val
TCHAR *GetTString(const char *asc);
// use system default codepage - called from external process where langpack codepage is not (easily) accessible

void RemoveFolder(const TCHAR *src_folder);
bool FolderIsEmpty(const TCHAR *folder);
bool DeleteNonDlls(const TCHAR *folder);

#endif
