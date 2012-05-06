/*
Stopspam for 
Miranda IM: the free IM client for Microsoft* Windows*

Copyright 2000-2012 Miranda IM project,
all portions of this codebase are copyrighted to the people
listed in contributors.txt.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

===============================================================================

File name      : $HeadURL: 
Revision       : $Revision: 
Last change on : $Date: 
Last change by : $Author:
$Id$		   : $Id$:

===============================================================================
*/

#define pluginName "StopSpam" //const char* ?
/*
TCHAR const * defAnswer = _T("nospam");
TCHAR const * defCongratulation = 
_T("Congratulations! You just passed human/robot test. Now you can write me a message.");
char const * defProtoList = "ICQ\r\n";
TCHAR const * infTalkProtPrefix = _T("StopSpam automatic message:\r\n");
char const * answeredSetting = "Answered";
char const * questCountSetting = "QuestionCount";
TCHAR const * defAufrepl = _T("StopSpam: send a message and reply to a anti-spam bot question.");*/


#ifdef _UNICODE
typedef std::wstring tstring;
#define PREF_TCHAR2 PREF_UTF
#else
typedef std::string tstring;
#define PREF_TCHAR2 0
#endif //_UNICODE

