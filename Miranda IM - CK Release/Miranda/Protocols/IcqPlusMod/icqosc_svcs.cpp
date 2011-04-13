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
// Revision       : $Revision: 51 $
// Last change on : $Date: 2007-08-30 23:46:51 +0300 (Ð§Ñ‚, 30 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  High-level code for exported API services
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"



int gbIdleAllow;
int icqGoingOnlineStatus;
int iIcqNewStatus = ID_STATUS_OFFLINE;

extern HANDLE hIconProtocol;

extern WORD wListenPort;

extern char* calcMD5Hash(char* szFile);
extern filetransfer *CreateFileTransfer(HANDLE hContact, DWORD dwUin, int nVersion);
extern DWORD addServContact(HANDLE hContact, const char *pszGroup);
extern DWORD _mirandaVersion;

INT_PTR IcqGetName(WPARAM wParam, LPARAM lParam)
{
    if (lParam)
    {
        strncpy((char *)lParam, ICQTranslate(ICQ_PROTOCOL_NAME), wParam);
        return 0; // Success
    }

    return 1; // Failure
}

INT_PTR IcqLoadIcon(WPARAM wParam, LPARAM lParam)
{
    char *id;

	switch (wParam & 0xFFFF)
    {
    case PLI_PROTOCOL:
        id = "proto";
        break;

    default:
        return 0; // Failure
    }

    if (wParam&PLIF_ICOLIBHANDLE)
        return (int)hIconProtocol;
    else
    {
        HICON icon = IconLibGetIcon(id);

        if (wParam&PLIF_ICOLIB)
            return (int)icon;
        else
            return (int)CopyIcon(icon);
    }
}

INT_PTR IcqSetPassword(WPARAM wParam, LPARAM lParam)
{
    char *pwd = (char*)lParam;
    int len = strlennull(pwd);

    if (len && len <= 8)
    {
        strcpy(gpszPassword, pwd);
        gbRememberPwd = 1;
    }
    return 0;
}

int IcqIdleChanged(WPARAM wParam, LPARAM lParam)
{
    int bIdle = (lParam&IDF_ISIDLE);
    int bPrivacy = (lParam&IDF_PRIVACY);

    if (bPrivacy) return 0;

    setSettingDword(NULL, "IdleTS", bIdle ? time(0) : 0);

    if (m_bTempVisListEnabled) // remove temporary visible users
        clearTemporaryVisibleList();

    icq_setidle(bIdle ? 1 : 0);

    return 0;
}



INT_PTR IcqGetAvatarInfo(WPARAM wParam, LPARAM lParam)
{
    PROTO_AVATAR_INFORMATION* pai = (PROTO_AVATAR_INFORMATION*)lParam;
    DWORD dwUIN;
    uid_str szUID;
    DBVARIANT dbv;
    int dwPaFormat;

    if (!m_bAvatarsEnabled) return GAIR_NOAVATAR;

    if (getSetting(pai->hContact, "AvatarHash", &dbv) || dbv.type != DBVT_BLOB || (dbv.cpbVal != 0x14 && dbv.cpbVal != 0x09))
        return GAIR_NOAVATAR; // we did not found avatar hash or hash invalid - no avatar available

    if (getContactUid(pai->hContact, &dwUIN, &szUID))
    {
        ICQFreeVariant(&dbv);

        return GAIR_NOAVATAR; // we do not support avatars for invalid contacts
    }

    dwPaFormat = getSettingByte(pai->hContact, "AvatarType", PA_FORMAT_UNKNOWN);
    if (dwPaFormat != PA_FORMAT_UNKNOWN)
    {
        // we know the format, test file
        GetFullAvatarFileName(dwUIN, szUID, dwPaFormat, pai->filename, MAX_PATH);

        pai->format = dwPaFormat;

        if (!IsAvatarSaved(pai->hContact, (char*)dbv.pbVal, dbv.cpbVal))
        {
            // hashes are the same
            if (access(pai->filename, 0) == 0)
            {
                ICQFreeVariant(&dbv);

                return GAIR_SUCCESS; // we have found the avatar file, whoala
            }
        }
    }

    if (IsAvatarSaved(pai->hContact, (char*)dbv.pbVal, dbv.cpbVal))
    {
        // we didn't received the avatar before - this ensures we will not request avatar again and again
        if ((wParam & GAIF_FORCE) != 0 && pai->hContact != 0)
        {
            // request avatar data
            GetAvatarFileName(dwUIN, szUID, pai->filename, MAX_PATH);
            GetAvatarData(pai->hContact, dwUIN, szUID, (char*)dbv.pbVal, dbv.cpbVal, pai->filename);
            ICQFreeVariant(&dbv);

            return GAIR_WAITFOR;
        }
    }
    ICQFreeVariant(&dbv);

    return GAIR_NOAVATAR;
}



INT_PTR IcqGetAvatarCaps(WPARAM wParam, LPARAM lParam)
{
    if (wParam == AF_MAXSIZE)
    {
        POINT *size = (POINT*)lParam;

        if (size)
        {
            if (getSettingByte(NULL, "AvatarsAllowBigger", DEFAULT_BIGGER_AVATARS))
            {
                // experimental server limits
                size->x = 128;
                size->y = 128;
            }
            else
            {
                // default limits (older)
                size->x = 64;
                size->y = 64;
            }

            return 0;
        }
    }
    else if (wParam == AF_PROPORTION)
    {
        return PIP_NONE;
    }
    else if (wParam == AF_FORMATSUPPORTED)
    {
        if (lParam == PA_FORMAT_JPEG || lParam == PA_FORMAT_GIF || lParam == PA_FORMAT_XML || lParam == PA_FORMAT_BMP)
            return 1;
        else
            return 0;
    }
    else if (wParam == AF_ENABLED)
    {
        if (m_bSsiEnabled && m_bAvatarsEnabled)
            return 1;
        else
            return 0;
    }
    else if (wParam == AF_DONTNEEDDELAYS)
    {
        return 0;
    }
    else if (wParam == AF_MAXFILESIZE)
    {
        // server accepts images of 7168 bytees, not bigger
        return 7168;
    }
    else if (wParam == AF_DELAYAFTERFAIL)
    {
        // do not request avatar again if server gave an error
        return 1 * 60 * 60 * 1000; // one hour
    }
    return 0;
}



