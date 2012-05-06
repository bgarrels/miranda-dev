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

#include "commonheaders.h"


int SendBroadcast( HANDLE hContact, int type, int result, HANDLE hProcess, LPARAM lParam ) {
	ACKDATA ack;
	memset(&ack,0,sizeof(ack));
	ack.cbSize = sizeof( ACKDATA );
	ack.szModule = 	(char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
	ack.hContact = hContact;
	ack.type = type;
	ack.result = result;
	ack.hProcess = hProcess;
	ack.lParam = lParam;
	return CallService( MS_PROTO_BROADCASTACK, 0, ( LPARAM )&ack );
}


unsigned __stdcall sttFakeAck( LPVOID param ) {

	TFakeAckParams* tParam = ( TFakeAckParams* )param;
	WaitForSingleObject( tParam->hEvent, INFINITE );

	Sleep( 100 );
	if ( tParam->msg == NULL )
		SendBroadcast( tParam->hContact, ACKTYPE_MESSAGE, ACKRESULT_SUCCESS, ( HANDLE )tParam->id, 0 );
	else
		SendBroadcast( tParam->hContact, ACKTYPE_MESSAGE, ACKRESULT_FAILED, ( HANDLE )tParam->id, LPARAM( tParam->msg ));

	CloseHandle( tParam->hEvent );
	delete tParam;

	return 0;
}


unsigned __stdcall sttWaitForExchange( LPVOID param ) {

	TWaitForExchange* tParam = ( TWaitForExchange* )param;
	WaitForSingleObject( tParam->hEvent, INFINITE );

	pUinKey ptr = getUinKey(tParam->hContact);
	delete tParam;

	if( !ptr ) return 0;

	for(int i=0;i<DBGetContactSettingWord(0,szModuleName,"ket",10)*10; i++) {
		Sleep( 100 );
		if( ptr->waitForExchange != 1 ) break;
	} // for

#if defined(_DEBUG) || defined(NETLIB_LOG)
	Sent_NetLog("sttWaitForExchange: %d",ptr->waitForExchange);
#endif
   	// if keyexchange failed or timeout
   	if( ptr->waitForExchange==1 || ptr->waitForExchange==3 ) { // ����嫮 - ��ࠢ�塞 ������஢����, �᫨ ����
   		if( ptr->msgQueue && msgbox1(0,sim104,szModuleName,MB_YESNO|MB_ICONQUESTION)==IDYES ) {
	   		EnterCriticalSection(&localQueueMutex);
	   		ptr->sendQueue = true;
	   		pWM ptrMessage = ptr->msgQueue;
   			while( ptrMessage ) {
#if defined(_DEBUG) || defined(NETLIB_LOG)
				Sent_NetLog("Sent (unencrypted) message from queue: %s",ptrMessage->Message);
#endif
   				// send unencrypted messages
   				CallContactService(ptr->hContact,PSS_MESSAGE,(WPARAM)ptrMessage->wParam|PREF_METANODB,(LPARAM)ptrMessage->Message);
   				mir_free(ptrMessage->Message);
   				pWM tmp = ptrMessage;
   				ptrMessage = ptrMessage->nextMessage;
   				mir_free(tmp);
   			}
   			ptr->msgQueue = NULL;
	   		ptr->sendQueue = false;
	   		LeaveCriticalSection(&localQueueMutex);
   		}
		ptr->waitForExchange = 0;
   		ShowStatusIconNotify(ptr->hContact);
   	}
   	else
   	if( ptr->waitForExchange==2 ) { // ��᫠�� ��।� �१ ��⠭�������� ᮥ�������
		EnterCriticalSection(&localQueueMutex);
		// we need to resend last send back message with new crypto Key
		pWM ptrMessage = ptr->msgQueue;
		while (ptrMessage) {
#if defined(_DEBUG) || defined(NETLIB_LOG)
			Sent_NetLog("Sent (encrypted) message from queue: %s",ptrMessage->Message);
#endif
			// send unencrypted messages
			CallContactService(ptr->hContact,PSS_MESSAGE,(WPARAM)ptrMessage->wParam|PREF_METANODB,(LPARAM)ptrMessage->Message);
			mir_free(ptrMessage->Message);
			pWM tmp = ptrMessage;
			ptrMessage = ptrMessage->nextMessage;
			mir_free(tmp);
		}
		ptr->msgQueue = NULL;
		ptr->waitForExchange = 0;
		LeaveCriticalSection(&localQueueMutex);
   	}
   	else
   	if( ptr->waitForExchange==0 ) { // ������ ��।�
		EnterCriticalSection(&localQueueMutex);
		// we need to resend last send back message with new crypto Key
		pWM ptrMessage = ptr->msgQueue;
		while (ptrMessage) {
			mir_free(ptrMessage->Message);
			pWM tmp = ptrMessage;
			ptrMessage = ptrMessage->nextMessage;
			mir_free(tmp);
		}
		ptr->msgQueue = NULL;
		LeaveCriticalSection(&localQueueMutex);
   	}
   	return 0;
}


// set wait flag and run thread
void waitForExchange(pUinKey ptr, int flag) {
	switch( flag ) {
	case 0: // �����
	case 2: // ��᫠�� ��஢���
	case 3: // ��᫠�� ����஢���
		if( ptr->waitForExchange ) 
			ptr->waitForExchange = flag;
		break;
	case 1: // ��������
		if( ptr->waitForExchange ) 
			break;
		ptr->waitForExchange = 1;
		// ����᪠�� ���
		HANDLE hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
		unsigned int tID;
		CloseHandle( (HANDLE) _beginthreadex(NULL, 0, sttWaitForExchange, new TWaitForExchange(hEvent,ptr->hContact), 0, &tID) );
		SetEvent( hEvent );
		break;
	}
}


// EOF
