// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin Öberg, Sam Kothari, Robert Rainwater
// Copyright © 2004,2005,2006,2007 Joe Kucera
// Copyright © 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Thief, Angeli-Ka, nullbie
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
// File name      : $HeadURL: https://icqjplusmod.googlecode.com/svn/trunk/utilities.h $
// Revision       : $Revision: 43 $
// Last change on : $Date: 2007-08-20 01:51:06 +0300 (ÐŸÐ½, 20 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#ifndef __UTILITIES_H
#define __UTILITIES_H

#define F_TEMPVIS  1
#define mir_asctime(ptr) ((ptr==NULL)?("<invalid timestamp>"):asctime(ptr))

typedef struct icq_ack_args_s
{
    HANDLE hContact;
    int    nAckType;
    int    nAckResult;
    HANDLE hSequence;
    LPARAM pszMessage;
} icq_ack_args;

typedef struct icq_contacts_cache_s
{
    HANDLE hContact;
    DWORD dwUin;
    DWORD flags;
} icq_contacts_cache;


/*---------* Functions *---------------*/

void EnableDlgItem(HWND hwndDlg, UINT control, int state);
void ShowDlgItem(HWND hwndDlg, UINT control, int state);
void icq_EnableMultipleControls(HWND hwndDlg, const UINT* controls, int cControls, int state);
void icq_ShowMultipleControls(HWND hwndDlg, const UINT* controls, int cControls, int state);
int IcqStatusToMiranda(WORD wStatus);
char* QIPStatusToString(int QipStatus);
WORD MirandaStatusToIcq(int nStatus);
int MirandaStatusToSupported(int nMirandaStatus);
char *MirandaStatusToString(int);
char *MirandaStatusToStringUtf(int);
char**MirandaStatusToAwayMsg(int nStatus);
int IDFromCacheByUin(DWORD dwUin);
int CacheIDCount(void);
int SetCacheFlagsByID(int ID, DWORD flags);
int GetCacheByID(int ID, icq_contacts_cache *icc);
int GetFromCacheByID(int ID, HANDLE *hContact, DWORD *dwUin);

int AwayMsgTypeToStatus(int nMsgType);

void SetGatewayIndex(HANDLE hConn, DWORD dwIndex);
DWORD GetGatewayIndex(HANDLE hConn);
void FreeGatewayIndex(HANDLE hConn);

void AddToSpammerList(DWORD dwUIN);
BOOL IsOnSpammerList(DWORD dwUIN);

void InitCache();
void UninitCache();
void DeleteFromCache(HANDLE hContact);
HANDLE HContactFromUIN(DWORD dwUin, int *Added);
HANDLE HContactFromUID(DWORD dwUIN, char *pszUID, int *Added);
char *NickFromHandle(HANDLE hContact);
char *NickFromHandleUtf(HANDLE hContact);
char *strUID(DWORD dwUIN, char *pszUID);
void NickFromHandleStatic(HANDLE hContact, char *szNick, WORD wLen);
void SetContactHidden(HANDLE hContact, BYTE bHidden);
void makeUserOffline(HANDLE hContact);

size_t __fastcall strlennull(const char *string);
int __fastcall strcmpnull(const char *str1, const char *str2);
int null_snprintf(char *buffer, size_t count, const char* fmt, ...);
char* __fastcall null_strdup(const char *string);
char* __fastcall null_strcpy(char *dest, const char *src, size_t maxlen);
size_t __fastcall null_strcut(char *string, size_t maxlen);

void parseServerAddress(char *szServer, WORD* wPort);

char *DemangleXml(const char *string, int len);
char *MangleXml(const char *string, int len);
char *EliminateHtml(const char *string, int len);
char* ApplyEncoding(const char *string, const char* pszEncoding);

void ResetSettingsOnListReload(void);
void ResetSettingsOnConnect(void);
void ResetSettingsOnLoad(void);
int RandRange(int nLow, int nHigh);

BOOL IsStringUIN(char* pszString);

void icq_SendProtoAck(HANDLE hContact, DWORD dwCookie, int nAckResult, int nAckType, char* pszMessage);

void SetCurrentStatus(int nStatus);

int IsMetaInfoChanged(HANDLE hContact);

BOOL writeDbInfoSettingString(HANDLE hContact, const char* szSetting, char** buf, WORD* pwLength);
BOOL writeDbInfoSettingWord(HANDLE hContact, const char *szSetting, char **buf, WORD* pwLength);
BOOL writeDbInfoSettingWordWithTable(HANDLE hContact, const char *szSetting, struct fieldnames_t *table, char **buf, WORD* pwLength);
BOOL writeDbInfoSettingByte(HANDLE hContact, const char *szSetting, char **buf, WORD* pwLength);
BOOL writeDbInfoSettingByteWithTable(HANDLE hContact, const char *szSetting, struct fieldnames_t *table, char **buf, WORD* pwLength);
void writeDbInfoSettingTLVStringUtf(HANDLE hContact, const char *szSetting, oscar_tlv_chain *chain, WORD wTlv);
void writeDbInfoSettingTLVString(HANDLE hContact, const char *szSetting, oscar_tlv_chain *chain, WORD wTlv);
void writeDbInfoSettingTLVWord(HANDLE hContact, const char *szSetting, oscar_tlv_chain *chain, WORD wTlv);
void writeDbInfoSettingTLVByte(HANDLE hContact, const char *szSetting, oscar_tlv_chain *chain, WORD wTlv);
void writeDbInfoSettingTLVDate(HANDLE hContact, const char* szSettingYear, const char* szSettingMonth, const char* szSettingDay, oscar_tlv_chain* chain, WORD wTlv);
void writeDbInfoSettingTLVDouble(HANDLE hContact, const char *szSetting, oscar_tlv_chain *chain, WORD wTlv);

