/* 
Copyright (C) 2012 Mataes

This is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this file; see the file license.txt.  If
not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  
*/

#include "common.h"

HANDLE hNetlibUser = NULL, hNetlibHttp;

BOOL IsMyContact(HANDLE hContact)
{
	const char* szProto = (char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
	return szProto != NULL && strcmp(MODULE, szProto) == 0;
}

VOID NetlibInit()
{
	NETLIBUSER nlu = {0};
	nlu.cbSize = sizeof(nlu);
	nlu.flags = NUF_OUTGOING | NUF_INCOMING | NUF_HTTPCONNS | NUF_TCHAR;	// | NUF_HTTPGATEWAY;
	nlu.ptszDescriptiveName = TranslateT("NewsAggr HTTP connection");
	nlu.szSettingsModule = MODULE;
	hNetlibUser = (HANDLE)CallService(MS_NETLIB_REGISTERUSER, 0, (LPARAM)&nlu);
}

VOID NetlibUnInit()
{
	Netlib_CloseHandle(hNetlibUser);
	hNetlibUser = NULL;
}

VOID GetNewsData(TCHAR *tszUrl, char** szData)
{
	NETLIBHTTPREQUEST nlhr = {0}, *nlhrReply;
	NETLIBHTTPHEADER headers[4];

	// initialize the netlib request
	nlhr.cbSize = sizeof(nlhr);
	nlhr.requestType = REQUEST_GET;
	nlhr.flags = NLHRF_DUMPASTEXT | NLHRF_HTTP11;
	char *szUrl = mir_t2a(tszUrl);
	nlhr.szUrl = szUrl;
	nlhr.nlc = hNetlibHttp;

	// change the header so the plugin is pretended to be IE 6 + WinXP
	nlhr.headersCount = 4;
	nlhr.headers = headers;
	nlhr.headers[0].szName  = "User-Agent";
	nlhr.headers[0].szValue = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)";
	nlhr.headers[1].szName  = "Cache-Control";
	nlhr.headers[1].szValue = "no-cache";
	nlhr.headers[2].szName  = "Pragma";
	nlhr.headers[2].szValue = "no-cache";
	nlhr.headers[3].szName  = "Connection";
	nlhr.headers[3].szValue = "close";

	// download the page
	nlhrReply = (NETLIBHTTPREQUEST*)CallService(MS_NETLIB_HTTPTRANSACTION, (WPARAM)hNetlibUser, (LPARAM)&nlhr);
	if (nlhrReply) 
	{
		// if the recieved code is 200 OK
		if(nlhrReply->resultCode == 200) 
		{
			if (nlhrReply->dataLength)
			{
				// allocate memory and save the retrieved data
				*szData = (char *)mir_alloc(nlhrReply->dataLength + 2);
				memcpy(*szData, nlhrReply->pData, nlhrReply->dataLength);
				(*szData)[nlhrReply->dataLength] = 0;
			}
		}
	}
	mir_free(szUrl);
}

VOID UpdateList (HWND hwndList)
{
	SendMessage(hwndList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);	

	LVCOLUMN lvc = {0}; 
	// Initialize the LVCOLUMN structure.
	// The mask specifies that the format, width, text, and
	// subitem members of the structure are valid. 
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
	lvc.fmt = LVCFMT_LEFT;
	  
	lvc.iSubItem = 0;
	lvc.pszText = TranslateT("Feed");	
	lvc.cx = 160;     // width of column in pixels
	ListView_InsertColumn(hwndList, 0, &lvc);

	lvc.iSubItem = 1;
	lvc.pszText = TranslateT("URL");	
	lvc.cx = 280;     // width of column in pixels
	ListView_InsertColumn(hwndList, 1, &lvc);

	LVITEM lvI = {0};

	// Some code to create the list-view control.
	// Initialize LVITEM members that are common to all
	// items. 
	HANDLE hContact= (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
	while (hContact != NULL) 
	{
		if(IsMyContact(hContact)) 
		{
			int i = 0;
			lvI.mask = LVIF_TEXT | LVIF_PARAM;
			lvI.iSubItem = 0;
			DBVARIANT dbVar = {0};
			DBGetContactSettingTString(hContact, MODULE, "Nick", &dbVar);
			if (lstrcmp(dbVar.ptszVal, NULL) == 0)
				DBFreeVariant(&dbVar);
			else
			{
				lvI.pszText = dbVar.ptszVal;
				lvI.iItem = i;
				ListView_InsertItem(hwndList, &lvI);
				lvI.mask = LVIF_TEXT;
				lvI.iSubItem = 1;
				DBGetContactSettingTString(hContact, MODULE, "URL", &dbVar);
				if (lstrcmp(dbVar.ptszVal, NULL) == 0)
					DBFreeVariant(&dbVar);
				else
				{
					lvI.pszText = dbVar.ptszVal;
					ListView_SetItem(hwndList, &lvI);
				}
			}
			i += 1;
		}
		hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0);
	}
}

