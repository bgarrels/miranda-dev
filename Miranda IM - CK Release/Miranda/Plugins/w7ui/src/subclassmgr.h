#ifndef subclassmgr_h__
#define subclassmgr_h__

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

struct TSubclassData
{
	WNDPROC oldWndProc;
	LPARAM lParam;
};

typedef LRESULT (*TSubclassProc)(MSG *msg, TSubclassData *data);

class CSubclassMgr
{
public:
	static void Subclass(HWND hwnd, TSubclassProc newWndProc, LPARAM lParam)
	{
		TWindowInfo *wi = new TWindowInfo;
		wi->hwnd = hwnd;
		wi->newWndProc = newWndProc;
		wi->lParam = lParam;
		Instance().m_windows.insert(wi);
		wi->oldWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)GlobalSubclassProc);
	}

private:
	CSubclassMgr(): m_windows(5, TWindowInfo::Compare) {}
	CSubclassMgr(const CSubclassMgr &);
	CSubclassMgr &operator=(const CSubclassMgr &);

	static CSubclassMgr &Instance()
	{
		static CSubclassMgr theInstance;
		return theInstance;
	}

	struct TWindowInfo
	{
		HWND hwnd;
		WNDPROC oldWndProc;
		TSubclassProc newWndProc;
		LPARAM lParam;

		static int Compare(const TWindowInfo *p1, const TWindowInfo *p2)
		{
			return (int)p1->hwnd - (int)p2->hwnd;
		}
	};

	OBJLIST<TWindowInfo> m_windows;

	static LRESULT CALLBACK GlobalSubclassProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		TWindowInfo search = { hwnd };
		TWindowInfo *wnd = Instance().m_windows.find(&search);
		if (!wnd) return DefWindowProc(hwnd, message, wParam, lParam);

		MSG msg = { hwnd, message, wParam, lParam };
		TSubclassData data = { wnd->oldWndProc, wnd->lParam };
		LRESULT result = wnd->newWndProc(&msg, &data);

		if (message == WM_DESTROY)
		{
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)wnd->oldWndProc);
			Instance().m_windows.remove(wnd);
		}

		return result;
	}
};

#endif // subclassmgr_h__
