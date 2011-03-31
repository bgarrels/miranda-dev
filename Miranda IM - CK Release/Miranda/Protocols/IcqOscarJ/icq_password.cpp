// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
// 
// Copyright ? 2000-2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright ? 2001-2002 Jon Keating, Richard Hughes
// Copyright ? 2002-2004 Martin ?berg, Sam Kothari, Robert Rainwater
// Copyright ? 2004-2008 Joe Kucera, Bio
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
// File name      : $URL: icq_password.cpp $
// Revision       : $Revision: 0 $
// Last change on : $Date: 2009-12-21 19:52:36 +0300 $
// Last change by : $Author: Unsane $
//
// DESCRIPTION:
//
//  Ported from jabber
//
//  P.S. Sory for my bad English...
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"

static INT_PTR CALLBACK IcqChangePasswordDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

INT_PTR __cdecl CIcqProto::OnMenuHandleChangePassword(WPARAM, LPARAM)
{
	if(!icqOnline())
		MessageBox(NULL, TranslateT("You can change your password only when you are online"), TranslateT("You must be online"), MB_OK | MB_ICONSTOP);
	else
	{
		if (IsWindow(m_hwndIcqChangePassword))
			SetForegroundWindow(m_hwndIcqChangePassword);
		else
			m_hwndIcqChangePassword = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_CHANGEPASSWORD), NULL, IcqChangePasswordDlgProc, (LPARAM)this);
	}

	return 0;
}

static INT_PTR CALLBACK IcqChangePasswordDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CIcqProto* ppro = (CIcqProto*)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
	switch (msg)
	{
		case WM_INITDIALOG:
			ppro = (CIcqProto*)lParam;
			SetWindowLongPtr(hwndDlg, GWLP_USERDATA, (LONG_PTR)lParam);

			SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)hStaticIcons[ISI_CHANGE_PASSWORD]->Handle());
			TranslateDialogDefault(hwndDlg);
			if (ppro->icqOnline())
			{
				TCHAR text[128];
				mir_sntprintf(text, SIZEOF(text), _T("%s %d"), TranslateT("Set New Password for"), ppro->m_dwLocalUIN);
				SetWindowText(hwndDlg, text);
			}
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					if (ppro->icqOnline())
					{
						char oldPasswd[PASSWORDMAXLEN], newPasswd[PASSWORDMAXLEN], newPasswd2[PASSWORDMAXLEN];
						
						GetDlgItemTextA(hwndDlg, IDC_NEWPASSWD, newPasswd, SIZEOF(newPasswd));
						GetDlgItemTextA(hwndDlg, IDC_NEWPASSWD2, newPasswd2, SIZEOF(newPasswd2));
						
						if (strcmp(newPasswd, newPasswd2))
						{
							MessageBox(hwndDlg, TranslateT("New password does not match."), TranslateT("Change Password"), MB_OK | MB_ICONSTOP | MB_SETFOREGROUND);
							break;
						}

						char* password = ppro->GetUserPassword(TRUE);
						GetDlgItemTextA(hwndDlg, IDC_OLDPASSWD, oldPasswd, SIZEOF(oldPasswd));

						if (strcmp(oldPasswd, password))
						{
							MessageBox(hwndDlg, TranslateT("Current password is incorrect."), TranslateT("Change Password"), MB_OK | MB_ICONSTOP | MB_SETFOREGROUND);
							break;
						}
						int nLen  = strlennull(newPasswd);
						if (nLen)
						{
							if (nLen > 8 || nLen < 1)
							{
								MessageBox(hwndDlg, TranslateT("The ICQ server does not support passwords longer than 8 characters. Please use a shorter password."), TranslateT("Change Password"), MB_OK | MB_ICONSTOP | MB_SETFOREGROUND);
								break;
							}

							ppro->icq_changeUserPasswordServ(newPasswd);
							if (ppro->m_bRememberPwd)
							{
								strcpy(ppro->m_szPassword, newPasswd);
								
								char ptmp[PASSWORDMAXLEN];
								if (ppro->GetUserStoredPassword(ptmp, sizeof(ptmp)))
								{
									strcpy(ptmp, newPasswd);
									CallService(MS_DB_CRYPT_ENCODESTRING, sizeof(ptmp), (LPARAM)ptmp);
									ppro->setSettingString(NULL, "Password", ptmp);
								}
							}
							MessageBox(hwndDlg, TranslateT("Password successfully changed."), TranslateT("Change Password"), MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND);
						}
					}
					DestroyWindow(hwndDlg);
					break;

				case IDCANCEL:
					DestroyWindow(hwndDlg);
					break;
			}
			break;

		case WM_CLOSE:
			DestroyWindow(hwndDlg);
			break;

		case WM_DESTROY:
			ppro->m_hwndIcqChangePassword = NULL;
			break;
	}

	return FALSE;
}
