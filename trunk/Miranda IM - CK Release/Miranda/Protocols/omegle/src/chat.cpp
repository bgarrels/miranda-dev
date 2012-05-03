/*
Omegle protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright � 2011-2012 Robert P�sel

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

#include "common.h"

void OmegleProto::UpdateChat(const TCHAR *name, const TCHAR *message, bool addtolog)
{
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);

	GCEVENT gce  = {sizeof(gce)};
	gce.pDest    = &gcd;
	gce.ptszText = message;
	gce.time     = ::time(NULL);
	gce.dwFlags  = GC_TCHAR;
	gcd.iType  = GC_EVENT_MESSAGE;

	if (name == NULL) {
		gcd.iType = GC_EVENT_INFORMATION;
		name = TranslateT("Server");
		gce.bIsMe = false;
	} else {
		gce.bIsMe = !_tcscmp(name, this->facy.nick_);
	}

	if (addtolog)
		gce.dwFlags  |= GCEF_ADDTOLOG;

	gce.ptszNick = name;
	gce.ptszUID  = gce.ptszNick;

	CallServiceSync(MS_GC_EVENT,0,reinterpret_cast<LPARAM>(&gce));
}

int OmegleProto::OnChatEvent(WPARAM wParam,LPARAM lParam)
{
	GCHOOK *hook = reinterpret_cast<GCHOOK*>(lParam);

	if(strcmp(hook->pDest->pszModule,m_szModuleName))
		return 0;

	switch(hook->pDest->iType)
	{
	case GC_USER_MESSAGE:
	{		
		std::string text = mir_t2a_cp(hook->ptszText,CP_UTF8);

		if (text.empty())
			break;

		if (text.substr(0,1) == "/")
		{ // Process commands
			
			std::string command = "";
			std::string params = "";						

			std::string::size_type pos = 0;
			if ((pos = text.find(" ")) != std::string::npos) {
				command = text.substr(1, pos-1);
				params = text.substr(pos+1);
			} else {
				command = text.substr(1);
			}

			if (!stricmp(command.c_str(), "new"))
			{
				facy.spy_mode_ = false;
				facy.question_ = "";

				ForkThread(&OmegleProto::NewChatWorker, this, NULL);
				break;
			}
			else if (!stricmp(command.c_str(), "quit"))
			{
				ForkThread(&OmegleProto::StopChatWorker, this, NULL);
				break;
			}
			else if (!stricmp(command.c_str(), "spy"))
			{
				facy.spy_mode_ = true;
				facy.question_ = "";
				
				ForkThread(&OmegleProto::NewChatWorker, this, NULL);
				break;
			}
			else if (!stricmp(command.c_str(), "ask"))
			{						
				if (params.empty()) {
					// Load last question
					DBVARIANT dbv;
					if ( !getU8String( OMEGLE_KEY_LAST_QUESTION,&dbv ) ) {
						params = dbv.pszVal;
						DBFreeVariant(&dbv);
					}
					
					if (params.empty()) {
						UpdateChat(NULL, TranslateT("Last question is empty."), false);
						break;
					}
				} else {
					// Save actual question as last question
					if (strlen(params.c_str()) >= OMEGLE_QUESTION_MIN_LENGTH)
					{
						setU8String( OMEGLE_KEY_LAST_QUESTION, params.c_str() );
					}
				}

				if (strlen(params.c_str()) < OMEGLE_QUESTION_MIN_LENGTH)
				{
					UpdateChat(NULL, TranslateT("Your question is too short."), false);
					break;
				}

				facy.spy_mode_ = true;
				facy.question_ = params;
				ForkThread(&OmegleProto::NewChatWorker, this, NULL);
				break;
			}
			else if (!stricmp(command.c_str(), "asl"))
			{
				DBVARIANT dbv;
				if ( !getU8String( OMEGLE_KEY_ASL,&dbv ) ) {
					text = dbv.pszVal;
					DBFreeVariant(&dbv);
				} else {
					UpdateChat(NULL, TranslateT("Your '/asl' setting is empty."), false);
					break;
				}
			}
			else if (!stricmp(command.c_str(), "help"))
			{
				UpdateChat(NULL, TranslateT("There are three different modes of chatting:\
\n1) Standard mode\t - You chat with random stranger privately\
\n2) Question mode\t - You ask two strangers a question and see how they discuss it (you can't join their conversation, only watch)\
\n3) Spy mode\t - You and stranger got a question to discuss from third stranger (he can't join your conversation, only watch)\
\n\nSend '/commands' for available commands."), false);
			}
			else if (!stricmp(command.c_str(), "commands"))
			{
				UpdateChat(NULL, TranslateT("You can use different commands:\
\n/help\t - show info about chat modes\
\n/new\t - start standard mode\
\n/ask <question> - start question mode with your question\
\n/ask\t - start question mode with your last asked question\
\n/spy\t - start spy mode\
\n/quit\t - disconnect from stranger or stop connecting\
\n/asl\t - send your predefined ASL message\
\n\nNote: You can reconnect to different stranger without disconnecting from current one."), false);
				break;
			}
			else
			{
				UpdateChat(NULL, TranslateT("Unknown command. Send '/commands' for list."), false);
				break;
			}

		}

		// Outgoing message
		switch (facy.state_)
		{
			case STATE_ACTIVE:
				LOG("**Chat - Outgoing message: %s", text.c_str());
				ForkThread(&OmegleProto::SendMsgWorker, this, (void*)new std::string(text));
				break;

			case STATE_INACTIVE:
				UpdateChat(NULL, TranslateT("You aren't connected to any stranger. Send '/help' or '/commands' for help."), false);
				break;

			case STATE_SPY:
				UpdateChat(NULL, TranslateT("You can't send messages in question mode."), false);
				break;

			//case STATE_WAITING:
			//case STATE_DISCONNECTING:
			default:
				break;
		}
	
		break;
	}

	case GC_USER_TYPNOTIFY:
		if ( facy.state_ == STATE_ACTIVE )
			ForkThread(&OmegleProto::SendTypingWorker, this, (void*)mir_tstrdup(hook->ptszText));
		break;

	case GC_USER_LEAVE:
	case GC_SESSION_TERMINATE:
		mir_free( facy.nick_ );
		ForkThread(&OmegleProto::StopChatWorker, this, NULL);
		break;
	}

	return 0;
}

/*void OmegleProto::SendChatEvent(int type)
{
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);
	gcd.iType = GC_EVENT_CONTROL;

	GCEVENT gce = {sizeof(gce)};
	gce.dwFlags = GC_TCHAR;
	gce.pDest = &gcd;

	CallServiceSync(MS_GC_EVENT,WINDOW_CLEARLOG,reinterpret_cast<LPARAM>(&gce));
}*/

