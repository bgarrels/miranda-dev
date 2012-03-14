/*

Facebook plugin for Miranda Instant Messenger
_____________________________________________

Copyright � 2009-11 Michal Zelinka, 2011-12 Robert P�sel

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

void facebook_client::client_notify( TCHAR* message )
{
	parent->NotifyEvent( parent->m_tszUserName, message, NULL, FACEBOOK_EVENT_CLIENT );
}

http::response facebook_client::flap( const int request_type, std::string* request_data )
{
	NETLIBHTTPREQUEST nlhr = {sizeof( NETLIBHTTPREQUEST )};
	nlhr.requestType = choose_method( request_type );
	std::string url = choose_request_url( request_type, request_data );
	nlhr.szUrl = (char*)url.c_str( );
	nlhr.flags = NLHRF_HTTP11 | /*NLHRF_NODUMP |*/ choose_security_level( request_type );
	nlhr.headers = get_request_headers( request_type, &nlhr.headersCount );
	
	switch (request_type)
	{
	case FACEBOOK_REQUEST_MESSAGES_RECEIVE:
		nlhr.timeout = 1000 * 65; break;
	case FACEBOOK_REQUEST_MESSAGE_SEND:
		nlhr.timeout = 1000 * 10; break;
	default:
		nlhr.timeout = 1000 * 15; break;
	}

	if ( request_data != NULL )
	{
		nlhr.pData = (char*)(*request_data).c_str();
		nlhr.dataLength = (int)request_data->length( );
	}

	parent->Log("@@@@@ Sending request to '%s'", nlhr.szUrl);

	switch ( request_type )
	{
	case FACEBOOK_REQUEST_LOGIN:
	case FACEBOOK_REQUEST_SETUP_MACHINE:
		nlhr.nlc = NULL;
		break;

	case FACEBOOK_REQUEST_MESSAGES_RECEIVE:
		nlhr.nlc = hMsgCon;
		nlhr.flags |= NLHRF_PERSISTENT;
		break;

	default:
		WaitForSingleObject(fcb_conn_lock_, INFINITE);
		nlhr.nlc = hFcbCon;
		nlhr.flags |= NLHRF_PERSISTENT;
		break;
	}

	NETLIBHTTPREQUEST* pnlhr = ( NETLIBHTTPREQUEST* )CallService( MS_NETLIB_HTTPTRANSACTION, (WPARAM)handle_, (LPARAM)&nlhr );

	utils::mem::detract(nlhr.headers[3].szValue);
	utils::mem::detract(nlhr.headers);

	http::response resp;

	switch ( request_type )
	{
	case FACEBOOK_REQUEST_LOGIN:
	case FACEBOOK_REQUEST_SETUP_MACHINE:
		break;

	case FACEBOOK_REQUEST_MESSAGES_RECEIVE:
		hMsgCon = pnlhr ? pnlhr->nlc : NULL;
		break;

	default:
		ReleaseMutex(fcb_conn_lock_);
		hFcbCon = pnlhr ? pnlhr->nlc : NULL;
		break;
	}

	if ( pnlhr != NULL )
	{
		parent->Log("@@@@@ Got response with code %d", pnlhr->resultCode);
		store_headers( &resp, pnlhr->headers, pnlhr->headersCount );
		resp.code = pnlhr->resultCode;
		resp.data = pnlhr->pData ? pnlhr->pData : "";

		CallService(MS_NETLIB_FREEHTTPREQUESTSTRUCT, 0, (LPARAM)pnlhr);
	} else {
		parent->Log("!!!!! No response from server (time-out)");
		resp.code = HTTP_CODE_FAKE_DISCONNECTED;
		// Better to have something set explicitely as this value
	    // is compaired in all communication requests
	}

	return resp;
}

bool facebook_client::validate_response( http::response* resp )
{
	if ( resp->code == HTTP_CODE_FAKE_DISCONNECTED )
	{
		parent->Log(" ! !  Request has timed out, connection or server error");
		return false;
	}

/*	std::string cookie = utils::text::source_get_value(&resp->data, 2, "setCookie(\\\"", ");");	
	if (!cookie.empty()) {
		std::string cookie_name = utils::text::source_get_value(&cookie, 1, "\\\"");
		std::string cookie_value = utils::text::source_get_value(&cookie, 3, "\\\"", "\\\"", "\\\"");

		parent->Log("      New cookie from response '%s': %s", cookie_name.c_str(), cookie_value.c_str());
		this->cookies[cookie_name] = cookie_value;
	}
*/
	std::string::size_type pos = resp->data.find( "\"error\":" );
	if ( pos != std::string::npos )
    try
  	{
		pos += 8;
	    int error_num = atoi( resp->data.substr( pos, resp->data.find( ",", pos ) - pos ).c_str() );
	    if ( error_num != 0 )
	    {
			std::string error = "";
			pos = resp->data.find( "\"errorDescription\":\"", pos );
			if (pos != std::string::npos ) {
				pos += 20;
				error = resp->data.substr( pos, resp->data.find( "\"", pos ) - pos );
				error = utils::text::trim(
					utils::text::special_expressions_decode( 
						utils::text::slashu_to_utf8( error ) ) );

			}

		    resp->error_number = error_num;
		    resp->error_text = error;
		    parent->Log(" ! !  Received Facebook error: %d -- %s", error_num, error.c_str());
			// client_notify( ... );
		    resp->code = HTTP_CODE_FAKE_ERROR;
		    return false;
	    }
    } catch (const std::exception &e) {
	    parent->Log(" @ @  validate_response: Exception: %s",e.what());
		return false;
    }

	return true;
}

bool facebook_client::handle_entry( std::string method )
{
	parent->Log("   >> Entering %s()", method.c_str());
	return true;
}

bool facebook_client::handle_success( std::string method )
{
	parent->Log("   << Quitting %s()", method.c_str());
	reset_error();
	return true;
}

