/*
Author Ricardo Pescuma Domenecci

Miranda IM: the free IM client for Microsoft* Windows*

Copyright 2005-2012 Miranda IM project,
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

#pragma once

#ifndef __POPUP_H__
# define __POPUP_H__

#include <m_popup.h>

extern BOOL EnablePopups;


#define POPUP_TYPE_NORMAL	0
#define POPUP_TYPE_TEST		1
#define POPUP_TYPE_ERROR	2

// Show an popup
void ShowPopup(const char *title, const char *description, int type);

// Show an error popup
void ShowErrPopup(const char *title, const char *description);

// Show an trace popup
void ShowTracePopup(const char *text);






#endif // __POPUP_H__
