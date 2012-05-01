#ifndef __M_AVATARHISTORY_H__
# define __M_AVATARHISTORY_H__

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

#define EVENTTYPE_AVATAR_CHANGE 9003

/*
Return TRUE is Avatar History is enabled for this contact

wParam: hContact
lParam: ignored
*/
#define MS_AVATARHISTORY_ENABLED				"AvatarHistory/IsEnabled"


/*
Get cached avatar

wParam: (char *) protocol name
lParam: (char *) hash 
return: (TCHAR *) NULL if none is found or the path to the avatar. You need to free this string 
        with mir_free.
*/
#define MS_AVATARHISTORY_GET_CACHED_AVATAR		"AvatarHistory/GetCachedAvatar"





#endif // __M_AVATARHISTORY_H__
