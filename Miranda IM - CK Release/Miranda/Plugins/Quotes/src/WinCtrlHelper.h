#ifndef __a05d6852_4497_4f28_85e1_48a15a170738_WinCtrlHelper_h__
#define __a05d6852_4497_4f28_85e1_48a15a170738_WinCtrlHelper_h__

/*
Quotes (Forex) plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) Dioksin

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

class IQuotesProvider;

inline tstring get_window_text(HWND hWnd)
{
	int cBytes = ::GetWindowTextLength(hWnd);

	std::vector<TCHAR> aBuf(cBytes+1);
	LPTSTR pBuffer = &*(aBuf.begin());
	::GetWindowText(hWnd,pBuffer,cBytes+1);

	return tstring(pBuffer);
}

inline void prepare_edit_ctrl_for_error(HWND hwndEdit)
{
	::SetFocus(hwndEdit);
	::SendMessage(hwndEdit, EM_SETSEL, 0, -1);
	::SendMessage(hwndEdit, EM_SCROLLCARET, 0, 0);
}

void show_variable_list(HWND hwndParent,const IQuotesProvider* pProvider);

inline int Quotes_MessageBox(HWND hWnd,LPCTSTR pszText,UINT nType = MB_OK)
{	
	return ::MessageBox(hWnd,pszText,quotes_a2t(MIRANDANAME).c_str(),nType);
}

inline void spin_set_range(HWND hwndSpin,short nLower,short nUpper)
{
	::SendMessage(hwndSpin,UDM_SETRANGE,0,MAKELPARAM(nUpper,nLower));
}


#endif //__a05d6852_4497_4f28_85e1_48a15a170738_WinCtrlHelper_h__
