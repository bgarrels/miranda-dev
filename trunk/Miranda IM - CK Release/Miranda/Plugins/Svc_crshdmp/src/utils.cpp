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
#include <lm.h>

static HMODULE hKernel = GetModuleHandle(TEXT("kernel32.dll")); 

tGetNativeSystemInfo pGetNativeSystemInfo = (tGetNativeSystemInfo)GetProcAddress(hKernel, "GetNativeSystemInfo");
tGetProductInfo pGetProductInfo = (tGetProductInfo) GetProcAddress(hKernel, "GetProductInfo");
tGlobalMemoryStatusEx pGlobalMemoryStatusEx = (tGlobalMemoryStatusEx) GetProcAddress(hKernel, "GlobalMemoryStatusEx");
tGetUserDefaultUILanguage pGetUserDefaultUILanguage = (tGetUserDefaultUILanguage) GetProcAddress(hKernel, "GetUserDefaultUILanguage");
tGetSystemDefaultUILanguage pGetSystemDefaultUILanguage = (tGetSystemDefaultUILanguage) GetProcAddress(hKernel, "GetSystemDefaultUILanguage");
tIsWow64Process pIsWow64Process = (tIsWow64Process) GetProcAddress(hKernel, "IsWow64Process");
tIsProcessorFeaturePresent pIsProcessorFeaturePresent = (tIsProcessorFeaturePresent) GetProcAddress(hKernel, "IsProcessorFeaturePresent");

#ifdef _UNICODE
tGetDiskFreeSpaceEx pGetDiskFreeSpaceEx = (tGetDiskFreeSpaceEx) GetProcAddress(hKernel, "GetDiskFreeSpaceExW");
#else
tGetDiskFreeSpaceEx pGetDiskFreeSpaceEx = (tGetDiskFreeSpaceEx) GetProcAddress(hKernel, "GetDiskFreeSpaceExA");
#endif


void CheckForOtherCrashReportingPlugins(void)
{
	HMODULE hModule = GetModuleHandle(TEXT("attache.dll"));
	if (hModule == NULL)
		hModule = GetModuleHandle(TEXT("crashrpt.dll"));
	if (hModule == NULL)
		hModule = GetModuleHandle(TEXT("crashdmp.dll"));

	if (hModule == NULL) return;

	MessageBox(NULL, TranslateT("More then one crash reporting plugin installed. This will result in inability of creating crash reports"),
		TEXT("Miranda Crash Dumper"), MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_TOPMOST);
}

