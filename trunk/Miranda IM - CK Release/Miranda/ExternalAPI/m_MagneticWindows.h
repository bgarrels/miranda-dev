#ifndef __M_MAGNETICWINDOWS_H__
#define __M_MAGNETICWINDOWS_H__

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

//#include "../include/newpluginapi.h"

// For other Plugins to start snapping for their windows 
// wparam: hwnd of window
// lparam: 0
// return: 0 on success, 1 on error
#define MS_MW_ADDWINDOW "Utils/MagneticWindows/Add"

// For other Plugins to stop snapping for their windows
// wparam: hwnd of window
// lparam: 0
// return: 0 on success, 1 on error
#define MS_MW_REMWINDOW "Utils/MagneticWindows/Rem"

//decide where to align on the list:
#define MS_MW_STL_List_Left    0x00000001  //Snaps the window to the left   border of the list
#define MS_MW_STL_List_Top     0x00000002  //Snaps the window to the top    border of the list
#define MS_MW_STL_List_Right   0x00000004  //Snaps the window to the right  border of the list
#define MS_MW_STL_List_Bottom  0x00000008  //Snaps the window to the bottom border of the list
//decide with what side (of the window you want to snap) to snap to the list
#define MS_MW_STL_Wnd_Left     0x00000010  //Snaps the window with the left   border to the left/right side of the list
#define MS_MW_STL_Wnd_Top      0x00000020  //Snaps the window with the top    border to the top/bottom side of the list
#define MS_MW_STL_Wnd_Right    0x00000040  //Snaps the window with the right  border to the left/right side of the list
#define MS_MW_STL_Wnd_Bottom   0x00000080  //Snaps the window with the bottom border to the top/bottom side of the list

#define MS_MW_STL_Wnd_FullWidth  (MS_MW_STL_Wnd_Left | MS_MW_STL_Wnd_Right)
											//Snaps to the top/bottom of the list and spans over the full width

#define MS_MW_STL_Wnd_FullHeight (MS_MW_STL_Wnd_Top | MS_MW_STL_Wnd_Bottom)
											//Snaps to the left/right of the list and spans over the full height

// to place the window in the list combine f.e. MS_MW_STL_List_Left | MS_MW_STL_Wnd_Right | *vetical alignment*

//For other Plugins to snap a window to the list for other Plugins
// wparam: hwnd of window
// lparam: combination of the above constants MS_MW_STL_*
// return: 0 on success, 1 on error
#define MS_MW_SNAPTOLIST "Utils/MagneticWindows/SnapToList"

// Helper functions
#ifndef _MW_NO_HELPPER_FUNCTIONS


static inline int MagneticWindows_AddWindow(HWND hWnd)
{
	if (ServiceExists(MS_MW_ADDWINDOW))
	{
		return CallService(MS_MW_ADDWINDOW, (WPARAM) hWnd, 0);
	}
	else
	{
		return -1;
	}
}

static inline int MagneticWindows_RemoveWindow(HWND hWnd)
{
	if (ServiceExists(MS_MW_REMWINDOW))
	{
		return CallService(MS_MW_REMWINDOW, (WPARAM) hWnd, 0);
	}
	else
	{
		return -1;
	}
}

static inline int MagneticWindows_SnapWindowToList(HWND hWnd, int MS_MW_STL_Options)
{
	if (ServiceExists(MS_MW_SNAPTOLIST))
	{
		return CallService(MS_MW_SNAPTOLIST, (WPARAM) hWnd, (LPARAM) MS_MW_STL_Options);
	}
	else
	{
		return -1;
	}
}

#endif


#endif // __M_MAGNETICWINDOWS_H__
