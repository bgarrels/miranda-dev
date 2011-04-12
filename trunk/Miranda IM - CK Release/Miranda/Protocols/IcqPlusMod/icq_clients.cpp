// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin Öberg, Sam Kothari, Robert Rainwater
// Copyright © 2004,2005,2006,2007 Joe Kucera
// Copyright © 2006,2007,2008 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
// Copyright © 2008 [sss], chaos.persei, nullbie, baloo, jarvis
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
//  Provides capability & signature based client detection
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"


capstr* MatchCap(BYTE* buf, int bufsize, const capstr* cap, int capsize)
{
    while (bufsize > 0) // search the buffer for a capability
    {
        if (!memcmp(buf, cap, capsize))
            return (capstr*)buf; // give found capability for version info
        else
        {
            buf += 0x10;		// FIXME: Should use sizeof(capstr) here.
            bufsize -= 0x10;
        }
    }
    return 0;
}



static const char * makeClientVersion(char *szBuf, const char* szClient, unsigned v1, unsigned v2, unsigned v3, unsigned v4)
{
    if (v4)
        null_snprintf(szBuf, 64, "%s%u.%u.%u.%u", szClient, v1, v2, v3, v4);
    else if (v3)
        null_snprintf(szBuf, 64, "%s%u.%u.%u", szClient, v1, v2, v3);
    else
        null_snprintf(szBuf, 64, "%s%u.%u", szClient, v1, v2);
    return szBuf;
}




char* cliLibicq2k  = "libicq2000";
char* cliLicqVer   = "Licq ";
char* cliCentericq = "Centericq";
char* cliLibicqUTF = "libicq2000 (Unicode)";
char* cliTrillian  = "Trillian";
char* cliQip       = "QIP %s";
char* cliIM2       = "IM2";
char* cliSpamBot   = "Spam Bot";


