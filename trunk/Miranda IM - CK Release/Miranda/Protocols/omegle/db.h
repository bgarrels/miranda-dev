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

#pragma once

// DB macros
#define getByte( setting, error )           DBGetContactSettingByte( NULL, m_szModuleName, setting, error )
#define setByte( setting, value )           DBWriteContactSettingByte( NULL, m_szModuleName, setting, value )
#define getWord( setting, error )           DBGetContactSettingWord( NULL, m_szModuleName, setting, error )
#define setWord( setting, value )           DBWriteContactSettingWord( NULL, m_szModuleName, setting, value )
#define getDword( setting, error )          DBGetContactSettingDword( NULL, m_szModuleName, setting, error )
#define setDword( setting, value )          DBWriteContactSettingDword( NULL, m_szModuleName, setting, value )
#define getString( setting, dest )          DBGetContactSettingString( NULL, m_szModuleName, setting, dest )
#define setString( setting, value )         DBWriteContactSettingString( NULL, m_szModuleName, setting, value )
#define getTString( setting, dest )         DBGetContactSettingTString( NULL, m_szModuleName, setting, dest )
#define setTString( setting, value )        DBWriteContactSettingTString( NULL, m_szModuleName, setting, value )
#define getU8String( setting, dest )        DBGetContactSettingUTF8String( NULL, m_szModuleName, setting, dest )
#define setU8String( setting, value )       DBWriteContactSettingUTF8String( NULL, m_szModuleName, setting, value )
#define deleteSetting( setting )            DBDeleteContactSetting( NULL, m_szModuleName, setting )

// DB settings
#define OMEGLE_KEY_TIMEOUTS_LIMIT "TimeoutsLimit" // [HIDDEN]
#define OMEGLE_KEY_ASL			  "MessageAsl" // [HIDDEN]
#define OMEGLE_KEY_HI			  "MessageHi" // [HIDDEN]
#define OMEGLE_KEY_NAME			  "ChatName" // [HIDDEN]
