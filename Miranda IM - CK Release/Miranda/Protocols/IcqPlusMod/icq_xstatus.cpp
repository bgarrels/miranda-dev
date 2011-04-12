// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin Öberg, Sam Kothari, Robert Rainwater
// Copyright © 2004,2005,2006,2007 Joe Kucera
// Copyright © 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// -----------------------------------------------------------------------------
//
// File name      : $Source$
// Revision       : $Revision: 64 $
// Last change on : $Date: 2007-10-16 17:39:11 +0300 (Ð’Ñ‚, 16 Ð¾ÐºÑ‚ 2007) $
// Last change by : $Author: chaos.persei $
//
// DESCRIPTION:
//
//  Support for Custom Statuses
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"
#include "m_cluiframes.h"
#include "m_folders.h"

#ifdef _WIN64
#define GWL_WNDPROC         (-4)
#define GWL_HINSTANCE       (-6)
#define GWL_HWNDPARENT      (-8)
#define GWL_STYLE           (-16)
#define GWL_EXSTYLE         (-20)
#define GWL_USERDATA        (-21)
#define GWL_ID              (-12)
#endif


extern void setUserInfo();

extern BOOL bXstatusIconShow;
extern HANDLE hxstatusiconchanged;
extern HANDLE hxstatuschanged;
extern HANDLE hExtraXStatus;
extern HANDLE hIconFolder;

int bHideXStatusUI = 0;
int bHideXStatusMenu = 0;
int bStatusMenu = 0;
HANDLE hHookExtraIconsRebuild = NULL;
HANDLE hHookStatusBuild = NULL;
HANDLE hHookExtraIconsApply = NULL;
HANDLE hXStatusIcons[37];
HANDLE hXStatusItems[38];
HANDLE hXStatusIconsHandle[37];
int hXStatusCListIcons[37];				//
BOOL bXStatusCListIconsValid[37];		//need to be changed dinamicly

void CListShowMenuItem(HANDLE hMenuItem, BYTE bShow);
DWORD dwLastXUpdateTime;

BYTE fXstatusIcons()
{
    if(getSettingByte(NULL, "NonStandartXstatus", 1))
        return 37;
    else
        return 32;
}

BYTE fXstatusItems()
{
    if(getSettingByte(NULL, "NonStandartXstatus", 1))
        return 38;
    else
        return 33;
}

#define XstatusIcons fXstatusIcons()
#define XstatusItems fXstatusItems()


BYTE __stdcall ICQGetContactXStatus(HANDLE hContact)
{
    BYTE bXStatus = getSettingByte(hContact, DBSETTING_XSTATUSID, 0);

    if (!gbXStatusEnabled || bXStatus < 1 || bXStatus > XstatusIcons) return 0;

    return bXStatus;
}


DWORD sendXStatusDetailsRequest(HANDLE hContact, int bForced)
{
    if (invis_for(0,hContact))
        return 0;
    {
        DWORD dwCookie = 0;
        if (getSettingByte(hContact, DBSETTING_XSTATUSID, -1) != -1)
        {
            // only request custom status detail when the contact has one
            char *szNotify;
            int nNotifyLen;
            nNotifyLen = 94 + UINMAXLEN;
            szNotify = (char*)icq_alloc_zero(nNotifyLen);
            nNotifyLen = null_snprintf(szNotify, nNotifyLen, "<srv><id>cAwaySrv</id><req><id>AwayStat</id><trans>1</trans><senderId>%d</senderId></req></srv>", dwLocalUIN);
            dwCookie = SendXtrazNotifyRequest(hContact, "<Q><PluginID>srvMng</PluginID></Q>", szNotify, bForced);
        }
        return dwCookie;
    }
}



static DWORD requestXStatusDetails(HANDLE hContact, BOOL bAllowDelay)
{
    rate_record rr = {0};

    if (!validateStatusMessageRequest(hContact, MTYPE_SCRIPT_NOTIFY))
        return 0; // apply privacy rules

    // delay is disabled only if fired from dialog
    if (!CheckContactCapabilities(hContact, CAPF_XTRAZ) && bAllowDelay)
        return 0; // Contact does not support xtraz, do not request details
    rr.hContact = hContact;
    rr.bType = RIT_XSTATUS_REQUEST;
    rr.nRequestType = 0x101; // request
    rr.nMinDelay = 1000;    // delay at least 1s
    EnterCriticalSection(&ratesMutex);
    rr.wGroup = ratesGroupFromSNAC(gRates, ICQ_MSG_FAMILY, ICQ_MSG_SRV_SEND);
    LeaveCriticalSection(&ratesMutex);

    if (!handleRateItem(&rr, bAllowDelay))
        return sendXStatusDetailsRequest(hContact, !bAllowDelay);

    return -1; // delayed
}

void UpdateXStatuses()
{
    HANDLE hContact;
    extern BOOL bXUpdaterPopUp;

    if(!icqOnline)
        return;
    if(!gbXStatusEnabled)
        return;

    hContact = FindFirstContact();
    if(hContact)
    {
        do
        {
            if (getContactStatus(hContact) != ID_STATUS_OFFLINE && (CheckContactCapabilities(hContact, CAPF_XTRAZ) && !invis_for(getContactUin(hContact), hContact) && getSettingByte(hContact, DBSETTING_XSTATUSID, 0) != 0))
                requestXStatusDetails(hContact, 1);
        }
        while(hContact = FindNextContact(hContact));
    }
    if(bXUpdaterPopUp)
    {
        static char title[32];
        strcpy(title, ICQ_PROTOCOL_NAME);
        strcat(title, " XStatus Updater");
        ShowPopUpMsg(0,0,title, "Requests sent to all contacts", LOG_NOTE);
    }
}

unsigned __stdcall icq_XStatusUpdaterThread(LPVOID lp)
{
    dwLastXUpdateTime = 0;
    while(1)
    {
        Sleep(60000);
        if(time(NULL) - dwLastXUpdateTime < DBGetContactSettingDword(0, ICQ_PROTOCOL_NAME, "XStatusUpdatePeriod", 5) * 60)
            continue;
        if(!DBGetContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "UpdateXStatus", 1))
            continue;

        UpdateXStatuses();
        dwLastXUpdateTime = time(NULL);
    }
    return 0;
}

static HANDLE LoadXStatusIconLibrary(char* path, const char* sub)
{
    char *p = path+strlen(path);
    HANDLE hLib;

    strcpy(p, sub);
    strcat(p, "\\xStatus_icons.dll");
    if (hLib = LoadLibraryA(path)) return hLib;
    strcpy(p, sub);
    strcat(p, "\\xStatus_ICQ.dll");
    if (hLib = LoadLibraryA(path)) return hLib;

    return hLib;
}