char* detectUserClient(HANDLE hContact, int nIsICQ, DWORD dwUin, WORD wUserClass, WORD wVersion, DWORD dwFT1, DWORD dwFT2, DWORD dwFT3, DWORD dwOnlineSince, BYTE bDirectFlag, DWORD dwDirectCookie, DWORD dwWebPort, BYTE* caps, WORD wLen, BYTE* bClientId, char* szClientBuf)
{
    capstr* capId;
    LPSTR szClient = {0};

    *bClientId = CLID_ALTERNATIVE; // Most clients does not tick as MsgIDs

    // Is this a Miranda IM client?
    if (dwFT1 == 0xffffffff)
    {
        if (dwFT2 == 0xffffffff)
        {
            // This is Gaim not Miranda
            szClient = "Gaim";
        }
        else if (!dwFT2 && wVersion == 7)
        {
            // This is WebICQ not Miranda
            szClient = "WebICQ";
        }
        else if (!dwFT2 && dwFT3 == 0x3B7248ED)
        {
            // And this is most probably Spam Bot
            szClient = cliSpamBot;
        }
        else if(!MatchCap(caps, wLen, &capIcqJs7, 4)&&!MatchCap(caps,wLen, &capIcqJSin, 4)&&!MatchCap(caps,wLen, &capIcqJp, 4)&&!MatchCap(caps,wLen, &capIcqJen, 4))
        {
            // Yes this is most probably Miranda, get the version info  (need better realization)
            unsigned ver1 = (dwFT2>>24)&0xFF;
            unsigned ver2 = (dwFT2>>16)&0xFF;
            unsigned ver3 = (dwFT2>>8)&0xFF;
            unsigned ver4 = (dwFT2)&0xFF;
            if(ver1 & 0x80)
            {
                makeClientVersion(szClientBuf, "Miranda IM (ICQ 0.", ver2, ver3, ver4, 0);
                lstrcatA(szClientBuf," alpha)");
            }
            else
            {
                makeClientVersion(szClientBuf, "Miranda IM (ICQ ", ver1, ver2, ver3, ver4);
                lstrcatA(szClientBuf,")");
            }
            szClient = szClientBuf;
            *bClientId = CLID_MIRANDA;
        }
    }
    else if ((dwFT1 & 0xFF7F0000) == 0x7D000000)
    {
        // This is probably an Licq client
        DWORD ver = dwFT1 & 0xFFFF;
        makeClientVersion(szClientBuf, cliLicqVer, ver / 1000, (ver / 10) % 100, ver % 10, 0);
        if (dwFT1 & 0x00800000)
            strcat(szClientBuf, "/SSL");
        szClient = szClientBuf;
    }
    else if (dwFT1 == 0xffffff8f)
    {
        szClient = "StrICQ";
    }
    else if (dwFT1 == 0xffffff42)
    {
        szClient = "mICQ";
    }
    else if (dwFT1 == 0xffffffbe)
    {
        unsigned ver1 = (dwFT2>>24)&0xFF;
        unsigned ver2 = (dwFT2>>16)&0xFF;
        unsigned ver3 = (dwFT2>>8)&0xFF;

        makeClientVersion(szClientBuf, "Alicq ", ver1, ver2, ver3, 0);

        szClient = szClientBuf;
    }
    else if (dwFT1 == 0xFFFFFF7F)
    {
        szClient = "&RQ";
    }
    else if (dwFT1 == 0xFFFFFFAB)
    {
        szClient = "YSM";
    }
    else if (dwFT1 == 0x04031980)
    {
        szClient = "vICQ";
    }
    else if ((dwFT1 == 0x3AA773EE) && (dwFT2 == 0x3AA66380))
    {
        szClient = cliLibicq2k;
    }
    else if (dwFT1 == 0x3B75AC09)
    {
        szClient = cliTrillian;
    }
    else if (dwFT1 == 0x3BA8DBAF) // FT2: 0x3BEB5373; FT3: 0x3BEB5262;
    {
        if (wVersion == 2)
            szClient = "stICQ";
    }
    else if (dwFT1 == 0xFFFFFFFE && dwFT3 == 0xFFFFFFFE)
    {
        szClient = "Jimm";
    }
    else if (dwFT1 == 0xEFFEEFFE && dwFT3 == 0xEFFEEFFE)
    {
        szClient = "wJimm";
    }
    else if (dwFT1 == 0x3FF19BEB && dwFT3 == 0x3FF19BEB)
    {
        szClient = cliIM2;
    }
    else if (dwFT1 == 0xDDDDEEFF && !dwFT2 && !dwFT3)
    {
        szClient = "SmartICQ";
    }
    else if ((dwFT1 & 0xFFFFFFF0) == 0x494D2B00 && !dwFT2 && !dwFT3)
    {
        // last byte of FT1: (5 = Win32, 3 = SmartPhone, Pocket PC)
        szClient = "IM+";
    }
    else if (dwFT1 == 0x3B4C4C0C && !dwFT2 && dwFT3 == 0x3B7248ed)
    {
        szClient = "KXicq2";
    }
    else if (dwFT1 == 0xFFFFF666 && !dwFT3)
    {
        // this is R&Q (Rapid Edition)
        null_snprintf(szClientBuf, 64, "R&Q %u", (unsigned)dwFT2);
        szClient = szClientBuf;
    }

    else if (dwFT1 == 0x48151623 && dwFT3 == 0x48151623)
    {
        // http://loc-id.ru

        {
            makeClientVersion(szClientBuf, "LocID v.", (dwFT2 >> 8) & 0x0F, (dwFT2 >> 4) & 0x0F, 0, 0);
        }

        szClient = szClientBuf;
    }
    else if (capId = MatchCap(caps, wLen, &capDiChat, 8))
    {
        strcpy(szClientBuf, "D[i]Chat");
        strncat(szClientBuf, (char*)(*capId) + 8, 8);	// FIXME
        szClient = szClientBuf;
    }
    else if (dwFT1 == 0x66666666 && dwFT3 == 0x66666666)
    {
        // http://dichat.ru/
        if (dwFT2 == 0x10000)
        {
            strcpy(szClientBuf, "D[i]Chat v.");
            strcat(szClientBuf, "0.1a");
        }
        else
        {
            makeClientVersion(szClientBuf, "D[i]Chat v.", (dwFT2 >> 8) & 0x0F, (dwFT2 >> 4) & 0x0F, 0, 0);
            if ((dwFT2 & 0x0F) == 1)
                strcat(szClientBuf, " alpha");
            else if ((dwFT2 & 0x0F) == 2)
                strcat(szClientBuf, " beta");
            else if ((dwFT2 & 0x0F) == 3)
                strcat(szClientBuf, " final");
        }
        szClient = szClientBuf;
    }
    else if (capId = MatchCap(caps, wLen, &capMraJava, 12))
    {
        unsigned ver1 = (*capId)[13];
        unsigned ver2 = (*capId)[14];
        makeClientVersion(szClientBuf, "Mail.ru Agent (Java) v", ver1, ver2, 0, 0);
        szClient = szClientBuf;
    }
    else if ((dwFT1 == dwFT2 == dwFT3) && wVersion == 8)
    {
        if ((dwFT1 < dwOnlineSince + 3600) && (dwFT1 > (dwOnlineSince - 3600)))
        {
            szClient = cliSpamBot;
        }
    }
    else if (!dwFT1 && !dwFT2 && !dwFT3 && !wVersion && !wLen && dwWebPort == 0x75BB)
    {
        szClient = cliSpamBot;
    }
    else if (dwFT1 == 0x44F523B0 && dwFT2 == 0x44F523A6 && dwFT3 == 0x44F523A6 && wVersion == 8)
    {
        szClient = "Virus";
    }

    {
        // capabilities based detection

        if (nIsICQ && caps)
        {
            // check capabilities for client identification
            if ((capId = MatchCap(caps, wLen, &capMirandaIm, 8))&&(!MatchCap(caps, wLen, &capIcqJSin, 4)&&!MatchCap(caps,wLen,&capIcqJs7, 4)&&!MatchCap(caps,wLen,&capIcqJp, 4)&&!MatchCap(caps,wLen, &capIcqJen, 4)))
            {
                // new Miranda Signature
                char sz[64];
                static char szTmp[256];
                BYTE mver0 = (*capId)[0x8];
                BYTE mver1 = (*capId)[0x9];
                BYTE mver2 = (*capId)[0xA];
                BYTE mver3 = (*capId)[0xB];
                BYTE iver0 = (*capId)[0xC];
                BYTE iver1 = (*capId)[0xD];
                BYTE iver2 = (*capId)[0xE];
                BYTE iver3 = (*capId)[0xF];
                strcpy(szTmp, "Miranda IM ");
                if (MatchCap(caps,wLen,&capMirandaMobile,13))
                    lstrcatA(szTmp, "Mobile ");
                if (mver0 == 0x80) // alpha builds directly stamped
                {
                    if (mver2 == 0x00)
                        mir_snprintf(sz, sizeof(sz), "0.%u alpha build #%u", mver1, mver3); // 0.x alpha #x
                    else
                        mir_snprintf(sz, sizeof(sz), "0.%u.%u preview #%u", mver1, mver2,mver3); // 0.x.x preview #x
                    lstrcatA(szTmp, sz);
                }
                else // others, but also for those where bug caused that alpha flag was not send
                {
                    if (mver2 == 0x00)
                        mir_snprintf(sz, sizeof(sz), "%u.%u", mver0, mver1); // 0.x final
                    else
                        mir_snprintf(sz, sizeof(sz), "%u.%u.%u", mver0, mver1, mver2); // 0.x.x final
                    lstrcatA(szTmp, sz);
                    if (mver3 != 0x00 && mver3 != 0x64) // if some build version is set, then it's probably alpha
                    {
                        mir_snprintf(sz, sizeof(sz), " alpha build #%u", mver3); // ".. alpha #x"
                        lstrcatA(szTmp, sz);
                    }
                }
                if (dwFT1 == 0x7fffffff ||(BYTE)((dwFT3>>24)&0xFF) == 0x80)
                    lstrcatA(szTmp, " Unicode");
                lstrcatA(szTmp," (ICQ ");
                if(MatchCap(caps,wLen,&capIcqJs7old, 0x10)||MatchCap(caps,wLen,&capIcqJs7s, 0x10)) //detecting sss & s7 old mod versions
                    lstrcatA(szTmp, " S7 & SSS (old)");
                else switch (iver0)
                    {
                    case 0x81 :
                        lstrcatA(szTmp, " BM");
                        break; // detecting BM mod
                        break;
                    case 0x88 :
                        lstrcatA(szTmp, " eternity (old)");
                        break; // eternity mod
                    default   :
                        break;
                    }
                switch (iver2)
                {
                case 0x58 :
                    lstrcatA(szTmp, " eternity/PlusPlus++");
                    break; // detecting naturalized eternity/PlusPlus++ mod
                default :
                    break;
                }
                lstrcatA(szTmp, " ");
                switch (iver0)
                {
                case 0x88 : // eternity (old)
                case 0x81 : // BM
                case 0x80 :
                    mir_snprintf(sz, sizeof(sz), "0.%u.%u.%u alpha)", iver1, iver2, iver3); // mir and bm 0.x
                    break;
                default   :
                    mir_snprintf(sz, sizeof(sz), "%u.%u.%u.%u)",iver0, iver1, iver2, iver3); // other x.x (only for 6.6.6.0?)
                }
                lstrcatA(szTmp,sz);
                if (dwFT3 == 0x5AFEC0DE||MatchCap(caps,wLen,&capIcqJs7s, 0x10))
                    lstrcatA(szTmp, " (SecureIM)");
                szClient = szTmp;
                *bClientId = CLID_MIRANDA;
            }
            else if (MatchCap(caps, wLen, &capTrillian, 0x10) || MatchCap(caps, wLen, &capTrilCrypt, 0x10))
            {
                // this is Trillian, check for new versions
                if (CheckContactCapabilities(hContact, CAPF_RTF))
                {
                    if (CheckContactCapabilities(hContact, CAPF_AIM_FILE))
                        szClient = "Trillian Astra";
                    else
                    {
                        // workaroud for a bug in Trillian - make it receive msgs, other features will not work!
                        ClearContactCapabilities(hContact, CAPF_SRV_RELAY);
                        szClient = "Trillian v3";
                    }
                }
                else
                    szClient = cliTrillian;
            }
            else if ((capId = MatchCap(caps, wLen, &capSimOld, 0xF)) && ((*capId)[0xF] != 0x92 && (*capId)[0xF] >= 0x20 || (*capId)[0xF] == 0))
            {
                int hiVer = (((*capId)[0xF]) >> 6) - 1;
                unsigned loVer = (*capId)[0xF] & 0x1F;
                if ((hiVer < 0) || ((hiVer == 0) && (loVer == 0)))
                    szClient = "Kopete";
                else
                {
                    makeClientVersion(szClientBuf, "SIM ", (unsigned)hiVer, loVer, 0, 0);
                    szClient = szClientBuf;
                }
            }
            else if (capId = MatchCap(caps, wLen, &capClimm, 0xC))
            {
                unsigned ver1 = (*capId)[0xC];
                unsigned ver2 = (*capId)[0xD];
                unsigned ver3 = (*capId)[0xE];
                unsigned ver4 = (*capId)[0xF];

                makeClientVersion(szClientBuf, "climm ", ver1, ver2, ver3, ver4);
                if ((ver1 & 0x80) == 0x80)
                    strcat(szClientBuf, " alpha");
                if (dwFT3 == 0x02000020)
                    strcat(szClientBuf, "/Win32");
                else if (dwFT3 == 0x03000800)
                    strcat(szClientBuf, "/MacOS X");

                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capSim, 0xC))
            {
                unsigned ver1 = (*capId)[0xC];
                unsigned ver2 = (*capId)[0xD];
                unsigned ver3 = (*capId)[0xE];
                unsigned ver4 = (*capId)[0xF];

                makeClientVersion(szClientBuf, "SIM ", ver1, ver2, ver3, ver4 & 0x0F);
                if (ver4 & 0x80)
                    strcat(szClientBuf,"/Win32");
                else if (ver4 & 0x40)
                    strcat(szClientBuf,"/MacOS X");

                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capLicq, 0xC))
            {
                unsigned ver1 = (*capId)[0xC];
                unsigned ver2 = (*capId)[0xD] % 100;
                unsigned ver3 = (*capId)[0xE];

                makeClientVersion(szClientBuf, cliLicqVer, ver1, ver2, ver3, 0);
                if ((*capId)[0xF])
                    strcat(szClientBuf,"/SSL");

                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capKopete, 0xC))
            {
                unsigned ver1 = (*capId)[0xC];
                unsigned ver2 = (*capId)[0xD];
                unsigned ver3 = (*capId)[0xE];
                unsigned ver4 = (*capId)[0xF];

                makeClientVersion(szClientBuf, "Kopete ", ver1, ver2, ver3, ver4);

                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capmIcq, 0xC))
            {
                unsigned ver1 = (*capId)[0xC];
                unsigned ver2 = (*capId)[0xD];
                unsigned ver3 = (*capId)[0xE];
                unsigned ver4 = (*capId)[0xF];

                makeClientVersion(szClientBuf, "mICQ ", ver1, ver2, ver3, ver4);
                if ((ver1 & 0x80) == 0x80) strcat(szClientBuf, " alpha");

                szClient = szClientBuf;
            }
            else if (MatchCap(caps, wLen, &capIm2, 0x10))
            {
                // IM2 v2 provides also Aim Icon cap
                szClient = cliIM2;
            }
            else if (capId = MatchCap(caps, wLen, &capAndRQ, 9))
            {
                unsigned ver1 = (*capId)[0xC];
                unsigned ver2 = (*capId)[0xB];
                unsigned ver3 = (*capId)[0xA];
                unsigned ver4 = (*capId)[9];

                makeClientVersion(szClientBuf, "&RQ ", ver1, ver2, ver3, ver4);

                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capRAndQ, 9))
            {
                unsigned ver1 = (*capId)[0xC];
                unsigned ver2 = (*capId)[0xB];
                unsigned ver3 = (*capId)[0xA];
                unsigned ver4 = (*capId)[9];

                makeClientVersion(szClientBuf, "R&Q ", ver1, ver2, ver3, ver4);

                szClient = szClientBuf;
            }
            else if (MatchCap(caps, wLen, &capIMadering, 0x10))
            {
                // http://imadering.com
                szClient = "IMadering";
            }
            else if (MatchCap(caps, wLen, &capQipPDA, 0x10))
            {
                szClient = "QIP PDA (Windows)";
            }
            else if (MatchCap(caps, wLen, &capQipMobile, 0x10))
            {
                szClient = "QIP Mobile (Java)";
            }
            else if (MatchCap(caps,wLen,&capQipSymbian,0x10))
            {
                szClient = "QIP Mobile (Symbian)";
            }
            else if (MatchCap(caps, wLen, &capQipInfium, 0x10))
            {
                char ver[10];

                strcpy(szClientBuf, "QIP Infium");
                if (dwFT1)
                {
                    // add build
                    null_snprintf(ver, 10, " (%d)", dwFT1);
                    strcat(szClientBuf, ver);
                }
                if (dwFT2 == 0x0000000B)
                    strcat(szClientBuf, " Beta");

                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capQip, 0xE))
            {
                char ver[10];

                if (dwFT3 == 0x0F)
                    strcpy(ver, "2005");
                else
                {
                    strncpy(ver, (char*)(*capId)+11, 5);
                    ver[5] = '\0'; // fill in missing zero
                }

                null_snprintf(szClientBuf, 64, cliQip, ver);
                if (dwFT1 && dwFT2 == 0x0E)
                {
                    // add QIP build
                    null_snprintf(ver, 10, " (%d%d%d%d)", dwFT1 >> 0x18, (dwFT1 >> 0x10) & 0xFF, (dwFT1 >> 0x08) & 0xFF, dwFT1 & 0xFF);
                    strcat(szClientBuf, ver);
                }
                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capmChat, 0xA))
            {
                strcpy(szClientBuf, "mChat ");
                strncat(szClientBuf, (char*)(*capId) + 0xA, 6);
                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capJimm, 5))
            {
                strcpy(szClientBuf, "Jimm ");
                strncat(szClientBuf, (char*)(*capId) + 5, 11);
                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capwJimm, 5))
            {
                strcpy(szClientBuf, "wJimm");
                strncat(szClientBuf, (char*)(*capId) + 5, 11);
                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capCorePager, 0xA))
            {
                // http://corepager.net.ru/index/0-2
                strcpy(szClientBuf, "CORE Pager");
                if (dwFT2 == 0x0FFFF0011 && dwFT3 == 0x1100FFFF && (dwFT1 >> 0x18))
                {
                    char ver[16];

                    null_snprintf(ver, 10, " %d.%d", dwFT1 >> 0x18, (dwFT1 >> 0x10) & 0xFF);
                    if ((dwFT1 & 0xFF) == 0x0B)
                        strcat(ver, " Beta");
                    strcat(szClientBuf, ver);
                }
                szClient = szClientBuf;
            }
            else if (MatchCap(caps, wLen, &capMacIcq, 0x10))
            {
                szClient = "ICQ for Mac";
            }
            else if (MatchCap(caps, wLen, &capUim, 0x10))
            {
                szClient = "uIM";
            }
            else if (MatchCap(caps, wLen, &capAnastasia, 0x10))
            {
                // http://chis.nnov.ru/anastasia
                szClient = "Anastasia";
            }
            else if (capId = MatchCap(caps, wLen, &capPalmJicq, 0xC))
            {
                // http://www.jsoft.ru
                unsigned ver1 = (*capId)[0xC];
                unsigned ver2 = (*capId)[0xD];
                unsigned ver3 = (*capId)[0xE];
                unsigned ver4 = (*capId)[0xF];

                makeClientVersion(szClientBuf, "JICQ ", ver1, ver2, ver3, ver4);

                szClient = szClientBuf;
            }
            else if (MatchCap(caps, wLen, &capInluxMsgr, 0x10))
            {
                // http://www.inlusoft.com
                szClient = "Inlux Messenger";
            }
            else if (capId = MatchCap(caps, wLen, &capVmICQ, 5))
            {
                strcpy(szClientBuf, "VmICQ ");
                strncat(szClientBuf, (char*)(*capId) + 5, 11);
                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capSmapeR, 0x07))
            {
                // http://www.smape.com/smaper
                strcpy(szClientBuf, "SmapeR");
                strncat(szClientBuf, (char*)(*capId) + 6, 10);
                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capMipClient, 4))
            {
                //http://mip.rufon.net - new signature
                strcpy(szClientBuf, "MIP ");
                strncat(szClientBuf, (char*)(*capId) + 4, 12);
                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capBayan, 8))
            {
                strcpy(szClientBuf, "BayanICQ v");
                strncat(szClientBuf, (char*)(*capId) + 8, 5);
                szClient = szClientBuf;
            }
            else if(capId = MatchCap(caps,wLen,&capYapp, 4))
            {
                strcpy(szClientBuf, "Yapp! v");
                strncat(szClientBuf, (char*)(*capId) + 8, 5);
                szClient = szClientBuf;
            }
            else if(!wVersion && CheckContactCapabilities(hContact, CAPF_DIRECT|CAPF_RTF|CAPF_TYPING|CAPF_UTF))
                szClient = "GlICQ";
            else if (MatchCap(caps, wLen, &capDigsbyBeta, 15) || MatchCap(caps, wLen, &capDigsby, 6))
            {
                // http://www.digsby.com - probably by mistake (feature detection as well)
                szClient = "Digsby";
            }
            else if (MatchCap(caps, wLen, &capJapp, 0x10))
            {
                // http://www.japp.org.ua
                szClient = "japp";
            }
            else if (MatchCap(caps, wLen, &capPigeon, 0x07))
            {
                // http://pigeon.vpro.ru
                szClient = "PIGEON!";
            }
            else if (capId = MatchCap(caps, wLen, &capQutIm, 0x05))
            {
                // http://www.qutim.org
                if ((*capId)[0x6] == 0x2E)
                {
                    // old qutim id
                    unsigned ver1 = (*capId)[0x5] - 0x30;
                    unsigned ver2 = (*capId)[0x7] - 0x30;

                    makeClientVersion(szClientBuf, "qutIM ", ver1, ver2, 0, 0);
                }
                else
                {
                    // new qutim id
                    unsigned ver1 = (*capId)[0x6];
                    unsigned ver2 = (*capId)[0x7];
                    unsigned ver3 = (*capId)[0x8];
                    unsigned ver4 = ((*capId)[0x9] << 8) || (*capId)[0xA];

                    makeClientVersion(szClientBuf, "qutIM ", ver1, ver2, ver3, ver4);

                    switch ((*capId)[0x5])
                    {
                    case 'l':
                        strcat(szClientBuf, "/Linux");
                        break;
                    case 'w':
                        strcat(szClientBuf, "/Win32");
                        break;
                    case 'm':
                        strcat(szClientBuf, "/MacOS X");
                        break;
                    }
                }
                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capJabberJIT, 0x04))
            {
                szClient = "Jabber ICQ Transport";
            }
            else if (capId = MatchCap(caps, wLen, &capIcqKid2, 0x07))
            {
                // http://sourceforge.net/projects/icqkid2
                unsigned ver1 = (*capId)[0x7];
                unsigned ver2 = (*capId)[0x8];
                unsigned ver3 = (*capId)[0x9];
                unsigned ver4 = (*capId)[0xA];

                makeClientVersion(szClientBuf, "IcqKid2 v", ver1, ver2, ver3, ver4);
                szClient = szClientBuf;
            }
            else if (capId = MatchCap(caps, wLen, &capWebIcqPro, 0x0A))
            {
                // http://intrigue.ru/workshop/webicqpro/webicqpro.html
                szClient = "WebIcqPro";
            }
            else if (szClient == cliLibicq2k)
            {
                // try to determine which client is behind libicq2000
                if (CheckContactCapabilities(hContact, CAPF_RTF))
                    szClient = cliCentericq; // centericq added rtf capability to libicq2000
                else if (CheckContactCapabilities(hContact, CAPF_UTF))
                    szClient = cliLibicqUTF; // IcyJuice added unicode capability to libicq2000
                // others - like jabber transport uses unmodified library, thus cannot be detected
            }
            else if (szClient == NULL && !((capId = MatchCap(caps, wLen, &capIcqJs7, 4))||(capId = MatchCap(caps, wLen, &capIcqJSin, 4))||(capId = MatchCap(caps, wLen, &capIcqJp, 4))||(capId = MatchCap(caps, wLen, &capIcqJen, 4)))) // HERE ENDS THE SIGNATURE DETECTION, after this only feature default will be detected
            {
                if (wVersion == 8 && CheckContactCapabilities(hContact, CAPF_XTRAZ) && (MatchCap(caps, wLen, &capIMSecKey1, 6) || MatchCap(caps, wLen, &capIMSecKey2, 6)))
                {
                    // ZA mangled the version, OMG!
                    wVersion = 9;
                }
                if (wVersion == 8 && (MatchCap(caps, wLen, &capComm20012, 0x10) || CheckContactCapabilities(hContact, CAPF_SRV_RELAY)))
                {
                    // try to determine 2001-2003 versions
                    if (MatchCap(caps, wLen, &capIs2001, 0x10))
                    {
                        if (!dwFT1 && !dwFT2 && !dwFT3)
                        {
                            if (CheckContactCapabilities(hContact, CAPF_RTF))
                                szClient = "TICQClient"; // possibly also older GnomeICU
                            else
                                szClient = "ICQ for Pocket PC";
                        }
                        else
                        {
                            *bClientId = CLID_GENERIC;
                            szClient = "ICQ 2001";
                        }
                    }
                    else if (MatchCap(caps, wLen, &capIs2002, 0x10))
                    {
                        *bClientId = CLID_GENERIC;
                        szClient = "ICQ 2002";
                    }
                    else if (CheckContactCapabilities(hContact, CAPF_SRV_RELAY | CAPF_UTF | CAPF_RTF) && !MatchCap(caps, wLen, &capIcqJs7, 0x4) && !MatchCap(caps,wLen,&capIcqJSin, 0x4))
                    {
                        if (!dwFT1 && !dwFT2 && !dwFT3)
                        {
                            if (!dwWebPort)
                                szClient = "GnomeICU 0.99.5+"; // no other way
                            else
                                szClient = "IC@";
                        }
                        else
                        {
                            *bClientId = 0;
                            szClient = "ICQ 2002/2003a";
                        }
                    }
                    else if (CheckContactCapabilities(hContact, CAPF_SRV_RELAY | CAPF_UTF | CAPF_TYPING))
                    {
                        if (!dwFT1 && !dwFT2 && !dwFT3)
                        {
                            szClient = "PreludeICQ";
                        }
                    }
                }
                else if (wVersion == 9)
                {
                    // try to determine lite versions
                    if (CheckContactCapabilities(hContact, CAPF_XTRAZ))
                    {
                        *bClientId = CLID_GENERIC;
                        if (CheckContactCapabilities(hContact, CAPF_AIM_FILE))
                        {
                            if (MatchCap(caps, wLen, &captZers, 0x10))
                            {
                                // capable of tZers ?
                                if (MatchCap(caps, wLen, &capIcqLiteNew, 0x10) && MatchCap(caps, wLen, &capStatusTextAware, 0x10) &&
                                        MatchCap(caps, wLen, &capAimLiveVideo, 0x10) && MatchCap(caps, wLen, &capAimLiveAudio, 0x10))
                                {
                                    strcpy(szClientBuf, "ICQ 7");
                                }
                                else if (MatchCap(caps, wLen, &capHtmlMsgs, 0x10))
                                {
                                    if(MatchCap(caps, wLen, &capAimLiveVideo, 0x10))
                                    {
                                        strcpy(szClientBuf, "ICQ 6");
                                        *bClientId = CLID_ICQ6;
                                    }
                                }
                                else
                                {
                                    strcpy(szClientBuf, "icq5.1");
                                }
                                SetContactCapabilities(hContact, CAPF_STATUSMSGEXT);
                            }
                            else
                            {
                                strcpy(szClientBuf, "icq5");
                            }
                            if (MatchCap(caps, wLen, &capRambler, 0x10))
                            {
                                strcat(szClientBuf, " (Rambler)");
                            }
                            else if (MatchCap(caps, wLen, &capAbv, 0x10))
                            {
                                strcat(szClientBuf, " (Abv)");
                            }
                            else if (MatchCap(caps, wLen, &capNetvigator, 0x10))
                            {
                                strcat(szClientBuf, " (Netvigator)");
                            }
                            szClient = szClientBuf;
                        }
                        else if (!CheckContactCapabilities(hContact, CAPF_DIRECT))
                            if (CheckContactCapabilities(hContact, CAPF_RTF))
                            {
                                // most probably Qnext - try to make that shit at least receiving our msgs
                                ClearContactCapabilities(hContact, CAPF_SRV_RELAY);
                                NetLog_Server("Forcing simple messages (QNext client).");
                                szClient = "QNext";
                            }
                            else if (CheckContactCapabilities(hContact, CAPF_ICQ_DEVIL | CAPF_XTRAZ| CAPF_SRV_RELAY | CAPF_TYPING | CAPF_XTRAZ_CHAT  | CAPF_UTF ) && MatchCap(caps, wLen, &capAimLiveAudio, 0x10) && MatchCap(caps, wLen, &captZers, 0x10) && MatchCap(caps, wLen, &capHtmlMsgs, 0x10))
                                szClient = "Mail.ru Agent (PC)";
                            else if (MatchCap(caps, wLen, &capHtmlMsgs, 0x10))
                                szClient = "Fring";
                            else
                                szClient = "pyICQ";
                        else
                            szClient = "ICQ Lite v4";
                    }
                    else if (CheckContactCapabilities(hContact, CAPF_AIM_FILE | CAPF_UTF))
                    {
                        if (MatchCap(caps, wLen, &capAimIcon, 0x10) && MatchCap(caps, wLen, &capAimContactSnd, 0x10) && MatchCap(caps, wLen, &capAimChat, 0x10) && MatchCap(caps, wLen, &capAimLiveVideo, 0x10) && MatchCap(caps, wLen, &capAimLiveAudio, 0x10))
                            szClient = "ICQ Lite v7";
                    }
                    else if (!CheckContactCapabilities(hContact, CAPF_DIRECT))
                    {
                        if (CheckContactCapabilities(hContact, CAPF_UTF) && !CheckContactCapabilities(hContact, CAPF_RTF))
                            szClient = "pyICQ";
                    }
                }
                else if (wVersion == 7)
                {
                    if (CheckContactCapabilities(hContact, CAPF_RTF))
                        szClient = "GnomeICU"; // this is an exception
                    else if (CheckContactCapabilities(hContact, CAPF_SRV_RELAY))
                    {
                        if (!dwFT1 && !dwFT2 && !dwFT3)
                            szClient = "&RQ";
                        else
                        {
                            *bClientId = CLID_GENERIC;
                            szClient = "ICQ 2000";
                        }
                    }
                    else if (CheckContactCapabilities(hContact, CAPF_UTF))
                    {
                        if (CheckContactCapabilities(hContact, CAPF_TYPING))
                            szClient = "Icq2Go! (Java)";
                        else if (wUserClass & CLASS_WIRELESS)
                            szClient = "Pocket Web 1&1";
                        else
                            szClient = "Icq2Go! (Flash)"; // fixing overlay in Fingerprint
                    }
                    /* else if (CheckContactCapabilities(hContact, CAPF_UTF))
                     {
                       if (CheckContactCapabilities(hContact, CAPF_TYPING))
                         szClient = "Icq2Go! (Java)";
                       else
                         szClient = "Icq2Go! (Flash)";
                     }*/
                }
                else if (wVersion == 0xA)
                {
                    if (!CheckContactCapabilities(hContact, CAPF_RTF) && !CheckContactCapabilities(hContact, CAPF_UTF))
                    {
                        // this is bad, but we must do it - try to detect QNext
                        ClearContactCapabilities(hContact, CAPF_SRV_RELAY);
                        NetLog_Server("Forcing simple messages (QNext client).");
                        szClient = "QNext";
                    }
                    else if (!CheckContactCapabilities(hContact, CAPF_RTF) && CheckContactCapabilities(hContact, CAPF_UTF) && !dwFT1 && !dwFT2 && !dwFT3)
                    {
                        // not really good, but no other option
                        szClient = "NanoICQ";
                    }
                }
                else if (wVersion == 0xB)
                {
                    if (CheckContactCapabilities(hContact, CAPF_XTRAZ| CAPF_SRV_RELAY | CAPF_DIRECT | CAPF_AIM_FILE | CAPF_TYPING | CAPF_UTF ) && MatchCap(caps, wLen, &capAimContactSnd, 0x10))
                    {
                        szClient = "Mail.ru Agent (Symbian)";
                    }
                    else if (CheckContactCapabilities(hContact, CAPF_XTRAZ| CAPF_SRV_RELAY | CAPF_TYPING | CAPF_AIM_FILE  | CAPF_UTF) && MatchCap(caps, wLen, &capIcqDevil, 0x10))
                    {
                        szClient = "Mail.ru Agent (Symbian)";
                    }
                }
                else if (wVersion == 0x7A69)
                {
                    szClient = "QIP Infium";
                }
                else if (!wVersion)
                {
                    // capability footprint based detection - not really reliable
                    if (!dwFT1 && !dwFT2 && !dwFT3 && !dwWebPort && !dwDirectCookie)
                    {
                        // DC info is empty
                        if (CheckContactCapabilities(hContact, CAPF_TYPING) && MatchCap(caps, wLen, &capIs2001, 0x10) &&
                                MatchCap(caps, wLen, &capIs2002, 0x10) && MatchCap(caps, wLen, &capComm20012, 0x10))
                            szClient = cliSpamBot;
                        else if (MatchCap(caps, wLen, &capAimIcon, 0x10) && MatchCap(caps, wLen, &capAimDirect, 0x10) &&
                                 CheckContactCapabilities(hContact, CAPF_AIM_FILE | CAPF_UTF))
                        {
                            // detect libgaim/libpurple versions
                            if (CheckContactCapabilities(hContact, CAPF_SRV_RELAY))
                                szClient = "Adium X"; // yeah, AFAIK only Adium has this fixed
                            else if (CheckContactCapabilities(hContact, CAPF_TYPING))
                                szClient = "libpurple";
                            else
                                szClient = "libgaim";
                        }
                        else if (MatchCap(caps, wLen, &capAimIcon, 0x10) && MatchCap(caps, wLen, &capAimDirect, 0x10) &&
                                 MatchCap(caps, wLen, &capAimChat, 0x10) && CheckContactCapabilities(hContact, CAPF_AIM_FILE) && wLen == 0x40)
                            szClient = "libgaim"; // Gaim 1.5.1 most probably
                        else if (MatchCap(caps, wLen, &capAimChat, 0x10) && CheckContactCapabilities(hContact, CAPF_AIM_FILE) && wLen == 0x20)
                            szClient = "Easy Message";
                        else if (MatchCap(caps, wLen, &capAimIcon, 0x10) && MatchCap(caps, wLen, &capAimChat, 0x10) && CheckContactCapabilities(hContact, CAPF_UTF | CAPF_TYPING) && wLen == 0x40)
                            szClient = "Meebo";
                        else if (MatchCap(caps, wLen, &capAimIcon, 0x10) && CheckContactCapabilities(hContact, CAPF_UTF) && wLen == 0x20)
                            szClient = "PyICQ-t Jabber Transport";
                        else if (MatchCap(caps, wLen, &capAimIcon, 0x10) && MatchCap(caps, wLen, &capIcqLite, 0x10) && CheckContactCapabilities(hContact, CAPF_UTF | CAPF_XTRAZ))
                            szClient = "PyICQ-t Jabber Transport";
                        else if (MatchCap(caps, wLen, &capAimIcon, 0x10) && CheckContactCapabilities(hContact, CAPF_UTF | CAPF_TYPING | CAPF_SRV_RELAY | CAPF_XTRAZ))
                            szClient = "PyICQ-t Jabber Transport";
                        else if (CheckContactCapabilities(hContact, CAPF_UTF | CAPF_SRV_RELAY | CAPF_DIRECT | CAPF_TYPING) && wLen == 0x40)
                            szClient = "Agile Messenger"; // Smartphone 2002
                        else if (CheckContactCapabilities(hContact, CAPF_UTF | CAPF_SRV_RELAY | CAPF_DIRECT | CAPF_AIM_FILE) && MatchCap(caps, wLen, &capAimFileShare, 0x10))
                            szClient = "Slick"; // http://lonelycatgames.com/?app=slick
                        else if(!caps)
                            szClient = "Icq2Go!"; //is anyone know better way for detect this client ?
                    }
                }
            }
        }
        else if (!dwUin)
        {
            // detect AIM clients
            if (caps)
            {
                if (capId = MatchCap(caps, wLen, &capAimOscar, 8))
                {
                    // AimOscar Signature
                    char sz[64];
                    static char szTmp[256];
                    BYTE mver0 = (*capId)[0x8];
                    BYTE mver1 = (*capId)[0x9];
                    BYTE mver2 = (*capId)[0xA];
                    BYTE mver3 = (*capId)[0xB];
                    BYTE iver0 = (*capId)[0xC];
                    BYTE iver1 = (*capId)[0xD];
                    BYTE iver2 = (*capId)[0xE];
                    BYTE iver3 = (*capId)[0xF];
                    strcpy(szTmp, "Miranda IM ");
                    if (mver0 & 0x80)
                    {
                        mir_snprintf(sz, sizeof(sz), "0.%u.%u alpha build #%u", mver1, mver2,mver3);
                        lstrcatA(szTmp, sz);
                    }
                    else
                    {
                        mir_snprintf(sz, sizeof(sz), "%u.%u.%u.%u", mver0, mver1, mver2,mver3);
                        lstrcatA(szTmp, sz);
                    }
                    lstrcatA(szTmp," (AIM v");
                    if (iver0 & 0x80)
                    {
                        mir_snprintf(sz, sizeof(sz),  "0.%u.%u.%u alpha)", iver1, iver2, iver3);
                        lstrcatA(szTmp,sz);
                    }
                    else
                    {
                        mir_snprintf(sz, sizeof(sz),  "%u.%u.%u.%u)",iver0, iver1, iver2, iver3);
                        lstrcatA(szTmp,sz);
                    }
                    /*			  if (dwFT3 == 0x5AFEC0DE) //I don't know if integratiion with SecureIM is done
                    		  lstrcatA(szTmp, " (with SecureIM)"); */
                    szClient = szTmp;
                }
                else if (capId = MatchCap(caps, wLen, &capSim, 0xC))
                {
                    // Sim is universal
                    unsigned ver1 = (*capId)[0xC];
                    unsigned ver2 = (*capId)[0xD];
                    unsigned ver3 = (*capId)[0xE];
                    makeClientVersion(szClientBuf, "SIM ", ver1, ver2, ver3, 0);
                    if ((*capId)[0xF] & 0x80)
                        strcat(szClientBuf,"/Win32");
                    else if ((*capId)[0xF] & 0x40)
                        strcat(szClientBuf,"/MacOS X");
                    szClient = szClientBuf;
                }
                else if (capId = MatchCap(caps, wLen, &capKopete, 0xC))
                {
                    unsigned ver1 = (*capId)[0xC];
                    unsigned ver2 = (*capId)[0xD];
                    unsigned ver3 = (*capId)[0xE];
                    unsigned ver4 = (*capId)[0xF];
                    makeClientVersion(szClientBuf, "Kopete ", ver1, ver2, ver3, ver4);
                    szClient = szClientBuf;
                }
                else if (MatchCap(caps, wLen, &capIm2, 0x10))
                {
                    // IM2 extensions
                    szClient = cliIM2;
                }
                else if (MatchCap(caps, wLen, &capNaim, 0x8))
                {
                    szClient = "naim";
                }
                else if(MatchCap(caps, wLen, &capAimIcon, 0x10))
                {
                    if(MatchCap(caps, wLen, &capAimAddins, 0x10) && MatchCap(caps, wLen, &capAimUnk2, 0x10))
                    {
                        szClient = "AIM 5";
                    }
                    else if(MatchCap(caps, wLen, &capAimLiveAudio, 0x10) && MatchCap(caps, wLen, &capAimContactSnd, 0x10))
                    {
                        szClient = "AIM Pro";
                    }
                    else if(MatchCap(caps, wLen, &capAimLiveAudio, 0x10))
                    {
                        szClient = "AIM 6";
                    }
                    else
                    {
                        szClient = "AIM";
                    }
                }
            }
        }
    }
    if (!szClient&&!MatchCap(caps, wLen, &capIcqJs7, 4)&&!MatchCap(caps, wLen, &capIcqJSin, 4)&&!MatchCap(caps, wLen, &capIcqJp, 4)&&!MatchCap(caps, wLen, &capIcqJen, 4))
    {
        NetLog_Server("No client identification, put default ICQ client for protocol.");
        *bClientId = CLID_GENERIC;
        switch (wVersion)
        {
            // client detection failed, provide default clients
        case 6:
            szClient = "ICQ99";
            break;
        case 7:
            szClient = "ICQ 2000/Icq2Go";
            break;
        case 8:
            szClient = "ICQ 2001-2003a";
            break;
        case 9:
            szClient = "ICQ Lite";
            break;
        case 0xA:
            szClient = "ICQ 2003b";
            break;
        case 0xB:
            szClient = "QIP 2005a";
            break;
        }
    }
    if (!szClient)
    {
        if ((capId = MatchCap(caps, wLen, &capIcqJs7, 4))||(capId = MatchCap(caps, wLen, &capIcqJSin, 4))||(capId = MatchCap(caps, wLen, &capIcqJp, 4))||(capId = MatchCap(caps, wLen, &capIcqJen, 4))) //detecting mod and core version, please fix me
        {
            char sz[64];
            static char szTmp[256];
            BYTE mver0 = (*capId)[0x4];
            BYTE mver1 = (*capId)[0x5];
            BYTE mver2 = (*capId)[0x6];
            BYTE mver3 = (*capId)[0x7];
            BYTE iver0 = (*capId)[0x8];
            BYTE iver1 = (*capId)[0x9];
            BYTE iver2 = (*capId)[0xA];
            BYTE iver3 = (*capId)[0xB];
            BYTE secure = (*capId)[0xC];
            BYTE ModVer = (*capId)[0x3];
            BYTE Unicode = (BYTE)(dwFT3>>24)&0xFF;
            if (mver1 < 20 && iver1 < 20)
            {
                strcpy(szTmp, "Miranda IM ");
                if (mver0 == 0x80)
                {
                    if (mver2 == 0x00)
                        mir_snprintf(sz, sizeof(sz), "0.%u alpha build #%u", mver1, mver3);
                    else
                        mir_snprintf(sz, sizeof(sz), "0.%u.%u alpha build #%u", mver1, mver2,mver3);
                    lstrcatA(szTmp, sz);
                }
                else
                {
                    if (mver2 == 0x00)
                        mir_snprintf(sz, sizeof(sz), "%u.%u", mver0, mver1);
                    else
                        mir_snprintf(sz, sizeof(sz), "%u.%u.%u", mver0, mver1, mver2);
                    lstrcatA(szTmp, sz);
                    if (mver3 != 0x00 && mver3 != 0x64)
                    {
                        mir_snprintf(sz, sizeof(sz), " alpha build #%u", mver3);
                        lstrcatA(szTmp, sz);
                    }
                }
                if (Unicode == 0x80||dwFT1 == 0x7fffffff)
                    lstrcatA(szTmp," Unicode");
                if (ModVer == 'p')
                    lstrcatA(szTmp," (ICQ "ICQ_MODNAME"");
                else if (((*capId)[0x0]) == 's')
                    lstrcatA(szTmp," (ICQ S!N");
                else if (((*capId)[0x0]) == 'e')
                    lstrcatA(szTmp," (ICQ eternity/PlusPlus++"); //mod of mod of mod of mod of mod
                else if(ModVer == 'j')
                    lstrcatA(szTmp," (ICQ S7 & SSS");
                if (iver0 == 0x80)
                {
                    mir_snprintf(sz, sizeof(sz),  " 0.%u.%u.%u alpha)", iver1, iver2, iver3);
                    lstrcatA(szTmp,sz);
                }
                else
                {
                    mir_snprintf(sz, sizeof(sz),  " %u.%u.%u.%u)",iver0, iver1, iver2, iver3);
                    lstrcatA(szTmp,sz);
                }
                if (secure != 0 && secure != 20||dwFT3 == 0x5AFEC0DE)
                    lstrcatA(szTmp, " (SecureIM)");
            }
            else if(MatchCap(caps,wLen,&capIcqJs7s,0x10))
                strcpy(szTmp,"Miranda IM (ICQ SSS & S7)(SecureIM)");
            else if(MatchCap(caps,wLen,&capIcqJs7old,0x10))
                strcpy(szTmp,"Miranda IM (ICQ SSS & S7)");
            szClient = szTmp;
        }
        else if(CheckContactCapabilities(hContact, CAPF_SRV_RELAY | CAPF_DIRECT | CAPF_TYPING) && wVersion > 1000)
        {
            szClient = "QIP 2005a (as Unknown)";
        }
        else if(CheckContactCapabilities(hContact, CAPF_UTF)&&!CheckContactCapabilities(hContact, CAPF_SRV_RELAY)&&!CheckContactCapabilities(hContact, CAPF_TYPING))//i hate pocket web ;)
            szClient = "Pocket Web 1&1";
    }
    if (szClient)
    {
        char* szExtra = NULL;
        if (MatchCap(caps, wLen, &capSimpLite, 0x10))
            szExtra = " + SimpLite";
        if (MatchCap(caps, wLen, &capSimpPro, 0x10))
            szExtra = " + SimpPro";
        if (MatchCap(caps, wLen, &capIMsecure, 0x10) || MatchCap(caps, wLen, &capIMSecKey1, 6) || MatchCap(caps, wLen, &capIMSecKey2, 6))
            szExtra = " + IMsecure";
        if (!strstr(szClient, "Miranda") && ((capId = MatchCap(caps, wLen, &capIcqJs7, 4))||(capId = MatchCap(caps, wLen, &capIcqJp, 4))||(capId = MatchCap(caps, wLen, &capIcqJSin, 4))||(capId = MatchCap(caps, wLen, &capIcqJen, 4)))) //detecting mod and core version, please fix me
        {
            char sz[64];
            static char szTmp[256];
            BYTE mver0 = (*capId)[0x4];
            BYTE mver1 = (*capId)[0x5];
            BYTE mver2 = (*capId)[0x6];
            BYTE mver3 = (*capId)[0x7];
            BYTE iver0 = (*capId)[0x8];
            BYTE iver1 = (*capId)[0x9];
            BYTE iver2 = (*capId)[0xA];
            BYTE iver3 = (*capId)[0xB];
            BYTE secure = (*capId)[0xC];
            BYTE ModVer = (*capId)[0x3];
            BYTE Unicode = (BYTE)(dwFT3>>24)&0xFF;
            if (mver1 < 20 && iver1 < 20)
            {
                strcpy(szTmp, " (Miranda IM ");
                if (mver0 & 0x80)
                {
                    mir_snprintf(sz, sizeof(sz), "0.%u.%u alpha build #%u", mver1, mver2,mver3);
                    lstrcatA(szTmp, sz);
                }
                else
                {
                    mir_snprintf(sz, sizeof(sz), "%u.%u.%u.%u", mver0, mver1, mver2,mver3);
                    lstrcatA(szTmp, sz);
                }
                if (Unicode == 0x80||dwFT1 == 0x7fffffff)
                    lstrcatA(szTmp," Unicode");
                if (ModVer == 'p')
                    lstrcatA(szTmp," (ICQ "ICQ_MODNAME"");
                else if (((*capId)[0x0]) == 's')
                    lstrcatA(szTmp," (ICQ S!N");
                else if (((*capId)[0x0]) == 'e')
                    lstrcatA(szTmp," (ICQ eternity/PlusPlus++"); //mod of mod of mod of mod of mod
                else if(ModVer == 'j')
                    lstrcatA(szTmp," (ICQ S7 & SSS");
                if (iver0 == 0x80)
                {
                    mir_snprintf(sz, sizeof(sz),  " 0.%u.%u.%u alpha)", iver1, iver2, iver3);
                    lstrcatA(szTmp,sz);
                }
                else
                {
                    mir_snprintf(sz, sizeof(sz),  " %u.%u.%u.%u)",iver0, iver1, iver2, iver3);
                    lstrcatA(szTmp,sz);
                }
                lstrcatA(szTmp,")");
                if (secure != 0 && secure != 20||dwFT3 == 0x5AFEC0DE)
                    lstrcatA(szTmp, " (SecureIM)");
            }
            if (iver1 >= 20 && mver1 >= 20||MatchCap(caps, wLen, &capIcqJs7old, 0x10))
            {
                strcpy(sz, " (Miranda IM (ICQ S7 & SSS))");
                strcpy(szTmp,sz);
                if (MatchCap(caps, wLen, &capIcqJs7s, 0x10))
                    lstrcatA(sz," (SecureIM))");
            }
            szExtra=szTmp;
        }
        if (szExtra)
        {
            if (szClient != szClientBuf)
            {
                strcpy(szClientBuf, szClient);
                szClient = szClientBuf;
            }
            if  (szClient != szExtra)
                lstrcatA(szClient, szExtra);
        }
    }
    /*
      { // custom miranda packs
    	  char *capId;
    	  if (capId = (char*)MatchCap(caps, wLen, &capMimPack, 4))
    	  {
    		  char szPack[16];
    		  mir_snprintf(szPack, 16, " [%.12s]", capId+4);
    		  lstrcatA(szClient, szPack);
    	  }
      }
    */
    {
        // custom miranda packs - multiple pack caps processing v1
        char *capId;
        char szOldPack[16];
        int i = 0;
        BYTE *capsPos;
        capsPos = caps;
        lstrcpyA( szOldPack, "" );
        for( i = 0; ; i++ )
        {
            if ( MatchCap( capsPos, wLen, &capMimPack, 4 ) )
            {
                char szPack[16];
                capId = ( char* )MatchCap( capsPos, wLen, &capMimPack, 4 );
                capsPos = caps + i * 16;
                mir_snprintf( szPack, 16, " [%.12s]", capId + 4 );
                if ( lstrcmpA( szPack, szOldPack) == 0 )
                    continue;
                else
                {
                    lstrcatA( szClient, szPack );
                    lstrcpyA( szOldPack, szPack );
                }
            }
            else
                break;
        }
    }
    NetLog_Server("Client identified as %s", szClient);
//  ICQWriteContactSettingUtf(hContact, "MirVer", szClient);  //thi already fixed in other place
    return szClient;
}