void GetOSDisplayString(bkstring& buffer)
{
	OSVERSIONINFOEX osvi = {0};
	SYSTEM_INFO si = {0};
	BOOL bOsVersionInfoEx;
	DWORD dwType = 0;

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);
	if (!bOsVersionInfoEx)
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO*)&osvi)) 
			return;
	}

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
	pGetNativeSystemInfo = (tGetNativeSystemInfo)GetProcAddress(hKernel, "GetNativeSystemInfo");
	if (NULL != pGetNativeSystemInfo) pGetNativeSystemInfo(&si);
	else GetSystemInfo(&si);

	if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4)
	{
		buffer.append(TEXT("Operating System: Microsoft "));

		// Test for the specific product.
		if (osvi.dwMajorVersion == 6)
		{
			switch (osvi.dwMinorVersion)
			{
			case 0:
				if (osvi.wProductType == VER_NT_WORKSTATION)
					buffer.append(TEXT("Windows Vista "));
				else 
					buffer.append(TEXT("Windows Server 2008 "));
				break;

			case 1:
				if (osvi.wProductType == VER_NT_WORKSTATION)
					buffer.append(TEXT("Windows 7 "));
				else 
					buffer.append(TEXT("Windows Server 2008 R2 "));
				break;

			default:
				if (osvi.wProductType == VER_NT_WORKSTATION)
					buffer.append(TEXT("Windows 8 "));
				else 
					buffer.append(TEXT("Windows Server 2012 "));
				break;
			}

			pGetProductInfo = (tGetProductInfo) GetProcAddress(hKernel, "GetProductInfo");
			if (pGetProductInfo != NULL) pGetProductInfo(6, 0, 0, 0, &dwType);

			switch(dwType)
			{
			case PRODUCT_ULTIMATE:
				buffer.append(TEXT("Ultimate Edition"));
				break;
			case PRODUCT_HOME_PREMIUM:
				buffer.append(TEXT("Home Premium Edition"));
				break;
			case PRODUCT_HOME_BASIC:
				buffer.append(TEXT("Home Basic Edition"));
				break;
			case PRODUCT_ENTERPRISE:
				buffer.append(TEXT("Enterprise Edition"));
				break;
			case PRODUCT_BUSINESS:
				buffer.append(TEXT("Business Edition"));
				break;
			case PRODUCT_STARTER:
				buffer.append(TEXT("Starter Edition"));
				break;
			case PRODUCT_CLUSTER_SERVER:
				buffer.append(TEXT("Cluster Server Edition"));
				break;
			case PRODUCT_DATACENTER_SERVER:
				buffer.append(TEXT("Datacenter Edition"));
				break;
			case PRODUCT_DATACENTER_SERVER_CORE:
				buffer.append(TEXT("Datacenter Edition (core installation)"));
				break;
			case PRODUCT_ENTERPRISE_SERVER:
				buffer.append(TEXT("Enterprise Edition"));
				break;
			case PRODUCT_ENTERPRISE_SERVER_CORE:
				buffer.append(TEXT("Enterprise Edition (core installation)"));
				break;
			case PRODUCT_ENTERPRISE_SERVER_IA64:
				buffer.append(TEXT("Enterprise Edition for Itanium-based Systems"));
				break;
			case PRODUCT_SMALLBUSINESS_SERVER:
				buffer.append(TEXT("Small Business Server"));
				break;
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
				buffer.append(TEXT("Small Business Server Premium Edition"));
				break;
			case PRODUCT_STANDARD_SERVER:
				buffer.append(TEXT("Standard Edition"));
				break;
			case PRODUCT_STANDARD_SERVER_CORE:
				buffer.append(TEXT("Standard Edition (core installation)"));
				break;
			case PRODUCT_WEB_SERVER:
				buffer.append(TEXT("Web Server Edition"));
				break;
			}

			if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
				buffer.append(TEXT(", 64-bit"));
			else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL)
				buffer.append(TEXT(", 32-bit"));
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
		{
			if (GetSystemMetrics(SM_SERVERR2))
				buffer.append(TEXT("Windows Server 2003 R2, "));
			else if (osvi.wSuiteMask==VER_SUITE_STORAGE_SERVER)
				buffer.append(TEXT("Windows Storage Server 2003"));
			else if (osvi.wProductType == VER_NT_WORKSTATION && 
				si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				buffer.append(TEXT("Windows XP Professional x64 Edition"));
			else buffer.append(TEXT("Windows Server 2003, "));

			// Test for the server type.
			if (osvi.wProductType != VER_NT_WORKSTATION)
			{
				if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64)
				{
					if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
						buffer.append(TEXT("Datacenter Edition for Itanium-based Systems"));
					else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
						buffer.append(TEXT("Enterprise Edition for Itanium-based Systems"));
				}

				else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
				{
					if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
						buffer.append(TEXT("Datacenter x64 Edition"));
					else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
						buffer.append(TEXT("Enterprise x64 Edition"));
					else buffer.append(TEXT("Standard x64 Edition"));
				}

				else
				{
					if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
						buffer.append(TEXT("Compute Cluster Edition"));
					else if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
						buffer.append(TEXT("Datacenter Edition"));
					else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
						buffer.append(TEXT("Enterprise Edition"));
					else if (osvi.wSuiteMask & VER_SUITE_BLADE)
						buffer.append(TEXT("Web Edition"));
					else buffer.append(TEXT("Standard Edition"));
				}
			}
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
		{
			buffer.append(TEXT("Windows XP "));
			if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
				buffer.append(TEXT("Home Edition"));
			else 
				buffer.append(TEXT("Professional"));
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
		{
			buffer.append(TEXT("Windows 2000 "));

			if (osvi.wProductType == VER_NT_WORKSTATION)
			{
				buffer.append(TEXT("Professional"));
			}
			else 
			{
				if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
					buffer.append(TEXT("Datacenter Server"));
				else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					buffer.append(TEXT("Advanced Server"));
				else buffer.append(TEXT("Server"));
			}
		}
		if (_tcslen(osvi.szCSDVersion) > 0)
		{
			buffer.append(TEXT(" "));
			buffer.append(osvi.szCSDVersion);
		}

		buffer.appendfmt(TEXT(" (build %d)"), osvi.dwBuildNumber);
	}
	else
	{
		if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId)
		{
			buffer.append(TEXT("Microsoft Windows NT "));
			if (osvi.wProductType == VER_NT_WORKSTATION)
				buffer.append(TEXT("Workstation 4.0 "));
			else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
				buffer.append(TEXT("Server 4.0, Enterprise Edition "));
			else 
				buffer.append(TEXT("Server 4.0 "));
		}

		if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && osvi.dwMajorVersion == 4)
		{
			if (osvi.dwMinorVersion == 0)
			{
				buffer.append(TEXT("Microsoft Windows 95 "));
				if (osvi.szCSDVersion[1]==TEXT('C') || osvi.szCSDVersion[1]==TEXT('B'))
					buffer.append(TEXT("OSR2 "));
			} 

			if (osvi.dwMinorVersion == 10)
			{
				buffer.append(TEXT("Microsoft Windows 98 "));
				if (osvi.szCSDVersion[1]==TEXT('A') || osvi.szCSDVersion[1]==TEXT('B'))
					buffer.append(TEXT("SE "));
			} 

			if (osvi.dwMinorVersion == 90)
			{
				buffer.append(TEXT("Microsoft Windows Millennium Edition"));
			} 
			buffer.appendfmt(TEXT("(build %d)"), LOWORD(osvi.dwBuildNumber));
		}
		else if (osvi.dwPlatformId == VER_PLATFORM_WIN32s)
		{
			buffer.append(TEXT("Microsoft Win32s"));
		}
	}
}

