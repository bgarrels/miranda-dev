// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin Öberg, Sam Kothari, Robert Rainwater
// Copyright © 2004,2005,2006,2007 Joe Kucera, LubomirR
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
// File name      : $Source: /cvsroot/miranda/miranda/protocols/IcqOscarJ/icq_servlist.c,v $
// Revision       : $Revision$
// Last change on : $Date$
// Last change by : $Author$
//
// DESCRIPTION:
//
//  tZers
//
// -----------------------------------------------------------------------------


#include "icqoscar.h"
#include "m_folders.h"


extern HANDLE hIconFolder;


INT_PTR CALLBACK tZersWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int DialogWidth = 0;
    int DialogHeight = 0;
    int x = GetSystemMetrics(SM_CXSCREEN);
    int y = GetSystemMetrics(SM_CYSCREEN);
    int newleft = 0;
    int newtop = 0;
    int curSel;
    int len;
    char * txt = {0};
    char * txt2 = {0};
    char * tzid = {0};
    char * tzurl = {0};
    RECT rect;

    static HANDLE hContact;
    static DWORD dwUin;


    switch (uMsg)
    {
    case WM_INITDIALOG:
        ICQTranslateDialog(hWnd);
        hContact = (HANDLE)lParam;
        if (!hContact)
        {
            EndDialog(hWnd, 0);
            return FALSE;
        }

        if (!(dwUin = ICQGetContactSettingUIN(hContact)))
        {
            EndDialog(hWnd, 0);
            return FALSE;
        }

        GetWindowRect(hWnd, &rect);
        DialogWidth = rect.right - rect.left;
        DialogHeight = rect.bottom - rect.top;

        newleft = ((x + 1) - DialogWidth) / 2;
        newtop = ((y + 1) - DialogHeight) / 2;

        MoveWindow(hWnd, newleft, newtop, DialogWidth, DialogHeight, TRUE);

        {
            int i = 0;
            while(i<12)
            {
                SendDlgItemMessageA(hWnd, IDC_TZER_COMBO, CB_ADDSTRING, 0, (LPARAM)(tZers[i].szTxt));
                i++;
            }
        }
        SendDlgItemMessageA(hWnd, IDC_TZER_COMBO, CB_SETCURSEL, 0, 0);

        SendDlgItemMessageA(hWnd, IDC_TZER_NAME, WM_SETTEXT, 0, (LPARAM)"Gangsta");

        SendDlgItemMessageA(hWnd, IDC_TZER_ICON, STM_SETICON, (WPARAM)IconLibGetIcon("tzer0"), 0);

        ShowWindow(hWnd, SW_SHOW);
        break;
    case WM_COMMAND:
        if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == IDC_TZER_COMBO)
        {
            char szTemp[16];
            curSel = SendDlgItemMessageA(hWnd, IDC_TZER_COMBO, CB_GETCURSEL, 0, 0);
            txt = tZers[curSel].szTxt;
            SendDlgItemMessageA(hWnd, IDC_TZER_NAME, WM_SETTEXT, 0, (LPARAM)txt);
            null_snprintf(szTemp, sizeof(szTemp), "tzer%d", curSel);
            SendDlgItemMessageA(hWnd, IDC_TZER_ICON, STM_SETICON, (WPARAM)IconLibGetIcon(szTemp), 0);
        }

        if (LOWORD(wParam) == IDOK)
        {
            curSel = SendDlgItemMessageA(hWnd, IDC_TZER_COMBO, CB_GETCURSEL, 0, 0);
            len = SendDlgItemMessageA(hWnd, IDC_TZER_NAME, WM_GETTEXTLENGTH, 0, 0);
            txt = (char*)malloc(len + 1);
            if (txt)
            {
                DBEVENTINFO Event = {0};
                char szText[MAX_PATH];
                PBYTE pCurBlob;
                WORD wTextLen;
                SendDlgItemMessageA(hWnd, IDC_TZER_NAME, WM_GETTEXT, (WPARAM)(len + 1), (LPARAM)txt);
                tzid = tZers[curSel].szId;
                tzurl = tZers[curSel].szUrl;
                txt2 = MangleXml(txt, len);
                Event.cbSize=sizeof(Event);
                Event.szModule=ICQ_PROTOCOL_NAME;
                Event.eventType=EVENTTYPE_MESSAGE;
                Event.flags=DBEF_SENT,DBEF_UTF;
                Event.timestamp=(DWORD)time(NULL);
                sprintf(szText, "tZer sent: %s\r\n%s", txt, tzurl);
                wTextLen = strlen(szText);
                Event.cbBlob = sizeof(DWORD)+sizeof(HANDLE)+wTextLen+1;
                pCurBlob = Event.pBlob = (PBYTE)icq_alloc_zero(Event.cbBlob);
                memcpy(pCurBlob,&szText,wTextLen);
                pCurBlob+=wTextLen;
                *(char *)pCurBlob = 0;
                pCurBlob++;
                memcpy(pCurBlob,&dwUin,sizeof(DWORD));
                pCurBlob+=sizeof(DWORD);
                memcpy(pCurBlob,&hContact,sizeof(HANDLE));
                CallService(MS_DB_EVENT_ADD, (WPARAM)(HANDLE)hContact, (LPARAM)&Event);
                free(txt);
                SendtZer(hContact, dwUin, "ID", txt2, tzurl);
                free(txt2);
                EndDialog(hWnd, 0);
            }
        }

        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hWnd, 0);
        }
    }
    return FALSE;
}

