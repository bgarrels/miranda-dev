// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin Öberg, Sam Kothari, Robert Rainwater
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
// Last change on : $Date: 2007-08-05 03:45:10 +0300 (Ð’Ñ, 05 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"

WORD generate_flap_sequence()
{
    DWORD n = rand(), s = 0, i;

    for (i = n; i >>= 3; s += i);

    return (WORD)((((0 - s) ^ (BYTE)n) & 7 ^ n) + 2);
}

void __fastcall init_generic_packet(icq_packet* pPacket, WORD wHeaderLen)
{
    pPacket->wPlace = 0;
    pPacket->wLen += wHeaderLen;
    pPacket->pData = (BYTE*)icq_alloc_zero(pPacket->wLen);
}



void write_httphdr(icq_packet* pPacket, WORD wType, DWORD dwSeq)
{
    init_generic_packet(pPacket, 14);

    packWord(pPacket, (WORD)(pPacket->wLen - 2));
    packWord(pPacket, HTTP_PROXY_VERSION);
    packWord(pPacket, wType);
    packDWord(pPacket, 0); // Flags?
    packDWord(pPacket, dwSeq); // Connection sequence ?
}



void __fastcall write_flap(icq_packet* pPacket, BYTE byFlapChannel)
{
    init_generic_packet(pPacket, 6);

    pPacket->nChannel = byFlapChannel;

    packByte(pPacket, FLAP_MARKER);
    packByte(pPacket, byFlapChannel);
    packWord(pPacket, 0);                 // This is the sequence ID, it is filled in during the actual sending
    packWord(pPacket, (WORD)(pPacket->wLen - 6)); // This counter should not include the flap header (thus the -6)
}



void __fastcall serverPacketInit(icq_packet* pPacket, WORD wSize)
{
    pPacket->wLen = wSize;
    write_flap(pPacket, ICQ_DATA_CHAN);
}



void __fastcall directPacketInit(icq_packet* pPacket, DWORD dwSize)
{
    pPacket->wPlace = 0;
    pPacket->wLen   = (WORD)dwSize;
    pPacket->pData  = (BYTE *)icq_alloc_zero(dwSize + 2);

    packLEWord(pPacket, pPacket->wLen);
}



void __fastcall serverCookieInit(icq_packet* pPacket, BYTE* pCookie, WORD wCookieSize)
{
    BYTE empty = 0;
    pPacket->wLen = (WORD)(wCookieSize + 8 + sizeof(CLIENT_ID_STRING) + 61 + 5);

    write_flap(pPacket, ICQ_LOGIN_CHAN);
    packDWord(pPacket, 0x00000001);
    packTLV(pPacket, 0x06, wCookieSize, (char *)pCookie);

    // Pack client identification details.
    packTLV(pPacket, 0x0003, (WORD)sizeof(CLIENT_ID_STRING)-1, CLIENT_ID_STRING);
    packTLVWord(pPacket, 0x0017, CLIENT_VERSION_MAJOR);
    packTLVWord(pPacket, 0x0018, CLIENT_VERSION_MINOR);
    packTLVWord(pPacket, 0x0019, CLIENT_VERSION_LESSER);
    packTLVWord(pPacket, 0x001a, CLIENT_VERSION_BUILD);
    packTLVWord(pPacket, 0x0016, CLIENT_ID_CODE);
    packTLVDWord(pPacket, 0x0014, CLIENT_DISTRIBUTION);
    packTLV(pPacket, 0x000f, 0x0002, CLIENT_LANGUAGE);
    packTLV(pPacket, 0x000e, 0x0002, CLIENT_COUNTRY);
    packDWord(pPacket, 0x00940001); // reconnect flag
    packByte(pPacket, 0);
    packTLVDWord(pPacket, 0x8003, 0x00100000); // Unknown
}



void __fastcall packByte(icq_packet* pPacket, BYTE byValue)
{
    pPacket->pData[pPacket->wPlace++] = byValue;
}



void __fastcall packWord(icq_packet* pPacket, WORD wValue)
{
    pPacket->pData[pPacket->wPlace++] = ((wValue & 0xff00) >> 8);
    pPacket->pData[pPacket->wPlace++] = (wValue & 0x00ff);
}

