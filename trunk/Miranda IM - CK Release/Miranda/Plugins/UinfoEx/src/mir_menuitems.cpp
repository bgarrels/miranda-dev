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

#include "commonheaders.h"
#include "svc_Reminder.h"

#include "svc_email.h"
#include "svc_homepage.h"
#include "..\ex_import\svc_ExImport.h"

enum ECascadeType {
	MCAS_DISABLED	= 2,
	MCAS_ALL		= 4,
	MCAS_EXIMPORT	= 8,
	MCAS_NOTINITIATED = 128
};

INT hMenuItemRefresh		= NULL;
HGENMENU *hMenuItemAccount	= NULL;

/**
 * Helper function to remove all menu items backward (first item second group).
 * Note: since miranda 0.8 genmenu is part of core (we don't need to check for).
 *
 * @param	pItems		- poiter to HGENMENU array
 * @param	Count		- number of array elements
 *
 * @return	0 on success, -1 on failure
 **/
INT_PTR RemoveMenuItems(HGENMENU * pItems, int Count)
{
	if (!Count || !pItems) {
		return -1;
	}
	while (Count--) {
		if (pItems[Count]) {
			CallService(MO_REMOVEMENUITEM, (WPARAM)pItems[Count], 0);
			pItems[Count] = NULL;
		}
	}
	return 0;
}

/**
 * This function rebuilds the contactmenu. If user selected to cascade menus,
 * a root menu item is created which holds the popup for all the other items.
 * Note: since miranda 0.8 genmenu is part of core (we don't need to check for).
 *
 * @param	none
 *
 * @return	nothing
 **/
VOID RebuildContact()
{
	int flag = 0;
	BYTE item = 0;
	CLISTMENUITEM mi;

	HGENMENU mhRoot = HGENMENU_ROOT;
	HGENMENU mhExIm = HGENMENU_ROOT;
	static HGENMENU hMenuItem[4] = {NULL, NULL, NULL, NULL };

	SvcEMailRebuildMenu();
	SvcHomepageRebuildMenu();

	// load options
	flag = DB::Setting::GetByte(SET_MI_CONTACT, MCAS_NOTINITIATED);
	if (flag == MCAS_NOTINITIATED){
		flag = MCAS_EXIMPORT|TRUE;
		DB::Setting::WriteByte(SET_MI_CONTACT, flag);
	}

	// delete all MenuItems and set all bytes 0 to avoid problems
	RemoveMenuItems (hMenuItem, SIZEOF(hMenuItem));
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);

	// support new genmenu style
	mi.flags = CMIF_ROOTHANDLE;
	mi.hParentMenu = HGENMENU_ROOT;

	switch (flag)
	{
		case 3:
			//cascade off
			mhRoot = mhExIm = HGENMENU_ROOT;
			hMenuItem[item++] = NULL;
			break;
		case 5:
			//cascade all
			mi.position = 1000050000;
			mi.popupPosition = 1000050000;
			mi.hIcon = (HICON)LoadImage(ghInst, MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0);
			mi.pszName = LPGEN(MODULELONGNAME);
			mhRoot = (HGENMENU) CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);
			hMenuItem[item++] = mhRoot;
			mhExIm = mhRoot;
			break;
		case 9:
			//cascade Ex/Import
			mi.position = 1000050100;
			mi.popupPosition = 1000050100;
			mi.hIcon = IcoLib_GetIcon(ICO_BTN_EXIMPORT);
			mi.pszName = LPGEN("Ex-/Import contact");
			mhExIm = (HGENMENU) CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);
			hMenuItem[item++] = mhExIm;
			mhRoot = HGENMENU_ROOT;
			break;
		default:
			//disable Menue
			return;
	}
	mi.popupPosition = NULL;

	// ContactDetailsPS's menuitem
	{
		mi.hParentMenu = mhRoot;
		mi.pszService = MS_USERINFO_SHOWDIALOG;
		mi.pszName = LPGEN("User &Details");
		mi.position = 1000050000;
		mi.hIcon = (HICON)LoadImage(ghInst, MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0);
		mi.hotKey = MAKELPARAM(VK_F3, MOD_ALT);
		hMenuItem[item++] = (HGENMENU) CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);
		mi.hotKey = NULL;
	}

	// VCard's Ex/Import menuitems
	{	mi.hParentMenu = mhExIm;

		// Export
		mi.pszService = MS_USERINFO_VCARD_EXPORT;
		mi.pszName = mhExIm != HGENMENU_ROOT ? LPGEN("&Export") : LPGEN("&Export User Details");
		mi.position = 1000050200;
		mi.hIcon = IcoLib_GetIcon(ICO_BTN_EXPORT);
		hMenuItem[item++] = (HGENMENU) CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);

		// Import
		mi.pszService = MS_USERINFO_VCARD_IMPORT;
		mi.pszName = mhExIm != HGENMENU_ROOT ? LPGEN("&Import") : LPGEN("&Import User Details");
		mi.position = 1000050300;
		mi.hIcon = IcoLib_GetIcon(ICO_BTN_IMPORT);
		hMenuItem[item++] = (HGENMENU) CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);
	}
}

