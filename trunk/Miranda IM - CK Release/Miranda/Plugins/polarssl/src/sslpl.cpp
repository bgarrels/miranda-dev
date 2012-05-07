/*
PolarSSL plugin interface for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
Copyright (C) 2009-2012 Piotr Piasutski, Boris Krasnovskiy

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

#define MIRANDA_VER 0x800

#include <m_stdhdr.h>
#include <windows.h>

#include "polarssl/ssl.h"
#include "polarssl/havege.h" 

#include <newpluginapi.h>
#include <m_system.h>
#include <m_ssl.h>

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif 

static const PLUGININFOEX pluginInfoEx = 
{
	sizeof(PLUGININFOEX),
#if defined(_WIN64) 
	"PolarSSL Interface x64",
#else
	"PolarSSL Interface",
#endif
	PLUGIN_MAKE_VERSION(0, 0, 0, 1),
	"PolarSSL Interface for Miranda IM",
	"the_leech, borkra",
	"borkra@miranda-im.org",
	"Copyright© 2009-2012 Piotr Piasutski, Boris Krasnovskiy",
	"http://code.google.com/p/mirandaimplugins/downloads/list",
	0,		//not transient
	DEFMOD_SSL,		//replaces SSL module
	// {5FA5FA8B-4C0B-4772-BBE8-D0DA06600154}
	{ 0x5fa5fa8b, 0x4c0b, 0x4772, { 0xbb, 0xe8, 0xd0, 0xda, 0x6, 0x60, 0x1, 0x54 } }
};

const PLUGININFOEX* GetPluginInfoEx(void) { return &pluginInfoEx; }


extern "C" __declspec(dllexport) const PLUGININFOEX* MirandaPluginInfoEx(DWORD mirVersion) 
{
	return &pluginInfoEx;
}

// {E6DFFABC-9F36-4a4a-810C-323707A44AFA}
#define MIID_SSL  { 0xe6dffabc, 0x9f36, 0x4a4a, { 0x81, 0xc, 0x32, 0x37, 0x7, 0xa4, 0x4a, 0xfa } }

// MirandaPluginInterfaces - returns the protocol interface to the core
static const MUUID interfaces[] = { MIID_SSL, MIID_LAST };
extern "C" __declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}

PLUGINLINK *pluginLink;
HANDLE hService;
MM_INTERFACE  mmi;

INT_PTR GetSslApi(WPARAM, LPARAM lParam);

extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink = link;

	mir_getMMI(&mmi);

	hService = CreateServiceFunction(MS_SYSTEM_GET_SI, GetSslApi);
	
	return 0;
}


extern "C" int __declspec(dllexport) Unload(void)
{
	DestroyServiceFunction(hService);

	return 0;
}


extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /*lpvReserved*/)
{
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls(hinstDLL);
			break;

		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}


typedef enum
{
	sockOpen,
	sockClosed,
	sockError
} SocketState;

struct SslHandle
{
	SOCKET s;
	havege_state hs;
	ssl_context ssl;
	ssl_session ssn;
 
	unsigned char *pbRecDataBuf;
	int cbRecDataBuf;
	int sbRecDataBuf;

	SocketState state; 
};


void NetlibSslFree(SslHandle *ssl)
{
	if (ssl == NULL) return;

	ssl_free(&ssl->ssl);
	mir_free(ssl->pbRecDataBuf);
	memset(ssl, 0, sizeof(SslHandle));
	mir_free(ssl);
}

BOOL NetlibSslPending(SslHandle *ssl)
{
	return ssl != NULL && (ssl->cbRecDataBuf != 0 || ssl_get_bytes_avail(&ssl->ssl));
}

int net_recv(void *ctx, unsigned char *buf, int len) 
{
	int cbData;
	cbData = recv((SOCKET)ctx, (char*)buf, len, 0);
	if (cbData == SOCKET_ERROR) 
		return POLARSSL_ERR_NET_RECV_FAILED; 
	else if (cbData == 0)
		return POLARSSL_ERR_NET_CONN_RESET; 
	return cbData; 
}

int net_send(void *ctx, unsigned char *buf, int len) 
{
	int cbData;
	cbData = send((SOCKET)ctx, (const char *)buf, len, 0);
	if (cbData == SOCKET_ERROR || cbData == 0)
		return POLARSSL_ERR_NET_SEND_FAILED;
	return cbData; 
}

