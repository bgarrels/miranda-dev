/*
UserinfoEx plugin for Miranda IM

Copyright:
� 2006-2010 DeathAxe, Yasnovidyashii, Merlin, K. Romanov, Kreol

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

===============================================================================

File name      : $HeadURL: http://userinfoex.googlecode.com/svn/trunk/commonheaders.h $
Revision       : $Revision: 212 $
Last change on : $Date: 2011-04-20 15:02:24 +0400 (Ср, 20 апр 2011) $
Last change by : $Author: kreol13 $

===============================================================================
*/
#pragma once

/***********************************************************************************************************
 * some compiler definitions
 ***********************************************************************************************************/

#define MIRANDA_CUSTOM_LP

#define _WIN32_WINNT	0x0501
#define _WIN32_IE			0x0500
#define WIN32_LEAN_AND_MEAN

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 0

#pragma warning(disable:4995)		// disable warning about strcpy, ... is old in VC2005
#pragma warning(disable:4996)		// disable warning about strcpy, ... is old in VC2005

/***********************************************************************************************************
 * standard windows includes
 ***********************************************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <winnt.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <malloc.h>
#include <string>
#include <list>
#include <Richedit.h>

using namespace std;
typedef std::basic_string<TCHAR>	tstring;

/***********************************************************************************************************
 * Miranda IM SDK includes and macros
 ***********************************************************************************************************/

#define MIRANDA_VER 0x0900

#include <newpluginapi.h>	// This must be included first
#include <m_stdhdr.h>
#include <m_button.h>
#include <m_clui.h>
#include <m_clist.h>
#include <m_clistint.h>
#include <m_cluiframes.h>
#include <m_database.h>
#include <m_genmenu.h>
#include <m_hotkeys.h>
#include <m_langpack.h>
#include <m_protomod.h>
#include <m_protosvc.h>
#include <m_options.h>
#include <m_contacts.h>
#include <m_utils.h>
#include <m_system.h>		// memory interface
#include <m_system_cpp.h>	// list template
#include <m_xml.h>			// XML API
#include <m_timezones.h>	// timezone interface
#include <m_imgsrvc.h>
#include <m_message.h>
#include <m_userinfo.h>
#include <win2k.h>
#include <msapi/vsstyle.h>
#include <msapi/vssym32.h>

/***********************************************************************************************************
 * Used Plugins SDK includes and macros
 ***********************************************************************************************************/

#include <m_popup.h>
#include "m_popup2.h"
#include "m_flags.h"
#include "m_metacontacts.h"
#include "m_magneticwindows.h"
#include "m_toolbar.h"
#include "m_toptoolbar.h"
#include "m_userinfoex.h"

#include <m_extraicons.h>	//change this to match extraicons header location

/***********************************************************************************************************
 * UserInfoEx plugin includes and macros
 ***********************************************************************************************************/

#pragma intrinsic(memcmp, memcpy, memset, strcmp, strlen)

#ifndef MIR_OK
#define MIR_OK		0		// success value of a miranda service function
#define MIR_FAIL	1		// general failure value of a miranda service function
#endif

#define MIRSUCCEEDED(f)		((f)==MIR_OK)
#define MIRFAILED(f)		((f)!=MIR_OK)
#define MIREXISTS(f)		((int)(f)!=CALLSERVICE_NOTFOUND)

#define PtrIsValid(p)		(((p)!=0)&&(((HANDLE)(p))!=INVALID_HANDLE_VALUE))
#define FREE(p)				{if (PtrIsValid(p)){free((VOID*)p);(p)=NULL;}}
#define MIR_DELETE(p)		{LPVOID ptr = (LPVOID)(p);if (PtrIsValid(ptr)){delete(ptr);(ptr)=NULL;}}
#define MIR_FREE(p)			{if (PtrIsValid(p)){mir_free((VOID*)p);(p)=NULL;}}

#define GetUserData(p)		GetWindowLongPtr((p), GWLP_USERDATA)
#define SetUserData(p, l)	SetWindowLongPtr((p), GWLP_USERDATA, (LONG_PTR) (l))

#include "..\res\resource.h"
#include "..\iconpacks/icons.h"

#include "svc_constants.h"

#include "mir_contactqueue.h"
#include "mir_db.h"
#include "mir_string.h"
#include "mir_icolib.h"
#include "dlg_msgbox.h"
#include "classMTime.h"
#include "classMAnnivDate.h"

/***********************************************************************************************************
 * UserInfoEx global variables
 ***********************************************************************************************************/