static char* InitXStatusIconLibrary(char* buf)
{
    char path[2*MAX_PATH];
    char *p;
    HMODULE hIconsDLL=NULL;

    if( hIconFolder )
    {
        FoldersGetCustomPath(hIconFolder, path, MAX_PATH, "icons");
        if (!hIconsDLL)
            hIconsDLL = (HMODULE)LoadXStatusIconLibrary(path, "");
    }
    if (!hIconsDLL)
    {
        // get miranda's exe path
        GetModuleFileNameA(NULL, path, MAX_PATH);
        p = strrchr(path, '\\');
        if(p) *p='\0';

        if (!hIconsDLL)
            hIconsDLL = (HMODULE)LoadXStatusIconLibrary(path, "\\Icons");

        if (!hIconsDLL)
            hIconsDLL = (HMODULE)LoadXStatusIconLibrary(path, "\\Plugins");

    }
    if (hIconsDLL)
    {
        FreeLibrary(hIconsDLL);
        strcpy(buf, path);
    }

    return buf;
}

HICON GetXStatusIcon(int bStatus, UINT flags)
{
    char szTemp[64];
    HICON icon;

    null_snprintf(szTemp, sizeof(szTemp), "xstatus%d", bStatus - 1);
    icon = IconLibGetIcon(szTemp);

    if (flags & LR_SHARED)
        return icon;
    else
        return CopyIcon(icon);
}

static int CListMW_ExtraIconsRebuild(WPARAM wParam, LPARAM lParam) ;

static void setContactExtraIcon(HANDLE hContact, int xstatus)
{
    HANDLE hIcon = INVALID_HANDLE_VALUE;
    if(!hExtraXStatus)
    {
        WORD icon_pos = getSettingWord(NULL, "xstatus_icon_pos", 9);
        hIcon = (xstatus <= 0 ? (HANDLE)-1 : hXStatusIcons[xstatus-1]);
        if (xstatus > 0)
            CListMW_ExtraIconsRebuild(0, 0);
        if(icon_pos<=0||icon_pos>9)
        {
            icon_pos=9;
        }
        if (bXstatusIconShow)
        {
            IconExtraColumn iec;
            iec.cbSize = sizeof(iec);
            iec.hImage = hIcon;
            iec.ColumnType = icon_pos;
            CallService(MS_CLIST_EXTRA_SET_ICON, (WPARAM)hContact, (LPARAM)&iec);
            NotifyEventHooks(hxstatusiconchanged, (WPARAM)hContact, (LPARAM)hIcon);
        }
    }
    else
    {
        EXTRAICON ico;
        char szTemp[MAX_PATH];
        ico.cbSize=sizeof(ico);
        hIcon = INVALID_HANDLE_VALUE;
        if(xstatus <= 0)
            ico.icoName=(char *)0;
        else
        {
            null_snprintf(szTemp, sizeof(szTemp), "%s_xstatus%d", ICQ_PROTOCOL_NAME, xstatus-1);
            ico.icoName=szTemp;
        }
        ico.hContact=hContact;
        ico.hExtraIcon=hExtraXStatus;
        CallService(MS_EXTRAICON_SET_ICON, (WPARAM)&ico, 0);
    }
}

static int CListMW_ExtraIconsRebuild(WPARAM wParam, LPARAM lParam)
{
    BYTE i;

    if (gbXStatusEnabled && ServiceExists(MS_CLIST_EXTRA_ADD_ICON))
    {
        for (i = 0; i < XstatusIcons; i++)
        {
            hXStatusIcons[i] = (HANDLE)CallService(MS_CLIST_EXTRA_ADD_ICON, (WPARAM)GetXStatusIcon(i + 1, LR_SHARED), 0);
        }
    }

    return 0;
}

static int CListMW_ExtraIconsApply(WPARAM wParam, LPARAM lParam)
{
    if (gbXStatusEnabled && ServiceExists(MS_CLIST_EXTRA_SET_ICON))
    {
        if (IsICQContact((HANDLE)wParam))
        {
            // only apply icons to our contacts, do not mess others
            DWORD bXStatus = ICQGetContactXStatus((HANDLE)wParam);


            setContactExtraIcon((HANDLE)wParam, bXStatus);
        }
    }
    return 0;
}



static int CListMW_BuildStatusItems(WPARAM wParam, LPARAM lParam)
{
    InitXStatusItems(TRUE);
    return 0;
}



void InitXStatusEvents()
{
    if (!hHookStatusBuild)
        if (bStatusMenu = ServiceExists(MS_CLIST_ADDSTATUSMENUITEM))
            hHookStatusBuild = HookEvent(ME_CLIST_PREBUILDSTATUSMENU, CListMW_BuildStatusItems);

    if (!hHookExtraIconsRebuild)
        hHookExtraIconsRebuild = HookEvent(ME_CLIST_EXTRA_LIST_REBUILD, CListMW_ExtraIconsRebuild);

    if (!hHookExtraIconsApply)
        hHookExtraIconsApply = HookEvent(ME_CLIST_EXTRA_IMAGE_APPLY, CListMW_ExtraIconsApply);
}



void UninitXStatusEvents()
{
    if (hHookStatusBuild)
        UnhookEvent(hHookStatusBuild);

    if (hHookExtraIconsRebuild)
        UnhookEvent(hHookExtraIconsRebuild);

    if (hHookExtraIconsApply)
        UnhookEvent(hHookExtraIconsApply);
}