static void packWord_PBYTE(PBYTE buf, WORD wValue)
{
    *(buf) = ((wValue & 0xff00) >> 8);
    *(buf + 1) = (wValue & 0x00ff);
}




void __fastcall packDWord(icq_packet* pPacket, DWORD dwValue)
{
    pPacket->pData[pPacket->wPlace++] = (BYTE)((dwValue & 0xff000000) >> 24);
    pPacket->pData[pPacket->wPlace++] = (BYTE)((dwValue & 0x00ff0000) >> 16);
    pPacket->pData[pPacket->wPlace++] = (BYTE)((dwValue & 0x0000ff00) >> 8);
    pPacket->pData[pPacket->wPlace++] = (BYTE) (dwValue & 0x000000ff);
}

static void packDWord_PBYTE(PBYTE buf, DWORD dwValue)
{
    *(buf) = (BYTE)((dwValue & 0xff000000) >> 24);
    *(buf + 1) = (BYTE)((dwValue & 0x00ff0000) >> 16);
    *(buf + 2) = (BYTE)((dwValue & 0x0000ff00) >> 8);
    *(buf + 3) = (BYTE) (dwValue & 0x000000ff);
}




void __fastcall packQWord(icq_packet* pPacket, DWORD64 qwValue)
{
    packDWord(pPacket, (DWORD)(qwValue >> 32));
    packDWord(pPacket, (DWORD)(qwValue & 0xffffffff));
}

static void packQWord_PBYTE(PBYTE buf, DWORD64 qwValue)
{
    packDWord_PBYTE(buf, (DWORD)(qwValue >> 32));
    packDWord_PBYTE(buf + 4, (DWORD)(qwValue & 0xffffffff));
}



void packTLV(icq_packet *pPacket, WORD wType, WORD wLength, const char *pbyValue)
{
    packWord(pPacket, wType);
    packWord(pPacket, wLength);
    packBuffer(pPacket, pbyValue, wLength);
}



void packTLVWord(icq_packet* pPacket, WORD wType, WORD wValue)
{
    packWord(pPacket, wType);
    packWord(pPacket, 0x02);
    packWord(pPacket, wValue);
}



void packTLVDWord(icq_packet* pPacket, WORD wType, DWORD dwValue)
{
    packWord(pPacket, wType);
    packWord(pPacket, 0x04);
    packDWord(pPacket, dwValue);
}



// Pack a preformatted buffer.
// This can be used to pack strings or any type of raw data.

// FIXME: Switch buffer to BYTE*
void packBuffer(icq_packet* pPacket, const char* pbyBuffer, WORD wLength)
{
    while (wLength)
    {
        pPacket->pData[pPacket->wPlace++] = *pbyBuffer++;
        wLength--;
    }
}



// Pack a buffer and prepend it with the size as a LE WORD.
// Commented out since its not actually used anywhere right now.
//void packLEWordSizedBuffer(icq_packet* pPacket, const BYTE* pbyBuffer, WORD wLength)
//{
//
//  packLEWord(pPacket, wLength);
//  packBuffer(pPacket, pbyBuffer, wLength);
//
//}


int __fastcall getUINLen(DWORD dwUin)
{
    // TODO: invent something more clever
    if (dwUin >= 1000000000) return 10;
    if (dwUin >= 100000000) return 9;
    if (dwUin >= 10000000) return 8;
    if (dwUin >= 1000000) return 7;
    if (dwUin >= 100000) return 6;
    if (dwUin >= 10000) return 5;
    if (dwUin >= 1000) return 4;
    if (dwUin >= 100) return 3;
    if (dwUin >= 10) return 2;
    return 1;
}



int __fastcall getUIDLen(DWORD dwUin, char* szUid)
{
    if (dwUin)
        return getUINLen(dwUin);
    else
        return strlennull(szUid);
}



void __fastcall packUIN(icq_packet* pPacket, DWORD dwUin)
{
    char pszUin[UINMAXLEN];
    BYTE nUinLen = getUINLen(dwUin);

    ltoa(dwUin, pszUin, 10);

    packByte(pPacket, nUinLen);           // Length of user id
    packBuffer(pPacket, pszUin, nUinLen); // Receiving user's id
}



