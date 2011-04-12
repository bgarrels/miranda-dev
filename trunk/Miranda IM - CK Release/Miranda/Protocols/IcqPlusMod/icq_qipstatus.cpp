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
// Revision       : $Revision: 224 $
// Last change on : $Date: 2009-03-22 03:08:11 +0300 (Ð’Ñ‚, 22 Ð¾ÐºÑ‚ 2008) $
// Last change by : $Author: HierOS $
//
// DESCRIPTION:
//
//  Support for Custom Statuses
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"
#include "m_cluiframes.h"
#include "m_folders.h"


int bQipStatusMenu = 0;

HANDLE hHookQipIconsRebuild = NULL;
HANDLE hHookQStatusBuild = NULL;
HANDLE hHookQipIconsApply = NULL;

extern HANDLE hIconFolder;
extern HANDLE hqipstatuschanged;
extern HANDLE hqipstatusiconchanged;
extern HANDLE hExtraqipstatus;

int hQipStatusCListIcons[5];
BOOL bQipStatusCListIconsValid[5];
HANDLE hQIPStatusRoot;
HANDLE hQIPStatusIcons[5];
HANDLE hQIPStatusItems[5];
HANDLE hQIPStatusHandle[5];
WORD wQIPStatusCap[6]= {ICQ_STATUS_ONLINE, ICQ_STATUS_DEPRESS, ICQ_STATUS_EVIL, ICQ_STATUS_HOME, ICQ_STATUS_WORK, ICQ_STATUS_LUNCH};
const char* nameQIPStatus[5] =
{
    LPGEN("Depress"),
    LPGEN("Angry"),
    LPGEN("At Home"),
    LPGEN("At Work"),
    LPGEN("Eating")
};


static HANDLE LoadQIPStatusIconLibrary(char* path, const char* sub)
{
    char* p = path+strlen(path);
    HANDLE hLib;

    strcpy(p, sub);
    strcat(p, "\\qipstatus_icons.dll");
    if (hLib = LoadLibraryA(path)) return hLib;
    strcpy(p, sub);
    strcat(p, "\\qipstatus_ICQ.dll");
    if (hLib = LoadLibraryA(path)) return hLib;

    return hLib;
}


static char* InitQIPStatusIconLibrary(char* buf)
{
    char path[2*MAX_PATH];
    char *p;
    HMODULE hIconsDLL=NULL;

    if( hIconFolder )
    {
        FoldersGetCustomPath(hIconFolder, path, MAX_PATH, "icons");
        if (!hIconsDLL)
            hIconsDLL = (HMODULE)LoadQIPStatusIconLibrary(path, "");
    }
    if (!hIconsDLL)
    {
        // get miranda's exe path
        GetModuleFileNameA(NULL, path, MAX_PATH);
        p = strrchr(path, '\\');
        if(p) *p='\0';

        if (!hIconsDLL)
            hIconsDLL = (HMODULE)LoadQIPStatusIconLibrary(path, "\\Icons");

        if (!hIconsDLL)
            hIconsDLL = (HMODULE)LoadQIPStatusIconLibrary(path, "\\Plugins");

    }
    if (hIconsDLL)
    {
        FreeLibrary(hIconsDLL);
        strcpy(buf, path);
    }

    return buf;
}


HICON GetQipStatusIcon(int bStatus, UINT flags)
{
    char szTemp[64];
    HICON icon;

    null_snprintf(szTemp, sizeof(szTemp), "QIPstatus%i", bStatus - 1);
    icon = IconLibGetIcon(szTemp);

    if (flags & LR_SHARED)
        return icon;
    else
        return CopyIcon(icon);
}





void UninitQipStatusEvents()
{
    if (hHookQStatusBuild)
        UnhookEvent(hHookQStatusBuild);

    if (hHookQipIconsRebuild)
        UnhookEvent(hHookQipIconsRebuild);

    if (hHookQipIconsApply)
        UnhookEvent(hHookQipIconsApply);
}



static void __fastcall setQIPstatus(BYTE bStatus)
{
    WORD nStatus;

//  ICQWriteContactSettingWord(NULL, "ICQStatus", wQIPStatusCap[bStatus]);
    setSettingWord(NULL, "QIPStatus", wQIPStatusCap[bStatus]);
    nStatus=(wQIPStatusCap[bStatus]==ICQ_STATUS_LUNCH)?ID_STATUS_AWAY:ID_STATUS_ONLINE;
    DBWriteContactSettingWord(NULL, "CList", "Status", nStatus);
    IcqSetStatus(nStatus,0);
}


