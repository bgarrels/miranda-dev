/*
Flash Avatar support plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	2001-2007 Big Muscle
							2001-2006 Jacek Sieka


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

#define MIRANDA_VER 0x0700

#include "m_stdhdr.h"
#include "win2k.h"

#include <newpluginapi.h>
#include <m_system.h>
#include <m_system_cpp.h>
#include <m_database.h>
#include <m_protosvc.h>
#include <statusmodes.h>
#include <m_clist.h>
#include <m_langpack.h>

#include <io.h>
#include <fcntl.h>
#include <Winsock2.h>

//miranda stuff
#include "m_flash.h"
#include "m_avatars.h"
#include "m_utils.h"
#include "m_netlib.h"

#include "m_clist.h"
#include "m_clistint.h"

#include "m_folders.h"

#include "CriticalSection.h"
#include "TigerHash.h"

//#import  "PROGID:ShockwaveFlash.ShockwaveFlash" no_namespace
#import "Macromed\Flash\Flash10p.ocx" no_namespace exclude("IServiceProvider")

PLUGININFOEX pluginInfo = {
  sizeof(PLUGININFOEX), 
	NULL,
	PLUGIN_MAKE_VERSION(0, 0, 1, 14), 
	"Load and display flash avatars",
	"Big Muscle",
	"",
	"Copyright 2000-2012 Miranda-IM project",
	"http://www.miranda-im.org",
	UNICODE_AWARE,
	0,
	// {72765A6F-B017-42f1-B30F-5E0941273A3F}
	{ 0x72765a6f, 0xb017, 0x42f1, { 0xb3, 0xf, 0x5e, 0x9, 0x41, 0x27, 0x3a, 0x3f } }
};


/* a strcmp() that likes NULL */
int __fastcall strcmpnull(const char *str1, const char *str2) {
	if (str1 && str2)
		return strcmp(str1, str2);

	if (!str1 && !str2)
		return 0;

	return 1;
}


struct flash_avatar_item {
	HANDLE hContact;
	FLASHAVATAR hFA;
	IShockwaveFlash* pFlash;

	char* getProto() { return (hFA.cProto) ? hFA.cProto : (char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hFA.hContact, 0); }

	__inline void* operator new(size_t size) { return mir_calloc(size); }
	__inline void operator delete( void* p ) { mir_free(p); }

	flash_avatar_item(HANDLE contact, FLASHAVATAR& fa, IShockwaveFlash *flash) { hContact = contact; hFA = fa; pFlash = flash; }
};

static int CompareFlashItems(const flash_avatar_item* p1, const flash_avatar_item* p2) {
	if (p1->hContact < p2->hContact)
		return -1;

	if (p1->hContact > p2->hContact)
		return 1;

	int cProto = strcmpnull(p1->hFA.cProto, p2->hFA.cProto);
	if (cProto)
		return cProto;

	return (p1->hFA.id > p2->hFA.id) ? -1 : (p1->hFA.id == p2->hFA.id) ? 0 : 1;
};

HINSTANCE g_hInst = 0;

PLUGINLINK *pluginLink;
MM_INTERFACE mmi;
LIST_INTERFACE li;

HANDLE hNetlibUser;

static char pluginName[64];

static HANDLE hHooks[4];
static HANDLE hServices[7];
static CriticalSection cs;

static HANDLE hAvatarsFolder = NULL;

static LIST<flash_avatar_item> FlashList(5, CompareFlashItems);

typedef HRESULT (WINAPI *LPAtlAxAttachControl)(IUnknown* pControl, HWND hWnd, IUnknown** ppUnkContainer);
LPAtlAxAttachControl MyAtlAxAttachControl;


#define getFace() \
	char* face;\
	switch (status) {\
		case ID_STATUS_OFFLINE:\
			face = AV_OFFLINE;\
			break;\
		case ID_STATUS_ONLINE:\
		case ID_STATUS_INVISIBLE:\
			face = AV_NORMAL;\
			break;\
		default:\
			face = AV_BUSY;\
			break;\
	}