/**
 * This function rebuilds the mainmenu. If user selected to cascade menus,
 * a root menu item is created which holds the popup for all the other items.
 * Note: since miranda 0.8 genmenu is part of core (we don't need to check for).
 *
 * @param	none
 *
 * @return	nothing
 **/
VOID RebuildMain()
{
	int flag = 0;
	BYTE item = 0;
	CLISTMENUITEM mi;

	HGENMENU mhRoot = HGENMENU_ROOT;
	HGENMENU mhExIm = HGENMENU_ROOT;
	static HGENMENU hMenuItem[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

	// load options
	flag = DB::Setting::GetByte(SET_MI_MAIN, MCAS_NOTINITIATED);
	if (flag == MCAS_NOTINITIATED){
		flag = MCAS_ALL|TRUE;
		DB::Setting::WriteByte(SET_MI_MAIN, flag);
	}

	// delete all MenuItems and set all bytes 0 to avoid problems
	RemoveMenuItems (hMenuItem, SIZEOF(hMenuItem));
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);

	// support new genmenu style
	mi.flags = CMIF_ROOTHANDLE;
	mi.hParentMenu = HGENMENU_ROOT;

	switch (flag)
	{
		case 3:
			//cascade off
			mhRoot = mhExIm = HGENMENU_ROOT;
			hMenuItem[item++] = NULL;
			break;
		case 5:
			//cascade all
			mi.position = 500050000;
			mi.popupPosition = 500050000;
			mi.hIcon = (HICON)LoadImage(ghInst, MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0);
			mi.pszName = LPGEN(MODULELONGNAME);
			mhRoot = (HGENMENU) CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi);
			hMenuItem[item++] = mhRoot;
			mhExIm = mhRoot;
			break;
		case 9:
			//cascade Ex/Import
			mi.position = 500050000;
			mi.popupPosition = 500050000;
			mi.hIcon = IcoLib_GetIcon(ICO_BTN_EXIMPORT);
			mi.pszName = LPGEN("Ex-/Import contact");
			mhExIm = (HGENMENU) CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi);
			hMenuItem[item++] = mhExIm;
			mhRoot = HGENMENU_ROOT;
			break;
		default:
			//disable Menue
			return;
	}
	mi.popupPosition = NULL;

	// details dialog
	{
		mi.hParentMenu = mhRoot;
		mi.pszService = MS_USERINFO_SHOWDIALOG;
		mi.pszName = LPGEN("View/Change My &Details...");
		mi.position = 500050000;
		mi.hIcon = (HICON)LoadImage(ghInst, MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0);
		hMenuItem[item++] = (HGENMENU) CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi);
	}

	// VCard's Ex/Import menuitems
	{	mi.hParentMenu = mhExIm;

		// Export
		mi.pszService = MS_USERINFO_VCARD_EXPORTALL;
		mi.pszName = LPGEN("Export all contacts");
		mi.position = 500150000;
		mi.hIcon = IcoLib_GetIcon(ICO_BTN_EXPORT);
		hMenuItem[item++] = (HGENMENU) CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi);

		// Import
		mi.pszService = MS_USERINFO_VCARD_IMPORTALL;
		mi.pszName = LPGEN("Import all contacts");
		mi.position = 500151000;
		mi.hIcon = IcoLib_GetIcon(ICO_BTN_IMPORT);
		hMenuItem[item++] = (HGENMENU) CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi);
	}

	mi.hParentMenu = mhRoot;

	// reminder
	{
		const BOOLEAN bRemindMenus =
			DB::Setting::GetByte(SET_REMIND_ENABLED, DEFVAL_REMIND_ENABLED) &&
			DB::Setting::GetByte(SET_REMIND_MENUENABLED, DEFVAL_REMIND_MENUENABLED);
		if (bRemindMenus) {
			// make backup of each protocol based birthday
			mi.pszService = MS_USERINFO_REMINDER_AGGRASIVEBACKUP;
			mi.pszName = LPGEN("Backup birthdays");
			mi.position = 500253000;
			mi.hIcon = IcoLib_GetIcon(ICO_BTN_BDAY_BACKUP);
			hMenuItem[item++] = (HGENMENU) CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi);
			// Check anniversaries
			mi.pszService = MS_USERINFO_REMINDER_CHECK;
			mi.pszName = LPGEN("Check anniversaries");
			mi.position = 500251000;
			mi.hIcon = IcoLib_GetIcon(ICO_BTN_SEARCH);
			hMenuItem[item++] = (HGENMENU) CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi);
		}
		else {
			hMenuItem[item++] = NULL;
			hMenuItem[item++] = NULL;
		}
		// Refresh Contact Details
		mi.pszService = MS_USERINFO_REFRESH;
		mi.pszName = LPGEN("Refresh Contact Details");
		mi.position = 500254000;
		mi.hIcon = IcoLib_GetIcon(ICO_BTN_UPDATE);
		hMenuItem[item++] = (HGENMENU) CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi);
	}

	// anniversary list
	{
		mi.pszService = MS_USERINFO_REMINDER_LIST;
		mi.pszName = LPGEN("Anniversary list");
		mi.position = 500252000;
		mi.hIcon = IcoLib_GetIcon(ICO_COMMON_ANNIVERSARY);
		hMenuItem[item++] = (HGENMENU) CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi);
	}
}

