// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin Öberg, Sam Kothari, Robert Rainwater
// Copyright © 2004,2005,2006,2007 Joe Kucera
// Copyright © 2006,2007,2008 [sss], chaos.persei, [sin], Faith Healer, Thief, Angeli-Ka, nullbie
// Copyright © 2008 [sss], chaos.persei, nullbie, baloo, jarvis
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// -----------------------------------------------------------------------------
//
// File name      : $HeadURL: https://icqjplusmod.googlecode.com/svn/trunk/utilities.c $
// Revision       : $Revision: 51 $
// Last change on : $Date: 2007-08-30 23:46:51 +0300 (Ð§Ñ‚, 30 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"
#include "IcqCore.h"

typedef struct gateway_index_s
{
    HANDLE hConn;
    DWORD  dwIndex;
} gateway_index;

extern CRITICAL_SECTION cookieMutex;

static gateway_index *gateways = NULL;
static int gatewayCount = 0;

static DWORD *spammerList = NULL;
static int spammerListCount = 0;


static icq_contacts_cache *contacts_cache = NULL;
static int cacheCount = 0;
static int cacheListSize = 0;
static CRITICAL_SECTION cacheMutex;

extern BOOL bIsSyncingCL;


void EnableDlgItem(HWND hwndDlg, UINT control, int state)
{
    EnableWindow(GetDlgItem(hwndDlg, control), state);
}

void ShowDlgItem(HWND hwndDlg, UINT control, int state)
{
    ShowWindow(GetDlgItem(hwndDlg, control), state);
}

void icq_EnableMultipleControls(HWND hwndDlg, const UINT *controls, int cControls, int state)
{
    int i;

    for (i = 0; i < cControls; i++)
        EnableDlgItem(hwndDlg, controls[i], state);
}

void icq_ShowMultipleControls(HWND hwndDlg, const UINT *controls, int cControls, int state)
{
    int i;

    for(i = 0; i < cControls; i++)
        ShowWindow(GetDlgItem(hwndDlg, controls[i]), state);
}


// Maps the ICQ status flag (as seen in the status change SNACS) and returns
// a Miranda style status.
int IcqStatusToMiranda(WORD nIcqStatus)
{
    int nMirandaStatus;

    // :NOTE: The order in which the flags are compared are important!
    // I don't like this method but it works.

    if (nIcqStatus & ICQ_STATUSF_INVISIBLE)
        nMirandaStatus = ID_STATUS_INVISIBLE;
    else if (nIcqStatus & ICQ_STATUSF_DND)
        nMirandaStatus = ID_STATUS_DND;
    else if (nIcqStatus & ICQ_STATUSF_OCCUPIED)
        nMirandaStatus = ID_STATUS_OCCUPIED;
    else if (nIcqStatus & ICQ_STATUSF_NA)
        nMirandaStatus = ID_STATUS_NA;
    else if (nIcqStatus & ICQ_STATUSF_AWAY)
        nMirandaStatus = ID_STATUS_AWAY;
    else if (nIcqStatus & ICQ_STATUSF_FFC)
        nMirandaStatus = ID_STATUS_FREECHAT;
    else
        // Can be discussed, but I think 'online' is the most generic ICQ status
        nMirandaStatus = ID_STATUS_ONLINE;

    return nMirandaStatus;
}

WORD MirandaStatusToIcq(int nMirandaStatus)
{
    WORD nIcqStatus;

    switch (nMirandaStatus)
    {
    case ID_STATUS_ONLINE:
        nIcqStatus = ICQ_STATUS_ONLINE; //ICQGetContactSettingWord(NULL, "QIPStatus", ICQ_STATUS_ONLINE);
        break;

    case ID_STATUS_AWAY:
        nIcqStatus = ICQ_STATUS_AWAY;
        break;

    case ID_STATUS_OUTTOLUNCH:
    case ID_STATUS_NA:
        nIcqStatus = ICQ_STATUS_NA;
        break;

    case ID_STATUS_ONTHEPHONE:
    case ID_STATUS_OCCUPIED:
        nIcqStatus = ICQ_STATUS_OCCUPIED;
        break;

    case ID_STATUS_DND:
        nIcqStatus = ICQ_STATUS_DND;
        break;

    case ID_STATUS_INVISIBLE:
        nIcqStatus = ICQ_STATUS_INVISIBLE;
        break;

    case ID_STATUS_FREECHAT:
        nIcqStatus = ICQ_STATUS_FFC;
        break;

    case ID_STATUS_OFFLINE:
        // Oscar doesnt have anything that maps to this status. This should never happen.
#ifdef _DEBUG
        _ASSERTE(nMirandaStatus != ID_STATUS_OFFLINE);
#endif
        nIcqStatus = 0;
        break;

    default:
        // Online seems to be a good default.
        // Since it cant be offline, it must be a new type of online status.
        nIcqStatus = ICQ_STATUS_ONLINE;
        break;
    }

    return nIcqStatus;
}

int MirandaStatusToSupported(int nMirandaStatus)
{
    int nSupportedStatus;

    switch (nMirandaStatus)
    {
	// These status mode does not need any mapping
    case ID_STATUS_ONLINE:
    case ID_STATUS_AWAY:
    case ID_STATUS_NA:
    case ID_STATUS_OCCUPIED:
    case ID_STATUS_DND:
    case ID_STATUS_INVISIBLE:
    case ID_STATUS_FREECHAT:
    case ID_STATUS_OFFLINE:
        nSupportedStatus = nMirandaStatus;
        break;

        // This mode is not support and must be mapped to something else
    case ID_STATUS_OUTTOLUNCH:
        nSupportedStatus = ID_STATUS_NA;
        break;

        // This mode is not support and must be mapped to something else
    case ID_STATUS_ONTHEPHONE:
        nSupportedStatus = ID_STATUS_OCCUPIED;
        break;

        // This is not supposed to happen.
    default:
#ifdef _DEBUG
        _ASSERTE(0);
#endif
        // Online seems to be a good default.
        nSupportedStatus = ID_STATUS_ONLINE;
        break;
    }

    return nSupportedStatus;
}

char* MirandaStatusToString(int mirandaStatus)
{
    return (char *)CallService(MS_CLIST_GETSTATUSMODEDESCRIPTION, mirandaStatus, 0);
}

char* MirandaStatusToStringUtf(int mirandaStatus)
{
    // return miranda status description in utf-8, use unicode service is possible
    return mir_utf8encodeT((TCHAR*)CallService(MS_CLIST_GETSTATUSMODEDESCRIPTION, mirandaStatus, gbUnicodeCore_dep ? 2 /*GCMDF_UNICODE*/ : 0));
}



char** CIcqProto::_MirandaStatusToAwayMsg(int nStatus)
{
    switch (nStatus)
    {
    case ID_STATUS_ONLINE:
        return &m_modeMsgs.szOnline;
        break;

    case ID_STATUS_AWAY:
        return &m_modeMsgs.szAway;
        break;

    case ID_STATUS_NA:
        return &m_modeMsgs.szNa;

    case ID_STATUS_OCCUPIED:
        return &m_modeMsgs.szOccupied;

    case ID_STATUS_DND:
        return &m_modeMsgs.szDnd;

    case ID_STATUS_FREECHAT:
        return &m_modeMsgs.szFfc;

    default:
        return NULL;
    }
}



int AwayMsgTypeToStatus(int nMsgType)
{
    switch (nMsgType)
    {
    case MTYPE_AUTOONLINE:
        return ID_STATUS_ONLINE;

    case MTYPE_AUTOAWAY:
        return ID_STATUS_AWAY;

    case MTYPE_AUTOBUSY:
        return ID_STATUS_OCCUPIED;

    case MTYPE_AUTONA:
        return ID_STATUS_NA;

    case MTYPE_AUTODND:
        return ID_STATUS_DND;

    case MTYPE_AUTOFFC:
        return ID_STATUS_FREECHAT;

    default:
        return ID_STATUS_OFFLINE;
    }
}



void SetGatewayIndex(HANDLE hConn, DWORD dwIndex)
{
    int i;

    EnterCriticalSection(&cookieMutex);

    for (i = 0; i < gatewayCount; i++)
    {
        if (hConn == gateways[i].hConn)
        {
            gateways[i].dwIndex = dwIndex;

            LeaveCriticalSection(&cookieMutex);

            return;
        }
    }

    gateways = (gateway_index *)mir_realloc(gateways, sizeof(gateway_index) * (gatewayCount + 1));
    gateways[gatewayCount].hConn = hConn;
    gateways[gatewayCount].dwIndex = dwIndex;
    gatewayCount++;

    LeaveCriticalSection(&cookieMutex);

    return;
}



DWORD GetGatewayIndex(HANDLE hConn)
{
    int i;

    EnterCriticalSection(&cookieMutex);

    for (i = 0; i < gatewayCount; i++)
    {
        if (hConn == gateways[i].hConn)
        {
            LeaveCriticalSection(&cookieMutex);

            return gateways[i].dwIndex;
        }
    }

    LeaveCriticalSection(&cookieMutex);

    return 1; // this is default
}



void FreeGatewayIndex(HANDLE hConn)
{
    int i;


    EnterCriticalSection(&cookieMutex);

    for (i = 0; i < gatewayCount; i++)
    {
        if (hConn == gateways[i].hConn)
        {
            gatewayCount--;
            memmove(&gateways[i], &gateways[i+1], sizeof(gateway_index) * (gatewayCount - i));
            gateways = (gateway_index*)mir_realloc(gateways, sizeof(gateway_index) * gatewayCount);

            // Gateway found, exit loop
            break;
        }
    }

    LeaveCriticalSection(&cookieMutex);
}



void AddToSpammerList(DWORD dwUIN)
{
    EnterCriticalSection(&cookieMutex);

    spammerList = (DWORD *)mir_realloc(spammerList, sizeof(DWORD) * (spammerListCount + 1));
    spammerList[spammerListCount] = dwUIN;
    spammerListCount++;

    LeaveCriticalSection(&cookieMutex);
}



BOOL IsOnSpammerList(DWORD dwUIN)
{
    int i;

    EnterCriticalSection(&cookieMutex);

    for (i = 0; i < spammerListCount; i++)
    {
        if (dwUIN == spammerList[i])
        {
            LeaveCriticalSection(&cookieMutex);

            return TRUE;
        }
    }

    LeaveCriticalSection(&cookieMutex);

    return FALSE;
}



// ICQ contacts cache
static void AddToCache(HANDLE hContact, DWORD dwUin)
{
    int i = 0;

    if (!hContact || !dwUin)
        return;

    EnterCriticalSection(&cacheMutex);

    if (cacheCount + 1 >= cacheListSize)
    {
        cacheListSize += 100;
        contacts_cache = (icq_contacts_cache *)mir_realloc(contacts_cache, sizeof(icq_contacts_cache) * cacheListSize);
    }

#ifdef _DEBUG
    Netlib_Logf(ghServerNetlibUser, "Adding contact to cache: %u, position: %u", dwUin, cacheCount);
#endif

    contacts_cache[cacheCount].hContact = hContact;
    contacts_cache[cacheCount].dwUin = dwUin;

    cacheCount++;

    LeaveCriticalSection(&cacheMutex);
}

int GetFromCacheByID(int ID, HANDLE *hContact, DWORD *dwUin)
{

    DWORD uin;

    if (ID < 0 || ID >= cacheCount)
        return 0;

    EnterCriticalSection(&cacheMutex);

    if (hContact)
        *hContact = contacts_cache[ID].hContact;

    uin = contacts_cache[ID].dwUin;

    if (dwUin)
        *dwUin = uin;

    LeaveCriticalSection(&cacheMutex);

    return uin; // to be sure it is not 0

}


void InitCache(void)
{
    HANDLE hContact;

    InitializeCriticalSection(&cacheMutex);
    cacheCount = 0;
    cacheListSize = 0;
    contacts_cache = NULL;

    // build cache
    EnterCriticalSection(&cacheMutex);

    hContact = FindFirstContact();

    while (hContact)
    {
        DWORD dwUin;

        dwUin = getContactUin(hContact);
        if (dwUin) AddToCache(hContact, dwUin);

        hContact = FindNextContact(hContact);
    }

    LeaveCriticalSection(&cacheMutex);
}