int GetGMTOffset(void);

BOOL validateStatusMessageRequest(HANDLE hContact, WORD byMessageType);

#define icqOnline ((gnCurrentStatus != ID_STATUS_OFFLINE) && (gnCurrentStatus != ID_STATUS_CONNECTING))

HANDLE NetLib_OpenConnection(HANDLE hUser, const char* szIdent, NETLIBOPENCONNECTION* nloc);
HANDLE NetLib_BindPort(NETLIBNEWCONNECTIONPROC_V2 pFunc, void* lParam, WORD* pwPort, DWORD* pdwIntIP);
void NetLib_CloseConnection(HANDLE *hConnection, int bServerConn);
void NetLib_SafeCloseHandle(HANDLE *hConnection);
int NetLog_Server(const char *fmt,...);
int NetLog_Direct(const char *fmt,...);
int NetLog_Uni(BOOL bDC, const char *fmt,...);

int ICQBroadcastAck(HANDLE hContact,int type,int result,HANDLE hProcess,LPARAM lParam);

int __fastcall ICQTranslateDialog(HWND hwndDlg);
char* __fastcall ICQTranslate(const char* src);
char* __fastcall ICQTranslateUtf(const char* src);
char* __fastcall ICQTranslateUtfStatic(const char* src, char* buf, size_t bufsize);

HANDLE ICQCreateThreadEx(pThreadFuncEx AFunc, void* arg, DWORD* pThreadID);
void ICQCreateThread(pThreadFuncEx AFunc, void* arg);

char* GetUserPassword(BOOL bAlways);
WORD GetMyStatusFlags();

BOOL invis_for(DWORD dwUin, HANDLE hContact);

typedef struct
{
    DWORD dwUin; //contact uin (dword)
    HANDLE hContact;  //hContact(HANDLE)
    int PSD; //value for calling setuserstatus (-1 for disable)
    BOOL popup; //set TRUE for show popup (BOOL)
    int popuptype; //set popup event type (int)
    BOOL historyevent; //set history event type (BOOL)
    BOOL logtofile; //set TRUE for write event to file (BOOL)
    char* msg; //set event message (char)
    int icqeventtype; //set db event type for history (int)
    int dbeventflag; //set additional db flags (int)
    BOOL nottmpcontact; //work with contact which not temporary (BOOL)
    BOOL fornotonlist; //work with contact which not on list (BOOL)
    BOOL foroffline; //work with offline contact (BOOL)
    BOOL weofflinefor; //work with contact which we offline for (BOOL)
} CHECKCONTACT;

void CheckContact(CHECKCONTACT chk);
extern BYTE bVisibility;

INT_PTR IncognitoAwayRequest(WPARAM wParam, LPARAM lParam);
extern BOOL bIncognitoRequest;

/* Unicode FS utility functions */

int IsValidRelativePath(const char *filename);
const char* ExtractFileName(const char *fullname);
char* FileNameToUtf(const char *filename);
int FileStatUtf(const char *path, struct _stati64 *buffer);
int MakeDirUtf(char *dir);
int OpenFileUtf(const char *filename, int oflag, int pmode);

/* Unicode UI utility functions */
WCHAR* GetWindowTextUcs(HWND hWnd);
void SetWindowTextUcs(HWND hWnd, WCHAR *text);
char* GetWindowTextUtf(HWND hWnd);
char* GetDlgItemTextUtf(HWND hwndDlg, int iItem);
void SetWindowTextUtf(HWND hWnd, const char* szText);
void SetDlgItemTextUtf(HWND hwndDlg, int iItem, const char* szText);
LONG SetWindowLongUtf(HWND hWnd, int nIndex, LONG dwNewLong);
LRESULT CallWindowProcUtf(WNDPROC OldProc, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int ComboBoxAddStringUtf(HWND hCombo, const char* szString, DWORD data);
int ListBoxAddStringUtf(HWND hList, const char* szString);

int MessageBoxUtf(HWND hWnd, const char* szText, const char* szCaption, UINT uType);
HWND DialogBoxUtf(BOOL bModal, HINSTANCE hInstance, const char* szTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
HWND CreateDialogUtf(HINSTANCE hInstance, const char* lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc);

void HistoryLog(HANDLE hContact, DWORD dwUin, char *data, int event_type, int flag);
void LogToFile(HANDLE hContact, DWORD dwUin, char *string, int event_type);
void SetTimeStamps(DWORD *dwFT1, DWORD *dwFT2, DWORD *dwFT3);
extern BOOL gbSecureIM;
extern BOOL gbExtraIcons;
WORD GetProtoVersion();
char* time2text(time_t time);

//this is not from here
void   parseDirectoryUserDetailsData(HANDLE hContact, oscar_tlv_chain *cDetails, DWORD dwCookie, fam15_cookie_data *pCookieData, WORD wReplySubType);
void   parseDirectorySearchData(oscar_tlv_chain *cDetails, DWORD dwCookie, fam15_cookie_data *pCookieData, WORD wReplySubType);
void* icq_alloc_zero(size_t size);

#endif /* __UTILITIES_H */
