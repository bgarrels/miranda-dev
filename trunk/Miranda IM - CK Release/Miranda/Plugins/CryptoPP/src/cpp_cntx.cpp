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


list<pCNTX> CL; // CL.size() CL.empty()

HANDLE thread_timeout = 0;

unsigned __stdcall sttTimeoutThread(LPVOID);


// get context data on context id
pCNTX get_context_on_id(HANDLE context) {

    if(	!thread_timeout ) {
	unsigned int tID;
	thread_timeout = (HANDLE) _beginthreadex(NULL, 0, sttTimeoutThread, NULL, 0, &tID);
    }

    if( context ) {
	pCNTX cntx = (pCNTX) context;
	if( cntx->header == HEADER && cntx->footer == FOOTER )
		return cntx;
#if defined(_DEBUG) || defined(NETLIB_LOG)
	else
		Sent_NetLog("get_context_on_id: corrupted context %08X", cntx);
#endif
    }
    return NULL;
}

/*
pCNTX get_context_on_id(HANDLE context) {
	return get_context_on_id((int)context);
}
*/

// create context, return context id
HANDLE __cdecl cpp_create_context(int mode) {

	list<pCNTX>::iterator i;
	pCNTX cntx = NULL;

	EnterCriticalSection(&localContextMutex);

	if( !CL.empty() ) {
		for(i=CL.begin(); i!=CL.end(); ++i) { // ищем пустой
			if( (*i)->header==EMPTYH && (*i)->footer==EMPTYH ) {
	    	    cntx = (pCNTX) *i;
	    	    break;
			}
		}
	}

	if( !cntx ) { // не нашли - создаем новый
	    cntx = (pCNTX) malloc(sizeof(CNTX));
	    CL.push_back(cntx); // добавили в конец списка
	}

	memset(cntx,0,sizeof(CNTX)); // очищаем выделенный блок
	cntx->header = HEADER;
	cntx->footer = FOOTER;
	cntx->mode = mode;

	LeaveCriticalSection(&localContextMutex);

	return (HANDLE)cntx;
}


// delete context
void __cdecl cpp_delete_context(HANDLE context) {

	pCNTX tmp = get_context_on_id(context);
	if(tmp) { // помечаем на удаление
		tmp->deleted = gettime()+10; // будет удален через 10 секунд
	}
}


// reset context
void __cdecl cpp_reset_context(HANDLE context) {

	pCNTX tmp = get_context_on_id(context);
	if(tmp)	cpp_free_keys(tmp);
}


// allocate pdata
PBYTE cpp_alloc_pdata(pCNTX ptr) {
	if( !ptr->pdata ) {
	    if( ptr->mode & MODE_PGP ) {
			ptr->pdata = (PBYTE) malloc(sizeof(PGPDATA));
			memset(ptr->pdata,0,sizeof(PGPDATA));
	    }
	    else
	    if( ptr->mode & MODE_GPG ) {
			ptr->pdata = (PBYTE) malloc(sizeof(GPGDATA));
			memset(ptr->pdata,0,sizeof(GPGDATA));
	    }
	    else
	    if( ptr->mode & MODE_RSA ) {
			rsa_alloc(ptr);
	    }
	    else {
			ptr->pdata = (PBYTE) malloc(sizeof(SIMDATA));
			memset(ptr->pdata,0,sizeof(SIMDATA));
	    }
	}
	return ptr->pdata;
}


// free memory from keys
void cpp_free_keys(pCNTX ptr) {

	SAFE_FREE(ptr->tmp);
	cpp_alloc_pdata(ptr);
	if( ptr->mode & MODE_PGP ) {
		pPGPDATA p = (pPGPDATA) ptr->pdata;
		SAFE_FREE(p->pgpKeyID);
		SAFE_FREE(p->pgpKey);
		SAFE_FREE(ptr->pdata);
	}
	else
	if( ptr->mode & MODE_GPG ) {
		pGPGDATA p = (pGPGDATA) ptr->pdata;
		SAFE_FREE(p->gpgKeyID);
		SAFE_FREE(ptr->pdata);
	}
	else
	if( ptr->mode & MODE_RSA ) {
		rsa_free(ptr);
		SAFE_DELETE(ptr->pdata);
	}
	else {
		pSIMDATA p = (pSIMDATA) ptr->pdata;
		SAFE_FREE(p->PubA);
		SAFE_FREE(p->KeyA);
		SAFE_FREE(p->KeyB);
		SAFE_FREE(p->KeyX);
		SAFE_FREE(p->KeyP);
		SAFE_DELETE(p->dh);
		SAFE_FREE(ptr->pdata);
	}
}


// search not established RSA/AES contexts && clear deleted contexts
unsigned __stdcall sttTimeoutThread( LPVOID ) {

	list<pCNTX>::iterator i;
	while(1) {
		Sleep( 1000 ); // раз в секунду
		if( CL.empty() ) continue;
		u_int time = gettime();
		// пробегаем все контексты
		EnterCriticalSection(&localContextMutex);
	    for(i=CL.begin(); i!=CL.end(); ++i) {
	    	pCNTX tmp = *i;
			if( tmp->header!=HEADER || tmp->footer!=FOOTER ) continue;
			// пропускаем приватные ключи
	    	if( tmp->mode&MODE_PRIV_KEY ) continue;
	    	else
			if( tmp->deleted && tmp->deleted < time ) {
				// удалить помеченный для удаления контекст
				cpp_free_keys(tmp);
				tmp->deleted = 0;
				tmp->header = tmp->footer = EMPTYH;
	    	}
	    	else
			if( tmp->mode&MODE_RSA && tmp->pdata ) {
				// проверяем не протухло ли соединение
				pRSADATA p = (pRSADATA) tmp->pdata;
				if( p->time && p->time < time ) {
					rsa_timeout((HANDLE)tmp,p);
				}
	    	}
	    } // for
		LeaveCriticalSection(&localContextMutex);
	} //while
}


// EOF
