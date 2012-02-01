/*
UserinfoEx plugin for Miranda IM

Copyright:
� 2006-2010 DeathAxe, Yasnovidyashii, Merlin, K. Romanov, Kreol

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

===============================================================================

File name      : $HeadURL: http://userinfoex.googlecode.com/svn/trunk/mir_db.cpp $
Revision       : $Revision: 210 $
Last change on : $Date: 2010-10-02 22:27:36 +0400 (Сб, 02 окт 2010) $
Last change by : $Author: ing.u.horn $

===============================================================================
*/

#include "commonheaders.h"
#include <m_metacontacts.h>
#include "ctrl_base.h"
#include "mir_string.h"
#include "mir_db.h"

namespace DB {

namespace MetaContact {

/**
 *
 *
 **/
INT_PTR	SubCount(HANDLE hMetaContact)
{
	INT_PTR result = CallService(MS_MC_GETNUMCONTACTS, (WPARAM) hMetaContact, 0);
	return (result == CALLSERVICE_NOTFOUND) ? -1 : result;
}

/**
 *
 *
 **/
INT_PTR	SubDefNum(HANDLE hMetaContact)
{
	INT_PTR result = CallService(MS_MC_GETDEFAULTCONTACTNUM, (WPARAM) hMetaContact, 0);
	return (result == CALLSERVICE_NOTFOUND) ? -1 : result;
}

/**
 *
 *
 **/
HANDLE	Sub(HANDLE hMetaContact, INT idx)
{
	if (idx != -1) {
		INT_PTR result = CallService(MS_MC_GETSUBCONTACT, (WPARAM) hMetaContact, (LPARAM) idx);
		return (result == CALLSERVICE_NOTFOUND) ? NULL : (HANDLE) result;
	}
	return NULL;
}

/**
 *
 *
 **/
BOOLEAN	IsSub(HANDLE hContact)
{
	return	myGlobals.szMetaProto &&
			DB::Setting::GetByte(myGlobals.szMetaProto, "Enabled", TRUE) &&
			DB::Setting::GetByte(hContact, myGlobals.szMetaProto, "IsSubcontact", FALSE);
}

/**
 *
 *
 **/
HANDLE	GetMeta(HANDLE hContact)
{
	HANDLE result;
	if (myGlobals.szMetaProto){
		result = (HANDLE)CallService(MS_MC_GETMETACONTACT, (WPARAM) hContact, 0);
		if (result == (HANDLE)CALLSERVICE_NOTFOUND) {
			result = NULL;
		}
	}
	else {
		result = NULL;
	}
	return (HANDLE) result;
}

} /* namespace MetaContact */

/**
* This namespace contains all functions used to access or modify contacts in the database.
**/
namespace Contact {

/**
 * This function retrieves the display name for a contact.
 * @param	hContact	- handle to the contact
 * @return	Returns the display name of a contact.
 **/
LPTSTR	DisplayName(HANDLE hContact)
{
	return (LPTSTR) CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM) hContact, GCDNF_TCHAR);
}

/**
 * This function is used to retrieve a contact's basic protocol
 * @param	hContact	- handle to the contact
 * @return	This function returns the basic protocol of a contact.
 **/
LPSTR	Proto(HANDLE hContact)
{
	if (hContact) {
		INT_PTR result;
		result = CallService(MS_PROTO_GETCONTACTBASEACCOUNT, (WPARAM) hContact, NULL);
		return (LPSTR) ((result == CALLSERVICE_NOTFOUND) ? NULL : result);
	}
	return NULL;
}

/**
 * Gets the number of contacts in the database, which does not count the user
 * @param	hContact	- handle to the contact
 * @return	Returns the number of contacts. They can be retrieved using
 *			contact/findfirst and contact/findnext
 **/
INT_PTR	GetCount()
{
	return CallService(MS_DB_CONTACT_GETCOUNT, 0, 0);
}

/**
 * This function searches the first contact in the database and returns its handle.
 * @retval	HANDLE		- handle of the next contact in the database	
 * @retval	NULL		- no more contacts in the database
 **/
HANDLE	FindFirst()
{
	return (HANDLE) CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
}

/**
 * This function searches the next contact in the database and returns its handle.
 * @param	hContact	- handle to the contact
 * @retval	HANDLE		- handle of the next contact in the database
 * @retval	NULL		- no more contacts in the database
 **/
HANDLE	FindNext(HANDLE hContact)
{
	return (HANDLE) CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM) hContact, 0);
}

/**
 * Simply adds a new contact without setting up any protocol or something else
 * @return	HANDLE		The function returns the HANDLE of the new contact
 **/
HANDLE	Add()
{
	return (HANDLE) CallService(MS_DB_CONTACT_ADD, 0, 0);
}

/**
 * This function deletes a contact from the database.
 * @param	hContact	- handle to the contact
 **/
BYTE	Delete(HANDLE hContact)
{
	return CallService(MS_DB_CONTACT_DELETE, (WPARAM) hContact, 0) != 0;
}

/**
 * This function trys to guess, when an ICQ contact was added to database.
 **/
DWORD	WhenAdded(DWORD dwUIN, LPCSTR pszProto)
{
	DBEVENTINFO		dbei; 
	HANDLE			edbe;
	DWORD			dwEvtUIN;

	ZeroMemory(&dbei, sizeof(dbei));
	dbei.cbSize = sizeof(dbei);
	for (edbe = DB::Event::FindFirst(NULL); edbe != NULL; edbe = DB::Event::FindNext(edbe)) {
		// get eventtype and compare
		if (!DB::Event::GetInfo(edbe, &dbei) && dbei.eventType == EVENTTYPE_ADDED) {
			if (!DB::Event::GetInfoWithData(edbe, &dbei)) {
				// extract UIN and compare with given one
				CopyMemory(&dwEvtUIN, dbei.pBlob, sizeof(DWORD));
				MIR_FREE(dbei.pBlob);
				if (dwEvtUIN == dwUIN) {
					return dbei.timestamp;
				}
			}
		}
	}
	return 0;
}

} /* Contact */

