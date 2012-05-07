/*
Crypto++ plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
Copyright (C) 2006-2009 Baloo

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

#include "commonheaders.h"


HMODULE hgpg;
HRSRC	hRS_gpg;
PBYTE	pRS_gpg;

extern DLLEXPORT int   __cdecl _gpg_init(void);
extern DLLEXPORT int   __cdecl _gpg_done(void);
extern DLLEXPORT int   __cdecl _gpg_open_keyrings(LPSTR,LPSTR);
extern DLLEXPORT int   __cdecl _gpg_close_keyrings(void);
extern DLLEXPORT void  __cdecl _gpg_set_log(LPCSTR);
extern DLLEXPORT void  __cdecl _gpg_set_tmp(LPCSTR);
extern DLLEXPORT LPSTR __cdecl _gpg_get_error(void);
extern DLLEXPORT int   __cdecl _gpg_size_keyid(void);
extern DLLEXPORT int   __cdecl _gpg_select_keyid(HWND,LPSTR);
extern DLLEXPORT LPSTR __cdecl _gpg_encrypt(LPCSTR,LPCSTR);
extern DLLEXPORT LPSTR __cdecl _gpg_decrypt(LPCSTR);
extern DLLEXPORT LPSTR __cdecl _gpg_get_passphrases();
extern DLLEXPORT void  __cdecl _gpg_set_passphrases(LPCSTR);

int   __cdecl _gpg_init(void);
int   __cdecl _gpg_done(void);
int   __cdecl _gpg_open_keyrings(LPSTR,LPSTR);
int   __cdecl _gpg_close_keyrings(void);
void  __cdecl _gpg_set_log(LPCSTR);
void  __cdecl _gpg_set_tmp(LPCSTR);
LPSTR __cdecl _gpg_get_error(void);
int   __cdecl _gpg_size_keyid(void);
int   __cdecl _gpg_select_keyid(HWND,LPSTR);
LPSTR __cdecl _gpg_encrypt(LPCSTR,LPCSTR);
LPSTR __cdecl _gpg_decrypt(LPCSTR);
LPSTR __cdecl _gpg_get_passphrases();
void  __cdecl _gpg_set_passphrases(LPCSTR);

int   (__cdecl *p_gpg_init)(void);
int   (__cdecl *p_gpg_done)(void);
int   (__cdecl *p_gpg_open_keyrings)(LPSTR,LPSTR);
int   (__cdecl *p_gpg_close_keyrings)(void);
void  (__cdecl *p_gpg_set_log)(LPCSTR);
void  (__cdecl *p_gpg_set_tmp)(LPCSTR);
LPSTR (__cdecl *p_gpg_get_error)(void);
int   (__cdecl *p_gpg_size_keyid)(void);
int   (__cdecl *p_gpg_select_keyid)(HWND,LPSTR);
LPSTR (__cdecl *p_gpg_encrypt)(LPCSTR,LPCSTR);
LPSTR (__cdecl *p_gpg_decrypt)(LPCSTR);
LPSTR (__cdecl *p_gpg_get_passphrases)();
void  (__cdecl *p_gpg_set_passphrases)(LPCSTR);


#define GPA(x)                                              \
  {                                                         \
    *((PVOID*)&p##x) = (PVOID)GetProcAddress(mod, TEXT(#x)); \
    if (!p##x) {                                            \
      return 0;                                             \
    }                                                       \
  }

int load_gpg_dll(HMODULE mod) {

	GPA(_gpg_init);
	GPA(_gpg_done);
	GPA(_gpg_open_keyrings);
	GPA(_gpg_close_keyrings);
	GPA(_gpg_set_log);
	GPA(_gpg_set_tmp);
	GPA(_gpg_get_error);
	GPA(_gpg_size_keyid);
	GPA(_gpg_select_keyid);
	GPA(_gpg_encrypt);
	GPA(_gpg_decrypt);
	GPA(_gpg_get_passphrases);
	GPA(_gpg_set_passphrases);

	return 1;
}

#undef GPA


#define GPA(x)                                              \
  {                                                         \
    *((PVOID*)&p##x) = (PVOID)MemGetProcAddress(mod, TEXT(#x)); \
    if (!p##x) {                                            \
      return 0;                                             \
    }                                                       \
  }

int load_gpg_mem(HMODULE mod) {

	GPA(_gpg_init);
	GPA(_gpg_done);
	GPA(_gpg_open_keyrings);
	GPA(_gpg_close_keyrings);
	GPA(_gpg_set_log);
	GPA(_gpg_set_tmp);
	GPA(_gpg_get_error);
	GPA(_gpg_size_keyid);
	GPA(_gpg_select_keyid);
	GPA(_gpg_encrypt);
	GPA(_gpg_decrypt);
	GPA(_gpg_get_passphrases);
	GPA(_gpg_set_passphrases);

	return 1;
}

#undef GPA


int __cdecl gpg_init()
{
	int r; char t[MAX_PATH];
	if( isVista ){
		sprintf(t,"%s\\gnupgw.dll",TEMP);
		ExtractFile(t,666,1);
		hgpg = LoadLibraryA(t);
	}
	else {
		hRS_gpg = FindResource( g_hInst, MAKEINTRESOURCE(1), MAKEINTRESOURCE(666) );
		pRS_gpg = (PBYTE) LoadResource( g_hInst, hRS_gpg ); LockResource( pRS_gpg );
		hgpg = MemLoadLibrary( pRS_gpg );
	}
	if (hgpg) {
		if( isVista )	load_gpg_dll(hgpg);
		else			load_gpg_mem(hgpg);
		r = p_gpg_init();
		if(r) {
			return r;
		}
		if( isVista ){
			FreeLibrary(hgpg);
		}
		else {
			MemFreeLibrary(hgpg);
	    	UnlockResource( pRS_gpg );
			FreeResource( pRS_gpg );
		}
	}

    hgpg = 0;

	return 0;
}


int __cdecl gpg_done()
{
    int r = 0;
    if(hgpg) {
    	r = p_gpg_done();
		if( isVista ){
			FreeLibrary(hgpg);
		}
		else {
			MemFreeLibrary(hgpg);
		    UnlockResource( pRS_gpg );
			FreeResource( pRS_gpg );
		}
		hgpg = 0;
    }
	return r;
}


int __cdecl gpg_open_keyrings(LPSTR ExecPath, LPSTR HomePath)
{
	return p_gpg_open_keyrings(ExecPath, HomePath);
}


int __cdecl gpg_close_keyrings()
{
	return p_gpg_close_keyrings();
}


void __cdecl gpg_set_log(LPCSTR LogPath)
{
	p_gpg_set_log(LogPath);
}


void __cdecl gpg_set_tmp(LPCSTR TmpPath)
{
	p_gpg_set_tmp(TmpPath);
}


LPSTR __cdecl gpg_get_error()
{
	return p_gpg_get_error();
}


LPSTR __cdecl gpg_encrypt(pCNTX ptr, LPCSTR szPlainMsg)
{
   	ptr->error = ERROR_NONE;
	pGPGDATA p = (pGPGDATA) ptr->pdata;
	SAFE_FREE(ptr->tmp);

	LPSTR szEncMsg;
	szEncMsg = p_gpg_encrypt(szPlainMsg,(LPCSTR)p->gpgKeyID);
	if(!szEncMsg) return 0;

	ptr->tmp = (LPSTR) strdup(szEncMsg);
	LocalFree((LPVOID)szEncMsg);

	return ptr->tmp;
}


LPSTR __cdecl gpg_decrypt(pCNTX ptr, LPCSTR szEncMsg)
{
    ptr->error = ERROR_NONE;
    SAFE_FREE(ptr->tmp);

    LPSTR szPlainMsg = p_gpg_decrypt(szEncMsg);
/*	if(!szPlainMsg) {
	    ptr = get_context_on_id(hPGPPRIV); // find private pgp keys
    	if(ptr && ptr->pgpKey)
			szPlainMsg = p_gpg_decrypt_key(szEncMsg,(LPCSTR)ptr->pgpKey);
		if(!szPlainMsg) return NULL;
    }*/

    ptr->tmp = (LPSTR) strdup(szPlainMsg);
    LocalFree((LPVOID)szPlainMsg);

    return ptr->tmp;
}


