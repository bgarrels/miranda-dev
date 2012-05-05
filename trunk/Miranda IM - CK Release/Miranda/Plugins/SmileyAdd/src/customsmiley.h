/*
SmileyAdd Plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author 
			Copyright (C) 2006 - 2012 Boris Krasnovskiy

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

#include "general.h"
#include "smileys.h"

#ifndef SMILEYADD_CUSTOMSMILEY_H_
#define SMILEYADD_CUSTOMSMILEY_H_

class SmileyCType : public SmileyType
{
public:
	SmileyCType(const bkstring& fullpath, const TCHAR* filepath);

	bool CreateTriggerText(char* text);
};

class SmileyPackCType
{
public:
	typedef SMOBJLIST<SmileyCType> SmileyVectorType;
	typedef SMOBJLIST<SmileyLookup> SmileyLookupType;

private:
	SmileyVectorType m_SmileyList;
	SmileyLookupType m_SmileyLookup;

	HANDLE m_id;

	void InsertLookup(SmileyCType& sml, bkstring& lk, bool first);
	void AddTriggersToSmileyLookup(void);

public:
	SmileyVectorType& GetSmileyList(void) { return m_SmileyList; }
	SmileyLookupType& GetSmileyLookup(void) { return m_SmileyLookup; }

	int SmileyCount(void) const { return m_SmileyList.getCount(); }

	SmileyCType* GetSmiley(unsigned index) { return &m_SmileyList[index]; }

	HANDLE GetId(void) { return m_id; }
	void SetId(HANDLE id) { m_id = id; }

	bool LoadSmileyDir(TCHAR* dir);
	bool LoadSmiley(TCHAR* path);
};


class SmileyPackCListType 
{
public:
	typedef SMOBJLIST<SmileyPackCType> SmileyPackVectorType;

private:
	SmileyPackVectorType m_SmileyPacks;

public:
	int NumberOfSmileyPacks(void) { return m_SmileyPacks.getCount(); }

	bool AddSmileyPack(HANDLE hContact, TCHAR* dir);
	bool AddSmiley(HANDLE hContact, TCHAR* path);

	void ClearAndFreeAll(void) { 	m_SmileyPacks.destroy(); }

	SmileyPackCType* GetSmileyPack(HANDLE id);
};

extern SmileyPackCListType g_SmileyPackCStore;

#endif
