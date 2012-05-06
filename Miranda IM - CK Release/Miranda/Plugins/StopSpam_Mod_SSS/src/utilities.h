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

tstring DBGetContactSettingStringPAN(HANDLE hContact, char const * szModule, char const * szSetting, tstring errorValue);
std::string DBGetContactSettingStringPAN_A(HANDLE hContact, char const * szModule, char const * szSetting, std::string errorValue);
tstring &GetDlgItemString(HWND hwnd, int id);
std::string &GetProtoList();
bool ProtoInList(std::string proto);
void RemoveExcludedUsers();
tstring variables_parse(tstring const &tstrFormat, HANDLE hContact);
const int Stricmp(const TCHAR *str, const TCHAR *substr);
//const int Stristr(const TCHAR *str, const TCHAR *substr);
TCHAR* ReqGetText(DBEVENTINFO* dbei);
BOOL IsUrlContains(TCHAR * Str);
void DeleteCListGroupsByName(TCHAR* szGroupName);
tstring GetContactUid(HANDLE hContact, std::string Protocol);
void LogSpamToFile(HANDLE hContact, tstring message);
std::string toUTF8(std::wstring str);
std::string toUTF8(std::string str);
std::wstring toUTF16(std::string str);
void HistoryLogFunc(HANDLE hContact, std::string message);
std::string get_random_num(int length);