void __fastcall packUID(icq_packet* pPacket, DWORD dwUin, char* szUid)
{
    if (dwUin)
        packUIN(pPacket, dwUin);
    else
    {
        BYTE nLen = strlennull(szUid);
        packByte(pPacket, nLen);
        packBuffer(pPacket, szUid, nLen);
    }
}



void packFNACHeader(icq_packet* pPacket, WORD wFamily, WORD wSubtype)
{
    packWord(pPacket, wFamily);   // Family type
    packWord(pPacket, wSubtype);  // Family subtype
    packDWord(pPacket, 0);        // SNAC flags // SNAC request id (sequence)
    packWord(pPacket, wSubtype);  // SNAC request id (command)
}



void packFNACHeaderFull(icq_packet* pPacket, WORD wFamily, WORD wSubtype, WORD wFlags, DWORD dwSeq)
{
    WORD wSeq = (WORD)dwSeq & 0x7FFF; // this is necessary, if that bit is there we get disconnected

    packWord(pPacket, wFamily);   // Family type
    packWord(pPacket, wSubtype);  // Family subtype
    packWord(pPacket, wFlags);    // SNAC flags
    packWord(pPacket, wSeq);      // SNAC request id (sequence)
    packWord(pPacket, (WORD)(dwSeq>>0x10));  // SNAC request id (command)
}

void packFNACHeaderFullwVersion(icq_packet* pPacket, WORD wFamily, WORD wSubtype, WORD wFlags, DWORD dwSeq, WORD wVersion)
{
    packFNACHeaderFull(pPacket, wFamily, wSubtype, wFlags | 0x8000, dwSeq);
    packWord(pPacket, 0x06);
    packTLVWord(pPacket, 0x01, wVersion);
}



void __fastcall packLEWord(icq_packet* pPacket, WORD wValue)
{
    pPacket->pData[pPacket->wPlace++] =  (wValue & 0x00ff);
    pPacket->pData[pPacket->wPlace++] = ((wValue & 0xff00) >> 8);
}



void __fastcall packLEDWord(icq_packet* pPacket, DWORD dwValue)
{
    pPacket->pData[pPacket->wPlace++] = (BYTE) (dwValue & 0x000000ff);
    pPacket->pData[pPacket->wPlace++] = (BYTE)((dwValue & 0x0000ff00) >> 8);
    pPacket->pData[pPacket->wPlace++] = (BYTE)((dwValue & 0x00ff0000) >> 16);
    pPacket->pData[pPacket->wPlace++] = (BYTE)((dwValue & 0xff000000) >> 24);
}



void ppackByte(PBYTE *buf,int *buflen,BYTE b)
{
    *buf=(PBYTE)mir_realloc(*buf,1+*buflen);
    *(*buf+*buflen)=b;
    ++*buflen;
}



void ppackLEWord(PBYTE *buf,int *buflen,WORD w)
{
    *buf=(PBYTE)mir_realloc(*buf,2+*buflen);
    *(PWORD)(*buf+*buflen)=w;
    *buflen+=2;
}



void ppackLEDWord(PBYTE *buf, int *buflen, DWORD d)
{
    *buf = (PBYTE)mir_realloc(*buf, 4 + *buflen);
    *(PDWORD)(*buf + *buflen) = d;
    *buflen += 4;
}



/*void ppackLNTS(PBYTE *buf, int *buflen, const char *str)
{
  WORD len = strlennull(str);
  ppackWord(buf, buflen, len);
  *buf = (PBYTE)mir_realloc(*buf, *buflen + len);
  memcpy(*buf + *buflen, str, len);
  *buflen += len;
}*/



void ppackLELNTS(PBYTE *buf, int *buflen, const char *str)
{
    WORD len = strlennull(str);
    ppackLEWord(buf, buflen, len);
    *buf = (PBYTE)mir_realloc(*buf, *buflen + len);
    memcpy(*buf + *buflen, str, len);
    *buflen += len;
}