static bool DownloadFlashFile(char *url, const TCHAR* save_file, int recurse_count /*=0*/) {
	if(!url || recurse_count > 5) {
		return false;
	}

	NETLIBHTTPREQUEST req = {0};
	req.cbSize = sizeof(req);
	req.requestType = REQUEST_GET;
	req.szUrl = url;
	req.flags = 0;//NLHRF_HTTP11;

	NETLIBHTTPREQUEST *resp = (NETLIBHTTPREQUEST *)CallService(MS_NETLIB_HTTPTRANSACTION, (WPARAM)hNetlibUser, (LPARAM)&req);
	if(resp) {
		if(resp->resultCode == 200) {
			HANDLE hSaveFile = CreateFile(save_file, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
			if(hSaveFile != INVALID_HANDLE_VALUE) {
				unsigned long bytes_written = 0;
				if(WriteFile(hSaveFile, resp->pData, resp->dataLength, &bytes_written, NULL) == TRUE) {
					CloseHandle(hSaveFile);
					CallService(MS_NETLIB_FREEHTTPREQUESTSTRUCT, 0, (LPARAM)resp);
					resp = 0;
					return true;
				}
				CloseHandle(hSaveFile);
			}
		} else if(resp->resultCode >= 300 && resp->resultCode < 400) {
			// get new location
			bool ret = false;
			for(int i = 0; i < resp->headersCount; i++) {
				if(strcmpnull(resp->headers[i].szName, "Location") == 0) {
					ret = DownloadFlashFile(resp->headers[i].szValue, save_file, recurse_count + 1);
					break;
				}
			}
			CallService(MS_NETLIB_FREEHTTPREQUESTSTRUCT, 0, (LPARAM)resp);
			resp = 0;
			return ret;
		}
		CallService(MS_NETLIB_FREEHTTPREQUESTSTRUCT, 0, (LPARAM)resp);
		resp = 0;
	}

	return false;
}
/*
static wchar_t *u2w(const char *utfs) {
	if(utfs) {
		int size = MultiByteToWideChar(CP_UTF8, 0, utfs, -1, 0, 0);
		wchar_t *buff = new wchar_t[size];
		MultiByteToWideChar(CP_UTF8, 0, utfs, -1, buff, size);
		return buff;
	} else
		return 0;
}
*/

static void __cdecl loadFlash_Thread(void *p) {
	debug("Avatar thread executed...\n");
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);

	flash_avatar_item* fai = (flash_avatar_item*)p;
	IShockwaveFlash* flash = fai->pFlash;

	if(strchr(fai->hFA.cUrl, '?') == NULL) {
		// make hash of url
		debug("Making TTH hash from URL...\n");
		TigerHash th;
		th.update(fai->hFA.cUrl, strlen(fai->hFA.cUrl));
		th.finalize();

		// create local path name
		TCHAR name[MAX_PATH], path[MAX_PATH];
		TCHAR tth[((TigerHash::HASH_SIZE * 8) / 5) + 2];
		FOLDERSGETDATA fgd = {0};

		fgd.cbSize = sizeof(FOLDERSGETDATA);
		fgd.nMaxPathSize = MAX_PATH;
		fgd.szPathT = path;
		if(!hAvatarsFolder || CallService(MS_FOLDERS_GET_PATH, (WPARAM)hAvatarsFolder, (LPARAM)&fgd)) {
			if(ServiceExists(MS_UTILS_REPLACEVARS)) {
				TCHAR *tmpPath = Utils_ReplaceVarsT(_T("%miranda_avatarcache%"));
				mir_sntprintf(path, MAX_PATH, _T("%s\\%s"), tmpPath, _T("Flash"));
				mir_free(tmpPath);
			} else
				CallService(MS_UTILS_PATHTOABSOLUTET, (WPARAM)(_T("Flash")), (LPARAM)path);
		} else {
			if(_tcslen(path) && path[_tcslen(path)-1]=='\\')
				path[_tcslen(path)-1] = 0;
		}

		CreateDirectory(path, NULL); // create directory if it doesn't exist
		_sntprintf(name, MAX_PATH, _T("%s\\%s.swf"), path, th.toBase32(tth));

		// download remote file if it doesn't exist
		if (GetFileAttributes(name) == 0xFFFFFFFF) {
			debug("Downloading flash file...\n");
			DownloadFlashFile(fai->hFA.cUrl, name, 0);
		}
		
		// load and play local flash movie
		debug("Loading flash movie...\n");
		flash->LoadMovie(0, _bstr_t(name).copy());
	}
	Sleep(100);
	flash->Play();

	// change flash frame according user's status
	int status;
	if (fai->hFA.hContact)
		status = DBGetContactSettingWord(fai->hFA.hContact, fai->getProto(), "Status", ID_STATUS_OFFLINE);
	else
		status = CallProtoService(fai->getProto(), PS_GETSTATUS, 0, 0);
	
	getFace();
	flash->SetVariable(L"face.emotion", _bstr_t(face).copy());
	flash->Release();
}