static INT_PTR menuQIPstatus(WPARAM wParam,LPARAM lParam,LPARAM fParam)
{
    setQIPstatus((BYTE)fParam);
    return 0;
}


char* QIPStatusToString(int QipStatus)
{
    int i;
    for (i = 0; i < SIZEOF(wQIPStatusCap); ++i)
        if (QipStatus == wQIPStatusCap[i])
            return (char*)(i?nameQIPStatus[i-1]:"");
    return "";
}


void InitQipStatusItems()
{
    CLISTMENUITEM mi = {0};
    BYTE i = 0;
    char srvFce[MAX_PATH + 64], szItem[MAX_PATH + 64];
    int bQipStatusMenuBuilt = 0;
    int status;

    if (!gbQipStatusEnabled)
        return;

    null_snprintf(szItem, sizeof(szItem), "%s", Translate("QIP Status"));
    mi.cbSize = sizeof(mi);
    mi.pszPopupName = szItem;
    mi.popupPosition= 500085000;
    mi.position = 2000050000;
    status = GetQipStatusID(NULL);

    for(i = 0; i < 6; i++)
    {
        null_snprintf(srvFce, sizeof(srvFce), "%s/menuQIPstatus%d", ICQ_PROTOCOL_NAME, i);
        mi.position++;

        if (!i)
            bQipStatusMenuBuilt = ServiceExists(srvFce);

        if (!bQipStatusMenuBuilt)
            CreateServiceFunctionParam(srvFce, menuQIPstatus, i);

        mi.flags = (i?CMIF_ICONFROMICOLIB:0) | ((status==i)?CMIF_CHECKED:0);
        mi.icolibItem = i ? hQIPStatusHandle[i-1] : NULL;

        mi.pszName = i ? (char*)nameQIPStatus[i-1] : LPGEN("None");
        mi.pszService = srvFce;
        mi.pszContactOwner = ICQ_PROTOCOL_NAME;

        hQIPStatusItems[i] = (HANDLE)CallService(MS_CLIST_ADDSTATUSMENUITEM, (WPARAM)&hQIPStatusRoot, (LPARAM)&mi);
    }
}


void InitQipStatusIcons()
{
    char szSection[MAX_PATH + 64];
    char str[MAX_PATH], prt[MAX_PATH];
    char lib[2*MAX_PATH] = {0};
    char* icon_lib;
    BYTE i;

    if (!gbQipStatusEnabled) return;
    icon_lib = InitQIPStatusIconLibrary(lib);
    null_snprintf(szSection, sizeof(szSection), ICQTranslateUtfStatic("Status Icons/%s/QIP Status", str, MAX_PATH), ICQTranslateUtfStatic(ICQ_PROTOCOL_NAME, prt, MAX_PATH));
    for (i = 0; i < SIZEOF(hQIPStatusHandle); i++)
    {
        char szTemp[64];
        null_snprintf(szTemp, sizeof(szTemp), "QIPstatus%i", i);
        hQIPStatusHandle[i] = IconLibDefine(nameQIPStatus[i], szSection, szTemp, 0, icon_lib, -(IDI_ADD1+i), 0);
    }
    // initialize arrays for CList custom status icons
    memset(bQipStatusCListIconsValid,0,sizeof(bQipStatusCListIconsValid));
    memset(hQipStatusCListIcons,-1,sizeof(hQipStatusCListIcons));
}


int GetQipStatusID(HANDLE hContact)
{
    int i = 0;
    switch (getSettingWord(hContact, "ICQStatus", 0))
    {
    case ICQ_STATUS_DEPRESS:
        i=1;
        break;
    case ICQ_STATUS_EVIL:
        i=2;
        break;
    case ICQ_STATUS_HOME:
        i=3;
        break;
    case ICQ_STATUS_WORK:
        i=4;
        break;
    case ICQ_STATUS_LUNCH:
        i=5;
        break;
    }
    return i;
}


void ChangedIconsQipStatus()
{
    memset(bQipStatusCListIconsValid,0,sizeof(bQipStatusCListIconsValid));
}

