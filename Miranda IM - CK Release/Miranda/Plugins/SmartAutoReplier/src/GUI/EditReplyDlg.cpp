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

#include "..\stdafx.h"
#include "editreplydlg.h"

extern LPTSTR g_strPluginName;

CEditReplyDlg::CEditReplyDlg(void)
{
}

CEditReplyDlg::~CEditReplyDlg(void)
{
}

BOOL CEditReplyDlg::PreTranslateMessage(MSG* pMsg)
{
	return IsDialogMessage(pMsg);
}

LRESULT CEditReplyDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{	
	TranslateDialogDefault(m_hWnd);
	m_editReplyText = GetDlgItem(IDC_ED_TEXT);
	
	if (m_commRule.Message)
	{
		m_editReplyText.SetWindowText(m_commRule.Message);
	}

	CenterWindow(GetDesktopWindow());	

	return FALSE;
}

LRESULT CEditReplyDlg::OnBtnOKClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nLength = 0;

	delete m_commRule.Message;

	nLength = m_editReplyText.GetWindowTextLength();

	if (nLength > SETTINGS_MESSAGE_MAXVALENGTH)	
	{
		MessageBox(TranslateT("message is too big"), g_strPluginName, MB_OK);

		return FALSE;
	}
	nLength++;
	m_commRule.Message = new TCHAR[nLength];

	if (!m_commRule.Message)
		return FALSE;
	
	memset(m_commRule.Message, 0, nLength * sizeof(TCHAR));
	m_editReplyText.GetWindowText((LPTSTR)m_commRule.Message, nLength);

	m_bAllOk = true;
	EndDialog(1);
	return FALSE;
}

LRESULT CEditReplyDlg::OnBtnCancelClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_bAllOk = false;
	EndDialog(0);
	return FALSE;
}