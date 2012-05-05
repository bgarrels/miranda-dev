#ifndef _DLGANNIVERSARYLIST_H_
#define _DLGANNIVERSARYLIST_H_

/*
UserinfoEx plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2006-2010 DeathAxe, Yasnovidyashii, Merlin_de, K. Romanov, Kreol

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

#define SET_ANNIVLIST_POPUP					"AnLstPopup"
#define SET_ANNIVLIST_FILTER_DAYSENABLED	"AnLstFltDaysEnabled"
#define SET_ANNIVLIST_FILTER_DAYS			"AnLstFltDays"
#define SET_ANNIVLIST_FILTER_INDEX			"AnLstFltIndex"

INT_PTR		DlgAnniversaryListShow(WPARAM wParam, LPARAM lParam);
VOID		DlgAnniversaryListOnTopToolBarLoaded();
VOID		DlgAnniversaryListOnToolBarLoaded();
VOID		DlgAnniversaryListLoadModule();

#endif /* _DLGANNIVERSARYLIST_H_ */