/**
 * This function rebuilds the clist context menu (clist main groupmenu). If user selected to
 * cascade menus, a root menu item is created which holds the popup for all the other items.
 * Note: since miranda 0.8 genmenu is part of core (we don't need to check for).
 *
 * @param	none
 *
 * @return	nothing
 **/
VOID RebuildGroup()
{
	int flag = 0;
	BYTE item = 0;
	CLISTMENUITEM mi;
	GroupMenuParam gmp = {0};

	HGENMENU mhRoot = HGENMENU_ROOT;
	HGENMENU mhExIm = HGENMENU_ROOT;
	static HGENMENU hMenuItem[3] = {NULL, NULL, NULL };

	// load options
	flag = DB::Setting::GetByte(SET_MI_GROUP, MCAS_NOTINITIATED);
	if (flag == MCAS_NOTINITIATED){
		flag = MCAS_EXIMPORT|TRUE;
		DB::Setting::WriteByte(SET_MI_GROUP, flag);
	}

	// delete all MenuItems and set all bytes 0 to avoid problems
	RemoveMenuItems (hMenuItem, SIZEOF(hMenuItem));
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);

	// create service name main (prevent to generate {(Null)/Ex-/Import Group} in db) and set pointer to end it
	char text[ 200 ];
	strcpy( text, "UserInfo");
	mi.pszService = text;
	char* tDest = text + strlen( text );

	// support new genmenu style
	mi.flags = CMIF_ROOTHANDLE;
	mi.hParentMenu = HGENMENU_ROOT;

	switch (flag)
	{
		case 3:
			//cascade off
			mhRoot = mhExIm = HGENMENU_ROOT;
			hMenuItem[item++] = NULL;
			break;
		case 5:
			//cascade all
			mi.position = 250000;
			mi.popupPosition = 250000;
			mi.hIcon = (HICON)LoadImage(ghInst, MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0);
			mi.pszName = LPGEN(MODULELONGNAME);
			mhRoot = (HGENMENU) CallService(MS_CLIST_ADDGROUPMENUITEM, 0, (LPARAM)&mi);
			hMenuItem[item++] = mhRoot;
			mhExIm = mhRoot;
			break;
		case 9:
			//cascade Ex/Import
			mi.position = 250100;
			mi.popupPosition = 250100;
			mi.hIcon = IcoLib_GetIcon(ICO_BTN_EXIMPORT);
			mi.pszName = LPGEN("Ex-/Import contact");
			mhExIm = (HGENMENU) CallService(MS_CLIST_ADDGROUPMENUITEM, 0, (LPARAM)&mi);
			hMenuItem[item++] = mhExIm;
			mhRoot = HGENMENU_ROOT;
			break;
		default:
			//disable Menue
			return;
	}
	mi.popupPosition = NULL;

	// VCard's Ex/Import menuitems
	{	mi.hParentMenu = mhExIm;

		// Export
		mi.pszService = MS_USERINFO_VCARD_EXPORTALL;
		mi.pszName = LPGEN("Export all contacts");
		mi.position = 250200;
		mi.hIcon = IcoLib_GetIcon(ICO_BTN_EXPORT);
		hMenuItem[item++] = (HGENMENU) CallService(MS_CLIST_ADDGROUPMENUITEM, (WPARAM)&gmp, (LPARAM)&mi);

		// Import
		mi.pszService = MS_USERINFO_VCARD_IMPORTALL;
		mi.pszName = LPGEN("Import all contacts");
		mi.position = 250300;
		mi.hIcon = IcoLib_GetIcon(ICO_BTN_IMPORT);
		hMenuItem[item++] = (HGENMENU) CallService(MS_CLIST_ADDGROUPMENUITEM, (WPARAM)&gmp, (LPARAM)&mi);
	}
}

