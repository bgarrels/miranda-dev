#ifndef _SOCKET_INC
#define _SOCKET_INC

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

#include "services.h"
#include "options.h"
#include "xmldata.h"
#include "popups.h"


// gets a file from a url, puts in in the temp_folder and names it after the plugin_name (same extension as url)
// it will automatically unzip a zip file, putting the contents in the temp_folder and using the names from the archive
// if it unzips the file, the zip archive is removed afterward
bool GetFile(char *url, TCHAR *temp_folder, char *plugin_name, char *version, bool dlls_only, int recurse_count = 0);

// check the xml backend data or version URL (depending on whether the plugin is on the file listing) to see if we need an update
// returns new version string (which you must free) and sets beta to indicate where update should be retreived from (i.e. if
// beta url is enabled, will return true in beta unless the non-beta version is higher)
char *UpdateRequired(UpdateInternal &update_internal, bool *beta);

#endif
