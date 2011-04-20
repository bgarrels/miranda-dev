//***************************************************************************************
//
//   Google Extension plugin for the Miranda IM's Jabber protocol
//   Copyright (c) 2011 bems@jabber.org, George Hazan (ghazan@jabber.ru)
//
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//***************************************************************************************

#include "stdafx.h"
#include "menu.h"
#include "resources.h"
#include "options.h"
#include "inbox.h"

static const LPSTR MS_GTALKEXT_OPENMAILBOX = SHORT_PLUGIN_NAME "/OpenMailbox";
static const LPTSTR _T(OPEN_MAILBOX_ITEM_CAPTION) = _T("Open mailbox");

HANDLE hOpenMailboxService = 0;
HANDLE hOpenMailboxMenuItem = 0;
HANDLE hOnPrebuildMenu = 0;

INT_PTR OpenMailboxMenuHandler(WPARAM wParam, LPARAM lParam)
{
	if (DBGetContactSettingByte((HANDLE)wParam, SHORT_PLUGIN_NAME, PSEUDOCONTACT_FLAG, 0))
		OpenContactInbox((HANDLE)wParam);
	return 0;
}

int OnPrebuildMenu(WPARAM wParam, LPARAM lParam)
{
	CLISTMENUITEM cmi = {0};
	cmi.cbSize = sizeof(cmi);
	cmi.flags = CMIM_FLAGS;
	if (!DBGetContactSettingByte((HANDLE)wParam, SHORT_PLUGIN_NAME, PSEUDOCONTACT_FLAG, 0))
		cmi.flags |= CMIF_HIDDEN;
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hOpenMailboxMenuItem, (LPARAM)&cmi);
	return 0;
}

BOOL InitMenus(BOOL init)
{
	if (init) {
		hOpenMailboxService = (HANDLE)CreateServiceFunction(MS_GTALKEXT_OPENMAILBOX, OpenMailboxMenuHandler);
		if (!hOpenMailboxService) {
			InitMenus(FALSE);
			return FALSE;
		}

		extern HICON g_hPopupIcon;

		CLISTMENUITEM cmi = {0};
		cmi.cbSize = sizeof(cmi);
		cmi.flags = CMIF_UNICODE;
		cmi.hIcon = g_hPopupIcon;
		cmi.ptszName = TranslateT(OPEN_MAILBOX_ITEM_CAPTION);
		cmi.pszService = MS_GTALKEXT_OPENMAILBOX;
		hOpenMailboxMenuItem = (HANDLE)CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&cmi);

		if (!hOpenMailboxMenuItem) {
			InitMenus(FALSE);
			return FALSE;
		}

		hOnPrebuildMenu = HookEvent(ME_CLIST_PREBUILDCONTACTMENU, OnPrebuildMenu);
		if (!hOnPrebuildMenu) {
			InitMenus(FALSE);
			return FALSE;
		}
	}
	else {
		if (hOnPrebuildMenu) {
			UnhookEvent(hOnPrebuildMenu);
			hOnPrebuildMenu = 0;
		}
		if (hOpenMailboxService) {
			DestroyServiceFunction(hOpenMailboxService);
			hOpenMailboxService = 0;
		}
	}

	return TRUE;
}