bool facebook_client::handle_error( std::string method, bool force_disconnect )
{
	bool result;
	increment_error();
	parent->Log("!!!!! %s(): Something with Facebook went wrong", method.c_str());

	if ( force_disconnect )
		result = false;
	else if ( error_count_ <= (UINT)DBGetContactSettingByte(NULL,parent->m_szModuleName,FACEBOOK_KEY_TIMEOUTS_LIMIT,FACEBOOK_TIMEOUTS_LIMIT))
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

DWORD facebook_client::choose_security_level( int request_type )
{
	if ( DBGetContactSettingByte( NULL, parent->m_szProtoName, FACEBOOK_KEY_FORCE_HTTPS, DEFAULT_FORCE_HTTPS ) ) {				
		if ( request_type != FACEBOOK_REQUEST_MESSAGES_RECEIVE
			|| DBGetContactSettingByte( NULL, parent->m_szProtoName, FACEBOOK_KEY_FORCE_HTTPS_CHANNEL, DEFAULT_FORCE_HTTPS_CHANNEL ) )
			return NLHRF_SSL;
	}

	switch ( request_type )
	{
	case FACEBOOK_REQUEST_LOGIN:
	case FACEBOOK_REQUEST_SETUP_MACHINE:
		return NLHRF_SSL;

//	case FACEBOOK_REQUEST_LOGOUT:
//	case FACEBOOK_REQUEST_HOME:
//	case FACEBOOK_REQUEST_BUDDY_LIST:
//	case FACEBOOK_REQUEST_FACEPILES:
//	case FACEBOOK_REQUEST_LOAD_FRIENDS:
//  case FACEBOOK_REQUEST_DELETE_FRIEND:
//	case FACEBOOK_REQUEST_ADD_FRIEND:
//	case FACEBOOK_REQUEST_FEEDS:
//	case FACEBOOK_REQUEST_NOTIFICATIONS:
//	case FACEBOOK_REQUEST_RECONNECT:
//	case FACEBOOK_REQUEST_STATUS_SET:
//	case FACEBOOK_REQUEST_MESSAGE_SEND:
//	case FACEBOOK_REQUEST_MESSAGES_RECEIVE:
//	case FACEBOOK_REQUEST_VISIBILITY:
//	case FACEBOOK_REQUEST_TABS:
//	case FACEBOOK_REQUEST_ASYNC:
//	case FACEBOOK_REQUEST_ASYNC_GET:
//	case FACEBOOK_REQUEST_TYPING_SEND:
	default:
		return ( DWORD )0;
	}
}

int facebook_client::choose_method( int request_type )
{
	switch ( request_type )
	{
	case FACEBOOK_REQUEST_LOGIN:
	case FACEBOOK_REQUEST_SETUP_MACHINE:
	case FACEBOOK_REQUEST_BUDDY_LIST:
	case FACEBOOK_REQUEST_FACEPILES:
	case FACEBOOK_REQUEST_STATUS_SET:
	case FACEBOOK_REQUEST_MESSAGE_SEND:
	case FACEBOOK_REQUEST_VISIBILITY:
	case FACEBOOK_REQUEST_TABS:
	case FACEBOOK_REQUEST_ASYNC:
	case FACEBOOK_REQUEST_TYPING_SEND:
	case FACEBOOK_REQUEST_LOGOUT:
	case FACEBOOK_REQUEST_DELETE_FRIEND:
	case FACEBOOK_REQUEST_ADD_FRIEND:
		return REQUEST_POST;

//	case FACEBOOK_REQUEST_HOME:
//	case FACEBOOK_REQUEST_MESSAGES_RECEIVE:
//	case FACEBOOK_REQUEST_FEEDS:
//	case FACEBOOK_REQUEST_NOTIFICATIONS:
//	case FACEBOOK_REQUEST_RECONNECT:
//	case FACEBOOK_REQUEST_LOAD_FRIENDS:
//	case FACEBOOK_REQUEST_ASYNC_GET:
	default:
		return REQUEST_GET;
	}
}

std::string facebook_client::choose_proto( int request_type )
{
	if ( DBGetContactSettingByte( NULL, parent->m_szProtoName, FACEBOOK_KEY_FORCE_HTTPS, DEFAULT_FORCE_HTTPS ) ) {				
		if ( request_type != FACEBOOK_REQUEST_MESSAGES_RECEIVE
			|| DBGetContactSettingByte( NULL, parent->m_szProtoName, FACEBOOK_KEY_FORCE_HTTPS_CHANNEL, DEFAULT_FORCE_HTTPS_CHANNEL ) )
			return HTTP_PROTO_SECURE;
	}			

	switch ( request_type )
	{
//	case FACEBOOK_REQUEST_LOGOUT:
//	case FACEBOOK_REQUEST_HOME:
//	case FACEBOOK_REQUEST_FEEDS:
//	case FACEBOOK_REQUEST_NOTIFICATIONS:
//	case FACEBOOK_REQUEST_RECONNECT:
//	case FACEBOOK_REQUEST_BUDDY_LIST:
//	case FACEBOOK_REQUEST_FACEPILES:
//	case FACEBOOK_REQUEST_LOAD_FRIENDS:
//	case FACEBOOK_REQUEST_STATUS_SET:
//	case FACEBOOK_REQUEST_MESSAGE_SEND:
//	case FACEBOOK_REQUEST_MESSAGES_RECEIVE:
//	case FACEBOOK_REQUEST_VISIBILITY:
//	case FACEBOOK_REQUEST_TABS:
//	case FACEBOOK_REQUEST_ASYNC:
//	case FACEBOOK_REQUEST_ASYNC_GET:
//	case FACEBOOK_REQUEST_TYPING_SEND:
//  case FACEBOOK_REQUEST_DELETE_FRIEND:
//	case FACEBOOK_REQUEST_ADD_FRIEND:
	default:
		return HTTP_PROTO_REGULAR;

	case FACEBOOK_REQUEST_LOGIN:
	case FACEBOOK_REQUEST_SETUP_MACHINE:
		return HTTP_PROTO_SECURE;
	}
}

std::string facebook_client::choose_server( int request_type, std::string* data )
{
	switch ( request_type )
	{
	case FACEBOOK_REQUEST_LOGIN:
		return FACEBOOK_SERVER_LOGIN;

	case FACEBOOK_REQUEST_MESSAGES_RECEIVE:
	{
		std::string server = FACEBOOK_SERVER_CHAT;
		if (!this->chat_channel_jslogger_.empty())
			server = FACEBOOK_SERVER_CHAT2;

		utils::text::replace_first( &server, "%s", "0" );
		utils::text::replace_first( &server, "%s", this->chat_channel_host_ );
		return server;
	}

//	case FACEBOOK_REQUEST_LOGOUT:
//	case FACEBOOK_REQUEST_HOME:
//	case FACEBOOK_REQUEST_BUDDY_LIST:
//	case FACEBOOK_REQUEST_FACEPILES:
//	case FACEBOOK_REQUEST_LOAD_FRIENDS:
//	case FACEBOOK_REQUEST_FEEDS:
//	case FACEBOOK_REQUEST_NOTIFICATIONS:
//	case FACEBOOK_REQUEST_RECONNECT:
//	case FACEBOOK_REQUEST_STATUS_SET:
//	case FACEBOOK_REQUEST_MESSAGE_SEND:
//	case FACEBOOK_REQUEST_VISIBILITY:
//	case FACEBOOK_REQUEST_TABS:
//	case FACEBOOK_REQUEST_ASYNC:
//	case FACEBOOK_REQUEST_ASYNC_GET:
//	case FACEBOOK_REQUEST_TYPING_SEND:
//	case FACEBOOK_REQUEST_SETUP_MACHINE:
//  case FACEBOOK_REQUEST_DELETE_FRIEND:
//	case FACEBOOK_REQUEST_ADD_FRIEND:
	default:
		return FACEBOOK_SERVER_REGULAR;
	}
}

std::string facebook_client::choose_action( int request_type, std::string* data )
{
	switch ( request_type )
	{
	case FACEBOOK_REQUEST_LOGIN:
		return "/login.php?login_attempt=1";

	case FACEBOOK_REQUEST_SETUP_MACHINE:
		return "/checkpoint/";

	case FACEBOOK_REQUEST_LOGOUT:
		return "/logout.php";

	case FACEBOOK_REQUEST_HOME:
		return "/home.php?_fb_noscript=1";

	case FACEBOOK_REQUEST_BUDDY_LIST:
		return "/ajax/chat/buddy_list.php?__a=1";

	case FACEBOOK_REQUEST_FACEPILES:
		return "/ajax/groups/chat/update_facepiles.php?__a=1";

	case FACEBOOK_REQUEST_LOAD_FRIENDS:
	{
		std::string action = "/ajax/chat/user_info_all.php?__a=1&viewer=%s";
		utils::text::replace_first( &action, "%s", self_.user_id );
		return action;
	}

	case FACEBOOK_REQUEST_DELETE_FRIEND:
	{
		return "/ajax/profile/removefriend.php?__a=1";
	}

	case FACEBOOK_REQUEST_ADD_FRIEND:
	{
		return "/ajax/add_friend/action.php?__a=1";
	}

	case FACEBOOK_REQUEST_FEEDS:
	{
		std::string action = "/ajax/intent.php?filter=";
		action += get_newsfeed_type();
		action += "&request_type=1&__a=1&newest=%s&ignore_self=true";
		std::string newest = utils::conversion::to_string((void*)&this->last_feeds_update_, UTILS_CONV_TIME_T);
		utils::text::replace_first( &action, "%s", newest );
		return action;
	}

	case FACEBOOK_REQUEST_NOTIFICATIONS:
	{
		std::string action = "/ajax/notifications/get.php?__a=1&user=%s&time=0&version=2";
		utils::text::replace_first( &action, "%s", self_.user_id );
		return action;
	}
	
	case FACEBOOK_REQUEST_RECONNECT:
	{
		std::string action = "/ajax/presence/reconnect.php?__a=1&reason=%s&fb_dtsg=%s&post_form_id=%s&__user=%s";
		
		if (this->chat_reconnect_reason_.empty())
			this->chat_reconnect_reason_ = "0"; // 6?

		utils::text::replace_first( &action, "%s", this->chat_reconnect_reason_ );
		utils::text::replace_first( &action, "%s", this->dtsg_ );
		utils::text::replace_first( &action, "%s", this->post_form_id_ );
		utils::text::replace_first( &action, "%s", this->self_.user_id );
		return action;
	}

	case FACEBOOK_REQUEST_STATUS_SET:
		return "/ajax/updatestatus.php?__a=1";

	case FACEBOOK_REQUEST_MESSAGE_SEND:
		return "/ajax/chat/send.php?__a=1";

	case FACEBOOK_REQUEST_MESSAGES_RECEIVE:
	{
		std::string action = "/x/%s/0/true/p_%s=%s";
		if (!this->chat_channel_jslogger_.empty()) {
			action = "/pull?clientid=&channel=p_%s&seq=%s&cb=&state=active";
			//utils::text::replace_first( &action, "%s", dtsg_ );
		} else {
			utils::text::replace_first( &action, "%s", utils::time::unix_timestamp() );
		}

		utils::text::replace_first( &action, "%s", self_.user_id );
		utils::text::replace_first( &action, "%s", chat_sequence_num_.empty() ? "0" : chat_sequence_num_ );
		return action;
	}

	case FACEBOOK_REQUEST_VISIBILITY:
		return "/ajax/chat/visibility.php?__a=1";

	case FACEBOOK_REQUEST_TABS:
		return "/ajax/chat/tabs.php?__a=1";

	case FACEBOOK_REQUEST_ASYNC:
		return "/ajax/messaging/async.php?__a=1";

	case FACEBOOK_REQUEST_ASYNC_GET:
	{
		std::string action = "/ajax/messaging/async.php?__a=1&%s";
		utils::text::replace_first( &action, "%s", (*data) );
		return action;
	}

	case FACEBOOK_REQUEST_TYPING_SEND:
		return "/ajax/messaging/typ.php?__a=1";

	default:
		return "/?_fb_noscript=1";
	}
}

std::string facebook_client::choose_request_url( int request_type, std::string* data )
{
	std::string url = choose_proto( request_type );
	url.append( choose_server( request_type, data ) );
	url.append( choose_action( request_type, data ) );
	return url;
}

NETLIBHTTPHEADER* facebook_client::get_request_headers( int request_type, int* headers_count )
{
	switch ( request_type )
	{
	case FACEBOOK_REQUEST_LOGIN:
	case FACEBOOK_REQUEST_SETUP_MACHINE:
	case FACEBOOK_REQUEST_BUDDY_LIST:
	case FACEBOOK_REQUEST_FACEPILES:
	case FACEBOOK_REQUEST_LOAD_FRIENDS:
	case FACEBOOK_REQUEST_STATUS_SET:
	case FACEBOOK_REQUEST_MESSAGE_SEND:
	case FACEBOOK_REQUEST_VISIBILITY:
	case FACEBOOK_REQUEST_TABS:
	case FACEBOOK_REQUEST_ASYNC:
	case FACEBOOK_REQUEST_ASYNC_GET:
	case FACEBOOK_REQUEST_TYPING_SEND:
	case FACEBOOK_REQUEST_DELETE_FRIEND:
	case FACEBOOK_REQUEST_ADD_FRIEND:
		*headers_count = 5;
		break;

	case FACEBOOK_REQUEST_HOME:
	case FACEBOOK_REQUEST_FEEDS:
	case FACEBOOK_REQUEST_NOTIFICATIONS:
	case FACEBOOK_REQUEST_RECONNECT:
	case FACEBOOK_REQUEST_MESSAGES_RECEIVE:
	default:
		*headers_count = 4;
		break;
	}

	NETLIBHTTPHEADER* headers = ( NETLIBHTTPHEADER* )utils::mem::allocate( sizeof( NETLIBHTTPHEADER )*( *headers_count ) );

	switch ( request_type )
	{
	case FACEBOOK_REQUEST_LOGIN:
	case FACEBOOK_REQUEST_SETUP_MACHINE:
	case FACEBOOK_REQUEST_BUDDY_LIST:
	case FACEBOOK_REQUEST_FACEPILES:
	case FACEBOOK_REQUEST_LOAD_FRIENDS:
	case FACEBOOK_REQUEST_STATUS_SET:
	case FACEBOOK_REQUEST_MESSAGE_SEND:
	case FACEBOOK_REQUEST_VISIBILITY:
	case FACEBOOK_REQUEST_TABS:
	case FACEBOOK_REQUEST_ASYNC:
	case FACEBOOK_REQUEST_ASYNC_GET:
	case FACEBOOK_REQUEST_TYPING_SEND:
	case FACEBOOK_REQUEST_DELETE_FRIEND:
	case FACEBOOK_REQUEST_ADD_FRIEND:
		headers[4].szName = "Content-Type";
		headers[4].szValue = "application/x-www-form-urlencoded; charset=utf-8";

	case FACEBOOK_REQUEST_HOME:
	case FACEBOOK_REQUEST_RECONNECT:
	case FACEBOOK_REQUEST_MESSAGES_RECEIVE:
	default:
		headers[3].szName = "Cookie";
		headers[3].szValue = load_cookies( );
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

std::string facebook_client::get_newsfeed_type( )
{
	BYTE feed_type = DBGetContactSettingByte(NULL, parent->m_szModuleName, FACEBOOK_KEY_FEED_TYPE, 0);
	if (feed_type < 0 || feed_type >= SIZEOF(feed_types))
		feed_type = 0;	
	return feed_types[feed_type].id;
}

char* facebook_client::load_cookies( )
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
	
	return mir_strdup(cookieString.c_str());
}

void facebook_client::store_headers( http::response* resp, NETLIBHTTPHEADER* headers, int headersCount )
{
	ScopedLock c( cookies_lock_ );

	for ( int i = 0; i < headersCount; i++ )
	{
		std::string header_name = headers[i].szName; // TODO: Casting?
		std::string header_value = headers[i].szValue; // TODO: Casting?

		if ( header_name == "Set-Cookie" )
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
		else
		{ // TODO RM: (un)comment
			//parent->Log("----- Got header '%s': %s", header_name.c_str(), header_value.c_str() );
			resp->headers[header_name] = header_value;
		}
	}
}

void facebook_client::clear_cookies( )
{
	ScopedLock s( cookies_lock_ );

	if ( !cookies.empty( ) )
		cookies.clear( );
}

bool facebook_client::login(const std::string &username,const std::string &password)
{
	handle_entry( "login" );

	username_ = username;
	password_ = password;

	// Access homepage to get initial cookies
	flap( FACEBOOK_REQUEST_HOME, NULL );

	// Prepare login data
	std::string data = "charset_test=%e2%82%ac%2c%c2%b4%2c%e2%82%ac%2c%c2%b4%2c%e6%b0%b4%2c%d0%94%2c%d0%84&locale=en&email=";
	data += utils::url::encode( username );
	data += "&pass=";
	data += utils::url::encode( password );
	data += "&pass_placeHolder=Password&login=Login&persistent=1";

	// Send validation
	http::response resp = flap( FACEBOOK_REQUEST_LOGIN, &data );

	// Process result data
	validate_response(&resp);

	if ( resp.code == HTTP_CODE_FOUND && resp.headers.find("Location") != resp.headers.end() )
	{
		// Check whether some Facebook things are required
		if ( resp.headers["Location"].find("help.php") != std::string::npos )
		{
			client_notify( TranslateT("Login error: Some Facebook things are required.") );
			parent->Log(" ! !  Login error: Some Facebook things are required.");
			// return handle_error( "login", FORCE_DISCONNECT );
		}
		
		// Check whether setting Machine name is required
		if ( resp.headers["Location"].find("/checkpoint/") != std::string::npos )
		{
			resp = flap( FACEBOOK_REQUEST_SETUP_MACHINE );
			
			std::string inner_data = "machine_name=MirandaIM&submit[Save%20Device]=Save%20Device";
			inner_data += "&post_form_id=";
			inner_data += utils::text::source_get_value(&resp.data, 3, "name=\"post_form_id\"", "value=\"", "\"" );
			
			inner_data += "&lsd=";
			inner_data += utils::text::source_get_value(&resp.data, 3, "name=\"lsd\"", "value=\"", "\"" );
			
			inner_data += "&nh=";
			inner_data += utils::text::source_get_value(&resp.data, 3, "name=\"nh\"", "value=\"", "\"" );

			resp = flap( FACEBOOK_REQUEST_SETUP_MACHINE, &inner_data );
			validate_response(&resp);
		}
	}
	
	if ( resp.code == HTTP_CODE_FOUND && resp.headers.find("Location") != resp.headers.end() )
	{
		// Check whether HTTPS connection is required and we don't have enabled it
		if ( !DBGetContactSettingByte( NULL, parent->m_szProtoName, FACEBOOK_KEY_FORCE_HTTPS, DEFAULT_FORCE_HTTPS ) )
		{    
			if ( resp.headers["Location"].find("https://") != std::string::npos )
			{
				client_notify(TranslateT("Your account requires HTTPS connection. Activating."));
				DBWriteContactSettingByte( NULL, parent->m_szProtoName, FACEBOOK_KEY_FORCE_HTTPS, 1 );
			}
		}

	}

	// Check for Device ID
	if ( cookies["datr"].length() )
		DBWriteContactSettingString( NULL, parent->m_szModuleName, FACEBOOK_KEY_DEVICE_ID, cookies["datr"].c_str() );

	switch ( resp.code )
	{
	case HTTP_CODE_FAKE_DISCONNECTED:
	{
		// When is error only because timeout, try login once more
		if ( handle_error( "login" ) )
			return login(username, password);
		else
			return false;
	}

	case HTTP_CODE_OK: // OK page returned, but that is regular login page we don't want in fact
	{ 
		// Check whether captcha code is required
		if ( resp.data.find("id=\"captcha\"") != std::string::npos )
		{
			client_notify( TranslateT("Login error: Captcha code is required. Bad login credentials?") );
			parent->Log(" ! !  Login error: Captcha code is required.");
			return handle_error( "login", FORCE_DISCONNECT );
		}
		
		// Get error message
		std::string error_str = utils::text::trim(
			utils::text::special_expressions_decode(
				utils::text::remove_html( 
					utils::text::edit_html(
						utils::text::source_get_value( &resp.data, 2, "id=\"standard_error\">", "</h2>" ) ) ) ) );

		if ( !error_str.length() )
			error_str = Translate("Unknown login error");
		parent->Log(" ! !  Login error: %s", error_str.c_str());

		std::string message = Translate("Login error: ") + error_str;
		TCHAR* tmessage = mir_a2t(message.c_str());
		client_notify( tmessage );
		mir_free( tmessage );
	}
	case HTTP_CODE_FORBIDDEN: // Forbidden
	case HTTP_CODE_NOT_FOUND: // Not Found
	default:
		return handle_error( "login", FORCE_DISCONNECT );

	case HTTP_CODE_FOUND: // Found and redirected to Home, Logged in, everything is OK
		if ( cookies.find("c_user") != cookies.end() )
		{
			this->self_.user_id = cookies.find("c_user")->second;
			DBWriteContactSettingString(NULL,parent->m_szModuleName,FACEBOOK_KEY_ID,this->self_.user_id.c_str());
			parent->Log("      Got self user id: %s", this->self_.user_id.c_str());
			return handle_success( "login" );
		} else {
			client_notify(TranslateT("Login error, probably bad login credentials."));
			parent->Log(" ! !  Login error, probably bad login credentials.");
			return handle_error( "login", FORCE_DISCONNECT );
		}
	}
}

bool facebook_client::logout( )
{
	if ( DBGetContactSettingByte(NULL, parent->m_szModuleName, FACEBOOK_KEY_DISABLE_LOGOUT, 0) )
		return true;

	handle_entry( "logout" );

	std::string data = "post_form_id=";
	data += ( this->post_form_id_.length( ) ) ? this->post_form_id_ : "0";
	data += "&fb_dtsg=";
	data += ( this->dtsg_.length( ) ) ? this->dtsg_ : "0";
	data += "&ref=mb&h=";
	data += this->logout_hash_;

	http::response resp = flap( FACEBOOK_REQUEST_LOGOUT, &data );

	if (hFcbCon)
		Netlib_CloseHandle(hFcbCon);
	hFcbCon = NULL;

	// Process result
	username_ = password_ = self_.user_id = "";

	switch ( resp.code )
	{
	case HTTP_CODE_OK:
	case HTTP_CODE_FOUND:
		return handle_success( "logout" );

	default:
		return false; // Logout not finished properly, but..okay, who cares :P
	}
}

bool facebook_client::home( )
{
	handle_entry( "home" );

	http::response resp = flap( FACEBOOK_REQUEST_HOME );

	// Process result data
	validate_response(&resp);

	switch ( resp.code )
	{
	case HTTP_CODE_OK:
	{		
		if ( resp.data.find( "id=\"navAccountName\"" ) != std::string::npos )
		{ // Backup for old fb version
			// Get real_name
			this->self_.real_name = utils::text::remove_html( utils::text::special_expressions_decode( utils::text::source_get_value( &resp.data, 2, " id=\"navAccountName\">", "</a" ) ) );
			DBWriteContactSettingUTF8String(NULL,parent->m_szModuleName,FACEBOOK_KEY_NAME,this->self_.real_name.c_str());
			DBWriteContactSettingUTF8String(NULL,parent->m_szModuleName,FACEBOOK_KEY_NICK,this->self_.real_name.c_str());
			parent->Log("      Got self real name: %s", this->self_.real_name.c_str());
		} else if ( resp.data.find("id=\"pageNav\"") != std::string::npos ) {
			// Get real_name
			this->self_.real_name = utils::text::remove_html( utils::text::special_expressions_decode( utils::text::source_get_value( &resp.data, 3, " class=\"headerTinymanName\"", ">", "</a" ) ) );
			DBWriteContactSettingUTF8String(NULL,parent->m_szModuleName,FACEBOOK_KEY_NAME,this->self_.real_name.c_str());
			DBWriteContactSettingUTF8String(NULL,parent->m_szModuleName,FACEBOOK_KEY_NICK,this->self_.real_name.c_str());
			parent->Log("      Got self real name: %s", this->self_.real_name.c_str());
		} else {
			client_notify(TranslateT("Something happened to Facebook. Maybe there was some major update so you should wait for an update."));
			return handle_error( "home", FORCE_DISCONNECT );
		}

		// Get avatar
		std::string avatar = utils::text::source_get_value( &resp.data, 3, "class=\\\"fbxWelcomeBoxImg", "src=\\\"", "\\\"" );
		if (avatar.empty())
			avatar = utils::text::source_get_value( &resp.data, 3, "class=\"fbxWelcomeBoxImg", "src=\"", "\"" );

		this->self_.image_url = utils::text::trim( utils::text::special_expressions_decode( avatar ) );
		parent->Log("      Got self avatar: %s", this->self_.image_url.c_str());
		parent->CheckAvatarChange(NULL, this->self_.image_url);

		// Get post_form_id
		this->post_form_id_ = utils::text::source_get_value( &resp.data, 3, "name=\"post_form_id\"", "value=\"", "\"" );
		parent->Log("      Got self post form id: %s", this->post_form_id_.c_str());

		// Get dtsg
		this->dtsg_ = utils::text::source_get_value( &resp.data, 3, "name=\"fb_dtsg\"", "value=\"", "\"" );
		parent->Log("      Got self dtsg: %s", this->dtsg_.c_str());

		// Get logout hash
		this->logout_hash_ = utils::text::source_get_value( &resp.data, 2, "<input type=\"hidden\" autocomplete=\"off\" name=\"h\" value=\"", "\"" );
		parent->Log("      Got self logout hash: %s", this->logout_hash_.c_str());
			

		// Get friend requests count and notify it
		std::string str_count = utils::text::source_get_value( &resp.data, 2, "<span id=\"requestsCountValue\">", "</span>" );
		if ( str_count.length() && str_count != std::string( "0" ) )
		{
			std::string message = Translate("Got new friend requests: ") + str_count;

			TCHAR* tmessage = mir_a2t(message.c_str());
			parent->NotifyEvent( parent->m_tszUserName, tmessage, NULL, FACEBOOK_EVENT_OTHER, TEXT(FACEBOOK_URL_REQUESTS) );
			mir_free( tmessage );
		}

		if (!DBGetContactSettingByte(NULL,parent->m_szModuleName,FACEBOOK_KEY_PARSE_MESSAGES, DEFAULT_PARSE_MESSAGES))
		{
			str_count = utils::text::source_get_value( &resp.data, 2, "<span id=\"messagesCountValue\">", "</span>" );
			if ( str_count.length() && str_count != std::string( "0" ) )
			{
				std::string message = Translate("Got new messages: ") + str_count;

				TCHAR* tmessage = mir_a2t(message.c_str());
				parent->NotifyEvent( parent->m_tszUserName, tmessage, NULL, FACEBOOK_EVENT_OTHER, TEXT(FACEBOOK_URL_MESSAGES) );
				mir_free( tmessage );
			}
		} else { // Parse messages directly for contacts
			ForkThread( &FacebookProto::ProcessUnreadMessages, this->parent, NULL );
		}
			
		str_count = utils::text::source_get_value( &resp.data, 2, "<span id=\"notificationsCountValue\">", "</span>" );
		if ( str_count.length() && str_count != std::string( "0" ) )
		{
			// Parse notifications directly to popups
			ForkThread( &FacebookProto::ProcessNotifications, this->parent, NULL );
		}

		if (DBGetContactSettingByte(NULL, parent->m_szModuleName, FACEBOOK_KEY_ENABLE_GROUPCHATS, DEFAULT_ENABLE_GROUPCHATS)) {
			// Get group chats
			std::string favorites = utils::text::source_get_value( &resp.data, 2, "<div id=\"leftCol\"", "<div id=\"contentCol\"" );

			std::string::size_type pos = 0;
			while ((pos = favorites.find("href=\"/groups/",pos)) != std::string::npos) {
				pos += 14;
				std::string item = favorites.substr(pos, favorites.find("</a>", pos) - pos);
				std::string id = item.substr(0, item.find("/"));
		
				if (!id.empty()) {
					std::string name = utils::text::source_get_value( &item, 3, "class=\"linkWrap", ">", "</div>" );
					name = utils::text::special_expressions_decode(utils::text::slashu_to_utf8( name ) );
					parent->Log("      Got new group chat: %s (id: %s)", name.c_str(), id.c_str());
					if (!name.empty())
						parent->AddChat(id.c_str(), name.c_str());
				}
			}
		}

		return handle_success( "home" );

	}
	case HTTP_CODE_FOUND:
		// Work-around for replica_down, f**king hell what's that?
		parent->Log("      REPLICA_DOWN is back in force!");
		return this->home();
	
	default:
		return handle_error( "home", FORCE_DISCONNECT );
	}
}

bool facebook_client::chat_state( bool online )
{
	handle_entry( "chat_state" );
  
	std::string data = "visibility=";
	data += ( online ) ? "true" : "false";
	data += "&window_id=0";
	data += "&post_form_id=";
	data += ( post_form_id_.length( ) ) ? post_form_id_ : "0";
	data += "&post_form_id_source=AsyncRequest";
	data += "&fb_dtsg=" + this->dtsg_;
	data += "&lsd=";
	http::response resp = flap( FACEBOOK_REQUEST_VISIBILITY, &data );
  
	return handle_success( "chat_state" );
}

bool facebook_client::reconnect( )
{
	handle_entry( "reconnect" );

	// Request reconnect
	http::response resp = flap( FACEBOOK_REQUEST_RECONNECT );

	// Process result data
	validate_response(&resp);

	switch ( resp.code )
	{
	case HTTP_CODE_OK:
	{
		this->chat_channel_jslogger_ = utils::text::source_get_value( &resp.data, 2, "\"jslogger_suffix\":\"", "\"" );
		parent->Log("      Got self channel jslogger: %s", this->chat_channel_jslogger_.c_str());
				
		this->chat_channel_partition_ = utils::text::source_get_value2( &resp.data, "\"partition\":", ",}" );
		parent->Log("      Got self channel partition: %s", this->chat_channel_partition_.c_str());
		
		this->chat_channel_host_ = utils::text::source_get_value( &resp.data, 2, "\"host\":\"", "\"" );
		parent->Log("      Got self channel host: %s", this->chat_channel_host_.c_str());

		this->chat_sequence_num_ = utils::text::source_get_value2( &resp.data, "\"seq\":", ",}" );
		parent->Log("      Got self sequence number: %s", this->chat_sequence_num_.c_str());

		if (this->chat_channel_jslogger_.empty()) {
			if (!atoi(this->chat_channel_host_.substr(0, this->chat_channel_host_.find(".")).c_str())) {
				this->chat_channel_jslogger_ = "SOMETHING";
				parent->Log("      Got no jslogger, changed.");
			}
		}
  		
		return handle_success( "reconnect" );
	}
	 
	default:
		return handle_error( "reconnect", FORCE_DISCONNECT );
	}
}

bool facebook_client::buddy_list( )
{
	handle_entry( "buddy_list" );

	// Prepare update data
	std::string data = "user=" + this->self_.user_id + "&fetch_mobile=true&post_form_id=" + this->post_form_id_ + "&fb_dtsg=" + this->dtsg_ + "&lsd=&post_form_id_source=AsyncRequest&__user=" + this->self_.user_id;

	{
		ScopedLock s(buddies_lock_);

		int counter = 0;
		for (List::Item< facebook_user >* i = buddies.begin(); i != NULL; i = i->next, counter++ )
		{
			data += "&available_user_info_ids[";
			data += utils::conversion::to_string(&counter, UTILS_CONV_UNSIGNED_NUMBER);
			data += "]=";
			data += i->data->user_id;
		}
	}

	// Get buddy list
	http::response resp = flap( FACEBOOK_REQUEST_BUDDY_LIST, &data );

	// Process result data
	validate_response(&resp);

	switch ( resp.code )
	{
	case HTTP_CODE_OK:
	{
		std::string* response_data = new std::string( resp.data );
		ForkThread( &FacebookProto::ProcessBuddyList, this->parent, ( void* )response_data );
		return handle_success( "buddy_list" );
	}

	case HTTP_CODE_FAKE_ERROR:
	case HTTP_CODE_FAKE_DISCONNECTED:
	default:
		return handle_error( "buddy_list" );
	}
}

bool facebook_client::facepiles( )
{	
	handle_entry( "facepiles" );

	int count = (int)CallServiceSync(MS_GC_GETSESSIONCOUNT, 0, (LPARAM)parent->m_szModuleName);
	for ( int i = 0; i < count; i++ ) {
		GC_INFO gci = {0};
		gci.Flags = BYINDEX | TYPE | ID;
		gci.iItem = i;
		gci.pszModule = parent->m_szModuleName;
		if ( !CallServiceSync( MS_GC_GETINFO, 0, (LPARAM)&gci ) && gci.iType == GCW_CHATROOM ) {
			char *id = mir_t2a(gci.pszID);

			// Prepare data
			std::string data = "id=";
			data += id;
			data += "&post_form_id=" + this->post_form_id_ + "&fb_dtsg=" + this->dtsg_ + "&lsd=&post_form_id_source=AsyncRequest&__user=" + this->self_.user_id + "&phstamp=0";

			// Get facepiles
			http::response resp = flap( FACEBOOK_REQUEST_FACEPILES, &data );

			// Process result data
			validate_response(&resp);

			std::string chat_id = id;
			mir_free(id);
			
			switch ( resp.code )
			{
			case HTTP_CODE_OK:
				ForkThread( &FacebookProto::ProcessFacepiles, this->parent, new send_chat(chat_id, resp.data) );
				break;

			case HTTP_CODE_FAKE_ERROR:
			case HTTP_CODE_FAKE_DISCONNECTED:
			default:
				return handle_error( "facepiles" );
			}
			
		}			
	}

	return handle_success( "facepiles" );
}

bool facebook_client::load_friends( )
{
	handle_entry( "load_friends" );

	// Get buddy list
	http::response resp = flap( FACEBOOK_REQUEST_LOAD_FRIENDS );

	// Process result data
	validate_response(&resp);

	switch ( resp.code )
	{
	case HTTP_CODE_OK:
	{
		std::string* response_data = new std::string( resp.data );
		ForkThread( &FacebookProto::ProcessFriendList, this->parent, ( void* )response_data );
		return handle_success( "load_friends" );
	}
	case HTTP_CODE_FAKE_ERROR:
	case HTTP_CODE_FAKE_DISCONNECTED:
	default:
		return handle_error( "load_friends" );
	}
}

bool facebook_client::feeds( )
{
	handle_entry( "feeds" );

	// Get feeds
	http::response resp = flap( FACEBOOK_REQUEST_FEEDS );

	// Process result data
	validate_response(&resp);
  
	std::string::size_type pos = 0;
	switch ( resp.code )
	{
	case HTTP_CODE_OK:
		pos = resp.data.find( "\"storyCount\":" );
		if ( pos != std::string::npos )
		{
			if (resp.data.substr( pos + 13, 1 ) != "0")
			{
				std::string* response_data = new std::string( resp.data );
			    ForkThread( &FacebookProto::ProcessFeeds, this->parent, ( void* )response_data );
			}
		}
		return handle_success( "feeds" );

	case HTTP_CODE_FAKE_ERROR:
	case HTTP_CODE_FAKE_DISCONNECTED:
	default:
		return handle_error( "feeds" );
	}
}

bool facebook_client::channel( )
{
	handle_entry( "channel" );

	// Get update
	http::response resp = flap( FACEBOOK_REQUEST_MESSAGES_RECEIVE );

	// Process result data
	validate_response(&resp);

	if ( resp.code != HTTP_CODE_OK )
	{
		// Something went wrong
	}
	else if ( resp.data.find( "\"t\":\"continue\"" ) != std::string::npos )
	{
		// Everything is OK, no new message received
	}
	else if ( resp.data.find( "\"t\":\"fullReload\"" ) != std::string::npos )
	{
		// Something went wrong (server flooding?)

		parent->Log("! ! ! Requested full reload");
    
		this->chat_sequence_num_ = utils::text::source_get_value2( &resp.data, "\"seq\":", ",}" );
		parent->Log("      Got self sequence number: %s", this->chat_sequence_num_.c_str());

		this->chat_reconnect_reason_ = utils::text::source_get_value2( &resp.data, "\"reason\":", ",}" );
		parent->Log("      Reconnect reason: %s", this->chat_reconnect_reason_.c_str());
	}
	else if ( resp.data.find( "\"t\":\"refresh\"" ) != std::string::npos )
	{
		// Something went wrong (server flooding?)
		parent->Log("! ! ! Requested channel refresh");
    
		this->chat_reconnect_reason_ = utils::text::source_get_value2( &resp.data, "\"reason\":", ",}" );
		parent->Log("      Reconnect reason: %s", this->chat_reconnect_reason_.c_str());

		this->chat_sequence_num_ = utils::text::source_get_value2( &resp.data, "\"seq\":", ",}" );
		parent->Log("      Got self sequence number: %s", this->chat_sequence_num_.c_str());

		return this->reconnect( );
	} else {
		// Something has been received, throw to new thread to process
		std::string* response_data = new std::string( resp.data );
		ForkThread( &FacebookProto::ProcessMessages, this->parent, ( void* )response_data );

		// Increment sequence number
		this->chat_sequence_num_ = utils::text::source_get_value2( &resp.data, "\"seq\":", ",}" );
		parent->Log("      Got self sequence number: %s", this->chat_sequence_num_.c_str());
	}

	// Return
	switch ( resp.code )
	{
	case HTTP_CODE_OK:
		return handle_success( "channel" );

	case HTTP_CODE_FAKE_DISCONNECTED:
	case HTTP_CODE_FAKE_ERROR:
	default:
		// Testing workaround for channel change
		if (!this->chat_channel_jslogger_.empty())
			this->chat_channel_jslogger_ = "_";
		else
			this->chat_channel_jslogger_.clear();
		
		return handle_error( "channel" );
	}
}

bool facebook_client::send_message( std::string message_recipient, std::string message_text, std::string *error_text, bool use_inbox )
{
	handle_entry( "send_message" );

	http::response resp;

	if (parent->isInvisible() || use_inbox) {
		// Use inbox send message when invisible
		std::string data = "action=send&body=";
		data += utils::url::encode( message_text );
		data += "&recipients[0]=";
		data += message_recipient;
		data += "&lsd=&fb_dtsg=";
		data += ( dtsg_.length( ) ) ? dtsg_ : "0";
		data += "&post_form_id=";
		data += ( post_form_id_.length( ) ) ? post_form_id_ : "0";

		resp = flap( FACEBOOK_REQUEST_ASYNC, &data );	
	} else {
		// Use standard send message
		std::string data = "msg_text=";
		data += utils::url::encode( message_text );
		data += "&msg_id=";
		data += utils::time::mili_timestamp( );
		data += "%3A";
		data += utils::time::unix_timestamp( );
		data += "&to=";
		data += message_recipient;
		data += "&__user=";
		data += this->self_.user_id;
		data += "&client_time=";
		data += utils::time::mili_timestamp( );
		data += "&pvs_time&fb_dtsg=";
		data += ( dtsg_.length( ) ) ? dtsg_ : "0";
		data += "&to_offline=false&to_idle=false&lsd&post_form_id_source=AsyncRequest&num_tabs=1";
		data += "&window_id=0&sidebar_launched=false&sidebar_enabled=false&sidebar_capable=false&sidebar_should_show=false&sidebar_visible=false";
		data += "&post_form_id=";
		data += ( post_form_id_.length( ) ) ? post_form_id_ : "0";

		resp = flap( FACEBOOK_REQUEST_MESSAGE_SEND, &data );
	}

	
	validate_response(&resp);
	*error_text = resp.error_text;

	switch ( resp.error_number )
	{
  	case 0: // Everything is OK
		break;

    //case 1356002: // You are offline - wtf??

	case 1356003: // Contact is offline
	{
		HANDLE hContact = parent->ContactIDToHContact( message_recipient );
		if (hContact != NULL)
  			DBWriteContactSettingWord(hContact,parent->m_szModuleName,"Status",ID_STATUS_OFFLINE);
		return false;
	} break;

  	case 1356026: // Contact has alternative client
	{
		client_notify(TranslateT("Need confirmation for sending messages to other clients.\nOpen facebook website and try to send message to this contact again!"));
      /*
          post na url http://www.facebook.com/ajax/chat/post_application_settings.php?__a=1

          enable_and_send      Povolit a odeslat                                                                                                                                                                                                                                                                                                                                                                                                                               
          to_send              AQCoweMPeszBoKpd4iahcOyhmh0kiTYIhv1b5wCtuBiD0AaPVZIdEp3Pf5JMBmQ-9wf0ju-xdi-VRuk0ERk_I7XzI5dVJCs6-B0FExTZhspD-4-kTZLmZI-_M6fIuF2328yMyT3R3UEUmMV8P9MHcZwu-_pS3mOhsaHf6rIVcQ2rocSqLKi03wLKCfg0m8VsptPADWpOI-UNcIo-xl1PAoC1yVnL2wEXEtnF1qI_xFcmlJZ40AOONfIF_LS_lBrGYA-oCWLUK-GLHtQAHjO8aDeNXDU8Jk7Z_ES-_oAHee2PVLHcG_ACHXpasE7Iu3XFLMrdN2hjM96AjPRIf0Vk8gBZzfW_lUspakZmXxMI7iSNQE8lourK_6B3Z1s4UHxDZCNXYuc9gh70nm_xnaxnF9K1bR00s4MltnFjUT_3ypThzA  
          __d                  1                                                                                                                                                                                                                                                                                                                                                                                                                                               
          post_form_id         c73ebd9d94b7449c40e6965410fcdcf6                                                                                                                                                                                                                                                                                                                                                                                                                
          fb_dtsg              Tb-T9                                                                                                                                                                                                                                                                                                                                                                                                                                           
          lsd                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
          post_form_id_source  AsyncRequest                                                                                                                                                                                                                                                                                                                                                                                                                                    
          */
		return false;
	} break;
 
    default: // Other error
		return false;
 	}

	switch ( resp.code )
	{
	case HTTP_CODE_OK:
		return handle_success( "send_message" );

	case HTTP_CODE_FAKE_ERROR:
	case HTTP_CODE_FAKE_DISCONNECTED:
	default:
		*error_text = Translate("Timeout when sending message.");

		handle_error( "send_message" );
		return false;
	}
}

void facebook_client::close_chat( std::string message_recipient )
{
	// TODO RM: better optimalization for close_chat
	// add items to list and then checking every x seconds
/*	if ( (::time(NULL) - parent->facy.last_close_chat_time_) < 8 )
		return;*/
	// parent->facy.last_close_chat_time_ = ::time(NULL);

	/* Wait some time before close window, because sometimes facebook
		can't close it so soon. But maybe this didnt help also. */
	Sleep(300); 

	std::string data = "close_chat=";
	data += message_recipient;
	data += "&window_id=0";
	data += "&post_form_id=";
	data += ( post_form_id_.length( ) ) ? post_form_id_ : "0";
	data += "&post_form_id_source=AsyncRequest";
	data += "&fb_dtsg=";
	data += ( this->dtsg_.length( ) ) ? this->dtsg_ : "0";
	
	http::response resp = flap( FACEBOOK_REQUEST_TABS, &data );
}

void facebook_client::chat_mark_read( std::string message_recipient )
{
	// TODO RM: optimalization?

	std::string data = "action=chatMarkRead&other_user=";
	data += message_recipient;
	data += "&post_form_id=";
	data += ( post_form_id_.length( ) ) ? post_form_id_ : "0";
	data += "&fb_dtsg=";
	data += ( this->dtsg_.length( ) ) ? this->dtsg_ : "0";
	data += "&post_form_id_source=AsyncRequest&lsd=";
	
	http::response resp = flap( FACEBOOK_REQUEST_ASYNC, &data );
}

bool facebook_client::set_status(const std::string &status_text)
{
	handle_entry( "set_status" );

	std::string data = "post_form_id_source=AsyncRequest&post_form_id=";
	data += ( this->post_form_id_.length( ) ) ? this->post_form_id_ : "0";
	data += "&fb_dtsg=";
	data += ( this->dtsg_.length( ) ) ? this->dtsg_ : "0";
	data += "&target_id=";
	data += this->self_.user_id;

	if ( status_text.length( ) )
	{
		data += "&action=PROFILE_UPDATE&app_id=&hey_kid_im_a_composer=true&display_context=profile&_log_display_context=profile&ajax_log=1&status=";
		data += utils::url::encode( status_text );
		data += "&profile_id=";
		data += this->self_.user_id;
	}

	http::response resp = flap( FACEBOOK_REQUEST_STATUS_SET, &data );

	validate_response(&resp);

	switch ( resp.code )
	{
	case HTTP_CODE_OK:
  		return handle_success( "set_status" );

  	case HTTP_CODE_FAKE_ERROR:
	case HTTP_CODE_FAKE_DISCONNECTED:
	default:
  		return handle_error( "set_status" );
	}
}

//////////////////////////////////////////////////////////////////////////////

bool facebook_client::save_url(const std::string &url,const std::string &filename, HANDLE &nlc)
{
	NETLIBHTTPREQUEST req = {sizeof(req)};
	NETLIBHTTPREQUEST *resp;
	req.requestType = REQUEST_GET;
	req.szUrl = const_cast<char*>(url.c_str());
	req.flags = NLHRF_HTTP11 | NLHRF_REDIRECT | NLHRF_PERSISTENT;
	req.nlc = nlc;

	resp = reinterpret_cast<NETLIBHTTPREQUEST*>(CallService( MS_NETLIB_HTTPTRANSACTION,
		reinterpret_cast<WPARAM>(this->parent->m_hNetlibUser), reinterpret_cast<LPARAM>(&req) ));

	if ( resp )
	{
		nlc = resp->nlc;
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
		nlc = NULL;
		return false;
	}
}
