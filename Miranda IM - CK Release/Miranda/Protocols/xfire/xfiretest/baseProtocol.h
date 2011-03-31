/*
 *  Plugin of miranda IM(ICQ) for Communicating with users of the XFire Network. 
 *
 *  Copyright (C) 2008 by
 *          dufte aka andreas h. <dufte@justmail.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 *  Based on J. Lawler              - BaseProtocol
 *			 Herbert Poul/Beat Wolf - xfirelib
 *
 *  Miranda ICQ: the free icq client for MS Windows 
 *  Copyright (C) 2000-2008  Richard Hughes, Roland Rabien & Tristan Van de Vreede
 *
 */

//=====================================================
//	Includes (yea why not include lots of stuff :D )
//=====================================================
#include <windows.h>
#include <gdiplus.h>
#include <Wininet.h>
#include <commctrl.h>
#include <stdio.h>
#include <time.h>
#include <stddef.h>
#include <process.h>
#include <string.h>
#include <winsock.h>
#include "resource.h"
#include <winbase.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <string.h>

//Miranda SDK headers
#include "newpluginapi.h"
#include "m_clist.h"
#include "m_clui.h"
#include "m_skin.h"
#include "m_langpack.h"
#include "m_protomod.h"
#include "m_database.h"
#include "m_system.h"
#include "m_protocols.h"
#include "m_userinfo.h"
#include "m_options.h"
#include "m_protosvc.h"
#include "m_utils.h"
#include "m_ignore.h"
#include "m_clc.h"

//Folder
#include "m_folders.h"

//=======================================================
//	Definitions
//=======================================================
#define protocolname		"XFire" //no spaces here :)
#define DEFAULT_SERVER		"206.220.42.147"
#define DEFAULT_SERVER_PORT 25999

//=======================================================
//	Defines
//=======================================================
//General
extern HINSTANCE hinstance;
extern int bpStatus;

//Services.c
int GetCaps(WPARAM wParam,LPARAM lParam);
int GetName(WPARAM wParam,LPARAM lParam);
int TMLoadIcon(WPARAM wParam,LPARAM lParam);
int SetStatus(WPARAM wParam,LPARAM lParam);
int GetStatus(WPARAM wParam,LPARAM lParam);
BOOL IsXFireContact(HANDLE h);
std::string GetGame(int id,int id2, HANDLE* hicon, HICON* ico,BOOL onyico=FALSE);

struct CONTACT // Contains info about users
{
	TCHAR* name;
	TCHAR* user;
	TCHAR* host;
	bool ExactOnly;
	bool ExactWCOnly;
	bool ExactNick;
};


#define MSGBOX(msg)  if(DBGetContactSettingByte(NULL,protocolname,"nomsgbox",-1)==0)  mir_forkthread(Message,(LPVOID)msg);

static void Message(LPVOID msg)
{
	MSGBOXPARAMS mbp;
	mbp.cbSize=sizeof(mbp);
	mbp.hwndOwner=NULL;
	mbp.hInstance=hinstance;
	mbp.lpszText=(char*)msg;
	mbp.lpszCaption="Miranda XFire Protocol Plugin";
	mbp.dwStyle=MB_USERICON;
	mbp.lpszIcon=MAKEINTRESOURCE(IDI_TM);
	mbp.dwContextHelpId=NULL;
	mbp.lpfnMsgBoxCallback=NULL;
	mbp.dwLanguageId=LANG_ENGLISH;
	MessageBoxIndirect(&mbp);

	//MessageBoxA(0,(char*)msg,"Miranda XFire Protocol Plugin",MB_OK|MB_ICONINFORMATION);
}

/*#define  EXTRA_ICON_EMAIL  1
#define  EXTRA_ICON_PROTO  2
#define  EXTRA_ICON_SMS    3
#define  EXTRA_ICON_ADV1  4
#define  EXTRA_ICON_ADV2  9 //5
#define  EXTRA_ICON_ADV3        9
#define  EXTRA_ICON_ADV4        10 */

