/*
Copyright © 2009 Jim Porter

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "common.h"
#include "version.h"

#include "proto.h"
#include "theme.h"

//ExternalApi
#include "m_updater.h"

PLUGINLINK *pluginLink;
MD5_INTERFACE md5i;
MM_INTERFACE mmi;
UTF8_INTERFACE utfi;
LIST_INTERFACE li;

CLIST_INTERFACE* pcli;

HINSTANCE g_hInstance;

PLUGININFOEX pluginInfo={
	sizeof(PLUGININFOEX),
	#ifdef WIN64
	"Twitter Plugin (x64)",
	#else
	"Twitter Plugin",
	#endif
	__VERSION_DWORD,
	"Provides basic support for Twitter protocol. [Built: "__DATE__" "__TIME__"]",
	"dentist, omniwolf, Thief",
	"",
	"© 2009-2010 dentist, 2010-2011 omniwolf and Thief",
	"http://code.google.com/p/miranda-twitter-oauth/",
	UNICODE_AWARE, //not transient
	0,             //doesn't replace anything built-in
	#ifdef WIN64
	// {ECD828A6-FF3C-4F24-95CE-5FF73A06FB6F}
	{ 0xecd828a6, 0xff3c, 0x4f24, { 0x95, 0xce, 0x5f, 0xf7, 0x3a, 0x6, 0xfb, 0x6f } }
	#else
	//{BC09A71B-B86E-4d33-B18D-82D30451DD3C}
	{ 0xbc09a71b, 0xb86e, 0x4d33, { 0xb1, 0x8d, 0x82, 0xd3, 0x4, 0x51, 0xdd, 0x3c } }
	#endif
};

/////////////////////////////////////////////////////////////////////////////
// Protocol instances
static int compare_protos(const TwitterProto *p1, const TwitterProto *p2)
{
	return _tcscmp(p1->m_tszUserName, p2->m_tszUserName);
}

OBJLIST<TwitterProto> g_Instances(1, compare_protos);

DWORD WINAPI DllMain(HINSTANCE hInstance,DWORD,LPVOID)
{
	g_hInstance = hInstance;
	return TRUE;
}

extern "C" __declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
	if(mirandaVersion < PLUGIN_MAKE_VERSION(0,8,0,29))
	{
		MessageBox(0,_T("The Twitter protocol plugin cannot be loaded. ")
			_T("It requires Miranda IM 0.9.4 or later."),_T("Miranda"),
			MB_OK|MB_ICONWARNING|MB_SETFOREGROUND|MB_TOPMOST);
		return NULL;
	}

	/*unsigned long mv=_htonl(mirandaVersion);
	memcpy(&AIM_CAP_MIRANDA[8],&mv,sizeof(DWORD));
	memcpy(&AIM_CAP_MIRANDA[12],AIM_OSCAR_VERSION,sizeof(DWORD));*/
	return &pluginInfo;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Interface information

static const MUUID interfaces[] = {MIID_PROTOCOL, MIID_LAST};
extern "C" __declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Load

static PROTO_INTERFACE* protoInit(const char *proto_name,const TCHAR *username )
{
	TwitterProto *proto = new TwitterProto(proto_name,username);
	g_Instances.insert(proto);
	return proto;
}

static int protoUninit(PROTO_INTERFACE *proto)
{
	g_Instances.remove(static_cast<TwitterProto*>(proto));
	return 0;
}

int OnModulesLoaded(WPARAM,LPARAM)
{
	if(ServiceExists(MS_UPDATE_REGISTER))
	{
		Update upd = {sizeof(upd)};
		char curr_version[30];

		upd.szComponentName = pluginInfo.shortName;

		upd.szUpdateURL = UPDATER_AUTOREGISTER;

		upd.szBetaVersionURL     = "http://twosx.net/mim/twitter/updater/version.html";
		upd.szBetaChangelogURL   = "http://twosx.net/mim/twitter/updater/changelog.html";
		upd.pbBetaVersionPrefix  = reinterpret_cast<BYTE*>("Twitter ");
		upd.cpbBetaVersionPrefix = strlen(reinterpret_cast<char*>(upd.pbBetaVersionPrefix));
#ifdef UNICODE
		upd.szBetaUpdateURL      = "http://twosx.net/mim/twitter/updater/twitter.zip";
#else
		upd.szBetaUpdateURL      = "http://www.teamboxel.com/update/twitter/ansi";
#endif

		upd.pbVersion = reinterpret_cast<BYTE*>( CreateVersionStringPlugin(
			reinterpret_cast<PLUGININFO*>(&pluginInfo),curr_version) );
		upd.cpbVersion = strlen(reinterpret_cast<char*>(upd.pbVersion));

		CallService(MS_UPDATE_REGISTER,0,(LPARAM)&upd);
	}

	return 0;
}

static HANDLE g_hEvents[1];

extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink = link;
	mir_getMMI(&mmi);
	mir_getMD5I(&md5i);
	mir_getUTFI(&utfi);
	mir_getLI(&li);

	pcli = reinterpret_cast<CLIST_INTERFACE*>( CallService(
		MS_CLIST_RETRIEVE_INTERFACE,0,reinterpret_cast<LPARAM>(g_hInstance)) );

	PROTOCOLDESCRIPTOR pd = {sizeof(pd)};
	pd.szName = "Twitter";
	pd.type = PROTOTYPE_PROTOCOL;
	pd.fnInit = protoInit;
	pd.fnUninit = protoUninit;
	CallService(MS_PROTO_REGISTERMODULE,0,reinterpret_cast<LPARAM>(&pd));

	g_hEvents[0] = HookEvent(ME_SYSTEM_MODULESLOADED,OnModulesLoaded);

	InitIcons();
	InitContactMenus();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Unload

extern "C" int __declspec(dllexport) Unload(void)
{
	UninitContactMenus();
	for(size_t i=1; i<SIZEOF(g_hEvents); i++)
		UnhookEvent(g_hEvents[i]);

	return 0;
}