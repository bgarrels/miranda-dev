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



static void handleAuthKeyResponse(BYTE *buf, WORD wPacketLen, serverthread_info *info);
static void handleRegImage(BYTE *buf, WORD wPacketLen, serverthread_info *info);
static void handleIcqNewUin(BYTE *buf, WORD wPacketLen, serverthread_info *info);

void handleAuthorizationFam(BYTE *pBuffer, WORD wBufferLength, snac_header* pSnacHeader, serverthread_info *info)
{
    switch (pSnacHeader->wSubtype)
    {

    case ICQ_SIGNON_ERROR:
    {
        WORD wError;

        if (wBufferLength >= 2)
            unpackWord(&pBuffer, &wError);
        else
            wError = 0;

        LogFamilyError(ICQ_AUTHORIZATION_FAMILY, wError);
        break;
    }

    case ICQ_SIGNON_AUTH_KEY:
        handleAuthKeyResponse(pBuffer, wBufferLength, info);
        break;

    case ICQ_SIGNON_LOGIN_REPLY:
        handleLoginReply(pBuffer, wBufferLength, info);
        break;
    case ICQ_SIGNON_REG_AUTH_IMAGE:
        handleRegImage(pBuffer, wBufferLength, info);
        break;
    case ICQ_SIGNON_NEW_UIN:
        handleIcqNewUin(pBuffer, wBufferLength, info);
        break;
    default:
        NetLog_Server("Warning: Ignoring SNAC(x%02x,x%02x) - Unknown SNAC (Flags: %u, Ref: %u)", ICQ_AUTHORIZATION_FAMILY, pSnacHeader->wSubtype, pSnacHeader->wFlags, pSnacHeader->dwRef);
        break;
    }
}



static void icq_encryptPassword(const char* szPassword, unsigned char* encrypted)
{
    unsigned int i;
    unsigned char table[] =
    {
        0xf3, 0x26, 0x81, 0xc4,
        0x39, 0x86, 0xdb, 0x92,
        0x71, 0xa3, 0xb9, 0xe6,
        0x53, 0x7a, 0x95, 0x7c
    };

    for (i = 0; szPassword[i]; i++)
    {
        encrypted[i] = (szPassword[i] ^ table[i % 16]);
    }
}



void sendClientAuth(const char* szKey, WORD wKeyLen, BOOL bSecure)
{
    char szUin[UINMAXLEN];
    WORD wUinLen;
    icq_packet packet;
    BYTE empty = 0;

    wUinLen = strlennull(strUID(dwLocalUIN, szUin));

    packet.wLen = 70 + sizeof(CLIENT_ID_STRING) + wUinLen + wKeyLen + (m_bSecureConnection ? 4 : 0);

    if (bSecure)
    {
        serverPacketInit(&packet, (WORD)(packet.wLen + 10));
        packFNACHeaderFull(&packet, ICQ_AUTHORIZATION_FAMILY, ICQ_SIGNON_LOGIN_REQUEST, 0, 0);
    }
    else
    {
        write_flap(&packet, ICQ_LOGIN_CHAN);
        packDWord(&packet, 0x00000001);
    }
    packTLV(&packet, 0x0001, wUinLen, szUin);

    if (bSecure)
    {
        // Pack MD5 auth digest
        packTLV(&packet, 0x0025, wKeyLen, szKey);
        packDWord(&packet, 0x004C0000); // empty TLV(0x4C): unknown
    }
    else
    {
        // Pack old style password hash
        char hash[20];

        icq_encryptPassword(szKey, (BYTE*)&hash);
        packTLV(&packet, 0x0002, wKeyLen, hash);
    }

    // Pack client identification details.
    packTLV(&packet, 0x0003, (WORD)sizeof(CLIENT_ID_STRING)-1, CLIENT_ID_STRING);
    packTLVWord(&packet, 0x0017, CLIENT_VERSION_MAJOR);
    packTLVWord(&packet, 0x0018, CLIENT_VERSION_MINOR);
    packTLVWord(&packet, 0x0019, CLIENT_VERSION_LESSER);
    packTLVWord(&packet, 0x001a, CLIENT_VERSION_BUILD);
    packTLVWord(&packet, 0x0016, CLIENT_ID_CODE);
    packTLVDWord(&packet, 0x0014, CLIENT_DISTRIBUTION);
    packTLV(&packet, 0x000f, 0x0002, CLIENT_LANGUAGE);
    packTLV(&packet, 0x000e, 0x0002, CLIENT_COUNTRY);
    packTLV(&packet, 0x0094, 0x0001, (char*)&m_bConnectionLost); // CLIENT_RECONNECT flag
    if (m_bSecureConnection)
        packDWord(&packet, 0x008C0000); // empty TLV(0x8C): use SSL
    sendServPacket(&packet);
}

