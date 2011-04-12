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
// Revision       : $Revision: 43 $
// Last change on : $Date: 2007-08-20 01:51:06 +0300 (ÐŸÐ½, 20 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Contains helper functions to convert text messages between different
//  character sets.
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"


static BOOL bHasCP_UTF8 = FALSE;


void InitI18N(void)
{
    CPINFO CPInfo;


    bHasCP_UTF8 = GetCPInfo(CP_UTF8, &CPInfo);
}



// Returns true if the buffer only contains 7-bit characters.
BOOL IsUSASCII(const unsigned char* pBuffer, int nSize)
{
    BOOL bResult = TRUE;
    int nIndex;

    for (nIndex = 0; nIndex < nSize; nIndex++)
    {
        if (pBuffer[nIndex] > 0x7F)
        {
            bResult = FALSE;
            break;
        }
    }

    return bResult;
}

// Returns true if the unicode buffer only contains 7-bit characters.
BOOL IsUnicodeAscii(const WCHAR* pBuffer, int nSize)
{
    BOOL bResult = TRUE;
    int nIndex;


    for (nIndex = 0; nIndex < nSize; nIndex++)
    {
        if (pBuffer[nIndex] > 0x7F)
        {
            bResult = FALSE;
            break;
        }
    }

    return bResult;
}


// Scans a string encoded in UTF-8 to verify that it contains
// only valid sequences. It will return 1 if the string contains
// only legitimate encoding sequences; otherwise it will return 0;
// From 'Secure Programming Cookbook', John Viega & Matt Messier, 2003
int UTF8_IsValid(const unsigned char* pszInput)
{
    int nb, i;
    const unsigned char* c = pszInput;

    if (!pszInput) return 0;

    for (c = pszInput; *c; c += (nb + 1))
    {
        if (!(*c & 0x80))
            nb = 0;
        else if ((*c & 0xc0) == 0x80) return 0;
        else if ((*c & 0xe0) == 0xc0) nb = 1;
        else if ((*c & 0xf0) == 0xe0) nb = 2;
        else if ((*c & 0xf8) == 0xf0) nb = 3;
        else if ((*c & 0xfc) == 0xf8) nb = 4;
        else if ((*c & 0xfe) == 0xfc) nb = 5;

        for (i = 1; i<=nb; i++) // we this forward, do not cross end of string
            if ((*(c + i) & 0xc0) != 0x80)
                return 0;
    }

    return 1;
}


// returns ansi string in all cases
char* detect_decode_utf8(const char *from)
{
    char* temp = NULL;

    temp = mir_utf8decodeA(from);
    if(temp && temp[0])


        if (IsUSASCII((BYTE*)from, strlennull(from)) || !UTF8_IsValid((BYTE*)from))
            return (char*)from;
    mir_free((char*)from);

    return temp;
}


/*
 * The following UTF8 routines are
 *
 * Copyright (C) 2001 Peter Harris <peter.harris@hummingbird.com>
 * Copyright (C) 2001 Edmund Grimley Evans <edmundo@rano.org>
 *
 * under a GPL license
 *
 * --------------------------------------------------------------
 * Convert a string between UTF-8 and the locale's charset.
 * Invalid bytes are replaced by '#', and characters that are
 * not available in the target encoding are replaced by '?'.
 *
 * If the locale's charset is not set explicitly then it is
 * obtained using nl_langinfo(CODESET), where available, the
 * environment variable CHARSET, or assumed to be US-ASCII.
 *
 * Return value of conversion functions:
 *
 *  -1 : memory allocation failed
 *   0 : data was converted exactly
 *   1 : valid data was converted approximately (using '?')
 *   2 : input was invalid (but still converted, using '#')
 *   3 : unknown encoding (but still converted, using '?')
 */



/*
 * Convert a string between UTF-8 and the locale's charset.
 */
