/*
Facebook plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			(C) 2009-2011 Michal Zelinka
			(C) 2011-2012 Robert Pösel

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

#include "common.h"

FacebookProto::FacebookProto(const char* proto_name,const TCHAR* username)
{
	m_iVersion = 2;
	m_szProtoName  = mir_strdup( proto_name );
	m_szModuleName = mir_strdup( proto_name );
	m_tszUserName  = mir_tstrdup( username );

	facy.parent = this;

	signon_lock_ = CreateMutex( NULL, FALSE, NULL );
	avatar_lock_ = CreateMutex( NULL, FALSE, NULL );
	log_lock_ = CreateMutex( NULL, FALSE, NULL );
	update_loop_lock_ = CreateEvent( NULL, FALSE, FALSE, NULL);
	facy.buddies_lock_ = CreateMutex( NULL, FALSE, NULL );
	facy.send_message_lock_ = CreateMutex( NULL, FALSE, NULL );
	facy.fcb_conn_lock_ = CreateMutex( NULL, FALSE, NULL );

	CreateProtoService(m_szModuleName, PS_CREATEACCMGRUI, &FacebookProto::SvcCreateAccMgrUI, this);
	CreateProtoService(m_szModuleName, PS_GETMYAWAYMSG,   &FacebookProto::GetMyAwayMsg,      this);
	CreateProtoService(m_szModuleName, PS_GETMYAVATAR,    &FacebookProto::GetMyAvatar,       this);
	CreateProtoService(m_szModuleName, PS_GETAVATARINFO,  &FacebookProto::GetAvatarInfo,     this);
	CreateProtoService(m_szModuleName, PS_GETAVATARCAPS,  &FacebookProto::GetAvatarCaps,     this);

	CreateProtoService(m_szModuleName, PS_JOINCHAT,  &FacebookProto::OnJoinChat,  this);
	CreateProtoService(m_szModuleName, PS_LEAVECHAT, &FacebookProto::OnLeaveChat, this);

	HookProtoEvent(ME_CLIST_PREBUILDSTATUSMENU,	&FacebookProto::OnBuildStatusMenu,	this);
	HookProtoEvent(ME_OPT_INITIALISE,			&FacebookProto::OnOptionsInit,		this);
	HookProtoEvent(ME_GC_EVENT,					&FacebookProto::OnChatOutgoing,		this);
	HookProtoEvent(ME_IDLE_CHANGED,             &FacebookProto::OnIdleChanged,		this);

	// Create standard network connection
	TCHAR descr[512];
	NETLIBUSER nlu = {sizeof(nlu)};
	nlu.flags = NUF_INCOMING | NUF_OUTGOING | NUF_HTTPCONNS | NUF_TCHAR;
	nlu.szSettingsModule = m_szModuleName;
	char module[512];
	mir_snprintf(module,SIZEOF(module),"%sAv",m_szModuleName);
	nlu.szSettingsModule = module;
	mir_sntprintf(descr,SIZEOF(descr),TranslateT("%s server connection"),m_tszUserName);
	nlu.ptszDescriptiveName = descr;
	m_hNetlibUser = (HANDLE)CallService(MS_NETLIB_REGISTERUSER,0,(LPARAM)&nlu);
	if(m_hNetlibUser == NULL)
		MessageBox(NULL,TranslateT("Unable to get Netlib connection for Facebook"),m_tszUserName,MB_OK);

	facy.set_handle(m_hNetlibUser);

	SkinAddNewSoundExT( "Notification", m_tszUserName, LPGENT( "Notification" ) );
	SkinAddNewSoundExT( "NewsFeed", m_tszUserName, LPGENT( "News Feed" ) );
	SkinAddNewSoundExT( "OtherEvent", m_tszUserName, LPGENT( "Other Event" ) );

	char *profile = Utils_ReplaceVars("%miranda_avatarcache%");
	def_avatar_folder_ = std::string(profile)+"\\"+m_szModuleName;
	mir_free(profile);
	hAvatarFolder_ = FoldersRegisterCustomPath(m_szModuleName, "Avatars", def_avatar_folder_.c_str());

	// Set all contacts offline -- in case we crashed
	SetAllContactStatuses( ID_STATUS_OFFLINE );
}

FacebookProto::~FacebookProto( )
{
	Netlib_CloseHandle( m_hNetlibUser );

	WaitForSingleObject( signon_lock_, IGNORE );
	WaitForSingleObject( avatar_lock_, IGNORE );
	WaitForSingleObject( log_lock_, IGNORE );
	WaitForSingleObject( facy.buddies_lock_, IGNORE );
	WaitForSingleObject( facy.send_message_lock_, IGNORE );

	CloseHandle( signon_lock_ );
	CloseHandle( avatar_lock_ );
	CloseHandle( log_lock_ );
	CloseHandle( update_loop_lock_ );
	//CloseHandle( this->message_loop_lock_ );
	CloseHandle( facy.buddies_lock_ );
	CloseHandle( facy.send_message_lock_ );
	CloseHandle( facy.fcb_conn_lock_ );

	mir_free( m_tszUserName );
	mir_free( m_szModuleName );
	mir_free( m_szProtoName );
}

//////////////////////////////////////////////////////////////////////////////

DWORD_PTR FacebookProto::GetCaps( int type, HANDLE hContact )
{
	switch(type)
	{
	case PFLAGNUM_1: // TODO: Other caps available: PF1_BASICSEARCH, PF1_SEARCHBYEMAIL
		if ( getByte( FACEBOOK_KEY_SET_MIRANDA_STATUS, 0 ) )
			return PF1_IM | PF1_CHAT | PF1_SERVERCLIST | PF1_MODEMSG;
		else
			return PF1_IM | PF1_CHAT | PF1_SERVERCLIST | PF1_MODEMSGRECV;
	case PFLAGNUM_2:
		return PF2_ONLINE | PF2_INVISIBLE | PF2_ONTHEPHONE | PF2_IDLE; // | PF2_SHORTAWAY;
	case PFLAGNUM_3:
		if ( getByte( FACEBOOK_KEY_SET_MIRANDA_STATUS, 0 ) )
			return PF2_ONLINE; // | PF2_SHORTAWAY;
		else
			return 0;
	case PFLAGNUM_4:
		return PF4_FORCEAUTH | PF4_NOCUSTOMAUTH | PF4_SUPPORTIDLE | PF4_IMSENDUTF | PF4_AVATARS | PF4_SUPPORTTYPING | PF4_NOAUTHDENYREASON | PF4_IMSENDOFFLINE;
	case PFLAGNUM_5:
		return PF2_ONTHEPHONE;
	case PFLAG_MAXLENOFMESSAGE:
		return FACEBOOK_MESSAGE_LIMIT;
	case PFLAG_UNIQUEIDTEXT:
		return (int) "Facebook ID";
	case PFLAG_UNIQUEIDSETTING:
		return (int) FACEBOOK_KEY_ID;
	}
	return 0;
}

HICON FacebookProto::GetIcon(int index)
{
	if(LOWORD(index) == PLI_PROTOCOL)
	{
		HICON ico = (HICON)CallService(MS_SKIN2_GETICON,0,(LPARAM)"Facebook_facebook");
		return CopyIcon(ico);
	} else {
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////

int FacebookProto::SetStatus( int new_status )
{
	LOG("===== Beginning SetStatus process");
	
	// Routing statuses not supported by Facebook
	switch ( new_status )
	{
	case ID_STATUS_INVISIBLE:
	case ID_STATUS_OFFLINE:	
		m_iDesiredStatus = new_status;
		break;

	// TODO RM: needed/useful?
	case ID_STATUS_CONNECTING:
		m_iDesiredStatus = ID_STATUS_OFFLINE;
		break;

	case ID_STATUS_IDLE:	
	default:
		m_iDesiredStatus = ID_STATUS_INVISIBLE;
		if (DBGetContactSettingByte(NULL,m_szModuleName,FACEBOOK_KEY_MAP_STATUSES, DEFAULT_MAP_STATUSES))
			break;
	case ID_STATUS_ONLINE:
	case ID_STATUS_FREECHAT:
		m_iDesiredStatus = ID_STATUS_ONLINE;
		break;
	}

	if ( m_iStatus == ID_STATUS_CONNECTING )
	{
		LOG("===== Status is connecting, no change");
		return 0;
	}

	if ( m_iStatus == m_iDesiredStatus)
	{
		LOG("===== Statuses are same, no change");
		return 0;
	}

	facy.invisible_ = ( new_status == ID_STATUS_INVISIBLE );
  
	ForkThread( &FacebookProto::ChangeStatus, this );

	return 0;
}

int FacebookProto::SetAwayMsg( int status, const PROTOCHAR *msg )
{
	if (!msg)
	{
		last_status_msg_.clear();
		return 0;
	}

	char *narrow = mir_utf8encodeT(msg);
	if (last_status_msg_ != narrow) last_status_msg_ = narrow;
	utils::mem::detract(narrow);

	if (isOnline() && getByte(FACEBOOK_KEY_SET_MIRANDA_STATUS, DEFAULT_SET_MIRANDA_STATUS))
	{
		ForkThread(&FacebookProto::SetAwayMsgWorker, this, NULL);
	}
	return 0;
}

void FacebookProto::SetAwayMsgWorker(void *)
{
	if ( !last_status_msg_.empty() )
		facy.set_status( last_status_msg_ );
}

//////////////////////////////////////////////////////////////////////////////
// SERVICES

int FacebookProto::GetMyAwayMsg( WPARAM wParam, LPARAM lParam )
{
	DBVARIANT dbv = { DBVT_TCHAR };
	if ( !getTString( "StatusMsg", &dbv ) && lstrlen( dbv.ptszVal ) != 0 )
	{
		int res = (lParam & SGMA_UNICODE) ? (INT_PTR)mir_t2u(dbv.ptszVal) : (INT_PTR)mir_t2a(dbv.ptszVal);
		DBFreeVariant( &dbv );
		return res;
	} else {
		return 0;
	}
}

int FacebookProto::OnIdleChanged( WPARAM wParam, LPARAM lParam )
{
	if (m_iStatus == ID_STATUS_INVISIBLE || m_iStatus <= ID_STATUS_OFFLINE)
		return 0;

	bool bIdle = (lParam & IDF_ISIDLE) != 0;
	bool bPrivacy = (lParam & IDF_PRIVACY) != 0;

	if (facy.is_idle_ && !bIdle)
	{
		facy.is_idle_ = false;
		SetStatus(m_iDesiredStatus);
	}
	else if (!facy.is_idle_ && bIdle && !bPrivacy && m_iDesiredStatus != ID_STATUS_INVISIBLE)
	{
		facy.is_idle_ = true;
		SetStatus(ID_STATUS_IDLE);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////

int FacebookProto::OnEvent(PROTOEVENTTYPE event,WPARAM wParam,LPARAM lParam)
{
	switch(event)
	{
	case EV_PROTO_ONLOAD:
		return OnModulesLoaded(wParam,lParam);

	case EV_PROTO_ONEXIT:
		return OnPreShutdown(wParam,lParam);
	
	case EV_PROTO_ONOPTIONS:
		return OnOptionsInit(wParam,lParam);

	case EV_PROTO_ONCONTACTDELETED:
 		return OnContactDeleted(wParam,lParam);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
// EVENTS

int FacebookProto::SvcCreateAccMgrUI(WPARAM wParam,LPARAM lParam)
{
	return (int)CreateDialogParam(g_hInstance,MAKEINTRESOURCE(IDD_FACEBOOKACCOUNT), 
		 (HWND)lParam, FBAccountProc, (LPARAM)this );
}

int FacebookProto::OnModulesLoaded(WPARAM wParam,LPARAM lParam)
{
	// Register group chat
	GCREGISTER gcr = {sizeof(gcr)};
	gcr.dwFlags = 0; //GC_ACKMSG;
	gcr.pszModule = m_szModuleName;
	gcr.pszModuleDispName = m_szModuleName;
	gcr.iMaxText = FACEBOOK_MESSAGE_LIMIT;
	gcr.nColors = 0;
	gcr.pColors = NULL;
	CallService(MS_GC_REGISTER,0,reinterpret_cast<LPARAM>(&gcr));

	return 0;
}

int FacebookProto::OnPreShutdown(WPARAM wParam,LPARAM lParam)
{
	SetStatus( ID_STATUS_OFFLINE );
	return 0;
}

int FacebookProto::OnPrebuildContactMenu(WPARAM wParam,LPARAM lParam)
{
	HANDLE hContact = reinterpret_cast<HANDLE>(wParam);
	if(IsMyContact(hContact/*, true*/)) {
		bool hide = (DBGetContactSettingDword(hContact, m_szModuleName, FACEBOOK_KEY_DELETED, 0)
			|| DBGetContactSettingDword(hContact, m_szModuleName, FACEBOOK_KEY_CONTACT_TYPE, 0) );
		ShowContactMenus(true, hide);
	}

	return 0;
}

