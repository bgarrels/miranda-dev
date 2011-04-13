#include "icqoscar.h"
#include "IcqCore.h"
#include "init.h"
#include "icq_opts.h"
#include "icq_constants.h"

#include "m_folders.h"

CIcqProto::CIcqProto(const char* cszProtoName, const TCHAR* ctszUsername) :
cheekySearchId( -1 )
{
	m_sProtoName = cszProtoName;
	m_sUsername = ctszUsername;
}

void CIcqProto::Initialize()
{
	// FIXME: Do I need to fix this ?
	//HookEvent(ME_DB_CONTACT_SETTINGCHANGED, &CIcqProto::ServListDbSettingChanged);

	InitializeCriticalSection(&connectionHandleMutex);
	InitializeCriticalSection(&localSeqMutex);
	InitializeCriticalSection(&m_modeMsgsMutex);

	// Initialize core modules
	InitDB();       // DB interface
	InitVars(); // global variables
	InitCookies();  // cookie utils
	InitCache();    // contacts cache
	InitReguin();   // reguin module

	DBWriteContactSettingDword(NULL, ICQ_PROTOCOL_NAME, "SrvLastUpdate", 0);
	DBWriteContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "SrvRecordCount", 0);

	InitRates();    // rate management

	// Initialize status message struct
	ZeroMemory(&m_modeMsgs, sizeof(icq_mode_messages));

	// Initialize temporary DB settings
	CreateResidentSetting("Status"); // NOTE: XStatus cannot be temporary
	CreateResidentSetting("ICQStatus");
	CreateResidentSetting("TemporaryVisible");
	CreateResidentSetting("TickTS");
	CreateResidentSetting("IdleTS");
	CreateResidentSetting("LogonTS");
	CreateResidentSetting("CapBuf");
	CreateResidentSetting("DCStatus");
	CreateResidentSetting("TmpContact");

	// Setup services
	CreateProtoService(PS_ICQP_SERVER_IGNORE, IcqServerIgnore);
	CreateProtoService(PS_GETNAME, IcqGetName);
	CreateProtoService(PS_LOADICON, IcqLoadIcon);
	CreateProtoService(PS_GETSTATUS, IcqGetStatus);
	CreateProtoService(MS_ICQ_SENDSMS, IcqSendSms);
	CreateProtoService(PS_FILERESUME, IcqFileResume);
	CreateProtoService(PS_SET_NICKNAME, IcqSetNickName);
	CreateProtoService(PSS_FILEALLOW, IcqFileAllow);
	CreateProtoService(PSS_FILEDENY, IcqFileDeny);
	CreateProtoService(PSS_FILECANCEL, IcqFileCancel);
	CreateProtoService(PSS_FILE, IcqSendFile);
	CreateProtoService(PSR_FILE, IcqRecvFile);
	CreateProtoService(PSS_ADDED, IcqSendYouWereAdded);
	CreateProtoService(PS_CREATEACCMGRUI, SvcCreateAccMgrUI);
	// Session password API
	CreateProtoService(PS_ICQ_SETPASSWORD, IcqSetPassword);
	// ChangeInfo API
	CreateProtoService(PS_CHANGEINFOEX, IcqChangeInfoEx);
	// Avatar API
	CreateProtoService(PS_GETAVATARINFO, IcqGetAvatarInfo);
	CreateProtoService(PS_GETAVATARCAPS, IcqGetAvatarCaps);
	CreateProtoService(PS_GETMYAVATAR, IcqGetMyAvatar);
	CreateProtoService(PS_SETMYAVATAR, IcqSetMyAvatar);
	// Custom Status API
	CreateProtoService(PS_ICQ_SETCUSTOMSTATUS, IcqSetXStatus); // obsolete (remove in next version)
	CreateProtoService(PS_ICQ_GETCUSTOMSTATUS, IcqGetXStatus); // obsolete
	CreateProtoService(PS_ICQ_SETCUSTOMSTATUSEX, IcqSetXStatusEx);
	CreateProtoService(PS_ICQ_GETCUSTOMSTATUSEX, IcqGetXStatusEx);
	CreateProtoService(PS_ICQ_GETCUSTOMSTATUSICON, IcqGetXStatusIcon);
	CreateProtoService(PS_ICQ_REQUESTCUSTOMSTATUS, IcqRequestXStatusDetails);
	CreateProtoService(PS_ICQ_GETADVANCEDSTATUSICON, IcqRequestAdvStatusIconIdx);
	// Custom caps
	CreateProtoService(PS_ICQ_ADDCAPABILITY, IcqAddCapability);
	CreateProtoService(PS_ICQ_CHECKCAPABILITY, IcqCheckCapability);



	hsmsgrequest = CreateProtoEvent(ME_ICQ_STATUSMSGREQ);
	hxstatuschanged = CreateProtoEvent(ME_ICQ_CUSTOMSTATUS_CHANGED);
	hxstatusiconchanged = CreateProtoEvent(ME_ICQ_CUSTOMSTATUS_EXTRAICON_CHANGED);
	hqipstatuschanged = CreateProtoEvent(ME_ICQ_CUSTOMSTATUS_CHANGED);//added
	hqipstatusiconchanged = CreateProtoEvent(ME_ICQ_CUSTOMSTATUS_EXTRAICON_CHANGED);//added

	InitDirectConns();
	InitOscarFileTransfer();
	InitServerLists();
	icq_InitInfoUpdate();
	RegEventType(ICQEVENTTYPE_IGNORECHECK_STATUS, "Check ICQ Ignore State");
	RegEventType(ICQEVENTTYPE_CHECK_STATUS, "Check ICQ Status");
	RegEventType(ICQEVENTTYPE_CLIENT_CHANGE, "Chenge ICQ Client");
	RegEventType(ICQEVENTTYPE_SELF_REMOVE, "ICQ Contact SelfRemove");
	RegEventType(ICQEVENTTYPE_AUTH_DENIED, "ICQ Auth Denied");
	RegEventType(ICQEVENTTYPE_AUTH_GRANTED, "ICQ Auth Granted");
	RegEventType(ICQEVENTTYPE_AUTH_REQUESTED, "ICQ Auth Requested");
	RegEventType(ICQEVENTTYPE_YOU_ADDED, "ICQ You Added");
	RegEventType(ICQEVENTTYPE_WAS_FOUND, "Detected via ASD\\PSD");
	RegEventType(ICQEVENTTYPE_CHECK_XSTATUS, "Read X-Status");


	icq_InitISee();

	UpdateGlobalSettings();

	gnCurrentStatus = ID_STATUS_OFFLINE;

	hIconFolder = FoldersRegisterCustomPath(ICQ_PROTOCOL_NAME, "Icons", MIRANDA_PATH"\\icons");
	if( hIconFolder==(HANDLE)CALLSERVICE_NOTFOUND ) hIconFolder = 0;

	CreateProtoService(MS_ICQ_ADDSERVCONTACT, IcqAddServerContact);

	CreateProtoService(MS_REQ_AUTH, icq_RequestAuthorization);
	CreateProtoService(MS_GRANT_AUTH, IcqGrantAuthorization);
	CreateProtoService(MS_REVOKE_AUTH, IcqRevokeAuthorization);
	CreateProtoService(MS_SETINVIS, IcqSetInvis);
	CreateProtoService(MS_SETVIS, IcqSetVis);
	CreateProtoService(MS_INCOGNITO_REQUEST, IncognitoAwayRequest);
	CreateProtoService(MS_SEND_TZER, IcqSendtZer);
	CreateProtoService(MS_TZER_DIALOG, IcqTzerDlg);
	CreateProtoService(MS_ICQ_GET_USER_STATUS, IcqGetUserStatus_service);

	CreateProtoService(MS_XSTATUS_SHOWDETAILS, IcqShowXStatusDetails);

	hHookIconsChanged = IconLibHookIconsChanged(::IconLibIconsChanged);

	InitXStatusIcons();
	InitQipStatusIcons();
	InitTzersIcons();

	// This must be here - the events are called too early, WTF?
	InitXStatusEvents();
	InitQipStatusEvents();

	if (DBGetContactSettingByte(NULL, ICQ_PROTOCOL_NAME, "PrivacyMenu", DEFAULT_PRIVACY_ENABLED))
		icq_BuildPrivacyMenu();



	//Custom caps
	lstCustomCaps = li.List_Create(0,1);
	lstCustomCaps->sortFunc = NULL;

	{
		char tmp[MAXMODULELABELLENGTH];
		DBCONTACTENUMSETTINGS dbces;
		mir_snprintf(tmp, MAXMODULELABELLENGTH, "%sCaps", ICQ_PROTOCOL_NAME);
		dbces.pfnEnumProc = EnumCustomCapsProc;
		dbces.lParam = (LPARAM)tmp;
		dbces.szModule = tmp;
		CallService(MS_DB_CONTACT_ENUMSETTINGS, 0, (LPARAM)&dbces);
	}

	{
		//ICQ_CUSTOMCAP icqCustomCap;
		//IcqBuildMirandaCap(&icqCustomCap, "Miranda ICQ Capability Test", NULL, "TestTest");
		//CallProtoService(gpszICQProtoName, PS_ICQ_ADDCAPABILITY, 0, (LPARAM)&icqCustomCap);
	}

	// Reset a bunch of session specific settings
	ResetSettingsOnLoad();
}

