/*
Jabber Protocol plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright (C) 2002-2004		Santithorn Bunchua
			Copyright (C) 2007			Maxim Mluhov, Victor Pavlychko, Artem Shpynov, Michael Stepura
			Copyright (C) 2008-09		Dmitriy Chervov
			Copyright (C) 2005-2012		George Hazan, Borkra

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

#include "../jabber.h"
#include "jabber_message_manager.h"

BOOL CJabberProto::OnMessageError( HXML node, ThreadData *pThreadData, CJabberMessageInfo* pInfo )
{
	// we check if is message delivery failure
	int id = JabberGetPacketID( node );
	JABBER_LIST_ITEM* item = ListGetItemPtr( LIST_ROSTER, pInfo->GetFrom() );
	if ( item == NULL )
		item = ListGetItemPtr( LIST_CHATROOM, pInfo->GetFrom() );
	if ( item != NULL ) { // yes, it is
		TCHAR *szErrText = JabberErrorMsg( pInfo->GetChildNode() );
		if ( id != -1 ) {
			char *errText = mir_t2a(szErrText);
			JSendBroadcast( pInfo->GetHContact(), ACKTYPE_MESSAGE, ACKRESULT_FAILED, ( HANDLE ) id, (LPARAM)errText );
			mir_free(errText);
		} else {
			TCHAR buf[512];
			HXML bodyNode = xmlGetChild( node, "body" );
			if (bodyNode)
				mir_sntprintf( buf, SIZEOF( buf ), _T( "%s:\n%s\n%s" ), pInfo->GetFrom(), xmlGetText( bodyNode ), szErrText );
			else
				mir_sntprintf( buf, SIZEOF( buf ), _T( "%s:\n%s" ), pInfo->GetFrom(), szErrText );

			 MsgPopup( NULL, buf, TranslateT( "Jabber Error" ));
		}
		mir_free(szErrText);
	}
	return TRUE;
}

BOOL CJabberProto::OnMessageIbb( HXML node, ThreadData *pThreadData, CJabberMessageInfo* pInfo )
{
	BOOL bOk = FALSE;
	const TCHAR *sid = xmlGetAttrValue( pInfo->GetChildNode(), _T("sid"));
	const TCHAR *seq = xmlGetAttrValue( pInfo->GetChildNode(), _T("seq"));
	if ( sid && seq && xmlGetText( pInfo->GetChildNode() ) ) {
		bOk = OnIbbRecvdData( xmlGetText( pInfo->GetChildNode() ), sid, seq );
	}
	return TRUE;
}

BOOL CJabberProto::OnMessagePubsubEvent( HXML node, ThreadData *pThreadData, CJabberMessageInfo* pInfo )
{
	OnProcessPubsubEvent( node );
	return TRUE;
}

BOOL CJabberProto::OnMessageGroupchat( HXML node, ThreadData *pThreadData, CJabberMessageInfo* pInfo )
{
	JABBER_LIST_ITEM *chatItem = ListGetItemPtr( LIST_CHATROOM, pInfo->GetFrom() );
	if ( chatItem )
	{	// process GC message
		GroupchatProcessMessage( node );
	} else
	{	// got message from unknown conference... let's leave it :)
//			TCHAR *conference = NEWTSTR_ALLOCA(from);
//			if (TCHAR *s = _tcschr(conference, _T('/'))) *s = 0;
//			XmlNode p( "presence" ); xmlAddAttr( p, "to", conference ); xmlAddAttr( p, "type", "unavailable" );
//			info->send( p );
	}
	return TRUE;
}
