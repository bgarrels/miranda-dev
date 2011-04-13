#pragma once

#include <m_system_cpp.h>
#include <m_protoint.h>

class CIcqProto;

extern CIcqProto* pProtocol;

typedef void    ( __cdecl CIcqProto::*IcqThreadFunc )( void* );

class CIcqProto : public PROTO_INTERFACE
{
public:
	CIcqProto() {}
	CIcqProto(const char* cszProtoName, const TCHAR* ctszUsername);
	~CIcqProto(void);
	void Initialize();

	static CIcqProto& Instance()
	{
		return *pProtocol;
	}

	static void SetInstance(CIcqProto* p)
	{
		pProtocol = p;
	}

	virtual	HANDLE   __cdecl AddToList( int flags, PROTOSEARCHRESULT* psr );
	virtual	HANDLE   __cdecl AddToListByEvent( int flags, int iContact, HANDLE hDbEvent );

	virtual	int      __cdecl Authorize( HANDLE hDbEvent );
	virtual	int      __cdecl AuthDeny( HANDLE hDbEvent, const PROTOCHAR* szReason );
	virtual	int      __cdecl AuthRecv( HANDLE hContact, PROTORECVEVENT* );
	virtual	int      __cdecl AuthRequest( HANDLE hContact, const PROTOCHAR* szMessage );

	virtual	HANDLE   __cdecl ChangeInfo( int iInfoType, void* pInfoData );

	virtual	HANDLE   __cdecl FileAllow( HANDLE hContact, HANDLE hTransfer, const PROTOCHAR* szPath );
	virtual	int      __cdecl FileCancel( HANDLE hContact, HANDLE hTransfer );
	virtual	int      __cdecl FileDeny( HANDLE hContact, HANDLE hTransfer, const PROTOCHAR* szReason );
	virtual	int      __cdecl FileResume( HANDLE hTransfer, int* action, const PROTOCHAR** szFilename );

	virtual	DWORD_PTR __cdecl GetCaps( int type, HANDLE hContact = NULL );
	virtual	HICON     __cdecl GetIcon( int iconIndex );
	virtual	int       __cdecl GetInfo( HANDLE hContact, int infoType ) ;

	virtual	HANDLE    __cdecl SearchBasic( const PROTOCHAR* id );
	virtual	HANDLE    __cdecl SearchByEmail( const PROTOCHAR* email );
	virtual	HANDLE    __cdecl SearchByName( const PROTOCHAR* nick, const PROTOCHAR* firstName, const PROTOCHAR* lastName );
	virtual	HWND      __cdecl SearchAdvanced( HWND owner );
	virtual	HWND      __cdecl CreateExtendedSearchUI( HWND owner );

	virtual	int       __cdecl RecvContacts( HANDLE hContact, PROTORECVEVENT* );
	virtual	int       __cdecl RecvFile( HANDLE hContact, PROTOFILEEVENT* );
	virtual	int       __cdecl RecvMsg( HANDLE hContact, PROTORECVEVENT* );
	virtual	int       __cdecl RecvUrl( HANDLE hContact, PROTORECVEVENT* );

	virtual	int       __cdecl SendContacts( HANDLE hContact, int flags, int nContacts, HANDLE* hContactsList );
	virtual	HANDLE    __cdecl SendFile( HANDLE hContact, const PROTOCHAR* szDescription, PROTOCHAR** ppszFiles );
	virtual	int       __cdecl SendMsg( HANDLE hContact, int flags, const char* msg );
	virtual	int       __cdecl SendUrl( HANDLE hContact, int flags, const char* url );

	virtual	int       __cdecl SetApparentMode( HANDLE hContact, int mode );
	virtual	int       __cdecl SetStatus( int iNewStatus );

	virtual	HANDLE    __cdecl GetAwayMsg( HANDLE hContact );
	virtual	int       __cdecl RecvAwayMsg( HANDLE hContact, int mode, PROTORECVEVENT* evt );
	virtual	int       __cdecl SendAwayMsg( HANDLE hContact, HANDLE hProcess, const char* msg );
	virtual	int       __cdecl SetAwayMsg( int iStatus, const PROTOCHAR* msg );

	virtual	int       __cdecl UserIsTyping( HANDLE hContact, int type );

	virtual	int       __cdecl OnEvent( PROTOEVENTTYPE iEventType, WPARAM wParam, LPARAM lParam );

	//////////////////////////////////////////////////////////////////////////
	icq_mode_messages _m_modeMsgs;
	CRITICAL_SECTION _m_modeMsgsMutex;
	char** _MirandaStatusToAwayMsg(int nStatus);
	static INT_PTR IcqSetAwayMsg(WPARAM wParam, LPARAM lParam);
	static INT_PTR IcqSetStatus(WPARAM wParam, LPARAM lParam);
	//static INT_PTR IcqGetInfo(WPARAM wParam, LPARAM lParam);

	//----| icq_db.cpp |------------------------------------------------------------------
	//BOOL bUtfReadyDB;
	HANDLE	_AddEvent(HANDLE hContact, WORD wType, DWORD dwTime, DWORD flags, DWORD cbBlob, PBYTE pBlob);
	void	_CreateResidentSetting(const char* szSetting);
	HANDLE	_FindFirstContact();
	HANDLE	_FindNextContact(HANDLE hContact);
	int		_IsICQContact(HANDLE hContact);

