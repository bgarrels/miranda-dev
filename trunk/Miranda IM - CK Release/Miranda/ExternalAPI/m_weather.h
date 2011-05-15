/*
Weather Protocol plugin for Miranda IM
Copyright (C) 2005-2009 Boris Krasnovskiy All Rights Reserved
Copyright (C) 2002-2005 Calvin Che

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef M_WEATHER_H__
#define M_WEATHER_H__ 1

// {6B612A34-DCF2-4e32-85CF-B6FD006B745E}
#define MIID_WEATHER { 0x6b612a34, 0xdcf2, 0x4e32, { 0x85, 0xcf, 0xb6, 0xfd, 0x0, 0x6b, 0x74, 0x5e } } 

//============  CONSTANTS  ============

// name
#define WEATHERPROTONAME	"Weather"
#define WEATHERPROTOTEXT	"Weather"
#define DEFCURRENTWEATHER	"WeatherCondition"
#define WEATHERCONDITION	"Current"

// weather conditions
#define SUNNY			ID_STATUS_ONLINE
#define NA				ID_STATUS_OFFLINE
#define PCLOUDY			ID_STATUS_AWAY
#define CLOUDY			ID_STATUS_NA
#define RAIN			ID_STATUS_OCCUPIED
#define RSHOWER			ID_STATUS_DND
#define SNOW			ID_STATUS_FREECHAT
#define LIGHT			ID_STATUS_INVISIBLE
#define THUNDER			ID_STATUS_INVISIBLE
#define SSHOWER			ID_STATUS_ONTHEPHONE
#define FOG				ID_STATUS_OUTTOLUNCH
#define UNAVAIL			40081


//============  WEATHER CONDITION STRUCT  ============

// weather conditions  (added in v0.1.2.0)
typedef struct {
	HANDLE hContact;
	char id[128];
	char city[128];
	char update[64];
	char cond[128];
	char temp[16];
	char low[16];
	char high[16];
	char feel[16];
	char wind[16];
	char winddir[64];
	char dewpoint[16];
	char pressure[16];
	char humid[16];
	char vis[16];
	char sunrise[32];
	char sunset[32];
// are the other ones that important!?
	WORD status;
} WEATHERINFO;



// ===============  WEATHER SERVICES  ================

// Enable or disable weather protocol.
// WPARAM = FALSE to toggle, TRUE to use the LPARAM
// LPARAM = TRUE to enable, FALSE to disable
#define MS_WEATHER_ENABLED		"Weather/EnableDisable"

// Update all weather info
// WPARAM = LPARAM = NULL
#define MS_WEATHER_UPDATEALL	"Weather/UpdateAll"

// Update all weather info + erase the old ones
// WPARAM = LPARAM = NULL
#define MS_WEATHER_REFRESHALL	"Weather/RefreshAll"

// Below are the service functions for weather contacts
// The plugin does NOT verify that they are used in weather contact,
// so bad call on these function may cause crashes.

// Update a single station
// WPARAM = (HANDLE)hContact
// LPARAM = NULL
#define MS_WEATHER_UPDATE		"Weather/Update"

// Update a single station + delete old settings
// WPARAM = (HANDLE)hContact
// LPARAM = NULL
#define MS_WEATHER_REFRESH		"Weather/Refresh"

// View the brief info of a contact
// WPARAM = (HANDLE)hContact
// LPARAM = NULL
#define MS_WEATHER_BRIEF		"Weather/Brief"

// Use default browser to open the complete forecast on web
// WPARAM = (HANDLE)hContact
// LPARAM = NULL
#define MS_WEATHER_COMPLETE		"Weather/CompleteForecast"

// Use default browser to open the weather map defined for the contact
// WPARAM = (HANDLE)hContact
// LPARAM = NULL
#define MS_WEATHER_MAP			"Weather/Map"

// Open the external log of the weather contact
// WPARAM = (HANDLE)hContact
// LPARAM = NULL
#define MS_WEATHER_LOG			"Weather/Log"

// Edit weather contact setting
// WPARAM = (HANDLE)hContact
// LPARAM = NULL
#define MS_WEATHER_EDIT			"Weather/Edit"

// parse the string to turn it to weather display
// WPARAM = (WEATHERINFO*)hContact
// LPARAM = (char*)display_str
#define MS_WEATHER_GETDISPLAY	"Weather/GetDisplay"

// ===============  WEATHER EVENTS  ================

/*
HANDLE hContact = (HANDLE)wParam;
BOOL Condition_Changed = (BOOL)lParam;

hContact is the handle of updated contact
If the weather condition is differ from the last update (either temperature/condition,
or the last update time, depend what the user choose in the options), then
Condition_Changed is true; otherwise is false.
*/
#define ME_WEATHER_UPDATED	"Miranda/Weather/Updated"

/*
Shows a warning message for Weather PopUp.
wParam = (char*) lpzMessage
lParam = Type
Type can either be SM_WARNING, SM_NOTIFY, or SM_WEATHERALERT

This event is used to avoid the error popup to occurs within a thread, so the "Use
multiply thread" fuction don't have to be enabled for weather popups to work.
*/
#define SM_WEATHERALERT		16
#define ME_WEATHER_ERROR	"Miranda/Weather/Error"


#endif //M_WEATHER_H__
