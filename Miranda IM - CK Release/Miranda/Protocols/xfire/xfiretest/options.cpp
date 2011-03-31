/*
 *  Plugin of miranda IM(ICQ) for Communicating with users of the XFire Network. 
 *
 *  Copyright (C) 2008 by
 *          dufte aka andreas h. <dufte@justmail.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 *  Based on J. Lawler              - BaseProtocol
 *			 Herbert Poul/Beat Wolf - xfirelib
 *
 *  Miranda ICQ: the free icq client for MS Windows 
 *  Copyright (C) 2000-2008  Richard Hughes, Roland Rabien & Tristan Van de Vreede
 *
 */

#include "stdafx.h"

#include "baseProtocol.h"

extern HANDLE XFireWorkingFolder;

//damit die änderungen sofort sichtbar sind
static BOOL CALLBACK DlgProcOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char inipath[XFIRE_MAX_STATIC_STRING_LEN]="";
	static BOOL inifound = FALSE;
	static BOOL dllfound = FALSE;

	switch (msg)
	{
		case WM_INITDIALOG:
		{
			DBVARIANT dbv;
			TranslateDialogDefault(hwndDlg);

			FoldersGetCustomPath( XFireWorkingFolder, inipath, 1024, 'W' );
			strcat(inipath,"\\");
			strcat(inipath,"xfire_games.ini");
			
			FILE * f = fopen(inipath,"r");
			if(f!=NULL)
			{
				fclose(f);
				CheckDlgButton(hwndDlg,IDC_CHKG,1);
				inifound = TRUE;
			}
			else
				inifound = FALSE;

			FoldersGetCustomPath( XFireWorkingFolder, inipath, 1024, 'W' );
			strcat(inipath,"\\");
			strcat(inipath,"icons.dll");
			
			f = fopen(inipath,"r");
			if(f!=NULL)
			{
				fclose(f);
				CheckDlgButton(hwndDlg,IDC_CHKI,1);
				dllfound = TRUE;
			}
			else
				dllfound = FALSE;

			if(!DBGetContactSetting(NULL,protocolname,"login",&dbv)) {
				SetDlgItemText(hwndDlg,IDC_LOGIN,dbv.pszVal);
				DBFreeVariant(&dbv);
			}
			if(!DBGetContactSetting(NULL,protocolname,"Nick",&dbv)) {
				SetDlgItemText(hwndDlg,IDC_NICK,dbv.pszVal);
				DBFreeVariant(&dbv);
			}
			if(!DBGetContactSetting(NULL,protocolname,"password",&dbv)) {
				//bit of a security hole here, since it's easy to extract a password from an edit box
				CallService(MS_DB_CRYPT_DECODESTRING,strlen(dbv.pszVal)+1,(LPARAM)dbv.pszVal);
				SetDlgItemText(hwndDlg,IDC_PASSWORD,dbv.pszVal);
				DBFreeVariant(&dbv);
			}

			CheckDlgButton(hwndDlg,IDC_NOAV,DBGetContactSettingByte(NULL,protocolname,"noavatars",0));
			CheckDlgButton(hwndDlg,IDC_NOMSG,DBGetContactSettingByte(NULL,protocolname,"nomsgbox",0));
			CheckDlgButton(hwndDlg,IDC_NOIPPORT,DBGetContactSettingByte(NULL,protocolname,"noipportinstatus",0));
			CheckDlgButton(hwndDlg,IDC_ONINAFK,DBGetContactSettingByte(NULL,protocolname,"oninsteadafk",0));
			CheckDlgButton(hwndDlg,IDC_NOCLANGROUP,DBGetContactSettingByte(NULL,protocolname,"noclangroups",0));
			CheckDlgButton(hwndDlg,IDC_SCANALWAYS,DBGetContactSettingByte(NULL,protocolname,"scanalways",0));

			//iconslots auswahl einfügen
			SendDlgItemMessage( hwndDlg, IDC_CBGICO, CB_ADDSTRING, 0, (LPARAM)TranslateT("Advanced #1 (ICQ X-Status)" ));
			SendDlgItemMessage( hwndDlg, IDC_CBGICO, CB_ADDSTRING, 0, (LPARAM)TranslateT("Advanced #2" ));
			SendDlgItemMessage( hwndDlg, IDC_CBGICO, CB_ADDSTRING, 0, (LPARAM)TranslateT("Advanced #3" ));
			SendDlgItemMessage( hwndDlg, IDC_CBGICO, CB_ADDSTRING, 0, (LPARAM)TranslateT("Advanced #4" ));
			SendDlgItemMessage( hwndDlg, IDC_CBVICO, CB_ADDSTRING, 0, (LPARAM)TranslateT("Advanced #1 (ICQ X-Status)" ));
			SendDlgItemMessage( hwndDlg, IDC_CBVICO, CB_ADDSTRING, 0, (LPARAM)TranslateT("Advanced #2" ));
			SendDlgItemMessage( hwndDlg, IDC_CBVICO, CB_ADDSTRING, 0, (LPARAM)TranslateT("Advanced #3" ));
			SendDlgItemMessage( hwndDlg, IDC_CBVICO, CB_ADDSTRING, 0, (LPARAM)TranslateT("Advanced #4" ));

			SendDlgItemMessage( hwndDlg, IDC_CBVICO, CB_SETCURSEL, DBGetContactSettingByte(NULL,protocolname,"voiceico",-1), 0);
			SendDlgItemMessage( hwndDlg, IDC_CBGICO, CB_SETCURSEL, DBGetContactSettingByte(NULL,protocolname,"gameico",-1), 0);

			return TRUE;
		}

		case WM_COMMAND:
			if(dllfound) CheckDlgButton(hwndDlg,IDC_CHKI,1);
			if(inifound) CheckDlgButton(hwndDlg,IDC_CHKG,1);

			if ((LOWORD(wParam) == IDC_LOGIN || LOWORD(wParam) == IDC_NICK || LOWORD(wParam) == IDC_PASSWORD) && (HIWORD(wParam) != EN_CHANGE || (HWND)lParam != GetFocus()) )
				return 0;
			SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			break;
		
		case WM_NOTIFY:
			switch (((LPNMHDR)lParam)->code)
			{
				case PSN_APPLY:
				{
					int reconnectRequired=0;
					char str[128];
					char login[128];
					int ccc;
					DBVARIANT dbv;

					GetDlgItemText(hwndDlg,IDC_LOGIN,login,sizeof(login));
					dbv.pszVal=NULL;
					if(DBGetContactSetting(NULL,protocolname,"login",&dbv) || strcmp(login,dbv.pszVal))
						reconnectRequired=1;
					if(dbv.pszVal!=NULL) DBFreeVariant(&dbv);
					DBWriteContactSettingString(NULL,protocolname,"login",login);

					GetDlgItemText(hwndDlg,IDC_NICK,login,sizeof(login));
					dbv.pszVal=NULL;
					if(DBGetContactSetting(NULL,protocolname,"Nick",&dbv) || strcmp(login,dbv.pszVal))
						reconnectRequired=1;
					if(dbv.pszVal!=NULL) DBFreeVariant(&dbv);
					DBWriteContactSettingString(NULL,protocolname,"Nick",login);

					GetDlgItemText(hwndDlg,IDC_PASSWORD,str,sizeof(str));
					CallService(MS_DB_CRYPT_ENCODESTRING,sizeof(str),(LPARAM)str);
					dbv.pszVal=NULL;
					if(DBGetContactSetting(NULL,protocolname,"password",&dbv) || strcmp(str,dbv.pszVal))
						reconnectRequired=1;
					if(dbv.pszVal!=NULL) DBFreeVariant(&dbv);
					DBWriteContactSettingString(NULL,protocolname,"password",str);
					GetDlgItemText(hwndDlg,IDC_SERVER,str,sizeof(str));

					//DBWriteContactSettingByte(NULL,protocolname,"keepalive",(BYTE)IsDlgButtonChecked(hwndDlg,IDC_KEEPALIVE));
					DBWriteContactSettingByte(NULL,protocolname,"noavatars",(BYTE)IsDlgButtonChecked(hwndDlg,IDC_NOAV));
					DBWriteContactSettingByte(NULL,protocolname,"nomsgbox",(BYTE)IsDlgButtonChecked(hwndDlg,IDC_NOMSG));
					DBWriteContactSettingByte(NULL,protocolname,"noipportinstatus",(BYTE)IsDlgButtonChecked(hwndDlg,IDC_NOIPPORT));
					DBWriteContactSettingByte(NULL,protocolname,"oninsteadafk",(BYTE)IsDlgButtonChecked(hwndDlg,IDC_ONINAFK));
					DBWriteContactSettingByte(NULL,protocolname,"noclangroups",(BYTE)IsDlgButtonChecked(hwndDlg,IDC_NOCLANGROUP));
					DBWriteContactSettingByte(NULL,protocolname,"scanalways",(BYTE)IsDlgButtonChecked(hwndDlg,IDC_SCANALWAYS));

					ccc=SendDlgItemMessage(hwndDlg, IDC_CBVICO, CB_GETCURSEL, 0, 0);
					DBWriteContactSettingByte(NULL,protocolname,"voiceico",(BYTE)ccc);
					ccc=SendDlgItemMessage(hwndDlg, IDC_CBGICO, CB_GETCURSEL, 0, 0);
					DBWriteContactSettingByte(NULL,protocolname,"gameico",(BYTE)ccc);

					if(reconnectRequired) MessageBox(hwndDlg,Translate("The changes you have made require you to reconnect to the XFire network before they take effect"),Translate("XFire Options"),MB_OK);
					return TRUE;
				}

			}
			break;
	}
	return FALSE;
}

int OptInit(WPARAM wParam,LPARAM lParam)
{
	OPTIONSDIALOGPAGE odp;

	ZeroMemory(&odp,sizeof(odp));
	odp.cbSize=sizeof(odp);
	odp.position=-790000000;
	odp.hInstance=hinstance;
	odp.pszTemplate=MAKEINTRESOURCE(IDD_OPT);
	odp.pszTitle=Translate("XFire");
	odp.pszGroup = Translate("Network");
	odp.flags=ODPF_BOLDGROUPS;
	odp.nIDBottomSimpleControl=IDC_GROUPMAIN;
	odp.pfnDlgProc=DlgProcOpts;
	CallService(MS_OPT_ADDPAGE,wParam,(LPARAM)&odp);
	return 0;
}