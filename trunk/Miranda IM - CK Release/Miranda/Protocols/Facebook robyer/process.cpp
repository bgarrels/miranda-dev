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

		facebook_user* fbu;

		if ( i->data->status_id == ID_STATUS_OFFLINE )
		{
			fbu = i->data;

			if (fbu->handle)
				DBWriteContactSettingWord(fbu->handle, m_szModuleName, "Status", ID_STATUS_OFFLINE);

			std::string to_delete( i->key );
			i = i->next;
			facy.buddies.erase( to_delete );
		} else {
			fbu = i->data;
			i = i->next;

			if (!fbu->handle) { // just been added
				fbu->handle = AddToContactList(fbu);

				if (!fbu->real_name.empty()) {
					DBWriteContactSettingUTF8String(fbu->handle,m_szModuleName,FACEBOOK_KEY_NAME,fbu->real_name.c_str());
					DBWriteContactSettingUTF8String(fbu->handle,m_szModuleName,FACEBOOK_KEY_NICK,fbu->real_name.c_str());
				}
			}

			if (DBGetContactSettingWord(fbu->handle,m_szModuleName,"Status", 0) != fbu->status_id ) {
				DBWriteContactSettingWord(fbu->handle,m_szModuleName,"Status", fbu->status_id );
			}

			if (DBGetContactSettingByte(fbu->handle,m_szModuleName,FACEBOOK_KEY_CONTACT_TYPE, 0)) {
				DBDeleteContactSetting(fbu->handle,m_szModuleName,FACEBOOK_KEY_CONTACT_TYPE); // Set type "on server-list" contact
			}

			// Wasn't contact removed from "server-list" someday?
			if ( DBGetContactSettingDword(fbu->handle, m_szModuleName, FACEBOOK_KEY_DELETED, 0) ) {
				DBDeleteContactSetting(fbu->handle, m_szModuleName, FACEBOOK_KEY_DELETED);

				std::string url = FACEBOOK_URL_PROFILE + fbu->user_id;					

				TCHAR* szTitle = mir_a2t_cp(fbu->real_name.c_str(), CP_UTF8);
				TCHAR* szUrl = mir_a2t_cp(url.c_str(), CP_UTF8);
				NotifyEvent(szTitle, TranslateT("Contact is back on server-list."), fbu->handle, FACEBOOK_EVENT_OTHER, szUrl);
				mir_free( szTitle );
				// mir_free( szUrl ); // url is free'd in popup procedure
			}

			// Check avatar change
			CheckAvatarChange(fbu->handle, fbu->image_url);
		}
	}

	LOG("***** Buddy list processed");

	CODE_BLOCK_CATCH

	LOG("***** Error processing buddy list: %s", e.what());

	CODE_BLOCK_END

exit:
	delete resp;
}

