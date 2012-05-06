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

#include "common.h"
#include "utils.h"

#include <unzip.h>

extern "C" void fill_memory_filefunc64 (zlib_filefunc64_def* pzlib_filefunc_def);

bool extractCurrentFile(unzFile uf, TCHAR *path)
{
    int err = UNZ_OK;
    unz_file_info64 file_info;
    char filename[MAX_PATH];
	char buf[8192];

    err = unzGetCurrentFileInfo64(uf, &file_info, filename, sizeof(filename), buf, sizeof(buf), NULL, 0);
    if (err != UNZ_OK) return false;

	// Get Unicode file name for InfoZip style archives, otherwise assume PKZip/WinZip style
	if (file_info.size_file_extra)
	{
		char *p = buf; 
		unsigned long size = min(file_info.size_file_extra, sizeof(buf));
		while (size > 0)
		{
			unsigned short id =  *(unsigned short*)p;
			unsigned len =  *(unsigned short*)(p + 2);
			
			if (size < (len + 4)) break;

			if (id == 0x7075 && len > 5 && (len - 5) < sizeof(filename) && *(p + 4) == 1)
			{
				memcpy(filename, p + 9, len - 5);
				filename[len - 5] = 0;
				break;
			}
			size -= len + 4;
			p += len + 4;
		}
	}

	TCHAR save_file[MAX_PATH];
	TCHAR* p = mir_utf8decodeT(filename);
	if (p == NULL) p = mir_a2t(filename);
	mir_sntprintf(save_file, SIZEOF(save_file), _T("%s\\%s"), path, p);
	mir_free(p);

	for (p = save_file; *p; ++p) if (*p == '/') *p = '\\'; 

	if (file_info.external_fa & FILE_ATTRIBUTE_DIRECTORY)
		CreatePath(save_file);
	else
	{
		err = unzOpenCurrentFile(uf);
		if (err != UNZ_OK) return false;

		p = _tcsrchr(save_file, '\\'); if (p) *p = 0;
		CreatePath(save_file);
		if (p) *p = '\\';

		HANDLE hFile = CreateFile(save_file, GENERIC_WRITE, FILE_SHARE_WRITE, 0, 
			CREATE_ALWAYS, file_info.external_fa, 0);
		
		if (hFile != INVALID_HANDLE_VALUE)
		{
			for (;;)
			{
				err = unzReadCurrentFile(uf, buf, sizeof(buf));
				if (err <= 0) break;

				DWORD bytes;
				if (!WriteFile(hFile, buf, err, &bytes, FALSE))
				{
					err = UNZ_ERRNO;
					break;
				}
			}

			FILETIME ftLocal, ftCreate, ftLastAcc, ftLastWrite;
			GetFileTime(hFile, &ftCreate, &ftLastAcc, &ftLastWrite);
			DosDateTimeToFileTime(HIWORD(file_info.dosDate), LOWORD(file_info.dosDate), &ftLocal);
			LocalFileTimeToFileTime(&ftLocal, &ftLastWrite);
			SetFileTime(hFile, &ftCreate, &ftLastAcc, &ftLastWrite);

			CloseHandle(hFile);
			unzCloseCurrentFile(uf); /* don't lose the error */
		}
    }
	return true;
}

void unzip_mem(char* buf, int len, TCHAR* dest)
{
	zlib_filefunc64_def ffunc;
	fill_memory_filefunc64(&ffunc);

	char zipfile[128];
	mir_snprintf(zipfile, sizeof(zipfile), "%p+%x", buf, len);

	unzFile uf = unzOpen2_64(zipfile, &ffunc);
	if (uf)
	{
		do {
			extractCurrentFile(uf, dest);
		}
		while (unzGoToNextFile(uf) == UNZ_OK);
		unzClose(uf);
	}
}
