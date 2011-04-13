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
//  Contains helper functions to handle oscar user capabilities. Scanning and
//  adding capabilities are assumed to be more timecritical than looking up
//  capabilites. During the login sequence there could possibly be many hundred
//  scans but only a few lookups. So when you add or change something in this
//  code you must have this in mind, dont do anything that will slow down the
//  adding process too much.
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"

// -----------------------------------------------------------------------------
const capstr capSrvRelay  = {CAP_SRV_RELAY};
const capstr capNewCap	  = {CAP_NEWCAP};
const capstr capUTF       = {CAP_UTF};
const capstr capTyping    = {CAP_TYPING};
const capstr capXtraz     = {CAP_XTRAZ};
const capstr capAimFile   = {CAP_AIM_FILE};
const capstr capPush2Talk = {CAP_PUSH2TALK};
// this is defined among all clients caps
const capstr capRTF       = {CAP_RTF};
const capstr capXtrazChat = {CAP_XTRAZ_CHAT};
const capstr capVoiceChat = {CAP_VOICE_CHAT};
const capstr capIcqDevil  = {CAP_ICQ_DEVIL};
const capstr capDirect    = {CAP_DIRECT};
// -----------------------------------------------------------------------------
const capstr capMirandaIm = {'M', 'i', 'r', 'a', 'n', 'd', 'a', 'M', 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capMirandaMobile = {'M', 'i', 'r', 'a', 'n', 'd', 'a', 'M', 'o', 'b', 'i', 'l', 'e', 0, 0, 0}; //port by Nullbie
const capstr capMimPack   = {'M', 'I', 'M', '/', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capIcqJs7    = {'i', 'c', 'q', 'j', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capIcqJp     = {'i', 'c', 'q', 'p', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capIcqJs7s	  = {'i', 'c', 'q', 'j', 0, 'S', 'e', 'c', 'u', 'r', 'e', 0, 'I', 'M', 0, 0};
const capstr capIcqJs7old =	{0x69, 0x63, 0x71, 0x6a, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capIcqJSin	  = {'s', 'i', 'n', 'j',  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Miranda ICQJ S!N
const capstr capIcqJen    = {'e', 'n', 'q', 'j', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // eternity mod (mod of mod of mod of mod of mod)
const capstr capAimOscar  = {'M', 'i', 'r', 'a', 'n', 'd', 'a', 'A', 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capTrillian  = {0x97, 0xb1, 0x27, 0x51, 0x24, 0x3c, 0x43, 0x34, 0xad, 0x22, 0xd6, 0xab, 0xf7, 0x3f, 0x14, 0x09};
const capstr capTrilCrypt = {0xf2, 0xe7, 0xc7, 0xf4, 0xfe, 0xad, 0x4d, 0xfb, 0xb2, 0x35, 0x36, 0x79, 0x8b, 0xdf, 0x00, 0x00};
const capstr capSim       = {'S', 'I', 'M', ' ', 'c', 'l', 'i', 'e', 'n', 't', ' ', ' ', 0, 0, 0, 0};
const capstr capSimOld    = {0x97, 0xb1, 0x27, 0x51, 0x24, 0x3c, 0x43, 0x34, 0xad, 0x22, 0xd6, 0xab, 0xf7, 0x3f, 0x14, 0x00};
const capstr capLicq      = {'L', 'i', 'c', 'q', ' ', 'c', 'l', 'i', 'e', 'n', 't', ' ', 0, 0, 0, 0};
const capstr capKopete    = {'K', 'o', 'p', 'e', 't', 'e', ' ', 'I', 'C', 'Q', ' ', ' ', 0, 0, 0, 0};
const capstr capmIcq      = {'m', 'I', 'C', 'Q', ' ', 0xA9, ' ', 'R', '.', 'K', '.', ' ', 0, 0, 0, 0};
const capstr capClimm     = {'c', 'l', 'i', 'm', 'm', 0xA9, ' ', 'R', '.', 'K', '.', ' ', 0, 0, 0, 0};
const capstr capAndRQ     = {'&', 'R', 'Q', 'i', 'n', 's', 'i', 'd', 'e', 0, 0, 0, 0, 0, 0, 0};
const capstr capRAndQ     = {'R', '&', 'Q', 'i', 'n', 's', 'i', 'd', 'e', 0, 0, 0, 0, 0, 0, 0};
const capstr capRAndQ_1   = {0xd6, 0x68, 0x7f, 0x4f, 0x3d, 0xc3, 0x4b, 0xdb, 0x8a, 0x8c, 0x4c, 0x1a, 0x57, 0x27, 0x63, 0xcd}; // R&Q-ProtectMsg
const capstr capmChat     = {'m', 'C', 'h', 'a', 't', ' ', 'i', 'c', 'q', ' ', 0, 0, 0, 0, 0, 0};
const capstr capJimm      = {'J', 'i', 'm', 'm', ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capwJimm     = {'w', 'J', 'i', 'm', 'm', ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capLocID     = {'L', 'o', 'c', 'I', 'D', ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // LocID
const capstr capBayan	  = {0x62, 0x61, 0x79, 0x61, 0x6e, 0x49, 0x43, 0x51, 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capCorePager = {'C', 'O', 'R', 'E', ' ', 'P', 'a', 'g', 'e', 'r', 0, 0, 0, 0, 0, 0};
const capstr capDiChat    = {'D', '[', 'i', ']', 'C', 'h', 'a', 't', ' ', 0, 0, 0, 0, 0, 0, 0};
const capstr capNaim      = {0xFF, 0xFF, 0xFF, 0xFF, 'n', 'a', 'i', 'm', 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capAnastasia = {0x44, 0xE5, 0xBF, 0xCE, 0xB0, 0x96, 0xE5, 0x47, 0xBD, 0x65, 0xEF, 0xD6, 0xA3, 0x7E, 0x36, 0x02};
const capstr capQip       = {0x56, 0x3F, 0xC8, 0x09, 0x0B, 0x6F, 0x41, 'Q', 'I', 'P', ' ', '2', '0', '0', '5', 'a'};
const capstr capQipPDA    = {0x56, 0x3F, 0xC8, 0x09, 0x0B, 0x6F, 0x41, 'Q', 'I', 'P', ' ', ' ', ' ', ' ', ' ', '!'};
const capstr capQipMobile = {0x56, 0x3F, 0xC8, 0x09, 0x0B, 0x6F, 0x41, 'Q', 'I', 'P', ' ', ' ', ' ', ' ', ' ', '"'};
const capstr capQipInfium = {0x7C, 0x73, 0x75, 0x02, 0xC3, 0xBE, 0x4F, 0x3E, 0xA6, 0x9F, 0x01, 0x53, 0x13, 0x43, 0x1E, 0x1A};
const capstr capQipPlugins = {0x7c, 0x53, 0x3f, 0xfa, 0x68, 0x00, 0x4f, 0x21, 0xbc, 0xfb, 0xc7, 0xd2, 0x43, 0x9a, 0xad, 0x31}; //QIP Plugins
const capstr capQipServices = {0x8c, 0x54, 0x3d, 0xfc, 0x69, 0x02, 0x4d, 0x25, 0xbf, 0xfa, 0xc0, 0xd3, 0x41, 0x9c, 0xaf, 0x30}; //QIP Services
const capstr capQip_1     = {0xd3, 0xd4, 0x53, 0x19, 0x8b, 0x32, 0x40, 0x3b, 0xac, 0xc7, 0xd1, 0xa9, 0xe2, 0xb5, 0x81, 0x3e}; // QIP-ProtectMsg (now use for fake id only)
const capstr capQipSymbian = {0x51, 0xad, 0xd1, 0x90, 0x72, 0x04, 0x47, 0x3d, 0xa1, 0xa1, 0x49, 0xf4, 0xa3, 0x97, 0xa4, 0x1f};
const capstr capVmICQ	  = {0x56, 0x6d, 0x49, 0x43, 0x51, 0x20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capSmapeR    = {'S', 'm', 'a', 'p', 'e', 'r', ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capPigeon    = {'P', 'I', 'G', 'E', 'O', 'N', '!', 0, 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capDigsbyBeta= {0x09, 0x46, 0x01, 0x05, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x45, 0x53, 0x54, 0};
const capstr capDigsby    = {0x64, 0x69, 0x67, 0x73, 0x62, 0x79, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capJapp      = {0x6a, 0x61, 0x70, 0x70, 0xa9, 0x20, 0x62, 0x79, 0x20, 0x53, 0x65, 0x72, 0x67, 0x6f, 0x00, 0x00};
const capstr capIMPlus	  = {0x8e, 0xcd, 0x90, 0xe7, 0x4f, 0x18, 0x28, 0xf8, 0x02, 0xec, 0xd6, 0x18, 0xa4, 0xe9, 0xde, 0x68};
const capstr capYapp	  = {0x59, 0x61, 0x70, 0x70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Yapp
const capstr capMraJava	  = {0x4a, 0x32, 0x4d, 0x45, 0x20, 0x6d, 0x40, 0x61, 0x67, 0x65, 0x6e, 0x74, 0, 0, 0, 0};
const capstr capQutIm     = {'q', 'u', 't', 'i', 'm', 0x30, 0x2e, 0x31, 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capJabberJIT = {'J', 'I', 'T', ' ', 0x76, 0x2E, 0x31, 0x2E, 0x78, 0x2E, 0x78, 0, 0, 0, 0, 0};
const capstr capIcqKid2   = {'I', 'c', 'q', 'K', 'i', 'd', '2', 0x00, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const capstr capWebIcqPro = {'W', 'e', 'b', 'I', 'c', 'q', 'P', 'r', 'o', ' ', 0, 0, 0, 0, 0, 0};
const capstr capIm2       = {0x74, 0xED, 0xC3, 0x36, 0x44, 0xDF, 0x48, 0x5B, 0x8B, 0x1C, 0x67, 0x1A, 0x1F, 0x86, 0x09, 0x9F}; // IM2 Ext Msg
const capstr capMacIcq    = {0xdd, 0x16, 0xf2, 0x02, 0x84, 0xe6, 0x11, 0xd4, 0x90, 0xdb, 0x00, 0x10, 0x4b, 0x9b, 0x4b, 0x7d};
const capstr capIs2001    = {0x2e, 0x7a, 0x64, 0x75, 0xfa, 0xdf, 0x4d, 0xc8, 0x88, 0x6f, 0xea, 0x35, 0x95, 0xfd, 0xb6, 0xdf};
const capstr capIs2002    = {0x10, 0xcf, 0x40, 0xd1, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
const capstr capComm20012 = {0xa0, 0xe9, 0x3f, 0x37, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
const capstr capStrIcq    = {0xa0, 0xe9, 0x3f, 0x37, 0x4f, 0xe9, 0xd3, 0x11, 0xbc, 0xd2, 0x00, 0x04, 0xac, 0x96, 0xdd, 0x96};
const capstr capIcqLite   = {0x17, 0x8C, 0x2D, 0x9B, 0xDA, 0xA5, 0x45, 0xBB, 0x8D, 0xDB, 0xF3, 0xBD, 0xBD, 0x53, 0xA1, 0x0A};
const capstr capIcqLiteNew= {0xc8, 0x95, 0x3a, 0x9f, 0x21, 0xf1, 0x4f, 0xaa, 0xb0, 0xb2, 0x6d, 0xe6, 0x63, 0xab, 0xf5, 0xb7}; // new cap icq lite 7
const capstr capUim       = {0xA7, 0xE4, 0x0A, 0x96, 0xB3, 0xA0, 0x47, 0x9A, 0xB8, 0x45, 0xC9, 0xE4, 0x67, 0xC5, 0x6B, 0x1F};
const capstr capRambler   = {0x7E, 0x11, 0xB7, 0x78, 0xA3, 0x53, 0x49, 0x26, 0xA8, 0x02, 0x44, 0x73, 0x52, 0x08, 0xC4, 0x2A};
const capstr capAbv       = {0x00, 0xE7, 0xE0, 0xDF, 0xA9, 0xD0, 0x4F, 0xe1, 0x91, 0x62, 0xC8, 0x90, 0x9A, 0x13, 0x2A, 0x1B};
const capstr capNetvigator= {0x4C, 0x6B, 0x90, 0xA3, 0x3D, 0x2D, 0x48, 0x0E, 0x89, 0xD6, 0x2E, 0x4B, 0x2C, 0x10, 0xD9, 0x9F};
const capstr captZers     = {0xb2, 0xec, 0x8f, 0x16, 0x7c, 0x6f, 0x45, 0x1b, 0xbd, 0x79, 0xdc, 0x58, 0x49, 0x78, 0x88, 0xb9}; // CAP_TZERS
const capstr capHtmlMsgs  = {0x01, 0x38, 0xca, 0x7b, 0x76, 0x9a, 0x49, 0x15, 0x88, 0xf2, 0x13, 0xfc, 0x00, 0x97, 0x9e, 0xa8}; // icq6 html msgs
const capstr capSimpLite  = {0x53, 0x49, 0x4D, 0x50, 0x53, 0x49, 0x4D, 0x50, 0x53, 0x49, 0x4D, 0x50, 0x53, 0x49, 0x4D, 0x50};
const capstr capSimpPro   = {0x53, 0x49, 0x4D, 0x50, 0x5F, 0x50, 0x52, 0x4F, 0x53, 0x49, 0x4D, 0x50, 0x5F, 0x50, 0x52, 0x4F};
const capstr capIMsecure  = {'I', 'M', 's', 'e', 'c', 'u', 'r', 'e', 'C', 'p', 'h', 'r', 0x00, 0x00, 0x06, 0x01}; // ZoneLabs
const capstr capMsgType2  =	{0x09, 0x49, 0x13, 0x49, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
const capstr capPalmJicq  = {'J', 'I', 'C', 'Q', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capInluxMsgr = {0xA7, 0xE4, 0x0A, 0x96, 0xB3, 0xA0, 0x47, 0x9A, 0xB8, 0x45, 0xC9, 0xE4, 0x67, 0xC5, 0x6B, 0x1F};
const capstr capMipClient = {0x4D, 0x49, 0x50, 0x20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const capstr capIMadering = {'I', 'M', 'a', 'd', 'e', 'r', 'i', 'n', 'g', ' ', 'C', 'l', 'i', 'e', 'n', 't'};
//
const capstr capIMSecKey1 = {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // ZoneLabs
const capstr capIMSecKey2 = {2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // ZoneLabs
const capstr capUnknown1 = {0x17, 0x8c, 0x2d,0x9b, 0xda, 0xa5, 0x45, 0xbb, 0x8d, 0xdb, 0xf3, 0xbd, 0xbd, 0x53, 0xa1,0x0a};

//aol caps //TODO: rework capabilities system
const capstr capAimSmartCaps = {0x09, 0x46, 0x01, 0xFF, 0x4C, 0x7F, 0x11, 0xD1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
const capstr capAimIcon   = {0x09, 0x46, 0x13, 0x46, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}; // CAP_AIM_BUDDYICON
const capstr capAimDirect = {0x09, 0x46, 0x13, 0x45, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}; // CAP_AIM_DIRECTIM
const capstr capAimFileShare = {0x09, 0x46, 0x13, 0x48, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}; // CAP_AIM_FILE_SHARE
const capstr capAimChat   = {0x74, 0x8F, 0x24, 0x20, 0x62, 0x87, 0x11, 0xD1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
const capstr capAimAddins = {0x09, 0x46, 0x13, 0x47, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
const capstr capAimContactSnd= {0x09, 0x46, 0x13, 0x4b, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
const capstr capAimUnk2   = {0x09, 0x46, 0x01, 0x02, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
const capstr capAimSendbuddylist = {0x09, 0x46, 0x00, 0x00, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x13, 0x4B};
const capstr capAimLiveVideo = {0x09, 0x46, 0x01, 0x01, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}; // icq6 live video
const capstr capAimIcq	  = {0x09, 0x46, 0x13, 0x4D, 0x4C, 0x7F, 0x11, 0xD1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
const capstr capAimVoice  = {0x09, 0x46, 0x13, 0x41, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
const capstr capAimLiveAudio  = {0x09, 0x46, 0x01, 0x04, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}; //added to ICQ 6 & AIM 6
const capstr capStatusTextAware  = {0x09, 0x46, 0x01, 0x0A, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
const capstr capAolUnknown = {0x09, 0x46, 0x00, 0x02, 0x4c, 0x7f, 0x11, 0xd1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};

// -----------------------------------------------------------------------------

typedef struct icq_capability_s
{
    DWORD fdwMirandaID;              // A bitmask, we use it in order to save database space
    const BYTE  *CapCLSID;      // A binary representation of a oscar capability
} icq_capability;

static icq_capability CapabilityRecord[] =
{
    {CAPF_SRV_RELAY,	capSrvRelay},
    {CAPF_UTF,		capUTF},
    {CAPF_TYPING,		capTyping},
    {CAPF_XTRAZ,		capXtraz},
    {CAPF_AIM_FILE,	capAimFile},
    {CAPF_PUSH2TALK,	capPush2Talk},
    {CAPF_RTF,		capRTF},
    {CAPF_XTRAZ_CHAT,	capXtrazChat},
    {CAPF_VOICE_CHAT,	capVoiceChat},
    {CAPF_ICQ_DEVIL,	capIcqDevil},
    {CAPF_DIRECT,		capDirect},
};

// Deletes all oscar capabilities for a given contact
void ClearAllContactCapabilities(HANDLE hContact)
{
    setSettingDword(hContact, DBSETTING_CAPABILITIES, 0);
}



// Deletes one or many oscar capabilities for a given contact
void ClearContactCapabilities(HANDLE hContact, DWORD fdwCapabilities)
{
	// Get current capability flags
    DWORD fdwContactCaps = getSettingDword(hContact, DBSETTING_CAPABILITIES, 0);

    // Clear unwanted capabilities
    fdwContactCaps &= ~fdwCapabilities;

    // And write it back to disk
    setSettingDword(hContact, DBSETTING_CAPABILITIES, fdwContactCaps);
}



// Sets one or many oscar capabilities for a given contact
void SetContactCapabilities(HANDLE hContact, DWORD fdwCapabilities)
{
    DWORD fdwContactCaps;


    // Get current capability flags
    fdwContactCaps = getSettingDword(hContact, DBSETTING_CAPABILITIES, 0);

    // Update them
    fdwContactCaps |= fdwCapabilities;

    // And write it back to disk
    setSettingDword(hContact, DBSETTING_CAPABILITIES, fdwContactCaps);
}



// Returns true if the given contact supports the requested capabilities
BOOL CheckContactCapabilities(HANDLE hContact, DWORD fdwCapabilities)
{
    DWORD fdwContactCaps;


    // Get current capability flags
    fdwContactCaps = getSettingDword(hContact, DBSETTING_CAPABILITIES, 0);

    // Check if all requested capabilities are supported
    if ((fdwContactCaps & fdwCapabilities) == fdwCapabilities)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



// Scans a binary buffer for oscar capabilities and adds them to the contact.
// You probably want to call ClearAllContactCapabilities() first.
void AddCapabilitiesFromBuffer(HANDLE hContact, BYTE* pbyBuffer, int nLength)
{
    DWORD fdwContactCaps;
    int iCapability;
    int nIndex;
    int nRecordSize;


    // Calculate the number of records
    nRecordSize = sizeof(CapabilityRecord)/sizeof(icq_capability);

    // Get current capability flags
    fdwContactCaps = getSettingDword(hContact, DBSETTING_CAPABILITIES, 0);

    // Loop over all capabilities in the buffer and
    // compare them to our own record of capabilities
    for (iCapability = 0; (iCapability + BINARY_CAP_SIZE) <= nLength; iCapability += BINARY_CAP_SIZE)
    {
        BOOL knownCAP = FALSE;
        for (nIndex = 0; nIndex < nRecordSize; nIndex++)
        {
            if (!memcmp(pbyBuffer + iCapability, CapabilityRecord[nIndex].CapCLSID, BINARY_CAP_SIZE))
            {
                // Match
                fdwContactCaps |= CapabilityRecord[nIndex].fdwMirandaID;
                knownCAP = TRUE;
                break;
            }
        }
        if(!knownCAP)
        {
            char szCap[64];
            int i;
            for (i = 0; i < 16; i++)
                wsprintfA(szCap+i*3, " %02X", (BYTE)((BYTE*)(pbyBuffer + iCapability)[i]));
            Netlib_Logf(ghServerNetlibUser, "Found unknown CAP%s", szCap);
        }
    }

    // And write it back to disk
    setSettingDword(hContact, DBSETTING_CAPABILITIES, fdwContactCaps);
}

// Pack binary caps to output buffer
void AddCapabilitiesToBuffer(BYTE* packet, DWORD fdwContactCaps)
{
    int iCapability = 0;
    int nIndex;
    int nRecordSize;

    nRecordSize = sizeof(CapabilityRecord)/sizeof(icq_capability);

    for (nIndex = 0; nIndex < nRecordSize; nIndex++)
    {
        if(CapabilityRecord[nIndex].fdwMirandaID & fdwContactCaps)
        {
            packBuffer(( icq_packet*)packet, (const char*)(CapabilityRecord[nIndex].CapCLSID), BINARY_CAP_SIZE);
        }
    }
}


typedef struct tagSTDCAPINFO
{
    const char *name;
    int type;
    const BYTE *caps;
    int capSize;
} STDCAPINFO;

STDCAPINFO stdCapInfo[] =
{
    {"ServerRelay",				0, capSrvRelay,		16},
    {"NewCaps support",			0, capNewCap,		16},
    {"Aim support",				0, capAimIcq,		16},
    {"UTF8 Messages",			0, capUTF,			16},
    {"Typing Notifications",	0, capTyping,		16},
    {"ICQ Xtraz",				0, capXtraz,		16},
    {"File Transfer (AIMFile)",	0, capAimFile,		16},
    {"Push2Talk",				0, capPush2Talk,	16},
    {"ICQ Lite",				0, capIcqLite,		16},
    {"RTF Messages",			0, capRTF,			16},
    {"Xtraz Chat",				0, capXtrazChat,	16},
    {"Voice Chat",				0, capVoiceChat,	16},
    {"Avatars (ICQDevil)",		0, capIcqDevil,		16},
    {"Direct Connections",		0, capDirect,		16},
    {"Miranda IM",						0, capMirandaIm,	8},
    {"Miranda IM Mobile",				0, capMirandaMobile,	13},
    {"Miranda IM Custom Pack",			0, capMimPack,		4},
    {"Miranda ICQJ eternity/PlusPlus++", 0, capIcqJen, 4},
    {"Miranda ICQJ S7&SSS OLD",			0, capIcqJs7old,	16},
    {"Miranda ICQJ S7&SSS + SecureIM",	0, capIcqJs7s,		16},
    {"Miranda ICQJ S7&SSS",				0, capIcqJs7,		4},
    {"Miranda ICQJ Plus",				0, capIcqJp,		4},
    {"Miranda ICQJ S!N",				0, capIcqJSin,		4},
    {"AIM Oscar",						0, capAimOscar,		8},
    {"IMadering Client",				0, capIMadering,    0x10},
    {"Trillian",						0, capTrillian,		16},
    {"Trillian Crypt",					0, capTrilCrypt,	16},
    {"SIM",								0, capSim,			0xC},
    {"SIM (old)",						0, capSimOld,		0xF},
    {"Licq",							0, capLicq,			0xC},
    {"Kopete",							0, capKopete,		0xC},
    {"mIcq",							0, capmIcq,			0xC},
    {"&RQ",								0, capAndRQ,		9},
    {"R&Q",								0, capRAndQ,		9},
    {"R&Q-ProtectMsg",					0, capRAndQ_1,		9},
    {"mChat",							0, capmChat,		0xA},
    {"Jimm",							0, capJimm,			5},
    {"wJimm",							0, capwJimm,		5},
    {"LocID",							0, capLocID,		0x10},
    {"Yapp",							0, capYapp,			4},
    {"Naim",							0, capNaim,			8},
    {"Anastacia",						0, capAnastasia,	16},
    {"JICQ",							0, capPalmJicq,		16},
    {"Inlux Messenger",					0, capInluxMsgr,	16},
    {"MIP Client",						0, capMipClient,	0xC},
    {"QIP 2005",						0, capQip,			16},
    {"QIP Infium",						0, capQipInfium,	16},
    {"QIP Infium Plugins",				0, capQipPlugins,   16},
    {"QIP Infium Services",				0, capQipServices,   16},
    {"QIP PDA",							0, capQipPDA,		16},
    {"QIP Mobile (Java)",				0, capQipMobile,	16},
    {"QIP Mobile (Symbian)",			0, capQipSymbian,	16},
    {"VmICQ (Symbian)",					0, capVmICQ,		6},
    {"QIP-ProtectMsg",					0, capQip_1,		16},
    {"IM2",								0, capIm2,			16},
    {"ICQ for Mac",						0, capMacIcq,		16},
    {"ICQ 2001",						0, capIs2001,		16},
    {"ICQ 2002",						0, capIs2002,		16},
    {"Comm20012",						0, capComm20012,	16},
    {"StrICQ",							0, capStrIcq,		16},
    {"AIM Buddy Icon",					0, capAimIcon,		16}, // CAP_AIM_BUDDYICON
    {"AIM Direct IM",					0, capAimDirect,	16}, // CAP_AIM_DIRECTIM
    {"AIM Send Buddy List",				0, capAimSendbuddylist, 16},
    {"AIM Send Contact",				0, capAimContactSnd,16},
    {"ICQ Lite",						0, capIcqLite,		16},
    {"AIM Chat",						0, capAimChat,		16},
    {"UIM",								0, capUim,			16},
    {"Rambler ICQ",						0, capRambler,		16},
    {"ABV ICQ",							0, capAbv,			16},
    {"Netvigator ICQ",					0, capNetvigator,	16},
    {"tZers",							0, captZers,		16}, // CAP_TZERS
    {"HTML Messages",					0, capHtmlMsgs,		16},
    {"Live Video",						0, capAimLiveVideo,	16},
    {"Simp Lite",						0, capSimpLite,		16},
    {"Simp Pro",						0, capSimpPro,		16},
    {"IMsecure (ZoneLabs)",				0, capIMsecure,		16},
    {"Messages Type 2 support",			0, capMsgType2,		16},
    {"Aim Voice Chat",					0, capAimVoice,		16},
    {"Live Audio(new VoiceChat)",		0, capAimLiveAudio,		16},
    {"climm client",					0, capClimm,		0xC},
    {"IM+ Client",						0, capIMPlus,		16},
    {"mChct Client",					0, capmChat,		0xA},
    {"SmapeR Client",					0, capSmapeR,		0x07},
    {"Mail.ru Agent (Java)",			0, capMraJava,		12},
    {"CORE Pager Client",				0, capCorePager,	0xA},
    {"Zone Alarm IMsecure key 1",		0, capIMSecKey1,	5},
    {"Zone Alarm IMsecure key 2",		0, capIMSecKey2,	5},
    {"capStatusTextAware ??",			0, capStatusTextAware, 16},
    {"Digsby",							0, capDigsbyBeta, 15},
    {"Digsby",							0, capDigsby, 6}
};

char *GetCapabilityName(BYTE *cap, ICQ_CAPINFO *info)
{
    static char capabilityName[256];
    int i;
    BOOL found;
    char *ptr;

    if (info)
    {
        ptr = info->caps;
        *ptr++ = '{';
        for (i = 0; i < 0x10; ++i)
        {
            wsprintfA(ptr, "0x%.2x%s", cap[i], ((i==0x0F)?"":", "));
            ptr += 4;
            ptr += ((i==0x0F)?0:2);
        }
        *ptr++ = '}';
        *ptr = 0;
    }

    ptr = capabilityName;
    found = FALSE;
    for (i = 0; i < sizeof(stdCapInfo)/(sizeof(*stdCapInfo)); ++i)
        if (!memcmp(stdCapInfo[i].caps, cap, stdCapInfo[i].capSize))
        {
            lstrcpyA(ptr, "01. ");
            ptr += 4;
            lstrcpyA(ptr, stdCapInfo[i].name);
            ptr += lstrlenA(stdCapInfo[i].name);
            if (info)
            {
                info->type = 1;
                info->hIcon = NULL;
                lstrcpyA(info->name, stdCapInfo[i].name);
            }
            found = TRUE;
            break;
        }
    for (i = 0; i < sizeof(capXStatus)/(sizeof(*capXStatus)); ++i)
        if (!memcmp(capXStatus[i], cap, 0x10))
        {
            lstrcpyA(ptr, "01. ");
            ptr += 4;
            lstrcpyA(ptr, "XStatus: ");
            ptr += lstrlenA("XStatus: ");
            lstrcpyA(ptr, nameXStatus[i]);
            ptr += lstrlenA(nameXStatus[i]);
            if (info)
            {
                info->type = 1;
                info->hIcon = NULL;
                lstrcpyA(info->name, capabilityName+4);
            }
            found = TRUE;
            break;
        }
    if (!found)
    {
        for (i = 0; i < lstCustomCaps->realCount; ++i)
            if (!memcmp(((ICQ_CUSTOMCAP *)(lstCustomCaps->items[i]))->caps, cap, BINARY_CAP_SIZE))
            {
                lstrcpyA(ptr, "02. ");
                ptr += 4;
                lstrcpyA(ptr, ((ICQ_CUSTOMCAP *)(lstCustomCaps->items[i]))->name);
                ptr += lstrlenA(((ICQ_CUSTOMCAP *)(lstCustomCaps->items[i]))->name);
                if (info)
                {
                    info->type = 2;
                    info->hIcon = NULL;
                    lstrcpyA(info->name, ((ICQ_CUSTOMCAP *)(lstCustomCaps->items[i]))->name);
                }
                found = TRUE;
                break;
            }
    }
    if (!found)
    {
        int nAlpha = 0;
        for (i = 0; i < 0x10; ++i)
            if ((cap[i] >= 0x20) && (cap[i] <= 0x7e) || (cap[i] == 0x99))
            {
                if ((cap[i] >= '0') && (cap[i] <= '9') || (cap[i] >= 'a') && (cap[i] <= 'z') || (cap[i] >= 'A') && (cap[i] <= 'Z'))
                    ++nAlpha;
                if (!found)
                {
                    lstrcpyA(ptr, "00. ");
                    ptr += 4;
                }
                *ptr++ = cap[i];
                found = TRUE;
            }
        *ptr = 0;
        if (found)
        {
            if (!strncmp(capabilityName+4, "Miranda/", 8))
            {
                capabilityName[1] = '3';
                if (info)
                {
                    info->type = 3;
                    info->hIcon = NULL;
                }
                lstrcpyA(info->name, capabilityName+4);
            }
            else if (nAlpha > 4)
            {
                capabilityName[1] = '4';
                if (info)
                {
                    info->type = 4;
                    info->hIcon = NULL;
                }
                lstrcpyA(info->name, capabilityName+4);
            }
            else
            {
                found = FALSE;
                ptr = capabilityName;
            }
        }
    }
    if (!found)
    {
        lstrcpyA(ptr, "05. ");
        ptr += 4;
        if (info)
        {
            info->type = 5;
            lstrcpyA(ptr, info->caps);
            ptr += lstrlenA(info->caps);
            lstrcpyA(info->name, info->caps);
        }
        else
        {
            info->type = 5;
            *ptr++ = '{';
            for (i = 0; i < 0x10; ++i)
            {
                wsprintfA(ptr, "0x%.2x%s", cap[i], ((i==0x0F)?"":", "));
                ptr += 4;
                ptr += ((i==0x0F)?0:2);
            }
            *ptr++ = '}';
            *ptr = 0;
        }
    }
    *ptr = 0;
    return capabilityName;
}