static void ShowBalloon(TCHAR *title, TCHAR *msg, int icon) {
	MIRANDASYSTRAYNOTIFY msn = {0};
	msn.cbSize = sizeof(MIRANDASYSTRAYNOTIFY);
	msn.dwInfoFlags = icon | NIIF_INTERN_UNICODE;
	msn.tszInfo = TranslateTS(msg);
	msn.tszInfoTitle = TranslateTS(title);
	msn.uTimeout = 5000;

	CLIST_INTERFACE* c = (CLIST_INTERFACE*)CallService(MS_CLIST_RETRIEVE_INTERFACE, 0, (LPARAM)g_hInst);
	if (c)
		c->pfnCListTrayNotify(&msn);
}

static void prepareFlash(char* pProto, const char* pUrl, FLASHAVATAR& fa, IShockwaveFlash* flash) {
	debug("Preparing flash...\n");
	if(flash == NULL) {
		// Flash component is not registered in the system
		ShowBalloon(LPGENT("Flash.ocx not registered!"), LPGENT("You don't have installed ShockwaveFlash interface in your system."), NIIF_ERROR);

		DestroyWindow(fa.hWindow);
		fa.hWindow = 0;
		return;
	}

	if(flash->FlashVersion() == 0x80000) {
		// Flash Version 8 has a bug which causes random crashes :(
		ShowBalloon(LPGENT("Bugged Flash detected!"), LPGENT("You have installed Flash 8.\r\nThis version of Flash contains a bug which can causes random crashes.\r\nIt is recommended to upgrade or downgrade your Flash library"),  NIIF_WARNING);
	}

	// attach flash object to window
	debug("Attaching flash to its window...\n");
	MyAtlAxAttachControl(flash, fa.hWindow, 0);

	// store avatar info
	debug("Storing avatar info...\n");
	fa.cProto = pProto;
	fa.cUrl = mir_strdup(pUrl);

	// create flash record
	flash_avatar_item *flash_item = new flash_avatar_item(fa.hContact, fa, flash);
	{
		Lock l(cs);
		FlashList.insert(flash_item);
	}
	
	// avatar contains parameter, load it from remote place
	if(strchr(fa.cUrl, '?')) {
		debug("Flash with parameters, loading...\n");
		flash->LoadMovie(0, fa.cUrl);
	}

	// refresh avatar's parent window
	// InvalidateRect(fa.hParentWindow, NULL, FALSE);

	// create thread to download/load flash avatar
	debug("Creating avatar thread...\n");
	flash->AddRef();
	mir_forkthread(&loadFlash_Thread, (void*)flash_item);
	//loadFlash(new FlashPair(make_pair(fa, flash)));
}

static int destroyAvatar(WPARAM wParam, LPARAM)
{
	flash_avatar_item key(((FLASHAVATAR*)wParam)->hContact, *(FLASHAVATAR*)wParam, NULL);
	
	Lock l(cs);

	flash_avatar_item *item = FlashList.find(&key);
	if (item) {
		if (item->pFlash)
			item->pFlash->Release();
		if (item->hFA.hWindow)
			DestroyWindow(item->hFA.hWindow);
		mir_free(item->hFA.cUrl);
		FlashList.remove(item);
		delete item;
	}
	return 0;
}

