#pragma once
#include "IExport.h"
class RichHtmlExport :
	public IExport
{
private:
	int groupId;
	std::wstring folder;
	std::wstring folderName;
	std::string baseProto;
	stdext::hash_set<std::wstring> smileys;
	std::wstring ReplaceSmileys(bool isMe, const std::wstring &msg, bool &isUrl);
public:
	virtual const TCHAR* GetExt()
	{
		return _T("html");
	}
	
	virtual void WriteHeader(const std::wstring &fileName, const std::wstring &filterName, const std::wstring &myName, const std::wstring &myId, const std::wstring &name1, const std::wstring &proto1, const std::wstring &id1, const std::string& baseProto1);
	virtual void WriteFooter();
	virtual void WriteGroup(bool isMe, const std::wstring &time, const std::wstring &user, const std::wstring &eventText, int ico);
	virtual void WriteMessage(bool isMe, int ico, const std::wstring &longDate, const std::wstring &shortDate, const std::wstring &user, const std::wstring &message);

	virtual ~RichHtmlExport();
};

