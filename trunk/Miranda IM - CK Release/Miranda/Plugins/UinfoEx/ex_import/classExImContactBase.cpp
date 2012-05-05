/*
UserinfoEx plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2006-2010 DeathAxe, Yasnovidyashii, Merlin_de, K. Romanov, Kreol

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

#include "..\src\commonheaders.h"
#include "..\src\svc_contactinfo.h"
#include "classExImContactBase.h"
#include "mir_rfcCodecs.h"

HANDLE CListFindGroup(LPCSTR pszGroup)
{
	CHAR buf[4];
	BYTE i;
	DBVARIANT dbv;

	for (i = 0; i < 255; i++) {
		_itoa(i, buf, 10);
		if (DB::Setting::GetUString(NULL, "CListGroups", buf, &dbv))
			break;
		if (dbv.pszVal && pszGroup && !_stricmp(dbv.pszVal + 1, pszGroup)) {
			DB::Variant::Free(&dbv);
			return (HANDLE)(INT_PTR)(i+1);
		}
		DB::Variant::Free(&dbv);
	}
	return INVALID_HANDLE_VALUE;
}

/**
 * name:	CExImContactBase
 * class:	CExImContactBase
 * desc:	default constructor
 * param:	none
 * return:	nothing
 **/
CExImContactBase::CExImContactBase()
{
	_pszNick		= NULL;
	_pszDisp		= NULL;
	_pszGroup		= NULL;
	_pszProto		= NULL;
	_pszProtoOld	= NULL;
	_pszAMPro		= NULL;
	_pszUIDKey		= NULL;
	_dbvUIDHash		= NULL;
	ZeroMemory(&_dbvUID, sizeof(DBVARIANT));
	_hContact		= INVALID_HANDLE_VALUE;
	_isNewContact	= FALSE;
}

/**
 * name:	~CExImContactBase
 * class:	CExImContactBase
 * desc:	default denstructor
 * param:	none
 * return:	nothing
 **/
CExImContactBase::~CExImContactBase()
{
	MIR_FREE(_pszNick);
	MIR_FREE(_pszDisp);
	MIR_FREE(_pszGroup);
	MIR_FREE(_pszProtoOld);
	MIR_FREE(_pszProto);
	MIR_FREE(_pszAMPro);
	MIR_FREE(_pszUIDKey);
	DB::Variant::Free(&_dbvUID);
}

/**
 * name:	fromDB
 * class:	CExImContactBase
 * desc:	get contact information from database
 * param:	hContact	- handle to contact whose information to read
 * return:	TRUE if successful or FALSE otherwise
 **/
BOOLEAN CExImContactBase::fromDB(HANDLE hContact)
{
	BOOLEAN		ret			= FALSE;
	BOOLEAN		isChatRoom	= FALSE;
	LPSTR		pszProto;
	LPCSTR		uidSetting;
	DBVARIANT	dbv;
	
	_hContact				= hContact;
	_dbvUIDHash				= 0;
	MIR_FREE(_pszProtoOld);
	MIR_FREE(_pszProto);
	MIR_FREE(_pszAMPro);
	MIR_FREE(_pszNick);
	MIR_FREE(_pszDisp);
	MIR_FREE(_pszGroup);
	MIR_FREE(_pszUIDKey);
	DB::Variant::Free(&_dbvUID);
	ZeroMemory(&_dbvUID, sizeof(DBVARIANT));

	// OWNER
	if (!_hContact) return TRUE;
	
	// Proto
	if (!(pszProto = DB::Contact::Proto(_hContact))) return FALSE;
	_pszProto = mir_strdup(pszProto);

	// AM_BaseProto
	if (!DB::Setting::GetUString(NULL, pszProto, "AM_BaseProto", &dbv )) {
		_pszAMPro = mir_strdup(dbv.pszVal);
		DB::Variant::Free(&dbv);
	}

	// unique id (for ChatRoom)
	if (isChatRoom = DB::Setting::GetByte(_hContact, pszProto, "ChatRoom", 0)) {
		uidSetting = "ChatRoomID";
		_pszUIDKey = mir_strdup(uidSetting);
		if (!DB::Setting::GetAsIs(_hContact, pszProto, uidSetting, &_dbvUID)) {
			ret = TRUE;
		}
	}
	// unique id (normal)
	else {
		uidSetting = (LPCSTR)CallProtoService(pszProto, PS_GETCAPS, PFLAG_UNIQUEIDSETTING, 0);
		// valid
		if (uidSetting != NULL && (INT_PTR)uidSetting != CALLSERVICE_NOTFOUND) {
			_pszUIDKey = mir_strdup(uidSetting);
			if (!DB::Setting::GetAsIs(_hContact, pszProto, uidSetting, &_dbvUID)) {
				ret = TRUE;
			}
		}
		// fails because the protocol is no longer installed
		else {
			// assert(ret == TRUE);
			ret = TRUE;
		}
	}
	
	// nickname
	if (!DB::Setting::GetUString(_hContact, pszProto, SET_CONTACT_NICK, &dbv)) {
		_pszNick = mir_strdup(dbv.pszVal);
		DB::Variant::Free(&dbv);
	}

	if (_hContact && ret) {
	// Clist Group
		if (!DB::Setting::GetUString(_hContact, MOD_CLIST, "Group", &dbv)) {
			_pszGroup = mir_strdup(dbv.pszVal);
			DB::Variant::Free(&dbv);
		}
	// Clist DisplayName
		if (!DB::Setting::GetUString(_hContact, MOD_CLIST, SET_CONTACT_MYHANDLE, &dbv)) {
			_pszDisp = mir_strdup(dbv.pszVal);
			DB::Variant::Free(&dbv);
		}
	}
	return ret;
}

