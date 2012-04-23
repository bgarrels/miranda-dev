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
#include "EventList.h"
#include "Options.h"
#include "ExportManager.h"
#include <assert.h>

extern int iconsNum;

bool DeleteDirectory(LPCTSTR lpszDir, bool noRecycleBin = true);
std::wstring GetName(const std::wstring &path);

EventList::EventList()
	:hWnd(NULL),
	isWnd(false),
	hContact(NULL),
	deltaTime(0),
	isFlat(false),
	useImportedMessages(true)
{
	memset(&gdbei, 0, sizeof(DBEVENTINFO));
	gdbei.cbSize = sizeof(DBEVENTINFO);
	goldBlobSize = 0;
}

EventList::EventList(HANDLE _hContact, int filter)
	:hWnd(NULL),
	isWnd(false),
	hContact(_hContact),
	deltaTime(0),
	isFlat(false),
	useImportedMessages(true)
{
	memset(&gdbei, 0, sizeof(DBEVENTINFO));
	gdbei.cbSize = sizeof(DBEVENTINFO);
	goldBlobSize = 0;
	SetDefFilter(filter);
}


EventList::~EventList()
{
	mir_free(gdbei.pBlob);
	eventList.clear();
}

bool EventList::CanShowHistory(DBEVENTINFO* dbei)
{
	if(deltaTime != 0)
	{
		if(deltaTime > 0)
		{
			if(now - deltaTime < dbei->timestamp)
				return false;
		}
		else
		{
			if(now + deltaTime > dbei->timestamp)
				return false;
		}
	}

	if(hContact == NULL || defFilter == 1)
		return true;

	else if(defFilter < 1)
	{
		switch( dbei->eventType ) 
		{
		case EVENTTYPE_MESSAGE:
		case EVENTTYPE_URL:
		case EVENTTYPE_FILE:
			return true;

		default:
			{
				DBEVENTTYPEDESCR* et = ( DBEVENTTYPEDESCR* )CallService( MS_DB_EVENT_GETTYPE, ( WPARAM )dbei->szModule, ( LPARAM )dbei->eventType );
				if ( et && ( et->flags & DETF_HISTORY )) 
				{
					return true;
				}
			}
		}

		return false;
	}
	else
	{
		if(filterMap.find(dbei->eventType) != filterMap.end())
		{
			if(onlyInFilter)
			{
				return !(dbei->flags & DBEF_SENT);
			}
			else if(onlyOutFilter)
			{
				return dbei->flags & DBEF_SENT;
			}
			return true;
		}
		return false;
	}
}

bool EventList::CanShowHistory(const IImport::ExternalMessage& message)
{
	if(deltaTime != 0)
	{
		if(deltaTime > 0)
		{
			if(now - deltaTime < message.timestamp)
				return false;
		}
		else
		{
			if(now + deltaTime > message.timestamp)
				return false;
		}
	}

	if(hContact == NULL || defFilter == 1)
		return true;

	else if(defFilter < 1)
	{
		switch(message.eventType ) 
		{
		case EVENTTYPE_MESSAGE:
		case EVENTTYPE_URL:
		case EVENTTYPE_FILE:
			return true;
		}

		return false;
	}
	else
	{
		if(filterMap.find(message.eventType) != filterMap.end())
		{
			if(onlyInFilter)
			{
				return !(message.flags & DBEF_SENT);
			}
			else if(onlyOutFilter)
			{
				return message.flags & DBEF_SENT;
			}
			return true;
		}
		return false;
	}
}

void EventList::InitFilters()
{
	filterMap.clear();
	onlyInFilter = false;
	onlyOutFilter = false;
	if(defFilter >= 2)
	{
		defFilter = 0;
		for(int i = 0; i < Options::instance->customFilters.size(); ++i)
		{
			if(filterName == Options::instance->customFilters[i].name)
			{
				defFilter = i + 2;
				if(Options::instance->customFilters[i].onlyIncomming && !Options::instance->customFilters[i].onlyOutgoing)
				{
					onlyInFilter = true;
				}
				else if(Options::instance->customFilters[i].onlyOutgoing && !Options::instance->customFilters[i].onlyIncomming)
				{
					onlyOutFilter = true;
				}

				for(std::vector<int>::iterator it = Options::instance->customFilters[i].events.begin(); it != Options::instance->customFilters[i].events.end(); ++it)
				{
					filterMap[*it] = true;
				}

				break;
			}
		}
	}
	else
		filterName = L"";
}

