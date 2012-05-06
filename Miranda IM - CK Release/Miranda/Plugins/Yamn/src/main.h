#ifndef __MAIN_H
#define __MAIN_H

/*
YAMN plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2002-2004 majvan
			Copyright (C) 2005-2007 tweety y_b

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

//For updater
//#define YAMN_9x
#ifndef WIN2IN1
#ifdef YAMN_9x
	#define YAMN_SHORTNAME "YAMN Release"
	#define YAMN_FILENAME "yamn_9x"
#else
	#define YAMN_SHORTNAME "YAMN Release"
	#define YAMN_FILENAME "yamn"
#endif
#else
	#define YAMN_SHORTNAME "YAMN Release"
	#define YAMN_FILENAME "yamn"
#endif //WIN2IN1

#include "version.h"
#define YAMN_NEWMAILSNDDESC		"YAMN: new mail message"
#define YAMN_CONNECTFAILSNDDESC	"YAMN: connect failed"
#define	YAMN_CONNECTFAILSOUND	"YAMN/Sound/ConnectFail"
#define	YAMN_NEWMAILSOUND		"YAMN/Sound/NewMail"

#define YAMN_DBMODULE		"YAMN"
#define YAMN_DBPOSX			"MailBrowserWinX"
#define YAMN_DBPOSY			"MailBrowserWinY"
#define YAMN_DBSIZEX		"MailBrowserWinW"
#define YAMN_DBSIZEY		"MailBrowserWinH"
#define YAMN_DBMSGPOSX		"MailMessageWinX"
#define YAMN_DBMSGPOSY		"MailMessageWinY"
#define YAMN_DBMSGSIZEX		"MailMessageWinW"
#define YAMN_DBMSGSIZEY		"MailMessageWinH"
#define YAMN_DBMSGPOSSPLIT	"MailMessageSplitY"
#define YAMN_HKCHECKMAIL	"HKCheckMail"
#define	YAMN_TTBFCHECK		"ForceCheckTTB"
#define	YAMN_SHOWMAINMENU	"ShowMainMenu"
#define	YAMN_CLOSEDELETE	"CloseOnDelete"
#define YAMN_SHOWASPROTO	"ShowAsProtcol"
#define	YAMN_DBTIMEOPTIONS	"MailBrowserTimeOpts"

#define YAMN_DEFAULTHK		MAKEWORD(VK_F11,MOD_CONTROL)

#define SHOWDATELONG		0x01
#define SHOWDATENOTODAY		0x02
#define SHOWDATENOSECONDS	0x04

extern unsigned char optDateTime;

void UnloadPlugins();

//  Loading Icon and checking for icolib 
void LoadIcons();
extern int iconIndexes[];


#endif

