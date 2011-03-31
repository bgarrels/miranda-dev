/*
Wannabe OSD
This plugin tries to become miranda's standard OSD ;-)

(C) 2005 Andrej Krutak

Distributed under GNU's GPL 2 or later
*/

#include "wbOSD.h"
#include "buildnumber.h"

HINSTANCE hI;
MM_INTERFACE mmi;

HWND hwnd=0;
HANDLE hservosda;
PLUGINLINK *pluginLink;
HANDLE hHookedNewEvent, hHookedInit, hProtoAck, hContactSettingChanged, hHookContactStatusChanged, hContactStatusChanged, hpluginShutDown;
HINSTANCE hUser32;
BOOL (WINAPI*pSetLayeredWindowAttributes)(HWND, COLORREF, BYTE, DWORD);

void logmsg2(char *str);
int MainInit(WPARAM wparam,LPARAM lparam);

/////////////////////////////////////////////////////////////////////////////////////////

extern "C" int __declspec(dllexport)Load(PLUGINLINK *link)
{
	pluginLink=link;
	mir_getMMI( &mmi );

	logmsg("Load");
	pSetLayeredWindowAttributes=0;

#ifndef FORCE_9XDRAWING
	hUser32=LoadLibraryA("user32.dll");
#else
	hUser32=0;
#endif

	if (hUser32) {
		pSetLayeredWindowAttributes=(BOOL(WINAPI*)(HWND, COLORREF, BYTE, DWORD))GetProcAddress(hUser32, "SetLayeredWindowAttributes");
		if (!pSetLayeredWindowAttributes) {
			FreeLibrary(hUser32);
			hUser32=0;
		}
	}

	hHookedInit = HookEvent(ME_SYSTEM_MODULESLOADED, MainInit);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport) int Unload(void)
{
	logmsg("Unload");
	UnhookEvent(hpluginShutDown);
	UnhookEvent(hProtoAck);
	UnhookEvent(hContactSettingChanged);
	UnhookEvent(hContactStatusChanged);
	UnhookEvent(hHookedNewEvent);
	UnhookEvent(hHookedInit);

	if (hUser32)
		FreeLibrary(hUser32);
	pSetLayeredWindowAttributes=0;

	DestroyServiceFunction(hservosda);
	DestroyHookableEvent(hHookContactStatusChanged);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

static PLUGININFO pluginInfo={
	sizeof(PLUGININFO),
	#ifdef UNICODE
		"WannaBe OSD (Unicode)",
	#else
		"WannaBe OSD",
	#endif
	__VERSION_DWORD,
	"Show new message/status change info using onscreen display",
	"Andrej Krutak",
	"andree182 {at} gmail (dot) com",
	"©2005 Andrej Krutak",
	"http://urtax.ms.mff.cuni.cz/~andree/",
	UNICODE_AWARE,
	0
};

extern "C" __declspec(dllexport) PLUGININFO* MirandaPluginInfo(DWORD mirandaVersion)
{
	return &pluginInfo;
}

/////////////////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hinst,DWORD fdwReason,LPVOID lpvReserved)
{
	hI = hinst;
	return 1;
}
