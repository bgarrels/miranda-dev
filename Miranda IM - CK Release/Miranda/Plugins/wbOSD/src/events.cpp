/*
Wannabe OSD
This plugin tries to become miranda's standard OSD ;-)

(C) 2005 Andrej Krutak

Distributed under GNU's GPL 2 or later
*/

#include "wbOSD.h"
#include <m_message.h>

extern HWND hwnd;
extern HANDLE hHookContactStatusChanged;

void logmsg2(char *str)
{
	FILE *f=fopen("c:\\logm.txt", "a");
	fprintf(f, "%s\n", str);
	fclose(f);
}

void showmsgwnd(unsigned int param)
{
	logmsg("showmsgwnd");
	if (DBGetContactSettingByte(NULL,THIS_MODULE, "showMessageWindow", DEFAULT_SHOWMSGWIN))
		CallService(MS_MSG_SENDMESSAGE, (WPARAM)param, 0);
}

LRESULT ShowOSD(TCHAR *str, int timeout, COLORREF color, HANDLE user)
{
	osdmsg om;

	logmsg("ShowOSD");

	if (!hwnd)
		return 0;

	if (timeout==0)
		timeout=DBGetContactSettingDword(NULL,THIS_MODULE, "timeout", DEFAULT_TIMEOUT);
	om.text=str;
	om.timeout=timeout;
	om.color=color;
	om.param=(unsigned int)user;
	om.callback=showmsgwnd;
	
	return SendMessage(hwnd, WM_USER+4, (WPARAM)&om, 0);
}

int ProtoAck(WPARAM wparam,LPARAM lparam)
{
	ACKDATA *ack=(ACKDATA *)lparam;
	
	logmsg("ProtoAck");

	if (!DBGetContactSettingByte(NULL,THIS_MODULE, "a_user", DEFAULT_ANNOUNCESTATUS))
		return 0;

	if (!(DBGetContactSettingDword(NULL,THIS_MODULE,"showWhen", DEFAULT_SHOWWHEN)&(1<<(DBGetContactSettingWord(NULL, "CList", "Status", ID_STATUS_OFFLINE)-ID_STATUS_OFFLINE))))
		return 0;

	if ( ack->type == ACKTYPE_STATUS ) {
		if (!DBGetContactSettingByte(NULL,THIS_MODULE, "showMyStatus", DEFAULT_SHOWMYSTATUS))
			return 0;

		if ( ack->result == ACKRESULT_SUCCESS && (LPARAM)ack->hProcess != ack->lParam ) {
			DWORD ann = DBGetContactSettingDword( NULL, THIS_MODULE, "announce", DEFAULT_ANNOUNCE );
			if ( ann & ( 1 << ( ack->lParam - ID_STATUS_OFFLINE ))) {
				TCHAR* ptszName = (TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)ack->hContact, GCDNF_TCHAR);
				TCHAR buffer[512];
				mir_sntprintf(buffer, SIZEOF(buffer), TranslateT("%s is %s"), ptszName, TranslateTS(statusValues[ack->lParam-ID_STATUS_OFFLINE].szDescr));
				ShowOSD(buffer, 0, DBGetContactSettingDword(NULL,THIS_MODULE, "clr_status", DEFAULT_CLRSTATUS), ack->hContact);
	}	}	}

	return 0;
}

int ContactSettingChanged(WPARAM wParam,LPARAM lParam)
{
	DBCONTACTWRITESETTING *cws=(DBCONTACTWRITESETTING*)lParam;
	WORD newStatus = 0,oldStatus = 0;
	DWORD dwStatuses = 0;

	logmsg("ContactSettingChanged1");

	if((HANDLE)wParam==NULL || lstrcmpA(cws->szSetting,"Status")) return 0;

	newStatus = cws->value.wVal;
	oldStatus = DBGetContactSettingRangedWord((HANDLE)wParam,"UserOnline","OldStatus2",ID_STATUS_OFFLINE, ID_STATUS_MIN, ID_STATUS_MAX);
	
	if (oldStatus == newStatus) return 0;
	
	logmsg("ContactSettingChanged2");

	DBWriteContactSettingWord((HANDLE)wParam,"UserOnline","OldStatus2",(WORD)newStatus);

	if(CallService(MS_IGNORE_ISIGNORED,wParam,IGNOREEVENT_USERONLINE)) return 0;

	dwStatuses = MAKELPARAM(oldStatus, newStatus);
	NotifyEventHooks(hHookContactStatusChanged, (WPARAM)wParam, (LPARAM)dwStatuses);

	return 0;
}