static int icoslot[] = { 4, 5, 9, 10 };


typedef struct
{
int cbSize;      //must be sizeof(IconExtraColumn)
int ColumnType;
HANDLE hImage;    //return value from MS_CLIST_EXTRA_ADD_ICON
}IconExtraColumn,*pIconExtraColumn;


//Set icon for contact at needed column
//wparam=hContact
//lparam=pIconExtraColumn
//return 0 on success,-1 on failure
//
//See above for supported columns
#define MS_CLIST_EXTRA_SET_ICON      "CListFrames/SetIconForExraColumn"

//Adding icon to extra image list. 
//Call this in ME_CLIST_EXTRA_LIST_REBUILD event
//
//wparam=hIcon
//lparam=0
//return hImage on success,-1 on failure
#define MS_CLIST_EXTRA_ADD_ICON      "CListFrames/AddIconToExtraImageList"



#define ME_CLIST_EXTRA_LIST_REBUILD      "CListFrames/OnExtraListRebuild"

//called with wparam=hContact
#define ME_CLIST_EXTRA_IMAGE_APPLY      "CListFrames/OnExtraImageApply"

#define FU_TBREDRAW      1 //redraw titlebar
#define FU_FMREDRAW      2 //redraw Frame
#define FU_FMPOS      4 //update Frame position
#define MS_CLIST_FRAMES_UPDATEFRAME      "CListFrame/UpdateFrame"

#define SKINICONDESC_SIZE     sizeof(SKINICONDESC)
#define SKINICONDESC_SIZE_V1  0x18
#define SKINICONDESC_SIZE_V2  0x1C
#define SKINICONDESC_SIZE_V3  0x24

typedef struct {
  int cbSize;
  union {
    char *pszSection;         // section name used to group icons
    TCHAR *ptszSection;
    wchar_t *pwszSection;
  };
  union {
    char *pszDescription;     // description for options dialog
    TCHAR *ptszDescription;
    wchar_t *pwszDescription;
  };
  char *pszName;              // name to refer to icon when playing and in db
  char *pszDefaultFile;       // default icon file to use
  int  iDefaultIndex;         // index of icon in default file
  HICON hDefaultIcon;         // handle to default icon
  int cx,cy;                  // dimensions of icon
  int flags; 
} SKINICONDESC;

#define SIDF_UNICODE  0x100   // Section and Description are in UCS-2

#if defined(_UNICODE)
  #define SIDF_TCHAR  SIDF_UNICODE
#else
  #define SIDF_TCHAR  0
#endif

//
//  Add a icon into options UI
//
//  wParam = (WPARAM)0
//  lParam = (LPARAM)(SKINICONDESC*)sid;
//
#define MS_SKIN2_ADDICON "Skin2/Icons/AddIcon"

//
//  Retrieve HICON with name specified in lParam
//  Returned HICON SHOULDN'T be destroyed, it is managed by IcoLib
//

#define MS_SKIN2_GETICON "Skin2/Icons/GetIcon"

//
//  Icons change notification
//
#define ME_SKIN2_ICONSCHANGED "Skin2/IconsChanged"

#define MS_SKIN2_GETICONBYHANDLE "Skin2/Icons/GetIconByHandle"

#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"gdiplus.lib")

//=====================================================
//	Definitions
//=====================================================

struct GameIco {
	int gameid;
	HANDLE handle;
	HICON hicon;
};

struct XFireContact {
	char * username;
	char * nick;
	int id;
	int sid;
};

struct XFireAvatar {
	int type;
	char backup[256];
	char file[256];
	char rfile[256];
};

struct XFire_FoundGame
{
	int gameid;
	int gameid2;
	char path[256];
	char launchparams[1024];
	char networkparams[1024];
};

struct XFire_SetAvatar
{
	HANDLE hContact;
	char* username;
};

#define XFIRE_MAX_STATIC_STRING_LEN 1024
#define XFIRE_SCAN_VAL 0x1

BOOL CreateToolTip(int toolID, HWND hDlg, CHAR* pText);