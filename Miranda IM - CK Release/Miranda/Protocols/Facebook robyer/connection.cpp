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

void FacebookProto::SignOn(void*)
{
	ScopedLock s(signon_lock_);
	LOG("***** Beginning SignOn process");

	KillThreads( );

	if ( NegotiateConnection( ) )
	{
		if (!getByte(FACEBOOK_KEY_SHOW_OLD_FEEDS, DEFAULT_SHOW_OLD_FEEDS))
			facy.last_feeds_update_ = ::time( NULL );

		setDword( "LogonTS", (DWORD)time(NULL) );
		m_hUpdLoop = ForkThreadEx( &FacebookProto::UpdateLoop,  this );
		m_hMsgLoop = ForkThreadEx( &FacebookProto::MessageLoop, this );

		LOG("***** Started messageloop thread handle %d", m_hMsgLoop);
	}
	ToggleStatusMenuItems(isOnline());

	LOG("***** SignOn complete");
}

// RM TODO: this is only for switch to invisible now, change it to use for other statuses too
void FacebookProto::ChangeStatus(void*)
{
	ScopedLock s(signon_lock_);
	LOG("***** Beginning ChangeStatus process");

	int old_status = m_iStatus;	

	facy.home( );
	facy.chat_state( true );
	facy.reconnect( );
	facy.buddy_list( );

	m_iStatus = facy.self_.status_id = m_iDesiredStatus;
	ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_SUCCESS,
	(HANDLE)old_status,m_iStatus);

	LOG("***** ChangeStatus complete");
}

void FacebookProto::SignOff(void*)
{
	ScopedLock s(signon_lock_);
	ScopedLock b(facy.buddies_lock_);
	LOG("##### Beginning SignOff process");

	KillThreads( );

	deleteSetting( "LogonTS" );

	facy.logout( );
	facy.clear_cookies( );
	facy.buddies.clear( );

	int old_status = m_iStatus;
	m_iStatus = facy.self_.status_id = ID_STATUS_OFFLINE;

	ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_SUCCESS,
		(HANDLE)old_status,m_iStatus);

	SetAllContactStatuses( ID_STATUS_OFFLINE );

	ToggleStatusMenuItems(isOnline());

	LOG("##### SignOff complete");
}

bool FacebookProto::NegotiateConnection( )
{
	LOG("***** Negotiating connection with Facebook");

	int old_status = m_iStatus;
	bool error;
	std::string user, pass;
	DBVARIANT dbv = {0};

	error = true;
	if ( !DBGetContactSettingString(NULL,m_szModuleName,FACEBOOK_KEY_LOGIN,&dbv) )
	{
		user = dbv.pszVal;
		DBFreeVariant(&dbv);
		if ( !user.empty() )
			error = false;
	}
	if (error)
	{
		NotifyEvent(m_tszUserName,TranslateT("Please enter a username."),NULL,FACEBOOK_EVENT_CLIENT);
		goto error;
	}

	error = true;
	if ( !DBGetContactSettingString(NULL,m_szModuleName,FACEBOOK_KEY_PASS,&dbv) )
	{
		CallService(MS_DB_CRYPT_DECODESTRING,strlen(dbv.pszVal)+1,
			reinterpret_cast<LPARAM>(dbv.pszVal));
		pass = dbv.pszVal;
		DBFreeVariant(&dbv);
		if ( !pass.empty() )
			error = false;
	}
	if (error)
	{
		NotifyEvent(m_tszUserName,TranslateT("Please enter a password."),NULL,FACEBOOK_EVENT_CLIENT);
		goto error;
	}

	// Load machine name
	if ( !DBGetContactSettingString(NULL,m_szModuleName,FACEBOOK_KEY_DEVICE_ID,&dbv) )
	{
		facy.cookies["datr"] = dbv.pszVal;
		DBFreeVariant(&dbv);
	}

	bool success;
	{
		success = facy.login( user, pass );
		if (success) success = facy.home( );
		if (success) success = facy.chat_state( this->m_iDesiredStatus != ID_STATUS_INVISIBLE );
		if (success) success = facy.reconnect( );
		if (success) success = facy.buddy_list( );
	}

	if (!success)
	{
	error:
		ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_FAILED,
			(HANDLE)old_status,m_iStatus);

		// Set to offline
		old_status = m_iStatus;
		m_iStatus = m_iDesiredStatus = facy.self_.status_id = ID_STATUS_OFFLINE;

		SetAllContactStatuses(ID_STATUS_OFFLINE);
		ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_SUCCESS,
			(HANDLE)old_status,m_iStatus);

		return false;
	} else {
		m_iStatus = facy.self_.status_id = m_iDesiredStatus;

		ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_SUCCESS,
			(HANDLE)old_status,m_iStatus);

		return true;
	}
}

void FacebookProto::UpdateLoop(void *)
{
	//ScopedLock s(update_loop_lock_); // TODO: Required?
	time_t tim = ::time(NULL);
	LOG( ">>>>> Entering Facebook::UpdateLoop[%d]", tim );

	for ( DWORD i = 0; ; i = ++i % 48 )
	{
		if ( !isOnline( ) )
			break;
		if ( i != 0 )
			if ( !facy.invisible_ )
				if ( !facy.buddy_list( ) )
    				break;
		if ( !isOnline( ) )
			break;
		if ( i % 6 == 3 && getByte( FACEBOOK_KEY_EVENT_FEEDS_ENABLE, DEFAULT_EVENT_FEEDS_ENABLE ) )
			if ( !facy.feeds( ) )
				break;
		if ( !isOnline( ) )
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
		if ( !isOnline( ) )
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
