#include "StdAfx.h"
#include "TxtExport.h"
#define EXP_FILE (*stream)

TxtExport::~TxtExport()
{
}

void TxtExport::WriteHeader(const std::wstring &fileName, const std::wstring &filterName, const std::wstring &myName, const std::wstring &myId, const std::wstring &name1, const std::wstring &proto1, const std::wstring &id1, const std::string& baseProto1)
{
	TCHAR* start = TranslateT("###");
	EXP_FILE << start << "\n" << start << _T(" ") << TranslateT("History Log") << _T("\n");
	EXP_FILE << start << _T(" ") << myName << _T(" (") << proto1 << _T(": ") << myId << _T(") - ") << name1 << _T(" (") << proto1 << _T(": ") << id1 << _T(")\n");
	EXP_FILE << start << _T(" ") << TranslateT("Filter:") << _T(" ") << filterName << _T("\n") << start << _T("\n");
}

void TxtExport::WriteFooter()
{
}

void TxtExport::WriteGroup(bool isMe, const std::wstring &time, const std::wstring &user, const std::wstring &eventText, int ico)
{
}

void TxtExport::WriteMessage(bool isMe, int ico, const std::wstring &longDate, const std::wstring &shortDate, const std::wstring &user, const std::wstring &message)
{
	EXP_FILE << "\n[" << longDate << "] " << user << ":\n" << message << "\n";
}
