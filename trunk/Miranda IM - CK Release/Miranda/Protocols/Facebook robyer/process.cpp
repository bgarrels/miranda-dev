/*

Facebook plugin for Miranda Instant Messenger
_____________________________________________

Copyright � 2009-11 Michal Zelinka

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

File name      : $HeadURL: http://eternityplugins.googlecode.com/svn/trunk/facebook/process.cpp $
Revision       : $Revision: 92 $
Last change by : $Author: n3weRm0re.ewer $
Last change on : $Date: 2011-01-20 21:38:59 +0100 (čt, 20 1 2011) $

*/

#include "common.h"

void FacebookProto::ProcessBuddyList( void* data )
{
	if ( data == NULL )
		return;

	ScopedLock s( facy.buddies_lock_ );

	std::string* resp = (std::string*)data;

	if ( isOffline() )
		goto exit;

	LOG("***** Starting processing buddy list");

	CODE_BLOCK_TRY

	facebook_json_parser* p = new facebook_json_parser( this );
	p->parse_buddy_list( data, &facy.buddies );
	delete p;

	for ( List::Item< facebook_user >* i = facy.buddies.begin( ); i != NULL; )
	{
		LOG("      Now %s: %s", (i->data->status_id == ID_STATUS_OFFLINE ? "offline" : "online"), i->data->real_name.c_str());

		facebook_user* fu;

		if ( i->data->status_id == ID_STATUS_OFFLINE )
		{
			fu = new facebook_user( i->data );
			std::string to_delete( i->key );
			i = i->next;
			facy.buddies.erase( to_delete );
		} else {
			fu = i->data;
			i = i->next;
		}

		ForkThread(&FacebookProto::UpdateContactWorker, this, (void*)fu);
	}

	LOG("***** Buddy list processed");

	CODE_BLOCK_CATCH

	LOG("***** Error processing buddy list: %s", e.what());

	CODE_BLOCK_END

exit:
	delete resp;
}

void FacebookProto::ProcessFriendList( void* data )
{
	if ( data == NULL )
		return;

	std::string* resp = (std::string*)data;

	LOG("***** Starting processing friend list");

	CODE_BLOCK_TRY

	std::map<std::string, facebook_user*> friends;

	facebook_json_parser* p = new facebook_json_parser( this );
	p->parse_friends( data, &friends );
	delete p;


	// Check and update old contacts
	for(HANDLE hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST,0,0);
	    hContact;
	    hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT,(WPARAM)hContact,0) )
	{
		if(!IsMyContact(hContact))
			continue;

		DBVARIANT dbv;
		facebook_user *fbu;
		if( !DBGetContactSettingString(hContact,m_szModuleName,FACEBOOK_KEY_ID,&dbv) ) {
			std::string id = dbv.pszVal;
			DBFreeVariant(&dbv);
			
			std::map< std::string, facebook_user* >::iterator iter;
			
			if ((iter = friends.find(id)) != friends.end()) {
				// Found contact, update it and remove from map
				fbu = iter->second;

				DBVARIANT dbv;
				bool update_required = true;

				// RM TODO: remove, because contacts cant change it, so its only for "first run"
				// Update gender
				if ( DBGetContactSettingByte(hContact, m_szModuleName, "Gender", 0) != fbu->gender )
					DBWriteContactSettingByte(hContact, m_szModuleName, "Gender", fbu->gender );

				// Update real name
				if ( !DBGetContactSettingUTF8String(hContact, m_szModuleName, FACEBOOK_KEY_NAME, &dbv) )
				{
					update_required = strcmp( dbv.pszVal, fbu->real_name.c_str() ) != 0;
					DBFreeVariant(&dbv);
				}
				if ( update_required )
				{
					DBWriteContactSettingUTF8String(hContact, m_szModuleName, FACEBOOK_KEY_NAME, fbu->real_name.c_str());
					DBWriteContactSettingUTF8String(hContact, m_szModuleName, "Nick", fbu->real_name.c_str());
				}

				// Check avatar change
				update_required = true;
				if ( !DBGetContactSettingString(hContact, m_szModuleName, FACEBOOK_KEY_AV_URL, &dbv) )
				{
					update_required = strcmp( dbv.pszVal, fbu->image_url.c_str() ) != 0;
					DBFreeVariant(&dbv);
				}
				if ( update_required ) {
					DBWriteContactSettingString(hContact, m_szModuleName, FACEBOOK_KEY_AV_URL, fbu->image_url.c_str());
					DBWriteContactSettingByte(hContact, m_szModuleName, FACEBOOK_KEY_NEW_AVATAR, 1);
					// TODO: notify that avatar should change
				}
			
				// TODO: remove and wait for avatar request...
				/* if ( update_required || !proto->AvatarExists(&fbu) )
				{
					proto->Log("***** Saving new avatar url: %s",fbu.image_url.c_str());
					proto->ProcessAvatar(hContact,&(fbu.image_url));
				} */
				
				delete fbu;
				friends.erase(iter);
			} else {
				// Contact was removed from "server-list", notify it

				// Wasnt we already been notified about this contact?
				if ( !DBGetContactSettingByte(hContact, m_szModuleName, FACEBOOK_KEY_DELETED, 0) ) {
					DBWriteContactSettingByte(hContact, m_szModuleName, FACEBOOK_KEY_DELETED, 1);

					std::string contactname = id;
					if ( !DBGetContactSettingUTF8String(hContact, m_szModuleName, FACEBOOK_KEY_NAME, &dbv) ) {
						contactname = dbv.pszVal;
						DBFreeVariant(&dbv);
					}

					std::string url = FACEBOOK_URL_PROFILE + id;

					TCHAR* szTitle = mir_a2t_cp(contactname.c_str(), CP_UTF8);
					TCHAR* szUrl = mir_a2t_cp(url.c_str(), CP_UTF8);
					NotifyEvent(szTitle, TranslateT("Contact was removed from server."), hContact, FACEBOOK_EVENT_CLIENT, szUrl);
					mir_free( szTitle );
					// mir_free( szUrl ); // url is free'd in popup procedure
				}
			}
		}
	}

	// Check remain contacts in map and add it to contact list
	for ( std::map< std::string, facebook_user* >::iterator iter = friends.begin(); iter != friends.end(); ++iter )
	{
		facebook_user *fbu = iter->second;
		
		HANDLE hContact = AddToContactList(fbu, true); // This contact is surely new

		DBWriteContactSettingByte(hContact, m_szModuleName, "Gender", fbu->gender );
		DBWriteContactSettingUTF8String(hContact, m_szModuleName, FACEBOOK_KEY_NAME, fbu->real_name.c_str());
		DBWriteContactSettingUTF8String(hContact, m_szModuleName, "Nick", fbu->real_name.c_str());
		DBWriteContactSettingString(hContact, m_szModuleName, FACEBOOK_KEY_AV_URL, fbu->image_url.c_str());
		DBWriteContactSettingByte(hContact, m_szModuleName, FACEBOOK_KEY_NEW_AVATAR, 1);
//		DBWriteContactSettingWord(hContact, m_szModuleName, "Status", ID_STATUS_OFFLINE );
	}

	LOG("***** Friend list processed");

	CODE_BLOCK_CATCH

	LOG("***** Error processing friend list: %s", e.what());

	CODE_BLOCK_END

	delete resp;
}

