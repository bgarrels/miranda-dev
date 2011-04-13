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
//  Internal Database API
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"

void InitDB()
{
    //gProtocol.bUtfReadyDB = ServiceExists(MS_DB_CONTACT_GETSETTING_STR);
    //if (!gProtocol.bUtfReadyDB)
    //    NetLog_Server("Warning: DB module does not support Unicode.");
}

void CIcqProto::_CreateResidentSetting(const char* szSetting)
{
    char pszSetting[2*MAX_PATH];

    strcpy(pszSetting, ICQ_PROTOCOL_NAME);
    strcat(pszSetting, "/");
    strcat(pszSetting, szSetting);
    CallService(MS_DB_SETSETTINGRESIDENT, 1, (WPARAM)pszSetting);
}

BYTE CIcqProto::_getSettingByte(HANDLE hContact, const char* szSetting, BYTE bDef)
{
    return DBGetContactSettingByte(hContact, ICQ_PROTOCOL_NAME, szSetting, bDef);
}

WORD CIcqProto::_getSettingWord(HANDLE hContact, const char* szSetting, WORD wDef)
{
    return DBGetContactSettingWord(hContact, ICQ_PROTOCOL_NAME, szSetting, wDef);
}

DWORD CIcqProto::_getSettingDword(HANDLE hContact, const char* szSetting, DWORD dwDef)
{
    DBVARIANT dbv;
    DBCONTACTGETSETTING cgs;
    DWORD dwRes;

    cgs.szModule = ICQ_PROTOCOL_NAME;
    cgs.szSetting = szSetting;
    cgs.pValue = &dbv;
    if (CallService(MS_DB_CONTACT_GETSETTING,(WPARAM)hContact,(LPARAM)&cgs))
        return dwDef; // not found, give default

    if (dbv.type != DBVT_DWORD)
        dwRes = dwDef; // invalid type, give default
    else // found and valid, give result
        dwRes = dbv.dVal;

    ICQFreeVariant(&dbv);
    return dwRes;
}

double CIcqProto::_getSettingDouble(HANDLE hContact, const char *szSetting, double dDef)
{
    DBVARIANT dbv = {DBVT_DELETED};
    double dRes;

    if (_getSetting(hContact, szSetting, &dbv))
        return dDef; // not found, give default

    if (dbv.type != DBVT_BLOB || dbv.cpbVal != sizeof(double))
        dRes = dDef;
    else
        dRes = *(double*)dbv.pbVal;

    ICQFreeVariant(&dbv);
    return dRes;
}

DWORD CIcqProto::_getContactUin(HANDLE hContact)
{
    return _getSettingDword(hContact, UNIQUEIDSETTING, 0);
}

int CIcqProto::_getContactUid(HANDLE hContact, DWORD *pdwUin, uid_str* ppszUid)
{
    DBVARIANT dbv;
    int iRes = 1;

    *pdwUin = 0;
    if (ppszUid) *ppszUid[0] = '\0';

    if (!_getSetting(hContact, UNIQUEIDSETTING, &dbv))
    {
        if (dbv.type == DBVT_DWORD)
        {
            *pdwUin = dbv.dVal;
            iRes = 0;
        }
        else if (dbv.type == DBVT_ASCIIZ)
        {
            if (ppszUid && m_bAimEnabled)
            {
                strcpy(*ppszUid, dbv.pszVal);
                iRes = 0;
            }
            else
                NetLog_Server("AOL screennames not accepted");
        }
        ICQFreeVariant(&dbv);
    }
    return iRes;
}

int CIcqProto::_getSetting(HANDLE hContact, const char* szSetting, DBVARIANT *dbv)
{
    //if (bUtfReadyDB)	// FIXME
        return DBGetContactSettingW(hContact, ICQ_PROTOCOL_NAME, szSetting, dbv);
    //else
    //    return DBGetContactSetting(hContact, ICQ_PROTOCOL_NAME, szSetting, dbv);
}

int CIcqProto::_getSettingString(HANDLE hContact, const char* szSetting, DBVARIANT *dbv)
{
    //if (bUtfReadyDB)	// FIXME
        return DBGetContactSettingString(hContact, ICQ_PROTOCOL_NAME, szSetting, dbv);
    //else
    //    return _getSetting(hContact, szSetting, dbv);
}

