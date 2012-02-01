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

File name      : $HeadURL: http://userinfoex.googlecode.com/svn/trunk/svc_contactinfo.cpp $
Revision       : $Revision: 211 $
Last change on : $Date: 2011-04-04 21:10:59 +0400 (Пн, 04 апр 2011) $
Last change by : $Author: kreol13@gmail.com $

===============================================================================
*/

/**
 * System Includes:
 **/
#include "commonheaders.h"
#include "m_contacts.h"
#include "svc_timezone.h"
#include "svc_timezone_old.h"
#include "svc_contactinfo.h"
#include "svc_gender.h"
#include "..\Flags\svc_countrylistext.h"

#define	CI_TCHAR(ci)	(((ci)->dwFlag & CNF_UNICODE) ? DBVT_WCHAR : DBVT_ASCIIZ)

#define NAMEORDERCOUNT 8
static BYTE gNameOrder[NAMEORDERCOUNT];	// name order as set up for contact list
HANDLE hSettingChanged;

/**
 * This function translates the DBVARIANT structure to an CONTACTINFO structure
 * and keeps the original data type.
 *
 * @warning	ci MUST NOT be NULL and dbv must be freed by caller on failure!
 *					
 * @param	dbv			- DBVARIANT to take the data for translation from
 * @param	ci			- CONTACTINFO structure to translate to
 *
 * @retval	0			- success
 * @retval	1			- failure
 **/
static FORCEINLINE INT_PTR VarToVarCI(const DBVARIANT *dbv, CONTACTINFO *ci)
{
	switch (dbv->type) {
	case DBVT_ASCIIZ:
	case DBVT_WCHAR:	{
			// string translation is to be done by caller!!!
			ci->pszVal = dbv->ptszVal;
			ci->type = (ci->pszVal != NULL) ? CNFT_ASCIIZ : 0;
		} break;

	case DBVT_BYTE:		{
			ci->type = CNFT_BYTE;
			ci->bVal = dbv->bVal;
		} break;

	case DBVT_WORD:		{
			ci->type = CNFT_WORD;
			ci->wVal = dbv->wVal;
		} break;

	case DBVT_DWORD:	{
			ci->type = CNFT_DWORD;
			ci->dVal = dbv->dVal;
		} break;

	default:			{
			ci->type = 0;
		}
	}
	return ci->type == 0;
}

/**
 * This function tries to read a setting from the contact's protocol module.
 *
 * @warning	ci MUST NOT be NULL!
 *
 * @param	ci			- pointer to a CONTACTINFO structure holding information about the contact
 * @param	pszSetting	- the desired setting to read
 *
 * @retval	0 - if setting was found and read correctly
 * @retval	1 - if any error occured or setting was not found
 **/
static FORCEINLINE INT_PTR GCIVar(CONTACTINFO *ci, LPCSTR pszSetting)
{
	DBVARIANT dbv;
	
	if (DB::Setting::Get(ci->hContact, ci->szProto, pszSetting, &dbv, CI_TCHAR(ci)) == 0) {
		if (VarToVarCI(&dbv, ci)) {
			// On a error, we need to make sure, read data is cleared out!
			DB::Variant::Free(&dbv);
		}
	}
	else {
		ci->type = 0;
	}
	return ci->type == 0;
}

/**
 * This function tries to read a setting from a certain module (e.g. USERINFO) and if failed it 
 * tries once again with the baseprotocol of the contact (e.g. ICQ). If nothing was found anyway
 * and this is an metacontact it can have a look into subcontacts to retrieve the information.
 * This depends on the settings the user did.
 *
 * @warning	ci MUST NOT be NULL!
 *
 * @param	ci			- pointer to a CONTACTINFO structure holding information about the contact
 * @param	pszSetting	- the desired setting to read
 *
 * @retval	0 - if setting was found and read correctly
 * @retval	1 - if any error occured or setting was not found
 **/