INT_PTR IcqGetMyAvatar(WPARAM wParam, LPARAM lParam)
{
    char* file;

    if (!m_bAvatarsEnabled) return -2;

    if (!wParam) return -3;

    file = loadMyAvatarFileName();
    if (file) strncpy((char*)wParam, file, (int)lParam);
    mir_free(file);
    if (!access((char*)wParam, 0)) return 0;
    return -1;
}



INT_PTR IcqSetMyAvatar(WPARAM wParam, LPARAM lParam)
{
    char* szFile = (char*)lParam;
    int iRet = -1;

    if (!m_bAvatarsEnabled || !m_bSsiEnabled) return -2;

    if (szFile)
    {
        // set file for avatar
        char szMyFile[MAX_PATH+1];
        int dwPaFormat = DetectAvatarFormat(szFile);
        char* hash;
        HBITMAP avt;

        if (dwPaFormat != PA_FORMAT_XML)
        {
            // if it should be image, check if it is valid
            avt = (HBITMAP)CallService(MS_UTILS_LOADBITMAP, 0, (WPARAM)szFile);
            if (!avt) return iRet;
            DeleteObject(avt);
        }
        GetFullAvatarFileName(0, NULL, dwPaFormat, szMyFile, MAX_PATH);
        // if not in our storage, copy
        if (strcmpnull(szFile, szMyFile) && !CopyFileA(szFile, szMyFile, FALSE))
        {
            NetLog_Server("Failed to copy our avatar to local storage.");
            return iRet;
        }

        hash = calcMD5Hash(szMyFile);
        if (hash)
        {
            char* ihash = (char*)icq_alloc_zero(0x14);
            // upload hash to server
            ihash[0] = 0;    //unknown
            ihash[1] = dwPaFormat == PA_FORMAT_XML ? AVATAR_HASH_FLASH : AVATAR_HASH_STATIC; //hash type
            ihash[2] = 1;    //hash status
            ihash[3] = 0x10; //hash len
            memcpy(ihash+4, hash, 0x10);
            updateServAvatarHash(ihash, 0x14);

            if (setSettingBlob(NULL, "AvatarHash", (BYTE*)ihash, 0x14))
            {
                NetLog_Server("Failed to save avatar hash.");
            }

            storeMyAvatarFileName(szMyFile);
            iRet = 0;

            mir_free(hash);
        }
    }
    else
    {
        // delete user avatar
        deleteSetting(NULL, "AvatarFile");
        setSettingBlob(NULL, "AvatarHash", hashEmptyAvatar, 9);
        updateServAvatarHash((char*)&hashEmptyAvatar, 9); // set blank avatar
        iRet = 0;
    }

    return iRet;
}



void updateAimAwayMsg()
{
    char** szMsg = MirandaStatusToAwayMsg(gnCurrentStatus);

    EnterCriticalSection(&m_modeMsgsMutex);
    if (szMsg && !bNoStatusReply)
        icq_sendSetAimAwayMsgServ(*szMsg);
    LeaveCriticalSection(&m_modeMsgsMutex);
}



INT_PTR CIcqProto::IcqSetStatus(WPARAM wParam, LPARAM lParam)
{
    int nNewStatus = MirandaStatusToSupported(wParam);

    iIcqNewStatus = nNewStatus;
    // check if netlib handles are ready
    if (!ghServerNetlibUser)
        return 0;

    if (m_bTempVisListEnabled) // remove temporary visible users
        clearTemporaryVisibleList();

    //if (nNewStatus != gnCurrentStatus)
    {
        if (getSettingByte(NULL, "XStatusReset", DEFAULT_XSTATUS_RESET))
        {
            // clear custom status on status change
            IcqSetXStatus(0, 0);
        }

        // New status is OFFLINE
        if (nNewStatus == ID_STATUS_OFFLINE)
        {
            // for quick logoff
            icqGoingOnlineStatus = nNewStatus;

            // Send disconnect packet
            icq_sendCloseConnection();

            icq_serverDisconnect(FALSE);

            SetCurrentStatus(ID_STATUS_OFFLINE);

            NetLog_Server("Logged off.");
        }
        else
        {
            switch (gnCurrentStatus)
            {

                // We are offline and need to connect
            case ID_STATUS_OFFLINE:
            {
                char *pszPwd;

                // Update user connection settings
                UpdateGlobalSettings();

                // Read UIN from database
                dwLocalUIN = getContactUin(NULL);
                if (dwLocalUIN == 0)
                {
                    SetCurrentStatus(ID_STATUS_OFFLINE);

                    icq_LogMessage(LOG_FATAL, "You have not entered a ICQ number.\nConfigure this in Options->Network->ICQ and try again.");
                    return 0;
                }

                // Set status to 'Connecting'
                icqGoingOnlineStatus = nNewStatus;
                SetCurrentStatus(ID_STATUS_CONNECTING);

                // Read password from database
                pszPwd = GetUserPassword(FALSE);

                if (pszPwd)
                    icq_login(pszPwd);
                else
                    RequestPassword();

                break;
            }

            // We are connecting... We only need to change the going online status
            case ID_STATUS_CONNECTING:
            {
                icqGoingOnlineStatus = nNewStatus;
                break;
            }

            // We are already connected so we should just change status
            default:
            {
                if( nNewStatus!=ID_STATUS_ONLINE )   // ñáðîñèì QIPStatus
                {
                    deleteSetting(NULL, "QIPStatus");
                }
                else
                {
                    setSettingWord(NULL, "ICQStatus", getSettingWord(NULL, "QIPStatus", 0));
                }
                SetCurrentStatus(nNewStatus);

                if (gnCurrentStatus == ID_STATUS_INVISIBLE)
                {
                    if (m_bSsiEnabled)
                        updateServVisibilityCode(3);
                    icq_setstatus(MirandaStatusToIcq(gnCurrentStatus), FALSE);
                    // Tell whos on our visible list
                    icq_sendEntireVisInvisList(0);
                    if (m_bAimEnabled)
                        updateAimAwayMsg();
                }
                else
                {
                    icq_setstatus(MirandaStatusToIcq(gnCurrentStatus), FALSE);
                    if (m_bSsiEnabled)
                        updateServVisibilityCode(4);
                    // Tell whos on our invisible list
                    icq_sendEntireVisInvisList(1);
                    if (m_bAimEnabled)
                        updateAimAwayMsg();
                }
            }
            }
        }
    }

    if(gbQipStatusEnabled)
    {
        CLISTMENUITEM mi = {0};
        int nQIPStatus = getSettingWord(NULL, "QIPStatus", ICQ_STATUS_ONLINE);
        int i;
        mi.cbSize = sizeof(mi);
        if( nQIPStatus==ICQ_STATUS_ONLINE )
        {
            // reset main item
            mi.flags = CMIM_FLAGS | CMIM_ICON | CMIM_NAME ;
            mi.pszName = Translate("QIP Status");
            CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hQIPStatusRoot, (LPARAM)&mi);
        }
        for(i=0; i<SIZEOF(wQIPStatusCap); ++i)
        {
            if( wQIPStatusCap[i]==nQIPStatus )
            {
                mi.flags = CMIM_FLAGS | CMIF_CHECKED;
                CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hQIPStatusItems[i], (LPARAM)&mi);
                if( i )
                {
                    // set main item
                    mi.flags = CMIM_FLAGS | CMIM_ICON | CMIM_NAME | (i?CMIF_ICONFROMICOLIB:0);
                    mi.icolibItem = hQIPStatusHandle[i-1];
                    mi.pszName = (char*)nameQIPStatus[i-1];
                    CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hQIPStatusRoot, (LPARAM)&mi);
                }
            }
            else
            {
                mi.flags = CMIM_FLAGS;
                CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hQIPStatusItems[i], (LPARAM)&mi);
            }
        }
    }

    return 0;
}