char* CIcqProto::_getSettingStringUtf(HANDLE hContact, const char *szModule,const char* szSetting, char* szDef)
{
    DBVARIANT dbv = {DBVT_DELETED};
    char* szRes;

    //if (bUtfReadyDB)	// FIXME
    //{
        if (DBGetContactSettingTString(hContact, szModule, szSetting, &dbv))
            return null_strdup(szDef);

        szRes = mir_utf8encodeW(dbv.ptszVal);
        ICQFreeVariant(&dbv);
    //}
    //else
    //{
    //    // old DB, we need to convert the string to UTF-8
    //    if (DBGetContactSetting(hContact, szModule, szSetting, &dbv))
    //        return null_strdup(szDef);

    //    szRes = mir_utf8encode(dbv.pszVal);

    //    ICQFreeVariant(&dbv);
    //}
    return szRes;
}

char* CIcqProto::_getSettingStringUtf(HANDLE hContact, const char* szSetting, char* szDef)
{
    return _getSettingStringUtf(hContact, ICQ_PROTOCOL_NAME, szSetting, szDef);
}

WORD CIcqProto::_getContactStatus(HANDLE hContact)
{
    return _getSettingWord(hContact, "Status", ID_STATUS_OFFLINE);
}

// (c) by George Hazan
int CIcqProto::_getSettingStringStatic(HANDLE hContact, const char* valueName, char* dest, int dest_len)
{
    DBVARIANT dbv;
    DBCONTACTGETSETTING sVal;

    dbv.pszVal = dest;
    dbv.cchVal = dest_len;
    dbv.type = DBVT_ASCIIZ;

    sVal.pValue = &dbv;
    sVal.szModule = ICQ_PROTOCOL_NAME;
    sVal.szSetting = valueName;

    if (CallService(MS_DB_CONTACT_GETSETTINGSTATIC, (WPARAM)hContact, (LPARAM)&sVal) != 0)
    {
        dbv.pszVal = dest;
        dbv.cchVal = dest_len;
        dbv.type = DBVT_UTF8;

        if (CallService(MS_DB_CONTACT_GETSETTINGSTATIC, (WPARAM)hContact, (LPARAM)&sVal) != 0)
            return 1; // this is here due to DB module bug...
    }

    return (dbv.type != DBVT_ASCIIZ);
}

int CIcqProto::_deleteSetting(HANDLE hContact, const char* szSetting)
{
    return DBDeleteContactSetting(hContact, ICQ_PROTOCOL_NAME, szSetting);
}

int CIcqProto::_setSettingByte(HANDLE hContact, const char* szSetting, BYTE bValue)
{
    return DBWriteContactSettingByte(hContact, ICQ_PROTOCOL_NAME, szSetting, bValue);
}

int CIcqProto::_setSettingWord(HANDLE hContact, const char* szSetting, WORD wValue)
{
    return DBWriteContactSettingWord(hContact, ICQ_PROTOCOL_NAME, szSetting, wValue);
}

int CIcqProto::_setSettingDword(HANDLE hContact, const char* szSetting, DWORD dwValue)
{
    return DBWriteContactSettingDword(hContact, ICQ_PROTOCOL_NAME, szSetting, dwValue);
}

int CIcqProto::_setSettingDouble(HANDLE hContact, const char *szSetting, double dValue)
{
    return _setSettingBlob(hContact, szSetting, (BYTE*)&dValue, sizeof(double));
}

int CIcqProto::_setSettingString(HANDLE hContact, const char* szSetting, char* szValue)
{
    return DBWriteContactSettingString(hContact, ICQ_PROTOCOL_NAME, szSetting, szValue);
}

int CIcqProto::_setSettingStringUtf(HANDLE hContact, const char *szModule,const char* szSetting, char* szValue)
{
    TCHAR *tmp = mir_utf8decodeW(szValue);
    return DBWriteContactSettingTString(hContact, szModule, szSetting, tmp);
    mir_free(tmp);
}

int CIcqProto::_setSettingStringUtf(HANDLE hContact, const char* szSetting, char* szValue)
{
    return _setSettingStringUtf(hContact, ICQ_PROTOCOL_NAME, szSetting, szValue);
}

int CIcqProto::_setSettingBlob(HANDLE hContact,const char *szSetting, BYTE *val, const int cbVal)
{
    DBCONTACTWRITESETTING cws;

    cws.szModule=ICQ_PROTOCOL_NAME;
    cws.szSetting=szSetting;
    cws.value.type=DBVT_BLOB;
    cws.value.pbVal=val;
    cws.value.cpbVal = cbVal;
    return CallService(MS_DB_CONTACT_WRITESETTING, (WPARAM)hContact,(LPARAM)&cws);
}

