#include "StdAfx.h"
#include "EventList.h"
#include "Options.h"

extern int iconsNum;

EventList::EventList()
	:hWnd(NULL),
	isWnd(false),
	hContact(NULL)
{
}

EventList::EventList(HANDLE _hContact, int filter)
	:hWnd(NULL),
	isWnd(false),
	hContact(_hContact)
{
	SetDefFilter(filter);
}


EventList::~EventList()
{
	eventList.clear();
}

bool EventList::CanShowHistory(DBEVENTINFO* dbei)
{
	if(defFilter == 1)
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

void EventList::RefreshEventList()
{
	HANDLE hDbEvent;
	DBEVENTINFO dbei = {0};
	DWORD newBlobSize,oldBlobSize;
	
	eventList.clear();

	dbei.cbSize=sizeof(dbei);
	oldBlobSize=0;
	bool isNewOnTop = Options::instance->groupNewOnTop;
	hDbEvent=(HANDLE)CallService(isNewOnTop ? MS_DB_EVENT_FINDLAST : MS_DB_EVENT_FINDFIRST,(WPARAM)hContact,0);
	char* findNext = isNewOnTop ? MS_DB_EVENT_FINDPREV : MS_DB_EVENT_FINDNEXT;
	
	InitNames();
	InitFilters();

	DWORD lastTime = MAXDWORD;
	eventList.push_back(std::deque<HANDLE>());
	int groupTime = Options::instance->groupTime * 60 * 60;
	int maxMess = Options::instance->groupMessagesNumber;
	int limitator = 0;
	bool isWndLocal = isWnd;
	while ( hDbEvent != NULL ) 
	{
		if (isWndLocal && !IsWindow( hWnd ))
			break;
		newBlobSize=CallService(MS_DB_EVENT_GETBLOBSIZE,(WPARAM)hDbEvent,0);
		if((int)newBlobSize != -1)
		{
			if(newBlobSize>oldBlobSize) 
			{
				dbei.pBlob=(PBYTE)mir_realloc(dbei.pBlob,newBlobSize);
				oldBlobSize=newBlobSize;
			}
			dbei.cbBlob = oldBlobSize;
			CallService( MS_DB_EVENT_GET, (WPARAM)hDbEvent, (LPARAM)&dbei );
			if(hContact == NULL || CanShowHistory(&dbei)) 
			{
				DWORD tm = isNewOnTop ? lastTime - dbei.timestamp : dbei.timestamp - lastTime;
				if(tm < groupTime && limitator < maxMess)
				{
					lastTime = dbei.timestamp;
					if(isNewOnTop)
						eventList.back().push_front(hDbEvent);
					else
						eventList.back().push_back(hDbEvent);
					++limitator;
				}
				else
				{
					limitator = 0;
					lastTime = dbei.timestamp;
					if(!eventList.back().empty())
					{
						HANDLE firstEvent = eventList.back().front();
						dbei.cbBlob = oldBlobSize;
						CallService( MS_DB_EVENT_GET, (WPARAM)firstEvent, (LPARAM)&dbei );
						AddGroup(&dbei);
						eventList.push_back(std::deque<HANDLE>());
					}
					eventList.back().push_front(hDbEvent);
				}
			}
		}
		hDbEvent=(HANDLE)CallService(findNext,(WPARAM)hDbEvent,0);
	}

	if(!eventList.back().empty()) 
	{
		HANDLE firstEvent = eventList.back().front();
		CallService( MS_DB_EVENT_GET, (WPARAM)firstEvent, (LPARAM)&dbei );
		AddGroup(&dbei);
	}

	mir_free(dbei.pBlob);
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

void EventList::AddGroup(DBEVENTINFO *dbei)
{
	TCHAR eventText[256];
	int i;
	eventText[0] = 0;
	tmi.printTimeStamp(NULL, dbei->timestamp, _T("d t"), eventText, 64, 0);
	std::wstring time = eventText;
	std::wstring user;
	if(dbei->flags & DBEF_SENT)
		user = myName;
	else
		user = contactName;
	GetObjectDescription( dbei, eventText, 256); 
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
	GetEventIcon(dbei->flags & DBEF_SENT, dbei->eventType, ico);
	AddGroup(dbei->flags & DBEF_SENT, time, user, eventText, ico);
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

std::wstring EventList::GetProtocolName()
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

std::string EventList::GetBaseProtocol()
{
	return (char *)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
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

std::wstring EventList::GetContactId()
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