void UninitCache(void)
{
    mir_free(contacts_cache);

    DeleteCriticalSection(&cacheMutex);
}



void DeleteFromCache(HANDLE hContact)
{
    int i;

    if (cacheCount == 0)
        return;

    EnterCriticalSection(&cacheMutex);

    for (i = cacheCount-1; i >= 0; i--)
        if (contacts_cache[i].hContact == hContact)
        {
            cacheCount--;

#ifdef _DEBUG
            Netlib_Logf(ghServerNetlibUser, "Removing contact from cache: %u, position: %u", contacts_cache[i].dwUin, i);
#endif
            // move last contact to deleted position
            if (i < cacheCount)
                memcpy(&contacts_cache[i], &contacts_cache[cacheCount], sizeof(icq_contacts_cache));

            // clear last contact position
            ZeroMemory(&contacts_cache[cacheCount], sizeof(icq_contacts_cache));

            break;
        }

    LeaveCriticalSection(&cacheMutex);
}



static HANDLE HandleFromCacheByUin(DWORD dwUin)
{
    int i;
    HANDLE hContact = NULL;

    if (cacheCount == 0)
        return hContact;

    EnterCriticalSection(&cacheMutex);

    for (i = cacheCount-1; i >= 0; i--)
        if (contacts_cache[i].dwUin == dwUin)
        {
            hContact = contacts_cache[i].hContact;
            break;
        }

    LeaveCriticalSection(&cacheMutex);

    return hContact;
}



HANDLE HContactFromUIN(DWORD uin, int *Added)
{
    HANDLE hContact;

    if (Added) *Added = 0;

    hContact = HandleFromCacheByUin(uin);
    if (hContact) return hContact;

    hContact = FindFirstContact();
    while (hContact != NULL)
    {
        DWORD dwUin;

        dwUin = getContactUin(hContact);
        if (dwUin == uin)
        {
            AddToCache(hContact, dwUin);
            return hContact;
        }

        hContact = FindNextContact(hContact);
    }

    //not present: add
    if (Added)
    {
        hContact = (HANDLE)CallService(MS_DB_CONTACT_ADD, 0, 0);
        if (!hContact)
        {
            NetLog_Server("Failed to create ICQ contact %u", uin);
            return INVALID_HANDLE_VALUE;
        }

        if (CallService(MS_PROTO_ADDTOCONTACT, (WPARAM)hContact, (LPARAM)ICQ_PROTOCOL_NAME) != 0)
        {
            // For some reason we failed to register the protocol to this contact
            CallService(MS_DB_CONTACT_DELETE, (WPARAM)hContact, 0);
            NetLog_Server("Failed to register ICQ contact %u", uin);
            return INVALID_HANDLE_VALUE;
        }

        setSettingDword(hContact, UNIQUEIDSETTING, uin);

        if (!bIsSyncingCL)
        {
            DBWriteContactSettingByte(hContact, "CList", "NotOnList", 1);
            setContactHidden(hContact, 1);

            setSettingWord(hContact, "Status", ID_STATUS_OFFLINE);

            icq_QueueUser(hContact);

            if (icqOnline)
            {
                icq_sendNewContact(uin, NULL);
                // else need to wait for CList/NotOnList to be deleted
                //icq_GetUserStatus(hContact, 3);
            }
            if (getSettingByte(NULL, "KillSpambots", DEFAULT_KILLSPAM_ENABLED))
                icq_sendGetLocationInfo(hContact, uin, NULL);
        }
        AddToCache(hContact, uin);
        *Added = 1;

        return hContact;
    }

    // not in list, check that uin do not belong to us
    if (getContactUin(NULL) == uin)
        return NULL;

    return INVALID_HANDLE_VALUE;
}



HANDLE HContactFromUID(DWORD dwUIN, char* pszUID, int *Added)
{
    HANDLE hContact;
    DWORD dwUin;
    uid_str szUid;

    if (dwUIN)
        return HContactFromUIN(dwUIN, Added);

    if (Added) *Added = 0;

    if (!m_bAimEnabled) return INVALID_HANDLE_VALUE;

    hContact = FindFirstContact();
    while (hContact != NULL)
    {
        if (!getContactUid(hContact, &dwUin, &szUid))
        {
            if (!dwUin && !stricmp(szUid, pszUID))
            {
                if (strcmpnull(szUid, pszUID))
                {
                    // fix case in SN
                    setSettingString(hContact, UNIQUEIDSETTING, pszUID);
                }
                return hContact;
            }
        }
        hContact = FindNextContact(hContact);
    }

    //not present: add
    if (Added)
    {
        hContact = (HANDLE)CallService(MS_DB_CONTACT_ADD, 0, 0);
        CallService(MS_PROTO_ADDTOCONTACT, (WPARAM)hContact, (LPARAM)ICQ_PROTOCOL_NAME);

        setSettingString(hContact, UNIQUEIDSETTING, pszUID);

        if (!bIsSyncingCL)
        {
            DBWriteContactSettingByte(hContact, "CList", "NotOnList", 1);
            setContactHidden(hContact, 1);

            setSettingWord(hContact, "Status", ID_STATUS_OFFLINE);

            if (icqOnline)
            {
                icq_sendNewContact(0, pszUID);
            }
            if (getSettingByte(NULL, "KillSpambots", DEFAULT_KILLSPAM_ENABLED))
                icq_sendGetLocationInfo(hContact, 0, pszUID);
        }
        *Added = 1;

        return hContact;
    }

    return INVALID_HANDLE_VALUE;
}



char *NickFromHandle(HANDLE hContact)
{
    if (hContact == INVALID_HANDLE_VALUE)
        return null_strdup(ICQTranslate("<invalid>"));

    return null_strdup((char *)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)hContact, 0));
}

void NickFromHandleStatic(HANDLE hContact, char *szNick, WORD wLen)
{

    if (!wLen || !szNick) return;

    if (hContact == INVALID_HANDLE_VALUE)
        lstrcpynA(szNick, "<invalid>", wLen);
    else
        lstrcpynA(szNick, (char *)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)hContact, 0), wLen);

}


char *NickFromHandleUtf(HANDLE hContact)
{
    if (hContact == INVALID_HANDLE_VALUE)
        return ICQTranslateUtf("<invalid>");

    return mir_utf8encodeW((TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)hContact, gbUnicodeCore_dep ? GCDNF_UNICODE : 0));
}



char *strUID(DWORD dwUIN, char *pszUID)
{
    if (dwUIN)
        ltoa(dwUIN, pszUID, 10);

    return pszUID;
}



void setContactHidden(HANDLE hContact, BYTE bHidden)
{
    DBWriteContactSettingByte(hContact, "CList", "Hidden", bHidden);

    if (!bHidden) // clear zero setting
        DBDeleteContactSetting(hContact, "CList", "Hidden");
}



/* a strlennull() that likes NULL */
int __fastcall strlennull(const char *string)
{
    if (string)
        return strlen(string);

    return 0;
}

/* a wcslen() that likes NULL */
int __fastcall strlennull(const WCHAR *string)
{
  if (string)
    return (int)wcslen(string);

  return 0;
}


/* a strcmp() that likes NULL */
int __fastcall strcmpnull(const char *str1, const char *str2)
{
    if (str1 && str2)
        return strcmp(str1, str2);

    return 1;
}

/* a stricmp() that likes NULL */
int __fastcall stricmpnull(const char *str1, const char *str2)
{
	if (str1 && str2)
		return _stricmp(str1, str2);

  if (!str1 && !str2)
    return 0;

	return 1;
}

char* __fastcall strstrnull(const char *str, const char *substr)
{
	if (str)
		return (char*)strstr(str, substr);

	return NULL;
}



int null_snprintf(char *buffer, size_t count, const char* fmt, ...)
{
    va_list va;
    int len;

    ZeroMemory(buffer, count);
    va_start(va, fmt);
    len = _vsnprintf(buffer, count-1, fmt, va);
    va_end(va);
    return len;
}

int null_snprintf(WCHAR *buffer, size_t count, const WCHAR *fmt, ...)
{
  va_list va;
  int len;

  ZeroMemory(buffer, count * sizeof(WCHAR));
  va_start(va, fmt);
  len = _vsnwprintf(buffer, count, fmt, va);
  va_end(va);
  return len;
}


char* __fastcall null_strdup(const char *string)
{
    if (string)
        return strdup(string);

    return NULL;
}

WCHAR* __fastcall null_strdup(const WCHAR *string)
{
  if (string)
    return wcsdup(string);

  return NULL;
}

char* __fastcall null_strcpy(char *dest, const char *src, size_t maxlen)
{
    if (!dest)
        return NULL;

    if (src && src[0])
    {
        strncpy(dest, src, maxlen);
        dest[maxlen] = '\0';
    }
    else
        dest[0] = '\0';

    return dest;
}


WCHAR* __fastcall null_strcpy(WCHAR *dest, const WCHAR *src, size_t maxlen)
{
  if (!dest)
    return NULL;

  if (src && src[0])
  {
    wcsncpy(dest, src, maxlen);
    dest[maxlen] = '\0';
  }
  else
    dest[0] = '\0';

  return dest;
}


int __fastcall null_strcut(char *string, int maxlen)
{
    // limit the string to max length (null & utf-8 strings ready)
    size_t len = strlennull(string);

    if (len < maxlen)
        return len;

    len = maxlen;

    if (UTF8_IsValid(string)) // handle utf-8 string
    {
        // find the first byte of possible multi-byte character
        while ((string[len] & 0xc0) == 0x80) len--;
    }
    // simply cut the string
    string[len] = '\0';

    return len;
}



void parseServerAddress(char* szServer, WORD* wPort)
{
    int i = 0;

    while (szServer[i] && szServer[i] != ':') i++;
    if (szServer[i] == ':')
    {
        // port included
        *wPort = atoi(&szServer[i + 1]);
    } // otherwise do not change port

    szServer[i] = '\0';
}



char *DemangleXml(const char *string, int len)
{
    char *szWork = (char*)icq_alloc_zero(len+1), *szChar = szWork;
    int i;

    for (i=0; i<len; i++)
    {
        if (!strnicmp(string+i, "&gt;", 4))
        {
            *szChar = '>';
            szChar++;
            i += 3;
        }
        else if (!strnicmp(string+i, "&lt;", 4))
        {
            *szChar = '<';
            szChar++;
            i += 3;
        }
        else if (!strnicmp(string+i, "&amp;", 5))
        {
            *szChar = '&';
            szChar++;
            i += 4;
        }
        else if (!strnicmp(string+i, "&quot;", 6))
        {
            *szChar = '"';
            szChar++;
            i += 5;
        }
        else
        {
            *szChar = string[i];
            szChar++;
        }
    }
    *szChar = '\0';

    return szWork;
}



char *MangleXml(const char *string, int len)
{
    int i, l = 1;
    char *szWork, *szChar;

    for (i = 0; i<len; i++)
    {
        if (string[i]=='<' || string[i]=='>') l += 4;
        else if (string[i]=='&') l += 5;
        else l++;
    }
    szChar = szWork = (char*)icq_alloc_zero(l + 1);
    for (i = 0; i<len; i++)
    {
        if (string[i]=='<')
        {
            *(DWORD*)szChar = ';tl&';
            szChar += 4;
        }
        else if (string[i]=='>')
        {
            *(DWORD*)szChar = ';tg&';
            szChar += 4;
        }
        else if (string[i]=='&')
        {
            *(DWORD*)szChar = 'pma&';
            szChar += 4;
            *szChar = ';';
            szChar++;
        }
        else
        {
            *szChar = string[i];
            szChar++;
        }
    }
    *szChar = '\0';

    return szWork;
}



char *EliminateHtml(const char *string, int len)
{
    char *tmp = (char*)icq_alloc_zero(len + 1);
    int i,j;
    BOOL tag = FALSE;
    char *res;

    for (i=0,j=0; i<len; i++)
    {
        if (!tag && string[i] == '<')
        {
            if ((i + 4 <= len) && (!strnicmp(string + i, "<br>", 4) || !strnicmp(string + i, "<br/>", 5)))
            {
                // insert newline
                tmp[j] = '\r';
                j++;
                tmp[j] = '\n';
                j++;
            }
            tag = TRUE;
        }
        else if (tag && string[i] == '>')
        {
            tag = FALSE;
        }
        else if (!tag)
        {
            tmp[j] = string[i];
            j++;
        }
        tmp[j] = '\0';
    }
    mir_free((char*)string);
    res = DemangleXml(tmp, strlennull(tmp));
    mir_free(tmp);

    return res;
}

