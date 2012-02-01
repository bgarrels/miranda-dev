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

File name      : $HeadURL: http://userinfoex.googlecode.com/svn/trunk/svc_phone.cpp $
Revision       : $Revision: 187 $
Last change on : $Date: 2010-09-08 16:05:54 +0400 (Ср, 08 сен 2010) $
Last change by : $Author: ing.u.horn $

===============================================================================
*/
#include "commonheaders.h"
#include "m_icq.h"

enum EPhoneType 
{
	PHONE_NONE,
	PHONE_NORMAL,
	PHONE_SMS
};

static HANDLE ghMenuItem			= NULL;
static HANDLE ghExtraIconDef[2]		= { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE };
static HANDLE ghExtraIconSvc		= INVALID_HANDLE_VALUE;

static HANDLE hChangedHook			= NULL;
static HANDLE hApplyIconHook		= NULL;
static HANDLE hRebuildIconsHook		= NULL;

/**
 * This function reads the contact's phone number from database and returns its type.
 *
 * @param	 hContact		- handle to contact to read email from
 *
 * @retval	PHONE_SMS:		The phone supports sms, so is a cellular
 * @retval	PHONE_NORMAL:	The phone is a normal phone
 * @retval	PHONE_NONE:		The contact does not provide any phone number
 **/
static INT_PTR Get(HANDLE hContact)
{
	INT_PTR nType = PHONE_NONE;

	// ignore owner
	if (hContact != NULL) 
	{
		LPCSTR pszProto = DB::Contact::Proto(hContact);
		if (pszProto != NULL) 
		{
			LPCSTR	e[2][4] = {
				{ SET_CONTACT_CELLULAR,			SET_CONTACT_PHONE,			"MyPhone0"			},
				{ SET_CONTACT_COMPANY_CELLULAR,	SET_CONTACT_COMPANY_PHONE,	"MyCompanyPhone0"	}
			};

			INT i, j;
			LPSTR pszPhone;

			for (i = 0; (i < 2) && (nType == PHONE_NONE); i++)
			{
				for (j = 0; (j < 3) && (nType == PHONE_NONE); j++)
				{
					pszPhone = DB::Setting::GetAStringEx(hContact, USERINFO, pszProto, e[i][j]);
					if (pszPhone)
					{
						nType = (strstr(pszPhone, " SMS")) ? PHONE_SMS : PHONE_NORMAL;
						MIR_FREE(pszPhone);
						break;
					}
				}
			}
		}
	}
	return nType;
}

/***********************************************************************************************************
 * Event Handler functions
 ***********************************************************************************************************/

static INT OnCListRebuildIcons(WPARAM wParam, LPARAM lParam)
{
	HICON hIcon			= IcoLib_GetIcon(ICO_BTN_PHONE);
	ghExtraIconDef[0]	= (HANDLE)CallService(MS_CLIST_EXTRA_ADD_ICON, (WPARAM)hIcon, 0);
	CallService(MS_SKIN2_RELEASEICON,(WPARAM)hIcon,0);
	hIcon				= IcoLib_GetIcon(ICO_BTN_CELLULAR);
	ghExtraIconDef[1]	= (HANDLE)CallService(MS_CLIST_EXTRA_ADD_ICON, (WPARAM)hIcon, 0);
	CallService(MS_SKIN2_RELEASEICON,(WPARAM)hIcon,0);
	return 0;
}

/**
 * Notification handler for clist extra icons to be applied for a contact.
 *
 * @param	wParam			- handle to the contact whose extra icon is to apply
 * @param	lParam			- not used
 **/
static INT OnCListApplyIcons(HANDLE hContact, LPARAM)
{
	if (!myGlobals.ExtraIconsServiceExist)
	{
		IconExtraColumn iec;
		iec.cbSize = sizeof(IconExtraColumn);
		iec.ColumnType = EXTRA_ICON_SMS;
		switch (Get(hContact)) 
		{
		case PHONE_NORMAL:
			{
				iec.hImage = ghExtraIconDef[0];
			}
			break;

		case PHONE_SMS:
			{
				iec.hImage = ghExtraIconDef[1];
			}
			break;

		default:
			{
				iec.hImage = INVALID_HANDLE_VALUE;
			}
		}
		CallService(MS_CLIST_EXTRA_SET_ICON, (WPARAM)hContact, (LPARAM)&iec);
	}
	else
	{
		EXTRAICON ico;

		ZeroMemory(&ico, sizeof(ico));
		ico.cbSize = sizeof(ico);
		ico.hContact = hContact;
		ico.hExtraIcon = ghExtraIconSvc;
		switch (Get(hContact)) 
		{
		case PHONE_NORMAL:
			{
				ico.icoName = ICO_BTN_PHONE;
			}
			break;

		case PHONE_SMS:
			{
				ico.icoName = ICO_BTN_CELLULAR;
			}
			break;

		default:
			{
				ico.icoName = (char *)0;
			}
		}
		CallService(MS_EXTRAICON_SET_ICON, (WPARAM)&ico, 0);
	}
	return 0;
}

