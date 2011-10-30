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

File name      : $HeadURL: http://eternityplugins.googlecode.com/svn/trunk/facebook/messages.cpp $
Revision       : $Revision: 95 $
Last change by : $Author: n3weRm0re.ewer $
Last change on : $Date: 2011-02-07 18:19:39 +0100 (po, 07 2 2011) $

*/

#include "common.h"

struct send_direct
{
	send_direct(HANDLE hContact,const std::string &msg, HANDLE msgid) : hContact(hContact), msg(msg), msgid(msgid) {}
	HANDLE hContact;
	std::string msg;
	HANDLE msgid;
};

struct send_typing
{
	send_typing(HANDLE hContact,const int status) : hContact(hContact), status(status) {}
	HANDLE hContact;
	int status;
};

struct send_messaging
{
	send_messaging(const std::string &user_id, const int type) : user_id(user_id), type(type) {}
	std::string user_id;
	int type;
};

int FacebookProto::RecvMsg(HANDLE hContact, PROTORECVEVENT *pre)
{
	DBVARIANT dbv;

	if( !DBGetContactSettingString(hContact,m_szModuleName,FACEBOOK_KEY_ID,&dbv) )
	{
		ForkThread( &FacebookProto::MessagingWorker, this, new send_messaging(dbv.pszVal, FACEBOOK_RECV_MESSAGE ) );
		DBFreeVariant(&dbv);
	}

	CallService(MS_PROTO_CONTACTISTYPING, (WPARAM)hContact, (LPARAM)PROTOTYPE_CONTACTTYPING_OFF);

	CCSDATA ccs = { hContact,PSR_MESSAGE,0,reinterpret_cast<LPARAM>(pre) };
	return CallService(MS_PROTO_RECVMSG,0,reinterpret_cast<LPARAM>(&ccs));
}

void FacebookProto::SendMsgWorker(void *p)
{
	if(p == NULL)
		return;
  
	ScopedLock s( facy.send_message_lock_ );

	send_direct *data = static_cast<send_direct*>(p);

	DBVARIANT dbv;

	if ( !isOnline( ) )
	{
		ProtoBroadcastAck(m_szModuleName, data->hContact, ACKTYPE_MESSAGE, ACKRESULT_FAILED, data->msgid, (LPARAM)Translate("You cannot send messages when you are offline."));
	}
	else if( !DBGetContactSettingString(data->hContact,m_szModuleName,FACEBOOK_KEY_ID,&dbv) )
	{
		int retries = 5;
		std::string error_text = "";
		bool result = false;
		while (!result && retries > 0) {
			result = facy.send_message(dbv.pszVal, data->msg, &error_text, retries % 2 == 0 );
			retries--;
		}
		if (result) {
			ProtoBroadcastAck(m_szModuleName,data->hContact,ACKTYPE_MESSAGE,ACKRESULT_SUCCESS, data->msgid,0);
			MessagingWorker( new send_messaging(dbv.pszVal, FACEBOOK_SEND_MESSAGE ) );
		} else {
			char *err = mir_utf8decodeA(error_text.c_str());
			ProtoBroadcastAck(m_szModuleName,data->hContact,ACKTYPE_MESSAGE,ACKRESULT_FAILED, data->msgid,(LPARAM)err);
		}
		DBFreeVariant(&dbv);
	}

	delete data;
}

int FacebookProto::SendMsg(HANDLE hContact, int flags, const char *msg)
{
	// TODO: msg comes as Unicode (retyped wchar_t*), why should we convert it as ANSI to UTF-8? o_O
	if ( flags & PREF_UNICODE )
		msg = mir_utf8encode(msg);
  
	facy.msgid_ = (facy.msgid_ % 1024)+1;
	ForkThread( &FacebookProto::SendMsgWorker, this,new send_direct(hContact,msg,(HANDLE)facy.msgid_) );
	return facy.msgid_;
}

int FacebookProto::UserIsTyping(HANDLE hContact,int type)
{ 
	if (hContact && isOnline())
		ForkThread(&FacebookProto::SendTypingWorker, this,new send_typing(hContact,type));

	return 0;
}

void FacebookProto::SendTypingWorker(void *p)
{
	if(p == NULL)
		return;

	send_typing *typing = static_cast<send_typing*>(p);

	// TODO RM: maybe better send typing optimalization
	facy.is_typing_ = (typing->status == PROTOTYPE_SELFTYPING_ON);
	SleepEx( 2000, true );

	if ( !facy.is_typing_ == (typing->status == PROTOTYPE_SELFTYPING_ON) )
	{
		delete typing;
		return;
	}
		
	DBVARIANT dbv;
	if( !DBGetContactSettingString(typing->hContact,m_szModuleName,FACEBOOK_KEY_ID,&dbv) )
	{
		std::string data = "typ=";
		data += ( typing->status == PROTOTYPE_SELFTYPING_ON ) ? "1" : "0"; // PROTOTYPE_SELFTYPING_OFF
		data += "&to=";
		data += dbv.pszVal;
		data += "&source=chat";
		data += "&fb_dtsg=" + facy.dtsg_;
		data += "&post_form_id=";
		data += ( facy.post_form_id_.length( ) ) ? facy.post_form_id_ : "0";
		data += "&post_form_id_source=AsyncRequest&lsd=";

		http::response resp = facy.flap( FACEBOOK_REQUEST_TYPING_SEND, &data );

		DBFreeVariant(&dbv);
	}		

	delete typing;
}

void FacebookProto::MessagingWorker(void *p)
{
	if (p == NULL)
		return;

	send_messaging *data = static_cast<send_messaging*>(p);

	if (data->type == FACEBOOK_RECV_MESSAGE)
		facy.chat_mark_read( data->user_id );

	if ( DBGetContactSettingByte(NULL, m_szModuleName, FACEBOOK_KEY_CLOSE_WINDOWS_ENABLE, DEFAULT_CLOSE_WINDOWS_ENABLE ) )
		facy.close_chat( data->user_id );

	delete data;
}