CIcqProto::~CIcqProto(void)
{
}

HANDLE CIcqProto::AddToList( int flags, PROTOSEARCHRESULT* psr )
{
	if (psr)
	{
		if (psr->cbSize == sizeof(ICQSEARCHRESULT))
		{
			ICQSEARCHRESULT *isr = (ICQSEARCHRESULT*)psr;
			if (isr->uin)
				return AddToListByUIN(isr->uin, flags);
			else
			{ // aim contact
				char szUid[MAX_PATH];

				if (isr->hdr.flags & PSR_UNICODE)
					unicode_to_ansi_static((WCHAR*)isr->hdr.id, szUid, MAX_PATH);
				else
					null_strcpy(szUid, (char*)isr->hdr.id, MAX_PATH);

				if (szUid[0] == 0) return 0;
				return AddToListByUID(szUid, flags);
			}
		}
		else
		{
			char szUid[MAX_PATH];

			if (psr->flags & PSR_UNICODE)
				unicode_to_ansi_static((WCHAR*)psr->id, szUid, MAX_PATH);
			else
				null_strcpy(szUid, (char*)psr->id, MAX_PATH);

			if (szUid[0] == 0) return 0;
			if (IsStringUIN(szUid))
				return AddToListByUIN(atoi(szUid), flags);
			else
				return AddToListByUID(szUid, flags);
		}
	}

	return 0; // Failure
}

HANDLE CIcqProto::AddToListByEvent( int flags, int iContact, HANDLE hDbEvent )
{
	DBEVENTINFO dbei = {0};
	DWORD uin = 0;
	uid_str uid = {0};

	dbei.cbSize = sizeof(dbei);

	if ((dbei.cbBlob = CallService(MS_DB_EVENT_GETBLOBSIZE, (WPARAM)hDbEvent, 0)) == -1)
		return 0;

	dbei.pBlob = (PBYTE)icq_alloc_zero(dbei.cbBlob + 1);
	dbei.pBlob[dbei.cbBlob] = '\0';

	if (CallService(MS_DB_EVENT_GET, (WPARAM)hDbEvent, (LPARAM)&dbei))
		return 0; // failed to get event

	if (strcmpnull(dbei.szModule, ICQ_PROTOCOL_NAME))
		return 0; // this event is not ours

	if (dbei.eventType == EVENTTYPE_CONTACTS)
	{
		int i;
		char* pbOffset, *pbEnd;

		for (i = 0, pbOffset = (char*)dbei.pBlob, pbEnd = pbOffset + dbei.cbBlob; i <= iContact; i++)
		{
			pbOffset += strlennull((char*)pbOffset) + 1;  // Nick
			if (pbOffset >= pbEnd) break;
			if (i == iContact)
			{
				// we found the contact, get uid
				if (IsStringUIN((char*)pbOffset))
					uin = atoi((char*)pbOffset);
				else
				{
					uin = 0;
					strcpy(uid, (char*)pbOffset);
				}
			}
			pbOffset += strlennull((char*)pbOffset) + 1;  // Uin
			if (pbOffset >= pbEnd) break;
		}
	}
	else if (dbei.eventType != EVENTTYPE_AUTHREQUEST && dbei.eventType != EVENTTYPE_ADDED)
	{
		return 0;
	}
	else // auth req or added event
	{
		HANDLE hContact = ((HANDLE*)dbei.pBlob)[1]; // this sucks - awaiting new auth system

		if (getContactUid(hContact, &uin, &uid))
			return 0;
	}

	if (uin != 0)
	{
		return AddToListByUIN(uin, flags); // Success
	}
	else if (strlennull(uid))
	{
		// add aim contact
		return AddToListByUID(uid, flags); // Success
	}

	return 0; // Failure
}

int CIcqProto::Authorize( HANDLE hDbEvent )
{
	if (icqOnline && hDbEvent)
	{
		HANDLE hContact = HContactFromAuthEvent( hDbEvent );
		if (hContact == INVALID_HANDLE_VALUE)
			return 1;

		DWORD uin;
		uid_str uid;
		if (getContactUid(hContact, &uin, &uid))
			return 1;

		icq_sendAuthResponseServ(uin, uid, 1, _T(""));

		deleteSetting(hContact, "Grant");

		return 0; // Success
	}

	return 1; // Failure
}

int CIcqProto::AuthDeny( HANDLE hDbEvent, const PROTOCHAR* szReason )
{
	if (icqOnline && hDbEvent)
	{
		HANDLE hContact = HContactFromAuthEvent(hDbEvent);
		if (hContact == INVALID_HANDLE_VALUE)
			return 1;

		DWORD uin;
		uid_str uid;
		if (getContactUid(hContact, &uin, &uid))
			return 1;

		icq_sendAuthResponseServ(uin, uid, 0, utf8_to_tchar(szReason) );	// FIXME: utf8_to_tchar is not needed after switch to MIRANDA_VER = 0x0900

		if (DBGetContactSettingByte(hContact, "CList", "NotOnList", 0))
			CallService(MS_DB_CONTACT_DELETE, (WPARAM)hContact, 0);

		return 0; // Success
	}

	return 1; // Failure
}

int CIcqProto::AuthRecv( HANDLE hContact, PROTORECVEVENT* pre )
{
	setContactHidden( hContact, 0 );
	ICQAddRecvEvent( NULL, EVENTTYPE_AUTHREQUEST, pre, pre->lParam, (PBYTE)pre->szMessage, 0 );
	return 0;
}

int CIcqProto::AuthRequest( HANDLE hContact, const PROTOCHAR* szMessage )
{
	if ( !icqOnline)
		return 1;

	if (hContact)
	{
		DWORD dwUin;
		uid_str szUid;
		if (getContactUid(hContact, &dwUin, &szUid))
			return 1; // Invalid contact

		if (dwUin)
		{
			char *utf = ansi_to_utf8(szMessage);// tchar_to_utf8(szMessage);

			icq_sendAuthReqServ(dwUin, szUid, utf);
			SAFE_FREE(&utf);
			return 0; // Success
		}
	}

	return 1; // Failure
}

