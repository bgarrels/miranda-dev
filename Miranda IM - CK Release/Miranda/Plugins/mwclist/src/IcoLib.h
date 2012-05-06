/*
Author Artem Shpynov aka FYR

Miranda IM: the free IM client for Microsoft* Windows*

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

===============================================================================
*/

typedef struct {
	int cbSize;
    char *pszSection;        //section name used to group icons
	char *pszDescription;	   //description for options dialog
	char *pszName;		   //name to refer to icon when playing and in db
    char *pszDefaultFile;    //default icon file to use
	int  iDefaultIndex;
} SKINICONDESC;

typedef struct {
	int cbSize;
    char *pszSection;        //section name used to group icons
	char *pszDescription;	   //description for options dialog
	char *pszName;		   //name to refer to icon when playing and in db
    char *pszDefaultFile;    //default icon file to use
	int  iDefaultIndex;
	HICON hDefaultIcon;
} SKINICONDESC2;

//
//  Add a icon into options UI
//
//  wParam = (WPARAM)0
//  lParam = (LPARAM)(SKINICONDESC*)sid;
//
#define MS_SKIN2_ADDICON "Skin2/Icons/AddIcon"
//
//  Retrieve HICON with name specified in lParam
//  Returned HICON SHOULDN'T be destroyed, it managed by IcoLib
//
#define MS_SKIN2_GETICON "Skin2/Icons/GetIcon"
//
//  Icons change notification
//
#define ME_SKIN2_ICONSCHANGED "Skin2/IconsChanged"
