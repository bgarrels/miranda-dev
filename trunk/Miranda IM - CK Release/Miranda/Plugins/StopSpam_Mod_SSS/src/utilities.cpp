/*
Stopspam for 
Miranda IM: the free IM client for Microsoft* Windows*

Copyright 2000-2012 Miranda IM project,
all portions of this codebase are copyrighted to the people
listed in contributors.txt.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

===============================================================================

File name      : $HeadURL: 
Revision       : $Revision: 
Last change on : $Date: 
Last change by : $Author:
$Id$		   : $Id$:

===============================================================================
*/

#include "headers.h"

tstring DBGetContactSettingStringPAN(HANDLE hContact, char const * szModule, char const * szSetting, tstring errorValue)
{
	DBVARIANT dbv;
	//if(DBGetContactSetting(hContact, szModule, szSetting, &dbv))
	if(DBGetContactSettingTString(hContact, szModule, szSetting, &dbv))
		return errorValue;
//	if(DBVT_TCHAR == dbv.type )
		errorValue = dbv.ptszVal;
	DBFreeVariant(&dbv);
	return errorValue;
}

std::string DBGetContactSettingStringPAN_A(HANDLE hContact, char const * szModule, char const * szSetting, std::string errorValue)
{
	DBVARIANT dbv;
	//if(DBGetContactSetting(hContact, szModule, szSetting, &dbv))
	if(DBGetContactSettingString(hContact, szModule, szSetting, &dbv))
		return errorValue;
//	if(DBVT_ASCIIZ == dbv.type )
		errorValue = dbv.pszVal;
	DBFreeVariant(&dbv);
	return errorValue;
}

tstring &GetDlgItemString(HWND hwnd, int id)
{
	HWND h = GetDlgItem(hwnd, id);
	int len = GetWindowTextLength(h);
	TCHAR * buf = new TCHAR[len + 1];
	GetWindowText(h, buf, len + 1);
	static tstring s;
	s = buf;
	delete []buf;
	return s;
}					

std::string &GetProtoList()
{
	static std::string s;
	return s = DBGetContactSettingStringPAN_A(NULL, pluginName, "protoList", "ICQ\r\n");
}


bool ProtoInList(std::string proto)
{
	return std::string::npos != GetProtoList().find(proto + "\r\n");
}

int CreateCListGroup(TCHAR* szGroupName)
{
  int hGroup;
  CLIST_INTERFACE *clint = NULL;

  if (ServiceExists(MS_CLIST_RETRIEVE_INTERFACE))
    clint = (CLIST_INTERFACE*)CallService(MS_CLIST_RETRIEVE_INTERFACE, 0, 0);

  hGroup = CallService(MS_CLIST_GROUPCREATE, 0, 0);

    TCHAR* usTmp = szGroupName;

    clint->pfnRenameGroup(hGroup, usTmp);

  return hGroup;
}

void DeleteCListGroupsByName(TCHAR* szGroupName)
{
	int GroupNumber = 0;
	TCHAR szValue[96] = {0};
	char szNumber[32] = {0};
	strcpy(szNumber, "0");
	BYTE ConfirmDelete=DBGetContactSettingByte(NULL, "CList", "ConfirmDelete", SETTING_CONFIRMDELETE_DEFAULT);
	if(ConfirmDelete) 
		DBWriteContactSettingByte(NULL, "CList", "ConfirmDelete",0);
	while(strcmp(DBGetContactSettingStringPAN_A(NULL, "CListGroups", szNumber, "0").c_str(), "0") != 0)
	{
		wcscpy(szValue, DBGetContactSettingStringPAN(NULL, "CListGroups", szNumber, _T("0")).c_str());
		if(wcscmp(szGroupName, szValue + 1) == 0)
			CallService(MS_CLIST_GROUPDELETE,(WPARAM)(HANDLE)GroupNumber+1,0); // bug or ??? @_@
		GroupNumber++;
#if defined(_MSC_VER) && _MSC_VER >= 1300
		_itoa_s(GroupNumber, szNumber, sizeof(szNumber), 10);
#else
		_itoa(GroupNumber, szNumber, 10);
#endif		
	};
	if(ConfirmDelete) 
		DBWriteContactSettingByte(NULL, "CList", "ConfirmDelete",ConfirmDelete);
}