HANDLE CIcqProto::ChangeInfo( int iInfoType, void* pInfoData )
{
	// IcqChangeInfoEx?
    return 0; // Failure
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
	DWORD_PTR nReturn = 0;

	switch ( type ) {

	case PFLAGNUM_1:
		nReturn = PF1_IM | PF1_URL | PF1_AUTHREQ | PF1_BASICSEARCH | PF1_ADDSEARCHRES |
				  PF1_VISLIST | PF1_INVISLIST | PF1_MODEMSG | PF1_FILE | PF1_EXTSEARCH |
				  PF1_EXTSEARCHUI | PF1_SEARCHBYEMAIL | PF1_SEARCHBYNAME |
				  PF1_ADDED | PF1_CONTACT;
		if (!m_bAimEnabled)
			nReturn |= PF1_NUMERICUSERID;
		if (m_bSsiEnabled && getSettingByte(NULL, "ServerAddRemove", DEFAULT_SS_ADDSERVER))
			nReturn |= PF1_SERVERCLIST;
		break;

	case PFLAGNUM_2:
		nReturn = PF2_ONLINE | PF2_SHORTAWAY | PF2_LONGAWAY | PF2_LIGHTDND | PF2_HEAVYDND |
				  PF2_FREECHAT | PF2_INVISIBLE;
		if (m_bAimEnabled)
			nReturn |= PF2_ONTHEPHONE;
		break;

	case PFLAGNUM_3:
		nReturn = PF2_ONLINE | PF2_SHORTAWAY | PF2_LONGAWAY | PF2_LIGHTDND | PF2_HEAVYDND |
				  PF2_FREECHAT | PF2_INVISIBLE;
		break;

	case PFLAGNUM_4:
		nReturn = PF4_SUPPORTIDLE | PF4_IMSENDUTF | PF4_IMSENDOFFLINE | PF4_INFOSETTINGSVC;
		if (m_bAvatarsEnabled)
			nReturn |= PF4_AVATARS;
#ifdef DBG_CAPMTN
		nReturn |= PF4_SUPPORTTYPING;
#endif
		break;

	case PFLAGNUM_5:
		nReturn = PF2_FREECHAT;
		if (m_bAimEnabled)
			nReturn |= PF2_ONTHEPHONE;
		break;

	case PFLAG_UNIQUEIDTEXT:
		nReturn = (DWORD_PTR)ICQTranslate("User ID");
		break;

	case PFLAG_UNIQUEIDSETTING:
		nReturn = (DWORD_PTR)UNIQUEIDSETTING;
		break;

	case PFLAG_MAXCONTACTSPERPACKET:
		if ( hContact )
		{
			// determine per contact
			BYTE bClientId = getSettingByte(hContact, "ClientID", CLID_GENERIC);

			if (bClientId == CLID_MIRANDA)
			{
				if (CheckContactCapabilities(hContact, CAPF_CONTACTS) && getContactStatus(hContact) != ID_STATUS_OFFLINE)
					nReturn = 0x100; // limited only by packet size
				else
					nReturn = MAX_CONTACTSSEND;
			}
			else if (bClientId == CLID_ICQ6)
			{
				if (CheckContactCapabilities(hContact, CAPF_CONTACTS))
					nReturn = 1; // crappy ICQ6 cannot handle multiple contacts in the transfer
				else
					nReturn = 0; // this version does not support contacts transfer at all
			}
			else
				nReturn = MAX_CONTACTSSEND;
		}
		else // return generic limit
			nReturn = MAX_CONTACTSSEND;
		break;

	case PFLAG_MAXLENOFMESSAGE:
		nReturn = MAX_MESSAGESNACSIZE-102;
	}

	return nReturn;
}

HICON CIcqProto::GetIcon( int iconIndex )
{
	return NULL;
}

int CIcqProto::GetInfo( HANDLE hContact, int infoType )
{
	if (icqOnline)
	{
		DWORD dwUin;
		uid_str szUid;

		if (getContactUid(hContact, &dwUin, &szUid))
			return 1; // Invalid contact

		DWORD dwCookie;
		if (dwUin)
			dwCookie = icq_sendGetInfoServ(hContact, dwUin, (infoType & SGIF_ONOPEN) != 0);
		else // TODO: this needs something better
			dwCookie = icq_sendGetAimProfileServ(hContact, szUid);

		return (dwCookie) ? 0 : 1;
	}

	return 1; // Failure
}

HANDLE CIcqProto::SearchBasic( const PROTOCHAR* pszSearch )
{
	if (strlennull(pszSearch) == 0)
		return 0;

	char pszUIN[255];
	int nHandle = 0;
	int i, j;

	if (!m_bAimEnabled)
	{
		for (i=j=0; (i<strlennull(pszSearch)) && (j<255); i++)
		{ // we take only numbers
			if ((pszSearch[i]>=0x30) && (pszSearch[i]<=0x39))
			{
				pszUIN[j] = pszSearch[i];
				j++;
			}
		}
	}
	else
	{
		for (i=j=0; (i<strlennull(pszSearch)) && (j<255); i++)
		{ // we remove spaces and slashes
			if ((pszSearch[i]!=0x20) && (pszSearch[i]!='-'))
			{
#ifdef _UNICODE
				if (pszSearch[i] >= 0x80) continue;
#endif
				pszUIN[j] = pszSearch[i];
				j++;
			}
		}
	}
	pszUIN[j] = 0;

	if (strlennull(pszUIN))
	{
		DWORD dwUin;
		if (IsStringUIN(pszUIN))
			dwUin = atoi(pszUIN);
		else
			dwUin = 0;

		// Cheeky instant UIN search
		if (!dwUin || GetKeyState(VK_CONTROL)&0x8000)
		{
			cheekySearchId = GenerateCookie(0);
			cheekySearchUin = dwUin;
			cheekySearchUid = null_strdup(pszUIN);
			ForkThread(&CIcqProto::CheekySearchThread, 0); // The caller needs to get this return value before the results
			nHandle = cheekySearchId;
		}
		else if (icqOnline)
		{
			nHandle = SearchByUin(dwUin);
		}

		// Success
		return (HANDLE)nHandle;
	}

	// Failure
	return 0;
}

HANDLE CIcqProto::SearchByEmail( const PROTOCHAR* email )
{
	if (email && icqOnline && strlennull(email) > 0)
	{
		DWORD dwSearchId, dwSecId;
		char *szEmail = ansi_to_utf8(email);	//tchar_to_ansi(email);

		// Success
		dwSearchId = SearchByMail(szEmail);
		if (m_bAimEnabled)
			dwSecId = icq_searchAimByEmail(szEmail, dwSearchId);
		else
			dwSecId = 0;

		SAFE_FREE(&szEmail);

		if (dwSearchId)
			return ( HANDLE )dwSearchId;
		else
			return ( HANDLE )dwSecId;
	}

	return 0; // Failure
}

HANDLE CIcqProto::SearchByName( const PROTOCHAR* nick, const PROTOCHAR* firstName, const PROTOCHAR* lastName )
{
	if (icqOnline)
	{
		if (nick || firstName || lastName)
		{
			char *nickUtf = ansi_to_utf8(nick);	//tchar_to_utf8(nick);
			char *firstNameUtf = ansi_to_utf8(firstName);	//tchar_to_utf8(firstName);
			char *lastNameUtf = ansi_to_utf8(lastName);	// tchar_to_utf8(lastName);

			// Success
			HANDLE dwCookie = (HANDLE)SearchByNames(nickUtf, firstNameUtf, lastNameUtf, 0);

			SAFE_FREE(&nickUtf);
			SAFE_FREE(&firstNameUtf);
			SAFE_FREE(&lastNameUtf);

			return dwCookie;
		}
	}

	return 0; // Failure
}

HWND CIcqProto::SearchAdvanced( HWND hwndDlg )
{
	if (icqOnline && IsWindow(hwndDlg))
	{
		int nDataLen;
		BYTE* bySearchData;

		if (bySearchData = createAdvancedSearchStructure(hwndDlg, &nDataLen))
		{
			int result = icq_sendAdvancedSearchServ(bySearchData, nDataLen);
			SAFE_FREE((void**)&bySearchData);
			return ( HWND )result; // Success
		}
	}

	return NULL; // Failure
}

HWND CIcqProto::CreateExtendedSearchUI( HWND parent )
{
	if (parent && hInst)
		return CreateDialog(hInst, MAKEINTRESOURCE(IDD_ICQADVANCEDSEARCH), parent, AdvancedSearchDlgProc);

	return NULL; // Failure
}

