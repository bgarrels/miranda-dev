/*

Facebook plugin for Miranda Instant Messenger
_____________________________________________

Copyright © 2009-11 Michal Zelinka

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

File name      : $HeadURL: http://eternityplugins.googlecode.com/svn/trunk/facebook/proto.cpp $
Revision       : $Revision: 93 $
Last change by : $Author: n3weRm0re.ewer $
Last change on : $Date: 2011-01-25 20:50:51 +0100 (út, 25 1 2011) $

*/

#include "common.h"

FacebookProto::FacebookProto(const char* proto_name,const TCHAR* username)
{
	m_iVersion = 2;
	m_szProtoName  = mir_strdup( proto_name );
	m_szModuleName = mir_strdup( proto_name );
	m_tszUserName  = mir_tstrdup( username );

	this->facy.parent = this;
	this->facy.last_feeds_update_ = getDword( "LastNotificationsUpdate", 0 ); // RM TODO: is it useful?

	this->signon_lock_ = CreateMutex( NULL, FALSE, NULL );
	this->avatar_lock_ = CreateMutex( NULL, FALSE, NULL );
	this->log_lock_ = CreateMutex( NULL, FALSE, NULL );
	this->facy.buddies_lock_ = CreateMutex( NULL, FALSE, NULL );
	this->facy.send_message_lock_ = CreateMutex( NULL, FALSE, NULL );
	this->facy.fcb_conn_lock_ = CreateMutex( NULL, FALSE, NULL );

	CreateProtoService(m_szModuleName, PS_CREATEACCMGRUI, &FacebookProto::SvcCreateAccMgrUI, this);
	CreateProtoService(m_szModuleName, PS_GETNAME,        &FacebookProto::GetName,           this);
	CreateProtoService(m_szModuleName, PS_GETSTATUS,      &FacebookProto::GetStatus,         this);
	CreateProtoService(m_szModuleName, PS_SETSTATUS,      &FacebookProto::SetStatus,         this);
	CreateProtoService(m_szModuleName, PS_GETMYAWAYMSG,   &FacebookProto::GetMyAwayMsg,      this);
	CreateProtoService(m_szModuleName, PS_SETAWAYMSG,     &FacebookProto::SetMyAwayMsg,      this);
	CreateProtoService(m_szModuleName, PS_GETMYAVATAR,    &FacebookProto::GetMyAvatar,       this);
	CreateProtoService(m_szModuleName, PS_GETAVATARINFO,  &FacebookProto::GetAvatarInfo,     this);
	CreateProtoService(m_szModuleName, PS_GETAVATARCAPS,  &FacebookProto::GetAvatarCaps,     this);

  // RM TODO: group chats
/*  CreateProtoService(m_szModuleName,PS_JOINCHAT, &FacebookProto::OnJoinChat, this);
	CreateProtoService(m_szModuleName,PS_LEAVECHAT,&FacebookProto::OnLeaveChat,this);*/
	if(g_mirandaVersion < PLUGIN_MAKE_VERSION(0, 10, 0, 2))
	{
		HookProtoEvent(ME_DB_CONTACT_DELETED,        &FacebookProto::OnContactDeleted,   this);
	}
	HookProtoEvent(ME_CLIST_PREBUILDSTATUSMENU,  &FacebookProto::OnBuildStatusMenu,  this);
	HookProtoEvent(ME_OPT_INITIALISE,            &FacebookProto::OnOptionsInit,      this);

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
	hAvatarFolder_ = FoldersRegisterCustomPath(m_szModuleName,"Avatars",
	    def_avatar_folder_.c_str());

	// Set all contacts offline -- in case we crashed
	SetAllContactStatuses( ID_STATUS_OFFLINE );

	// Check plugin API
	//facy.api_check( );
}

