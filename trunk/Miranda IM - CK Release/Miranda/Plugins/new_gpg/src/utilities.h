#ifndef UTILITIES_H
#define UTILITIES_H

/*
New_gpg plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author 
			Copyright (C) 2010-2011 sss

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

TCHAR* __stdcall UniGetContactSettingUtf(HANDLE hContact, const char *szModule,const char* szSetting, TCHAR* szDef);
char* __stdcall UniGetContactSettingUtf(HANDLE hContact, const char *szModule,const char* szSetting, char* szDef);
void GetFilePath(TCHAR *WindowTittle, char *szSetting, TCHAR *szExt, TCHAR *szExtDesc);
TCHAR *GetFilePath(TCHAR *WindowTittle, TCHAR *szExt, TCHAR *szExtDesc, bool save_file = false);
void GetFolderPath(TCHAR *WindowTittle, char *szSetting);

void storeOutput(HANDLE ahandle, string *output);

int ComboBoxAddStringUtf(HWND hCombo, const TCHAR *szString, DWORD data);
bool isContactSecured(HANDLE hContact);
bool isContactHaveKey(HANDLE hContact);
bool isGPGConfigured();
const bool StriStr(const char *str, const char *substr);
string toUTF8(wstring str);
wstring toUTF16(string str);
string get_random(int length);
string time_str();

struct db_event : public DBEVENTINFO
{
public:
	db_event(char* msg)
	{
		eventType = EVENTTYPE_MESSAGE;
		flags = 0;
		timestamp = 0;
		szModule = 0;
		cbSize = 0;
		cbBlob = strlen(msg)+1;
		pBlob = (PBYTE)msg;
	}
	db_event(char* msg, DWORD time)
	{
		cbBlob = strlen(msg)+1;
		pBlob = (PBYTE)msg;
		eventType = EVENTTYPE_MESSAGE;
		flags = 0;
		timestamp = time;
		szModule = 0;
		cbSize = 0;
	}
	db_event(char* msg, DWORD time, int type)
	{
		cbBlob = strlen(msg)+1;
		pBlob = (PBYTE)msg;
		if(type)
			eventType = type;
		else
			eventType = EVENTTYPE_MESSAGE;
		flags = 0;
		timestamp = time;
		szModule = 0;
		cbSize = 0;
	}
	db_event(char* msg, int type)
	{
		cbBlob = strlen(msg)+1;
		pBlob = (PBYTE)msg;
		flags = 0;
		if(type)
			eventType = type;
		else
			eventType = EVENTTYPE_MESSAGE;
		timestamp = 0;
		szModule = 0;
		cbSize = 0;
	}
	db_event(char* msg, DWORD time, int type, DWORD _flags)
	{
		cbBlob = strlen(msg)+1;
		pBlob = (PBYTE)msg;
		if(type)
			eventType = type;
		else
			eventType = EVENTTYPE_MESSAGE;
		flags = _flags;
		timestamp = time;
		szModule = 0;
		cbSize = 0;
	}
};
void HistoryLog(HANDLE, db_event);

#endif
