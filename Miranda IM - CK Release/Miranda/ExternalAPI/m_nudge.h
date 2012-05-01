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

#define	MS_SHAKE_CLIST			"SHAKE/Service/ShakeClist"
#define	MS_SHAKE_CHAT			"SHAKE/Service/ShakeChat"
#define	MS_NUDGE_SEND			"NUDGE/Send"

// Hide or Show the context menu "send nudge" 
// wParam = char *szProto
// lParam = BOOL show
#define MS_NUDGE_SHOWMENU	"NudgeShowMenu"

#define MUUID_NUDGE_SEND { 0x9c66a9a, 0x57dc, 0x454d, { 0xa9, 0x30, 0xf8, 0xc0, 0x4f, 0xe2, 0x98, 0x38 } }
