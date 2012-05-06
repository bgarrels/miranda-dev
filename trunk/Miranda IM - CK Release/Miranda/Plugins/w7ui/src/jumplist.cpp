/*
W7ui plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2009 nullbie, persei

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

#include "headers.h"

static HANDLE hProcessJumpList = 0;

static void ProcessJumpListImpl(char *arg)
{
	char *prefix = mir_strdup(arg);
	char *argument = strchr(prefix, ':');
	if (argument) *argument++ = 0;
	NotifyEventHooks(hProcessJumpList, (WPARAM)prefix, (LPARAM)argument);
	mir_free(prefix);
}

static LRESULT CALLBACK MirandaJumpListProcessorWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_COPYDATA:
		{
			COPYDATASTRUCT *data = (COPYDATASTRUCT *)lParam;
			ProcessJumpListImpl((char *)data->lpData);
			break;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

extern "C" __declspec(dllexport) void ProcessJumpList(HWND, HINSTANCE, LPSTR arg, UINT)
{
	char miranda_path[MAX_PATH];
	GetModuleFileNameA(g_hInst, miranda_path, SIZEOF(miranda_path));
	lstrcpyA(strstr(_strlwr(miranda_path), "plugins\\w7ui.dll"), "miranda32.exe");

	if (HWND hwnd = FindWindowA("MirandaJumpListProcessor", miranda_path))
	{
		COPYDATASTRUCT data = {0};
		data.dwData = 0;
		data.cbData = lstrlenA(arg) + 1;
		data.lpData = arg;
		SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)&data);
	} else
	{
		char command[MAX_PATH * 2];
		wsprintfA(command, "\"%s\" -jump %s", miranda_path, arg);
		WinExec(command, SW_SHOWNORMAL);
	}
}

static int OnJumpListItems(WPARAM, LPARAM lParam)
{
	WCHAR *category = (WCHAR *)lParam;

	if (!category)
	{
		MJumpList_AddItem(SKINICON_STATUS_ONLINE,    TranslateT("Online"),         L"status", L"online");
		MJumpList_AddItem(SKINICON_STATUS_DND,       TranslateT("Do not disturb"), L"status", L"dnd");
		MJumpList_AddItem(SKINICON_STATUS_INVISIBLE, TranslateT("Invisible"),      L"status", L"invisible");
		MJumpList_AddItem(SKINICON_STATUS_OFFLINE,   TranslateT("Offline"),        L"status", L"offline");
		return 0;
	}

	return 0;
}

static int OnJumpListProcess(WPARAM wParam, LPARAM lParam)
{
	char *prefix = (char *)wParam;
	char *argument = (char *)lParam;

	if (!lstrcmpA(prefix, "status"))
	{
		if (!lstrcmpA(argument, "online"))
		{
			CallService(MS_CLIST_SETSTATUSMODE, ID_STATUS_ONLINE, 0);
		} else
		if (!lstrcmpA(argument, "dnd"))
		{
			CallService(MS_CLIST_SETSTATUSMODE, ID_STATUS_DND, 0);
		} else
		if (!lstrcmpA(argument, "invisible"))
		{
			CallService(MS_CLIST_SETSTATUSMODE, ID_STATUS_INVISIBLE, 0);
		} else
		if (!lstrcmpA(argument, "offline"))
		{
			CallService(MS_CLIST_SETSTATUSMODE, ID_STATUS_OFFLINE, 0);
		}
		return 1;
	}

	return 0;
}

void LoadJumpList()
{
	hProcessJumpList = CreateHookableEvent(ME_JUMPLIST_PROCESS);
}

void InitJumpList()
{
	HookEvent(ME_JUMPLIST_BUILDITEMS, OnJumpListItems);
	HookEvent(ME_JUMPLIST_PROCESS, OnJumpListProcess);

	if (char *cmd = strstr(GetCommandLineA(), " -jump "))
		ProcessJumpListImpl(cmd + 7);

	WNDCLASSEX wcx = {0};
	wcx.cbSize = sizeof(wcx);
	wcx.lpfnWndProc = MirandaJumpListProcessorWndProc;
	wcx.hInstance = g_hInst;
	wcx.lpszClassName = _T("MirandaJumpListProcessor");
	RegisterClassEx(&wcx);

	char miranda_path[MAX_PATH];
	GetModuleFileNameA(g_hInst, miranda_path, SIZEOF(miranda_path));
	lstrcpyA(strstr(_strlwr(miranda_path), "plugins\\w7ui.dll"), "miranda32.exe");

	HWND hwnd = CreateWindowA("MirandaJumpListProcessor", miranda_path, 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_DESKTOP, NULL, g_hInst, NULL);
	ShowWindow(hwnd, SW_HIDE);

	CJumpListBuilder::Rebuild();
}
