// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright � 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright � 2001,2002 Jon Keating, Richard Hughes
// Copyright � 2002,2003,2004 Martin �berg, Sam Kothari, Robert Rainwater
// Copyright � 2004,2005,2006,2007 Joe Kucera
// Copyright � 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
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
// Last change on : $Date: 2007-08-05 03:45:10 +0300 (Вс, 05 авг 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  ChangeInfo Plugin stuff
//
// -----------------------------------------------------------------------------

#include "../icqoscar.h"


int InitChangeDetails(WPARAM wParam, LPARAM lParam)
{
    static char szTmp[256];
    OPTIONSDIALOGPAGE odp = {0};

    if (lParam) return 0;

    ZeroMemory(&odp,sizeof(odp));
    odp.cbSize = sizeof(odp);
    mir_snprintf( szTmp, sizeof( szTmp ), Translate("%s Details"), ICQ_PROTOCOL_NAME);
    odp.hIcon = NULL;
    odp.hInstance = hInst;
    odp.position = -1899999999;
    odp.pszTitle=szTmp;
    odp.pszTemplate = MAKEINTRESOURCEA(IDD_INFO_CHANGEINFO);
    odp.pfnDlgProc = ChangeInfoDlgProc;

    CallService(MS_USERINFO_ADDPAGE,wParam,(LPARAM)&odp);

    return 0;
}
