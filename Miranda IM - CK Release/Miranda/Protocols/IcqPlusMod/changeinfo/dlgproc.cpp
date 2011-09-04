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


static int editTopIndex;
static HANDLE hAckHook = NULL;
static HFONT hMyFont = NULL;

#define DM_PROTOACK  (WM_USER+10)

static int DrawTextUtf(HDC hDC, char* text, LPRECT lpRect, UINT uFormat, LPSIZE lpSize)
{
    int res;

    if (gbUnicodeAPI_dep)
    {
        wchar_t *tmp = mir_utf8decodeW(text);

        res = DrawTextW(hDC, tmp, -1, lpRect, uFormat);
        if (lpSize)
            GetTextExtentPoint32W(hDC, tmp, wcslen(tmp), lpSize);
        mir_free(tmp);
    }
    else
    {
        // caution, here we change text's contents
        char *tmp = mir_utf8decodeA(text);
        strcpy(text, tmp);
        mir_free(tmp);
        res = DrawTextA(hDC, text, -1, lpRect, uFormat);
        if (lpSize)
            GetTextExtentPoint32A(hDC, text, strlennull(text), lpSize);
    }
    return res;
}




void PaintItemSetting(HDC hdc, RECT *rc, int i, UINT itemState)
{
    char str[MAX_PATH];
    char *text = GetItemSettingText(i, str, SIZEOF(str));

    if (setting[i].value == 0 && !(setting[i].displayType & LIF_ZEROISVALID))
        SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));

    if ((setting[i].displayType & LIM_TYPE) == LI_LIST && (itemState & CDIS_SELECTED || iEditItem == i))
    {
        RECT rcBtn;

        rcBtn = *rc;
        rcBtn.left = rcBtn.right - rc->bottom + rc->top;
        InflateRect(&rcBtn,-2,-2);
        rc->right = rcBtn.left;
        DrawFrameControl(hdc, &rcBtn, DFC_SCROLL, iEditItem == i ? DFCS_SCROLLDOWN|DFCS_PUSHED : DFCS_SCROLLDOWN);
    }
    DrawTextUtf(hdc, text, rc, DT_END_ELLIPSIS|DT_LEFT|DT_NOCLIP|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER, NULL);
}



static int InfoDlg_Resize(HWND hwndDlg,LPARAM lParam,UTILRESIZECONTROL *urc)
{
    switch (urc->wId)
    {
    case IDC_LIST:
        return RD_ANCHORX_WIDTH | RD_ANCHORY_HEIGHT;
        break;
    case IDC_SAVE:
        return RD_ANCHORX_RIGHT | RD_ANCHORY_BOTTOM;
        break;
    case IDC_UPLOADING:
        return RD_ANCHORX_WIDTH | RD_ANCHORY_BOTTOM;
        break;
    }
    return RD_ANCHORX_LEFT | RD_ANCHORY_TOP; // default
}




