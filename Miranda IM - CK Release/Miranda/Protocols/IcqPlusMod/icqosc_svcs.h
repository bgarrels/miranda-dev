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
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#ifndef __ICQOSC_SVCS_H
#define __ICQOSC_SVCS_H

#define ICQ_DB_GETEVENTTEXT_MISSEDMESSAGE "ICQ/GetEventTextMissedMessage"

INT_PTR icq_getEventTextMissedMessage(WPARAM wParam, LPARAM lParam);

/*---------* Functions *---------------*/

INT_PTR IcqGetName(WPARAM wParam, LPARAM lParam);
INT_PTR IcqLoadIcon(WPARAM wParam, LPARAM lParam);
INT_PTR IcqGetStatus(WPARAM wParam, LPARAM lParam);
INT_PTR IcqSetNickName(WPARAM wParam, LPARAM lParam);
INT_PTR IcqChangeInfoEx(WPARAM wParam, LPARAM lParam);
INT_PTR IcqSendSms(WPARAM wParam, LPARAM lParam);
INT_PTR IcqSendFile(WPARAM wParam, LPARAM lParam);
INT_PTR IcqFileAllow(WPARAM wParam, LPARAM lParam);
INT_PTR IcqFileDeny(WPARAM wParam, LPARAM lParam);
INT_PTR IcqFileCancel(WPARAM wParam, LPARAM lParam);
INT_PTR IcqFileResume(WPARAM wParam, LPARAM lParam);
INT_PTR IcqGrantAuthorization(WPARAM wParam, LPARAM lParam);
INT_PTR IcqRevokeAuthorization(WPARAM wParam, LPARAM lParam);
INT_PTR IcqSendtZer(WPARAM,LPARAM);
INT_PTR IcqTzerDlg(WPARAM,LPARAM);
INT_PTR IcqSendYouWereAdded(WPARAM,LPARAM);
INT_PTR IcqAddServerContact(WPARAM wParam, LPARAM lParam);
INT_PTR SvcCreateAccMgrUI(WPARAM wParam, LPARAM lParam);

INT_PTR IcqRecvFile(WPARAM wParam, LPARAM lParam);

int IcqIdleChanged(WPARAM wParam, LPARAM lParam);

INT_PTR IcqGetAvatarInfo(WPARAM wParam, LPARAM lParam);
INT_PTR IcqGetAvatarCaps(WPARAM wParam, LPARAM lParam);
INT_PTR IcqGetMyAvatar(WPARAM wParam, LPARAM lParam);
INT_PTR IcqSetMyAvatar(WPARAM wParam, LPARAM lParam);

INT_PTR IcqSetPassword(WPARAM wParam, LPARAM lParam);

INT_PTR IcqAddCapability(WPARAM wParam, LPARAM lParam);
INT_PTR IcqCheckCapability(WPARAM wParam, LPARAM lParam);
INT_PTR IcqServerIgnore(WPARAM wParam, LPARAM lParam);

extern int iIcqNewStatus;

// FIXME: Move to another place //////////////////////////////////////////
extern DWORD ReportGenericSendError(HANDLE hContact, int nType, const char* szErrorMsg);
extern cookie_message_data* CreateMessageCookieData(BYTE bMsgType, HANDLE hContact, DWORD dwUin, int bUseSrvRelay);
extern void ICQAddRecvEvent(HANDLE hContact, WORD wType, PROTORECVEVENT* pre, DWORD cbBlob, PBYTE pBlob, DWORD flags);
extern HANDLE HContactFromAuthEvent(HANDLE hEvent);
extern HANDLE AddToListByUIN(DWORD dwUin, DWORD dwFlags);
extern HANDLE AddToListByUID(char *szUID, DWORD dwFlags);


#endif /* __ICQOSC_SVCS_H */
