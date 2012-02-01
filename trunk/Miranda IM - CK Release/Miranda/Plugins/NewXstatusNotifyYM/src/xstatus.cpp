/*
	NewXstatusNotify YM - Plugin for Miranda IM
	Copyright (c) 2007-2011 yaho
	Copyright (c) 2011-2012 CHEF-KOCH, Mataes

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "common.h"
#include "options.h"
#include "popup.h"
#include "utils.h"
#include "xstatus.h"

extern SortedList *eventList;
extern OPTIONS opt;
extern TEMPLATES templates;

XSTATUSCHANGE *NewXSC(HANDLE hContact, char *szProto, int xstatusType, int action, TCHAR *stzTitle, TCHAR *stzText)
{
	XSTATUSCHANGE *xsc = (XSTATUSCHANGE *)mir_alloc(sizeof(XSTATUSCHANGE));
	xsc->hContact = hContact;
	xsc->szProto = szProto;
	xsc->type = xstatusType;
	xsc->action = action;
	xsc->stzTitle = stzTitle;
	xsc->stzText = stzText;
	return xsc;
}

void FreeXSC(XSTATUSCHANGE *xsc)
{
	if (xsc)
	{
		if (xsc->stzTitle) mir_free(xsc->stzTitle);
		if (xsc->stzText) mir_free(xsc->stzText);
		mir_free(xsc);
		xsc = NULL;
	}
}

void RemoveLoggedEvents(HANDLE hContact) 
{
	for (int i = eventList->realCount-1; i >= 0; i--)
	{
		DBEVENT *dbevent = (DBEVENT *)eventList->items[i];
		if (dbevent->hContact == hContact)
		{
			CallService(MS_DB_EVENT_DELETE, (WPARAM)dbevent->hContact, (LPARAM)dbevent->hDBEvent);
			li.List_Remove(eventList, i);
			mir_free(dbevent);
		}	
	}
}

TCHAR *GetStatusTypeAsString(int type, TCHAR *buff)
{
	switch (type)
	{
		case TYPE_JABBER_MOOD:
			_tcscpy(buff, TranslateT("Mood")); break;
		case TYPE_JABBER_ACTIVITY:
			_tcscpy(buff, TranslateT("Activity")); break;
		case TYPE_ICQ_XSTATUS:
			_tcscpy(buff, TranslateT("Xstatus")); break;
		default:
			_tcscpy(buff, TranslateT("<unknown>"));
	}

	return buff;
}

void ReplaceVars(XSTATUSCHANGE *xsc , TCHAR *Template, TCHAR *delimiter, TCHAR *buff)
{
	buff[0] = 0;
	TCHAR *pch = _tcschr(Template, _T('%'));
	while (pch != NULL)
	{
		size_t len = _tcslen(buff);
		_tcsncat(buff, Template, pch - Template);
		buff[len + pch - Template] = 0;

		if (pch[1] == _T('N') || pch[1] == _T('T') || pch[1] == _T('I') || pch[1] == _T('D') || pch[1] == _T('B'))
		{
			switch (pch[1])
			{
				case _T('N'):
				{
					TCHAR stzType[32];
					_tcscat(buff, GetStatusTypeAsString(xsc->type, stzType));
					break;
				}	
				case _T('T'):
				{
					if (xsc->stzTitle)
						_tcscat(buff, xsc->stzTitle);
					break;
				}
				case _T('I'):
				{
					if (xsc->stzText)
						_tcscat(buff, xsc->stzText);
					break;
				}
				case _T('D'):
				{
					if (xsc->stzText)
					{
						if (_tcscmp(delimiter, _T("%B")) == 0)
							_tcscat(buff, _T("\r\n"));
						else
							_tcscat(buff, delimiter);
					}
					break;
				}
				case _T('B'):
				{
					_tcscat(buff, _T("\r\n"));
					break;
				}
			}

			Template = pch + 2;
		}
		else
		{
			_tcscat(buff, _T("%"));
			Template = pch + 1;
		}
	
		pch = _tcschr(Template, _T('%'));
	}

	// append rest of the text
	if (Template != NULL) 
		_tcscat(buff, Template);
}

void ShowPopup(XSTATUSCHANGE *xsc)
{
	DBVARIANT dbv;
	char szSetting[64];

	POPUPDATAT ppd = {0};
	ppd.lchContact = xsc->hContact;

	switch(xsc->type)
	{
		case TYPE_JABBER_MOOD:
		case TYPE_JABBER_ACTIVITY:
		{
			mir_snprintf(szSetting, SIZEOF(szSetting), "%s/%s/%s", xsc->szProto, (xsc->type == TYPE_JABBER_MOOD) ? "mood" : "activity", "icon");
			if (!DBGetContactSettingString(xsc->hContact, "AdvStatus", szSetting, &dbv)) 
			{
				ppd.lchIcon = (HICON)CallService(MS_SKIN2_GETICON, 0, (LPARAM)dbv.pszVal);
				DBFreeVariant(&dbv);
			}
			break;
		}
		case TYPE_ICQ_XSTATUS:
		{
			int statusId = DBGetContactSettingByte(xsc->hContact, xsc->szProto, "XStatusId", 0);
			ppd.lchIcon = (HICON)CallProtoService(xsc->szProto, PS_ICQ_GETCUSTOMSTATUSICON, statusId, LR_SHARED);
		}
	}

	if (ppd.lchIcon == NULL)
		ppd.lchIcon = LoadSkinnedProtoIcon(xsc->szProto, DBGetContactSettingWord(xsc->hContact, xsc->szProto, "Status", ID_STATUS_ONLINE));

	switch (opt.Colors)
	{
		case POPUP_COLOR_OWN:
			ppd.colorBack = DBGetContactSettingDword(0, MODULE, "40081bg", COLOR_BG_AVAILDEFAULT);
			ppd.colorText = DBGetContactSettingDword(0, MODULE, "40081tx", COLOR_TX_DEFAULT);
			break;
		case POPUP_COLOR_WINDOWS:
			ppd.colorBack = GetSysColor(COLOR_BTNFACE);
			ppd.colorText = GetSysColor(COLOR_WINDOWTEXT);
			break;
		case POPUP_COLOR_POPUP:
			ppd.colorBack = ppd.colorText = 0;
			break;
	}


	_tcscpy(ppd.lptzContactName, (TCHAR *)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)xsc->hContact, GSMDF_TCHAR));
	if (opt.ShowGroup) //add group name to popup title
	{
		if (!DBGetContactSettingTString(xsc->hContact, "CList", "Group", &dbv))
		{
			_tcscat(ppd.lptzContactName, _T(" ("));
			_tcscat(ppd.lptzContactName, dbv.ptszVal);
			_tcscat(ppd.lptzContactName, _T(")"));
			DBFreeVariant(&dbv);
		}
	}

	// cut message if needed
	if (opt.PTruncateMsg && (opt.PMsgLen > 0) && xsc->stzText && (_tcslen(xsc->stzText) > opt.PMsgLen))
	{
		TCHAR buff[MAX_TEXT_LEN + 3];
		_tcsncpy(buff, xsc->stzText, opt.PMsgLen);
		buff[opt.PMsgLen] = 0;
		_tcscat(buff, _T("..."));
		mir_free(xsc->stzText);
		xsc->stzText = mir_tstrdup(buff);
	}

	TCHAR *Template;
	switch (xsc->action)
	{
		case NOTIFY_NEW_XSTATUS:
			Template = templates.PopupNewXstatus; break;
		case NOTIFY_NEW_MESSAGE:
			Template = templates.PopupNewMsg; break;
		case NOTIFY_REMOVE:
			Template = templates.PopupRemove; break;
	}

	TCHAR stzPopupText[2*MAX_TEXT_LEN]; 
	ReplaceVars(xsc, Template, templates.PopupDelimiter, stzPopupText);
	_tcsncpy(ppd.lptzText, stzPopupText, SIZEOF(ppd.lptzText));
	ppd.lptzText[SIZEOF(ppd.lptzText) - 1] = 0;

	ppd.PluginWindowProc = (WNDPROC)PopupDlgProc;
	ppd.iSeconds = opt.PopupTimeout; 
	PUAddPopUpT(&ppd);
}

void PlayXStatusSound(int action)
{
	switch (action)
	{
		case NOTIFY_NEW_XSTATUS:
			SkinPlaySound(XSTATUS_SOUND_CHANGED); break;
		case NOTIFY_NEW_MESSAGE:
			SkinPlaySound(XSTATUS_SOUND_MSGCHANGED); break;
		case NOTIFY_REMOVE:
			SkinPlaySound(XSTATUS_SOUND_REMOVED); break;
	}
}

void LogToMessageWindow(XSTATUSCHANGE *xsc, BOOL opening)
{
	// cut message if needed
	if (opt.LTruncateMsg && (opt.LMsgLen > 0) && xsc->stzText && (_tcslen(xsc->stzText) > opt.LMsgLen))
	{
		TCHAR buff[MAX_TEXT_LEN + 3];
		_tcsncpy(buff, xsc->stzText, opt.LMsgLen);
		buff[opt.LMsgLen] = 0;
		_tcscat(buff, _T("..."));
		mir_free(xsc->stzText);
		xsc->stzText = mir_tstrdup(buff);
	}

	TCHAR *Template;
	switch (xsc->action)
	{
		case NOTIFY_NEW_XSTATUS:
			Template = templates.LogNewXstatus; break;
		case NOTIFY_NEW_MESSAGE:
			Template = templates.LogNewMsg; break;
		case NOTIFY_REMOVE:
			Template = templates.LogRemove; break;
		case NOTIFY_OPENING_ML:
			Template = templates.LogOpening; break;
	}

	TCHAR stzLogText[2*MAX_TEXT_LEN]; 
	TCHAR stzLastLog[2*MAX_TEXT_LEN]; 
	ReplaceVars(xsc, Template, templates.LogDelimiter, stzLogText);
	DBGetStringDefault(xsc->hContact, MODULE, DB_LASTLOG, stzLastLog, SIZEOF(stzLastLog), _T(""));

	if (!opt.KeepInHistory || !(opt.PreventIdentical && _tcscmp(stzLastLog, stzLogText) == 0)) 
	{
		DBWriteContactSettingTString(xsc->hContact, MODULE, DB_LASTLOG, stzLogText);

		char *blob;
#ifdef _UNICODE
		blob = mir_utf8encodeT(stzLogText);
#else
		blob = mir_strdup(stzLogText);
#endif

		DBEVENTINFO dbei = {0};
		dbei.cbSize = sizeof(dbei);
		dbei.cbBlob = (DWORD)strlen(blob) + 1;
		dbei.pBlob = (PBYTE) blob;
		dbei.eventType = EVENTTYPE_STATUSCHANGE;
		dbei.flags = DBEF_READ;
#ifdef _UNICODE
		dbei.flags |= DBEF_UTF;
#endif
		dbei.timestamp = (DWORD)time(NULL);
		dbei.szModule = xsc->szProto;
		HANDLE hDBEvent = (HANDLE)CallService(MS_DB_EVENT_ADD, (WPARAM)xsc->hContact, (LPARAM)&dbei);
		mir_free(blob);

		if (!opt.KeepInHistory) 
		{
			DBEVENT *dbevent = (DBEVENT *)mir_alloc(sizeof(DBEVENT));
			dbevent->hContact = xsc->hContact;
			dbevent->hDBEvent = hDBEvent;	
			li.List_Insert(eventList, dbevent, eventList->realCount);
		}
	}
}

void LogChangeToFile(XSTATUSCHANGE *xsc)
{
	TCHAR stzName[256], stzType[32];
	TCHAR stzDate[32], stzTime[32];
	TCHAR stzText[MAX_TEXT_LEN];

	GetStatusTypeAsString(xsc->type, stzType);

	_tcscpy(stzName, (TCHAR *)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)xsc->hContact, GCDNF_TCHAR));
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL,_T("HH':'mm"), stzTime, SIZEOF(stzTime));
	GetDateFormat(LOCALE_USER_DEFAULT, 0, NULL,_T("dd/MM/yyyy"), stzDate, SIZEOF(stzDate));

	if (xsc->action == NOTIFY_REMOVE)
		mir_sntprintf(stzText, SIZEOF(stzText), TranslateT("%s, %s. %s removed %s.\r\n"), stzDate, stzTime, stzName, stzType);
	else
		mir_sntprintf(stzText, SIZEOF(stzText), TranslateT("%s, %s. %s changed %s to: %s.\r\n"), stzDate, stzTime, stzName, stzType, xsc->stzTitle);

	LogToFile(stzText);
}

void ExtraStatusChanged(XSTATUSCHANGE *xsc) 
{
	BOOL bEnablePopup = true, bEnableSound = true;
	char buff[12] = {0};

	wsprintfA(buff, "%d", ID_STATUS_EXTRASTATUS); 

	if ((DBGetContactSettingByte(0, MODULE, buff, 1) == 0) ||
		(DBGetContactSettingWord(xsc->hContact, xsc->szProto, "Status", ID_STATUS_OFFLINE) == ID_STATUS_OFFLINE) ||
		(!opt.HiddenContactsToo && DBGetContactSettingByte(xsc->hContact, "CList", "Hidden", 0)) ||	
		(opt.TempDisabled))
	{
		return;
	}

	char statusIDs[12], statusIDp[12];
	if (opt.AutoDisable)
	{
		WORD myStatus = (WORD)CallProtoService(xsc->szProto, PS_GETSTATUS, 0, 0); 
		wsprintfA(statusIDs, "s%d", myStatus);
		wsprintfA(statusIDp, "p%d", myStatus);
		bEnableSound = DBGetContactSettingByte(0, MODULE, statusIDs, 1) ? FALSE : TRUE;
		bEnablePopup = DBGetContactSettingByte(0, MODULE, statusIDp, 1) ? FALSE : TRUE;
	}

	if (!(templates.PopupFlags & xsc->action))
		bEnableSound = bEnablePopup = false;

	int xstatusID = DBGetContactSettingByte(xsc->hContact, xsc->szProto, "XStatusId", 0);
	if (opt.PDisableForMusic && xsc->type == TYPE_ICQ_XSTATUS && xstatusID == XSTATUS_MUSIC)
		bEnableSound = bEnablePopup = false;

	if (bEnablePopup && DBGetContactSettingByte(xsc->hContact, MODULE, "EnableXStatusNotify", 1) && TimeoutCheck()) 
		ShowPopup(xsc);

	if (bEnableSound && DBGetContactSettingByte(xsc->hContact, MODULE, "EnableXStatusNotify", 1)) 
		PlayXStatusSound(xsc->action);

	BYTE enableLog = opt.EnableLogging;
	if (opt.LDisableForMusic && xsc->type == TYPE_ICQ_XSTATUS && xstatusID == XSTATUS_MUSIC)
		enableLog = FALSE;

	if (!(templates.LogFlags & xsc->action))
		enableLog = FALSE;

	if (enableLog && DBGetContactSettingByte(xsc->hContact, MODULE, "EnableLogging", 1) && 
		CallService(MS_MSG_MOD_MESSAGEDIALOGOPENED, (WPARAM)xsc->hContact, 0))
	{
		LogToMessageWindow(xsc, FALSE);
	}

	if (opt.Log) 
		LogChangeToFile(xsc);
}

TCHAR *GetDefaultXstatusName(int statusID, char *szProto, TCHAR *buff, int bufflen)
{
	TCHAR nameBuff[64];
	buff[0] = 0;

	ICQ_CUSTOM_STATUS xstatus = {0};
	xstatus.cbSize = sizeof(ICQ_CUSTOM_STATUS);
	xstatus.flags = CSSF_MASK_NAME | CSSF_DEFAULT_NAME | CSSF_TCHAR;
	xstatus.ptszName = nameBuff;
	xstatus.wParam = (WPARAM *)&statusID;
	if (!CallProtoService(szProto, PS_ICQ_GETCUSTOMSTATUSEX, 0, (LPARAM)&xstatus))
	{
		_tcsncpy(buff, TranslateTS(nameBuff), bufflen);
		buff[bufflen - 1] = 0;
	}

	return buff;
}

TCHAR *GetIcqXStatus(HANDLE hContact, char *szProto, char *szValue, TCHAR *buff, int bufflen)
{
	DBVARIANT dbv;
	buff[0] = 0;

	int statusID = DBGetContactSettingByte(hContact, szProto, "XStatusId", -1);
	if (statusID != -1)
	{
		if (!DBGetContactSettingTString(hContact, szProto, szValue, &dbv))
		{
			if ((strcmp(szValue, "XStatusName") == 0) && dbv.ptszVal[0] == 0)
				GetDefaultXstatusName(statusID, szProto, buff, bufflen);
			else
				_tcsncpy(buff, dbv.ptszVal, bufflen);

			buff[bufflen - 1] = 0;
			DBFreeVariant(&dbv);
		}
	}
	
	return buff;
}

TCHAR *GetJabberAdvStatusText(HANDLE hContact, char *szProto, char *szSlot, char *szValue, TCHAR *buff, int bufflen) 
{
	DBVARIANT dbv;
	char szSetting[128];
	buff[0] = 0;

	mir_snprintf(szSetting, SIZEOF(szSetting), "%s/%s/%s", szProto, szSlot, szValue);
	if (!DBGetContactSettingTString(hContact, "AdvStatus", szSetting, &dbv))
	{
		_tcsncpy(buff, dbv.ptszVal, bufflen);
		buff[bufflen - 1] = 0;
		DBFreeVariant(&dbv);
	}

	return buff;
}

void LogXstatusChange(HANDLE hContact, char *szProto, int xstatusType, TCHAR *stzTitle, TCHAR *stzText)
{
	XSTATUSCHANGE *xsc = 
		NewXSC(
			hContact, 
			szProto, 
			xstatusType, 
			NOTIFY_OPENING_ML, 
			stzTitle[0] ? mir_tstrdup(stzTitle): NULL,
			stzText[0] ? mir_tstrdup(stzText) : NULL
		);

	LogToMessageWindow(xsc, TRUE);		
	FreeXSC(xsc);
}

void AddEventThread(void *arg) 
{
	HANDLE hContact = (HANDLE)arg;
	TCHAR stzTitle[MAX_TITLE_LEN], stzText[MAX_TEXT_LEN];

	char *szProto = (char *)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
	if (szProto == NULL) return;

	if (ProtoServiceExists(szProto, JS_PARSE_XMPP_URI))
	{
		GetJabberAdvStatusText(hContact, szProto, "mood", "title", stzTitle, SIZEOF(stzTitle));
		if (stzTitle[0])
		{
			GetJabberAdvStatusText(hContact, szProto, "mood", "text", stzText, SIZEOF(stzText));
			LogXstatusChange(hContact, szProto, TYPE_JABBER_MOOD, stzTitle, stzText);
		}

		GetJabberAdvStatusText(hContact, szProto, "activity", "title", stzTitle, SIZEOF(stzTitle));
		if (stzTitle[0])
		{
			GetJabberAdvStatusText(hContact, szProto, "activity", "text", stzText, SIZEOF(stzText));
			LogXstatusChange(hContact, szProto, TYPE_JABBER_ACTIVITY, stzTitle, stzText);
		}
	}
	else
	{
		GetIcqXStatus(hContact, szProto, "XStatusName", stzTitle, SIZEOF(stzTitle));
		if (stzTitle[0])
		{
			GetIcqXStatus(hContact, szProto, "XStatusMsg", stzText, SIZEOF(stzText));
			LogXstatusChange(hContact, szProto, TYPE_ICQ_XSTATUS, stzTitle, stzText);
		}
	}
}

int OnWindowEvent(WPARAM wParam, LPARAM lParam) 
{
	MessageWindowEventData *mwed = (MessageWindowEventData *)lParam;

	if (mwed->uType == MSG_WINDOW_EVT_CLOSE && !opt.KeepInHistory)
	{
		RemoveLoggedEvents(mwed->hContact);
		return 0;
	}

	if (opt.EnableLogging &&
	   (mwed->uType == MSG_WINDOW_EVT_OPEN) && 
	   (templates.LogFlags & NOTIFY_OPENING_ML) &&
	   (DBGetContactSettingByte(mwed->hContact, MODULE, "EnableLogging", 1) == 1)) 
	{
		mir_forkthread(AddEventThread, mwed->hContact);
	}

	return 0;
}