/******************************
 * (Sub)GroupMenu
 ******************************/

/**
 * This function rebuilds the group context menu (clist main groupmenu). If user selected to
 * cascade menus, a root menu item is created which holds the popup for all the other items.
 * Note: since miranda 0.8 genmenu is part of core (we don't need to check for).
 *
 * @param	none
 *
 * @return	nothing
 **/
VOID RebuildSubGroup()
{
	int flag = 0;
	BYTE item = 0;
	CLISTMENUITEM mi;
	GroupMenuParam gmp = {0};

	HGENMENU mhRoot = HGENMENU_ROOT;
	HGENMENU mhExIm = HGENMENU_ROOT;
	static HGENMENU hMenuItem[3] = {NULL, NULL, NULL };

	// load options
	flag = DB::Setting::GetByte(SET_MI_SUBGROUP, MCAS_NOTINITIATED);
	if (flag == MCAS_NOTINITIATED){
		flag = MCAS_DISABLED|TRUE;
		DB::Setting::WriteByte(SET_MI_SUBGROUP, flag);
	}

	// delete all MenuItems and set all bytes 0 to avoid problems
	RemoveMenuItems (hMenuItem, SIZEOF(hMenuItem));
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);

	// create service name main (prevent to generate {(Null)/Ex-/Import Group} in db) and set pointer to end it
	char text[ 200 ];
	strcpy( text, "UserInfo");
	mi.pszService = text;
	char* tDest = text + strlen( text );

	// support new genmenu style
	mi.flags = CMIF_ROOTHANDLE;
	mi.hParentMenu = HGENMENU_ROOT;

	switch (flag)
	{
		case 3:
			//cascade off
			mhRoot = mhExIm = HGENMENU_ROOT;
			hMenuItem[item++] = NULL;
			break;
		case 5:
			//cascade all
			mi.position = 1050000;
			mi.popupPosition = 1050000;
			mi.hIcon = (HICON)LoadImage(ghInst, MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0);
			mi.pszName = LPGEN(MODULELONGNAME);
			mhRoot = (HGENMENU) CallService(MS_CLIST_ADDSUBGROUPMENUITEM, 0, (LPARAM)&mi);
			hMenuItem[item++] = mhRoot;
			mhExIm = mhRoot;
			break;
		case 9:
			//cascade Ex/Import
			mi.position = 1050100;
			mi.popupPosition = 1050100;
			mi.hIcon = IcoLib_GetIcon(ICO_BTN_EXIMPORT);
			mi.pszName = LPGEN("Ex-/Import Group");
			mhExIm = (HGENMENU) CallService(MS_CLIST_ADDSUBGROUPMENUITEM, 0, (LPARAM)&mi);
			hMenuItem[item++] = mhExIm;
			mhRoot = HGENMENU_ROOT;
			break;
		default:
			//disable Menue
			return;
	}
	mi.popupPosition = NULL;

	// VCard's Ex/Import menuitems
	{	mi.hParentMenu = mhExIm;

		// Export
		strcpy( tDest, "/ExportGroup");		//mi.pszService
		if(!ServiceExists(mi.pszService)) myCreateServiceFunction(mi.pszService, svcExIm_Group_Service);
		mi.pszName = mhExIm != HGENMENU_ROOT ? LPGEN("&Export") : LPGEN("&Export Group");
		mi.position = 1050200;
		mi.hIcon = IcoLib_GetIcon(ICO_BTN_EXPORT);
		gmp.lParam=0;
		gmp.wParam=TRUE;
		hMenuItem[item++] = (HGENMENU) CallService(MS_CLIST_ADDSUBGROUPMENUITEM, (WPARAM)&gmp, (LPARAM)&mi);

		// Import
		strcpy( tDest, "/ImportGroup");		//mi.pszService
		if(!ServiceExists(mi.pszService)) myCreateServiceFunction(mi.pszService, svcExIm_Group_Service);
		mi.pszName = mhExIm != HGENMENU_ROOT ? LPGEN("&Import") : LPGEN("&Import Group");
		mi.position = 1050300;
		mi.hIcon = IcoLib_GetIcon(ICO_BTN_IMPORT);
		gmp.lParam=0;
		gmp.wParam=FALSE;
		hMenuItem[item++] = (HGENMENU) CallService(MS_CLIST_ADDSUBGROUPMENUITEM, (WPARAM)&gmp, (LPARAM)&mi);
	}
}