void EventList::SetDefFilter(int filter)
{
	defFilter = filter;
	if(filter >= 2 && filter - 2 < Options::instance->customFilters.size())
	{
		filterName = Options::instance->customFilters[filter - 2].name;
	}
	else if(filter == 1)
	{
		filterName = TranslateT("All events");
	}
	else
	{
		filterName = TranslateT("Default history events");
	}
}

int EventList::GetFilterNr()
{
	return defFilter;
}

std::wstring EventList::GetFilterName()
{
	return filterName;
}

void EventList::GetTempList(std::list<EventTempIndex>& tempList, bool noFilter)
{
	HANDLE hDbEvent;
	bool isWndLocal = isWnd;
	EventTempIndex ti;
	EventIndex ei;
	EventData data;
	ti.isExternal = false;
	ei.isExternal = false;
	hDbEvent=(HANDLE)CallService(MS_DB_EVENT_FINDFIRST,(WPARAM)hContact,0);
	while ( hDbEvent != NULL ) 
	{
		if (isWndLocal && !IsWindow( hWnd ))
			break;
		ei.hEvent = hDbEvent;
		if(GetEventData(ei, data))
		{
			if(noFilter || CanShowHistory(&gdbei)) 
			{
				ti.hEvent = hDbEvent;
				ti.timestamp = data.timestamp;
				tempList.push_back(ti);
			}
		}
		hDbEvent=(HANDLE)CallService(MS_DB_EVENT_FINDNEXT,(WPARAM)hDbEvent,0);
	}

	std::list<EventTempIndex>::iterator itL = tempList.begin();
	ti.isExternal = true;
	for(int i = 0; i < importedMessages.size(); ++i)
	{
		if(noFilter || CanShowHistory(importedMessages[i]))
		{
			DWORD ts = importedMessages[i].timestamp;
			while(itL != tempList.end() && itL->timestamp < ts)++itL;
			if(itL == tempList.end() || itL->timestamp > ts)
			{
				ti.exIdx = i;
				ti.timestamp = ts;
				tempList.insert(itL, ti);
			}
		}
	}
}

void EventList::RefreshEventList()
{
	InitNames();
	InitFilters();

	if(useImportedMessages)
	{
		std::vector<IImport::ExternalMessage> messages;
	
		EnterCriticalSection(&criticalSection);
		std::map<HANDLE, EventList::ImportDiscData>::iterator it = contactFileMap.find(hContact);
		if(it != contactFileMap.end())
		{
			ExportManager imp(hWnd, hContact, 1);
			imp.SetAutoImport(it->second.file);
			if(!imp.Import(it->second.type, messages, NULL))
			{
				messages.clear();
			}
		}

		LeaveCriticalSection(&criticalSection);
	
		ImportMessages(messages);
	}

	std::list<EventTempIndex> tempList;
	GetTempList(tempList, false);
	std::list<EventTempIndex> revTempList;
	std::list<EventTempIndex>& nrTempList = tempList;
	bool isNewOnTop = Options::instance->groupNewOnTop;
	if(isNewOnTop)
	{
		revTempList.insert(revTempList.begin(), tempList.rbegin(), tempList.rend());
		nrTempList = revTempList;
	}
	
	eventList.clear();
	eventList.push_back(std::deque<EventIndex>());
	DWORD lastTime = MAXDWORD;
	int groupTime = Options::instance->groupTime * 60 * 60;
	int maxMess = Options::instance->groupMessagesNumber;
	int limitator = 0;
	EventIndex ei;
	for(std::list<EventTempIndex>::iterator itL = nrTempList.begin(); itL != nrTempList.end(); ++itL)
	{
		DWORD tm = isNewOnTop ? lastTime - itL->timestamp : itL->timestamp - lastTime;
		if(isFlat || tm < groupTime && limitator < maxMess)
		{
			lastTime = itL->timestamp;
			ei.isExternal = itL->isExternal;
			ei.hEvent = itL->hEvent;
			if(isNewOnTop)
				eventList.back().push_front(ei);
			else
				eventList.back().push_back(ei);
			++limitator;
		}
		else
		{
			limitator = 0;
			lastTime = itL->timestamp;
			if(!eventList.back().empty())
			{
				ei = eventList.back().front();
				AddGroup(ei);
				eventList.push_back(std::deque<EventIndex>());
			}
			ei.isExternal = itL->isExternal;
			ei.hEvent = itL->hEvent;
			eventList.back().push_front(ei);
		}
	}

	if(!eventList.back().empty()) 
	{
		ei = eventList.back().front();
		AddGroup(ei);
	}
}

