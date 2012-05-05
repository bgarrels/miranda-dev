#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

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

struct Suggestions {
	TCHAR ** words;
	size_t count;
};


// A Dictionary interface
// All dictionaries use a lazy interface
class Dictionary {
public:
	TCHAR language[128];
	TCHAR localized_name[128];
	TCHAR english_name[128];
	TCHAR full_name[256];
	TCHAR source[128];
	AutoReplaceMap *autoReplace;

	virtual ~Dictionary() {}

	// Return TRUE if the word is correct
	virtual BOOL spell(const TCHAR *) =0;

	// Return a list of suggestions to a word
	virtual Suggestions suggest(const TCHAR * word) =0;

	// Return a list of auto suggestions to a word
	virtual Suggestions autoSuggest(const TCHAR * word) =0;

	// Return a auto suggestions to a word
	// You have to free the item
	virtual TCHAR * autoSuggestOne(const TCHAR * word) =0;

	// Return TRUE if the char is a word char
	virtual BOOL isWordChar(TCHAR c) =0;

	// Add a word to the user custom dict
	virtual void addWord(const TCHAR * word) =0;

	// Add a word to the list of ignored words
	virtual void ignoreWord(const TCHAR * word) =0;

	// Assert that all needed data is loaded
	virtual void load() =0;

	// Return TRUE if the dict is fully loaded
	virtual BOOL isLoaded() =0;
};



// Return a list of avaible languages
void GetAvaibleDictionaries(LIST<Dictionary> &dicts, TCHAR *path, TCHAR *user_path);

// Free the list returned by GetAvaibleDictionaries
void FreeDictionaries(LIST<Dictionary> &dicts);

// Free the list returned by GetAvaibleDictionaries
void FreeSuggestions(Suggestions &suggestions);


	
#endif // __DICTIONARY_H__
