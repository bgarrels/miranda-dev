/*

Omegle plugin for Miranda Instant Messenger
_____________________________________________

Copyright © 2011 Robert Pösel

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "common.h"

void OmegleProto::KillThreads( bool log )
{
	// Kill the old threads if they are still around
	if(m_hMsgLoop != NULL)
	{
		if ( log )
			LOG("***** Requesting MessageLoop to exit... %d", m_hMsgLoop );
		WaitForSingleObject(m_hMsgLoop,IGNORE);
		TerminateThread(m_hMsgLoop, 0);
		ReleaseMutex(m_hMsgLoop);
	}
}

void OmegleProto::SignOn(void*)
{
	ScopedLock s(signon_lock_);
	LOG("***** Beginning SignOn process");

	int old_status = m_iStatus;

	if (facy.home()) {
		m_iStatus = m_iDesiredStatus;
		ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_SUCCESS,
			(HANDLE)old_status,m_iStatus);

		setDword( "LogonTS", (DWORD)time(NULL) );
		ClearChat();
		OnJoinChat(0,false);
	} else {
		ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_FAILED,
			(HANDLE)old_status,m_iStatus);

		// Set to offline
		old_status = m_iStatus;
		m_iStatus = m_iDesiredStatus = ID_STATUS_OFFLINE;

		ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_SUCCESS,
			(HANDLE)old_status,m_iStatus);
	}

	LOG("***** SignOn complete");
}

void OmegleProto::SignOff(void*)
{
	ScopedLock s(signon_lock_);
	LOG("##### Beginning SignOff process");
	
	StopChat();

	facy.server_ = "";
	deleteSetting( "LogonTS" );

	OnLeaveChat(NULL, NULL);

	int old_status = m_iStatus;
	m_iStatus = ID_STATUS_OFFLINE;

	ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_SUCCESS,
		(HANDLE)old_status,m_iStatus);

	LOG("##### SignOff complete");
}

void OmegleProto::StartChat()
{
	// tohle se bude nejak volat kdyz uzivatel bude chtit zacit chatovat...

/*musi byt connected == false
  - zapne se messages thread, musi byt server_ definovany
  - stranger se do chatu prida az podle eventu... nebo ne?
  */
	KillThreads( );

	if (facy.connected_)
		return;

	ScopedLock s(events_loop_lock_);

	ClearChat();
	AddChatContact("Stranger");

	if (facy.start())
	{
		facy.connected_ = true;
		LOG("***** Connected to stranger %s", facy.chat_id_.c_str());

		m_hMsgLoop = ForkThreadEx( &OmegleProto::EventsLoop, this );
		LOG("***** Started messageloop thread handle %d", m_hMsgLoop);
	}
}

void OmegleProto::StopChat(bool disconnect)
{
	// tohle se bude nejak volat kdyz uzivatel bude chtit prestat

	//musi byt connected == true u vsech nize
	//- vypne se messages thread
	// odpoji se stranger z mistnosti

	KillThreads( );
	
	// If we are chatting with some stranger, disconnect
	if (!facy.connected_)
		return;

	facy.connected_ = false;

	ScopedLock s(events_loop_lock_);
	DeleteChatContact("Stranger");	

	if (disconnect && facy.stop()) {
		LOG("***** Disconnected from stranger %s", facy.chat_id_.c_str());
		UpdateChat(NULL, "You have disconnected.", true);
	}

	facy.chat_id_ = "";
}

void OmegleProto::NewChat()
{
	if (facy.connected_)
	{
		DeleteChatContact("Stranger");
		ClearChat();
		AddChatContact("Stranger");

		if (facy.stop()) {
			LOG("***** Disconnected from stranger %s", facy.chat_id_.c_str());
		}
		if (facy.start())
		{
			LOG("***** Connected to stranger %s", facy.chat_id_.c_str());
		}
	} else {
		StartChat();
	}
}

void OmegleProto::EventsLoop(void *)
{
	ScopedLock s(events_loop_lock_);

	time_t tim = ::time(NULL);
	LOG( ">>>>> Entering Omegle::MessageLoop[%d]", tim );

	while ( facy.events( ) )
	{
		if ( !facy.connected_ || !isOnline() )
			break;
		LOG( "***** OmegleProto::MessageLoop[%d] refreshing...", tim );
	}

	LOG( "<<<<< Exiting OmegleProto::MessageLoop[%d]", tim );
}
