#ifndef M_WWI_NOTIFIERS_H
#define M_WWI_NOTIFIERS_H

/*
WhenWasIt (birthday reminder) plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2006 Cristian Libotean

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

#ifdef _UNICODE
#define POPUPDATAT POPUPDATAW
#define PUAddPopUpT PUAddPopUpW
#else
#define POPUPDATAT POPUPDATAEX
#define PUAddPopUpT PUAddPopUpEx
#endif

#define BIRTHDAY_TODAY_SOUND "WWIBirthdayToday"
#define BIRTHDAY_NEAR_SOUND "WWIBirthdayNear"

#define BIRTHDAY_NEAR_DEFAULT_DAYS 3

#define DUMMY_MODULE ModuleName
#define DUMMY_SETTING "refreshIconsDummyVal"

struct TUpcomingBirthday{
	HANDLE hContact;
	TCHAR *name;
	TCHAR *message;
	int dtb;
	int age;
};

typedef TUpcomingBirthday *PUpcomingBirthday;

int PopupNotifyNoBirthdays();
int PopupNotifyBirthday(HANDLE hContact, int dtb, int age);
int PopupNotifyMissedBirthday(HANDLE hContact, int dab, int age);
int ClistIconNotifyBirthday(HANDLE hContact, int dtb, int advancedIcon);
int DialogNotifyBirthday(HANDLE hContact, int dtb, int age);
int DialogNotifyMissedBirthday(HANDLE hContact, int dab, int age);
int SoundNotifyBirthday(int dtb);

int ClearClistIcon(HANDLE hContact, int advancedIcon);

int RefreshContactListIcons(HANDLE hContact);
int RefreshAllContactListIcons(int oldClistIcon = -1);

#endif //M_WWI_NOTIFIERS_H