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
std::map<HANDLE, HWND>* contactWindows;
DWORD gMirandaVersion;

HINSTANCE hInst;
HANDLE hModulesLoaded, hTabsrmmButtonPressed;
HANDLE g_hNetlibUser;
HANDLE hPrebuildContactMenu;
HANDLE hServiceContactMenu;
HGENMENU hContactMenu;
HGENMENU hWebPageMenus[PasteToWeb::pages];
HANDLE hMainIcon;
HANDLE hOptionsInit;
HANDLE hWindowEvent = NULL;
struct MM_INTERFACE mmi;
XML_API xi = {0};
int hLangpack;

#define MODULE				"PasteIt"
#define FROM_CLIPBOARD 10
#define FROM_FILE 11
#define DEF_PAGES_START 20
#define MS_PASTEIT_CONTACTMENU	"PasteIt/ContactMenu"

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
	0,
	MIID_PASTEIT
};

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	hInst = hinstDLL;
	return TRUE;
}

extern "C" __declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
	gMirandaVersion = mirandaVersion;
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
		char *szProto = (char *)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
		if (szProto && (INT_PTR)szProto != CALLSERVICE_NOTFOUND)
		{
			BOOL isChat = DBGetContactSettingByte(hContact, szProto, "ChatRoom", 0); 
			if(Options::instance->autoSend)
			{
				if(!isChat)
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
				}
				else
				{
					// PSS_MESSAGE is not compatible with chat rooms
					// there are no simple method to send text to all users
					// in chat room. 
					// First I check if protocol is unicode or ascii.
					BOOL isUnicodePlugin = TRUE;
					PROTOACCOUNT* protoAc = ProtoGetAccount(szProto);
					if(protoAc != NULL)
					{
						// protoAc->ppro is abstract class, that contains
						// methods implemented in protocol ddl`s segment.
						// Method address in vptr table must be converted
						// to hInstance of protocol dll.
						PROTO_INTERFACE* protoInt = protoAc->ppro;
						MEMORY_BASIC_INFORMATION mb;
						INT_PTR *vptr = *(INT_PTR**)&protoAc->ppro;  
						INT_PTR *vtable = (INT_PTR *)*vptr;   
						if(VirtualQuery((void*)vtable[0], &mb, sizeof(MEMORY_BASIC_INFORMATION)))
						{
							typedef PLUGININFO * (__cdecl * Miranda_Plugin_Info) ( DWORD mirandaVersion );
							typedef PLUGININFOEX * (__cdecl * Miranda_Plugin_InfoEx) ( DWORD mirandaVersion );
							HINSTANCE hInst = (HINSTANCE)mb.AllocationBase;
							// Now I can get PLUGININFOEX from protocol
							Miranda_Plugin_Info info = (Miranda_Plugin_Info) GetProcAddress(hInst, "MirandaPluginInfo");
							Miranda_Plugin_InfoEx infoEx = (Miranda_Plugin_InfoEx) GetProcAddress(hInst, "MirandaPluginInfoEx");
							PLUGININFOEX* pi = NULL;
							if(infoEx != NULL)
							{
								pi = infoEx(gMirandaVersion);
							}
							else if(info != NULL)
							{
								pi = (PLUGININFOEX*)info(gMirandaVersion);
							}

							// If PLUGININFOEX flags contains UNICODE_AWARE,
							// this mean that protocol is unicode.
							if(pi != NULL && pi->cbSize >= sizeof(PLUGININFO))
							{
								isUnicodePlugin = pi->flags & UNICODE_AWARE;
							}
						}
					}

					// Next step is to get all protocol sessions and find
					// one with correct hContact 
					GC_INFO gci = {0};    
					GCDEST  gcd = {0};    
					GCEVENT gce = {0};
					int cnt = (int)CallService(MS_GC_GETSESSIONCOUNT, 0, (LPARAM)szProto);  
					for (int i = 0; i < cnt ; i++ ) 
					{
						gci.iItem = i;
						gci.pszModule = szProto;
						gci.Flags = BYINDEX | HCONTACT | ID;
						CallService(MS_GC_GETINFO, 0, (LPARAM)(GC_INFO *) &gci);
						if (gci.hContact == hContact) 
						{
							// In this place session was finded, gci.pszID contains
							// session ID, but it is in unicode or ascii format,
							// depends on protocol wersion
							gcd.pszModule = szProto;
							gcd.iType = GC_EVENT_SENDMESSAGE;
							gcd.ptszID = gci.pszID;
							gce.cbSize = sizeof(GCEVENT);                     
							gce.pDest = &gcd;                 
							gce.bIsMe = TRUE;             
							gce.dwFlags = isUnicodePlugin ? (GCEF_ADDTOLOG | GC_UNICODE) : GCEF_ADDTOLOG;  
							wchar_t* s = NULL;
							if(isUnicodePlugin)
							{
								// If session ID is in unicode, text must be too in unicode
								s = mir_a2u_cp(pasteToWeb->szFileLink, CP_ACP);
								gce.ptszText = s;             
							}
							else
							{
								// If session ID is in ascii, text must be too in ascii
								gce.pszText = pasteToWeb->szFileLink;            
							}
							gce.time = time(NULL);                                            
							CallService(MS_GC_EVENT, 0, (LPARAM)(GCEVENT *) &gce); 
							if(s != NULL)
								mir_free(s);                       
							break;  
						}
					}
				}
				
				// Send message to focus window
				CallServiceSync(MS_MSG_SENDMESSAGE, (WPARAM)hContact, 0);
			}
			else
			{
				if(isChat)
				{
					// MS_MSG_SENDMESSAGE in incompatible with chat rooms,
					// because it sends text to IDC_MESSAGE window,
					// but in chat rooms is only IDC_CHAT_MESSAGE window.
					// contactWindows map contains all opened hContact
					// with assaigned to them chat windows. 
					// This map is prepared in ME_MSG_WINDOWEVENT event. 
					std::map<HANDLE, HWND>::iterator it = contactWindows->find(hContact);
					if(it != contactWindows->end())
					{
						// it->second is imput window, so now I can send to them 
						// new text. Afterr all is sended MS_MSG_SENDMESSAGE 
						// to focus window.
						SendMessage(it->second, EM_SETSEL, -1, SendMessage(it->second, WM_GETTEXTLENGTH, 0, 0));
						SendMessageA(it->second, EM_REPLACESEL, FALSE, (LPARAM)pasteToWeb->szFileLink);
						CallServiceSync(MS_MSG_SENDMESSAGE, (WPARAM)hContact, NULL);
					}
					else
					{
						// If window do not exist, maybe it is not chat
						CallServiceSync(MS_MSG_SENDMESSAGE, (WPARAM)hContact, (LPARAM)pasteToWeb->szFileLink);
					}
				}
				else
				{
					CallServiceSync(MS_MSG_SENDMESSAGE, (WPARAM)hContact, (LPARAM)pasteToWeb->szFileLink);
				}
			}
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
				AppendMenu(hMenu, MF_STRING, FROM_CLIPBOARD, TranslateT("Paste from clipboard"));
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
	sid.ptszDescription = LPGENT("Paste It");
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
	mi.ptszName = _T("Paste It");

	hContactMenu = (HGENMENU)CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);

	memset(&mi, 0, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.flags = CMIF_CHILDPOPUP | CMIF_ROOTHANDLE | CMIF_TCHAR;
	mi.pszService = MS_PASTEIT_CONTACTMENU;
	mi.hParentMenu = hContactMenu;
	mi.popupPosition = FROM_CLIPBOARD;	
	mi.ptszName = _T("Paste from clipboard");
	CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);

	mi.popupPosition = FROM_FILE;
	mi.ptszName = _T("Paste from file");
	CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi);

	mi.popupPosition = DEF_PAGES_START - 1;
	mi.ptszName = _T("Default web page");
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
		btn.bbbFlags = BBBF_ISARROWBUTTON | BBBF_ISIMBUTTON | BBBF_ISLSIDEBUTTON | BBBF_CANBEHIDDEN | BBBF_ISCHATBUTTON;
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