/**
 * name:	fromIni
 * class:	CExImContactBase
 * desc:	get contact information from a row of a ini file
 * param:	row	- the rows data
 * return:	TRUE if successful or FALSE otherwise
 **/
BOOLEAN CExImContactBase::fromIni(LPSTR& row)
{
	LPSTR p1, p2 = NULL;
	LPSTR pszUIDValue, pszUIDSetting, pszProto = NULL;
	LPSTR pszBuf = &row[0];
	size_t cchBuf = strlen(row);

	MIR_FREE(_pszProtoOld);
	MIR_FREE(_pszProto);
	MIR_FREE(_pszAMPro);
	MIR_FREE(_pszNick);
	MIR_FREE(_pszDisp);
	MIR_FREE(_pszGroup);
	MIR_FREE(_pszUIDKey);
	DB::Variant::Free(&_dbvUID);
	ZeroMemory(&_dbvUID, sizeof(DBVARIANT));
	_dbvUIDHash = 0;

	// read uid value
	if (cchBuf > 10 && (p1 = mir_strrchr(pszBuf, '*{')) && (p2 = mir_strchr(p1, '}*')) && p1 + 2 < p2) {
		pszUIDValue = p1 + 1;
		*p1 = *(p2 - 1) = 0;

		// insulate the uid setting from buffer pointer
		if (cchBuf > 0 && (p1 = mir_strrchr(pszBuf, '*<')) && (p2 = mir_strchr(p1, '>*')) && p1 + 2 < p2) {
			pszUIDSetting = p1 + 1;
			*p1 = *(p2 - 1) = 0;

			// insulate the protocol name from buffer pointer
			if (cchBuf > 0 && (p1 = mir_strrchr(pszBuf, '*(')) && (p2 = mir_strchr(p1, ')*')) && p1 + 2 < p2) {
				pszProto = p1 + 1;
				*(--p1) = *(p2 - 1) = 0;

				// DBVT_DWORD
				if (strspn(pszUIDValue, "0123456789") == mir_strlen(pszUIDValue)) {
					_dbvUID.dVal = _atoi64(pszUIDValue);
					_dbvUID.type = DBVT_DWORD;
				}
				else {
				// DBVT_UTF8
					_dbvUID.pszVal = mir_strdup(pszUIDValue);
					_dbvUID.type = DBVT_UTF8;
				}
				_pszUIDKey = mir_strdup(pszUIDSetting);
				_pszProto = mir_strdup(pszProto);
			} //end insulate the protocol name from buffer pointer
		} //end insulate the uid setting from buffer pointer
	} //end read uid value

	// create valid nickname
	_pszNick = mir_strdup(pszBuf);
	size_t i = strlen(_pszNick)-1;
	while (i > 0 && (_pszNick[i] == ' ' || _pszNick[i] == '\t')) {
		_pszNick[i] = 0;
		i--;
	}
	// finally try to find contact in contact list
	findHandle();
	return FALSE;
}

