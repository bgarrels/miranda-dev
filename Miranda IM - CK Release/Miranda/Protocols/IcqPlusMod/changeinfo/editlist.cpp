// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright � 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright � 2001,2002 Jon Keating, Richard Hughes
// Copyright � 2002,2003,2004 Martin �berg, Sam Kothari, Robert Rainwater
// Copyright � 2004,2005,2006,2007 Joe Kucera
// Copyright � 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
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
// Revision       : $Revision: 43 $
// Last change on : $Date: 2007-08-20 01:51:06 +0300 (Пн, 20 авг 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  ChangeInfo Plugin stuff
//
// -----------------------------------------------------------------------------

#include "../icqoscar.h"


static HWND hwndListEdit=NULL;
static BOOL (WINAPI *MyAnimateWindow)(HWND,DWORD,DWORD);
static WNDPROC OldListEditProc;


static LRESULT CALLBACK ListEditSubclassProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch(msg)
    {
    case WM_LBUTTONUP:
        CallWindowProc(OldListEditProc, hwnd, msg, wParam, lParam);
        {
            POINT pt;

            pt.x = (short)LOWORD(lParam);
            pt.y = (short)HIWORD(lParam);
            ClientToScreen(hwnd, &pt);
            if (SendMessage(hwnd, WM_NCHITTEST, 0, MAKELPARAM(pt.x, pt.y)) == HTVSCROLL) break;
        }
        {
            int i = SendMessage(hwnd, LB_GETCURSEL, 0, 0);

            EndListEdit(i != LB_ERR);
        }
        return 0;

    case WM_CHAR:
        if (wParam != '\r') break;
        {
            int i = SendMessage(hwnd, LB_GETCURSEL, 0, 0);

            EndListEdit(i != LB_ERR);
        }
        return 0;
    case WM_KILLFOCUS:
        EndListEdit(1);
        return 0;
    }
    return CallWindowProc(OldListEditProc, hwnd, msg, wParam, lParam);
}




void BeginListEdit(int iItem, RECT *rc, int iSetting, WORD wVKey)
{
    int j,n;
    POINT pt;
    int itemHeight, listCount;
    char str[MAX_PATH];
    FieldNamesItem *list;

    EndListEdit(0);

    pt.x=pt.y=0;
    ClientToScreen(hwndList,&pt);
    OffsetRect(rc,pt.x,pt.y);
    InflateRect(rc,-2,-2);
    rc->left-=2;
    iEditItem = iItem;
    ListView_RedrawItems(hwndList, iEditItem, iEditItem);
    UpdateWindow(hwndList);

    hwndListEdit = CreateWindowExA(WS_EX_TOOLWINDOW|WS_EX_TOPMOST, "LISTBOX", "", WS_POPUP|WS_BORDER|WS_VSCROLL, rc->left, rc->bottom, rc->right - rc->left, 150, NULL, NULL, hInst, NULL);
    SendMessage(hwndListEdit, WM_SETFONT, (WPARAM)hListFont, 0);
    itemHeight = SendMessage(hwndListEdit, LB_GETITEMHEIGHT, 0, 0);

    list = (FieldNamesItem*)setting[iSetting].pList;

    if (list == countryField)
    {
        // some country codes were changed leaving old details uknown, convert it for the user
        if (setting[iSetting].value == 420) setting[iSetting].value = 42; // conversion of obsolete codes (OMG!)
        else if (setting[iSetting].value == 421) setting[iSetting].value = 4201;
        else if (setting[iSetting].value == 102) setting[iSetting].value = 1201;
    }

    n = ListBoxAddStringUtf(hwndListEdit, "Unspecified");
    for (j=0; ; j++)
        if (!list[j].text)
        {
            SendMessage(hwndListEdit, LB_SETITEMDATA, n, j);
            if ((setting[iSetting].value == 0 && list[j].code == 0)
                    || (setting[iSetting].dbType != DBVT_ASCIIZ && setting[iSetting].value == list[j].code))
                SendMessage(hwndListEdit, LB_SETCURSEL, n, 0);
            break;
        }

    for (j=0; list[j].text; j++)
    {
        n = ListBoxAddStringUtf(hwndListEdit, list[j].text);
        SendMessage(hwndListEdit, LB_SETITEMDATA, n, j);
        if ((setting[iSetting].dbType == DBVT_ASCIIZ && (!strcmpnull((char*)setting[iSetting].value, list[j].text))
                || (setting[iSetting].dbType == DBVT_ASCIIZ && (!strcmpnull((char*)setting[iSetting].value, ICQTranslateUtfStatic(list[j].text, str, MAX_PATH))))
                || ((char*)setting[iSetting].value == NULL && list[j].code == 0))
                || (setting[iSetting].dbType != DBVT_ASCIIZ && setting[iSetting].value == list[j].code))
            SendMessage(hwndListEdit, LB_SETCURSEL, n, 0);
    }
    SendMessage(hwndListEdit, LB_SETTOPINDEX, SendMessage(hwndListEdit, LB_GETCURSEL, 0, 0) - 3, 0);
    listCount = SendMessage(hwndListEdit, LB_GETCOUNT, 0, 0);
    if (itemHeight * listCount < 150)
        SetWindowPos(hwndListEdit, 0, 0, 0, rc->right - rc->left, itemHeight * listCount + GetSystemMetrics(SM_CYBORDER) * 2, SWP_NOZORDER|SWP_NOMOVE);
    OldListEditProc = (WNDPROC)SetWindowLongPtr(hwndListEdit, GWLP_WNDPROC, (LONG_PTR)ListEditSubclassProc);
    if (MyAnimateWindow = (BOOL (WINAPI*)(HWND,DWORD,DWORD))GetProcAddress(GetModuleHandleA("user32"), "AnimateWindow"))
    {
        BOOL enabled;

        SystemParametersInfo(SPI_GETCOMBOBOXANIMATION, 0, &enabled, FALSE);
        if (enabled) MyAnimateWindow(hwndListEdit, 200, AW_SLIDE|AW_ACTIVATE|AW_VER_POSITIVE);
    }
    ShowWindow(hwndListEdit, SW_SHOW);
    SetFocus(hwndListEdit);
    if (wVKey)
        PostMessage(hwndListEdit, WM_KEYDOWN, wVKey, 0);
}



