#ifndef __CRYPTOPP_H__
#define __CRYPTOPP_H__

/*
SecureIM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 	2003 Johell
							2005-2009 Baloo

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

#define CPP_FEATURES_UTF8		0x01
#define CPP_FEATURES_BASE64		0x02
#define CPP_FEATURES_GZIP		0x04
#define CPP_FEATURES_CRC32		0x08
#define CPP_FEATURES_PSK		0x10
#define CPP_FEATURES_NEWPG		0x20
#define CPP_FEATURES_RSA		0x40

#define CPP_MODE_BASE16			0x0000
#define CPP_MODE_BASE64			0x0001
#define CPP_MODE_PGP			0x0002
#define CPP_MODE_GPG			0x0004
#define CPP_MODE_GPG_ANSI		0x0008
#define CPP_MODE_PRIV_KEY		0x0010
#define CPP_MODE_RSA_2048		0x0020
#define CPP_MODE_RSA_4096		0x0040
#define CPP_MODE_RSA			CPP_MODE_RSA_4096
#define CPP_MODE_RSA_ONLY		0x0080
#define CPP_MODE_RSA_ZLIB		0x0100
#define CPP_MODE_RSA_BER 		0x0200

#define CPP_ERROR_NONE			0
#define CPP_ERROR_SEH			1
#define CPP_ERROR_NO_KEYA		2
#define CPP_ERROR_NO_KEYB		3
#define CPP_ERROR_NO_KEYX		4
#define CPP_ERROR_BAD_LEN		5
#define CPP_ERROR_BAD_CRC		6
#define CPP_ERROR_NO_PSK		7
#define CPP_ERROR_BAD_PSK		8
#define CPP_ERROR_BAD_KEYB		9
#define CPP_ERROR_NO_PGP_KEY		10

typedef struct {
    int (__cdecl *rsa_gen_keypair)(short);				// ������� RSA-����� ��� ��������� ����� (���� ���� 2048, ���� 2048 � 4096)
    int (__cdecl *rsa_get_keypair)(short,PBYTE,int*,PBYTE,int*);	// ���������� ���� ������ ��� ��������� �����
    int (__cdecl *rsa_get_keyhash)(short,PBYTE,int*,PBYTE,int*);	// ���������� hash ���� ������ ��� ��������� �����
    int (__cdecl *rsa_set_keypair)(short,PBYTE,int);			// ������������� �����, ��������� �����
    int (__cdecl *rsa_get_pubkey)(HANDLE,PBYTE,int*);			// ���������� ������ ���� �� ���������� ���������
    int (__cdecl *rsa_set_pubkey)(HANDLE,PBYTE,int);			// ��������� ������ ���� ��� ���������� ���������
    void (__cdecl *rsa_set_timeout)(int);				// ���������� ������� ��� ��������� ��������� ����������
    int (__cdecl *rsa_get_state)(HANDLE);				// �������� ������ ���������� ���������
    int (__cdecl *rsa_get_hash)(PBYTE,int,PBYTE,int*);			// ��������� SHA1(key)
    int (__cdecl *rsa_connect)(HANDLE);					// ��������� ������� ��������� ��������� � ��������� ����������
    int (__cdecl *rsa_disconnect)(HANDLE);				// ��������� ���������� � ��������� ����������
    int (__cdecl *rsa_disabled)(HANDLE);				// ��������� ���������� �� ������� "disabled"
    LPSTR (__cdecl *rsa_recv)(HANDLE,LPCSTR);				// ���������� ���������� ���� ��� �������� ������������ ���������
    int   (__cdecl *rsa_send)(HANDLE,LPCSTR);				// �������� ��� �������� ��������� �������
    int (__cdecl *rsa_encrypt_file)(HANDLE,LPCSTR,LPCSTR);
    int (__cdecl *rsa_decrypt_file)(HANDLE,LPCSTR,LPCSTR);
    LPSTR  (__cdecl *utf8encode)(LPCWSTR);
    LPWSTR (__cdecl *utf8decode)(LPCSTR);
    int (__cdecl *is_7bit_string)(LPCSTR);
    int (__cdecl *is_utf8_string)(LPCSTR);
    int (__cdecl *rsa_export_keypair)(short,LPSTR,LPSTR,LPSTR);		// export private key
    int (__cdecl *rsa_import_keypair)(short,LPSTR,LPSTR);		// import & activate private key
    int (__cdecl *rsa_export_pubkey)(HANDLE,LPSTR);			// export public key from context
    int (__cdecl *rsa_import_pubkey)(HANDLE,LPSTR);			// import public key into context
} RSA_EXPORT;
typedef RSA_EXPORT* pRSA_EXPORT;

typedef struct {
    int  (__cdecl *rsa_inject)(HANDLE,LPCSTR);			// ��������� ��������� � ������� �� ��������
    int  (__cdecl *rsa_check_pub)(HANDLE,PBYTE,int,PBYTE,int);	// ��������� ������������ SHA � ��������� ����, ���� ��� ���������
    void (__cdecl *rsa_notify)(HANDLE,int);			// ����������� � ����� ���������
} RSA_IMPORT;
typedef RSA_IMPORT* pRSA_IMPORT;


#endif