static FORCEINLINE INT_PTR GCIVarEx(CONTACTINFO *ci, LPCSTR pszSetting)
{
	DBVARIANT dbv;

	if (DB::Setting::GetEx(ci->hContact, USERINFO, ci->szProto, pszSetting, &dbv, CI_TCHAR(ci)) == 0) {
		if (VarToVarCI(&dbv, ci)) {
			// On a error, we need to make sure, read data is cleared out!
			DB::Variant::Free(&dbv);
		}
	}
	else {
		ci->type = 0;
	}
	return ci->type == 0;
}

/**
 * This function tries to read a Language from a certain module (e.g. USERINFO) and if failed it 
 * tries once again with the baseprotocol of the contact (e.g. ICQ). If nothing was found anyway
 * and this is an metacontact it can have a look into subcontacts to retrieve the information.
 * This depends on the settings the user did.
 *
 * @warning	ci MUST NOT be NULL!
 *
 * @param	ci			- pointer to a CONTACTINFO structure holding information about the contact
 * @param	pszSetting	- the desired setting to read
 *
 * @retval	0 - if setting was found and read correctly
 * @retval	1 - if any error occured or setting was not found
 **/
static FORCEINLINE INT_PTR GCILangEx(CONTACTINFO *ci, LPCSTR pszSetting)
{
	if (0 == GCIVarEx(ci, pszSetting)) {
		if(ci->type!= CNFT_ASCIIZ) {
			//lang was safed in database as code
			LPIDSTRLIST pList;
			UINT nList, i, res = 0;
			switch (ci->type) {
				case CNFT_BYTE:		res = ci->bVal;	break;
				case CNFT_WORD:		res = ci->wVal;	break;
				case CNFT_DWORD:	res = ci->dVal;	break;
				default:			return 1;
			}
			GetLanguageList(&nList, &pList);
			for(i = 0; i<nList; i++) {
				if(pList[i].nID == res)	{
					//use untranslate string (pszText member)
					ci->pszVal = (ci->dwFlag & CNF_UNICODE)
								? (LPTSTR) mir_a2u(pList[i].pszText)
								: (LPTSTR) mir_strdup(pList[i].pszText);
					ci->type = (ci->pszVal != NULL) ? CNFT_ASCIIZ : 0;
					return 0;
				}
			} /*end for*/
			ci->type	= 0;
			ci->pszVal	= NULL;
		}
	}
	else {
		ci->type = 0;
	}
	return ci->type == 0;
}

/**
 * This function read a setting from the baseprotocol of the contact (e.g. ICQ). 
 *
 * @warning	ci MUST NOT be NULL!
 *
 * @param	ci			- pointer to a CONTACTINFO structure holding information about the contact
 * @param	pszSetting	- the desired setting to read
 *
 * @retval	0 - if setting was found and read correctly
 * @retval	1 - if any error occured or setting was not found
 **/
static FORCEINLINE INT_PTR GCIStr(CONTACTINFO *ci, LPCSTR pszSetting)
{
	const BYTE type = CI_TCHAR(ci);
	DBVARIANT dbv;
	
	if (DB::Setting::Get(ci->hContact, ci->szProto, pszSetting, &dbv, type) == 0) {
		if (DB::Variant::dbv2String(&dbv, type) == 0) {
			ci->pszVal = dbv.ptszVal;
		}
		else {
			DB::Variant::Free(&dbv);
			ci->pszVal = NULL;
		}
	}
	else {
		ci->pszVal = NULL;
	}
	ci->type = (ci->pszVal) ? CNFT_ASCIIZ : 0;
	return ci->type == 0;
}

/**
 * Format the full name for the contact.
 *
 * @params	ci			- CONTACTINFO structure
 *
 * @retval	0 - if setting was found and read correctly
 * @retval	1 - if any error occured or setting was not found
 **/