const capstr capXStatus[37] =
{
    {0x01, 0xD8, 0xD7, 0xEE, 0xAC, 0x3B, 0x49, 0x2A, 0xA5, 0x8D, 0xD3, 0xD8, 0x77, 0xE6, 0x6B, 0x92},// XStatus(Angry)
    {0x5A, 0x58, 0x1E, 0xA1, 0xE5, 0x80, 0x43, 0x0C, 0xA0, 0x6F, 0x61, 0x22, 0x98, 0xB7, 0xE4, 0xC7},// XStatus(Duck)
    {0x83, 0xC9, 0xB7, 0x8E, 0x77, 0xE7, 0x43, 0x78, 0xB2, 0xC5, 0xFB, 0x6C, 0xFC, 0xC3, 0x5B, 0xEC},// XStatus(Tired)
    {0xE6, 0x01, 0xE4, 0x1C, 0x33, 0x73, 0x4B, 0xD1, 0xBC, 0x06, 0x81, 0x1D, 0x6C, 0x32, 0x3D, 0x81},// XStatus(Party)
    {0x8C, 0x50, 0xDB, 0xAE, 0x81, 0xED, 0x47, 0x86, 0xAC, 0xCA, 0x16, 0xCC, 0x32, 0x13, 0xC7, 0xB7},// XStatus(Beer)
    {0x3F, 0xB0, 0xBD, 0x36, 0xAF, 0x3B, 0x4A, 0x60, 0x9E, 0xEF, 0xCF, 0x19, 0x0F, 0x6A, 0x5A, 0x7F},// XStatus(Thinking)
    {0xF8, 0xE8, 0xD7, 0xB2, 0x82, 0xC4, 0x41, 0x42, 0x90, 0xF8, 0x10, 0xC6, 0xCE, 0x0A, 0x89, 0xA6},// XStatus(Eating)
    {0x80, 0x53, 0x7D, 0xE2, 0xA4, 0x67, 0x4A, 0x76, 0xB3, 0x54, 0x6D, 0xFD, 0x07, 0x5F, 0x5E, 0xC6},// XStatus(TV)
    {0xF1, 0x8A, 0xB5, 0x2E, 0xDC, 0x57, 0x49, 0x1D, 0x99, 0xDC, 0x64, 0x44, 0x50, 0x24, 0x57, 0xAF},// XStatus(Friends)
    {0x1B, 0x78, 0xAE, 0x31, 0xFA, 0x0B, 0x4D, 0x38, 0x93, 0xD1, 0x99, 0x7E, 0xEE, 0xAF, 0xB2, 0x18},// XStatus(Coffee)
    {0x61, 0xBE, 0xE0, 0xDD, 0x8B, 0xDD, 0x47, 0x5D, 0x8D, 0xEE, 0x5F, 0x4B, 0xAA, 0xCF, 0x19, 0xA7},// XStatus(Music)
    {0x48, 0x8E, 0x14, 0x89, 0x8A, 0xCA, 0x4A, 0x08, 0x82, 0xAA, 0x77, 0xCE, 0x7A, 0x16, 0x52, 0x08},// XStatus(Business)
    {0x10, 0x7A, 0x9A, 0x18, 0x12, 0x32, 0x4D, 0xA4, 0xB6, 0xCD, 0x08, 0x79, 0xDB, 0x78, 0x0F, 0x09},// XStatus(Camera)
    {0x6F, 0x49, 0x30, 0x98, 0x4F, 0x7C, 0x4A, 0xFF, 0xA2, 0x76, 0x34, 0xA0, 0x3B, 0xCE, 0xAE, 0xA7},// XStatus(Funny)
    {0x12, 0x92, 0xE5, 0x50, 0x1B, 0x64, 0x4F, 0x66, 0xB2, 0x06, 0xB2, 0x9A, 0xF3, 0x78, 0xE4, 0x8D},// XStatus(Phone)
    {0xD4, 0xA6, 0x11, 0xD0, 0x8F, 0x01, 0x4E, 0xC0, 0x92, 0x23, 0xC5, 0xB6, 0xBE, 0xC6, 0xCC, 0xF0},// XStatus(Games)
    {0x60, 0x9D, 0x52, 0xF8, 0xA2, 0x9A, 0x49, 0xA6, 0xB2, 0xA0, 0x25, 0x24, 0xC5, 0xE9, 0xD2, 0x60},// XStatus(College)
    {0x63, 0x62, 0x73, 0x37, 0xA0, 0x3F, 0x49, 0xFF, 0x80, 0xE5, 0xF7, 0x09, 0xCD, 0xE0, 0xA4, 0xEE},// XStatus(Shopping)
    {0x1F, 0x7A, 0x40, 0x71, 0xBF, 0x3B, 0x4E, 0x60, 0xBC, 0x32, 0x4C, 0x57, 0x87, 0xB0, 0x4C, 0xF1},// XStatus(Sick)
    {0x78, 0x5E, 0x8C, 0x48, 0x40, 0xD3, 0x4C, 0x65, 0x88, 0x6F, 0x04, 0xCF, 0x3F, 0x3F, 0x43, 0xDF},// XStatus(Sleeping)
    {0xA6, 0xED, 0x55, 0x7E, 0x6B, 0xF7, 0x44, 0xD4, 0xA5, 0xD4, 0xD2, 0xE7, 0xD9, 0x5C, 0xE8, 0x1F},// XStatus(Surfing)
    {0x12, 0xD0, 0x7E, 0x3E, 0xF8, 0x85, 0x48, 0x9E, 0x8E, 0x97, 0xA7, 0x2A, 0x65, 0x51, 0xE5, 0x8D},// XStatus(@)
    {0xBA, 0x74, 0xDB, 0x3E, 0x9E, 0x24, 0x43, 0x4B, 0x87, 0xB6, 0x2F, 0x6B, 0x8D, 0xFE, 0xE5, 0x0F},// XStatus(Engineering)
    {0x63, 0x4F, 0x6B, 0xD8, 0xAD, 0xD2, 0x4A, 0xA1, 0xAA, 0xB9, 0x11, 0x5B, 0xC2, 0x6D, 0x05, 0xA1},// XStatus(Typing)
    //Netvigator (China)
    {0x2C, 0xE0, 0xE4, 0xE5, 0x7C, 0x64, 0x43, 0x70, 0x9C, 0x3A, 0x7A, 0x1C, 0xE8, 0x78, 0xA7, 0xDC},// XStatus(China1)
    {0x10, 0x11, 0x17, 0xC9, 0xA3, 0xB0, 0x40, 0xF9, 0x81, 0xAC, 0x49, 0xE1, 0x59, 0xFB, 0xD5, 0xD4},// XStatus(China2)
    {0x16, 0x0C, 0x60, 0xBB, 0xDD, 0x44, 0x43, 0xF3, 0x91, 0x40, 0x05, 0x0F, 0x00, 0xE6, 0xC0, 0x09},// XStatus(China3)
    {0x64, 0x43, 0xC6, 0xAF, 0x22, 0x60, 0x45, 0x17, 0xB5, 0x8C, 0xD7, 0xDF, 0x8E, 0x29, 0x03, 0x52},// XStatus(China4)
    {0x16, 0xF5, 0xB7, 0x6F, 0xA9, 0xD2, 0x40, 0x35, 0x8C, 0xC5, 0xC0, 0x84, 0x70, 0x3C, 0x98, 0xFA},// XStatus(China5)
    //ProSieben (De)
    {0x63, 0x14, 0x36, 0xff, 0x3f, 0x8a, 0x40, 0xd0, 0xa5, 0xcb, 0x7b, 0x66, 0xe0, 0x51, 0xb3, 0x64},// XStatus(De1)
    {0xb7, 0x08, 0x67, 0xf5, 0x38, 0x25, 0x43, 0x27, 0xa1, 0xff, 0xcf, 0x4c, 0xc1, 0x93, 0x97, 0x97},// XStatus(De2)
    {0xdd, 0xcf, 0x0e, 0xa9, 0x71, 0x95, 0x40, 0x48, 0xa9, 0xc6, 0x41, 0x32, 0x06, 0xd6, 0xf2, 0x80},// XStatus(De3)
    //Rambler (Ru)                                                                                   // by BeteTest
    {0xD4, 0xE2, 0xB0, 0xBA, 0x33, 0x4E, 0x4F, 0xA5, 0x98, 0xD0, 0x11, 0x7D, 0xBF, 0x4D, 0x3C, 0xC8},// XStatus(Ru1)
    {0xCD, 0x56, 0x43, 0xA2, 0xC9, 0x4C, 0x47, 0x24, 0xB5, 0x2C, 0xDC, 0x01, 0x24, 0xA1, 0xD0, 0xCD},// XStatus(Ru2)
    {0x00, 0x72, 0xD9, 0x08, 0x4A, 0xD1, 0x43, 0xDD, 0x91, 0x99, 0x6F, 0x02, 0x69, 0x66, 0x02, 0x6F},// XStatus(Ru3)
    //RnQ
    {0xE6, 0x01, 0xE4, 0x1C, 0x33, 0x73, 0x4B, 0xD1, 0xBC, 0x06, 0x81, 0x1D, 0x6C, 0x32, 0x3D, 0x82},// XStatus(Sex)
    {0x3F, 0xB0, 0xBD, 0x36, 0xAF, 0x3B, 0x4A, 0x60, 0x9E, 0xEF, 0xCF, 0x19, 0x0F, 0x6A, 0x5A, 0x7E}
};//XStatus(Smoking)
const char* nameXStatus[37] =
{
    LPGEN("Angry"),         // 23
    LPGEN("Taking a bath"), // 1
    LPGEN("Tired"),         // 2
    LPGEN("Party"),         // 3
    LPGEN("Drinking beer"), // 4
    LPGEN("Thinking"),      // 5
    LPGEN("Eating"),        // 6
    LPGEN("Watching TV"),   // 7
    LPGEN("Meeting"),       // 8
    LPGEN("Coffee"),        // 9
    LPGEN("Listening to music"),// 10
    LPGEN("Business"),      // 11
    LPGEN("Shooting"),      // 12
    LPGEN("Having fun"),    // 13
    LPGEN("On the phone"),  // 14
    LPGEN("Gaming"),        // 15
    LPGEN("Studying"),      // 16
    LPGEN("Shopping"),      // 0
    LPGEN("Feeling sick"),  // 17
    LPGEN("Sleeping"),      // 18
    LPGEN("Surfing"),       // 19
    LPGEN("Browsing"),      // 20
    LPGEN("Working"),       // 21
    LPGEN("Typing"),        // 22
    //Netvigator (China)
    LPGEN("Picnic"),
    LPGEN("Cooking"),
    LPGEN("Mobile"),
    LPGEN("I'm high"),
    LPGEN("On WC"),
    //ProSieben (De)
    LPGEN("To be or not to be"),
    LPGEN("Watching pro7 on TV"),
    LPGEN("Love"),
    //Rambler (Ru)
    LPGEN("Searching"),
    LPGEN("Love"),
    LPGEN("Journal"),
    //RnQ
    LPGEN("Sex"),
    LPGEN("Smoking")
};

