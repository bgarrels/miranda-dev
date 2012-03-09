#include "StdAfx.h"
#include "ExportManeger.h"
#include "TxtExport.h"
#include "PlainHtmlExport.h"
#include "RichHtmlExport.h"
#include "Options.h"
#include "codecvt_CodePage.h"

ExportManeger::ExportManeger(HANDLE _hContact, int filter)
	:EventList(_hContact, filter)
{
}

std::wstring GetFile(const TCHAR* ext)
{
	TCHAR filter[512];
	std::locale loc;
	TCHAR extUpper[32];
	_tcscpy_s(extUpper, ext);
	extUpper[0] = std::toupper(ext[0], loc);
	_stprintf_s(filter, TranslateT("%s Files (*.%s)"), extUpper, ext);
	int len = _tcslen(filter) + 1;
	_stprintf_s(filter + len, 512 - len, TranslateT("*.%s"), ext);
	len += _tcslen(filter + len);
	filter[++len] = 0;
	TCHAR stzFilePath[1024];
	_tcscpy_s(stzFilePath, TranslateT("History"));
	_tcscat_s(stzFilePath, _T("."));
	_tcscat_s(stzFilePath, ext);
	len = _tcslen(stzFilePath) + 1;
	stzFilePath[len] = 0;
	OPENFILENAME ofn = {0};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = 0;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = stzFilePath;
	ofn.lpstrTitle = TranslateT("Export");
	ofn.nMaxFile = 1024;
	ofn.lpstrDefExt = ext;
	ofn.Flags = OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	if(GetSaveFileName(&ofn))
	{
		return stzFilePath;
	}

	return L"";
}

std::wstring ReplaceExt(const std::wstring& file, const TCHAR* ext)
{
	size_t pos = file.find(_T("<ext>"));
	if(pos < file.length())
	{
		std::wstring fileName = file.substr(0, pos);
		fileName += ext;
		fileName += file.substr(pos + 5);
		return fileName;
	}

	return file;
}

bool ExportManeger::Export(IExport::ExportType type)
{
	exp = NULL;
	UINT cp;
	std::wstring encoding;
	switch(type)
	{
	case IExport::Txt:
		exp = new TxtExport();
		cp = Options::instance->codepageTxt;
		encoding = Options::instance->encodingTxt;
		break;
	case IExport::PlainHtml:
		exp = new PlainHtmlExport();
		cp = Options::instance->codepageHtml1;
		encoding = Options::instance->encodingHtml1;
		break;
	case IExport::RichHtml:
		exp = new RichHtmlExport();
		cp = Options::instance->codepageHtml2;
		encoding = Options::instance->encodingHtml2;
		break;
	default:
		return false;
	}

	std::wstring fileName;
	if(file.empty())
		fileName = GetFile(exp->GetExt());
	else
	{
		fileName = ReplaceExt(file, exp->GetExt());
	}

	if(fileName.empty())
		return false;

	std::wofstream stream (fileName.c_str());//, std::ios_base::binary);
	if(!stream.is_open())
		return false;
	
	std::locale filelocale(std::locale(), new codecvt_CodePage<wchar_t>(cp));
	stream.imbue(filelocale);
	exp->SetStream(&stream);

	exp->WriteHeader(fileName, GetFilterName(), GetMyName(), GetMyId(), GetContactName(), GetProtocolName(), GetContactId(), GetBaseProtocol(), encoding);

	RefreshEventList();

	exp->WriteFooter();
	stream.close();
	delete exp;
	return true;
}

void ExportManeger::AddGroup(bool isMe, const std::wstring &time, const std::wstring &user, const std::wstring &eventText, int ico)
{
	if(exp == NULL)
		return;
	exp->WriteGroup(isMe, time, user, eventText, ico);
#define MAXSELECTSTR 8184
	TCHAR str[MAXSELECTSTR + 8]; // for safety reason
	DBEVENTINFO dbei = {0};
	DWORD newBlobSize,oldBlobSize;
	dbei.cbSize=sizeof(dbei);
	oldBlobSize = 0;
	str[0] = 0;
	tm lastTime;
	bool isFirst = true;
	bool lastMe = false;
	std::deque<HANDLE> revDeq;
	std::deque<HANDLE>& deq = eventList.back();
	if(Options::instance->messagesNewOnTop)
	{
		revDeq.insert(revDeq.begin(), deq.rbegin(), deq.rend());
		deq = revDeq;
	}
	for(std::deque<HANDLE>::iterator it = deq.begin(); it != deq.end(); ++it)
	{
		HANDLE hDbEvent = *it;
		newBlobSize=CallService(MS_DB_EVENT_GETBLOBSIZE,(WPARAM)hDbEvent,0);
		if ((int)dbei.cbBlob != -1)
		{
			if(newBlobSize>oldBlobSize) 
			{
				dbei.pBlob=(PBYTE)mir_realloc(dbei.pBlob,newBlobSize);
				oldBlobSize=newBlobSize;
			}
			dbei.cbBlob = oldBlobSize;
			if (CallService(MS_DB_EVENT_GET,(WPARAM)hDbEvent,(LPARAM)&dbei) == 0)
			{
				lastMe = dbei.flags & DBEF_SENT;
				int imId;
				GetEventIcon(lastMe, dbei.eventType, imId);

				TCHAR* formatDate = Options::instance->messagesShowSec ? _T("d s") : _T("d t");
				TCHAR* longFormatDate = Options::instance->messagesShowSec ? _T("d s") : _T("d t");
				if(!Options::instance->messagesShowDate)
				{
					if(isFirst)
					{
						isFirst = false;
						formatDate = Options::instance->messagesShowSec ? _T("s") : _T("t");
						time_t tt = dbei.timestamp;
						lastTime = *localtime(&tt);
					}
					else
					{
						time_t tt = dbei.timestamp;
						tm* t = localtime(&tt);
						if(lastTime.tm_yday == t->tm_yday && lastTime.tm_year == t->tm_year)
							formatDate = Options::instance->messagesShowSec ? _T("s") : _T("t");
					}
				}
				
				tmi.printTimeStamp(NULL, dbei.timestamp, longFormatDate, str , MAXSELECTSTR, 0);
				std::wstring longDate = str;
				tmi.printTimeStamp(NULL, dbei.timestamp, formatDate, str , MAXSELECTSTR, 0);
				std::wstring shortDate = str;

				std::wstring user;
				if(lastMe)
					user = myName;
				else
					user = contactName;
				
				GetObjectDescription(&dbei,str, MAXSELECTSTR);
				std::wstring strMessage = str;
				int i = strMessage.length();
				if(i + 1 >= MAXSELECTSTR)
					continue;

				//if(Options::instance->messagesUseSmileys)
				//	ReplaceIcons(editWindow, startAt, lastMe);

				exp->WriteMessage(lastMe, imId, longDate, shortDate, user, strMessage);
			}
		}
	}
	
	mir_free(dbei.pBlob);
}

void ExportManeger::DeleteExportedEvents()
{
	for(int j = 0; j < eventList.size(); ++j)
	{
		for(int i = 0; i < eventList[j].size(); ++i)
		{
			CallService(MS_DB_EVENT_DELETE,(WPARAM)hContact,(LPARAM)(HANDLE)eventList[j][i]);
		}
	}
}

void ExportManeger::SetDeleteWithoutExportEvents(int _deltaTime, DWORD _now)
{
	exp = NULL;
	deltaTime = _deltaTime;
	now = _now;
	RefreshEventList();
}
