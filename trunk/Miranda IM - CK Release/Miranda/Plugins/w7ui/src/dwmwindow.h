#ifndef dwmwindow_h__
#define dwmwindow_h__

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

class CDwmWindow
{
public:
	CDwmWindow();
	virtual ~CDwmWindow() {}

	HWND hwnd() { return m_hwnd; }

	template<class TWindow>
	static TWindow *GetWindow(HWND hwnd)
	{
		return (TWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}

protected:
	// events
	virtual void OnActivate(HWND hwndFrom) {}
	virtual void OnClose() {}
	virtual void OnRenderThumbnail(int mzxWidth, int maxHeight) {}
	virtual void OnRenderPreview() {}
	virtual void OnTimer(int id) {}
	virtual void OnToolbar(int id, INT_PTR data) {}

	// timer stuff
	void SetTimer(int id, int timeout);
	void KillTimer(int id);

	// manage thumbnail and aero peek
	void InvalidateThumbnail();
	void SetPreview(HBITMAP hbmp, int x, int y);
	void SetThumbnail(HBITMAP hbmp);

	// manage toolbar
	bool AddButton(HICON hIcon, TCHAR *text, INT_PTR data, DWORD flags = THBF_ENABLED);
	void UpdateButtons(ITaskbarList3 *p);

	//utilities
	static HBITMAP CreateDwmBitmap(int width, int height);
	static void MakeBitmapOpaque(HBITMAP hBmp);
	static void DrawGradient(HDC hdc, int x, int y, int width, int height, RGBQUAD *rgb0, RGBQUAD *rgb1);

private:
	HWND m_hwnd;

	bool m_btnInitialized;
	int m_btnCount;
	THUMBBUTTON m_btnInfo[7];
	INT_PTR m_btnData[7];

	LRESULT CALLBACK WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

	static void GlobalInitWndClass();
	static LRESULT CALLBACK GlobalWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif // dwmwindow_h__