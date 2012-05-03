/*
Twitter protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Auhor		Copyright � 2009-2012 Jim Porter

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

#include "common.h"
#include "utility.h"
//#include "tc2.h"
#include "stdafx.h"
#include "oauth.h"

#include <m_protoint.h>

class TwitterProto : public PROTO_INTERFACE
{
public:
	TwitterProto(const char *,const TCHAR *);
	~TwitterProto();

	__inline void* operator new(size_t size)
	{
		return calloc(1,size);
	}
	__inline void operator delete(void *p)
	{
		free(p);
	}

	inline const char * ModuleName() const
	{
		return m_szModuleName;
	}

	//PROTO_INTERFACE

	virtual	HANDLE __cdecl AddToList(int,PROTOSEARCHRESULT *);
	virtual	HANDLE __cdecl AddToListByEvent(int,int,HANDLE);

	virtual	int    __cdecl Authorize(HANDLE);
	virtual	int    __cdecl AuthDeny(HANDLE,const char *);
	virtual	int    __cdecl AuthRecv(HANDLE,PROTORECVEVENT *);
	virtual	int    __cdecl AuthRequest(HANDLE,const char *);

	virtual	HANDLE __cdecl ChangeInfo(int,void *);

	virtual	HANDLE __cdecl FileAllow(HANDLE,HANDLE,const char *);
	virtual	int    __cdecl FileCancel(HANDLE,HANDLE);
	virtual	int    __cdecl FileDeny(HANDLE,HANDLE,const char *);
	virtual	int    __cdecl FileResume(HANDLE,int *,const char **);

	virtual	DWORD_PTR  __cdecl GetCaps(int,HANDLE = 0);
	virtual	HICON  __cdecl GetIcon(int);
	virtual	int    __cdecl GetInfo(HANDLE,int);

	virtual	HANDLE __cdecl SearchBasic(const char *);
	virtual	HANDLE __cdecl SearchByEmail(const char *);
	virtual	HANDLE __cdecl SearchByName(const char *,const char *,const char *);
	virtual	HWND   __cdecl SearchAdvanced(HWND);
	virtual	HWND   __cdecl CreateExtendedSearchUI(HWND);

	virtual	int    __cdecl RecvContacts(HANDLE,PROTORECVEVENT *);
	virtual	int    __cdecl RecvFile(HANDLE,PROTORECVFILE *);
	virtual	int    __cdecl RecvMsg(HANDLE,PROTORECVEVENT *);
	virtual	int    __cdecl RecvUrl(HANDLE,PROTORECVEVENT *);

	virtual	int    __cdecl SendContacts(HANDLE,int,int,HANDLE *);
	virtual	HANDLE __cdecl SendFile(HANDLE,const char *,char **);
	virtual	int    __cdecl SendMsg(HANDLE,int,const char *);
	virtual	int    __cdecl SendUrl(HANDLE,int,const char *);

	virtual	int    __cdecl SetApparentMode(HANDLE,int);
	virtual	int    __cdecl SetStatus(int);

	virtual	HANDLE __cdecl GetAwayMsg(HANDLE);
	virtual	int    __cdecl RecvAwayMsg(HANDLE,int,PROTORECVEVENT *);
	virtual	int    __cdecl SendAwayMsg(HANDLE,HANDLE,const char *);
	virtual	int    __cdecl SetAwayMsg(int,const char *);

	virtual	int    __cdecl UserIsTyping(HANDLE,int);

	virtual	int    __cdecl OnEvent(PROTOEVENTTYPE,WPARAM,LPARAM);

	void UpdateSettings();

	// Services
	int  __cdecl SvcCreateAccMgrUI(WPARAM,LPARAM);
	int  __cdecl GetName(WPARAM,LPARAM);
	int  __cdecl GetStatus(WPARAM,LPARAM);
	INT_PTR  __cdecl ReplyToTweet(WPARAM,LPARAM);
	INT_PTR  __cdecl VisitHomepage(WPARAM,LPARAM);
	int  __cdecl GetAvatar(WPARAM,LPARAM);
	int  __cdecl SetAvatar(WPARAM,LPARAM);

	// Events
	int  __cdecl OnContactDeleted(WPARAM,LPARAM);
	int  __cdecl OnBuildStatusMenu(WPARAM,LPARAM);
	int  __cdecl OnOptionsInit(WPARAM,LPARAM);
	int  __cdecl OnTweet(WPARAM,LPARAM);
	int  __cdecl OnModulesLoaded(WPARAM,LPARAM);
	int  __cdecl OnPreShutdown(WPARAM,LPARAM);
	int  __cdecl OnPrebuildContactMenu(WPARAM,LPARAM);
	int  __cdecl OnChatOutgoing(WPARAM,LPARAM);
	int  __cdecl OnJoinChat(WPARAM,LPARAM);
	int  __cdecl OnLeaveChat(WPARAM,LPARAM);

	void __cdecl SendTweetWorker(void *);
private:
	// Worker threads
	void __cdecl AddToListWorker(void *p);
	void __cdecl SendSuccess(void *);
	void __cdecl DoSearch(void *);
	void __cdecl SignOn(void *);
	void __cdecl MessageLoop(void *);
	void __cdecl GetAwayMsgWorker(void *);
	void __cdecl UpdateAvatarWorker(void *);
	void __cdecl UpdateInfoWorker(void *);

	bool NegotiateConnection();

	void UpdateStatuses(bool pre_read,bool popups, bool tweetToMsg);
	void UpdateMessages(bool pre_read);
	void UpdateFriends();
	void UpdateAvatar(HANDLE,const std::string &,bool force=false);

	int ShowPinDialog();
	void ShowPopup(const wchar_t *, int Error = 0);
	void ShowPopup(const char *, int Error = 0);
	void ShowContactPopup(HANDLE,const std::string &);

	bool IsMyContact(HANDLE,bool include_chat = false);
	HANDLE UsernameToHContact(const char *);
	HANDLE AddToClientList(const char *,const char *);
	void SetAllContactStatuses(int);

	INT_PTR LOG(const char *fmt,...);
	INT_PTR WLOG(const char* first, const wstring last);
	static void CALLBACK APC_callback(ULONG_PTR p);

	void UpdateChat(const twitter_user &update);
	void AddChatContact(const char *name,const char *nick=0);
	void DeleteChatContact(const char *name);
	void SetChatStatus(int);

	void TwitterProto::resetOAuthKeys();


	std::string GetAvatarFolder();

	HANDLE signon_lock_;
	HANDLE avatar_lock_;
	HANDLE twitter_lock_;

	HANDLE hNetlib_;
	HANDLE hAvatarNetlib_;
	HANDLE hMsgLoop_;
	mir_twitter twit_;

	twitter_id since_id_;
	twitter_id dm_since_id_;

	std::string def_avatar_folder_;
	HANDLE hAvatarFolder_;

	bool in_chat_;

	int disconnectionCount;

	//mirandas keys
	wstring ConsumerKey;
	wstring ConsumerSecret;

	// various twitter api URLs
	wstring AccessUrl;
	wstring AuthorizeUrl;
	wstring RequestUrl;
	wstring UserTimelineUrl;
};

// TODO: remove this
inline std::string profile_base_url(const std::string &url)
{
	size_t x = url.find("://");
	if(x == std::string::npos)
		return url.substr(0,url.find('/')+1);
	else
		return url.substr(0,url.find('/',x+3)+1);
}