namespace Module {

/**
 * Deletes all settings in the module.
 * @param	hContact	- handle to the contact
 * @param	pszModule	- the module to delete the setting from (e.g. USERINFO)
 * return:	nothing
 **/
VOID	Delete(HANDLE hContact, LPCSTR pszModule)
{
	CEnumList	Settings;
	if (!Settings.EnumSettings(hContact, pszModule)) {
		INT i;
		for (i = 0; i < Settings.getCount(); i++) {
			DB::Setting::Delete(hContact, pszModule, Settings[i]);
		}
	}
}

/**
 * Enum Proc for DBModule_IsEmpty
 * @param	pszSetting	- the setting
 * @param	lParam		- DBCONTACTENUMSETTINGS - (LPARAM)&dbces
 * @retval	TRUE		- always true
 **/
static	INT IsEmptyEnumProc(LPCSTR pszSetting, LPARAM lParam)
{
	return 1;
}

/**
 * This function tests, whether a module is empty for the given contact or not
 * @param	hContact	- handle to the contact
 * @param	pszModule	- the module to read the setting from (e.g. USERINFO)
 * @retval	TRUE		- the module is empty
 * @retval	FALSE		- the module contains settings
 **/
BOOLEAN	IsEmpty(HANDLE hContact, LPCSTR pszModule)
{
	DBCONTACTENUMSETTINGS dbces;
	dbces.pfnEnumProc	= IsEmptyEnumProc;
	dbces.szModule		= pszModule;
	dbces.ofsSettings	= 0;
	dbces.lParam		= 0;
	return (0 > CallService(MS_DB_CONTACT_ENUMSETTINGS, (WPARAM)hContact, (LPARAM)&dbces));
}

/**
 * This function tests, whether a module belongs to a metacontact protocol
 * @param	pszModule	- the module to read the setting from (e.g. USERINFO)
 * @retval	TRUE		- the module belongs to a metacontact protocol
 * @retval	FALSE		- the module belongs to a other protocol
 **/
BOOLEAN	IsMeta(LPCSTR pszModule)
{
	if(myGlobals.szMetaProto)
		return !mir_strcmp(pszModule, myGlobals.szMetaProto);
	return !mir_strcmp(pszModule, "MetaContacts");
}

/**
 * This function tests, whether a module is a meta contact, and user wants to scan it for settings
 * @param	pszModule	- the module to read the setting from (e.g. USERINFO)
 * @retval	TRUE		- the module is empty
 * @retval	FALSE		- the module contains settings
 **/
BOOLEAN	IsMetaAndScan	(LPCSTR pszModule)
{
	return DB::Setting::GetByte(SET_META_SCAN, TRUE) && IsMeta(pszModule);
}

} /* namespace Module */