int FacebookProto::OnOptionsInit(WPARAM wParam,LPARAM lParam)
{
	OPTIONSDIALOGPAGE odp = {sizeof(odp)};
	odp.hInstance   = g_hInstance;
	odp.ptszTitle   = m_tszUserName;
	odp.dwInitParam = LPARAM(this);
	odp.flags       = ODPF_BOLDGROUPS | ODPF_TCHAR | ODPF_DONTTRANSLATE;

	odp.position    = 271828;
	odp.ptszGroup   = LPGENT("Network");
	odp.ptszTab     = LPGENT("Account");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPTIONS);
	odp.pfnDlgProc  = FBOptionsProc;
	CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);
	
	odp.position    = 271829;
	odp.ptszTab     = LPGENT("Advanced");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPTIONS_ADVANCED);
	odp.pfnDlgProc  = FBOptionsAdvancedProc;
	CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);

	odp.position    = 271830;
	if(ServiceExists(MS_POPUP_ADDPOPUPT))
		odp.ptszGroup   = LPGENT("Popups");
	odp.ptszTab     = LPGENT("Events");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPTIONS_EVENTS);
	odp.pfnDlgProc  = FBEventsProc;
	CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);

	return 0;
}

int FacebookProto::OnBuildStatusMenu(WPARAM wParam,LPARAM lParam)
{
	char text[200];
	strcpy(text,m_szModuleName);
	char *tDest = text+strlen(text);

	HGENMENU hRoot;
	CLISTMENUITEM mi = {sizeof(mi)};
	mi.pszService = text;

	hRoot = MO_GetProtoRootMenu(m_szModuleName);
	if (hRoot == NULL)
	{
		mi.popupPosition = 500085000;
		mi.hParentMenu = HGENMENU_ROOT;
		mi.flags = CMIF_ICONFROMICOLIB | CMIF_ROOTPOPUP | CMIF_TCHAR | CMIF_KEEPUNTRANSLATED | ( this->isOnline() ? 0 : CMIF_GRAYED );
		mi.icolibItem = GetIconHandle( "facebook" );
		mi.ptszName = m_tszUserName;
		hRoot = m_hMenuRoot = reinterpret_cast<HGENMENU>( CallService(
			MS_CLIST_ADDPROTOMENUITEM,0,reinterpret_cast<LPARAM>(&mi)) );
	} else {
		if ( m_hMenuRoot )
			CallService( MS_CLIST_REMOVEMAINMENUITEM, ( WPARAM )m_hMenuRoot, 0 );
		m_hMenuRoot = NULL;
	}

	mi.flags = CMIF_ICONFROMICOLIB | CMIF_CHILDPOPUP | ( this->isOnline() ? 0 : CMIF_GRAYED );
	mi.position = 201001;

	CreateProtoService(m_szModuleName,"/Mind",&FacebookProto::OnMind,this);
	strcpy(tDest,"/Mind");
	mi.hParentMenu = hRoot;
	mi.pszName = LPGEN("Mind...");
	mi.icolibItem = GetIconHandle("mind");
	m_hStatusMind = reinterpret_cast<HGENMENU>( CallService(
		MS_CLIST_ADDPROTOMENUITEM,0,reinterpret_cast<LPARAM>(&mi)) );

	CreateProtoService(m_szModuleName,"/VisitProfile",&FacebookProto::VisitProfile,this);
	strcpy(tDest,"/VisitProfile");
	mi.flags = CMIF_ICONFROMICOLIB | CMIF_CHILDPOPUP;
	mi.pszName = LPGEN("Visit Profile");
	mi.icolibItem = GetIconHandle("homepage");
	// TODO RM: remember and properly free in destructor?
	/*m_hStatusMind = */reinterpret_cast<HGENMENU>( CallService(
		MS_CLIST_ADDPROTOMENUITEM,0,reinterpret_cast<LPARAM>(&mi)) );

	return 0;
}

