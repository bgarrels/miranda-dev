/*
Author Artem Shpynov aka FYR

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

#ifndef modern_statusbar_h__
/*
Author Artem Shpynov aka FYR

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

#define modern_statusbar_h__

#include "modern_commonprototypes.h"
#include "../m_api/m_xpTheme.h"

int ModernDrawStatusBar(HWND hwnd, HDC hDC);
int ModernDrawStatusBarWorker(HWND hWnd, HDC hDC);

typedef struct tagSTATUSBARDATA 
{
  BOOL sameWidth;
  RECT rectBorders;
  BYTE extraspace;
  BYTE Align;
  BYTE VAlign;
  BYTE showProtoIcon;
  BYTE showProtoName;
  BYTE showStatusName;
  HFONT BarFont;
  DWORD fontColor;
  BYTE connectingIcon;
  BYTE TextEffectID;
  DWORD TextEffectColor1;
  DWORD TextEffectColor2;
  BYTE xStatusMode;     // 0-only main, 1-xStatus, 2-main as overlay
  BYTE nProtosPerLine;
  BYTE showProtoEmails;

  HBITMAP hBmpBackground;
  COLORREF bkColour;
  DWORD backgroundBmpUse;
  BOOL  bkUseWinColors;

  XPTHANDLE hTheme;

  BOOL perProtoConfig;
  BYTE SBarRightClk;

} STATUSBARDATA;

#endif // modern_statusbar_h__
