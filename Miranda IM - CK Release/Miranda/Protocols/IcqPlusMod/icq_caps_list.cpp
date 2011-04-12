// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright � 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright � 2001,2002 Jon Keating, Richard Hughes
// Copyright � 2002,2003,2004 Martin �berg, Sam Kothari, Robert Rainwater
// Copyright � 2004,2005,2006,2007 Joe Kucera
// Copyright � 2006,2007,2008 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie, jarvis
// Copyright � 2008 [sss], chaos.persei, nullbie, baloo, jarvis
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
// Last change by : $Author: jarvis141 $
//
//
// DESCRIPTION:
//
//  Capabilities List GUI
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"

#ifndef LVS_EX_FULLROWSELECT
#define LVS_EX_FULLROWSELECT    0x00000020
#endif

static HWND hwndCapsList = NULL;
extern HWND hCaller;
extern BOOL id;
WORD OldCapsCount = 0;
static INT_PTR CALLBACK DlgProcCapsList( HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    char DBModule[64], buf2[64];
    TCHAR tmp[64], buf1[64];
    WORD CapsCount = 0;
    int iIndex, i, iRow;
    LVCOLUMN col;
    LVITEM item;
    HWND hwndList = GetDlgItem( hwndDlg, IDC_CAPABILITIES );
    NMLISTVIEW * hdr = ( NMLISTVIEW * )lParam;
    mir_snprintf( DBModule, 64, "%sCaps", ICQ_PROTOCOL_NAME );
    buf2[0]=0;
    switch ( message )
    {
    case WM_INITDIALOG:
    {
        id = FALSE;
        ICQTranslateDialog( hwndDlg );
        col.pszText = TranslateW( _T("Capability") );
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.fmt = LVCFMT_LEFT;
        col.cx = 200;
        ListView_InsertColumn( hwndList, 0, &col );
        ListView_SetExtendedListViewStyleEx( hwndList, 0, LVS_EX_FULLROWSELECT );

        for ( i = 1; ; i++ )
        {
//        DBVARIANT dbv = { 0 };
//        strcpy( dbv.pszVal, "" );
            mir_snprintf( buf2, 64, "cap%luname", i );
            UniGetContactSettingUtf(NULL, DBModule, buf2, 0);
//        DBGetContactSettingString( NULL, DBModule, buf2, &dbv );
            if ( buf2 ) break;
            else
            {
                TCHAR *tmp = mir_a2t(buf2);
                item.mask = LVIF_TEXT;
                item.iItem = i;
                item.iSubItem = 0;
                item.pszText = tmp;
                iRow = ListView_InsertItem( hwndList, &item );
//          strcpy( buf2, dbv.pszVal );
                ListView_SetItemText( hwndList, iRow, 0, tmp );
//		  mir_freetmp);
            }
        }
        OldCapsCount = ListView_GetItemCount( hwndList );
    }
    return TRUE;
    case WM_COMMAND:
    {
        ZeroMemory( &item, sizeof( LVITEM ) );
        switch ( LOWORD( wParam ) )
        {
        case IDC_CAPL_ADD:
            CapsCount = ListView_GetItemCount( hwndList );
            GetDlgItemText( hwndDlg, IDC_CAPL_EDIT, tmp, 64 );
            if( buf2[0] == 0 )
                break;
            item.mask = LVIF_TEXT;
            item.iItem = CapsCount;
            item.iSubItem = 0;
            item.pszText = NULL;
            iRow = ListView_InsertItem( hwndList, &item );
            ListView_SetItemText( hwndList, iRow, 0, tmp );
            break;
        case IDC_CAPL_MODIFY:
            iIndex = ListView_GetSelectionMark( hwndList );
            if ( iIndex < 0 ) return FALSE;
            GetDlgItemText( hwndDlg, IDC_CAPL_EDIT, tmp , 64 );
            ListView_SetItemText( hwndList, iIndex, 0, tmp );
            break;
        case IDC_CAPL_DELETE:
            iIndex = ListView_GetSelectionMark( hwndList );
            if( iIndex < 0 ) return FALSE;
            ListView_DeleteItem( hwndList, iIndex );
            break;
        case IDOK:
        case IDCANCEL:
            SendMessage( hCaller, WM_INITDIALOG, 0, 0 );
            DestroyWindow( hwndDlg );
            break;
        }
    }
    break;
    case WM_NOTIFY:
        if ( hdr && hdr->hdr.code == LVN_ITEMCHANGED && IsWindowVisible( hdr->hdr.hwndFrom ) && hdr->iItem != ( -1 ) )
        {
            iIndex = hdr->iItem;
            if( iIndex < 0 ) return FALSE;
            ListView_GetItemText( hwndList, iIndex, 0, tmp, 64 );
            SetDlgItemText( hwndDlg, IDC_CAPL_EDIT, tmp );
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow( hwndDlg );
        break;
    case WM_DESTROY:
    {
        int j;
        hwndCapsList = NULL;
        CapsCount = ListView_GetItemCount( hwndList );
        for ( iRow = 0; iRow < CapsCount; iRow++ )
        {
            char *tmp;
            mir_snprintf( buf2, 64, "cap%luname", iRow + 1 );
            ListView_GetItemText( hwndList, iRow, 0, buf1, 64 );
            tmp = mir_t2a(buf1);
            DBWriteContactSettingString( NULL, DBModule, buf2, tmp );
            id = TRUE;
//		mir_freetmp);
        }
        for ( j = CapsCount + 1; j <= OldCapsCount; j++ )
        {
            mir_snprintf( buf2, 64, "cap%luname", j );
            DBDeleteContactSetting( NULL, DBModule, buf2 );
            id = TRUE;
        }
    }
    break;
    }
    return FALSE;
}

void ShowCapsListDialog( HWND hwndCaller )
{
    hCaller = hwndCaller;
    if ( hwndCapsList == NULL )
    {
        hwndCapsList = CreateDialogUtf( hInst, MAKEINTRESOURCEA( IDD_ICQ_CAPS_LIST ), NULL, DlgProcCapsList );
    }
    SetForegroundWindow( hwndCapsList );
}