int FacebookProto::OnMind(WPARAM,LPARAM)
{
	HWND hDlg = CreateDialogParam( g_hInstance, MAKEINTRESOURCE( IDD_MIND ),
		 ( HWND )0, FBMindProc, reinterpret_cast<LPARAM>( this ) );
	ShowWindow( hDlg, SW_SHOW );
	return FALSE;
}

int FacebookProto::VisitProfile(WPARAM wParam,LPARAM lParam)
{
	HANDLE hContact = reinterpret_cast<HANDLE>(wParam);

	DBVARIANT dbv;
	if( wParam != 0 && !DBGetContactSettingString(hContact,m_szModuleName,"Homepage",&dbv) )
	{
		CallService(MS_UTILS_OPENURL,1,reinterpret_cast<LPARAM>(dbv.pszVal));
		DBFreeVariant(&dbv);
	} else {
		// self contact, probably
		// TODO: why isn't wParam == 0 when is status menu moved to main menu?
		CallService(MS_UTILS_OPENURL,1,reinterpret_cast<LPARAM>(FACEBOOK_URL_PROFILE));
		return 0;
	}

	return 0;
}

int FacebookProto::RemoveFriend(WPARAM wParam,LPARAM lParam)
{
	if (wParam == NULL)
	{ // self contact
	//	CallService(MS_UTILS_OPENURL,1,reinterpret_cast<LPARAM>(FACEBOOK_URL_PROFILE));
		return 0;
	}
	
	if (isOffline())
		return 0;

	if (MessageBox( 0, TranslateT("Are you sure?"), TranslateT("Delete contact from server list"), MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2 ) != IDYES)
		return 0;

	HANDLE hContact = reinterpret_cast<HANDLE>(wParam);

	DBVARIANT dbv;			
	if( !DBGetContactSettingString(hContact,m_szModuleName,FACEBOOK_KEY_ID,&dbv) )
	{
		if (!isOffline()) {
			std::string* id = new std::string(dbv.pszVal);
			ForkThread( &FacebookProto::DeleteContactFromServer, this, ( void* )id );
			DBFreeVariant(&dbv);

			if ( !DBGetContactSettingDword(hContact, m_szModuleName, FACEBOOK_KEY_DELETED, 0) )
				DBWriteContactSettingDword(hContact, m_szModuleName, FACEBOOK_KEY_DELETED, ::time(NULL));
		}
	}

	return 0;
}

