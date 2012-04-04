/*
Omegle plugin for Miranda Instant Messenger
_____________________________________________

Copyright © 2011-2012 Robert Pösel

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

http::response Omegle_client::flap( const int request_type, std::string* request_data, std::string* get_data )
{
	NETLIBHTTPREQUEST nlhr = {sizeof( NETLIBHTTPREQUEST )};
	nlhr.requestType = choose_method( request_type );
	std::string url = choose_request_url( request_type, request_data, get_data );
	nlhr.szUrl = (char*)url.c_str( );
	nlhr.flags = NLHRF_HTTP11 | /*NLHRF_NODUMP |*/ NLHRF_GENERATEHOST;
	nlhr.headers = get_request_headers( request_type, &nlhr.headersCount );
	nlhr.timeout = 1000 * (( request_type == OMEGLE_REQUEST_EVENTS ) ? 60 : 15);

	if ( request_data != NULL )
	{
		nlhr.pData = (char*)(*request_data).c_str();
		nlhr.dataLength = (int)request_data->length( );
	}

	parent->Log("@@@@@ Sending request to '%s'", nlhr.szUrl);

	switch ( request_type )
	{
	case OMEGLE_REQUEST_HOME:
		nlhr.nlc = NULL;
		break;

	case OMEGLE_REQUEST_EVENTS:
		nlhr.nlc = hEventsConnection;
		nlhr.flags |= NLHRF_PERSISTENT;
		break;

	default:
		WaitForSingleObject(connection_lock_, INFINITE);
		nlhr.nlc = hConnection;
		nlhr.flags |= NLHRF_PERSISTENT;
		break;
	}

	NETLIBHTTPREQUEST* pnlhr = ( NETLIBHTTPREQUEST* )CallService( MS_NETLIB_HTTPTRANSACTION, (WPARAM)handle_, (LPARAM)&nlhr );

	http::response resp;

	switch ( request_type )
	{
	case OMEGLE_REQUEST_HOME:
		break;

	case OMEGLE_REQUEST_EVENTS:
		hEventsConnection = pnlhr ? pnlhr->nlc : NULL;
		break;

	default:
		ReleaseMutex(connection_lock_);
		hConnection = pnlhr ? pnlhr->nlc : NULL;
		break;
	}

	if ( pnlhr != NULL )
	{
		parent->Log("@@@@@ Got response with code %d", pnlhr->resultCode);
		store_headers( &resp, pnlhr->headers, pnlhr->headersCount );
		resp.code = pnlhr->resultCode;
		resp.data = pnlhr->pData ? pnlhr->pData : "";

		parent->Log("&&&&& Got response: %s", resp.data.c_str());

		CallService(MS_NETLIB_FREEHTTPREQUESTSTRUCT, 0, (LPARAM)pnlhr);
	} else {
		parent->Log("!!!!! No response from server (time-out)");
		resp.code = HTTP_CODE_FAKE_DISCONNECTED;
		// Better to have something set explicitely as this value
	    // is compaired in all communication requests
	}

	return resp;
}

void Omegle_client::validate_response( http::response* resp )
{
	if ( resp->code == HTTP_CODE_FAKE_DISCONNECTED )
	{
		parent->Log(" ! !  Request has timed out, connection or server error");
		return;
	}

	if (resp->data == "fail") {
//
	}
}

bool Omegle_client::handle_entry( std::string method )
{
	parent->Log("   >> Entering %s()", method.c_str());
	return true;
}

bool Omegle_client::handle_success( std::string method )
{
	parent->Log("   << Quitting %s()", method.c_str());
	reset_error();
	return true;
}

