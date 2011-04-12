#pragma once

#include <m_system_cpp.h>
#include <m_protoint.h>

class CIcqProto : public PROTO_INTERFACE
{
public:
	CIcqProto() {}
	CIcqProto(const char* cszProtoName, const TCHAR* ctszUsername);
	~CIcqProto(void);

	virtual	HANDLE   AddToList( int flags, PROTOSEARCHRESULT* psr );
	virtual	HANDLE   AddToListByEvent( int flags, int iContact, HANDLE hDbEvent );

	virtual	int      Authorize( HANDLE hDbEvent );
	virtual	int      AuthDeny( HANDLE hDbEvent, const PROTOCHAR* szReason );
	virtual	int      AuthRecv( HANDLE hContact, PROTORECVEVENT* );
	virtual	int      AuthRequest( HANDLE hContact, const PROTOCHAR* szMessage );

	virtual	HANDLE   ChangeInfo( int iInfoType, void* pInfoData );

	virtual	HANDLE   FileAllow( HANDLE hContact, HANDLE hTransfer, const PROTOCHAR* szPath );
	virtual	int      FileCancel( HANDLE hContact, HANDLE hTransfer );
	virtual	int      FileDeny( HANDLE hContact, HANDLE hTransfer, const PROTOCHAR* szReason );
	virtual	int      FileResume( HANDLE hTransfer, int* action, const PROTOCHAR** szFilename );

	virtual	DWORD_PTR GetCaps( int type, HANDLE hContact = NULL );
	virtual	HICON     GetIcon( int iconIndex );
	virtual	int       GetInfo( HANDLE hContact, int infoType ) ;

	virtual	HANDLE    SearchBasic( const PROTOCHAR* id );
	virtual	HANDLE    SearchByEmail( const PROTOCHAR* email );
	virtual	HANDLE    SearchByName( const PROTOCHAR* nick, const PROTOCHAR* firstName, const PROTOCHAR* lastName );
	virtual	HWND      SearchAdvanced( HWND owner );
	virtual	HWND      CreateExtendedSearchUI( HWND owner );

	virtual	int       RecvContacts( HANDLE hContact, PROTORECVEVENT* );
	virtual	int       RecvFile( HANDLE hContact, PROTOFILEEVENT* );
	virtual	int       RecvMsg( HANDLE hContact, PROTORECVEVENT* );
	virtual	int       RecvUrl( HANDLE hContact, PROTORECVEVENT* );

	virtual	int       SendContacts( HANDLE hContact, int flags, int nContacts, HANDLE* hContactsList );
	virtual	HANDLE    SendFile( HANDLE hContact, const PROTOCHAR* szDescription, PROTOCHAR** ppszFiles );
	virtual	int       SendMsg( HANDLE hContact, int flags, const char* msg );
	virtual	int       SendUrl( HANDLE hContact, int flags, const char* url );

	virtual	int       SetApparentMode( HANDLE hContact, int mode );
	virtual	int       SetStatus( int iNewStatus );

	virtual	HANDLE    GetAwayMsg( HANDLE hContact );
	virtual	int       RecvAwayMsg( HANDLE hContact, int mode, PROTORECVEVENT* evt );
	virtual	int       SendAwayMsg( HANDLE hContact, HANDLE hProcess, const char* msg );
	virtual	int       SetAwayMsg( int iStatus, const PROTOCHAR* msg );

	virtual	int       UserIsTyping( HANDLE hContact, int type );

	virtual	int       OnEvent( PROTOEVENTTYPE iEventType, WPARAM wParam, LPARAM lParam );

	//////////////////////////////////////////////////////////////////////////
	icq_mode_messages m_modeMsgs;
	CRITICAL_SECTION m_modeMsgsMutex;
	char** MirandaStatusToAwayMsg(int nStatus);
	static INT_PTR IcqSetAwayMsg(WPARAM wParam, LPARAM lParam);

private:
	std::string		m_sProtoName;
	std::wstring	m_sUsername;
};

