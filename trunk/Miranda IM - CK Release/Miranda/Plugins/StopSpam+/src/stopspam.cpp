#include "../headers.h"

struct MM_INTERFACE mmi;
UTF8_INTERFACE utfi;
HANDLE hFunc, hTempRemove;
int hLangpack;

/////////////////////////////////////////////////////////////////////////////////////////
// returns plugin's extended information

// {553811EE-DEB6-48b8-8902-A8A00C1FD679}
#define MIID_STOPSPAM { 0x553811ee, 0xdeb6, 0x48b8, { 0x89, 0x2, 0xa8, 0xa0, 0xc, 0x1f, 0xd6, 0x79 } }

PLUGININFOEX pluginInfoEx = {
	sizeof(PLUGININFOEX),
	__PLUGIN_NAME,
	PLUGIN_MAKE_VERSION(__MAJOR_VERSION, __MINOR_VERSION, __RELEASE_NUM, __BUILD_NUM),
	__DESC,
	__AUTHOR,
	__AUTHOREMAIL,
	__COPYRIGHT,
	__AUTHORWEB,
	UNICODE_AWARE,
	0,
	MIID_STOPSPAM
};

PLUGINLINK *pluginLink;
HINSTANCE hInst;

_inline unsigned int MakeVer(int a,int b,int c,int d)
{
	return PLUGIN_MAKE_VERSION(a,b,c,d);
}

extern "C" __declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
	if ( mirandaVersion < MakeVer(__PRODVERSION_STRING))
		return NULL;

	return &pluginInfoEx;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	hInst = hinstDLL;
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////
// returns plugin's interfaces information

static const MUUID interfaces[] = { MIID_STOPSPAM, MIID_LAST };

extern "C"  __declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}

extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	CLISTMENUITEM mi;
	pluginLink = link;
	mir_getLP(&pluginInfoEx);
	mir_getMMI(&mmi);
	mir_getUTFI(&utfi);

	plSets=new Settings;

	hFunc = CreateServiceFunction(MS_STOPSPAM_CONTACTPASSED, IsContactPassed);

	HookEvent(ME_SYSTEM_MODULESLOADED, OnSystemModulesLoaded);

	// Add deliting temporary contacts
	hTempRemove = CreateServiceFunction(MS_STOPSPAM_REMTEMPCONTACTS, RemoveTempContacts);
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.position = -0x7FFFFFFF;
	mi.flags = CMIF_TCHAR;
	mi.hIcon=LoadSkinnedIcon(SKINICON_OTHER_MIRANDA);
	mi.ptszName = _T("Remove Temporary Contacts");
	mi.pszService = pluginName"/RemoveTempContacts";
	CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi);

	miranda::EventHooker::HookAll();
	return 0;
}

extern "C" int __declspec(dllexport) Unload(void)
{
	miranda::EventHooker::UnhookAll();
	
	if(hFunc)
	{
		DestroyServiceFunction(hFunc);
		hFunc = 0;
	}
	if(hTempRemove)
	{
		DestroyServiceFunction(hTempRemove);
		hFunc = 0;
	}
	delete plSets;
	
	return 0;
}