void requestImage()
{
    icq_packet packet;

    SetDlgItemText(hwndRegImageDialog, IDC_NEWUININFO, TranslateT("Requesting image..."));

    packet.wLen = 4;
    write_flap(&packet, ICQ_LOGIN_CHAN);
    packDWord(&packet, 0x00000001);
    sendServPacket(&packet);  // greet login server

    serverPacketInit(&packet, (WORD)(18));
    packFNACHeaderFull(&packet, ICQ_AUTHORIZATION_FAMILY, ICQ_SIGNON_REQUEST_IMAGE, 0, 0);
    packTLV(&packet, 0x0001, 0, 0);
    packDWord(&packet, 0x00000000);
    sendServPacket(&packet);  // request image
}

void registerUin(char* password, char* image)
{
    icq_packet packet;
    DWORD dwCookie = 0;
    WORD wTlvLen = 48 + strlen(password) + 3;

    SetDlgItemText(hwndRegImageDialog, IDC_NEWUININFO, TranslateT("Requesting UIN..."));
    EnableWindow(GetDlgItem(hwndRegImageDialog, IDC_REGISTER), FALSE);

    packet.wLen = (WORD)(wTlvLen + 14 + 4 + strlen(image));

    //Init packet
    serverPacketInit(&packet, (WORD)(wTlvLen + 14 + 4 + strlen(image)));
    packFNACHeaderFull(&packet, ICQ_AUTHORIZATION_FAMILY, ICQ_SIGNON_REGISTRATION_REQ, 0, 0);


    //Init TLV
    packWord(&packet, 0x0001);		//TLV type - 01
    packWord(&packet, wTlvLen);		//TLV length
    //TLV data
    packDWord(&packet, 0);			//Four zeros
    packWord(&packet, 0x2800);		//Subcommand - request new uin
    packWord(&packet, 0x0300);		//Unknown
    packDWord(&packet, 0);			//Four zeros
    packDWord(&packet, 0);			//zeros...
    packDWord(&packet, dwCookie);		//registration cookie
    packDWord(&packet, dwCookie);		//same
    packDWord(&packet, 0);			//zeros
    packDWord(&packet, 0);			//hm... zeros...
    packDWord(&packet, 0);			//it cant be! zeros!
    packDWord(&packet, 0);			//You not beleive, again zeros!:)
    //Password & ImCode
    packLEWord(&packet, (WORD)(strlen(password) + 1));	//Password length + null termination
    packBuffer(&packet, password, (WORD)strlen(password));	//Password
    packBuffer(&packet, "\0", 1);		//Null-terminator
    packDWord(&packet, dwCookie);		//Same cookie again
    packDWord(&packet, 0xE3070000);		//password TLV end, unknown

    //TLV with image code
    packWord(&packet, 0x0009);		//TLV type, 09
    packWord(&packet, (WORD)strlen(image));	//Packet length
    packBuffer(&packet, image, (WORD)strlen(image));	//Image code

    sendServPacket(&packet);  // send packet - request uin
}


static void handleRegImage(BYTE *buf, WORD wPacketLen, serverthread_info *info)
{
    HANDLE hFile;
    DWORD dwBytesWritten;

    char szTempName[MAX_PATH];
    char lpPathBuffer[4096];
    char* szType;
    WORD wLen;

    //Extracting image type (added 16 may 2007 by chaos.persei only for better debuging)
    unpackTypedTLV(buf, wPacketLen, 0x0001, NULL, &wLen, &szType);
    NetLog_Server("Image with protection code recieved, type is %s", szType);

    buf += wLen + 4;
    wPacketLen -= wLen;
    buf += 2;
    wPacketLen -= 4;
    unpackWord(&buf, &wLen);

    NetLog_Server("Image size: %d bytes", wLen);
    NetLog_Server("Packet size: %d bytes", wPacketLen);


    // Get the temp path
    GetTempPathA(4096,   // length of the buffer
                 lpPathBuffer);      // buffer for path

    // Create a temporary file.
    GetTempFileNameA(lpPathBuffer, // directory for temp files
                     "image",                    // temp file name prefix
                     0,                        // create unique name
                     szTempName);              // buffer for name

    hFile = CreateFile((LPTSTR) szTempName,     // file to create
                       GENERIC_WRITE,          // open for writing
                       0,                      // do not share
                       NULL,                   // default security
                       CREATE_ALWAYS,          // overwrite existing
                       FILE_ATTRIBUTE_NORMAL,  // normal file
                       NULL);                  // no attr. template

    WriteFile(hFile, buf, wLen,
              &dwBytesWritten, NULL);

    CloseHandle(hFile);

    SendDlgItemMessage(hwndRegImageDialog, IDC_UIN_IMAGE, STM_SETIMAGE, IMAGE_BITMAP, (WPARAM)(HBITMAP)CallService(MS_UTILS_LOADBITMAP, 0, (WPARAM)szTempName));
    DeleteFileA(szTempName);
    icq_regNewUin = 0;
    SetDlgItemText(hwndRegImageDialog, IDC_NEWUININFO, TranslateT("Image arrived!\nEnter image code and password, then press \"Register\""));
    EnableWindow(GetDlgItem(hwndRegImageDialog, IDC_REGISTER), TRUE);
}

