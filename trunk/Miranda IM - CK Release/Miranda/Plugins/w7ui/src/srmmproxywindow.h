#ifndef srmmproxywindow_h__
#define srmmproxywindow_h__

/*
W7ui plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2009 nullbie, persei

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

class CSrmmProxyWindow: public CDwmWindow
{
public:
	CSrmmProxyWindow(HANDLE hContact, HWND hwndWindow, HWND hwndParent);
	~CSrmmProxyWindow();

	void OnTabActive();
	void OnTabInactive();

private:
	HANDLE m_hContact;
	HWND m_hwndWindow, m_hwndParent;
	HBITMAP m_hbmpPreview;
	bool m_refreshPreview;
	bool m_bActive, m_bUnread, m_bTyping;

	void Refresh();

	int __cdecl OnDbEventAdded(WPARAM wParam, LPARAM lParam);
	int __cdecl OnDbSettingChanged(WPARAM wParam, LPARAM lParam);
	int __cdecl OnAvatarChanged(WPARAM wParam, LPARAM lParam);
	int __cdecl OnContactTyping(WPARAM wParam, LPARAM lParam);

	HANDLE SetEventHook(char *evt, int (__cdecl CSrmmProxyWindow::*fn)(WPARAM, LPARAM));

	void UpdateIcon();

	bool IsActive();

protected:
	void OnActivate(HWND hwndFrom);
	void OnToolbar(int id, INT_PTR data);
	void OnRenderThumbnail(int width, int height);
	void OnRenderPreview();
	void OnTimer(int);
	void OnClose();
};

#endif // srmmproxywindow_h__