void ppackLELNTSfromDB(PBYTE *buf, int *buflen, const char *szSetting)
{
    DBVARIANT dbv;

    if (getSettingString(NULL, szSetting, &dbv))
    {
        ppackLEWord(buf, buflen, 0);
    }
    else
    {
        ppackLELNTS(buf, buflen, dbv.pszVal);
        ICQFreeVariant(&dbv);
    }
}



// *** TLV based (!!! WORDs and DWORDs are LE !!!)

void ppackBuffer(PBYTE *buf, int *buflen, WORD wLength, const BYTE *pbyValue)
{
    if (wLength)
    {
        *buf = (PBYTE)mir_realloc(*buf, wLength + *buflen);
        memcpy(*buf + *buflen, pbyValue, wLength);
        *buflen += wLength;
    }
}


void ppackTLV(PBYTE *buf, int *buflen, WORD wType, WORD wLength, const BYTE *pbyValue)
{
    *buf = (PBYTE)mir_realloc(*buf, 4 + wLength + *buflen);
    packWord_PBYTE(*buf + *buflen, wType);
    packWord_PBYTE(*buf + *buflen + 2, wLength);
    if (wLength)
        memcpy(*buf + *buflen + 4, pbyValue, wLength);
    *buflen += 4 + wLength;
}

void ppackWord(PBYTE *buf, int *buflen, WORD wValue)
{
    *buf = (PBYTE)mir_realloc(*buf, 2 + *buflen);
    packWord_PBYTE(*buf + *buflen, wValue);
    *buflen += 2;
}



void ppackTLVByte(PBYTE *buf, int *buflen, BYTE b, WORD wType, BYTE always)
{
    if (!always && !b) return;

    *buf = (PBYTE)mir_realloc(*buf, 5 + *buflen);
    *(PWORD)(*buf + *buflen) = wType;
    *(PWORD)(*buf + *buflen + 2) = 1;
    *(*buf + *buflen + 4) = b;
    *buflen += 5;
}



void ppackTLVWord(PBYTE *buf, int *buflen, WORD w, WORD wType, BYTE always)
{
    if (!always && !w) return;

    *buf = (PBYTE)mir_realloc(*buf, 6 + *buflen);
    *(PWORD)(*buf + *buflen) = wType;
    *(PWORD)(*buf + *buflen + 2) = 2;
    *(PWORD)(*buf + *buflen + 4) = w;
    *buflen += 6;
}

void ppackTLVDouble(PBYTE *buf, int *buflen, WORD wType, double dValue)
{
    DWORD64 qwValue;

    memcpy(&qwValue, &dValue, 8);

    *buf = (PBYTE)mir_realloc(*buf, 12 + *buflen);
    packWord_PBYTE(*buf + *buflen, wType);
    packWord_PBYTE(*buf + *buflen + 2, 8);
    packQWord_PBYTE(*buf + *buflen + 4, qwValue);
    *buflen += 12;
}


void ppackTLVDWord(PBYTE *buf, int *buflen, DWORD d, WORD wType, BYTE always)
{
    if (!always && !d) return;

    *buf = (PBYTE)mir_realloc(*buf, 8 + *buflen);
    *(PWORD)(*buf + *buflen) = wType;
    *(PWORD)(*buf + *buflen + 2) = 4;
    *(PDWORD)(*buf + *buflen + 4) = d;
    *buflen += 8;
}

void ppackTLVUID(PBYTE *buf, int *buflen, WORD wType, DWORD dwUin, const char *szUid)
{
    if (dwUin)
    {
        char szUin[UINMAXLEN];

        _ltoa(dwUin, szUin, 10);

        ppackTLV(buf, buflen, wType, getUINLen(dwUin), (BYTE*)szUin);
    }
    else if (szUid)
        ppackTLV(buf, buflen, wType, strlennull(szUid), (BYTE*)szUid);
}


void packTLVUID(icq_packet *pPacket, WORD wType, DWORD dwUin, const char *szUid)
{
    if (dwUin)
    {
        char szUin[UINMAXLEN];

        _ltoa(dwUin, szUin, 10);

        packTLV(pPacket, wType, getUINLen(dwUin), szUin);
    }
    else if (szUid)
        packTLV(pPacket, wType, strlennull(szUid), szUid);
}



