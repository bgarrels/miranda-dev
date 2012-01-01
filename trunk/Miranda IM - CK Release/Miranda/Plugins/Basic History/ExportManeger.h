#pragma once
#include "EventList.h"
#include "IExport.h"
class ExportManeger : public EventList
{
private:
	IExport* exp;
protected:
	virtual void AddGroup(bool isMe, const std::wstring &time, const std::wstring &user, const std::wstring &eventText, int ico);
public:
	ExportManeger(HANDLE _hContact, int filter);
	enum ExportType
	{
		Txt,
		PlainHtml,
		RichHtml
	};

	void Export(ExportType type);
};

