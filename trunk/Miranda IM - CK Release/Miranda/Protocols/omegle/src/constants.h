/*

Omegle plugin for Miranda Instant Messenger
_____________________________________________

Copyright � 2011-12 Robert P�sel

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

#pragma once

// Version management
#define __VERSION_DWORD             PLUGIN_MAKE_VERSION(0,0,0,6)
#define __VERSION_STRING            "0.0.0.6"

// Product management
#define OMEGLE_NAME               "Omegle"
#define OMEGLE_SERVER_REGULAR     "http://www.omegle.com"
#define OMEGLE_SERVER_CHAT        "http://%s.omegle.com"

// Limits
#define OMEGLE_TIMEOUTS_LIMIT     5
// There is "no limit" on Omegle, but we should set some wise limit
#define OMEGLE_MESSAGE_LIMIT      4096
#define OMEGLE_MESSAGE_LIMIT_TEXT "4096"

// Omegle request types
#define OMEGLE_REQUEST_HOME						100 // getting server name
#define OMEGLE_REQUEST_START					110 // starting conversation
#define OMEGLE_REQUEST_STOP						120 // ending conversation
#define OMEGLE_REQUEST_SEND						300 // sending message
#define OMEGLE_REQUEST_EVENTS			        301 // receiving events
#define OMEGLE_REQUEST_TYPING_START				310 // started typing
#define OMEGLE_REQUEST_TYPING_STOP				311 // stoped typing
#define OMEGLE_REQUEST_RECAPTCHA				400 // recaptcha handling
