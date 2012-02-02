// Copyright � 2010 SecureIM developers (baloo and others), sss
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "commonheaders.h"

extern bool bMetaContacts;

bool metaIsProtoMetaContacts(HANDLE hContact)
{
    if(bMetaContacts) {
    	LPSTR proto = (LPSTR)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
    	if( proto && strcmp(proto,"MetaContacts")==0 ) {
    		return true;
    	}
    }
    return false;
}


bool metaIsDefaultSubContact(HANDLE hContact) 
{

    if(bMetaContacts)
		return (HANDLE)CallService(MS_MC_GETDEFAULTCONTACT,(WPARAM)CallService(MS_MC_GETMETACONTACT,(WPARAM)hContact,0),0)==hContact;
    return false;
}


HANDLE metaGetContact(HANDLE hContact) 
{
    if(bMetaContacts)
		if(metaIsSubcontact(hContact))
			return (HANDLE)CallService(MS_MC_GETMETACONTACT,(WPARAM)hContact,0);
    return hContact;
}
bool metaIsSubcontact(HANDLE hContact)
{
	if(bMetaContacts)
		return (HANDLE)CallService(MS_MC_GETMETACONTACT,(WPARAM)hContact,0);
    return false;
}


HANDLE metaGetMostOnline(HANDLE hContact) 
{

    if(bMetaContacts)
		if(metaIsProtoMetaContacts(hContact))
			return (HANDLE)CallService(MS_MC_GETMOSTONLINECONTACT,(WPARAM)hContact,0);
    return hContact;
}
HANDLE metaGetDefault(HANDLE hContact) 
{

    if(bMetaContacts)
		if(metaIsProtoMetaContacts(hContact))
			return (HANDLE)CallService(MS_MC_GETDEFAULTCONTACT,(WPARAM)hContact,0);
    return hContact;
}


DWORD metaGetContactsNum(HANDLE hContact)
{
	if(bMetaContacts)
		return CallService(MS_MC_GETNUMCONTACTS, (WPARAM)hContact, 0);
	return 0;
}
HANDLE metaGetSubcontact(HANDLE hContact, int num)
{
	if(bMetaContacts)
		return (HANDLE)CallService(MS_MC_GETSUBCONTACT, (WPARAM)hContact, (LPARAM)num);
	return 0;
}