static FORCEINLINE INT_PTR GCIFirstLast(CONTACTINFO *ci)
{
	DBVARIANT dbvf, dbvl;
	size_t cbf, cbl;

	BYTE type = CI_TCHAR(ci);

	if (DB::Setting::GetEx(ci->hContact, USERINFO, ci->szProto, SET_CONTACT_FIRSTNAME, &dbvf, type))
	{
		dbvf.type = DBVT_DELETED;
	}
	if (DB::Setting::GetEx(ci->hContact, USERINFO, ci->szProto, SET_CONTACT_LASTNAME, &dbvl, type))
	{
		dbvl.type = DBVT_DELETED;
	}

	if (type == DBVT_WCHAR)
	{
		// both firstname and lastname are valid
		if (dbvf.type == DBVT_WCHAR && dbvl.type == DBVT_WCHAR)
		{
			cbf = mir_wcslen(dbvf.pwszVal);
			cbl = mir_wcslen(dbvl.pwszVal);

			ci->pszVal = (LPTSTR) mir_alloc((cbf + cbl + 2) * sizeof(WCHAR));
			if (ci->pszVal)
			{
				mir_snwprintf((LPWSTR) ci->pszVal, cbf + cbl + 2, L"%s %s", dbvf.pwszVal, dbvl.pwszVal);
			}
			DB::Variant::Free(&dbvf);
			DB::Variant::Free(&dbvl);
		}
		// set firstname as result
		else if (dbvf.type == DBVT_WCHAR)
		{
			ci->pszVal = (LPTSTR) dbvf.pwszVal;
			DB::Variant::Free(&dbvl);
		}
		// set lastname as result
		else if (dbvl.type == DBVT_WCHAR)
		{
			ci->pszVal = (LPTSTR) dbvl.pwszVal;
			DB::Variant::Free(&dbvf);
		}
		else
		{
			ci->pszVal = NULL;
			DB::Variant::Free(&dbvf);
			DB::Variant::Free(&dbvl);
		}
	}
	else
	{
		// both firstname and lastname are valid
		if (dbvf.type == DBVT_ASCIIZ && dbvl.type == DBVT_ASCIIZ)
		{
			cbf = mir_strlen(dbvf.pszVal);
			cbl = mir_strlen(dbvl.pszVal);

			ci->pszVal = (LPTSTR) mir_alloc((cbf + cbl + 2) * sizeof(CHAR));
			if (ci->pszVal)
			{
				mir_snprintf((LPSTR) ci->pszVal, cbf + cbl + 2, "%s %s", dbvf.pszVal, dbvl.pszVal);
			}
			DB::Variant::Free(&dbvf);
			DB::Variant::Free(&dbvl);
		}
		// set firstname as result
		else if (dbvf.type == DBVT_ASCIIZ)
		{
			ci->pszVal = (LPTSTR) dbvf.pszVal;
			DB::Variant::Free(&dbvl);
		}
		// set lastname as result
		else if (dbvl.type == DBVT_ASCIIZ)
		{
			ci->pszVal = (LPTSTR) dbvl.pszVal;
			DB::Variant::Free(&dbvf);
		}
		else
		{
			ci->pszVal = NULL;
			DB::Variant::Free(&dbvf);
			DB::Variant::Free(&dbvl);
		}
	}
	ci->type = (ci->pszVal != NULL) ? CNFT_ASCIIZ : 0;
	return ci->type == 0;
}

/**
 * return the country name
 *
 * @param	ci			- pointer to a CONTACTINFO structure holding information about the contact
 * @param	pszSetting	- the desired setting to read the countrys id from
 *
 * @retval	0 - if setting was found and read correctly
 * @retval	1 - if any error occured or setting was not found
 **/
static FORCEINLINE INT_PTR GCICountry(CONTACTINFO *ci, LPCSTR pszSetting)
{
	if (0 == GCIVarEx(ci, pszSetting)) {
		if (ci->type != CNFT_ASCIIZ) {
			// country id was safed in database as code
			UINT res = 0;
			switch (ci->type) {
				case CNFT_WORD:		res = ci->wVal;	break;
				case CNFT_DWORD:	res = ci->dVal;	break;
				default:			return 1;
			}

//			LPSTR szCountry = res ? (LPSTR)CallService(MS_UTILS_GETCOUNTRYBYNUMBER, res, 0) : NULL;
			LPSTR szCountry = res ? (LPSTR)ServiceGetCountryByNumber(res, 0) : NULL;
			if (szCountry) {
				ci->pszVal = (ci->dwFlag & CNF_UNICODE) 
							 ? (LPTSTR) mir_a2u(szCountry) 
							 : (LPTSTR) mir_strdup(szCountry);
			}
			else {
				ci->pszVal = NULL;
			}
			ci->type = (ci->pszVal != NULL) ? CNFT_ASCIIZ : 0;
		}
	}
	else {
		ci->type = 0;
	}
	return ci->type == 0;
}

