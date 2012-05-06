#ifndef __OPTIONS_H__
#define __OPTIONS_H__

/*
SecureIM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 	2003 Johell
							2005-2009 Baloo

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

INT_PTR CALLBACK OptionsDlgProc(HWND,UINT,WPARAM,LPARAM);
INT_PTR CALLBACK DlgProcOptionsGeneral(HWND,UINT,WPARAM,LPARAM);
INT_PTR CALLBACK DlgProcOptionsProto(HWND,UINT,WPARAM,LPARAM);
INT_PTR CALLBACK DlgProcOptionsPGP(HWND,UINT,WPARAM,LPARAM);
INT_PTR CALLBACK DlgProcOptionsGPG(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK DlgProcSetPSK(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK DlgProcSetPassphrase(HWND,UINT,WPARAM,LPARAM);
void ApplyGeneralSettings(HWND);
void ApplyProtoSettings(HWND);
void ApplyPGPSettings(HWND);
void ApplyGPGSettings(HWND);
void RefreshGeneralDlg(HWND,BOOL);
void RefreshProtoDlg(HWND);
void RefreshPGPDlg(HWND,BOOL);
void RefreshGPGDlg(HWND,BOOL);
void ResetGeneralDlg(HWND);
void ResetProtoDlg(HWND);
LPARAM getListViewParam(HWND,UINT);
void setListViewIcon(HWND,UINT,pUinKey);
void setListViewMode(HWND,UINT,UINT);
void setListViewStatus(HWND,UINT,UINT);
UINT getListViewPSK(HWND,UINT);
void setListViewPSK(HWND,UINT,UINT);
UINT getListViewPUB(HWND,UINT);
void setListViewPUB(HWND,UINT,UINT);
int onRegisterOptions(WPARAM,LPARAM);
int CALLBACK CompareFunc(LPARAM,LPARAM,LPARAM);
void ListView_Sort(HWND,LPARAM);
BOOL ShowSelectKeyDlg(HWND,LPSTR);
LPSTR LoadKeys(LPCSTR,BOOL);
BOOL SaveExportRSAKeyDlg(HWND,LPSTR,BOOL);
BOOL LoadImportRSAKeyDlg(HWND,LPSTR,BOOL);

#define getListViewContact(h,i)	(HANDLE)getListViewParam(h,i)
#define getListViewProto(h,i)	(int)getListViewParam(h,i)

#endif
