/*
Smart Auto Replier (SAR) for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2004 - 2012 by Volodymyr M. Shcherbyna <volodymyr@shcherbyna.com>

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

#include "stdafx.h"
#include "resource.h"

/// these dlg are necessary here..
#include "gui\settingsdlgholder.h"
#include "gui\EditReplyDlg.h" 
#include "gui\addruledlg.h"

#include "../updater/m_updater.h"

/// let's advertise myselft and this plugin ;)
#define ICQPROTONAME		"SAR"
#ifdef WIN64
#define ICQPROTODECRSHORT   "Smart Auto Replier x64 (Unicode)"
#define PLUGIN_UUID { 0x98f73c6, 0xd963, 0x4c2f, { 0x88, 0x38, 0x97, 0xa0, 0xec, 0x92, 0x69, 0xe0 } } // {098F73C6-D963-4C2F-8838-97A0EC9269E0}
#elif( _UNICODE )
#define ICQPROTODECRSHORT   "Smart Auto Replier (Unicode)"
#define PLUGIN_UUID { 0x9e536082, 0x17e, 0x423b, { 0xbf, 0x4f, 0xde, 0xdf, 0xeb, 0x9b, 0x3b, 0x60 } } // {9E536082-017E-423B-BF4F-DEDFEB9B3B60}
#else
#define ICQPROTODECRSHORT   "Smart Auto Replier"
#define PLUGIN_UUID { 0x1b7152c1, 0xcdef, 0x4d39, { 0xa2, 0x6e, 0xef, 0x71, 0x7a, 0x4c, 0x79, 0x21 } } // {1B7152C1-CDEF-4D39-A26E-EF717A4C7921}
#endif
#define ICQPROTODECR		"Plugin is able to reply on all incoming messages, making possible use of rules that are applied to specific contacts. Plugin allows to use Lua scripts as a rules, thus allowing user to make virtually any type of customizations."
#define DEVNAME				"Volodymyr M. Shcherbyna"
#define DEVMAIL				"volodymyr@shcherbyna.com"
#define DEVCOPYRIGHT		"© 2004-2012 Volodymyr M. Shcherbyna, www.shcherbyna.com"
#define DEVWWW				"http://www.shcherbyna.com/forum/viewforum.php?f=8"
#define GLOB_HOOKS			3

/// global menu items strings...
#define MENU_ITEM_DISABLE_CAPTION	LPGENT("Disable Smart Auto Replier")
#define MENU_ITEM_ENABLE_CAPTION	LPGENT("Enable Smart Auto Replier")

//#define 
/// global data...
HINSTANCE		hInst = NULL;				/// hinstance 
PLUGINLINK		*pluginLink = NULL;			/// ptr to plugin object
CMessagesHandler *g_pMessHandler = NULL;	/// ptr to "main" global manager object
CSettingsDlgHolder *g_pSettingsDlg = NULL;
static HANDLE g_hHook[GLOB_HOOKS] = {NULL};	/// global hooks
CLISTMENUITEM g_mi = {NULL};				/// handle to menu item that is used to switch on/off plugin
CLISTMENUITEM g_miAUR2This = {NULL};		/// handle to menu item that is used to add user to aur 
bool g_bEnableMItem = true;					/// flag for menu item..
HANDLE g_hMenuItem;							/// global handle to menu service
CCrushLog CRUSHLOGOBJ;						/// global object that handles all crushes
LPTSTR g_strPluginName = TEXT(ICQPROTONAME);		/// global string that represents plugin name...
INT	   g_nCurrentMode = 0;					/// current mode of a protocol == i duno which exactly protocol...
HANDLE g_hAUR2User = NULL;					/// handle to proto service of "autoreply to this user"
HANDLE hMenuCommand,hUserCommand;

struct MM_INTERFACE mmi;
int hLangpack = 0;

/// forming an info structure
PLUGININFOEX pluginInfo =
{
	sizeof(PLUGININFOEX),
	ICQPROTODECRSHORT,
	PLUGIN_MAKE_VERSION(2, 0, 0, 5),
	ICQPROTODECR,
	DEVNAME,
	DEVMAIL,
	DEVCOPYRIGHT,
	DEVWWW,
	UNICODE_AWARE,	
	0,
	PLUGIN_UUID
};

/// mapping into mirandaim.exe
extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{	
BEGIN_PROTECT_AND_LOG_CODE
	hInst = hinstDLL;
	DisableThreadLibraryCalls(hinstDLL);
	return TRUE;
END_PROTECT_AND_LOG_CODE
}

extern "C" __declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{	
	return &pluginInfo;
}

static const MUUID interfaces[] = {MIID_CHAT, MIID_SRMM, MIID_LAST};

extern "C" __declspec(dllexport) const MUUID * MirandaPluginInterfaces(void)
{
	return interfaces;
}

/// handler of event when the options are choosed
static int OptionsInitialized(WPARAM wp, LPARAM lp)
{	
BEGIN_PROTECT_AND_LOG_CODE
	OPTIONSDIALOGPAGE optsDialog = {0};
	optsDialog.cbSize = sizeof(OPTIONSDIALOGPAGE);
	optsDialog.hInstance = hInst;	
	optsDialog.position = 910000000;
	optsDialog.pszTemplate = MAKEINTRESOURCEA(IDD_SDLGHOLDER);
	
	optsDialog.ptszGroup = LPGENT("Plugins");
	optsDialog.ptszTitle = LPGENT("Smart Auto Replier");

	optsDialog.pfnDlgProc = &CSettingsDlgHolder::FakeDlgProc;
	optsDialog.flags = ODPF_BOLDGROUPS | ODPF_TCHAR /*|ODPF_EXPERTONLY*/; /// some lames are scaring that option...
	
	g_pSettingsDlg->m_bDestroying = false;
	CallService(MS_OPT_ADDPAGE, wp, reinterpret_cast<LPARAM>(&optsDialog));
