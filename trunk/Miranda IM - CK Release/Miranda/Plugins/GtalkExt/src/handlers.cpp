/*
Google Extension plugin for 
Miranda IM: the free IM client for Microsoft* Windows*
and the Jabber plugin.

Authors
			Copyright (C) 2011-2012	bems@jabber.org
									George Hazan (ghazan@jabber.ru)

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

#include "StdAfx.h"
#include "handlers.h"
#include "db.h"
#include "notifications.h"
#include "options.h"
#include "popups.h"

static const LPCTSTR JABBER_IQID = _T("mir_");
static const LPCTSTR JABBER_IQID_FORMAT = _T("mir_%d");

static const LPCTSTR NOTIFY_FEATURE_XMLNS = _T("google:mail:notify");
static const LPCTSTR SETTING_FEATURE_XMLNS = _T("google:setting");
static const LPCTSTR DISCOVERY_XMLNS = _T("http://jabber.org/protocol/disco#info");

static const LPCTSTR MESSAGE_URL_FORMAT_STANDARD = _T("%s/#inbox/%x%08x");
static const LPCTSTR MESSAGE_URL_FORMAT_HTML = _T("%s/h/?v=c&th=%x%08x");

static const LPCTSTR ATTRNAME_TYPE = _T("type");
static const LPCTSTR ATTRNAME_FROM = _T("from");
static const LPCTSTR ATTRNAME_TO = _T("to");
static const LPCTSTR ATTRNAME_URL = _T("url");
static const LPCTSTR ATTRNAME_TID = _T("tid");
static const LPCTSTR ATTRNAME_UNREAD = _T("unread");
static const LPCTSTR ATTRNAME_XMLNS = _T("xmlns");
static const LPCTSTR ATTRNAME_ID = _T("id");
static const LPCTSTR ATTRNAME_TOTAL_MATCHED = _T("total-matched");
static const LPCTSTR ATTRNAME_NAME = _T("name");
static const LPCTSTR ATTRNAME_ADDRESS = _T("address");
static const LPCTSTR ATTRNAME_RESULT_TIME = _T("result-time");
static const LPCTSTR ATTRNAME_NEWER_THAN_TIME = _T("newer-than-time");
static const LPCTSTR ATTRNAME_NEWER_THAN_TID = _T("newer-than-tid");
static const LPCTSTR ATTRNAME_VALUE = _T("value");
static const LPCTSTR ATTRNAME_VAR = _T("var");

static const LPCTSTR IQTYPE_RESULT = _T("result");
static const LPCTSTR IQTYPE_SET = _T("set");
static const LPCTSTR IQTYPE_GET = _T("get");

static const LPCTSTR NODENAME_MAILBOX = _T("mailbox");
static const LPCTSTR NODENAME_QUERY = _T("query");
static const LPCTSTR NODENAME_IQ = _T("iq");
static const LPCTSTR NODENAME_USERSETTING = _T("usersetting");
static const LPCTSTR NODENAME_MAILNOTIFICATIONS = _T("mailnotifications");
static const LPCTSTR NODENAME_SUBJECT = _T("subject");
static const LPCTSTR NODENAME_SNIPPET = _T("snippet");
static const LPCTSTR NODENAME_SENDERS = _T("senders");
static const LPCTSTR NODENAME_FEATURE = _T("feature");
static const LPCTSTR NODENAME_NEW_MAIL = _T("new-mail");

static const LPCTSTR SETTING_TRUE = _T("true");

static const DWORD RESPONSE_TIMEOUT = 1000 * 60 * 60;
static const DWORD TIMER_INTERVAL = 1000 * 60 * 2;

XML_API xi = {0};

#include <tchar.h>

void FormatMessageUrl(LPCTSTR format, LPTSTR buf, LPCTSTR mailbox, LPCTSTR tid)
{
	ULARGE_INTEGER iTid; iTid.QuadPart = _tstoi64(tid);
	int l = lstrlen(buf);
	wsprintf(buf, format, mailbox, iTid.HighPart, iTid.LowPart);
	assert(l >= lstrlen(buf));
}

void MakeUrlHex(LPTSTR url, LPCTSTR tid)
{
	ULARGE_INTEGER iTid; iTid.QuadPart = _tstoi64(tid);
	LPTSTR tidInUrl = _tcsstr(url, tid);
	LPTSTR trail = tidInUrl + lstrlen(tid);
	wsprintf(tidInUrl, _T("%x%08x"), iTid.HighPart, iTid.LowPart);
	wmemmove(tidInUrl + lstrlen(tidInUrl), trail, lstrlen(trail) + 1);
}

LPTSTR ExtractJid(LPCTSTR jidWithRes)
{
	int l;
	for (l = 0; jidWithRes[l] && jidWithRes[l] != '/'; l++) {};
	assert('/' == jidWithRes[l]);

	LPTSTR result = (LPTSTR)malloc((l + 1) * sizeof(TCHAR));
	__try {
		memcpy(result, jidWithRes, l * sizeof(TCHAR));
		result[l] = 0;
	}
	__except (
		free(result),
		EXCEPTION_CONTINUE_SEARCH
	) {};

	return result;
}

BOOL TimerHandler(IJabberInterface *ji, HXML node, void *pUserData);

BOOL InternalListHandler(IJabberInterface *ji, HXML node, LPCTSTR jid, LPCTSTR mailboxUrl)
{
	ULONGLONG maxTid = 0;
	LPCTSTR sMaxTid = NULL;
	int unreadCount = 0;
	for (int i = 0; i < xi.getChildCount(node); i++) {
		LPCTSTR sTid = xi.getAttrValue(xi.getChild(node, i), ATTRNAME_TID);
		ULONGLONG tid = _tcstoui64(sTid, NULL, 10);
		if (tid > maxTid) {
			maxTid = tid;
			sMaxTid = sTid;
		}

		HXML senders = xi.getChildByPath(xi.getChild(node, i), NODENAME_SENDERS, FALSE);
		for (int j = 0; j < xi.getChildCount(senders); j++)
			if (xi.getAttrValue(xi.getChild(senders, j), ATTRNAME_UNREAD)) {
				unreadCount++;
				break;
			}
	}

	LPCSTR acc = GetJidAcc(jid);
	if (!acc) return FALSE;

	if (!unreadCount) {
		SetupPseudocontact(jid, xi.getAttrValue(node, ATTRNAME_TOTAL_MATCHED), acc);
		return TRUE;
	}

	DWORD settings = ReadNotificationSettings(acc);

	if (unreadCount > 5) {
		CloseNotifications(acc, mailboxUrl, jid, FALSE);
		UnreadMailNotification(acc, jid, mailboxUrl, xi.getAttrValue(node, ATTRNAME_TOTAL_MATCHED));
	}
	else
		for (int i = 0; i < xi.getChildCount(node); i++) {
			MAIL_THREAD_NOTIFICATION mtn = {0};
			HXML thread = xi.getChild(node, i);

			mtn.subj = xi.getText(xi.getChildByPath(thread, NODENAME_SUBJECT, FALSE));
			mtn.snip = xi.getText(xi.getChildByPath(thread, NODENAME_SNIPPET, FALSE));

			int threadUnreadCount = 0;
			HXML senders = xi.getChildByPath(thread, NODENAME_SENDERS, FALSE);
			for (int j = 0; threadUnreadCount < SENDER_COUNT && j < xi.getChildCount(senders); j++) {
				HXML sender = xi.getChild(senders, j);
				if (xi.getAttrValue(sender, ATTRNAME_UNREAD)) {
					mtn.senders[threadUnreadCount].name = xi.getAttrValue(sender, ATTRNAME_NAME);
					mtn.senders[threadUnreadCount].addr = xi.getAttrValue(sender, ATTRNAME_ADDRESS);
					threadUnreadCount++;
				}
			}

			LPCTSTR url = xi.getAttrValue(thread, ATTRNAME_URL);
			LPCTSTR tid = xi.getAttrValue(thread, ATTRNAME_TID);

			if (ReadCheckbox(0, IDC_STANDARDVIEW, settings))
				FormatMessageUrl(MESSAGE_URL_FORMAT_STANDARD, (LPTSTR)url, mailboxUrl, tid);
			else
				if (ReadCheckbox(0, IDC_HTMLVIEW, settings))
					FormatMessageUrl(MESSAGE_URL_FORMAT_HTML, (LPTSTR)url, mailboxUrl, tid);
				else
					MakeUrlHex((LPTSTR)url, tid);

			CloseNotifications(acc, url, jid, i);
			UnreadThreadNotification(acc, jid, url, xi.getAttrValue(node, ATTRNAME_TOTAL_MATCHED), &mtn);
		}

	LPCTSTR time = xi.getAttrValue(node, ATTRNAME_RESULT_TIME);
	WriteJidSetting(LAST_MAIL_TIME_FROM_JID, jid, time);
	WriteJidSetting(LAST_THREAD_ID_FROM_JID, jid, sMaxTid);
	return TRUE;
}

BOOL MailListHandler(IJabberInterface *ji, HXML node, void *pUserData)
{
	LPCTSTR jidWithRes = xi.getAttrValue(node, ATTRNAME_TO);
	__try {
		if (!node || lstrcmp(xi.getAttrValue(node, ATTRNAME_TYPE), IQTYPE_RESULT)) return TRUE;

		LPCTSTR jid = xi.getAttrValue(node, ATTRNAME_FROM);
		assert(jid);

		node = xi.getChildByPath(node, NODENAME_MAILBOX, FALSE);
		if (!node) return TRUE; // empty list

		LPCTSTR url = xi.getAttrValue(node, ATTRNAME_URL);

		return InternalListHandler(ji, node, jid, url);
	}
	__finally {
		if (jidWithRes)
			ji->Net()->AddTemporaryIqHandler(TimerHandler, JABBER_IQ_TYPE_RESULT, 0,
				(PVOID)_tcsdup(jidWithRes), TIMER_INTERVAL);
			// Never get a real result stanza. Results elapsed request after WAIT_TIMER_INTERVAL ms
	}
}

void RequestMail(LPCTSTR jidWithRes, IJabberInterface *ji)
{
	HXML child = NULL;
	HXML node = xi.createNode(NODENAME_IQ, NULL, FALSE);
	__try {
		xi.addAttr(node, ATTRNAME_TYPE, IQTYPE_GET);
		xi.addAttr(node, ATTRNAME_FROM, jidWithRes);

		UINT uID;
		LPTSTR lastMailTime = NULL;
		LPTSTR lastThreadId = NULL;
		__try {
			LPTSTR jid = ExtractJid(jidWithRes);
			__try {
				xi.addAttr(node, ATTRNAME_TO, jid);
				lastMailTime = ReadJidSetting(LAST_MAIL_TIME_FROM_JID, jid);
				lastThreadId = ReadJidSetting(LAST_THREAD_ID_FROM_JID, jid);
			}
			__finally {
				free(jid);
			}

			LPTSTR id = (LPTSTR)malloc((_tcslen(JABBER_IQID) + 10 + 1) * sizeof(id[0])); // max int fits 10 chars
			__try {
				wsprintf(id, JABBER_IQID_FORMAT, uID = ji->Net()->SerialNext());
				xi.addAttr(node, ATTRNAME_ID, id);
			}
			__finally {
				free(id);
			}

			child = xi.addChild(node, NODENAME_QUERY, NULL);
			xi.addAttr(child, ATTRNAME_XMLNS, NOTIFY_FEATURE_XMLNS);
			xi.addAttr(child, ATTRNAME_NEWER_THAN_TIME, lastMailTime);
			xi.addAttr(child, ATTRNAME_NEWER_THAN_TID, lastThreadId);
		}
		__finally {
			if (lastMailTime) free(lastMailTime);
			if (lastThreadId) free(lastThreadId);
		}

		if (ji->Net()->SendXmlNode(node))
			ji->Net()->AddTemporaryIqHandler(MailListHandler, JABBER_IQ_TYPE_RESULT, (int)uID, NULL, RESPONSE_TIMEOUT);
	}
	__finally {
		if (child) xi.destroyNode(child);
		if (node) xi.destroyNode(node);
	}
}

BOOL TimerHandler(IJabberInterface *ji, HXML node, void *pUserData)
{
	__try {
		assert(!node); // should not intercept real "mir_0" id
		RequestMail((LPCTSTR)pUserData, ji);
		return FALSE;
	}
	__finally {
		free(pUserData);
	}
}

BOOL NewMailHandler(IJabberInterface *ji, HXML node, void *pUserData)
{
	HXML response = xi.createNode(NODENAME_IQ, NULL, FALSE);
	__try {
		xi.addAttr(response, ATTRNAME_TYPE, IQTYPE_RESULT);

		LPCTSTR attr = xi.getAttrValue(node, ATTRNAME_ID);
		if (!attr) return FALSE;
		xi.addAttr(response, ATTRNAME_ID, attr);

		attr = xi.getAttrValue(node, ATTRNAME_FROM);
		if (attr) xi.addAttr(response, ATTRNAME_TO, attr);

		attr = xi.getAttrValue(node, ATTRNAME_TO);
		if (!attr) return FALSE;
		xi.addAttr(response, ATTRNAME_FROM, attr);

		int bytesSent = ji->Net()->SendXmlNode(response);
		RequestMail(attr, ji);
		return bytesSent > 0;
	}
	__finally {
		xi.destroyNode(response);
	}
}

void SetNotificationSetting(LPCTSTR jidWithResource, IJabberInterface *ji)
{
	HXML child = NULL;
	HXML node = xi.createNode(NODENAME_IQ, NULL, FALSE);
	__try {
		xi.addAttr(node, ATTRNAME_TYPE, IQTYPE_SET);
		xi.addAttr(node, ATTRNAME_FROM, jidWithResource);

		LPTSTR jid = ExtractJid(jidWithResource);
		__try {
			xi.addAttr(node, ATTRNAME_TO, jid);
		}
		__finally {
			free(jid);
		}

		LPTSTR id = (LPTSTR)malloc((_tcslen(JABBER_IQID) + 10 + 1) * sizeof(id[0])); // max int fits 10 chars
		__try {
			wsprintf(id, JABBER_IQID_FORMAT, ji->Net()->SerialNext());
			xi.addAttr(node, ATTRNAME_ID, id);
		}
		__finally {
			free(id);
		}

		child = xi.addChild(node, NODENAME_USERSETTING, NULL);
		xi.addAttr(child, ATTRNAME_XMLNS, SETTING_FEATURE_XMLNS);

		child = xi.addChild(child, NODENAME_MAILNOTIFICATIONS, NULL);
		xi.addAttr(child, ATTRNAME_VALUE, SETTING_TRUE);

		ji->Net()->SendXmlNode(node);
	}
	__finally {
		if (child) xi.destroyNode(child);
		if (node) xi.destroyNode(node);
	}
}

BOOL DiscoverHandler(IJabberInterface *ji, HXML node, void *pUserData)
{
	if (!node) return FALSE;

	LPCTSTR jid = xi.getAttrValue(node, ATTRNAME_TO);
	assert(jid);
	node = xi.getChildByAttrValue(node, NODENAME_QUERY, ATTRNAME_XMLNS, DISCOVERY_XMLNS);

	HXML child = xi.getChildByAttrValue(node, NODENAME_FEATURE, ATTRNAME_VAR, SETTING_FEATURE_XMLNS);
	if (child) SetNotificationSetting(jid, ji);

	child = xi.getChildByAttrValue(node, NODENAME_FEATURE, ATTRNAME_VAR, NOTIFY_FEATURE_XMLNS);
	if (child) {
		ji->Net()->AddIqHandler(NewMailHandler, JABBER_IQ_TYPE_SET, NOTIFY_FEATURE_XMLNS, NODENAME_NEW_MAIL);
		RequestMail(jid, ji);
	}

	return FALSE;
}

extern DWORD itlsRecursion;

BOOL SendHandler(IJabberInterface *ji, HXML node, void *pUserData)
{
	HXML queryNode = xi.getChildByAttrValue(node, NODENAME_QUERY, ATTRNAME_XMLNS, DISCOVERY_XMLNS);
	if (!queryNode) return FALSE;
	if (lstrcmp(xi.getName(node), NODENAME_IQ)) return FALSE;
	if (lstrcmp(xi.getAttrValue(node, ATTRNAME_TYPE), IQTYPE_GET)) return FALSE;

	if (TlsGetValue(itlsRecursion)) return FALSE;
	TlsSetValue(itlsRecursion, (PVOID)TRUE);
	__try {
		UINT id = ji->Net()->SerialNext();
		HXML newNode = xi.createNode(NODENAME_IQ, NULL, FALSE);
		__try {
			xi.addAttr(newNode, ATTRNAME_TYPE, IQTYPE_GET);
			xi.addAttr(newNode, ATTRNAME_TO, xi.getAttrValue(node, ATTRNAME_TO));

			LPTSTR idAttr = (LPTSTR)malloc(((int)_tcslen(JABBER_IQID) + 10) * sizeof(TCHAR));
			__try {
				wsprintf(idAttr, JABBER_IQID_FORMAT, id);
				xi.addAttr(newNode, ATTRNAME_ID, idAttr);
			}
			__finally {
				free(idAttr);
			}

			xi.addAttr(xi.addChild(newNode, NODENAME_QUERY, NULL), ATTRNAME_XMLNS, DISCOVERY_XMLNS);
			ji->Net()->SendXmlNode(newNode);
		}
		__finally {
			xi.destroyNode(newNode);
		}

		ji->Net()->AddTemporaryIqHandler(DiscoverHandler, JABBER_IQ_TYPE_RESULT, id, NULL, RESPONSE_TIMEOUT);
		return FALSE;
	}
	__finally {
		TlsSetValue(itlsRecursion, (PVOID)FALSE);
	}
}

int AccListChanged(WPARAM wParam, LPARAM lParam)
{
	if (PRAC_ADDED == wParam) {
		IJabberInterface *japi = getJabberApi(((PROTOACCOUNT*)lParam)->szModuleName);
		if (japi) japi->Net()->AddSendHandler(SendHandler);
	}
	return 0;
}

int ModulesLoaded(WPARAM wParam, LPARAM lParam)
{
	RenewPseudocontactHandles();
	DetectPopupModule();

	int count;
	PROTOCOLDESCRIPTOR **protos;
	CallService(MS_PROTO_ENUMPROTOCOLS, (WPARAM)&count, (LPARAM)&protos);
	for (int i = 0; i < count; i++) {
		IJabberInterface *japi = getJabberApi(protos[i]->szName);
		if (japi) japi->Net()->AddSendHandler(SendHandler);
	}

	HookOptionsInitialization();

	return 0;
}