static int makeAvatar(WPARAM wParam, LPARAM)
{
	debug("Searching for flash avatar...\n");
	FLASHAVATAR* hFA = (FLASHAVATAR*)wParam;

	PROTO_AVATAR_INFORMATION AI = {0};
	AI.cbSize = sizeof(AI);
	AI.hContact = hFA->hContact;
	AI.format = PA_FORMAT_UNKNOWN;

	flash_avatar_item key(hFA->hContact, *hFA, NULL);
	
	bool avatarOK = false;
	if(hFA->hContact) {
		avatarOK = (int)CallProtoService(key.getProto(), PS_GETAVATARINFO, 0, (LPARAM)&AI) == GAIR_SUCCESS;
	} else {
		avatarOK = (int)CallProtoService(key.getProto(), PS_GETMYAVATAR, (WPARAM)AI.filename, (LPARAM)255) == 0;
		if(avatarOK) {
			char* ext = strrchr(AI.filename, '.');
			if(ext && (_stricmp(ext, ".xml") == 0))
				AI.format = PA_FORMAT_XML;
		}
	}
	
	if(!avatarOK) return 0;
	debug("Avatar found...\n");
	
	char* url = NULL;
	switch(AI.format) {
		case PA_FORMAT_SWF:
			url = AI.filename;
			break;
		case PA_FORMAT_XML: {
			int src = _open(AI.filename, _O_BINARY | _O_RDONLY);
			if(src != -1) {
				char pBuf[2048];
				char* urlBuf;
				_read(src, pBuf, sizeof(pBuf));
				_close(src);
					
				urlBuf = strstr(pBuf, "<URL>");
				if(urlBuf)
 					url = strtok(urlBuf + 5, "\r\n <");
 				else
 					return 0;
 			} else {
 				return 0;
 			}
 			break;
 		}
 		default:
 			destroyAvatar(wParam, 0);
 			return 0;
	}
	
	Lock l(cs);
	flash_avatar_item *item = FlashList.find(&key);
	if (item) {
		debug("Flash already exists...\n");
		hFA->hWindow = item->hFA.hWindow;
		ShowWindow(hFA->hWindow, SW_SHOW);
				
		if(_stricmp(item->hFA.cUrl, url) != 0) {
			debug("Refreshing flash...\n");
			IShockwaveFlash* flash = item->pFlash;
			mir_free(item->hFA.cUrl);
			FlashList.remove(item);
			delete item;
	
			prepareFlash(key.getProto(), url, *hFA, flash);
		}
	}	else {
		debug("Creating new flash...\n");
		RECT rc;
		GetWindowRect(hFA->hParentWindow, &rc);
		hFA->hWindow = CreateWindowEx(WS_EX_TOPMOST, _T("AtlAxWin"), _T(""), WS_VISIBLE | WS_CHILD, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hFA->hParentWindow, (HMENU) 0, g_hInst, NULL);

		IShockwaveFlash* flash = NULL;
		debug("Creating flash instance...\n");
		CoCreateInstance(__uuidof(ShockwaveFlash),0,CLSCTX_ALL, __uuidof(IShockwaveFlash), (void **)&flash);
		debug("Initialized.\n");

		prepareFlash(key.getProto(), url, *hFA, flash);
	}	
	return 0;
}

static int resizeAvatar(WPARAM wParam, LPARAM lParam)
{
	FLASHAVATAR* hFA = (FLASHAVATAR*)wParam;
	RECT rc = *((LPRECT)lParam);
	flash_avatar_item key(hFA->hContact, *hFA, NULL);	
	
	Lock l(cs);
	flash_avatar_item *item = FlashList.find(&key);
	if (item)
		SetWindowPos(item->hFA.hWindow, HWND_TOP, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);

	return 0;
}

