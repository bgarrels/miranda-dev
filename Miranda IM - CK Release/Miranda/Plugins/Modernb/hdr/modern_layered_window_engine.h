/*
Author Pescuma

Miranda IM: the free IM client for Microsoft* Windows*

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

===============================================================================
*/

#pragma once

#include "../m_api/m_skin_eng.h"

// typedef int (/*__stdcall*/ *tPaintCallbackProc)(HWND hWnd, HDC hDC, RECT * rcPaint, HRGN rgnUpdate, DWORD dFlags, void * CallBackData);

class CLayeredWindowEngine
{
private:
	/*class CLweInfo
	{
		HWND hWnd;
		HRGN hInvalidRgn;
	};
	  */
	//typedef std::map<HWND, CLweInfo> WndInfos;

	enum { state_invalid, state_normal };

	//WndInfos			m_infos;
	DWORD				m_hValidatorThread;
	CRITICAL_SECTION	m_cs;
	int					m_state;
	volatile bool		m_invalid;

public:
	CLayeredWindowEngine(void);
	~CLayeredWindowEngine(void);

	void _init();
	void _deinit();

	void lock() { EnterCriticalSection( &m_cs ); }
	void unlock() { LeaveCriticalSection( &m_cs ); }

	int get_state();

public:
	static void __cdecl LweValidatorProc();
	
	void LweValidatorProcWorker();

	void LweValidatorWorker();
	int LweInvalidateRect( HWND hWnd, const RECT * rect, BOOL bErase );
	// int LweValidateWindowRect( HWND hWnd, RECT * rect );
	// int RegisterWindow( HWND hwnd, tPaintCallbackProc pPaintCallBackProc );

};

extern CLayeredWindowEngine _lwe;

#define _InvalidateRect _lwe.LweInvalidateRect