static void handleIcqNewUin(BYTE *buf, WORD wPacketLen, serverthread_info *info)
{
    DWORD dwUIN;
    char szUin[UINMAXLEN], szInfo[1024];
    HGLOBAL hglbUIN;
    LPTSTR lptstrUIN;

    buf += 46;
    unpackLEDWord(&buf, &dwUIN);
    ltoa(dwUIN, szUin, 10);

    NetLog_Server("New ICQ uin registered and received: %s", szUin);

    if (OpenClipboard(NULL))
    {
        EmptyClipboard();
        hglbUIN = GlobalAlloc(GMEM_MOVEABLE, (strlen(szUin) + 1) * sizeof(TCHAR)) ;
        lptstrUIN = (LPTSTR)GlobalLock(hglbUIN);	// FIXME
        memcpy(lptstrUIN, szUin, strlen(szUin) * sizeof(TCHAR)) ;
        GlobalUnlock(hglbUIN);
        SetClipboardData(CF_TEXT, hglbUIN);
        CloseClipboard();
    }

    sprintf(szInfo, "%s\n%s: %s\n%s: %s\n%s", Translate("New ICQ UIN registered!"), Translate("UIN"), szUin, Translate("Password"), icq_regpassword, Translate("Look for new UIN in clipboard"));

    SetDlgItemTextA(hwndRegImageDialog, IDC_NEWUININFO, szInfo);

    icq_regedUin = dwUIN;

    icq_regNewUin = 0;
    bImageRequested = 0;
    icq_serverDisconnect(0);
}


static void handleAuthKeyResponse(BYTE *buf, WORD wPacketLen, serverthread_info *info)
{
    WORD wKeyLen;
    char szKey[64] = {0};
    mir_md5_state_t state;
    mir_md5_byte_t digest[16];

#ifdef _DEBUG
    NetLog_Server("Received %s", "ICQ_SIGNON_AUTH_KEY");
#endif

    if (wPacketLen < 2)
    {
        NetLog_Server("Malformed %s", "ICQ_SIGNON_AUTH_KEY");
        icq_LogMessage(LOG_FATAL, "Secure login failed.\nInvalid server response.");
        SetCurrentStatus(ID_STATUS_OFFLINE);
        return;
    }

    unpackWord(&buf, &wKeyLen);
    wPacketLen -= 2;

    if (!wKeyLen || wKeyLen > wPacketLen || wKeyLen > sizeof(szKey))
    {
        NetLog_Server("Invalid length in %s: %u", "ICQ_SIGNON_AUTH_KEY", wKeyLen);
        icq_LogMessage(LOG_FATAL, "Secure login failed.\nInvalid key length.");
        SetCurrentStatus(ID_STATUS_OFFLINE);
        return;
    }

    unpackString(&buf, szKey, wKeyLen);

    {
        char *pwd = (char*)info->szAuthKey;

        mir_md5_init(&state);
        mir_md5_append(&state, (const mir_md5_byte_t*)pwd, info->wAuthKeyLen);
        mir_md5_finish(&state, digest);
    }

    mir_md5_init(&state);
    mir_md5_append(&state, (const mir_md5_byte_t*)&szKey, wKeyLen);
    mir_md5_append(&state, digest, 16);
    mir_md5_append(&state, (const mir_md5_byte_t*)&CLIENT_MD5_STRING, sizeof(CLIENT_MD5_STRING)-1);
    mir_md5_finish(&state, digest);

#ifdef _DEBUG
    NetLog_Server("Sending ICQ_SIGNON_LOGIN_REQUEST to login server");
#endif
    sendClientAuth((char*)&digest, 0x10, TRUE);
}

DWORD icq_regedUin;