int CIcqProto::_setContactHidden(HANDLE hContact, BYTE bHidden)
{
	int nResult = DBWriteContactSettingByte(hContact, "CList", "Hidden", bHidden);

	if (!bHidden) // clear zero setting
		DBDeleteContactSetting(hContact, "CList", "Hidden");

	return nResult;
}

void CIcqProto::_setStatusMsgVar(HANDLE hContact, char* szStatusMsg, bool isAnsi)
{
	if (szStatusMsg && szStatusMsg[0])
	{
		if (isAnsi)
		{
			char* szStatusNote = _getSettingStringUtf(hContact, DBSETTING_STATUS_NOTE, "");
			wchar_t* szStatusNoteW = make_unicode_string(szStatusNote);
			int len = (int)wcslen(szStatusNoteW) * 3 + 1;
			char* szStatusNoteAnsi = (char*)alloca(len);
			WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, szStatusNoteW, -1, szStatusNoteAnsi, len, NULL, NULL);
			bool notmatch = false;
			for (int i=0; ;++i)
			{
				if (szStatusNoteAnsi[i] != szStatusMsg[i] && szStatusNoteAnsi[i] != '?' && szStatusMsg[i] != '?')
				{
					notmatch = true;
					break;
				}
				if (!szStatusNoteAnsi[i] || !szStatusMsg[i])
					break;
			}
			szStatusMsg = notmatch ? ansi_to_utf8(szStatusMsg) : szStatusNote;
			SAFE_FREE(&szStatusNoteW);
			if (notmatch) SAFE_FREE(&szStatusNote);
		}

		char* oldStatusMsg = NULL;
		DBVARIANT dbv;
		if (!DBGetContactSetting(hContact, "CList", "StatusMsg", &dbv))
		{
			switch (dbv.type)
			{
			case DBVT_UTF8:
				oldStatusMsg = null_strdup(dbv.pszVal);
				break;

			case DBVT_WCHAR:
				oldStatusMsg = make_utf8_string(dbv.pwszVal);
				break;
			}
			ICQFreeVariant(&dbv);
		}

		if (!oldStatusMsg || strcmp(oldStatusMsg, szStatusMsg))
			_setSettingStringUtf(hContact, "CList", "StatusMsg", szStatusMsg);
		SAFE_FREE(&oldStatusMsg);
		if (isAnsi) SAFE_FREE(&szStatusMsg);
	}
	else
		DBDeleteContactSetting(hContact, "CList", "StatusMsg");
}

int CIcqProto::_IsICQContact(HANDLE hContact)
{
    char* szProto = (char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);

    return !strcmpnull(szProto, ICQ_PROTOCOL_NAME);
}

HANDLE CIcqProto::_AddEvent(HANDLE hContact, WORD wType, DWORD dwTime, DWORD flags, DWORD cbBlob, PBYTE pBlob)
{
    DBEVENTINFO dbei = {0};

    dbei.cbSize = sizeof(dbei);
    dbei.szModule = ICQ_PROTOCOL_NAME;
    dbei.timestamp = dwTime;
    dbei.flags = flags;
    dbei.eventType = wType;
    dbei.cbBlob = cbBlob;
    dbei.pBlob = pBlob;

    return (HANDLE)CallService(MS_DB_EVENT_ADD, (WPARAM)hContact, (LPARAM)&dbei);
}

HANDLE CIcqProto::_FindFirstContact()
{
    HANDLE hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, (LPARAM)ICQ_PROTOCOL_NAME);

    if (_IsICQContact(hContact))
        return hContact;

	return _FindNextContact(hContact);
}

HANDLE CIcqProto::_FindNextContact(HANDLE hContact)
{
    hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT,(WPARAM)hContact,(LPARAM)ICQ_PROTOCOL_NAME);

    while (hContact != NULL)
    {
        if (_IsICQContact(hContact))
        {
            return hContact;
        }
        hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT,(WPARAM)hContact,(LPARAM)ICQ_PROTOCOL_NAME);
    }
    return hContact;
}

char* CIcqProto::_getContactCListGroup(HANDLE hContact)
{
    return _getSettingStringUtf(hContact, "CList", "Group", NULL);
}

int ICQFreeVariant(DBVARIANT* dbv)
{
	return DBFreeVariant(dbv);
}