const int moodXStatus[] =
{
    23,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    0,
    17,
    18,
    19,
    20,
    21,
    22,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1
};


void handleXStatusCaps(HANDLE hContact, char* caps, int capsize, char* moods, int moodsize)
{
    int bChanged = FALSE;
    int xstatus = 0;

    if (!gbXStatusEnabled)
        return;

    if (caps)
    {
        // detect custom status capabilities
        BYTE i;

        for (i = 0; i<XstatusIcons; i++)
        {
            if (MatchCap((BYTE*)caps, capsize, (const capstr*)capXStatus[i], 0x10))
            {
                BYTE bXStatusId = (BYTE)(i+1);
                char str[MAX_PATH];

                if (ICQGetContactXStatus(hContact) != bXStatusId)
                {
                    // only write default name when it is really needed, i.e. on Custom Status change
                    setSettingByte(hContact, DBSETTING_XSTATUSID, bXStatusId);
                    setSettingStringUtf(hContact, DBSETTING_XSTATUSNAME, ICQTranslateUtfStatic(nameXStatus[i], str, MAX_PATH));
                    deleteSetting(hContact, DBSETTING_XSTATUSMSG);

                    bChanged = TRUE;
                }

                if (getSettingByte(NULL, "XStatusAuto", DEFAULT_XSTATUS_AUTO))
                    requestXStatusDetails(hContact, TRUE);

                xstatus = bXStatusId;

                break;
            }
        }
    }
    if (!xstatus && moods && moodsize < 32)
    {
        // process custom statuses (moods) from ICQ6
        int i;

        for (i = 0; i < XstatusIcons; i++)
        {
            char szMoodId[32], szMoodData[32];

            strncpy(szMoodData, moods, moodsize);
            szMoodData[moodsize] = '\0';

            if (moodXStatus[i] == -1) continue;
            null_snprintf(szMoodId, 32, "icqmood%d", moodXStatus[i]);
            if (!strcmpnull(szMoodId, szMoodData))
            {
                BYTE bXStatusId = (BYTE)(i+1);
                char str[MAX_PATH];

                if (ICQGetContactXStatus(hContact) != bXStatusId)
                {
                    // only write default name when it is really needed, i.e. on Custom Status change
                    setSettingByte(hContact, DBSETTING_XSTATUSID, bXStatusId);
                    setSettingStringUtf(hContact, DBSETTING_XSTATUSNAME, ICQTranslateUtfStatic(nameXStatus[i], str, MAX_PATH));
                    deleteSetting(hContact, DBSETTING_XSTATUSMSG);

                    bChanged = TRUE;
                }
                // cannot retrieve mood details here - need to be processed with new user details
                xstatus = bXStatusId;

                break;
            }
        }
    }
    if (!xstatus)
    {
        if (getSettingByte(hContact, DBSETTING_XSTATUSID, -1) != -1)
            bChanged = TRUE;
        deleteSetting(hContact, DBSETTING_XSTATUSID);
        deleteSetting(hContact, DBSETTING_XSTATUSNAME);
        deleteSetting(hContact, DBSETTING_XSTATUSMSG);
    }

    if (gbXStatusEnabled != 10)
    {
        setContactExtraIcon(hContact, xstatus);

        if (bChanged)
            NotifyEventHooks(hxstatuschanged, (WPARAM)hContact, 0);
    }
}


static void updateServerCustomStatus()
{
    setUserInfo();

    icq_setstatus(MirandaStatusToIcq(gnCurrentStatus), TRUE);
    // Tell whos on our invisible/visible list
    icq_sendEntireVisInvisList(gnCurrentStatus == ID_STATUS_INVISIBLE ? 0 : 1);
}

static WNDPROC OldMessageEditProc;

