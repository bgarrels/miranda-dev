#ifndef _PRESETITEMS_INC
#define _PRESETITEMS_INC

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

#define MAX_PRESET_SUBST_COUNT	3

typedef struct {
	char *szID;
	TCHAR *swzName;
	TCHAR *swzLabel;
	TCHAR *swzValue;
	char *szNeededSubst[MAX_PRESET_SUBST_COUNT];
} PRESETITEM;

typedef struct {
	char *szID;
	TCHAR *swzName;
	DisplaySubstType type;
	char *szModuleName;
	char *szSettingName;
	int iTranslateFuncId;
} PRESETSUBST;

typedef struct {
	char *szName;
	bool bValueNewline;
} DEFAULTITEM;

extern PRESETITEM presetItems[];
extern PRESETSUBST presetSubsts[];
extern DEFAULTITEM defaultItemList[];

PRESETITEM *GetPresetItemByName(char *szName);
PRESETSUBST *GetPresetSubstByName(char *szName);

#endif