INT_PTR CALLBACK ChangeInfoDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    hwndList=GetDlgItem(hwndDlg,IDC_LIST);

    switch(msg)
    {
    case WM_INITDIALOG:
        ICQTranslateDialog(hwndDlg);



        ListView_SetExtendedListViewStyle(hwndList, LVS_EX_FULLROWSELECT);
        iEditItem = -1;
        {
            HFONT hFont;
            LOGFONT lf;

            hListFont = (HFONT)SendMessage(hwndList, WM_GETFONT, 0, 0);
            GetObject(hListFont, sizeof(lf), &lf);
            lf.lfHeight -= 5;
            hFont = CreateFontIndirect(&lf);
            SendMessage(hwndList, WM_SETFONT, (WPARAM)hFont, 0);
        }
        {
            // Prepare ListView Columns
            LV_COLUMN lvc = {0};
            RECT rc;

            GetClientRect(hwndList, &rc);
            rc.right -= GetSystemMetrics(SM_CXVSCROLL);
            lvc.mask = LVCF_WIDTH;
            lvc.cx = rc.right / 3;
            ListView_InsertColumn(hwndList, 0, &lvc);
            lvc.cx = rc.right - lvc.cx;
            ListView_InsertColumn(hwndList, 1, &lvc);
        }
        {
            // Prepare Setting Items
            LV_ITEM lvi = {0};
            lvi.mask = LVIF_PARAM | LVIF_TEXT;

            for (lvi.iItem=0; lvi.iItem<settingCount; lvi.iItem++)
            {
                TCHAR text[MAX_PATH];

                lvi.lParam = lvi.iItem;
                lvi.pszText = text;
                {
                    TCHAR *tmp = mir_utf8decodeW(setting[lvi.iItem].szDescription);
                    wcsncpy(text, tmp, SIZEOF(text));
                    mir_free(tmp);
                }
                ListView_InsertItem(hwndList, &lvi);
            }
        }

        SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
        return TRUE;

    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->idFrom)
        {
        case 0:
            switch (((LPNMHDR)lParam)->code)
            {
            case PSN_PARAMCHANGED:
                LoadSettingsFromDb(0);
                {
                    char *pwd = GetUserPassword(TRUE);
                    strcpy(Password, (pwd) ? pwd : "" ); /// FIXME
                }
                break;

            case PSN_INFOCHANGED:
                LoadSettingsFromDb(1);
                break;

            case PSN_KILLACTIVE:
                EndStringEdit(1);
                EndListEdit(1);
                break;

            case PSN_APPLY:
                if (ChangesMade())
                {
                    if (IDYES!=MessageBoxUtf(hwndDlg, LPGEN("You've made some changes to your ICQ details but it has not been saved to the server. Are you sure you want to close this dialog?"), LPGEN("Change ICQ Details"), MB_YESNOCANCEL))
                    {
                        SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
                        return TRUE;
                    }
                }
                break;
            }
            break;

        case IDC_LIST:
            switch (((LPNMHDR)lParam)->code)
            {
            case LVN_GETDISPINFO:
                if (iEditItem != -1)
                {
                    if (editTopIndex != ListView_GetTopIndex(hwndList))
                    {
                        EndStringEdit(1);
                        EndListEdit(1);
                    }
                }
                break;

            case NM_CUSTOMDRAW:
            {
                LPNMLVCUSTOMDRAW cd=(LPNMLVCUSTOMDRAW)lParam;

                switch(cd->nmcd.dwDrawStage)
                {
                case CDDS_PREPAINT:
                    SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
                    return TRUE;

                case CDDS_ITEMPREPAINT:
                {
                    RECT rcItem;
                    HFONT hoFont;

                    if (iEditItem != -1)
                    {
                        if (editTopIndex != ListView_GetTopIndex(hwndList))
                        {
                            EndStringEdit(1);
                            EndListEdit(1);
                        }
                    }

                    ListView_GetItemRect(hwndList, cd->nmcd.dwItemSpec, &rcItem, LVIR_BOUNDS);

                    if (GetWindowLong(hwndList, GWL_STYLE) & WS_DISABLED)
                    {
                        // Disabled List
                        SetTextColor(cd->nmcd.hdc, cd->clrText);
                        FillRect(cd->nmcd.hdc, &rcItem, GetSysColorBrush(COLOR_3DFACE));
                    }
                    else if ((cd->nmcd.uItemState & CDIS_SELECTED || iEditItem == (int)cd->nmcd.dwItemSpec)
                             && setting[cd->nmcd.lItemlParam].displayType != LI_DIVIDER)
                    {
                        // Selected item
                        SetTextColor(cd->nmcd.hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
                        FillRect(cd->nmcd.hdc, &rcItem, GetSysColorBrush(COLOR_HIGHLIGHT));
                    }
                    else
                    {
                        // Unselected item
                        SetTextColor(cd->nmcd.hdc, GetSysColor(COLOR_WINDOWTEXT));
                        FillRect(cd->nmcd.hdc, &rcItem, GetSysColorBrush(COLOR_WINDOW));
                    }

                    hoFont = (HFONT)SelectObject(cd->nmcd.hdc, hListFont);

                    if (setting[cd->nmcd.lItemlParam].displayType == LI_DIVIDER)
                    {
                        RECT rcLine;
                        SIZE textSize;
                        char str[MAX_PATH];
                        char *szText = ICQTranslateUtfStatic(setting[cd->nmcd.lItemlParam].szDescription, str, MAX_PATH);

                        SetTextColor(cd->nmcd.hdc, GetSysColor(COLOR_3DSHADOW));
                        DrawTextUtf(cd->nmcd.hdc, szText, &rcItem, DT_CENTER|DT_NOCLIP|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER, &textSize);
                        rcLine.top = (rcItem.top + rcItem.bottom) / 2 - 1;
                        rcLine.bottom = rcLine.top + 2;
                        rcLine.left = rcItem.left + 3;
                        rcLine.right = (rcItem.left + rcItem.right - textSize.cx) / 2 - 3;
                        DrawEdge(cd->nmcd.hdc, &rcLine, BDR_SUNKENOUTER, BF_RECT);
                        rcLine.left = (rcItem.left + rcItem.right + textSize.cx) / 2 + 3;
                        rcLine.right = rcItem.right - 3;
                        DrawEdge(cd->nmcd.hdc, &rcLine, BDR_SUNKENOUTER, BF_RECT);
                    }
                    else
                    {
                        RECT rcItemDescr, rcItemValue;
                        char str[MAX_PATH];

                        ListView_GetSubItemRect(hwndList, cd->nmcd.dwItemSpec, 0, LVIR_BOUNDS, &rcItemDescr);
                        ListView_GetSubItemRect(hwndList, cd->nmcd.dwItemSpec, 1, LVIR_BOUNDS, &rcItemValue);

                        rcItemDescr.right = rcItemValue.left;
                        rcItemDescr.left += 2;
                        DrawTextUtf(cd->nmcd.hdc, ICQTranslateUtfStatic(setting[cd->nmcd.lItemlParam].szDescription, str, MAX_PATH), &rcItemDescr, DT_END_ELLIPSIS|DT_LEFT|DT_NOCLIP|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER, NULL);

                        PaintItemSetting(cd->nmcd.hdc, &rcItemValue, cd->nmcd.lItemlParam, cd->nmcd.uItemState);
                    }
                    SelectObject(cd->nmcd.hdc, hoFont);

                    SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, CDRF_SKIPDEFAULT);

                    return TRUE;
                }
                }
                break;
            }
            case NM_CLICK:
            {
                LPNMLISTVIEW nm=(LPNMLISTVIEW)lParam;
                LV_ITEM lvi;
                RECT rc;

                EndStringEdit(1);
                EndListEdit(1);
                if (nm->iSubItem != 1) break;
                lvi.mask = LVIF_PARAM|LVIF_STATE;
                lvi.stateMask = 0xFFFFFFFF;
                lvi.iItem = nm->iItem;
                lvi.iSubItem = nm->iSubItem;
                ListView_GetItem(hwndList, &lvi);
                if (!(lvi.state & LVIS_SELECTED)) break;
                ListView_EnsureVisible(hwndList, lvi.iItem, FALSE);
                ListView_GetSubItemRect(hwndList, lvi.iItem, lvi.iSubItem, LVIR_BOUNDS, &rc);
                editTopIndex = ListView_GetTopIndex(hwndList);
                switch (setting[lvi.lParam].displayType & LIM_TYPE)
                {
                case LI_STRING:
                case LI_LONGSTRING:
                case LI_NUMBER:
                    BeginStringEdit(nm->iItem, &rc, lvi. lParam, 0);
                    break;
                case LI_LIST:
                    BeginListEdit(nm->iItem, &rc, lvi. lParam, 0);
                    break;
                }
                break;
            }
            case LVN_KEYDOWN:
            {
                LPNMLVKEYDOWN nm=(LPNMLVKEYDOWN)lParam;
                LV_ITEM lvi;
                RECT rc;

                EndStringEdit(1);
                EndListEdit(1);
                if(nm->wVKey==VK_SPACE || nm->wVKey==VK_RETURN || nm->wVKey==VK_F2) nm->wVKey=0;
                if(nm->wVKey && (nm->wVKey<'0' || (nm->wVKey>'9' && nm->wVKey<'A') || (nm->wVKey>'Z' && nm->wVKey<VK_NUMPAD0) || nm->wVKey>=VK_F1))
                    break;
                lvi.mask=LVIF_PARAM|LVIF_STATE;
                lvi.stateMask=0xFFFFFFFF;
                lvi.iItem = ListView_GetNextItem(hwndList, -1, LVNI_ALL|LVNI_SELECTED);
                if (lvi.iItem==-1) break;
                lvi.iSubItem=1;
                ListView_GetItem(hwndList,&lvi);
                ListView_EnsureVisible(hwndList,lvi.iItem,FALSE);
                ListView_GetSubItemRect(hwndList,lvi.iItem,lvi.iSubItem,LVIR_BOUNDS,&rc);
                editTopIndex = ListView_GetTopIndex(hwndList);
                switch(setting[lvi.lParam].displayType & LIM_TYPE)
                {
                case LI_STRING:
                case LI_LONGSTRING:
                case LI_NUMBER:
                    BeginStringEdit(lvi.iItem,&rc,lvi.lParam,nm->wVKey);
                    break;
                case LI_LIST:
                    BeginListEdit(lvi.iItem,&rc,lvi.lParam,nm->wVKey);
                    break;
                }
                SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, TRUE);
                return TRUE;
            }
            case NM_KILLFOCUS:
                if(!IsStringEditWindow(GetFocus())) EndStringEdit(1);
                if(!IsListEditWindow(GetFocus())) EndListEdit(1);
                break;
            }
            break;
        }
        break;
    case WM_KILLFOCUS:
        EndStringEdit(1);
        EndListEdit(1);
        break;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDCANCEL:
            SendMessage(GetParent(hwndDlg), msg, wParam, lParam);
            break;

        case IDC_SAVE:
            if (!SaveSettingsToDb(hwndDlg))
                break;

            EnableDlgItem(hwndDlg, IDC_SAVE, FALSE);
            EnableDlgItem(hwndDlg, IDC_LIST, FALSE);
            {
                char str[MAX_PATH];
                SetDlgItemTextUtf(hwndDlg, IDC_UPLOADING, ICQTranslateUtfStatic(LPGEN("Upload in progress..."), str, MAX_PATH));
            }
            EnableDlgItem(hwndDlg, IDC_UPLOADING, TRUE);
            ShowDlgItem(hwndDlg, IDC_UPLOADING, SW_SHOW);
            hAckHook = HookEventMessage(ME_PROTO_ACK, hwndDlg, DM_PROTOACK);

            if (!UploadSettings(hwndDlg))
            {
                EnableDlgItem(hwndDlg, IDC_SAVE, TRUE);
                EnableDlgItem(hwndDlg, IDC_LIST, TRUE);
                ShowDlgItem(hwndDlg, IDC_UPLOADING, SW_HIDE);
                UnhookEvent(hAckHook);
                hAckHook = NULL;
            }
            break;
        }
        break;

    case WM_SIZE:
    {
        // make the dlg resizeable
        UTILRESIZEDIALOG urd = {0};

        if (IsIconic(hwndDlg)) break;
        urd.cbSize = sizeof(urd);
        urd.hInstance = hInst;
        urd.hwndDlg = hwndDlg;
        urd.lParam = 0; // user-defined
        urd.lpTemplate = MAKEINTRESOURCEA(IDD_INFO_CHANGEINFO);
        urd.pfnResizer = InfoDlg_Resize;
        CallService(MS_UTILS_RESIZEDIALOG, 0, (LPARAM) &urd);

        {
            // update listview column widths
            RECT rc;

            GetClientRect(hwndList, &rc);
            rc.right -= GetSystemMetrics(SM_CXVSCROLL);
            ListView_SetColumnWidth(hwndList, 0, rc.right / 3);
            ListView_SetColumnWidth(hwndList, 1, rc.right - rc.right / 3);
        }
        break;
    }

    case DM_PROTOACK:
    {
        ACKDATA *ack=(ACKDATA*)lParam;
        int i,done;
        char str[MAX_PATH];
        char buf[MAX_PATH];

        if (ack->type != ACKTYPE_SETINFO) break;
        if (ack->result == ACKRESULT_SUCCESS)
        {
            for (i=0; i < SIZEOF(hUpload); i++)
                if (hUpload[i] && ack->hProcess == hUpload[i]) break;

            if (i == SIZEOF(hUpload)) break;
            hUpload[i] = NULL;
            for (done = 0, i = 0; i < SIZEOF(hUpload); i++)
                done += hUpload[i] == NULL;
            null_snprintf(buf, sizeof(buf), "%s%d%%", ICQTranslateUtfStatic(LPGEN("Upload in progress..."), str, MAX_PATH), 100*done/(SIZEOF(hUpload)));
            SetDlgItemTextUtf(hwndDlg, IDC_UPLOADING, buf);
            if (done < SIZEOF(hUpload)) break;

            ClearChangeFlags();
            UnhookEvent(hAckHook);
            hAckHook = NULL;
            EnableDlgItem(hwndDlg, IDC_LIST, TRUE);
            EnableDlgItem(hwndDlg, IDC_UPLOADING, FALSE);
            SetDlgItemTextUtf(hwndDlg, IDC_UPLOADING, ICQTranslateUtfStatic(LPGEN("Upload complete"), str, MAX_PATH));
            SendMessage(GetParent(hwndDlg), PSM_FORCECHANGED, 0, 0);
        }
        else if (ack->result==ACKRESULT_FAILED)
        {
            UnhookEvent(hAckHook);
            hAckHook = NULL;
            EnableDlgItem(hwndDlg, IDC_LIST, TRUE);
            EnableDlgItem(hwndDlg, IDC_UPLOADING, FALSE);
            SetDlgItemTextUtf(hwndDlg, IDC_UPLOADING, ICQTranslateUtfStatic(LPGEN("Upload FAILED"), str, MAX_PATH));
            SendMessage(GetParent(hwndDlg), PSM_FORCECHANGED, 0, 0);
            EnableDlgItem(hwndDlg, IDC_SAVE, TRUE);
        }
        break;
    }
    case WM_DESTROY:
        if (hAckHook)
        {
            UnhookEvent(hAckHook);
            hAckHook = NULL;
        }
        {
            HFONT hFont = (HFONT)SendMessage(hwndList, WM_GETFONT, 0, 0);
            DeleteObject(hFont);
        }
        FreeStoredDbSettings();
        SetWindowLongPtr(hwndDlg, GWLP_USERDATA, 0);
        break;
    }
    return FALSE;
}


