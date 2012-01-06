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
			DBGetContactSettingTString(hContact ,MODULE, "Nick", &dbVar);
			if (lstrcmp(dbVar.ptszVal, NULL) == 0)
				DBFreeVariant(&dbVar);
			else
			{
				lvI.pszText = dbVar.ptszVal;
				lvI.iItem = i;
				ListView_InsertItem(hwndList, &lvI);
				lvI.mask = LVIF_TEXT;
				lvI.iSubItem = 1;
				DBGetContactSettingTString(hContact ,MODULE, "URL", &dbVar);
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