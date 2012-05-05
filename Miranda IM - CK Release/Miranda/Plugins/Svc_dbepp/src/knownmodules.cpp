/*
Database Editor++ for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2003-2011 Bio, Jonathan Gordon

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

#include "headers.h"

BYTE UseKnownModList;

#define MAXMODS 1024
char *KnownModules[MAXMODS];
int KnownModulesCount = 0;

INT_PTR RegisterModule(WPARAM wParam, LPARAM lParam)
{
	char **mods = (char**)wParam;
	int count = lParam;
	int i;
	for (i=0;i<count && KnownModulesCount<MAXMODS;i++)
		KnownModules[KnownModulesCount++] = mir_tstrdup(mods[i]);
	return 0;
}

INT_PTR RegisterSingleModule(WPARAM wParam, LPARAM lParam)
{
	char *mods = (char*)wParam;
	if (KnownModulesCount<MAXMODS)
		KnownModules[KnownModulesCount++] = mir_tstrdup(mods);
	return 0;
}

int IsModuleKnown(char *module)
{
	int i;

	if (!UseKnownModList) return 1; // not using known list so all are "known"

	for(i=0;i<KnownModulesCount;i++)
	{
		if (KnownModules[i] && !mir_strcmp(module,KnownModules[i]))
			return 1;
	}

	return 0;
}

void FreeKnownModuleList()
{
	int i;
	for(i=0;i<KnownModulesCount;i++)
	{
		mir_free(KnownModules[i]);
	}
}

void doOldKnownModulesList()
{
	ModuleSettingLL msll;
	struct ModSetLinkLinkItem *setting;
	DBVARIANT dbv;
	char *var, *temp;

	if (!EnumSettings(NULL,"KnownModules", &msll)) return;

	setting = msll.first;
	while(setting)
	{
		if (!DBGetContactSetting(NULL,"KnownModules",setting->name,&dbv) && dbv.type == DBVT_ASCIIZ)
		{
			temp = (char*)mir_alloc((strlen(dbv.pszVal)+5)*sizeof(char));
			if (!temp) break;
			strcpy(temp,dbv.pszVal);
			strcat(temp,",\0");
			var = strtok(temp,", ");
			while (var)
			{
				if (KnownModulesCount<MAXMODS)
					KnownModules[KnownModulesCount++] = mir_tstrdup(var);
				var = strtok(NULL,", ");
			}
			mir_free(temp);
		}
		DBFreeVariant(&dbv);
		setting = (struct ModSetLinkLinkItem *)setting->next;
	}
	FreeModuleSettingLL(&msll);

	UseKnownModList = DBGetContactSettingByte(NULL,modname,"UseKnownModList",0);
}