namespace Setting {

/**
 * This function calls MS_DB_CONTACT_GETSETTING_STR service to get database values. 
 * @param	hContact	- handle to the contact
 * @param	pszModule	- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting	- the setting to read
 * @param	destType	- desired string type (DBVT_ASCIIZ, DBVT_WCHAR, DBVT_UTF8)
 *
 * @retval	0 - success
 * @retval	1 - error
 **/
BYTE	Get(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting, DBVARIANT *dbv, const BYTE destType)
{
	BYTE result;
	DBCONTACTGETSETTING dgs;

	dgs.szModule	= pszModule;
	dgs.szSetting	= pszSetting;
	dgs.pValue		= dbv;
	dbv->type		= 0;
	
	// read value without translation to specific type
	result = CallService(MS_DB_CONTACT_GETSETTING_STR, (WPARAM) hContact, (LPARAM) &dgs) != 0;

	// Is value read successfully and destination type set?
	if (!result && destType) {
		result = DB::Variant::ConvertString(dbv, destType);
	}
	return result;
}

/**
 * This function reads a value from the database and returns it as an ansi encoded string.
 * @param	hContact	- handle to the contact
 * @param	pszModule	- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting	- the setting to read
 *
 * @return	string value
 **/
LPSTR	GetAString(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting)
{
	DBVARIANT dbv;
	if (GetAString(hContact, pszModule, pszSetting, &dbv) == 0){
		if (DB::Variant::dbv2String(&dbv, DBVT_WCHAR) == 0) {
			return dbv.pszVal;
		}
		DB::Variant::Free(&dbv);
	}
	return NULL;
}

/**
 * This function reads a value from the database and returns it as an unicode encoded string. 
 * @param	hContact	- handle to the contact
 * @param	pszModule	- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting	- the setting to read
 *
 * @return	string value
 **/
LPWSTR	GetWString(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting)
{
	DBVARIANT dbv;
	if (GetWString(hContact, pszModule, pszSetting, &dbv) == 0) {
		if (DB::Variant::dbv2String(&dbv, DBVT_WCHAR) == 0) {
			return dbv.pwszVal;
		}
		DB::Variant::Free(&dbv);
	}
	return NULL;
}

/**
 * This function calls MS_DB_CONTACT_GETSETTING_STR service to get database values. 
 * It searches in pszModule first and if the setting does not exist there it tries proto to retrieve it.
 * @param	hContact	- handle to the contact
 * @param	pszModule	- the module to read the setting from (e.g. USERINFO)
 * @param	szProto		- the contact's protocol to read the setting from (e.g. ICQ)
 * @param	szSetting	- the setting to read
 * @param	destType	- desired string type (DBVT_ASCIIZ, DBVT_WCHAR, DBVT_UTF8)
 *
 * @retval	0 - success
 * @retval	1 - error
 **/
BYTE	GetEx(HANDLE hContact, LPCSTR pszModule, LPCSTR pszProto, LPCSTR pszSetting, DBVARIANT *dbv, const BYTE destType)
{
	BYTE result;
	result = !pszModule || Get(hContact, pszModule, pszSetting, dbv, destType);
	// try to read setting from the contact's protocol module 
	if (result && pszProto) {
		result = Get(hContact, pszProto, pszSetting, dbv, destType) != 0;
		// try to get setting from a metasubcontact
		if (result && DB::Module::IsMetaAndScan(pszProto)) {
			const INT_PTR def = DB::MetaContact::SubDefNum(hContact);
			HANDLE hSubContact;
			// try to get setting from the default subcontact first
			if (def > -1 && def < INT_MAX) {
				hSubContact = DB::MetaContact::Sub(hContact, def);
				if (hSubContact != NULL) {
					result = DB::Setting::GetEx(hSubContact, pszModule, DB::Contact::Proto(hSubContact), pszSetting, dbv, destType) != 0;
				}
			}
			// scan all subcontacts for the setting
			if (result) {
				const INT_PTR cnt = DB::MetaContact::SubCount(hContact);
				if (cnt < INT_MAX) {
					INT_PTR i;
					for (i = 0; result && i < cnt; i++) {
						if (i != def) {
							hSubContact = DB::MetaContact::Sub(hContact, i);
							if (hSubContact != NULL) {
								result = DB::Setting::GetEx(hSubContact, pszModule, DB::Contact::Proto(hSubContact), pszSetting, dbv, destType) != 0;
	}	}	}	}	}	}	}
	return result;
}

/**
 * This function is used by the controls of the details dialog and calls MS_DB_CONTACT_GETSETTING_STR service
 * to get database values. It searches in pszModule first and if the setting does not exist there it tries proto
 * to retrieve it.
 * @param	hContact		- handle to the contact
 * @param	pszModule		- the module to read the setting from (e.g. USERINFO)
 * @param	pszSubModule	- the module to read the setting from a meta subcontract (e.g. USERINFO)
 * @param	pszProto		- the contact's protocol to read the setting from (e.g. ICQ)
 * @param	pszSetting		- the setting to read
 * @param	destType		- desired string type (DBVT_ASCIIZ, DBVT_WCHAR, DBVT_UTF8)
 *
 * @return	This function returns the WORD which contains the source of information.
 **/
WORD	GetCtrl(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSubModule, LPCSTR pszProto, LPCSTR pszSetting, DBVARIANT *dbv, const BYTE destType)
{
	WORD wFlags = 0;

	// read setting from given module
	if (hContact && pszModule && *pszModule && !Get(hContact, pszModule, pszSetting, dbv, destType)) {
		wFlags |= CTRLF_HASCUSTOM;
		if (Exists(hContact, pszProto, pszSetting)) {
			wFlags |= CTRLF_HASPROTO;
		}
	}
	// read setting from contact's basic protocol
	else if (pszProto && *pszProto) {
		// try to read the setting from the basic protocol
		if (!Get(hContact, pszProto, pszSetting, dbv, destType)) {
			wFlags |= CTRLF_HASPROTO;
		}
		// try to read the setting from the sub contacts' modules
		else if (DB::Module::IsMetaAndScan(pszProto)) {
			const INT_PTR def = DB::MetaContact::SubDefNum(hContact);
			HANDLE hSubContact;
			// try to get setting from the default subcontact first
			if (def > -1 && def < INT_MAX) {
				hSubContact = DB::MetaContact::Sub(hContact, def);
				if (hSubContact != NULL) {
					wFlags = GetCtrl(hSubContact, pszSubModule, NULL, DB::Contact::Proto(hSubContact), pszSetting, dbv, destType);
					if (wFlags != 0) {
						wFlags &= ~CTRLF_HASCUSTOM;
						wFlags |= CTRLF_HASMETA;
					}
				}
			}
			// copy the missing settings from the other subcontacts
			if (wFlags == 0) {
				INT_PTR i;
				const INT_PTR cnt = DB::MetaContact::SubCount(hContact);
				for (i = 0; i < cnt; i++) {
					if (i != def) {
						hSubContact = DB::MetaContact::Sub(hContact, i);
						if (hSubContact != NULL) {
							wFlags = GetCtrl(hSubContact, pszSubModule, NULL, DB::Contact::Proto(hSubContact), pszSetting, dbv, destType);
							if (wFlags != 0) {
								wFlags &= ~CTRLF_HASCUSTOM;
								wFlags |= CTRLF_HASMETA;
								break;
	}	}	}	}	}	}	}
	if (wFlags == 0) {
		dbv->type = DBVT_DELETED;
	}
	return wFlags;
}

/**
 * This function reads a setting from database into a predefined portion of memory
 * and convert numbers into a string, too.
 * @param	hContact		- handle to the contact
 * @param	pszModule		- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting		- the setting to read
 * @param	pszValue		- buffer, that retrieves the value
 * @param	cchValue		- number of characters the buffer can take
 *
 * @retval	0 - success
 * @retval	1 - error
 **/
BYTE	GetStatic(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting, LPSTR pszValue, INT cchValue)
{
	DBVARIANT dbv;
	DBCONTACTGETSETTING sVal;

	if (pszValue && cchValue) {
		pszValue[0]	= 0;
		dbv.pszVal	= pszValue;
		dbv.cchVal	= cchValue;
		dbv.type	= DBVT_ASCIIZ;
		
		sVal.pValue		= &dbv;
		sVal.szModule	= pszModule;
		sVal.szSetting	= pszSetting;
		
		if (!CallService(MS_DB_CONTACT_GETSETTINGSTATIC, (WPARAM)hContact, (LPARAM)&sVal)) {
			switch (dbv.type) {
			case DBVT_BYTE:
				_itoa(dbv.bVal, pszValue, 10);
				break;
			case DBVT_WORD:
				_itoa(dbv.wVal, pszValue, 10);
				break;
			case DBVT_DWORD:
				_itoa(dbv.dVal, pszValue, 10);
			}
			return (pszValue[0] == 0);
		}
	}
	return 1;
}

/**
 * This function reads a byte from the database. If required it converts it to a byte value
 * @param	hContact		- handle to the contact
 * @param	pszModule		- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting		- the setting to read
 * @param	errorValue		- value to return if something goes wrong
 *
 * @return	byte value
 **/
BYTE	GetByte(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting, BYTE errorValue)
{
	DBVARIANT dbv;
	BYTE result;
	if (GetAsIs(hContact, pszModule, pszSetting, &dbv)) {
		result = errorValue;
	}
	else {
		switch (dbv.type) {
		case DBVT_BYTE:
			result = dbv.bVal;
			break;
		case DBVT_WORD:
			result = (dbv.wVal < 0x0100) ? (BYTE) dbv.wVal : errorValue;
			break;
		case DBVT_DWORD:
			result = (dbv.wVal < 0x00000100) ? (BYTE) dbv.dVal : errorValue;
			break;
		default:
			DB::Variant::Free(&dbv);
			result = errorValue;
		}
	}
	return result;
}		

/**
 * This function reads a word from the database. If required it converts it to a word value
 * @param	hContact		- handle to the contact
 * @param	pszModule		- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting		- the setting to read
 * @param	errorValue		- value to return if something goes wrong
 *
 * @return	word value
 **/
WORD	GetWord(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting, WORD errorValue)
{
	DBVARIANT dbv;
	WORD result;
	if (GetAsIs(hContact, pszModule, pszSetting, &dbv)) {
		result = errorValue;
	}
	else {
		switch (dbv.type) {
		case DBVT_BYTE:
			result = 0x00ff & dbv.bVal;
			break;
		case DBVT_WORD:
			result = dbv.wVal;
			break;
		case DBVT_DWORD:
			result = (dbv.wVal < 0x00010000) ? (WORD) dbv.dVal : errorValue;
			break;
		default:
			DB::Variant::Free(&dbv);
			result = errorValue;
		}
	}
	return result;
}	

/**
 * This function reads a double word from the database. If required it converts it to a double word value
 * @param	hContact		- handle to the contact
 * @param	pszModule		- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting		- the setting to read
 * @param	errorValue		- value to return if something goes wrong
 *
 * @return	double word value
 **/
DWORD	GetDWord(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting, DWORD errorValue)
{
	DBVARIANT dbv;
	DWORD result;
	if (GetAsIs(hContact, pszModule, pszSetting, &dbv)) {
		result = errorValue;
	}
	else {
		switch (dbv.type) {
		case DBVT_BYTE:
			result = 0x000000ff & dbv.bVal;
			break;
		case DBVT_WORD:
			result = 0x0000ffff & dbv.wVal;
			break;
		case DBVT_DWORD:
			result = dbv.dVal;
			break;
		default:
			DB::Variant::Free(&dbv);
			result = errorValue;
		}
	}
	return result;
}

/**
 * This function calls MS_DB_CONTACT_WRITESETTING to write a DBVARIANT structure to the database.
 * @param	hContact		- handle to the contact
 * @param	pszModule		- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting		- the setting to write
 * @param	dbv				- the DBVARIANT to store
 *
 * @retval	0 - success
 * @retval	1 - error
 **/
BYTE	WriteVariant(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting, const DBVARIANT *dbv)
{
	DBCONTACTWRITESETTING cws;

	cws.szModule	= pszModule;
	cws.szSetting	= pszSetting;
	memcpy(&cws.value, dbv, sizeof(DBVARIANT));
	return CallService(MS_DB_CONTACT_WRITESETTING, (WPARAM) hContact, (LPARAM) &cws) != 0;
}

/**
 * This function calls MS_DB_CONTACT_WRITESETTING to write a BYTE to the database.
 * @param	hContact		- handle to the contact
 * @param	pszModule		- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting		- the setting to write
 * @param	value			- the byte to store
 *
 * @retval	0 - success
 * @retval	1 - error
 **/
BYTE	WriteByte(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting, BYTE value)
{
	DBCONTACTWRITESETTING cws;

	cws.szModule	= pszModule;
	cws.szSetting	= pszSetting;
	cws.value.type	= DBVT_BYTE;
	cws.value.bVal	= value;
	return CallService(MS_DB_CONTACT_WRITESETTING, (WPARAM) hContact, (LPARAM) &cws) != 0;
}

/**
 * This function calls MS_DB_CONTACT_WRITESETTING to write a WORD to the database.
 * @param	hContact		- handle to the contact
 * @param	pszModule		- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting		- the setting to write
 * @param	value			- the word to store
 *
 * @retval	0 - success
 * @retval	1 - error
 **/
BYTE	WriteWord(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting, WORD value)
{
	DBCONTACTWRITESETTING cws;

	cws.szModule	= pszModule;
	cws.szSetting	= pszSetting;
	cws.value.type	= DBVT_WORD;
	cws.value.wVal	= value;
	return CallService(MS_DB_CONTACT_WRITESETTING, (WPARAM) hContact, (LPARAM) &cws) != 0;
}

/**
 * This function calls MS_DB_CONTACT_WRITESETTING to write a DWORD to the database.
 * @param	hContact		- handle to the contact
 * @param	pszModule		- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting		- the setting to write
 * @param	value			- the double word to store
 *
 * @retval	0 - success
 * @retval	1 - error
 **/
BYTE	WriteDWord(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting, DWORD value)
{
	DBCONTACTWRITESETTING cws;

	cws.szModule	= pszModule;
	cws.szSetting	= pszSetting;
	cws.value.type	= DBVT_DWORD;
	cws.value.dVal	= value;
	return CallService(MS_DB_CONTACT_WRITESETTING, (WPARAM) hContact, (LPARAM) &cws) != 0;
}

/**
 * This function calls MS_DB_CONTACT_WRITESETTING to write an ansi string to the database.
 * @param		hContact	- handle to the contact
 * @param		pszModule	- the module to read the setting from (e.g. USERINFO)
 * @param		pszSetting	- the setting to write
 * @param		value		- the string to store
 *
 * @retval	0 - success
 * @retval	1 - error
 **/
BYTE	WriteAString(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting, LPSTR value)
{
	DBCONTACTWRITESETTING cws;
	cws.szModule		= pszModule;
	cws.szSetting		= pszSetting;
	cws.value.type		= DBVT_ASCIIZ;
	cws.value.pszVal	= value;
	return CallService(MS_DB_CONTACT_WRITESETTING, (WPARAM) hContact, (LPARAM) &cws) != 0;
}

/**
 * This function calls MS_DB_CONTACT_WRITESETTING to write an unicode string to the database.
 * @param	hContact		- handle to the contact
 * @param	pszModule		- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting		- the setting to write
 * @param	value			- the string to store
 *
 * @retval	0 - success
 * @retval	1 - error
 **/
BYTE	WriteWString(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting, LPWSTR value)
{
	DBCONTACTWRITESETTING cws;
	cws.szModule		= pszModule;
	cws.szSetting		= pszSetting;
	cws.value.type		= DBVT_WCHAR;
	cws.value.pwszVal	= value;
	return CallService(MS_DB_CONTACT_WRITESETTING, (WPARAM) hContact, (LPARAM) &cws) != 0;
}

/**
 * This function calls MS_DB_CONTACT_WRITESETTING to write an utf8 string to the database.
 * @param	hContact		- handle to the contact
 * @param	pszModule		- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting		- the setting to write
 * @param	value			- the string to store
 *
 * @retval	0 - success
 * @retval	1 - error
 **/
BYTE	WriteUString(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting, LPSTR value)
{
	DBCONTACTWRITESETTING cws;
	cws.szModule		= pszModule;
	cws.szSetting		= pszSetting;
	cws.value.type		= DBVT_UTF8;
	cws.value.pszVal	= value;
	return CallService(MS_DB_CONTACT_WRITESETTING, (WPARAM) hContact, (LPARAM) &cws) != 0;
}

/**
 * This function checks for the existence of the given setting in the database
 * @param	hContact		- handle to the contact
 * @param	pszModule		- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting		- the setting to check
 *
 * @retval	TRUE			- setting exists
 * @retval	FALSE			- setting does not exist
 **/
BOOLEAN	Exists(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting)
{
	if (pszModule && pszSetting) {
		DBCONTACTGETSETTING cgs;
		DBVARIANT dbv;
		CHAR szDummy[1];

		dbv.pszVal		= szDummy;
		dbv.cchVal		= sizeof(szDummy);
		dbv.type		= 0;
		cgs.pValue		= &dbv;
		cgs.szModule	= pszModule;
		cgs.szSetting	= pszSetting;
		if (!CallService(MS_DB_CONTACT_GETSETTINGSTATIC, (WPARAM) hContact, (LPARAM) &cgs)) {
			return (dbv.type > DBVT_DELETED);
		}
	}
	return FALSE;
}

/**
 * This function deletes the given setting from database 
 * @param	hContact		- handle to the contact
 * @param	pszModule		- the module to read the setting from (e.g. USERINFO)
 * @param	pszSetting		- the setting to read
 *
 * @retval	0 - success
 * @retval	1 - failure
 **/
BYTE	Delete(HANDLE hContact, LPCSTR pszModule, LPCSTR pszSetting)
{
	DBCONTACTGETSETTING cgs;
	cgs.szModule	= pszModule;
	cgs.szSetting	= pszSetting;
	return CallService(MS_DB_CONTACT_DELETESETTING, (WPARAM) hContact, (LPARAM) &cgs) != 0;
}

/**
 * This function deletes all reluctant settings of an setting array such as My-phoneXX.
 * @param	hContact		- handle to the contact
 * @param	pszModule		- the module to read the setting from (e.g. USERINFO)
 * @param	pszFormat		- the format, telling what a array of settings is ment
 * @param	iStart			- the first index of the setting to delete
 *
 * @return	nothing
 **/
VOID	DeleteArray(HANDLE hContact, LPCSTR pszModule, LPCSTR pszFormat, INT iStart)
{
	CHAR pszSetting[MAXSETTING];
	do {
		mir_snprintf(pszSetting, MAXSETTING, pszFormat, iStart++);
	}
	while (!DB::Setting::Delete(hContact, pszModule, pszSetting));
}

/**
 * This function can prevent a setting from being stored to database permanently.
 * @param	pszSetting		- the setting to read
 * @param	enabled			- if set to 'true' the setting will not be stored in database
 *
 * @retval	0 - success
 * @retval	1 - failure
 **/
BYTE	Resident(LPCSTR pszSetting, const bool enabled)
{
	return CallService(MS_DB_SETSETTINGRESIDENT, (WPARAM) enabled, (LPARAM) pszSetting) != 0;
}

} /* namespace Setting */