void EventList::InitNames()
{
	TCHAR str[200];
	if(hContact)
	{
		_tcscpy_s(contactName, 256, (TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM) hContact, GCDNF_TCHAR ));
		mir_sntprintf(str,200,TranslateT("History for %s"),contactName);
	}
	else
	{
		_tcscpy_s(contactName, 256, TranslateT("Server"));
		mir_sntprintf(str,200,TranslateT("History"));
	}

	if(isWnd)
	{
		SetWindowText(hWnd,str);
	}

	_tcscpy_s(myName, GetMyName().c_str());
}

void EventList::AddGroup(const EventIndex& ev)
{
	EventData data;
	GetEventData(ev, data);
	TCHAR eventText[256];
	int i;
	eventText[0] = 0;
	tmi.printTimeStamp(NULL, data.timestamp, _T("d t"), eventText, 64, 0);
	std::wstring time = eventText;
	std::wstring user;
	if(data.isMe)
		user = myName;
	else
		user = contactName;
	GetEventMessage(ev, eventText, 256);
	for(i = 0; eventText[i] != 0 && eventText[i] != _T('\r') && eventText[i] != _T('\n'); ++i);
	eventText[i] = 0;
	if(i > Options::instance->groupMessageLen)
	{
		eventText[Options::instance->groupMessageLen - 3] = '.';
		eventText[Options::instance->groupMessageLen - 2] = '.';
		eventText[Options::instance->groupMessageLen - 1] = '.';
		eventText[Options::instance->groupMessageLen] = 0;
	}
	
	int ico = 0;
	GetEventIcon(data.isMe, data.eventType, ico);
	AddGroup(data.isMe, time, user, eventText, ico);
}

std::wstring EventList::GetContactName()
{
	if(hContact)
	{
		return (TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM) hContact, GCDNF_TCHAR );
	}
	else
	{
		return TranslateT("Server");
	}
}

void GetInfo(CONTACTINFO& ci, std::wstring& str)
{
	if (!CallService(MS_CONTACT_GETCONTACTINFO, 0, (LPARAM) & ci)) 
	{
		if (ci.type == CNFT_ASCIIZ) 
		{
			str =  ci.pszVal;
			mir_free(ci.pszVal);

		}
		else if (ci.type == CNFT_DWORD) 
		{
			TCHAR buf[20];
			_ltot_s(ci.dVal, buf, 10 );
			str = buf;
		}	
		else if (ci.type == CNFT_WORD) 
		{
			TCHAR buf[20];
			_ltot_s(ci.wVal, buf, 10 );
			str = buf;
		}	
	}
}

std::wstring EventList::GetMyName()
{
	std::wstring myName;
	CONTACTINFO ci;
	ZeroMemory(&ci, sizeof(ci));
	ci.cbSize = sizeof(ci);
	ci.szProto = (char *)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
	ci.hContact = 0;
	ci.dwFlag = CNF_DISPLAY | CNF_TCHAR;
	GetInfo(ci, myName);
	if(myName.empty())
	{
		return TranslateT("Me");
	}

	return myName;
}

inline std::wstring GetProtocolName(HANDLE hContact)
{
	char* ac = (char *)CallService(MS_PROTO_GETCONTACTBASEACCOUNT, (WPARAM)hContact, 0);
	std::wstring proto1;
	if(ac != NULL)
	{
		PROTOACCOUNT* acnt = ProtoGetAccount(ac);
		if(acnt != NULL && acnt->szModuleName != NULL)
		{
			wchar_t* proto = mir_a2u(acnt->szProtoName);
			proto1 = proto;
			mir_free(proto);
		}
	}

	return proto1;
}

std::wstring EventList::GetProtocolName()
{
	return ::GetProtocolName(hContact);
}

