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
//  Helper functions for Oscar TLV chains
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"



/* set maxTlvs<=0 to get all TLVs in length, or a positive integer to get at most the first n */
oscar_tlv_chain* readIntoTLVChain(BYTE **buf, WORD wLen, int maxTlvs)
{
    oscar_tlv_chain *now = NULL, *last = NULL, *chain = NULL;
    WORD now_tlv_len;
    int len = wLen;

    if (!buf || !wLen) return NULL;

    while (len > 0) /* don't use unsigned variable for this check */
    {
        now = (oscar_tlv_chain *)icq_alloc_zero(sizeof(oscar_tlv_chain));

        if (!now)
        {
            disposeChain(&chain);
            return NULL;
        }

        unpackWord(buf, &(now->tlv.wType));
        unpackWord(buf, &now_tlv_len);
        now->tlv.wLen = now_tlv_len;
        len -= 4;

        if (now_tlv_len < 1)
        {
            now->tlv.pData = NULL;
        }
        else if (now_tlv_len <= len)
        {
            now->tlv.pData = (BYTE *)icq_alloc_zero(now_tlv_len);
            if (now->tlv.pData)
                memcpy(now->tlv.pData, *buf, now_tlv_len);
        }
        else
        {
            // the packet is shorter than it should be
            mir_free(now);
            return chain; // give at least the rest of chain
        }

        if (chain) // keep the original order
            last->next = now;
        else
            chain = now;

        last = now;

        len -= now_tlv_len;
        *buf += now_tlv_len;

        if (--maxTlvs == 0)
            break;
    }

    return chain;
}

// Returns a pointer to the TLV with type wType and number wIndex in the chain
// If wIndex = 1, the first matching TLV will be returned, if wIndex = 2,
// the second matching one will be returned.
// wIndex must be > 0
oscar_tlv* getTLV(oscar_tlv_chain *list, WORD wType, WORD wIndex)
{
    int i = 0;

    while (list)
    {
        if (list->tlv.wType == wType)
            i++;
        if (i >= wIndex)
            return &list->tlv;
        list = list->next;
    }

    return NULL;
}

WORD getLenFromChain(oscar_tlv_chain *list, WORD wType, WORD wIndex)
{
    oscar_tlv *tlv;
    WORD wLen = 0;

    tlv = getTLV(list, wType, wIndex);
    if (tlv)
    {
        wLen = tlv->wLen;
    }

    return wLen;
}

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/* Values are returned in MSB format */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

DWORD getDWordFromChain(oscar_tlv_chain *list, WORD wType, WORD wIndex)
{
    oscar_tlv *tlv;
    DWORD dw = 0;

    tlv = getTLV(list, wType, wIndex);
    if (tlv && tlv->wLen >= 4)
    {
        dw |= (*((tlv->pData)+0) << 24);
        dw |= (*((tlv->pData)+1) << 16);
        dw |= (*((tlv->pData)+2) << 8);
        dw |= (*((tlv->pData)+3));
    }

    return dw;
}

WORD getWordFromChain(oscar_tlv_chain *list, WORD wType, WORD wIndex)
{
    oscar_tlv *tlv;
    WORD w = 0;

    tlv = getTLV(list, wType, wIndex);
    if (tlv && tlv->wLen >= 2)
    {
        w |= (*((tlv->pData)+0) << 8);
        w |= (*((tlv->pData)+1));
    }

    return w;
}

BYTE getByteFromChain(oscar_tlv_chain *list, WORD wType, WORD wIndex)
{
    oscar_tlv *tlv;
    BYTE b = 0;

    tlv = getTLV(list, wType, wIndex);
    if (tlv && tlv->wLen)
    {
        b = *(tlv->pData);
    }

    return b;
}

int getNumberFromChain(oscar_tlv_chain *list, WORD wType, WORD wIndex)
{
    oscar_tlv *tlv;
    BYTE b = 0;

    tlv = getTLV(list, wType, wIndex);
    if (tlv)
    {
        if (tlv->wLen == 1)
            return getByteFromChain(list, wType, wIndex);
        else if (tlv->wLen == 2)
            return getWordFromChain(list, wType, wIndex);
        else if (tlv->wLen == 4)
            return getDWordFromChain(list, wType, wIndex);
    }
    return 0;
}
double getDoubleFromChain(oscar_tlv_chain *list, WORD wType, WORD wIndex)
{
    oscar_tlv *tlv = getTLV(list, wType, wIndex);

    if (tlv && tlv->wLen == 8)
    {
        BYTE *buf = tlv->pData;
        double d = 0;

        unpackQWord(&buf, (DWORD64*)&d);

        return d;
    }
    return 0;
}


char* getStrFromChain(oscar_tlv_chain *list, WORD wType, WORD wIndex)
{
    oscar_tlv *tlv;
    char *str = NULL;

    tlv = getTLV(list, wType, wIndex);
    if (tlv)
    {
        str = (char*)icq_alloc_zero(tlv->wLen+1); /* For \0 */

        if (!str) return NULL;

        memcpy(str, tlv->pData, tlv->wLen);
        *(str+tlv->wLen) = '\0';
    }

    return str;
}

void disposeRecordList(oscar_tlv_record_list** list)
{
    oscar_tlv_record_list *now;
    if (!list || !*list)
        return;

    now = *list;

    while (now)
    {
        oscar_tlv_record_list *next = now->next;

        disposeChain(&now->item);
        mir_free(now);
        now = next;
    }

    *list = NULL;
}


oscar_tlv_chain* getRecordByTLV(oscar_tlv_record_list *list, WORD wType, int nValue)
{
    oscar_tlv_record_list *l = list;

    while (l)
    {
        if (l->item && getTLV(list->item, wType, 1) && getNumberFromChain(l->item, wType, 1) == nValue)
            return l->item;
        l = l->next;
    }

    return NULL;
}

oscar_tlv_record_list* readIntoTLVRecordList(BYTE **buf, WORD wLen, int nCount)
{
    oscar_tlv_record_list *list = NULL, *last = NULL;

    while (wLen >= 2)
    {
        WORD wRecordSize;

        unpackWord(buf, &wRecordSize);
        wLen -= 2;
        if (wRecordSize && wRecordSize <= wLen)
        {
            BYTE *pData = *buf;
            oscar_tlv_record_list *pRecord = (oscar_tlv_record_list*)icq_alloc_zero(sizeof(oscar_tlv_record_list));

            *buf += wRecordSize;
            wLen -= wRecordSize;

            pRecord->item = readIntoTLVChain(&pData, wRecordSize, 0);
            if (pRecord->item)
            {
                // keep the order
                if (list)
                    last->next = pRecord;
                else
                    list = pRecord;

                last = pRecord;
            }
            else
                mir_free(&pRecord);
        }

        if (--nCount == 0) break;
    }
    return list;
}



void disposeChain(oscar_tlv_chain **list)
{
    oscar_tlv_chain *now;

    if (!list || !*list)
        return;

    now = *list;

    while (now)
    {
        oscar_tlv_chain *temp;

        mir_free(now->tlv.pData);

        temp = now->next;
        mir_free(now);
        now = temp;
    }

    *list = NULL;
}
