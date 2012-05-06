/*
ListeningTo plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
                Copyright (C) 2005-2011 Ricardo Pescuma Domenecci
                          (C) 2010-2011 Merlin_de

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

#pragma once


class mRadio : public Player
{
protected:
	virtual void EnableDisable();

	char*	m_proto;
	DWORD	m_version;

	LPTSTR	m_ptszURL;
	HANDLE	m_hContact;									//CurrentStation
	HANDLE	FindContact(LPTSTR ptszURL = NULL);			//Fill

	BOOL	COM_Start();
	BOOL	COM_Stop();
	BOOL	COM_infoCache();							//get the listeningTo info

	HANDLE			m_hRadioStatus;						//HookEventObj on ME_RADIO_STATUS
	int __cdecl		EVT_RadioStatus(WPARAM wParam, LPARAM lParam);

	virtual ~mRadio();


public:
	//common
	mRadio (int index);

	//Plugin ...
	virtual BOOL GetListeningInfo(LISTENINGTOINFO *lti);
};
