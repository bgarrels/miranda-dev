/*
Basic History plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2011-2012 Krzysztof Kral

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

#pragma once
#include "SearchContext.h"

class Searcher
{
private:
	SearchContext& context;
	int lastFindSelection;
	int startFindPos;
	int startFindSel;
	bool isFindSelChanged;
	bool findBack, matchCase, matchWholeWords, onlyIn, onlyOut, onlyGroup, searchForInLG, searchForInMes;

	bool CompareStr(std::wstring str, TCHAR *strFind);
	bool IsInSel(int sel, TCHAR *strFind);
public:
	Searcher(SearchContext& _context);
	void Find();
	void ChangeFindDirection(bool isBack);
	void ClearFind();
	void SetMatchCase(bool val){
		matchCase = val;
		ClearFind();
	}
	void SetMatchWholeWords(bool val){
		matchWholeWords = val;
		ClearFind();
	}
	bool IsMatchCase(){
		return matchCase;
	}
	bool IsMatchWholeWords(){
		return matchWholeWords;
	}
	bool IsFindBack(){
		return findBack;
	}
	void SetOnlyIn(bool val){
		onlyIn = val;
		if(val && onlyOut)
			onlyOut = false;
		ClearFind();
	}
	void SetOnlyOut(bool val){
		onlyOut = val;
		if(val && onlyIn)
			onlyIn = false;
		ClearFind();
	}
	bool IsOnlyIn(){
		return onlyIn;
	}
	bool IsOnlyOut(){
		return onlyOut;
	}
	void SetOnlyGroup(bool val){
		onlyGroup = val;
		ClearFind();
	}
	bool IsOnlyGroup(){
		return onlyGroup;
	}
	void SetSearchForInLG(bool val){
		if(searchForInLG != val)
		{
			searchForInLG = val;
			ClearFind();
		}
	}
	void SetSearchForInMes(bool val){
		if(searchForInMes != val)
		{
			searchForInMes = val;
			ClearFind();
		}
	}
	bool IsSearchForInLG(){
		return searchForInLG;
	}
	bool IsSearchForInMes(){
		return searchForInMes;
	}
};