END_PROTECT_AND_LOG_CODE
	return FALSE;
}

/// pre building menu item for a contact..
static int AURContactPreBuildMenu(WPARAM wParam, LPARAM lParam)
{
	LPTSTR lpContactName = g_pMessHandler->GetContactName(reinterpret_cast<HANDLE>(wParam));
	CRulesStorage & storage = g_pMessHandler->getSettings().getStorage();
	bool bval = storage.RuleIsRegistered(lpContactName);

	g_miAUR2This.flags = (bval ? (CMIM_FLAGS | CMIF_GRAYED) : CMIM_FLAGS);

	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)g_hAUR2User, (LPARAM)&g_miAUR2This);

	return 0;
}

/// handler of clicking on menu item
/// aur to this user...
static INT_PTR AUR2User(WPARAM wParam, LPARAM lParam)
{
	CAddRuleDlg dlg;
	RULE_ITEM item;
	item.ContactName = g_pMessHandler->GetContactName(reinterpret_cast<HANDLE>(wParam));
	//item.ReplyAction = " ";
	item.ReplyText = SETTINGS_DEF_MESSAGE_RULE;
	TCHAR rulename[MAX_PATH * 5] = {0};
	_tcscat(rulename, TEXT("reply to "));
	_tcscat(rulename, item.ContactName);
	item.RuleName = rulename;

	dlg.m_baur2thisMode = true;
	dlg.m_item = item;
	dlg.DoModal();
	if (dlg.m_bAddedOk)
	{	/// already added...
		OnRefreshSettings();
	}

	return 0;
}

/// handler of clicking on menu item enablr/disable plugin...
static INT_PTR PluginMenuCommand(WPARAM wParam, LPARAM lParam)
{
BEGIN_PROTECT_AND_LOG_CODE
	if (!g_pMessHandler)
		return 0;

	if (g_bEnableMItem)
	{/// lets handle enabling of AUR feature
		g_pMessHandler->MakeAction(true); /// this will enable plugin
	}
	else
	{/// lets handle disabling of AUR feature
		g_pMessHandler->MakeAction(false); /// this will disable plugin
	}

	/// put here stuff to do
	g_mi.ptszName = (g_bEnableMItem ? MENU_ITEM_DISABLE_CAPTION : MENU_ITEM_ENABLE_CAPTION);
	g_mi.flags = CMIM_NAME | CMIF_TCHAR;

#ifdef _DEBUG
	int nretval = 
#endif
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)g_hMenuItem, (LPARAM)&g_mi);

	g_bEnableMItem = !g_bEnableMItem;
END_PROTECT_AND_LOG_CODE
	return 0;
}