void FacebookProto::ProcessUnreadMessages( void* )
{
	facy.handle_entry( "messages" );

	std::string data = "sk=inbox&query=is%3Aunread";

	// Get unread inbox threads
	http::response resp = facy.flap( FACEBOOK_REQUEST_ASYNC2, &data );

	// Process result data
	facy.validate_response(&resp);

	if (resp.code != HTTP_CODE_OK) {
		facy.handle_error( "messages" );
		return;
	}

	std::string threadlist = utils::text::slashu_to_utf8(resp.data);
	
	std::string::size_type pos = 0;

	while ( ( pos = threadlist.find( "<li class=\\\"threadRow noDraft unread", pos ) ) != std::string::npos )
	{
		std::string::size_type pos2 = threadlist.find( "<\\/li", pos );		
		std::string thread_content = threadlist.substr( pos, pos2 - pos );

		pos = pos2;

		data = "sk=inbox&query=is%3Aunread&thread_query=is%3Aunread&action=read&tid=";
		data += utils::text::source_get_value( &thread_content, 2, "id=\\\"", "\\\"" );
		
		resp = facy.flap( FACEBOOK_REQUEST_ASYNC2, &data );

		facy.validate_response(&resp);

		if (resp.code != HTTP_CODE_OK) {
			LOG(" !! !! Error when getting messages list");
			continue;
		}
		
		std::string messageslist = utils::text::slashu_to_utf8(resp.data);
		
		std::string user_id = utils::text::source_get_value( &messageslist, 2, "single_thread_id\":", "," );
		if (user_id.empty()) {
			LOG(" !! !! Thread id is empty - this is groupchat message."); // todo remove as this is not such 'error'
			continue;
		}

		messageslist = utils::text::source_get_value( &messageslist, 3, "class=\\\"MessagingMessage", "MessagingMessageUnread", "class=\\\"MessagingComposer" );

		facebook_user fbu = new facebook_user();
		fbu.user_id = user_id;

		HANDLE hContact = AddToContactList(&fbu);

		pos2 = 0;		
		while ( ( pos2 = messageslist.find( "class=\\\"content noh", pos2 ) ) != std::string::npos )
		{	
			std::string message_text;
			message_text = messageslist.substr(pos2, messageslist.find( "<\\/div", pos2) + 6 - pos2);
			message_text = utils::text::source_get_value( &message_text, 2, "\\\">", "<\\/div" );
			message_text = utils::text::trim(
							utils::text::special_expressions_decode(
								utils::text::remove_html( message_text ) ) );

			PROTORECVEVENT recv = {};
			CCSDATA ccs = {};

			recv.flags = PREF_UTF;
			recv.szMessage = const_cast<char*>(message_text.c_str());
			recv.timestamp = static_cast<DWORD>(::time(NULL));

			ccs.hContact = hContact;
			ccs.szProtoService = PSR_MESSAGE;
			ccs.wParam = 0;
			ccs.lParam = reinterpret_cast<LPARAM>(&recv);
			CallService(MS_PROTO_CHAINRECV,0,reinterpret_cast<LPARAM>(&ccs));

			pos2++;
		}

		
	}

}