void makeUserOffline(HANDLE hContact)
{

    WORD w;
    DWORD dw;

    if (DBGetContactSettingWord(hContact, ICQ_PROTOCOL_NAME, "Status", 0) != ID_STATUS_OFFLINE)
        DBWriteContactSettingWord(hContact, ICQ_PROTOCOL_NAME, "Status", ID_STATUS_OFFLINE);

    if (dw=DBGetContactSettingDword(hContact, ICQ_PROTOCOL_NAME, "LogonTS", 0))
    {
        DBWriteContactSettingDword(hContact, ICQ_PROTOCOL_NAME, "LogonTS", 0);
        DBWriteContactSettingDword(hContact, ICQ_PROTOCOL_NAME, "OldLogonTS", dw);
    }

    if (dw=DBGetContactSettingDword(hContact, ICQ_PROTOCOL_NAME, "IdleTS", 0))
    {
        DBWriteContactSettingDword(hContact, ICQ_PROTOCOL_NAME, "IdleTS", 0);
        DBWriteContactSettingDword(hContact, ICQ_PROTOCOL_NAME, "OldIdleTS", dw);
    }

    if (dw=DBGetContactSettingDword(hContact, ICQ_PROTOCOL_NAME, "RealIP", 0))
    {
        DBWriteContactSettingDword(hContact, ICQ_PROTOCOL_NAME, "RealIP", 0);
        DBWriteContactSettingDword(hContact, ICQ_PROTOCOL_NAME, "OldRealIP", dw);
    }

    if (dw=DBGetContactSettingDword(hContact, ICQ_PROTOCOL_NAME, "IP", 0))
    {
        DBWriteContactSettingDword(hContact, ICQ_PROTOCOL_NAME, "IP", 0);
        DBWriteContactSettingDword(hContact, ICQ_PROTOCOL_NAME, "OldIP", dw);
    }

    if (w=DBGetContactSettingWord(hContact, ICQ_PROTOCOL_NAME, "UserPort", 0))
    {
        DBWriteContactSettingWord(hContact, ICQ_PROTOCOL_NAME, "UserPort", 0);
        DBWriteContactSettingWord(hContact, ICQ_PROTOCOL_NAME, "OldUserPort", w);
    }

    if (w=DBGetContactSettingWord(hContact, ICQ_PROTOCOL_NAME, "Version", 0))
    {
        DBWriteContactSettingWord(hContact, ICQ_PROTOCOL_NAME, "Version", 0);
        DBWriteContactSettingWord(hContact, ICQ_PROTOCOL_NAME, "OldVersion", w);
    }


    // if (DBGetContactSettingDword(hContact, gpszICQProtoName, "TickTS", 0))
    //	DBWriteContactSettingDword(hContact, gpszICQProtoName, "TickTS", 0);

    //DBWriteContactSettingDword(hContact, gpszICQProtoName, "DirectCookie", 0);
}

char* ApplyEncoding(const char *string, const char* pszEncoding)
{
    // decode encoding to Utf-8
    if (string && pszEncoding)
    {
        // we do only encodings known to icq5.1 // TODO: check if this is enough
        if (!strnicmp(pszEncoding, "utf-8", 5))
        {
            // it is utf-8 encoded
            return null_strdup(string);
        }
        else if (!strnicmp(pszEncoding, "unicode-2-0", 11))
        {
            // it is UCS-2 encoded
            int wLen = wcslen((WCHAR*)string) + 1;
            WCHAR *szStr = (WCHAR*)icq_alloc_zero(wLen*2);
            char *tmp = (char*)string;

            unpackWideString((BYTE**)&tmp, szStr, (WORD)(wLen*2));

            return mir_utf8encodeW(szStr);
        }
        else if (!strnicmp(pszEncoding, "iso-8859-1", 10))
        {
            // we use "Latin I" instead - it does the job
            return mir_utf8encodecp(string, 1252);
        }
    }
    if (string)
    {
        // consider it CP_ACP
        return mir_utf8encode(string);
    }

    return NULL;
}



void ResetSettingsOnListReload()
{
    HANDLE hContact;

    // Reset a bunch of session specific settings
    setSettingWord(NULL, "SrvVisibilityID", 0);
    setSettingWord(NULL, "SrvAvatarID", 0);
    setSettingWord(NULL, "SrvPhotoID", 0);
    setSettingWord(NULL, "SrvRecordCount", 0);

    hContact = FindFirstContact();

    while (hContact)
    {
        // All these values will be restored during the serv-list receive
        setSettingWord(hContact, "ServerId", 0);
        setSettingWord(hContact, "SrvGroupId", 0);
        setSettingWord(hContact, "SrvPermitId", 0);
        setSettingWord(hContact, "SrvDenyId", 0);
        setSettingByte(hContact, "Auth", 0);

        hContact = FindNextContact(hContact);
    }

    FlushSrvGroupsCache();
}



void ResetSettingsOnConnect()
{
    HANDLE hContact;

    // Reset a bunch of session specific settings
    setSettingByte(NULL, "SrvVisibility", 0);
    setSettingDword(NULL, "IdleTS", 0);

    hContact = FindFirstContact();

    while (hContact)
    {
        setSettingDword(hContact, "LogonTS", 0);
        setSettingDword(hContact, "IdleTS", 0);
        setSettingDword(hContact, "TickTS", 0);
        setSettingByte(hContact, "TemporaryVisible", 0);

        // All these values will be restored during the login
        if (getContactStatus(hContact) != ID_STATUS_OFFLINE)
            setSettingWord(hContact, "Status", ID_STATUS_OFFLINE);

        hContact = FindNextContact(hContact);
    }
}



void ResetSettingsOnLoad()
{
    HANDLE hContact;

    setSettingDword(NULL, "IdleTS", 0);
    setSettingDword(NULL, "LogonTS", 0);

    hContact = FindFirstContact();

    while (hContact)
    {
        setSettingDword(hContact, "LogonTS", 0);
        setSettingDword(hContact, "IdleTS", 0);
        setSettingDword(hContact, "TickTS", 0);
        if (getContactStatus(hContact) != ID_STATUS_OFFLINE)
        {
            setSettingWord(hContact, "Status", ID_STATUS_OFFLINE);

            deleteSetting(hContact, DBSETTING_XSTATUSID);
            deleteSetting(hContact, DBSETTING_XSTATUSNAME);
            deleteSetting(hContact, DBSETTING_XSTATUSMSG);
        }
        setSettingByte(hContact, "DCStatus", 0);

        hContact = FindNextContact(hContact);
    }
}



int RandRange(int nLow, int nHigh)
{
    return nLow + (int)((nHigh-nLow+1)*rand()/(RAND_MAX+1.0));
}


BOOL IsStringUIN(const char *pszString)
{
    int i;
    int nLen = strlennull(pszString);


    if (nLen > 0 && pszString[0] != '0')
    {
        for (i=0; i<nLen; i++)
        {
            if ((pszString[i] < '0') || (pszString[i] > '9'))
                return FALSE;
        }

        return TRUE;
    }

    return FALSE;
}



static unsigned __stdcall icq_ProtocolAckThread(void* pParam)
{
    icq_ack_args* pArguments = (icq_ack_args*)pParam;

    BroadcastAck(pArguments->hContact, pArguments->nAckType, pArguments->nAckResult, pArguments->hSequence, pArguments->pszMessage);

    if (pArguments->nAckResult == ACKRESULT_SUCCESS)
        NetLog_Server("Sent fake message ack");
    else if (pArguments->nAckResult == ACKRESULT_FAILED)
        NetLog_Server("Message delivery failed");

    mir_free((char*)pArguments->pszMessage);
    mir_free(pArguments);

    return 0;
}



void SendProtoAck(HANDLE hContact, DWORD dwCookie, int nAckResult, int nAckType, char* pszMessage)
{
    icq_ack_args* pArgs;


    pArgs = (icq_ack_args*)icq_alloc_zero(sizeof(icq_ack_args)); // This will be freed in the new thread

    pArgs->hContact = hContact;
    pArgs->hSequence = (HANDLE)dwCookie;
    pArgs->nAckResult = nAckResult;
    pArgs->nAckType = nAckType;
    pArgs->pszMessage = (LPARAM)null_strdup(pszMessage);

    ICQCreateThread(icq_ProtocolAckThread, pArgs);
}



void SetCurrentStatus(int nStatus)
{
    int nOldStatus = gnCurrentStatus;

    gnCurrentStatus = nStatus;
    BroadcastAck(NULL, ACKTYPE_STATUS, ACKRESULT_SUCCESS, (HANDLE)nOldStatus, nStatus);
}

int IsMetaInfoChanged(HANDLE hContact)
{
    DBVARIANT infoToken = {DBVT_DELETED};
    int res = 0;
    if (!getSetting(hContact, DBSETTING_METAINFO_TOKEN, &infoToken))
    {
        // contact does have info from directory, check if it is not outdated
        double dInfoTime = 0;
        double dInfoSaved = 0;
        if ((dInfoTime = getSettingDouble(hContact, DBSETTING_METAINFO_TIME, 0)) > 0)
        {
            if ((dInfoSaved = getSettingDouble(hContact, DBSETTING_METAINFO_SAVED, 0)) > 0)
            {
                if (dInfoSaved < dInfoTime)
                    res = 2; // directory info outdated
            }
            else
                res = 1; // directory info not saved at all
        }

        ICQFreeVariant(&infoToken);
    }
    else
    {
        // it cannot be detected if user info was not changed, so use a generic threshold
        DBVARIANT infoSaved = {DBVT_DELETED};
        DWORD dwInfoTime = 0;

        if (!getSetting(hContact, DBSETTING_METAINFO_SAVED, &infoSaved))
        {
            if (infoSaved.type == DBVT_BLOB && infoSaved.cpbVal == 8)
            {
                double dwTime = *(double*)infoSaved.pbVal;

                dwInfoTime = (dwTime - 25567) * 86400;
            }
            else if (infoSaved.type == DBVT_DWORD)
                dwInfoTime = infoSaved.dVal;

            ICQFreeVariant(&infoSaved);

            if ((time(NULL) - dwInfoTime) > 14*3600*24)
            {
                res = 3; // threshold exceeded
            }
        }
        else
            res = 4; // no timestamp found
    }

    return res;
}



BOOL writeDbInfoSettingString(HANDLE hContact, const char* szSetting, char** buf, WORD* pwLength)
{
    WORD wLen;
	
    if (*pwLength < 2)
        return FALSE;

    unpackLEWord((BYTE**)buf, &wLen);
    *pwLength -= 2;

    if (*pwLength < wLen)
        return FALSE;

    if ((wLen > 0) && (**buf) && ((*buf)[wLen-1]==0)) // Make sure we have a proper string
    {
        WORD wCp = getSettingWord(hContact, "InfoCodePage", getSettingWord(hContact, "InfoCP", CP_ACP));

        if (wCp != CP_ACP)
        {
            char *szUtf = mir_utf8encodecp(*buf, wCp);

            if (szUtf)
            {
                setSettingStringUtf(hContact, szSetting, szUtf);
                mir_free(szUtf);
            }
            else
                setSettingString(hContact, szSetting, *buf);
        }
        else
            setSettingString(hContact, szSetting, *buf);
    }
    else
        deleteSetting(hContact, szSetting);

    *buf += wLen;
    *pwLength -= wLen;

    return TRUE;
}



BOOL writeDbInfoSettingWord(HANDLE hContact, const char *szSetting, char **buf, WORD* pwLength)
{
    WORD wVal;


    if (*pwLength < 2)
        return FALSE;

    unpackLEWord((BYTE**)buf, &wVal);
    *pwLength -= 2;

    if (wVal != 0)
        setSettingWord(hContact, szSetting, wVal);
    else
        deleteSetting(hContact, szSetting);

    return TRUE;
}



