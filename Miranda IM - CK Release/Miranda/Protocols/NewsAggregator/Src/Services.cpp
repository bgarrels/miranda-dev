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

int g_nStatus = ID_STATUS_OFFLINE;
UINT_PTR timerId = 0;

void SetContactStatus(HANDLE hContact,int nNewStatus)
{
	if(DBGetContactSettingWord(hContact,MODULE,"Status",ID_STATUS_OFFLINE) != nNewStatus)
		DBWriteContactSettingWord(hContact,MODULE,"Status",nNewStatus);
}

static void __cdecl WorkingThread(void* param)
{
	int nStatus = (int)param;
//	UpdateAll(FALSE, FALSE);
	HANDLE hContact= (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
	while (hContact != NULL) 
	{
		if(IsMyContact(hContact)) 
		{
			SetContactStatus(hContact, nStatus);
		}
		hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0);
	}
}

INT_PTR NewsAggrInit(WPARAM wParam,LPARAM lParam)
{
	HANDLE hContact= (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
	while (hContact != NULL) 
	{
		if(IsMyContact(hContact)) 
		{
			SetContactStatus(hContact, ID_STATUS_OFFLINE);
		}
		hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0);
	}
	NetlibInit();
	InitIcons();
	InitMenu();

	// timer for the first update
	timerId = SetTimer(NULL, 0, 5000, (TIMERPROC)timerProc2);  // first update is 5 sec after load

	return 0;
}

INT_PTR NewsAggrPreShutdown(WPARAM wParam,LPARAM lParam)
{
	if (hAddFeedDlg)
	{
		SendMessage(hAddFeedDlg, WM_CLOSE, 0, 0);
	}
	mir_forkthread(WorkingThread, (void*)ID_STATUS_OFFLINE);
	KillTimer(NULL, timerId);
	NetlibUnInit();

	return 0;
}

INT_PTR NewsAggrGetName(WPARAM wParam, LPARAM lParam)
{
	if(lParam)
	{
		lstrcpynA((char*)lParam, MODULE, wParam);
		return 0;
	}
	else
	{
		return 1;
	}
}	

INT_PTR NewsAggrGetCaps(WPARAM wp,LPARAM lp)
{
	int ret = 0;
	switch(wp)
	{        
	case PFLAGNUM_1:
		ret = PF1_IM | PF1_PEER2PEER;
		break;
	case PFLAGNUM_3:
	case PFLAGNUM_2:
		ret = PF2_ONLINE;
		break;
	}

	return ret;
}

INT_PTR NewsAggrSetStatus(WPARAM wp,LPARAM /*lp*/)
{
	int nStatus = wp;
	if((ID_STATUS_ONLINE == nStatus) || (ID_STATUS_OFFLINE == nStatus))
	{
		int nOldStatus = g_nStatus;
		if(nStatus != g_nStatus)
		{
			g_nStatus = nStatus;
			mir_forkthread(WorkingThread, (void*)g_nStatus);
			ProtoBroadcastAck(MODULE, NULL, ACKTYPE_STATUS, ACKRESULT_SUCCESS, (HANDLE)nOldStatus, g_nStatus);
		}

	}

	return 0;
}

INT_PTR NewsAggrGetStatus(WPARAM/* wp*/,LPARAM/* lp*/)
{
	return g_nStatus;
}

INT_PTR NewsAggrLoadIcon(WPARAM wParam,LPARAM lParam)
{
	return (LOWORD(wParam) == PLI_PROTOCOL) ? (INT_PTR)CopyIcon(LoadIconEx("main", FALSE)) : 0;
}

static void __cdecl AckThreadProc(HANDLE param) 
{
	Sleep(100);
	ProtoBroadcastAck(MODULE, param, ACKTYPE_GETINFO, ACKRESULT_SUCCESS, (HANDLE) 1, 0);
}

INT_PTR NewsAggrGetInfo(WPARAM wParam,LPARAM lParam)
{
	CCSDATA *ccs = (CCSDATA *) lParam;
	mir_forkthread(AckThreadProc, ccs->hContact);
	return 0;
}

INT_PTR CheckAllFeeds(WPARAM wParam,LPARAM lParam)
{
	HANDLE hContact= (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
	while (hContact != NULL) 
	{
		if(IsMyContact(hContact)) 
		{
			char *szData = NULL;
			DBVARIANT dbVar = {0};
			DBGetContactSettingTString(hContact ,MODULE, "URL", &dbVar);
			if (lstrcmp(dbVar.ptszVal, NULL) == 0)
				DBFreeVariant(&dbVar);
			else if (DBGetContactSettingWord(hContact ,MODULE, "Status", ID_STATUS_OFFLINE) != ID_STATUS_OFFLINE)
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
									LPCTSTR title, link, timet, descr;
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
											timet = xi.getText(itemval);
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

									DBEVENTINFO dbei = {0};
									dbei.cbSize = sizeof(dbei);
									dbei.eventType = EVENTTYPE_MESSAGE;
									dbei.flags = DBEF_UTF;
									dbei.szModule = MODULE;
									dbei.timestamp = time(NULL);
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
							//text = xi.getText(node);
						//HXML node = xi.getChildByPath(hXml, _T("NewsAggr"), 0);
						//if(node != NULL)
						//{
							//HXML chan = xi.getChildByPath(node, _T("channel"), 0);
							//HXML tit = xi.getChildByPath(chan, _T("title"), 0);
							//LPCTSTR title = xi.getText(tit);
							//TCHAR t[MAX_PATH];
							//_tcscpy(t, title);
					}
				}
			}
		}
		hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0);
	}

	return 0;
}

INT_PTR AddFeed(WPARAM wParam,LPARAM lParam)
{
	hAddFeedDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_ADDFEED), NULL, DlgProcAddFeedOpts);
	ShowWindow(hAddFeedDlg, SW_SHOW);
	return 0;
}

INT_PTR ImportFeeds(WPARAM wParam,LPARAM lParam)
{
	return 0;
}

INT_PTR ExportFeeds(WPARAM wParam,LPARAM lParam)
{
	return 0;
}

INT_PTR CheckFeed(WPARAM wParam,LPARAM lParam)
{
	return 0;
}
