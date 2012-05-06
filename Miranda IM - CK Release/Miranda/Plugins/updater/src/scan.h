#ifndef _SCAN_INC
#define _SCAN_INC

/*
Updater plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	2005-2006 Scott Ellis
							2009-2012 Boris Krasnovskiy

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

#include "xmldata.h"
#include "services.h"
#include "utils.h"
#include "allocations.h"
#include "options.h"

void ScanPlugins(FilenameMap *fn_map, UpdateList *update_list);
void ScanLangpacks(FilenameMap *fn_map, UpdateList *update_list);

// returns true if any dll is not 'disabled' in db
bool RearrangeDlls(char *shortName, StrList &filenames);

bool RearrangeLangpacks(char *shortName, StrList &filenames);

#endif