/**
 * name:	toDB
 * class:	CExImContactBase
 * desc:	searches the database for a contact representing the one
 *			identified by this class or creates a new one if it was not found
 * param:	hMetaContact - a meta contact to add this contact to
 * return:	handle of the contact if successful
 **/
HANDLE CExImContactBase::toDB()
{
	DBCONTACTWRITESETTING cws;
	
	// create new contact if none exists
	if (_hContact == INVALID_HANDLE_VALUE && _pszProto && _pszUIDKey && _dbvUID.type != DBVT_DELETED) {
		PROTOACCOUNT* pszAccount = 0;
		if(NULL == (pszAccount = ProtoGetAccount( _pszProto ))) {
			//account does not exist
			_hContact = INVALID_HANDLE_VALUE;
			return INVALID_HANDLE_VALUE;
		}
		if(!IsAccountEnabled(pszAccount)) {
			;
		}
		// create new contact
		_hContact = DB::Contact::Add();
		if (!_hContact) {
			_hContact = INVALID_HANDLE_VALUE;
			return INVALID_HANDLE_VALUE;
		}
		// Add the protocol to the new contact
		if (CallService(MS_PROTO_ADDTOCONTACT, (WPARAM)_hContact, (LPARAM)_pszProto)) {
			DB::Contact::Delete(_hContact);
			_hContact = INVALID_HANDLE_VALUE;
			return INVALID_HANDLE_VALUE;
		}
		// write uid to protocol module
		cws.szModule	= _pszProto;
		cws.szSetting	= _pszUIDKey;
		cws.value		= _dbvUID;
		if (CallService(MS_DB_CONTACT_WRITESETTING, (WPARAM)_hContact, (LPARAM)&cws)) {
			DB::Contact::Delete(_hContact);
			_hContact = INVALID_HANDLE_VALUE;
			return INVALID_HANDLE_VALUE;
		}
		// write nick and display name
		if (_pszNick) DB::Setting::WriteUString(_hContact, _pszProto, SET_CONTACT_NICK, _pszNick);
		if (_pszDisp) DB::Setting::WriteUString(_hContact, MOD_CLIST, SET_CONTACT_MYHANDLE, _pszDisp);

		// add group
		if (_pszGroup) {
			DB::Setting::WriteUString(_hContact, MOD_CLIST, "Group", _pszGroup);
			if (CListFindGroup(_pszGroup) == INVALID_HANDLE_VALUE) {
				WPARAM hGroup = (WPARAM)CallService(MS_CLIST_GROUPCREATE, 0, 0);
				if (hGroup) {
					// renaming twice is stupid but the only way to avoid error dialog telling shit like
					// a group with that name does exist
					CallService(MS_CLIST_GROUPRENAME, (WPARAM)hGroup, (LPARAM)_pszGroup);
				}
			}
		}
	}
	return _hContact;
}

/**
 * name:	toIni
 * class:	CExImContactBase
 * desc:	writes the line to an opened ini file which identifies the contact
 *			whose information are stored in this class
 * param:	file	- pointer to the opened file
 * return:	nothing
 **/