/*
void RemoveExcludedUsers()
{
	HANDLE hContact;
	hContact_entry *first, *plist, *tmp;
	hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
	first = new hContact_entry;
	plist = first;
	plist->hContact = INVALID_HANDLE_VALUE;
	if(hContact)
	{
		do{
			if(DBGetContactSettingByte(hContact, "CList", "NotOnList", 0) && DBGetContactSettingByte(hContact, pluginName, "Excluded", 0))
			{
				plist->hContact = hContact;
				plist->next = new hContact_entry;
				plist = plist->next;
				plist->hContact = INVALID_HANDLE_VALUE;
			}
		}while(hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT,(WPARAM)hContact, 0));
		
		plist = first;
		while(plist->hContact != INVALID_HANDLE_VALUE)
		{
			std::string proto=DBGetContactSettingStringPAN_A(plist->hContact,"Protocol","p","");
			UINT status = CallProtoService(proto.c_str(), PS_GETSTATUS, 0, 0);
			
			if(status>= ID_STATUS_CONNECTING && status <= ID_STATUS_OFFLINE){ 
				LogSpamToFile(plist->hContact, _T("Mark for delete"));
				DBWriteContactSettingByte(plist->hContact,"CList","Delete", 1);
			}else{
				LogSpamToFile(plist->hContact, _T("Deleted"));
				CallService(MS_DB_CONTACT_DELETE, (WPARAM)plist->hContact, 0);
			};
			tmp = plist;
			plist = plist->next;
			delete tmp;
		}
		delete plist;
	}
}

void RemoveTemporaryUsers()
{
	HANDLE hContact;
	hContact_entry *first, *plist, *tmp;
	hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
	first = new hContact_entry;
	plist = first;
	plist->hContact = INVALID_HANDLE_VALUE;
	if(hContact)
	{
		do{
			if(DBGetContactSettingByte(hContact, "CList", "NotOnList", 0)|| 
				(_T("Not In List")== DBGetContactSettingStringPAN(hContact,"CList","Group",_T("")))
			)
			{
				plist->hContact = hContact;
				plist->next = new hContact_entry;
				plist = plist->next;
				plist->hContact = INVALID_HANDLE_VALUE;
			}
		}while(hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT,(WPARAM)hContact, 0));

		plist = first;
		while(plist->hContact != INVALID_HANDLE_VALUE)
		{
			std::string proto=DBGetContactSettingStringPAN_A(plist->hContact,"Protocol","p","");
			UINT status = CallProtoService(proto.c_str(), PS_GETSTATUS, 0, 0);
			
			if(status>= ID_STATUS_CONNECTING && status <= ID_STATUS_OFFLINE){ 
				LogSpamToFile(plist->hContact, _T("Mark for delete"));
				DBWriteContactSettingByte(plist->hContact,"CList","Delete", 1);
			}else{
				LogSpamToFile(plist->hContact, _T("Deleted"));
				CallService(MS_DB_CONTACT_DELETE, (WPARAM)plist->hContact, 0);
			};

			tmp = plist; 
			plist = plist->next;
			delete tmp;
		}
		delete plist;
	};
	DeleteCListGroupsByName(_T("Not In List"));
}*/
int RemoveTmp(WPARAM,LPARAM)
{
	void CleanThread();
	CleanThread();
	return 0;
}
tstring variables_parse(tstring const &tstrFormat, HANDLE hContact){
	if (gbVarsServiceExist) {
		FORMATINFO fi;
		TCHAR *tszParsed;
		tstring tstrResult;

		ZeroMemory(&fi, sizeof(fi));
		fi.cbSize = sizeof(fi);
		fi.tszFormat = _tcsdup(tstrFormat.c_str());
		fi.hContact = hContact;
		fi.flags |= FIF_TCHAR;
		tszParsed = (TCHAR *)CallService(MS_VARS_FORMATSTRING, (WPARAM)&fi, 0);
		free(fi.tszFormat);
		if (tszParsed) {
			tstrResult = tszParsed;
			CallService(MS_VARS_FREEMEMORY, (WPARAM)tszParsed, 0);
			return tstrResult;
		}
	}
	return tstrFormat;
}