/******************************
 * Account Menu
 ******************************/

/**
 * This function rebuilds the account context menu (clist status <account>). If user selected to
 * cascade menus, a root menu item is created which holds the popup for all the other items.
 * Note: since miranda 0.8 genmenu is part of core (we don't need to check for).
 *
 * @param	wParam				- 0 not used
 * @param	lParam				- clear bit for old menu items
 *								  0 don't delete old items (its calld by ME_CLIST_PREBUILDSTATUSMENU hook)
 *								  other then 0 delete old items first
 *
 * @return	always 0
 **/
INT_PTR RebuildAccount(WPARAM wParam, LPARAM lParam)
{
	const BYTE mItems = 3;				// menuitems to create
	int flag = 0, mProtoCount = 0;
	BYTE i = 0, item = 0;
	TCHAR sztName[MAXSETTING];
	PROTOACCOUNT* pAccountName = NULL;
	CLISTMENUITEM mi;

	mProtoCount = pcli->menuProtoCount;

	HGENMENU mhRoot = HGENMENU_ROOT;
	HGENMENU mhExIm = HGENMENU_ROOT;

	// on call by hook or first start
	if (!lParam || !hMenuItemAccount) {
		size_t sizeNew = mItems * mProtoCount * sizeof(HGENMENU);
		hMenuItemAccount = (HGENMENU*) mir_realloc(hMenuItemAccount,sizeNew);
		// set all bytes 0 to avoid problems
		memset(hMenuItemAccount, 0, sizeNew);
	}
	// on options change
	else {
		// delete all MenuItems backward (first item second group)
		RemoveMenuItems (hMenuItemAccount, mItems * mProtoCount);
	}

	// load options
	flag = DB::Setting::GetByte(SET_MI_ACCOUNT, MCAS_NOTINITIATED);
	if (flag == MCAS_NOTINITIATED){
		flag = MCAS_EXIMPORT|TRUE;
		DB::Setting::WriteByte(SET_MI_ACCOUNT, flag);
	}


	// loop for all account names
	for (i = 0; i < mProtoCount; i++) {

		// set all bytes 0 to avoid problems
		item = 0;
		mhRoot = 0;
		ZeroMemory(&mi, sizeof(mi));
		mi.cbSize = sizeof(mi);

		mhRoot = pcli->menuProtos[i].pMenu;
		if ( mhRoot == NULL )
			break;
		pAccountName = ProtoGetAccount(pcli->menuProtos[i].szProto);

		// create service name main (account module name) and set pointer to end it
		char text[ 200 ];
		strcpy( text, pcli->menuProtos[i].szProto);
		mi.pszService = text;
		char* tDest = text + strlen( text );

		// support new genmenu style
		mi.flags = CMIF_ROOTHANDLE|CMIF_TCHAR|CMIF_KEEPUNTRANSLATED;
		mi.hParentMenu = mhRoot;

		switch (flag)
		{
			case 3:
				//cascade off
				mhExIm = mhRoot;
				// seperator
				mi.position = 50100;
				hMenuItemAccount[mItems*i + item++]  = (HGENMENU) CallService( MS_CLIST_ADDSTATUSMENUITEM, 0, (LPARAM)&mi);
				break;
			case 5:
				//cascade all
				mi.position = 50100;
				mi.hIcon = (HICON)LoadImage(ghInst, MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0);
				mi.ptszName = TranslateT(MODULELONGNAME);
				hMenuItemAccount[mItems*i + item] = (HGENMENU) CallService(MS_CLIST_ADDSTATUSMENUITEM, 0, (LPARAM)&mi);
				mhRoot = hMenuItemAccount[mItems*i + item++];
				mhExIm = mhRoot;
				break;
			case 9:
				//cascade Ex/Import
				mi.position = 50100;
				mi.hIcon = IcoLib_GetIcon(ICO_BTN_EXIMPORT);
				mir_sntprintf(sztName, SIZEOF(sztName),_T("%s %s"), pAccountName->tszAccountName, TranslateT("Ex-/Import"));
				mi.ptszName = sztName;
				hMenuItemAccount[mItems*i + item] = (HGENMENU) CallService(MS_CLIST_ADDSTATUSMENUITEM, 0, (LPARAM)&mi);
				mhExIm = hMenuItemAccount[mItems*i + item++];
				break;
			default:
				//disable Menue
				return 0;
		}

		// VCard's Ex/Import menuitems
		{
			mi.hParentMenu = mhExIm;

			// Export
			strcpy( tDest, "/ExportAccount");		//mi.pszService
			if(!ServiceExists(mi.pszService)) myCreateServiceFunction(mi.pszService, svcExIm_Account_Service);
			mir_sntprintf(sztName, SIZEOF(sztName),_T("%s %s"), pAccountName->tszAccountName, TranslateT("&Export"));
			mi.ptszName = sztName;
			mi.position = 50200;
			mi.hIcon = IcoLib_GetIcon(ICO_BTN_EXPORT);
			hMenuItemAccount[mItems*i + item++] = (HGENMENU) CallService(MS_CLIST_ADDSTATUSMENUITEM, 0, (LPARAM)&mi);

			// Import
			strcpy( tDest, "/ImportAccount");		//mi.pszService
			if(!ServiceExists(mi.pszService)) myCreateServiceFunction(mi.pszService, svcExIm_Account_Service);
			mir_sntprintf(sztName, SIZEOF(sztName),_T("%s %s"), pAccountName->tszAccountName, TranslateT("&Import"));
			mi.ptszName = sztName;
			mi.position = 50300;
			mi.hIcon = IcoLib_GetIcon(ICO_BTN_IMPORT);
			hMenuItemAccount[mItems*i + item++] = (HGENMENU) CallService(MS_CLIST_ADDSTATUSMENUITEM, 0, (LPARAM)&mi);
		}
	}
	return 0;
}


VOID RebuildMenu()
{
	RebuildMain();
	RebuildContact();
	RebuildGroup();
	RebuildSubGroup();
	RebuildAccount(NULL, 1);
	return;
}