char* GetItemSettingText(int i, char *buf, size_t bufsize)
{
    char *text = buf;
    int alloced = 0;

    buf[0] = '\0';

    if (setting[i].value == 0 && !(setting[i].displayType & LIF_ZEROISVALID))
    {
        if (setting[i].displayType & LIF_CHANGEONLY)
            text = ICQTranslateUtfStatic(LPGEN("<unremovable once applied>"), buf, bufsize);
        else
            text = ICQTranslateUtfStatic(LPGEN("<empty>"), buf, bufsize);
    }
    else
    {
        switch (setting[i].displayType & LIM_TYPE)
        {
        case LI_STRING:
        case LI_LONGSTRING:
            text = BinaryToEscapes((char*)setting[i].value);
            alloced = 1;
            break;

        case LI_NUMBER:
            _itoa(setting[i].value, text, 10);
            break;

        case LI_LIST:
            if (setting[i].dbType == DBVT_ASCIIZ)
                text = ICQTranslateUtfStatic((char*)setting[i].value, buf, bufsize);
            else
            {
                FieldNamesItem *list = (FieldNamesItem*)setting[i].pList;
                int j;
                text = ICQTranslateUtfStatic(LPGEN("Unknown value"), buf, bufsize);

                for (j=0; TRUE; j++)
                    if (list[j].code == setting[i].value)
                    {
                        text = ICQTranslateUtfStatic(list[j].text, buf, bufsize);
                        break;
                    }
                    else if (!list[j].text)
                    {
                        if (list[j].code == setting[i].value)
                            text = ICQTranslateUtfStatic("Unspecified", buf, bufsize);
                        break;
                    }
            }
            break;
        }
    }
    if (setting[i].displayType & LIF_PASSWORD)
    {
        if (setting[i].changed)
        {
            int j;
            for (j=0; text[j]; j++) text[j] = '*';
        }
        else
        {
            if (alloced)
            {
                mir_free(text);
                alloced = 0;
            }
            text = "********";
        }
    }
    if (text != buf)
    {
        char *tmp = text;

        text = null_strcpy(buf, text, bufsize - 1);
        if (alloced)
            mir_free(tmp);
    }

    return text;
}

HFONT hListFont;
HWND hwndList;