INT_PTR IcqGetStatus(WPARAM wParam, LPARAM lParam)
{
    return gnCurrentStatus;
}



INT_PTR CIcqProto::IcqSetAwayMsg(WPARAM wParam, LPARAM lParam)
{
    char** ppszMsg = NULL;
    char* szNewUtf = NULL;

    EnterCriticalSection(&m_modeMsgsMutex);

    ppszMsg = MirandaStatusToAwayMsg(wParam);
    if (!ppszMsg)
    {
        LeaveCriticalSection(&m_modeMsgsMutex);
        return 1; // Failure
    }

    // Prepare UTF-8 status message
    szNewUtf = mir_utf8encode((char*)lParam);

    if (strcmpnull(szNewUtf, *ppszMsg))
    {
        // Free old message
        mir_free(ppszMsg);

        // Set new message
        *ppszMsg = szNewUtf;
        szNewUtf = NULL;

        if (m_bAimEnabled && (gnCurrentStatus == (int)wParam) && !bNoStatusReply)
            icq_sendSetAimAwayMsgServ(*ppszMsg);
    }
    mir_free(szNewUtf);

    LeaveCriticalSection(&m_modeMsgsMutex);

    return 0; // Success
}



HANDLE HContactFromAuthEvent(HANDLE hEvent)
{
    DBEVENTINFO dbei;
    DWORD body[2];

    ZeroMemory(&dbei, sizeof(dbei));
    dbei.cbSize = sizeof(dbei);
    dbei.cbBlob = sizeof(DWORD)*2;
    dbei.pBlob = (PBYTE)&body;

    if (CallService(MS_DB_EVENT_GET, (WPARAM)hEvent, (LPARAM)&dbei))
        return INVALID_HANDLE_VALUE;

    if (dbei.eventType != EVENTTYPE_AUTHREQUEST)
        return INVALID_HANDLE_VALUE;

    if (strcmpnull(dbei.szModule, ICQ_PROTOCOL_NAME))
        return INVALID_HANDLE_VALUE;

    return (HANDLE)body[1]; // this is bad - needs new auth system
}


// TODO: Adding needs some more work in general
HANDLE AddToListByUIN(DWORD dwUin, DWORD dwFlags)
{
    HANDLE hContact;
    int bAdded;

    hContact = HContactFromUIN(dwUin, &bAdded);

    if (hContact)
    {
        if ((!dwFlags & PALF_TEMPORARY) && DBGetContactSettingByte(hContact, "CList", "NotOnList", 1))
        {
            DBDeleteContactSetting(hContact, "CList", "NotOnList");
            setContactHidden(hContact, 0);
        }

        return hContact; // Success
    }

    return NULL; // Failure
}


HANDLE AddToListByUID(char *szUID, DWORD dwFlags)
{
    HANDLE hContact;
    int bAdded;

    hContact = HContactFromUID(0, szUID, &bAdded);

    if (hContact)
    {
        if ((!dwFlags & PALF_TEMPORARY) && DBGetContactSettingByte(hContact, "CList", "NotOnList", 1))
        {
            DBDeleteContactSetting(hContact, "CList", "NotOnList");
            setContactHidden(hContact, 0);
        }

        return hContact; // Success
    }

    return NULL; // Failure
}


INT_PTR IcqSetNickName(WPARAM wParam, LPARAM lParam)
{
    if (icqOnline)
    {
        setSettingString(NULL, "Nick", (char*)lParam);

        return IcqChangeInfoEx(CIXT_BASIC, 0);
    }

    return 0; // Failure
}



