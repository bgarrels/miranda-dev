/*
 *  SecureDB
 *
 *  Copyright (C) 2005  Piotr Pawluczuk (piotrek@piopawlu.net)
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "../commonheaders.h"
#include "../dbtool.h"
#include "SecureDB.h"
#include "sha256.h"


//global vars
char g_password[128] = {0};
long g_passlen = 1;
unsigned char g_sha256sum[32] = {0};

char g_newpassword[128] = {0};
long g_newpasslen = 0;
unsigned char g_new256sum[32] = {0};

long g_secured = 0;

UINT oldLangID;

//global handles
HANDLE hSetPwdMenu = NULL;
HANDLE hOnLoadHook = NULL;

//encryption offset; all data except the header;
#define ENC_OFFSET sizeof(struct DBHeader)

BOOL CALLBACK DlgStdInProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			SendDlgItemMessage(hDlg,IDC_EDIT1,EM_LIMITTEXT,sizeof(g_password)-1,0);
			TranslateDialog(hDlg);

			if(lParam) SetDlgItemText(hDlg,IDC_INFO2,(LPCSTR)lParam);
			oldLangID = 0;
			SetTimer(hDlg,1,200,NULL);
			LanguageChanged(hDlg);

			return (TRUE);
		}
		case WM_CTLCOLORSTATIC:
			switch(GetWindowLong((HWND)lParam, GWL_ID)) {
				case IDC_WHITERECT:
				case IDC_DLGLOGO:
				case IDC_INFO:
				case IDC_INFO2:
					SetBkColor((HDC)wParam, RGB(255, 255, 255));
					return (BOOL)GetStockObject(WHITE_BRUSH);
			}
			if ((HWND)lParam == GetDlgItem(hDlg, IDC_LANG))
			{
				SetTextColor((HDC)wParam, GetSysColor(COLOR_HIGHLIGHTTEXT));
				SetBkMode((HDC)wParam, TRANSPARENT);
				return (BOOL)GetSysColorBrush(COLOR_HIGHLIGHT);
			}

			return FALSE;

	case WM_COMMAND:
		{
			UINT uid = LOWORD(wParam);
			if(uid == IDOK){

					g_passlen = GetDlgItemText(hDlg,IDC_EDIT1,g_password,sizeof(g_password)-1);
					if(g_passlen<4){
						MessageBox(hDlg,Translate("Password is too short!"),"SecureDB",MB_ICONEXCLAMATION);
						return TRUE;
					}else{
						EndDialog(hDlg,IDOK);
					}

			}else if(uid == IDCANCEL){
				EndDialog(hDlg,IDCANCEL);
			}
		}
	case WM_TIMER:
		{
			LanguageChanged(hDlg);
			return FALSE;
		}
	case WM_DESTROY:
		{
			KillTimer(hDlg, 1);
			return FALSE;
		}
	}

	return FALSE;
}

void LanguageChanged(HWND hDlg)
{
	UINT LangID = (UINT)GetKeyboardLayout(0);
	char Lang[3] = {0};
	if (LangID != oldLangID)
	{
		oldLangID = LangID;
		GetLocaleInfo(MAKELCID((LangID & 0xffffffff),  SORT_DEFAULT),  LOCALE_SABBREVLANGNAME, Lang, 2);
		SetDlgItemText(hDlg, IDC_LANG, Lang);
	}
}


int EncGetPassword(void* pdbh,const char* dbase)
{
	extern HINSTANCE hInst;
	int res;
	unsigned long pwdcrc=0;
	char* tmp;
	sha256_context ctx;

	struct DBHeader* dbh = (struct DBHeader*)pdbh;

	g_secured = 0;
	memset(g_password,0,sizeof(g_password));

	if(dbase){
		tmp = strrchr(dbase,'\\');
		if(tmp)dbase = tmp + 1;
	}

Again:
	res = DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_DIALOG1),NULL,(DLGPROC)DlgStdInProc,(LPARAM)dbase);
	if(res != IDOK)return -1;

	//generate the sha256 sum for the password; used in encryption + password checking
	sha256_starts(&ctx);
	sha256_update(&ctx,(uint8*)g_password,g_passlen);
	sha256_finish(&ctx,g_sha256sum);

	pwdcrc = (g_sha256sum[0] | (g_sha256sum[5] << 8)) + ((g_sha256sum[10]) | (g_sha256sum[31] << 8));

	if(dbh->version != pwdcrc){
		MessageBox(NULL,Translate("Password is not correct!"),"SecureDB",MB_ICONERROR);
		goto Again;
	}

	g_secured = 1;
	return 0;
}

static __inline void rotl(unsigned char* a,unsigned char n)
{
	_asm mov eax,a;
	_asm mov cl,n;
	_asm rol BYTE PTR[eax],cl;
}

static __inline void rotr(unsigned char* a,unsigned char n)
{
	_asm mov eax,a;
	_asm mov cl,n;
	_asm ror BYTE PTR[eax],cl;
}

int EncReadFile(HANDLE hFile,void* data,unsigned long toread,unsigned long* read,void* dummy)
{
	unsigned char* bd = (unsigned char*)data;
	unsigned long i;
	unsigned long rtw;
	unsigned char pw;

	if(!g_secured){
		return ReadFile(hFile,data,toread,read,NULL);
	}

	i = SetFilePointer(hFile,0,NULL,FILE_CURRENT);

	if(ReadFile(hFile,data,toread,read,NULL))
	{
		for(rtw=i+*read;i<rtw;i++){
			if(i >= ENC_OFFSET){
				pw = (unsigned char)g_password[i%g_passlen];
				if(g_sha256sum[i%32] & 0x01){
					*bd = ~*bd;
				}
				if(pw & 0x01){
					rotr(bd,2);
				}
				*bd ^= pw;
				rotr(bd,pw);
				*bd ^= g_sha256sum[i%32];
			}
			bd++;
		}
		return TRUE;
	}else{
		return FALSE;
	}
}

int EncWriteFile(HANDLE hFile,void* data,unsigned long towrite,unsigned long* written,void* dummy)
{
	static unsigned char statbuff[16*1024];

	unsigned char* bd;
	unsigned char* dynbuff = NULL;
	unsigned long rtw;
	unsigned char* buffer;
	unsigned long i;
	unsigned char pw;

	if(!g_secured){
		return WriteFile(hFile,data,towrite,written,NULL);
	}

	if(towrite > 16*1024){
		buffer = dynbuff = (unsigned char*)malloc(towrite+1);
		if(!buffer)return FALSE;
	}else{
		buffer = statbuff;
	}

	memcpy(buffer,data,towrite);
	bd = buffer;

	i = SetFilePointer(hFile,0,NULL,FILE_CURRENT);
	rtw = i + towrite;

	for(;i<rtw;i++){
		if(i >= ENC_OFFSET){
			pw = (unsigned char)g_password[i%g_passlen];
			*bd ^= g_sha256sum[i%32];
			rotl(bd,pw);
			*bd ^= pw;
			if(pw & 0x01){
				rotl(bd,2);
			}
			if(g_sha256sum[i%32] & 0x01){
				*bd = ~*bd;
			}
		}else{
			pw = *bd;
		}
		bd++;
	}

	i = WriteFile(hFile,buffer,towrite,written,NULL);

	if(dynbuff){
		free(dynbuff);
	}
	return i;
}
