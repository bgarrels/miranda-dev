/*
Crash Dumper plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2008 - 2012 Boris Krasnovskiy All Rights Reserved

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

#include "utils.h"
#include <commctrl.h>
#include <richedit.h>
#include <m_popup.h>

HWND hViewWnd;
extern HINSTANCE hInst;


HDWP MyResizeWindow (HDWP hDwp, HWND hwndDlg, HWND hwndCtrl, int nHorizontalOffset, int nVerticalOffset, 
					 int nWidthOffset, int nHeightOffset) 
{
	POINT pt;
	RECT rcinit;

	// get current bounding rectangle
	GetWindowRect(hwndCtrl, &rcinit);

	// get current top left point
	pt.x = rcinit.left;
	pt.y = rcinit.top;
	ScreenToClient(hwndDlg, &pt);

	return DeferWindowPos(hDwp, hwndCtrl, NULL,
		pt.x + nHorizontalOffset, 
		pt.y + nVerticalOffset,
		rcinit.right - rcinit.left + nWidthOffset, 
		rcinit.bottom - rcinit.top + nHeightOffset,
		SWP_NOZORDER);
}

BOOL MyResizeGetOffset(HWND hwndCtrl, int nWidth, int nHeight, int* nDx, int* nDy) 
{
	RECT rcinit;

	// get current bounding rectangle
	GetWindowRect(hwndCtrl, &rcinit);

	// calculate offsets
	*nDx = nWidth - (rcinit.right - rcinit.left);
	*nDy = nHeight - (rcinit.bottom - rcinit.top);

	return rcinit.bottom != rcinit.top && nHeight > 0;
}

INT_PTR CALLBACK DlgProcView(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	switch (msg) 
	{
	case WM_INITDIALOG:
		if (hViewWnd == NULL)
		{
			hViewWnd = hwndDlg;
			TranslateDialogDefault(hwndDlg);
			SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIconEx("versionInfo", true));
			SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIconEx("versionInfo"));

			CHARFORMAT2 chf;
			chf.cbSize = sizeof(chf);
			SendDlgItemMessage(hwndDlg, IDC_VIEWVERSIONINFO, EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&chf);
			_tcscpy(chf.szFaceName, TEXT("Courier New"));
			SendDlgItemMessage(hwndDlg, IDC_VIEWVERSIONINFO, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&chf);

			bkstring buffer;
			buffer.reserve(0x1800);
			PrintVersionInfo(buffer, (unsigned int)lParam);
			SetDlgItemText(hwndDlg, IDC_VIEWVERSIONINFO, buffer.c_str());
			SetWindowLongPtr(hwndDlg, GWLP_USERDATA, lParam);

			Utils_RestoreWindowPositionNoMove(hwndDlg, NULL, PluginName, "ViewInfo_");
			ShowWindow(hwndDlg, SW_SHOW);
		}
		else
			DestroyWindow(hwndDlg);
		break;

	case WM_SIZE: 
		{
			int dx, dy, bsz;
			HDWP hDwp;
			RECT rc;

			GetWindowRect(GetDlgItem(hwndDlg, IDC_FILEVER), &rc);
			bsz = rc.bottom - rc.top;

			if (MyResizeGetOffset(GetDlgItem(hwndDlg, IDC_VIEWVERSIONINFO), 
				LOWORD(lParam)-20, HIWORD(lParam)-30-bsz, &dx, &dy))
			{
				hDwp = BeginDeferWindowPos(4);
				hDwp = MyResizeWindow(hDwp, hwndDlg, GetDlgItem(hwndDlg, IDC_FILEVER), 0, dy, 0, 0);
				hDwp = MyResizeWindow(hDwp, hwndDlg, GetDlgItem(hwndDlg, IDC_CLIPVER), dx/2, dy, 0, 0);
				hDwp = MyResizeWindow(hDwp, hwndDlg, GetDlgItem(hwndDlg, IDCANCEL), dx, dy, 0, 0);
				hDwp = MyResizeWindow(hDwp, hwndDlg, GetDlgItem(hwndDlg, IDC_VIEWVERSIONINFO), 0, 0, dx, dy);
				EndDeferWindowPos(hDwp);
			}
		}				
		break;

	case WM_GETMINMAXINFO: 
		{
			LPMINMAXINFO mmi = (LPMINMAXINFO)lParam;

			// The minimum width in points
			mmi->ptMinTrackSize.x = 350;
			// The minimum height in points
			mmi->ptMinTrackSize.y = 300;
		}
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case IDC_CLIPVER:
			CallService(MS_CRASHDUMPER_STORETOCLIP, 0, GetWindowLongPtr(hwndDlg, GWLP_USERDATA));
			break;

		case IDC_FILEVER:
			CallService(MS_CRASHDUMPER_STORETOFILE, 0, GetWindowLongPtr(hwndDlg, GWLP_USERDATA));
			break;

		case IDCANCEL:
			DestroyWindow(hwndDlg);
			break;
		}
		break;

	case WM_CONTEXTMENU:
		{
			HWND hView = GetDlgItem(hwndDlg, IDC_VIEWVERSIONINFO);
			RECT rc;
			GetWindowRect(hView, &rc);

			POINT pt;
			pt.x = LOWORD(lParam); 
			pt.y = HIWORD(lParam); 
			if (PtInRect(&rc, pt))
			{
				static const CHARRANGE all = { 0, -1 };

				HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_CONTEXT));
				HMENU hSubMenu = GetSubMenu(hMenu, 0);
				CallService(MS_LANGPACK_TRANSLATEMENU, (WPARAM) hSubMenu, 0);

				CHARRANGE sel; 
				SendMessage(hView, EM_EXGETSEL, 0, (LPARAM)&sel);
				if (sel.cpMin == sel.cpMax)
					EnableMenuItem(hSubMenu, IDM_COPY, MF_BYCOMMAND | MF_GRAYED);

				switch (TrackPopupMenu(hSubMenu, TPM_RETURNCMD, pt.x, pt.y, 0, hwndDlg, NULL)) 
				{
				case IDM_COPY:
					SendMessage(hView, WM_COPY, 0, 0);
					break;

				case IDM_COPYALL:
					SendMessage(hView, EM_EXSETSEL, 0, (LPARAM)&all);
					SendMessage(hView, WM_COPY, 0, 0);
					SendMessage(hView, EM_EXSETSEL, 0, (LPARAM)&sel);
					break;

				case IDM_SELECTALL:
					SendMessage(hView, EM_EXSETSEL, 0, (LPARAM)&all);
					break;
				}
				DestroyMenu(hMenu);
			}
		}
		break;

	case WM_DESTROY: 
		hViewWnd = NULL;
		ReleaseIconEx((HICON)SendMessage(hwndDlg, WM_SETICON, ICON_BIG, 0));
		ReleaseIconEx((HICON)SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, 0));
		Utils_SaveWindowPosition(hwndDlg, NULL, PluginName, "ViewInfo_");
		if (servicemode) PostQuitMessage(0); 
		break;
	}
	return FALSE;
}


void DestroyAllWindows(void)
{
	if (hViewWnd != NULL) DestroyWindow(hViewWnd);
	hViewWnd = NULL;
}


INT_PTR CALLBACK DlgProcOptions(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	switch (msg) 
	{
	case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);

			DBVARIANT dbv;
			if (DBGetContactSettingString(NULL, PluginName, "Username", &dbv) == 0)
			{
				SetDlgItemTextA(hwndDlg, IDC_USERNAME, dbv.pszVal);
				DBFreeVariant(&dbv);
			}
			if (DBGetContactSettingString(NULL, PluginName, "Password", &dbv) == 0)
			{
				CallService(MS_DB_CRYPT_DECODESTRING, strlen(dbv.pszVal)+1, (LPARAM)dbv.pszVal);
				SetDlgItemTextA(hwndDlg, IDC_PASSWORD, dbv.pszVal);
				DBFreeVariant(&dbv);
			}
			CheckDlgButton(hwndDlg, IDC_UPLOADCHN, DBGetContactSettingByte(NULL, PluginName, "UploadChanged", 0));
			CheckDlgButton(hwndDlg, IDC_CLASSICDATES, clsdates);
		}
		break;

	case WM_COMMAND:
		if ((HIWORD(wParam) == EN_CHANGE || HIWORD(wParam) == BN_CLICKED) && (HWND)lParam == GetFocus()) 
			SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
		break;

	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->code == (unsigned)PSN_APPLY)
		{
			char szSetting[100];
			GetDlgItemTextA(hwndDlg, IDC_USERNAME, szSetting, SIZEOF(szSetting));
			DBWriteContactSettingString(NULL, PluginName, "Username", szSetting);

			GetDlgItemTextA(hwndDlg, IDC_PASSWORD, szSetting, SIZEOF(szSetting));
			CallService(MS_DB_CRYPT_ENCODESTRING, SIZEOF(szSetting), (LPARAM)szSetting);
			DBWriteContactSettingString(NULL, PluginName, "Password", szSetting);

			DBWriteContactSettingByte(NULL, PluginName, "UploadChanged", 
				(BYTE)IsDlgButtonChecked(hwndDlg, IDC_UPLOADCHN));

			clsdates = IsDlgButtonChecked(hwndDlg, IDC_CLASSICDATES) == BST_CHECKED;
			if (clsdates)
				DBWriteContactSettingByte(NULL, PluginName, "ClassicDates", 1);
			else
				DBDeleteContactSetting(NULL, PluginName, "ClassicDates");
		}
		break;
	}
	return FALSE;
}


LRESULT CALLBACK DlgProcPopup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CONTEXTMENU:
		PUDeletePopUp(hWnd);
		break;

	case WM_COMMAND:
		switch ((int)PUGetPluginData(hWnd))
		{
		case 0:
			OpenAuthUrl("http://www.miranda-vi.org/");
			break;

		case 1:
			OpenAuthUrl("http://%s.miranda-vi.org/global");
			break;

		case 3:
			TCHAR path[MAX_PATH];
			crs_sntprintf(path, MAX_PATH, TEXT("%s\\VersionInfo.txt"), VersionInfoFolder);
			ShellExecute(NULL, TEXT("open"), path, NULL, NULL, SW_SHOW);
			break;

		}
		PUDeletePopUp(hWnd);
		break;

	case UM_FREEPLUGINDATA:
		ReleaseIconEx("versionInfo");
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void ShowMessage(int type, const TCHAR* format, ...)
{
	POPUPDATAT pi = {0};

	va_list va;
	va_start(va, format);
	int len = _vsntprintf(pi.lptzText, SIZEOF(pi.lptzText)-1, format, va);
	pi.lptzText[len] = 0;
	va_end(va);

	if (ServiceExists(MS_POPUP_ADDPOPUPT))
	{
		_tcscpy(pi.lptzContactName, TEXT(PluginName));
		pi.lchIcon = LoadIconEx("versionInfo");
		pi.PluginWindowProc = DlgProcPopup;
		pi.PluginData = (void*)type;

		PUAddPopUpT(&pi);
	}
	else
		MessageBox(NULL, pi.lptzText, TEXT(PluginName), MB_OK | MB_ICONINFORMATION);
}