typedef struct _MGLOBAL
{
	DWORD		mirandaVersion;					// mirandaVersion
	BOOLEAN		CanChangeDetails : 1;			// is service to upload own contact information for icq present?
	BOOLEAN		HaveCListExtraIcons : 1;		// are extra icons supported by current clist
	BOOLEAN		ExtraIconsServiceExist : 1;		// Extra Icon plugin / service exist
	BOOLEAN		MsgAddIconExist : 1;			// Messsage Window support status Icon
	BOOLEAN		TzIndexExist : 1;				// Win Reg has Timzone Index Info
	BOOLEAN		PopUpActionsExist : 1;			// Popup++ or MS_POPUP_REGISTERACTIONS exist
	BOOLEAN		ShowPropsheetColours : 1;		// cached SET_PROPSHEET_SHOWCOLOURS database value
	BOOLEAN		WantAeroAdaption : 1;			// reserved for later use
	BOOLEAN		UseDbxTree : 1;					// use dbx_tree ?
	LPCSTR		szMetaProto;

} MGLOBAL, *LPMGLOBAL;

extern HINSTANCE		ghInst;
extern MGLOBAL			myGlobals;
extern MM_INTERFACE		mmi;
extern LIST_INTERFACE	li;
extern UTF8_INTERFACE	utfi;
//extern TIME_API		tmi;
extern FI_INTERFACE		*FIP;
extern HANDLE hPrebuildMenuHook, hSettingChanged, ghChangedHook, hPreShutDown;
/***********************************************************************************************************
 * MIRANDA_CPP_PLUGIN_API
 ***********************************************************************************************************/

/**
 * These macros provide an interface for classes to use member
 * function as services and event hooks. 
 *
 * @note	This requires Miranda Core 0.8+!
 *
 **/
#define MIRANDA_CPP_PLUGIN_API(CCoreClass) \
	typedef INT (__cdecl CCoreClass::*EVENTHOOK)(WPARAM, LPARAM);	\
	typedef INT (__cdecl CCoreClass::*EVENTHOOKPARAM)(WPARAM, LPARAM, LPARAM); \
	typedef INT (__cdecl CCoreClass::*SERVICEFUNC)(WPARAM, LPARAM); \
	typedef INT (__cdecl CCoreClass::*SERVICEFUNCPARAM)(WPARAM, LPARAM, LPARAM); \
	\
	HANDLE ThisHookEvent(const char* szEvent, EVENTHOOK pfnEvent) \
	{	return (HANDLE) ::HookEventObj(szEvent, (MIRANDAHOOKOBJ) (*(PVOID*) &pfnEvent), (PVOID)this);} \
	HANDLE ThisHookEventParam(const char* szEvent, EVENTHOOKPARAM pfnEvent, LPARAM lParam) \
	{	return (HANDLE) ::HookEventObjParam(szEvent, (MIRANDAHOOKOBJPARAM) (*(PVOID*) &pfnEvent), (PVOID)this, lParam);	} \
	\
	HANDLE ThisCreateService(const char* szService, SERVICEFUNC pfnService) \
	{	return (HANDLE) ::CreateServiceFunctionObj(szService, (MIRANDASERVICEOBJ) (*(PVOID*) &pfnService), (PVOID)this);	} \
	HANDLE ThisCreateServiceParam(const char* szService, SERVICEFUNCPARAM pfnService, LPARAM lParam) \
	{	return (HANDLE) ::CreateServiceFunctionObjParam(szService, (MIRANDASERVICEOBJPARAM) (*(PVOID*) &pfnService), (PVOID)this, lParam); } \

/***********************************************************************************************************
 * UserInfoEx common used functions
 ***********************************************************************************************************/

DWORD	hashSetting(LPCSTR szStr);					//old miranda hash

unsigned int hashSetting_M2(const wchar_t * key);	//new Murma2 hash
unsigned int hashSetting_M2(const char * key);		//new Murma2 hash
unsigned int hashSettingW_M2(const char * key);		//new Murma2 hash

INT_PTR	myDestroyServiceFunction(const char * key);

static FORCEINLINE BOOL IsProtoOnline(LPSTR pszProto)
{
	return pszProto && pszProto[0] && CallProtoService(pszProto, PS_GETSTATUS, NULL, NULL) >= ID_STATUS_ONLINE;
}
static FORCEINLINE BOOL IsProtoLoaded(LPSTR pszProto)
{
	return (CallService(MS_PROTO_ISPROTOCOLLOADED, NULL, (LPARAM)pszProto) != NULL);
}
static FORCEINLINE BOOL IsProtoAccountEnabled(PROTOACCOUNT *pAcc)
{
	return ((pAcc->type == PROTOTYPE_PROTOCOL) && pAcc->bIsEnabled && IsProtoLoaded(pAcc->szModuleName));
}

typedef HRESULT (STDAPICALLTYPE *pfnDwmIsCompositionEnabled)(BOOL *);
extern pfnDwmIsCompositionEnabled dwmIsCompositionEnabled;
static FORCEINLINE BOOLEAN IsAeroMode()
{
	BOOL result;
	return myGlobals.WantAeroAdaption && dwmIsCompositionEnabled && (dwmIsCompositionEnabled(&result) == S_OK) && result;
}

