/*
ICQ plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright � 2001-2004 Richard Hughes
			Copyright � 2002-2004 Martin �berg, Sam Kothari, Robert Rainwater
			Copyright � 2004-2010 Joe Kucera, Bio
			Copyright � 2010-2012 Borkra, Georg Hazan

Copyright 2000-2012 Miranda IM project,
all portions of this codebase are copyrighted to the people
listed in contributors.txt.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

===============================================================================

File name      : $HeadURL: 
Revision       : $Revision: 
Last change on : $Date: 
Last change by : $Author:
$Id$		   : $Id$:

===============================================================================

* UI
*/

#include "../src/icqoscar.h"

struct AskAuthParam
{
	CIcqProto* ppro;
	HANDLE hContact;
};

static INT_PTR CALLBACK AskAuthProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	AskAuthParam* dat = (AskAuthParam*)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);

	switch (msg) {
	case WM_INITDIALOG:
		dat = (AskAuthParam*)lParam;
		if (!dat->hContact || !dat->ppro->icqOnline())
			EndDialog(hwndDlg, 0);

		TranslateDialogDefault(hwndDlg);
		SetWindowLongPtr(hwndDlg, GWLP_USERDATA, lParam);
		SendDlgItemMessage(hwndDlg, IDC_EDITAUTH, EM_LIMITTEXT, (WPARAM)255, 0);
		SetDlgItemText(hwndDlg, IDC_EDITAUTH, TranslateT("Please authorize me to add you to my contact list."));
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			if (dat->ppro->icqOnline())
			{
				DWORD dwUin;
				uid_str szUid;
				if ( dat->ppro->getContactUid(dat->hContact, &dwUin, &szUid))
					return TRUE; // Invalid contact

				char* szReason = GetDlgItemTextUtf(hwndDlg, IDC_EDITAUTH);
				dat->ppro->icq_sendAuthReqServ(dwUin, szUid, szReason);
				SAFE_FREE((void**)&szReason);

				// auth bug fix (thx Bio)
				if (dat->ppro->m_bSsiEnabled && dwUin)
					dat->ppro->resetServContactAuthState(dat->hContact, dwUin);

				EndDialog(hwndDlg, 0);
			}
			return TRUE;

		case IDCANCEL:
			EndDialog(hwndDlg, 0);
			return TRUE;
		}

		break;

	case WM_CLOSE:
		EndDialog(hwndDlg,0);
		return TRUE;
	}

	return FALSE;
}

INT_PTR CIcqProto::RequestAuthorization(WPARAM wParam, LPARAM lParam)
{
	AskAuthParam param = { this, (HANDLE)wParam };
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_ASKAUTH), NULL, AskAuthProc, (LPARAM)&param);
	return 0;
}
