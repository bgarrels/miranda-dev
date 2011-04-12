// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin ÷berg, Sam Kothari, Robert Rainwater
// Copyright © 2004,2005,2006,2007 Joe Kucera
// Copyright © 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// -----------------------------------------------------------------------------
//
// File name      : $Source$
// Revision       : $Revision: 36 $
// Last change on : $Date: 2007-08-05 03:45:10 +0300 (–í—Å, 05 –∞–≤–≥ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#ifndef __ICQ_DB_H
#define __ICQ_DB_H

void InitDB();

HANDLE	AddEvent(HANDLE hContact, WORD wType, DWORD dwTime, DWORD flags, DWORD cbBlob, PBYTE pBlob);
void	CreateResidentSetting(const char* szSetting);
HANDLE	FindFirstContact();
HANDLE	FindNextContact(HANDLE hContact);
int		IsICQContact(HANDLE hContact);

int		getSetting(HANDLE hContact, const char* szSetting, DBVARIANT *dbv);
BYTE	getSettingByte(HANDLE hContact, const char* szSetting, BYTE bDef);
WORD	getSettingWord(HANDLE hContact, const char* szSetting, WORD wDef);
DWORD	getSettingDword(HANDLE hContact, const char* szSetting, DWORD dwDef);
double	getSettingDouble(HANDLE hContact, const char *szSetting, double dDef);
int		getSettingString(HANDLE hContact, const char* szSetting, DBVARIANT *dbv);
//int		getSettingStringW(HANDLE hContact, const char *szSetting, DBVARIANT *dbv);
int		getSettingStringStatic(HANDLE hContact, const char* valueName, char* dest, int dest_len);
char*	getSettingStringUtf(HANDLE hContact, const char* szSetting, char* szDef);
char*	getSettingStringUtf(HANDLE hContact, const char *szModule,const char* szSetting, char* szDef);
int		getContactUid(HANDLE hContact, DWORD *pdwUin, uid_str* ppszUid);
DWORD	getContactUin(HANDLE hContact);
WORD	getContactStatus(HANDLE hContact);
char*	getContactCListGroup(HANDLE hContact);

int		deleteSetting(HANDLE hContact, const char* szSetting);

int		setSettingByte(HANDLE hContact, const char* szSetting, BYTE bValue);
int		setSettingWord(HANDLE hContact, const char* szSetting, WORD wValue);
int		setSettingDword(HANDLE hContact, const char* szSetting, DWORD dwValue);
int		setSettingDouble(HANDLE hContact, const char *szSetting, double dValue);
int		setSettingString(HANDLE hContact, const char* szSetting, char* szValue);
//int		setSettingStringW(HANDLE hContact, const char *szSetting, const WCHAR *wszValue);
int		setSettingStringUtf(HANDLE hContact, const char* szSetting, char* szValue);
int		setSettingStringUtf(HANDLE hContact, const char *szModule,const char* szSetting, char* szValue);
int		setSettingBlob(HANDLE hContact,const char *szSetting, BYTE *val, const int cbVal);

int		setContactHidden(HANDLE hContact, BYTE bHidden);
void	setStatusMsgVar(HANDLE hContact, char* szStatusMsg, bool isAnsi);

int ICQFreeVariant(DBVARIANT* dbv);
int ICQSetContactCListGroup(HANDLE hContact, const char *szGroup);


#endif /* __ICQ_DB_H */