BOOL writeDbInfoSettingWordWithTable(HANDLE hContact, const char *szSetting, struct fieldnames_t *table, char **buf, WORD* pwLength)
{
    WORD wVal;
    char sbuf[MAX_PATH];
    char *text;

    if (*pwLength < 2)
        return FALSE;

    unpackLEWord((BYTE**)buf, &wVal);
    *pwLength -= 2;

    text = LookupFieldNameUtf(table, wVal, sbuf, MAX_PATH);
    if (text)
        setSettingStringUtf(hContact, szSetting, text);
    else
        deleteSetting(hContact, szSetting);

    return TRUE;
}



BOOL writeDbInfoSettingByte(HANDLE hContact, const char *pszSetting, char **buf, WORD* pwLength)
{
    BYTE byVal;

    if (*pwLength < 1)
        return FALSE;

    unpackByte((BYTE**)buf, &byVal);
    *pwLength -= 1;

    if (byVal != 0)
        setSettingByte(hContact, pszSetting, byVal);
    else
        deleteSetting(hContact, pszSetting);

    return TRUE;
}



BOOL writeDbInfoSettingByteWithTable(HANDLE hContact, const char *szSetting, struct fieldnames_t *table, char **buf, WORD* pwLength)
{
    BYTE byVal;
    char sbuf[MAX_PATH];
    char *text;

    if (*pwLength < 1)
        return FALSE;

    unpackByte((BYTE**)buf, &byVal);
    *pwLength -= 1;

    text = LookupFieldNameUtf(table, byVal, sbuf, MAX_PATH);
    if (text)
        setSettingStringUtf(hContact, szSetting, text);
    else
        deleteSetting(hContact, szSetting);

    return TRUE;
}

void writeDbInfoSettingTLVStringUtf(HANDLE hContact, const char *szSetting, oscar_tlv_chain *chain, WORD wTlv)
{
    oscar_tlv *pTLV = getTLV(chain, wTlv, 1);

    if (pTLV && pTLV->wLen > 0)
    {
        char *str = (char*)icq_alloc_zero(pTLV->wLen + 1);

        memcpy(str, pTLV->pData, pTLV->wLen);
        str[pTLV->wLen] = '\0';
        setSettingStringUtf(hContact, szSetting, str);
    }
    else
        deleteSetting(hContact, szSetting);
}

void writeDbInfoSettingTLVString(HANDLE hContact, const char *szSetting, oscar_tlv_chain *chain, WORD wTlv)
{
    oscar_tlv *pTLV = getTLV(chain, wTlv, 1);

    if (pTLV && pTLV->wLen > 0)
    {
        char *str = (char*)icq_alloc_zero(pTLV->wLen + 1);

        memcpy(str, pTLV->pData, pTLV->wLen);
        str[pTLV->wLen] = '\0';
        setSettingString(hContact, szSetting, str);
    }
    else
        deleteSetting(hContact, szSetting);
}

void writeDbInfoSettingTLVWord(HANDLE hContact, const char *szSetting, oscar_tlv_chain *chain, WORD wTlv)
{
    int num = getNumberFromChain(chain, wTlv, 1);

    if (num > 0)
        setSettingWord(hContact, szSetting, num);
    else
        deleteSetting(hContact, szSetting);
}

void writeDbInfoSettingTLVByte(HANDLE hContact, const char *szSetting, oscar_tlv_chain *chain, WORD wTlv)
{
    int num = getNumberFromChain(chain, wTlv, 1);


    if (num > 0)
        setSettingByte(hContact, szSetting, num);
    else
        deleteSetting(hContact, szSetting);
}

void writeDbInfoSettingTLVDate(HANDLE hContact, const char* szSettingYear, const char* szSettingMonth, const char* szSettingDay, oscar_tlv_chain* chain, WORD wTlv)
{
    double time = getDoubleFromChain(chain, wTlv, 1);

    if (time > 0)
    {
        // date is stored as double with unit equal to a day, incrementing since 1/1/1900 0:00 GMT
        SYSTEMTIME sTime = {0};
        if (VariantTimeToSystemTime(time + 2, &sTime))
        {
            setSettingWord(hContact, szSettingYear, sTime.wYear);
            setSettingByte(hContact, szSettingMonth, (BYTE)sTime.wMonth);
            setSettingByte(hContact, szSettingDay, (BYTE)sTime.wDay);
        }
        else
        {
            deleteSetting(hContact, szSettingYear);
            deleteSetting(hContact, szSettingMonth);
            deleteSetting(hContact, szSettingDay);
        }
    }
    else
    {
        deleteSetting(hContact, szSettingYear);
        deleteSetting(hContact, szSettingMonth);
        deleteSetting(hContact, szSettingDay);
    }
}
void writeDbInfoSettingTLVDouble(HANDLE hContact, const char *szSetting, oscar_tlv_chain *chain, WORD wTlv)
{
    double num = getDoubleFromChain(chain, wTlv, 1);
	
    if (num > 0)
        setSettingDouble(hContact, szSetting, num);
    else
        deleteSetting(hContact, szSetting);
}

// Returns the current GMT offset in seconds
int GetGMTOffset(void)
{
    TIME_ZONE_INFORMATION tzinfo;
    DWORD dwResult;
    int nOffset = 0;


    dwResult = GetTimeZoneInformation(&tzinfo);

    switch(dwResult)
    {

    case TIME_ZONE_ID_STANDARD:
        nOffset = -(tzinfo.Bias + tzinfo.StandardBias) * 60;
        break;

    case TIME_ZONE_ID_DAYLIGHT:
        nOffset = -(tzinfo.Bias + tzinfo.DaylightBias) * 60;
        break;

    case TIME_ZONE_ID_UNKNOWN:
    case TIME_ZONE_ID_INVALID:
    default:
        nOffset = 0;
        break;

    }

    return nOffset;
}



BOOL validateStatusMessageRequest(HANDLE hContact, WORD byMessageType)
{
    // Don't send statusmessage to unknown contacts
    //Don't send any status message reply, if on
    if(bNoStatusReply)
        return FALSE;
    if (hContact == INVALID_HANDLE_VALUE)
        return FALSE;
    // Privacy control
    if (getSettingByte(NULL, "StatusMsgReplyCList", 0))
    {

        // Don't send statusmessage to temporary contacts or hidden contacts
        if (DBGetContactSettingByte(hContact, "CList", "NotOnList", 0) ||
                DBGetContactSettingByte(hContact, "CList", "Hidden", 0))
            return FALSE;

        // Don't send statusmessage to invisible contacts
        if (getSettingByte(NULL, "StatusMsgReplyVisible", 0))
        {
            WORD wStatus = getContactStatus(hContact);
            if (wStatus == ID_STATUS_OFFLINE)
                return FALSE;
        }
    }

    // Dont send messages to people you are hiding from
    if (getSettingWord(hContact, "ApparentMode", 0) == ID_STATUS_OFFLINE)
    {
        CHECKCONTACT chk = {0};
        chk.hContact = hContact;
        chk.dbeventflag=DBEF_READ;
        chk.popup=chk.logtofile=chk.historyevent=TRUE;
        chk.popuptype=POPTYPE_VIS;
        chk.icqeventtype=ICQEVENTTYPE_CHECK_STATUS;
        chk.msg = "contact which you are invisible for requests your status message";
        CheckContact(chk);
        return FALSE;
    }

    // Dont respond to request for other statuses than your current one
    if ((byMessageType == MTYPE_AUTOONLINE && gnCurrentStatus != ID_STATUS_ONLINE) ||
            (byMessageType == MTYPE_AUTOAWAY && gnCurrentStatus != ID_STATUS_AWAY) ||
            (byMessageType == MTYPE_AUTOBUSY && gnCurrentStatus != ID_STATUS_OCCUPIED) ||
            (byMessageType == MTYPE_AUTONA   && gnCurrentStatus != ID_STATUS_NA) ||
            (byMessageType == MTYPE_AUTODND  && gnCurrentStatus != ID_STATUS_DND) ||
            (byMessageType == MTYPE_AUTOFFC  && gnCurrentStatus != ID_STATUS_FREECHAT))
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dbeventflag=DBEF_READ;
        chk.popup=chk.logtofile=chk.historyevent=TRUE;
        chk.icqeventtype=ICQEVENTTYPE_CHECK_STATUS;
        chk.popuptype=POPTYPE_INFO_REQUEST;
        chk.msg="requested wrong status type - trying to find you";
        CheckContact(chk);
        return FALSE;
    }

    if (gnCurrentStatus==ID_STATUS_INVISIBLE)
    {
        if(invis_for(getContactUin(hContact), hContact))
        {
            if (!getSettingByte(hContact, "TemporaryVisible", 0))
            {
                // Allow request to temporary visible contacts
                return FALSE;
            }
        }
    }
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.PSD = -1;
        CheckContact(chk);
    }
    // All OK!
    return TRUE;
}

void __fastcall SAFE_DELETE(void_struct **p)
{
	if (*p)
	{
		delete *p;
		*p = NULL;
	}
}


void __fastcall SAFE_DELETE(lockable_struct **p)
{
	if (*p)
	{
		(*p)->_Release();
		*p = NULL;
	}
}

void __fastcall SAFE_FREE(void** p)
{
	if (*p)
	{
		free(*p);
		*p = NULL;
	}
}


void* __fastcall SAFE_MALLOC(size_t size)
{
	void* p = NULL;

	if (size)
	{
		p = malloc(size);

		if (p)
			ZeroMemory(p, size);
	}
	return p;
}


void* __fastcall SAFE_REALLOC(void* p, size_t size)
{
	if (p)
	{
		return realloc(p, size);
	}
	else
		return SAFE_MALLOC(size);
}


DWORD ICQWaitForSingleObject(HANDLE hObject, DWORD dwMilliseconds, int bWaitAlways)
{
	DWORD dwResult;

	do { // will get WAIT_IO_COMPLETION for QueueUserAPC(), ignore it unless terminating
		dwResult = WaitForSingleObjectEx(hObject, dwMilliseconds, TRUE);
	} while (dwResult == WAIT_IO_COMPLETION && (bWaitAlways || !Miranda_Terminated()));

	return dwResult;
}


HANDLE NetLib_OpenConnection(HANDLE hUser, const char* szIdent, NETLIBOPENCONNECTION* nloc)
{
    HANDLE hConnection;

    Netlib_Logf(hUser, "%sConnecting to %s:%u", szIdent?szIdent:"", nloc->szHost, nloc->wPort);

    nloc->cbSize = sizeof(NETLIBOPENCONNECTION);
    nloc->flags |= NLOCF_V2;

    hConnection = (HANDLE)CallService(MS_NETLIB_OPENCONNECTION, (WPARAM)hUser, (LPARAM)nloc);
    if (!hConnection && (GetLastError() == 87))
    {
        // this ensures, an old Miranda will be able to connect also
        nloc->cbSize = NETLIBOPENCONNECTION_V1_SIZE;
        hConnection = (HANDLE)CallService(MS_NETLIB_OPENCONNECTION, (WPARAM)hUser, (LPARAM)nloc);
    }
    return hConnection;
}



HANDLE NetLib_BindPort(NETLIBNEWCONNECTIONPROC_V2 pFunc, void* lParam, WORD* pwPort, DWORD* pdwIntIP)
{
    NETLIBBIND nlb = {0};
    HANDLE hBoundPort;

    nlb.cbSize = sizeof(NETLIBBIND);
    nlb.pfnNewConnectionV2 = pFunc;
    nlb.pExtra = lParam;
    SetLastError(ERROR_INVALID_PARAMETER); // this must be here - NetLib does not set any error :((
    hBoundPort = (HANDLE)CallService(MS_NETLIB_BINDPORT, (WPARAM)ghDirectNetlibUser, (LPARAM)&nlb);
    if (!hBoundPort && (GetLastError() == ERROR_INVALID_PARAMETER))
    {
        // this ensures older Miranda also can bind a port for a dc - pre 0.6
        nlb.cbSize = NETLIBBIND_SIZEOF_V2;
        hBoundPort = (HANDLE)CallService(MS_NETLIB_BINDPORT, (WPARAM)ghDirectNetlibUser, (LPARAM)&nlb);
    }
    if (pwPort) *pwPort = nlb.wPort;
    if (pdwIntIP) *pdwIntIP = nlb.dwInternalIP;

    return hBoundPort;
}