VOID CExImContactBase::toIni(FILE* file, int modCount)
{
	// getting dbeditor++ NickFromHContact(hContact)
	static char name[512] = "";
	char* ret = 0;

	if (_hContact){
		int loaded = _pszUIDKey ? 1 : 0;
		if (_pszProto == NULL || !loaded) {
			if (_pszProto){
				if (_pszNick)
					mir_snprintf(name, sizeof(name),"%s (%s)", _pszNick, _pszProto);
				else
					mir_snprintf(name, sizeof(name),"(UNKNOWN) (%s)", _pszProto);
			}
			else
				mir_snprintf(name, sizeof(name),"(UNKNOWN)");
		}
		else {
			// Proto loadet - GetContactName(hContact,pszProto,0)
			LPSTR pszCI	= NULL;
			CONTACTINFO ci;
			ZeroMemory(&ci, sizeof(ci));

			ci.cbSize		= sizeof(ci);
			ci.hContact		= _hContact;
			ci.szProto		= _pszProto;
			ci.dwFlag		= CNF_DISPLAY;

			if (!GetContactInfo(NULL, (LPARAM) &ci)){
				// CNF_DISPLAY always returns a string type
				pszCI = (LPSTR)ci.pszVal;
			}
			LPSTR pszUID = uid2String(FALSE);
			if (_pszUIDKey && pszUID)
				mir_snprintf(name, sizeof(name), "%s *(%s)*<%s>*{%s}*", pszCI, _pszProto, _pszUIDKey, pszUID);
			else 
				mir_snprintf(name, sizeof(name), "%s (%s)", pszCI, _pszProto);

			mir_free(pszCI);
			mir_free(pszUID);
		} // end else (Proto loadet)

		// it is not the best solution (but still works if only basic modules export) - need rework
		if (modCount > 3)
			fprintf(file, "CONTACT: %s\n", name);
		else
			fprintf(file, "FROM CONTACT: %s\n", name);

	} // end *if (_hContact)
	else {
		fprintf(file, "SETTINGS:\n");
	}
}

BOOLEAN CExImContactBase::compareUID(DBVARIANT *dbv)
{
	DWORD hash = 0;
	switch (dbv->type) {
		case DBVT_BYTE:
			if (dbv->bVal == _dbvUID.bVal) {
				_dbvUID.type = dbv->type;
				return TRUE;
			}
			break;
		case DBVT_WORD:
			if (dbv->wVal == _dbvUID.wVal) {
				_dbvUID.type = dbv->type;
				return TRUE;
			}
			break;
		case DBVT_DWORD:
			if (dbv->dVal == _dbvUID.dVal) {
				_dbvUID.type = dbv->type;
				return TRUE;
			}
			break;
		case DBVT_ASCIIZ:
			hash = hashSetting_M2(dbv->pszVal);
		case DBVT_WCHAR:
			if(!hash) hash = hashSettingW_M2((const char *)dbv->pwszVal);
		case DBVT_UTF8:
			if(!hash) {
				LPWSTR tmp = mir_utf8decodeW(dbv->pszVal);
				hash = hashSettingW_M2((const char *)tmp);
				mir_free(tmp);
			}
			if(hash == _dbvUIDHash) return TRUE;
			break;
		case DBVT_BLOB:		//'n' cpbVal and pbVal are valid
			if (_dbvUID.type == dbv->type && 
				_dbvUID.cpbVal == dbv->cpbVal &&
				memcmp(_dbvUID.pbVal, dbv->pbVal, dbv->cpbVal) == 0) {
				return TRUE;
			}
			break;
		default:
			return FALSE;
	}
	return FALSE;
}

LPSTR CExImContactBase::uid2String(BOOLEAN bPrependType)
{
	CHAR szUID[MAX_PATH];
	LPSTR ptr = szUID;

	switch (_dbvUID.type) {
		case DBVT_BYTE:		//'b' bVal and cVal are valid
			if (bPrependType) *ptr++ = 'b';
			_itoa(_dbvUID.bVal, ptr, 10);
			break;
		case DBVT_WORD:		//'w' wVal and sVal are valid
			if (bPrependType) *ptr++ = 'w';
			_itoa(_dbvUID.wVal, ptr, 10);
			break;
		case DBVT_DWORD:	//'d' dVal and lVal are valid
			if (bPrependType) *ptr++ = 'd';
			_itoa(_dbvUID.dVal, ptr, 10);
			break;
		case DBVT_WCHAR:	//'u' pwszVal is valid
		{
			LPSTR r = mir_utf8encodeW(_dbvUID.pwszVal);
			if (bPrependType) {
				LPSTR t = (LPSTR)mir_alloc(strlen(r)+2);
				t[0] = 'u';
				strcpy(t+1, r);
				mir_free(r);
				r = t;
			}
			return r;
		}
		case DBVT_UTF8:		//'u' pszVal is valid
			{
				if (bPrependType) *ptr++ = 'u';
				mir_strncpy(ptr, _dbvUID.pszVal, SIZEOF(szUID)-1);
			}
			break;
		case DBVT_ASCIIZ: {
			LPSTR r = mir_utf8encode(_dbvUID.pszVal);
			if (bPrependType) {
				LPSTR t = (LPSTR)mir_alloc(strlen(r)+2);
				t[0] = 's';
				strcpy(t+1, r);
				mir_free(r);
				r = t;
			}
			return r;
		}
		case DBVT_BLOB:		//'n' cpbVal and pbVal are valid
		{
			if (bPrependType) {		//True = XML
				INT_PTR baselen = Base64EncodeGetRequiredLength(_dbvUID.cpbVal, BASE64_FLAG_NOCRLF);
				LPSTR t = (LPSTR)mir_alloc(baselen + 5 + bPrependType);
				assert(t != NULL);
				if (Base64Encode(_dbvUID.pbVal, _dbvUID.cpbVal, t + bPrependType, &baselen, BASE64_FLAG_NOCRLF)){
					if (baselen){
						t[baselen + bPrependType] = 0;
						if (bPrependType) t[0] = 'n';
						return t;
					}
				}
				mir_free(t);
				return NULL;
			}
			else {		//FALSE = INI
				WORD j;
				INT_PTR baselen = (_dbvUID.cpbVal * 3);
				LPSTR t = (LPSTR)mir_alloc(3 + baselen);
				ZeroMemory(t, (3 + baselen));
				for (j = 0; j < _dbvUID.cpbVal; j++) {
					mir_snprintf((t + bPrependType + (j * 3)), 4,"%02X ", (BYTE)_dbvUID.pbVal[j]);
				}
				if (t){
					if (bPrependType) t[0] = 'n';
					return t;
				}
				else {
					mir_free(t);
					return NULL;
				}
			}
			break;
		}
		default:
			return NULL;
	}
	return mir_strdup(szUID);
}

