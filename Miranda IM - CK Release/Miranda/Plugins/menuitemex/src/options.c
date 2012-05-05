/*
MenuEx plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
				Copyright (C) 2001-03 Heiko Schillinger
				Copyright (C) 2006-08 Baloo
				Copyright (C) 2009-10 Billy_Bons

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
*/

#include "../include/menuex.h"


extern HINSTANCE hinstance;

struct {
	int idc;
	int flag;
}
static const checkboxes[]={
	{	IDC_COPYID,				VF_CID		},
	{	IDC_STATUSMSG,			VF_STAT		},
	{	IDC_COPYIP,				VF_CIP		},
	{	IDC_COPYMIRVER,			VF_CMV		},
	{	IDC_VIS,				VF_VS		},
	{	IDC_SHOWALPHAICONS,		VF_SAI		},
	{	IDC_HIDE,				VF_HFL		},
	{	IDC_IGNORE,				VF_IGN		},
	{	IDC_PROTOS,				VF_PROTO	},
	{	IDC_ADDED,				VF_ADD		},
	{	IDC_AUTHREQ,			VF_REQ		},
	{	IDC_SHOWID,				VF_SHOWID	},
	{	IDC_COPYIDNAME,			VF_CIDN		},
	{	IDC_RECVFILES,			VF_RECV		},
	{	IDC_SMNAME,				VF_SMNAME	},
	{	IDC_TRIMID,				VF_TRIMID	}
};

INT_PTR CALLBACK OptionsProc(HWND hdlg,UINT msg,WPARAM wparam,LPARAM lparam)
{
	WORD flags=DBGetContactSettingWord(NULL,VISPLG,"flags",vf_default);
	TCHAR buffer[64] = {0};
	int i;
	
	switch(msg)
	{
		case WM_INITDIALOG:

			TranslateDialogDefault(hdlg);

			for (i = 0; i < SIZEOF(checkboxes); i++)
			{
				CheckDlgButton(hdlg, checkboxes[i].idc, (flags & checkboxes[i].flag) ? BST_CHECKED : BST_UNCHECKED);
			}

			if (ServiceExists(MS_POPUP_ADDPOPUP))
			{
				for (i = 0; i < 4; i++)
				{
					GetDlgItemText(hdlg, checkboxes[i].idc, buffer, SIZEOF(buffer) - 3);
					_tcscat(buffer, _T(" *"));
					SetDlgItemText(hdlg, checkboxes[i].idc, buffer);
				}
			}
			else
				ShowWindow(GetDlgItem(hdlg, IDC_HINT1), SW_HIDE);

			if (!bMir_08)
				EnableWindow(GetDlgItem(hdlg,IDC_PROTOS), 0);

			SendMessage(hdlg, WM_USER + 50, 0, 0);
			return 0;

		case WM_USER+50:

			EnableWindow(GetDlgItem(hdlg,IDC_SHOWALPHAICONS),
				IsDlgButtonChecked(hdlg,IDC_VIS) == BST_CHECKED);
			EnableWindow(GetDlgItem(hdlg,IDC_COPYIDNAME),
				IsDlgButtonChecked(hdlg,IDC_COPYID) == BST_CHECKED);
			EnableWindow(GetDlgItem(hdlg,IDC_SHOWID),
				IsDlgButtonChecked(hdlg,IDC_COPYID) == BST_CHECKED);
			EnableWindow(GetDlgItem(hdlg,IDC_TRIMID),
				(IsDlgButtonChecked(hdlg,IDC_COPYID) == BST_CHECKED && IsDlgButtonChecked(hdlg,IDC_SHOWID) == BST_CHECKED));
			EnableWindow(GetDlgItem(hdlg,IDC_SMNAME),
				IsDlgButtonChecked(hdlg,IDC_STATUSMSG) == BST_CHECKED);
			return 1;

		case WM_NOTIFY:
			switch(((LPNMHDR)lparam)->code){
				case PSN_APPLY:
				{
					WORD mod_flags=0;

					for (i = 0; i < SIZEOF(checkboxes); i++)
					{
						mod_flags |= IsDlgButtonChecked(hdlg, checkboxes[i].idc) ? checkboxes[i].flag : 0;
					}

					//DBDeleteContactSetting(NULL,VISPLG,"flags");
					DBWriteContactSettingWord(NULL,VISPLG,"flags",mod_flags);
					
					return 1;
				}
			}
			break;

		case WM_COMMAND:
			if(HIWORD(wparam)==BN_CLICKED && GetFocus()==(HWND)lparam) {
				SendMessage(GetParent(hdlg),PSM_CHANGED,0,0);
				if (LOWORD(wparam) == IDC_VIS ||
					LOWORD(wparam) == IDC_COPYID ||
					LOWORD(wparam) == IDC_STATUSMSG ||
					LOWORD(wparam) == IDC_SHOWID) {

					SendMessage(hdlg, WM_USER + 50, 0, 0);
				}
			}
			return 0;

		case WM_CLOSE:
			EndDialog(hdlg,0);
			return 0;
	}
	return 0;
}

int OptionsInit(WPARAM wparam,LPARAM lparam)
{
	OPTIONSDIALOGPAGE odp={0};

	odp.cbSize=sizeof(odp);
	odp.position=955000000;
	odp.hInstance=hinstance;
	odp.pszTemplate=MAKEINTRESOURCEA(IDD_OPTIONS);
	odp.ptszTitle= _T(MODULENAME);
	odp.pfnDlgProc=OptionsProc;
	odp.ptszGroup=_T("Customize");
	odp.flags=ODPF_BOLDGROUPS | ODPF_TCHAR;

	CallService(MS_OPT_ADDPAGE,wparam,(LPARAM)&odp);
	return 0;
}
