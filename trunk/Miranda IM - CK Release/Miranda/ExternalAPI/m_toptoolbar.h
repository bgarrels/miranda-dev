#ifndef M_TOPTOOLBAR_H
#define M_TOPTOOLBAR_H

/*
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

//button flags
#define TTBBF_DISABLED			1
#define TTBBF_VISIBLE			2 
#define TTBBF_PUSHED			4
#define TTBBF_SHOWTOOLTIP		8
#define TTBBF_DRAWBORDER		16//draw border for bitmap,bitmap must be WxH 16x12
#define TTBBF_ISSEPARATOR		32

//for internal launch buttons
#define TTBBF_ISLBUTTON			64

typedef struct {
	int cbSize;
	HBITMAP hbBitmapUp;
	HBITMAP hbBitmapDown;
	char *pszServiceUp;
	char *pszServiceDown;
	DWORD dwFlags;
	LPARAM lParamUp;
	WPARAM wParamUp;
	LPARAM lParamDown;
	WPARAM wParamDown;
	char *name;

} TTBButton, * lpTTBButton;

typedef struct {
	int cbSize;
	HBITMAP hbBitmapUp;
	HBITMAP hbBitmapDown;
	char *pszServiceUp;
	char *pszServiceDown;
	DWORD dwFlags;
	LPARAM lParamUp;
	WPARAM wParamUp;
	LPARAM lParamDown;
	WPARAM wParamDown;
	char *name;
	HICON hIconUp,hIconDn;
	char *tooltipUp;
	char *tooltipDn;

} TTBButtonV2, * lpTTBButtonV2;

//=== EVENTS ===
/*
toptoolbar/moduleloaded event
wParam = lParam = 0
Called when the toolbar services are available

!!!Warning you may work with TTB services only in this event or later.

*/
#define ME_TTB_MODULELOADED				"TopToolBar/ModuleLoaded"



//=== SERVICES ===
/*
toptoolbar/addbutton service
wparam = (TTBButton*)lpTTBButton
lparam = 0
returns: hTTBButton - handle of added button on success, -1 on failure.
*/
#define MS_TTB_ADDBUTTON						"TopToolBar/AddButton"

/*
toptoolbar/removebutton service
wparam = (HANDLE)hTTButton
lparam = 0
returns: 0 on success, -1 on failure.
*/
#define MS_TTB_REMOVEBUTTON					"TopToolBar/RemoveButton"

/*
toptoolbar/setstate service
wparam = (HANDLE)hTTButton
lparam = (LPARAM) state
returns: 0 on success, -1 on failure.
*/
#define TTBST_PUSHED			1
#define TTBST_RELEASED			2

#define MS_TTB_SETBUTTONSTATE					"TopToolBar/SetState"

/*
toptoolbar/getstate service
wparam = (HANDLE)hTTButton
lparam = 0
returns: state on success, -1 on failure.
*/
#define MS_TTB_GETBUTTONSTATE					"TopToolBar/GetState"

/*
toptoolbar/getoptions service
(HIWORD)wparam = (HANDLE)hTTButton
(LOWORD)wparam = TTBO_FLAG
lparam = 0,or lparam=lpTTBButton if flag=TTBO_ALLDATA
returns: value on success, -1 on failure.
*/
#define TTBO_FLAGS				0     //get/set all flags
#define TTBO_POS				1     //position
#define TTBO_WIDTH				2     //not impemented
#define TTBO_HEIGHT				3     //not impemented
#define TTBO_TIPNAME			4     //tool tip name
#define TTBO_ALLDATA			5     //change all data via lparam=lpTTBButton


#define MS_TTB_GETBUTTONOPTIONS					"TopToolBar/GetOptions"

/*
toptoolbar/setoptions service
(HIWORD)wparam = (HANDLE)hTTButton
(LOWORD)wparam = TTBO_FLAG
lparam = value
returns: 1 on success, -1 on failure.
*/
#define MS_TTB_SETBUTTONOPTIONS					"TopToolBar/SetOptions"


#endif