	int		_getSetting(HANDLE hContact, const char* szSetting, DBVARIANT *dbv);
	BYTE	_getSettingByte(HANDLE hContact, const char* szSetting, BYTE bDef);
	WORD	_getSettingWord(HANDLE hContact, const char* szSetting, WORD wDef);
	DWORD	_getSettingDword(HANDLE hContact, const char* szSetting, DWORD dwDef);
	double	_getSettingDouble(HANDLE hContact, const char *szSetting, double dDef);
	int		_getSettingString(HANDLE hContact, const char* szSetting, DBVARIANT *dbv);
	//int		getSettingStringW(HANDLE hContact, const char *szSetting, DBVARIANT *dbv);
	int		_getSettingStringStatic(HANDLE hContact, const char* valueName, char* dest, int dest_len);
	char*	_getSettingStringUtf(HANDLE hContact, const char* szSetting, char* szDef);
	char*	_getSettingStringUtf(HANDLE hContact, const char *szModule,const char* szSetting, char* szDef);
	int		_getContactUid(HANDLE hContact, DWORD *pdwUin, uid_str* ppszUid);
	DWORD	_getContactUin(HANDLE hContact);
	WORD	_getContactStatus(HANDLE hContact);
	char*	_getContactCListGroup(HANDLE hContact);

	int		_deleteSetting(HANDLE hContact, const char* szSetting);

	int		_setSettingByte(HANDLE hContact, const char* szSetting, BYTE bValue);
	int		_setSettingWord(HANDLE hContact, const char* szSetting, WORD wValue);
	int		_setSettingDword(HANDLE hContact, const char* szSetting, DWORD dwValue);
	int		_setSettingDouble(HANDLE hContact, const char *szSetting, double dValue);
	int		_setSettingString(HANDLE hContact, const char* szSetting, char* szValue);
	//int		setSettingStringW(HANDLE hContact, const char *szSetting, const WCHAR *wszValue);
	int		_setSettingStringUtf(HANDLE hContact, const char* szSetting, char* szValue);
	int		_setSettingStringUtf(HANDLE hContact, const char *szModule,const char* szSetting, char* szValue);
	int		_setSettingBlob(HANDLE hContact,const char *szSetting, BYTE *val, const int cbVal);

	int		_setContactHidden(HANDLE hContact, BYTE bHidden);
	void	_setStatusMsgVar(HANDLE hContact, char* szStatusMsg, bool isAnsi);

private:
	char* ConvertMsgToUserSpecificAnsi(HANDLE hContact, const char* szMsg);
	void ForkThread( IcqThreadFunc pFunc, void* arg );
	void   __cdecl CheekySearchThread( void* );

	int cheekySearchId;
	DWORD cheekySearchUin;
	char* cheekySearchUid;

	std::string		m_sProtoName;
	std::wstring	m_sUsername;
};

#define m_modeMsgsMutex CIcqProto::Instance()._m_modeMsgsMutex
#define m_modeMsgs CIcqProto::Instance()._m_modeMsgs
#define MirandaStatusToAwayMsg CIcqProto::Instance()._MirandaStatusToAwayMsg
//////////////////////////////////////////////////////////////////////////

#define AddEvent CIcqProto::Instance()._AddEvent
#define CreateResidentSetting CIcqProto::Instance()._CreateResidentSetting
#define FindFirstContact CIcqProto::Instance()._FindFirstContact
#define FindNextContact CIcqProto::Instance()._FindNextContact
#define IsICQContact CIcqProto::Instance()._IsICQContact

#define getSetting CIcqProto::Instance()._getSetting
#define getSettingByte CIcqProto::Instance()._getSettingByte
#define getSettingWord CIcqProto::Instance()._getSettingWord
#define getSettingDword CIcqProto::Instance()._getSettingDword
#define getSettingDouble CIcqProto::Instance()._getSettingDouble
#define getSettingString CIcqProto::Instance()._getSettingString

#define getSettingStringStatic CIcqProto::Instance()._getSettingStringStatic
#define getSettingStringUtf CIcqProto::Instance()._getSettingStringUtf
#define getContactUid CIcqProto::Instance()._getContactUid
#define getContactUin CIcqProto::Instance()._getContactUin
#define getContactStatus CIcqProto::Instance()._getContactStatus
#define getContactCListGroup CIcqProto::Instance()._getContactCListGroup

#define deleteSetting CIcqProto::Instance()._deleteSetting

#define setSettingByte CIcqProto::Instance()._setSettingByte
#define setSettingWord CIcqProto::Instance()._setSettingWord
#define setSettingDword CIcqProto::Instance()._setSettingDword
#define setSettingDouble CIcqProto::Instance()._setSettingDouble
#define setSettingString CIcqProto::Instance()._setSettingString

#define setSettingStringUtf CIcqProto::Instance()._setSettingStringUtf
#define setSettingBlob CIcqProto::Instance()._setSettingBlob

//#define setContactHidden gProtocol._setContactHidden
#define setStatusMsgVar CIcqProto::Instance()._setStatusMsgVar