int GetTZOffset(void)
{
	TIME_ZONE_INFORMATION tzInfo = {0};
	DWORD type = GetTimeZoneInformation(&tzInfo);

	int offset = 0;
	switch (type)
	{
	case TIME_ZONE_ID_DAYLIGHT:
		offset = -(tzInfo.Bias + tzInfo.DaylightBias);
		break;

	case TIME_ZONE_ID_STANDARD:
		offset = -(tzInfo.Bias + tzInfo.StandardBias);
		break;

	case TIME_ZONE_ID_UNKNOWN:
		offset = -tzInfo.Bias;
		break;
	}
	return offset;
}

void GetISO8061Time(SYSTEMTIME* stLocal, LPTSTR lpszString, DWORD dwSize)
{
	SYSTEMTIME loctime;
	if (stLocal == NULL) 
	{
		stLocal = &loctime;
		GetLocalTime(stLocal);
	}

	if (clsdates)
	{
		GetDateFormat(LOCALE_INVARIANT, 0, stLocal, TEXT("d MMM yyyy"), lpszString, dwSize);
		int dlen = (int)_tcslen(lpszString);
		GetTimeFormat(LOCALE_INVARIANT, 0, stLocal, TEXT(" H:mm:ss"), lpszString+dlen, dwSize-dlen);
	}
	else
	{
		int offset = GetTZOffset();

		// Build a string showing the date and time.
		crs_sntprintf(lpszString, dwSize, TEXT("%d-%02d-%02d %02d:%02d:%02d%+03d%02d"), 
			stLocal->wYear, stLocal->wMonth, stLocal->wDay, 
			stLocal->wHour, stLocal->wMinute, stLocal->wSecond,
			offset / 60, offset % 60);
	}
}

void GetLastWriteTime(FILETIME* ftime, LPTSTR lpszString, DWORD dwSize)
{
	FILETIME ftLocal;
	SYSTEMTIME stLocal;

	// Convert the last-write time to local time.
	FileTimeToLocalFileTime(ftime, &ftLocal);
	FileTimeToSystemTime(&ftLocal, &stLocal);

	GetISO8061Time(&stLocal, lpszString, dwSize);
}

