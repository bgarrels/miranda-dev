// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin ÷berg, Sam Kothari, Robert Rainwater
// Copyright © 2004,2005,2006,2007 Joe Kucera
// Copyright © 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
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
//
// -----------------------------------------------------------------------------
//
// File name      : $Source$
// Revision       : $Revision: 36 $
// Last change on : $Date: 2007-08-05 03:45:10 +0300 (–í—Å, 05 –∞–≤–≥ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#ifndef __ICQ_PACKET_H
#define __ICQ_PACKET_H

typedef unsigned char  BYTE;
typedef unsigned short  WORD;
typedef unsigned long  DWORD;

/*---------* Structures *--------------*/

typedef struct icq_packet_s
{
    WORD wPlace;
    BYTE nChannel;
    WORD wLen;
    BYTE *pData;
} icq_packet;

/*---------* Functions *---------------*/
WORD generate_flap_sequence();
void __fastcall init_generic_packet(icq_packet* pPacket, WORD wHeaderLen);
void write_httphdr(icq_packet *d, WORD wType, DWORD dwSeq);
void __fastcall write_flap(icq_packet *, BYTE);
void __fastcall serverPacketInit(icq_packet *, WORD);
void __fastcall directPacketInit(icq_packet *, DWORD);

void __fastcall serverCookieInit(icq_packet *, BYTE *, WORD);

void __fastcall packByte(icq_packet *, BYTE);
void __fastcall packWord(icq_packet *, WORD);
void __fastcall packDWord(icq_packet *, DWORD);
void __fastcall packQWord(icq_packet *, DWORD64);
void packTLV(icq_packet *pPacket, WORD wType, WORD wLength, const char *pbyValue);
void packTLVWord(icq_packet *pPacket, WORD wType, WORD wData);
void packTLVDWord(icq_packet *pPacket, WORD wType, DWORD dwData);

void packBuffer(icq_packet* pPacket, const char* pbyBuffer, WORD wLength);
//void packLEWordSizedBuffer(icq_packet* pPacket, const BYTE* pbyBuffer, WORD wLength);
int __fastcall getUINLen(DWORD dwUin);
int __fastcall getUIDLen(DWORD dwUin, char* szUid);
void __fastcall packUIN(icq_packet *pPacket, DWORD dwUin);
void __fastcall packUID(icq_packet *pPacket, DWORD dwUin, char* szUid);
void packFNACHeader(icq_packet *d, WORD wFamily, WORD wSubtype);
void packFNACHeaderFull(icq_packet *d, WORD wFamily, WORD wSubtype, WORD wFlags, DWORD wSeq);
void packFNACHeaderFullwVersion(icq_packet* pPacket, WORD wFamily, WORD wSubtype, WORD wFlags, DWORD dwSeq, WORD wVersion);

void __fastcall packLEWord(icq_packet *, WORD);
void __fastcall packLEDWord(icq_packet *, DWORD);

void packTLVLNTS(PBYTE *buf, int *bufpos, const char *str, WORD wType);

void ppackByte(PBYTE *buf,int *buflen,BYTE b);
void ppackLEWord(PBYTE *buf,int *buflen,WORD w);
void ppackLEDWord(PBYTE *buf,int *buflen,DWORD d);
void ppackLELNTS(PBYTE *buf, int *buflen, const char *str);
void ppackLELNTSfromDB(PBYTE *buf, int *buflen, const char *szSetting);

void ppackWord(PBYTE *buf, int *buflen, WORD wValue);

void ppackBuffer(PBYTE *buf, int *buflen, WORD wLength, const BYTE *pbyValue);

void ppackTLV(PBYTE *buf, int *buflen, WORD wType, WORD wLength, const BYTE *pbyValue);
void ppackTLVByte(PBYTE *buf, int *buflen, BYTE b, WORD wType, BYTE always);
void ppackTLVWord(PBYTE *buf, int *buflen, WORD w, WORD wType, BYTE always);
void ppackTLVDWord(PBYTE *buf, int *buflen, DWORD d, WORD wType, BYTE always);
void ppackTLVDouble(PBYTE *buf, int *buflen, WORD wType, double dValue);
void ppackTLVUID(PBYTE *buf, int *buflen, WORD wType, DWORD dwUin, const char *szUid);
void packTLVUID(icq_packet *pPacket, WORD wType, DWORD dwUin, const char *szUid);
void ppackTLVLNTS(PBYTE *buf, int *buflen, const char *str, WORD wType, BYTE always);
void ppackTLVWordLNTS(PBYTE *buf, int *buflen, WORD w, const char *str, WORD wType, BYTE always);
void ppackTLVLNTSByte(PBYTE *buf, int *buflen, const char *str, BYTE b, WORD wType);

void ppackTLVLNTSfromDB(PBYTE *buf, int *buflen, const char *szSetting, WORD wType);
void ppackTLVWordLNTSfromDB(PBYTE *buf, int *buflen, WORD w, const char *szSetting, WORD wType);
void ppackTLVLNTSBytefromDB(PBYTE *buf, int *buflen, const char *szSetting, BYTE b, WORD wType);

int ppackTLVWordStringItemFromDB(PBYTE *buf, int *buflen, const char *szSetting, WORD wTypeID, WORD wTypeData, WORD wID);
void ppackTLVStringUtfFromDB(PBYTE *buf, int *buflen, const char *szSetting, WORD wType);
void ppackTLVStringFromDB(PBYTE *buf, int *buflen, const char *szSetting, WORD wType);
void ppackTLVDateFromDB(PBYTE *buf, int *buflen, const char *szSettingYear, const char *szSettingMonth, const char *szSettingDay, WORD wType);

void ppackTLVBlockItems(PBYTE *buf, int *buflen, WORD wType, int *nItems, PBYTE *pBlock, WORD *wLength, BOOL bSingleItem);
int ppackTLVWordStringUtfItemFromDB(PBYTE *buf, int *buflen, const char *szSetting, WORD wTypeID, WORD wTypeData, WORD wID);

void __fastcall unpackByte(unsigned char **, BYTE *);
void __fastcall unpackWord(unsigned char **, WORD *);
void __fastcall unpackDWord(unsigned char **, DWORD *);
void __fastcall unpackQWord(unsigned char **, DWORD64 *);
void unpackString(unsigned char **buf, char *string, WORD len);
void unpackWideString(unsigned char **buf, WCHAR *string, WORD len);
void unpackTypedTLV(unsigned char *, int, WORD, WORD *, WORD *, char **);
BOOL unpackUID(unsigned char** ppBuf, WORD* pwLen, DWORD *pdwUIN, uid_str* ppszUID);

void __fastcall unpackLEWord(unsigned char **, WORD *);
void __fastcall unpackLEDWord(unsigned char **, DWORD *);

#endif /* __ICQ_PACKET_H */
