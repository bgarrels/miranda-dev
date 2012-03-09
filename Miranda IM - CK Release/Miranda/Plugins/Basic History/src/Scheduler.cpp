/*
Basic History plugin
Copyright (C) 2011-2012 Krzysztof Kral

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "StdAfx.h"
#include "Options.h"
#include "ExportManeger.h"
#include "zip\zip.h"
#include "zip\iowin32.h"

// Sorry for plain C implementation
void DoTask(TaskOptions& to);
bool IsValidTask(TaskOptions& to, std::list<TaskOptions>* top = NULL, std::wstring* err = NULL, std::wstring* errDescr = NULL);
std::wstring GetFileName(const std::wstring &baseName, std::wstring contactName, std::map<std::wstring, bool>& existingContacts, bool replaceContact);
std::wstring GetDirectoryName(const std::wstring &path);
std::wstring GetName(const std::wstring &path);
bool DeleteDirectory(LPCTSTR lpszDir, bool noRecycleBin = true);
void ListDirectory(const std::wstring &basePath, const std::wstring &path, std::list<std::wstring>& files);
std::wstring ReplaceStr(const std::wstring& str, wchar_t oldCh, wchar_t newCh);
time_t GetNextExportTime(TaskOptions& to);
void SchedulerThreadFunc(void*);
volatile bool finishThread = false;
bool initTask = false;
HANDLE thread = NULL;
HANDLE threadEvent;
time_t nextExportTime;
void StartThread(bool init);
void StopThread();
bool GetNextExportTime(bool init);
bool ExecuteCurrentTask(time_t now);
void GetZipFileTime(const TCHAR *file, uLong *dt);
std::wstring ReplaceExt(const std::wstring& file, const TCHAR* ext);
bool ZipFiles(const std::wstring& dir, std::wstring zipFilePath);
void FtpFiles(const std::wstring& dir, const std::wstring& filePath, const std::wstring& ftpName);

void OptionsSchedulerChanged()
{
	StartThread(false);
}

void InitScheduler()
{
	StartThread(true);
}

void DeinitScheduler()
{
	StopThread();
}

int DoLastTask(WPARAM, LPARAM)
{
	for(std::vector<TaskOptions>::iterator it = Options::instance->taskOptions.begin(); it != Options::instance->taskOptions.end(); ++it)
	{
		if(it->trigerType == TaskOptions::AtEnd && it->active)
		{
			DoTask(*it);
		}
	}

	return 0;
}

bool IsValidTask(TaskOptions& to, std::list<TaskOptions>* top, std::wstring* err, std::wstring* errDescr)
{
	if(to.taskName.empty())
	{
		if(err != NULL)
			*err = TranslateT("Name");
		return false;
	}
	if(top != NULL)
	{
		for(std::list<TaskOptions>::iterator it = top->begin(); it != top->end(); ++it)
		{
			if(it->taskName == to.taskName)
			{
				if(err != NULL)
					*err = TranslateT("Name");
				return false;
			}
		}
	}
	if(!to.isSystem && to.contacts.size() == 0)
	{
		if(err != NULL)
			*err = TranslateT("Contacts");
		if(errDescr != NULL)
			*errDescr = TranslateT("At least one contact should be selected.");
		return false;
	}
	if(to.filterId > 1)
	{
		int filter = 0;
		
		for(int i = 0; i < Options::instance->customFilters.size(); ++i)
		{
			if(to.filterName == Options::instance->customFilters[i].name)
			{
				filter = i + 2;
				break;
			}
		}

		if(filter < 2)
		{
			if(err != NULL)
				*err = TranslateT("Filter");
			return false;
		}

		to.filterId = filter;
	}
	else if(to.filterId < 0)
	{
		if(err != NULL)
			*err = TranslateT("Filter");
		return false;
	}
	if(to.type == TaskOptions::Delete)
	{
		return true;
	}

	if(!Options::FTPAvail() && to.useFtp)
	{
		if(err != NULL)
			*err = TranslateT("Upload to FTP");
		return false;
	}
	if(to.filePath.empty())
	{
		if(err != NULL)
			*err = TranslateT("Path to output file");
		return false;
	}
	if(to.useFtp && to.ftpName.empty())
	{
		if(err != NULL)
			*err = TranslateT("Session name");
		if(errDescr != NULL)
			*errDescr = TranslateT("To create session open WinSCP, click New Session, enter data and save with specific name. Remember if FTP server using password you should save it in WinSCP.");
		return false;
	}
	if(to.useFtp && (to.filePath.find(_T('\\')) < to.filePath.length() || to.filePath.find(_T(':')) < to.filePath.length() || to.filePath[0] != L'/'))
	{
		if(err != NULL)
			*err = TranslateT("Path to output file");
		if(errDescr != NULL)
			*errDescr = TranslateT("FTP path must contains '/' instead '\\' and starts from '/'.");
		return false;
	}
	if(to.exportType < IExport::RichHtml || to.exportType > IExport::Txt)
	{
		if(err != NULL)
			*err = TranslateT("Export to");
		return false;
	}
	if((to.trigerType == TaskOptions::Daily || to.trigerType == TaskOptions::Weekly || to.trigerType == TaskOptions::Monthly) && (to.dayTime < 0 || to.dayTime >= 24 * 60))
	{
		if(err != NULL)
			*err = TranslateT("Time");
		return false;
	}
	if(to.trigerType == TaskOptions::Weekly && (to.dayOfWeek < 0 || to.dayOfWeek >= 7))
	{
		if(err != NULL)
			*err = TranslateT("Day of week");
		return false;
	}
	if(to.trigerType == TaskOptions::Monthly && (to.dayOfMonth <= 0 || to.dayOfMonth >= 32))
	{
		if(err != NULL)
			*err = TranslateT("Day");
		return false;
	}
	if((to.trigerType == TaskOptions::DeltaMin || to.trigerType == TaskOptions::DeltaHour) && (to.deltaTime < 0 || to.deltaTime >= 10000))
	{
		if(err != NULL)
			*err = TranslateT("Delta time");
		return false;
	}

	return true;
}

void DoTask(TaskOptions& to)
{
	if(!IsValidTask(to))
		return;

	DWORD now = time(NULL);
	long long int t = to.eventDeltaTime * 60;
	if(to.eventUnit > TaskOptions::Minute)
		t *= 60LL;
	if(to.eventUnit > TaskOptions::Hour)
		t *= 24LL;
	if(t > 2147483647LL)
		return;

	bool error = false;
	std::list<ExportManeger*> managers;
	if(to.type == TaskOptions::Delete)
	{
		if(to.isSystem)
		{
			ExportManeger *exp = new ExportManeger(NULL, to.filterId);
			exp->SetDeleteWithoutExportEvents(t, now);
			managers.push_back(exp);
		}

		for(int i = 0; i < to.contacts.size(); ++i)
		{
			ExportManeger *exp = new ExportManeger(to.contacts[i], to.filterId);
			exp->SetDeleteWithoutExportEvents(t, now);
			managers.push_back(exp);
		}
	}
	else
	{
		std::map<std::wstring, bool> existingContacts;
		std::wstring filePath = to.filePath;
		std::wstring dir;
		if(!to.useFtp && !to.compress)
		{
			dir = GetDirectoryName(filePath);
			if(!dir.empty())
			{
				CreateDirectory(dir.c_str(), NULL);
			}
		}
		else
		{
			filePath = GetName(filePath);
			TCHAR temp[MAX_PATH];
			temp[0] = 0;
			GetTempPath(MAX_PATH, temp);
			dir = temp;
			dir += filePath;
			dir = GetFileName(dir, L"", existingContacts, true);
			dir = ReplaceExt(dir, L"");
			size_t pos = dir.find_last_of(_T('.'));
			if(pos < dir.length())
			{
				dir = dir.substr(0, pos);
			}

			DeleteDirectory(dir.c_str());
			CreateDirectory(dir.c_str(), NULL);
			filePath = dir + L"\\" + filePath;
		}
		if(to.isSystem)
		{
			ExportManeger *exp = new ExportManeger(NULL, to.filterId);
			exp->SetAutoExport(GetFileName(filePath, exp->GetContactName(), existingContacts, true), t, now);
			if(!exp->Export(to.exportType))
			{
				error = true;
			}

			if(to.type == TaskOptions::Export)
			{
				delete exp;
			}
			else
			{
				managers.push_back(exp);
			}
		}

		if(!error)
		{
			for(int i = 0; i < to.contacts.size(); ++i)
			{
				ExportManeger *exp = new ExportManeger(to.contacts[i], to.filterId);
				exp->SetAutoExport(GetFileName(filePath, exp->GetContactName(), existingContacts, true), t, now);
				if(!exp->Export(to.exportType))
				{
					error = true;
					break;
				}

				if(to.type == TaskOptions::Export)
				{
					delete exp;
				}
				else
				{
					managers.push_back(exp);
				}
			}
		}

		if(error)
		{
			if(to.compress && !to.useFtp)
			{
				DeleteDirectory(dir.c_str());
			}
		}
		else if(to.compress)
		{
			std::wstring zipFilePath = to.filePath;
			std::wstring zipDir = dir;
			if(!to.useFtp)
			{
				zipDir = GetDirectoryName(zipFilePath);
				if(!zipDir.empty())
				{
					CreateDirectory(zipDir.c_str(), NULL);
				}
			}
			else
			{
				zipFilePath = GetName(zipFilePath);
				TCHAR temp[MAX_PATH];
				temp[0] = 0;
				GetTempPath(MAX_PATH, temp);
				zipDir = temp;
				zipDir += L"zip<date>";
				zipDir = GetFileName(zipDir, L"", existingContacts, true);
				DeleteDirectory(zipDir.c_str());
				CreateDirectory(zipDir.c_str(), NULL);
				zipFilePath = zipDir + L"\\" + zipFilePath;
			}
			error = ZipFiles(dir + L"\\", zipFilePath);
			dir = zipDir;
		}

		if(to.useFtp)
		{
			if(!error)
			{
				FtpFiles(dir, to.filePath, to.ftpName);
			}
		
			DeleteDirectory(dir.c_str());
		}
	}
	
	if(to.type != TaskOptions::Export)
	{
		for(std::list<ExportManeger*>::iterator it = managers.begin(); it != managers.end(); ++it)
		{
			(*it)->DeleteExportedEvents();
			delete *it;
		}
	}
}

std::wstring GetFileName(const std::wstring &baseName, std::wstring contactName, std::map<std::wstring, bool>& existingContacts, bool replaceContact)
{
	std::wstring str = baseName;
	size_t pos = baseName.find(_T("<contact>"));
	if(replaceContact && pos < baseName.length())
	{
		str = baseName.substr(0, pos);
		std::wstring baseName1 = contactName;
		if(!baseName1.empty())
		{
			std::wstring name = baseName1;
			int i = 0;
			TCHAR buf[32];
			std::map<std::wstring, bool>::iterator it = existingContacts.find(name);
			while(it != existingContacts.end())
			{
				name = baseName1 + _itot(++i, buf, 10);
				it = existingContacts.find(name);
			}

			str += name;
			existingContacts[name] = true;
		}
		str += baseName.substr(pos + 9);
	}

	pos = str.find(_T("<date>"));
	if(pos < str.length())
	{
		TCHAR time[256];
		SYSTEMTIME st;
		GetLocalTime(&st);
		_stprintf_s(time, _T("%d-%02d-%02d %02d%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
		std::wstring str1 = str.substr(0, pos);
		str1 += time;
		str1 += str.substr(pos + 6);
		str = str1;
	}

	return str;
}

std::wstring GetDirectoryName(const std::wstring &path)
{
	size_t find = path.find_last_of(L"\\/");
	if(find < path.length())
	{
		return path.substr(0, find);
	}

	return L"";
}

void ListDirectory(const std::wstring &basePath, const std::wstring &path, std::list<std::wstring>& files)
{   
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((basePath + path + _T("*")).c_str(), &findFileData);
	if (hFind == INVALID_HANDLE_VALUE) 
		return;
	do
	{
		if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			std::wstring name = findFileData.cFileName;
			if(name != L"." && name != L"..")
				ListDirectory(basePath, path + findFileData.cFileName + _T("\\"), files);
		}
		else
		{
			files.push_back(path + findFileData.cFileName);
		}
	}
	while(FindNextFile(hFind, &findFileData));
	FindClose(hFind);
}

std::wstring ReplaceStr(const std::wstring& str, wchar_t oldCh, wchar_t newCh)
{
	std::wstring ret;
	size_t start = 0;
	size_t find;
	while((find = str.find_first_of(oldCh, start)) < str.length())
	{
		ret += str.substr(start, find - start);
		ret += newCh;
		start = find + 1;
	}
	
	ret += str.substr(start, str.length() - start);
	return ret;
}

time_t GetNextExportTime(TaskOptions& to)
{
	switch(to.trigerType)
	{
	case TaskOptions::Daily:
	{
		tm t;
		localtime_s(&t, &to.lastExport);
		t.tm_hour = to.dayTime/60;
		t.tm_min = to.dayTime%60;
		t.tm_sec = 0;
		time_t newTime = mktime(&t);
		if(newTime <= to.lastExport)
		{
			newTime += 60 * 60 * 24;
		}

		return newTime;
	}
	case TaskOptions::Weekly:
	{
		tm t;
		localtime_s(&t, &to.lastExport);
		t.tm_hour = to.dayTime/60;
		t.tm_min = to.dayTime%60;
		t.tm_sec = 0;
		int dow = (to.dayOfWeek + 1) % 7;
		time_t newTime = mktime(&t);
		while(dow != t.tm_wday)
		{
			newTime += 60 * 60 * 24;
			localtime_s(&t, &newTime);
			newTime = mktime(&t);
		}

		if(newTime <= to.lastExport)
		{
			newTime += 7 * 60 * 60 * 24;
		}

		return newTime;
	}
	case TaskOptions::Monthly:
	{
		tm t;
		localtime_s(&t, &to.lastExport);
		t.tm_hour = to.dayTime/60;
		t.tm_min = to.dayTime%60;
		t.tm_sec = 0;
		time_t newTime = mktime(&t);
		int lastM = t.tm_mon;
		int lastD;
		while(to.dayOfMonth != t.tm_mday || newTime <= to.lastExport)
		{
			lastD = t.tm_mday;
			newTime += 60 * 60 * 24;
			localtime_s(&t, &newTime);
			newTime = mktime(&t);
			if(to.dayOfMonth > 28 && t.tm_mon != lastM && (newTime - 60 * 60 * 24) > to.lastExport)
			{
				lastM = t.tm_mon;
				if(to.dayOfMonth > lastD)
				{
					newTime -= 60 * 60 * 24;
					break;
				}
			}
		}

		return newTime;
	}
	case TaskOptions::DeltaMin:
		return to.lastExport + to.deltaTime * 60;
	case TaskOptions::DeltaHour:
		return to.lastExport + to.deltaTime * 60 * 60;
	default:
		return to.lastExport;
	}
}

void SchedulerThreadFunc(void*)
{
	if(initTask)
	{
		WaitForSingleObject(threadEvent, 5 * 1000);
		if(!finishThread)
		{
			time_t now = time(NULL);
			if(!ExecuteCurrentTask(now))
				return;
		}
	}

	while(!finishThread)
	{
		DWORD timeWait;
		time_t now = time(NULL);
		while(nextExportTime <= now)
		{
			if(!ExecuteCurrentTask(now))
				return;
		}

		time_t dif = nextExportTime - now;
		timeWait = (dif > 60 * 60 * 24) ? (60 * 60 * 1000) : (60 * 1000);

		WaitForSingleObject(threadEvent, timeWait);
	}
}

void StartThread(bool init)
{
	StopThread();
	
	initTask = false;
	bool isExport = GetNextExportTime(init);
	if(isExport)
	{
		finishThread = false;
		threadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		thread = mir_forkthread(SchedulerThreadFunc, NULL);
	}
}

void StopThread()
{
	if(thread != NULL)
	{
		finishThread = true;
		SetEvent(threadEvent);
		WaitForSingleObject(thread, INFINITE);
		//CloseHandle(thread);
		CloseHandle(threadEvent);
		thread = NULL;
		threadEvent = NULL;
	}
}

bool GetNextExportTime(bool init)
{
	EnterCriticalSection(&Options::instance->criticalSection);
	bool isExport = false;
	for(std::vector<TaskOptions>::iterator it = Options::instance->taskOptions.begin(); it != Options::instance->taskOptions.end(); ++it)
	{
		if(it->active && it->trigerType != TaskOptions::AtStart && it->trigerType != TaskOptions::AtEnd)
		{
			time_t t = GetNextExportTime(*it);
			if(isExport)
			{
				if(t < nextExportTime)
					nextExportTime = t;
			}
			else
			{
				nextExportTime = t;
				isExport = true;
				initTask = init;
			}
		}
		else if(it->active && it->trigerType == TaskOptions::AtStart && init)
		{
			nextExportTime = GetNextExportTime(*it);
			isExport = true;
			initTask = true;
		}
	}
	
	LeaveCriticalSection(&Options::instance->criticalSection);
	return isExport;
}

bool ExecuteCurrentTask(time_t now)
{
	EnterCriticalSection(&Options::instance->criticalSection);
	TaskOptions to;
	bool isExport = false;
	for(std::vector<TaskOptions>::iterator it = Options::instance->taskOptions.begin(); it != Options::instance->taskOptions.end(); ++it)
	{
		if(it->active && it->trigerType != TaskOptions::AtStart && it->trigerType != TaskOptions::AtEnd && !initTask)
		{
			time_t t = GetNextExportTime(*it);
			if(t <= now)
			{
				it->lastExport = time(NULL);
				Options::instance->SaveTaskTime(*it);
				to = *it;
				isExport = true;
				break;
			}
		}
		else if(it->active && it->trigerType == TaskOptions::AtStart && initTask)
		{
			it->lastExport = time(NULL);
			Options::instance->SaveTaskTime(*it);
			to = *it;
			isExport = true;
			break;
		}
	}

	LeaveCriticalSection(&Options::instance->criticalSection);
	
	initTask = false;
	if(isExport)
	{
		DoTask(to);
	}

	return GetNextExportTime(false);
}

void GetZipFileTime(const TCHAR *file, uLong *dt)
{
	FILETIME ftLocal;
	HANDLE hFind;
	WIN32_FIND_DATA ff32;

	hFind = FindFirstFile(file, &ff32);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
		FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
		FindClose(hFind);
	}
}

bool ZipFiles(const std::wstring& dir, std::wstring zipFilePath)
{
	std::list<std::wstring> files;
	std::map<std::wstring, bool> existingContacts;
	ListDirectory(dir, L"", files);
	bool error = false;
	if(files.size() > 0)
	{
		zlib_filefunc_def pzlib_filefunc_def;
		fill_win32_filefunc(&pzlib_filefunc_def);
		zipFilePath = GetFileName(zipFilePath, L"", existingContacts, true);
		zipFilePath = ReplaceExt(zipFilePath, L"zip");
		zipFile zf = zipOpen2((LPCSTR)(LPTSTR)zipFilePath.c_str(), APPEND_STATUS_CREATE, NULL, &pzlib_filefunc_def);
		if (zf != NULL)
		{
			TCHAR buf[1024];
			char bufF[MAX_PATH + 20];
			while(files.size() > 0)
			{
				std::wstring zipDir = *files.begin();
				std::wstring localDir = dir + L"\\" + zipDir;
				zip_fileinfo zi = {0};	
				GetZipFileTime(localDir.c_str(), &zi.dosDate);
				BOOL badChar = FALSE;
				WideCharToMultiByte(437, WC_NO_BEST_FIT_CHARS, zipDir.c_str(), -1, bufF, MAX_PATH + 20, NULL, &badChar);
				int flag = 0;
				if(badChar)
				{
					flag = 0x800; // UTF
					WideCharToMultiByte(CP_UTF8, 0, zipDir.c_str(), -1, bufF, MAX_PATH + 20, NULL, NULL);
				}
				int err = zipOpenNewFileInZip4_64 (zf, bufF, &zi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0,
                                -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, 0, 0, flag, 0);
				if (err == ZIP_OK)
				{
					HANDLE hFile = CreateFile(localDir.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
					if(hFile != INVALID_HANDLE_VALUE)
					{
						DWORD readed;
						do
						{
							err = ZIP_OK;
							if(!ReadFile(hFile, buf, 1024, &readed, NULL))
							{
								error = true;
								break;
							}
								
							if (readed > 0)
							{
								err = zipWriteInFileInZip(zf, buf, readed);
							}
						} 
						while ((err == ZIP_OK) && (readed > 0));
						CloseHandle(hFile);
					}
						
					if(zipCloseFileInZip(zf) != ZIP_OK)
					{
						error = true;
						break;
					}
				}
				else
				{
					error = true;
					break;
				}

				files.pop_front();
			}

			zipClose(zf, NULL);
		}
		else
		{
			error = true;
		}
	}

	DeleteDirectory(dir.c_str());
	return error;
}

void FtpFiles(const std::wstring& dir, const std::wstring& filePath, const std::wstring& ftpName)
{
	std::list<std::wstring> files;
	std::map<std::wstring, bool> existingContacts;
	ListDirectory(dir, L"\\", files);
	if(files.size() > 0)
	{
		std::wofstream stream ((dir + _T("\\script.sc")).c_str());
		if(stream.is_open())
		{
			std::wstring ftpDir = GetDirectoryName(filePath);
			ftpDir = GetFileName(ftpDir, L"", existingContacts, false);
			stream << "option batch continue\noption confirm off\nopen \""
				<< ftpName << "\"\noption transfer binary\n";
			std::wstring lastCD;
			while(files.size() > 0)
			{
				std::wstring localDir = *files.begin();
				std::wstring currentCD = ftpDir + GetDirectoryName(ReplaceStr(localDir, L'\\', L'/'));
				if(currentCD != lastCD)
				{
					if(!currentCD.empty() && currentCD != L"/")
						stream << "mkdir \"" << currentCD << "\"\n";
					stream << "cd \"" << currentCD << "\"\n";
					lastCD = currentCD;
				}
					
				stream << "put \"." << localDir << "\"\n";
				files.pop_front();
			}

			stream.close();
			std::wstring &log = Options::instance->ftpLogPath;
			CreateDirectory(GetDirectoryName(log).c_str(), NULL);
			DeleteFile(log.c_str());
			TCHAR cmdLine[MAX_PATH];
			_stprintf_s(cmdLine, _T("\"%s\" /nointeractiveinput /log=\"%s\" /script=script.sc"), Options::instance->ftpExePath.c_str(), log.c_str());
			STARTUPINFO				startupInfo = {0};
			PROCESS_INFORMATION		processInfo;
			startupInfo.cb			= sizeof(STARTUPINFO);
			if(CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, dir.c_str(), &startupInfo, &processInfo))
			{
				WaitForSingleObject(processInfo.hProcess, INFINITE);
				CloseHandle(processInfo.hThread);
				CloseHandle(processInfo.hProcess);
			}
		}
	}
}