SslHandle *NetlibSslConnect(SOCKET s, const char* host)
{
	SslHandle *ssl = (SslHandle*)mir_calloc(sizeof(SslHandle));
	ssl->s = s;
	if (ssl_init(&ssl->ssl) != 0)
	{
		memset(ssl, 0, sizeof(SslHandle));
		mir_free(ssl);
		return NULL;
	}

	havege_init(&ssl->hs);
	memset(&ssl->ssn, 0, sizeof(ssl_session));
	ssl_set_endpoint(&ssl->ssl, SSL_IS_CLIENT);
	ssl_set_authmode(&ssl->ssl, SSL_VERIFY_NONE);
	ssl_set_rng(&ssl->ssl, havege_random, &ssl->hs);
	ssl_set_bio(&ssl->ssl, net_recv, &ssl->s, net_send, &ssl->s);
	ssl_set_ciphersuites(&ssl->ssl, ssl_default_ciphersuites);
	ssl_set_hostname(&ssl->ssl, (char*)host);
	ssl_set_session(&ssl->ssl, 0, 6, &ssl->ssn); 

	int res = ssl_handshake(&ssl->ssl);
	if (res == POLARSSL_ERR_SSL_PRIVATE_KEY_REQUIRED)
	{
		ssl->ssl.state++;
		res = ssl_handshake(&ssl->ssl);
	}
	if (res)
	{
		if (ssl->ssl.state > SSL_SERVER_HELLO)
			ssl_close_notify(&ssl->ssl);

		mir_free(ssl);
		ssl = NULL;
	}
	return ssl;
}

void NetlibSslShutdown(SslHandle *ssl)
{
	if (ssl == NULL) return;

	ssl_close_notify(&ssl->ssl); 
}

static int NetlibSslReadSetResult(SslHandle *ssl, char *buf, int num, int peek) 
{
	if (ssl->cbRecDataBuf == 0)
	{
		return (ssl->state == sockClosed ? 0: SOCKET_ERROR);
	}

	int bytes = min(num, ssl->cbRecDataBuf);
	CopyMemory(buf, ssl->pbRecDataBuf, bytes);
	if (!peek) 
	{
		int rbytes = ssl->cbRecDataBuf - bytes;
		MoveMemory(ssl->pbRecDataBuf, ssl->pbRecDataBuf + bytes, rbytes);
		ssl->cbRecDataBuf = rbytes;
	}
	return bytes;
}

int NetlibSslRead(SslHandle *ssl, char *buf, int num, int peek)
{
	int cbData;
	if (ssl == NULL) return SOCKET_ERROR;
	if (num == 0) return 0;
	if (ssl->state != sockOpen || (ssl->cbRecDataBuf != 0 && (!peek || ssl->cbRecDataBuf >= num)))
	{
		return NetlibSslReadSetResult(ssl, buf, num, peek);
	}

	if (ssl->sbRecDataBuf < num) 
	{
		ssl->sbRecDataBuf = num;
		ssl->pbRecDataBuf = (unsigned char*)mir_realloc(ssl->pbRecDataBuf, ssl->sbRecDataBuf);
	}
	if (peek && !ssl_get_bytes_avail(&ssl->ssl))
	{
		TIMEVAL tv = {0};
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(ssl->s, &fd);
		cbData = select(1, &fd, NULL, NULL, &tv);

		if (cbData == SOCKET_ERROR)
		{
			ssl->state = sockError;
			return NetlibSslReadSetResult(ssl, buf, num, peek);
		}
		if (cbData == 0 && ssl->cbRecDataBuf)
		{
			return NetlibSslReadSetResult(ssl, buf, num, peek);
		}
	}
	int bytes = min(num, ssl->sbRecDataBuf - ssl->cbRecDataBuf);
	int ret = ssl_read(&ssl->ssl, ssl->pbRecDataBuf + ssl->cbRecDataBuf, bytes); 
	if (ret >= 0)
		ssl->cbRecDataBuf += ret;
	else if (ret == POLARSSL_ERR_NET_CONN_RESET)
		ssl->state = sockClosed;
	else
		ssl->state = sockError;

	return NetlibSslReadSetResult(ssl, buf, num, peek);
}

int NetlibSslWrite(SslHandle *ssl, const char *buf, int num)
{
	int ret = ssl_write(&ssl->ssl, (unsigned char*)buf, num) ;
	if (ret <= 0)
		return SOCKET_ERROR;
	return num;
}

INT_PTR GetSslApi(WPARAM, LPARAM lParam)
{
	SSL_API* si = (SSL_API*)lParam;
	if (si == NULL) return FALSE;

	if (si->cbSize < sizeof(SSL_API))
		return FALSE;

	si->connect  = (HSSL (__cdecl *)(SOCKET,const char *,int))NetlibSslConnect;
	si->pending  = (BOOL (__cdecl *)(HSSL))NetlibSslPending;
	si->read     = (int  (__cdecl *)(HSSL,char *,int,int))NetlibSslRead;
	si->write    = (int  (__cdecl *)(HSSL,const char *,int))NetlibSslWrite;
	si->shutdown = (void (__cdecl *)(HSSL))NetlibSslShutdown;
	si->sfree    = (void (__cdecl *)(HSSL))NetlibSslFree;

	return TRUE;
}


extern "C" unsigned long hardclock( void )
{
#ifdef __GNUC__
	asm("rdtsc");
#elif _MSC_VER < 1400
	unsigned long tsc;
	__asm   rdtsc
	__asm   mov  [tsc], eax
	return tsc; 
#else
	return (unsigned long)__rdtsc();
#endif
}
