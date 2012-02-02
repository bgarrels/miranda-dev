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

#include "commonheaders.h"
#include "database.h"
#include "sha256.h"
#include <m_popup.h>

char g_password[128], g_newpassword[128];
int g_passlen = 1, g_newpasslen = 0;
unsigned char g_sha256sum[32], g_new256sum[32];
int g_secured, g_WrongPass, g_rempass;
UINT oldLangID;
HANDLE hSetPwdMenu, hOnLoadHook;

//encryption offset; all data except the header;
#define ENC_OFFSET sizeof(struct DBHeader)

static BOOL IsAeroMode()
{
	BOOL result;
	return dwmIsCompositionEnabled && (dwmIsCompositionEnabled(&result) == S_OK) && result;
}

INT_PTR CALLBACK DlgStdInProc(HWND hDlg, UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HICON hIcon = 0;
	RECT rect;
	TCHAR tszHeaderTxt[256];
	TCHAR* tszDbName;
	HWND hHeader = 0;
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			SendDlgItemMessage(hDlg,IDC_EDIT1,EM_LIMITTEXT,sizeof(g_password)-1,0);
			SendDlgItemMessage(hDlg,IDC_EDIT2,EM_LIMITTEXT,sizeof(g_password)-1,0);

			hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_DLGPASSWD));
			hHeader = GetDlgItem(hDlg, IDC_HEADERBAR);

			if (IsAeroMode())
			{
				SetWindowLongPtr(hDlg, GWL_STYLE, GetWindowLongPtr(hDlg, GWL_STYLE) | WS_DLGFRAME | WS_SYSMENU);
				
				GetClientRect(hDlg, &rect);
				SetWindowPos(hDlg, 0, 0, 0, rect.right, rect.bottom +
					GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXSIZEFRAME), 
					SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER);
			}
			SendMessage(GetDlgItem(hDlg, IDC_HEADERBAR), WM_SETICON, 0, (LPARAM)hIcon);

			if(lParam) {
				if(!g_WrongPass)
				{
					tszDbName = mir_a2t((char*)lParam);
					mir_sntprintf(tszHeaderTxt, SIZEOF(tszHeaderTxt), _T("%s\n%s"), TranslateT("Please type in your password for"), tszDbName);
					SetWindowText(hHeader, tszHeaderTxt);
					mir_free(tszDbName);
				}
				else
					SetWindowText(hHeader, _T("Password is not correct!\nPlease, enter correct password."));
			}
			else
			{
				mir_sntprintf(tszHeaderTxt, SIZEOF(tszHeaderTxt), _T("%s\n%s"), TranslateT("Change password"), g_secured ? TranslateT("Leave this fields blank to disable password") : TranslateT("Please type in your new password"));
				SetWindowText(hHeader, tszHeaderTxt);
			}
			
			TranslateDialogDefault(hDlg);
			SetWindowLongPtr(hDlg,GWLP_USERDATA,(lParam==0));
			oldLangID = 0;
			SetTimer(hDlg,1,200,NULL);
			LanguageChanged(hDlg);

			return TRUE;
		}

	case WM_CTLCOLORSTATIC:
		{
			if ((HWND)lParam == GetDlgItem(hDlg, IDC_LANG))
			{
				SetTextColor((HDC)wParam, GetSysColor(COLOR_HIGHLIGHTTEXT));
				SetBkMode((HDC)wParam, TRANSPARENT);
				return (BOOL)GetSysColorBrush(COLOR_HIGHLIGHT);
			}

			return FALSE;
		}


	case WM_COMMAND:
		{
			UINT uid = LOWORD(wParam);

			if(uid == IDOK){
				if(!GetWindowLongPtr(hDlg,GWLP_USERDATA))
				{
					g_passlen = GetDlgItemTextA(hDlg,IDC_EDIT1,g_password,sizeof(g_password)-1);
						EndDialog(hDlg,IDOK);
				}else{
					char temp1[sizeof(g_password)+1] = {0};
					char temp2[sizeof(g_password)+1] = {0};
					int tlen1 = 0;
					int tlen2 = 0;

					tlen1 = GetDlgItemTextA(hDlg,IDC_EDIT2,temp1,sizeof(temp1)-1);
					tlen2 = GetDlgItemTextA(hDlg,IDC_EDIT1,temp2,sizeof(temp2)-1);

					if(g_secured && tlen1 == 0 && tlen2 == 0){
						g_rempass = 1;
						EndDialog(hDlg,IDOK);
					}else if(strcmp(temp1,temp2)){
						SetWindowText(hHeader, TranslateT("Passwords do not match!"));
						SendMessage(hHeader, WM_NCPAINT, 0, 0);
						SetDlgItemTextA(hDlg,IDC_EDIT1,"");
						SetDlgItemTextA(hDlg,IDC_EDIT2,"");

					}else if(tlen1 < 4 || tlen2 < 4){
						SetWindowText(hHeader, TranslateT("Password is too short!"));
						SendMessage(hHeader, WM_NCPAINT, 0, 0);
						SetDlgItemTextA(hDlg,IDC_EDIT1,"");
						SetDlgItemTextA(hDlg,IDC_EDIT2,"");
					}else{
						strcpy(g_newpassword,temp1);
						g_newpasslen = tlen1;
						EndDialog(hDlg,IDOK);
					}
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
			DestroyIcon(hIcon);
			return FALSE;
		}
	//case WM_INPUTLANGCHANGE:
	//	{
	//		LanguageChanged(hDlg);
	//		return FALSE;			
	//	}
		//break;
	}

	return FALSE;
}

