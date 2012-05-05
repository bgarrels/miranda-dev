/*
PackUpdater plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C)	2010-2012 Mataes
							2007 ZERO_BiT

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

#define POPUPS 4

//=== Objects =====
//This one is used to easily tie status id, icon, text...
typedef struct
{
	int ID;
	int Icon;
	COLORREF colorBack;
	COLORREF colorText;
} aPopups;

typedef struct _MSGPOPUPDATA
{
	POPUPACTION	pa[4];
	HWND		hDialog;
}
MSGPOPUPDATA, *LPMSGPOPUPDATA;

#define DEFAULT_POPUP_LCLICK				1
#define DEFAULT_POPUP_RCLICK				0
#define DEFAULT_POPUP_ENABLED				1
#define DEFAULT_MESSAGE_ENABLED				1
#define DEFAULT_TIMEOUT_VALUE				0

#define COLOR_BG_FIRSTDEFAULT	RGB(173,206,247)
#define COLOR_BG_SECONDDEFAULT	RGB(255,189,189)
#define COLOR_TX_DEFAULT		RGB(0,0,0)

#define byCOLOR_OWN	0x1
#define byCOLOR_WINDOWS	0x2
#define byCOLOR_POPUP	0x3
#define DEFAULT_COLORS	byCOLOR_POPUP

// Actions on popup click
#define PCA_CLOSEPOPUP		0	// close popup
#define PCA_DONOTHING		1   // do nothing

//===== Options flags
typedef struct tagMYOPTIONS {
	BYTE DefColors;
	BYTE LeftClickAction;
	BYTE RightClickAction;
	int Timeout;
} MYOPTIONS;

static struct {
	TCHAR *Text;
	int Action;
} PopupActions[] = {
	_T("Close popup"), PCA_CLOSEPOPUP,
	_T("Do nothing"), PCA_DONOTHING
};