void NetLib_CloseConnection(HANDLE *hConnection, int bServerConn)
{
    if (*hConnection)
    {
//    int sck = CallService(MS_NETLIB_GETSOCKET, (WPARAM)*hConnection, (LPARAM)0);

//    if (sck!=INVALID_SOCKET) shutdown(sck, 2); // close gracefully
        CallService(MS_NETLIB_SHUTDOWN, (WPARAM)*hConnection, 0);

        NetLib_SafeCloseHandle(hConnection);

        if (bServerConn)
            FreeGatewayIndex(*hConnection);
    }
}



void NetLib_SafeCloseHandle(HANDLE *hConnection)
{
    if (*hConnection)
    {
        Netlib_CloseHandle(*hConnection);
        *hConnection = NULL;
    }
}



int NetLog_Server(const char *fmt,...)
{
    va_list va;
    char szText[1024];

    va_start(va,fmt);
    mir_vsnprintf(szText,sizeof(szText),fmt,va);
    va_end(va);
    return CallService(MS_NETLIB_LOG,(WPARAM)ghServerNetlibUser,(LPARAM)szText);
}



int NetLog_Direct(const char *fmt,...)
{
    va_list va;
    char szText[1024];

    va_start(va,fmt);
    mir_vsnprintf(szText,sizeof(szText),fmt,va);
    va_end(va);
    return CallService(MS_NETLIB_LOG,(WPARAM)ghDirectNetlibUser,(LPARAM)szText);
}



int NetLog_Uni(BOOL bDC, const char *fmt,...)
{
    va_list va;
    char szText[1024];
    HANDLE hNetlib;

    va_start(va,fmt);
    mir_vsnprintf(szText,sizeof(szText),fmt,va);
    va_end(va);

    if (bDC)
        hNetlib = ghDirectNetlibUser;
    else
        hNetlib = ghServerNetlibUser;

    return CallService(MS_NETLIB_LOG,(WPARAM)hNetlib,(LPARAM)szText);
}



int BroadcastAck(HANDLE hContact,int type,int result,HANDLE hProcess,LPARAM lParam)
{
    ACKDATA ack= {0};

    ack.cbSize=sizeof(ACKDATA);
    ack.szModule=ICQ_PROTOCOL_NAME;
    ack.hContact=hContact;
    ack.type=type;
    ack.result=result;
    ack.hProcess=hProcess;
    ack.lParam=lParam;
    return CallService(MS_PROTO_BROADCASTACK,0,(LPARAM)&ack);
}



int __fastcall ICQTranslateDialog(HWND hwndDlg)
{
    LANGPACKTRANSLATEDIALOG lptd;

    lptd.cbSize=sizeof(lptd);
    lptd.flags=0;
    lptd.hwndDlg=hwndDlg;
    lptd.ignoreControls=NULL;
    return CallService(MS_LANGPACK_TRANSLATEDIALOG,0,(LPARAM)&lptd);
}



char* __fastcall ICQTranslate(const char* src)
{
    return (char*)CallService(MS_LANGPACK_TRANSLATESTRING,0,(LPARAM)src);
}



char* __fastcall ICQTranslateUtf(const char* src)
{
    // this takes UTF-8 strings only!!!
    char* szRes = NULL;

    if (!strlennull(src))
    {
        // for the case of empty strings
        return null_strdup(src);
    }

    {
        // we can use unicode translate (0.5+)
        WCHAR* usrc = mir_utf8decodeW(src);

        szRes = mir_utf8encodeW(TranslateW(usrc));

        mir_free(usrc);
    }
    return szRes;
}



char* __fastcall ICQTranslateUtfStatic(const char* src, char* buf, size_t bufsize)
{
    // this takes UTF-8 strings only!!!
    if (src && src[0])
    {
        // we can use unicode translate (0.5+)
        WCHAR* usrc = mir_utf8decodeW(src);
        char *tmp = mir_utf8encodeW(TranslateW(usrc));
        strcpy(buf, tmp);
        mir_free(tmp);
        mir_free(usrc);
    }
    else
        buf[0] = '\0';

    return buf;
}



HANDLE ICQCreateThreadEx(pThreadFuncEx AFunc, void* arg, DWORD* pThreadID)
{
    FORK_THREADEX_PARAMS params;
    unsigned dwThreadId;	// FIXME: It's highly unreliable code due to different size of "unsigned" on different platforms
    HANDLE hThread;

    params.pFunc      = AFunc;
    params.arg        = arg;
    params.iStackSize = 0;
    params.threadID   = &dwThreadId;
    hThread = (HANDLE)CallService(MS_SYSTEM_FORK_THREAD_EX, 0, (LPARAM)&params);
    if (pThreadID)
        *pThreadID = dwThreadId;

    return hThread;
}



void ICQCreateThread(pThreadFuncEx AFunc, void* arg)
{
    HANDLE hThread = ICQCreateThreadEx(AFunc, arg, NULL);

    CloseHandle(hThread);
}



char* GetUserPassword(BOOL bAlways)
{
    if (gpszPassword[0] != '\0' && (gbRememberPwd || bAlways))
        return gpszPassword;

    if (!getSettingStringStatic(NULL, "Password", gpszPassword, sizeof(gpszPassword)))
    {
        CallService(MS_DB_CRYPT_DECODESTRING, strlennull(gpszPassword) + 1, (LPARAM)gpszPassword);

        if (!strlennull(gpszPassword)) return NULL;

        gbRememberPwd = TRUE;

        return gpszPassword;
    }

    return NULL;
}



WORD GetMyStatusFlags()
{
    WORD wFlags = 0;

    // Webaware setting bit flag
    if (getSettingByte(NULL, "WebAware", 0))
        wFlags = STATUS_WEBAWARE;

    // DC setting bit flag
    switch (getSettingByte(NULL, "DCType", 0))
    {
    case 0:
        break;

    case 1:
        wFlags = wFlags | STATUS_DCCONT;
        break;

    case 2:
        wFlags = wFlags | STATUS_DCAUTH;
        break;

    default:
        wFlags = wFlags | STATUS_DCDISABLED;
        break;
    }
    return wFlags;
}



int IsValidRelativePath(const char *filename)
{
    if (strstr(filename, "..\\") || strstr(filename, "../") ||
            strstr(filename, ":\\") || strstr(filename, ":/") ||
            filename[0] == '\\' || filename[0] == '/')
        return 0; // Contains malicious chars, Failure

    return 1; // Success
}



const char* ExtractFileName(const char *fullname)
{
    const char* szFileName;

    if (((szFileName = strrchr(fullname, '\\')) == NULL) && ((szFileName = strrchr(fullname, '/')) == NULL))
    {
        // already is only filename
        return (char*)fullname;
    }
    szFileName++; // skip backslash

    return szFileName;
}


char* FileNameToUtf_old(const char *filename)
{
    if (gbUnicodeAPI_dep)
    {
        // reasonable only on NT systems
        HINSTANCE hKernel;
        DWORD (CALLBACK *RealGetLongPathName)(LPCWSTR, LPWSTR, DWORD);

        hKernel = GetModuleHandle(_T("KERNEL32"));
        *(FARPROC *)&RealGetLongPathName = GetProcAddress(hKernel, "GetLongPathNameW");

        if (RealGetLongPathName)
        {
            // the function is available (it is not on old NT systems)
            WCHAR *unicode, *usFileName = NULL;
            int wchars;

            wchars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, filename,
                                         strlennull(filename), NULL, 0);

            unicode = (WCHAR*)icq_alloc_zero((wchars + 1) * sizeof(WCHAR));
            unicode[wchars] = 0;

            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, filename,
                                strlennull(filename), unicode, wchars);

            wchars = RealGetLongPathName(unicode, usFileName, 0);
            usFileName = (WCHAR*)icq_alloc_zero((wchars + 1) * sizeof(WCHAR));
            RealGetLongPathName(unicode, usFileName, wchars);

            return mir_utf8encodeW(usFileName);
        }
        else
            return mir_utf8encode(filename);
    }
    else
        return mir_utf8encode(filename);
}


char* FileNameToUtf(const TCHAR *filename)
{
#if defined( _UNICODE )
	// reasonable only on NT systems
	HINSTANCE hKernel = GetModuleHandle(_T("KERNEL32"));
	DWORD (CALLBACK *RealGetLongPathName)(LPCWSTR, LPWSTR, DWORD);

	*(FARPROC *)&RealGetLongPathName = GetProcAddress(hKernel, "GetLongPathNameW");

	if (RealGetLongPathName)
	{ // the function is available (it is not on old NT systems)
		WCHAR *usFileName = NULL;
		int wchars = RealGetLongPathName(filename, usFileName, 0);
		usFileName = (WCHAR*)_alloca((wchars + 1) * sizeof(WCHAR));
		RealGetLongPathName(filename, usFileName, wchars);

		return make_utf8_string(usFileName);
	}
	return make_utf8_string(filename);
#else
	return ansi_to_utf8(filename);
#endif
}


int FileAccessUtf(const char *path, int mode)
{
  int size = strlennull(path) + 2;
	TCHAR *szPath = (TCHAR*)_alloca(size * sizeof(TCHAR));

  if (utf8_to_tchar_static(path, szPath, size))
	  return _taccess(szPath, mode);

	return -1;
}


int FileStatUtf(const char *path, struct _stati64 *buffer)
{
    int wRes = -1;

    if (gbUnicodeAPI_dep)
    {
        WCHAR* usPath = mir_utf8decodeW(path);

        wRes = _wstati64(usPath, buffer);
        mir_free(usPath);
    }
    else
    {
        int size = strlennull(path)+2;
        char* szAnsiPath = (char*)icq_alloc_zero(size);
        char* tmp = mir_strdup(path);
        mir_utf8decode(tmp, NULL);
        if(tmp)
        {
            strcpy(szAnsiPath, tmp);
            wRes = _stati64(szAnsiPath, buffer);
            mir_free(tmp);
        }
    }
    return wRes;
}



int MakeDirUtf(const char *dir)
{
	int wRes = -1;
	int size = strlennull(dir) + 2;
	TCHAR *szDir = (TCHAR*)_alloca(size * sizeof(TCHAR));

	if (utf8_to_tchar_static(dir, szDir, size))
	{ // _tmkdir can created only one dir at once
		wRes = _tmkdir(szDir);
		// check if dir not already existed - return success if yes
		if (wRes == -1 && errno == 17 /* EEXIST */)
			wRes = 0;
		else if (wRes && errno == 2 /* ENOENT */)
		{ // failed, try one directory less first
			char *szLast = (char*)strrchr(dir, '\\');
			if (!szLast) szLast = (char*)strrchr(dir, '/');
			if (szLast)
			{
				char cOld = *szLast;

				*szLast = '\0';
				if (!MakeDirUtf(dir))
					wRes = _tmkdir(szDir);

				*szLast = cOld;
			}
		}
	}

	return wRes;
}



int OpenFileUtf(const char *filename, int oflag, int pmode)
{
    int hFile = -1;

    if (gbUnicodeAPI_dep)
    {
        WCHAR* usFile = mir_utf8decodeW(filename);

        hFile = _wopen(usFile, oflag, pmode);
        mir_free(usFile);
    }
    else
    {
        int size = strlennull(filename)+2;
        char* szAnsiFile = (char*)icq_alloc_zero(size);
        char* tmp = mir_strdup(filename);
        mir_utf8decode(tmp, NULL);
        strcpy(szAnsiFile, tmp);
        mir_free(tmp);

        if (szAnsiFile && szAnsiFile[0])
            hFile = _open(szAnsiFile, oflag, pmode);
    }
    return hFile;
}



WCHAR *GetWindowTextUcs(HWND hWnd)
{
    WCHAR *utext;

    if (gbUnicodeAPI_dep)
    {
        int nLen = GetWindowTextLengthW(hWnd);

        utext = (WCHAR*)icq_alloc_zero((nLen+2)*sizeof(WCHAR));
        GetWindowTextW(hWnd, utext, nLen + 1);
    }
    else
    {
        char *text;
        int wchars, nLen = GetWindowTextLengthA(hWnd);

        text = (char*)icq_alloc_zero(nLen+2);
        GetWindowTextA(hWnd, text, nLen + 1);

        wchars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, text,
                                     strlennull(text), NULL, 0);

        utext = (WCHAR*)icq_alloc_zero((wchars + 1)*sizeof(WCHAR));

        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, text,
                            strlennull(text), utext, wchars);
    }
    return utext;
}