int WindowEvent(WPARAM wParam, MessageWindowEventData* lParam)
{
	if(lParam->uType == MSG_WINDOW_EVT_OPEN)
	{
		char *szProto = (char *)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)lParam->hContact, 0);
		if (szProto && (INT_PTR)szProto != CALLSERVICE_NOTFOUND)
		{
			if(DBGetContactSettingByte(lParam->hContact, szProto, "ChatRoom", 0))
			{
				(*contactWindows)[lParam->hContact] = lParam->hwndInput;
			}
		}
	}
	else if(lParam->uType == MSG_WINDOW_EVT_CLOSE)
	{
		std::map<HANDLE, HWND>::iterator it = contactWindows->find(lParam->hContact);
		if(it != contactWindows->end())
		{
			contactWindows->erase(it);
		}
	}

	return 0;
}

int ModulesLoaded(WPARAM wParam, LPARAM lParam)
{
	InitIcolib();
	InitMenuItems();
	InitTabsrmmButton();
	InitUpdater();
	hWindowEvent = HookEvent(ME_MSG_WINDOWEVENT, (MIRANDAHOOK)WindowEvent);

	return 0;
}

extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink = link;
	mir_getMMI(&mmi);
	mir_getXI(&xi);
	mir_getLP(&pluginInfo);
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
	contactWindows = new std::map<HANDLE, HWND>();
	return 0;
}

extern "C" int __declspec(dllexport) Unload(void) 
{	
	UnhookEvent(hModulesLoaded);
	UnhookEvent(hPrebuildContactMenu);
	UnhookEvent(hOptionsInit);
	if(hWindowEvent != NULL)
	{
		UnhookEvent(hWindowEvent);
	}
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

	delete contactWindows;
	return 0;
}
