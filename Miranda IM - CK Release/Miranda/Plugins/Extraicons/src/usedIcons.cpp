/*
Extraicons service plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			 Copyright (C) 2009-2012 Ricardo Pescuma Domenecci

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

#include "commons.h"

struct Icon
{
	string name;
	int refCount;
	HANDLE hImage;

	Icon(const char *icolibName) :
		name(icolibName), refCount(0), hImage(NULL)
	{
	}
};

static vector<Icon> usedIcons;

static Icon * FindIcon(const char *icolibName)
{
	Icon *icon = NULL;

	for (unsigned int i = 0; i < usedIcons.size(); ++i)
	{
		Icon *tmp = &usedIcons[i];
		if (tmp->name != icolibName)
			continue;

		icon = tmp;
		break;
	}

	if (icon == NULL)
	{
		usedIcons.push_back(Icon(icolibName));
		icon = &usedIcons[usedIcons.size() - 1];
	}

	if (icon->hImage == NULL)
	{
		HICON hIcon = IcoLib_LoadIcon(icon->name.c_str());
		if (hIcon != NULL)
		{
			icon->hImage = (HANDLE) CallService(MS_CLIST_EXTRA_ADD_ICON, (WPARAM) hIcon, 0);
			if (icon->hImage == (HANDLE) -1)
				icon->hImage = NULL;

			IcoLib_ReleaseIcon(hIcon);
		}
	}

	return icon;
}

HANDLE GetIcon(const char *icolibName)
{
	return FindIcon(icolibName)->hImage;
}

HANDLE AddIcon(const char *icolibName)
{
	Icon *icon = FindIcon(icolibName);
	icon->refCount++;
	return icon->hImage;
}

void RemoveIcon(const char *icolibName)
{
	for (unsigned int i = 0; i < usedIcons.size(); ++i)
	{
		Icon *icon = &usedIcons[i];

		if (icon->name != icolibName)
			continue;

		icon->refCount--;
		break;
	}
}

static bool NotUsedIcon(const Icon &icon) 
{
	return icon.refCount <= 0;
}

void ResetIcons()
{
	usedIcons.erase(std::remove_if(usedIcons.begin(), usedIcons.end(), NotUsedIcon), usedIcons.end());

	for (unsigned int i = 0; i < usedIcons.size(); ++i)
		usedIcons[i].hImage = NULL;
}

