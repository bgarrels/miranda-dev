#ifndef _COOLSB_INTERNAL_INCLUDED
#define _COOLSB_INTERNAL_INCLUDED

/*
Authors Richard Hughes, Roland Rabien & Tristan Van de Vreede

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

#ifdef __cplusplus
extern "C"{
#endif

#include <windows.h>

//
//	SCROLLBAR datatype. There are two of these structures per window
//
typedef struct 
{
	UINT		fScrollFlags;		//flags
	BOOL		fScrollVisible;		//if this scrollbar visible?
	SCROLLINFO	scrollInfo;			//positional data (range, position, page size etc)
	
	int			nArrowLength;		//perpendicular size (height of a horizontal, width of a vertical)
	int			nArrowWidth;		//parallel size (width of horz, height of vert)

	//data for inserted buttons
	SCROLLBUT	sbButtons[MAX_COOLSB_BUTS];
	int			nButtons;
	int			nButSizeBefore;		//size to the left / above the bar
	int			nButSizeAfter;		//size to the right / below the bar

	BOOL		fButVisibleBefore;	//if the buttons to the left are visible
	BOOL		fButVisibleAfter;	//if the buttons to the right are visible

	int			nBarType;			//SB_HORZ / SB_VERT

	UINT		fFlatScrollbar;		//do we display flat scrollbars?
	int			nMinThumbSize;

} SCROLLBAR;

//
//	Container structure for a cool scrollbar window.
//
typedef struct
{
	UINT bars;				//which of the scrollbars do we handle? SB_VERT / SB_HORZ / SB_BOTH
	WNDPROC oldproc;		//old window procedure to call for every message

	SCROLLBAR sbarHorz;		//one scrollbar structure each for 
	SCROLLBAR sbarVert;		//the horizontal and vertical scrollbars

	BOOL fThumbTracking;	// are we currently thumb-tracking??
	BOOL fLeftScrollbar;	// support the WS_EX_LEFTSCROLLBAR style

	HWND hwndToolTip;		// tooltip support!!!

	//size of the window borders
	int cxLeftEdge, cxRightEdge;
	int cyTopEdge,  cyBottomEdge;

	// To prevent calling original WindowProc in response
	// to our own temporary style change (fixes TreeView problem)
	BOOL bPreventStyleChange;

} SCROLLWND;


//
//	PRIVATE INTERNAL FUNCTIONS
//
SCROLLWND *GetScrollWndFromHwnd(HWND hwnd);
#define InvertCOLORREF(col) ((~col) & 0x00ffffff)

#define COOLSB_TIMERID1			65533		//initial timer
#define COOLSB_TIMERID2			65534		//scroll message timer
#define COOLSB_TIMERID3			-14			//mouse hover timer
#define COOLSB_TIMERINTERVAL1	300
#define COOLSB_TIMERINTERVAL2	55
#define COOLSB_TIMERINTERVAL3	20			//mouse hover time


//
//	direction: 0 - same axis as scrollbar (i.e.  width of a horizontal bar)
//             1 - perpendicular dimesion (i.e. height of a horizontal bar)
//
#define SM_CXVERTSB 1
#define SM_CYVERTSB 0
#define SM_CXHORZSB 0
#define SM_CYHORZSB 1
#define SM_SCROLL_WIDTH	1
#define SM_SCROLL_LENGTH 0


#ifdef __cplusplus
}
#endif

#endif
