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


BOOL isProtoMetaContacts(HANDLE hContact) {
    if(bMetaContacts) {
    	LPSTR proto = (LPSTR)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
    	if( proto && strcmp(proto,"MetaContacts")==0 ) {
    		return true;
    	}
    }
//    for(int j=0;j<clist_cnt;j++)
//	if(clist[j].hContact==hContact && clist[j].proto->inspecting)
//	    return strstr(clist[j].proto->name,"MetaContacts")!=NULL;
    return false;
}


BOOL isDefaultSubContact(HANDLE hContact) {

    if(bMetaContacts) {
	return (HANDLE)CallService(MS_MC_GETDEFAULTCONTACT,(WPARAM)CallService(MS_MC_GETMETACONTACT,(WPARAM)hContact,0),0)==hContact;
    }
    return false;
}


HANDLE getMetaContact(HANDLE hContact) {

    if(bMetaContacts) {
	return (HANDLE)CallService(MS_MC_GETMETACONTACT,(WPARAM)hContact,0);
    }
    return 0;
}


HANDLE getMostOnline(HANDLE hContact) {

    if(bMetaContacts) {
	return (HANDLE)CallService(MS_MC_GETMOSTONLINECONTACT,(WPARAM)hContact,0);
    }
    return 0;
}


// remove all secureim connections on subcontacts
void DeinitMetaContact(HANDLE hContact) {

	HANDLE hMetaContact = isProtoMetaContacts(hContact) ? hContact : getMetaContact(hContact);

	if( hMetaContact ) {
		for(int i=0;i<CallService(MS_MC_GETNUMCONTACTS,(WPARAM)hMetaContact,0);i++) {
			HANDLE hSubContact = (HANDLE)CallService(MS_MC_GETSUBCONTACT,(WPARAM)hMetaContact,(LPARAM)i);
			if( hSubContact && isContactSecured(hSubContact)&SECURED ) {
				CallContactService(hSubContact,PSS_MESSAGE,(WPARAM)PREF_METANODB,(LPARAM)SIG_DEIN);
			}
		}
	}
}

// EOF
