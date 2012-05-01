/*
Miranda IM: the free IM client for Microsoft* Windows*

Author Pescuma

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

#define MS_HOTKEYSPLUS_ADDKEY "HotkeysPlus/Add"

/*
This service registers hotkey for 
WPARAM - service to perform
LPARAM - decription of the service
Returned values:
	 0 - success,
	 1 - hotkey for this function is already existing,
	 2 - the service, that you try to register the hotkey for, not exists
*/

#define MS_HOTKEYSPLUS_EXISTKEY "HotkeysPlus/Exist"
/*
This service checks whether hotkey  for service (WPARAM) exists
LPARAM - not used
Returned values:
	 0 - failed,
	 1 - the hotkey for this function exists,
	 2 - the service not exists
*/

