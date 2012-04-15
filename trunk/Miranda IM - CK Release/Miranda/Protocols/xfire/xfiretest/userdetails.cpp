/*
Plugin of Miranda IM for Communicating with users of the XFire Network. 
 
Copyright (C) 2008-2012 by
           dufte aka andreas h. <dufte@justmail.de>
 

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 
Based on	J. Lawler              - BaseProtocol
			Herbert Poul/Beat Wolf - xfirelib

Miranda : the free icq client for MS Windows 
Copyright (C) 2000-2012  Richard Hughes, Roland Rabien & Tristan Van de Vreede
*/

#include "stdafx.h"

#include "baseProtocol.h"

HWND ghwndDlg;
extern HANDLE XFireWorkingFolder;

void SetItemTxt(HWND hwndDlg,int feldid,char*feld,HANDLE hcontact,char*stdfeld,int type)
{
	DBVARIANT dbv;
	if(!DBGetContactSetting(hcontact,protocolname,feld,&dbv)) {
		if(type==1)
		{
			char temp[255];
			sprintf(temp,"%i",dbv.wVal);
			SetDlgItemText(hwndDlg,feldid,temp);
		}
		else
		{
			SetDlgItemText(hwndDlg,feldid,dbv.pszVal);
		}
		DBFreeVariant(&dbv);
	}
	else
	{
		SetDlgItemText(hwndDlg,feldid,stdfeld);
	}
}

static void DLProfilePic(LPVOID lpv)
{
	char address[XFIRE_MAX_STATIC_STRING_LEN]="http://miniprofile.xfire.com/bg/sh/type/1/";
	char path[XFIRE_MAX_STATIC_STRING_LEN]="";
	char bufQuery[XFIRE_MAX_STATIC_STRING_LEN]="";
	char * username=(char*)lpv;
	DWORD dwBytesRead = 0;
	BOOL bRead;

	strcat(address,username);
	strcat(address,".png");

	FoldersGetCustomPath( XFireWorkingFolder, path, 256, 'W' );
	strcat(path,"\\temp.png");

	//verbindung zur xfireseite herrstellen
	HINTERNET hNet = InternetOpen("XFire",PRE_CONFIG_INTERNET_ACCESS,	NULL,INTERNET_INVALID_PORT_NUMBER,0);
	if(hNet == NULL)
		return;

	//profilseite laden
	HINTERNET hUrlFile = InternetOpenUrl(hNet,address,NULL,0,INTERNET_FLAG_RELOAD,0);
	if(hUrlFile == NULL)
	{
		InternetCloseHandle(hNet);
		return;
	}

	//avatar speichern
	FILE* f=fopen(path,"wb");
	if(f==NULL)
	{
		InternetCloseHandle(hUrlFile);
		InternetCloseHandle(hNet);
		return;
	}

	//bild empfangen
	do
	{
		bRead = InternetReadFile(hUrlFile,bufQuery,XFIRE_MAX_STATIC_STRING_LEN,&dwBytesRead);
		fwrite(bufQuery,dwBytesRead,1,f);
	}
	while(dwBytesRead);
	fclose(f);

	InternetCloseHandle(hUrlFile);
	InternetCloseHandle(hNet);

	SendMessage(ghwndDlg,WM_COMMAND,250,0);

	return;
}

static int GetIPPortUDetails(HANDLE wParam,char* feld1,char* feld2)
{
	char temp[255];
    HGLOBAL clipbuffer;
	char* buffer;

	if(DBGetContactSettingWord((HANDLE)wParam, protocolname, feld2, -1)==0)
		return 0;

	DBVARIANT dbv;
	if(DBGetContactSettingTString((HANDLE)wParam, protocolname, feld1,&dbv))
		return 0;

	sprintf(temp,"%s:%d",dbv.pszVal,DBGetContactSettingWord((HANDLE)wParam, protocolname, feld2, -1));

	DBFreeVariant(&dbv);

	if(OpenClipboard(NULL))
	{
		EmptyClipboard();

		clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(temp)+1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, LPCSTR(temp));
		GlobalUnlock(clipbuffer);

		SetClipboardData(CF_TEXT, clipbuffer);
		CloseClipboard();
	}

	return 0;
}

