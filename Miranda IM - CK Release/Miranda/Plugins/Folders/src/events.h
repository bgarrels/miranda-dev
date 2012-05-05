#ifndef M_FOLDERS_EVENTS_H
#define M_FOLDERS_EVENTS_H

/*
Custom profile folders plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			 Copyright © 2005 Cristian Libotean

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

#include "commonheaders.h"
#include "m_folders.h"

extern HANDLE hPathChanged;

int InitEvents();
int DestroyEvents();

int CallPathChangedEvents();



#endif //M_FOLDERS_EVENTS_H