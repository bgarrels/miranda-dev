#ifndef __M_SPELLCHECKER_H__
#define __M_SPELLCHECKER_H__

/*
Spellchecker plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors Ricardo Pescuma Domenecci

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

#define MIID_SPELLCHECKER { 0x26eed12a, 0x7016, 0x4d0f, { 0x9b, 0x4a, 0xc, 0xaa, 0x7e, 0x22, 0x29, 0xf3 } }

typedef struct {
	int cbSize;
	HANDLE hContact;	// The contact to get the settings from, or NULL
	HWND hwnd;			// The hwnd of the richedit
	char *window_name;	// A name for this richedit
} SPELLCHECKER_ITEM;

typedef struct {
	int cbSize;
	HWND hwnd;			// The hwnd of the richedit
	HMENU hMenu;		// The handle to the menu
	POINT pt;			// The point, in screen coords
	HWND hwndOwner;		// The hwnd of owner of the popup menu. If it is null, hwnd is used
} SPELLCHECKER_POPUPMENU;


/*
Adds a richedit control for the spell checker to check

wParam: SPELLCHECKER_ITEM *
lParam: ignored
return: 0 on success
*/
#define MS_SPELLCHECKER_ADD_RICHEDIT	"SpellChecker/AddRichedit"


/*
Removes a richedit control for the spell checker to check

wParam: HWND
lParam: ignored
return: 0 on success
*/
#define MS_SPELLCHECKER_REMOVE_RICHEDIT	"SpellChecker/RemoveRichedit"


/*
Show context menu

wParam: SPELLCHECKER_POPUPMENU 
lParam: ignored
return: the control id selected by the user, 0 if no one was selected, < 0 on error
*/
#define MS_SPELLCHECKER_SHOW_POPUP_MENU	"SpellChecker/ShowPopupMenu"




#endif // __M_SPELLCHECKER_H__