INT_PTR IcqChangeInfoEx(WPARAM wParam, LPARAM lParam)
{
    if (icqOnline && wParam)
    {
        PBYTE buf = NULL;
        int buflen = 0;
        BYTE b;

        // userinfo
//    ppackTLVWord(&buf, &buflen, (WORD)GetACP(), TLV_CODEPAGE, 0);
        ppackTLVWord(&buf, &buflen, (WORD)GetACP(), 0x1C2, 1);

        if (wParam & CIXT_CONTACT)
        {
            // contact information
            BYTE *pBlock = NULL;
            int cbBlock = 0;
            int nItems = 0;

            nItems += ppackTLVWordStringItemFromDB(&pBlock, &cbBlock, "e-mail0", 0x78, 0x64, 0x00);
            nItems += ppackTLVWordStringItemFromDB(&pBlock, &cbBlock, "e-mail1", 0x78, 0x64, 0x00);
            nItems += ppackTLVWordStringItemFromDB(&pBlock, &cbBlock, "e-mail2", 0x78, 0x64, 0x00);
            ppackTLVBlockItems(&buf, &buflen, 0x8C, &nItems, &pBlock, (WORD*)&cbBlock, FALSE);


            nItems += ppackTLVWordStringItemFromDB(&pBlock, &cbBlock, "Phone", 0x6E, 0x64, 0x01);
            nItems += ppackTLVWordStringItemFromDB(&pBlock, &cbBlock, "CompanyPhone", 0x6E, 0x64, 0x02);
            nItems += ppackTLVWordStringItemFromDB(&pBlock, &cbBlock, "Cellular", 0x6E, 0x64, 0x03);
            nItems += ppackTLVWordStringItemFromDB(&pBlock, &cbBlock, "Fax", 0x6E, 0x64, 0x04);
            nItems += ppackTLVWordStringItemFromDB(&pBlock, &cbBlock, "CompanyFax", 0x6E, 0x64, 0x05);
            ppackTLVBlockItems(&buf, &buflen, 0xC8, &nItems, &pBlock, (WORD*)&cbBlock, FALSE);

            ppackTLVByte(&buf, &buflen, getSettingByte(NULL, "AllowSpam", 0),0x1EA ,1);

        }

        if (wParam & CIXT_BASIC)
        {
            // upload basic user info
            ppackTLVStringUtfFromDB(&buf, &buflen, "Nick", 0x78);
            ppackTLVStringUtfFromDB(&buf, &buflen, "FirstName", 0x64);
            ppackTLVStringUtfFromDB(&buf, &buflen, "LastName", 0x6E);
            ppackTLVStringUtfFromDB(&buf, &buflen, "About", 0x186);
        }

        if (wParam & CIXT_MORE)
        {
            b = getSettingByte(NULL, "Gender", 0);
            ppackTLVByte(&buf, &buflen, (BYTE)(b ? (b == 'M' ? 2 : 1) : 0), 0x82, 1);

            ppackTLVDateFromDB(&buf, &buflen, "BirthYear", "BirthMonth", "BirthDay", 0x1A4);

            ppackTLVWord(&buf, &buflen, getSettingByte(NULL, "Language1", 0), 0xAA, 1);
            ppackTLVWord(&buf, &buflen, getSettingByte(NULL, "Language2", 0), 0xB4, 1);
            ppackTLVWord(&buf, &buflen, getSettingByte(NULL, "Language3", 0), 0xBE, 1);

            ppackTLVWord(&buf, &buflen, getSettingByte(NULL, "MaritalStatus", 0), 0x12C, 1);

        }

        if (wParam & CIXT_WORK)
        {
            BYTE *pBlock = NULL;
            int cbBlock = 0;
            int nItems = 1;

            // Jobs
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "CompanyPosition", 0x64);
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "Company", 0x6E);
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "CompanyDepartment", 0x7D);
            ppackTLVStringFromDB(&pBlock, &cbBlock, "CompanyHomepage", 0x78);
            ppackTLVWord(&pBlock, &cbBlock, getSettingWord(NULL, "CompanyIndustry", 0), 0x82, 1);
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "CompanyStreet", 0xAA);
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "CompanyCity", 0xB4);
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "CompanyState", 0xBE);
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "CompanyZIP", 0xC8);
            ppackTLVDWord(&pBlock, &cbBlock, getSettingWord(NULL, "CompanyCountry", 0), 0xD2, 1);
            /// TODO: pack unknown data (need to preserve them in Block Items)
            ppackTLVBlockItems(&buf, &buflen, 0x118, &nItems, &pBlock, (WORD*)&cbBlock, TRUE);

        }

        if (wParam & CIXT_EDUCATION)
        {
            BYTE *pBlock = NULL;
            int cbBlock = 0;
            int nItems = 1;

            // Studies
            ppackTLVWord(&pBlock, &cbBlock, getSettingWord(NULL, "StudyLevel", 0), 0x64, 1);
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "StudyInstitute", 0x6E);
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "StudyDegree", 0x78);
            ppackTLVWord(&pBlock, &cbBlock, getSettingWord(NULL, "StudyYear", 0), 0x8C, 1);
            ppackTLVBlockItems(&buf, &buflen, 0x10E, &nItems, &pBlock, (WORD*)&cbBlock, TRUE);
        }


        if (wParam & CIXT_LOCATION)
        {
            BYTE *pBlock = NULL;
            int cbBlock = 0;
            int nItems = 1;
            WORD wTimezone = getSettingByte(NULL, "Timezone", 0);

            // Home Address
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "Street", 0x64);
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "City", 0x6E);
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "State", 0x78);
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "ZIP", 0x82);
            ppackTLVDWord(&pBlock, &cbBlock, getSettingWord(NULL, "Country", 0), 0x8C, 1);
            ppackTLVBlockItems(&buf, &buflen, 0x96, &nItems, &pBlock, (WORD*)&cbBlock, TRUE);

            nItems = 1;
            // Origin Address
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "OriginStreet", 0x64);
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "OriginCity", 0x6E);
            ppackTLVStringUtfFromDB(&pBlock, &cbBlock, "OriginState", 0x78);
            ppackTLVDWord(&pBlock, &cbBlock, getSettingWord(NULL, "OriginCountry", 0), 0x8C, 1);
            ppackTLVBlockItems(&buf, &buflen, 0xA0, &nItems, &pBlock, (WORD*)&cbBlock, TRUE);

            ppackTLVStringFromDB(&buf, &buflen, "Homepage", 0xFA);

            // Timezone

            if ((wTimezone & 0x0080) == 0x80) wTimezone |= 0xFF00; // extend signed number
            ppackTLVWord(&buf, &buflen, wTimezone, 0x17C, 1);
        }

        if (wParam & CIXT_BACKGROUND)
        {

            BYTE *pBlock = NULL;
            int cbBlock = 0;
            int nItems = 0;

            // Interests
            nItems += ppackTLVWordStringUtfItemFromDB(&pBlock, &cbBlock, "Interest0Text", 0x6E, 0x64, getSettingWord(NULL, "Interest0Cat", 0));
            nItems += ppackTLVWordStringUtfItemFromDB(&pBlock, &cbBlock, "Interest1Text", 0x6E, 0x64, getSettingWord(NULL, "Interest1Cat", 0));
            nItems += ppackTLVWordStringUtfItemFromDB(&pBlock, &cbBlock, "Interest2Text", 0x6E, 0x64, getSettingWord(NULL, "Interest2Cat", 0));
            nItems += ppackTLVWordStringUtfItemFromDB(&pBlock, &cbBlock, "Interest3Text", 0x6E, 0x64, getSettingWord(NULL, "Interest3Cat", 0));
            ppackTLVBlockItems(&buf, &buflen, 0x122, &nItems, &pBlock, (WORD*)&cbBlock, FALSE);

            /*		WORD w;

                  w = StringToListItemId("Interest0Cat", 0);
                  ppackTLVWordLNTSfromDB(&buf, &buflen, w, "Interest0Text", TLV_INTERESTS);
                  w = StringToListItemId("Interest1Cat", 0);
                  ppackTLVWordLNTSfromDB(&buf, &buflen, w, "Interest1Text", TLV_INTERESTS);
                  w = StringToListItemId("Interest2Cat", 0);
                  ppackTLVWordLNTSfromDB(&buf, &buflen, w, "Interest2Text", TLV_INTERESTS);
                  w = StringToListItemId("Interest3Cat", 0);
                  ppackTLVWordLNTSfromDB(&buf, &buflen, w, "Interest3Text", TLV_INTERESTS); */

            /*  w = StringToListItemId("Past0", 0);
              ppackTLVWordLNTSfromDB(&buf, &buflen, w, "Past0Text", TLV_PASTINFO);
              w = StringToListItemId("Past1", 0);
              ppackTLVWordLNTSfromDB(&buf, &buflen, w, "Past1Text", TLV_PASTINFO);
              w = StringToListItemId("Past2", 0);
              ppackTLVWordLNTSfromDB(&buf, &buflen, w, "Past2Text", TLV_PASTINFO);

              w = StringToListItemId("Affiliation0", 0);
              ppackTLVWordLNTSfromDB(&buf, &buflen, w, "Affiliation0Text", TLV_AFFILATIONS);
              w = StringToListItemId("Affiliation1", 0);
              ppackTLVWordLNTSfromDB(&buf, &buflen, w, "Affiliation1Text", TLV_AFFILATIONS);
              w = StringToListItemId("Affiliation2", 0);
              ppackTLVWordLNTSfromDB(&buf, &buflen, w, "Affiliation2Text", TLV_AFFILATIONS);*/
        }

        return icq_changeUserDirectoryInfoServ(buf, (WORD)buflen, DIRECTORYREQUEST_UPDATEOWNER);
    }

    return 0; // Failure
}