VOID DeleteAllItems(HWND hwndList)
{	
	ListView_DeleteAllItems(hwndList);
}

time_t __stdcall DateToUnixTime(LPCTSTR stamp, BOOL FeedType)
{
	struct tm timestamp;
	TCHAR date[9];
	int i, y;
	time_t t;

	if ( stamp == NULL ) return ( time_t ) 0;

	TCHAR *p = (TCHAR*)stamp;

	if (FeedType)
	{
		// skip '-' chars
		int si = 0, sj = 0;
		while (1) {
			if ( p[si] == _T('-') )
				si++;
			else
				if ( !( p[sj++] = p[si++] ))
					break;
		};
	}
	else
	{
		TCHAR weekday[4], monthstr[4], timezonesign[2];
		INT day, month, year, hour, min, sec, timezoneh, timezonem;
		_stscanf( p, _T("%3s, %d %3s %d %d:%d:%d %1s%02d%02d"), &weekday, &day, &monthstr, &year, &hour, &min, &sec, &timezonesign, &timezoneh, &timezonem);
		if (_tcsicmp(monthstr, _T("Jan")) ==0)
			month = 1;
		if (_tcsicmp(monthstr, _T("Feb")) ==0)
			month = 2;
		if (_tcsicmp(monthstr, _T("Mar")) ==0)
			month = 3;
		if (_tcsicmp(monthstr, _T("Apr")) ==0)
			month = 4;
		if (_tcsicmp(monthstr, _T("May")) ==0)
			month = 5;
		if (_tcsicmp(monthstr, _T("Jun")) ==0)
			month = 6;
		if (_tcsicmp(monthstr, _T("Jul")) ==0)
			month = 7;
		if (_tcsicmp(monthstr, _T("Aug")) ==0)
			month = 8;
		if (_tcsicmp(monthstr, _T("Sep")) ==0)
			month = 9;
		if (_tcsicmp(monthstr, _T("Oct")) ==0)
			month = 10;
		if (_tcsicmp(monthstr, _T("Nov")) ==0)
			month = 11;
		if (_tcsicmp(monthstr, _T("Dec")) ==0)
			month = 12;
		if (lstrcmp(timezonesign, _T("+")) ==0)
			mir_sntprintf(p, 4+2+2+1+2+1+2+1+2+1, _T("%04d%02d%02dT%02d:%02d:%02d"), year, month, day, hour-timezoneh, min-timezonem, sec);
		if (lstrcmp(timezonesign, _T("-")) ==0)
			mir_sntprintf(p, 4+2+2+1+2+1+2+1+2+1, _T("%04d%02d%02dT%02d:%02d:%02d"), year, month, day, hour+timezoneh, min+timezonem, sec);
	}
	// Get the date part
	for ( i=0; *p!='\0' && i<8 && isdigit( *p ); p++,i++ )
		date[i] = *p;

	// Parse year
	if ( i == 6 ) {
		// 2-digit year ( 1970-2069 )
		y = ( date[0]-'0' )*10 + ( date[1]-'0' );
		if ( y < 70 ) y += 100;
	}
	else if ( i == 8 ) {
		// 4-digit year
		y = ( date[0]-'0' )*1000 + ( date[1]-'0' )*100 + ( date[2]-'0' )*10 + date[3]-'0';
		y -= 1900;
	}
	else
		return ( time_t ) 0;
	timestamp.tm_year = y;
	// Parse month
	timestamp.tm_mon = ( date[i-4]-'0' )*10 + date[i-3]-'0' - 1;
	// Parse date
	timestamp.tm_mday = ( date[i-2]-'0' )*10 + date[i-1]-'0';

	// Skip any date/time delimiter
	for ( ; *p!='\0' && !isdigit( *p ); p++ );

	// Parse time
	if ( _stscanf( p, _T("%d:%d:%d"), &timestamp.tm_hour, &timestamp.tm_min, &timestamp.tm_sec ) != 3 )
		return ( time_t ) 0;

	timestamp.tm_isdst = 0;	// DST is already present in _timezone below
	t = mktime( &timestamp );

	_tzset();
	t -= _timezone;

	if ( t >= 0 )
		return t;
	else
		return ( time_t ) 0;
}