void packTLVLNTS(PBYTE *buf, int *bufpos, const char *str, WORD wType)
{
    int len = strlennull(str) + 1;

    *(PWORD)(*buf + *bufpos) = wType;
    *(PWORD)(*buf + *bufpos + 2) = len + 2;
    *(PWORD)(*buf + *bufpos + 4) = len;
    memcpy(*buf + *bufpos + 6, str, len);
    *bufpos += len + 6;
}



void ppackTLVLNTS(PBYTE *buf, int *buflen, const char *str, WORD wType, BYTE always)
{
    int len = strlennull(str) + 1;

    if (!always && len < 2) return;

    *buf = (PBYTE)mir_realloc(*buf, 6 + *buflen + len);
    packTLVLNTS(buf, buflen, str, wType);
}



void ppackTLVWordLNTS(PBYTE *buf, int *buflen, WORD w, const char *str, WORD wType, BYTE always)
{
    int len = strlennull(str) + 1;

    if (!always && len < 2 && !w) return;

    *buf = (PBYTE)mir_realloc(*buf, 8 + *buflen + len);
    *(PWORD)(*buf + *buflen) = wType;
    *(PWORD)(*buf + *buflen + 2) = len + 4;
    *(PWORD)(*buf + *buflen + 4) = w;
    *(PWORD)(*buf + *buflen + 6) = len;
    memcpy(*buf + *buflen + 8, str, len);
    *buflen += len + 8;
}



void ppackTLVLNTSByte(PBYTE *buf, int *buflen, const char *str, BYTE b, WORD wType)
{
    int len = strlennull(str) + 1;

    *buf = (PBYTE)mir_realloc(*buf, 7 + *buflen + len);
    *(PWORD)(*buf + *buflen) = wType;
    *(PWORD)(*buf + *buflen + 2) = len + 3;
    *(PWORD)(*buf + *buflen + 4) = len;
    memcpy(*buf + *buflen + 6, str, len);
    *(*buf + *buflen + 6 + len) = b;
    *buflen += len + 7;
}



void ppackTLVLNTSfromDB(PBYTE *buf, int *buflen, const char *szSetting, WORD wType)
{
    char szTmp[1024];
    char *str = "";

    if (!getSettingStringStatic(NULL, szSetting, szTmp, sizeof(szTmp)))
        str = szTmp;

    ppackTLVLNTS(buf, buflen, str, wType, 1);
}



void ppackTLVWordLNTSfromDB(PBYTE *buf, int *buflen, WORD w, const char *szSetting, WORD wType)
{
    char szTmp[1024];
    char *str = "";

    if (!getSettingStringStatic(NULL, szSetting, szTmp, sizeof(szTmp)))
        str = szTmp;

    ppackTLVWordLNTS(buf, buflen, w, str, wType, 1);
}



void ppackTLVLNTSBytefromDB(PBYTE *buf, int *buflen, const char *szSetting, BYTE b, WORD wType)
{
    char szTmp[1024];
    char *str = "";

    if (!getSettingStringStatic(NULL, szSetting, szTmp, sizeof(szTmp)))
        str = szTmp;

    ppackTLVLNTSByte(buf, buflen, str, b, wType);
}

int ppackTLVWordStringItemFromDB(PBYTE *buf, int *buflen, const char *szSetting, WORD wTypeID, WORD wTypeData, WORD wID)
{
    char szTmp[1024];
    char *str = NULL;

    if (!getSettingStringStatic(NULL, szSetting, szTmp, sizeof(szTmp)))
        str = szTmp;

    if (str)
    {
        WORD wLen = strlennull(str);

        ppackWord(buf, buflen, wLen + 0x0A);
        ppackTLVWord(buf, buflen, wID, wTypeID, 1);
        ppackTLV(buf, buflen, wTypeData, wLen, (PBYTE)str);

        return 1; // Success
    }

    return 0; // No data
}