INT_PTR IcqFileAllow(WPARAM wParam, LPARAM lParam)
{
    if (lParam)
    {
        CCSDATA* ccs = (CCSDATA*)lParam;
        DWORD dwUin;
        uid_str szUid;

        if (getContactUid(ccs->hContact, &dwUin, &szUid))
            return 0; // Invalid contact

        if (icqOnline && ccs->hContact && ccs->lParam && ccs->wParam)
        {
            // approve old fashioned file transfer
            basic_filetransfer* ft = ((basic_filetransfer *)ccs->wParam);

            if (dwUin && ft->ft_magic == FT_MAGIC_ICQ)
            {
                filetransfer* ft = ((filetransfer *)ccs->wParam);

                ft->szSavePath = null_strdup((char *)ccs->lParam);
                AddExpectedFileRecv(ft);

                // Was request received thru DC and have we a open DC, send through that
                if (ft->bDC && IsDirectConnectionOpen(ccs->hContact, DIRECTCONN_STANDARD, 0))
                    icq_sendFileAcceptDirect(ccs->hContact, ft);
                else
                    icq_sendFileAcceptServ(dwUin, ft, 0);

                return ccs->wParam; // Success
            }
            else if (ft->ft_magic == FT_MAGIC_OSCAR)
            {
                // approve oscar file transfer
                return oftFileAllow(ccs->hContact, ccs->wParam, ccs->lParam);
            }
        }
    }

    return 0; // Failure
}



INT_PTR IcqFileDeny(WPARAM wParam, LPARAM lParam)
{
    int nReturnValue = 1;

    if (lParam)
    {
        CCSDATA *ccs = (CCSDATA *)lParam;
        DWORD dwUin;
        uid_str szUid;
        basic_filetransfer *ft = (basic_filetransfer*)ccs->wParam;

        if (getContactUid(ccs->hContact, &dwUin, &szUid))
            return 1; // Invalid contact

        if (icqOnline && ccs->wParam && ccs->hContact)
        {
            if (dwUin && ft->ft_magic == FT_MAGIC_ICQ)
            {
                // deny old fashioned file transfer
                filetransfer *ft = (filetransfer*)ccs->wParam;
                // Was request received thru DC and have we a open DC, send through that
                if (ft->bDC && IsDirectConnectionOpen(ccs->hContact, DIRECTCONN_STANDARD, 0))
                    icq_sendFileDenyDirect(ccs->hContact, ft, (char*)ccs->lParam);
                else
                    icq_sendFileDenyServ(dwUin, ft, (char*)ccs->lParam, 0);

                nReturnValue = 0; // Success
            }
            else if (ft->ft_magic == FT_MAGIC_OSCAR)
            {
                // deny oscar file transfer
                return oftFileDeny(ccs->hContact, ccs->wParam, ccs->lParam);
            }
        }
        // Release possible orphan structure
        SafeReleaseFileTransfer((void**)&ft);
    }

    return nReturnValue;
}