void GetLastWriteTime(LPCTSTR fileName, LPTSTR lpszString, DWORD dwSize)
{
	WIN32_FIND_DATA FindFileData;

	HANDLE hFind = FindFirstFile(fileName, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) return;
	FindClose(hFind);

	GetLastWriteTime(&FindFileData.ftLastWriteTime, lpszString, dwSize);
}


typedef PLUGININFO * (__cdecl * Miranda_Plugin_Info) (DWORD mirandaVersion);

PLUGININFO* GetMirInfo(HMODULE hModule)
{
	Miranda_Plugin_Info bpi = (Miranda_Plugin_Info)GetProcAddress(hModule, "MirandaPluginInfoEx");
	if (bpi == NULL) bpi = (Miranda_Plugin_Info)GetProcAddress(hModule, "MirandaPluginInfo");
	if (bpi == NULL) return NULL;

	return bpi(mirandaVersion);
}


void GetInternetExplorerVersion(bkstring& buffer)
{
	HKEY hKey;
	DWORD size;

	TCHAR ieVersion[1024] = {0};
	TCHAR ieBuild[512] = {0};
	TCHAR iVer[64] = {0};

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Internet Explorer"), 0, 
		KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		size = sizeof(ieBuild)/sizeof(ieBuild[0]);
		if (RegQueryValueEx(hKey, TEXT("Build"), NULL, NULL, (LPBYTE) ieBuild, &size) != ERROR_SUCCESS)
			ieBuild[0] = 0;

		size = sizeof(ieVersion)/sizeof(ieVersion[0]);
		if (RegQueryValueEx(hKey, TEXT("Version"), NULL, NULL, (LPBYTE) ieVersion, &size) != ERROR_SUCCESS)
			ieVersion[0] = 0;

		size = sizeof(iVer)/sizeof(iVer[0]);
		if (RegQueryValueEx(hKey, TEXT("IVer"), NULL, NULL, (LPBYTE) iVer, &size) != ERROR_SUCCESS)
			iVer[0] = 0;

		RegCloseKey(hKey);
	}

	buffer.append(TEXT("Internet Explorer: "));
	if (ieVersion[0] == 0)
	{
		if (iVer[0] == 0)
			buffer.append(TEXT("<not installed>"));
		else if (_tcscmp(iVer, TEXT("100")) == 0)
			buffer.append(TEXT("1.0"));
		else if (_tcscmp(iVer, TEXT("101")) == 0)
			buffer.append(TEXT("NT"));
		else if (_tcscmp(iVer, TEXT("102")) == 0)
			buffer.append(TEXT("2.0"));
		else if (_tcscmp(iVer, TEXT("103")) == 0)
			buffer.append(TEXT("3.0"));
	}
	else
	{
		buffer.append(ieVersion);
	}
	if (ieBuild[0] != 0)
	{
		buffer.appendfmt(TEXT(" (build %s)"), ieBuild);
	}
}


void TrimMultiSpaces(TCHAR* str)
{
	TCHAR *src = str, *dest = str;
	bool trimst = false;

	for(;;)
	{
		if (*src == TEXT(' '))
		{
			if (!trimst)
			{
				trimst = true;
				*dest++ = *src;
			}
		}
		else
		{
			trimst = false;
			*dest++ = *src;
		}
		if (*src++ == 0) break; 
	}
}				

void GetProcessorString(bkstring& buffer)
{
	HKEY hKey;
	DWORD size;

	TCHAR cpuIdent[512] = {0};
	TCHAR cpuName[512] = {0};

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Hardware\\Description\\System\\CentralProcessor\\0"), 0, 
		KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		size = sizeof(cpuName)/sizeof(cpuName[0]);
		if (RegQueryValueEx(hKey, TEXT("ProcessorNameString"), NULL, NULL, (LPBYTE) cpuName, &size) != ERROR_SUCCESS)
			_tcscpy(cpuName, TEXT("Unknown"));

		size = sizeof(cpuIdent)/sizeof(cpuIdent[0]);
		if (RegQueryValueEx(hKey, TEXT("Identifier"), NULL, NULL, (LPBYTE) cpuIdent, &size) != ERROR_SUCCESS)
			if (RegQueryValueEx(hKey, TEXT("VendorIdentifier"), NULL, NULL, (LPBYTE) cpuIdent, &size) != ERROR_SUCCESS)
				_tcscpy(cpuIdent, TEXT("Unknown"));

		RegCloseKey(hKey);
	}
	TrimMultiSpaces(cpuName);
	buffer.appendfmt(TEXT("CPU: %s [%s]"), cpuName, cpuIdent);

	if (pIsProcessorFeaturePresent && pIsProcessorFeaturePresent(PF_NX_ENABLED))
		buffer.append(TEXT(" [DEP Enabled]"));

	SYSTEM_INFO si = {0};
	GetSystemInfo(&si);

	if (si.dwNumberOfProcessors > 1)
		buffer.appendfmt(TEXT(" [%u CPUs]"), si.dwNumberOfProcessors);
}