void SetWindowTextUcs(HWND hWnd, WCHAR *text)
{
    if (gbUnicodeAPI_dep)
    {
        SetWindowTextW(hWnd, text);
    }
    else
    {
        char *tmp = (char*)icq_alloc_zero(wcslen(text) + 1);

        WideCharToMultiByte(CP_ACP, 0, text, -1, tmp, wcslen(text)+1, NULL, NULL);
        SetWindowTextA(hWnd, tmp);
        mir_free(tmp);
    }
}



char* GetWindowTextUtf(HWND hWnd)
{
    TCHAR* szText;

    if (gbUnicodeAPI_dep)
    {
        int nLen = GetWindowTextLengthW(hWnd);

        szText = (TCHAR*)icq_alloc_zero((nLen+2)*sizeof(WCHAR));
        GetWindowTextW(hWnd, (WCHAR*)szText, nLen + 1);
    }
    else
    {
        int nLen = GetWindowTextLengthA(hWnd);

        szText = (TCHAR*)icq_alloc_zero(nLen+2);
        GetWindowTextA(hWnd, (char*)szText, nLen + 1);
    }
    return mir_utf8encodeW(szText);
}



char* GetDlgItemTextUtf(HWND hwndDlg, int iItem)
{
    return GetWindowTextUtf(GetDlgItem(hwndDlg, iItem));
}



void SetWindowTextUtf(HWND hWnd, const char* szText)
{
    if (gbUnicodeAPI_dep)
    {
        WCHAR* usText = mir_utf8decodeW(szText);

        SetWindowTextW(hWnd, usText);
        mir_free(usText);
    }
    else
    {
        int size = strlennull(szText)+2;
        char* szAnsi = (char*)icq_alloc_zero(size);
        char* tmp = mir_strdup(szText);
        mir_utf8decode(tmp, NULL);
        strcpy(szAnsi, tmp);
        mir_free(tmp);

        if (szAnsi && szAnsi[0])
            SetWindowTextA(hWnd, szAnsi);
    }
}



void SetDlgItemTextUtf(HWND hwndDlg, int iItem, const char* szText)
{
    SetWindowTextUtf(GetDlgItem(hwndDlg, iItem), szText);
}



LONG SetWindowLongUtf(HWND hWnd, int nIndex, LONG dwNewLong)
{
    if (gbUnicodeAPI_dep)
        return SetWindowLongW(hWnd, nIndex, dwNewLong);
    else
        return SetWindowLongA(hWnd, nIndex, dwNewLong);
}



LRESULT CallWindowProcUtf(WNDPROC OldProc, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (gbUnicodeAPI_dep)
        return CallWindowProcW(OldProc,hWnd,msg,wParam,lParam);
    else
        return CallWindowProcA(OldProc,hWnd,msg,wParam,lParam);
}



static int ControlAddStringUtf(HWND ctrl, DWORD msg, const char* szString)
{
    char str[MAX_PATH];
    char *szItem = ICQTranslateUtfStatic(szString, str, MAX_PATH);
    int item = -1;

    if (gbUnicodeAPI_dep)
    {
        WCHAR *wItem = mir_utf8decodeW(szItem);

        item = SendMessageW(ctrl, msg, 0, (LPARAM)wItem);
        mir_free(wItem);
    }
    else
    {
        int size = strlennull(szItem) + 2;
        char *aItem = (char*)icq_alloc_zero(size);
        char *tmp = mir_strdup(szItem);
        mir_utf8decode(tmp, NULL);
        strcpy(aItem, tmp);
        mir_free(tmp);

        if (aItem && aItem[0])
            item = SendMessageA(ctrl, msg, 0, (LPARAM)aItem);
    }
    return item;
}



int ComboBoxAddStringUtf(HWND hCombo, const char* szString, DWORD data)
{
    int item = ControlAddStringUtf(hCombo, CB_ADDSTRING, szString);
    SendMessage(hCombo, CB_SETITEMDATA, item, data);

    return item;
}



int ListBoxAddStringUtf(HWND hList, const char* szString)
{
    return ControlAddStringUtf(hList, LB_ADDSTRING, szString);
}



HWND DialogBoxUtf(BOOL bModal, HINSTANCE hInstance, const char* szTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
    // Unicode pump ready dialog box
    if (gbUnicodeAPI_dep)
    {
        if (bModal)
            return (HWND)DialogBoxParamW(hInstance, (LPCWSTR)szTemplate, hWndParent, lpDialogFunc, dwInitParam);
        else
            return CreateDialogParamW(hInstance, (LPCWSTR)szTemplate, hWndParent, lpDialogFunc, dwInitParam);
    }
    else
    {
        if (bModal)
            return (HWND)DialogBoxParamA(hInstance, szTemplate, hWndParent, lpDialogFunc, dwInitParam);
        else
            return CreateDialogParamA(hInstance, szTemplate, hWndParent, lpDialogFunc, dwInitParam);
    }
}



HWND CreateDialogUtf(HINSTANCE hInstance, const char* lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc)
{
    if (gbUnicodeAPI_dep)
        return CreateDialogW(hInstance, (LPCWSTR)lpTemplate, hWndParent, lpDialogFunc);
    else
        return CreateDialogA(hInstance, lpTemplate, hWndParent, lpDialogFunc);
}



int MessageBoxUtf(HWND hWnd, const char* szText, const char* szCaption, UINT uType)
{
    int res;
    char str[1024];
    char cap[MAX_PATH];

    if (gbUnicodeAPI_dep)
    {
        WCHAR *text = mir_utf8decodeW(ICQTranslateUtfStatic(szText, str, 1024));
        WCHAR *caption = mir_utf8decodeW(ICQTranslateUtfStatic(szCaption, cap, MAX_PATH));
        res = MessageBoxW(hWnd, text, caption, uType);
        mir_free(caption);
        mir_free(text);
    }
    else
    {
        int size = strlennull(szText) + 2, size2 = strlennull(szCaption) + 2;
        char *text = (char*)icq_alloc_zero(size);
        char *caption = (char*)icq_alloc_zero(size2);
        char* tmp = mir_strdup(ICQTranslateUtfStatic(szText, str, 1024));
        mir_utf8decode(tmp, NULL);
        strcpy(text, tmp);
        mir_free(tmp);
        tmp = mir_strdup(ICQTranslateUtfStatic(szCaption, cap, MAX_PATH));
        mir_utf8decode(tmp, NULL);
        strcpy(cap, tmp);
        mir_free(tmp);

        res = MessageBoxA(hWnd, text, caption, uType);
    }
    return res;
}


int CacheIDCount(void)
{

    return cacheCount;

}

int IDFromCacheByUin(DWORD dwUin)
{

    int i = -1;

    if (cacheCount == 0)
        return i;

    EnterCriticalSection(&cacheMutex);

    for (i = cacheCount-1; i >= 0; i--)
        if (contacts_cache[i].dwUin == dwUin)
            break;

    LeaveCriticalSection(&cacheMutex);

    return i;

}

int SetCacheFlagsByID(int ID, DWORD flags)
{

    if (ID < 0 || ID >= cacheCount)
        return 0;

    contacts_cache[ID].flags = flags;

    return 1;

}

int GetCacheByID(int ID, icq_contacts_cache *icc)
{

    if (!icc || ID < 0 || ID >= cacheCount)
        return 0;

    EnterCriticalSection(&cacheMutex);

    icc->hContact = contacts_cache[ID].hContact;
    icc->dwUin = contacts_cache[ID].dwUin;
    icc->flags = contacts_cache[ID].flags;

    LeaveCriticalSection(&cacheMutex);

    return icc->dwUin; // to be sure it is not 0

}

/*BOOL invis_for(DWORD dwUin, HANDLE hContact)  //this function checking are you invisible for hContact or dwUin, need optimization...
{
	WORD wApparent = DBGetContactSettingWord(hContact,gpszICQProtoName,"ApparentMode",0);

	if(bVisibility==0x02)
		return TRUE;
	if (dwUin && !hContact)
		hContact=HContactFromUIN(dwUin,0);
	if (!hContact)
	{
		return TRUE;
	}
	if(DBGetContactSettingByte(hContact, "CList", "NotOnList", 0)&&bVisibility!=0x01||bVisibility!=0x04)
		return TRUE;
	if((wApparent==ID_STATUS_OFFLINE||gnCurrentStatus == ID_STATUS_INVISIBLE&&bVisibility!=0x04&&wApparent!=ID_STATUS_ONLINE)&&bVisibility!=0x01)
		return TRUE;
	return FALSE;
} */



BOOL invis_for(DWORD dwUin, HANDLE hContact)  //this function checking are you invisible for hContact or dwUin, need optimization... (modified by Bio)
{
    WORD wApparent;
    if (hContact == NULL && !dwUin)
        return FALSE;

    // Block all users from seeing you
    if ( bVisibility==0x02) //we support AIM which do not use UIN's
    {
        NetLog_Server("we invisible for %u, blocking all unsafe requests", dwUin);
        return TRUE;
    }

    // Allow all users to see you
    if ( bVisibility == 0x01 )
        return FALSE;

    if ( !hContact )
    {
        hContact=HContactFromUIN(dwUin,0);
        if (!hContact)
        {
            NetLog_Server("we invisible for %u, blocking all unsafe requests", dwUin);
            return TRUE;
        }
    }

    if ( DBGetContactSettingByte(hContact, "CList", "NotOnList", 0) )
    {
        NetLog_Server("we invisible for %u, blocking all unsafe requests", dwUin);
        return TRUE;
    }

    // Allow only users in the buddy list to see you
    if ( bVisibility == 0x05 )
    {
        if ( DBGetContactSettingByte(hContact, "CList", "Hidden", 0) )
        {
            NetLog_Server("we invisible for %u, blocking all unsafe requests", dwUin);
            return TRUE;
        }
        else
            return FALSE;
    }

    wApparent = DBGetContactSettingWord(hContact,ICQ_PROTOCOL_NAME,"ApparentMode",0);

    // Allow only users in the permit list to see you
    if ( bVisibility == 0x03 )
    {
        if ( wApparent == ID_STATUS_ONLINE )
            return FALSE;
        else
        {
            NetLog_Server("we invisible for %u, blocking all unsafe requests", dwUin);
            return TRUE;
        }
    }

    // Block only users in the invisible list from seeing you
    if ( bVisibility == 0x04 )
    {
        if ( wApparent == ID_STATUS_OFFLINE )
        {
            NetLog_Server("we invisible for %u, blocking all unsafe requests", dwUin);
            return TRUE;
        }
        else
            return FALSE;
    }

    // invisible by default
    NetLog_Server("we invisible for %u, blocking all unsafe requests", dwUin);
    return TRUE;
}