int CIcqProto::RecvContacts( HANDLE hContact, PROTORECVEVENT* pre)
{
	ICQSEARCHRESULT **isrList = (ICQSEARCHRESULT**)pre->szMessage;
	int i;
	DWORD cbBlob = 0;
	DWORD flags = 0;

	if (pre->flags & PREF_UTF || pre->flags & PREF_UNICODE)
		flags |= DBEF_UTF;

	for (i = 0; i < pre->lParam; i++)
	{
		if (pre->flags & PREF_UNICODE)
			cbBlob += get_utf8_size((WCHAR*)isrList[i]->hdr.nick) + 2;
		else
			cbBlob += strlennull((char*)isrList[i]->hdr.nick) + 2; // both trailing zeros
		if (isrList[i]->uin)
			cbBlob += getUINLen(isrList[i]->uin);
		else if (pre->flags & PREF_UNICODE)
			cbBlob += strlennull((WCHAR*)isrList[i]->hdr.id);
		else
			cbBlob += strlennull((char*)isrList[i]->hdr.id);
	}
	PBYTE pBlob = (PBYTE)_alloca(cbBlob), pCurBlob;
	for (i = 0, pCurBlob = pBlob; i < pre->lParam; i++)
	{
		if (pre->flags & PREF_UNICODE)
			make_utf8_string_static((WCHAR*)isrList[i]->hdr.nick, (char*)pCurBlob, cbBlob - (pCurBlob - pBlob));
		else
			strcpy((char*)pCurBlob, (char*)isrList[i]->hdr.nick);
		pCurBlob += strlennull((char*)pCurBlob) + 1;
		if (isrList[i]->uin)
		{
			char szUin[UINMAXLEN];
			_itoa(isrList[i]->uin, szUin, 10);
			strcpy((char*)pCurBlob, szUin);
		}
		else
		{ // aim contact
			if (pre->flags & PREF_UNICODE)
				unicode_to_ansi_static((WCHAR*)isrList[i]->hdr.id, (char*)pCurBlob, cbBlob - (pCurBlob - pBlob));
			else
				strcpy((char*)pCurBlob, (char*)isrList[i]->hdr.id);
		}
		pCurBlob += strlennull((char*)pCurBlob) + 1;
	}

	ICQAddRecvEvent(hContact, EVENTTYPE_CONTACTS, pre, cbBlob, pBlob, flags);
	return 0;
}

int CIcqProto::RecvFile( HANDLE hContact, PROTOFILEEVENT* )
{
	return 0;
}

int CIcqProto::RecvMsg( HANDLE hContact, PROTORECVEVENT* pre)
{
	DWORD cbBlob;
	DWORD flags = 0;

	cbBlob = strlennull(pre->szMessage) + 1;
	// process utf-8 encoded messages
	if ((pre->flags & PREF_UTF) && !IsUSASCII(pre->szMessage, strlennull(pre->szMessage)))
		flags |= DBEF_UTF;
	// process unicode ucs-2 messages
	if ((pre->flags & PREF_UNICODE) && !IsUnicodeAscii((WCHAR*)(pre->szMessage+cbBlob), strlennull((WCHAR*)(pre->szMessage+cbBlob))))
		cbBlob *= (sizeof(WCHAR)+1);

	ICQAddRecvEvent(hContact, EVENTTYPE_MESSAGE, pre, cbBlob, (PBYTE)pre->szMessage, flags);

	// stop contact from typing - some clients do not sent stop notify
	if (CheckContactCapabilities(hContact, CAPF_TYPING))
		CallService(MS_PROTO_CONTACTISTYPING, (WPARAM)hContact, PROTOTYPE_CONTACTTYPING_OFF);

	return 0;
}

int CIcqProto::RecvUrl( HANDLE hContact, PROTORECVEVENT* pre)
{
	return 1;
}

