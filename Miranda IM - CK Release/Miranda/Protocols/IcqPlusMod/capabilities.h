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
//  Contains helper functions to handle oscar user capabilities.
//
// -----------------------------------------------------------------------------



// Deletes all oscar capabilities for a given contact.
void ClearAllContactCapabilities(HANDLE hContact);

// Deletes one or many oscar capabilities for a given contact.
void ClearContactCapabilities(HANDLE hContact, DWORD fdwCapabilities);

// Sets one or many oscar capabilities for a given contact.
void SetContactCapabilities(HANDLE hContact, DWORD fdwCapabilities);

// Returns true if the given contact supports the requested capabilites.
BOOL CheckContactCapabilities(HANDLE hContact, DWORD fdwCapabilities);

// Scans a binary buffer for oscar capabilities and adds them to the db.
void AddCapabilitiesFromBuffer(HANDLE hContact, BYTE* pbyBuffer, int nLength);

// Pack binary caps to output buffer
void AddCapabilitiesToBuffer(BYTE* packet, DWORD fdwContactCaps);

// Get capability name
typedef struct
{
    HANDLE hIcon;
    int type;
    char name[128];
    char caps[128];
} ICQ_CAPINFO;
char *GetCapabilityName(BYTE *cap, ICQ_CAPINFO *info);

// -----------------------------------------------------------------------------
extern const capstr capSrvRelay;
extern const capstr capUTF;
extern const capstr capTyping;
extern const capstr capXtraz;
extern const capstr capAimFile;
extern const capstr capPush2Talk;
extern const capstr capIcqLite;
extern const capstr capRTF;
extern const capstr capXtrazChat;
extern const capstr capVoiceChat;
extern const capstr capIcqDevil;
extern const capstr capDirect;
extern const capstr capNewCap;
extern const capstr capAimIcq;
// -----------------------------------------------------------------------------
extern const capstr capMirandaIm;
extern const capstr capMirandaMobile;
extern const capstr capMimPack;
extern const capstr capIcqJs7;
extern const capstr capIcqJp;
extern const capstr capIcqJs7s;
extern const capstr capIcqJs7old;
extern const capstr capIcqJSin;
extern const capstr capIcqJen;
extern const capstr capAimOscar;
extern const capstr capTrillian;
extern const capstr capTrilCrypt;
extern const capstr capSim;
extern const capstr capSimOld;
extern const capstr capLicq;
extern const capstr capKopete;
extern const capstr capmIcq;
extern const capstr capAndRQ;
extern const capstr capRAndQ;
extern const capstr capRAndQ_1;
extern const capstr capmChat;
extern const capstr capJimm;
extern const capstr capwJimm;
extern const capstr capLocID;//added
extern const capstr capBayan;
extern const capstr capCorePager;
extern const capstr capDiChat;
extern const capstr capNaim;
extern const capstr capAnastasia;
extern const capstr capQip;
extern const capstr capQip_1;
extern const capstr capQipSymbian;
extern const capstr capQipPDA;
extern const capstr capQipMobile;
extern const capstr capVmICQ;
extern const capstr capSmapeR;
extern const capstr capPigeon;
extern const capstr capDigsbyBeta;
extern const capstr capDigsby;
extern const capstr capJapp;
extern const capstr capIMPlus;
extern const capstr capYapp;
extern const capstr capMraJava;
extern const capstr capQutIm;
extern const capstr capJabberJIT;
extern const capstr capIcqKid2;
extern const capstr capWebIcqPro;
extern const capstr capQipInfium;
extern const capstr capQipPlugins;
extern const capstr capQipServices;
extern const capstr capIm2;
extern const capstr capMacIcq;
extern const capstr capIs2001;
extern const capstr capIs2002;
extern const capstr capComm20012;
extern const capstr capStrIcq;
extern const capstr capAimIcon; // CAP_AIM_BUDDYICON
extern const capstr capAimDirect; // CAP_AIM_DIRECTIM
extern const capstr capAimFileShare;
extern const capstr capIcqLite;
extern const capstr capIcqLiteNew;
extern const capstr capAimChat;
extern const capstr capUim;
extern const capstr capRambler;
extern const capstr capAbv;
extern const capstr capNetvigator;
extern const capstr captZers; // CAP_TZERS
extern const capstr capHtmlMsgs; // icq6
extern const capstr capAimLiveVideo;// icq6
extern const capstr capSimpLite;
extern const capstr capSimpPro;
extern const capstr capIMsecure; // ZoneLabs
extern const capstr capAimLiveAudio;
extern const capstr capAimAddins;
extern const capstr capAimContactSnd;
extern const capstr capAimUnk2;
extern const capstr capPalmJicq;
extern const capstr capInluxMsgr;
extern const capstr capMipClient;
extern const capstr capIMadering;
extern const capstr capIMSecKey1;
extern const capstr capIMSecKey2;
extern const capstr capUnknown1;
extern const capstr capAimSendbuddylist;
extern const capstr capClimm;
extern const capstr capStatusTextAware;
// -----------------------------------------------------------------------------
extern const capstr capXStatus[37];
extern const char* nameXStatus[37];
extern const int moodXStatus[37];

extern const char* nameQIPStatus[5];
