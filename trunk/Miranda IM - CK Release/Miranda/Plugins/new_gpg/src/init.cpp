/*
New_gpg plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author 
			Copyright (C) 2010-2011 sss

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

//global variables
bool bAppendTags = false, bDebugLog = false, bJabberAPI = false, bIsMiranda09 = false, bMetaContacts = false, bFileTransfers = false, bAutoExchange = false;
TCHAR *inopentag = NULL, *inclosetag = NULL, *outopentag = NULL, *outclosetag = NULL, *password = NULL;

list <JabberAccount*> Accounts;

HINSTANCE hInst;
HANDLE hLoadPubKey = NULL, hToggleEncryption = NULL, hOnPreBuildContactMenu = NULL, hSendKey = NULL, g_hCLIcon = NULL;
PLUGINLINK *pluginLink;
IconExtraColumn g_IEC = {0};
static int OnModulesLoaded(WPARAM wParam,LPARAM lParam);
extern char *date();
MM_INTERFACE mmi = {0};
UTF8_INTERFACE  utfi = {0};
RECT key_from_keyserver_rect = {0}, firstrun_rect = {0}, new_key_rect = {0}, key_gen_rect = {0}, load_key_rect = {0}, import_key_rect = {0}, key_password_rect = {0}, load_existing_key_rect = {0};
XML_API xi = {0};
logtofile debuglog;
bool gpg_configured = false;
std::map<HANDLE, contact_data> hcontact_data;


#define MIID_GPG { 0x4227c050, 0x8d97, 0x48d2, { 0x91, 0xec, 0x6a, 0x95, 0x2b, 0x3d, 0xab, 0x94 } }

PLUGININFOEX pluginInfo={
	sizeof(PLUGININFOEX),
	0,
	PLUGIN_MAKE_VERSION(0,0,0,11),
	"new GPG encryption support plugin, based on code from old gpg plugin and secureim",
	"sss",
	"sss123next@list.ru",
	"� 2010-2011 sss",
	"http://sss.chaoslab.ru/tracker/mim_plugs/",
	1,		//unicode
	0,		//doesn't replace anything built-in
	MIID_GPG
};

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	hInst=hinstDLL;
	return TRUE;
}


extern "C" __declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
	static char plugname[52];
	strcpy(plugname, szGPGModuleName" [");
	strcat(plugname, date());
	strcat(plugname, " ");
	strcat(plugname, __TIME__);
	strcat(plugname, "]");
	pluginInfo.shortName = plugname;
	return &pluginInfo;
}

static const MUUID interfaces[] = {MIID_GPG, MIID_LAST};
extern "C" __declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}

int LoadKey(WPARAM w, LPARAM l);
int ToggleEncryption(WPARAM w, LPARAM l);
int SendKey(WPARAM w, LPARAM l);

void init_vars()
{
	bAppendTags = DBGetContactSettingByte(NULL, szGPGModuleName, "bAppendTags", 0);
	inopentag = UniGetContactSettingUtf(NULL, szGPGModuleName, "szInOpenTag", _T("<GPGdec>"));
	inclosetag = UniGetContactSettingUtf(NULL, szGPGModuleName, "szInCloseTag", _T("</GPGdec>"));
	outopentag = UniGetContactSettingUtf(NULL, szGPGModuleName, "szOutOpenTag", _T("<GPGenc>"));
	outclosetag = UniGetContactSettingUtf(NULL, szGPGModuleName, "szOutCloseTag", _T("</GPGenc>"));
	bDebugLog = DBGetContactSettingByte(NULL, szGPGModuleName, "bDebugLog", 0);
	bAutoExchange = DBGetContactSettingByte(NULL, szGPGModuleName, "bAutoExchange", 0);
	debuglog.init();
	bJabberAPI = DBGetContactSettingByte(NULL, szGPGModuleName, "bJabberAPI", bIsMiranda09?1:0);
	bFileTransfers = DBGetContactSettingByte(NULL, szGPGModuleName, "bFileTransfers", 1);
	firstrun_rect.left = DBGetContactSettingDword(NULL, szGPGModuleName, "FirstrunWindowX", 0);
	firstrun_rect.top = DBGetContactSettingDword(NULL, szGPGModuleName, "FirstrunWindowY", 0);
	key_password_rect.left = DBGetContactSettingDword(NULL, szGPGModuleName, "PasswordWindowX", 0);
	key_password_rect.top = DBGetContactSettingDword(NULL, szGPGModuleName, "PasswordWindowY", 0);
	key_gen_rect.left = DBGetContactSettingDword(NULL, szGPGModuleName, "KeyGenWindowX", 0);
	key_gen_rect.top = DBGetContactSettingDword(NULL, szGPGModuleName, "KeyGenWindowY", 0);
	load_key_rect.left = DBGetContactSettingDword(NULL, szGPGModuleName, "LoadKeyWindowX", 0);
	load_key_rect.top = DBGetContactSettingDword(NULL, szGPGModuleName, "LoadKeyWindowY", 0);
	import_key_rect.left = DBGetContactSettingDword(NULL, szGPGModuleName, "ImportKeyWindowX", 0);
	import_key_rect.top = DBGetContactSettingDword(NULL, szGPGModuleName, "ImportKeyWindowY", 0);
	new_key_rect.left = DBGetContactSettingDword(NULL, szGPGModuleName, "NewKeyWindowX", 0);
	new_key_rect.top = DBGetContactSettingDword(NULL, szGPGModuleName, "NewKeyWindowY", 0);
	load_existing_key_rect.left = DBGetContactSettingDword(NULL, szGPGModuleName, "LoadExistingKeyWindowX", 0);
	load_existing_key_rect.top = DBGetContactSettingDword(NULL, szGPGModuleName, "LoadExistingKeyWindowY", 0);
}

extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink=link;
	init_vars();
	HookEvent(ME_SYSTEM_MODULESLOADED, OnModulesLoaded);
	mir_getMMI(&mmi);
	mir_getUTFI(&utfi);
	mir_getXI(&xi);	//TODO: check if we have access to api
	CreateServiceFunction("/LoadPubKey",(MIRANDASERVICE)LoadKey);
	CreateServiceFunction("/ToggleEncryption",(MIRANDASERVICE)ToggleEncryption);
	CreateServiceFunction("/SendKey",(MIRANDASERVICE)SendKey);
	CLISTMENUITEM mi = {0};
	mi.cbSize=sizeof(mi);
	mi.position=-0x7FFFFFFF;
	mi.flags=0;
	mi.hIcon=LoadSkinnedIcon(SKINICON_OTHER_MIRANDA);
	mi.pszName="Load GPG public key";
	mi.pszService="/LoadPubKey";
	hLoadPubKey = (HANDLE)CallService(MS_CLIST_ADDCONTACTMENUITEM,0,(LPARAM)&mi);
	ZeroMemory(&mi,sizeof(mi));
	mi.cbSize=sizeof(mi);
	mi.position=-0x7FFFFFFe;
	mi.flags=0;
	mi.hIcon=LoadSkinnedIcon(SKINICON_OTHER_MIRANDA);
	mi.pszName="Toggle GPG encryption";
	mi.pszService="/ToggleEncryption";
	hToggleEncryption = (HANDLE)CallService(MS_CLIST_ADDCONTACTMENUITEM,0,(LPARAM)&mi);
	ZeroMemory(&mi,sizeof(mi));
	mi.cbSize=sizeof(mi);
	mi.position=-0x7FFFFFFe;
	mi.flags=0;
	mi.hIcon=LoadSkinnedIcon(SKINICON_OTHER_MIRANDA);
	mi.pszName="Send public key";
	mi.pszService="/SendKey";
	hSendKey = (HANDLE)CallService(MS_CLIST_ADDCONTACTMENUITEM,0,(LPARAM)&mi);
	return 0;
}

int AddContact(WPARAM w, LPARAM l)
{
	CallService(MS_PROTO_ADDTOCONTACT,w,(LPARAM)szGPGModuleName);
	return 0;
}


static int OnModulesLoaded(WPARAM wParam,LPARAM lParam)
{

	int GpgOptInit(WPARAM wParam,LPARAM lParam);
	int OnPreBuildContactMenu(WPARAM w, LPARAM l);
	int RecvMsgSvc(WPARAM w, LPARAM l);
	int SendMsgSvc(WPARAM w, LPARAM l);
	int HookSendMsg(WPARAM w, LPARAM l);
//	int TestHook(WPARAM w, LPARAM l);
	int GetJabberInterface(WPARAM w, LPARAM l);
	int onWindowEvent(WPARAM wParam, LPARAM lParam);
	int onIconPressed(WPARAM wParam, LPARAM lParam);
	int onExtraImageListRebuilding(WPARAM, LPARAM);
	int onExtraImageApplying(WPARAM wParam, LPARAM);
	int onProtoAck(WPARAM, LPARAM);
	int onSendFile(WPARAM, LPARAM);
	void InitIconLib();

	void InitCheck();
	void FirstRun();
	bIsMiranda09 = (DWORD)CallService(MS_SYSTEM_GETVERSION, 0, 0) >= 0x00090001?true:false;
	FirstRun();
	InitCheck();
	InitIconLib();
	if(ServiceExists(MS_MSG_ADDICON)) 
	{
		HICON IconLibGetIcon(const char* ident);
		StatusIconData sid = {0};
		sid.cbSize = sizeof(sid);
		sid.szModule = szGPGModuleName;
		sid.flags = MBF_HIDDEN;
		sid.dwId = 0x00000001;
		sid.hIcon = IconLibGetIcon("secured");
		sid.szTooltip = Translate("GPG Turn off encryption");
		CallService(MS_MSG_ADDICON, 0, (LPARAM)&sid);
		ZeroMemory(&sid, sizeof(sid));
		sid.cbSize = sizeof(sid);
		sid.szModule = szGPGModuleName;
		sid.flags = MBF_HIDDEN;
		sid.dwId = 0x00000002;
		sid.hIcon = IconLibGetIcon("unsecured");
		sid.szTooltip = Translate("GPG Turn on encryption");
		CallService(MS_MSG_ADDICON, 0, (LPARAM)&sid);
	}


	bMetaContacts = ServiceExists(MS_MC_GETMETACONTACT);
	
	if(bJabberAPI && bIsMiranda09)
		GetJabberInterface(0,0);

	HookEvent(ME_OPT_INITIALISE, GpgOptInit);
	HookEvent(ME_DB_EVENT_FILTER_ADD, HookSendMsg);
	if(bJabberAPI && bIsMiranda09)
		HookEvent(ME_PROTO_ACCLISTCHANGED, GetJabberInterface);

	HookEvent(ME_PROTO_ACK, onProtoAck); //filetransfer unimplemented now

	HookEvent(ME_CLIST_PREBUILDCONTACTMENU, OnPreBuildContactMenu);

	HookEvent(ME_MSG_WINDOWEVENT, onWindowEvent);
	HookEvent(ME_MSG_ICONPRESSED, onIconPressed);

	if(ServiceExists(MS_EXTRAICON_REGISTER)) 
		g_hCLIcon = ExtraIcon_Register(szGPGModuleName, Translate("GPG encryption status"), "secured", (MIRANDAHOOK)onExtraImageListRebuilding, (MIRANDAHOOK)onExtraImageApplying);



	PROTOCOLDESCRIPTOR pd = {0};
	pd.cbSize=sizeof(PROTOCOLDESCRIPTOR);
	pd.szName=szGPGModuleName;
	pd.type=PROTOTYPE_ENCRYPTION;
	CallService(MS_PROTO_REGISTERMODULE,0,(LPARAM)&pd);
	
	CreateProtoServiceFunction(szGPGModuleName, PSR_MESSAGE, (MIRANDASERVICE)RecvMsgSvc);
	CreateProtoServiceFunction(szGPGModuleName, PSS_MESSAGE, (MIRANDASERVICE)SendMsgSvc);
	CreateProtoServiceFunction(szGPGModuleName, PSR_MESSAGE"W", (MIRANDASERVICE)RecvMsgSvc);
	CreateProtoServiceFunction(szGPGModuleName, PSS_MESSAGE"W", (MIRANDASERVICE)SendMsgSvc);

	CreateProtoServiceFunction(szGPGModuleName, PSS_FILE, (MIRANDASERVICE)onSendFile);
	CreateProtoServiceFunction(szGPGModuleName, PSS_FILE"W", (MIRANDASERVICE)onSendFile);

	for (HANDLE hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0); hContact; hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0))
		if (!CallService(MS_PROTO_ISPROTOONCONTACT, (WPARAM)hContact, (LPARAM)szGPGModuleName))
			CallService(MS_PROTO_ADDTOCONTACT, (WPARAM)hContact, (LPARAM)szGPGModuleName);

	HookEvent(ME_DB_CONTACT_ADDED,AddContact);


	return 0;
}

extern list<wstring> transfers;
extern "C" int __declspec(dllexport) Unload(void)
{
//	for (HANDLE hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0); hContact; hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0))
//		DBDeleteContactSetting(hContact, szGPGModuleName, "KeyID_Prescense");
	if(!transfers.empty())
	{
		for(list<wstring>::iterator p = transfers.begin(); p != transfers.end(); p++)
			if(!(*p).empty())
				DeleteFile((*p).c_str());
	}
	mir_free(inopentag);
	mir_free(inclosetag);
	mir_free(outopentag);
	mir_free(outclosetag);
	if(password)
		delete [] password;
	return 0;
}
