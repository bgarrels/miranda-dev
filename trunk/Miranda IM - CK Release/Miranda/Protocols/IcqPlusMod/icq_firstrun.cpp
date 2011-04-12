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
// Revision       : $Revision: 36 $
// Last change on : $Date: 2007-08-05 03:45:10 +0300 (Ð’Ñ, 05 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"

INT_PTR CALLBACK DlgProcAccMgrUI(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK icq_FirstRunDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
extern DWORD _mirandaVersion;

void icq_FirstRunCheck()
{
    if (getSettingByte(NULL, "FirstRun", 0))
        return;
    DialogBoxUtf(TRUE, hInst, MAKEINTRESOURCEA(IDD_ICQACCOUNT), NULL, icq_FirstRunDlgProc, 0);
    return;
}

INT_PTR CALLBACK icq_FirstRunDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {

    case WM_INITDIALOG:
    {
        char* pszPwd;
        DWORD dwUIN;
        char pszUIN[20];

        InitIconLib();

        ICQTranslateDialog(hwndDlg);
        SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM) IconLibGetIcon("proto"));

        dwUIN = getContactUin(NULL);

        if (dwUIN)
        {
            null_snprintf(pszUIN, 20, "%u", dwUIN);
            SetDlgItemTextA(hwndDlg, IDC_UIN, pszUIN);
        }

        SendDlgItemMessage(hwndDlg, IDC_PW, EM_LIMITTEXT, 8, 0);
        pszPwd = GetUserPassword(FALSE);
        if (pszPwd)
        {
            SetDlgItemTextA(hwndDlg, IDC_PW, pszPwd);
        }
    }
    break;

    case WM_CLOSE:
        EndDialog(hwndDlg, 0);
        break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {

        case IDC_REG:
        {
            ShowRegUinDialog();
            break;
        }

        case IDOK:
        {
            char str[128];
            DWORD dwUIN;

            GetDlgItemTextA(hwndDlg, IDC_UIN, str, sizeof(str));
            dwUIN = atoi(str);
            setSettingDword(NULL, UNIQUEIDSETTING, dwUIN);
            GetDlgItemTextA(hwndDlg, IDC_PW, str, 9);
            strcpy(gpszPassword, str);
            CallService(MS_DB_CRYPT_ENCODESTRING, sizeof(gpszPassword), (LPARAM) str);
            setSettingString(NULL, "Password", str);
        }
        // fall through

        case IDCANCEL:
        {
            // Mark first run as completed
            setSettingByte(NULL, "FirstRun", 1);
            EndDialog(hwndDlg, IDCANCEL);
        }
        break;

        }
    }
    break;
    }

    return FALSE;
}

INT_PTR SvcCreateAccMgrUI(WPARAM wParam, LPARAM lParam)
{
    return (int)CreateDialogUtf(hInst, MAKEINTRESOURCEA(IDD_ACCMGRUI), (HWND)lParam, DlgProcAccMgrUI);
}

INT_PTR CALLBACK DlgProcAccMgrUI(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        char* pszPwd;
        DWORD dwUIN;
        char pszUIN[20];

        ICQTranslateDialog(hwndDlg);

        dwUIN = getContactUin(NULL);

        if (dwUIN)
        {
            null_snprintf(pszUIN, 20, "%u", dwUIN);
            SetDlgItemTextA(hwndDlg, IDC_UIN, pszUIN);
        }
        SendDlgItemMessage(hwndDlg, IDC_PW, EM_LIMITTEXT, 8, 0);
        pszPwd = GetUserPassword(FALSE);
        if (pszPwd)
        {
            SetDlgItemTextA(hwndDlg, IDC_PW, pszPwd);
        }
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_LOOKUPLINK:
            CallService(MS_UTILS_OPENURL, 1, (LPARAM)URL_FORGOT_PASSWORD);
            return TRUE;

        case IDC_NEWUINLINK:
            CallService(MS_UTILS_OPENURL, 1, (LPARAM)URL_REGISTER_UIN);
            return TRUE;


        case IDC_REGUIN:
            ShowRegUinDialog();
            return TRUE;
        }
    }
    break;
    case WM_NOTIFY:
    {
        switch (((LPNMHDR)lParam)->code)
        {

        case PSN_APPLY:
        {
            char str[128];
            DWORD dwUIN;
            GetDlgItemTextA(hwndDlg, IDC_UIN, str, sizeof(str));
            dwUIN = atoi(str);
            setSettingDword(NULL, UNIQUEIDSETTING, dwUIN);
            GetDlgItemTextA(hwndDlg, IDC_PW, str, 9);
            strcpy(gpszPassword, str);
            CallService(MS_DB_CRYPT_ENCODESTRING, sizeof(gpszPassword), (LPARAM)str);
            setSettingString(NULL, "Password", str);
            return TRUE;
        }
        }
    }
    break;
    }

    return FALSE;
}