int ContactStatusChanged(WPARAM wParam, LPARAM lParam)
{
	WORD oldStatus = LOWORD(lParam);
	WORD newStatus = HIWORD(lParam);
	DWORD ann=DBGetContactSettingDword(NULL,THIS_MODULE,"announce", DEFAULT_ANNOUNCE);
	char buffer[512];
#if defined(_UNICODE)
	wchar_t bufferW[512];
#endif

	logmsg("ContactStatusChanged1");

	if (!DBGetContactSettingByte(NULL,THIS_MODULE, "a_user", DEFAULT_ANNOUNCESTATUS))
		return 0;

	if (!(DBGetContactSettingDword(NULL,THIS_MODULE,"showWhen", DEFAULT_SHOWWHEN)&(1<<(DBGetContactSettingWord(NULL, "CList", "Status", ID_STATUS_OFFLINE)-ID_STATUS_OFFLINE))))
		return 0;

	if (!(ann&(1<<(newStatus-ID_STATUS_OFFLINE))) )
		return 0;
	
	logmsg("ContactStatusChanged2");

	if (DBGetContactSettingByte((HANDLE)wParam,"CList","NotOnList",0) || 
		DBGetContactSettingByte((HANDLE)wParam,"CList","Hidden",0) || 
		(CallService(MS_IGNORE_ISIGNORED,wParam,IGNOREEVENT_USERONLINE) && newStatus==ID_STATUS_ONLINE)
	)
		return 0;

	mir_snprintf(buffer, sizeof(buffer), Translate("%s is %s"), (char*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, wParam, 0), Translate(statusValues[newStatus-ID_STATUS_OFFLINE].szDescr));
#if defined(_UNICODE)
				MultiByteToWideChar(CP_ACP, 0, buffer, -1, bufferW, 511);
				bufferW[511] = 0;
				ShowOSD(bufferW, 0, DBGetContactSettingDword(NULL,THIS_MODULE, "clr_status", DEFAULT_CLRSTATUS), (HANDLE)wParam);
#else
				ShowOSD(buffer, 0, DBGetContactSettingDword(NULL,THIS_MODULE, "clr_status", DEFAULT_CLRSTATUS), (HANDLE)wParam);
#endif
	return 0;
}

int HookedNewEvent(WPARAM wParam, LPARAM lParam)
{
	logmsg("HookedNewEvent1");

	DBEVENTINFO dbe;
	dbe.cbSize = sizeof(dbe);
	dbe.cbBlob = CallService(MS_DB_EVENT_GETBLOBSIZE, (WPARAM) lParam, 0);
	if (dbe.cbBlob == -1)
		return 0;

	dbe.pBlob = (PBYTE) malloc(dbe.cbBlob);
	CallService(MS_DB_EVENT_GET, (WPARAM)lParam, (LPARAM)&dbe);

	if (dbe.flags & DBEF_SENT || dbe.eventType == 25368)
	    return 0;

	if (DBGetContactSettingByte(NULL,THIS_MODULE, "messages", DEFAULT_ANNOUNCEMESSAGES)==0)
		return 0;

	if (!(DBGetContactSettingDword(NULL,THIS_MODULE,"showWhen", DEFAULT_SHOWWHEN)&(1<<(DBGetContactSettingWord(NULL, "CList", "Status", ID_STATUS_OFFLINE)-ID_STATUS_OFFLINE))))
		return 0;
	
	logmsg("HookedNewEvent2");

	TCHAR buf[512];
	_tcscpy(buf, DEFAULT_MESSAGEFORMAT);

	DBVARIANT dbv;
	if(!DBGetContactSettingTString(NULL,THIS_MODULE,"message_format",&dbv)) {
		_tcscpy(buf, dbv.ptszVal);
		DBFreeVariant(&dbv);
	}

	int i1=-1, i2=-1;
	TCHAR* pbuf = buf;
	while (*pbuf) {
		if (*pbuf=='%') {
			if (*(pbuf+1)=='n') {
				if (i1==-1)
					i1=1;
				else i2=1;
				*(pbuf+1)='s';
			} else if (*(pbuf+1)=='m') {
				if (i1==-1)
					i1=2;
				else i2=2;
				*(pbuf+1)='s';
			} else if (*(pbuf+1)=='l') {
				*pbuf=0x0d;
				*(pbuf+1)=0x0a;
			}
		}
		pbuf++;
	}

	TCHAR *c1 = 0, *c2 = 0;
	if ( i1 == 1 )
		c1 = mir_tstrdup(( TCHAR* )CallService(MS_CLIST_GETCONTACTDISPLAYNAME, wParam, GCDNF_TCHAR));
	else if ( i1 == 2 )
		c1 = DbGetEventTextT( &dbe, 0 );

	if ( i2 == 1 )
		c2 = mir_tstrdup(( TCHAR* )CallService(MS_CLIST_GETCONTACTDISPLAYNAME, wParam, GCDNF_TCHAR));
	else if ( i2 == 2 )
		c2 = DbGetEventTextT( &dbe, 0 );

	TCHAR buffer[512];
	mir_sntprintf(buffer, SIZEOF(buffer), buf, c1, c2);
	ShowOSD(buffer, 0, DBGetContactSettingDword(NULL,THIS_MODULE, "clr_msg", DEFAULT_CLRMSG), (HANDLE)wParam);

	mir_free( c1 );
	mir_free( c2 );
	return 0;
}