std::string EventList::GetBaseProtocol()
{
	char* proto = (char *)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
	return proto == NULL ? "" : proto;
}

std::wstring EventList::GetMyId()
{
	std::wstring myId;
	CONTACTINFO ci;
	ZeroMemory(&ci, sizeof(ci));
	ci.cbSize = sizeof(ci);
	ci.szProto = (char *)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
	ci.hContact = 0;
	ci.dwFlag = CNF_DISPLAYUID | CNF_TCHAR;
	GetInfo(ci, myId);
	return myId;
}

inline std::wstring GetContactId(HANDLE hContact)
{
	std::wstring id;
	CONTACTINFO ci;
	ZeroMemory(&ci, sizeof(ci));
	ci.cbSize = sizeof(ci);
	ci.szProto = (char *)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
	ci.hContact = hContact;
	ci.dwFlag = CNF_DISPLAYUID | CNF_TCHAR;
	GetInfo(ci, id);
	return id;
}

std::wstring EventList::GetContactId()
{
	return ::GetContactId(hContact);
}

static void GetMessageDescription( DBEVENTINFO *dbei, TCHAR* buf, int cbBuf )
{
	TCHAR* msg = DbGetEventTextT( dbei, CP_ACP );
    _tcsncpy( buf, msg ? msg : TranslateT("Invalid Message"), cbBuf );
    buf[ cbBuf-1 ] = 0;
	mir_free( msg );
}

static void GetAuthRequestDescription( DBEVENTINFO *dbei, TCHAR* buf, int cbBuf )
{
	std::wstring allName;
	buf[0] = 0;
	int pos = sizeof( DWORD ) + sizeof( HANDLE );
	if(pos >= dbei->cbBlob)
		return;
	DWORD uin = *((DWORD*)dbei->pBlob);
	HANDLE hContact = *((HANDLE*)(dbei->pBlob + sizeof( DWORD )));
	char* nick, *firstName, *lastName, *jid, *reason;
	nick = ( char* )( dbei->pBlob + sizeof( DWORD )+ sizeof( HANDLE ));
	pos += strnlen_s(nick, dbei->cbBlob - pos) + 1;
	if(pos >= dbei->cbBlob)
		return;
	firstName = ( char* )dbei->pBlob + pos;
	pos += strnlen_s(firstName, dbei->cbBlob - pos) + 1;
	if(pos >= dbei->cbBlob)
		return;
	lastName = ( char* )dbei->pBlob + pos;
	pos += strnlen_s(lastName, dbei->cbBlob - pos) + 1;
	if(pos >= dbei->cbBlob)
		return;
	jid = (char*)dbei->pBlob + pos;
	pos += strnlen_s(jid, dbei->cbBlob - pos) + 1;
	if(pos >= dbei->cbBlob)
		return;
	reason = (char*)dbei->pBlob + pos;
	TCHAR *newNick, *newFirstName, *newLastName, *newJid, *newReason;
	if(dbei->flags & DBEF_UTF)
	{
		newNick = mir_utf8decodeT( nick );
		newFirstName = mir_utf8decodeT( firstName );
		newLastName = mir_utf8decodeT( lastName );
		newJid = mir_utf8decodeT( jid );
		newReason = mir_utf8decodeT( reason );
	}
	else
	{
		newNick = mir_a2t( nick );
		newFirstName = mir_a2t( firstName );
		newLastName = mir_a2t( lastName );
		newJid = mir_a2t( jid );
		newReason = mir_a2t( reason );
	}

	if(newFirstName[0] != 0)
	{
		allName += newFirstName;
		if(newLastName[0] != 0)
			allName += _T(" ");
	}

	if(newLastName[0] != 0)
		allName += newLastName;
	if(!allName.empty())
		allName += _T(", ");
	if(newJid[0] != 0)
	{
		allName += newJid;
		allName += _T(", ");
	}

	_sntprintf_s(buf, cbBuf, _TRUNCATE, TranslateT("Authorisation request by %s (%s%d): %s"), 
		(newNick[0] == 0 ? (TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM) hContact, GCDNF_TCHAR) : newNick),
		allName.c_str(), uin, newReason);
	mir_free( newNick );
	mir_free( newFirstName );
	mir_free( newLastName );
	mir_free( newJid );
	mir_free( newReason );
}

