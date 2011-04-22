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

File name      : $HeadURL: http://eternityplugins.googlecode.com/svn/trunk/facebook/contacts.cpp $
Revision       : $Revision: 91 $
Last change by : $Author: n3weRm0re.ewer $
Last change on : $Date: 2011-01-08 11:10:34 +0100 (so, 08 1 2011) $

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

HANDLE FacebookProto::AddToContactList(facebook_user* fbu)
{
	// First, check if this contact exists
	HANDLE hContact = ContactIDToHContact(fbu->user_id);
	if( hContact )
		return hContact;

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
			if (getByte(FACEBOOK_KEY_DISABLE_STATUS_NOTIFY, 0))
				CallService(MS_IGNORE_IGNORE, (WPARAM)hContact, (LPARAM)IGNOREEVENT_USERONLINE);
			return hContact;
		} else {
			CallService(MS_DB_CONTACT_DELETE,(WPARAM)hContact,0);
		}
	}

	return 0;
}

bool FacebookProto::ContactNeedsUpdate(facebook_user* fbu)
{
	return ( ::time(NULL) - fbu->last_update ) > FACEBOOK_USER_UPDATE_RATE;
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

		if (status == ID_STATUS_OFFLINE)
			DBDeleteContactSetting(hContact,m_szModuleName,"IdleTS");

		DBWriteContactSettingWord(hContact,m_szModuleName,"Status",status);
	}
}

void FacebookProto::UpdateContactWorker(void *p)
{
	if ( p == NULL )
		return;

	facebook_user* fbu = ( facebook_user* )p;

	if ( this->isOffline( ) )
		goto exit;

	LOG("***** Updating contact: %s",fbu->real_name.c_str());

	if ( fbu->status_id == ID_STATUS_OFFLINE )
	{
		DBWriteContactSettingWord(fbu->handle,m_szModuleName,"Status",ID_STATUS_OFFLINE );
		DBDeleteContactSetting(fbu->handle,m_szModuleName,"IdleTS");
		goto exit;
	} else { // ID_STATUS_ONLINE + _CONNECTING for self-contact
		bool update_required = false;

		if ( fbu->user_id != facy.self_.user_id ) // if not self-contact
		{ 
			if (!fbu->handle) // just been added
				fbu->handle = AddToContactList(fbu);

			if (!fbu->last_update) { // just come online
				update_required = true;

				// RM TODO: remove this
				DBVARIANT dbv;
				if ( !DBGetContactSettingString(fbu->handle,m_szModuleName,"Homepage",&dbv) )
				{
					DBFreeVariant(&dbv);
				} else {
					std::string url = FACEBOOK_URL_PROFILE + fbu->user_id;
					DBWriteContactSettingString(fbu->handle,m_szModuleName,"Homepage",url.c_str());
				}

			} else // is in database non-actual status?
				update_required = DBGetContactSettingWord(fbu->handle,m_szModuleName,"Status", 0) != (fbu->is_idle ? ID_STATUS_AWAY : ID_STATUS_ONLINE);

			if (update_required)
			{
				DBWriteContactSettingWord(fbu->handle,m_szModuleName,"Status", fbu->is_idle ? ID_STATUS_AWAY : ID_STATUS_ONLINE );
				DBWriteContactSettingDword(fbu->handle,m_szModuleName,"IdleTS", fbu->is_idle ? (DWORD)time(NULL) : 0);
			}
		}

		if ( fbu->user_id == facy.self_.user_id || ContactNeedsUpdate( fbu ) )
		{
			DBVARIANT dbv;
			
			// Update Real name
			update_required = true;
			if ( !DBGetContactSettingUTF8String(fbu->handle,m_szModuleName,FACEBOOK_KEY_NAME,&dbv) )
			{
				update_required = strcmp( dbv.pszVal, fbu->real_name.c_str() ) != 0;
				DBFreeVariant(&dbv);
			}
			if ( update_required )
			{
				DBWriteContactSettingUTF8String(fbu->handle,m_szModuleName,FACEBOOK_KEY_NAME,fbu->real_name.c_str());
				DBWriteContactSettingUTF8String(fbu->handle,m_szModuleName,"Nick",fbu->real_name.c_str());
			}

			// Check avatar change
			update_required = true;
			if ( !DBGetContactSettingString(fbu->handle,m_szModuleName,FACEBOOK_KEY_AV_URL,&dbv) )
			{
				update_required = strcmp( dbv.pszVal, fbu->image_url.c_str() ) != 0;
				DBFreeVariant(&dbv);
			}
			if ( update_required || !AvatarExists(fbu) )
			{
				LOG("***** Saving new avatar url: %s",fbu->image_url.c_str());
				DBWriteContactSettingString(fbu->handle,m_szModuleName,FACEBOOK_KEY_AV_URL,fbu->image_url.c_str());
				ProcessAvatar(fbu->handle,&fbu->image_url);

/*				if ( fbu->user_id == facy.self_.user_id )
				{
					LOG("***** Reporting MyAvatar changed");
					CallService(MS_AV_REPORTMYAVATARCHANGED, (WPARAM)this->m_szModuleName, 0);
				}*/
			}

			// Update update timestamp
			fbu->last_update = ::time( NULL );
		}
	}

exit:
	if ( fbu->status_id == ID_STATUS_OFFLINE && fbu->user_id != this->facy.self_.user_id )
		delete fbu;
}

void FacebookProto::GetAwayMsgWorker(void *hContact)
{
/*	if(hContact == 0)
    return;

	DBVARIANT dbv;
	if( !DBGetContactSettingString(hContact,"CList","StatusMsg",&dbv) )
	{
		ProtoBroadcastAck(m_szModuleName,hContact,ACKTYPE_AWAYMSG,ACKRESULT_SUCCESS,
			(HANDLE)1,(LPARAM)dbv.pszVal);
		DBFreeVariant(&dbv);
	} else {
		ProtoBroadcastAck(m_szModuleName,hContact,ACKTYPE_AWAYMSG,ACKRESULT_FAILED,
			(HANDLE)1,(LPARAM)0);
	}*/
}

HANDLE FacebookProto::GetAwayMsg(HANDLE hContact)
{
	return 0; // Status messages is disabled

	//ForkThread(&FacebookProto::GetAwayMsgWorker, this,hContact);
	//return (HANDLE)1;
}

int FacebookProto::OnContactDeleted(WPARAM wparam,LPARAM)
{
	HANDLE hContact = (HANDLE)wparam;

	if (IsMyContact(hContact))
	{
		ScopedLock s(facy.buddies_lock_);

		for (List::Item< facebook_user >* i = facy.buddies.begin( ); i != NULL; i = i->next )
		{
			if (hContact == i->data->handle)
			{
				facy.buddies.erase(i->key);
				break;
			}
		}
	}

	return 0;
}
