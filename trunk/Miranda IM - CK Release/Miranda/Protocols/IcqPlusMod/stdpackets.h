// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin Öberg, Sam Kothari, Robert Rainwater
// Copyright © 2004,2005,2006,2007 Joe Kucera
// Copyright © 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
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
// File name      : $Source$
// Revision       : $Revision: 51 $
// Last change on : $Date: 2007-08-30 23:46:51 +0300 (Ð§Ñ‚, 30 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Low-level functions that really sends data to server
//
// -----------------------------------------------------------------------------

#ifndef __STDPACKETS_H
#define __STDPACKETS_H

struct icq_contactsend_s
{
    DWORD uin;
    char *uid;
    char *szNick;
};

void packMsgColorInfo(icq_packet *packet);

void icq_sendCloseConnection();

void icq_requestnewfamily(WORD wFamily, void (*familyhandler)(HANDLE hConn, char* cookie, WORD cookieLen));

void icq_setidle(int bAllow);
void icq_setstatus(WORD wStatus, int bSetMood);
DWORD icq_sendGetInfoServ(HANDLE, DWORD, int);
DWORD icq_sendGetAimProfileServ(HANDLE hContact, char *szUid);
DWORD icq_sendGetAwayMsgServ(HANDLE, DWORD, int, WORD);
DWORD icq_sendGetStealthAwayMsgServ(HANDLE hContact, DWORD dwUin, int type, WORD wVersion);
DWORD icq_sendGetAwayMsgServExt(HANDLE hContact, DWORD dwUin, int type, WORD wVersion);
DWORD icq_sendGetAimAwayMsgServ(HANDLE hContact, char *szUID, int type);
void icq_sendSetAimAwayMsgServ(char *szMsg);

DWORD icq_sendGetLocationInfo(HANDLE hContact, DWORD dwUIN, char *szUID);

void icq_sendFileSendServv7(filetransfer* ft, const char *szFiles);
void icq_sendFileSendServv8(filetransfer* ft, const char *szFiles, int nAckType);

void icq_sendFileAcceptServ(DWORD dwUin, filetransfer *ft, int nAckType);
void icq_sendFileAcceptServv7(DWORD dwUin, DWORD TS1, DWORD TS2, DWORD dwCookie, const char *szFiles, const char *szDescr, DWORD dwTotalSize, WORD wPort, BOOL accepted, int nAckType);
void icq_sendFileAcceptServv8(DWORD dwUin, DWORD TS1, DWORD TS2, DWORD dwCookie, const char *szFiles, const char *szDescr, DWORD dwTotalSize, WORD wPort, BOOL accepted, int nAckType);

void icq_sendFileDenyServ(DWORD dwUin, filetransfer* ft, char *szReason, int nAckType);

DWORD icq_sendAdvancedSearchServ(BYTE *fieldsBuffer,int bufferLen);
DWORD icq_changeUserPasswordServ(const char *szPassword);
DWORD icq_changeUserDirectoryInfoServ(const BYTE *pData, WORD wDataLen, BYTE bRequestType);
void icq_sendGenericContact(DWORD dwUin, char* szUid, WORD wFamily, WORD wSubType);
void icq_sendNewContact(DWORD dwUin, char* szUid);
//void icq_sendRemoveContact(DWORD dwUin, char* szUid);
void icq_sendChangeVisInvis(HANDLE hContact, DWORD dwUin, char* szUID, int list, int add);
void icq_sendEntireVisInvisList(int);
void icq_sendAwayMsgReplyServ(DWORD, DWORD, DWORD, WORD, WORD, BYTE, const char **);
DWORD icq_sendSMSServ(const char *szPhoneNumber, const char *szMsg);
void icq_sendMessageCapsServ(DWORD dwUin);
void icq_sendRevokeAuthServ(DWORD dwUin, char *szUid);
void icq_sendGrantAuthServ(DWORD dwUin, char* szUid, char *szMsg);
void icq_sendAuthReqServ(DWORD dwUin, char* szUid, char *szMsg);
void icq_sendAuthResponseServ(DWORD dwUin, char* szUid, int auth, const TCHAR *szReason);
void icq_sendYouWereAddedServ(DWORD,DWORD);