FacebookProto::~FacebookProto( )
{
	KillThreads( false );
	Netlib_CloseHandle( m_hNetlibUser );

	WaitForSingleObject( this->signon_lock_, IGNORE );
	WaitForSingleObject( this->avatar_lock_, IGNORE );
	WaitForSingleObject( this->log_lock_, IGNORE );
	WaitForSingleObject( this->facy.buddies_lock_, IGNORE );
	WaitForSingleObject( this->facy.send_message_lock_, IGNORE );
	WaitForSingleObject( this->update_loop_lock_, IGNORE );
	WaitForSingleObject( this->message_loop_lock_, IGNORE );

	CloseHandle( this->signon_lock_ );
	CloseHandle( this->avatar_lock_ );
	CloseHandle( this->log_lock_ );
	CloseHandle( this->facy.buddies_lock_ );
	CloseHandle( this->facy.send_message_lock_ );
	CloseHandle( this->facy.fcb_conn_lock_ );
	CloseHandle( this->update_loop_lock_ );
	CloseHandle( this->message_loop_lock_ );

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
			return PF1_IM | PF1_MODEMSG;
		else
			return PF1_IM | PF1_MODEMSGRECV;
	case PFLAGNUM_2:
		return PF2_ONLINE | PF2_INVISIBLE; // | PF2_IDLE | PF2_SHORTAWAY;
	case PFLAGNUM_3:
		if ( getByte( FACEBOOK_KEY_SET_MIRANDA_STATUS, 0 ) )
			return PF2_ONLINE; // | PF2_SHORTAWAY;
		else
			return 0;
	case PFLAGNUM_4:
		return PF4_FORCEAUTH | PF4_NOCUSTOMAUTH /*| PF4_SUPPORTIDLE*/ | PF4_IMSENDUTF | PF4_AVATARS | PF4_SUPPORTTYPING | PF4_NOAUTHDENYREASON | PF4_IMSENDOFFLINE;
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
	LOG("***** Beginning SetStatus process");
	
	// Routing statuses not supported by Facebook
	switch ( new_status )
	{
	case ID_STATUS_INVISIBLE:
	case ID_STATUS_OFFLINE:
		// No change
		break;

	// RM TODO: needed/useful?
	case ID_STATUS_CONNECTING:
		new_status = ID_STATUS_OFFLINE;
		break;
	
	default:
		new_status = ID_STATUS_ONLINE;
		break;
	}

	int old_status = m_iStatus;
	m_iDesiredStatus = new_status;

	if ( old_status == ID_STATUS_CONNECTING && new_status != ID_STATUS_OFFLINE )
	{
		LOG("===== Status is connecting, no change");
		return 0;
	}

	if ( new_status == old_status)
	{
		LOG("===== Statuses are same, no change");
		return 0;
	}

	facy.invisible_ = ( new_status == ID_STATUS_INVISIBLE );

	if ( new_status == ID_STATUS_OFFLINE )
	{
		LOG("===== New status: Offline");
		
		m_iStatus = facy.self_.status_id = ID_STATUS_CONNECTING;
		ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_SUCCESS, 
			(HANDLE)old_status,m_iStatus);

		ForkThread( &FacebookProto::SignOff, this );
	}
	else if ( old_status == ID_STATUS_OFFLINE )
	{
		LOG("===== Old status: Offline");
		
		m_iStatus = facy.self_.status_id = ID_STATUS_CONNECTING;
		ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_SUCCESS, 
			(HANDLE)old_status,m_iStatus);

		ForkThread( &FacebookProto::SignOn, this );
	}
	else if ( old_status == ID_STATUS_INVISIBLE )
	{
		LOG("===== Old status: Invisible");
		
		m_iStatus = facy.self_.status_id = ID_STATUS_CONNECTING;
		ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_SUCCESS, 
			(HANDLE)old_status,m_iStatus);

		ForkThread( &FacebookProto::ChangeStatus, this );
	}
	else if ( new_status == ID_STATUS_INVISIBLE )
	{
		LOG("===== New status: Invisible");

		m_iStatus = facy.self_.status_id = ID_STATUS_INVISIBLE;
		ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_SUCCESS, 
			(HANDLE)old_status,m_iStatus);

		facy.chat_state( false );
		facy.buddies.clear( );
		this->SetAllContactStatuses( ID_STATUS_OFFLINE );
	} else { 
		LOG("===== New status: Else");

		m_iStatus = facy.self_.status_id = new_status;
		ProtoBroadcastAck(m_szModuleName,0,ACKTYPE_STATUS,ACKRESULT_SUCCESS, 
			(HANDLE)old_status,m_iStatus);
	}
  
	LOG("***** SetStatus complete");
	return 0;
}

int FacebookProto::SetAwayMsg( int status, const PROTOCHAR *msg )
{
	if ( isOnline() && msg != NULL && getByte( FACEBOOK_KEY_SET_MIRANDA_STATUS, DEFAULT_SET_MIRANDA_STATUS ) )
	{
		char *narrow = mir_utf8encodeT(msg);
		ForkThread(&FacebookProto::SetAwayMsgWorker, this, narrow);
	}
	return 0;
}

