#ifndef _SMILEYROUTINES_
#define _SMILEYROUTINES_

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

#include "smileys.h"
#include "customsmiley.h"

typedef struct ReplaceSmileyType_tag
{
	CHARRANGE loc;
	SmileyType* sml;
	SmileyCType* smlc;
	bool ldspace;
	bool trspace;
} ReplaceSmileyType;

// Queue to store smileys found
typedef SMOBJLIST<ReplaceSmileyType> SmileysQueueType;



void LookupAllSmileys(SmileyPackType* smileyPack, SmileyPackCType* smileyCPack, const TCHAR* lpstrText,
	SmileysQueueType& smllist, const bool firstOnly);
void ReplaceSmileys(HWND hwnd, SmileyPackType* smp, SmileyPackCType* smcp, const CHARRANGE& sel,
	bool useHidden, bool ignoreLast, bool unFreeze);
void ReplaceSmileysWithText(HWND hwnd, CHARRANGE& sel, bool keepFrozen);
void FindSmileyInText(SmileyPackType* smp, const TCHAR* str, 
	unsigned& first, unsigned& size, SmileyType** index);
SmileyType* FindButtonSmiley(SmileyPackType* smp);

#endif