void EventList::GetObjectDescription( DBEVENTINFO *dbei, TCHAR* str, int cbStr )
{
	switch( dbei->eventType ) 
	{
	case EVENTTYPE_AUTHREQUEST:
		GetAuthRequestDescription( dbei, str, cbStr );
		break;

	default:
		GetMessageDescription( dbei, str, cbStr );
	}
}

bool EventList::GetEventIcon(bool isMe, int eventType, int &id)
{
	switch(eventType)
	{
	case EVENTTYPE_MESSAGE:
		id = isMe ? 1 : 0;
		return true;
	case EVENTTYPE_STATUSCHANGE:
		id = 2;
		return true;
	case EVENTTYPE_FILE:
		id = iconsNum;
		return true;
	case EVENTTYPE_URL:
		id = iconsNum + 1;
		return true;
	case EVENTTYPE_AUTHREQUEST:
		id = iconsNum + 2;
		return true;
	default:
		id = isMe ? 1 : 0;
		return false;
	}
}

void EventList::ImportMessages(const std::vector<IImport::ExternalMessage>& messages)
{
	DWORD lastTime = 0;
	importedMessages.clear();
	for(int i = 0; i < messages.size(); ++i)
	{
		if(messages[i].timestamp >= lastTime)
		{
			importedMessages.push_back(messages[i]);
			lastTime = messages[i].timestamp;
		}
		else
		{
			assert(FALSE);
		}
	}
}

void EventList::MargeMessages(const std::vector<IImport::ExternalMessage>& messages)
{
	ImportMessages(messages);
	std::list<EventTempIndex> tempList;
	GetTempList(tempList, true);
	DBEVENTINFO dbei = {0};
	dbei.cbSize = sizeof(DBEVENTINFO);
	dbei.szModule = (char *)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
	CallService(MS_DB_SETSAFETYMODE, (WPARAM)FALSE, 0);
	for(std::list<EventTempIndex>::iterator it = tempList.begin(); it != tempList.end(); ++it)
	{
		if(it->isExternal)
		{
			IImport::ExternalMessage& msg = importedMessages[it->exIdx];
			dbei.flags = msg.flags & (~(DBEF_FIRST));
			dbei.flags |= DBEF_READ;
			dbei.timestamp = msg.timestamp;
			// For now I do not convert event data from string to blob, and event type must be message to handle it properly
			dbei.eventType = EVENTTYPE_MESSAGE;
			UINT cp = dbei.flags & DBEF_UTF ? CP_UTF8 : CP_ACP;
			dbei.cbBlob = WideCharToMultiByte(cp, 0, msg.message.c_str(), msg.message.length() + 1, NULL, 0, NULL, NULL);
			char* buf = new char[dbei.cbBlob];
			dbei.cbBlob = WideCharToMultiByte(cp, 0, msg.message.c_str(), msg.message.length() + 1, buf, dbei.cbBlob, NULL, NULL);
			dbei.pBlob = (PBYTE)buf;
			CallService(MS_DB_EVENT_ADD, (WPARAM) hContact, (LPARAM) & dbei);
			delete buf;
		}
	}

	CallService(MS_DB_SETSAFETYMODE, (WPARAM)TRUE, 0);
	std::vector<IImport::ExternalMessage> emessages;
	ImportMessages(emessages);
}

bool EventList::GetEventData(const EventIndex& ev, EventData& data)
{
	if(!ev.isExternal)
	{
		DWORD newBlobSize=CallService(MS_DB_EVENT_GETBLOBSIZE,(WPARAM)ev.hEvent,0);
		if(newBlobSize>goldBlobSize) 
		{
			gdbei.pBlob=(PBYTE)mir_realloc(gdbei.pBlob,newBlobSize);
			goldBlobSize=newBlobSize;
		}

		gdbei.cbBlob = goldBlobSize;
		if (CallService(MS_DB_EVENT_GET,(WPARAM)ev.hEvent,(LPARAM)&gdbei) == 0)
		{
			data.isMe = gdbei.flags & DBEF_SENT;
			data.eventType = gdbei.eventType;
			data.timestamp = gdbei.timestamp;
			return true;
		}
	}
	else
	{
		if(ev.exIdx >= 0 && ev.exIdx < importedMessages.size())
		{
			IImport::ExternalMessage& em = importedMessages[ev.exIdx];
			data.isMe = em.flags & DBEF_SENT;
			data.eventType = em.eventType;
			data.timestamp = em.timestamp;
			return true;
		}
	}

	return false;
}

