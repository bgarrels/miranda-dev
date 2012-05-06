#ifndef clistproxywindow_h__
#define clistproxywindow_h__

/*
W7ui plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
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

class CClistProxyWindow: public CDwmWindow
{
public:
	CClistProxyWindow();
	~CClistProxyWindow();

	void Flash();
	void SetOverlayIcon(HICON hIcon);
	void SetOverlayIconHandle(HANDLE hIcolibIcon);
	void AddOverlayEvent(int idx);
	void RemoveOverlayEvent(int idx);
	int AllocateOverlayEvent(HANDLE hIcolibIcon);

private:
	class COverlayEventSlot
	{
	private:
		HANDLE m_hIcolibItem;
		TCHAR *m_overlayText;
		int m_level;

	public:
		COverlayEventSlot(HANDLE hIcolibItem, TCHAR *overlayText)
		{
			m_hIcolibItem = hIcolibItem;
			m_overlayText = mir_tstrdup(overlayText);
			m_level = 0;
		}
		~COverlayEventSlot() { mir_free(m_overlayText); }
		void Push() { m_level++; }
		void Pop() { if (m_level > 0) m_level--; }
		operator bool() { return m_level > 0; }
		HANDLE GetIcon() { return m_hIcolibItem; }
	};

	int m_activeOverlay;
	HICON m_overlayIcon;
	HANDLE m_overlayIconHandle;
	TCHAR *m_overlayText;
	OBJLIST<COverlayEventSlot> m_overlayEvents;

	int __cdecl OnStatusModeChanged(WPARAM wParam, LPARAM lParam);
	int __cdecl OnAvatarChanged(WPARAM wParam, LPARAM lParam);
	int __cdecl OnModulesLoaded(WPARAM wParam, LPARAM lParam);
	int __cdecl OnAccListChanged(WPARAM wParam, LPARAM lParam);

	HANDLE SetEventHook(char *evt, int (__cdecl CClistProxyWindow::*fn)(WPARAM, LPARAM));

	void Update();

protected:
	void OnActivate(HWND hwndFrom);
	void OnToolbar(int id, INT_PTR data);
	void OnRenderThumbnail(int width, int height);
	void OnClose();
	void OnTimer(int id);
};

extern CClistProxyWindow *g_clistProxyWnd;

#endif // clistproxywindow_h__
