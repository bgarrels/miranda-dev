#ifndef NUDGE_H
#define NUDGE_H

/*
Nudge plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	francois.mean@skynet.be
							Sylvain.gougouzian@gmail.com
							wishmaster51@googlemail.com

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

//	NUDGE account status flags
#define	NUDGE_ACC_ST0	0x00000001		//Check (countdown) when Offline
#define	NUDGE_ACC_ST1	0x00000002		//Check (countdown) when Online
#define	NUDGE_ACC_ST2	0x00000004		//Check (countdown) when Away
#define	NUDGE_ACC_ST3	0x00000008		//Check (countdown) when N/A
#define	NUDGE_ACC_ST4	0x00000010		//Check (countdown) when Occupied
#define NUDGE_ACC_ST5	0x00000020		//Check (countdown) when DND
#define NUDGE_ACC_ST6	0x00000040		//Check (countdown) when Free for chat
#define NUDGE_ACC_ST7   0x00000080		//Check (countdown) when Invisible
#define NUDGE_ACC_ST8   0x00000100		//Check (countdown) when On the phone
#define NUDGE_ACC_ST9   0x00000200		//Check (countdown) when Out to lunch

// For status log
#define EVENTTYPE_STATUSCHANGE	25368

#define TEXT_LEN 1024

class CNudge
{
public:
	bool useByProtocol;
	int sendTimeSec;
	int recvTimeSec;
	int resendDelaySec;

	void Load(void);
	void Save(void);
};

class CNudgeElement
{
public:
	char ProtocolName[64];
	TCHAR AccountName[128];
	char NudgeSoundname[100];
	TCHAR recText[TEXT_LEN];
	TCHAR senText[TEXT_LEN];
	bool showPopup;
	bool showStatus;
	bool openMessageWindow;
	bool openContactList;
	bool useIgnoreSettings;
	bool shakeClist;
	bool shakeChat;
	bool enabled;
	bool autoResend;
	DWORD statusFlags;
	int iProtoNumber;
	HANDLE hIcoLibItem;
	HANDLE hEvent;
	HANDLE hContactMenu;

	void Load(void);
	void Save(void);
	int ShowContactMenu(bool show);
};

typedef struct NudgeElementList
{
	CNudgeElement item;
	NudgeElementList *next;
} NUDGEELEMENTLIST;

#endif