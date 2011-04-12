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
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#include "../icqoscar.h"


INT_PTR CALLBACK LoginPasswdDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);


void RequestPassword()
{
    DialogBoxUtf(TRUE, hInst, MAKEINTRESOURCEA(IDD_LOGINPW), NULL, LoginPasswdDlgProc, 0);
}


INT_PTR CALLBACK LoginPasswdDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        char pszUIN[MAX_PATH];
        char str[MAX_PATH];
        DWORD dwUin;

        InitIconLib();

        ICQTranslateDialog(hwndDlg);
        SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)IconLibGetIcon("proto"));
        dwUin = ICQGetContactSettingUIN(NULL);
        null_snprintf(pszUIN, 128, ICQTranslateUtfStatic("Enter a password for UIN %u:", str, MAX_PATH), dwUin);
        SetDlgItemTextUtf(hwndDlg, IDC_INSTRUCTION, pszUIN);

        SendDlgItemMessage(hwndDlg, IDC_LOGINPW, EM_LIMITTEXT, 10, 0);

        CheckDlgButton(hwndDlg, IDC_SAVEPASS, ICQGetContactSettingByte(NULL, "RememberPass", 0));
    }
    break;

    case WM_CLOSE:

        EndDialog(hwndDlg, 0);
        break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            gbRememberPwd = (BYTE)IsDlgButtonChecked(hwndDlg, IDC_SAVEPASS);
            ICQWriteContactSettingByte(NULL, "RememberPass", gbRememberPwd);

            GetDlgItemTextA(hwndDlg, IDC_LOGINPW, gpszPassword, sizeof(gpszPassword));

            icq_login(gpszPassword);

            EndDialog(hwndDlg, IDOK);
        }
        break;

        case IDCANCEL:
        {
            SetCurrentStatus(ID_STATUS_OFFLINE);

            EndDialog(hwndDlg, IDCANCEL);
        }
        break;
        }
    }
    break;
    }

    return FALSE;
}