int CIcqProto::SendContacts( HANDLE hContact, int flags, int nContacts, HANDLE* hContactsList )
{
	if (hContact && hContactsList)
	{
		int i;
		DWORD dwUin;
		uid_str szUid;
		WORD wRecipientStatus;
		DWORD dwCookie;

		if (getContactUid(hContact, &dwUin, &szUid))
		{ // Invalid contact
			return ReportGenericSendError(hContact, ACKTYPE_CONTACTS, "The receiver has an invalid user ID.");
		}

		wRecipientStatus = getContactStatus(hContact);

		// Failures
		if (!icqOnline)
		{
			dwCookie = ReportGenericSendError(hContact, ACKTYPE_CONTACTS, "You cannot send messages when you are offline.");
		}
		else if (!hContactsList || (nContacts < 1) || (nContacts > MAX_CONTACTSSEND))
		{
			dwCookie = ReportGenericSendError(hContact, ACKTYPE_CONTACTS, "Bad data (internal error #1)");
		}
		// OK
		else
		{
			if (CheckContactCapabilities(hContact, CAPF_CONTACTS) && wRecipientStatus != ID_STATUS_OFFLINE)
			{ // Use the new format if possible
				int nDataLen, nNamesLen;
				struct icq_contactsend_s* contacts = NULL;

				// Format the data part and the names part
				// This is kinda messy, but there is no simple way to do it. First
				// we need to calculate the length of the packet.
				contacts = (struct icq_contactsend_s*)_alloca(sizeof(struct icq_contactsend_s)*nContacts);
				ZeroMemory(contacts, sizeof(struct icq_contactsend_s)*nContacts);
				{
					nDataLen = 0; nNamesLen = 0;
					for (i = 0; i < nContacts; i++)
					{
						uid_str szContactUid;

						if (!IsICQContact(hContactsList[i]))
							break; // Abort if a non icq contact is found
						if (getContactUid(hContactsList[i], &contacts[i].uin, &szContactUid))
							break; // Abort if invalid contact
						contacts[i].uid = contacts[i].uin?NULL:null_strdup(szContactUid);
						contacts[i].szNick = NickFromHandleUtf(hContactsList[i]);
						nDataLen += getUIDLen(contacts[i].uin, contacts[i].uid) + 4;
						nNamesLen += strlennull(contacts[i].szNick) + 8;
					}

					if (i == nContacts)
					{
						icq_packet mData, mNames;

#ifdef _DEBUG
						NetLog_Server("Sending contacts to %s.", strUID(dwUin, szUid));
#endif
						// Do not calculate the exact size of the data packet - only the maximal size (easier)
						// Sumarize size of group information
						// - we do not utilize the full power of the protocol and send all contacts with group "General"
						//   just like ICQ6 does
						nDataLen += 9;
						nNamesLen += 9;

						// Create data structures
						mData.wPlace = 0;
						mData.pData = (LPBYTE)SAFE_MALLOC(nDataLen);
						mData.wLen = nDataLen;
						mNames.wPlace = 0;
						mNames.pData = (LPBYTE)SAFE_MALLOC(nNamesLen);

						// pack Group Name
						packWord(&mData, 7);
						packBuffer(&mData, "General", 7);
						packWord(&mNames, 7);
						packBuffer(&mNames, "General", 7);

						// all contacts in one group
						packWord(&mData, (WORD)nContacts);
						packWord(&mNames, (WORD)nContacts);
						for (i = 0; i < nContacts; i++)
						{
							uid_str szContactUid;
							WORD wLen;

							if (contacts[i].uin)
								strUID(contacts[i].uin, szContactUid);
							else
								strcpy(szContactUid, contacts[i].uid);

							// prepare UID
							wLen = strlennull(szContactUid);
							packWord(&mData, wLen);
							packBuffer(&mData, szContactUid, wLen);

							// prepare Nick
							wLen = strlennull(contacts[i].szNick);
							packWord(&mNames, (WORD)(wLen + 4));
							packTLV(&mNames, 0x01, wLen, contacts[i].szNick);
						}

						// Cleanup temporary list
						for(i = 0; i < nContacts; i++)
						{
							SAFE_FREE(&contacts[i].szNick);
							SAFE_FREE(&contacts[i].uid);
						}

						// Rate check
						if (IsServerOverRate(ICQ_MSG_FAMILY, ICQ_MSG_SRV_SEND, RML_LIMIT))
						{ // rate is too high, the message will not go thru...
							SAFE_FREE((void**)&mData.pData);
							SAFE_FREE((void**)&mNames.pData);

							return ReportGenericSendError(hContact, ACKTYPE_CONTACTS, "The message could not be delivered. You are sending too fast. Wait a while and try again.");
						}

						// Set up the ack type
						cookie_message_data *pCookieData = CreateMessageCookieData(MTYPE_CONTACTS, hContact, dwUin, FALSE);

						// AIM clients do not send acknowledgement
						if (!dwUin && pCookieData->nAckType == ACKTYPE_CLIENT)
							pCookieData->nAckType = ACKTYPE_SERVER;
						// Send the message
						dwCookie = icq_SendChannel2Contacts(dwUin, szUid, hContact, (char*)mData.pData, mData.wPlace, (char*)mNames.pData, mNames.wPlace, pCookieData);

						// This will stop the message dialog from waiting for the real message delivery ack
						if (pCookieData->nAckType == ACKTYPE_NONE)
						{
							SendProtoAck(hContact, dwCookie, ACKRESULT_SUCCESS, ACKTYPE_CONTACTS, NULL);
							// We need to free this here since we will never see the real ack
							// The actual cookie value will still have to be returned to the message dialog though
							ReleaseCookie(dwCookie);
						}
						// Release our buffers
						SAFE_FREE((void**)&mData.pData);
						SAFE_FREE((void**)&mNames.pData);
					}
					else
					{
						dwCookie = ReportGenericSendError(hContact, ACKTYPE_CONTACTS, "Bad data (internal error #2)");
					}

					for(i = 0; i < nContacts; i++)
					{
						SAFE_FREE(&contacts[i].szNick);
						SAFE_FREE(&contacts[i].uid);
					}
				}
			}
			else if (dwUin)
			{ // old format is only understood by ICQ clients
				int nBodyLength;
				char szContactUin[UINMAXLEN];
				char szCount[17];
				struct icq_contactsend_s* contacts = NULL;
				uid_str szContactUid;


				// Format the body
				// This is kinda messy, but there is no simple way to do it. First
				// we need to calculate the length of the packet.
				contacts = (struct icq_contactsend_s*)_alloca(sizeof(struct icq_contactsend_s)*nContacts);
				ZeroMemory(contacts, sizeof(struct icq_contactsend_s)*nContacts);
				{
					nBodyLength = 0;
					for (i = 0; i < nContacts; i++)
					{
						if (!IsICQContact(hContactsList[i]))
							break; // Abort if a non icq contact is found
						if (getContactUid(hContactsList[i], &contacts[i].uin, &szContactUid))
							break; // Abort if invalid contact
						contacts[i].uid = contacts[i].uin?NULL:null_strdup(szContactUid);
						contacts[i].szNick = NickFromHandle(hContactsList[i]);
						// Compute this contact's length
						nBodyLength += getUIDLen(contacts[i].uin, contacts[i].uid) + 1;
						nBodyLength += strlennull(contacts[i].szNick) + 1;
					}

					if (i == nContacts)
					{
						char* pBody;
						char* pBuffer;

#ifdef _DEBUG
						NetLog_Server("Sending contacts to %d.", dwUin);
#endif
						// Compute count record's length
						_itoa(nContacts, szCount, 10);
						nBodyLength += strlennull(szCount) + 1;

						// Finally we need to copy the contact data into the packet body
						pBuffer = pBody = (char *)SAFE_MALLOC(nBodyLength);
						null_strcpy(pBuffer, szCount, nBodyLength - 1);
						pBuffer += strlennull(pBuffer);
						*pBuffer++ = (char)0xFE;
						for (i = 0; i < nContacts; i++)
						{
							if (contacts[i].uin)
							{
								_itoa(contacts[i].uin, szContactUin, 10);
								strcpy(pBuffer, szContactUin);
							}
							else
								strcpy(pBuffer, contacts[i].uid);
							pBuffer += strlennull(pBuffer);
							*pBuffer++ = (char)0xFE;
							strcpy(pBuffer, contacts[i].szNick);
							pBuffer += strlennull(pBuffer);
							*pBuffer++ = (char)0xFE;
						}

						for (i = 0; i < nContacts; i++)
						{ // release memory
							SAFE_FREE(&contacts[i].szNick);
							SAFE_FREE(&contacts[i].uid);
						}

						// Set up the ack type
						cookie_message_data *pCookieData = CreateMessageCookieData(MTYPE_CONTACTS, hContact, dwUin, TRUE);

						if (m_bDCMsgEnabled && IsDirectConnectionOpen(hContact, DIRECTCONN_STANDARD, 0))
						{
							int iRes = icq_SendDirectMessage(hContact, pBody, nBodyLength, 1, pCookieData, NULL);

							if (iRes)
							{
								SAFE_FREE((void**)&pBody);

								return iRes; // we succeded, return
							}
						}

						// Rate check
						if (IsServerOverRate(ICQ_MSG_FAMILY, ICQ_MSG_SRV_SEND, RML_LIMIT))
						{ // rate is too high, the message will not go thru...
							SAFE_FREE((void**)&pCookieData);
							SAFE_FREE((void**)&pBody);

							return ReportGenericSendError(hContact, ACKTYPE_CONTACTS, "The message could not be delivered. You are sending too fast. Wait a while and try again.");
						}
						// Select channel and send
						if (!CheckContactCapabilities(hContact, CAPF_SRV_RELAY) || wRecipientStatus == ID_STATUS_OFFLINE)
						{
							dwCookie = icq_SendChannel4Message(dwUin, hContact, MTYPE_CONTACTS, (WORD)nBodyLength, pBody, pCookieData);
						}
						else
						{
							WORD wPriority;

							if (wRecipientStatus == ID_STATUS_ONLINE || wRecipientStatus == ID_STATUS_FREECHAT)
								wPriority = 0x0001;
							else
								wPriority = 0x0021;

							dwCookie = icq_SendChannel2Message(dwUin, hContact, pBody, nBodyLength, wPriority, pCookieData, NULL);
						}

						// This will stop the message dialog from waiting for the real message delivery ack
						if (pCookieData->nAckType == ACKTYPE_NONE)
						{
							SendProtoAck(hContact, dwCookie, ACKRESULT_SUCCESS, ACKTYPE_CONTACTS, NULL);
							// We need to free this here since we will never see the real ack
							// The actual cookie value will still have to be returned to the message dialog though
							ReleaseCookie(dwCookie);
						}
						SAFE_FREE((void**)&pBody);
					}
					else
					{
						dwCookie = ReportGenericSendError(hContact, ACKTYPE_CONTACTS, "Bad data (internal error #2)");
					}
				}
			}
			else
			{
				dwCookie = ReportGenericSendError(hContact, ACKTYPE_CONTACTS, "The reciever does not support receiving of contacts.");
			}
		}
		return dwCookie;
	}

	// Exit with Failure
	return 0;
}

HANDLE CIcqProto::SendFile( HANDLE hContact, const PROTOCHAR* szDescription, PROTOCHAR** ppszFiles )
{
	return NULL;
}