/**
 * This is the service procedure to retrieve contact information
 *
 * @param	wParam		- not used
 * @param	lParam		- pointer to a CONTACTINFO structure which tells what information is desired
 *
 * @retval	0 - if contact information was found and read correctly 
 * @retval	1 - if any error occured or setting was not found
 **/
INT_PTR GetContactInfo(WPARAM wParam, LPARAM lParam) 
{
	CONTACTINFO *ci = (CONTACTINFO*) lParam;
	INT_PTR result;

	if (ci && ci->cbSize == sizeof(CONTACTINFO) && (ci->szProto != NULL || (ci->szProto = DB::Contact::Proto(ci->hContact)) != NULL))
	{
		switch (ci->dwFlag & 0x7F) {

		//
		// contact name
		//
		case CNF_FIRSTNAME:		{
				result = GCIVarEx(ci, SET_CONTACT_FIRSTNAME);
			} break;

		case CNF_LASTNAME:		{
				result = GCIVarEx(ci, SET_CONTACT_LASTNAME);
			} break;

		case CNF_FIRSTLAST:		{
				result = GCIVarEx(ci, SET_CONTACT_FIRSTLASTNAME);	//first try to read "FullName"
				if(result) result = GCIFirstLast(ci);				//fallback to "FirstName" + "LastName"
			} break;

		case CNF_NICK:			{
				result = GCIVarEx(ci, SET_CONTACT_NICK);
			} break;

		case CNF_CUSTOMNICK:	{
				LPSTR s = ci->szProto;
				ci->szProto = MOD_CLIST;
				result = GCIVar(ci, SET_CONTACT_MYHANDLE);
				ci->szProto = s;
			} break;

		case CNF_LANGUAGE1:			{
				result = GCILangEx(ci, SET_CONTACT_LANG1);
			} break;

		case CNF_LANGUAGE2:			{
				result = GCILangEx(ci, SET_CONTACT_LANG2);
			} break;

		case CNF_LANGUAGE3:			{
				result = GCILangEx(ci, SET_CONTACT_LANG3);
			} break;

		//
		// private contact
		//
		case CNF_STREET:		{
				result = GCIVarEx(ci, SET_CONTACT_STREET);
			} break;

		case CNF_ZIP:			{
				result = GCIVarEx(ci, SET_CONTACT_ZIP); 
			} break;

		case CNF_CITY:			{
				result = GCIVarEx(ci, SET_CONTACT_CITY);
			} break;

		case CNF_STATE:			{
				result = GCIVarEx(ci, SET_CONTACT_STATE);
			} break;

		case CNF_COUNTRY:		{
				result = GCICountry(ci, SET_CONTACT_COUNTRY);
			} break;

		case CNF_PHONE:			{
				result = GCIVarEx(ci, SET_CONTACT_PHONE);
			} break;

		case CNF_FAX:			{
				result = GCIVarEx(ci, SET_CONTACT_FAX);
			} break;

		case CNF_CELLULAR:		{
				result = GCIVarEx(ci, SET_CONTACT_CELLULAR);
			} break;

		case CNF_EMAIL:			{
				result = GCIVarEx(ci, SET_CONTACT_EMAIL);
			} break;

		case CNF_HOMEPAGE:		{
				result = GCIVarEx(ci, SET_CONTACT_HOMEPAGE);
			} break;

		//
		// company information
		//
		case CNF_CONAME:		{
				result = GCIVarEx(ci, SET_CONTACT_COMPANY);
			} break;

		case CNF_CODEPT:		{
				result = GCIVarEx(ci, SET_CONTACT_COMPANY_DEPARTMENT);
			} break;

		case CNF_COPOSITION:	{
				result = GCIVarEx(ci, SET_CONTACT_COMPANY_POSITION);
			} break;

		case CNF_COSTREET:		{
				result = GCIVarEx(ci, SET_CONTACT_COMPANY_STREET);
			} break;

		case CNF_COZIP:			{
				result = GCIVarEx(ci, SET_CONTACT_COMPANY_ZIP);
			} break;

		case CNF_COCITY:		{
				result = GCIVarEx(ci, SET_CONTACT_COMPANY_CITY);
			} break;

		case CNF_COSTATE:		{
				result = GCIVarEx(ci, SET_CONTACT_COMPANY_STATE);
			} break;

		case CNF_COCOUNTRY:		{
				result = GCICountry(ci, SET_CONTACT_COMPANY_COUNTRY);
			} break;

		case CNF_COPHONE:		{
				result = GCIVarEx(ci, SET_CONTACT_COMPANY_PHONE);
			} break;

		case CNF_COFAX:			{
				result = GCIVarEx(ci, SET_CONTACT_COMPANY_FAX);
			} break;

		case CNF_COCELLULAR:	{
				result = GCIVarEx(ci, SET_CONTACT_COMPANY_CELLULAR);
			} break;

		case CNF_COEMAIL:		{
				result = GCIVarEx(ci, SET_CONTACT_COMPANY_EMAIL);
			} break;

		case CNF_COHOMEPAGE:	{
				result = GCIVarEx(ci, SET_CONTACT_COMPANY_HOMEPAGE);
			} break;

		//
		// personal information
		//
		case CNF_ABOUT:			{
				result = GCIVarEx(ci, SET_CONTACT_ABOUT);
			} break;

		case CNF_MYNOTES:		{
				result = GCIVarEx(ci, SET_CONTACT_MYNOTES);
			} break;

		case CNF_AGE:			{
				result = GCIVarEx(ci, SET_CONTACT_AGE);
			} break;	  // returns age (byte, 0==unspecified) ??

		case CNF_GENDER:		{
				ci->bVal = GenderOf(ci->hContact, ci->szProto);
				ci->type = (ci->bVal != 0) ? CNFT_BYTE : 0;
				result = ci->type == 0;
			} break;

		case CNF_BIRTHDAY:		{
				MAnnivDate mda;
				result = mda.DBGetBirthDate(ci->hContact, ci->szProto);
				if (result == 0) {
					ci->bVal = (BYTE) mda.Day();
					ci->type = CNFT_BYTE;
				}
			} break;

		case CNF_BIRTHMONTH:	{
				MAnnivDate mda;
				result = mda.DBGetBirthDate(ci->hContact, ci->szProto);
				if (result == 0) {
					ci->bVal = (BYTE) mda.Month();
					ci->type = CNFT_BYTE;
				}
			} break;

		case CNF_BIRTHYEAR:		{
				MAnnivDate mda;
				result = mda.DBGetBirthDate(ci->hContact, ci->szProto);
				if (result == 0) {
					ci->wVal = (WORD) mda.Year();
					ci->type = CNFT_WORD;
				}
			} break;

		case CNF_BIRTHDATE:		{
				MAnnivDate mda;
				result = mda.DBGetBirthDate(ci->hContact, ci->szProto);
				if (result == 0) {
					SYSTEMTIME st = mda.SystemTime();
					ci->pszVal = NULL;
					if (ci->dwFlag & CNF_UNICODE) {
						WCHAR wszDate[80];
						if (GetDateFormatW(LOCALE_USER_DEFAULT, wParam == 1 ? DATE_LONGDATE : DATE_SHORTDATE, &st, NULL, wszDate, SIZEOF(wszDate))) {
							ci->pszVal = (LPTSTR)mir_wcsdup(wszDate);
						}
					}
					else {
						CHAR szDate[80];
						if (GetDateFormatA(LOCALE_USER_DEFAULT, wParam == 1 ? DATE_LONGDATE : DATE_SHORTDATE, &st, NULL, szDate, SIZEOF(szDate))) {
							ci->pszVal = (LPTSTR)mir_strdup(szDate);
						}
					}
					ci->type = (ci->pszVal != NULL) ? CNFT_ASCIIZ : 0;
					result = ci->type == 0;
				}
			} break;

		case CNF_TIMEZONE:		{
				//use new core tz interface
				if(tmi.prepareList) {
					HANDLE hTz = tmi.createByContact(ci->hContact, TZF_KNOWNONLY);
					if (hTz) {
						LPTIME_ZONE_INFORMATION tzi = tmi.getTzi(hTz);
						int offset = tzi->Bias + tzi->StandardBias;

						char str[80];
						mir_snprintf(str, SIZEOF(str), offset ? "UTC%+d:%02d" : "UTC", offset / -60, abs(offset % 60));
						ci->pszVal =	ci->dwFlag & CNF_UNICODE
										? (TCHAR*)mir_a2u(str) 
										: (TCHAR*)mir_strdup(str);
						ci->type = CNFT_ASCIIZ;
						return 0;
					}
					else {
						ci->pszVal = NULL;
					}
				}
				//fallback use old UIEX method
				else {
					CTimeZone* ptz = GetContactTimeZone(ci->hContact, ci->szProto);
					if (ptz) {
						if (ci->dwFlag & CNF_UNICODE) {
							ci->pszVal = (LPTSTR) mir_t2u(ptz->ptszDisplay);
						}
						else {
							ci->pszVal = (LPTSTR) mir_t2a(ptz->ptszDisplay);
						}
					}
					else {
						/* If a timezone does not exist in CTzMgr, it is a invalid timezone,
						because Windows and CTzMgr know all existing timezones and it
						would not be shown anywhere anyway as UserInfoEx displays only 
						known windows timezones in the details dialog!
						*/
						ci->pszVal = NULL;
					}
				}
				ci->type = (ci->pszVal != NULL) ? CNFT_ASCIIZ : 0;
				result = ci->type == 0;
			} break;

		//
		// information about IM specific stuff
		//
		case CNF_UNIQUEID:		{
				// protocol must define a PFLAG_UNIQUEIDSETTING
				result = CallProtoService(ci->szProto, PS_GETCAPS, PFLAG_UNIQUEIDSETTING, 0);
				if (result != CALLSERVICE_NOTFOUND && result != NULL) {
					result = GCIVar(ci, (LPCSTR) result);
				}
			} break;

		case CNF_DISPLAYUID:	{
				if (!GCIVar(ci, "display_uid"))
					result=0;
				else {
					result = CallProtoService(ci->szProto,PS_GETCAPS,PFLAG_UNIQUEIDSETTING,0);
					if (result != CALLSERVICE_NOTFOUND && result != NULL) {
						result = GCIVar(ci, (LPCSTR) result);
					}
				}
			} break;

		case CNF_DISPLAYNC:
		case CNF_DISPLAY:		{
				INT i;
				for (i = 0; i < NAMEORDERCOUNT; i++) {
					switch (gNameOrder[i]) {
					case 0: // custom name
						{
							// make sure we aren't in CNF_DISPLAYNC mode
							// don't get custom name for NULL contact
							if (ci->hContact != NULL && (ci->dwFlag & 0x7F) == CNF_DISPLAY) {
								BYTE dwFlag = ci->dwFlag;
								ci->dwFlag = (ci->dwFlag & CNF_UNICODE) | CNF_CUSTOMNICK;
								if (!GetContactInfo(NULL, (LPARAM)ci)) {
									ci->dwFlag = dwFlag;
									return 0;
								}
								ci->dwFlag = dwFlag;
							}
						} break;
					case 1: // nick
						{
							if (!GCIVarEx(ci, SET_CONTACT_NICK))
								return 0;
						} break;
					case 2: // First Name
						{
							if (!GCIVarEx(ci, SET_CONTACT_FIRSTNAME))
								return 0;
						} break;
					case 3: // E-mail
						{
							if (!GCIVarEx(ci, SET_CONTACT_EMAIL))
								return 0;
						} break;
					case 4: // Last Name
						{
							if (!GCIVarEx(ci, SET_CONTACT_LASTNAME))
								return 0;
						} break;
					case 5: // Unique id
						{
							// protocol must define a PFLAG_UNIQUEIDSETTING
							result = CallProtoService(ci->szProto, PS_GETCAPS, PFLAG_UNIQUEIDSETTING, 0);
							if (result != CALLSERVICE_NOTFOUND && result != NULL) {
								if (!GCIStr(ci, (LPCSTR) result))
									return 0;
							}
						} break;
					case 6: // first + last name
						{
							if (!GCIFirstLast(ci))
								return 0;
						} break;
					default: // unknown contact
						{
							if (ci->dwFlag & CNF_UNICODE) {
								ci->pszVal = (LPTSTR) mir_wcsdup(TranslateW(L"'(Unknown Contact)'"));
							}
							else {
								ci->pszVal = (LPTSTR) mir_strdup(Translate("'(Unknown Contact)'"));
							}
							ci->type = (ci->pszVal != NULL) ? CNFT_ASCIIZ : 0;
							return ci->type == 0;
						}
					}
				}
			}

		default:				{
				result = 1;
			}
		}
	}
	else
	{
		result = 1;
	}
	return result;
}

