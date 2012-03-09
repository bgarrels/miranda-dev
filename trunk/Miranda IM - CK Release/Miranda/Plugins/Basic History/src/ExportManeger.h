#pragma once
#include "EventList.h"
#include "IExport.h"
class ExportManeger : public EventList
{
private:
	IExport* exp;
	std::wstring file;
protected:
	virtual void AddGroup(bool isMe, const std::wstring &time, const std::wstring &user, const std::wstring &eventText, int ico);
public:
	ExportManeger(HANDLE _hContact, int filter);

	void SetAutoExport(const std::wstring _file, int _deltaTime, DWORD _now)
	{
		file = _file;
		deltaTime = _deltaTime;
		now = _now;
	}

	bool Export(IExport::ExportType type);
	void SetDeleteWithoutExportEvents(int _deltaTime, DWORD _now);
	void DeleteExportedEvents();
};