void FacebookProto::ProcessMessages( void* data )
{
	if ( data == NULL )
		return;

	std::string* resp = (std::string*)data;

	if ( isOffline() )
		goto exit;

	LOG("***** Starting processing messages");

	CODE_BLOCK_TRY

	std::vector< facebook_message* > messages;
	std::vector< facebook_notification* > notifications;

	facebook_json_parser* p = new facebook_json_parser( this );
	p->parse_messages( data, &messages, &notifications );
	delete p;

	for(std::vector<facebook_message*>::size_type i=0; i<messages.size( ); i++)
	{
		if ( messages[i]->user_id != facy.self_.user_id )
		{
			LOG("      Got message: %s", messages[i]->message_text.c_str());
			facebook_user fbu;
			fbu.user_id = messages[i]->user_id;

			HANDLE hContact = AddToContactList(&fbu);
			
			PROTORECVEVENT recv = {};
			CCSDATA ccs = {};

			recv.flags = PREF_UTF;
			recv.szMessage = const_cast<char*>(messages[i]->message_text.c_str());
			recv.timestamp = static_cast<DWORD>(messages[i]->time);

			ccs.hContact = hContact;
			ccs.szProtoService = PSR_MESSAGE;
			ccs.wParam = ID_STATUS_ONLINE;
			ccs.lParam = reinterpret_cast<LPARAM>(&recv);
			CallService(MS_PROTO_CHAINRECV,0,reinterpret_cast<LPARAM>(&ccs));
		}
		delete messages[i];
	}
	messages.clear();

	// RM TODO: needed if notify?
	BYTE notify = getByte( FACEBOOK_KEY_EVENT_NOTIFICATIONS_ENABLE, DEFAULT_EVENT_NOTIFICATIONS_ENABLE );
	for(std::vector<facebook_notification*>::size_type i=0; i<notifications.size( ); i++)
	{
		if ( notify )
		{
			LOG("      Got notification: %s", notifications[i]->text.c_str());
			TCHAR* szTitle = mir_a2t_cp(this->m_szModuleName, CP_UTF8);
			TCHAR* szText = mir_a2t_cp(notifications[i]->text.c_str(), CP_UTF8);
			TCHAR* szUrl = mir_a2t_cp(notifications[i]->link.c_str(), CP_UTF8);
			NotifyEvent( szTitle, szText, ContactIDToHContact(notifications[i]->user_id), FACEBOOK_EVENT_NOTIFICATION, szUrl );
			mir_free( szTitle );
			mir_free( szText );
//			mir_free( szUrl ); // URL is free'd in popup procedure
		}
		delete notifications[i];
	}
	notifications.clear();

	LOG("***** Messages processed");

	CODE_BLOCK_CATCH

	LOG("***** Error processing messages: %s", e.what());

	CODE_BLOCK_END

exit:
	delete resp;
}

void FacebookProto::ProcessNotifications( void* )
{
	if ( isOffline() )
		return;

	if (!getByte( FACEBOOK_KEY_EVENT_NOTIFICATIONS_ENABLE, DEFAULT_EVENT_NOTIFICATIONS_ENABLE ))
		return;

	facy.handle_entry( "notifications" );

	// Get notifications
	http::response resp = facy.flap( FACEBOOK_REQUEST_NOTIFICATIONS );

	// Process result data
	facy.validate_response(&resp);
  
	if (resp.code != HTTP_CODE_OK) {
		facy.handle_error( "notifications" );
		return;
	}


	// Process notifications
	LOG("***** Starting processing notifications");

	CODE_BLOCK_TRY

	std::vector< facebook_notification* > notifications;

	facebook_json_parser* p = new facebook_json_parser( this );
	p->parse_notifications( &(resp.data), &notifications );
	delete p;

	for(std::vector<facebook_notification*>::size_type i=0; i<notifications.size( ); i++)
	{
		LOG("      Got notification: %s", notifications[i]->text.c_str());
		TCHAR* szTitle = mir_a2t_cp(this->m_szModuleName, CP_UTF8);
		TCHAR* szText = mir_a2t_cp(notifications[i]->text.c_str(), CP_UTF8);
		TCHAR* szUrl = mir_a2t_cp(notifications[i]->link.c_str(), CP_UTF8);
		NotifyEvent( szTitle, szText, ContactIDToHContact(notifications[i]->user_id), FACEBOOK_EVENT_NOTIFICATION, szUrl );
		mir_free( szTitle );
		mir_free( szText );
//		mir_free( szUrl ); // URL is free'd in popup procedure

		delete notifications[i];
	}
	notifications.clear();

	LOG("***** Notifications processed");

	CODE_BLOCK_CATCH

	LOG("***** Error processing notifications: %s", e.what());

	CODE_BLOCK_END
}