void GetFreeMemoryString(bkstring& buffer)
{
	unsigned ram;
	if (pGlobalMemoryStatusEx)
	{
		MEMORYSTATUSEX ms = {0};
		ms.dwLength = sizeof(ms);
		pGlobalMemoryStatusEx(&ms);
		ram = (unsigned int) ((ms.ullTotalPhys / (1024 * 1024)) + 1);
	}
	else
	{
		MEMORYSTATUS ms = {0};
		ZeroMemory(&ms, sizeof(ms));
		ms.dwLength = sizeof(ms);
		GlobalMemoryStatus(&ms);
		ram = (unsigned int)(ms.dwTotalPhys/(1024*1024))+1;
	}
	buffer.appendfmt(TEXT("Installed RAM: %u MBytes"), ram);
}

void GetFreeDiskString(LPCTSTR dirname, bkstring& buffer)
{
	ULARGE_INTEGER tnb, tfb, fs = {0};
	if (pGetDiskFreeSpaceEx)
		pGetDiskFreeSpaceEx(dirname, &fs, &tnb, &tfb);
	else
	{
		DWORD SectorsPerCluster, BytesPerSector;
		DWORD NumberOfFreeClusters, TotalNumberOfClusters;

		GetDiskFreeSpace(dirname, &SectorsPerCluster, &BytesPerSector, 
			&NumberOfFreeClusters, &TotalNumberOfClusters);

		fs.QuadPart = BytesPerSector * SectorsPerCluster;
		fs.QuadPart *= NumberOfFreeClusters;
	}
	fs.QuadPart /= (1024*1024);

	buffer.appendfmt(TEXT("Free disk space on Miranda partition: %u MBytes"), fs.LowPart);
}

void ReadableExceptionInfo(PEXCEPTION_RECORD excrec, bkstring& buffer)
{
	buffer.append(TEXT("Exception: "));

	switch (excrec->ExceptionCode)
	{
	case EXCEPTION_BREAKPOINT:
		buffer.append(TEXT("User Defined Breakpoint"));
		break;

	case EXCEPTION_ACCESS_VIOLATION:
		buffer.append(TEXT("Access Violation"));
		break;

	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		buffer.append(TEXT("Array Bounds Exceeded"));
		break;

	case EXCEPTION_DATATYPE_MISALIGNMENT:
		buffer.append(TEXT("Datatype Misalignment"));
		break;

	case EXCEPTION_FLT_DENORMAL_OPERAND:
		buffer.append(TEXT("Floating Point denormlized operand"));
		break;

	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		buffer.append(TEXT("Floating Point divide by 0"));
		break;

	case EXCEPTION_FLT_INEXACT_RESULT:
		buffer.append(TEXT("Floating Point inexact result"));
		break;

	case EXCEPTION_FLT_INVALID_OPERATION:
		buffer.append(TEXT("Floating Point invalid operation"));
		break;

	case EXCEPTION_FLT_OVERFLOW:
		buffer.append(TEXT("Floating Point overflow"));
		break;

	case EXCEPTION_FLT_STACK_CHECK:
		buffer.append(TEXT("Floating Point stack overflow/underflow"));
		break;

	case EXCEPTION_FLT_UNDERFLOW:
		buffer.append(TEXT("Floating Point underflow"));
		break;

	case EXCEPTION_ILLEGAL_INSTRUCTION:
		buffer.append(TEXT("Invalid instruction executed"));
		break;

	case EXCEPTION_IN_PAGE_ERROR:
		buffer.append(TEXT("Access to the not present page"));
		break;

	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		buffer.append(TEXT("Integer divide by zero"));
		break;

	case EXCEPTION_INT_OVERFLOW:
		buffer.append(TEXT("Integer overflow"));
		break;

	case EXCEPTION_PRIV_INSTRUCTION:
		buffer.append(TEXT("Priveleged instruction executed"));
		break;

	case EXCEPTION_STACK_OVERFLOW:
		buffer.append(TEXT("Stack overflow"));
		break;

	case 0xe06d7363:
		buffer.append(TEXT("Unhandled C++ software exception"));
		break;

	default:
		buffer.appendfmt(TEXT("%x"), excrec->ExceptionCode); 
		break;
	}

	buffer.appendfmt(TEXT(" at address %p."), excrec->ExceptionAddress); 

	if (excrec->ExceptionCode == EXCEPTION_ACCESS_VIOLATION || 
		excrec->ExceptionCode == EXCEPTION_IN_PAGE_ERROR)
	{
		switch(excrec->ExceptionInformation[0])
		{
		case 0:
			buffer.appendfmt(TEXT(" Reading from address %p."), (LPVOID)excrec->ExceptionInformation[1]);
			break;

		case 1:
			buffer.appendfmt(TEXT(" Writing to address %p."), (LPVOID)excrec->ExceptionInformation[1]);
			break;

		case 8:
			buffer.appendfmt(TEXT(" DEP at address %p."), (LPVOID)excrec->ExceptionInformation[1]);
			break;
		}
	}
}