static BOOL CALLBACK DlgProcUserDetails(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static char path[XFIRE_MAX_STATIC_STRING_LEN]="";
	static WCHAR wpath[256];
	static HICON gameicon=0;
	static HICON voiceicon=0;
	static HANDLE uhandle=0;

	switch (msg)
	{
		case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);

			if(strlen(path)==0)
			{
				FoldersGetCustomPath( XFireWorkingFolder, path, 256, 'W' );
				strcat(path,"\\temp.png");
				MultiByteToWideChar(CP_ACP, 0, path, -1, wpath, 150);
			}
	
			ghwndDlg=hwndDlg;

			return TRUE;
		}
		case WM_CTLCOLORSTATIC:
			{
			break;
			}

		case WM_NOTIFY:
		{
			switch (((LPNMHDR)lParam)->idFrom)
			  {
		        
			  case 0:
				{
				  switch (((LPNMHDR)lParam)->code)
				  {
		            
				  case PSN_INFOCHANGED:
					{
					  char* szProto;
					  HANDLE hContact = (HANDLE)((LPPSHNOTIFY)lParam)->lParam;
					  uhandle=hContact; //handle sichern

					  if (hContact == NULL)
						szProto = protocolname;
					  else
						szProto = (char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);

					  if (szProto == NULL)
						break;

					  if (hContact)
					  {
							DBVARIANT dbv;
							if(!DBGetContactSetting(hContact,protocolname,"Username",&dbv))
							{
								mir_forkthread(DLProfilePic,dbv.pszVal);
							}

							SetItemTxt(hwndDlg,IDC_DNICK,"Nick",hContact,"-",0);
							SetItemTxt(hwndDlg,IDC_DUSERNAME,"Username",hContact,"-",0);

							SetItemTxt(hwndDlg,IDC_GIP,"ServerIP",hContact,"-",0);
							SetItemTxt(hwndDlg,IDC_VIP,"VServerIP",hContact,"-",0);
							SetItemTxt(hwndDlg,IDC_GPORT,"Port",hContact,"-",1);
							SetItemTxt(hwndDlg,IDC_VPORT,"VPort",hContact,"-",1);

							SetItemTxt(hwndDlg,IDC_GAME,"RGame",hContact,"-",0);
							SetItemTxt(hwndDlg,IDC_VNAME,"RVoice",hContact,"-",0);

							//render icons
							{
								DBVARIANT dbv;
								HANDLE icon;

								if(!DBGetContactSetting(hContact,protocolname,"GameId",&dbv))
								{
									GetGame(dbv.wVal,0,&icon,&gameicon);
									SendMessage(GetDlgItem(hwndDlg,IDC_GAMEICO),STM_SETICON,(WPARAM)gameicon,0);
								}							
								if(!DBGetContactSetting(hContact,protocolname,"VoiceId",&dbv))
								{
									GetGame(dbv.wVal,0,&icon,&voiceicon);
									SendMessage(GetDlgItem(hwndDlg,IDC_VOICEICO),STM_SETICON,(WPARAM)voiceicon,0);
								}
								
								if(DBGetContactSetting(hContact,protocolname,"ServerIP",&dbv))
								{
									EnableWindow(GetDlgItem(hwndDlg,IDC_COPYGAME),FALSE);
								}
								if(DBGetContactSetting(hContact,protocolname,"VServerIP",&dbv))
								{
									EnableWindow(GetDlgItem(hwndDlg,IDC_COPYVOICE),FALSE);
								}

								//ShowWindow(GetDlgItem(hwndDlg,IDC_VOICEICO),FALSE)
							}
					  }
					}
					break;
				  }
				}
				break;
			  }
		}
		break;

		case WM_COMMAND:
			{
				switch(wParam)
				{
				case IDC_COPYGAME:
					GetIPPortUDetails(uhandle,"ServerIP","Port");
					break;
				case IDC_COPYVOICE:
					GetIPPortUDetails(uhandle,"VServerIP","VPort");
					break;
				case 250:
					Gdiplus::Graphics grp(GetDC(GetDlgItem(hwndDlg,IDC_PROFILIMG)));
					Gdiplus::Bitmap image(wpath,0);
					HBITMAP HBM;
					image.GetHBITMAP(0,&HBM);
					SendMessage(GetDlgItem(hwndDlg,IDC_PROFILIMG),STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)HBM); 
					break;
				}
			}
	}
	return FALSE;
}

int OnDetailsInit(WPARAM wParam,LPARAM lParam)
{
	OPTIONSDIALOGPAGE odp = {0};

	if (!IsXFireContact((HANDLE)lParam))
		return 0;

	odp.cbSize = sizeof(odp);
	odp.hIcon = NULL;
	odp.hInstance = hinstance;
	odp.pfnDlgProc = DlgProcUserDetails;
	odp.position = -1900000000;
	odp.pszTemplate = MAKEINTRESOURCE(IDD_UD);
	odp.pszTitle = Translate("XFire");
	odp.pszGroup = NULL;

	CallService(MS_USERINFO_ADDPAGE,wParam,(LPARAM)&odp);	

	return 0;
}