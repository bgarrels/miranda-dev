/*
UseActions plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2009-2012 2012 Igor Abakumov

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

#define MODULE "UseActions"
#define ICOLIB_ACTSECTION "Actions/Registered actions"
#define ICOLIB_MNUSECTION "Actions/Menu icons"
#define SERVICE_WITH_LPARAM_NAME MODULE"/CallAction"

#define MAX_ACT_DESC_LEN 512
#define MAX_VARSCRIPT_LEN 1024

typedef struct tagMyActionItem{
	DWORD	dwActID;
	WORD	wSortIndex;
	char	szNameID[32];
	TCHAR	tszActDescr[MAX_ACT_DESC_LEN];
	char	szActDescr[MAX_ACT_DESC_LEN];
	HANDLE	hIcolibIcon;
	BOOL	bReggedHotkey;
	HANDLE	hMTBButton;
	HANDLE	hMenuService;
	HANDLE	hMainMenuItem;
	TCHAR	tszMMenuPopup[MAX_ACT_DESC_LEN];
	TCHAR	tszMMenuNameVars[MAX_VARSCRIPT_LEN];
	HANDLE	hMMenuRoot;
	TCHAR	tszMMenuShowWhenVars[MAX_VARSCRIPT_LEN];
	HANDLE	hContactMenuItem;
	TCHAR	tszCMenuNameVars[MAX_VARSCRIPT_LEN];
	TCHAR	tszCMenuPopup[MAX_ACT_DESC_LEN];
	HANDLE	hCMenuRoot;
	TCHAR	tszCMenuShowWhenVars[MAX_VARSCRIPT_LEN];
	HANDLE	hTrayMenuItem;
	TCHAR	tszTMenuPopup[MAX_ACT_DESC_LEN];
	HANDLE	hTMenuRoot;
	HANDLE	hTTBButton;
	WORD	wReggedTabButton;
	WORD	wFlags;
	} MyActionItem;

#define UAF_NONE		0
#define UAF_REGHOTKEY	(1<<0)
#define UAF_REGMTBB		(1<<1)
#define UAF_REGMMENU	(1<<2)
#define UAF_REGCMENU	(1<<3)
#define UAF_REGTTBB		(1<<4)
#define UAF_REGTABB     (1<<5)
// #define UAF_CMENU_CNTCT	(1<<6)
// #define UAF_CMENU_CHAT	(1<<7)

#define UAF_MMENU_VARS	(1<<7)

#define UAF_CMENU_VARS	(1<<8)

#define UAF_REGTMENU	(1<<9)
#define UAF_MMENUSEP	(1<<10)
#define UAF_CMENUSEP	(1<<11)

//#define UAF_MMENU_VARS	(1<<12)
