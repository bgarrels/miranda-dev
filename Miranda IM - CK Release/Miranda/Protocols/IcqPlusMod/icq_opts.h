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
// Revision       : $Revision: 37 $
// Last change on : $Date: 2007-08-07 04:37:56 +0300 (Ð’Ñ‚, 07 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Describe me here please...
//
// -----------------------------------------------------------------------------



int IcqOptInit(WPARAM, LPARAM);
void ShowRegUinDialog(void);
//this must be in other place ...
extern BOOL bPSD;
extern BOOL bNoASDInInvisible;
extern BOOL bASDForOffline;
extern BOOL bNoPSDForHidden;
extern BOOL bLogSelfRemoveFile;
extern BOOL bLogIgnoreCheckFile;
extern BOOL bLogStatusCheckFile;
extern BOOL bLogASDFile;
extern BOOL bLogClientChangeFile;
extern BOOL bLogAuthFile;
extern BOOL bLogInfoRequestFile;
extern BOOL bLogReadXStatusFile;
extern BOOL bLogSelfRemoveHistory;
extern BOOL bLogIgnoreCheckHistory;
extern BOOL bLogStatusCheckHistory;
extern BOOL bLogASDHistory;
extern BOOL bLogClientChangeHistory;
extern BOOL bLogAuthHistory;
extern BOOL bLogInfoRequestHistory;
extern BOOL bLogReadXStatusHistory;
extern BOOL bTmpContacts;
extern BOOL bTmpAuthRequet;
extern BOOL bTmpSendAdded;
extern char* TmpGroupName;
extern BOOL bAddTemp;
extern BOOL bNoStatusReply;
extern BOOL bServerAutoChange;
extern BOOL bIncognitoGlobal;
extern BOOL bPrivacyMenuPlacement;
extern BOOL bQipstatusIconShow;
extern BOOL bAuthIgnore;
extern BOOL m_bSecureConnection;
extern BYTE m_bConnectionLost;
//BOOL bStealthRequest;