BOOLEAN CExImContactBase::isMeta() const
{
	return DB::Module::IsMeta(_pszProto);
}

BOOLEAN CExImContactBase::isHandle(HANDLE hContact)
{
	LPCSTR pszProto;
	DBVARIANT dbv;
	BOOLEAN isEqual = FALSE;

	// owner contact ?
	if (!_pszProto) return hContact == NULL;

	// compare protocols
	pszProto = DB::Contact::Proto(hContact);
	if (pszProto == NULL || (INT_PTR)pszProto == CALLSERVICE_NOTFOUND || mir_stricmp(pszProto, _pszProto))
		return FALSE;

	// compare uids
	if (_pszUIDKey) {
		// get uid
		if (DB::Setting::GetAsIs(hContact, pszProto,_pszUIDKey, &dbv))
			return FALSE;

		isEqual = compareUID (&dbv);
		DB::Variant::Free(&dbv);
	}
	// compare nicknames if no UID
	else if (!DB::Setting::GetUString(hContact, _pszProto, SET_CONTACT_NICK, &dbv)) {
		if (dbv.type == DBVT_UTF8 && dbv.pszVal && !mir_stricmp(dbv.pszVal,_pszNick)) {
			LPTSTR ptszNick = mir_utf8decodeT(_pszNick);
			LPTSTR ptszProto = mir_a2t(_pszProto);
			INT ans = MsgBox(NULL, MB_ICONQUESTION|MB_YESNO, LPGENT("Question"), LPGENT("contact identificaion"),
				LPGENT("The contact %s(%s) has no unique id in the vCard,\nbut there is a contact in your clist with the same nick and protocol.\nDo you wish to use this contact?"),
				ptszNick, ptszProto);
			MIR_FREE(ptszNick);
			MIR_FREE(ptszProto);
			isEqual = ans == IDYES;
		}
		DB::Variant::Free(&dbv);
	}
	return isEqual;
}

/**
 * name:	handle
 * class:	CExImContactBase
 * desc:	return the handle of the contact
 *			whose information are stored in this class
 * param:	none
 * return:	handle if successful, INVALID_HANDLE_VALUE otherwise
 **/
HANDLE CExImContactBase::findHandle()
{
	HANDLE hContact;

	for (hContact = DB::Contact::FindFirst();
		hContact != NULL;
		hContact  = DB::Contact::FindNext(hContact))
	{
		if (isHandle(hContact)) {
			_hContact = hContact;
			_isNewContact = FALSE;
			return hContact;
		}
	}
	_isNewContact = TRUE;
	_hContact = INVALID_HANDLE_VALUE;
	return INVALID_HANDLE_VALUE;
}
