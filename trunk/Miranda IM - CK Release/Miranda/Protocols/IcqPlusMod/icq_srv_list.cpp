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
// Revision       : $Revision: 36 $
// Last change on : $Date: 2007-08-05 03:45:10 +0300 (Вс, 05 авг 2007) $
// Last change by : $Author: sss123next $
//
//
// DESCRIPTION:
//
//  Server List GUI
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"
#ifndef LVS_EX_CHECKBOXES
#define LVS_EX_CHECKBOXES       0x00000004
#endif
#ifndef LVS_EX_FULLROWSELECT
#define LVS_EX_FULLROWSELECT    0x00000020 // applies to report mode only
#endif
#ifndef _MSC_VER
#define ListView_SetExtendedListViewStyleEx(w,m,s) (DWORD)SNDMSG((w),LVM_SETEXTENDEDLISTVIEWSTYLE,(m),(s))
#define ListView_GetSelectionMark(w) (INT)SNDMSG((w),LVM_GETSELECTIONMARK,0,0)
#endif


static HWND hwndSrvList = NULL;
HWND hCaller;
static INT_PTR CALLBACK DlgProcSrvList(HWND hwndDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    char DBModule[64], buf[64], buf2[64], buf3[64];
    TCHAR *tmp;
    WORD SrvCount;
    int iIndex, i, iRow;
    LVCOLUMN col;
    LVITEM item;
    HWND hwndList=GetDlgItem(hwndDlg, IDC_SERVERS);
    NMLISTVIEW * hdr = (NMLISTVIEW *) lParam;
    DBCONTACTGETSETTING dbgcs;
    mir_snprintf(DBModule, 64, "%sSrvs", ICQ_PROTOCOL_NAME);
    SrvCount = DBGetContactSettingWord(NULL,DBModule,"SrvCount",0);
    switch(message)
    {
    case WM_INITDIALOG:
    {
        ICQTranslateDialog(hwndDlg);
        col.pszText = TranslateT("Use");
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.fmt = LVCFMT_LEFT;
        col.cx = 50;
        ListView_InsertColumn(hwndList, 1, &col);
        col.pszText = TranslateT("Server");
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.fmt = LVCFMT_LEFT;
        col.cx = 1000;
        ListView_InsertColumn(hwndList, 1, &col);
        col.pszText = TranslateT("Port");
        col.cx = 1000;
        ListView_InsertColumn(hwndList, 2, &col);
        ListView_SetExtendedListViewStyleEx(hwndList, 0, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

        if(!DBGetContactSettingWord(NULL,DBModule,"SrvCount",0))
            DBWriteContactSettingWord(NULL,DBModule,"SrvCount",0);
        for(i = 1; i <= SrvCount; i++)
        {
            mir_snprintf(buf2, 64, "server%luhost", i);
            mir_snprintf(buf3, 64, "server%luport", i);
            item.mask = LVIF_TEXT;
            item.iItem = i;
            item.iSubItem = 0;
            tmp = mir_a2t(buf2);
            item.pszText = tmp;
            iRow = ListView_InsertItem(hwndList, &item);
            ListView_SetItemText(hwndList, iRow, 0, NULL);
//				mir_freetmp);
            tmp = mir_a2t(UniGetContactSettingUtf(NULL, DBModule, buf2, 0));
            ListView_SetItemText(hwndList, iRow, 1, tmp);
            mir_snprintf(buf, 64, "%d", DBGetContactSettingWord(NULL, DBModule, buf3, 0));
//				mir_freetmp);
            tmp = mir_a2t(buf);
            ListView_SetItemText(hwndList, iRow, 2, tmp);
//				mir_freetmp);
            mir_snprintf(buf, 64, "server%luuse", i);
            if(DBGetContactSettingByte(0, DBModule, buf, 1))
                ListView_SetItemState(hwndList, iRow, 0x2000, 0xF000);
        }
        ListView_SetColumnWidth(hwndList, 0, LVSCW_AUTOSIZE);
        ListView_SetColumnWidth(hwndList, 1, LVSCW_AUTOSIZE);
        ListView_SetColumnWidth(hwndList, 2, LVSCW_AUTOSIZE);
    }
    return TRUE;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {

        case IDADD:
            SrvCount = DBGetContactSettingWord(NULL, DBModule, "SrvCount", 0);
            mir_snprintf(buf, 64, "server%luuse", SrvCount + 1);
            DBWriteContactSettingWord(NULL, DBModule, buf, 1);
            GetDlgItemTextA(hwndDlg, IDC_EDIT_SERVER, buf , 64);
            if(buf[0]==0)
                break;
            mir_snprintf(buf2, 64, "server%luhost", SrvCount + 1);
            DBWriteContactSettingString(NULL, DBModule, buf2, buf);
            mir_snprintf(buf3, 64, "server%luport", SrvCount + 1);
            if(GetDlgItemInt(hwndDlg, IDC_EDIT_PORT, NULL, FALSE)==0)
                DBWriteContactSettingWord(NULL, DBModule, buf3, 5190);
            else
                DBWriteContactSettingWord(NULL, DBModule, buf3, (WORD)GetDlgItemInt(hwndDlg, IDC_EDIT_PORT, NULL, FALSE));

            item.mask = LVIF_TEXT;
            item.iItem = SrvCount + 1;
            item.iSubItem = 0;
            tmp = mir_a2t(buf2);
            item.pszText = tmp;

            iRow = ListView_InsertItem(hwndList, &item);
//				mir_freetmp);
            tmp = mir_a2t(UniGetContactSettingUtf(NULL, DBModule, buf2, 0));
            ListView_SetItemText(hwndList, iRow, 1, tmp);
//				mir_freetmp);

            mir_snprintf(buf, 64, "%d", DBGetContactSettingWord(NULL, DBModule, buf3, 0));
            tmp = mir_a2t(buf);
            ListView_SetItemText(hwndList, iRow, 2, tmp);
//				mir_freetmp);
            ListView_SetItemText(hwndList, iRow, 0, NULL);
            DBWriteContactSettingWord(NULL, DBModule, "SrvCount", (WORD)(SrvCount + 1));

            mir_snprintf(buf, 64, "server%luuse", SrvCount + 1);
            if(DBGetContactSettingByte(0, DBModule, buf, 1))
                ListView_SetItemState(hwndList, iRow, 0x2000, 0xF000);
            break;
        case IDEDIT:
            iIndex = ListView_GetSelectionMark(hwndList);
            if(iIndex < 0) return FALSE;
            mir_snprintf(buf, 64, "server%luhost", iIndex + 1);
            mir_snprintf(buf2, 64, "server%luport", iIndex + 1);
            DBWriteContactSettingWord(0, DBModule, buf2, (WORD)GetDlgItemInt(hwndDlg, IDC_EDIT_PORT, 0, 0));
            GetDlgItemTextA(hwndDlg, IDC_EDIT_SERVER, buf3 , 64);
            DBWriteContactSettingString(NULL, DBModule, buf, buf3);

            ListView_SetItemText(hwndList, iIndex, 0, NULL);
            tmp = mir_a2t(UniGetContactSettingUtf(NULL, DBModule, buf, 0));
            ListView_SetItemText(hwndList, iIndex, 1, tmp);
//				mir_freetmp);
            mir_snprintf(buf, 64, "%d", DBGetContactSettingWord(NULL, DBModule, buf2, 0));
            tmp = mir_a2t(buf);
            ListView_SetItemText(hwndList, iIndex, 2, tmp);
//				mir_freetmp);

            break;
        case IDDEL:
            iIndex = ListView_GetSelectionMark(hwndList);
            if(iIndex < 0) return FALSE;
            ListView_DeleteItem(hwndList, iIndex);

            dbgcs.szModule = DBModule;
            mir_snprintf(buf, 64, "server%luhost", iIndex + 1);
            dbgcs.szSetting = buf;
            CallService(MS_DB_CONTACT_DELETESETTING, (WPARAM)NULL, (LPARAM)&dbgcs);
            mir_snprintf(buf, 64, "server%luport", iIndex + 1);
            dbgcs.szSetting = buf;
            CallService(MS_DB_CONTACT_DELETESETTING, (WPARAM)NULL, (LPARAM)&dbgcs);
            mir_snprintf(buf, 64, "server%luuse", iIndex + 1);
            dbgcs.szSetting = buf;
            CallService(MS_DB_CONTACT_DELETESETTING, (WPARAM)NULL, (LPARAM)&dbgcs);
            SrvCount = DBGetContactSettingWord(0, DBModule, "SrvCount", 0);
            for(i = iIndex + 2; i <= SrvCount; i++)
            {
                dbgcs.szModule = DBModule;

                mir_snprintf(buf2, 64, "server%luuse", i);
                mir_snprintf(buf3, 64, "server%luuse", i - 1);
                DBWriteContactSettingByte(NULL, DBModule, buf3, (BYTE)DBGetContactSettingByte(0, DBModule, buf2, 1));
                dbgcs.szSetting = buf2;
                CallService(MS_DB_CONTACT_DELETESETTING, (WPARAM)0, (LPARAM)&dbgcs);

                mir_snprintf(buf2, 64, "server%luhost", i);
                mir_snprintf(buf3, 64, "server%luhost", i - 1);
                DBWriteContactSettingString(NULL, DBModule, buf3, UniGetContactSettingUtf(NULL, DBModule, buf2, 0));
                dbgcs.szSetting = buf2;
                CallService(MS_DB_CONTACT_DELETESETTING, (WPARAM)0, (LPARAM)&dbgcs);

                mir_snprintf(buf2, 64, "server%luport", i);
                mir_snprintf(buf3, 64, "server%luport", i - 1);
                DBWriteContactSettingWord(NULL, DBModule, buf3, (WORD)DBGetContactSettingWord(NULL,DBModule,buf2,0));
                dbgcs.szSetting = buf2;
                CallService(MS_DB_CONTACT_DELETESETTING, (WPARAM)0, (LPARAM)&dbgcs);
            }
            DBWriteContactSettingWord(NULL, DBModule, "SrvCount", (WORD)(SrvCount - 1));
            break;
        case IDCCLOSE:
            SendMessage(hCaller, WM_INITDIALOG, 0, 0);
            DestroyWindow(hwndDlg);
            break;
        }
    }
    break;
    case WM_NOTIFY:
        if ( hdr && hdr->hdr.code == LVN_ITEMCHANGED && IsWindowVisible(hdr->hdr.hwndFrom)
                && hdr->iItem != (-1) )
        {
            iIndex = hdr->iItem;
            if(iIndex < 0) return FALSE;
            mir_snprintf(buf, 64, "server%luhost", iIndex + 1);
            mir_snprintf(buf2, 64, "server%luport", iIndex + 1);
            SetDlgItemTextA(hwndDlg, IDC_EDIT_SERVER, UniGetContactSettingUtf(NULL, DBModule, buf, 0));
            SetDlgItemInt(hwndDlg, IDC_EDIT_PORT, (UINT)DBGetContactSettingWord(0, DBModule, buf2, 5190), 0);
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwndDlg);
        break;
    case WM_DESTROY:
        hwndSrvList = NULL;
        for (iRow = 0 ; iRow != (-1) ; )
        {
            int iState = ListView_GetItemState(hwndList, iRow, LVIS_STATEIMAGEMASK);
            mir_snprintf(buf, 64, "server%luuse", iRow + 1);
            DBWriteContactSettingByte(0, DBModule, buf, (BYTE)(iState&0x2000?1:0));
            iRow = ListView_GetNextItem(hwndList, iRow, LVNI_ALL);
        }
        break;
    }

    return FALSE;
}

void ShowSrvListDialog(HWND hwndCaller)
{
    hCaller = hwndCaller;
    if (hwndSrvList == NULL)
    {
        hwndSrvList = CreateDialogUtf(hInst, MAKEINTRESOURCEA(IDD_ICQ_SRV_LIST), NULL, DlgProcSrvList);
    }
    SetForegroundWindow(hwndSrvList);
}