namespace Variant {

BYTE	Free(DBVARIANT *dbv)
{
	return CallService(MS_DB_CONTACT_FREEVARIANT, 0, (LPARAM) dbv) != 0;
}

/**
 * This function converts a string value of the DBVARIANT to the destination type
 * but keeps all other values as is.
 * @param		dbv			- pointer to DBVARIANT structure which is to manipulate
 * @param		destType	- one of (DBVT_ASCIIZ, DBVT_UTF8 or DBVT_WCHAR)
 *
 * @retval		0			- success
 * @retval		1			- error
 **/
BYTE	ConvertString(DBVARIANT* dbv, const BYTE destType)
{
	if (dbv) {
		switch (dbv->type) {
		// source value is of type "ascii"
		case DBVT_ASCIIZ:
			{
				switch (destType) {
				// destination type is "utf8"
				case DBVT_UTF8:
					{
						LPSTR tmpBuf = mir_utf8encode(dbv->pszVal);
						mir_free(dbv->pszVal);
						dbv->pszVal = tmpBuf;
						dbv->type = (dbv->pszVal) ? destType : DBVT_DELETED;
					} break;
				// destination type is "wchar"
				case DBVT_WCHAR:
					{
						LPWSTR tmpBuf = mir_a2u(dbv->pszVal);
						mir_free(dbv->pszVal);
						dbv->pwszVal = tmpBuf;
						dbv->type = (dbv->pwszVal) ? destType : DBVT_DELETED;
					}
				}
			} break;
		// source value is of type "utf8"
		case DBVT_UTF8:
			{
				switch (destType) {
				// destination type is "ascii"
				case DBVT_ASCIIZ:
					{
						mir_utf8decode(dbv->pszVal, NULL);
						dbv->type = (dbv->pszVal) ? destType : DBVT_DELETED;
					} break;
				// destination type is "wchar"
				case DBVT_WCHAR:
					{
						LPSTR savePtr = dbv->pszVal;
						dbv->pszVal = NULL;
						mir_utf8decode(savePtr, &dbv->pwszVal);
						mir_free(savePtr);
						dbv->type = (dbv->pwszVal) ? destType : DBVT_DELETED;
					}
				}
			} break;
		// source value is of type "wchar"
		case DBVT_WCHAR:
			{
			switch (destType) {
				// destination type is "ascii"
				case DBVT_ASCIIZ:
					{
						LPSTR tmpBuf = mir_u2a(dbv->pwszVal);
						mir_free(dbv->pwszVal);
						dbv->pszVal = tmpBuf;
						dbv->type = (dbv->pszVal) ? destType : DBVT_DELETED;
					} break;
				// destination type is "utf8"
				case DBVT_UTF8:
					{
						LPSTR tmpBuf = mir_utf8encodeW(dbv->pwszVal);
						mir_free(dbv->pwszVal);
						dbv->pszVal = tmpBuf;
						dbv->type = (dbv->pszVal) ? destType : DBVT_DELETED;
					}
				}
			}
		}
		return dbv->type == DBVT_DELETED;
	}
	return 1;
}

/**
 * This function completely converts a DBVARIANT to the destination string type.
 * It includes BYTE, WORD, DWORD and all string types
 * @param		dbv			- pointer to DBVARIANT structure which is to manipulate
 * @param		destType	- one of (DBVT_ASCIIZ, DBVT_UTF8 or DBVT_WCHAR)
 *
 * @retval	0 - success
 * @retval	1 - error
 **/
BYTE	dbv2String(DBVARIANT* dbv, const BYTE destType)
{
	if (dbv) {
		switch (destType) {
		// destination type is "utf8" or "ascii"
		case DBVT_ASCIIZ:
		case DBVT_UTF8:
			{
				CHAR buf[32];
				switch (dbv->type) {
				// source value is of type "byte"
				case DBVT_BYTE:
					{
						_ultoa(dbv->bVal, buf, 10);
						dbv->pszVal = mir_strdup(buf);
						dbv->type = (dbv->pszVal) ? destType : DBVT_DELETED;
					} break;
				// source value is of type "word"
				case DBVT_WORD:
					{
						_ultoa(dbv->wVal, buf, 10);
						dbv->pszVal = mir_strdup(buf);
						dbv->type = (dbv->pszVal) ? destType : DBVT_DELETED;
					} break;
				// source value is of type "dword"
				case DBVT_DWORD:
					{
						_ultoa(dbv->dVal, buf, 10);
						dbv->pszVal = mir_strdup(buf);
						dbv->type = (dbv->pszVal) ? destType : DBVT_DELETED;
					} break;
				// source value is of any string type
				case DBVT_ASCIIZ:
				case DBVT_WCHAR:
				case DBVT_UTF8:
					{
						return ConvertString(dbv, destType);
					}
				}
			} break;
		// destination type is "wchar"
		case DBVT_WCHAR:
			{
				WCHAR buf[32];
				switch (dbv->type) {
				// source value is of type "byte"
				case DBVT_BYTE:
					{
						_ultow(dbv->bVal, buf, 10);
						dbv->pwszVal = mir_wcsdup(buf);
						dbv->type = (dbv->pwszVal) ? destType : DBVT_DELETED;
					} break;
				// source value is of type "word"
				case DBVT_WORD:
					{
						_ultow(dbv->wVal, buf, 10);
						dbv->pwszVal = mir_wcsdup(buf);
						dbv->type = (dbv->pwszVal) ? destType : DBVT_DELETED;
					} break;
				// source value is of type "dword"
				case DBVT_DWORD:
					{
						_ultow(dbv->dVal, buf, 10);
						dbv->pwszVal = mir_wcsdup(buf);
						dbv->type = (dbv->pwszVal) ? destType : DBVT_DELETED;
					} break;
				// source value is of any string type
				case DBVT_ASCIIZ:
				case DBVT_WCHAR:
				case DBVT_UTF8:
					{
						return ConvertString(dbv, destType);
					}
				}
			}
		}
		return dbv->type != destType;
	}
	return 1;
}

} /* namespace Variant */

