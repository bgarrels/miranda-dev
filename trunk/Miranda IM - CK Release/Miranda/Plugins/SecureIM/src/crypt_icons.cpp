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

#include "commonheaders.h"


typedef struct {
	HICON icon;
	SHORT mode;
} ICON_CACHE;


ICON_CACHE *ICONS_CACHE = NULL;
int icons_cache = 0;


// преобразует mode в HICON который НЕ НУЖНО разрушать в конце
HICON mode2icon(int mode,int type) {

	int m=mode&0x0f,s=(mode&SECURED)>>4,i; // разобрали на части - режим и состояние
	HICON icon;

	if( icons_cache ) {
		for(i=0;i<icons_cache;i++) {
			if( ICONS_CACHE[i].mode == ((type<<8) | mode) ) {
				return ICONS_CACHE[i].icon;
			}
		}
	}

	i=s;
	switch(type) {
	case 1: i+=IEC_CL_DIS; break;
	case 2: i+=ICO_CM_DIS; break;
	case 3: i+=ICO_MW_DIS; break;
	}

	if( type==1 ) {
		icon = BindOverlayIcon(g_hIEC[i],g_hICO[ICO_OV_NAT+m]);
	}
	else {
		icon = BindOverlayIcon(g_hICO[i],g_hICO[ICO_OV_NAT+m]);
	}

	ICONS_CACHE = (ICON_CACHE*) mir_realloc(ICONS_CACHE,sizeof(ICON_CACHE)*(icons_cache+1));
	ICONS_CACHE[icons_cache].icon = icon;
	ICONS_CACHE[icons_cache].mode = (type<<8) | mode;
	icons_cache++;

	return icon;
}


// преобразует mode в HICON который НУЖНО разрушить в конце
HICON mode2icon2(int mode,int type) {
	return CopyIcon(mode2icon(mode,type));
}


// преобразует mode в IconExtraColumn который НЕ нужно разрушать в конце
IconExtraColumn mode2iec(int mode) {

	int m=mode&0x0f,s=(mode&SECURED)>>4; // разобрали на части - режим и состояние

	if( mode==-1 || (!s && !bASI && m!=MODE_PGP && m!=MODE_GPG) ) {
		return g_IEC[0]; // вернем пустое место
	}

	int i=1+m*IEC_CNT+IEC_CL_DIS+s;
	if( g_IEC[i].hImage==(HANDLE)-1 ) {
/*		g_hIEC[i] = mode2icon(mode,1);
		g_IEC[i].hImage = (HANDLE) CallService(MS_CLIST_EXTRA_ADD_ICON, (WPARAM)g_hIEC[i], (LPARAM)0);*/
		HICON icon = mode2icon(mode,1);
		g_IEC[i].hImage = (HANDLE) CallService(MS_CLIST_EXTRA_ADD_ICON, (WPARAM)icon, (LPARAM)0);
	}
	return g_IEC[i];
}


// обновляет иконки в clist и в messagew
void ShowStatusIcon(HANDLE hContact,int mode) {

	HANDLE hMC = getMetaContact(hContact);
	if( bADV || g_hCLIcon ) { // обновить иконки в clist
		if( mode!= -1 ) {
			IconExtraColumn iec=mode2iec(mode);
			if( g_hCLIcon ) {
				ExtraIcon_SetIcon(g_hCLIcon, hContact, iec.hImage);
				if( hMC )
				ExtraIcon_SetIcon(g_hCLIcon, hMC, iec.hImage);
			}
			else {
				CallService(MS_CLIST_EXTRA_SET_ICON, (WPARAM)hContact, (LPARAM)&iec);
				if( hMC )
				CallService(MS_CLIST_EXTRA_SET_ICON, (WPARAM)hMC, (LPARAM)&iec);
			}
		}
	}
	if( ServiceExists(MS_MSG_MODIFYICON) ) {  // обновить иконки в srmm
		StatusIconData sid;
		memset(&sid,0,sizeof(sid));
		sid.cbSize = sizeof(sid);
		sid.szModule = (char*)szModuleName;
		for(int i=MODE_NATIVE; i<MODE_CNT;i++) {
			sid.dwId = i;
			sid.flags = (mode&SECURED)?0:MBF_DISABLED;
			if( mode==-1 || (mode&0x0f)!=i || isChatRoom(hContact) )
				sid.flags |= MBF_HIDDEN;  // отключаем все ненужные иконки
			CallService(MS_MSG_MODIFYICON, (WPARAM)hContact, (LPARAM)&sid);
			if( hMC )
			CallService(MS_MSG_MODIFYICON, (WPARAM)hMC, (LPARAM)&sid);
		}
	}
}


void ShowStatusIcon(HANDLE hContact) {
	ShowStatusIcon(hContact,isContactSecured(hContact));
}


void ShowStatusIconNotify(HANDLE hContact) {
	int mode = isContactSecured(hContact);
	NotifyEventHooks(g_hEvent[(mode&SECURED)!=0], (WPARAM)hContact, 0);
	ShowStatusIcon(hContact,mode);
}


void RefreshContactListIcons(void) {

	HANDLE hContact;
//	CallService(MS_CLUI_LISTBEGINREBUILD,0,0);
	if( !g_hCLIcon ) {
	    hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
	    while (hContact) { // сначала все выключаем
		ShowStatusIcon(hContact,-1);
		hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0);
	    }
	    // менем местоположение иконки
	    for(int i=0;i<1+MODE_CNT*IEC_CNT;i++){
		g_IEC[i].ColumnType = bADV;
	    }
	}
	hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
	while (hContact) { // и снова зажигаем иконку
		if( isSecureProtocol(hContact) )
			ShowStatusIcon(hContact);
		hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0);
	}
//	CallService(MS_CLUI_LISTENDREBUILD,0,0);
}


// EOF
