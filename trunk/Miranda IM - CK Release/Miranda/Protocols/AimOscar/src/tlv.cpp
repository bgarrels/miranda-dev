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
#include "tlv.h"

TLV::TLV(char* buf)
{
    type_=_htons((*(unsigned short*)&buf[0]));
    length_=_htons((*(unsigned short*)&buf[2]));
    if (length_ > 0)
    {
        value_=(char*)mir_alloc(length_+1);
        memcpy(value_,&buf[4],length_);
    }
    else
        value_= NULL;
}

TLV::TLV(unsigned short type, unsigned short length, const char* value)
{
    type_ = type;
    length_ = length;
    if(length_ > 0)
    {
        value_ = (char*)mir_alloc(length_+1);
        memcpy(value_, value, length_);
    }
    else
        value_= NULL;
}

TLV::~TLV()
{
    mir_free(value_);
}

unsigned short TLV::ushort(int pos)
{
    return _htons(*(unsigned short*)&value_[pos]);
}

unsigned long TLV::ulong(int pos)
{
    return _htonl(*(unsigned long*)&value_[pos]);
}

unsigned __int64 TLV::u64(int pos)
{
    return _htonl64(*(unsigned __int64*)&value_[pos]);
}

unsigned char TLV::ubyte(int pos)
{
    return value_[pos];
}

char* TLV::part(int pos, int length)//returns part of the tlv value
{
    if ((pos + length) > length_) return NULL;

    char* value = (char*)mir_alloc(length + 2);
    memcpy(value, &value_[pos], length);
    value[length]   = '\0';
    value[length+1] = '\0';
    
    return value;
}

char* TLV::dupw(void) 
{ 
    wchar_t *str = (wchar_t*)part(0, length_);
    wcs_htons(str);

    char* stru = mir_utf8encodeW(str);
    mir_free(str);

    return stru; 
}


unsigned short TLV::whole(char* buf)//returns the whole tlv
{
    *(unsigned short*)buf = _htons(type_);
    *(unsigned short*)&buf[2] = _htons(length_);
    memcpy(&buf[4], value_, length_);
    return length_ + TLV_HEADER_SIZE;
}