int CIcqProto::SendMsg( HANDLE hContact, int flags, const char* pszSrc )
{
	if (hContact && pszSrc)
	{
		DWORD dwCookie;
		char* puszText = NULL;
		int bNeedFreeU = 0;

		// Invalid contact
		DWORD dwUin;
		uid_str szUID;
		if (getContactUid(hContact, &dwUin, &szUID))
			return ReportGenericSendError(hContact, ACKTYPE_MESSAGE, "The receiver has an invalid user ID.");

		if (flags & PREF_UNICODE)
		{
			puszText = make_utf8_string((WCHAR*)(pszSrc + strlennull(pszSrc) + 1)); // get the UTF-16 part
			bNeedFreeU = 1;
		}
		else if (flags & PREF_UTF)
			puszText = (char*)pszSrc;
		else
		{
			puszText = (char*)ansi_to_utf8(pszSrc);
			bNeedFreeU = 1;
		}

		WORD wRecipientStatus = getContactStatus(hContact);

		BOOL plain_ascii = IsUSASCII(puszText, strlennull(puszText));

		BOOL oldAnsi = plain_ascii || !m_bUtfEnabled || 
			(!(flags & (PREF_UTF | PREF_UNICODE)) && m_bUtfEnabled == 1) ||
			!CheckContactCapabilities(hContact, CAPF_UTF) || 
			!getSettingByte(hContact, "UnicodeSend", 1);

		if (m_bTempVisListEnabled && m_iStatus == ID_STATUS_INVISIBLE)
			makeContactTemporaryVisible(hContact);  // make us temporarily visible to contact

		// Failure scenarios
		if (!icqOnline)
		{
			dwCookie = ReportGenericSendError(hContact, ACKTYPE_MESSAGE, "You cannot send messages when you are offline.");
		}
		else if ((wRecipientStatus == ID_STATUS_OFFLINE) && (strlennull(puszText) > 4096))
		{
			dwCookie = ReportGenericSendError(hContact, ACKTYPE_MESSAGE, "Messages to offline contacts must be shorter than 4096 characters.");
		}
		// Looks OK
		else
		{
			// Set up the ack type
			cookie_message_data *pCookieData = CreateMessageCookieData(MTYPE_PLAIN, hContact, dwUin, TRUE);

#ifdef _DEBUG
			NetLog_Server("Send %smessage - Message cap is %u", puszText ? "unicode " : "", CheckContactCapabilities(hContact, CAPF_SRV_RELAY));
			NetLog_Server("Send %smessage - Contact status is %u", puszText ? "unicode " : "", wRecipientStatus);
#endif
			if (dwUin && m_bDCMsgEnabled && IsDirectConnectionOpen(hContact, DIRECTCONN_STANDARD, 0))
			{ // send thru direct
				char *dc_msg = puszText;
				char *dc_cap = plain_ascii ? NULL : CAP_UTF8MSGS;
				char *szUserAnsi = NULL;

				if (!plain_ascii && oldAnsi)
				{
					szUserAnsi = ConvertMsgToUserSpecificAnsi(hContact, puszText);
					if (szUserAnsi)
					{
						dc_msg = szUserAnsi;
						dc_cap = NULL;
					}
				}

				dwCookie = icq_SendDirectMessage(hContact, dc_msg, strlennull(dc_msg), 1, pCookieData, dc_cap);

				SAFE_FREE(&szUserAnsi);
				if (dwCookie)
				{ // free the buffers if alloced
					if (bNeedFreeU) SAFE_FREE(&puszText);

					return dwCookie; // we succeded, return
				}
				// on failure, fallback to send thru server
			}
			if (!dwUin || !CheckContactCapabilities(hContact, CAPF_SRV_RELAY) || wRecipientStatus == ID_STATUS_OFFLINE)
			{
				/// TODO: add support for RTL & user customizable font
				{
					char *mng = MangleXml(puszText, strlennull(puszText));
					int len = strlennull(mng);
					mng = (char*)SAFE_REALLOC(mng, len + 28);
					memmove(mng + 12, mng, len + 1);
					memcpy(mng, "<HTML><BODY>", 12);
					strcat(mng, "</BODY></HTML>");
					if (bNeedFreeU) SAFE_FREE(&puszText);
					puszText = mng;
					bNeedFreeU = 1;
				}

				WCHAR *pwszText = plain_ascii ? NULL : make_unicode_string(puszText);
				if ((plain_ascii ? strlennull(puszText) : strlennull(pwszText) * sizeof(WCHAR)) > MAX_MESSAGESNACSIZE)
				{ // max length check // TLV(2) is currently limited to 0xA00 bytes in online mode
					// only limit to not get disconnected, all other will be handled by error 0x0A
					dwCookie = ReportGenericSendError(hContact, ACKTYPE_MESSAGE, "The message could not be delivered, it is too long.");

					SAFE_FREE((void**)&pCookieData);
					// free the buffers if alloced
					SAFE_FREE((void**)&pwszText);
					if (bNeedFreeU) SAFE_FREE(&puszText);

					return dwCookie;
				}
				// Rate check
				if (IsServerOverRate(ICQ_MSG_FAMILY, ICQ_MSG_SRV_SEND, RML_LIMIT))
				{ // rate is too high, the message will not go thru...
					dwCookie = ReportGenericSendError(hContact, ACKTYPE_MESSAGE, "The message could not be delivered. You are sending too fast. Wait a while and try again.");

					SAFE_FREE((void**)&pCookieData);
					// free the buffers if alloced
					SAFE_FREE((void**)&pwszText);
					if (bNeedFreeU) SAFE_FREE(&puszText);

					return dwCookie;
				}
				// set flag for offline messages - to allow proper error handling
				if (wRecipientStatus == ID_STATUS_OFFLINE) ((cookie_message_data_ext*)pCookieData)->isOffline = TRUE;

				if (plain_ascii)
					dwCookie = icq_SendChannel1Message(dwUin, szUID, hContact, puszText, pCookieData);
				else
					dwCookie = icq_SendChannel1MessageW(dwUin, szUID, hContact, pwszText, pCookieData);
				// free the unicode message
				SAFE_FREE((void**)&pwszText);
			}
			else
			{
				WORD wPriority;

				char *srv_msg = puszText;
				char *srv_cap = plain_ascii ? NULL : CAP_UTF8MSGS;
				char *szUserAnsi = NULL;

				if (!plain_ascii && oldAnsi)
				{
					szUserAnsi = ConvertMsgToUserSpecificAnsi(hContact, puszText);
					if (szUserAnsi)
					{
						srv_msg = szUserAnsi;
						srv_cap = NULL;
					}
				}

				if (wRecipientStatus == ID_STATUS_ONLINE || wRecipientStatus == ID_STATUS_FREECHAT)
					wPriority = 0x0001;
				else
					wPriority = 0x0021;

				if (strlennull(srv_msg) + (!oldAnsi ? 144 : 102) > MAX_MESSAGESNACSIZE)
				{ // max length check
					dwCookie = ReportGenericSendError(hContact, ACKTYPE_MESSAGE, "The message could not be delivered, it is too long.");

					SAFE_FREE(&szUserAnsi);
					SAFE_FREE((void**)&pCookieData);
					// free the buffers if alloced
					if (bNeedFreeU) SAFE_FREE(&puszText);

					return dwCookie;
				}
				// Rate check
				if (IsServerOverRate(ICQ_MSG_FAMILY, ICQ_MSG_SRV_SEND, RML_LIMIT))
				{ // rate is too high, the message will not go thru...
					dwCookie = ReportGenericSendError(hContact, ACKTYPE_MESSAGE, "The message could not be delivered. You are sending too fast. Wait a while and try again.");

					SAFE_FREE(&szUserAnsi);
					SAFE_FREE((void**)&pCookieData);
					// free the buffers if alloced
					if (bNeedFreeU) SAFE_FREE(&puszText);

					return dwCookie;
				}
				// WORKAROUND!!
				// Nasty workaround for ICQ6 client's bug - it does not send acknowledgement when in temporary visible mode
				// - This uses only server ack, but also for visible invisible contact!
				if (wRecipientStatus == ID_STATUS_INVISIBLE && pCookieData->nAckType == ACKTYPE_CLIENT && getSettingByte(hContact, "ClientID", CLID_GENERIC) == CLID_ICQ6)
					pCookieData->nAckType = ACKTYPE_SERVER;

				dwCookie = icq_SendChannel2Message(dwUin, hContact, srv_msg, strlennull(srv_msg), wPriority, pCookieData, srv_cap);
				SAFE_FREE(&szUserAnsi);
			}

			// This will stop the message dialog from waiting for the real message delivery ack
			if (pCookieData->nAckType == ACKTYPE_NONE)
			{
				SendProtoAck(hContact, dwCookie, ACKRESULT_SUCCESS, ACKTYPE_MESSAGE, NULL);
				// We need to free this here since we will never see the real ack
				// The actual cookie value will still have to be returned to the message dialog though
				ReleaseCookie(dwCookie);
			}
		}
		// free the buffers if alloced
		if (bNeedFreeU) SAFE_FREE(&puszText);

		return dwCookie; // Success
	}

	return 0; // Failure
}

