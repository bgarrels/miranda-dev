#ifndef __CRYPT_H__
#define __CRYPT_H__

/*
SecureIM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 	2003 Johell
							2005-2009 Baloo

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

#include "secureim.h"
#include "cryptopp.h"
#include "gettime.h"

#define KEYSIZE 256
#define NAMSIZE 512
#define DEFMSGS 4096

#define KEY_A_SIG	0x000000
#define KEY_B_SIG	0x010000

// struct to store all supported protocols
struct SupPro {
	LPSTR name;
	BOOL inspecting;
	int split_on,tsplit_on;
	int split_off,tsplit_off;
};
typedef SupPro *pSupPro;

// struct to store wainting messages
struct waitingMessage {
	LPSTR Message;
	WPARAM wParam;
	waitingMessage *nextMessage;
};
typedef waitingMessage* pWM;

struct partitionMessage {
	int id;
	LPSTR *message; // array of message parts
	partitionMessage *nextMessage;
};
typedef partitionMessage* pPM;

#define HEADER 0xABCD1234
#define FOOTER 0x9876FEDC
#define EMPTYH 0xF1E2D3C4

// memory struct for keys
struct UinKey {
	u_int header;		// HEADER
	HANDLE hContact;	// handle of contact
	pSupPro proto;		// proto struct
	BYTE mode,tmode;	// mode: Native,PGP,GPG,RSA/AES,RSA [0..4]
	BYTE status,tstatus;	// status: Disabled,Enabled,AlwaysTry [0..2] for Native mode
	LPSTR msgSplitted;	// message to combine
	pPM msgPart;		// parts of message
	pWM msgQueue;		// last messages not sended or to resend;
	BOOL sendQueue;		// идет отсылка очереди - не обрабатываются сообщения
	BOOL offlineKey;
	char waitForExchange;	// 0 - сбросить очередь
				// 1 - ожидаем
				// 2 - дослать с шифрованием
				// 3 - дослать без шифрования с вопросом
	BOOL decoded;		// false on decode error
	short features;
	HANDLE cntx;		// crypto context
	BYTE keyLoaded;		// ( 1-PGP, 2-GPG ) | 1-RSA
	BYTE gpgMode,tgpgMode;	// 0-UTF8, 1-ANSI
	char *lastFileRecv;
	char *lastFileSend;
	char **fileSend;
	BOOL finFileRecv;
	BOOL finFileSend;
	LPSTR tmp;		// tmp text string
	u_int footer;		// FOOTER
};
typedef UinKey* pUinKey;

struct TFakeAckParams {
	inline TFakeAckParams( HANDLE p1, HANDLE p2, LONG p3, LPCSTR p4 ) :
		hEvent( p1 ),
		hContact( p2 ),
		id( p3 ),
		msg( p4 )
		{}

	HANDLE	hEvent;
	HANDLE	hContact;
	LONG	id;
	LPCSTR	msg;
};

struct TWaitForExchange {
	inline TWaitForExchange( HANDLE p1, HANDLE p2 ) :
		hEvent( p1 ),
		hContact( p2 )
		{}

	HANDLE	hEvent;
	HANDLE	hContact;
};

extern char szUIN[NAMSIZE];
extern char szName[NAMSIZE];
extern pSupPro proto;
extern pUinKey clist;
extern int proto_cnt;
extern int clist_cnt;

// crypt_lists.cpp
void loadContactList();
void freeContactList();
pUinKey addContact(HANDLE);
void delContact(HANDLE);
pSupPro getSupPro(HANDLE);
pUinKey getUinKey(HANDLE);
pUinKey getUinCtx(HANDLE);
void addMsg2Queue(pUinKey,WPARAM,LPSTR);

void getContactName(HANDLE hContact, LPSTR szName);
void getContactNameA(HANDLE hContact, LPSTR szName);
void getContactUin(HANDLE hContact, LPSTR szUIN);
void getContactUinA(HANDLE hContact, LPSTR szUIN);

// crypt_check.cpp
int getContactStatus(HANDLE);

BOOL isSecureProtocol(HANDLE);
BYTE isContactSecured(HANDLE);
BOOL isClientMiranda(pUinKey ptr, BOOL emptyMirverAsMiranda=FALSE);
BOOL isClientMiranda(HANDLE hContact, BOOL emptyMirverAsMiranda=FALSE);
BOOL isProtoSmallPackets(HANDLE);
BOOL isContactInvisible(HANDLE);
BOOL isNotOnList(HANDLE);
BOOL isContactNewPG(HANDLE);
BOOL isContactPGP(HANDLE);
BOOL isContactGPG(HANDLE);
BOOL isContactRSAAES(HANDLE);
BOOL isContactRSA(HANDLE);
BOOL isChatRoom(HANDLE);
BOOL isFileExist(LPCSTR);
BOOL isSecureIM(pUinKey ptr, BOOL emptyMirverAsSecureIM=FALSE);
BOOL isSecureIM(HANDLE hContact, BOOL emptyMirverAsSecureIM=FALSE);

// crypt_icons.cpp
HICON mode2icon(int,int);
HICON mode2icon2(int,int); // создает КОПИЮ иконки, которую надо разрушить
IconExtraColumn mode2iec(int);
void RefreshContactListIcons(void);
void ShowStatusIcon(HANDLE,UINT);
void ShowStatusIcon(HANDLE);
void ShowStatusIconNotify(HANDLE);

// crypt_popups.cpp
//static int CALLBACK PopupDlgProc(HWND,UINT,WPARAM,LPARAM);
void showPopUp(LPCSTR,HANDLE,HICON,UINT);
void showPopUpEC(HANDLE);
void showPopUpDCmsg(HANDLE,LPCSTR);
void showPopUpDC(HANDLE);
void showPopUpKS(HANDLE);
void showPopUpKRmsg(HANDLE,LPCSTR);
void showPopUpKR(HANDLE);
void showPopUpSM(HANDLE);
void showPopUpRM(HANDLE);

// crypt_meta.cpp
BOOL isProtoMetaContacts(HANDLE);
BOOL isDefaultSubContact(HANDLE);
HANDLE getMetaContact(HANDLE);
HANDLE getMostOnline(HANDLE);
void DeinitMetaContact(HANDLE);

// crypt_dll.cpp
LPSTR InitKeyA(pUinKey,int);
int InitKeyB(pUinKey,LPCSTR);
void InitKeyX(pUinKey,BYTE*);
BOOL CalculateKeyX(pUinKey,HANDLE);
LPSTR encodeMsg(pUinKey,LPARAM);
LPSTR decodeMsg(pUinKey,LPARAM,LPSTR);
BOOL LoadKeyPGP(pUinKey);
BOOL LoadKeyGPG(pUinKey);

// crypt_misc.cpp
unsigned __stdcall sttFakeAck(LPVOID);
unsigned __stdcall sttWaitForExchange(LPVOID);
void waitForExchange(pUinKey ptr, int flag = 1);

#endif