INT_PTR IcqFileCancel(WPARAM wParam, LPARAM lParam)
{
    if (lParam /*&& icqOnline*/)
    {
        CCSDATA* ccs = (CCSDATA*)lParam;
        DWORD dwUin;
        uid_str szUid;

        if (getContactUid(ccs->hContact, &dwUin, &szUid))
            return 1; // Invalid contact

        if (ccs->hContact && ccs->wParam)
        {
            basic_filetransfer *ft = (basic_filetransfer *)ccs->wParam;

            if (dwUin && ft->ft_magic == FT_MAGIC_ICQ)
            {
                // cancel old fashioned file transfer
                filetransfer * ft = (filetransfer * ) ccs->wParam;

                icq_CancelFileTransfer(ccs->hContact, ft);

                return 0; // Success
            }
            else if (ft->ft_magic = FT_MAGIC_OSCAR)
            {
                // cancel oscar file transfer
                return oftFileCancel(ccs->hContact, ccs->wParam, ccs->lParam);
            }
        }
    }

    return 1; // Failure
}



INT_PTR IcqFileResume(WPARAM wParam, LPARAM lParam)
{
    if (icqOnline && wParam)
    {
        PROTOFILERESUME *pfr = (PROTOFILERESUME*)lParam;
        basic_filetransfer *ft = (basic_filetransfer *)wParam;

        if (ft->ft_magic == FT_MAGIC_ICQ)
        {
            icq_sendFileResume((filetransfer *)wParam, pfr->action, pfr->szFilename);
        }
        else if (ft->ft_magic == FT_MAGIC_OSCAR)
        {
            oftFileResume((oscar_filetransfer *)wParam, pfr->action, pfr->szFilename);
        }
        else
            return 1; // Failure

        return 0; // Success
    }

    return 1; // Failure
}



INT_PTR IcqSendSms(WPARAM wParam, LPARAM lParam)
{
    if (icqOnline && wParam && lParam)
        return icq_sendSMSServ((const char *)wParam, (const char *)lParam);

    return 0; // Failure
}

cookie_message_data* CreateMessageCookieData(BYTE bMsgType, HANDLE hContact, DWORD dwUin, int bUseSrvRelay)
{
    BYTE bAckType;
    WORD wStatus = getContactStatus(hContact);

    if (!getSettingByte(NULL, "SlowSend", DEFAULT_SLOWSEND))
        bAckType = ACKTYPE_NONE;
    else if ((bUseSrvRelay && ((!dwUin) || (!CheckContactCapabilities(hContact, CAPF_SRV_RELAY)) ||
                               (wStatus == ID_STATUS_OFFLINE))) || getSettingByte(NULL, "OnlyServerAcks", DEFAULT_ONLYSERVERACKS))
        bAckType = ACKTYPE_SERVER;
    else
        bAckType = ACKTYPE_CLIENT;

    return CreateMessageCookie(bMsgType, bAckType);
}



DWORD ReportGenericSendError(HANDLE hContact, int nType, const char* szErrorMsg)
{
    // just broadcast generic send error with dummy cookie and return that cookie
    DWORD dwCookie = GenerateCookie(0);

    SendProtoAck(hContact, dwCookie, ACKRESULT_FAILED, nType, ICQTranslate(szErrorMsg));

    return dwCookie;
}



static char* convertMsgToUserSpecificAnsi(HANDLE hContact, const char* szMsg)
{
    // this takes utf-8 encoded message
    WORD wCP = getSettingWord(hContact, "CodePage", m_wAnsiCodepage);
    char* szAnsi = NULL;

    if (wCP != CP_ACP)
    {
        // convert to proper codepage
        char *tmp = mir_strdup((char *)szMsg);	// FIXME
        mir_utf8decodecp(tmp, wCP, NULL);
    }
    return szAnsi;
}

