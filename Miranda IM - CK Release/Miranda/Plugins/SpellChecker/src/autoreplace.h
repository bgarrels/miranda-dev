#ifndef __AUTOREPLACE_H__
#define __AUTOREPLACE_H__

/*
Spellchecker plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2009-2010 Ricardo Pescuma Domenecci

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

struct AutoReplacement
{
	tstring replace;
	BOOL useVariables;

	AutoReplacement();
	AutoReplacement(const TCHAR *replace, BOOL useVariables);
};


class Dictionary;


class AutoReplaceMap 
{
private:
	TCHAR filename[1024];
	Dictionary *dict;
	map<tstring, AutoReplacement> replacements;

	void loadAutoReplaceMap();
	void writeAutoReplaceMap();

public:
	AutoReplaceMap(TCHAR *filename, Dictionary *dict);

	TCHAR *filterText(const TCHAR *find);
	BOOL isWordChar(TCHAR c);

	/// Return an auto replacement to a word or NULL if none exists.
	/// You have to free the item.
	TCHAR * autoReplace(const TCHAR * word);

	/// Add a word to the list of auto-replaced words
	void add(const TCHAR * from, const TCHAR * to, BOOL useVariables = FALSE);

	/// Make a copy of the auto replace map
	void copyMap(map<tstring, AutoReplacement> *replacements);

	/// Make a copy of the auto replace map
	void setMap(const map<tstring, AutoReplacement> &replacements);
};


	
#endif // __AUTOREPLACE_H__
