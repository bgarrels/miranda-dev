/*
Skype protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright © 2008-2012 leecher, tweety, jls17,
						Laurent Marechal (aka Peorth)
						Gennady Feldman (aka Gena01) 

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

#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <process.h>
#include <time.h>
#include "resource.h"

void CreateProtoService(const char* szService, MIRANDASERVICE svc);
void HookEvents(void);
void HookEventsLoaded(void);
void UnhookEvents(void);
void CreateServices(void);
INT_PTR SkypeLoadIcon(WPARAM wParam, LPARAM lParam);
INT_PTR SkypeGetName(WPARAM wParam, LPARAM lParam);
INT_PTR SkypeGetCaps(WPARAM wParam, LPARAM lParam);

/* SkypeGetAvatar
  
  Purpose: Return the avatar file name
  Params : wParam=0
 			lParam=0
  Returns: 0 - Success
 		   -1 - Failure
 */

INT_PTR SkypeGetAvatar(WPARAM wParam,LPARAM lParam);