INT_PTR IcqSendFile(WPARAM wParam, LPARAM lParam)
{
    if (lParam && icqOnline)
    {
        CCSDATA* ccs = (CCSDATA*)lParam;

        if (ccs->hContact && ccs->lParam && ccs->wParam)
        {
            HANDLE hContact = ccs->hContact;
            char** files = (char**)ccs->lParam;
            char* pszDesc = (char*)ccs->wParam;
            DWORD dwUin;
            uid_str szUid;

            if(_mirandaVersion >= PLUGIN_MAKE_VERSION(0,9,0,1) && _mirandaVersion <= PLUGIN_MAKE_VERSION(0,9,0,6))
            {
                int i;
                wchar_t **files_w = (wchar_t**)ccs->lParam;
                wchar_t *desc_w = (wchar_t*)ccs->wParam;
                for(i=0;; i++)
                {
                    files[i] = (char*)mir_u2a(files_w[i]);
                    if(!(files_w[i+1]))
                        break;
                }
                pszDesc = (char*)mir_u2a(desc_w);
            }

            if (getContactUid(hContact, &dwUin, &szUid))
                return 0; // Invalid contact

            if (getContactStatus(hContact) != ID_STATUS_OFFLINE)
            {
                if (CheckContactCapabilities(hContact, CAPF_AIM_FILE))
                {
                    return oftInitTransfer(hContact, dwUin, szUid, files, pszDesc);
                }
                else if (dwUin)
                {
                    WORD wClientVersion;

                    wClientVersion = getSettingWord(ccs->hContact, "Version", 7);
                    if (wClientVersion < 7)
                    {
                        NetLog_Server("IcqSendFile() can't send to version %u", wClientVersion);
                    }
                    else
                    {
                        int i;
                        filetransfer* ft;
                        struct _stat statbuf;

                        // Initialize filetransfer struct
                        ft = CreateFileTransfer(hContact, dwUin, (wClientVersion == 7) ? 7: 8);

                        for (ft->dwFileCount = 0; files[ft->dwFileCount]; ft->dwFileCount++);
                        ft->pszFiles = (char **)icq_alloc_zero(sizeof(char *) * ft->dwFileCount);
                        ft->dwTotalSize = 0;
                        for (i = 0; i < (int)ft->dwFileCount; i++)
                        {
                            ft->pszFiles[i] = null_strdup(files[i]);

                            if (_stat(files[i], &statbuf))
                                NetLog_Server("IcqSendFile() was passed invalid filename(s)");
                            else
                                ft->dwTotalSize += statbuf.st_size;
                        }
                        ft->szDescription = null_strdup(pszDesc);
                        ft->dwTransferSpeed = 100;
                        ft->sending = 1;
                        ft->fileId = -1;
                        ft->iCurrentFile = 0;
                        ft->dwCookie = AllocateCookie(CKT_FILE, 0, hContact, ft);
                        ft->hConnection = NULL;

                        // Send file transfer request
                        {
                            char szFiles[64];
                            char* pszFiles;


                            NetLog_Server("Init file send");

                            if (ft->dwFileCount == 1)
                            {
                                pszFiles = strrchr(ft->pszFiles[0], '\\');
                                if (pszFiles)
                                    pszFiles++;
                                else
                                    pszFiles = ft->pszFiles[0];
                            }
                            else
                            {
                                null_snprintf(szFiles, 64, ICQTranslate("%d Files"), ft->dwFileCount);
                                pszFiles = szFiles;
                            }

                            // Send packet
                            {
                                if (ft->nVersion == 7)
                                {
                                    if (m_bDCMsgEnabled && IsDirectConnectionOpen(hContact, DIRECTCONN_STANDARD, 0))
                                    {
                                        int iRes = icq_sendFileSendDirectv7(ft, pszFiles);
                                        if (iRes) return (int)(HANDLE)ft; // Success
                                    }
                                    NetLog_Server("Sending v%u file transfer request through server", 7);
                                    icq_sendFileSendServv7(ft, pszFiles);
                                }
                                else
                                {
                                    if (m_bDCMsgEnabled && IsDirectConnectionOpen(hContact, DIRECTCONN_STANDARD, 0))
                                    {
                                        int iRes = icq_sendFileSendDirectv8(ft, pszFiles);
                                        if (iRes) return (int)(HANDLE)ft; // Success
                                    }
                                    NetLog_Server("Sending v%u file transfer request through server", 8);
                                    icq_sendFileSendServv8(ft, pszFiles, ACKTYPE_NONE);
                                }
                            }
                        }

                        return (int)(HANDLE)ft; // Success
                    }
                }
            }
        }
    }

    return 0; // Failure
}




INT_PTR IcqSendYouWereAdded(WPARAM wParam, LPARAM lParam)
{
    if (lParam && icqOnline)
    {
        CCSDATA* ccs = (CCSDATA*)lParam;

        if (ccs->hContact)
        {
            DWORD dwUin, dwMyUin;

            if (getContactUid(ccs->hContact, &dwUin, NULL))
                return 1; // Invalid contact

            dwMyUin = getContactUin(NULL);

            if (dwUin)
            {
                icq_sendYouWereAddedServ(dwUin, dwMyUin);

                return 0; // Success
            }
        }
    }

    return 1; // Failure
}



INT_PTR IcqGrantAuthorization(WPARAM wParam, LPARAM lParam)
{
    if (gnCurrentStatus != ID_STATUS_OFFLINE && gnCurrentStatus != ID_STATUS_CONNECTING && wParam != 0)
    {
        DWORD dwUin;
        uid_str szUid;

        if (getContactUid((HANDLE)wParam, &dwUin, &szUid))
            return 0; // Invalid contact

        // send without reason, do we need any ?
        icq_sendGrantAuthServ(dwUin, szUid, NULL);
        // auth granted, remove contact menu item
        deleteSetting((HANDLE)wParam, "Grant");
    }

    return 0;
}



INT_PTR IcqRevokeAuthorization(WPARAM wParam, LPARAM lParam)
{
    if (gnCurrentStatus != ID_STATUS_OFFLINE && gnCurrentStatus != ID_STATUS_CONNECTING && wParam != 0)
    {
        DWORD dwUin;
        uid_str szUid;

        if (getContactUid((HANDLE)wParam, &dwUin, &szUid))
            return 0; // Invalid contact

        if (MessageBoxUtf(NULL, LPGEN("Are you sure you want to revoke user's authorization (this will remove you from his/her list on some clients) ?"), LPGEN("Confirmation"), MB_ICONQUESTION | MB_YESNO) != IDYES)
            return 0;

        icq_sendRevokeAuthServ(dwUin, szUid);
        // auth revoked, add contact menu item
        setSettingByte((HANDLE)wParam, "Grant", 1);
    }

    return 0;
}

INT_PTR IcqAddServerContact(WPARAM wParam, LPARAM lParam)
{
    DWORD dwUin;
    uid_str szUid;

    if (!m_bSsiEnabled) return 0;

    // Does this contact have a UID?
    if (!getContactUid((HANDLE)wParam, &dwUin, &szUid) && !getSettingWord((HANDLE)wParam, "ServerId", 0) && !getSettingWord((HANDLE)wParam, "SrvIgnoreId", 0))
    {
        char *pszGroup;

        // Read group from DB
        pszGroup = getContactCListGroup((HANDLE)wParam);

        addServContact((HANDLE)wParam, pszGroup);

        mir_free(pszGroup);
        if(getSettingByte((HANDLE)wParam,"CheckSelfRemove", 0))
            setSettingByte((HANDLE)wParam,"CheckSelfRemove", 0);
    }
    return 0;
}

INT_PTR IcqSendtZer(WPARAM wParam, LPARAM lParam)
{
    if (wParam && wParam)
    {
        DWORD dwUin = getSettingDword((HANDLE)wParam, "UIN", 0);
        if (!dwUin)
            return 0;
        SendtZer((HANDLE)wParam, dwUin, tZers[(int)lParam].szId, tZers[(int)lParam].szTxt, tZers[(int)lParam].szUrl);
    }
    return 0;
}