void ppackTLVStringUtfFromDB(PBYTE *buf, int *buflen, const char *szSetting, WORD wType)
{
    char *str = getSettingStringUtf(NULL, szSetting, NULL);

    ppackTLV(buf, buflen, wType, strlennull(str), (PBYTE)str);

    mir_free(str);
}
void ppackTLVStringFromDB(PBYTE *buf, int *buflen, const char *szSetting, WORD wType)
{
    char szTmp[1024];
    char *str = "";

    if (!getSettingStringStatic(NULL, szSetting, szTmp, sizeof(szTmp)))
        str = szTmp;

    ppackTLV(buf, buflen, wType, strlennull(str), (PBYTE)str);
}


void ppackTLVDateFromDB(PBYTE *buf, int *buflen, const char *szSettingYear, const char *szSettingMonth, const char *szSettingDay, WORD wType)
{
    SYSTEMTIME sTime = {0};
    double time = 0;
    sTime.wYear = getSettingWord(NULL, szSettingYear, 0);
    sTime.wMonth = getSettingByte(NULL, szSettingMonth, 0);
    sTime.wDay = getSettingByte(NULL, szSettingDay, 0);
    if (sTime.wYear || sTime.wMonth || sTime.wDay)
    {
        SystemTimeToVariantTime(&sTime, &time);
        time -= 2;
    }

    ppackTLVDouble(buf, buflen, wType, time);
}



void ppackTLVBlockItems(PBYTE *buf, int *buflen, WORD wType, int *nItems, PBYTE *pBlock, WORD *wLength, BOOL bSingleItem)
{
    *buf = (PBYTE)mir_realloc(*buf, 8 + *buflen + *wLength);
    packWord_PBYTE(*buf + *buflen, wType);
    packWord_PBYTE(*buf + *buflen + 2, (bSingleItem ? 4 : 2) + *wLength);
    packWord_PBYTE(*buf + *buflen + 4, *nItems);
    if (bSingleItem)
        packWord_PBYTE(*buf + *buflen + 6, *wLength);
    if (*wLength)
        memcpy(*buf + *buflen + (bSingleItem ? 8 : 6), *pBlock, *wLength);
    *buflen += (bSingleItem ? 8 : 6) + *wLength;

    mir_free(pBlock);
    *wLength = 0;
    *nItems = 0;
}

int ppackTLVWordStringUtfItemFromDB(PBYTE *buf, int *buflen, const char *szSetting, WORD wTypeID, WORD wTypeData, WORD wID)
{
    char *str = getSettingStringUtf(NULL, szSetting, NULL);

    if (str)
    {
        WORD wLen = strlennull(str);

        ppackWord(buf, buflen, wLen + 0x0A);
        ppackTLVWord(buf, buflen, wID, wTypeID, 1);
        ppackTLV(buf, buflen, wTypeData, wLen, (PBYTE)str);

        mir_free(str);

        return 1; // Success
    }

    return 0; // No data
}



void __fastcall unpackByte(BYTE** pSource, BYTE* byDestination)
{
    if (byDestination)
    {
        *byDestination = *(*pSource)++;
    }
    else
    {
        *pSource += 1;
    }
}



void __fastcall unpackWord(BYTE** pSource, WORD* wDestination)
{
    unsigned char *tmp = *pSource;

    if (wDestination)
    {
        *wDestination  = *tmp++ << 8;
        *wDestination |= *tmp++;

        *pSource = tmp;
    }
    else
    {
        *pSource += 2;
    }
}



void __fastcall unpackDWord(BYTE** pSource, DWORD* dwDestination)
{
    unsigned char *tmp = *pSource;

    if (dwDestination)
    {
        *dwDestination  = *tmp++ << 24;
        *dwDestination |= *tmp++ << 16;
        *dwDestination |= *tmp++ << 8;
        *dwDestination |= *tmp++;

        *pSource = tmp;
    }
    else
    {
        *pSource += 4;
    }
}



void __fastcall unpackQWord(BYTE** pSource, DWORD64* qwDestination)
{
    DWORD dwData;

    if (qwDestination)
    {
        unpackDWord(pSource, &dwData);
        *qwDestination = ((DWORD64)dwData) << 32;
        unpackDWord(pSource, &dwData);
        *qwDestination |= dwData;
    }
    else
    {
        *pSource += 8;
    }
}