/**
 * This is the implementation of the MS_DB_CONTACT_GETSETTING_STR_EX service.
 *
 * @param	wParam		- handle of the contact a setting was written for (must be NULL in this case)
 * @param	lParam		- DBCONTACTGETSETTING structure holding information about the setting to read
 *
 * @retval	0 - success
 * @retval	1 - error
 **/
static INT_PTR GetContactSettingStrExService(WPARAM wParam, LPARAM lParam)
{
	DBCONTACTGETSETTING *cgs = (DBCONTACTGETSETTING*)lParam;
	return DB::Setting::GetEx((HANDLE)wParam, USERINFO, 
		cgs->szModule, cgs->szSetting, cgs->pValue, cgs->pValue->type);
}

/**
 * If the user changes the name order update the global value.
 *
 * @param	wParam		- handle of the contact a setting was written for (must be NULL in this case)
 * @param	lParam		- DBCONTACTWRITESETTING structure holding information about the written data
 * @return	0
 **/
static INT OnSettingChanged(WPARAM wParam, LPARAM lParam)
{
	if ((HANDLE)wParam == NULL) {
		DBCONTACTWRITESETTING *pdbcws = (DBCONTACTWRITESETTING*) lParam;
		if (!mir_strcmp(pdbcws->szModule, "Contact") &&
			!mir_strcmp(pdbcws->szSetting, "NameOrder"))
		{
			memcpy(gNameOrder, pdbcws->value.pbVal,pdbcws->value.cpbVal);
		}
	}
	return 0;
}

/**
 * Loads the module at startup and replaces the service.
 *
 * @param	none
 * @return	nothing
 **/
VOID SvcContactInfoLoadModule()
{
	myCreateServiceFunction(MS_DB_CONTACT_GETSETTING_STR_EX, GetContactSettingStrExService);

	if (DB::Setting::GetByte(SET_GETCONTACTINFO_ENABLED, DEFVAL_GETCONTACTINFO_ENABLED)) 
	{
		if (!myDestroyServiceFunction(MS_CONTACT_GETCONTACTINFO)) {
			if (myCreateServiceFunction(MS_CONTACT_GETCONTACTINFO, GetContactInfo)) {
				DBVARIANT dbv;
				if (DB::Setting::GetAString(NULL, "Contact", "NameOrder", &dbv)) {
					BYTE i;
					for (i = 0; i < NAMEORDERCOUNT; i++) {
						gNameOrder[i] = i;
					}
				}
				else {
					memcpy(gNameOrder, dbv.pbVal, dbv.cpbVal);
					DB::Variant::Free(&dbv);
				}
				hSettingChanged = HookEvent(ME_DB_CONTACT_SETTINGCHANGED, OnSettingChanged);
			}
		}
	}
}