/**
 * Notification handler for changed contact settings
 *
 * @param	wParam			- (HANDLE)hContact
 * @param	lParam			- (DBCONTACTWRITESETTING*)pdbcws
 **/
static INT OnContactSettingChanged(HANDLE hContact, DBCONTACTWRITESETTING* pdbcws)
{
	if (hContact && pdbcws && pdbcws->szSetting && 
			((pdbcws->value.type & DBVTF_VARIABLELENGTH) || (pdbcws->value.type == DBVT_DELETED)) &&
			(!strcmp(pdbcws->szSetting, SET_CONTACT_PHONE) ||
			 !strcmp(pdbcws->szSetting, SET_CONTACT_CELLULAR) ||
			 !strcmp(pdbcws->szSetting, SET_CONTACT_COMPANY_PHONE) ||
			 !strcmp(pdbcws->szSetting, SET_CONTACT_COMPANY_CELLULAR) ||
			 !strncmp(pdbcws->szSetting, "MyPhone0", 8)))
	{
		OnCListApplyIcons(hContact, 0);
	}
	return 0;
}

/***********************************************************************************************************
 * public Module Interface functions
 ***********************************************************************************************************/

/**
 * Force all icons to be reloaded.
 *
 * @param	wParam			- handle to the contact whose extra icon is to apply
 * @param	lParam			- not used
 **/
VOID SvcPhoneApplyCListIcons()
{
	HANDLE hContact;

	//walk through all the contacts stored in the DB
	for (hContact = DB::Contact::FindFirst();	hContact != NULL;	hContact = DB::Contact::FindNext(hContact))
	{
		OnCListApplyIcons(hContact, 0);
	}
}

/**
 * Enable or disable the replacement of clist extra icons.
 *
 * @param	bEnable			- determines whether icons are enabled or not
 * @param	bUpdateDB		- if true the database setting is updated, too.
 **/
VOID SvcPhoneEnableExtraIcons(BOOLEAN bEnable, BOOLEAN bUpdateDB) 
{
	if (myGlobals.HaveCListExtraIcons)
	{
		if (bUpdateDB)
		{
			DB::Setting::WriteByte(SET_CLIST_EXTRAICON_PHONE, bEnable);
		}

		// force module enabled, if extraicon plugin was found
		if (bEnable) 
		{
			// hook events
			if (hChangedHook == NULL) 
			{
				hChangedHook = HookEvent(ME_DB_CONTACT_SETTINGCHANGED, (MIRANDAHOOK)OnContactSettingChanged);
			}
			if (hApplyIconHook == NULL) 
			{
				hApplyIconHook = HookEvent(ME_CLIST_EXTRA_IMAGE_APPLY, (MIRANDAHOOK)OnCListApplyIcons);
			}
			if (myGlobals.ExtraIconsServiceExist)
			{
				if (ghExtraIconSvc == INVALID_HANDLE_VALUE)
				{
					EXTRAICON_INFO ico;
					
					ZeroMemory(&ico, sizeof(ico));
					ico.cbSize = sizeof(ico);
					ico.type = EXTRAICON_TYPE_ICOLIB;
					ico.name = "sms";	//must be the same as the group name in extraicon
					ico.description = "(uinfoex)";
					ico.descIcon = ICO_BTN_CELLULAR;
					ghExtraIconSvc = (HANDLE)CallService(MS_EXTRAICON_REGISTER, (WPARAM)&ico, 0);
				}
			}
			else if (hRebuildIconsHook == NULL) 
			{
				hRebuildIconsHook = HookEvent(ME_CLIST_EXTRA_LIST_REBUILD, (MIRANDAHOOK)OnCListRebuildIcons);
				OnCListRebuildIcons(0, 0);
			}
		}
		else 
		{
			if (hChangedHook)
			{
				UnhookEvent(hChangedHook); 
				hChangedHook = NULL;
			}			
			if (hApplyIconHook)
			{
				UnhookEvent(hApplyIconHook); 
				hApplyIconHook = NULL;
			}			
			if (hRebuildIconsHook)
			{
				UnhookEvent(hRebuildIconsHook); 
				hRebuildIconsHook = NULL;
			}
		}
		SvcPhoneApplyCListIcons();
	}
}

/**
 * This function initially loads the module uppon startup.
 **/
VOID SvcPhoneLoadModule()
{
	SvcPhoneEnableExtraIcons(
		myGlobals.ExtraIconsServiceExist || 
		DB::Setting::GetByte(SET_CLIST_EXTRAICON_PHONE, DEFVAL_CLIST_EXTRAICON_PHONE), FALSE);
}

/**
 * This function unloads the Email module.
 *
 * @param	none
 *
 * @return	nothing
 **/
VOID SvcPhoneUnloadModule()
{	
	// unhook event handlers
	UnhookEvent(hChangedHook);		hChangedHook		= NULL;
	UnhookEvent(hApplyIconHook);	hApplyIconHook		= NULL;
	UnhookEvent(hRebuildIconsHook);	hRebuildIconsHook	= NULL;
}
