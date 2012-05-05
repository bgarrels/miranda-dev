#ifndef __MIR_OPTIONS_H__
#define __MIR_OPTIONS_H__

/*
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2009-2010 Ricardo Pescuma Domenecci

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

#include <windows.h>

#define CONTROL_CHECKBOX 0			// Stored as BYTE
#define CONTROL_SPIN 1				// Stored as WORD
#define CONTROL_COLOR 2				// Stored as DWORD
#define CONTROL_RADIO 3				// Stored as WORD
#define CONTROL_COMBO 4				// Stored as WORD
#define CONTROL_PROTOCOL_LIST 5		// Stored as BYTEs
#define CONTROL_TEXT 6				// Stored as TCHARs, max len 1024
#define CONTROL_COMBO_TEXT 7		// Stored as TCHARs, max len 1024
#define CONTROL_COMBO_ITEMDATA 8	// Stored as TCHARs, max len 1024
#define CONTROL_FILE 9				// Stored as TCHARs, max len 1024
#define CONTROL_INT 10				// Stored as DWORD
#define CONTROL_PASSWORD 11			// Stored as chars, max len 1024


typedef BOOL (* FPAllowProtocol) (const char *proto);

typedef struct {
	void *var;
	int type;
	unsigned int nID;
	char *setting;
	union {
		ULONG_PTR dwDefValue;
		TCHAR *tszDefValue;
		char *szDefValue;
	};
	union {
		int nIDSpin;
		int value;
		FPAllowProtocol allowProtocol;
		unsigned int checkboxID;
	};
	WORD min;
	WORD max;
} OptPageControl;

INT_PTR CALLBACK SaveOptsDlgProc(OptPageControl *controls, int controlsSize, char *module, HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void LoadOpts(OptPageControl *controls, int controlsSize, char *module);






#endif // __MIR_OPTIONS_H__
