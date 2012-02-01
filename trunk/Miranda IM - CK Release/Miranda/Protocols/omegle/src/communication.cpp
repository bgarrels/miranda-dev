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

http::response Omegle_client::flap( const int request_type, std::string* request_data )
{
	NETLIBHTTPREQUEST nlhr = {sizeof( NETLIBHTTPREQUEST )};
	nlhr.requestType = choose_method( request_type );
	std::string url = choose_request_url( request_type, request_data );
	nlhr.szUrl = (char*)url.c_str( );
	nlhr.flags = NLHRF_HTTP11 | NLHRF_NODUMP | NLHRF_GENERATEHOST;
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

int Omegle_client::choose_method( int request_type )
{
	switch ( request_type )
	{
	case OMEGLE_REQUEST_HOME:
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

std::string Omegle_client::choose_server( int request_type, std::string* data )
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
*/	default:
		std::string server = OMEGLE_SERVER_CHAT;
		utils::text::replace_first( &server, "%s", this->server_ );
		return server;
	}
}

std::string Omegle_client::choose_action( int request_type, std::string* data )
{
	switch ( request_type )
	{
	case OMEGLE_REQUEST_START:
		return "/start?rcs=1&spid=";

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

/*	case OMEGLE_REQUEST_HOME:
*/	default:
		return "/";
	}
}

std::string Omegle_client::choose_request_url( int request_type, std::string* data )
{
	std::string url = "";
	url.append( choose_server( request_type, data ) );
	url.append( choose_action( request_type, data ) );
	return url;
}

NETLIBHTTPHEADER* Omegle_client::get_request_headers( int request_type, int* headers_count )
{
	ScopedLock s( headers_lock_ );

	switch ( request_type )
	{
	case OMEGLE_REQUEST_START:
	case OMEGLE_REQUEST_STOP:
	case OMEGLE_REQUEST_SEND:
	case OMEGLE_REQUEST_EVENTS:
	case OMEGLE_REQUEST_TYPING_START:
	case OMEGLE_REQUEST_TYPING_STOP:
	case OMEGLE_REQUEST_RECAPTCHA:
		*headers_count = 5;
		break;

	case OMEGLE_REQUEST_HOME:
	default:
		*headers_count = 4;
		break;

/*		*headers_count = 3;
		break;*/
	}

	NETLIBHTTPHEADER* headers = ( NETLIBHTTPHEADER* )utils::mem::allocate( sizeof( NETLIBHTTPHEADER )*( *headers_count ) );

	refresh_headers( );

	switch ( request_type )
	{
	case OMEGLE_REQUEST_START:
	case OMEGLE_REQUEST_STOP:
	case OMEGLE_REQUEST_SEND:
	case OMEGLE_REQUEST_EVENTS:
	case OMEGLE_REQUEST_TYPING_START:
	case OMEGLE_REQUEST_TYPING_STOP:
	case OMEGLE_REQUEST_RECAPTCHA:
		set_header( &headers[4], "Content-Type" );

	case OMEGLE_REQUEST_HOME:
	default:
		set_header( &headers[3], "Cookie" );
		set_header( &headers[2], "User-Agent" );
		set_header( &headers[1], "Accept" );
		set_header( &headers[0], "Accept-Language" );
		break;
	}

	return headers;
}

void Omegle_client::set_header( NETLIBHTTPHEADER* header, char* name )
{
	header->szName  = name;
	header->szValue = (char*)this->headers[name].c_str();
}

void Omegle_client::refresh_headers( )
{
	ScopedLock s( headers_lock_ );

	if ( headers.size() < 5 )
	{
		this->headers["Accept"] = "*/*";
		this->headers["Accept-Language"] = "en,en-US;q=0.9";
		this->headers["Content-Type"] = "application/x-www-form-urlencoded";
	}
	this->headers["User-Agent"] = g_strUserAgent;
	//this->headers["Cookie"] = load_cookies( );
}

/*std::string Omegle_client::load_cookies( )
{
	ScopedLock s( cookies_lock_ );

	std::string cookieString = "isfbe=false;";

	if ( !cookies.empty( ) )
		for ( std::map< std::string, std::string >::iterator iter = cookies.begin(); iter != cookies.end(); ++iter )
		{
			cookieString.append( iter->first );
			cookieString.append( 1, '=' );
			cookieString.append( iter->second );
			cookieString.append( 1, ';' );
		}
	
  return cookieString;
}*/

void Omegle_client::store_headers( http::response* resp, NETLIBHTTPHEADER* headers, int headersCount )
{
	ScopedLock h( headers_lock_ );
//	ScopedLock c( cookies_lock_ );

	for ( int i = 0; i < headersCount; i++ )
	{
		std::string header_name = headers[i].szName; // TODO: Casting?
		std::string header_value = headers[i].szValue; // TODO: Casting?

/*		if ( header_name == "Set-Cookie" )
		{
			std::string cookie_name = header_value.substr( 0, header_value.find( "=" ) );
			std::string cookie_value = header_value.substr( header_value.find( "=" ) + 1, header_value.find( ";" ) - header_value.find( "=" ) - 1 );
			if ( cookie_value == "deleted" )
			{
				parent->Log("      Deleted cookie '%s'", cookie_name.c_str());
				cookies.erase( cookie_name );
			} else {
				parent->Log("      New cookie '%s': %s", cookie_name.c_str(), cookie_value.c_str());
				cookies[cookie_name] = cookie_value;
			}
		}
		else*/
		{ // RM TODO: uncomment
			//parent->Log("----- Got header '%s': %s", header_name.c_str(), header_value.c_str() );
			resp->headers[header_name] = header_value;
		}
	}
}

/*void Omegle_client::clear_cookies( )
{
	ScopedLock s( cookies_lock_ );

	if ( !cookies.empty( ) )
		cookies.clear( );
}*/

//////////////////////////////////////////////////////////////////////////////

bool Omegle_client::start()
{
	handle_entry( "start" );

	std::string data = "id=";
	data += this->chat_id_;

	// Send validation
	http::response resp = flap( OMEGLE_REQUEST_START, &data );

	// Process result data
	validate_response(&resp);

	switch ( resp.code )
	{
	case HTTP_CODE_FAKE_DISCONNECTED:
	{
		// If is is only timeout error, try login once more
		if ( handle_error( "login" ) )
			return start();
		else
			return false;
	}

	case HTTP_CODE_OK:
	{ 
		if (!resp.data.empty()) {
			this->chat_id_ = resp.data.substr(1,resp.data.length()-2);
			this->connected_ = true;

			return handle_success( "login" );
		} else {
			return handle_error( "login", FORCE_DISCONNECT );
		}
	}

	default:
		return handle_error( "login", FORCE_DISCONNECT );
	}
}

bool Omegle_client::stop( )
{
	if ( parent->isOffline() )
		return true;

	handle_entry( "stop" );

	std::string data = "id=";
	data += this->chat_id_;

	http::response resp = flap( OMEGLE_REQUEST_STOP, &data );

	if (hConnection)
		Netlib_CloseHandle(hConnection);
	hConnection = NULL;

	if (hEventsConnection)
		Netlib_CloseHandle(hEventsConnection);
	hEventsConnection = NULL;

	if (resp.data == "win") {
		return handle_success( "logout" );
	} else {
		return handle_error( "logout" );
	}

/*	switch ( resp.code )
	{
	case HTTP_CODE_OK:
	case HTTP_CODE_FOUND:

	default:
		
	}*/
}

bool Omegle_client::home( )
{
	handle_entry( "home" );

	http::response resp = flap( OMEGLE_REQUEST_HOME );

	// Process result data
	validate_response(&resp);

	switch ( resp.code )
	{
	case HTTP_CODE_OK:
	{
		std::string::size_type pos = 0;
		if ( (pos = resp.data.find( "<frame src=\"http://" )) != std::string::npos )
		{
			// Get chat server address
			pos += 19;
			this->server_ = resp.data.substr(pos, resp.data.find(".omegle.com", pos) - pos);
			parent->Log("      Got chat server name: %s", this->server_.c_str());

			return handle_success( "home" );
		}
		else
		{
			/*client_notify(TranslateT("Something happened to Omegle. Maybe there was some major update so you should wait for an update."));*/
			return handle_error( "home", FORCE_DISCONNECT );
		}
	}
	case HTTP_CODE_FOUND:
		return this->home();
	
	default:
		return handle_error( "home", FORCE_DISCONNECT );
	}
}

bool Omegle_client::events( )
{
	handle_entry( "events" );

	std::string data = "id=";
	data += this->chat_id_;

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
		}
		else if ( resp.data == "fail" ) {
			// Something went wrong
			return handle_error( "events" );
		}
		
		if ( resp.data.find( "[\"waiting\"]" ) != std::string::npos ) {
			// We are just waiting for new Stranger
		}
		if ( resp.data.find( "[\"connected\"]" ) != std::string::npos ) {
			// Stranger connected
			parent->AddChatContact(Translate("Stranger"));
		}
		if ( resp.data.find( "[\"typing\"]" ) != std::string::npos ) {
			// Stranger is typing
			// TODO: not supported by Group chats right now
		}
		if ( resp.data.find( "[\"stoppedtyping\"]" ) != std::string::npos ) {
			// Stranger stopped typing
			// TODO: not supported by Group chats right now
		}
		
		std::string::size_type pos = 0;
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

	std::string data = "msg=";
	data += utils::url::encode( message_text );
	data += "&id=";
	data += this->chat_id_;

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

	std::string data = "id=";
	data += this->chat_id_;

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

	std::string data = "id=";
	data += this->chat_id_;

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


//////////////////////////////////////////////////////////////////////////////

/*bool Omegle_client::save_url(const std::string &url,const std::string &filename)
{
	NETLIBHTTPREQUEST req = {sizeof(req)};
	NETLIBHTTPREQUEST *resp;
	req.requestType = REQUEST_GET;
	req.szUrl = const_cast<char*>(url.c_str());

	resp = reinterpret_cast<NETLIBHTTPREQUEST*>(CallService( MS_NETLIB_HTTPTRANSACTION,
		reinterpret_cast<WPARAM>(this->parent->m_hNetlibUser), reinterpret_cast<LPARAM>(&req) ));

	if ( resp )
	{
		parent->Log( "@@@@@ Saving avatar URL %s to path %s", url.c_str(), filename.c_str() );

		// Create folder if necessary
		std::string dir = filename.substr(0,filename.rfind('\\'));
		if(_access(dir.c_str(),0))
			CallService(MS_UTILS_CREATEDIRTREE, 0, (LPARAM)dir.c_str());

		// Write to file
		FILE *f = fopen(filename.c_str(),"wb");
		fwrite(resp->pData,1,resp->dataLength,f);
		fclose(f);

		CallService(MS_NETLIB_FREEHTTPREQUESTSTRUCT,0,(LPARAM)resp);
		return true;
	} else {
		return false;
	}
}*/
