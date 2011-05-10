/*
Paste It plugin
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
#include "PasteToWeb1.h"
#include "PasteToWeb2.h"
#include "version.h"
#include "resource.h"
#include "Options.h"

// {1AAC15E8-DCEC-4050-B66F-2AA0E6120C22}
#define	MIID_PASTEIT { 0x1aac15e8, 0xdcec, 0x4050, { 0xb6, 0x6f, 0x2a, 0xa0, 0xe6, 0x12, 0xc, 0x22 } }

PLUGINLINK *pluginLink;
PasteToWeb* pasteToWebs[PasteToWeb::pages];

extern HINSTANCE hInst;
HANDLE hModulesLoaded, hTabsrmmButtonPressed;
HANDLE g_hNetlibUser;
HANDLE hPrebuildContactMenu;
HANDLE hServiceContactMenu;
HGENMENU hContactMenu;
HGENMENU hWebPageMenus[PasteToWeb::pages];
HANDLE hMainIcon;
HANDLE hOptionsInit;

#define MODULE				"PasteIt"
#define FROM_CLIPBOARD 10
#define FROM_FILE 11
#define DEF_PAGES_START 20
#define MS_PASTEIT_CONTACTMENU	"PasteIt/ContactMenu"

PLUGININFOEX pluginInfo={
	sizeof(PLUGININFOEX),
	"Paste It",
	__VERSION_DWORD,
	"Uploads the text to web page and sends the URL to your friend",
	"Krzysztof Kral",
	"programista@poczta.of.pl",
	"Copyright � 2011 Krzysztof Kral",
	"http://programista.free.of.pl/miranda/",
	UNICODE_AWARE, 
	0,
	MIID_PASTEIT
};

MM_INTERFACE mmi = {0};
XML_API xi = {0};

extern "C" __declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
	if (mirandaVersion < PLUGIN_MAKE_VERSION(0, 8, 0, 0))
		return NULL;

	return &pluginInfo;
}

static const MUUID interfaces[] = {MIID_PASTEIT, MIID_LAST};
extern "C" __declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}

std::wstring GetFile()
{
	TCHAR filter[512];
	_tcscpy_s(filter, 512, TranslateT("All Files (*.*)"));
	memcpy(filter + _tcslen(filter), _T("\0*.*\0"), 6 * sizeof(TCHAR));
	TCHAR stzFilePath[1024];
	stzFilePath[0] = 0;
	stzFilePath[1] = 0;
	OPENFILENAME ofn = {0};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = 0;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = stzFilePath;
	ofn.lpstrTitle = TranslateT("Paste It - Select file");
	ofn.nMaxFile = 1024;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	if(GetOpenFileName(&ofn))
	{
		return stzFilePath;
	}

	return L"";
}

void PasteIt(HANDLE hContact, int mode)
{
	PasteToWeb* pasteToWeb = pasteToWebs[Options::instance->defWeb];
	if(mode == FROM_CLIPBOARD)
	{
		pasteToWeb->FromClipboard();
	}
	else if(mode == FROM_FILE)
	{
		std::wstring file = GetFile();
		if(file.length() > 0)
		{
			pasteToWeb->FromFile(file);
		}
		else return;
	}
	else return;

	if(pasteToWeb->szFileLink[0] == 0 && pasteToWeb->error != NULL)
	{
		MessageBox(NULL, pasteToWeb->error, TranslateT("Error"), MB_OK | MB_ICONERROR);
	}
	else if(hContact != NULL && pasteToWeb->szFileLink[0] != 0)
	{
		if(Options::instance->autoSend)
		{
			char *szProto = (char *)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
			if (szProto && (INT_PTR)szProto != CALLSERVICE_NOTFOUND)
			{
				DBEVENTINFO dbei = {0};
				dbei.cbSize = sizeof(dbei);
				dbei.eventType = EVENTTYPE_MESSAGE;
				dbei.flags = DBEF_SENT;
				dbei.szModule = szProto;
				dbei.timestamp = (DWORD)time(NULL);
				dbei.cbBlob = (DWORD)strlen(pasteToWeb->szFileLink) + 1;
				dbei.pBlob = (PBYTE)pasteToWeb->szFileLink;
				CallService(MS_DB_EVENT_ADD, (WPARAM)hContact, (LPARAM)&dbei);
				CallContactService(hContact, PSS_MESSAGE, 0, (LPARAM)pasteToWeb->szFileLink);
				CallServiceSync(MS_MSG_SENDMESSAGE, (WPARAM)hContact, 0);
			}
		}
		else
		{
			CallServiceSync(MS_MSG_SENDMESSAGE, (WPARAM)hContact, (LPARAM)pasteToWeb->szFileLink);
		}
	}
}

int TabsrmmButtonPressed(WPARAM wParam, LPARAM lParam) 
{
	CustomButtonClickData *cbc = (CustomButtonClickData *)lParam;
	HANDLE hContact = (HANDLE)wParam;

	if (!strcmp(cbc->pszModule, MODULE) && cbc->dwButtonId == 1 && hContact) 
	{
		if (cbc->flags == BBCF_ARROWCLICKED) 
		{
			HMENU hMenu = CreatePopupMenu();
			if (hMenu) 
			{
				AppendMenu(hMenu, MF_STRING, FROM_CLIPBOARD, TranslateT("Paste from clopboard"));
				AppendMenu(hMenu, MF_STRING, FROM_FILE, TranslateT("Paste from file"));
				HMENU hDefMenu = CreatePopupMenu();
				for(int i = 0 ; i < PasteToWeb::pages; ++i)
				{
					UINT flags = MF_STRING;
					if(Options::instance->defWeb == i)
						flags |= MF_CHECKED;

					AppendMenu(hDefMenu, flags, DEF_PAGES_START + i, pasteToWebs[i]->GetName());
				}

				AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hDefMenu, TranslateT("Default web page"));
				DestroyMenu(hDefMenu);

				POINT pt;
				GetCursorPos(&pt);
				HWND hwndBtn = WindowFromPoint(pt);
				if (hwndBtn) 
				{
					RECT rc;
					GetWindowRect(hwndBtn, &rc);
					SetForegroundWindow(cbc->hwndFrom);
					int selected = TrackPopupMenu(hMenu, TPM_RETURNCMD, rc.left, rc.bottom, 0, cbc->hwndFrom, 0);
					if (selected != 0)
					{
						if(selected >= DEF_PAGES_START)
						{
							Options::instance->SetDefWeb(selected - DEF_PAGES_START);
						}
						else
						{
							PasteIt(hContact, selected);
						}
					}
				}

				DestroyMenu(hMenu);
			}
		} 
		else
		{
			PasteIt(hContact, FROM_CLIPBOARD);
		}
	}

	return 0;
}

int PrebuildContactMenu(WPARAM wParam, LPARAM lParam)
{
	bool bIsContact = false;
	
	char *szProto = (char *)CallService(MS_PROTO_GETCONTACTBASEPROTO, wParam, 0);
	if (szProto && (INT_PTR)szProto != CALLSERVICE_NOTFOUND) bIsContact = (CallProtoService(szProto, PS_GETCAPS, PFLAGNUM_1, 0) & PF1_IM) ? true : false;
	
	CLISTMENUITEM mi = {0};
	mi.cbSize = sizeof(mi);
	mi.flags = CMIM_FLAGS;

	if (!bIsContact) mi.flags |= CMIF_HIDDEN;
	else mi.flags &= ~CMIF_HIDDEN;
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hContactMenu, (LPARAM)&mi);

	return 0;
}

INT_PTR ContactMenuService(WPARAM wParam, LPARAM lParam) 
{
	if(lParam >= DEF_PAGES_START)
	{
		Options::instance->SetDefWeb(lParam - DEF_PAGES_START);
	}
	else
	{
		HANDLE hContact = (HANDLE)wParam;
		PasteIt(hContact, lParam);
	}
	return 0;
}

void InitIcolib()
{
	TCHAR stzFile[MAX_PATH];

	SKINICONDESC sid = {0};
	sid.cbSize = sizeof(sid);
	sid.cx = sid.cy = 16;
	sid.ptszDefaultFile = stzFile;
	sid.ptszSection = LPGENT("Paste It");
	sid.flags = SIDF_ALL_TCHAR;

	GetModuleFileName(hInst, stzFile, MAX_PATH);
	
	sid.pszName = "PasteIt_main";
	sid.ptszDescription = TranslateT("Paste It");
	sid.iDefaultIndex = -IDI_MENU;
	hMainIcon = (HANDLE)CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);
}

void InitMenuItems()
{
	CLISTMENUITEM mi = {0};
	CLISTMENUITEM mi2 = {0};

	mi.cbSize = sizeof(mi);
	mi.flags = CMIF_ROOTPOPUP | CMIF_ICONFROMICOLIB | CMIF_TCHAR;
	mi.icolibItem = hMainIcon;
	mi.position = 3000090005;
	mi.ptszName = TranslateT("Paste It");

	hContactMenu = (HGENMENU)CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);

	memset(&mi, 0, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.flags = CMIF_CHILDPOPUP | CMIF_ROOTHANDLE | CMIF_TCHAR;
	mi.pszService = MS_PASTEIT_CONTACTMENU;
	mi.hParentMenu = hContactMenu;
	mi.popupPosition = FROM_CLIPBOARD;	
	mi.ptszName =TranslateT("Paste from clopboard");
	CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);

	mi.popupPosition = FROM_FILE;
	mi.ptszName =TranslateT("Paste from file");
	CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);

	mi.popupPosition = DEF_PAGES_START - 1;
	mi.ptszName =TranslateT("Default web page");
	HGENMENU hDefWebMenu = (HGENMENU)CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);
	
	mi2.cbSize = sizeof(mi2);
	mi2.pszService = MS_PASTEIT_CONTACTMENU;
	mi2.hParentMenu = hDefWebMenu;
	for(int i = 0 ; i < PasteToWeb::pages; ++i)
	{
		mi2.flags =  CMIF_CHILDPOPUP | CMIF_ROOTHANDLE | CMIF_TCHAR;
		if(Options::instance->defWeb == i)
			mi2.flags |= CMIF_CHECKED;
		mi2.ptszName = pasteToWebs[i]->GetName();
		mi2.popupPosition = mi2.position = DEF_PAGES_START + i;
		hWebPageMenus[i] = (HGENMENU)CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi2);
	}

	hPrebuildContactMenu = HookEvent(ME_CLIST_PREBUILDCONTACTMENU, PrebuildContactMenu);
}

void DefWebPageChanged()
{
	CLISTMENUITEM mi = {0};
	mi.cbSize = sizeof(mi);

	for (int i = 0; i < PasteToWeb::pages; i++) 
	{
		mi.flags = CMIM_FLAGS;
		if (Options::instance->defWeb == i) 
		{
			mi.flags |= CMIF_CHECKED;
		} 

		CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hWebPageMenus[i], (LPARAM)&mi);
	}
}

void InitTabsrmmButton()
{
	if (ServiceExists(MS_BB_ADDBUTTON)) 
	{
		BBButton btn = {0};
		btn.cbSize = sizeof(btn);
		btn.dwButtonID = 1;
		btn.pszModuleName = MODULE;
		btn.dwDefPos = 110;
		btn.hIcon = hMainIcon;
		btn.bbbFlags = BBBF_ISARROWBUTTON | BBBF_ISIMBUTTON | BBBF_ISLSIDEBUTTON | BBBF_CANBEHIDDEN;
		btn.ptszTooltip = TranslateT("Paste It");
		CallService(MS_BB_ADDBUTTON, 0, (LPARAM)&btn);
		if(hTabsrmmButtonPressed != NULL)
		{
			UnhookEvent(hTabsrmmButtonPressed);
		}

		hTabsrmmButtonPressed = HookEvent(ME_MSG_BUTTONPRESSED, TabsrmmButtonPressed);
	}
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
		update.szUpdateURL = "http://programista.free.of.pl/miranda/PasteIt64.zip";
#else
		update.szUpdateURL = "http://programista.free.of.pl/miranda/PasteIt.zip";
#endif
		update.szVersionURL = "http://programista.free.of.pl/miranda/PasteItVersion.txt";
		update.pbVersionPrefix = (BYTE *)"Paste It ";
		update.cpbVersionPrefix = (int)strlen((char *)update.pbVersionPrefix);
		CallService(MS_UPDATE_REGISTER, 0, (WPARAM)&update);
	}
}

int ModulesLoaded(WPARAM wParam, LPARAM lParam)
{
	InitIcolib();
	InitMenuItems();
	InitTabsrmmButton();
	InitUpdater();

	return 0;
}

extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink = link;
	mir_getMMI(&mmi);
	mir_getXI(&xi);
	NETLIBUSER nlu = {0};
	nlu.cbSize = sizeof(nlu);
  	nlu.flags = NUF_TCHAR | NUF_OUTGOING | NUF_HTTPCONNS;
	nlu.szSettingsModule = MODULE;
	nlu.ptszDescriptiveName = TranslateT("Paste It HTTP connections");
	g_hNetlibUser = ( HANDLE )CallService( MS_NETLIB_REGISTERUSER, 0, ( LPARAM )&nlu );
	
	pasteToWebs[0] = new PasteToWeb1();
	pasteToWebs[0]->pageIndex = 0;
	pasteToWebs[1] = new PasteToWeb2();
	pasteToWebs[1]->pageIndex = 1;
	Options::instance = new Options();
	pasteToWebs[0]->ConfigureSettings();
	pasteToWebs[1]->ConfigureSettings();
	Options::instance->Load();
	hModulesLoaded = HookEvent(ME_SYSTEM_MODULESLOADED, ModulesLoaded);
	hOptionsInit = HookEvent(ME_OPT_INITIALISE, Options::InitOptions);
	hTabsrmmButtonPressed = NULL;
	hServiceContactMenu = CreateServiceFunction(MS_PASTEIT_CONTACTMENU, ContactMenuService);
	return 0;
}

extern "C" int __declspec(dllexport) Unload(void) 
{	
	UnhookEvent(hModulesLoaded);
	UnhookEvent(hPrebuildContactMenu);
	UnhookEvent(hOptionsInit);
	DestroyServiceFunction(hServiceContactMenu);
	Netlib_CloseHandle(g_hNetlibUser);
	if(hTabsrmmButtonPressed != NULL)
	{
		UnhookEvent(hTabsrmmButtonPressed);
	}
	for(int i=0; i < PasteToWeb::pages; ++i)
	{
		if(pasteToWebs[i] != NULL)
		{
			delete pasteToWebs[i];
			pasteToWebs[i] = NULL;
		}
	}
	if(Options::instance != NULL)
	{
		delete Options::instance;
		Options::instance = NULL;
	}
	return 0;
}