static LRESULT CALLBACK MessageEditSubclassProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch(msg)
    {
    case WM_CHAR:
        if(wParam=='\n' && GetKeyState(VK_CONTROL)&0x8000)
        {
            PostMessage(GetParent(hwnd),WM_COMMAND,IDOK,0);
            return 0;
        }
        if (wParam == 1 && GetKeyState(VK_CONTROL) & 0x8000)
        {
            // ctrl-a
            SendMessage(hwnd, EM_SETSEL, 0, -1);
            return 0;
        }
        if (wParam == 23 && GetKeyState(VK_CONTROL) & 0x8000)
        {
            // ctrl-w
            SendMessage(GetParent(hwnd), WM_CLOSE, 0, 0);
            return 0;
        }
        if (wParam == 127 && GetKeyState(VK_CONTROL) & 0x8000)
        {
            // ctrl-backspace
            DWORD start, end;
            wchar_t *text;

            SendMessage(hwnd, EM_GETSEL, (WPARAM) & end, (LPARAM) (PDWORD) NULL);
            SendMessage(hwnd, WM_KEYDOWN, VK_LEFT, 0);
            SendMessage(hwnd, EM_GETSEL, (WPARAM) & start, (LPARAM) (PDWORD) NULL);
            text = GetWindowTextUcs(hwnd);
            MoveMemory(text + start, text + end, sizeof(wchar_t) * (wcslen(text) + 1 - end));
            SetWindowTextUcs(hwnd, text);
            mir_free(text);
            SendMessage(hwnd, EM_SETSEL, start, start);
            SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hwnd), EN_CHANGE), (LPARAM) hwnd);
            return 0;
        }
        break;
    }
    return CallWindowProcUtf(OldMessageEditProc,hwnd,msg,wParam,lParam);
}


typedef struct SetXStatusData_s
{
    BYTE bAction;
    BYTE bXStatus;
    HANDLE hContact;
    HANDLE hEvent;
    DWORD iEvent;
    int countdown;
    char *okButtonFormat;
    HICON hDlgIcon;
} SetXStatusData;

typedef struct InitXStatusData_s
{
    BYTE bAction;
    BYTE bXStatus;
    char *szXStatusName;
    char *szXStatusMsg;
    HANDLE hContact;
} InitXStatusData;

#define HM_PROTOACK (WM_USER+10)
static INT_PTR CALLBACK SetXStatusDlgProc(HWND hwndDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    SetXStatusData *dat = (SetXStatusData*)GetWindowLong(hwndDlg,GWL_USERDATA);
    char str[MAX_PATH];

    switch(message)
    {
    case HM_PROTOACK:
    {
        ACKDATA *ack = (ACKDATA*)lParam;
        char *szText;

        if (ack->type != ICQACKTYPE_XSTATUS_RESPONSE) break;
        if (ack->hContact != dat->hContact) break;
        if ((DWORD)ack->hProcess != dat->iEvent) break;

        ShowWindow(GetDlgItem(hwndDlg, IDC_RETRXSTATUS), SW_HIDE);
        ShowWindow(GetDlgItem(hwndDlg, IDC_XMSG), SW_SHOW);
        ShowWindow(GetDlgItem(hwndDlg, IDC_XTITLE), SW_SHOW);
        ShowWindow(GetDlgItem(hwndDlg, IDCANCEL), SW_HIDE);
        SetDlgItemTextUtf(hwndDlg,IDOK,ICQTranslateUtfStatic("Close", str, MAX_PATH));
        UnhookEvent(dat->hEvent);
        dat->hEvent = NULL;
        szText = getSettingStringUtf(dat->hContact, DBSETTING_XSTATUSNAME, "");
        SetDlgItemTextUtf(hwndDlg, IDC_XTITLE, szText);
        mir_free(szText);
        szText = getSettingStringUtf(dat->hContact, DBSETTING_XSTATUSMSG, "");
        SetDlgItemTextUtf(hwndDlg, IDC_XMSG, szText);
        mir_free(szText);

        break;
    }
    case WM_INITDIALOG:
    {
        InitXStatusData *init = (InitXStatusData*)lParam;

        ICQTranslateDialog(hwndDlg);
        dat = (SetXStatusData*)icq_alloc_zero(sizeof(SetXStatusData));
        SetWindowLong(hwndDlg,GWL_USERDATA,(LONG)dat);
        dat->bAction = init->bAction;

        if (!init->bAction)
        {
            // set our xStatus
            dat->bXStatus = init->bXStatus;
            SendDlgItemMessage(hwndDlg, IDC_XTITLE, EM_LIMITTEXT, 256, 0);
            SendDlgItemMessage(hwndDlg, IDC_XMSG, EM_LIMITTEXT, 1024, 0);
            OldMessageEditProc = (WNDPROC)SetWindowLongUtf(GetDlgItem(hwndDlg,IDC_XTITLE),GWL_WNDPROC,(LONG)MessageEditSubclassProc);
            OldMessageEditProc = (WNDPROC)SetWindowLongUtf(GetDlgItem(hwndDlg,IDC_XMSG),GWL_WNDPROC,(LONG)MessageEditSubclassProc);
            dat->okButtonFormat = GetDlgItemTextUtf(hwndDlg,IDOK);

            SetDlgItemTextUtf(hwndDlg, IDC_XTITLE, init->szXStatusName);
            SetDlgItemTextUtf(hwndDlg, IDC_XMSG, init->szXStatusMsg);

            dat->countdown=5;
            SendMessage(hwndDlg, WM_TIMER, 0, 0);
            SetTimer(hwndDlg,1,1000,0);
        }
        else
        {
            // retrieve contact's xStatus
            dat->hContact = init->hContact;
            dat->bXStatus = ICQGetContactXStatus(dat->hContact);
            dat->okButtonFormat = NULL;
            ShowWindow(GetDlgItem(hwndDlg, IDCANCEL), SW_HIDE);
            SendMessage(GetDlgItem(hwndDlg, IDC_XTITLE), EM_SETREADONLY, 1, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_XMSG), EM_SETREADONLY, 1, 0);
            if (!getSettingByte(NULL, "XStatusAuto", DEFAULT_XSTATUS_AUTO))
            {
                SetDlgItemTextUtf(hwndDlg,IDOK,ICQTranslateUtfStatic("Cancel", str, MAX_PATH));
                dat->hEvent = HookEventMessage(ME_PROTO_ACK, hwndDlg, HM_PROTOACK);
                ShowWindow(GetDlgItem(hwndDlg, IDC_RETRXSTATUS), SW_SHOW);
                ShowWindow(GetDlgItem(hwndDlg, IDC_XMSG), SW_HIDE);
                ShowWindow(GetDlgItem(hwndDlg, IDC_XTITLE), SW_HIDE);
                dat->iEvent = requestXStatusDetails(dat->hContact, FALSE);
            }
            else
            {
                char *szText;

                SetDlgItemTextUtf(hwndDlg,IDOK,ICQTranslateUtfStatic("Close", str, MAX_PATH));
                ShowWindow(GetDlgItem(hwndDlg, IDCANCEL), SW_HIDE);
                dat->hEvent = NULL;
                szText = getSettingStringUtf(dat->hContact, DBSETTING_XSTATUSNAME, "");
                SetDlgItemTextUtf(hwndDlg, IDC_XTITLE, szText);
                mir_free(szText);
                szText = getSettingStringUtf(dat->hContact, DBSETTING_XSTATUSMSG, "");
                SetDlgItemTextUtf(hwndDlg, IDC_XMSG, szText);
                mir_free(szText);
            }
        }

        if (dat->bXStatus)
        {
            SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)GetXStatusIcon(dat->bXStatus, LR_SHARED));
        }

        {
            char *format;
            char buf[MAX_PATH];

            format = GetWindowTextUtf(hwndDlg);
            null_snprintf(str, sizeof(str), format, dat->bXStatus?ICQTranslateUtfStatic(nameXStatus[dat->bXStatus-1], buf, MAX_PATH):"");
            SetWindowTextUtf(hwndDlg, str);
            mir_free(format);
        }
        return TRUE;
    }
    case WM_TIMER:
        if(dat->countdown==-1)
        {
            DestroyWindow(hwndDlg);
            break;
        }
        {
            null_snprintf(str,sizeof(str),dat->okButtonFormat,dat->countdown);
            SetDlgItemTextUtf(hwndDlg,IDOK,str);
        }
        dat->countdown--;
        break;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            DestroyWindow(hwndDlg);
            break;
        case IDCANCEL:
            EndDialog(hwndDlg, 0);
            KillTimer(hwndDlg, 1);
            break;
        case IDC_XTITLE:
        case IDC_XMSG:
            if (!dat->bAction)
            {
                // set our xStatus
                KillTimer(hwndDlg,1);
                SetDlgItemTextUtf(hwndDlg,IDOK,ICQTranslateUtfStatic("OK", str, MAX_PATH));
            }
            break;
        }
        break;

    case WM_DESTROY:
        if (!dat->bAction)
        {
            // set our xStatus
            char szSetting[64];
            char* szValue;
            char *ansi = NULL;
            CLISTMENUITEM mi = {0};

            setSettingByte(NULL, DBSETTING_XSTATUSID, dat->bXStatus);
            szValue = GetDlgItemTextUtf(hwndDlg,IDC_XMSG);
            sprintf(szSetting, "XStatus%dMsg", dat->bXStatus);
            setSettingStringUtf(NULL, szSetting, szValue);
            setSettingStringUtf(NULL, DBSETTING_XSTATUSMSG, szValue);
            mir_free(szValue);
            szValue = GetDlgItemTextUtf(hwndDlg,IDC_XTITLE);
            sprintf(szSetting, "XStatus%dName", dat->bXStatus);
            setSettingStringUtf(NULL, szSetting, szValue);
            setSettingStringUtf(NULL, DBSETTING_XSTATUSNAME, szValue);
            mi.cbSize = sizeof(mi);
            ansi = mir_utf8decodeA(szValue);
            mi.pszName = ((strlen(ansi) > 0) && DBGetContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "ShowMyXText", 1)) ? ansi : ICQTranslate(nameXStatus[dat->bXStatus-1]);
            mi.flags = CMIM_NAME;
            CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hXStatusItems[dat->bXStatus], (LPARAM)&mi);
            mir_free(szValue);
            mir_free(ansi);


            updateServerCustomStatus();

            SetWindowLongUtf(GetDlgItem(hwndDlg,IDC_XMSG),GWL_WNDPROC,(LONG)OldMessageEditProc);
            SetWindowLongUtf(GetDlgItem(hwndDlg,IDC_XTITLE),GWL_WNDPROC,(LONG)OldMessageEditProc);
        }
        if (dat->hEvent) UnhookEvent(dat->hEvent);
        mir_free(dat->okButtonFormat);
        mir_free(dat);
        break;

    case WM_CLOSE:
        DestroyWindow(hwndDlg);
        break;
    }
    return FALSE;
}