int FacebookProto::AddFriend(WPARAM wParam,LPARAM lParam)
{
	if (wParam == NULL)
	{ // self contact
//		CallService(MS_UTILS_OPENURL,1,reinterpret_cast<LPARAM>(FACEBOOK_URL_PROFILE));
		return 0;
	}

	if (isOffline())
		return 0;

	HANDLE hContact = reinterpret_cast<HANDLE>(wParam);

	DBVARIANT dbv;
	if( !DBGetContactSettingString(hContact,m_szModuleName,FACEBOOK_KEY_ID,&dbv) )
	{
		if (!isOffline()) {
			std::string* id = new std::string(dbv.pszVal);
			ForkThread( &FacebookProto::AddContactToServer, this, ( void* )id );
			DBFreeVariant(&dbv);
		}
	}

	return 0;
}

void FacebookProto::ToggleStatusMenuItems( BOOL bEnable )
{
	CLISTMENUITEM clmi = { 0 };
	clmi.cbSize = sizeof( CLISTMENUITEM );
	clmi.flags = CMIM_FLAGS | (( bEnable ) ? 0 : CMIF_GRAYED);

	CallService( MS_CLIST_MODIFYMENUITEM, ( WPARAM )m_hMenuRoot,   ( LPARAM )&clmi );
	CallService( MS_CLIST_MODIFYMENUITEM, ( WPARAM )m_hStatusMind, ( LPARAM )&clmi );
}
