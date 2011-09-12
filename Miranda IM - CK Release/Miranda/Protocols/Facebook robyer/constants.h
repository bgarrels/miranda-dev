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

File name      : $HeadURL$
Revision       : $Revision$
Last change by : $Author$
Last change on : $Date$

*/

#pragma once

// Version management
#include "build.h"
#define __VERSION_DWORD             PLUGIN_MAKE_VERSION(0, 0, 4, 0)
#define __PRODUCT_DWORD             PLUGIN_MAKE_VERSION(0, 9, 14, 0)
#define __VERSION_STRING            "0.0.4.0"
#define __PRODUCT_STRING            "0.9.14.0"
#define __VERSION_VS_FILE           0,0,4,0
#define __VERSION_VS_PROD           0,9,14,0
#define __VERSION_VS_FILE_STRING    "0, 0, 4, 0"
#define __VERSION_VS_PROD_STRING    "0, 9, 14, 0"

// Product management
#define FACEBOOK_NAME               "Facebook"
#define FACEBOOK_URL_HOMEPAGE       "http://www.facebook.com/"
#define FACEBOOK_URL_REQUESTS       "http://www.facebook.com/n/?reqs.php"
#define FACEBOOK_URL_MESSAGES       "http://www.facebook.com/n/?inbox"
#define FACEBOOK_URL_NOTIFICATIONS  "http://www.facebook.com/n/?notifications.php"
#define FACEBOOK_URL_PROFILE        "http://www.facebook.com/profile.php?id="
#define FACEBOOK_URL_GROUP          "http://www.facebook.com/n/?home.php&sk=group_"

// Connection
#define FACEBOOK_SERVER_REGULAR     "www.facebook.com"
//#define FACEBOOK_SERVER_MOBILE      "m.facebook.com"
#define FACEBOOK_SERVER_CHAT        "%s.%s.facebook.com"
#define FACEBOOK_SERVER_LOGIN       "login.facebook.com"
#define FACEBOOK_SERVER_APPS        "apps.facebook.com"

// Limits
#define FACEBOOK_MESSAGE_LIMIT      1024
#define FACEBOOK_MESSAGE_LIMIT_TEXT "1024"
#define FACEBOOK_MIND_LIMIT         420
#define FACEBOOK_MIND_LIMIT_TEXT    "420"
#define FACEBOOK_TIMEOUTS_LIMIT     5
#define FACEBOOK_GROUP_NAME_LIMIT   100

// Defaults
#define FACEBOOK_MINIMAL_POLL_RATE              10
#define FACEBOOK_DEFAULT_POLL_RATE              24 // in seconds
#define FACEBOOK_MAXIMAL_POLL_RATE              60
#define FACEBOOK_USER_UPDATE_RATE               7200 // in seconds

#define DEFAULT_FORCE_HTTPS             0
#define DEFAULT_CLOSE_WINDOWS_ENABLE    0
#define DEFAULT_SET_MIRANDA_STATUS      0
#define DEFAULT_LOGGING_ENABLE          0
#define DEFAULT_EVENT_NOTIFICATIONS_ENABLE  1
#define DEFAULT_EVENT_FEEDS_ENABLE          1
#define DEFAULT_EVENT_OTHER_ENABLE          1
#define DEFAULT_EVENT_CLIENT_ENABLE         1
#define DEFAULT_EVENT_COLBACK           0x00ffffff
#define DEFAULT_EVENT_COLTEXT           0x00000000
#define DEFAULT_EVENT_TIMEOUT_TYPE      0
#define DEFAULT_EVENT_TIMEOUT           20
#define DEFAULT_SYSTRAY_NOTIFY          0
#define DEFAULT_SHOW_OLD_FEEDS          0

// Event flags
#define FACEBOOK_EVENT_CLIENT          0x10000000 // Facebook error or info message
#define FACEBOOK_EVENT_NEWSFEED        0x20000000 // Facebook newsfeed (wall) message
#define FACEBOOK_EVENT_NOTIFICATION    0x40000000 // Facebook new notification
#define FACEBOOK_EVENT_OTHER           0x80000000 // Facebook other event - friend requests/new messages

// Facebook request types // TODO: Provide MS_ and release in FB plugin API?
#define FACEBOOK_REQUEST_API_CHECK              50  // check latest API version
#define FACEBOOK_REQUEST_LOGIN                  100 // connecting physically
#define FACEBOOK_REQUEST_SETUP_MACHINE          102 // setting machine name
#define FACEBOOK_REQUEST_LOGOUT                 106 // disconnecting physically
#define FACEBOOK_REQUEST_HOME                   110 // getting __post_form_id__ + __fb_dtsg__ + ...
#define FACEBOOK_REQUEST_BUDDY_LIST             120 // getting regular updates (friends online, ...)
#define FACEBOOK_REQUEST_LOAD_FRIENDS			121 // getting list of all friends
#define FACEBOOK_REQUEST_FEEDS                  125 // getting feeds
#define FACEBOOK_REQUEST_RECONNECT              130 // getting __sequence_num__ and __channel_id__
#define FACEBOOK_REQUEST_PROFILE_GET            200 // getting others' profiles
#define FACEBOOK_REQUEST_STATUS_SET             251 // setting my "What's on my mind?"
#define FACEBOOK_REQUEST_MESSAGE_SEND           300 // sending message
#define FACEBOOK_REQUEST_MESSAGES_RECEIVE       301 // receiving messages
#define FACEBOOK_REQUEST_TYPING_SEND            304 // sending typing notification
#define FACEBOOK_REQUEST_SETTINGS               305 // setting chat visibility
#define FACEBOOK_REQUEST_TABS					306 // closing message window, setting chat visibility
#define	FACEBOOK_REQUEST_ASYNC					307 // marking messages read

#define FACEBOOK_RECV_MESSAGE	1
#define FACEBOOK_SEND_MESSAGE	2

// News Feed types
static const struct
{
	char *name;
	char *id;
} feed_types[] = {
	{ "Most Recent", "lf" },
	{ "Status Updates", "app_2915120374" },
	{ "Top News", "h" },
};

// User-Agents
static const struct
{
	char *name;
	char *id;
} user_agents[] = {
	{ "Miranda IM (default)", "Miranda IM (default)" },
	{ "Internet Explorer 8", "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Trident/4.0)" },
	{ "Internet Explorer 9", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)" },
	{ "Mozilla Firefox 4.0", "Mozilla/5.0 (Windows NT 6.1; rv:2.0) Gecko/20100101 Firefox/4.0" },
	{ "Opera 11.01 (Windows XP)", "Opera/9.80 (Windows NT 5.1; U; en) Presto/2.7.62 Version/11.01" },
	{ "Opera 11.01 (Mac OS X 10.5.8)", "Opera/9.80 (Macintosh; Intel Mac OS X 10.5.8; U; en) Presto/2.7.62 Version/11.01" },
	{ "Safari 5.0.3 (Mac OS X 10.5.8)", "Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_7; en-us) AppleWebKit/534.16+ (KHTML, like Gecko) Version/5.0.3 Safari/533.19.4" },
	{ "Google Chrome 11.0.661 (Windows XP)", "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US) AppleWebKit/534.19 (KHTML, like Gecko) Chrome/11.0.661.0 Safari/534.19" },
};