static void setXStatusEx(BYTE bXStatus, BYTE bQuiet)
{
    CLISTMENUITEM mi = {0};
    BYTE bOldXStatus = getSettingByte(NULL, DBSETTING_XSTATUSID, 0);

    mi.cbSize = sizeof(mi);

    if (bOldXStatus <= XstatusIcons)
    {
        mi.flags = CMIM_FLAGS;
        CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hXStatusItems[bOldXStatus], (LPARAM)&mi);
    }

    mi.flags = CMIM_FLAGS | CMIF_CHECKED;
    CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hXStatusItems[bXStatus], (LPARAM)&mi);

    if (bXStatus)
    {
        char szSetting[64];
        char str[MAX_PATH];
        char *szName, *szMsg;

        sprintf(szSetting, "XStatus%dName", bXStatus);
        szName = getSettingStringUtf(NULL, szSetting, ICQTranslateUtfStatic(nameXStatus[bXStatus-1], str, MAX_PATH));
        sprintf(szSetting, "XStatus%dMsg", bXStatus);
        szMsg = getSettingStringUtf(NULL, szSetting, "");

        sprintf(szSetting, "XStatus%dStat", bXStatus);
        if (!bQuiet && !getSettingByte(NULL, szSetting, 0))
        {
            InitXStatusData init;

            //set xstatus to null, wenn dialog box opened
            //setXStatusEx(0, 1);

            init.bAction = 0; // set
            init.bXStatus = bXStatus;
            init.szXStatusName = szName;
            init.szXStatusMsg = szMsg;
            DialogBoxUtf(FALSE, hInst, MAKEINTRESOURCEA(IDD_SETXSTATUS),NULL,SetXStatusDlgProc,(LPARAM)&init);
        }
        else
        {
            //by [sin]
            if(getSettingByte(NULL, "ForceXstatus", 0))
            {
                if(bXStatus != 0 && icqOnline)
                {
                    setXStatusEx(0, 1);
                    SleepEx(500, TRUE);//should be 500, less makes icq sometimes not reread
                }
            }

            setSettingByte(NULL, DBSETTING_XSTATUSID, bXStatus);
            setSettingStringUtf(NULL, DBSETTING_XSTATUSNAME, szName);
            setSettingStringUtf(NULL, DBSETTING_XSTATUSMSG, szMsg);

            updateServerCustomStatus();
        }
        mir_free(szName);
        mir_free(szMsg);
    }
    else
    {
        setSettingByte(NULL, DBSETTING_XSTATUSID, bXStatus);
        deleteSetting(NULL, DBSETTING_XSTATUSNAME);
        deleteSetting(NULL, DBSETTING_XSTATUSMSG);

        updateServerCustomStatus();
    }
}

static INT_PTR menuXStatus(WPARAM wParam,LPARAM lParam,LPARAM fParam)
{
    setXStatusEx((BYTE)fParam, 0);

    return 0;
}

