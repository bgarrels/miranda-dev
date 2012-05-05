#ifndef SMILEYADD_OPTIONS_H_
#define SMILEYADD_OPTIONS_H_

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

int SmileysOptionsInitialize(WPARAM wParam, LPARAM lParam);

class OptionsType
{
public:
	bool PluginSupportEnabled;
	bool EnforceSpaces;
	bool ScaleToTextheight;
	bool UseOneForAll;
	bool SurroundSmileyWithSpaces;
	bool ScaleAllSmileys;
	bool IEViewStyle;
	bool AnimateSel;
	bool AnimateDlg;
	bool InputSmileys;
	bool DCursorSmiley;
	bool DisableCustom;
    bool HQScaling;
	unsigned ButtonStatus;
	unsigned SelWndBkgClr;
	unsigned MaxCustomSmileySize;
    unsigned MinSmileySize;

	void Load(void);
	void Save(void);
	static void ReadPackFileName(bkstring& filename, const bkstring& name, 
		const bkstring& defaultFilename);
	static void ReadCustomCategories(bkstring& cats);
	static void ReadContactCategory(HANDLE hContact, bkstring& cats);
	static void WritePackFileName(const bkstring& filename, const bkstring& name);
	static void WriteCustomCategories(const bkstring& cats);
	static void WriteContactCategory(HANDLE hContact, const bkstring& cats);
};

extern OptionsType opt;

#endif // SMILEYADD_OPTIONS_H_