/// handler that is invoked when mode is changed
static int OnStatusModeChanged(WPARAM wp, LPARAM lp)
{	
BEGIN_PROTECT_AND_LOG_CODE
	g_nCurrentMode = static_cast<INT>(wp);
	if (!g_pMessHandler)
		return FALSE;

	bool bNonOnOffLine = (wp != ID_STATUS_ONLINE &&
								  wp != ID_STATUS_OFFLINE &&
								  wp != ID_STATUS_FREECHAT);

	REPLYER_SETTINGS & s = g_pMessHandler->getSettings().getSettings();
	COMMON_RULE_ITEM & r = g_pMessHandler->getSettings().getStorage().getCommonRule();

	if (!bNonOnOffLine && s.bDisableWhenModeIsSet)
	{
		if (s.bEnabled)	/// plugin is enabled... we have a chance to disable it
		{
			bool bDisableIt = false;

			if (s.ModeDisValue == 0)
				bDisableIt = true;
			else if (s.ModeDisValue == 1 && wp == ID_STATUS_ONLINE)
				bDisableIt = true;
			else if (s.ModeDisValue == 2 && wp == ID_STATUS_FREECHAT)
				bDisableIt = true;

			if (bDisableIt)
			{	/// disable plugin...
				PluginMenuCommand(0, 0);
			}
		}
		return FALSE;
	}		

	if (s.bShowAURDlgWhenModeChanges)
	{
		CEditReplyDlg dlg;
		dlg.m_commRule = r;

		dlg.DoModal();
		if (dlg.m_bAllOk)
		{
			r = dlg.m_commRule;
		}
	}
	if (s.bEnableWhenModeIsSet)
	{
		if (!s.bEnabled)
		{			
			bool bEnable = false;
			if (s.ModeValue == 0 && bNonOnOffLine)
				bEnable = true;
			else if (s.ModeValue == 1 && wp == ID_STATUS_DND)
				bEnable = true;
			else if (s.ModeValue == 2 && wp == ID_STATUS_NA)
				bEnable = true;
			else if (s.ModeValue == 3 && wp == ID_STATUS_AWAY)
				bEnable = true;
			else if (s.ModeValue == 4 && wp == ID_STATUS_OCCUPIED)
				bEnable = true;

			if (bEnable) /// enabling plugin... (I realize that enabling and disabling is made via ass but i am lazy today)
				PluginMenuCommand(0, 0);
		}
	}

	return FALSE;
END_PROTECT_AND_LOG_CODE
	return FALSE;
}

#ifndef _DEBUG
LPTOP_LEVEL_EXCEPTION_FILTER UnhandledExceptionFilter(
  LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter
)
{
	LPTOP_LEVEL_EXCEPTION_FILTER f = {0};
BEGIN_PROTECT_AND_LOG_CODE	
	throw f;	/// show msgbox to user and store inf to file	
END_PROTECT_AND_LOG_CODE
	return f;	/// let compiler shut ups...
}
#endif

int OnModulesLoaded(WPARAM wParam, LPARAM lParam)
{
	if(ServiceExists(MS_UPDATE_REGISTER)) 
	{
		Update update = {0}; // for c you'd use memset or ZeroMemory...
		char szVersion[16];

		update.cbSize = sizeof(Update);

		update.szComponentName	= pluginInfo.shortName;
		update.pbVersion		= (BYTE*)CreateVersionStringPluginEx(&pluginInfo, szVersion);
		update.cpbVersion		= (int) strlen((char *)update.pbVersion);

		update.szUpdateURL = UPDATER_AUTOREGISTER;

		update.szBetaVersionURL = "http://code.google.com/p/smart-auto-replier/downloads/list";
#ifdef WIN64
		update.szBetaUpdateURL = "http://smart-auto-replier.googlecode.com/files/sar-%VERSION%-x64.zip";
		update.pbBetaVersionPrefix = (BYTE*) "Smart Auto Replier (x64) ";
#else
		update.szBetaUpdateURL = "http://smart-auto-replier.googlecode.com/files/sar-%VERSION%-x86.zip";
		update.pbBetaVersionPrefix = (BYTE*) "Smart Auto Replier ";
#endif
		update.cpbBetaVersionPrefix = (int) (strlen((char *)update.pbBetaVersionPrefix));
	// these are the three lines that matter - the archive, the page containing the version string, and the text (or data) 
	// before the version that we use to locate it on the page
	// (note that if the update URL and the version URL point to standard file listing entries, the backend xml
	// data will be used to check for updates rather than the actual web page - this is not true for beta urls)

		update.szBetaChangelogURL = "http://code.google.com/p/smart-auto-replier/source/list";

		CallService(MS_UPDATE_REGISTER,0,(WPARAM)&update);
	}
	return 0;
}

/// loading plugin
extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
#ifndef _DEBUG
	SetUnhandledExceptionFilter(UnhandledExceptionFilter);