void FacebookProto::SetAwayMsgWorker(void * data)
{
	std::string new_status = ( char* )data;
	facy.set_status( new_status );
	utils::mem::detract( ( void** )&data );
}

//////////////////////////////////////////////////////////////////////////////
// SERVICES

int FacebookProto::GetStatus( WPARAM wParam, LPARAM lParam )
{
	return m_iStatus;
}

int FacebookProto::SetStatus( WPARAM wParam, LPARAM lParam )
{
	return SetStatus( (int)wParam );
}

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

int FacebookProto::SetMyAwayMsg( WPARAM wParam, LPARAM lParam )
{
	return SetAwayMsg( (int)wParam, (const TCHAR*)lParam );
}

//////////////////////////////////////////////////////////////////////////////

int FacebookProto::OnEvent(PROTOEVENTTYPE event,WPARAM wParam,LPARAM lParam)
{
	switch(event)
	{
	case EV_PROTO_ONLOAD:
		return OnModulesLoaded(wParam,lParam);

	case EV_PROTO_ONEXIT:
		return OnPreShutdown  (wParam,lParam);
	
	case EV_PROTO_ONOPTIONS:
		return OnOptionsInit  (wParam,lParam);

	case EV_PROTO_ONCONTACTDELETED:
 		return OnContactDeleted(wParam,lParam);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
// EVENTS

int FacebookProto::GetName( WPARAM wParam, LPARAM lParam )
{
	lstrcpynA(reinterpret_cast<char*>(lParam),m_szProtoName,wParam);
	return 0;
}

int FacebookProto::SvcCreateAccMgrUI(WPARAM wParam,LPARAM lParam)
{
	return (int)CreateDialogParam(g_hInstance,MAKEINTRESOURCE(IDD_FACEBOOKACCOUNT), 
		 (HWND)lParam, FBAccountProc, (LPARAM)this );
}

int FacebookProto::OnModulesLoaded(WPARAM wParam,LPARAM lParam)
{
/*
	// RM TODO: group chats
	// Register group chat
	GCREGISTER gcr = {sizeof(gcr)};
	gcr.dwFlags = GC_ACKMSG;
	gcr.pszModule = m_szModuleName;
	gcr.pszModuleDispName = m_szModuleName;
	gcr.iMaxText = FACEBOOK_MESSAGE_LIMIT;
	CallService(MS_GC_REGISTER,0,reinterpret_cast<LPARAM>(&gcr));
*/
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
	if(IsMyContact(hContact, true))
		ShowContactMenus(true);

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
	odp.ptszTab     = LPGENT("Account && Integration");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPTIONS);
	odp.pfnDlgProc  = FBOptionsProc;
	CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);

	odp.position    = 271829;
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
	// RM TODO: remember and properly free in destructor
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
	if (wParam == NULL)
	{ // self contact
		CallService(MS_UTILS_OPENURL,1,reinterpret_cast<LPARAM>(FACEBOOK_URL_PROFILE));
		return 0;
	}

	HANDLE hContact = reinterpret_cast<HANDLE>(wParam);

	DBVARIANT dbv;
	if( !DBGetContactSettingString(hContact,m_szModuleName,"Homepage",&dbv) )
	{
		CallService(MS_UTILS_OPENURL,1,reinterpret_cast<LPARAM>(dbv.pszVal));
		DBFreeVariant(&dbv);
	}/* else {
		// RM TODO: remove this
		std::string key, url;
		if (DBGetContactSettingByte(hContact,m_szModuleName,"ChatRoom",0) == 0)
		{ // usual contact
			key = FACEBOOK_KEY_ID;
			url = FACEBOOK_URL_PROFILE;
		} else {
			key = "ChatRoomID";
			url = FACEBOOK_URL_GROUP;
		}

		if ( !DBGetContactSettingString(hContact,m_szModuleName,key.c_str(),&dbv) )
		{
			url += dbv.pszVal;
			DBWriteContactSettingString(hContact,m_szModuleName,"Homepage",url.c_str());

			CallService(MS_UTILS_OPENURL,1,reinterpret_cast<LPARAM>(url.c_str()));
			DBFreeVariant(&dbv);
		}
	}*/

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
