#ifndef _SUBST_INC
#define _SUBST_INC

/*
TipperYM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
            Copyright (C) 2006-2009 Scott Ellis
            Copyright (C) 2007-2011 Jan Holub

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
#include "translations.h"

bool GetLabelText(HANDLE hContact, const DISPLAYITEM &di, TCHAR *buff, int iBufflen);
bool GetValueText(HANDLE hContact, const DISPLAYITEM &di, TCHAR *buff, int iBufflen);

void StripBBCodesInPlace(TCHAR *text);
int ProtoServiceExists(const char *szModule, const char *szService); 

// can be used with hContact == 0 to get uid for a given proto
bool UidName(char *szProto, TCHAR *buff, int bufflen); 
bool Uid(HANDLE hContact, char *szProto, TCHAR *buff, int bufflen);

// get info for status and tray tooltip
bool DBGetContactSettingAsString(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen); 
bool CanRetrieveStatusMsg(HANDLE hContact, char *szProto);
TCHAR *GetProtoStatusMessage(char *szProto, WORD status);
TCHAR *GetProtoExtraStatusTitle(char *szProto);
TCHAR *GetProtoExtraStatusMessage(char *szProto); 
TCHAR *GetListeningTo(char *szProto);
TCHAR *GetJabberAdvStatusText(char *szProto, const char *szSlot, const char *szValue); 
HICON GetJabberActivityIcon(HANDLE hContact, char *szProto); 

#endif
