/*

Facebook plugin for Miranda Instant Messenger
_____________________________________________

Copyright © 2009-11 Michal Zelinka

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

File name      : $HeadURL: http://eternityplugins.googlecode.com/svn/trunk/facebook/connection.cpp $
Revision       : $Revision: 91 $
Last change by : $Author: n3weRm0re.ewer $
Last change on : $Date: 2011-01-08 11:10:34 +0100 (so, 08 1 2011) $

*/

#include "common.h"

void FacebookProto::KillThreads( bool log )
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
	
	if(m_hUpdLoop != NULL)
	{
		if ( log )
			LOG("***** Requesting UpdateLoop to exit");
		WaitForSingleObject(m_hUpdLoop,IGNORE);
		TerminateThread(m_hUpdLoop, 0);
		ReleaseMutex(m_hUpdLoop);
	}
}

void FacebookProto::ChangeStatus(void*)
{
	ScopedLock s(signon_lock_);
	ScopedLock b(facy.buddies_lock_);
	
	int new_status = m_iDesiredStatus;
	int old_status = m_iStatus;

	m_iStatus = facy.self_.status_id = ID_STATUS_CONNECTING;
	ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_SUCCESS, (HANDLE)old_status, m_iStatus);

	if ( new_status == ID_STATUS_OFFLINE )
	{ // Logout	
		LOG("##### Beginning SignOff process");

		KillThreads( );

		deleteSetting( "LogonTS" );

		facy.logout( );
		facy.clear_cookies( );
		facy.buddies.clear( );

		m_iStatus = facy.self_.status_id = ID_STATUS_OFFLINE;
		ProtoBroadcastAck(m_szModuleName, 0, ACKTYPE_STATUS, ACKRESULT_SUCCESS, (HANDLE)old_status, m_iStatus);

		SetAllContactStatuses( ID_STATUS_OFFLINE );

		ToggleStatusMenuItems(false);

		LOG("##### SignOff complete");

		return;
	}
	else if ( old_status == ID_STATUS_OFFLINE )
	{ // Login
		LOG("***** Beginning SignOn process");

		KillThreads( );

		if ( NegotiateConnection( ) )
		{			
			facy.last_feeds_update_ = ::time( NULL );

			facy.home();
			facy.reconnect();

			facy.load_friends();

			setDword( "LogonTS", (DWORD)time(NULL) );
			m_hUpdLoop = ForkThreadEx( &FacebookProto::UpdateLoop,  this );
			m_hMsgLoop = ForkThreadEx( &FacebookProto::MessageLoop, this );

			LOG("***** Started messageloop thread handle %d", m_hMsgLoop);
		} else {
			ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_FAILED,
				(HANDLE)old_status,m_iStatus);

			// Set to offline
			m_iStatus = m_iDesiredStatus = facy.self_.status_id = ID_STATUS_OFFLINE;
			ProtoBroadcastAck(m_szModuleName, 0, ACKTYPE_STATUS, ACKRESULT_SUCCESS, (HANDLE)old_status, m_iStatus);

			LOG("***** SignOn failed");

			return;
		}

		ToggleStatusMenuItems(true);
		LOG("***** SignOn complete");
	}
	else if ( new_status == ID_STATUS_INVISIBLE )
	{
		facy.buddies.clear( );
		this->SetAllContactStatuses( ID_STATUS_OFFLINE );
	}

	facy.chat_state( m_iDesiredStatus != ID_STATUS_INVISIBLE );	
	facy.buddy_list( );

	m_iStatus = facy.self_.status_id = m_iDesiredStatus;
	ProtoBroadcastAck(m_szModuleName, 0, ACKTYPE_STATUS, ACKRESULT_SUCCESS, (HANDLE)old_status, m_iStatus);

	LOG("***** ChangeStatus complete");
}

/** Return true on success, false on error. */
bool FacebookProto::NegotiateConnection( )
{
	LOG("***** Negotiating connection with Facebook");

	bool error;
	std::string user, pass;
	DBVARIANT dbv = {0};

	error = true;
	if ( !DBGetContactSettingString(NULL,m_szModuleName,FACEBOOK_KEY_LOGIN,&dbv) )
	{
		user = dbv.pszVal;
		DBFreeVariant(&dbv);
		error = user.empty();
	}
	if (error)
	{
		NotifyEvent(m_tszUserName,TranslateT("Please enter a username."),NULL,FACEBOOK_EVENT_CLIENT);
		return false;
	}

	error = true;
	if ( !DBGetContactSettingString(NULL,m_szModuleName,FACEBOOK_KEY_PASS,&dbv) )
	{
		CallService(MS_DB_CRYPT_DECODESTRING,strlen(dbv.pszVal)+1,
			reinterpret_cast<LPARAM>(dbv.pszVal));
		pass = dbv.pszVal;
		DBFreeVariant(&dbv);
		error = pass.empty();
	}
	if (error)
	{
		NotifyEvent(m_tszUserName,TranslateT("Please enter a password."),NULL,FACEBOOK_EVENT_CLIENT);
		return false;
	}

	// Load machine name
	if ( !DBGetContactSettingString(NULL,m_szModuleName,FACEBOOK_KEY_DEVICE_ID,&dbv) )
	{
		facy.cookies["datr"] = dbv.pszVal;
		DBFreeVariant(&dbv);
	}

	return facy.login( user, pass );
}

void FacebookProto::UpdateLoop(void *)
{
	//ScopedLock s(update_loop_lock_); // TODO: Required?
	time_t tim = ::time(NULL);
	LOG( ">>>>> Entering Facebook::UpdateLoop[%d]", tim );

	for ( int i = -1; !isOffline(); i = ++i % 6 )
	{
		if ( i != -1 )
			if ( !facy.invisible_ )
				if ( !facy.buddy_list( ) )
    				break;
		if ( i == 2 && getByte( FACEBOOK_KEY_EVENT_FEEDS_ENABLE, DEFAULT_EVENT_FEEDS_ENABLE ) )
			if ( !facy.feeds( ) )
				break;
		LOG( "***** FacebookProto::UpdateLoop[%d] going to sleep...", tim );
		if ( SleepEx( GetPollRate( ) * 1000, true ) == WAIT_IO_COMPLETION )
			break;
		LOG( "***** FacebookProto::UpdateLoop[%d] waking up...", tim );
	}

	LOG( "<<<<< Exiting FacebookProto::UpdateLoop[%d]", tim );
}

void FacebookProto::MessageLoop(void *)
{
	//ScopedLock s(message_loop_lock_); // TODO: Required?
	time_t tim = ::time(NULL);
	LOG( ">>>>> Entering Facebook::MessageLoop[%d]", tim );

	while ( facy.channel( ) )
	{
		if ( isOffline() )
			break;
		LOG( "***** FacebookProto::MessageLoop[%d] refreshing...", tim );
	}

	LOG( "<<<<< Exiting FacebookProto::MessageLoop[%d]", tim );
}

BYTE FacebookProto::GetPollRate( )
{
	BYTE poll_rate = getByte( FACEBOOK_KEY_POLL_RATE, FACEBOOK_DEFAULT_POLL_RATE );

	return (
	    ( poll_rate >= FACEBOOK_MINIMAL_POLL_RATE &&
	      poll_rate <= FACEBOOK_MAXIMAL_POLL_RATE )
	    ? poll_rate : FACEBOOK_DEFAULT_POLL_RATE );
}
