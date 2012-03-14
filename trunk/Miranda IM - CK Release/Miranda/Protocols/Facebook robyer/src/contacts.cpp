/*

Facebook plugin for Miranda Instant Messenger
_____________________________________________

Copyright © 2009-11 Michal Zelinka, 2011-12 Robert Pösel

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

bool FacebookProto::IsMyContact(HANDLE hContact, bool include_chat)
{
	const char *proto = reinterpret_cast<char*>( CallService(MS_PROTO_GETCONTACTBASEPROTO,
		reinterpret_cast<WPARAM>(hContact),0) );

	if( proto && strcmp(m_szModuleName,proto) == 0 )
	{
		if( include_chat )
			return true;
		else
			return DBGetContactSettingByte(hContact,m_szModuleName,"ChatRoom",0) == 0;
	} else {
		return false;
	}
}

HANDLE FacebookProto::ContactIDToHContact(std::string user_id)
{
	for(HANDLE hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST,0,0);
	    hContact;
	    hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT,(WPARAM)hContact,0) )
	{
		if(!IsMyContact(hContact))
			continue;

		DBVARIANT dbv;
		if( !DBGetContactSettingString(hContact,m_szModuleName,FACEBOOK_KEY_ID,&dbv) )
		{
			if( strcmp(user_id.c_str(),dbv.pszVal) == 0 )
			{
				DBFreeVariant(&dbv);
				return hContact;
			} else {
				DBFreeVariant(&dbv);
			}
		}
	}

	return 0;
}

HANDLE FacebookProto::AddToContactList(facebook_user* fbu, bool dont_check, const char *new_name)
{
	HANDLE hContact;

	if (!dont_check) {
		// First, check if this contact exists
		hContact = ContactIDToHContact(fbu->user_id);
		if( hContact )
			return hContact;
	}

	// If not, make a new contact!
	hContact = (HANDLE)CallService(MS_DB_CONTACT_ADD, 0, 0);
	if( hContact )
	{
		if( CallService(MS_PROTO_ADDTOCONTACT,(WPARAM)hContact,(LPARAM)m_szModuleName) == 0 )
		{
			DBWriteContactSettingString(hContact,m_szModuleName,FACEBOOK_KEY_ID,fbu->user_id.c_str());
      
			std::string homepage = FACEBOOK_URL_PROFILE;
			homepage += fbu->user_id;
			DBWriteContactSettingString(hContact,m_szModuleName,"Homepage",homepage.c_str());

			DBDeleteContactSetting(hContact, "CList", "MyHandle");
			DBVARIANT dbv;
			if( !DBGetContactSettingTString(NULL,m_szModuleName,FACEBOOK_KEY_DEF_GROUP,&dbv) )
			{
				DBWriteContactSettingTString(hContact,"CList","Group",dbv.ptszVal);
				DBFreeVariant(&dbv);
			}

			if (strlen(new_name) > 0) {
				DBWriteContactSettingUTF8String(hContact, m_szModuleName, FACEBOOK_KEY_NAME, new_name);
				DBWriteContactSettingUTF8String(hContact, m_szModuleName, FACEBOOK_KEY_NICK, new_name);
				DBWriteContactSettingByte(hContact, m_szModuleName, FACEBOOK_KEY_CONTACT_TYPE, 1); // We suppose he is not on server list
			}						

			if (getByte(FACEBOOK_KEY_DISABLE_STATUS_NOTIFY, 0))
				CallService(MS_IGNORE_IGNORE, (WPARAM)hContact, (LPARAM)IGNOREEVENT_USERONLINE);

			return hContact;
		} else {
			CallService(MS_DB_CONTACT_DELETE,(WPARAM)hContact,0);
		}
	}

	return 0;
}

void FacebookProto::SetAllContactStatuses(int status)
{
	for (HANDLE hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST,0,0);
	    hContact;
	    hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT,(WPARAM)hContact,0))
	{
		if (!IsMyContact(hContact))
			continue;

		if (DBGetContactSettingWord(hContact,m_szModuleName,"Status",ID_STATUS_OFFLINE) == status)
			continue;

		DBWriteContactSettingWord(hContact,m_szModuleName,"Status",status);
	}
}

void FacebookProto::DeleteContactFromServer(void *data)
{
	facy.handle_entry( "DeleteContactFromServer" );

	if ( data == NULL )
		return;

	std::string *id = (std::string*)data;
	
	std::string query = "norefresh=false&post_form_id_source=AsyncRequest&lsd=&fb_dtsg=";
	query += facy.dtsg_;
	query += "&post_form_id=";
	query += facy.post_form_id_;
	query += "&uid=";
	query += *id;
	query += "&__user=";
	query += facy.self_.user_id;

	delete data;

	// Get unread inbox threads
	http::response resp = facy.flap( FACEBOOK_REQUEST_DELETE_FRIEND, &query );

	// Process result data
	facy.validate_response(&resp);

	if (resp.code != HTTP_CODE_OK) {
		facy.handle_error( "DeleteContactFromServer" );
	}

	// TODO: better notify - check result code
	NotifyEvent(TranslateT("Deleting contact"), TranslateT("Contact was sucessfully removed from server."), NULL, FACEBOOK_EVENT_OTHER, NULL);	
}

void FacebookProto::AddContactToServer(void *data)
{
	facy.handle_entry( "AddContactToServer" );

	if ( data == NULL )
		return;

	std::string *id = (std::string*)data;
	
	std::string query = "action=add_friend&how_found=profile_button&ref_param=ts&outgoing_id=&unwanted=&logging_location=&no_flyout_on_click=false&ego_log_data=&post_form_id_source=AsyncRequest&lsd=&fb_dtsg=";
	query += facy.dtsg_;
	query += "&post_form_id=";
	query += facy.post_form_id_;	
	query += "&to_friend=";
	query += *id;
	query += "&__user=";
	query += facy.self_.user_id;

	delete data;

	// Get unread inbox threads
	http::response resp = facy.flap( FACEBOOK_REQUEST_ADD_FRIEND, &query );

	// Process result data
	facy.validate_response(&resp);

	if (resp.code != HTTP_CODE_OK || resp.data.find("\"success\":true") == std::string::npos) {
		facy.handle_error( "AddContactToServer" );
	}
// RM TODO: better notify... - check result code
	NotifyEvent(TranslateT("Adding contact"), TranslateT("Request for friendship was sent successfully."), NULL, FACEBOOK_EVENT_OTHER, NULL);
}


HANDLE FacebookProto::GetAwayMsg(HANDLE hContact)
{
	return 0; // Status messages are disabled
}

int FacebookProto::OnContactDeleted(WPARAM wparam,LPARAM)
{
	HANDLE hContact = (HANDLE)wparam;
	
	// TODO: load contact name and show it in messagebox

	if (MessageBox( 0, TranslateT("Do you want to delete this contact also from server list?"), m_tszUserName, MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2 ) == IDYES) {
		DBVARIANT dbv;			
		if( !DBGetContactSettingString(hContact,m_szModuleName,FACEBOOK_KEY_ID,&dbv) )
		{
			if (!isOffline()) { // TODO: is this needed?
				std::string* id = new std::string(dbv.pszVal);
				ForkThread( &FacebookProto::DeleteContactFromServer, this, ( void* )id );
				DBFreeVariant(&dbv);
			}
		}
	}

	//ScopedLock s(facy.buddies_lock_); 
	for (List::Item< facebook_user >* i = facy.buddies.begin( ); i != NULL; i = i->next )
	{
		if (hContact == i->data->handle)
		{
			facy.buddies.erase(i->key);
			break;
		}
	}

	return 0;
}
