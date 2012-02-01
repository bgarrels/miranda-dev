/*
	BossKey - Hide Miranda IM from your boss :)
	Copyright (C) 2002-2003 Goblineye Entertainment, (C) 2007-2010 Billy_Bons

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
*/

#pragma once

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE

#define WINVER 0x0700
#define _WIN32_WINNT 0x0700
#define _WIN32_IE 0x0601
#define MIRANDA_VER 0x0900
#define MIRANDA_CUSTOM_LP

#define BOSSKEY_LISTEN_INFO _T("MY6BossKey_Param")

#include <m_stdhdr.h>
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <stdio.h>

//Miranda IM
#include <win2k.h>
#include "resource.h"
#include "version.h"
#include <newpluginapi.h>
#include <m_utils.h>
#include <m_clist.h>
#include <m_database.h>
#include <m_clui.h>
#include <m_langpack.h>
#include <m_protosvc.h>
#include <m_options.h>
#include <m_popup.h>
#include <m_awaymsg.h>
#include <m_icolib.h>
#include <m_clistint.h>
#include <m_message.h>
#include <m_hotkeys.h>

//ExternalAPI
#include "m_trigger.h"
#include "m_variables.h"
#include "m_toolbar.h"

#define MOD_NAME "BossKey"
#define KEY_DOWN(key) ((GetAsyncKeyState(key) & 0x8000) ? (true) : (false))
#define MIID_BOSSKEY  { 0xb0f05b6a, 0x8167, 0x4dc3, { 0xa1, 0x8a, 0xaf, 0xba, 0x4c, 0x77, 0xc4, 0x1d } }
#define MAXPASSLEN 16
#define	DEFAULTSETTING	( OPT_SETONLINEBACK | OPT_DISABLESNDS | OPT_CHANGESTATUS | OPT_USEDEFMSG )

#define OLD_SOUND			1
#define OLD_POPUP			2
#define OLD_FLTCONT			4
#define OLD_WASHIDDEN		128

#define OPT_SETONLINEBACK	1
#define OPT_CHANGESTATUS	2
#define OPT_DISABLESNDS		8
#define OPT_REQPASS			16
#define OPT_ONLINEONLY		32
#define OPT_USEDEFMSG		128
#define OPT_TRAYICON		256

#define OPT_HIDEIFLOCK		1
#define OPT_HIDEIFWINIDLE	2
#define OPT_HIDEIFMIRIDLE	4
#define OPT_HIDEIFSCRSVR	8
#define OPT_MENUITEM		16
#define OPT_HIDEONSTART		32
#define OPT_RESTORE			64

#define MS_BOSSKEY_HIDE				"BossKey/HideMiranda"
#define MS_BOSSKEY_ISMIRANDAHIDDEN	"BossKey/IsMirandaHidden"

#define TRIGGERNAME					"BossKey: Key pressed"
#define TRIGGER_BK_HIDE				0x01
#define TRIGGER_BK_SHOW				0x02
#define SETTING_TRIGGERON			"TriggerOn"

const unsigned STATUS_ARR_TO_ID[8] = { ID_STATUS_OFFLINE, ID_STATUS_ONLINE, ID_STATUS_AWAY, ID_STATUS_NA, ID_STATUS_OCCUPIED, ID_STATUS_DND, ID_STATUS_FREECHAT, ID_STATUS_INVISIBLE };
extern bool g_fOptionsOpen; // options dialog is open. be sure not to hide anything while we're there.
extern WORD g_wHotKey, g_wMask, g_wMaskAdv;
extern HINSTANCE g_hInstance;
extern bool g_bWindowHidden;
extern UINT minutes;

int OptsDlgInit(WPARAM wParam,LPARAM lParam);
INT_PTR BossKeyHideMiranda(WPARAM wParam, LPARAM lParam);
void BossKeyMenuItemInit(void);
void BossKeyMenuItemUnInit(void);
int BossKeyEvent(WPARAM wParam, LPARAM lParam);
int RegisterTrigger(void);
TCHAR *GetBossKeyText(void);
void InitIdleTimer(void);
void UninitIdleTimer(void);
LRESULT CALLBACK KbdProc(int nCode,WPARAM wParam,LPARAM lParam);
TCHAR* GetDefStatusMsg(unsigned uMode, const char* szProto);

// I never really liked STL's vector... hmm this is nicer anyway, more flexible
typedef struct HWND_ITEM_TYPE
{
	HWND hWnd;
	HWND_ITEM_TYPE *next;
}	HWND_ITEM;

typedef BOOL (WINAPI *PWTSRegisterSessionNotification)(HWND hWnd, DWORD dwFlags);
typedef BOOL (WINAPI *PWTSUnRegisterSessionNotification)(HWND hWnd);
typedef HRESULT (WINAPI *PFNDwmIsCompositionEnabled)(BOOL *);
