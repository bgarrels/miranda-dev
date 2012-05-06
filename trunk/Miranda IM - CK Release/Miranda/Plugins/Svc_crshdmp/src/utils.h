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

#define MIRANDA_VER 0x0A00

#include <m_stdhdr.h>
#include "sdkstuff.h"

#ifdef _MSC_VER
#include <delayimp.h>
#endif

#include <stdio.h>

#include "resource.h"

#include <newpluginapi.h>

#ifdef _MSC_VER

#pragma warning( push )
#pragma warning( disable : 4201 4100 )
#include <m_database.h>
#pragma warning( pop )

#else

#include <m_database.h>

#endif

#ifdef __GNUC__
#endif

#include <m_system.h>
#include <m_utils.h>
#include <m_langpack.h>
#include <m_clist.h>
#include <m_skin.h>
#include <m_hotkeys.h>
#include <m_protocols.h>
#include <m_help.h>

#include "bkstring.h"

#define MS_PROTO_ENUMPROTOS        "Proto/EnumProtos"

int  crs_sntprintf(TCHAR *buffer, size_t count, const TCHAR* fmt, ...);

#define crsi_u2a(dst, src) \
{ \
	int cbLen = WideCharToMultiByte(CP_ACP, 0, src, -1, NULL, 0, NULL, NULL); \
	dst = (char*)alloca(cbLen+1); \
	WideCharToMultiByte(CP_ACP, 0, src, -1, dst, cbLen, NULL, NULL); \
}

#define crsi_a2u(dst, src, alloc) \
{ \
	int cbLen = MultiByteToWideChar(CP_ACP, 0, src, -1, NULL, 0); \
	dst = (wchar_t*)alloc(sizeof(wchar_t) * (cbLen+1)); \
	MultiByteToWideChar(CP_ACP, 0, src, -1, dst, cbLen); \
}

#ifdef _UNICODE

#define crsi_t2a(d,s) crsi_u2a(d,s)
#define crsi_a2t(d,s) crsi_a2u(d,s,alloca)
#define crs_a2t(d,s) crsi_a2u(d,s,mir_alloc)

#else

#define crsi_t2a(d,s) (d=s)
#define crsi_a2t(d,s) (d=s)
#define crs_a2t(d,s) (d=mir_strdup(s))

#endif

#define SIZEOF(X) (sizeof(X)/sizeof(X[0]))

#define MS_CRASHDUMPER_STORETOFILE "CrashDmp/StoreVerInfoToFile"
#define MS_CRASHDUMPER_STORETOCLIP "CrashDmp/StoreVerInfoToClip"
#define MS_CRASHDUMPER_GETINFO     "Versioninfo/GetInfo"
#define MS_CRASHDUMPER_VIEWINFO    "CrashDmp/ViewInfo"
#define MS_CRASHDUMPER_UPLOAD      "CrashDmp/UploadInfo"
#define MS_CRASHDUMPER_URL         "CrashDmp/StartUrl"

#define PluginName "Crash Dumper"

#define VI_FLAG_FORMAT  1
#define VI_FLAG_PRNVAR  2
#define VI_FLAG_PRNDLL  4
#define VI_FLAG_WEATHER 8

struct VerTrnsfr
{
	char* buf;
	bool  autot;
};

extern HMODULE hInst;
extern DWORD mirandaVersion;
extern LCID packlcid;
extern bool servicemode; 
extern bool clsdates;

extern TCHAR CrashLogFolder[MAX_PATH];
extern TCHAR VersionInfoFolder[MAX_PATH];

void WriteBBFile(bkstring& buffer, bool hdr);
void WriteUtfFile(HANDLE hDumpFile, char* bufu);
void UnloadDbgHlp(void);

LONG WINAPI myfilter(PEXCEPTION_POINTERS exc_ptr);
LONG WINAPI myfilterv(PEXCEPTION_POINTERS exc_ptr);
DWORD MirandaThreadFilter(DWORD code, EXCEPTION_POINTERS* info);

void GetOSDisplayString(bkstring& buffer);
void GetInternetExplorerVersion(bkstring& buffer);
void GetProcessorString(bkstring& buffer);
void GetFreeMemoryString(bkstring& buffer);
void GetFreeDiskString(LPCTSTR dirname, bkstring& buffer);
void GetAdminString(bkstring& buffer);
void GetLanguageString(bkstring& buffer);
void GetLanguagePackString(bkstring& buffer);
void GetWow64String(bkstring& buffer);
void GetVersionInfo(HMODULE hLib, bkstring& buffer);

void GetISO8061Time(SYSTEMTIME* stLocal, LPTSTR lpszString, DWORD dwSize);

void ReadableExceptionInfo(PEXCEPTION_RECORD excrec, bkstring& buffer);

void GetLastWriteTime(LPCTSTR fileName, LPTSTR lpszString, DWORD dwSize);
void GetLastWriteTime(FILETIME* ftime, LPTSTR lpszString, DWORD dwSize);
bool CreateDirectoryTree(LPTSTR szDir);
void StoreStringToClip(bkstring& buffer);
void ShowMessage(int type, const TCHAR* format, ...);
bool IsPluginEnabled(TCHAR* filename);

PLUGININFO* GetMirInfo(HMODULE hModule);
const PLUGININFOEX* GetPluginInfoEx(void);

void CreateMiniDump   (HANDLE hDumpFile, PEXCEPTION_POINTERS exc_ptr);
void CreateCrashReport(HANDLE hDumpFile, PEXCEPTION_POINTERS exc_ptr, const TCHAR* msg);
void PrintVersionInfo(bkstring& buffer, unsigned flags = VI_FLAG_PRNVAR);
bool ProcessVIHash(bool store);

void InitExceptionHandler(void);
void DestroyExceptionHandler(void);
void SetExceptionHandler(void);
void RemoveExceptionHandler(void);
void CheckForOtherCrashReportingPlugins(void);

INT_PTR CALLBACK DlgProcOptions(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgProcView(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void DestroyAllWindows(void);

void UploadInit(void); 
void UploadClose(void); 
void OpenAuthUrl(const char* url);
void __cdecl VersionInfoUploadThread(void* arg);

void InitIcons(void);
HICON LoadIconEx(const char* name, bool big = false);
HANDLE GetIconHandle(const char* name);
void ReleaseIconEx(const char* name);
void ReleaseIconEx(HICON hIcon);
