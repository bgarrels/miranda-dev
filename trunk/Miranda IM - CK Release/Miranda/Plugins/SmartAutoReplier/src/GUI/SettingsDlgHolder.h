/*
Smart Auto Replier (SAR) for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2004 - 2012 by Volodymyr M. Shcherbyna <volodymyr@shcherbyna.com>

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

#pragma once

#include "..\resource.h"

#include <commctrl.h>
#include <atlbase.h>
#include <WTL/atlapp.h>
#include <atlwin.h>

#include "optionsdlg.h"

/// dialog that holds all 
/// gui dialogs..
class CSettingsDlgHolder : public CDialogImpl<CSettingsDlgHolder>,
						   public CMessageFilter, 
						   public IMSingeltone<CSettingsDlgHolder>
{
public: /// ctors
	CSettingsDlgHolder(void);
		/// dctors
	~CSettingsDlgHolder(void);
public:
	enum {IDD = IDD_SDLGHOLDER};	
	BEGIN_MSG_MAP(CSettingsDlgHolder)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(PSM_CHANGED, OnSettingsChanged)
	END_MSG_MAP()
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNotify(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSettingsChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
private:
	void OnNotifyParents(LPNMHDR lpnmhdr, int nCode);
private:
	bool m_bTabSelected;
public:
	static INT_PTR CALLBACK FakeDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
	CTabCtrl m_tabCtrl;			/// holder of all settings dlg
	CStatic m_statusMsg;
	COptionsDlg m_generalOpt;	/// general options dlg
public:
	bool m_bDestroying;
};
