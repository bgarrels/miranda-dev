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
// Revision       : $Revision: 50 $
// Last change on : $Date: 2007-08-28 02:57:00 +0300 (Ð’Ñ‚, 28 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Headers for PopUp Plugin support
//
// -----------------------------------------------------------------------------


extern BOOL bPopUpService;
extern BOOL bPopUpsEnabled;
extern BOOL bSpamPopUp;
extern BOOL bUnknownPopUp;
extern BOOL bFoundPopUp;
extern BOOL bScanPopUp;
extern BOOL bClientChangePopUp;
extern BOOL bCloseWindowPopUp;
extern BOOL bVisPopUp;
extern BOOL bPopUpForNotOnList;
extern BOOL bIgnoreCheckPop;
extern BOOL bPopSelfRem;
extern BOOL bInfoRequestPopUp;
extern BOOL bAuthPopUp;
extern BOOL bUinPopup;
extern BOOL bReadXStatusPopUp;
extern BOOL bPopupsForHidden;
extern BOOL bPopupsForIgnored;

extern HANDLE hPopUpsList;



#ifndef __ICQ_POPUPS_H
#define __ICQ_POPUPS_H


#define POPTYPE_SPAM    254  // this is for spambots
#define POPTYPE_UNKNOWN 253
#define POPTYPE_VIS	252
#define POPTYPE_INFO_REQUEST 251
#define POPTYPE_CLIENT_CHANGE 250
#define POPTYPE_IGNORE_CHECK 249
#define POPTYPE_SELFREMOVE 248
#define POPTYPE_AUTH 247
#define POPTYPE_XUPDATER 246


#define POPTYPE_FOUND	   5
#define POPTYPE_SCAN       6



//void InitPopUps();
void InitPopupOpts(WPARAM wParam);

int ShowPopUpMsg(HANDLE hContact, DWORD dwUin, const char* szTitle, const char* szMsg, BYTE bType);


#endif /* __ICQ_POPUPS_H */