void FacebookProto::ProcessFeeds( void* data )
{
	if ( data == NULL )
		return;

	std::string* resp = (std::string*)data;

	if (!isOnline())
		goto exit;

	CODE_BLOCK_TRY

	LOG("***** Starting processing feeds");

	std::vector< facebook_newsfeed* > news;

	std::string::size_type pos = 0;
	UINT limit = 0;

	*resp = utils::text::slashu_to_utf8(*resp);

	// RM TODO: first parse against <li>...</li>?
	while ( ( pos = resp->find( "<h6", pos ) ) != std::string::npos && limit <= 25 )
	{
		std::string::size_type pos2 = resp->find( "<abbr title", pos );
		if (pos2 == std::string::npos)
			pos2 = resp->find( "<\\/h6", pos );

		std::string post_content = resp->substr( pos, pos2 - pos );
		std::string rest_content;

		if ( (pos2 = post_content.find( "class=\\\"uiStreamSource\\\"" ), pos) != std::string::npos )
			rest_content = post_content.substr( pos2, post_content.find( "<abbr title=", pos2 ) - pos2 );

		pos += 4;
		facebook_newsfeed* nf = new facebook_newsfeed;

		nf->title = utils::text::source_get_value( &post_content, 3, "<a ", "\\\">", "<\\/a" );
		nf->user_id = utils::text::source_get_value( &post_content, 2, "user.php?id=", "\\\"" );
		
		if ( (pos2 = post_content.find( "<\\/a>" )) != std::string::npos )
			nf->text = post_content.substr( pos2, post_content.length() - pos2 );
		nf->text = nf->text.substr(0, nf->text.find("<form"));

		nf->link = utils::text::source_get_value( &rest_content, 2, "href=\\\"", "\\\">" );

		nf->title = utils::text::trim(
			utils::text::special_expressions_decode(
				utils::text::remove_html( nf->title ) ) );
		
		nf->text = utils::text::trim(
			utils::text::special_expressions_decode(
				utils::text::remove_html(
					utils::text::edit_html( nf->text ) ) ) );
		
		nf->link = utils::text::special_expressions_decode( nf->link );

		if ( !nf->title.length() || !nf->text.length() )
		{
			delete nf;
			continue;
		}

		if (nf->text.length() > 500)
		{
			nf->text = nf->text.substr(0, 500);
			nf->text += "...";
		}

		news.push_back( nf );
		pos++;
		limit++;
	}

	for(std::vector<facebook_newsfeed*>::size_type i=0; i<news.size( ); i++)
	{
		LOG("      Got newsfeed: %s %s", news[i]->title.c_str(), news[i]->text.c_str());
		TCHAR* szTitle = mir_a2t_cp(news[i]->title.c_str(), CP_UTF8);
		TCHAR* szText = mir_a2t_cp(news[i]->text.c_str(), CP_UTF8);
		TCHAR* szUrl = mir_a2t_cp(news[i]->link.c_str(), CP_UTF8);
		NotifyEvent(szTitle,szText,this->ContactIDToHContact(news[i]->user_id),FACEBOOK_EVENT_NEWSFEED, szUrl);
		mir_free(szTitle);
		mir_free(szText);
//		mir_free(szUrl); // URL is free'd in popup procedure
		delete news[i];
	}
	news.clear();

	this->facy.last_feeds_update_ = ::time( NULL );
	setDword( "LastNotificationsUpdate", this->facy.last_feeds_update_ ); // RM TODO: is this useful?

	LOG("***** Feeds processed");

	CODE_BLOCK_CATCH

	LOG("***** Error processing feeds: %s", e.what());

	CODE_BLOCK_END

exit:
	delete resp;
}

void FacebookProto::ProcessAvatar(HANDLE hContact,const std::string* url,bool force)
{
	ForkThread(&FacebookProto::UpdateAvatarWorker, this,
	    new update_avatar(hContact,(*url)));
}