void CheckContact(CHECKCONTACT chk)
{
    BYTE Hidden = DBGetContactSettingByte(chk.hContact, "CList", "Hidden", 0);
    BOOL Ignored = FALSE, Cancel = FALSE;
    BYTE InDb = CallService(MS_DB_CONTACT_IS, (WPARAM)chk.hContact, 0);
    BYTE NotOnList = DBGetContactSettingByte(chk.hContact,"CList","NotOnList",0);
    char popmsg[512] = {0}, eventmsghistory[512] = {0}, eventmsgfile[512] = {0};
    if(chk.hContact == NULL)
        return;
    if(!chk.dwUin)
        chk.dwUin = getSettingDword(chk.hContact, "UIN", 0);
    if(DBGetContactSettingDword(chk.hContact, "Ignore", "Mask1", 0) == 0x0000007F)
        Ignored = TRUE;
    if(!InDb||NotOnList)
    {
        if(!bPopUpForNotOnList)
            chk.popup = FALSE;
        if(bNoPSDForHidden)
            Cancel = TRUE;
    }
    if(Hidden && !bPopupsForHidden)
        chk.popup = FALSE;
    if(Ignored && !bPopupsForIgnored)
        chk.popup = FALSE;
    if(!chk.popuptype)                  //
        chk.popup=FALSE;				//
    if(!chk.icqeventtype)				//
    {
        //wrong call check ... (missing event types)
        chk.historyevent=FALSE;			//
        chk.logtofile=FALSE;			//
    }									//
    if(chk.dwUin)
    {
        if (bTmpContacts)
        {
            int added;
            static BOOL TmpContact;
            chk.hContact = HContactFromUIN(chk.dwUin, &added);
            TmpContact = getSettingByte(chk.hContact, "TmpContact", 0);
            if(NotOnList && !TmpContact)
            {
                uid_str szUid;
                DWORD TmpUin;
                DBWriteContactSettingStringUtf(chk.hContact, "CList", "Group", TmpGroupName);
                DBWriteContactSettingByte(chk.hContact,"CList","Hidden",0);
                DBWriteContactSettingByte(chk.hContact,"CList","NotOnList",bAddTemp);
                if(bAddTemp)
                    setSettingByte(chk.hContact, "TmpContact", bAddTemp);
                setSettingByte(chk.hContact, "CheckSelfRemove", 1);//excluding from checkselfremove
                getContactUid(chk.hContact, &TmpUin, &szUid);
                if(bTmpAuthRequet && szUid)
                    icq_sendAuthReqServ(TmpUin, szUid, Translate("Automated authorization request\nYou automatically added by CheckContact(CHECKCONTACT chk) function\n////utiliyies.c:2350"));
                if(bTmpSendAdded)
                    icq_sendYouWereAddedServ(chk.dwUin, getSettingDword(NULL, "UIN", 0));
                ShowPopUpMsg(chk.hContact, chk.dwUin, NickFromHandleUtf(chk.hContact), Translate("Added to temporary group"), LOG_NOTE); //temporary solution
            }
        }
        if(InDb)
        {
            if(chk.msg)
            {
                if(strlen(chk.msg) > 1 && chk.popup)
                {
                    popmsg[0] = '\0';
                    switch(chk.popuptype)
                    {
                    case POPTYPE_SPAM:
                        //nothing here, used direct ShowPopupMsg call
                        break;
                    case POPTYPE_UNKNOWN:
                        //nothing here, used direct ShowPopupMsg call
                        break;
                    case POPTYPE_VIS:
                        if(bVisPopUp)
                        {
                            strcpy(popmsg,"... is checking your real status ");
                        }
                        break;
                    case LOG_NOTE:
                    case LOG_WARNING:
                    case LOG_ERROR:
                    case LOG_FATAL:
                        strcpy(popmsg,chk.msg);
                        break;
                    case POPTYPE_FOUND:
                        if(bFoundPopUp)
                        {
                            strcpy(popmsg,chk.msg);
                        }
                        break;
                    case POPTYPE_SCAN:
                        if(bScanPopUp)
                        {
                            strcpy(popmsg,chk.msg);
                        }
                        break;
                    case POPTYPE_CLIENT_CHANGE:
                        if(bClientChangePopUp)
                        {
                            strcpy(popmsg,chk.msg);
                        }
                        break;
                    case POPTYPE_INFO_REQUEST:
                        if(bInfoRequestPopUp)
                            strcpy(popmsg,chk.msg);
                        break;
                    case POPTYPE_IGNORE_CHECK:
                        if(bIgnoreCheckPop)
                            strcpy(popmsg,chk.msg);
                        break;
                    case POPTYPE_SELFREMOVE:
                        if(bPopSelfRem)
                            strcpy(popmsg,chk.msg);
                        break;
                    case POPTYPE_AUTH:
                        if(bAuthPopUp)
                            strcpy(popmsg,chk.msg);
                        break;
                    default:
                        break;
                    }
                    if(chk.historyevent||chk.logtofile)
                    {
                        switch(chk.icqeventtype)
                        {
                        case ICQEVENTTYPE_AUTH_DENIED:
                            if(bLogAuthHistory)
                                strcpy(eventmsghistory,"was denied your authorization request");
                            if(bLogAuthFile)
                                strcpy(eventmsgfile,"was denied your authorization request");
                            break;
                        case ICQEVENTTYPE_AUTH_GRANTED:
                            if(bLogAuthHistory)
                                strcpy(eventmsghistory,"Authorization request granted");
                            if(bLogAuthFile)
                                strcpy(eventmsgfile,"Authorization request granted");
                            break;
                        case ICQEVENTTYPE_CHECK_STATUS:
                            if(bLogStatusCheckHistory)
                                strcpy(eventmsghistory,"check your status ");
                            if(bLogStatusCheckFile)
                                strcpy(eventmsgfile,"check your status ");
                            break;
                        case ICQEVENTTYPE_CHECK_XSTATUS:
                            if(bLogReadXStatusHistory)
                                strcpy(eventmsghistory,"reading your xtraz ");
                            if(bLogReadXStatusFile)
                                strcpy(eventmsgfile,"reading your xtraz ");
                            break;
                        case ICQEVENTTYPE_WAS_FOUND:
                            if(bLogASDHistory)
                                strcpy(eventmsghistory,chk.msg);
                            if(bLogASDFile)
                                strcpy(eventmsgfile,chk.msg);
                            break;
                        case ICQEVENTTYPE_CLIENT_CHANGE:
                            if(bLogIgnoreCheckHistory)
                                strcpy(eventmsghistory,chk.msg);
                            if(bLogIgnoreCheckFile)
                                strcpy(eventmsgfile,chk.msg);
                            break;
                        case ICQEVENTTYPE_EMAILEXPRESS:
                            //nothing to do here at this time ...
                            break;
                        case ICQEVENTTYPE_FUTURE_AUTH:
                            //i do not know what can i do here, fix me....
                            break;
                        case ICQEVENTTYPE_IGNORECHECK_STATUS:
                            if(bLogIgnoreCheckHistory)
                                strcpy(eventmsghistory,"check your ignore list");
                            if(bLogIgnoreCheckFile)
                                strcpy(eventmsgfile,"check your ignore list");
                            break;
                        case ICQEVENTTYPE_SELF_REMOVE:
                            if(bLogSelfRemoveHistory)
                                strcpy(eventmsghistory,"removed himself from your Serverlist!");
                            if(bLogSelfRemoveFile)
                                strcpy(eventmsgfile,"removed himself from your Serverlist!");
                            break;
                        case ICQEVENTTYPE_SMS:
                            //nothing to do here at this time ...
                            break;
                        case ICQEVENTTYPE_WEBPAGER:
                            //nothing to do here at this time ...
                            break;
                        case ICQEVENTTYPE_YOU_ADDED:
                            break;
                        case ICQEVENTTYPE_AUTH_REQUESTED:
                            break;
                        default:
                            break;
                        }
                        if(popmsg && strlen(popmsg) > 1)
                        {
                            if(!strstr(chk.msg,popmsg))
                                strcat(popmsg, chk.msg);
                            ShowPopUpMsg(chk.hContact, chk.dwUin, NickFromHandleUtf(chk.hContact), popmsg, chk.popuptype);
                        }
                        if(eventmsghistory && strlen(eventmsghistory) > 1)
                        {
                            if(!strstr(chk.msg,eventmsghistory))
                                strcat(eventmsghistory, chk.msg);
                            HistoryLog(chk.hContact, chk.dwUin, eventmsghistory, chk.icqeventtype, chk.dbeventflag);
                        }
                        if(eventmsgfile && strlen(eventmsgfile) > 1)
                        {
                            if(!strstr(chk.msg,eventmsgfile))
                                strcat(eventmsgfile, chk.msg);
                            LogToFile(chk.hContact, chk.dwUin, eventmsgfile, chk.icqeventtype);
                        }
                    }
                }
            }
            if(chk.PSD != -1 && !Cancel)
            {
                if(!chk.PSD)
                    chk.PSD=21;
                if(gbASD&&chk.PSD<10)
                    icq_SetUserStatus(chk.dwUin, 0, chk.PSD, 0);
                else if(getSettingWord(chk.hContact, "Status", ID_STATUS_OFFLINE) == ID_STATUS_OFFLINE && (bPSD&&chk.PSD>20)) //check for wrong PSD call
                    icq_SetUserStatus(chk.dwUin, 0, chk.PSD, 0);
            }
        }
    }
}

INT_PTR IncognitoAwayRequest(WPARAM wParam, LPARAM lParam)
{
    bIncognitoRequest = TRUE;

    CallService(MS_AWAYMSG_SHOWAWAYMSG, wParam, 0);
    return 0;
}



void HistoryLog(HANDLE hContact, DWORD dwUin, char *data, int event_type, int flag)
{
    extern BOOL bHcontactHistory;
    PBYTE pCurBlob;
    WORD wTextLen;
    char szText[MAX_PATH];
    DBEVENTINFO Event = {0};
    Event.cbSize=sizeof(Event);
    Event.szModule=ICQ_PROTOCOL_NAME;
    Event.eventType=event_type;
    Event.flags=flag,DBEF_UTF;
    Event.timestamp=(DWORD)time(NULL);
    mir_snprintf(szText, sizeof(szText), "%s %s ( %s: %s, %s: (%u) )", Translate(data), Translate("by"), Translate("Nick"), (NickFromHandle(hContact)), Translate("UIN"), dwUin);
    wTextLen = strlen(szText);
    Event.cbBlob = sizeof(DWORD)+sizeof(HANDLE)+wTextLen+1;
    pCurBlob = Event.pBlob = (PBYTE)icq_alloc_zero(Event.cbBlob);
    memcpy(pCurBlob,&szText,wTextLen);
    pCurBlob+=wTextLen;
    *(char *)pCurBlob = 0;
    pCurBlob++;
    memcpy(pCurBlob,&dwUin,sizeof(DWORD));
    pCurBlob+=sizeof(DWORD);
    memcpy(pCurBlob,&hContact,sizeof(HANDLE));
    if(bHcontactHistory)
        CallService(MS_DB_EVENT_ADD,(WPARAM)(HANDLE)hContact,(LPARAM)&Event);
    else
        CallService(MS_DB_EVENT_ADD,(WPARAM)(HANDLE)NULL,(LPARAM)&Event);
    //we need show popups for events from here
}


void LogToFile(HANDLE hContact, DWORD dwUin, char *string, int event_type)
{

    char szTime[30];
    char content[MAX_PATH];
    char filename[1024];
    time_t now;
    FILE *f;

    wsprintfA(filename, getSettingStringUtf(NULL, ICQ_PROTOCOL_NAME, "EventsLog", "EventsLog.txt"));


    switch(event_type)
    {
    case ICQEVENTTYPE_AUTH_GRANTED:
        if(!bLogAuthFile)
            return;
        mir_snprintf(content, sizeof(content), "%s %s", Translate("granted your authorization request "), Translate(string));
        break;
    case ICQEVENTTYPE_AUTH_DENIED:
        if(!bLogAuthFile)
            return;
        mir_snprintf(content, sizeof(content), "%s %s", Translate("denied your authorization request "), Translate(string));
        break;
    case ICQEVENTTYPE_SELF_REMOVE:
        if(!bLogSelfRemoveFile)
            return;
        mir_snprintf(content, sizeof(content), "%s %s", Translate("removed himself from your serverlist "), Translate(string));
        break;
    case ICQEVENTTYPE_FUTURE_AUTH:
        if(!bLogAuthFile)
            return;
        mir_snprintf(content, sizeof(content), "%s %s", Translate("granted you future authorization "), Translate(string));
        break;
    case ICQEVENTTYPE_CLIENT_CHANGE:
        if(!bLogClientChangeFile)
            return;
        {
            char* tmp = mir_strdup(string);
            mir_utf8decode(tmp, NULL);
            strcpy(content, tmp);
            mir_free(tmp);
        }
        break;

    case ICQEVENTTYPE_CHECK_STATUS:
        if(!bLogStatusCheckFile)
            return;
        mir_snprintf(content, sizeof(content), "%s %s", Translate("checked your real status "), Translate(string));
        break;
    case ICQEVENTTYPE_CHECK_XSTATUS:
        if(!bLogReadXStatusFile)
            return;
        mir_snprintf(content, sizeof(content), "%s %s", Translate("reading your xtraz "), Translate(string));
        break;
    case ICQEVENTTYPE_WAS_FOUND:
        if(!bLogASDFile)
            return;
        mir_snprintf(content, sizeof(content), "%s", Translate(string));
        break;
    case ICQEVENTTYPE_IGNORECHECK_STATUS:
        if(!bLogIgnoreCheckFile)
            return;
        mir_snprintf(content, sizeof(content), "%s %s", Translate("checked his ignore state"), Translate(string));
        break;

    default:
        wsprintfA(content, "unknown eventype processed");
        return;
    }


    now = time(NULL);
    strftime(szTime, sizeof(szTime), "%a, %d %b %Y %H:%M:%S", localtime(&now));
    /* Sun, 00 Jan 0000 00:00:00 */
    f = fopen( filename, "a+" );
    if( f != NULL )
    {
        fprintf( f, "[%s] %s (%u) %s\n", szTime, NickFromHandle(hContact), dwUin, content);
        fclose(f);
    }
}

