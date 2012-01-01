#pragma once
class IExport
{
protected:
	std::wostream* stream;
public:

	void SetStream(std::wostream *str)
	{
		stream = str;
	}

	virtual const TCHAR* GetExt() = 0;
	virtual void WriteHeader(const std::wstring &fileName, const std::wstring &filterName, const std::wstring &myName, const std::wstring &myId, const std::wstring &name1, const std::wstring &proto1, const std::wstring &id1, const std::string& baseProto1) = 0;
	virtual void WriteFooter() = 0;
	virtual void WriteGroup(bool isMe, const std::wstring &time, const std::wstring &user, const std::wstring &eventText, int ico) = 0;
	virtual void WriteMessage(bool isMe, int ico, const std::wstring &longDate, const std::wstring &shortDate, const std::wstring &user, const std::wstring &message) = 0;

	virtual ~IExport()
	{
	}
};