// case-insensitive _tcscmp
//by nullbie as i remember...
#define NEWTSTR_MALLOC(A) (A==NULL)?NULL:_tcscpy((TCHAR*)mir_alloc(sizeof(TCHAR)*(_tcslen(A)+1)),A)
const int Stricmp(const TCHAR *str, const TCHAR *substr)
{
	int i = 0;

	TCHAR *str_up = NEWTSTR_MALLOC(str);
	TCHAR *substr_up = NEWTSTR_MALLOC(substr);

	CharUpperBuff(str_up, lstrlen(str_up));
	CharUpperBuff(substr_up, lstrlen(substr_up));

	i = _tcscmp(str_up, substr_up);

	mir_free(str_up);
	mir_free(substr_up);
	
	return i;
}

TCHAR* ReqGetText(DBEVENTINFO* dbei)
{
	if ( !dbei->pBlob ) 
		return 0;

	char * ptr=(char *)&dbei->pBlob[sizeof(DWORD)*2];
	int len=dbei->cbBlob-sizeof(DWORD)*2; 
	int i=0;
		
	while(len&&(i<4))
	{
		if(!ptr[0]) i++;
		ptr++;
		len--;
	};

	if(len)
	{
		char * tstr=(char *)mir_alloc(len+1);
		memcpy(tstr, ptr, len);
		tstr[len]=0;
		WCHAR* msg = NULL;
		msg=(dbei->flags&DBEF_UTF)?mir_utf8decodeW(tstr):mir_a2u(tstr);
		mir_free(tstr);
		return (TCHAR *)msg;
	};
	return 0;
}


BOOL IsUrlContains(TCHAR * Str)
{
	const int CountUrl=11;
	const TCHAR  URL[CountUrl][5]=
	{
		_T("http"),
		_T("www"),
		_T(".ru"),
		_T(".com"),
		_T(".de"),
		_T(".cz"),
		_T(".org"),
		_T(".net"),
		_T(".su"),
		_T(".ua"),
		_T(".tv")
	}; 

	if(Str&&_tcslen(Str)>0) {
		TCHAR *StrLower = NEWTSTR_MALLOC(Str);
		CharLowerBuff(StrLower, lstrlen(StrLower)); 
		for (int i=0; i<CountUrl; i++)
			if(_tcsstr (StrLower, URL[i]))
			{
				mir_free(StrLower);
				return 1;
			}
		mir_free(StrLower);
	}
	return 0;
}

tstring GetContactUid(HANDLE hContact, tstring Protocol)
{
	tstring Uid;
	TCHAR dUid[32]={0};
	char aUid[32]={0};
	char *szProto = mir_utf8encodeW(Protocol.c_str());
	CONTACTINFO ci;
	ZeroMemory((void *)&ci, sizeof(ci));

	ci.hContact = hContact;
	ci.szProto = szProto;
	ci.cbSize = sizeof(ci);

	ci.dwFlag = CNF_DISPLAYUID | CNF_TCHAR;
	if (!CallService(MS_CONTACT_GETCONTACTINFO, 0, (LPARAM) & ci)) {
		switch (ci.type) {
			case CNFT_ASCIIZ:
				Uid=ci.pszVal;
				mir_free((void *)ci.pszVal);
			break;
			case CNFT_DWORD:
#if defined(_MSC_VER) && _MSC_VER >= 1300
		
		_itoa_s(ci.dVal,aUid,32,10);
#else
		_itoa(ci.dVal,aUid,10);
		
#endif	
				OemToChar(aUid, dUid);
				Uid=dUid;
			break;
			default:
				 Uid=_T("");
			break;
		};
	} 
	mir_free(szProto);
	return Uid;
}


