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
#include "packets.h"

int aim_writesnac(unsigned short service, unsigned short subgroup,unsigned short &offset, char* out, unsigned short id)
{
	snac_header *snac = (snac_header*)&out[offset];
	snac->service=_htons(service);
	snac->subgroup=_htons(subgroup);
	snac->flags=0;
	snac->request_id[0]=_htons(id);
	snac->request_id[1]=_htons(subgroup);
	offset+=sizeof(snac_header);
	return 0;
}

int aim_writetlv(unsigned short type,unsigned short length, const char* value,unsigned short &offset,char* out)
{
	TLV tlv(type,length,value);
	offset += tlv.whole(&out[offset]);
	return 0;
}

int aim_writetlvchar(unsigned short type, unsigned char value, unsigned short &offset, char* out)
{
	return aim_writetlv(type, sizeof(value), (char*)&value, offset, out);
}


int aim_writetlvshort(unsigned short type, unsigned short value, unsigned short &offset, char* out)
{
	value = _htons(value);
	return aim_writetlv(type, sizeof(value), (char*)&value, offset, out);
}


int aim_writetlvlong(unsigned short type, unsigned long value, unsigned short &offset, char* out)
{
	value = _htonl(value);
	return aim_writetlv(type, sizeof(value), (char*)&value, offset, out);
}

int aim_writetlvlong64(unsigned short type, unsigned __int64 value, unsigned short &offset, char* out)
{
	value = _htonl64(value);
	return aim_writetlv(type, sizeof(value), (char*)&value, offset, out);
}


int CAimProto::aim_sendflap(HANDLE hServerConn, char type,unsigned short length,const char *buf, unsigned short &seqno)
{
	EnterCriticalSection(&SendingMutex);
	const int slen = FLAP_SIZE + length;
	char* obuf = (char*)_malloca(slen);
	flap_header *flap = (flap_header*)obuf;
	flap->ast = '*';
	flap->type = type;
	flap->seqno = _htons(seqno++);
	flap->len = _htons(length);
	memcpy(&obuf[FLAP_SIZE], buf, length);
	int rlen= Netlib_Send(hServerConn, obuf, slen, 0);
	if (rlen == SOCKET_ERROR) seqno--;
	LeaveCriticalSection(&SendingMutex);
	return rlen >= 0 ? 0 : -1;
}

void aim_writefamily(const char *buf,unsigned short &offset,char* out)
{
	memcpy(&out[offset],buf,4);
	offset+=4;
}

void aim_writechar(unsigned char val, unsigned short &offset,char* out)
{
	out[offset++] = val;
}

void aim_writeshort(unsigned short val, unsigned short &offset,char* out)
{
	out[offset++] = (char)(val >> 8);
	out[offset++] = (char)(val & 0xFF);
}

void aim_writelong(unsigned long val, unsigned short &offset,char* out)
{
	out[offset++] = (char)(val >> 24);
	out[offset++] = (char)((val >> 16) & 0xFF);
	out[offset++] = (char)((val >> 8) & 0xFF);
	out[offset++] = (char)(val & 0xFF);
}

void aim_writegeneric(unsigned short size,const char *buf,unsigned short &offset,char* out)
{
	memcpy(&out[offset],buf,size);
	offset+=size;
}

void aim_writebartid(unsigned short type, unsigned char flags, unsigned short size,const char *buf,unsigned short &offset,char* out)
{
	out[offset++]=(unsigned char)(type >> 8);
	out[offset++]=(unsigned char)(type & 0xff);
	out[offset++]=flags;
	out[offset++]=(char)size;
	memcpy(&out[offset],buf,size);
	offset+=size;
}
