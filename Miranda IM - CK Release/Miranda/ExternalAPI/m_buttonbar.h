#ifndef _BUTTONSBAR_H
#define _BUTTONSBAR_H

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

#define MIN_CBUTTONID 4000
#define MAX_CBUTTONID 5000

#define BBSF_IMBUTTON		(1<<0)
#define BBSF_CHATBUTTON		(1<<1)
#define BBSF_CANBEHIDDEN	(1<<2)
#define BBSF_NTBSWAPED		(1<<3)
#define BBSF_NTBDESTRUCT	(1<<4)

typedef struct _tagCustomButtonData
	{
	DWORD dwButtonOrigID;		 // id of button used while button creation and to store button info in DB
	char * pszModuleName;		 //module name without spaces and underline symbols (e.g. "tabsrmm")

	DWORD dwButtonCID;
	DWORD dwArrowCID;			//only use with BBBF_ISARROWBUTTON flag

	TCHAR * ptszTooltip;		//button's tooltip

	DWORD dwPosition;			 // default order pos of button, counted from window edge (left or right)
	int iButtonWidth;		//must be 22 for regular button and 33 for button with arrow
	HANDLE hIcon;		//Handle to icolib registred icon
	BOOL bIMButton,bChatButton;
	BOOL bCanBeHidden,bHidden,bAutoHidden,bDummy,bDisabled,bPushButton;
	BOOL bLSided,bRSided;
	BYTE opFlags;
	}CustomButtonData;

static INT_PTR CB_ModifyButton(WPARAM wParam, LPARAM lParam);
static INT_PTR CB_RemoveButton(WPARAM wParam, LPARAM lParam);
static INT_PTR CB_AddButton(WPARAM wParam, LPARAM lParam);
static INT_PTR CB_GetButtonState(WPARAM wParam, LPARAM lParam);
static INT_PTR CB_SetButtonState(WPARAM wParam, LPARAM lParam);
static void CB_GetButtonSettings(HANDLE hContact,CustomButtonData *cbd);

void CB_WriteButtonSettings(HANDLE hContact,CustomButtonData *cbd);
int  sstSortButtons(const void * vmtbi1, const void * vmtbi2);

void CB_DeInitCustomButtons();
void CB_InitCustomButtons();
void CB_InitDefaultButtons();
void CB_ReInitCustomButtons();

/* MinGW doesn't like this struct declatations below */
void BB_UpdateIcons(HWND hdlg,struct TWindowData *dat);
void BB_RefreshTheme(const TWindowData *dat);
void CB_DestroyAllButtons(HWND hwndDlg,struct TWindowData *dat);
void CB_DestroyButton(HWND hwndDlg,struct TWindowData *dat,DWORD dwButtonCID,DWORD dwFlags);
void CB_ChangeButton(HWND hwndDlg,struct TWindowData *dat,CustomButtonData* cbd);

#endif