void FacebookProto::ProcessFacepiles( void* data )
{
	if ( data == NULL )
		return;

	send_chat *resp = static_cast<send_chat*>(data);

	if ( isOffline() )
		goto exit;

	LOG("***** Starting processing facepiles");	

	CODE_BLOCK_TRY

	std::map< std::string, std::string > facepiles;

	facebook_json_parser* p = new facebook_json_parser( this );
	p->parse_facepiles( &(resp->msg), &facepiles );
	delete p;

	std::map< std::string, std::string >::iterator iter;

	char *chat_users = this->GetChatUsers(resp->chat_id.c_str());
	std::string users = chat_users;
	mir_free(chat_users);

	std::vector<std::string> users_list;
	utils::text::explode(users, " ", &users_list);

	for( std::vector<std::string>::size_type i=0; i<users_list.size( ); i++) {
		iter = facepiles.find( users_list[i] );
		if (iter == facepiles.end()) {
			// Contact is now offline, remove him from chat	
			this->RemoveChatContact(resp->chat_id.c_str(), users_list[i].c_str());
			LOG("      Now offline facepile (%s): [%s]", resp->chat_id.c_str(), users_list[i].c_str());
		} else {
			facepiles.erase(iter);
		}
	}

	for ( iter = facepiles.begin( ); iter != facepiles.end(); ++iter )
	{
		// These contacts are newly online, add them...
		this->AddChatContact(resp->chat_id.c_str(), iter->first.c_str(), iter->second.c_str());		
		LOG("      Now online facepile (%s): [%s] %s", resp->chat_id.c_str(), iter->first.c_str(), iter->second.c_str());
	}

	LOG("***** Facepiles processed");

	CODE_BLOCK_CATCH

	LOG("***** Error processing facepiles: %s", e.what());

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

				// TODO RM: remove, because contacts cant change it, so its only for "first run"
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
					DBWriteContactSettingUTF8String(hContact, m_szModuleName, FACEBOOK_KEY_NICK, fbu->real_name.c_str());
				}

				if (DBGetContactSettingByte(fbu->handle,m_szModuleName,FACEBOOK_KEY_CONTACT_TYPE, 0)) {
					DBDeleteContactSetting(fbu->handle,m_szModuleName,FACEBOOK_KEY_CONTACT_TYPE); // Has type "on server-list" contact
				}

				// Wasn't contact removed from "server-list" someday?
				if ( DBGetContactSettingDword(hContact, m_szModuleName, FACEBOOK_KEY_DELETED, 0) ) {
					DBDeleteContactSetting(hContact, m_szModuleName, FACEBOOK_KEY_DELETED);

					std::string url = FACEBOOK_URL_PROFILE + fbu->user_id;					

					TCHAR* szTitle = mir_a2t_cp(fbu->real_name.c_str(), CP_UTF8);
					TCHAR* szUrl = mir_a2t_cp(url.c_str(), CP_UTF8);
					NotifyEvent(szTitle, TranslateT("Contact is back on server-list."), hContact, FACEBOOK_EVENT_OTHER, szUrl);					
					mir_free( szTitle );
					// mir_free( szUrl ); // url is free'd in popup procedure
				}

				// Check avatar change
				CheckAvatarChange(hContact, fbu->image_url);
			
				delete fbu;
				friends.erase(iter);
			} else {
				// Contact was removed from "server-list", notify it

				// Wasnt we already been notified about this contact?
				if ( !DBGetContactSettingDword(hContact, m_szModuleName, FACEBOOK_KEY_DELETED, 0) 
					&& !DBGetContactSettingByte(hContact, m_szModuleName, FACEBOOK_KEY_CONTACT_TYPE, 0)	) { // And is this contact "on-server" contact?
					
					DBWriteContactSettingDword(hContact, m_szModuleName, FACEBOOK_KEY_DELETED, ::time(NULL));

					std::string contactname = id;
					if ( !DBGetContactSettingUTF8String(hContact, m_szModuleName, FACEBOOK_KEY_NAME, &dbv) ) {
						contactname = dbv.pszVal;
						DBFreeVariant(&dbv);
					}

					std::string url = FACEBOOK_URL_PROFILE + id;

					TCHAR* szTitle = mir_a2t_cp(contactname.c_str(), CP_UTF8);
					TCHAR* szUrl = mir_a2t_cp(url.c_str(), CP_UTF8);
					NotifyEvent(szTitle, TranslateT("Contact is no longer on server-list."), hContact, FACEBOOK_EVENT_OTHER, szUrl);
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
		DBWriteContactSettingUTF8String(hContact, m_szModuleName, FACEBOOK_KEY_NICK, fbu->real_name.c_str());
		DBWriteContactSettingString(hContact, m_szModuleName, FACEBOOK_KEY_AV_URL, fbu->image_url.c_str());
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
	http::response resp = facy.flap( FACEBOOK_REQUEST_ASYNC_GET, &data );

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
		
		resp = facy.flap( FACEBOOK_REQUEST_ASYNC_GET, &data );

		facy.validate_response(&resp);

		if (resp.code != HTTP_CODE_OK) {
			LOG(" !! !! Error when getting messages list");
			continue;
		}
		
		std::string messageslist = utils::text::slashu_to_utf8(resp.data);
		
		std::string user_id = utils::text::source_get_value( &messageslist, 2, "single_thread_id\":", "," );
		if (user_id.empty()) {
			LOG(" !! !! Thread id is empty - this is groupchat message."); // TODO: remove as this is not such 'error'
			continue;
		}

		messageslist = utils::text::source_get_value( &messageslist, 3, "class=\\\"MessagingMessage", "MessagingMessageUnread", "class=\\\"MessagingComposer" );

		facebook_user fbu;
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

			PROTORECVEVENT recv = {0};
			CCSDATA ccs = {0};

			recv.flags = PREF_UTF;
			recv.szMessage = const_cast<char*>(message_text.c_str());
			recv.timestamp = static_cast<DWORD>(::time(NULL));

			ccs.hContact = hContact;
			ccs.szProtoService = PSR_MESSAGE;
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

			HANDLE hContact = AddToContactList(&fbu, false, messages[i]->sender_name.c_str());

			PROTORECVEVENT recv = {0};
			CCSDATA ccs = {0};

			recv.flags = PREF_UTF;
			recv.szMessage = const_cast<char*>(messages[i]->message_text.c_str());
			recv.timestamp = static_cast<DWORD>(messages[i]->time);

			ccs.hContact = hContact;
			ccs.szProtoService = PSR_MESSAGE;
			ccs.lParam = reinterpret_cast<LPARAM>(&recv);
			CallService(MS_PROTO_CHAINRECV,0,reinterpret_cast<LPARAM>(&ccs));
		}
		delete messages[i];
	}
	messages.clear();

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

	// TODO RM: first parse against <li>...</li>?
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

	this->facy.last_feeds_update_ = ::time(NULL);

	LOG("***** Feeds processed");

	CODE_BLOCK_CATCH

	LOG("***** Error processing feeds: %s", e.what());

	CODE_BLOCK_END

exit:
	delete resp;
}
