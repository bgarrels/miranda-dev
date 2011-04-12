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
//  Registers new uin
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"

void icq_registerNewUin(char* password, char* image)
{
    strcpy(icq_regpassword, password);
    icq_regNewUin = 2;
    registerUin(password, image);
}

void icq_requestRegImage(HWND hwndDlg)
{
    icq_serverDisconnect(0);
    iIcqNewStatus = ID_STATUS_OFFLINE;
    hwndRegImageDialog = hwndDlg;
    bImageRequested = 1;
    SetDlgItemText(hwndRegImageDialog, IDC_NEWUININFO, TranslateT("Connecting to server..."));
    icq_login(NULL);
    icq_regNewUin = 1;
}


void InitReguin()
{
    //0 - default icq working, 1 - get image, 2 - request image
    icq_regNewUin = 0;
    bImageRequested = 0;
}

char icq_regpassword[10];
HWND hwndRegImageDialog;
int icq_regNewUin;
