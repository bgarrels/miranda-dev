#ifndef __font_h__
#define __font_h__

/*
Popup Plus plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	2002 Luca Santarelli
							2004-2007 Victor Pavlychko
							2010-2012 Merlin_de, MPK

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

//basic constants for all popup plugins
#define PU_FNT_AND_COLOR		"Popups"			//common main group for customice\font&color
#define PU_FNT_AND_COLOR_DB		PU_COMMONMODUL		//use eg strcpy(fid.dbSettingsGroup, PU_FNT_GROUP_DB);

#define PU_FNT_PREFIX			"fnt%s"				//use eg mir_snprintf(fid.prefix, sizeof(fid.prefix), PU_FNT_PREFIX, PU_FNT_NAME_....);
#define PU_FNT_NAME_TITLE		"Title"				//use eg lstrcpy(fid.name, _T(FNT_NAME_....)) for FontIDT
#define PU_FNT_NAME_CLOCK		"Clock"
#define PU_FNT_NAME_TEXT		"Text"

#define PU_COL_BACK_NAME		"Background"
#define PU_COL_BACK_SETTING		"ColourBg"
#define PU_COL_BORD_NAME		"Border"
#define PU_COL_BORD_SETTING		"ColourBorder"
#define PU_COL_SIDE_NAME		"Sidebar"
#define PU_COL_SIDE_SETTING		"ColourSidebar"
#define PU_COL_LINE_NAME		"Title underline"
#define PU_COL_LINE_SETTING		"ColourUnderline"
#define PU_COL_AVAT_NAME		"Avatar Border"
#define PU_COL_AVAT_SETTING		"ColourAvatarBorder"

struct PopupFonts
{
	HFONT title;
	HFONT text;
	HFONT clock;
	HFONT action;
	HFONT actionHover;

	COLORREF clTitle;
	COLORREF clText;
	COLORREF clClock;
	COLORREF clBack;
	COLORREF clAction;
	COLORREF clActionHover;
	COLORREF clAvatarBorder;

};

extern PopupFonts fonts;

void InitFonts();

void ReloadFonts();


#endif // __font_h__