void GetAdminString(bkstring& buffer)
{
	BOOL b;
	__try
	{
		SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
		PSID AdministratorsGroup; 

		b = AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
			DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsGroup); 
		if (b) 
		{
			if (!CheckTokenMembership(NULL, AdministratorsGroup, &b))
				b = FALSE;
			FreeSid(AdministratorsGroup); 
		}
		else
			b = GetLastError() == ERROR_CALL_NOT_IMPLEMENTED;
	}
	__except(EXCEPTION_EXECUTE_HANDLER) 
	{
		b = TRUE;
	}

	buffer.appendfmt(TEXT("Administrator privileges: %s"), b ? TEXT("Yes") : TEXT ("No"));
}

void GetLanguageString(bkstring& buffer)
{
	TCHAR name1[256], name2[256], name3[256], name4[256];

	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, name1, 256);
	GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SENGLANGUAGE, name2, 256);

	if (pGetUserDefaultUILanguage && pGetSystemDefaultUILanguage)
	{
		GetLocaleInfo(MAKELCID(pGetUserDefaultUILanguage(), SORT_DEFAULT), LOCALE_SENGLANGUAGE, name3, 256);
		GetLocaleInfo(MAKELCID(pGetSystemDefaultUILanguage(), SORT_DEFAULT), LOCALE_SENGLANGUAGE, name4, 256);
	}
	else
	{
		_tcscpy(name3, name1);
		_tcscpy(name4, name2);
	}

	buffer.appendfmt(TEXT("OS Languages: (UI | Locale (User/System)) : %s/%s | %s/%s"), name3, name4, name1, name2);
}

void GetLanguagePackString(bkstring& buffer)
{
	buffer.append(TEXT("Language pack: ")); 
	if (packlcid == LOCALE_USER_DEFAULT)
		buffer.append(TEXT("No language pack installed"));
	else
	{
		TCHAR path[MAX_PATH] = TEXT("Locale id invalid");
		GetLocaleInfo(packlcid, LOCALE_SENGLANGUAGE, path, MAX_PATH);
		buffer.append(path);

		GetLocaleInfo(packlcid, LOCALE_SISO3166CTRYNAME, path, MAX_PATH);
		buffer.appendfmt(TEXT(" (%s) [%04x]"), path, packlcid);

		GetModuleFileName(NULL, path, MAX_PATH);

		LPTSTR fname = _tcsrchr(path, TEXT('\\'));
		if (fname == NULL) fname = path;
		crs_sntprintf(fname, MAX_PATH-(fname-path), TEXT("\\langpack_*.txt"));

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = FindFirstFile(path, &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) return;
		FindClose(hFind);

		crs_sntprintf(fname, MAX_PATH-(fname-path), TEXT("\\%s"), FindFileData.cFileName);
		HANDLE hDumpFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hDumpFile != INVALID_HANDLE_VALUE) 
		{
			char buf[8192];

			DWORD bytes = 0;
			ReadFile(hDumpFile, buf, 8190, &bytes, NULL);
			buf[bytes] = 0;

			char *id = strstr(buf, "FLID:");
			if (id != NULL)
			{
				char *endid = strchr(id, '\r');
				if (endid != NULL) *endid = 0;

				endid = strchr(id, '\n');
				if (endid != NULL) *endid = 0;

				TCHAR mirtime[30];
				GetLastWriteTime(path, mirtime, 30);

				TCHAR* tid;
				crsi_a2t(tid, id+5);
				buffer.appendfmt(TEXT(", %s, modified: %s"), tid, mirtime);
			}
			CloseHandle(hDumpFile);
		}
	}
}

