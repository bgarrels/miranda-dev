#include "icqoscar.h"
#include "IcqCore.h"


CIcqProto::CIcqProto(const char* cszProtoName, const TCHAR* ctszUsername)
{
	m_sProtoName = cszProtoName;
	m_sUsername = ctszUsername;
}

CIcqProto::~CIcqProto(void)
{
}

HANDLE CIcqProto::AddToList( int flags, PROTOSEARCHRESULT* psr )
{
	return NULL;
}

HANDLE CIcqProto::AddToListByEvent( int flags, int iContact, HANDLE hDbEvent )
{
	return NULL;
}

int CIcqProto::Authorize( HANDLE hDbEvent )
{
	return 0;
}

int CIcqProto::AuthDeny( HANDLE hDbEvent, const PROTOCHAR* szReason )
{
	return 0;
}

int CIcqProto::AuthRecv( HANDLE hContact, PROTORECVEVENT* )
{
	return 0;
}

int CIcqProto::AuthRequest( HANDLE hContact, const PROTOCHAR* szMessage )
{
	return 0;
}

HANDLE CIcqProto::ChangeInfo( int iInfoType, void* pInfoData )
{
	return NULL;
}

HANDLE CIcqProto::FileAllow( HANDLE hContact, HANDLE hTransfer, const PROTOCHAR* szPath )
{
	return NULL;
}

int CIcqProto::FileCancel( HANDLE hContact, HANDLE hTransfer )
{
	return 0;
}

int CIcqProto::FileDeny( HANDLE hContact, HANDLE hTransfer, const PROTOCHAR* szReason )
{
	return 0;
}

int CIcqProto::FileResume( HANDLE hTransfer, int* action, const PROTOCHAR** szFilename )
{
	return 0;
}

DWORD_PTR CIcqProto::GetCaps( int type, HANDLE hContact )
{
	return NULL;
}

HICON CIcqProto::GetIcon( int iconIndex )
{
	return NULL;
}

int CIcqProto::GetInfo( HANDLE hContact, int infoType )
{
	return NULL;
}

HANDLE CIcqProto::SearchBasic( const PROTOCHAR* id )
{
	return NULL;
}

HANDLE CIcqProto::SearchByEmail( const PROTOCHAR* email )
{
	return NULL;
}

HANDLE CIcqProto::SearchByName( const PROTOCHAR* nick, const PROTOCHAR* firstName, const PROTOCHAR* lastName )
{
	return NULL;
}

HWND CIcqProto::SearchAdvanced( HWND owner )
{
	return NULL;
}

HWND CIcqProto::CreateExtendedSearchUI( HWND owner )
{
	return NULL;
}

int CIcqProto::RecvContacts( HANDLE hContact, PROTORECVEVENT* )
{
	return 0;
}

int CIcqProto::RecvFile( HANDLE hContact, PROTOFILEEVENT* )
{
	return 0;
}

int CIcqProto::RecvMsg( HANDLE hContact, PROTORECVEVENT* )
{
	return 0;
}

int CIcqProto::RecvUrl( HANDLE hContact, PROTORECVEVENT* )
{
	return 0;
}

int CIcqProto::SendContacts( HANDLE hContact, int flags, int nContacts, HANDLE* hContactsList )
{
	return 0;
}

HANDLE CIcqProto::SendFile( HANDLE hContact, const PROTOCHAR* szDescription, PROTOCHAR** ppszFiles )
{
	return NULL;
}

int CIcqProto::SendMsg( HANDLE hContact, int flags, const char* msg )
{
	return 0;
}

int CIcqProto::SendUrl( HANDLE hContact, int flags, const char* url )
{
	return 0;
}

int CIcqProto::SetApparentMode( HANDLE hContact, int mode )
{
	return 0;
}

int CIcqProto::SetStatus( int iNewStatus )
{
	return 0;
}

HANDLE CIcqProto::GetAwayMsg( HANDLE hContact )
{
	return NULL;
}

int CIcqProto::RecvAwayMsg( HANDLE hContact, int mode, PROTORECVEVENT* evt )
{
	return 0;
}

int CIcqProto::SendAwayMsg( HANDLE hContact, HANDLE hProcess, const char* msg )
{
	return 0;
}

int CIcqProto::SetAwayMsg( int iStatus, const PROTOCHAR* msg )
{
	return 0;
}

int CIcqProto::UserIsTyping( HANDLE hContact, int type )
{
	return 0;
}

int CIcqProto::OnEvent( PROTOEVENTTYPE iEventType, WPARAM wParam, LPARAM lParam )
{
	switch (iEventType)
	{
	case EV_PROTO_ONLOAD:
		//return OnModulesLoaded(0, 0);
		break;
	case EV_PROTO_ONREADYTOEXIT: break;
	case EV_PROTO_ONEXIT:
		//return OnPreShutdown(0, 0);
		break;
	case EV_PROTO_ONRENAME: break;
	case EV_PROTO_ONOPTIONS:
		//return OnOptionsInit(wParam, lParam);
		break;
	case EV_PROTO_ONERASE:
		{
			//char szDbSetting[MAX_PATH];

			//null_snprintf(szDbSetting, sizeof(szDbSetting), "%sP2P", m_szModuleName);
			//CallService(MS_DB_MODULE_DELETE, 0, (LPARAM)szDbSetting);
			//null_snprintf(szDbSetting, sizeof(szDbSetting), "%sSrvGroups", m_szModuleName);
			//CallService(MS_DB_MODULE_DELETE, 0, (LPARAM)szDbSetting);
			//null_snprintf(szDbSetting, sizeof(szDbSetting), "%sGroups", m_szModuleName);
			//CallService(MS_DB_MODULE_DELETE, 0, (LPARAM)szDbSetting);

			break;
		}
	case EV_PROTO_ONMENU: break;
	}

	return 1;
}