void LogSpamToFile(HANDLE hContact, tstring message)
{
	
	if (!gbLogToFile) return;

	tstring LogStrW, LogTime, LogProtocol, LogContactId, LogContactName;
	std::string filename;
	std::fstream file;
	
	UINT cbName=255;
	char* pszName = (char *)mir_alloc(cbName);
	extern HANDLE hStopSpamLogDirH;

	if (FoldersGetCustomPath(hStopSpamLogDirH, pszName, cbName, ""))
		CallService(MS_DB_GETPROFILEPATH,(WPARAM) cbName, (LPARAM)pszName);
	filename=pszName;
	filename=filename+"\\stopspam_mod.log";
	mir_free(pszName);

	file.open(filename.c_str(),std::ios::out |std::ios::app);

	// Time Log line
	time_t time_now;
	tm   *TimeNow;
	time(&time_now);
	TimeNow = localtime(&time_now);
    LogTime=_wasctime( TimeNow ); 
	// Time Log line
	
	// Name, UID and Protocol Log line
	LogProtocol=DBGetContactSettingStringPAN(hContact,"Protocol","p",_T(""));
	LogContactName=(TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM) hContact, GCDNF_TCHAR);
	LogContactId=(LogProtocol==_T(""))?_T(""):GetContactUid(hContact,LogProtocol);
	// Name, UID  and Protocol Log line
	
	LogStrW=_T("[")+LogTime.substr(0,LogTime.length()-1)+_T("] ")+
		LogContactId+_T(" - ")+
		LogContactName+_T(" (")+
		LogProtocol+_T("): ")+
		message+_T("\n");

	char * buf=mir_u2a(LogStrW.c_str());
	file.write(buf,LogStrW.length());
	mir_free(buf);

	file.close();

}

boost::mutex clean_mutex;

void CleanProtocolTmpThread(std::string proto)
{
	while(true)
	{
		UINT status = CallProtoService(proto.c_str(), PS_GETSTATUS, 0, 0);
		if(status > ID_STATUS_OFFLINE)
			break;
		boost::this_thread::sleep(boost::posix_time::seconds(2));
	}
	std::list<HANDLE> contacts;
	for(HANDLE hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0); hContact; hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0))
	{
		char *proto_tmp = (char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
		if(proto_tmp)
			if(!strcmp(proto.c_str(), proto_tmp))
				if(DBGetContactSettingByte(hContact, "CList", "NotOnList", 0)|| (_T("Not In List")== DBGetContactSettingStringPAN(hContact,"CList","Group",_T(""))))
					contacts.push_back(hContact);
	}
	boost::this_thread::sleep(boost::posix_time::seconds(5));
	clean_mutex.lock();
	std::list<HANDLE>::iterator end = contacts.end();
	for(std::list<HANDLE>::iterator i = contacts.begin(); i != end; ++i)
	{		
		LogSpamToFile(*i, _T("Deleted"));
		HistoryLogFunc(*i, "Deleted");
		CallService(MS_DB_CONTACT_DELETE, (WPARAM)*i, 0);
	}
	clean_mutex.unlock();
}

