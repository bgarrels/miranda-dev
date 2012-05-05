#ifndef __ARDIALOG_H__
#define __ARDIALOG_H__

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

typedef void (*AutoReplaceDialogCallback)(BOOL canceled, Dictionary *dict, 
										  const TCHAR *find, const TCHAR *replace, BOOL useVariables, 
										  const TCHAR *original_find, void *param);

BOOL ShowAutoReplaceDialog(HWND parent, BOOL modal, 
						   Dictionary *dict, const TCHAR *find, const TCHAR *replace, BOOL useVariables,
						   BOOL findReadOnly, AutoReplaceDialogCallback callback, void *param);


#endif // __ARDIALOG_H__
