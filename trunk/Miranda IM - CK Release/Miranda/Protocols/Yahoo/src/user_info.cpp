/*
Yahoo protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Gennady Feldman (aka Gena01) 
			Laurent Marechal (aka Peorth)

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

#include "../yahoo.h"

#include <m_options.h>
#include <m_skin.h>
#include <m_userinfo.h>

#include "resource.h"

#include <commctrl.h>

static INT_PTR CALLBACK YahooUserInfoDlgProc( HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg ) {
	case WM_INITDIALOG:
		// lParam is hContact
		TranslateDialogDefault( hwndDlg );

		//SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadSkinnedIcon(SKINICON_OTHER_USERDETAILS));

		
		break;

	case WM_NOTIFY:
		if (( ( LPNMHDR )lParam )->idFrom == 0 ) {
			switch (( ( LPNMHDR )lParam )->code ) {
			case PSN_PARAMCHANGED:
				SetWindowLongPtr(hwndDlg, GWLP_USERDATA, (( PSHNOTIFY* )lParam )->lParam );
				break;
			case PSN_INFOCHANGED:
				{
					CYahooProto* ppro = (CYahooProto*)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);

					if (!ppro)
						break;

					char* szProto;
					HANDLE hContact = (HANDLE)((LPPSHNOTIFY)lParam)->lParam;

					if (hContact == NULL) {
						szProto = ppro->m_szModuleName;
					} else {
						szProto = (char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
					}

					if (!szProto)
						break;

					if (hContact) {
						DBVARIANT dbv;
						char z[128];
						
						if (ppro->GetString( hContact,  YAHOO_LOGINID, &dbv ) == 0) {
							strcpy(z, dbv.pszVal);
							DBFreeVariant( &dbv );
						} else {
							strcpy(z, "???");
						}
						
						SetDlgItemTextA(hwndDlg, IDC_SEARCH_ID, z);
						
						if (ppro->GetString( hContact,  "Transport", &dbv ) == 0) {
							strcpy(z, dbv.pszVal);
							DBFreeVariant( &dbv );
						} else {
							strcpy(z, "Yahoo");
						}
						
						SetDlgItemTextA(hwndDlg, IDC_SEARCH_PROTOCOL, z);
						
						if (ppro->GetString( hContact,  "MirVer", &dbv ) == 0) {
							strcpy(z, dbv.pszVal);
							DBFreeVariant( &dbv );
						} else {
							strcpy(z, "???");
						}
						
						SetDlgItemTextA(hwndDlg, IDC_NFO_CLIENT, z);
						
					} else {
					}
				}
				break;
			}	
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwndDlg);
		break;

	case WM_DESTROY:
		
		break;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// OnInfoInit - initializes user info option dialogs

INT_PTR __cdecl CYahooProto::OnUserInfoInit( WPARAM wParam, LPARAM lParam )
{
	//if ( !JCallService( MS_PROTO_ISPROTOCOLLOADED, 0, ( LPARAM )m_szModuleName ))
	//	return 0;

	OPTIONSDIALOGPAGE odp = {0};
	odp.cbSize = sizeof( odp );
	odp.hInstance = hInstance;
	odp.dwInitParam = ( LPARAM )this;

	HANDLE hContact = ( HANDLE )lParam;
	if ( hContact )
	{
		char* szProto = ( char* )CallService( MS_PROTO_GETCONTACTBASEPROTO, ( WPARAM ) hContact, 0 );
		if ( szProto != NULL && !strcmp( szProto, m_szModuleName ))
		{
			odp.pfnDlgProc = YahooUserInfoDlgProc;
			odp.position = -1900000000;
			odp.pszTemplate = MAKEINTRESOURCEA( IDD_USER_INFO );
			odp.pszTitle = m_szModuleName;
			CallService( MS_USERINFO_ADDPAGE, wParam, ( LPARAM )&odp );

		}
	} 

	return 0;
}

