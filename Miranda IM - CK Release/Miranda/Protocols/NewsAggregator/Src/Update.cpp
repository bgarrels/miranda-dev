/* 
Copyright (C) 2012 Mataes

This is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this file; see the file license.txt.  If
not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  
*/

#include "common.h"

// check if Feed is currently updating
BOOL ThreadRunning;

// main auto-update timer
VOID CALLBACK timerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	// only run if it is not current updating and the auto update option is enabled
//	if (!ThreadRunning && !Miranda_Terminated() && status == ID_STATUS_ONLINE)
//		UpdateAll(TRUE, FALSE);
}

// temporary timer for first run
// when this is run, it kill the old startup timer and create the permenant one above
VOID CALLBACK timerProc2(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	KillTimer(NULL, timerId);
	ThreadRunning = FALSE;

	if (!Miranda_Terminated())
	{
//		UpdateAll(FALSE, FALSE);
		//timerId = SetTimer(NULL, 0, ((int)opt.UpdateTime)*60000, (TIMERPROC)timerProc);
	}
}