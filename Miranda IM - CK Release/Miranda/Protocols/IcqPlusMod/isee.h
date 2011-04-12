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
//BOOL ASD = FALSE;


#define hIconInst hInst

#define CLI_ADDTEMPVISIBLE          0x000A
#define CLI_REMOVETEMPVISIBLE       0x000B
void icq_InitISee();
void icq_ISeeCleanup();
void icq_InitPopUps();

void icq_GetUserStatus(HANDLE hContact, WORD wEvent);
void icq_SetUserStatus(DWORD dwUin, DWORD dwCookie, signed nStatus, HANDLE hContact);
void icq_CheckSpeed(WORD wStatus);
void icq_SetCustomStatus(HANDLE hContact, int xstatus);
//void handleXStatusCaps(HANDLE hContact, char* caps, int capsize);
BOOL PopUpErrMsg(char* aMsg);

INT_PTR icq_SelfRemove(WPARAM wParam,LPARAM lParam);
INT_PTR icq_GrantAuth(WPARAM wParam,LPARAM lParam);

extern BYTE gbASD;

INT_PTR icq_CheckAllUsersStatus(WPARAM wParam,LPARAM lParam);
INT_PTR IcqSetVis(WPARAM wparam,LPARAM lparam);
INT_PTR IcqSetInvis(WPARAM wparam,LPARAM lparam);
INT_PTR IcqGetUserStatus_service(WPARAM wParam, LPARAM lParam);
void SetWebAware(BYTE bSend);
void sendVisContactServ(DWORD dwUin, int mode);
void sendVisListServ(int mode);

extern BOOL bASDViaAwayMsg;
extern BOOL bASDViaXtraz;
extern BOOL bASDViaURL;
extern BOOL bASDUnauthorized; //work in progress
extern BOOL bASDViaAuth; //