WORD GetProtoVersion()
{
    int ver = 0;
    if (gbVerEnabled)
    {
        ver = (DBGetContactSettingWord(NULL,ICQ_PROTOCOL_NAME,"setVersion", 0));
    }
    else
    {
        switch (DBGetContactSettingWord(NULL,ICQ_PROTOCOL_NAME,"CurrentID",0)) //client change version
        {
        case 1:						//unknown
            ver = 66;
            break;
        case 23:
        case 24:
        case 2:						//qip
        case 3:						//ysm
        case 7:						//trillian
        case 46:                   //qip infium
            ver = 11;
            break;
        case 6:						//Jimm
            ver = 8;
            break;
        case 9:						//Kopete
        case 52:                  //NanoICQ
            ver = 10;
            break;
        case 11:
            ver = 13;
            break;
        case 4:						//icq lite
        case 5:						//&RQ
        case 12:					//rambler
        case 13:						//icq 5.1
        case 14:
        case 15:
        case 16:
        case 17:
        case 26:         //icq6
        case 44:        //QNEXT
        case 45:         //pyICQ
            ver = 9;
            break;
        case 21:                 //stICQ
            ver = 2;
            break;
        case 36:       //ICQ99
            ver = 6;
            break;
        case 37:               //WebICQ
            ver = 7;
            break;
        case 35:          //GAIM
            ver = 0;
            break;
        case 57:         //bayanICQ
            ver = 11;
        case 58:         //Core Pager
            ver = 11;
        default :						//miranda
            ver = 8;
            break;
        }
    }
    DBWriteContactSettingWord(NULL,ICQ_PROTOCOL_NAME,"setVersion",(WORD)ver);
    return ver;
}

static void SetDwFT(DWORD *dwFT, char* DbValue, DWORD DwValue)
{
    *dwFT=DwValue;
    setSettingDword(NULL, DbValue, DwValue);
}

void SetTimeStamps(DWORD *dwFT1, DWORD *dwFT2, DWORD *dwFT3)
{
    switch (DBGetContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "CurrentID", 0))  //client change dwFT
    {
    case 3:											//ysm
        SetDwFT(dwFT1, "dwFT1", 0xFFFFFFAB);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 4:											//ICQ lite
        SetDwFT(dwFT1, "dwFT1", 0x3AA773EE);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 5:													//&RQ
        SetDwFT(dwFT1, "dwFT1", 0xFFFFFF7F);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 7:										//trillian
        SetDwFT(dwFT1, "dwFT1", 0x3B75AC09);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 8:													//licq
        SetDwFT(dwFT1, "dwFT1", 0x2C0BA3DD);
        SetDwFT(dwFT2, "dwFT2", 0x7D800403);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 2:											//qip
        SetDwFT(dwFT1, "dwFT1", 0x08000902);
        SetDwFT(dwFT2, "dwFT2", 0x0000000E);
        SetDwFT(dwFT3, "dwFT3", 0x0000000F);
        break;
    case 46:       //QIP Infium
        SetDwFT(dwFT1, "dwFT1", 0x00002346);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 18:
    case 19:
        SetDwFT(dwFT1, "dwFT1", 0x3AA773EE);
        SetDwFT(dwFT2, "dwFT2", 0x3AA66380);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 21:
        SetDwFT(dwFT1, "dwFT1", 0x3BA8DBAF);
        SetDwFT(dwFT2, "dwFT2", 0x3BEB5373);
        SetDwFT(dwFT3, "dwFT3", 0x3BEB5262);
        break;
    case 22:
        SetDwFT(dwFT1, "dwFT1", 0x3B4C4C0C);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x3B7248ed);
        break;
    case 1:											//unknown
    case 6:											//Jimm
    case 9:											//Kopete
    case 10:										////icq for mac
    case 12:										//rambler
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 20:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 40:           //uIM
    case 41:           //TICQClient
    case 42:           //IC@
    case 43:          //PreludeICQ
    case 44:         //QNEXT
    case 45:         //pyICQ
    case 47:       //JICQ
    case 49:       //MIP
    case 50:     //Trillian Astra
    case 52:     //NanoICQ
    case 53:		//IMadering
        SetDwFT(dwFT1, "dwFT1", 0x00000000);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 55:		//D[i]Chat
        SetDwFT(dwFT1, "dwFT1", 0x66666666);
        SetDwFT(dwFT2, "dwFT2", 0x00000071);
        SetDwFT(dwFT3, "dwFT3", 0x66666666);
        break;
    case 56:		//LocID
        SetDwFT(dwFT1, "dwFT1", 0x48151623);
        SetDwFT(dwFT2, "dwFT2", 0x00180117);
        SetDwFT(dwFT3, "dwFT3", 0x48151623);
        break;
    case 57:		//BayanICQ
        SetDwFT(dwFT1, "dwFT1", 0x2000249B);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 58:		//Core Pager
        SetDwFT(dwFT1, "dwFT1", 0x0204000A);
        SetDwFT(dwFT2, "dwFT2", 0xFFFF0011);
        SetDwFT(dwFT3, "dwFT3", 0x1100FFFF);
        break;
    case 29:
        SetDwFT(dwFT1, "dwFT1", 0x44F523B0);
        SetDwFT(dwFT2, "dwFT2", 0x44F523A6);
        SetDwFT(dwFT3, "dwFT3", 0x44F523A6);
        break;
    case 30:           //alicq
        SetDwFT(dwFT1, "dwFT1", 0xffffffbe);
        SetDwFT(dwFT2, "dwFT2", 0x00090800);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 31:           //mICQ
        SetDwFT(dwFT1, "dwFT1", 0xFFFFFF42);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 32:            //StrICQ 0.4
        SetDwFT(dwFT1, "dwFT1", 0xFFFFFF8F);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 33:            //vICQ 0.43.0.0
        SetDwFT(dwFT1, "dwFT1", 0x04031980);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 34:            //IM2
        SetDwFT(dwFT1, "dwFT1", 0x3FF19BEB);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x3FF19BEB);
        break;
    case 35:           //GAIM
        SetDwFT(dwFT1, "dwFT1", 0xffffffff);
        SetDwFT(dwFT2, "dwFT2", 0xffffffff);
        SetDwFT(dwFT3, "dwFT3", 0xffffffff);
        break;
    case 36:          //ICQ99
        SetDwFT(dwFT1, "dwFT1", 0x3AA773EE);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 37:          //WebICQ
        SetDwFT(dwFT1, "dwFT1", 0xFFFFFFFF);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 38:          //SmartICQ
        SetDwFT(dwFT1, "dwFT1", 0xDDDDEEFF);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 39:           //IM+
        SetDwFT(dwFT1, "dwFT1", 0x494D2B00);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 48:  //SpamBot
        SetDwFT(dwFT1, "dwFT1", 0xffffffff);
        SetDwFT(dwFT2, "dwFT2", 0x00000000);
        SetDwFT(dwFT3, "dwFT3", 0x3B7248ED);
        break;
    case 51:           //R&Q
        SetDwFT(dwFT1, "dwFT1", 0xFFFFF666);
        SetDwFT(dwFT2, "dwFT2", 0x00000453);
        SetDwFT(dwFT3, "dwFT3", 0x00000000);
        break;
    case 11:
        SetDwFT(dwFT1, "dwFT1", gbUnicodeCore_dep ? 0x7FFFFFFF : 0xFFFFFFFF);
        SetDwFT(dwFT2, "dwFT2", 0x06060600);
        if(gbHideIdEnabled)
            SetDwFT(dwFT3, "dwFT3", gbUnicodeCore_dep?0x80000000:0x00000000);
        else
            SetDwFT(dwFT3, "dwFT3", gbSecureIM?0x5AFEC0DE:0x00000000);
        break;
    default :								//miranda
        SetDwFT(dwFT1, "dwFT1", gbUnicodeCore_dep ? 0x7FFFFFFF : 0xFFFFFFFF);
        SetDwFT(dwFT2, "dwFT2", ICQ_PLUG_VERSION);
        if(gbHideIdEnabled)
            SetDwFT(dwFT3, "dwFT3", gbUnicodeCore_dep ? 0x80000000 : 0x00000000);
        else
            SetDwFT(dwFT3, "dwFT3", gbSecureIM ? 0x5AFEC0DE : 0x00000000);
        break;
    }
}

void RemoveTempUsers()
{
    typedef struct hContact_entry_s
    {
        HANDLE hContact;
        struct hContact_entry_s *next;
    } hContact_entry;
    HANDLE hContact;
    hContact_entry *first, *plist, *tmp;
    DBVARIANT dbv = {0};
    hContact = FindFirstContact();
    first = (hContact_entry *)icq_alloc_zero(sizeof(hContact_entry));
    plist = first;
    plist->hContact = INVALID_HANDLE_VALUE;
    if(hContact)
    {
        do
        {
            if(DBGetContactSettingByte(hContact, "CList", "NotOnList", 0) || (DBGetContactSettingString(hContact, "CList", "Group", &dbv) && dbv.pszVal == "Not In List" ))
            {
                plist->hContact = hContact;
                plist->next = (hContact_entry *)icq_alloc_zero(sizeof(hContact_entry));
                plist = plist->next;
                plist->hContact = INVALID_HANDLE_VALUE;
            }
        }
        while(hContact = FindNextContact(hContact))
            ;
        plist = first;
        while(plist->hContact != INVALID_HANDLE_VALUE)
        {
            CallService(MS_DB_CONTACT_DELETE, (WPARAM)plist->hContact, 0);
            tmp = plist;
            plist = plist->next;
            mir_free(tmp);
        }
        mir_free(plist);
    }
}
char* time2text(time_t time)
{
    struct tm *local = localtime(&time);

    if (local)
    {
        char *str = asctime(local);
        str[24] = '\0'; // remove new line
        return str;
    }
    else
        return "<invalid>";
}

char *date()
{
    static char d[11];
    char *tmp = __DATE__, m[4], mn[3] = "01";
    m[0]=tmp[0];
    m[1]=tmp[1];
    m[2]=tmp[2];
    if(strstr(m,"Jan"))
        strcpy(mn,"01");
    else if(strstr(m,"Feb"))
        strcpy(mn,"02");
    else if(strstr(m,"Mar"))
        strcpy(mn,"03");
    else if(strstr(m,"Apr"))
        strcpy(mn,"04");
    else if(strstr(m,"May"))
        strcpy(mn,"05");
    else if(strstr(m,"Jun"))
        strcpy(mn,"06");
    else if(strstr(m,"Jul"))
        strcpy(mn,"07");
    else if(strstr(m,"Aug"))
        strcpy(mn,"08");
    else if(strstr(m,"Sep"))
        strcpy(mn,"09");
    else if(strstr(m,"Oct"))
        strcpy(mn,"10");
    else if(strstr(m,"Nov"))
        strcpy(mn,"11");
    else if(strstr(m,"Dec"))
        strcpy(mn,"12");
    d[0]=tmp[7];
    d[1]=tmp[8];
    d[2]=tmp[9];
    d[3]=tmp[10];
    d[4]='.';
    d[5]=mn[0];
    d[6]=mn[1];
    d[7]='.';
    if (tmp[4] == ' ')
        d[8] = '0';
    else
        d[8]=tmp[4];
    d[9]=tmp[5];
    return d;
}

void* icq_alloc_zero(size_t size)
{
    void *mem = NULL;
    mem = mir_alloc(size);
    if(mem)
    {
        ZeroMemory(mem, size);
        return mem;
    }
    return 0;
}