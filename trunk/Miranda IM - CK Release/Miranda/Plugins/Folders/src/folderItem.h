#ifndef M_FOLDERS_FOLDER_ITEM_H
#define M_FOLDERS_FOLDER_ITEM_H

/*
Custom profile folders plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			 Copyright � 2005 Cristian Libotean

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

#include <string.h>
#include <malloc.h>
#include <windows.h>

#define FOLDERS_NO_HELPER_FUNCTIONS
#include "m_folders.h"
#undef FOLDERS_NO_HELPER_FUNCTIONS


#define FOLDER_SUCCESS 1
#define FOLDER_FAILURE 0

class CFolderItem{
	protected:
		char szSection[FOLDERS_NAME_MAX_SIZE];
		char szName[FOLDERS_NAME_MAX_SIZE]; //don't forget to modify in m_folders.h
		union{
			char *szFormat;
			wchar_t *szFormatW;
		};
		union{
			char *szOldFormat;
			wchar_t *szOldFormatW;
		};
		DWORD flags;

		void GetDataFromDatabase(const char *szNotFound);
		void WriteDataToDatabase();
		
		int FolderCreateDirectory(int showFolder = 0);
		int FolderDeleteOldDirectory(int showFolder = 0);
		
	public:
		CFolderItem(const char *sectionName, const char *name, const char *format, const DWORD flags);
		virtual ~CFolderItem();
		void Expand(char *buffer, int size);
		void Save();
		
		int IsUnicode() const;
		int IsEqual(const CFolderItem *other);
		int IsEqual(const char *section, const char *name);
		int IsEqualTranslated(const char *trSection, const char *trName);
		int operator ==(const CFolderItem *other);

		const char *GetSection() const;
		const char *GetName() const;
		const char *GetFormat() const;
		const wchar_t *GetFormatW() const;
		void SetFormat(const char *newFormat);
		void SetFormatW(const wchar_t *newFormat);
};

typedef CFolderItem *PFolderItem;

#endif //M_FOLDERS_FOLDER_ITEM_H