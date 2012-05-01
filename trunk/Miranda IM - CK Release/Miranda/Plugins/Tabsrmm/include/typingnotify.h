/*
Tabsrmm plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Copyright 2000-2012 Miranda IM project,
all portions of this codebase are copyrighted to the people
listed in contributors.txt.

Author Silvercircle

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

#define UM_SETDLGITEMINT          5674

#define TIMEOUT_MINVALUE     -1
#define TIMEOUT_MAXVALUE     99
#define TIMEOUT_POPUP        1
#define TIMEOUT_CUSTOM       2
#define TIMEOUT_PROTO        3
#define TIMEOUT_PERMANENT    4
#define COLOR_OWN            1
#define COLOR_WINDOWS        2
#define COLOR_POPUP          3

#define Module "TypingNotify"

#define SET_WO               "NotWhenFocused"
#define DEF_WO               0
#define SET_DISABLED         "Disabled"
#define DEF_DISABLED         0
#define SET_TIMEOUT          "Timeout"
#define DEF_TIMEOUT          7
#define SET_TIMEOUT2         "Timeout2"
#define DEF_TIMEOUT2         7
#define SET_TIMEOUT_MODE     "TimeoutMode"
#define DEF_TIMEOUT_MODE     TIMEOUT_POPUP
#define SET_TIMEOUT_MODE2    "TimeoutMode2"
#define DEF_TIMEOUT_MODE2    TIMEOUT_POPUP
#define SET_COLOR_MODE       "ColorMode"
#define DEF_COLOR_MODE       COLOR_OWN
#define SET_ICON_SETID       "IconSet"
#define DEF_ICON_SETID       0
#define SET_SHOWDISABLEMENU  "ShowDisableMenu"
#define DEF_SHOWDISABLEMENU  1
#define SET_ONEPOPUP         "OnePopUp"
#define DEF_ONEPOPUP         1


static HANDLE hDisableMenu = NULL;
static HANDLE hPopUpsList = NULL;

static BYTE OnePopUp;
static BYTE ShowMenu;
static BYTE PopupService=0;
static BYTE StartDisabled;
static BYTE StopDisabled;
static BYTE Disabled;
static BYTE ColorMode;
static BYTE TimeoutMode;
static BYTE TimeoutMode2;
static int Timeout;
static int Timeout2;
static int newTimeout;
static int newTimeout2;
static BYTE newTimeoutMode;
static BYTE newTimeoutMode2;
static BYTE newColorMode;
static TCHAR szStart[128];
static TCHAR szStop[128];

static HANDLE hntfStarted = 0;
static HANDLE hntfStopped = 0;


struct colors_s
{
        int res;
        char desc[10];
        COLORREF color;
};

static struct colors_s colorPicker[4] =
{
        {IDC_TYPEON_BG,  "ON_BG",  RGB(255,255,255)},
        {IDC_TYPEON_TX,  "ON_TX",  RGB(0,0,0)},
        {IDC_TYPEOFF_BG, "OFF_BG", RGB(255,255,255)},
        {IDC_TYPEOFF_TX, "OFF_TX", RGB(0,0,0)}
};