namespace Event {

/**
 * This function searches for the first event for the given contact.
 * @param	hContact		- the handle of the contact to search events for
 *
 * @return	This function returns the HANDLE of the first event for the given contact.
 **/
HANDLE	FindFirst(HANDLE hContact)
{
	return (HANDLE)CallService(MS_DB_EVENT_FINDFIRST, (WPARAM)hContact, 0);
}

/**
 * This function searches for the last event for the given contact.
 * @param	hContact		- the handle of the contact to search events for
 *
 * @return	This function returns the HANDLE of the last event for the given contact.
 **/
HANDLE	FindLast(HANDLE hContact)
{
	return (HANDLE)CallService(MS_DB_EVENT_FINDLAST, (WPARAM)hContact, 0);
}

/**
 * This function searches for the next event in the chain, which follows the given event.
 * @param	hEvent			- the handle of the event where to continue searching
 *
 * @return	This function returns the HANDLE of the next event in the event chain.
 **/
HANDLE	FindNext(HANDLE hEvent)
{
	return (HANDLE)CallService(MS_DB_EVENT_FINDNEXT, (WPARAM)hEvent, 0);
}

/**
 * This function searches for the previous event in the chain, which follows the given event.
 * @param	hEvent			- the handle of the event where to continue searching
 *
 * @return	This function returns the HANDLE of the previous event in the event chain.
 **/
HANDLE	FindPrev(HANDLE hEvent)
{
	return (HANDLE)CallService(MS_DB_EVENT_FINDPREV, (WPARAM)hEvent, 0);
}

/**
 * This function initializes the DBEVENTINFO structure and calls 
 * the MS_DB_EVENT_GET service to retrieve information about an event.
 * @param	hEvent			- the handle of the event to get information for
 * @param	dbei			- the pointer to a DBEVENTINFO structure, which retrieves all information.
 *
 * @retval	0				- success
 * @retval	nonezero		- failure
 **/
BYTE	GetInfo(HANDLE hEvent, DBEVENTINFO *dbei)
{
	dbei->cbSize = sizeof(DBEVENTINFO);
	dbei->cbBlob = 0;
	dbei->pBlob  = NULL;
	return CallService(MS_DB_EVENT_GET, (WPARAM)hEvent, (LPARAM)dbei) != 0;
}

/**
 * This function initializes the DBEVENTINFO structure and calls 
 * the MS_DB_EVENT_GET service to retrieve information about an event.
 * @param	hEvent			- the handle of the event to get information for
 * @param	dbei			- the pointer to a DBEVENTINFO structure, which retrieves all information.
 *
 * @retval	0				- success
 * @retval	1				- failure
 **/
BYTE	GetInfoWithData(HANDLE hEvent, DBEVENTINFO *dbei)
{
	BYTE result;
	dbei->cbSize = sizeof(DBEVENTINFO);
	if(!dbei->cbBlob) {
		INT_PTR size = BlobSizeOf(hEvent);
		dbei->cbBlob = (size != -1) ? (DWORD)size : 0;
	}
	if(dbei->cbBlob) {
		dbei->pBlob = (PBYTE) mir_alloc(dbei->cbBlob);
		if (dbei->pBlob == NULL) {
			dbei->cbBlob = 0;
		}
	}
	else {
		dbei->pBlob = NULL;
	}

	result = CallService(MS_DB_EVENT_GET, (WPARAM)hEvent, (LPARAM)dbei) != 0;
	if (result && dbei->pBlob) {
		mir_free(dbei->pBlob);
		dbei->pBlob = NULL;
	}
	return result;
}

/**
 * This function returns the timestamp for the given event.
 * @param	hEvent			- the handle of the event to get the timestamp for
 *
 * @retval	0 if no timestamp is available
 * @retval	timestamp
 **/
DWORD	TimeOf(HANDLE hEvent)
{
	DBEVENTINFO dbei;
	if (!GetInfo(hEvent, &dbei)) {
		return dbei.timestamp;
	}
	return 0;
}

/**
 * This function returns the number of bytes required to retrieve
 * binary data associated with the event.
 * @param	hEvent			- the handle of the event to get the number of bytes for
 *
 * @retval	size of event data
 * @retval	-1 if hEvent is invalid
 **/
INT_PTR	BlobSizeOf(HANDLE hEvent)
{
	return CallService(MS_DB_EVENT_GETBLOBSIZE, (WPARAM)hEvent, 0);
}

/**
 * This function compares two DBEVENTINFO structures against each other.
 * It compares the timestamp, eventType and module names.
 * @param	d1				- pointer to the first DBEVENTINFO structure
 * @param	d2				- pointer to the second DBEVENTINFO structure
 * @param	Data			- default false, if true compare also blob data
.*
 * @retval	TRUE			- The structures describe the same event.
 * @retval	FALSE			- The two structure's events differ from each other.
 **/
static	FORCEINLINE
BOOLEAN	IsEqual(const DBEVENTINFO *d1, const DBEVENTINFO *d2, bool Data)
{
	BOOLEAN res = d1 && d2 && 
				(d1->timestamp == d2->timestamp) && 
				(d1->eventType == d2->eventType) &&
				(d1->cbBlob == d2->cbBlob) && 
				(!d1->szModule || !d2->szModule || !stricmp(d1->szModule, d2->szModule))
				;
	if(Data) {
			return res &&
				(!d1->pBlob || !d2->pBlob || !memcmp(d1->pBlob,d2->pBlob,d1->cbBlob))
				;
	}
	return res;
}

/**
 * This functions searches for an existing event in the database, which matches
 * the information provided by 'dbei'. In order to fasten up the search e.g. 
 * while checking many events, this function stars searching from the last
 * found event.
 * @param	hContact			- the handle of the contact to search events for
 * @param	hDbExistingEvent	- an existing database event to start the search from.
 * @param	dbei				- the pointer to a DBEVENTINFO structure
 *
 * @retval	TRUE	- the event identified by its information in @c dbei exists.
 * @retval	FALSE	- no event with the information of @c dbei exists.
 *
 **/
BOOLEAN	Exists(HANDLE hContact, HANDLE& hDbExistingEvent, DBEVENTINFO *dbei)
{
	BOOLEAN		result = FALSE;
	DBEVENTINFO	edbei;
	HANDLE		sdbe,
				edbe;

	if (!hDbExistingEvent) {
		hDbExistingEvent = FindFirst(hContact);
		if (hDbExistingEvent) {
			if (!GetInfo(hDbExistingEvent, &edbei)) {
				if ((dbei->timestamp < edbei.timestamp)) {
					return FALSE;
				}
				if(IsEqual(dbei, &edbei, false)) {
					if (!GetInfoWithData(hDbExistingEvent, &edbei)) {
						if(IsEqual(dbei, &edbei, true)) {
							mir_free(edbei.pBlob);
							return TRUE;
						}
						mir_free(edbei.pBlob);
					}
				}
			}
			edbe = FindLast(hContact);
			if (edbe == hDbExistingEvent) {
				return FALSE;
			}
			hDbExistingEvent = edbe;
		}
	}
	if (hDbExistingEvent) {
		sdbe = hDbExistingEvent;
		for (	edbe = sdbe;
				edbe && !GetInfo(edbe, &edbei) && (dbei->timestamp <= edbei.timestamp);
				edbe = FindPrev(edbe) ) {
			hDbExistingEvent = edbe;
			//compare without data (faster)
			if( result = IsEqual(dbei, &edbei, false)) {
				if(NULL == (result = !GetInfoWithData(edbe, &edbei))) continue;
				//compare with data
				result = IsEqual(dbei, &edbei, true);
				mir_free(edbei.pBlob);
				if (result) {
					break;
				}
			}
		} /*end for*/

		if (!result) {
			for (	edbe = FindNext(sdbe);
					edbe && !GetInfo(edbe, &edbei) && (dbei->timestamp >= edbei.timestamp);
					edbe = FindNext(edbe) ) {
				hDbExistingEvent = edbe;
				//compare without data (faster)
				if( result = IsEqual(dbei, &edbei, false)) {
					if(NULL == (result = !GetInfoWithData(edbe, &edbei))) continue;
					//compare with data
					result = IsEqual(dbei, &edbei, true);
					mir_free(edbei.pBlob);
					if (result) {
						break;
					}
				}
			}
		}
	}
	return result;
}

} /* namespace Events */