LPSTR __cdecl gpg_encode(HANDLE context, LPCSTR szPlainMsg)
{
	pCNTX ptr = get_context_on_id(context); if(!ptr) return NULL;
	pGPGDATA p = (pGPGDATA) cpp_alloc_pdata(ptr);
	if(!p->gpgKeyID) { ptr->error = ERROR_NO_GPG_KEY; return NULL; }

	// utf8 message: encrypt.
	LPSTR szUtfMsg;
	if( ptr->mode & MODE_GPG_ANSI ) {
		LPWSTR wszMsg = utf8decode(szPlainMsg);
		int wlen = wcslen(wszMsg)+1;
		szUtfMsg = (LPSTR) alloca(wlen);
		WideCharToMultiByte(CP_ACP, 0, wszMsg, -1, szUtfMsg, wlen, 0, 0);
	}
	else {
		szUtfMsg = (LPSTR)szPlainMsg;
	}
	return gpg_encrypt(ptr, szUtfMsg);
}


LPSTR __cdecl gpg_decode(HANDLE context, LPCSTR szEncMsg)
{
	pCNTX ptr = get_context_on_id(context);
	if(!ptr) return NULL;

	LPSTR szNewMsg = NULL;
	LPSTR szOldMsg = gpg_decrypt(ptr, szEncMsg);

	if(szOldMsg) {
		if( !is_7bit_string(szOldMsg) && !is_utf8_string(szOldMsg) ) {
			int slen = strlen(szOldMsg)+1;
			LPWSTR wszMsg = (LPWSTR) alloca(slen*sizeof(WCHAR));
			MultiByteToWideChar(CP_ACP, 0, szOldMsg, -1, wszMsg, slen*sizeof(WCHAR));
			szNewMsg = _strdup(utf8encode(wszMsg));
		}
		else {
			szNewMsg = _strdup(szOldMsg);
		}
	}
	SAFE_FREE(ptr->tmp);
	ptr->tmp = szNewMsg;
	return szNewMsg;
}


