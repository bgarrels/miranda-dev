/*
Basic History plugin
Copyright (C) 2011 Krzysztof Kral

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
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