void CleanProtocolExclThread(std::string proto)
{
	while(true)
	{
		UINT status = CallProtoService(proto.c_str(), PS_GETSTATUS, 0, 0);
		if(status > ID_STATUS_OFFLINE) 
			break;
		boost::this_thread::sleep(boost::posix_time::seconds(2));
	}
	std::list<HANDLE> contacts;
	for(HANDLE hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0); hContact; hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0))
	{
		char *proto_tmp = (char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
		if(proto_tmp)
			if(!strcmp(proto.c_str(), proto_tmp))
				if(DBGetContactSettingByte(hContact, "CList", "NotOnList", 0) && DBGetContactSettingByte(hContact, pluginName, "Excluded", 0))
					contacts.push_back(hContact);
	}
	boost::this_thread::sleep(boost::posix_time::seconds(5));
	clean_mutex.lock();
	std::list<HANDLE>::iterator end = contacts.end();
	for(std::list<HANDLE>::iterator i = contacts.begin(); i != end; ++i)
	{		
		LogSpamToFile(*i, _T("Deleted"));
		HistoryLogFunc(*i, "Deleted");
		CallService(MS_DB_CONTACT_DELETE, (WPARAM)*i, 0);
	}
	clean_mutex.unlock();
}


void CleanThread()
{
	std::list<std::string> protocols;
	int count = 0;
	PROTOACCOUNT **accounts;
	ProtoEnumAccounts(&count, &accounts);
	for(int i = 0; i < count; i++)
	{
		if(!strstr(accounts[i]->szModuleName, "MetaContacts") && !strstr(accounts[i]->szModuleName, "Weather")) //not real protocols
			protocols.push_back(accounts[i]->szModuleName);
	}
	std::list<std::string>::iterator end = protocols.end();
	for(std::list<std::string>::iterator i = protocols.begin(); i != end; ++i)
	{
		if(gbDelAllTempory)
			boost::thread *thr = new boost::thread(boost::bind(&CleanProtocolTmpThread, *i));
		if(gbDelExcluded)
			boost::thread *thr = new boost::thread(boost::bind(&CleanProtocolExclThread, *i));
	}
}
void HistoryLog(HANDLE hContact, char *data, int event_type, int flags)
{
	DBEVENTINFO Event = {0};
	Event.cbSize = sizeof(Event);
	Event.szModule = pluginName;
	Event.eventType = event_type;
	Event.flags = flags | DBEF_UTF;
	Event.timestamp = (DWORD)time(NULL);
	Event.cbBlob = strlen(data)+1;
	Event.pBlob = (PBYTE)_strdup(data);
	CallService(MS_DB_EVENT_ADD, (WPARAM)(HANDLE)hContact,(LPARAM)&Event);
}
void HistoryLogFunc(HANDLE hContact, std::string message)
{
	if(gbHistoryLog)
	{
		if(hContact == INVALID_HANDLE_VALUE)
			return;
		std::string msg = message;
		msg.append("\n");
		msg.append("Protocol: ").append((char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0)).append(" Contact: ");
		msg.append(toUTF8((TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM) hContact, GCDNF_TCHAR))).append(" ID: ");
		msg.append(toUTF8(GetContactUid(hContact,toUTF16((char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0)))));
		HistoryLog(NULL, (char*)msg.c_str(), EVENTTYPE_MESSAGE, DBEF_READ);
	}
}

std::string toUTF8(std::wstring str)
{
	std::string ustr;
	try{
	utf8::utf16to8(str.begin(), str.end(), back_inserter(ustr));
	}
	catch(const std::exception &e)
	{
		//TODO: handle utf8cpp exceptions
	}
	return ustr;
}

std::string toUTF8(std::string str)
{
	return toUTF8(toUTF16(str));
}


std::wstring toUTF16(std::string str) //convert as much as possible
{
	std::wstring ustr;
	std::string tmpstr;
	try{
	utf8::replace_invalid(str.begin(), str.end(), back_inserter(tmpstr));
	utf8::utf8to16(tmpstr.begin(), tmpstr.end(), back_inserter(ustr));
	}
	catch(const std::exception &e)
	{
		//TODO: handle utf8cpp exceptions
	}
	return ustr;
}

std::string get_random_num(int length)
{
	std::string chars("123456789");
	std::string data;
	boost::random_device rng;
	boost::variate_generator<boost::random_device&, boost::uniform_int<>> gen(rng, boost::uniform_int<>(0, chars.length()-1));
	for(int i = 0; i < length; ++i) 
		data += chars[gen()];
	return data;
}