void __fastcall unpackLEWord(unsigned char **buf, WORD *w)
{
    unsigned char *tmp = *buf;

    if (w)
    {
        *w = (*tmp++);
        *w |= ((*tmp++) << 8);
    }
    else
        tmp += 2;

    *buf = tmp;
}



void __fastcall unpackLEDWord(unsigned char **buf, DWORD *dw)
{
    unsigned char *tmp = *buf;

    if (dw)
    {
        *dw = (*tmp++);
        *dw |= ((*tmp++) << 8);
        *dw |= ((*tmp++) << 16);
        *dw |= ((*tmp++) << 24);
    }
    else
        tmp += 4;

    *buf = tmp;
}



void unpackString(unsigned char **buf, char *string, WORD len)
{
    unsigned char *tmp = *buf;

    if (string)
    {
        while (len)  /* Can have 0x00 so go by len */
        {
            *string++ = *tmp++;
            len--;
        }
    }
    else
        tmp += len;

    *buf = tmp;
}



void unpackWideString(unsigned char **buf, WCHAR *string, WORD len)
{
    unsigned char *tmp = *buf;

    while (len > 1)
    {
        *string = (*tmp++ << 8);
        *string |= *tmp++;

        string++;
        len -= 2;
    }

    // We have a stray byte at the end, this means that the buffer had an odd length
    // which indicates an error.
#ifdef _DEBUG
    _ASSERTE(len == 0);
#endif
    if (len != 0)
    {
        // We dont copy the last byte but we still need to increase the buffer pointer
        // (we assume that 'len' was correct) since the calling function expects
        // that it is increased 'len' bytes.
        *tmp += len;
    }

    *buf = tmp;
}



void unpackTypedTLV(unsigned char *buf, int buflen, WORD type, WORD *ttype, WORD *tlen, char **tlv)
{
    WORD wType, wLen;

NextTLV:
    // Unpack type and length
    unpackWord(&buf, &wType);
    unpackWord(&buf, &wLen);
    buflen -= 4;

    if (wType != type && buflen >= wLen + 4)
    {
        // Not the right TLV, try next
        buflen -= wLen;
        buf += wLen;
        goto NextTLV;
    }
    // Check buffer size
    if (wLen > buflen) wLen = buflen;

    // Make sure we have a good pointer
    if (tlv)
    {
        if (wLen)
        {
            // Unpack and save value
            *tlv = (char *)icq_alloc_zero(wLen + 1); // Add 1 for \0
            unpackString(&buf, *tlv, wLen);
            *(*tlv + wLen) = '\0';
        }
        else
            *tlv = NULL;
    }

    // Save type and length
    if (ttype)
        *ttype = wType;
    if (tlen)
        *tlen = wLen;
}



BOOL unpackUID(unsigned char** ppBuf, WORD* pwLen, DWORD *pdwUIN, uid_str* ppszUID)
{
    BYTE nUIDLen;
    char szUIN[UINMAXLEN+1];

    // Sender UIN
    unpackByte(ppBuf, &nUIDLen);
    *pwLen -= 1;

    if ((nUIDLen > *pwLen) || (nUIDLen == 0))
        return FALSE;

    if (nUIDLen <= UINMAXLEN)
    {
        // it can be uin, check
        unpackString(ppBuf, szUIN, nUIDLen);
        szUIN[nUIDLen] = '\0';
        *pwLen -= nUIDLen;

        if (IsStringUIN(szUIN))
        {
            *pdwUIN = atoi(szUIN);
            return TRUE;
        }
        else if (!ppszUID || !m_bAimEnabled)
        {
            NetLog_Server("Malformed UIN in packet");
            return FALSE;
        }
        else
        {
            // go back
            *ppBuf -= nUIDLen;
            *pwLen += nUIDLen;
        }
    }
    else if (!ppszUID || ! m_bAimEnabled)
    {
        NetLog_Server("Malformed UIN in packet");
        return FALSE;
    }
    if (!(*ppszUID)) return FALSE;

    unpackString(ppBuf, *ppszUID, nUIDLen);
    *pwLen -= nUIDLen;
    (*ppszUID)[nUIDLen] = '\0';

    *pdwUIN = 0; // this is how we determine aim contacts internally

    return TRUE;
}