static int setPos(WPARAM wParam, LPARAM lParam)
{
	FLASHAVATAR* hFA = (FLASHAVATAR*)wParam;
	RECT rc = *((LPRECT)lParam);
	flash_avatar_item key(hFA->hContact, *hFA, NULL);	
	
	Lock l(cs);
	flash_avatar_item *item = FlashList.find(&key);
	if (item) 
		SetWindowPos(item->hFA.hWindow, HWND_TOP, rc.left, rc.top, rc.right, rc.bottom, SWP_SHOWWINDOW);

	return 0;
}

static int getInfo(WPARAM wParam, LPARAM)
{
	FLASHAVATAR* hFA = (FLASHAVATAR*)wParam;
	flash_avatar_item key(hFA->hContact, *hFA, NULL);	
	
	Lock l(cs);
	flash_avatar_item *item = FlashList.find(&key);
	if (item) {
		//IShockwaveFlash* flash = item->pFlash;
		hFA->hWindow = item->hFA.hWindow;
		hFA->cUrl = item->hFA.cUrl;
		hFA->cProto = item->hFA.cProto;
	}
	return 0;
}

static int setEmoFace(WPARAM wParam, LPARAM lParam)
{
	FLASHAVATAR* hFA = (FLASHAVATAR*)wParam;
	flash_avatar_item key(hFA->hContact, *hFA, NULL);	
	
	Lock l(cs);
	flash_avatar_item *item = FlashList.find(&key);
	if (item && item->pFlash) {
		IShockwaveFlash* flash = item->pFlash;
		flash->SetVariable(L"face.emotion", (BSTR)lParam);
	}
	return 0;
}

static int setBkColor(WPARAM wParam, LPARAM lParam)
{
	FLASHAVATAR* hFA = (FLASHAVATAR*)wParam;
	COLORREF clr = (COLORREF)lParam;
	flash_avatar_item key(hFA->hContact, *hFA, NULL);	

	Lock l(cs);
	flash_avatar_item *item = FlashList.find(&key);
	if (item && item->pFlash) {
		IShockwaveFlash* flash = item->pFlash;

		char buf[10];
		_snprintf(buf, sizeof(buf), "%02X%02X%02X", LOBYTE(LOWORD(clr)), HIBYTE(LOWORD(clr)), LOBYTE(HIWORD(clr)));
		flash->put_BGColor(_bstr_t(buf));	
	}
	return 0;
}

static int ownStatusChanged(WPARAM wParam, LPARAM lParam)
{
	WORD status = (WORD)wParam;
	const char* proto = (char*)lParam;

	Lock l(cs);
	for(int i = 0; i < FlashList.getCount(); i++) {
		flash_avatar_item *item = FlashList[i];
		if(item->hContact == NULL && (!proto || (strcmpnull(item->hFA.cProto, proto) == 0))) {
			IShockwaveFlash* flash = item->pFlash;
			if (flash) {
				getFace();
				flash->SetVariable(L"face.emotion", _bstr_t(face).copy());
			}
		} else if (item->hContact)
			break; // the list is sorted by hContact
	}
	return 0;
}

static int statusChanged(WPARAM wParam, LPARAM lParam)
{
	WORD status = HIWORD(lParam);

	Lock l(cs);
	for(int i = 0; i < FlashList.getCount(); i++) {
		flash_avatar_item *item = FlashList[i];
		if (item->hContact == (HANDLE)wParam) {
  		IShockwaveFlash* flash = item->pFlash;	
			if (flash) {
				getFace();
				flash->SetVariable(L"face.emotion", _bstr_t(face).copy());
			}
		} else if (item->hContact > (HANDLE)wParam)
			break; // the list is sorted by hContact
	}
	return 0;
}

