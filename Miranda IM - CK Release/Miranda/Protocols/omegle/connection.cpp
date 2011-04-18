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

void OmegleProto::KillThreads( bool log)
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

void OmegleProto::StopChat(bool disconnect)
{		
	if (!facy.connected_)
		return;

	DeleteChatContact(Translate("Stranger"));

	if (disconnect)
	{
		UpdateChat(NULL, Translate("You have disconnected."), true);

		if (facy.stop())
			LOG("***** Disconnected from stranger %s", facy.chat_id_.c_str());
		else
			LOG("***** Error in disconnecting from stranger %s", facy.chat_id_.c_str());
	}

	facy.connected_ = false;
	facy.chat_id_ = "";

	// Just for case
	KillThreads( );
}

void OmegleProto::NewChat()
{
	if (facy.connected_)
	{
		DeleteChatContact(Translate("Stranger"));
		UpdateChat(NULL, Translate("You have disconnected."), true);
		ClearChat();
		UpdateChat(NULL, Translate("Waiting for Stranger..."), true);

		if (facy.stop())
			LOG("***** Disconnected from stranger %s", facy.chat_id_.c_str());
		else
			LOG("***** Error in disconnecting from stranger %s", facy.chat_id_.c_str());
		
		if (facy.start())
			LOG("***** Waiting for stranger %s", facy.chat_id_.c_str());
		else
			LOG("***** Error in starting connection to stranger %s", facy.chat_id_.c_str());
	
	} else {
		KillThreads( );

		ScopedLock s(events_loop_lock_);

		ClearChat();
		UpdateChat(NULL, Translate("Waiting for Stranger..."), true);
		
		if (facy.start())
		{
			facy.connected_ = true;
			LOG("***** Waiting for stranger %s", facy.chat_id_.c_str());

			m_hMsgLoop = ForkThreadEx( &OmegleProto::EventsLoop, this );
			LOG("***** Started messageloop thread handle %d", m_hMsgLoop);
		}

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