void OmegleProto::AddChatContact(const TCHAR *name)
{	
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);
	gcd.iType  = GC_EVENT_JOIN;

	GCEVENT gce    = {sizeof(gce)};
	gce.pDest      = &gcd;
	gce.dwFlags    = GC_TCHAR | GCEF_ADDTOLOG;
	gce.ptszNick   = name;
	gce.ptszUID    = gce.ptszNick;
	gce.time       = static_cast<DWORD>(time(0));

	if (name == NULL)
		gce.bIsMe = false;
	else 
		gce.bIsMe = !_tcscmp(name, this->facy.nick_);

	if (gce.bIsMe)
		gce.ptszStatus = _T("Admin");
	else
		gce.ptszStatus = _T("Normal");

	CallServiceSync(MS_GC_EVENT,0,reinterpret_cast<LPARAM>(&gce));
}

void OmegleProto::DeleteChatContact(const TCHAR *name)
{
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);
	gcd.iType  = GC_EVENT_PART;

	GCEVENT gce    = {sizeof(gce)};
	gce.pDest      = &gcd;
	gce.dwFlags    = GC_TCHAR | GCEF_ADDTOLOG;
	gce.ptszNick   = name;
	gce.ptszUID    = gce.ptszNick;
	gce.time       = static_cast<DWORD>(time(0));
	if (name == NULL)
		gce.bIsMe = false;
	else 
		gce.bIsMe = !_tcscmp(name, this->facy.nick_);

	CallServiceSync(MS_GC_EVENT,0,reinterpret_cast<LPARAM>(&gce));
}