int __cdecl gpg_set_key(HANDLE context, LPCSTR RemoteKey)
{
/*    pCNTX ptr = get_context_on_id(context);
    if(!ptr) return 0;
   	ptr->error = ERROR_NONE;

//   	if(!p_gpg_check_key(RemoteKey)) return 0;

   	SAFE_FREE(ptr->pgpKey);
	ptr->pgpKey = (BYTE *) malloc(strlen(RemoteKey)+1);
	strcpy((LPSTR)ptr->pgpKey,RemoteKey);

   	return 1;
*/
	return 0;
}


int __cdecl gpg_set_keyid(HANDLE context, LPCSTR RemoteKeyID)
{
	pCNTX ptr = get_context_on_id(context); if(!ptr) return 0;
	pGPGDATA p = (pGPGDATA) cpp_alloc_pdata(ptr);
   	ptr->error = ERROR_NONE;

   	SAFE_FREE(p->gpgKeyID);
	p->gpgKeyID = (PBYTE) strdup(RemoteKeyID);

   	return 1;
}


int __cdecl gpg_size_keyid()
{
	return p_gpg_size_keyid();
}


int __cdecl gpg_select_keyid(HWND hDlg,LPSTR szKeyID)
{
	return p_gpg_select_keyid(hDlg,szKeyID);
}


LPSTR __cdecl gpg_get_passphrases()
{
	return p_gpg_get_passphrases();
}


void __cdecl gpg_set_passphrases(LPCSTR buffer)
{
	p_gpg_set_passphrases(buffer);
}


// EOF