void InitXStatusItems(BOOL bAllowStatus)
{
    CLISTMENUITEM mi;
    BYTE i = 0;
    BYTE j = 0;
    char srvFce[MAX_PATH + 64];
    char szItem[MAX_PATH + 64];
    HANDLE hXStatusRoot;
    int bXStatusMenuBuilt = 0;

    BYTE bXStatus = ICQGetContactXStatus(NULL);

    // integration of Custom Status List replacing ICQ Custom status menu
    // note: I'd like a revision of calling PS_ICQ_SETCUSTOMSTATUSEX with param CSSF_DISABLE_UI,
    // which would disable Custom Status itselves, instead of solving hiding of menu here ;)
    if( ServiceExists( "CSList/ShowList" ) )
        if( DBGetContactSettingByte( NULL, "CSList", "hideCS", 0 ) )
            return;

    if (!gbXStatusEnabled) return;

    if (bStatusMenu && !bAllowStatus) return;

    if (bHideXStatusUI || bHideXStatusMenu) return;

    null_snprintf(szItem, sizeof(szItem), ICQTranslate("%s Custom Status"), ICQ_PROTOCOL_NAME);
    mi.cbSize = sizeof(mi);
    mi.pszPopupName = szItem;
    mi.popupPosition= 500084000;
    mi.position = 2000040000;

    while  (i < XstatusItems)
    {
        char szTemp[64];
        char* szValue;
        char* ansi = NULL;

        //if(i == 34) j--;//by BeteTest

        null_snprintf(srvFce, sizeof(srvFce), "%s/menuXStatus%d", ICQ_PROTOCOL_NAME, j);

        mi.position++;

        if (!i)
            bXStatusMenuBuilt = ServiceExists(srvFce);

        if (!bXStatusMenuBuilt)
            CreateServiceFunctionParam(srvFce, menuXStatus, j);

        mi.flags = (j ? CMIF_ICONFROMICOLIB : 0) | (bXStatus == j?CMIF_CHECKED:0);
        mi.icolibItem = j ? hXStatusIconsHandle[j-1] : NULL;

        mi.pszName = j ? (char*)nameXStatus[j-1] : LPGEN("None");
        sprintf(szTemp, "XStatus%dName", j);
        szValue = getSettingStringUtf(NULL, szTemp, "");
        ansi = mir_utf8decodeA(szValue);

        mi.pszName = ((ansi && strlen(ansi) == 0) || !DBGetContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "ShowMyXText", 1)) ? ICQTranslate(j?nameXStatus[j-1]:"None") : ansi;

        /*if(i==33)
        {
            mi.pszName = LPGEN("Other clients");
            mi.pszName = ICQTranslate("Other clients");
        }*/
        mi.pszService = srvFce;
        mi.pszContactOwner = ICQ_PROTOCOL_NAME;
        //Create menu icons
        if (bStatusMenu)
            hXStatusItems[j] = (HANDLE)CallService(MS_CLIST_ADDSTATUSMENUITEM, (WPARAM)&hXStatusRoot, (LPARAM)&mi);
        else
            hXStatusItems[j] = (HANDLE)CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi);

        mir_free(szValue);
        mir_free(ansi);
        i++;
        j++;
    }
}

void InitXStatusIcons()
{
    char szSection[MAX_PATH + 64];
    char str[MAX_PATH], prt[MAX_PATH];
    char lib[2*MAX_PATH] = {0};
    char* icon_lib;
    BYTE i;

    if (!gbXStatusEnabled) return;

    icon_lib = InitXStatusIconLibrary(lib);

    null_snprintf(szSection, sizeof(szSection), ICQTranslateUtfStatic("Status Icons/%s/Custom Status", str, MAX_PATH), ICQTranslateUtfStatic(ICQ_PROTOCOL_NAME, prt, MAX_PATH));

    for (i = 0; i < XstatusIcons; i++)
    {
        char szTemp[16];

        null_snprintf(szTemp, sizeof(szTemp), "xstatus%d", i);
        hXStatusIconsHandle[i] = IconLibDefine(nameXStatus[i], szSection, szTemp, 0, icon_lib, -(IDI_XSTATUS1+i),0);
    }

    // initialize arrays for CList custom status icons
    memset(bXStatusCListIconsValid,0,sizeof(bXStatusCListIconsValid));
    memset(hXStatusCListIcons,-1,sizeof(hXStatusCListIcons));
}


void ChangedIconsXStatus()
{
    memset(bXStatusCListIconsValid,0,sizeof(bXStatusCListIconsValid));
}



INT_PTR IcqShowXStatusDetails(WPARAM wParam, LPARAM lParam)
{
    // nasty hack, should be done better x)
    DBVARIANT dbv = { DBVT_TCHAR };
    DBGetContactSettingTString( ( HANDLE )wParam, ICQ_PROTOCOL_NAME, "MirVer", &dbv );
    if ( _tcscmp( dbv.ptszVal, _T( "ICQ 6" ) ) == 0 )
        CallService(MS_AWAYMSG_SHOWAWAYMSG, wParam, 0);
    else // other clients
    {
        InitXStatusData init;

        sendXStatusDetailsRequest((HANDLE)wParam, 1);

        init.bAction = 1; // retrieve
        init.hContact = (HANDLE)wParam;
        DialogBoxUtf(FALSE, hInst, MAKEINTRESOURCEA(IDD_SETXSTATUS), NULL, SetXStatusDlgProc, (LPARAM)&init);
    }
    return 0;
}

INT_PTR IcqSetXStatus(WPARAM wParam, LPARAM lParam)
{
    // obsolete (TODO: remove in next version)
    if (!gbXStatusEnabled) return 0;

    if (wParam >= 0 && wParam <= XstatusIcons)
    {
        setXStatusEx((BYTE)wParam, 1);

        return wParam;
    }
    return 0;
}



INT_PTR IcqGetXStatus(WPARAM wParam, LPARAM lParam)
{
    // obsolete (TODO: remove in next version)
    BYTE status = ICQGetContactXStatus(NULL);

    if (!gbXStatusEnabled) return 0;

    if (!icqOnline) return 0;

    if (status < 1 || status > XstatusIcons) status = 0;

    if (wParam) *((char**)wParam) = DBSETTING_XSTATUSNAME;
    if (lParam) *((char**)lParam) = DBSETTING_XSTATUSMSG;

    return status;
}



INT_PTR IcqSetXStatusEx(WPARAM wParam, LPARAM lParam)
{
    ICQ_CUSTOM_STATUS *pData = (ICQ_CUSTOM_STATUS*)lParam;

    if (!gbXStatusEnabled) return 1;

    if (pData->cbSize < sizeof(ICQ_CUSTOM_STATUS)) return 1; // Failure

    if (pData->flags & CSSF_MASK_STATUS)
    {
        // set custom status
        BYTE status = *pData->status;

        if (status >= 0 && status <= XstatusIcons)
        {
            setXStatusEx((BYTE)status, 1);
        }
        else
            return 1; // Failure
    }

    if (pData->flags & (CSSF_MASK_NAME | CSSF_MASK_MESSAGE))
    {
        BYTE status = ICQGetContactXStatus(NULL);

        if (!status) return 1; // Failure

        if (pData->flags & CSSF_MASK_NAME)
        {
            // set custom status name
            if (pData->flags & CSSF_UNICODE)
            {
                char* utf = mir_utf8encodeW(pData->pwszName);

                setSettingStringUtf(NULL, DBSETTING_XSTATUSNAME, utf);
                mir_free(utf);
            }
            else
                setSettingString(NULL, DBSETTING_XSTATUSNAME, pData->pszName);
        }
        if (pData->flags & CSSF_MASK_MESSAGE)
        {
            // set custom status message
            if (pData->flags & CSSF_UNICODE)
            {
                char* utf = mir_utf8encodeW(pData->pwszMessage);

                setSettingStringUtf(NULL, DBSETTING_XSTATUSMSG, utf);
                mir_free(utf);
            }
            else
                setSettingString(NULL, DBSETTING_XSTATUSMSG, pData->pszMessage);
        }
    }

    if (pData->flags & CSSF_DISABLE_UI)
    {
        // hide menu items
        BYTE n;

        bHideXStatusUI = (*pData->wParam) ? 0 : 1;

        for (n = 0; n<=XstatusIcons; n++)
            CListShowMenuItem(hXStatusItems[n], (BYTE)!bHideXStatusUI);
    }

    if (pData->flags & CSSF_DISABLE_MENU)
    {
        // hide menu items
        bHideXStatusMenu = (*pData->wParam) ? 0 : 1;
    }

    return 0; // Success
}



