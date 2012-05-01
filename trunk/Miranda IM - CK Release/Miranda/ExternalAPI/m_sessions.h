#ifndef M_SESSIONS_H__
#define M_SESSIONS_H__

/*
Sessions Management plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author Danil Mozhar

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

//////////////////////////////////////////////////////////////////////////
// Services
//
//////////////////////////////////////////////////////////////////////////
// Opens session load dialog
//
// wParam  = 0
// lParam  = 0
#define	MS_SESSIONS_OPENMANAGER			"Sessions/Service/OpenManager"

//////////////////////////////////////////////////////////////////////////
// Loads last session
//
// wParam = 0 
// lParam  = 0
#define	MS_SESSIONS_RESTORELASTSESSION		"Sessions/Service/OpenLastSession"

//////////////////////////////////////////////////////////////////////////
// Opens current/user-defined session save dialog 
//
// wParam = 0
// lParam = 0
#define	MS_SESSIONS_SAVEUSERSESSION		"Sessions/Service/SaveUserSession"

//////////////////////////////////////////////////////////////////////////
// Closes current opened session 
//
// wParam = 0
// lParam = 0
#define	MS_SESSIONS_CLOSESESSION			"Sessions/Service/CloseSession"

//////////////////////////////////////////////////////////////////////////
// Builds Favorite Sessions menu 
//
// wParam = 0
// lParam = 0
#define	MS_SESSIONS_SHOWFAVORITESMENU		"Sessions/Service/ShowFavMenu"


#endif  //M_SESSIONS_H__