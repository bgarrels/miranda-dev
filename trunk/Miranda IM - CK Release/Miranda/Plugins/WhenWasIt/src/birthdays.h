#ifndef M_WWI_BIRTHDAYS_H
#define M_WWI_BIRTHDAYS_H

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

struct TBirthdayContact{
	HANDLE hContact;
	HANDLE hClistIcon;
};

typedef TBirthdayContact *PBirthdayContact;

class CBirthdays
{
	protected:
		int count;
		int size;
		PBirthdayContact *birthdays;
		int advancedIcon;
		
		void Realloc(int increaseCapacity);
		void EnsureCapacity();
		
		void ClearItem(int index);
	
	public:
		CBirthdays(int initialSize = 10);
		~CBirthdays();
		
		int Add(HANDLE hContact, HANDLE hClistIcon);
		int Remove(int index);
		int Remove(HANDLE hContact);
		void Clear();
		void Destroy();
		
		int Index(HANDLE hContact) const;
		int Contains(HANDLE hContact) const;
		
		void SetAdvancedIconIndex(int advIcon);
		int GetAdvancedIconIndex() const;
		
		HANDLE GetClistIcon(HANDLE hContact) const;
		
		int Count() const;
		int Size() const;
};

extern CBirthdays &birthdays;

#endif //M_WWI_BIRTHDAYS_H
