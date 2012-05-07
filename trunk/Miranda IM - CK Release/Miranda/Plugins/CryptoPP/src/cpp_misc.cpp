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


int __cdecl cpp_get_features(HANDLE context) {
    pCNTX ptr = get_context_on_id(context); if(!ptr) return 0;
    return ptr->features;
}


int __cdecl cpp_get_error(HANDLE context) {
    pCNTX ptr = get_context_on_id(context); if(!ptr) return 0;
    return ptr->error;
}


int __cdecl cpp_get_version(void) {
    return __VERSION_DWORD;
}


BOOL cpp_get_simdata(HANDLE context, pCNTX *ptr, pSIMDATA *p) {
    *ptr = get_context_on_id(context);
//    if(!*ptr || !(*ptr)->pdata || (*ptr)->mode&(MODE_PGP|MODE_GPG|MODE_RSA)) return FALSE;
    if(!*ptr || (*ptr)->mode&(MODE_PGP|MODE_GPG|MODE_RSA)) return FALSE;
    *p = (pSIMDATA) cpp_alloc_pdata(*ptr);
    return TRUE;
}


int __cdecl cpp_size_keyx(void) {
    return(Tiger::DIGESTSIZE+2);
}


void __cdecl cpp_get_keyx(HANDLE context, BYTE *key) {
    pCNTX ptr; pSIMDATA p; if(!cpp_get_simdata(context,&ptr,&p)) return;
    memcpy(key,p->KeyX,Tiger::DIGESTSIZE);
    memcpy(key+Tiger::DIGESTSIZE,&ptr->features,2);
}


void __cdecl cpp_set_keyx(HANDLE context, BYTE *key) {
    pCNTX ptr; pSIMDATA p; if(!cpp_get_simdata(context,&ptr,&p)) return;
    SAFE_FREE(p->PubA);
    SAFE_FREE(p->KeyA);
    SAFE_FREE(p->KeyB);
    SAFE_FREE(p->KeyX);
    p->KeyX = (PBYTE) malloc(Tiger::DIGESTSIZE+2);
    memcpy(p->KeyX,key,Tiger::DIGESTSIZE);
    memcpy(&ptr->features,key+Tiger::DIGESTSIZE,2);
}


void __cdecl cpp_get_keyp(HANDLE context, BYTE *key) {
    pCNTX ptr; pSIMDATA p; if(!cpp_get_simdata(context,&ptr,&p)) return;
    memcpy(key,p->KeyP,Tiger::DIGESTSIZE);
}


int __cdecl cpp_size_keyp(void) {
    return(Tiger::DIGESTSIZE);
}


void __cdecl cpp_set_keyp(HANDLE context, BYTE *key) {
    pCNTX ptr; pSIMDATA p; if(!cpp_get_simdata(context,&ptr,&p)) return;
    SAFE_FREE(p->KeyP);
    p->KeyP = (PBYTE) malloc(Tiger::DIGESTSIZE);
    memcpy(p->KeyP,key,Tiger::DIGESTSIZE);
}


int __cdecl cpp_keya(HANDLE context) {
    pCNTX ptr; pSIMDATA p; if(!cpp_get_simdata(context,&ptr,&p)) return 0;
    return p->KeyA!=NULL;
}


int __cdecl cpp_keyb(HANDLE context) {
    pCNTX ptr; pSIMDATA p; if(!cpp_get_simdata(context,&ptr,&p)) return 0;
    return p->KeyB!=NULL;
}


int __cdecl cpp_keyx(HANDLE context) {
    pCNTX ptr; pSIMDATA p; if(!cpp_get_simdata(context,&ptr,&p)) return 0;
    return p->KeyX!=NULL;
}


int __cdecl cpp_keyp(HANDLE context) {
    pCNTX ptr; pSIMDATA p; if(!cpp_get_simdata(context,&ptr,&p)) return 0;
    return p->KeyP!=NULL;
}


// EOF