int CIcqProto::SendUrl( HANDLE hContact, int flags, const char* url )
{
	if (hContact && url)
	{
		DWORD dwCookie;
		WORD wRecipientStatus;
		DWORD dwUin;

		if (getContactUid(hContact, &dwUin, NULL))
		{ // Invalid contact
			return ReportGenericSendError(hContact, ACKTYPE_URL, "The receiver has an invalid user ID.");
		}

		wRecipientStatus = getContactStatus(hContact);

		// Failure
		if (!icqOnline)
		{
			dwCookie = ReportGenericSendError(hContact, ACKTYPE_URL, "You cannot send messages when you are offline.");
		}
		// Looks OK
		else
		{
			char* szDesc;
			char* szBody;
			int nBodyLen;
			int nDescLen;
			int nUrlLen;


			// Set up the ack type
			cookie_message_data *pCookieData = CreateMessageCookieData(MTYPE_URL, hContact, dwUin, TRUE);

			// Format the body
			nUrlLen = strlennull(url);
			szDesc = (char *)url + nUrlLen + 1;
			nDescLen = strlennull(szDesc);
			nBodyLen = nUrlLen + nDescLen + 2;
			szBody = (char *)_alloca(nBodyLen);
			strcpy(szBody, szDesc);
			szBody[nDescLen] = (char)0xFE; // Separator
			strcpy(szBody + nDescLen + 1, url);

			if (m_bDCMsgEnabled && IsDirectConnectionOpen(hContact, DIRECTCONN_STANDARD, 0))
			{
				int iRes = icq_SendDirectMessage(hContact, szBody, nBodyLen, 1, pCookieData, NULL);
				if (iRes) return iRes; // we succeded, return
			}

			// Rate check
			if (IsServerOverRate(ICQ_MSG_FAMILY, ICQ_MSG_SRV_SEND, RML_LIMIT))
			{ // rate is too high, the message will not go thru...
				SAFE_FREE((void**)&pCookieData);

				return ReportGenericSendError(hContact, ACKTYPE_URL, "The message could not be delivered. You are sending too fast. Wait a while and try again.");
			}
			// Select channel and send
			if (!CheckContactCapabilities(hContact, CAPF_SRV_RELAY) ||
				wRecipientStatus == ID_STATUS_OFFLINE)
			{
				dwCookie = icq_SendChannel4Message(dwUin, hContact, MTYPE_URL,
					(WORD)nBodyLen, szBody, pCookieData);
			}
			else
			{
				WORD wPriority;

				if (wRecipientStatus == ID_STATUS_ONLINE || wRecipientStatus == ID_STATUS_FREECHAT)
					wPriority = 0x0001;
				else
					wPriority = 0x0021;

				dwCookie = icq_SendChannel2Message(dwUin, hContact, szBody, nBodyLen, wPriority, pCookieData, NULL);
			}

			// This will stop the message dialog from waiting for the real message delivery ack
			if (pCookieData->nAckType == ACKTYPE_NONE)
			{
				SendProtoAck(hContact, dwCookie, ACKRESULT_SUCCESS, ACKTYPE_URL, NULL);
				// We need to free this here since we will never see the real ack
				// The actual cookie value will still have to be returned to the message dialog though
				ReleaseCookie(dwCookie);
			}
		}

		return dwCookie; // Success
	}

	return 0; // Failure
}

int CIcqProto::SetApparentMode( HANDLE hContact, int mode )
{
	DWORD uin;
	uid_str uid;

	if (getContactUid(hContact, &uin, &uid))
		return 1; // Invalid contact

	if (hContact)
	{
		// Only 3 modes are supported
		if (mode == 0 || mode == ID_STATUS_ONLINE || mode == ID_STATUS_OFFLINE)
		{
			int oldMode = getSettingWord(hContact, "ApparentMode", 0);

			// Don't send redundant updates
			if (mode != oldMode)
			{
				setSettingWord(hContact, "ApparentMode", (WORD)mode);

				// Not being online is only an error when in SS mode. This is not handled
				// yet so we just ignore this for now.
				if (icqOnline)
				{
					if (oldMode != 0)
					{ // Remove from old list
						if (oldMode == ID_STATUS_OFFLINE && getSettingWord(hContact, DBSETTING_SERVLIST_IGNORE, 0))
						{ // Need to remove Ignore item as well
							icq_removeServerPrivacyItem(hContact, uin, uid, getSettingWord(hContact, DBSETTING_SERVLIST_IGNORE, 0), SSI_ITEM_IGNORE);

							setSettingWord(hContact, DBSETTING_SERVLIST_IGNORE, 0);
						}
						icq_sendChangeVisInvis(hContact, uin, uid, oldMode==ID_STATUS_OFFLINE, 0);
					}
					if (mode != 0)
					{ // Add to new list
						if (mode==ID_STATUS_OFFLINE && getSettingWord(hContact, DBSETTING_SERVLIST_IGNORE, 0))
							return 0; // Success: offline by ignore item

						icq_sendChangeVisInvis(hContact, uin, uid, mode==ID_STATUS_OFFLINE, 1);
					}
				}

				return 0; // Success
			}
		}
	}

	return 1; // Failure
}

int CIcqProto::SetStatus( int iNewStatus )
{
	IcqSetStatus(iNewStatus, 0);
	return 0;
}

HANDLE CIcqProto::GetAwayMsg( HANDLE hContact )	// FIXME: This mother wasn't copied from official plugin
{
	if (icqOnline)
	{
		DWORD dwUin;
		uid_str szUID;
		WORD wStatus;

		if (getContactUid(hContact, &dwUin, &szUID))
			return 0; // Invalid contact

		wStatus = getContactStatus(hContact);

		if (dwUin)
		{
			int wMessageType = 0;

			switch(wStatus)
			{
			case ID_STATUS_ONLINE:
				{
					DBVARIANT dbv = { DBVT_TCHAR };
					DBGetContactSettingTString( hContact, ICQ_PROTOCOL_NAME, "MirVer", &dbv );
					if ( _tcscmp( dbv.ptszVal, _T( "ICQ 6" ) ) == 0 )
						wMessageType = MTYPE_AUTOAWAY; // works with ICQ 6
					else // other clients
						wMessageType = MTYPE_AUTOONLINE; // works with ICQJ Plus Mod + most of all clients
				}
				break;

			case ID_STATUS_AWAY:
				wMessageType = MTYPE_AUTOAWAY;
				break;

			case ID_STATUS_NA:
				wMessageType = MTYPE_AUTONA;
				break;

			case ID_STATUS_OCCUPIED:
				wMessageType = MTYPE_AUTOBUSY;
				break;

			case ID_STATUS_DND:
				wMessageType = MTYPE_AUTODND;
				break;

			case ID_STATUS_FREECHAT:
				wMessageType = MTYPE_AUTOFFC;
				break;

			default:
				break;
			}

			if (wMessageType)
			{
				if (m_bDCMsgEnabled && IsDirectConnectionOpen(hContact, DIRECTCONN_STANDARD, 0)&& !invis_for(0,hContact))
				{
					int iRes = icq_sendGetAwayMsgDirect(hContact, wMessageType);
					if (iRes)
						return (HANDLE)iRes; // we succeeded, return
				}
				else if(/*bStealthRequest && */(bIncognitoRequest || bIncognitoGlobal))
				{
					bIncognitoRequest = FALSE;
					return (HANDLE)icq_sendGetStealthAwayMsgServ(hContact, dwUin, wMessageType,
						(WORD)(getSettingWord(hContact, "Version", 0)==9?9:ICQ_VERSION)); // Success
				}
				else if(CheckContactCapabilities(hContact, CAPF_STATUSMSGEXT)&& !invis_for(0, hContact))
				{
					return (HANDLE)icq_sendGetAwayMsgServExt(hContact, dwUin, wMessageType,
						(WORD)(getSettingWord(hContact, "Version", 0)==9?9:ICQ_VERSION)); // Success
				}
				else if(!invis_for(0, hContact))
				{
					return (HANDLE)icq_sendGetAwayMsgServ(hContact, dwUin, wMessageType, (WORD)(getSettingWord(hContact, "Version", 0)==9?9:ICQ_VERSION)); // Success
				}
			}
		}
		else
		{
			if (wStatus == ID_STATUS_AWAY)
				return (HANDLE)icq_sendGetAimAwayMsgServ(hContact, szUID, MTYPE_AUTOAWAY);
		}
	}

	return 0; // Failure
}

