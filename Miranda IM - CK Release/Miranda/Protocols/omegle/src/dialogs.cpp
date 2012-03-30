/*

Omegle plugin for Miranda Instant Messenger
_____________________________________________

Copyright © 2011-12 Robert Pösel

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "common.h"

static BOOL LoadDBCheckState(OmegleProto* ppro, HWND hwnd, int idCtrl, const char* szSetting, BYTE bDef)
{
	BOOL state = DBGetContactSettingByte(NULL, ppro->m_szModuleName, szSetting, bDef);
	CheckDlgButton(hwnd, idCtrl, state);
	return state;
}

static BOOL StoreDBCheckState(OmegleProto* ppro, HWND hwnd, int idCtrl, const char* szSetting)
{
	BOOL state = IsDlgButtonChecked(hwnd, idCtrl);
	DBWriteContactSettingByte(NULL, ppro->m_szModuleName, szSetting, (BYTE)state);
	return state;
}

INT_PTR CALLBACK OmegleAccountProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{
/*	OmegleProto *proto;

	switch ( message )
	{

	case WM_INITDIALOG:
		TranslateDialogDefault(hwnd);

		proto = reinterpret_cast<OmegleProto*>(lparam);
		SetWindowLong(hwnd,GWLP_USERDATA,lparam);

		DBVARIANT dbv;
		if( !DBGetContactSettingString(0,proto->ModuleName(),OMEGLE_KEY_LOGIN,&dbv) )
		{
			SetDlgItemTextA(hwnd,IDC_UN,dbv.pszVal);
			DBFreeVariant(&dbv);
		}

		if ( !DBGetContactSettingString(0,proto->ModuleName(),OMEGLE_KEY_PASS,&dbv) )
		{
			CallService(MS_DB_CRYPT_DECODESTRING,strlen(dbv.pszVal)+1,
				reinterpret_cast<LPARAM>(dbv.pszVal));
			SetDlgItemTextA(hwnd,IDC_PW,dbv.pszVal);
			DBFreeVariant(&dbv);
		}

		if (!proto->isOffline()) {
			SendMessage(GetDlgItem(hwnd,IDC_UN),EM_SETREADONLY,1,0);
			SendMessage(GetDlgItem(hwnd,IDC_PW),EM_SETREADONLY,1,0); }

		return TRUE;

	case WM_COMMAND:
		if ( LOWORD( wparam ) == IDC_NEWACCOUNTLINK )
		{
			CallService(MS_UTILS_OPENURL,1,reinterpret_cast<LPARAM>
				( OMEGLE_URL_HOMEPAGE ) );
			return TRUE;
		}

		if ( HIWORD( wparam ) == EN_CHANGE && reinterpret_cast<HWND>(lparam) == GetFocus() )
		{
			switch(LOWORD(wparam))
			{
			case IDC_UN:
			case IDC_PW:
				SendMessage(GetParent(hwnd),PSM_CHANGED,0,0);
			}
		}
		break;

	case WM_NOTIFY:
		if ( reinterpret_cast<NMHDR*>(lparam)->code == PSN_APPLY )
		{
			proto = reinterpret_cast<OmegleProto*>(GetWindowLong(hwnd,GWLP_USERDATA));
			char str[128];

			GetDlgItemTextA(hwnd,IDC_UN,str,sizeof(str));
			DBWriteContactSettingString(0,proto->ModuleName(),OMEGLE_KEY_LOGIN,str);

			GetDlgItemTextA(hwnd,IDC_PW,str,sizeof(str));
			CallService(MS_DB_CRYPT_ENCODESTRING,sizeof(str),reinterpret_cast<LPARAM>(str));
			DBWriteContactSettingString(0,proto->ModuleName(),OMEGLE_KEY_PASS,str);

			return TRUE;
		}
		break;

	}
	*/
	return FALSE;
}

