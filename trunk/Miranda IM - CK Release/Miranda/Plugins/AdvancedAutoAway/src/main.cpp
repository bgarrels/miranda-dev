/*
	AdvancedAutoAway Plugin for Miranda-IM (www.miranda-im.org)
	Copyright 2003-2006 P. Boon

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


	Some code is copied from Miranda's AutoAway module
*/

#include "commonstatus.h"
#include "advancedautoaway.h"
#include "../resource.h"

// {F0FDF73A-753D-499d-8DBA-336DB79CDD41}
#define MIID_ADVAUTOAWAY { 0xf0fdf73a, 0x753d, 0x499d, { 0x8d, 0xba, 0x33, 0x6d, 0xb7, 0x9c, 0xdd, 0x41 } }

HINSTANCE hInst;
PLUGINLINK *pluginLink;

MM_INTERFACE mmi;
LIST_INTERFACE li;

HANDLE hCSModuleLoadedHook;
HANDLE hStateChangedEvent;
int hLangpack = 0;

int CSModuleLoaded(WPARAM wParam, LPARAM lParam);

/////////////////////////////////////////////////////////////////////////////////////////
// dll entry point

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	hInst = hinstDLL;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// returns plugin's extended information

PLUGININFOEX pluginInfoEx={
	sizeof(PLUGININFOEX),
	#if defined( _UNICODE )
		__PLUGIN_NAME __PLATFORM_NAME " (Unicode)",
	#else
		__PLUGIN_NAME,
	#endif
	PLUGIN_MAKE_VERSION(__MAJOR_VERSION, __MINOR_VERSION, __RELEASE_NUM, __BUILD_NUM),
	__DESC,
	__AUTHOR,
	__AUTHOREMAIL,
	__COPYRIGHT,
	__AUTHORWEB,
	UNICODE_AWARE,
	0,
	MIID_ADVAUTOAWAY
};

extern "C" __declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
	return &pluginInfoEx;
}

/////////////////////////////////////////////////////////////////////////////////////////
// returns plugin's interfaces information

static const MUUID interfaces[] = { MIID_AUTOAWAY, MIID_LAST };

extern "C" __declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}

/////////////////////////////////////////////////////////////////////////////////////////
// plugin's entry point

extern "C" __declspec(dllexport) int Load(PLUGINLINK *link)
{
	pluginLink = link;
	mir_getMMI(&mmi);
	mir_getLI(&li);
	mir_getLP( &pluginInfoEx );

	InitCommonStatus();
	hCSModuleLoadedHook = HookEvent(ME_SYSTEM_MODULESLOADED, CSModuleLoaded);
	hStateChangedEvent = CreateHookableEvent(ME_AAA_STATECHANGED);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// plugin's exit point

extern "C" __declspec(dllexport) int Unload(void)
{
	DestroyHookableEvent( hStateChangedEvent );
	return 0;
}