static void LanguageChanged(HWND hDlg)
{
	UINT LangID = (UINT)GetKeyboardLayout(0);
	char Lang[3] = {0};
	if (LangID != oldLangID)
	{
		oldLangID = LangID;
		GetLocaleInfoA(MAKELCID((LangID & 0xffffffff),  SORT_DEFAULT),  LOCALE_SABBREVLANGNAME, Lang, 2);
		Lang[0] = toupper(Lang[0]);
		Lang[1] = tolower(Lang[1]);
		SetDlgItemTextA(hDlg, IDC_LANG, Lang);
	}
}

void dbxShowPopup(WPARAM wParam, LPARAM lParam)
{
	POPUPDATAT ppd = {0};

	lstrcpy(ppd.lptzContactName, TranslateT("SecureDB"));
	lstrcpy(ppd.lptzText, (TCHAR *)wParam);
	ppd.lchIcon = (HICON)CallService(MS_SKIN2_GETICON, 0, (LPARAM)"password");

	PUAddPopUpT(&ppd);
}

void updateCachedHdr(struct DBHeader* dbh)
{
	extern struct DBCacheSectionInfo cacheSectionInfo[CACHESECTIONCOUNT];
	extern PBYTE pDbCache;
	extern struct DBHeader dbHeader;
	int i=0;

	struct DBHeader* pdbh;

	dbHeader.version = dbh->version;
	memcpy(dbHeader.signature,dbh->signature,sizeof(pdbh->signature));

	//this is not that necessary, but it's better to make sure :)
	for(;i<CACHESECTIONCOUNT;i++){
		if(cacheSectionInfo[i].ofsBase == 0)
		{
			pdbh = (struct DBHeader*)(pDbCache + (i*CACHESECTIONSIZE));
			pdbh->version = dbh->version;
			memcpy(pdbh->signature,dbh->signature,sizeof(pdbh->signature));
			break;
		}
	}
}

int EncGetPassword(void* pdbh,const char* dbase)
{
	int res;
	unsigned int pwdcrc=0;
	char* tmp;
	sha256_context ctx;

	struct DBHeader* dbh = (struct DBHeader*)pdbh;

	ZeroMemory(g_password, sizeof(g_password));

	if(dbase){
		tmp = strrchr(dbase,'\\');
		if(tmp)dbase = tmp + 1;
	}

	while(TRUE)
	{
		FILE *passlog;
		SYSTEMTIME systime;

		res = DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_DIALOG1),NULL,(DLGPROC)DlgStdInProc,(LPARAM)dbase);
		if(res != IDOK)return -1;

		//generate the sha256 sum for the password; used in encryption + password checking
		sha256_starts(&ctx);
		sha256_update(&ctx,(uint8*)g_password,g_passlen);
		sha256_finish(&ctx,g_sha256sum);

		pwdcrc = (g_sha256sum[0] | (g_sha256sum[5] << 8)) + ((g_sha256sum[10]) | (g_sha256sum[31] << 8));

		if(dbh->version == pwdcrc) break;

		//logging wrong passwords. For enable logging create file pass.log in the Miranda root directory.
		if ( (passlog = fopen("pass.log", "r")) != 0 ){
			fclose(passlog);
			GetLocalTime(&systime);
			if ( (passlog = fopen("pass.log", "a")) != 0 ){
				fprintf(passlog,"%02d.%02d.%04d %02d:%02d:%02d | %s | %s \n", systime.wDay, systime.wMonth, systime.wYear, systime.wHour, systime.wMinute, systime.wSecond, dbase, g_password);
				fclose(passlog);
			}
		}
		g_WrongPass = TRUE;
	}
	g_WrongPass = FALSE;
	g_secured = 1;

	return 0;
}

