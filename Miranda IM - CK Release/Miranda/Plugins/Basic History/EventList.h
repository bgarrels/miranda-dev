#pragma once
class EventList
{
private:
	std::map<int, bool> filterMap;
	bool onlyInFilter;
	bool onlyOutFilter;
	int defFilter;
	std::wstring filterName;

	bool CanShowHistory(DBEVENTINFO* dbei);
	void InitFilters();
	void InitNames();
	void AddGroup(DBEVENTINFO *dbei);
protected:
	HANDLE hContact;
	TCHAR contactName[256];
	TCHAR myName[256];
	bool isWnd;
	
	virtual void AddGroup(bool isMe, const std::wstring &time, const std::wstring &user, const std::wstring &eventText, int ico) = 0;
	bool GetEventIcon(bool isMe, int eventType, int &id);
public:
	EventList();
	EventList(HANDLE _hContact, int filter);
	~EventList();
	
	HWND hWnd;
	std::vector<std::deque<HANDLE> > eventList;

	void SetDefFilter(int filter);
	int GetFilterNr();
	std::wstring GetFilterName();
	void RefreshEventList();
	std::wstring GetContactName();
	std::wstring GetMyName();
	std::wstring GetProtocolName();
	std::wstring GetMyId();
	std::wstring GetContactId();
	std::string GetBaseProtocol();
	void GetObjectDescription( DBEVENTINFO *dbei, TCHAR* str, int cbStr );
};