void GetWow64String(bkstring& buffer)
{
	BOOL wow64 = 0;
	if (pIsWow64Process)
	{
		if (!pIsWow64Process(GetCurrentProcess(), &wow64))
		{
			wow64 = 0;
		}
	}
	if (wow64) buffer.append(TEXT(" [running inside WOW64]")); 
}


bool CreateDirectoryTree(LPTSTR szDir)
{
	DWORD dwAttr = GetFileAttributes(szDir);
	if (dwAttr != INVALID_FILE_ATTRIBUTES && (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
		return true;

	TCHAR* pszSlash = _tcsrchr(szDir, TEXT('\\'));
	if (pszSlash == NULL)
		return false;

	*pszSlash = 0;
	bool res = CreateDirectoryTree(szDir);
	*pszSlash = TEXT('\\');

	if (res) res = CreateDirectory(szDir, NULL) != 0;

	return res;
}

int crs_sntprintf(TCHAR *buffer, size_t count, const TCHAR* fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	int len = _vsntprintf(buffer, count-1, fmt, va);
	buffer[len] = 0;

	va_end(va);
	return len;
}

void GetVersionInfo(HMODULE hLib, bkstring& buffer)
{
	HRSRC hVersion = FindResource(hLib, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	if (hVersion != NULL)
	{
		HGLOBAL hGlobal = LoadResource(hLib, hVersion); 
		if (hGlobal != NULL)  
		{  
			LPVOID versionInfo  = LockResource(hGlobal);  
			if (versionInfo != NULL)
			{
				int vl = *(unsigned short*)versionInfo;
				unsigned *res = (unsigned*)versionInfo;
				while (*res != 0xfeef04bd && ((char*)res - (char*)versionInfo) < vl) ++res;

				if (((char*)res - (char*)versionInfo) < vl)
				{
					VS_FIXEDFILEINFO *vsInfo = (VS_FIXEDFILEINFO*)res;
					buffer.appendfmt(TEXT(" v.%u.%u.%u.%u"), 
						HIWORD(vsInfo->dwFileVersionMS), LOWORD(vsInfo->dwFileVersionMS),
						HIWORD(vsInfo->dwFileVersionLS), LOWORD(vsInfo->dwFileVersionLS));
				}
			}
			FreeResource(hGlobal);  
		}
	}
}

void StoreStringToClip(bkstring& buffer)
{
	HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, buffer.sizebytes() + sizeof(TCHAR));
	LPSTR buf = (LPSTR)GlobalLock(hData);

	memcpy(buf, buffer.c_str(), buffer.sizebytes() + sizeof(TCHAR));

	GlobalUnlock(hData);

	OpenClipboard(NULL);
	EmptyClipboard();

#ifdef _UNICODE
	SetClipboardData(CF_UNICODETEXT, hData);
#else
	SetClipboardData(CF_TEXT, hData);
#endif
	CloseClipboard();
}

bool IsPluginEnabled(TCHAR* filename)
{
	char* fname;
	crsi_t2a(fname, filename);
	char* ext = strstr(_strlwr(fname), ".dll");
	bool res = ext && ext[4] == '\0' && DBGetContactSettingByte(NULL, "PluginDisable", fname, 0) == 0;
	return res;
}