INT CEnumList::EnumProc(LPCSTR pszName, DWORD ofsModuleName, LPARAM lParam)
{
	if (pszName) {
		((CEnumList*)lParam)->Insert(pszName);
	}
	return 0;
}

INT CEnumList::EnumSettingsProc(LPCSTR pszName, LPARAM lParam)
{
	return EnumProc(pszName, 0, lParam);
}

INT CEnumList::CompareProc(LPCSTR p1, LPCSTR p2)
{
	if (p1) {
		if (p2) {
			return strcmp(p1, p2);
		}
		return 1;
	}
	return 0;
}

CEnumList::CEnumList()	: LIST<CHAR>(50, (FTSortFunc)CEnumList::CompareProc)
{
}

CEnumList::~CEnumList() 
{ 
	INT i, cnt;
	LPSTR p;

	for (i = 0, cnt = getCount(); i < cnt; i++) {
		p = (*this)[i]; 
		if (p) {
			mir_free(p);
		}
	}
	destroy();
}

LPSTR CEnumList::Insert(LPCSTR str)
{
	LPSTR p = mir_strdup(str);
	if (p && !insert(p)) {
		mir_free(p);
		p = NULL;
	}
	return p;
}

INT_PTR CEnumList::EnumModules()
{
	return CallService(MS_DB_MODULES_ENUM, (WPARAM)this, (LPARAM)CEnumList::EnumProc);
}

/**
 * @retval	-1	- no settings to enumerate
 * @retval	 0	- success
 **/
INT_PTR CEnumList::EnumSettings(HANDLE hContact, LPCSTR pszModule)
{
	DBCONTACTENUMSETTINGS dbces;
	
	dbces.pfnEnumProc = (DBSETTINGENUMPROC)CEnumList::EnumSettingsProc;
	dbces.szModule = pszModule;
	dbces.lParam = (LPARAM)this;
	dbces.ofsSettings = 0;
	return CallService(MS_DB_CONTACT_ENUMSETTINGS, (WPARAM)hContact, (LPARAM)&dbces);
}

} /* namespace DB */