// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin ÷berg, Sam Kothari, Robert Rainwater
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
// Revision       : $Revision: 36 $
// Last change on : $Date: 2007-08-05 03:45:10 +0300 (–í—Å, 05 –∞–≤–≥ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#ifndef __COOKIES_H
#define __COOKIES_H


#define CKT_MESSAGE         0x01
#define CKT_FILE            0x02
#define CKT_SEARCH          0x04
#define CKT_SERVERLIST      0x08
#define CKT_SERVICEREQUEST  0x0A
#define CKT_REVERSEDIRECT   0x0C
#define CKT_FAMILYSPECIAL   0x10
#define CKT_OFFLINEMESSAGE  0x12
#define CKT_DIRECTORY_QUERY   0x18
#define CKT_DIRECTORY_UPDATE  0x19
#define CKT_AVATAR          0x20
#define CKT_CHECKSPAMBOT    0x40

typedef struct icq_cookie_info_s
{
    DWORD dwCookie;
    HANDLE hContact;
    void *pvExtra;
    DWORD dwTime;
    BYTE bType;
} icq_cookie_info;

typedef struct familyrequest_rec_s
{
    WORD wFamily;
    void (*familyhandler)(HANDLE hConn, char* cookie, WORD cookieLen);
} familyrequest_rec;


typedef struct offline_message_cookie_s
{
    int nMessages;
    int nMissed;
} offline_message_cookie;

typedef struct message_cookie_data_s
{
    DWORD dwMsgID1;
    DWORD dwMsgID2;
    WORD bMessageType;
    BYTE nAckType;
} cookie_message_data;

#define ACKTYPE_NONE   0
#define ACKTYPE_SERVER 1
#define ACKTYPE_CLIENT 2

typedef struct message_cookie_data_ex_s
{
    cookie_message_data msg;
    BYTE isOffline;
} cookie_message_data_ext;

typedef struct fam15_cookie_data_s
{
    BYTE bRequestType;
} fam15_cookie_data;

#define DIRECTORYREQUEST_INFOUSER       0x01
#define DIRECTORYREQUEST_INFOOWNER      0x02
#define DIRECTORYREQUEST_INFOMULTI      0x03
#define DIRECTORYREQUEST_SEARCH         0x08
#define DIRECTORYREQUEST_UPDATEOWNER    0x10
#define DIRECTORYREQUEST_UPDATENOTE     0x11
#define DIRECTORYREQUEST_UPDATEPRIVACY  0x12


#define REQUESTTYPE_OWNER        0
#define REQUESTTYPE_USERAUTO     1
#define REQUESTTYPE_USERMINIMAL  2
#define REQUESTTYPE_USERDETAILED 3
#define REQUESTTYPE_PROFILE      4


typedef struct search_cookie_s
{
    BYTE bSearchType;
    char* szObject;
    DWORD dwMainId;
    DWORD dwStatus;
} search_cookie;

#define SEARCHTYPE_UID     0
#define SEARCHTYPE_EMAIL   1
#define SEARCHTYPE_NAMES   2
#define SEARCHTYPE_DETAILS 4

typedef struct avatarcookie_t
{
    DWORD dwUin;
    HANDLE hContact;
    unsigned int hashlen;
    char *hash;
    unsigned int cbData;
    char *szFile;
} avatarcookie;

typedef struct
{
    cookie_message_data pMessage;
    HANDLE hContact;
    DWORD dwUin;
    int type;
    void *ft;
} reverse_cookie;


void InitCookies(void);
void UninitCookies(void);

DWORD AllocateCookie(BYTE bType, WORD wIdent, HANDLE hContact, void *pvExtra);
void FreeCookie(DWORD dwCookie);
void ReleaseCookie(DWORD dwCookie);
DWORD GenerateCookie(WORD wIdent);

int GetCookieType(DWORD dwCookie);

int FindCookie(DWORD wCookie, HANDLE *phContact, void **ppvExtra);
int FindCookieByData(void *pvExtra, DWORD *pdwCookie, HANDLE *phContact);
int FindCookieByType(BYTE bType, DWORD *pdwCookie, HANDLE *phContact, void** ppvExtra);
int FindMessageCookie(DWORD dwMsgID1, DWORD dwMsgID2, DWORD *pdwCookie, HANDLE *phContact, cookie_message_data **ppvExtra);

void InitMessageCookie(cookie_message_data *pCookie);
cookie_message_data *CreateMessageCookie(WORD bMsgType, BYTE bAckType);

#endif /* __COOKIES_H */
