#ifndef _XMLDATA_INC
#define _XMLDATA_INC

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

#include "options.h"
#include "../bzip2-1.0.6/bzlib.h"
#include "utils.h"
#include "popups.h"
#include "services.h"

#define MIM_BACKEND_URL_PREFIX		"http://addons.miranda-im.org/backend/"
#define MIM_DOWNLOAD_URL_PREFIX		"http://addons.miranda-im.org/feed.php?dlfile="
#define MIM_VIEW_URL_PREFIX			"http://addons.miranda-im.org/details.php?action=viewfile&id="
#define MIM_CHANGELOG_URL_PREFIX	"http://addons.miranda-im.org/details.php?action=viewlog&id="
// this is the minimum age (in hours) of the local copy before a new download is allowed
#define	MIN_XMLDATA_AGE		8

void FreeXMLData(const Category cat);
bool OldXMLDataExists(const Category cat);
bool LoadOldXMLData(const Category cat, bool update_age);
bool UpdateXMLData(const Category cat, const char *redirect_url = 0, int recurse_count = 0);
bool XMLDataAvailable(const Category cat);

const char *FindVersion(int fileID, BYTE *pbVersionBytes, int cpbVersionBytes, const Category cat);
int FindFileID(const char *name, const Category cat, UpdateList *update_list);

// update file listing id's for non-plugin AUTOREGISTER components
void UpdateFLIDs(UpdateList &update_list);

#endif
