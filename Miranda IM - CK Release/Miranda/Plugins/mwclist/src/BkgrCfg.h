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

/*
  Register of plugin's user

wParam = (WPARAM)szSetting - string that describes a user
          format: Category/ModuleName,
          eg: "Contact list background/CLUI",
              "Status bar background/StatusBar"
lParam = (LPARAM)dwFlags

*/
#define MS_BACKGROUNDCONFIG_REGISTER "BkgrCfg/Register"

/*
  Notification about changed background
  wParam = ModuleName
  lParam = 0
*/
#define ME_BACKGROUNDCONFIG_CHANGED "BkgrCfg/Changed"

#define CLBF_TILEVTOROWHEIGHT        0x0100
