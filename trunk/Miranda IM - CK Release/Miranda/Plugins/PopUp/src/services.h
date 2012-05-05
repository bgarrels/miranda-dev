#ifndef __services_h__
#define __services_h__

/*
Popup Plus plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	2002 Luca Santarelli
							2004-2007 Victor Pavlychko
							2010-2012 Merlin_de, MPK

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

extern int num_classes;			//for core class api support

INT_PTR PopUp_AddPopUp(WPARAM, LPARAM);
INT_PTR PopUp_AddPopUpW(WPARAM, LPARAM);

INT_PTR PopUp_AddPopUpEx(WPARAM, LPARAM);
INT_PTR PopUp_AddPopUp2(WPARAM, LPARAM);

INT_PTR PopUp_GetContact(WPARAM, LPARAM);
INT_PTR PopUp_IsSecondLineShown(WPARAM, LPARAM);

INT_PTR PopUp_ChangeText(WPARAM, LPARAM);
INT_PTR PopUp_ChangeTextW(WPARAM, LPARAM);

INT_PTR PopUp_Change(WPARAM, LPARAM);
INT_PTR PopUp_ChangeW(WPARAM, LPARAM);
INT_PTR PopUp_Change2(WPARAM, LPARAM);

INT_PTR PopUp_ShowMessage(WPARAM, LPARAM);
INT_PTR PopUp_ShowMessageW(WPARAM, LPARAM);

INT_PTR PopUp_Query(WPARAM, LPARAM);
INT_PTR PopUp_GetPluginData(WPARAM, LPARAM);
INT_PTR PopUp_RegisterActions(WPARAM wParam, LPARAM lParam);
INT_PTR PopUp_RegisterNotification(WPARAM wParam, LPARAM lParam);
INT_PTR PopUp_UnhookEventAsync(WPARAM wParam, LPARAM lParam);
INT_PTR PopUp_RegisterVfx(WPARAM wParam, LPARAM lParam);

INT_PTR PopUp_RegisterPopupClass(WPARAM wParam, LPARAM lParam);
INT_PTR PopUp_CreateClassPopup(WPARAM wParam, LPARAM lParam);

#endif // __services_h__