void EndListEdit(int save)
{
    if (hwndListEdit == NULL || iEditItem == -1) return;
    if (save)
    {
        int iItem = SendMessage(hwndListEdit, LB_GETCURSEL, 0, 0);
        int i = SendMessage(hwndListEdit, LB_GETITEMDATA, iItem, 0);

        if (setting[iEditItem].dbType == DBVT_ASCIIZ)
        {
            char *szNewValue = (((FieldNamesItem*)setting[iEditItem].pList)[i].text);
            if (((FieldNamesItem*)setting[iEditItem].pList)[i].code || setting[iEditItem].displayType & LIF_ZEROISVALID)
            {
                setting[iEditItem].changed = strcmpnull(szNewValue, (char*)setting[iEditItem].value);
                mir_free((void*)setting[iEditItem].value);
                setting[iEditItem].value = (LPARAM)null_strdup(szNewValue);
            }
            else
            {
                setting[iEditItem].changed = (char*)setting[iEditItem].value!=NULL;
                mir_free((void*)setting[iEditItem].value);
            }
        }
        else
        {
            setting[iEditItem].changed = ((FieldNamesItem*)setting[iEditItem].pList)[i].code != setting[iEditItem].value;
            setting[iEditItem].value = ((FieldNamesItem*)setting[iEditItem].pList)[i].code;
        }
        if (setting[iEditItem].changed)
        {
            char buf[MAX_PATH] = {0};
            TCHAR tbuf[MAX_PATH] = {0};
            TCHAR *tmp = mir_utf8decodeW(ICQTranslateUtfStatic(((FieldNamesItem*)setting[iEditItem].pList)[i].text, buf, SIZEOF(buf)));
            wcsncpy(tbuf, tmp, SIZEOF(buf));
            mir_free(tmp);

            if(tbuf && tbuf[0]);
            ListView_SetItemText(hwndList, iEditItem, 1, tbuf);

            EnableDlgItem(GetParent(hwndList), IDC_SAVE, TRUE);

        }
    }
    ListView_RedrawItems(hwndList, iEditItem, iEditItem);
    iEditItem = -1;
    DestroyWindow(hwndListEdit);
    hwndListEdit = NULL;
}



int IsListEditWindow(HWND hwnd)
{
    if (hwnd == hwndListEdit) return 1;
    return 0;
}

int iEditItem;
