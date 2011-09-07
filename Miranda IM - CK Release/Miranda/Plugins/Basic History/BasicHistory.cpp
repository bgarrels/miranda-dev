/*
Basic History plugin
Copyright (C) 2011 Krzysztof Kral

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "version.h"
#include "HistoryWindow.h"
#include "resource.h"
#include "Options.h"

// {E25367A2-51AE-4044-BE28-131BC18B71A4}
#define	MIID_BASICHISTORY { 0xe25367a2, 0x51ae, 0x4044, { 0xbe, 0x28, 0x13, 0x1b, 0xc1, 0x8b, 0x71, 0xa4 } }

PLUGINLINK *pluginLink;
HCURSOR     hCurSplitNS, hCurSplitWE;

extern HINSTANCE hInst;

HANDLE hModulesLoaded, hOptionsInit, hPrebuildContactMenu, hServiceShowContactHistory, hPreShutdownHistoryModule, hHistoryContactDelete, hFontsChanged;
HANDLE hInIcon, hOutIcon, hPlusIcon, hMinusIcon, hFindNextIcon, hFindPrevIcon, hConfigIcon, hDeleteIcon;
HGENMENU hContactMenu;
bool g_SmileyAddAvail = false;
const IID IID_ITextDocument={0x8CC497C0, 0xA1DF, 0x11ce, {0x80, 0x98, 0x00, 0xAA, 0x00, 0x47, 0xBE, 0x5D}};

#define MODULE				"BasicHistory"

PLUGININFOEX pluginInfo={
	sizeof(PLUGININFOEX),
	__PLUGIN_NAME,
	PLUGIN_MAKE_VERSION(__MAJOR_VERSION, __MINOR_VERSION, __RELEASE_NUM, __BUILD_NUM),
	__DESCRIPTION,
	__AUTHOR,
	__AUTHOREMAIL,
	__COPYRIGHT,
	__AUTHORWEB,
	UNICODE_AWARE, 
	DEFMOD_UIHISTORY,
	MIID_BASICHISTORY
};

MM_INTERFACE mmi = {0};
TIME_API tmi = {0};
int hLangpack = 0;

extern "C" __declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
	if (mirandaVersion < PLUGIN_MAKE_VERSION(0, 9, 0, 0))
	{
		MessageBox(0, _T("This version of BasicHistory requires Miranda 0.9.0 or later. The plugin cannot be loaded."), _T("BasicHistory"), MB_OK | MB_ICONERROR);
		return NULL;
	}

	return &pluginInfo;
}

static const MUUID interfaces[] = {MIID_UIHISTORY, MIID_LAST};
extern "C" __declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}

int PrebuildContactMenu(WPARAM wParam, LPARAM lParam)
{
	int count = CallService(MS_DB_EVENT_GETCOUNT,wParam,0);

	CLISTMENUITEM mi = {0};
	mi.cbSize = sizeof(mi);
	mi.flags = CMIM_FLAGS;

	if (!count) mi.flags |= CMIF_HIDDEN;
	else mi.flags &= ~CMIF_HIDDEN;
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hContactMenu, (LPARAM)&mi);

	return 0;
}

void InitMenuItems()
{
	CLISTMENUITEM mi = { 0 };
	mi.cbSize = sizeof(mi);
	mi.position = 1000090000;
	mi.flags = CMIF_ICONFROMICOLIB;
	mi.icolibItem = LoadSkinnedIconHandle( SKINICON_OTHER_HISTORY );
	mi.pszName = LPGEN("View &History");
	mi.pszService = MS_HISTORY_SHOWCONTACTHISTORY;
	hContactMenu = (HGENMENU)CallService(MS_CLIST_ADDCONTACTMENUITEM,0,(LPARAM)&mi);

	mi.position = 500060000;
	mi.pszService = MS_HISTORY_SHOWCONTACTHISTORY;
	CallService(MS_CLIST_ADDMAINMENUITEM,0,(LPARAM)&mi);

	hPrebuildContactMenu = HookEvent(ME_CLIST_PREBUILDCONTACTMENU, PrebuildContactMenu);
}

void InitIcolib()
{
	TCHAR stzFile[MAX_PATH];

	SKINICONDESC sid = {0};
	sid.cbSize = sizeof(sid);
	sid.cx = sid.cy = 16;
	sid.ptszDefaultFile = stzFile;
	sid.ptszSection = LPGENT("History");
	sid.flags = SIDF_ALL_TCHAR;

	GetModuleFileName(hInst, stzFile, MAX_PATH);
	
	sid.pszName = "BasicHistory_in";
	sid.ptszDescription = LPGENT("Incoming message");
	sid.iDefaultIndex = -IDI_INM;
	hInIcon = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);

	sid.pszName = "BasicHistory_out";
	sid.ptszDescription = LPGENT("Outgoing message");
	sid.iDefaultIndex = -IDI_OUTM;
	hOutIcon = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);

	sid.pszName = "BasicHistory_show";
	sid.ptszDescription = LPGENT("Show Contacts");
	sid.iDefaultIndex = -IDI_SHOW;
	hPlusIcon = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);

	sid.pszName = "BasicHistory_hide";
	sid.ptszDescription = LPGENT("Hide Contacts");
	sid.iDefaultIndex = -IDI_HIDE;
	hMinusIcon = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);

	sid.pszName = "BasicHistory_findnext";
	sid.ptszDescription = LPGENT("Find Next");
	sid.iDefaultIndex = -IDI_FINDNEXT;
	hFindNextIcon = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);

	sid.pszName = "BasicHistory_findprev";
	sid.ptszDescription = LPGENT("Find Previous");
	sid.iDefaultIndex = -IDI_FINDPREV;
	hFindPrevIcon = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);

	sid.pszName = "BasicHistory_options";
	sid.ptszDescription = LPGENT("Options");
	sid.iDefaultIndex = -IDI_CONFIG;
	hConfigIcon = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);

	sid.pszName = "BasicHistory_delete";
	sid.ptszDescription = LPGENT("Delete");
	sid.iDefaultIndex = -IDI_ERASE;
	hDeleteIcon = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);
}

void InitUpdater()
{
	if (ServiceExists(MS_UPDATE_REGISTER)) 
	{
		Update update = {0};
		char szVersion[16];
		update.cbSize = sizeof(Update);
		update.szComponentName = pluginInfo.shortName;
		update.pbVersion = (BYTE *)CreateVersionStringPluginEx(&pluginInfo, szVersion);
		update.cpbVersion = (int)strlen((char *)update.pbVersion);

#ifdef _WIN64
		update.szUpdateURL = "http://programista.free.of.pl/miranda/BasicHistory64.zip";
#else
		update.szUpdateURL = "http://programista.free.of.pl/miranda/BasicHistory.zip";
#endif
		update.szVersionURL = "http://programista.free.of.pl/miranda/BasicHistoryVersion.txt";
		update.pbVersionPrefix = (BYTE *)"Basic History ";
		update.cpbVersionPrefix = (int)strlen((char *)update.pbVersionPrefix);
		CallService(MS_UPDATE_REGISTER, 0, (WPARAM)&update);
	}
}

INT_PTR ShowContactHistory(WPARAM wParam, LPARAM lParam) 
{
	HANDLE hContact = (HANDLE)wParam;
	HistoryWindow::Open(hContact);
	return 0;
}

int PreShutdownHistoryModule(WPARAM, LPARAM)
{
	HistoryWindow::Deinit();
	return 0;
}

int HistoryContactDelete(WPARAM wParam, LPARAM)
{
	HistoryWindow::Close((HANDLE)wParam);
	return 0;
}

int ModulesLoaded(WPARAM wParam, LPARAM lParam)
{
	InitIcolib();
	InitMenuItems();
	InitUpdater();

	Options::instance->Load();
	
	hPreShutdownHistoryModule = HookEvent(ME_SYSTEM_PRESHUTDOWN,PreShutdownHistoryModule);
	hHistoryContactDelete = HookEvent(ME_DB_CONTACT_DELETED,HistoryContactDelete);
	hFontsChanged  = HookEvent(ME_FONT_RELOAD, HistoryWindow::FontsChanged);
	if (ServiceExists(MS_SMILEYADD_REPLACESMILEYS)) 
	{
		g_SmileyAddAvail = true;
	}
	return 0;
}

extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink = link;
	mir_getMMI(&mmi);
	mir_getTMI(&tmi);
	mir_getLP(&pluginInfo);
	hCurSplitNS = LoadCursor(NULL, IDC_SIZENS);
	hCurSplitWE = LoadCursor(NULL, IDC_SIZEWE);
	hServiceShowContactHistory = CreateServiceFunction(MS_HISTORY_SHOWCONTACTHISTORY, ShowContactHistory);
	Options::instance = new Options();
	hModulesLoaded = HookEvent(ME_SYSTEM_MODULESLOADED, ModulesLoaded);
	hOptionsInit = HookEvent(ME_OPT_INITIALISE, Options::InitOptions);
	return 0;
}

extern "C" int __declspec(dllexport) Unload(void) 
{	
	UnhookEvent(hModulesLoaded);
	UnhookEvent(hPrebuildContactMenu);
	UnhookEvent(hPreShutdownHistoryModule);
	UnhookEvent(hHistoryContactDelete);
	UnhookEvent(hOptionsInit);
	UnhookEvent(hFontsChanged);
	DestroyServiceFunction(hServiceShowContactHistory);
	HistoryWindow::Deinit();
	DestroyCursor(hCurSplitNS);
	DestroyCursor(hCurSplitWE);
	if(Options::instance != NULL)
	{
		delete Options::instance;
		Options::instance = NULL;
	}
	return 0;
}
