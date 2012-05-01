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
#include "flap.h"

FLAP::FLAP(char* buf,int num_bytes)
{
	if(FLAP_SIZE>num_bytes)
	{
		length_=0;
	}
	else
	{
		length_=_htons((*(unsigned short*)&buf[4]));
		if(FLAP_SIZE+length_>num_bytes)
		{
			length_=0;
		}
		else
		{
			type_=buf[1];
			value_=&buf[FLAP_SIZE];
		}
	}
}
unsigned short FLAP::len()
{
	return length_;
}
unsigned short FLAP::snaclen()
{
	return length_-10;
}
int FLAP::cmp(unsigned short type)
{
	if(type_==type)
		return 1;
	else 
		return 0;
}
char* FLAP::val()
{
	return value_;
}