#endif
#ifdef _DEBUG
	//MessageBox (NULL, __FUNCTION__, __FILE__, MB_OK);
#endif	

	CCrushLog::Init(); /// crushes manager should be inited first
	int nRetVal = 0;
BEGIN_PROTECT_AND_LOG_CODE	
	pluginLink = link;

#ifndef _DEBUG
	/// creates help file near plugin
	/// if it is absent...
	/// note: originally help is in recources of plugin
	//CheckForHelpFile();
#endif
					 /// main manager...
	g_pMessHandler = new CMessagesHandler();
	g_pSettingsDlg = new CSettingsDlgHolder();

	if (g_pMessHandler)
	{	/// is inited
		REPLYER_SETTINGS & s = g_pMessHandler->getSettings().getSettings();
		if (s.bDisableWhenMirandaIsOn)
		{	
			s.bEnabled = false;
		}
		else
		{
			if (s.bEnabled)
				g_pMessHandler->HookEvents();
		}
		
		/// hook main events
		//HookEvent(MS_AWAYMSG_SHOWAWAYMSG, OnShow
		g_hHook[0] = HookEvent(ME_OPT_INITIALISE, OptionsInitialized);
		g_hHook[1] = HookEvent(ME_CLIST_STATUSMODECHANGE, OnStatusModeChanged);
		g_hHook[2] = HookEvent(ME_SYSTEM_MODULESLOADED, OnModulesLoaded);

		/// Yushenko Tak, Yanukovich - Mudak. (Orange revolution)
		/// this was written in 2004-2005 in Kiev :)

		/// create menu item for disabling / enabling plugin
		hMenuCommand = CreateServiceFunction("AUR/MenuCommand", PluginMenuCommand);
		ZeroMemory(&g_mi, sizeof(g_mi));
		g_mi.cbSize = sizeof(g_mi);
		g_mi.position = -0x7FFFFFFF;
		g_mi.flags = 0;
		g_mi.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SAR_ICON)); //LoadSkinnedIcon(IDI_AUR_ICON/*SKINICON_OTHER_MIRANDA*/);
		bool bVal = g_pMessHandler->getSettings().getSettings().bEnabled;
		g_mi.ptszName = (bVal ? MENU_ITEM_DISABLE_CAPTION : MENU_ITEM_ENABLE_CAPTION);
		g_mi.flags = CMIF_TCHAR;
		g_mi.pszService = "AUR/MenuCommand";

		g_hMenuItem = (HANDLE)CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&g_mi);
		g_bEnableMItem = !bVal;

		/// creating menu item for a contact		
		ZeroMemory(&g_miAUR2This, sizeof(g_miAUR2This));
		g_miAUR2This.cbSize = sizeof(g_miAUR2This);
		g_miAUR2This.position = -2000010000;
		g_miAUR2This.flags = 0;
		g_miAUR2This.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SAR_ICON));
		g_miAUR2This.pszContactOwner = NULL;
		g_miAUR2This.ptszName = LPGENT("&Smart Auto Reply to this user ...");
		g_miAUR2This.flags = CMIF_TCHAR;
		hUserCommand = CreateServiceFunction("AUR/AURToThis", AUR2User);
		g_miAUR2This.pszService = "AUR/AURToThis";
		g_hAUR2User = (HANDLE)CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&g_miAUR2This);
		HookEvent(ME_CLIST_PREBUILDCONTACTMENU, AURContactPreBuildMenu);
	}

	mir_getMMI(&mmi);
	mir_getLP(&pluginInfo);

END_PROTECT_AND_LOG_CODE
	return FALSE;
}

/// unloading plugin
extern "C" int __declspec(dllexport) Unload(void)
{
BEGIN_PROTECT_AND_LOG_CODE
	for (size_t i = 0; i < GLOB_HOOKS; i++)
		UnhookEvent(g_hHook[i]);

	DestroyServiceFunction(hMenuCommand);
	DestroyServiceFunction(hUserCommand);

	if (g_pSettingsDlg)
	{
		if (g_pSettingsDlg->IsWindow())
			g_pSettingsDlg->DestroyWindow();
		delete g_pSettingsDlg;
	}
	if (g_pMessHandler)
	{	/// unhook manager's events...
		g_pMessHandler->UnHookEvents();
		delete g_pMessHandler; /// call dtor
		g_pMessHandler = NULL;
	}	
END_PROTECT_AND_LOG_CODE

	CCrushLog::DeInit(); /// destroy crushes manager obj
	return FALSE;
}
