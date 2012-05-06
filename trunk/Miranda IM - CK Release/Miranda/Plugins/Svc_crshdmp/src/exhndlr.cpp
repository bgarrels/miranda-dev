/*
Crash Dumper plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2008 - 2012 Boris Krasnovskiy All Rights Reserved

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

#include "utils.h"
#include "crtdbg.h"

static PVOID exchndlr, exchndlrv;
static pfnExceptionFilter  threadfltr;
static PEXCEPTION_POINTERS lastptr;

static HMODULE hKernel = GetModuleHandle(TEXT("kernel32.dll")); 

tAddVectoredExceptionHandler pAddVectoredExceptionHandler = (tAddVectoredExceptionHandler)GetProcAddress(hKernel, "AddVectoredExceptionHandler");
tRemoveVectoredExceptionHandler pRemoveVectoredExceptionHandler = (tRemoveVectoredExceptionHandler)GetProcAddress(hKernel, "RemoveVectoredExceptionHandler");
tRtlCaptureContext pRtlCaptureContext = (tRtlCaptureContext)GetProcAddress(hKernel, "RtlCaptureContext");

void SetExceptionHandler(void)
{
	//	if (pAddVectoredExceptionHandler && !exchndlrv)
	//		exchndlrv = pAddVectoredExceptionHandler(0, myfilterv);
	/*exchndlr = */ SetUnhandledExceptionFilter(myfilter);
}

void RemoveExceptionHandler(void)
{
	if (pRemoveVectoredExceptionHandler && exchndlrv)
		pRemoveVectoredExceptionHandler(exchndlrv);
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)exchndlr);
	exchndlr = NULL;
	exchndlrv = NULL;
}

void UnloadDbgHlp(void)
{
#ifdef _MSC_VER
#if _MSC_VER > 1200
	__FUnloadDelayLoadedDLL2("dbghelp.dll");
#else
	__FUnloadDelayLoadedDLL("dbghelp.dll");
#endif
#endif
}

int myDebugFilter(unsigned int code, PEXCEPTION_POINTERS ep)
{
	if (code ==	VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND) ||
		code == VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND))
	{
		PDelayLoadInfo dlld = (PDelayLoadInfo)ep->ExceptionRecord->ExceptionInformation[0];

		char str[256];
		int off = mir_snprintf(str, SIZEOF(str), "dbghelp.dll v.5.0 or later required to provide a crash report\n");
		off += mir_snprintf(str+off, SIZEOF(str)-off, "Missing Module: %s ", dlld->szDll);

		if (dlld->dlp.fImportByName)
			mir_snprintf(str+off, SIZEOF(str)-off, "Function: %s ", dlld->dlp.szProcName);
		else
			mir_snprintf(str+off, SIZEOF(str)-off, "Ordinal: %x ", dlld->dlp.dwOrdinal);

		MessageBoxA(NULL, str, "Miranda Crash Dumper", MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_TOPMOST);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}


void myfilterWorker(PEXCEPTION_POINTERS exc_ptr, bool notify)
{
	TCHAR path[MAX_PATH];
	SYSTEMTIME st; 
	HANDLE hDumpFile = NULL;

	GetLocalTime(&st);
	CreateDirectoryTree(CrashLogFolder);

	__try 
	{
		crs_sntprintf(path, MAX_PATH, TEXT("%s\\crash%02d%02d%02d%02d%02d%02d.mdmp"), CrashLogFolder, 
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		hDumpFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hDumpFile != INVALID_HANDLE_VALUE) 
			CreateMiniDump(hDumpFile, exc_ptr);
		else if (GetLastError() != ERROR_ALREADY_EXISTS)
			MessageBox(NULL, TranslateT("Crash Report write location is inaccesible"),
			TEXT("Miranda Crash Dumper"), MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_TOPMOST);

	} 
	__except(EXCEPTION_EXECUTE_HANDLER) {}

	bool empty = GetFileSize(hDumpFile, NULL) == 0;
	CloseHandle(hDumpFile);
	if (empty) DeleteFile(path);

	__try 
	{
		crs_sntprintf(path, MAX_PATH, TEXT("%s\\crash%02d%02d%02d%02d%02d%02d.txt"), CrashLogFolder, 
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		hDumpFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

		crs_sntprintf(path, MAX_PATH, TranslateT("Miranda crashed. Crash report stored in the folder:\n %s\n\n Would you like store it in the clipboard as well?"), CrashLogFolder); 

		if (hDumpFile != INVALID_HANDLE_VALUE) 
			CreateCrashReport(hDumpFile, exc_ptr, notify ? path : NULL);
	} 
	__except(myDebugFilter(GetExceptionCode(), GetExceptionInformation())) {}

	bool empty1 = GetFileSize(hDumpFile, NULL) == 0;
	CloseHandle(hDumpFile);
	if (empty1) DeleteFile(path);

	UnloadDbgHlp();
}

LONG WINAPI myfilter(PEXCEPTION_POINTERS exc_ptr)
{
	if (exc_ptr == lastptr) return EXCEPTION_EXECUTE_HANDLER;
	lastptr = exc_ptr;

	myfilterWorker(exc_ptr, true);

	return exchndlr ? ((LPTOP_LEVEL_EXCEPTION_FILTER)exchndlr)(exc_ptr) : EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI myfilterv(PEXCEPTION_POINTERS exc_ptr)
{
	if (0xC0000000L <= exc_ptr->ExceptionRecord->ExceptionCode && 0xC0000500L >= exc_ptr->ExceptionRecord->ExceptionCode)
	{
		if (exc_ptr == lastptr) return EXCEPTION_EXECUTE_HANDLER;
		lastptr = exc_ptr;

		myfilterWorker(exc_ptr, true);
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

DWORD MirandaThreadFilter(DWORD code, EXCEPTION_POINTERS* info)
{
	if (info != lastptr)
	{
		lastptr = info;
		myfilterWorker(info, true);
	}
	return threadfltr(code, info);
}

#if _MSC_VER >= 1400
void InvalidParameterHandler(const wchar_t*, const wchar_t*, const wchar_t*, unsigned int, UINT_PTR)
{
	EXCEPTION_RECORD         ExceptionRecord = {0};
	CONTEXT                  ContextRecord = {0};
	EXCEPTION_POINTERS info = { &ExceptionRecord, &ContextRecord };

	if (pRtlCaptureContext)
		pRtlCaptureContext(&ContextRecord);
	else
	{
		ContextRecord.ContextFlags = CONTEXT_ALL;
		GetThreadContext(GetCurrentThread(), &ContextRecord);
	}

#if defined(_AMD64_)
	ExceptionRecord.ExceptionAddress = (PVOID)ContextRecord.Rip;
#elif defined(_IA64_)
	ExceptionRecord.ExceptionAddress = (PVOID)ContextRecord.BrRp;
#else
	ExceptionRecord.ExceptionAddress = (PVOID)ContextRecord.Eip;
#endif

	ExceptionRecord.ExceptionCode  = STATUS_INVALID_CRUNTIME_PARAMETER;
	ExceptionRecord.ExceptionFlags = EXCEPTION_NONCONTINUABLE;

	myfilterWorker(&info, true);
}
#endif

void InitExceptionHandler(void)
{
#if _MSC_VER >= 1400
	_set_invalid_parameter_handler(InvalidParameterHandler);
#endif
	threadfltr = Miranda_SetExceptFilter(MirandaThreadFilter);
	SetExceptionHandler();
}

void DestroyExceptionHandler(void)
{
	Miranda_SetExceptFilter(threadfltr);
	RemoveExceptionHandler();
}