int CIcqProto::RecvAwayMsg( HANDLE hContact, int mode, PROTORECVEVENT* evt )
{
	if (evt->flags & PREF_UTF)
	{
		setStatusMsgVar(hContact, evt->szMessage, false);

		char* pszMsg = NULL;
		utf8_decode(evt->szMessage, &pszMsg);
		BroadcastAck(hContact, ACKTYPE_AWAYMSG, ACKRESULT_SUCCESS, (HANDLE)evt->lParam, (LPARAM)pszMsg);
		SAFE_FREE(&pszMsg);
	}
	else
	{
		setStatusMsgVar(hContact, evt->szMessage, true);
		BroadcastAck(hContact, ACKTYPE_AWAYMSG, ACKRESULT_SUCCESS, (HANDLE)evt->lParam, (LPARAM)evt->szMessage);
	}
	return 0;
}

int CIcqProto::SendAwayMsg( HANDLE hContact, HANDLE hProcess, const char* msg )
{
	return 1;
}

// FIXME: It's not copied from official plugin.
int CIcqProto::SetAwayMsg( int iStatus, const PROTOCHAR* msg )
{
	//char *szNewUtf = tchar_to_utf8(msg);
	return IcqSetAwayMsg(iStatus, (LPARAM)msg);
}

int CIcqProto::UserIsTyping( HANDLE hContact, int type )
{
	if (hContact && icqOnline)
	{
		if (CheckContactCapabilities(hContact, CAPF_TYPING))
		{
			switch (type) {
			case PROTOTYPE_SELFTYPING_ON:
				sendTypingNotification(hContact, MTN_BEGUN);
				return 0;

			case PROTOTYPE_SELFTYPING_OFF:
				sendTypingNotification(hContact, MTN_FINISHED);
				return 0;
			}
		}
	}

	return 1;
}

int CIcqProto::OnEvent( PROTOEVENTTYPE iEventType, WPARAM wParam, LPARAM lParam )
{
	switch (iEventType)
	{
	case EV_PROTO_ONLOAD:
		return OnSystemModulesLoaded(0, 0);
		break;

	case EV_PROTO_ONREADYTOEXIT:
		break;

	case EV_PROTO_ONEXIT:
		// OnSystemPreShutdown(0, 0);	// It's not called in IcqJ
		icq_InfoUpdateCleanup();
		return 0;
		break;

	case EV_PROTO_ONRENAME:
		break;

	case EV_PROTO_ONOPTIONS:
		return IcqOptInit(0, 0);
		//return OnOptionsInit(wParam, lParam);
		break;

	case EV_PROTO_ONERASE:
		{
			char szDbSetting[MAX_PATH];

			null_snprintf(szDbSetting, sizeof(szDbSetting), "%sP2P", m_szModuleName);
			CallService(MS_DB_MODULE_DELETE, 0, (LPARAM)szDbSetting);
			null_snprintf(szDbSetting, sizeof(szDbSetting), "%sSrvGroups", m_szModuleName);
			CallService(MS_DB_MODULE_DELETE, 0, (LPARAM)szDbSetting);
			null_snprintf(szDbSetting, sizeof(szDbSetting), "%sGroups", m_szModuleName);
			CallService(MS_DB_MODULE_DELETE, 0, (LPARAM)szDbSetting);

			break;
		}
	case EV_PROTO_ONMENU: break;
	}

	return 1;
}

char* CIcqProto::ConvertMsgToUserSpecificAnsi(HANDLE hContact, const char* szMsg)
{ // this takes utf-8 encoded message
	WORD wCP = getSettingWord(hContact, "CodePage", m_wAnsiCodepage);
	char* szAnsi = NULL;

	if (wCP != CP_ACP) // convert to proper codepage
		if (!utf8_decode_codepage(szMsg, &szAnsi, wCP))
			return NULL;

	return szAnsi;
}

void CIcqProto::ForkThread( IcqThreadFunc pFunc, void* arg )
{
	CloseHandle(( HANDLE )mir_forkthreadowner(( pThreadFuncOwner )*( void** )&pFunc, this, arg, NULL ));
}

void CIcqProto::CheekySearchThread( void* )
{
	char szUin[UINMAXLEN];
	ICQSEARCHRESULT isr = {0};
	isr.hdr.cbSize = sizeof(isr);

	if (cheekySearchUin)
	{
		_itoa(cheekySearchUin, szUin, 10);
		isr.hdr.id = (FNAMECHAR*)szUin;
	}
	else
	{
		isr.hdr.id = (FNAMECHAR*)cheekySearchUid;
	}
	isr.uin = cheekySearchUin;

	BroadcastAck(NULL, ACKTYPE_SEARCH, ACKRESULT_DATA, (HANDLE)cheekySearchId, (LPARAM)&isr);
	BroadcastAck(NULL, ACKTYPE_SEARCH, ACKRESULT_SUCCESS, (HANDLE)cheekySearchId, 0);
	cheekySearchId = -1;
}

extern HANDLE hServerConn;

int CIcqProto::OnSystemPreShutdown(WPARAM wParam,LPARAM lParam)
{
	// all threads should be terminated here
	if (hServerConn)
	{
		icq_sendCloseConnection();
		icq_serverDisconnect(TRUE);
	}
	CheckSelfRemoveShutdown();

	return 0;
}

HANDLE CIcqProto::CreateProtoEvent(const char* szEvent)
{
	char str[MAX_PATH + 32];
	strcpy(str, ICQ_PROTOCOL_NAME);
	strcat(str, szEvent);
	return CreateHookableEvent(str);
}

HANDLE CIcqProto::CreateProtoService(const char* szService, MIRANDASERVICE serviceProc)
{
	char str[MAX_PATH + 32];
	strcpy(str, ICQ_PROTOCOL_NAME);
	strcat(str, szService);
	return CreateServiceFunction(str, serviceProc);
}

// FIXME: Sync this with IcqJ
int CIcqProto::ServListDbSettingChanged(WPARAM wParam,LPARAM lParam)
{
	DBCONTACTWRITESETTING *cws = ( DBCONTACTWRITESETTING* )lParam;
	HANDLE hCSCContact = ( HANDLE )wParam;

	if( hCSCContact == NULL || lstrcmpA( cws->szSetting, "Status" ) )
		return 0;
	if ( cws->value.wVal == ID_STATUS_OFFLINE )
	{
		// if contact goes offline, delete xstatus details
		DBDeleteContactSetting( hCSCContact, ICQ_PROTOCOL_NAME, "XStatusId" );
		DBDeleteContactSetting( hCSCContact, ICQ_PROTOCOL_NAME, "XStatusMsg" );
		DBDeleteContactSetting( hCSCContact, ICQ_PROTOCOL_NAME, "XStatusName" );
	}
	return 0;
}

CIcqProto* pProtocol = NULL;