void EventList::GetExtEventDBei(const EventIndex& ev)
{
	IImport::ExternalMessage& em = importedMessages[ev.exIdx];
	gdbei.flags = (em.flags & (~(DBEF_FIRST)) ) | 0x800;
	gdbei.eventType = em.eventType;
	gdbei.timestamp = em.timestamp;
}

HICON EventList::GetEventCoreIcon(const EventIndex& ev)
{
	if(ev.isExternal)
		return NULL;
	HICON ico;
	ico = (HICON)CallService(MS_DB_EVENT_GETICON, LR_SHARED, (LPARAM)&gdbei);
	HICON icoMsg = LoadSkinnedIcon(SKINICON_EVENT_MESSAGE);
	if(icoMsg == ico)
	{
		return NULL;
	}

	return ico;
}

void EventList::RebuildGroup(int selected)
{
	std::deque<EventIndex> newGroup;
	for(int i = 0; i < eventList[selected].size(); ++i)
	{
		EventIndex& ev = eventList[selected][i];
		if(!ev.isExternal)
		{
			if(CallService(MS_DB_EVENT_GETBLOBSIZE,(WPARAM)(HANDLE)ev.hEvent,0) >= 0)
			{
				// If event exist, we add it to new group
				newGroup.push_back(eventList[selected][i]);
			}
		}
		else
		{
			newGroup.push_back(eventList[selected][i]);
		}
	}
	eventList[selected].clear();
	eventList[selected].insert(eventList[selected].begin(), newGroup.begin(), newGroup.end());
}

CRITICAL_SECTION EventList::criticalSection;
std::map<HANDLE, EventList::ImportDiscData> EventList::contactFileMap;
std::wstring EventList::contactFileDir;

void EventList::AddImporter(HANDLE hContact, IImport::ImportType type, const std::wstring& file)
{
	EnterCriticalSection(&criticalSection);
	TCHAR buf[32];
	_stprintf_s(buf, _T("%016llx"), (unsigned long long int)hContact);
	std::wstring internalFile = contactFileDir;
	ImportDiscData data;
	data.file = contactFileDir + buf;
	data.type = type;
	CopyFile(file.c_str(), data.file.c_str(), FALSE);
	contactFileMap[hContact] = data;
	LeaveCriticalSection(&criticalSection);
}

void EventList::Init()
{
	InitializeCriticalSection(&EventList::criticalSection);
	TCHAR temp[MAX_PATH];
	temp[0] = 0;
	GetTempPath(MAX_PATH, temp);
	contactFileDir = temp;
	contactFileDir += L"BasicHistoryImportDir\\";
	DeleteDirectory(contactFileDir.c_str());
	CreateDirectory(contactFileDir.c_str(), NULL);
}

void EventList::Deinit()
{
	DeleteCriticalSection(&EventList::criticalSection);
}

int EventList::GetContactMessageNumber(HANDLE hContact)
{
	int count = CallService(MS_DB_EVENT_GETCOUNT,(WPARAM)hContact,0);
	EnterCriticalSection(&criticalSection);
	std::map<HANDLE, EventList::ImportDiscData>::iterator it = contactFileMap.find(hContact);
	if(it != contactFileMap.end())
	{
		++count;
	}

	LeaveCriticalSection(&criticalSection);
	return count;
}

bool EventList::IsImportedHistory(HANDLE hContact)
{
	bool count = false;
	EnterCriticalSection(&criticalSection);
	std::map<HANDLE, EventList::ImportDiscData>::iterator it = contactFileMap.find(hContact);
	if(it != contactFileMap.end())
	{
		count = true;
	}

	LeaveCriticalSection(&criticalSection);
	return count;
}

void EventList::DeleteImporter(HANDLE hContact)
{
	EnterCriticalSection(&criticalSection);
	std::map<HANDLE, EventList::ImportDiscData>::iterator it = contactFileMap.find(hContact);
	if(it != contactFileMap.end())
	{
		DeleteFile(it->second.file.c_str());
		contactFileMap.erase(it);
	}

	LeaveCriticalSection(&criticalSection);
}
