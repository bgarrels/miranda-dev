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

File name      : $HeadURL: http://eternityplugins.googlecode.com/svn/trunk/facebook/client.h $
Revision       : $Revision: 92 $
Last change by : $Author: n3weRm0re.ewer $
Last change on : $Date: 2011-01-20 21:38:59 +0100 (čt, 20 1 2011) $

*/

#pragma once

#define FORCE_DISCONNECT true

class facebook_client
{
public:

	////////////////////////////////////////////////////////////

	// Client definition

	facebook_client( )
	{
		username_ = password_ = \
		post_form_id_ = dtsg_ = \
		chat_sequence_num_ = chat_channel_host_ = chat_channel_partition_ = chat_channel_jslogger_ = \
		logout_hash_ = "";

		msgid_ = error_count_ = last_feeds_update_ = \
		last_notification_time_ = last_message_time_ = \
		last_grpmessage_time_ = last_close_chat_time_ = 0;

		invisible_ = is_typing_ = false;

		buddies_lock_ = send_message_lock_ = NULL;
		hMsgCon = NULL;
		hFcbCon = NULL;
		fcb_conn_lock_ = NULL;
	}

	HANDLE hMsgCon;
	HANDLE hFcbCon;
	HANDLE fcb_conn_lock_;

	// Parent handle

	FacebookProto*  parent;

	// User data

	facebook_user   self_;

	std::string username_;
	std::string password_;

	std::string post_form_id_;
	std::string dtsg_;
	std::string logout_hash_;
	std::string chat_channel_host_;
	std::string chat_channel_jslogger_;
	std::string chat_channel_partition_;
	std::string chat_sequence_num_;
	std::string chat_reconnect_reason_;
	bool    invisible_;
	bool    is_typing_;
	time_t  last_feeds_update_;
	time_t  last_notification_time_;
	time_t  last_message_time_;
	time_t  last_grpmessage_time_;
	time_t  last_close_chat_time_;
	int     msgid_;

	//bool api_check( );

	////////////////////////////////////////////////////////////

	// Client vs protocol communication

	void    client_notify( TCHAR* message );

	////////////////////////////////////////////////////////////

	// Cookies, Data storage

	HANDLE cookies_lock_;
	HANDLE headers_lock_;

	std::map< std::string, std::string >    cookies;
	std::map< std::string, std::string >    headers;

	std::string get_user_agent( );
	std::string get_newsfeed_type( );

	std::string load_cookies( );
	void    store_headers( http::response* resp, NETLIBHTTPHEADER* headers, int headers_count );
	void    clear_cookies( );

	////////////////////////////////////////////////////////////

	// Connection handling

	unsigned int error_count_;

	bool    validate_response( http::response* );

	bool    handle_entry( std::string method );
	bool    handle_success( std::string method );
	bool    handle_error( std::string method, bool force_disconnect = false );

	void __inline increment_error( ) { this->error_count_++; }
	void __inline decrement_error( ) { if ( error_count_ > 0 ) error_count_--; }
	void __inline reset_error( ) { error_count_ = 0; }

	////////////////////////////////////////////////////////////

	// Login handling

	bool    login( const std::string &username, const std::string &password );
	bool    logout( );

	const std::string & get_username() const;

	////////////////////////////////////////////////////////////

	// Session handling

	bool    home( );
	bool    reconnect( );
	bool    chat_state( bool online = true );

	////////////////////////////////////////////////////////////

	// Updates handling

	List::List< facebook_user > buddies;
	HANDLE  buddies_lock_;
	HANDLE  send_message_lock_;

	bool    buddy_list( );
	bool	load_friends( );
	bool    feeds( );

	////////////////////////////////////////////////////////////

	// Messages handling

	bool    channel( );
	bool    send_message( std::string message_recipient, std::string message_text, std::string *error_text );
	void    close_chat( std::string message_recipient );
	void    chat_mark_read( std::string message_recipient );

	////////////////////////////////////////////////////////////

	// Status handling

	bool    set_status(const std::string &text);

	////////////////////////////////////////////////////////////

	// HTTP communication

	http::response  flap( const int request_type, std::string* request_data = NULL );
	bool    save_url(const std::string &url,const std::string &filename, HANDLE &nlc);

	DWORD   choose_security_level( int );
	int     choose_method( int );
	std::string choose_proto( int );
	std::string choose_server( int, std::string* data = NULL );
	std::string choose_action( int, std::string* data = NULL );
	std::string choose_request_url( int, std::string* data = NULL );

	NETLIBHTTPHEADER*   get_request_headers( int request_type, int* headers_count );
	void    set_header( NETLIBHTTPHEADER* header, char* name );
	void    refresh_headers( );

	////////////////////////////////////////////////////////////

	// Netlib handle

	HANDLE handle_;

	void set_handle(HANDLE h)
	{
		handle_ = h;
	}
};
