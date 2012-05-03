/*
Skype protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright © 2008-2012 leecher, tweety, jls17,
						Laurent Marechal (aka Peorth)
						Gennady Feldman (aka Gena01) 

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

#include "skype.h"
#include "skypeapi.h"
#include "skypesvc.h"
#include "voiceservice.h"
#include "m_voiceservice.h"

#pragma warning (push)
#pragma warning (disable: 4100) // unreferenced formal parameter
#include "m_utils.h"
#pragma warning (pop)

HANDLE hVoiceNotify = NULL;
BOOL has_voice_service = FALSE;

extern char g_szProtoName[];


BOOL HasVoiceService()
{
	return has_voice_service;
}

void NofifyVoiceService(HANDLE hContact, char *callId, int state) 
{
	VOICE_CALL vc = {0};
	vc.cbSize = sizeof(vc);
	vc.szModule = SKYPE_PROTONAME;
	vc.id = callId;
	vc.flags = VOICE_CALL_CONTACT;
	vc.state = state;
	vc.hContact = hContact;
	NotifyEventHooks(hVoiceNotify, (WPARAM) &vc, 0);
}

static INT_PTR VoiceGetInfo(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return VOICE_SUPPORTED | VOICE_CALL_CONTACT | VOICE_CAN_HOLD;
}

static HANDLE FindContactByCallId(char *callId)
{
	HANDLE hContact;
	int iCmpRes;
	for (hContact = (HANDLE) CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
			hContact != NULL;
			hContact = (HANDLE) CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0)) 
	{
		char *szProto = (char*) CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);

		DBVARIANT dbv;
		if (szProto != NULL 
			&& !strcmp(szProto, SKYPE_PROTONAME) 
			&& DBGetContactSettingByte(hContact, SKYPE_PROTONAME, "ChatRoom", 0) == 0 
			&& !DBGetContactSettingString(hContact, SKYPE_PROTONAME, "CallId", &dbv)) 
		{
			iCmpRes = strcmp(callId, dbv.pszVal);
			DBFreeVariant(&dbv);
			if (iCmpRes == 0) return hContact;
		}
	}

	return NULL;
}

static INT_PTR VoiceCall(WPARAM wParam, LPARAM lParam)
{
	DBVARIANT dbv;

	UNREFERENCED_PARAMETER(lParam);

	if (!wParam) return -1;

	if (DBGetContactSettingString((HANDLE)wParam, SKYPE_PROTONAME, SKYPE_NAME, &dbv)) 
		return -1;

	SkypeSend("CALL %s", dbv.pszVal);
	DBFreeVariant (&dbv);

	return 0;
}

static INT_PTR VoiceAnswer(WPARAM wParam, LPARAM lParam)
{
	char *callId = (char *) wParam;

	UNREFERENCED_PARAMETER(lParam);

	if (!wParam) return -1;
	
	if (FindContactByCallId(callId) == NULL)
		return -1;

	SkypeSend("SET %s STATUS INPROGRESS", callId);
	testfor("ERROR", 200);

	return 0;
}

static INT_PTR VoiceDrop(WPARAM wParam, LPARAM lParam)
{
	char *callId = (char *) wParam;

	UNREFERENCED_PARAMETER(lParam);

	if (!wParam) return -1;

	if (FindContactByCallId(callId) == NULL)
		return -1;

	SkypeSend("SET %s STATUS FINISHED", callId);

	return 0;
}

static INT_PTR VoiceHold(WPARAM wParam, LPARAM lParam)
{
	char *callId = (char *) wParam;

	UNREFERENCED_PARAMETER(lParam);

	if (!wParam) return -1;

	if (FindContactByCallId(callId) == NULL)
		return -1;

	SkypeSend("SET %s STATUS ONHOLD", callId);

	return 0;
}

void VoiceServiceInit() 
{
	// leecher, 26.03.2011: Did this ever work in the old versions?? 
	char szEvent[MAXMODULELABELLENGTH];

	_snprintf (szEvent, sizeof(szEvent), "%s%s", SKYPE_PROTONAME, PE_VOICE_CALL_STATE);
	hVoiceNotify = CreateHookableEvent( szEvent );
	CreateProtoService( PS_VOICE_GETINFO, VoiceGetInfo );
	CreateProtoService( PS_VOICE_CALL, VoiceCall );
	CreateProtoService( PS_VOICE_ANSWERCALL, VoiceAnswer );
	CreateProtoService( PS_VOICE_DROPCALL, VoiceDrop );
	CreateProtoService( PS_VOICE_HOLDCALL, VoiceHold );
}

void VoiceServiceExit()
{
	DestroyHookableEvent(hVoiceNotify);
}

void VoiceServiceModulesLoaded() 
{
	has_voice_service = ServiceExists(MS_VOICESERVICE_REGISTER);
}