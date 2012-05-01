/*
AIM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			(C) 2008-2012 Boris Krasnovskiy
			(C) 2005-2006 Aaron Myles Landwehr

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

#include "../aim.h"
#include "snac.h"
#include "packets.h"

SNAC::SNAC(char* buf,unsigned short length)
{
	service_=_htons((*(unsigned short*)&buf[0]));
	subgroup_=_htons((*(unsigned short*)&buf[2]));
	flags_=_htons((*(unsigned short*)&buf[4]));
	idh_=_htons((*(unsigned short*)&buf[6]));
	id_=_htons((*(unsigned short*)&buf[8]));
	value_=&buf[SNAC_SIZE];
	length_=length;
}
int SNAC::cmp(unsigned short service)
{
	if(service_==service)
		return 1;
	else 
		return 0;
}
int SNAC::subcmp(unsigned short subgroup)
{
	if(subgroup_==subgroup)
		return 1;
	else 
		return 0;
}
unsigned short SNAC::ushort(int pos)
{
	return _htons(*(unsigned short*)&value_[pos]);
}
unsigned long SNAC::ulong(int pos)
{
	return _htonl(*(unsigned long*)&value_[pos]);
}
unsigned char SNAC::ubyte(int pos)
{
	return value_[pos];
}
char* SNAC::part(int pos, int length)
{
	char* value = (char*)mir_alloc(length+1);
	memcpy(value, &value_[pos], length);
	value[length] = '\0';
	return value;
}
