#ifndef __OPTIONS_H__
#define __OPTIONS_H__

/*
Spellchecker plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2009-2010 Ricardo Pescuma Domenecci

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

#define POPUP_ACTION_DONOTHING 0
#define POPUP_ACTION_CLOSEPOPUP 1
#define POPUP_ACTION_OPENHISTORY 2

#define POPUP_DELAY_DEFAULT 0
#define POPUP_DELAY_CUSTOM 1
#define POPUP_DELAY_PERMANENT 2


struct Options {
	TCHAR default_language[32];
	BOOL auto_replace_dict;
	BOOL auto_replace_user;
	BOOL ignore_uppercase;
	BOOL ignore_with_numbers;
	BOOL ask_when_sending_with_error;

	WORD underline_type;
	BOOL cascade_corrections;
	BOOL show_all_corrections;
	BOOL show_wrong_word;
	BOOL use_flags;
	BOOL auto_locale;
	BOOL use_other_apps_dicts;
	BOOL handle_underscore;
};

extern Options opts;


// Initializations needed by options
void InitOptions();

// Deinitializations needed by options
void DeInitOptions();


// Loads the options from DB
// It don't need to be called, except in some rare cases
void LoadOptions();



#endif // __OPTIONS_H__