INT_PTR IcqTzerDlg(WPARAM wParam, LPARAM lParam)
{
    if (wParam && icqOnline)
        DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_TZER), NULL, tZersWndProc, (LPARAM)(HANDLE)wParam);
    return 0;
}

/*
   ---------------------------------
   |           Receiving           |
   ---------------------------------
*/

void ICQAddRecvEvent(HANDLE hContact, WORD wType, PROTORECVEVENT* pre, DWORD cbBlob, PBYTE pBlob, DWORD flags)
{
    if (pre->flags & PREF_CREATEREAD)
        flags |= DBEF_READ;

    if (hContact && DBGetContactSettingByte(hContact, "CList", "Hidden", 0))
    {
        DWORD dwUin;
        uid_str szUid;

        setContactHidden(hContact, 0);
        // if the contact was hidden, add to client-list if not in server-list authed
        if (!getSettingWord(hContact, "ServerId", 0) || getSettingByte(hContact, "Auth", 0))
        {
            getContactUid(hContact, &dwUin, &szUid);
            icq_sendNewContact(dwUin, szUid);
        }
    }

    AddEvent(hContact, wType, pre->timestamp, flags, cbBlob, pBlob);
}



INT_PTR IcqRecvFile(WPARAM wParam, LPARAM lParam)
{
    CCSDATA* ccs = (CCSDATA*)lParam;
    PROTORECVEVENT* pre = (PROTORECVEVENT*)ccs->lParam;
    char* szDesc;
    char* szFile;
    DWORD cbBlob;

    szFile = pre->szMessage + sizeof(DWORD);
    szDesc = szFile + strlennull(szFile) + 1;

    cbBlob = sizeof(DWORD) + strlennull(szFile) + strlennull(szDesc) + 2;

    ICQAddRecvEvent(ccs->hContact, EVENTTYPE_FILE, pre, cbBlob, (PBYTE)pre->szMessage, 0);

    return 0;
}


INT_PTR IcqAddCapability(WPARAM wParam, LPARAM lParam)
{
    ICQ_CUSTOMCAP *icqCustomCapIn = (ICQ_CUSTOMCAP *)lParam;
    ICQ_CUSTOMCAP *icqCustomCap = (ICQ_CUSTOMCAP *)malloc(sizeof(ICQ_CUSTOMCAP));
    memcpy(icqCustomCap, icqCustomCapIn, sizeof(ICQ_CUSTOMCAP));
    li.List_Insert(lstCustomCaps, icqCustomCap, lstCustomCaps->realCount);
//	MessageBoxA(NULL, ((ICQ_CUSTOMCAP *)(lstCustomCaps->items[lstCustomCaps->realCount-1]))->name, "custom cap", MB_OK);
    return 0;
}


INT_PTR IcqCheckCapability(WPARAM wParam, LPARAM lParam)
{
    int res = 0;
    DBCONTACTGETSETTING dbcgs;
    DBVARIANT dbvariant;
    HANDLE hContact = (HANDLE)wParam;
    ICQ_CUSTOMCAP *icqCustomCap = (ICQ_CUSTOMCAP *)lParam;
    dbcgs.pValue = &dbvariant;
    dbcgs.szModule = ICQ_PROTOCOL_NAME;
    dbcgs.szSetting = "CapBuf";

    CallService(MS_DB_CONTACT_GETSETTING, (WPARAM)hContact, (LPARAM)&dbcgs);

    if (dbvariant.type == DBVT_BLOB)
    {
        res = MatchCap(dbvariant.pbVal, dbvariant.cpbVal, (const capstr*)&icqCustomCap->caps, 0x10)?1:0;	// FIXME: Why icqCustomCap->caps is not capstr?
    }

    CallService(MS_DB_CONTACT_FREEVARIANT,0,(LPARAM)(DBVARIANT*)&dbvariant);

    return res;
}
extern void setContactIgnore(HANDLE hContact, BOOL bIgnore);
INT_PTR IcqServerIgnore(WPARAM wParam, LPARAM lParam)
{
    setContactIgnore((HANDLE)wParam, !getSettingByte((HANDLE)wParam, "SrvIgnore", 0));

    return 0;
}

BYTE gbRememberPwd;

//////////////////////////////////////////////////////////////////////////

INT_PTR icq_getEventTextMissedMessage(WPARAM wParam, LPARAM lParam)
{
	DBEVENTGETTEXT *pEvent = (DBEVENTGETTEXT *)lParam;

	INT_PTR nRetVal = 0;
	char *pszText = NULL;

	if (pEvent->dbei->cbBlob > 1)
	{
		switch (((WORD*)pEvent->dbei->pBlob)[0])
		{
		case 0:
			pszText = LPGEN("** This message was blocked by the ICQ server ** The message was invalid.");
			break;

		case 1:
			pszText = LPGEN("** This message was blocked by the ICQ server ** The message was too long.");
			break;

		case 2:
			pszText = LPGEN("** This message was blocked by the ICQ server ** The sender has flooded the server.");
			break;

		case 4:
			pszText = LPGEN("** This message was blocked by the ICQ server ** You are too evil.");
			break;

		default:
			pszText = LPGEN("** Unknown missed message event.");
			break;
		}
		if (pEvent->datatype == DBVT_WCHAR)
		{
			WCHAR *pwszText;
			int wchars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszText, strlennull(pszText), NULL, 0);

			pwszText = (WCHAR*)_alloca((wchars + 1) * sizeof(WCHAR));
			pwszText[wchars] = 0;

			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszText, strlennull(pszText), pwszText, wchars);

			nRetVal = (INT_PTR)mir_wstrdup(TranslateW(pwszText));
		}
		else if (pEvent->datatype == DBVT_ASCIIZ)
			nRetVal = (INT_PTR)mir_strdup(Translate(pszText));
	}

	return nRetVal;
}