bool Omegle_client::handle_error( std::string method, bool force_disconnect )
{
	bool result;
	increment_error();
	parent->Log("!!!!! %s(): Something with Omegle went wrong", method.c_str());

	if ( force_disconnect )
		result = false;
	else if ( error_count_ <= (UINT)DBGetContactSettingByte(NULL,parent->m_szModuleName,OMEGLE_KEY_TIMEOUTS_LIMIT,OMEGLE_TIMEOUTS_LIMIT))
		result = true;
	else
		result = false;

	if ( result == false )
	{
		reset_error();
		parent->SetStatus(ID_STATUS_OFFLINE);
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////////

std::string Omegle_client::get_server( )
{
	BYTE server = DBGetContactSettingByte(NULL, parent->m_szModuleName, OMEGLE_KEY_SERVER, 0);
	if (server < 0 || server >= SIZEOF(servers))
		server = 0;	

	if (server == 0) {
		srand(::time(NULL));
		server = (rand() % (SIZEOF(servers)-1))+1;
	}

	return servers[server];
}

int Omegle_client::choose_method( int request_type )
{
	switch ( request_type )
	{
	case OMEGLE_REQUEST_HOME:
	case OMEGLE_REQUEST_COUNT:
		return REQUEST_GET;
	
/*	case OMEGLE_REQUEST_START:
	case OMEGLE_REQUEST_STOP:
	case OMEGLE_REQUEST_SEND:
	case OMEGLE_REQUEST_EVENTS:
	case OMEGLE_REQUEST_TYPING_START:
	case OMEGLE_REQUEST_TYPING_STOP:
	case OMEGLE_REQUEST_RECAPTCHA:
*/	default:
		return REQUEST_POST;
	}
}

std::string Omegle_client::choose_server( int request_type, std::string* data, std::string* get_data )
{
	switch ( request_type )
	{
	case OMEGLE_REQUEST_HOME:
		return OMEGLE_SERVER_REGULAR;
	
/*	case OMEGLE_REQUEST_START:
	case OMEGLE_REQUEST_STOP:
	case OMEGLE_REQUEST_SEND:
	case OMEGLE_REQUEST_EVENTS:
	case OMEGLE_REQUEST_TYPING_START:
	case OMEGLE_REQUEST_TYPING_STOP:
	case OMEGLE_REQUEST_RECAPTCHA:
	case OMEGLE_REQUEST_COUNT:
*/	default:
		std::string server = OMEGLE_SERVER_CHAT;
		utils::text::replace_first( &server, "%s", this->server_ );
		return server;
	}
}

std::string Omegle_client::choose_action( int request_type, std::string* data, std::string* get_data )
{
	switch ( request_type )
	{
	case OMEGLE_REQUEST_START:
		{
			std::string action = "/start?rcs=1&spid=";
			if (get_data != NULL)
				action += (*get_data);

			return action;
		}

	case OMEGLE_REQUEST_STOP:
		return "/disconnect";

	case OMEGLE_REQUEST_SEND:
		return "/send";

	case OMEGLE_REQUEST_EVENTS:
		return "/events";

	case OMEGLE_REQUEST_TYPING_START:
		return "/typing";

	case OMEGLE_REQUEST_TYPING_STOP:
		return "/stoppedtyping";

	case OMEGLE_REQUEST_RECAPTCHA:
		return "/recaptcha";

	case OMEGLE_REQUEST_COUNT:
		return "/count";

	// "/stoplookingforcommonlikes"

/*	case OMEGLE_REQUEST_HOME:
*/	default:
		return "/";
	}
}

std::string Omegle_client::choose_request_url( int request_type, std::string* data, std::string* get_data )
{
	std::string url = "";
	url.append( choose_server( request_type, data, get_data ) );
	url.append( choose_action( request_type, data, get_data ) );
	return url;
}

NETLIBHTTPHEADER* Omegle_client::get_request_headers( int request_type, int* headers_count )
{
	switch ( request_type )
	{
	case OMEGLE_REQUEST_START:
	case OMEGLE_REQUEST_STOP:
	case OMEGLE_REQUEST_SEND:
	case OMEGLE_REQUEST_EVENTS:
	case OMEGLE_REQUEST_TYPING_START:
	case OMEGLE_REQUEST_TYPING_STOP:
	case OMEGLE_REQUEST_RECAPTCHA:
		*headers_count = 4;
		break;

	case OMEGLE_REQUEST_HOME:
	case OMEGLE_REQUEST_COUNT:
	default:
		*headers_count = 3;
		break;
	}

	NETLIBHTTPHEADER* headers = ( NETLIBHTTPHEADER* )utils::mem::allocate( sizeof( NETLIBHTTPHEADER )*( *headers_count ) );

	switch ( request_type )
	{
	case OMEGLE_REQUEST_START:
	case OMEGLE_REQUEST_STOP:
	case OMEGLE_REQUEST_SEND:
	case OMEGLE_REQUEST_EVENTS:
	case OMEGLE_REQUEST_TYPING_START:
	case OMEGLE_REQUEST_TYPING_STOP:
	case OMEGLE_REQUEST_RECAPTCHA:
		headers[3].szName = "Content-Type";
		headers[3].szValue = "application/x-www-form-urlencoded; charset=utf-8";

	case OMEGLE_REQUEST_HOME:
	case OMEGLE_REQUEST_COUNT:
	default:
		headers[2].szName = "User-Agent";
		headers[2].szValue = (char *)g_strUserAgent.c_str( );
		headers[1].szName = "Accept";
		headers[1].szValue = "*/*";
		headers[0].szName = "Accept-Language";
		headers[0].szValue = "en,en-US;q=0.9";
		break;
	}

	return headers;
}

void Omegle_client::store_headers( http::response* resp, NETLIBHTTPHEADER* headers, int headersCount )
{
	for ( int i = 0; i < headersCount; i++ )
	{
		std::string header_name = headers[i].szName; // TODO: Casting?
		std::string header_value = headers[i].szValue; // TODO: Casting?

		// TODO RM: (un)comment
		//parent->Log("----- Got header '%s': %s", header_name.c_str(), header_value.c_str() );
		resp->headers[header_name] = header_value;
	}
}

//////////////////////////////////////////////////////////////////////////////

bool Omegle_client::start()
{
	handle_entry( "start" );

	this->server_ = get_server();
	//parent->Log("Chosing server %s", this->server_.c_str());
	//std::string log = Translate("Chosing server: ") + this->server_;
	//parent->UpdateChat(NULL, log.c_str());

	std::string data;

	/*if (1) {
		data = "&wantsspy=1";

		std::string question = "How are you?";
		data = "&ask=" + utils::url::encode(question);
		data += "&cansavequestion=" + DBGetContactSettingByte(NULL, parent->m_szModuleName, OMEGLE_KEY_REUSE_QUESTION, 0) ? "1" : "0";
	}
	else */if ( DBGetContactSettingByte(NULL, parent->m_szModuleName, OMEGLE_KEY_MEET_COMMON, 0) )
	{
		DBVARIANT dbv;
		if (!DBGetContactSettingUTF8String(NULL, parent->m_szModuleName, OMEGLE_KEY_INTERESTS, &dbv))
		{
			std::string topics = dbv.pszVal;
			std::string topic;
		
			DBFreeVariant(&dbv);

			std::string::size_type pos = 0;
			std::string::size_type pos2 = 0;
			while ((pos2 = topics.find(",", pos)) != std::string::npos) {
				topic = topics.substr(pos, pos2 - pos);
				topic = utils::text::trim(topic);

				if (!topic.empty()) {
					if (pos > 0)
						data += ",";

					data += "\"" + topic + "\"";
				}

				pos = pos2 + 1;
			}
		
			topic = topics.substr(pos);
			topic = utils::text::trim(topic);
			if (!topic.empty()) {			
				if (pos > 0)
					data += ",";
				data += "\"" + topic + "\"";			
			}

			parent->Log("TOPICS: %s", data.c_str());

			if (!data.empty()) {
				data = "[" + data + "]";
				data = "&topics=" + utils::url::encode(data);
			}
				
			// Interests feature supports only Quarks server
			this->server_ = "quarks";
		}
	}

	std::string count = get_page( OMEGLE_REQUEST_COUNT );
	if (!count.empty()) {
		char str[255];
		mir_snprintf(str, sizeof(str), Translate("Connected to server %s. There are %s users online now."), server_.c_str(), count.c_str());
		parent->UpdateChat(NULL, str);
	}

	// Send validation
	http::response resp = flap( OMEGLE_REQUEST_START, NULL, &data );

	// Process result data
	validate_response(&resp);

	switch ( resp.code )
	{
	case HTTP_CODE_FAKE_DISCONNECTED:
	{
		// If is is only timeout error, try login once more
		if ( handle_error( "start" ) )
			return start();
		else
			return false;
	}

	case HTTP_CODE_OK:
	{ 
		if (!resp.data.empty()) {
			this->chat_id_ = resp.data.substr(1,resp.data.length()-2);
			this->connected_ = true;

			return handle_success( "start" );
		} else {
			return handle_error( "start", FORCE_DISCONNECT );
		}
	}

	default:
		return handle_error( "start", FORCE_DISCONNECT );
	}
}

bool Omegle_client::stop( )
{
	if ( parent->isOffline() )
		return true;

	handle_entry( "stop" );

	std::string data = "id=" + this->chat_id_;

	http::response resp = flap( OMEGLE_REQUEST_STOP, &data );

	if (hConnection)
		Netlib_CloseHandle(hConnection);
	hConnection = NULL;

	if (hEventsConnection)
		Netlib_CloseHandle(hEventsConnection);
	hEventsConnection = NULL;

	if (resp.data == "win") {
		return handle_success( "stop" );
	} else {
		return handle_error( "stop" );
	}

/*	switch ( resp.code )
	{
	case HTTP_CODE_OK:
	case HTTP_CODE_FOUND:

	default:
		
	}*/
}

bool Omegle_client::events( )
{
	handle_entry( "events" );

	std::string data = "id=" + this->chat_id_;

	// Get update
	http::response resp = flap( OMEGLE_REQUEST_EVENTS, &data );

	// Process result data
	validate_response(&resp);	

	// Return
	switch ( resp.code )
	{
	case HTTP_CODE_OK:
	{
		if ( resp.data == "null" ) {
			// Everything is OK, no new message received
			return handle_success( "events" );
		} else if ( resp.data == "fail" ) {
			// Something went wrong
			return handle_error( "events" );
		}
		
		std::string::size_type pos = 0;
		bool newStranger = false;
		bool waiting = false;
		
		if ( resp.data.find( "[\"waiting\"]" ) != std::string::npos ) {
			// We are just waiting for new Stranger
			waiting = true;
		}

		if ( (pos = resp.data.find( "[\"count\"," )) != std::string::npos ) {
			// We got info about count of connected people there
			pos += 9;

			std::string count = utils::text::trim( resp.data.substr(pos, resp.data.find("]", pos) - pos) );

			char str[255];
			mir_snprintf(str, sizeof(str), Translate("On whole Omegle are %s strangers online now."), count.c_str());
			parent->UpdateChat(NULL, str);
		}

		if ( resp.data.find( "[\"connected\"]" ) != std::string::npos ) {
			// Stranger connected
			parent->AddChatContact(Translate("Stranger"));
			newStranger = true;
			waiting = false;
		}

		if ( (pos = resp.data.find( "[\"commonLikes\",", pos )) != std::string::npos ) {
			pos += 18;
			std::string like = resp.data.substr(pos, resp.data.find("\"]", pos) - pos);
			utils::text::replace_all(&like, "\", \"", ", ");

			parent->Log("Got common likes: '%s'", like.c_str());

			like = Translate("You and the Stranger both like: ") + like;
			parent->UpdateChat(NULL, like.c_str());
		}

		if ( (pos = resp.data.find( "[\"question\"," )) != std::string::npos ) {
			pos += 14;

			std::string question = utils::text::trim(
				utils::text::special_expressions_decode(
					utils::text::slashu_to_utf8(
						resp.data.substr(pos, resp.data.find("\"]", pos) - pos)	) ) );
			
			question = Translate("Question to discuss: ") + question;

			parent->UpdateChat(NULL, question.c_str());
		}

		if ( resp.data.find( "[\"typing\"]" ) != std::string::npos ) {
			// Stranger is typing
			// TODO: not supported by Group chats right now
		}

		if ( resp.data.find( "[\"stoppedTyping\"]" ) != std::string::npos ) {
			// Stranger stopped typing
			// TODO: not supported by Group chats right now
		}


		if ( (pos = resp.data.find( "[\"spyTyping\",", pos )) != std::string::npos ) {
			pos += 15;

			std::string stranger = utils::text::trim(
				utils::text::special_expressions_decode(
					utils::text::slashu_to_utf8(
						resp.data.substr(pos, resp.data.find("\"]", pos) - pos)	) ) );

			// parent->UpdateChat(NULL, question.c_str());
		}
		
		if ( (pos = resp.data.find( "[\"spyStoppedTyping\",", pos )) != std::string::npos ) {
			pos += 22;

			std::string stranger = utils::text::trim(
				utils::text::special_expressions_decode(
					utils::text::slashu_to_utf8(
						resp.data.substr(pos, resp.data.find("\"]", pos) - pos)	) ) );

			// parent->UpdateChat(NULL, question.c_str());
		}

		if ( (pos = resp.data.find( "[\"spyDisconnected\",", pos )) != std::string::npos ) {
			pos += 21;

			std::string stranger = utils::text::trim(
				utils::text::special_expressions_decode(
					utils::text::slashu_to_utf8(
						resp.data.substr(pos, resp.data.find("\"]", pos) - pos)	) ) );

			// Stranger disconnected
			if (DBGetContactSettingByte(NULL, parent->m_szModuleName, OMEGLE_KEY_DONT_STOP, 0))
				parent->NewChat();
			else			
				parent->StopChat(false);
		}

		// TODO: "spyMessage", ["name", "message"]
		
		pos = 0;
		while ( (pos = resp.data.find( "[\"gotMessage\",", pos )) != std::string::npos ) {
			pos += 16;

			std::string message = utils::text::trim(
				utils::text::special_expressions_decode(
					utils::text::slashu_to_utf8(
						resp.data.substr(pos, resp.data.find("\"]", pos) - pos)	) ) );
			
			parent->UpdateChat(Translate("Stranger"), message.c_str());
		}

		if ( resp.data.find( "[\"strangerDisconnected\"]" ) != std::string::npos ) {
			// Stranger disconnected
			if (DBGetContactSettingByte(NULL, parent->m_szModuleName, OMEGLE_KEY_DONT_STOP, 0))
				parent->NewChat();
			else			
				parent->StopChat(false);
		}

		if ( resp.data.find( "[\"recaptchaRequired\"" ) != std::string::npos ) {
			// Nothing to do with recaptcha
			parent->UpdateChat(NULL, Translate("Recaptcha is required.\nOpen Omegle chat in webbrowser, solve Recaptcha and try again."));
			parent->StopChat(false);
		}

		if ( resp.data.find( "[\"recaptchaRejected\"]" ) != std::string::npos ) {
			// Nothing to do with recaptcha
			parent->StopChat(false);
		}

		pos = 0;
		if ( (pos = resp.data.find( "[\"error\",", pos )) != std::string::npos ) {
			pos += 11;

			std::string error = utils::text::trim(
				utils::text::special_expressions_decode(
					utils::text::slashu_to_utf8(
						resp.data.substr(pos, resp.data.find("\"]", pos) - pos)	) ) );

			error = Translate("Error: ") + error; 
			parent->UpdateChat(NULL, error.c_str());
		}
				
		if (newStranger) {
			// We got new stranger in this event, lets say him "Hi message" if enabled			
			if ( DBGetContactSettingByte( NULL, parent->m_szModuleName, OMEGLE_KEY_HI_ENABLED, 0 ) ) {
				DBVARIANT dbv;
				if ( !DBGetContactSettingUTF8String( NULL, parent->m_szModuleName, OMEGLE_KEY_HI, &dbv ) ) {
					std::string *message = new std::string(dbv.pszVal);
					DBFreeVariant(&dbv);
	
					parent->Log("**Chat - saying Hi! message");
					ForkThread(&OmegleProto::SendMsgWorker, parent, (void*)message);
				} else {
					parent->Log("**Chat - Hi message is enabled but not used");
				}
			}
		}

		if (waiting) {
			// If we are only waiting in this event...
			parent->UpdateChat(NULL, Translate("We are still waiting..."));
		}

		return handle_success( "events" );
	}

	case HTTP_CODE_FAKE_DISCONNECTED:
		// timeout
		return handle_success( "events" );

	case HTTP_CODE_FAKE_ERROR:
	default:
		return handle_error( "events" );
	}
}

bool Omegle_client::send_message( std::string message_text )
{
	handle_entry( "send_message" );

	std::string data = "msg=" + utils::url::encode( message_text );
	data += "&id=" + this->chat_id_;

	http::response resp = flap( OMEGLE_REQUEST_SEND, &data );

	validate_response(&resp);

	switch ( resp.code )
	{
	case HTTP_CODE_OK:
		if (resp.data == "win") {
			return handle_success( "send_message" );
		}

	case HTTP_CODE_FAKE_ERROR:
	case HTTP_CODE_FAKE_DISCONNECTED:
	default:
		return handle_error( "send_message" );
	}
}

bool Omegle_client::typing_start()
{
	handle_entry( "typing_start" );

	std::string data = "id=" + this->chat_id_;

	http::response resp = flap( OMEGLE_REQUEST_TYPING_START, &data );

	validate_response(&resp);

	switch ( resp.code )
	{
	case HTTP_CODE_OK:
		if (resp.data == "win") {
			return handle_success( "typing_start" );
		}

	case HTTP_CODE_FAKE_ERROR:
	case HTTP_CODE_FAKE_DISCONNECTED:
	default:
		return handle_error( "typing_start" );
	}
}

bool Omegle_client::typing_stop()
{
	handle_entry( "typing_stop" );

	std::string data = "id=" + this->chat_id_;

	http::response resp = flap( OMEGLE_REQUEST_TYPING_STOP, &data );

	validate_response(&resp);

	switch ( resp.code )
	{
	case HTTP_CODE_OK:
		if (resp.data == "win") {
			return handle_success( "typing_stop" );
		}

	case HTTP_CODE_FAKE_ERROR:
	case HTTP_CODE_FAKE_DISCONNECTED:
	default:
		return handle_error( "typing_stop" );
	}
}

bool Omegle_client::recaptcha()
{
	// TODO: Implement!

	handle_entry( "recaptcha" );

	// data:{id:this.clientID,challenge:b,response:a}}
	//std::string data = "?id=...&challenge= ..., &response= ...";

	http::response resp = flap( OMEGLE_REQUEST_RECAPTCHA );

	validate_response(&resp);

	switch ( resp.code )
	{
	case HTTP_CODE_OK:
/*		if (resp.data == "win") {
			return handle_success( "typing_start" );
		}*/

	case HTTP_CODE_FAKE_ERROR:
	case HTTP_CODE_FAKE_DISCONNECTED:
	default:
		return handle_error( "typing_start" );
	}
}

std::string Omegle_client::get_page( const int request_type )
{
	handle_entry( "get_page" );

	http::response resp = flap( OMEGLE_REQUEST_COUNT );

	switch ( resp.code )
	{
	case HTTP_CODE_OK:
		handle_success( "get_page" );
		return resp.data;
		break;

	case HTTP_CODE_FAKE_ERROR:
	case HTTP_CODE_FAKE_DISCONNECTED:
	default:
		handle_error( "get_page" );
		return NULL;
	}	
}