void sendOwnerInfoRequest(void);
DWORD sendUserInfoMultiRequest(BYTE *pRequestData, WORD wDataLen, int nItems);

DWORD icq_SendChannel1Message(DWORD dwUin, char *szUID, HANDLE hContact, char *pszText, cookie_message_data *pCookieData);
DWORD icq_SendChannel1MessageW(DWORD dwUin, char *szUID, HANDLE hContact, wchar_t *pszText, cookie_message_data *pCookieData); // UTF-16
DWORD icq_SendChannel2Message(DWORD dwUin, HANDLE hContact, const char *szMessage, int nBodyLength, WORD wPriority, cookie_message_data *pCookieData, char *szCap);
DWORD icq_SendChannel2Contacts(DWORD dwUin, char *szUid, HANDLE hContact, const char *pData, WORD wDataLen, const char *pNames, WORD wNamesLen, cookie_message_data *pCookieData);
DWORD icq_SendChannel4Message(DWORD dwUin, HANDLE hContact, BYTE bMsgType, WORD wMsgLen, const char *szMsg, cookie_message_data *pCookieData);

void icq_sendAdvancedMsgAck(DWORD, DWORD, DWORD, WORD, BYTE, BYTE);
void icq_sendContactsAck(DWORD dwUin, char *szUid, DWORD dwMsgID1, DWORD dwMsgID2);

void icq_sendReverseReq(directconnect *dc, DWORD dwCookie, cookie_message_data *pCookie);
void icq_sendReverseFailed(directconnect* dc, DWORD dwMsgID1, DWORD dwMsgID2, DWORD dwCookie);

void icq_sendXtrazRequestServ(DWORD dwUin, DWORD dwCookie, char* szBody, int nBodyLen, cookie_message_data *pCookieData);
void icq_sendXtrazResponseServ(DWORD dwUin, DWORD dwMID, DWORD dwMID2, WORD wCookie, char* szBody, int nBodyLen, int nType);

DWORD SearchByUin(DWORD dwUin);
DWORD SearchByNames(const char *pszNick, const char *pszFirstName, const char *pszLastName, WORD wPage);
DWORD SearchByNames_v9(const TCHAR *nick, const TCHAR *firstName, const TCHAR *lastName);
DWORD SearchByMail(char *pszEmail);
DWORD SearchByEmail_v9( const TCHAR *email );

DWORD icq_searchAimByEmail(char* pszEmail, DWORD dwSearchId);

void oft_sendFileRequest(DWORD dwUin, char *szUid, oscar_filetransfer* ft, const char* pszFiles, DWORD dwLocalInternalIP);
void oft_sendFileAccept(DWORD dwUin, char *szUid, oscar_filetransfer* ft);
void oft_sendFileDeny(DWORD dwUin, char *szUid, oscar_filetransfer* ft);
void oft_sendFileCancel(DWORD dwUin, char *szUid, oscar_filetransfer* ft);
void oft_sendFileResponse(DWORD dwUin, char *szUid, oscar_filetransfer* ft, WORD wResponse);
void oft_sendFileRedirect(DWORD dwUin, char *szUid, oscar_filetransfer* ft, DWORD dwIP, WORD wPort, int bProxy);

void SendtZer(HANDLE hContact, DWORD dwUin, char * sztZId, char * sztZName, char * sztZUrl);

void packServMsgSendHeader(icq_packet *p, DWORD dwSequence, DWORD dwID1, DWORD dwID2, DWORD dwUin, char *szUID, WORD wFmt, WORD wLen);
void packEmptyMsg(icq_packet *packet);

#endif /* __STDPACKETS_H */