int OmegleProto::OnJoinChat(WPARAM,LPARAM suppress)
{	
	GCSESSION gcw = {sizeof(gcw)};

	// Create the group chat session
	gcw.dwFlags   = GC_TCHAR;
	gcw.iType     = GCW_CHATROOM;
	gcw.pszModule = m_szModuleName;
	gcw.ptszName  = m_tszUserName;
	gcw.ptszID    = m_tszUserName;
	CallServiceSync(MS_GC_NEWSESSION, 0, (LPARAM)&gcw);

	if(m_iStatus == ID_STATUS_OFFLINE)
		return 0;

	// Create a group
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);

	GCEVENT gce = {sizeof(gce)};
	gce.pDest = &gcd;
	gce.dwFlags = GC_TCHAR;

	gcd.iType = GC_EVENT_ADDGROUP;

	gce.ptszStatus = _T("Admin");
	CallServiceSync( MS_GC_EVENT, NULL, reinterpret_cast<LPARAM>(&gce) );
	
	gce.ptszStatus = _T("Normal");
	CallServiceSync( MS_GC_EVENT, NULL, reinterpret_cast<LPARAM>(&gce) );

	SetTopic();
		
	// Note: Initialization will finish up in SetChatStatus, called separately
	if(!suppress)
		SetChatStatus(m_iStatus);

	return 0;
}

void OmegleProto::SetTopic(const TCHAR *topic)
{		
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);
	gcd.iType = GC_EVENT_TOPIC;

	GCEVENT gce = {sizeof(gce)};
	gce.pDest = &gcd;
	gce.dwFlags = GC_TCHAR;
	gce.time = ::time(NULL);
	
	if (topic == NULL)
		gce.ptszText = TranslateT("Omegle is a great way of meeting new friends!");
	else
		gce.ptszText = topic;

	CallServiceSync(MS_GC_EVENT,0,  reinterpret_cast<LPARAM>(&gce));
}

int OmegleProto::OnLeaveChat(WPARAM,LPARAM)
{
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);
	gcd.iType = GC_EVENT_CONTROL;

	GCEVENT gce = {sizeof(gce)};
	gce.dwFlags = GC_TCHAR;
	gce.time = ::time(NULL);
	gce.pDest = &gcd;

	CallServiceSync(MS_GC_EVENT,SESSION_OFFLINE,  reinterpret_cast<LPARAM>(&gce));
	CallServiceSync(MS_GC_EVENT,SESSION_TERMINATE,reinterpret_cast<LPARAM>(&gce));

	return 0;
}

void OmegleProto::SetChatStatus(int status)
{
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);
	gcd.iType = GC_EVENT_CONTROL;

	GCEVENT gce = {sizeof(gce)};
	gce.dwFlags = GC_TCHAR;
	gce.time = ::time(NULL);
	gce.pDest = &gcd;
	
	if(status == ID_STATUS_ONLINE)
	{		
		// Free previously loaded name
		mir_free(facy.nick_);
		
		// Load actual name from database
		DBVARIANT dbv;
		if ( !DBGetContactSettingTString(NULL, m_szModuleName, OMEGLE_KEY_NAME, &dbv) )
		{
			facy.nick_ = mir_tstrdup(dbv.ptszVal);
			DBFreeVariant(&dbv);
		} else {
			facy.nick_ = mir_tstrdup(TranslateT("You"));
			DBWriteContactSettingTString(NULL, m_szModuleName, OMEGLE_KEY_NAME, facy.nick_);
		}
		
		// Add self contact
		AddChatContact(facy.nick_);

		CallServiceSync(MS_GC_EVENT,SESSION_INITDONE,reinterpret_cast<LPARAM>(&gce));
		CallServiceSync(MS_GC_EVENT,SESSION_ONLINE,  reinterpret_cast<LPARAM>(&gce));
	}
	else
	{
		CallServiceSync(MS_GC_EVENT,SESSION_OFFLINE,reinterpret_cast<LPARAM>(&gce));
	}
}

void OmegleProto::ClearChat()
{
	if (getByte(OMEGLE_KEY_NO_CLEAR, 0))
		return;

	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);
	gcd.iType = GC_EVENT_CONTROL;

	GCEVENT gce = {sizeof(gce)};
	gce.dwFlags = GC_TCHAR;
	gce.pDest = &gcd;

	CallServiceSync(MS_GC_EVENT,WINDOW_CLEARLOG,reinterpret_cast<LPARAM>(&gce));
}