VOID CheckCurrentFeed (HANDLE hContact)
{
	char *szData = NULL;
	DBVARIANT dbVar = {0};
	DBGetContactSettingTString(hContact, MODULE, "URL", &dbVar);
	if (lstrcmp(dbVar.ptszVal, NULL) == 0)
		DBFreeVariant(&dbVar);
	else if (DBGetContactSettingWord(hContact, MODULE, "Status", ID_STATUS_OFFLINE) != ID_STATUS_OFFLINE)
	{
		GetNewsData(dbVar.ptszVal, &szData);
		if (szData)
		{
			TCHAR *tszData = mir_a2t(szData);
			int bytesParsed = 0;
			HXML hXml = xi.parseString(tszData, &bytesParsed, NULL);
			mir_free(tszData);
			if(hXml != NULL)
			{
				HXML node = xi.getChild(hXml, 0);
				if (_tcsicmp(xi.getName(node), _T("rss")) == 0)
				{
					for (int i = 0; i < xi.getAttrCount(node); i++)
					{
						if (_tcsicmp(xi.getAttrName(node, i), _T("version")) == 0)
						{
							TCHAR ver[MAX_PATH];
							mir_sntprintf(ver, SIZEOF(ver), _T("RSS %s"), xi.getAttrValue(node, xi.getAttrName(node, i)));
							DBWriteContactSettingTString(hContact, MODULE, "MirVer", ver);
							break;
						}
					}
					HXML chan = xi.getChild(node, 0);
					for (int j = 0; j < xi.getChildCount(chan); j++)
					{
						HXML child = xi.getChild(chan, j);
						if (_tcsicmp(xi.getName(child), _T("title")) == 0)
						{
							DBWriteContactSettingTString(hContact, MODULE, "FirstName", xi.getText(child));
							continue;
						}
						if (_tcsicmp(xi.getName(child), _T("link")) == 0)
						{
							DBWriteContactSettingTString(hContact, MODULE, "Homepage", xi.getText(child));
							continue;
						}
						if (_tcsicmp(xi.getName(child), _T("description")) == 0)
						{
							DBWriteContactSettingTString(hContact, MODULE, "About", xi.getText(child));
							continue;
						}
						if (_tcsicmp(xi.getName(child), _T("language")) == 0)
						{
							DBWriteContactSettingTString(hContact, MODULE, "Language1", xi.getText(child));
							continue;
						}
						if (_tcsicmp(xi.getName(child), _T("item")) == 0)
						{
							LPCTSTR title, link, datetime, descr;
							for (int z = 0; z < xi.getChildCount(child); z++)
							{
								HXML itemval = xi.getChild(child, z);
								if (_tcsicmp(xi.getName(itemval), _T("title")) == 0)
								{
									title = xi.getText(itemval);
									continue;
								}
								if (_tcsicmp(xi.getName(itemval), _T("link")) == 0)
								{
									link = xi.getText(itemval);
									continue;
								}
								if (_tcsicmp(xi.getName(itemval), _T("pubDate")) == 0)
								{
									datetime = xi.getText(itemval);
									continue;
								}
								if (_tcsicmp(xi.getName(itemval), _T("description")) == 0)
								{
									descr = xi.getText(itemval);
									continue;
								}
							}
							TCHAR message[MAX_PATH];
							mir_sntprintf(message, SIZEOF(message), _T("%s\n%s\n%s"), title, link, descr);
							char* pszUtf = mir_utf8encodeT(message);

							time_t stamp = DateToUnixTime(datetime, 0);
							DBEVENTINFO dbei = {0};
							dbei.cbSize = sizeof(dbei);
							dbei.eventType = EVENTTYPE_MESSAGE;
							dbei.flags = DBEF_UTF;
							dbei.szModule = MODULE;
							dbei.timestamp = stamp;//time(NULL);
							dbei.cbBlob = lstrlenA(pszUtf) + 1;
							dbei.pBlob = (PBYTE)pszUtf;
							CallService(MS_DB_EVENT_ADD, (WPARAM)hContact, (LPARAM)&dbei);
							mir_free(pszUtf);
						}

					}
				}
				else if (_tcsicmp(xi.getName(node), _T("feed")) == 0)
				{
					DBWriteContactSettingTString(hContact, MODULE, "MirVer", _T("Atom 3"));
				}
			}
		}
	}
}