#ifndef _WIN64

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

#else

static __inline void rotl(unsigned char* a,unsigned char n)
{
	n = n % 8;
	*a = (*a << n) | (*a >> (8 - n));
}

static __inline void rotr(unsigned char* a,unsigned char n)
{
	n = n % 8;
	*a = (*a >> n) | (*a << (8 - n));
}

#endif

static int RemovePassword()
{
	extern CRITICAL_SECTION csDbAccess;
	extern HANDLE hDbFile;
	extern struct DBSignature dbSignature;

	int result = 1;
	unsigned int size;
	unsigned int rw;
	unsigned char* buffer = NULL;
	struct DBHeader *dbh = NULL;

	EnterCriticalSection(&csDbAccess);

	FlushFileBuffers(hDbFile);
	size = GetFileSize(hDbFile,NULL);
	if(size < ENC_OFFSET)goto End;
	buffer = (unsigned char*)malloc(size + 1);
	if(!buffer)goto End;
	dbh = (struct DBHeader*)buffer;

	SetFilePointer(hDbFile,0,NULL,FILE_BEGIN);
	if(!EncReadFile(hDbFile,buffer,size,&rw,NULL) || rw!=size)goto End;
	g_secured = 0;

	dbh->version = DB_THIS_VERSION;
	memcpy(dbh->signature,&dbSignature,sizeof(dbh->signature));
	updateCachedHdr(dbh);

	SetFilePointer(hDbFile,0,NULL,FILE_BEGIN);
	if(!EncWriteFile(hDbFile,buffer,size,&rw,NULL) || rw!=size)goto End;
	FlushFileBuffers(hDbFile);
	result = 0;
End:
	if(buffer)free(buffer);
	LeaveCriticalSection(&csDbAccess);

	if(!g_secured){
		xModifyMenu(hSetPwdMenu, LPGENT("Set Password"));
	}
	return result;
}

static int SetPassword()
{
	extern CRITICAL_SECTION csDbAccess;
	extern HANDLE hDbFile;
	extern struct DBSignature dbSecSignature;

	int result = 1;
	unsigned int size;
	unsigned int rw;
	unsigned char* buffer = NULL;
	struct DBHeader *dbh = NULL;

	EnterCriticalSection(&csDbAccess);

	FlushFileBuffers(hDbFile);
	size = GetFileSize(hDbFile,NULL);
	if(size < ENC_OFFSET)goto End;
	buffer = (unsigned char*)malloc(size + 1);
	if(!buffer)goto End;
	dbh = (struct DBHeader*)buffer;

	SetFilePointer(hDbFile,0,NULL,FILE_BEGIN);
	if(!EncReadFile(hDbFile,buffer,size,&rw,NULL) || rw!=size)goto End;
	g_secured = 1;

	dbh->version = (g_sha256sum[0] | (g_sha256sum[5] << 8)) + ((g_sha256sum[10]) | (g_sha256sum[31] << 8));
	memcpy(dbh->signature,&dbSecSignature,sizeof(dbh->signature));

	updateCachedHdr(dbh);

	SetFilePointer(hDbFile,0,NULL,FILE_BEGIN);
	if(!EncWriteFile(hDbFile,buffer,size,&rw,NULL) || rw!=size)goto End;
	FlushFileBuffers(hDbFile);
	result = 0;
End:
	if(buffer)free(buffer);
	LeaveCriticalSection(&csDbAccess);

	if(g_secured){
		xModifyMenu(hSetPwdMenu, LPGENT("Change Password"));
	}
	return result;
}