static HANDLE LoadTzersIconLibrary(char* path, const char* sub)
{
    char* p = path+strlen(path);
    HANDLE hLib;

    strcpy(p, sub);
    strcat(p, "\\tzers_icons.dll");
    if (hLib = LoadLibraryA(path)) return hLib;
    strcpy(p, sub);
    strcat(p, "\\tzers_ICQ.dll");
    if (hLib = LoadLibraryA(path)) return hLib;

    return hLib;
}


static char* InitTzersIconLibrary(char* buf)
{
    char path[2*MAX_PATH];
    char *p;
    HMODULE hIconsDLL=NULL;

    if( hIconFolder )
    {
        FoldersGetCustomPath(hIconFolder, path, MAX_PATH, "icons");
        if (!hIconsDLL)
            hIconsDLL = (HMODULE)LoadTzersIconLibrary(path, "");
    }
    if (!hIconsDLL)
    {
        // get miranda's exe path
        GetModuleFileNameA(NULL, path, MAX_PATH);
        p = strrchr(path, '\\');
        if(p) *p='\0';

        if (!hIconsDLL)
            hIconsDLL = (HMODULE)LoadTzersIconLibrary(path, "\\Icons");

        if (!hIconsDLL)
            hIconsDLL = (HMODULE)LoadTzersIconLibrary(path, "\\Plugins");

    }
    if (hIconsDLL)
    {
        FreeLibrary(hIconsDLL);
        strcpy(buf, path);
    }

    return buf;
}
void InitTzersIcons()
{
    char szSection[MAX_PATH + 64];
    char str[MAX_PATH], prt[MAX_PATH];
    char lib[2*MAX_PATH] = {0};
    char* icon_lib;
    BYTE i;

    if (!gbXStatusEnabled) return;
    icon_lib = InitTzersIconLibrary(lib);
    null_snprintf(szSection, sizeof(szSection), ICQTranslateUtfStatic("Status Icons/%s/Tzers", str, MAX_PATH), ICQTranslateUtfStatic(ICQ_PROTOCOL_NAME, prt, MAX_PATH));
    for (i = 0; i < TZER_COUNT; i++)
    {
        char szTemp[64];
        null_snprintf(szTemp, sizeof(szTemp), "tzer%i", i);
        IconLibDefine(tZers[i].szTxt, szSection, szTemp, 0, icon_lib, -(IDI_TZER1+i), 64);
    }
}



TZerInfo tZers[TZER_COUNT] =
{
    { "Gangsta", "gangSh", "http://c.icq.com/xtraz/products/teaser/anims/common/gangsterSheep.swf"},
    { "Can't Hear U", "cantH", "http://c.icq.com/xtraz/products/teaser/anims/common/cant_hear.swf"},
    { "Scratch", "scratch", "http://c.icq.com/xtraz/products/teaser/anims/common/scratch.swf"},
    { "Booooo", "boo", "http://c.icq.com/xtraz/products/teaser/anims/common/boo.swf"},
    { "Kisses", "kisses", "http://c.icq.com/xtraz/products/teaser/anims/common/kisses.swf"},
    { "Chill Out!", "rasta", "http://c.icq.com/xtraz/products/teaser/anims/common/rastamab.swf"},
    { "Akitaka", "arakiri", "http://c.icq.com/xtraz/products/teaser/anims/common/sappuko.swf"},
    { "I'm Sorry", "sorry", "http://c.icq.com/xtraz/products/teaser/anims/common/sorry.swf"},
    { "Hilaaarious", "laugh", "http://c.icq.com/xtraz/products/teaser/anims/common/laugh.swf"},
    { "Like Duh!", "da", "http://c.icq.com/xtraz/products/teaser/anims/common/dahh.swf"},
    { "L8R", "beback", "http://c.icq.com/xtraz/products/teaser/anims/common/beBack.swf"},
    { "Like U!", "ilikeu", "http://c.icq.com/xtraz/products/teaser/anims/common/iLikeU.swf"},

};
