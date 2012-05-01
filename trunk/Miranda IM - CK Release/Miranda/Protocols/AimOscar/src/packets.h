#ifndef PACKETS_H
#define PACKETS_H

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

struct flap_header
{
	unsigned char ast;
	unsigned char type;
	unsigned short seqno;
	unsigned short len;
};
struct snac_header
{
	unsigned short service;
	unsigned short subgroup;
	unsigned short flags;
	unsigned short request_id[2];
};

inline unsigned short _htons(unsigned short s)
{
	return s>>8|s<<8;
}

inline unsigned long _htonl(unsigned long s)
{
	return s<<24|(s&0xff00)<<8|((s>>8)&0xff00)|s>>24;
}

inline unsigned __int64 _htonl64(unsigned __int64 s)
{
	return (unsigned __int64)_htonl(s & 0xffffffff) << 32 | _htonl(s >> 32);
}


int    aim_writesnac(unsigned short service, unsigned short subgroup,unsigned short &offset,char* out, unsigned short id=0);
int    aim_writetlv(unsigned short type,unsigned short size, const char* value,unsigned short &offset,char* out);
int    aim_writetlvchar(unsigned short type, unsigned char value, unsigned short &offset, char* out);
int    aim_writetlvshort(unsigned short type, unsigned short value, unsigned short &offset, char* out);
int    aim_writetlvlong(unsigned short type, unsigned long value, unsigned short &offset, char* out);
int    aim_writetlvlong64(unsigned short type, unsigned __int64 value, unsigned short &offset, char* out);
void   aim_writefamily(const char *buf,unsigned short &offset,char* out);
void   aim_writegeneric(unsigned short size,const char *buf,unsigned short &offset,char* out);
void   aim_writebartid(unsigned short type, unsigned char flags, unsigned short size,const char *buf,unsigned short &offset,char* out);
void   aim_writechar(unsigned char val, unsigned short &offset,char* out);
void   aim_writeshort(unsigned short val, unsigned short &offset,char* out);
void   aim_writelong(unsigned long val, unsigned short &offset,char* out);

#endif