static int eventAdded(WPARAM wParam, LPARAM lParam)
{
	DBEVENTINFO dbei = {0};
	dbei.cbSize = sizeof(dbei);
	dbei.cbBlob = CallService(MS_DB_EVENT_GETBLOBSIZE, (WPARAM)lParam , 0);
	if (dbei.cbBlob == 0xFFFFFFFF)
		return 0;

	dbei.pBlob = new BYTE[dbei.cbBlob];
	CallService(MS_DB_EVENT_GET, (WPARAM)lParam, (LPARAM) & dbei);

	if (dbei.eventType == EVENTTYPE_MESSAGE && !(dbei.flags & DBEF_READ)) {
		Lock l(cs);
		if(FlashList.getCount() > 0) {
			//size_t aLen = strlen((char *)dbei.pBlob)+1;
			char* face = NULL;
	
			if(	(strstr((char*)dbei.pBlob, (char*)":-)") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)":)") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)";)") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)";-)") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)"*THUMBS UP*") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)"O:-)") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)":P") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)":-P") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)"*Drink*") != NULL)) { face = AV_SMILE; }
			else
			if(	(strstr((char*)dbei.pBlob, (char*)":-(") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)":-$") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)":-!") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)":-X") != NULL)) { face = AV_SAD; }
			else
			if(	(strstr((char*)dbei.pBlob, (char*)"*JOKINGLY*") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)":-D") != NULL)) { face = AV_LAUGH; }
			else
			if(	(strstr((char*)dbei.pBlob, (char*)":'(") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)":'-(") != NULL)) { face = AV_CRY; }
			else
			if(	(strstr((char*)dbei.pBlob, (char*)">:o") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)":-@") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)"*STOP*") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)"]:->") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)"@=") != NULL)) { face = AV_MAD; }
			else
			if(	(strstr((char*)dbei.pBlob, (char*)":-*") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)":-[") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)"*KISSED*") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)"*KISSING*") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)"@}->--") != NULL) ||
				(strstr((char*)dbei.pBlob, (char*)"*IN LOVE*") != NULL)) { face = AV_LOVE; }
			else {
				face = AV_NORMAL;
			}

			HANDLE hContact = (dbei.flags & DBEF_SENT) ? 0 : (HANDLE)wParam;
			for(int i=0; i<FlashList.getCount(); i++) {
				flash_avatar_item *item = FlashList[i];
				if (item->hContact == hContact && !strcmpnull(dbei.szModule, item->getProto())) {
					IShockwaveFlash* flash = item->pFlash;
					flash->SetVariable(L"face.emotion", _bstr_t(face).copy());
					//break;
				} else if (item->hContact > hContact)
					break; // the list is sorted
			}
		}
	}

	delete[] dbei.pBlob;
	return 0;
}


static int systemModulesLoaded(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	HMODULE hAtl = LoadLibrary(_T("atl"));
	void* init = GetProcAddress(hAtl, "AtlAxWinInit"); _asm call init;
	MyAtlAxAttachControl = (LPAtlAxAttachControl)GetProcAddress(hAtl, "AtlAxAttachControl"); 

	hServices[0] = CreateServiceFunction(MS_FAVATAR_DESTROY,    destroyAvatar);	
	hServices[1] = CreateServiceFunction(MS_FAVATAR_MAKE,       makeAvatar);
	hServices[2] = CreateServiceFunction(MS_FAVATAR_RESIZE,     resizeAvatar);
	hServices[3] = CreateServiceFunction(MS_FAVATAR_SETPOS,     setPos);
	hServices[4] = CreateServiceFunction(MS_FAVATAR_GETINFO,    getInfo);
	hServices[5] = CreateServiceFunction(MS_FAVATAR_SETEMOFACE, setEmoFace);
	hServices[6] = CreateServiceFunction(MS_FAVATAR_SETBKCOLOR, setBkColor);

	hHooks[1] = HookEvent(ME_DB_EVENT_ADDED, eventAdded);
	hHooks[2] = HookEvent("Miranda/StatusChange/ContactStatusChanged", statusChanged); // NewStatusNotify
	hHooks[3] = HookEvent(ME_CLIST_STATUSMODECHANGE, ownStatusChanged);

	NETLIBUSER nl_user = {0};
	nl_user.cbSize = sizeof(nl_user);
	nl_user.szSettingsModule = "FlashAvatars";
	nl_user.flags = NUF_OUTGOING | NUF_HTTPCONNS;
	nl_user.szDescriptiveName = Translate("Flash Avatars");
	
	hNetlibUser = (HANDLE)CallService(MS_NETLIB_REGISTERUSER, 0, (LPARAM)&nl_user);

	TCHAR path[MAX_PATH];
	if (ServiceExists(MS_UTILS_REPLACEVARS))
	{ // default Avatar Cache path for MIM 0.8+
		TCHAR *tmpPath = Utils_ReplaceVarsT(_T("%miranda_avatarcache%"));
		mir_sntprintf(path, MAX_PATH, _T("%s\\%s\\"), tmpPath, _T("Flash"));
		mir_free(tmpPath);
	} // default for older Mirandas
	else
		CallService(MS_UTILS_PATHTOABSOLUTET, (WPARAM)(_T("Flash\\")), (LPARAM)path);
	hAvatarsFolder = FoldersRegisterCustomPathT("Flash Avatars", "Avatars Cache", path);

	return 0;
}