INT_PTR CALLBACK OmegleOptionsProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{
/*	OmegleProto *proto = reinterpret_cast<OmegleProto*>(GetWindowLong(hwnd,GWLP_USERDATA));

	switch ( message )
	{

	case WM_INITDIALOG:
	{
		TranslateDialogDefault(hwnd);

		proto = reinterpret_cast<OmegleProto*>(lparam);
		SetWindowLong(hwnd,GWLP_USERDATA,lparam);

		DBVARIANT dbv;
		if( !DBGetContactSettingString(0,proto->ModuleName(),OMEGLE_KEY_LOGIN,&dbv) )
		{
			SetDlgItemTextA(hwnd,IDC_UN,dbv.pszVal);
			DBFreeVariant(&dbv);
		}

		if ( !DBGetContactSettingString(0,proto->ModuleName(),OMEGLE_KEY_PASS,&dbv) )
		{
			CallService(MS_DB_CRYPT_DECODESTRING,strlen(dbv.pszVal)+1,reinterpret_cast<LPARAM>(dbv.pszVal));
			SetDlgItemTextA(hwnd,IDC_PW,dbv.pszVal);
			DBFreeVariant(&dbv);
		}

		if (!proto->isOffline())
	    {
			SendMessage(GetDlgItem(hwnd,IDC_UN),EM_SETREADONLY,TRUE,0);
			SendMessage(GetDlgItem(hwnd,IDC_PW),EM_SETREADONLY,TRUE,0);
		}

		SendDlgItemMessage(hwnd, IDC_GROUP, EM_LIMITTEXT, OMEGLE_GROUP_NAME_LIMIT, 0);

		if( !DBGetContactSettingTString(0,proto->ModuleName(),OMEGLE_KEY_DEF_GROUP,&dbv) )
		{
			SetDlgItemText(hwnd,IDC_GROUP,dbv.ptszVal);
			DBFreeVariant(&dbv);
		}

		LoadDBCheckState(proto, hwnd, IDC_SET_IGNORE_STATUS, OMEGLE_KEY_DISABLE_STATUS_NOTIFY, DEFAULT_DISABLE_STATUS_NOTIFY);
		LoadDBCheckState(proto, hwnd, IDC_BIGGER_AVATARS, OMEGLE_KEY_BIG_AVATARS, DEFAULT_BIG_AVATARS);
		LoadDBCheckState(proto, hwnd, IDC_LOAD_MOBILE, OMEGLE_KEY_LOAD_MOBILE, DEFAULT_LOAD_MOBILE);

	} return TRUE;

	case WM_COMMAND: {
		if ( LOWORD( wparam ) == IDC_NEWACCOUNTLINK )
		{
			CallService(MS_UTILS_OPENURL,1,reinterpret_cast<LPARAM>
				( OMEGLE_URL_HOMEPAGE ) );
			return TRUE;
		}

		if ( LOWORD( wparam ) == IDC_SECURE ) {			
			EnableWindow(GetDlgItem(hwnd, IDC_SECURE_CHANNEL), IsDlgButtonChecked(hwnd, IDC_SECURE));
		}		
		
		if ((LOWORD(wparam)==IDC_UN || LOWORD(wparam)==IDC_PW || LOWORD(wparam)==IDC_GROUP) &&
		    (HIWORD(wparam)!=EN_CHANGE || (HWND)lparam!=GetFocus()))
			return 0;
		else
			SendMessage(GetParent(hwnd),PSM_CHANGED,0,0);

	} break;

	case WM_NOTIFY:
		if ( reinterpret_cast<NMHDR*>(lparam)->code == PSN_APPLY )
		{
			char str[128]; TCHAR tstr[128];

			GetDlgItemTextA(hwnd,IDC_UN,str,sizeof(str));
			DBWriteContactSettingString(0,proto->ModuleName(),OMEGLE_KEY_LOGIN,str);

			GetDlgItemTextA(hwnd,IDC_PW,str,sizeof(str));
			CallService(MS_DB_CRYPT_ENCODESTRING,sizeof(str),reinterpret_cast<LPARAM>(str));
			DBWriteContactSettingString(NULL,proto->m_szModuleName,OMEGLE_KEY_PASS,str);

			GetDlgItemText(hwnd,IDC_GROUP,tstr,sizeof(tstr));
			if ( lstrlen( tstr ) > 0 )
			{
				DBWriteContactSettingTString(NULL,proto->m_szModuleName,OMEGLE_KEY_DEF_GROUP,tstr);
				CallService( MS_CLIST_GROUPCREATE, 0, (LPARAM)tstr );
			}
			else
				DBDeleteContactSetting(NULL,proto->m_szModuleName,OMEGLE_KEY_DEF_GROUP);

			StoreDBCheckState(proto, hwnd, IDC_SET_IGNORE_STATUS, OMEGLE_KEY_DISABLE_STATUS_NOTIFY);
			StoreDBCheckState(proto, hwnd, IDC_BIGGER_AVATARS, OMEGLE_KEY_BIG_AVATARS);
			StoreDBCheckState(proto, hwnd, IDC_LOAD_MOBILE, OMEGLE_KEY_LOAD_MOBILE);
			
			return TRUE;
		}
		break;

	}
	*/
	return FALSE;
}
