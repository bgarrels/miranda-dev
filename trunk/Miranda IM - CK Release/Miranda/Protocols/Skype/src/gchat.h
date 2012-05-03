/*
Skype protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright © 2008-2012 leecher, tweety, jls17 

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

// m_chat users these BaseTSD types, so if we are compiling with an old PSDK, these typedefs
// are not there, so better define them, just in case...
#ifndef LongToPtr
#define DWORD_PTR DWORD
#endif

#pragma warning (push)
#pragma warning (disable: 4201) // nonstandard extension used : nameless struct/union
#include "m_chat.h"
#pragma warning (pop)

#define MAX_BUF 256    // Buffer for topic-string

typedef struct {
	HANDLE hContact;
	TCHAR who[33];
	TCHAR szRole[12];
} gchat_contact;

typedef struct {
	TCHAR*		   szChatName;		 // name of chat session
	gchat_contact* mJoinedContacts;  //	contacts
	int            mJoinedCount;     // contacts count
} gchat_contacts;

int ChatInit(WPARAM, LPARAM);
int  __cdecl ChatStart(char *szChatId, BOOL bJustCreate);
gchat_contacts *GetChat(TCHAR *szChatId);
HANDLE find_chat(TCHAR *chatname);
#ifdef _UNICODE
HANDLE find_chatA(char *chatname);
#else
#define find_chatA find_chat
#endif
void RemChatContact(gchat_contacts*, const TCHAR*);
gchat_contact *GetChatContact(gchat_contacts *gc, const TCHAR *who);
int AddMembers(char*);
void AddMembersThread(char *szSkypeMsg);
void RemChat(TCHAR *szChatId);
int GCEventHook (WPARAM, LPARAM);
int GCMenuHook (WPARAM, LPARAM);
void KillChatSession(GCDEST*);
INT_PTR GCOnLeaveChat(WPARAM wParam,LPARAM lParam);
INT_PTR GCOnJoinChat(WPARAM wParam,LPARAM lParam);
void GCInit(void);
void GCExit(void);
void SetChatTopic (TCHAR *szChatId, TCHAR *szTopic, BOOL bSet);