INT_PTR DB3XSSetPassword(WPARAM wParam, LPARAM lParam)
{
	extern dbpanic();
	extern struct DBHeader dbHeader;
	extern char szDbPath[MAX_PATH];
	sha256_context ctx;
	int res;
	unsigned int pwdcrc = 0;
	char* dbase;
	char* tmp;

	if (g_secured) {

		tmp = strrchr(szDbPath,'\\');
		dbase = tmp + 1;

		while(TRUE)
		{
			res = DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_DIALOG1),NULL,(DLGPROC)DlgStdInProc,(LPARAM)dbase);
			if(res != IDOK){
				g_WrongPass = FALSE;
				return 0;
			}

			sha256_starts(&ctx);
			sha256_update(&ctx,(uint8*)g_password,g_passlen);
			sha256_finish(&ctx,g_sha256sum);

			pwdcrc = (g_sha256sum[0] | (g_sha256sum[5] << 8)) + ((g_sha256sum[10]) | (g_sha256sum[31] << 8));

			if(dbHeader.version == pwdcrc) break;

			g_WrongPass = TRUE;
		}
		g_WrongPass = FALSE;
	}

	res = DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_DIALOG2),NULL,(DLGPROC)DlgStdInProc,FALSE);
	if(res != IDOK)return 0;

	if(g_rempass == 1) {
		DB3XSRemovePassword(wParam, lParam);
		g_rempass = 0;
		return 0;
	}
	if(g_newpasslen != 0){

		sha256_starts(&ctx);
		sha256_update(&ctx,(uint8*)g_newpassword,g_newpasslen);
		sha256_finish(&ctx,g_new256sum);


		if(g_secured){RemovePassword();}
		g_passlen = g_newpasslen;
		memcpy(g_password,g_newpassword,g_newpasslen+1);
		memcpy(g_sha256sum,g_new256sum,32);

		if(SetPassword()){
			dbpanic(NULL);
		}else{
			if (ServiceExists(MS_POPUP_SHOWMESSAGE))
				dbxShowPopup((WPARAM)TranslateT("Password has been changed!"), 0);
			else
				MessageBox(NULL,(LPCTSTR)TranslateT("Password has been changed!"),(LPCTSTR)TranslateT("SecureDB"),MB_ICONINFORMATION);
		}
		return 0;		
	}
	return 0;
}

static int DB3XSRemovePassword(WPARAM wParam, LPARAM lParam)
{
	if(g_secured && MessageBox(NULL,(LPCTSTR)TranslateT("Are you sure you want to remove the password?"),TranslateT("SecureDB"),MB_ICONQUESTION | MB_YESNO)==IDYES)
	{
		if(RemovePassword()){
			dbpanic(NULL);
		}else{
			if (ServiceExists(MS_POPUP_SHOWMESSAGE))
				dbxShowPopup((WPARAM)TranslateT("Password has been removed!"), 0);
			else
				MessageBox(NULL,(LPCTSTR)TranslateT("Password has been removed!"),(LPCTSTR)TranslateT("SecureDB"),MB_ICONINFORMATION);
		}
	}
	return 0;
}

int EncReadFile(HANDLE hFile,void* data,unsigned int toread,unsigned int* read,void* dummy)
{
	unsigned char* bd = (unsigned char*)data;
	unsigned int i;
	unsigned int rtw;
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

int EncWriteFile(HANDLE hFile,void* data,unsigned int towrite,unsigned int* written,void* dummy)
{
	static unsigned char statbuff[16*1024];

	unsigned char* bd;
	unsigned char* dynbuff = NULL;
	unsigned int rtw;
	unsigned char* buffer;
	unsigned int i;
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

int dbxOnLoad()
{
	if (IsWinVerVistaPlus())
	{
		hDwmApi = LoadLibrary(_T("dwmapi.dll"));
		if (hDwmApi)
		{
			dwmIsCompositionEnabled = (PFNDwmIsCompositionEnabled)GetProcAddress(hDwmApi,"DwmIsCompositionEnabled");
		}
	}
	hOnLoadHook = HookEvent(ME_SYSTEM_MODULESLOADED, dbxMenusInit);
	return hOnLoadHook == NULL;
}