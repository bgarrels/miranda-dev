#ifndef __M_AVATARHISTORY_H__
#define __M_AVATARHISTORY_H__

/*
Avatar History Plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors:
			Copyright (C) 2006  Matthew Wild - Email: mwild1@gmail.com
			Copyright (C) 2006-2010 Pescuma
			Copyright (C) 2012 wishmaster51

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

==============================================================================
*/

#define MIID_AVATAR_CHANGE_LOGGER { 0x95e3f3d3, 0x9678, 0x4561, { 0x96, 0xf8, 0x95, 0x88, 0x33, 0x7b, 0x86, 0x68 } }
#define MIID_AVATAR_CHANGE_NOTIFIER { 0x91af9298, 0x8570, 0x4063, { 0xbf, 0x2f, 0xca, 0x68, 0xd0, 0xe3, 0xb3, 0x6a } }


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
lParam: (TCHAR *) hash 
return: (TCHAR *) NULL if none is found or the path to the avatar. You need to free this string 
        with mir_free.
*/
#define MS_AVATARHISTORY_GET_CACHED_AVATAR		"AvatarHistory/GetCachedAvatar"


#define MS_AVATARHISTORY_SHOWDIALOG "AvatarHistory/ShowDialog"



#endif // __M_AVATARHISTORY_H__
