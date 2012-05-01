#ifndef __TLV_H
#define __TLV_H

/*
ICQ plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright © 2001-2004 Richard Hughes
			Copyright © 2002-2004 Martin Öberg, Sam Kothari, Robert Rainwater
			Copyright © 2004-2010 Joe Kucera, Bio
			Copyright © 2010-2012 Borkra, Georg Hazan

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

* Structure 
*/

struct oscar_tlv
{
  WORD wType;
  WORD wLen;
  BYTE *pData;
};


struct oscar_tlv_chain
{
  oscar_tlv tlv;
  oscar_tlv_chain *next;

  WORD getChainLength();

  oscar_tlv* getTLV(WORD wType, WORD wIndex);
  oscar_tlv* putTLV(WORD wType, WORD wLen, BYTE *pData, BOOL bReplace);
  oscar_tlv_chain* removeTLV(oscar_tlv *tlv);
  WORD getLength(WORD wType, WORD wIndex);

  DWORD getDWord(WORD wType, WORD wIndex);
  WORD getWord(WORD wType, WORD wIndex);
  BYTE getByte(WORD wType, WORD wIndex);
  int getNumber(WORD wType, WORD wIndex);
  double getDouble(WORD wType, WORD wIndex);
  char* getString(WORD wType, WORD wIndex);
};


struct oscar_tlv_record_list
{
  oscar_tlv_chain *item;
  oscar_tlv_record_list *next;

  oscar_tlv_chain* getRecordByTLV(WORD wType, int nValue);
};

/*---------* Functions *---------------*/
oscar_tlv_chain* readIntoTLVChain(BYTE **buf, WORD wLen, int maxTlvs);
void disposeChain(oscar_tlv_chain** chain);

oscar_tlv_record_list* readIntoTLVRecordList(BYTE **buf, WORD wLen, int nCount);
void disposeRecordList(oscar_tlv_record_list** list);


#endif /* __TLV_H */
