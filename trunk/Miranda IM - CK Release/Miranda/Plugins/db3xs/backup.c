/*

Miranda IM: the free IM client for Microsoft* Windows*

Copyright 2000-2008 Miranda ICQ/IM project, 
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
*/

#include "commonheaders.h"
#include "database.h"

extern HANDLE hDbFile;
extern CRITICAL_SECTION csDbAccess;
struct DBHeader dbHeader;

INT_PTR BackupService(WPARAM wParam, LPARAM lParam);

static HANDLE hEventBackup;

int InitBackups()
{
	CreateServiceFunction(MS_DB_BACKUP,BackupService);	
	hEventBackup = CreateHookableEvent(ME_DB_BACKUPED);
	return 0;
}

int Backup(char* backup_filename, HWND progress_dialog)
{
	HANDLE hBackupFile;
	char buff[8192];
	DWORD bytes_read, bytes_written, file_size, total_bytes_copied = 0;
	HWND prog;
	MSG msg;
	DWORD start_time = GetTickCount();
	prog = GetDlgItem(progress_dialog, 0xDEAD);
	
	hBackupFile = CreateFileA(backup_filename, GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	
	if(hBackupFile != INVALID_HANDLE_VALUE) {
		if(progress_dialog) {
			SetDlgItemText(progress_dialog, 0xDAED, (LPCTSTR)TranslateT("Copying database file..."));
			SendMessage(prog, PBM_SETPOS, (WPARAM)(int)(0), 0);
			UpdateWindow(progress_dialog);
		}

		EnterCriticalSection(&csDbAccess);

		file_size = GetFileSize(hDbFile, 0);

		if(file_size == 0) {
			LeaveCriticalSection(&csDbAccess);
			CloseHandle(hBackupFile);
			DestroyWindow(progress_dialog);
			return 1;
		}

		SetFilePointer(hDbFile, sizeof(dbHeader), 0, FILE_BEGIN);

		if(WriteFile(hBackupFile,&dbHeader,sizeof(dbHeader),&bytes_written,NULL) == TRUE) {
			total_bytes_copied += bytes_written;
			if(progress_dialog) {
				SendMessage(prog, PBM_SETPOS, (WPARAM)(int)(100.0 * total_bytes_copied / file_size), 0);
				UpdateWindow(progress_dialog);
			}
		} else {
			LeaveCriticalSection(&csDbAccess);
			CloseHandle(hBackupFile);
			return 1;
		}

		while(ReadFile(hDbFile, buff, sizeof(buff), &bytes_read, 0) == TRUE && bytes_read > 0
			&& GetWindowLongPtr(progress_dialog, GWLP_USERDATA) == 0)
		{
			if(!WriteFile(hBackupFile, buff, bytes_read, &bytes_written, 0)) {
				LeaveCriticalSection(&csDbAccess);
				CloseHandle(hBackupFile);
				return 1;
			}
			total_bytes_copied += bytes_written;

			if(progress_dialog) {

				if(GetTickCount() - start_time >= SHOW_PROGRESS_TIME)
					ShowWindow(progress_dialog, SW_SHOW);

				while(PeekMessage(&msg, progress_dialog, 0, 0, PM_REMOVE) != 0) {
					if(!IsDialogMessage(progress_dialog, &msg)) {
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}

				SendMessage(prog, PBM_SETPOS, (WPARAM)(int)(100.0 * total_bytes_copied / file_size), 0);
				UpdateWindow(progress_dialog);
			}
		}
		LeaveCriticalSection(&csDbAccess);

		CloseHandle(hBackupFile);

		if(progress_dialog && GetWindowLongPtr(progress_dialog, GWLP_USERDATA) != 0)
			DeleteFileA(backup_filename);
		else
			NotifyEventHooks(hEventBackup, (WPARAM)backup_filename, 0);

		return 0;
	}
	
	return 1;
}

INT_PTR BackupService(WPARAM wParam,LPARAM lParam)
{
	return Backup((char*)wParam, (HWND)lParam);	
}