extern "C" __declspec(dllexport) PLUGININFOEX * MirandaPluginInfoEx(DWORD mirandaVersion)
{
	if (mirandaVersion < PLUGIN_MAKE_VERSION(0, 4, 0, 0))
		return NULL;

	TCHAR filename[MAX_PATH];
	GetModuleFileName(g_hInst, filename, SIZEOF(filename));

	DWORD unused;
	DWORD verInfoSize = GetFileVersionInfoSize(filename, &unused);
	PVOID pVerInfo = malloc(verInfoSize);
	GetFileVersionInfo(filename, 0, verInfoSize, pVerInfo);

	TCHAR *productVersion;
	UINT blockSize;
	VerQueryValue(pVerInfo, _T("\\StringFileInfo\\040504b0\\FileVersion"), (PVOID*)&productVersion, &blockSize);

#ifdef _UNICODE
	_snprintf(pluginName, SIZEOF(pluginName), "Flash avatars service [build #%S]", _tcsrchr(productVersion, ' ') + 1);
#else
	_snprintf(pluginName, SIZEOF(pluginName), "Flash avatars service [build #%s]", strrchr(productVersion, ' ') + 1);
#endif
	pluginInfo.shortName = pluginName;

	free(pVerInfo);
	return &pluginInfo;
}

static const MUUID interfaces[] = { { 0xc6fbb128, 0x81b4, 0x4221, { 0xa4, 0xb9, 0xe6, 0x34, 0x7c, 0x26, 0x4a, 0x49 } }, MIID_LAST};
extern "C" __declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}


extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink = link;
	mir_getMMI(&mmi);
	mir_getLI(&li);

	hHooks[0] = HookEvent(ME_SYSTEM_MODULESLOADED, systemModulesLoaded);

  return 0;
}

extern "C" int __declspec(dllexport) Unload(void)
{
	// Shutdown cleanup	
	{
		Lock l(cs);
		for (int i = FlashList.getCount()-1; i >= 0; i--)
		{
			flash_avatar_item *item = FlashList[i];
			if (item->pFlash)
				item->pFlash->Release();
			if (item->hFA.hWindow)
				DestroyWindow(item->hFA.hWindow);
			mir_free(item->hFA.cUrl);
			delete item;
		}
		FlashList.destroy();
	}
	
	int i;

	for (i = 0; i < SIZEOF(hHooks); i++)
		if (hHooks[i])
			UnhookEvent(hHooks[i]);

	for (i = 0; i < SIZEOF(hServices); i++)
		if (hServices[i])
			DestroyServiceFunction(hServices[i]);

	if (hNetlibUser)
		CallService(MS_NETLIB_CLOSEHANDLE, (WPARAM)hNetlibUser, 0);

	return 0;
}

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD /*dwReason*/, LPVOID /*reserved*/)
{
	g_hInst = hInstDLL;
	return TRUE;
}