INT_PTR IcqGetXStatusEx(WPARAM wParam, LPARAM lParam)
{
    ICQ_CUSTOM_STATUS *pData = (ICQ_CUSTOM_STATUS*)lParam;
    HANDLE hContact = (HANDLE)wParam;

    if (!gbXStatusEnabled)
        return 1;

    if (pData->cbSize < sizeof(ICQ_CUSTOM_STATUS)) return 1; // Failure

    if (pData->flags & CSSF_MASK_STATUS)
    {
        // fill status member
        *pData->status = ICQGetContactXStatus(hContact);
    }

    if (pData->flags & CSSF_MASK_NAME)
    {
        // fill status name member
        if (pData->flags & CSSF_DEFAULT_NAME)
        {
            BYTE status = *pData->wParam;

            if (status < 1 || status > XstatusIcons) return 1; // Failure

            if (pData->flags & CSSF_UNICODE)
            {
                char *text = (char*)nameXStatus[status -1];

                MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, text, -1, pData->pwszName, MAX_PATH);
            }
            else
                strcpy(pData->pszName, nameXStatus[status - 1]);
        }
        else
        {
            if (pData->flags & CSSF_UNICODE)
            {
                char* str = getSettingStringUtf(hContact, DBSETTING_XSTATUSNAME, "");
                wchar_t* wstr = mir_utf8decodeW(str);

                wcscpy(pData->pwszName, wstr);
                mir_free(str);
                mir_free(wstr);
            }
            else
            {
                DBVARIANT dbv = {0};

                if (!getSettingString(hContact, DBSETTING_XSTATUSNAME, &dbv) && dbv.pszVal)
                    strcpy(pData->pszName, dbv.pszVal);
                else
                    strcpy(pData->pszName, "");

                ICQFreeVariant(&dbv);
            }
        }
    }

    if (pData->flags & CSSF_MASK_MESSAGE)
    {
        // fill status message member
        if (pData->flags & CSSF_UNICODE)
        {
            char* str = getSettingStringUtf(hContact, DBSETTING_XSTATUSMSG, "");
            wchar_t* wstr = mir_utf8decodeW(str);

            wcscpy(pData->pwszMessage, wstr);
            mir_free(str);
            mir_free(wstr);
        }
        else
        {
            DBVARIANT dbv = {0};

            if (!getSettingString(hContact, DBSETTING_XSTATUSMSG, &dbv) && dbv.pszVal)
                strcpy(pData->pszMessage, dbv.pszVal);
            else
                strcpy(pData->pszMessage, "");

            ICQFreeVariant(&dbv);
        }
    }

    if (pData->flags & CSSF_DISABLE_UI)
    {
        if (pData->wParam) *pData->wParam = !bHideXStatusUI;
    }

    if (pData->flags & CSSF_DISABLE_MENU)
    {
        if (pData->wParam) *pData->wParam = !bHideXStatusMenu;
    }

    if (pData->flags & CSSF_STATUSES_COUNT)
    {
        if (pData->wParam) *pData->wParam = XstatusIcons;
    }

    if (pData->flags & CSSF_STR_SIZES)
    {
        DBVARIANT dbv = {0};

        if (pData->wParam)
        {
            if (!getSettingString(hContact, DBSETTING_XSTATUSNAME, &dbv))
            {
                *pData->wParam = strlennull(dbv.pszVal);
                ICQFreeVariant(&dbv);
            }
            else
                *pData->wParam = 0;
        }
        if (pData->lParam)
        {
            if (!getSettingString(hContact, DBSETTING_XSTATUSMSG, &dbv))
            {
                *pData->lParam = strlennull(dbv.pszVal);
                ICQFreeVariant(&dbv);
            }
            else
                *pData->lParam = 0;
        }
    }

    return 0; // Success
}



INT_PTR IcqGetXStatusIcon(WPARAM wParam, LPARAM lParam)
{
    if (!gbXStatusEnabled) return 0;

    if (!wParam)
        wParam = ICQGetContactXStatus(NULL);

    if (wParam >= 1 && wParam <= XstatusIcons)
    {
        if (lParam == LR_SHARED)
            return (int)GetXStatusIcon((BYTE)wParam, lParam);
        else
            return (int)GetXStatusIcon((BYTE)wParam, 0);
    }
    return 0;
}



INT_PTR IcqRequestXStatusDetails(WPARAM wParam, LPARAM lParam)
{
    HANDLE hContact = (HANDLE)wParam;

    if (!gbXStatusEnabled)
        return 0;
    if (hContact && !getSettingByte(NULL, "XStatusAuto", DEFAULT_XSTATUS_AUTO) &&
            ICQGetContactXStatus(hContact))
    {
        // user has xstatus, no auto-retrieve details, valid contact, request details
        return requestXStatusDetails(hContact, TRUE);
    }
    return 0;
}



INT_PTR IcqRequestAdvStatusIconIdx(WPARAM wParam, LPARAM lParam)
{
    BYTE bXStatus;

    if (!gbXStatusEnabled) return -1;

    bXStatus = ICQGetContactXStatus((HANDLE)wParam);

    if (bXStatus)
    {
        int idx=-1;

        if (!bXStatusCListIconsValid[bXStatus-1])
        {
            // adding icon
            int idx = hXStatusCListIcons[bXStatus-1];
            HIMAGELIST hCListImageList = (HIMAGELIST)CallService(MS_CLIST_GETICONSIMAGELIST,0,0);

            if (hCListImageList)
            {
                HICON hXStatusIcon = GetXStatusIcon(bXStatus, LR_SHARED);

                if (idx > 0)
                    ImageList_ReplaceIcon(hCListImageList, idx, hXStatusIcon);
                else
                    hXStatusCListIcons[bXStatus-1] = ImageList_AddIcon(hCListImageList, hXStatusIcon);
                // mark icon index in the array as valid
                bXStatusCListIconsValid[bXStatus-1] = TRUE;
            }
        }
        idx = bXStatusCListIconsValid[bXStatus-1] ? hXStatusCListIcons[bXStatus-1] : -1;

        if (idx > 0)
            return (idx & 0xFFFF) << 16;
    }
    return -1;
}
