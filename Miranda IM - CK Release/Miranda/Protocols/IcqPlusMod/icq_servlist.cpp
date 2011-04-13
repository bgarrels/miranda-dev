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
//  Functions that handles list of used server IDs, sends low-level packets for SSI information
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"



extern BOOL bIsSyncingCL;

static HANDLE hHookSettingChanged = NULL;
static HANDLE hHookContactDeleted = NULL;
static DWORD* pdwServerIDList = NULL;
static int nServerIDListCount = 0;
static int nServerIDListSize = 0;



// cookie struct for pending records
typedef struct ssipendingitem_t
{
    HANDLE hContact;
    char* szGroupPath;
    GROUPADDCALLBACK ofCallback;
    servlistcookie* pCookie;
} ssipendingitem;

static CRITICAL_SECTION servlistMutex;
static int nPendingCount = 0;
static int nPendingSize = 0;
static ssipendingitem** pdwPendingList = NULL;
static int nJustAddedCount = 0;
static int nJustAddedSize = 0;
static HANDLE* pdwJustAddedList = NULL;
static WORD* pwGroupRenameList = NULL;
static int nGroupRenameCount = 0;
static int nGroupRenameSize = 0;

static DWORD updateServContact(HANDLE hContact);


// Add running group rename operation
void AddGroupRename(WORD wGroupID)
{
    EnterCriticalSection(&servlistMutex);
    if (nGroupRenameCount >= nGroupRenameSize)
    {
        nGroupRenameSize += 10;
        pwGroupRenameList = (WORD*)mir_realloc(pwGroupRenameList, nGroupRenameSize * sizeof(WORD));
    }

    pwGroupRenameList[nGroupRenameCount] = wGroupID;
    nGroupRenameCount++;
    LeaveCriticalSection(&servlistMutex);
}


// Remove running group rename operation
void RemoveGroupRename(WORD wGroupID)
{
    int i, j;

    EnterCriticalSection(&servlistMutex);
    if (pwGroupRenameList)
    {
        for (i = 0; i<nGroupRenameCount; i++)
        {
            if (pwGroupRenameList[i] == wGroupID)
            {
                // we found it, so remove
                for (j = i+1; j<nGroupRenameCount; j++)
                {
                    pwGroupRenameList[j-1] = pwGroupRenameList[j];
                }
                nGroupRenameCount--;
            }
        }
    }
    LeaveCriticalSection(&servlistMutex);
}


// Returns true if dwID is reserved
BOOL IsGroupRenamed(WORD wGroupID)
{
    int i;

    EnterCriticalSection(&servlistMutex);
    if (pwGroupRenameList)
    {
        for (i = 0; i<nGroupRenameCount; i++)
        {
            if (pwGroupRenameList[i] == wGroupID)
            {
                LeaveCriticalSection(&servlistMutex);
                return TRUE;
            }
        }
    }
    LeaveCriticalSection(&servlistMutex);

    return FALSE;
}


void FlushGroupRenames()
{
    EnterCriticalSection(&servlistMutex);
    mir_free(pwGroupRenameList);
    nGroupRenameCount = 0;
    nGroupRenameSize = 0;
    LeaveCriticalSection(&servlistMutex);
}


// used for adding new contacts to list - sync with visible items
void AddJustAddedContact(HANDLE hContact)
{
    EnterCriticalSection(&servlistMutex);
    if (nJustAddedCount >= nJustAddedSize)
    {
        nJustAddedSize += 10;
        pdwJustAddedList = (HANDLE*)mir_realloc(pdwJustAddedList, nJustAddedSize * sizeof(HANDLE));
    }

    pdwJustAddedList[nJustAddedCount] = hContact;
    nJustAddedCount++;
    LeaveCriticalSection(&servlistMutex);
}


// was the contact added during this serv-list load
BOOL IsContactJustAdded(HANDLE hContact)
{
    int i;

    EnterCriticalSection(&servlistMutex);
    if (pdwJustAddedList)
    {
        for (i = 0; i<nJustAddedCount; i++)
        {
            if (pdwJustAddedList[i] == hContact)
            {
                LeaveCriticalSection(&servlistMutex);
                return TRUE;
            }
        }
    }
    LeaveCriticalSection(&servlistMutex);

    return FALSE;
}


void FlushJustAddedContacts()
{
    EnterCriticalSection(&servlistMutex);
    mir_free(&pdwJustAddedList);
    nJustAddedSize = 0;
    nJustAddedCount = 0;
    LeaveCriticalSection(&servlistMutex);
}



// Used for event-driven adding of contacts, before it is completed this is used
static BOOL AddPendingOperation(HANDLE hContact, const char* szGroup, servlistcookie* cookie, GROUPADDCALLBACK ofEvent)
{
    BOOL bRes = TRUE;
    ssipendingitem* pItem = NULL;
    int i;

    EnterCriticalSection(&servlistMutex);
    if (pdwPendingList)
    {
        for (i = 0; i<nPendingCount; i++)
        {
            if (pdwPendingList[i]->hContact == hContact)
            {
                // we need the last item for this contact
                pItem = pdwPendingList[i];
            }
        }
    }

    if (pItem) // we found a pending operation, so link our data
    {
        pItem->ofCallback = ofEvent;
        pItem->pCookie = cookie;
        pItem->szGroupPath = null_strdup(szGroup); // we need to duplicate the string
        bRes = FALSE;

        NetLog_Server("Operation postponed.");
    }

    if (nPendingCount >= nPendingSize) // add new
    {
        nPendingSize += 10;
        pdwPendingList = (ssipendingitem**)mir_realloc(pdwPendingList, nPendingSize * sizeof(ssipendingitem*));
    }

    pdwPendingList[nPendingCount] = (ssipendingitem*)icq_alloc_zero(sizeof(ssipendingitem));
    pdwPendingList[nPendingCount]->hContact = hContact;

    nPendingCount++;
    LeaveCriticalSection(&servlistMutex);

    return bRes;
}



// Check if any pending operation is in progress
// If yes, get its data and remove it from queue
void RemovePendingOperation(HANDLE hContact, int nResult)
{
    int i, j;
    ssipendingitem* pItem = NULL;

    EnterCriticalSection(&servlistMutex);
    if (pdwPendingList)
    {
        for (i = 0; i<nPendingCount; i++)
        {
            if (pdwPendingList[i]->hContact == hContact)
            {
                pItem = pdwPendingList[i];
                for (j = i+1; j<nPendingCount; j++)
                {
                    pdwPendingList[j-1] = pdwPendingList[j];
                }
                nPendingCount--;
                if (nResult) // we succeded, go on, resume operation
                {
                    LeaveCriticalSection(&servlistMutex);

                    if (pItem->ofCallback)
                    {
                        NetLog_Server("Resuming postponed operation.");

                        makeGroupId(pItem->szGroupPath, pItem->ofCallback, pItem->pCookie);
                    }
                    else if ((int)pItem->pCookie == 1)
                    {
                        NetLog_Server("Resuming postponed update.");

                        updateServContact(hContact);
                    }

                    mir_free(pItem->szGroupPath); // free the string
                    mir_free(pItem);
                    return;
                } // else remove all pending operations for this contact
                NetLog_Server("Purging postponed operation.");
                if ((pItem->pCookie) && ((int)pItem->pCookie != 1))
                    mir_free(pItem->pCookie->szGroupName); // do not leak nick name on error
                mir_free(pItem->szGroupPath);
                mir_free(pItem);
            }
        }
    }
    LeaveCriticalSection(&servlistMutex);
    return;
}



// Remove All pending operations
void FlushPendingOperations()
{
    int i;

    EnterCriticalSection(&servlistMutex);

    for (i = 0; i<nPendingCount; i++)
    {
        mir_free(&pdwPendingList[i]);
    }
    mir_free(&pdwPendingList);
    nPendingCount = 0;
    nPendingSize = 0;

    LeaveCriticalSection(&servlistMutex);
}



// Add a server ID to the list of reserved IDs.
// To speed up the process, no checks is done, if
// you try to reserve an ID twice, it will be added again.
// You should call CheckServerID before reserving an ID.
void ReserveServerID(WORD wID, int bGroupType, int bFlags)
{
    EnterCriticalSection(&servlistMutex);
    if (nServerIDListCount >= nServerIDListSize)
    {
        nServerIDListSize += 100;
        pdwServerIDList = (DWORD*)mir_realloc(pdwServerIDList, nServerIDListSize * sizeof(DWORD));
    }
    pdwServerIDList[nServerIDListCount] = wID | (bGroupType & 0x00FF0000) | (bFlags & 0xFF000000);
    nServerIDListCount++;
    LeaveCriticalSection(&servlistMutex);
    if (!bIsSyncingCL)
        StoreServerIDs();
}



// Remove a server ID from the list of reserved IDs.
// Used for deleting contacts and other modifications.
void FreeServerID(WORD wID, int bGroupType)
{
    int i, j;
    DWORD dwId = wID | (bGroupType & 0x00FF0000);

    EnterCriticalSection(&servlistMutex);
    if (pdwServerIDList)
    {
        for (i = 0; i<nServerIDListCount; i++)
        {
            if ((pdwServerIDList[i] & 0x00FFFFFF) == dwId)
            {
                // we found it, so remove
                for (j = i+1; j<nServerIDListCount; j++)
                    pdwServerIDList[j-1] = pdwServerIDList[j];
                nServerIDListCount--;
            }
        }
    }
    LeaveCriticalSection(&servlistMutex);
}


// Returns true if dwID is reserved
BOOL CheckServerID(WORD wID, unsigned int wCount)
{
    int i;

    EnterCriticalSection(&servlistMutex);
    if (pdwServerIDList)
    {
        for (i = 0; i<nServerIDListCount; i++)
        {
            if (((pdwServerIDList[i] & 0xFFFF) >= wID) && ((pdwServerIDList[i] & 0xFFFF) <= wID + wCount))
            {
                LeaveCriticalSection(&servlistMutex);
                return TRUE;
            }
        }
    }
    LeaveCriticalSection(&servlistMutex);

    return FALSE;
}



void FlushServerIDs()
{
    EnterCriticalSection(&servlistMutex);
    mir_free(pdwServerIDList);
    pdwServerIDList = NULL;	// NOTE: We must null it otherwise realloc won't be able to allocate memory with dirty pointer
    nServerIDListCount = 0;
    nServerIDListSize = 0;
    LeaveCriticalSection(&servlistMutex);
}



static int GroupReserveIdsEnumProc(const char *szSetting,LPARAM lParam)
{
    if (szSetting && strlennull(szSetting)<5)
    {
        // it is probably server group
        char val[MAX_PATH+2]; // dummy
        DBVARIANT dbv;
        DBCONTACTGETSETTING cgs;

        dbv.type = DBVT_ASCIIZ;
        dbv.pszVal = val;
        dbv.cchVal = MAX_PATH;

        cgs.szModule=(char*)lParam;
        cgs.szSetting=szSetting;
        cgs.pValue=&dbv;
        if (CallService(MS_DB_CONTACT_GETSETTINGSTATIC,(WPARAM)NULL,(LPARAM)&cgs))
        {
            // we failed to read setting, try also utf8 - DB bug
            dbv.type = DBVT_UTF8;
            dbv.pszVal = val;
            dbv.cchVal = MAX_PATH;
            if (CallService(MS_DB_CONTACT_GETSETTINGSTATIC,(WPARAM)NULL,(LPARAM)&cgs))
                return 0; // we failed also, invalid setting
        }
        if (dbv.type!=DBVT_ASCIIZ)
        {
            // it is not a cached server-group name
            return 0;
        }
        ReserveServerID((WORD)strtoul(szSetting, NULL, 0x10), SSIT_GROUP, 0);
#ifdef _DEBUG
        NetLog_Server("Loaded group %u:'%s'", strtoul(szSetting, NULL, 0x10), val);
#endif
    }
    return 0;
}



int ReserveServerGroups()
{
    DBCONTACTENUMSETTINGS dbces;
    int nStart = nServerIDListCount;

    char szModule[MAX_PATH+9];

    strcpy(szModule, ICQ_PROTOCOL_NAME);
    strcat(szModule, "SrvGroups");

    dbces.pfnEnumProc = &GroupReserveIdsEnumProc;
    dbces.szModule = szModule;
    dbces.lParam = (LPARAM)szModule;

    CallService(MS_DB_CONTACT_ENUMSETTINGS, (WPARAM)NULL, (LPARAM)&dbces);

    return nServerIDListCount - nStart;
}



// Load all known server IDs from DB to list
void LoadServerIDs()
{
    HANDLE hContact;
    WORD wSrvID;
    int nGroups = 0, nContacts = 0, nPermits = 0, nDenys = 0, nIgnores = 0, nUnhandled = 0;

    EnterCriticalSection(&servlistMutex);
    if (wSrvID = getSettingWord(NULL, "SrvAvatarID", 0))
        ReserveServerID(wSrvID, SSIT_ITEM, 0);
    if (wSrvID = getSettingWord(NULL, "SrvPhotoID", 0))
        ReserveServerID(wSrvID, SSIT_ITEM, 0);
    if (wSrvID = getSettingWord(NULL, "SrvVisibilityID", 0))
        ReserveServerID(wSrvID, SSIT_ITEM, 0);
    if (wSrvID = getSettingWord(NULL, "SrvImportID", 0))
        ReserveServerID(wSrvID, SSIT_ITEM, 0);


    nGroups = ReserveServerGroups();

    hContact = FindFirstContact();

    while (hContact)
    {
        // search all our contacts, reserve their server IDs
        if (wSrvID = getSettingWord(hContact, "ServerId", 0))
        {
            ReserveServerID(wSrvID, SSIT_ITEM, 0);
            nContacts++;
        }
        if (wSrvID = getSettingWord(hContact, "SrvDenyId", 0))
        {
            ReserveServerID(wSrvID, SSIT_ITEM, 0);
            nDenys++;
        }
        if (wSrvID = getSettingWord(hContact, "SrvPermitId", 0))
        {
            ReserveServerID(wSrvID, SSIT_ITEM, 0);
            nPermits++;
        }
        if (wSrvID = getSettingWord(hContact, "SrvIgnoreId", 0))
        {
            ReserveServerID(wSrvID, SSIT_ITEM, 0);
            nIgnores++;
        }

        hContact = FindNextContact(hContact);
    }
    LeaveCriticalSection(&servlistMutex);
    {
        DBVARIANT dbv = {0};
        if (!DBGetContactSettingW(NULL, ICQ_PROTOCOL_NAME, "SrvUnhandledIDList", &dbv))
        {
            int dataLen = dbv.cpbVal;
            BYTE *pData = dbv.pbVal;

            while (dataLen >= 4)
            {
                BYTE bGroupType;
                BYTE bFlags;

                unpackLEWord(&pData, &wSrvID);
                unpackByte(&pData, &bGroupType);
                unpackByte(&pData, &bFlags);

                ReserveServerID(wSrvID, bGroupType, bFlags);
                dataLen -= 4;
                nUnhandled++;
            }
            ICQFreeVariant(&dbv);
        }
        NetLog_Server("Loaded SSI: %d contacts, %d groups, %d permit, %d deny, %d ignore, %d unknown items.", nContacts, nGroups, nPermits, nDenys, nIgnores, nUnhandled);
    }


    return;
}



void StoreServerIDs()
{
    BYTE *pUnhandled = NULL;
    int cbUnhandled = 0, i;
    EnterCriticalSection(&servlistMutex);
    if (pdwServerIDList)
        for (i = 0; i<nServerIDListCount; i++)
            if ((pdwServerIDList[i] & 0xFF000000) == SSIF_UNHANDLED)
            {
                ppackLEWord(&pUnhandled, &cbUnhandled, pdwServerIDList[i] & 0xFFFF);
                ppackByte(&pUnhandled, &cbUnhandled, (pdwServerIDList[i] & 0x00FF0000) >> 0x10);
                ppackByte(&pUnhandled, &cbUnhandled, (pdwServerIDList[i] & 0xFF000000) >> 0x18);
            }
    LeaveCriticalSection(&servlistMutex);
    if (pUnhandled)
        setSettingBlob(NULL, "SrvUnhandledIDList", pUnhandled, cbUnhandled);
    else
        DBDeleteContactSetting(NULL, ICQ_PROTOCOL_NAME, "SrvUnhandledIDList");
}



void ResetUserSSISettings(HANDLE hContact)
{
    DBWriteContactSettingWord(hContact, ICQ_PROTOCOL_NAME, "ServerId", 0);
    DBWriteContactSettingWord(hContact, ICQ_PROTOCOL_NAME, "SrvGroupId", 0);
    DBWriteContactSettingWord(hContact, ICQ_PROTOCOL_NAME, "SrvPermitId", 0);
    DBWriteContactSettingWord(hContact, ICQ_PROTOCOL_NAME, "SrvDenyId", 0);
    DBWriteContactSettingByte(hContact, ICQ_PROTOCOL_NAME, "Auth", 0);
}


void ResetAllSSISettings(void)
{

    HANDLE hContact;
    int ID = 0;

    while (GetFromCacheByID(ID++, &hContact, NULL))
    {
        ResetUserSSISettings(hContact);
    }
    DBWriteContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "SrvAvatarID", 0);
    DBWriteContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "SrvVisibilityID", 0);
    DBWriteContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "SrvDefGroupId", 0);
    DBWriteContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "SrvRecordCount", 0);
    DBWriteContactSettingWord(NULL, ICQ_PROTOCOL_NAME, "SrvImportId", 0);
    DBWriteContactSettingDword(NULL, ICQ_PROTOCOL_NAME, "ImportTS", 0);

    // reset only if we reload list
    FlushServerIDs();

}


// Generate server ID with wCount IDs free after it, for sub-groups.
WORD GenerateServerID(int bGroupType, int bFlags, int wCount)
{
    WORD wId;

    while (TRUE)
    {
        // Randomize a new ID
        // Max value is probably 0x7FFF, lowest value is probably 0x0001 (generated by Icq2Go)
        // We use range 0x1000-0x7FFF.
        wId = (WORD)RandRange(0x1000, 0x7FFF);

        if (!CheckServerID(wId, wCount))
            break;
    }

    ReserveServerID(wId, bGroupType, 0);

    return wId;
}


/***********************************************
 *
 *  --- Low-level packet sending functions ---
 *
 */


static DWORD icq_sendServerItem(DWORD dwCookie, WORD wAction, WORD wGroupId, WORD wItemId, const char *szName, BYTE *pTLVs, int nTlvLength, WORD wItemType)
{
    // generic packet
    icq_packet packet;
    int nNameLen;
    WORD wTLVlen = (WORD)nTlvLength;

    // Prepare item name length
    nNameLen = strlennull(szName);

    // Build the packet
    serverPacketInit(&packet, (WORD)(nNameLen + 20 + wTLVlen));
    packFNACHeaderFull(&packet, ICQ_LISTS_FAMILY, wAction, 0, dwCookie);
    packWord(&packet, (WORD)nNameLen);
    if (nNameLen)
        packBuffer(&packet, szName, (WORD)nNameLen);
    packWord(&packet, wGroupId);
    packWord(&packet, wItemId);
    packWord(&packet, wItemType);
    packWord(&packet, wTLVlen);
    if (wTLVlen)
        packBuffer(&packet, (char*)pTLVs, wTLVlen);

    // Send the packet and return the cookie
    sendServPacket(&packet);

    // Force reload of server-list after change
    setSettingWord(NULL, "SrvRecordCount", 0);

    return dwCookie;
}



DWORD icq_sendServerContact(HANDLE hContact, DWORD dwCookie, WORD wAction, WORD wGroupId, WORD wContactId)
{
    DWORD dwUin;
    uid_str szUid;
    icq_packet pBuffer;
    char *szNick = NULL, *szNote = NULL;
    BYTE *pData = NULL;
    int nNickLen, nNoteLen, nDataLen;
    WORD wTLVlen;
    BYTE bAuth;
    int bDataTooLong = FALSE;

    // Prepare UID
    if (getContactUid(hContact, &dwUin, &szUid))
    {
        NetLog_Server("Buddy upload failed (UID missing).");
        return 0;
    }

    bAuth = getSettingByte(hContact, "Auth", 0);
    szNick = getSettingStringUtf(hContact, "CList", "MyHandle", NULL);
    szNote = getSettingStringUtf(hContact, "UserInfo", "MyNotes", NULL);

    {
        DBVARIANT dbv;

        if (!getSetting(hContact, "ServerData", &dbv))
        {
            // read additional server item data
            nDataLen = dbv.cpbVal;
            pData = (BYTE*)icq_alloc_zero(nDataLen);
            memcpy(pData, dbv.pbVal, nDataLen);

            ICQFreeVariant(&dbv);
        }
        else
        {
            pData = NULL;
            nDataLen = 0;
        }
    }

    nNickLen = strlennull(szNick);
    nNoteLen = strlennull(szNote);

    // Limit the strings
    if (nNickLen > MAX_SSI_TLV_NAME_SIZE)
    {
        bDataTooLong = TRUE;
        nNickLen = null_strcut(szNick, MAX_SSI_TLV_NAME_SIZE);
    }
    if (nNoteLen > MAX_SSI_TLV_COMMENT_SIZE)
    {
        bDataTooLong = TRUE;
        nNoteLen = null_strcut(szNote, MAX_SSI_TLV_COMMENT_SIZE);
    }
    if (bDataTooLong)
    {
        // Inform the user
        /// TODO: do something with this for Manage Server-List dialog.
        if (wAction != ICQ_LISTS_REMOVEFROMLIST) // do not report this when removing from list
            icq_LogMessage(LOG_WARNING, "The contact's information was too big and was truncated.");
    }

    // Build the packet
    wTLVlen = (nNickLen?4+nNickLen:0) + (nNoteLen?4+nNoteLen:0) + (bAuth?4:0) + nDataLen;

    // Initialize our handy data buffer
    pBuffer.wPlace = 0;
    pBuffer.pData = (BYTE *)icq_alloc_zero(wTLVlen);
    pBuffer.wLen = wTLVlen;

    if (nNickLen)
        packTLV(&pBuffer, SSI_TLV_NAME, (WORD)nNickLen, szNick);  // Nickname TLV

    if (nNoteLen)
        packTLV(&pBuffer, SSI_TLV_COMMENT, (WORD)nNoteLen, szNote);  // Comment TLV

    if (pData)
        packBuffer(&pBuffer, (char*)pData, (WORD)nDataLen);

    if (bAuth) // icq5 gives this as last TLV
        packDWord(&pBuffer, 0x00660000);  // "Still waiting for auth" TLV

    mir_free(szNick);
    mir_free(szNote);

    return icq_sendServerItem(dwCookie, wAction, wGroupId, wContactId, strUID(dwUin, szUid), pBuffer.pData, wTLVlen, SSI_ITEM_BUDDY);
}



DWORD icq_sendSimpleItem(DWORD dwCookie, WORD wAction, DWORD dwUin, char* szUID, WORD wGroupId, WORD wItemId, WORD wItemType)
{
    // for privacy items
    return icq_sendServerItem(dwCookie, wAction, wGroupId, wItemId, strUID(dwUin, szUID), NULL, 0, wItemType);
}



DWORD icq_sendGroupUtf(DWORD dwCookie, WORD wAction, WORD wGroupId, const char *szName, void *pContent, int cbContent)
{
    WORD wTLVlen;
    icq_packet pBuffer; // I reuse the ICQ packet type as a generic buffer
    // I should be ashamed! ;)

    if (strlennull(szName) == 0 && wGroupId != 0)
    {
        NetLog_Server("Group upload failed (GroupName missing).");
        return 0; // without name we could not change the group
    }

    // Calculate buffer size
    wTLVlen = (cbContent?4+cbContent:0);

    // Initialize our handy data buffer
    pBuffer.wPlace = 0;
    pBuffer.pData = (BYTE *)icq_alloc_zero(wTLVlen);
    pBuffer.wLen = wTLVlen;

    if (wTLVlen)
        packTLV(&pBuffer, SSI_TLV_SUBITEMS, (WORD)cbContent, (const char*)pContent);  // Groups TLV

    return icq_sendServerItem(dwCookie, wAction, wGroupId, 0, szName, pBuffer.pData, wTLVlen, SSI_ITEM_GROUP);
}



DWORD icq_modifyServerPrivacyItem(HANDLE hContact, DWORD dwUin, char* szUid, WORD wAction, DWORD dwOperation, WORD wItemId, WORD wType)
{
    servlistcookie* ack = NULL;
    DWORD dwCookie;

    ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));

    if (!ack)
    {
        // cookie failed, use old fake
        dwCookie = GenerateCookie(wAction);
    }
    else
    {
        ack->dwAction = dwOperation; // remove privacy item
        ack->dwUin = dwUin;
        ack->hContact = hContact;
        ack->wContactId = wItemId;

        dwCookie = AllocateCookie(CKT_SERVERLIST, wAction, hContact, ack);
    }
    return icq_sendSimpleItem(dwCookie, wAction, dwUin, szUid, 0, wItemId, wType);
}



DWORD icq_removeServerPrivacyItem(HANDLE hContact, DWORD dwUin, char* szUid, WORD wItemId, WORD wType)
{
    return icq_modifyServerPrivacyItem(hContact, dwUin, szUid, ICQ_LISTS_REMOVEFROMLIST, SSA_PRIVACY_REMOVE, wItemId, wType);
}



DWORD icq_addServerPrivacyItem(HANDLE hContact, DWORD dwUin, char* szUid, WORD wItemId, WORD wType)
{
    return icq_modifyServerPrivacyItem(hContact, dwUin, szUid, ICQ_LISTS_ADDTOLIST, SSA_PRIVACY_ADD, wItemId, wType);
}


/*****************************************
 *
 *     --- Contact DB Utilities ---
 *
 */

static int GroupNamesEnumProc(const char *szSetting,LPARAM lParam)
{
    // if we got pointer, store setting name, return zero
    if (lParam)
    {
        char** block = (char**)icq_alloc_zero(2*sizeof(char*));
        block[1] = null_strdup(szSetting);
        block[0] = ((char**)lParam)[0];
        ((char**)lParam)[0] = (char*)block;
    }
    return 0;
}



void DeleteModuleEntries(const char* szModule)
{
    DBCONTACTENUMSETTINGS dbces;
    char** list = NULL;

    dbces.pfnEnumProc = &GroupNamesEnumProc;
    dbces.szModule = szModule;
    dbces.lParam = (LPARAM)&list;
    CallService(MS_DB_CONTACT_ENUMSETTINGS, (WPARAM)NULL, (LPARAM)&dbces);
    while (list)
    {
        void* bet;

        DBDeleteContactSetting(NULL, szModule, list[1]);
        mir_free(list[1]);
        bet = list;
        list = (char**)list[0];
        mir_free(bet);
    }
}



int IsServerGroupsDefined()
{
    int iRes = 1;

    if (getSettingDword(NULL, "Version", 0) < 0x00030608)
    {
        // group cache & linking data too old, flush, reload from server
        char szModule[MAX_PATH+9];

        strcpy(szModule, ICQ_PROTOCOL_NAME);
        strcat(szModule, "Groups"); // flush obsolete linking data
        DeleteModuleEntries(szModule);

        iRes = 0; // no groups defined, or older version
    }
    // store our current version
    setSettingDword(NULL, "Version", ICQ_PLUG_VERSION & 0x00FFFFFF);

    return iRes;
}



void FlushSrvGroupsCache()
{
    char szModule[MAX_PATH+9];

    strcpy(szModule, ICQ_PROTOCOL_NAME);
    strcat(szModule, "SrvGroups");
    DeleteModuleEntries(szModule);
}



// Look thru DB and collect all ContactIDs from a group
void* collectBuddyGroup(WORD wGroupID, int *count)
{
    WORD* buf = NULL;
    int cnt = 0;
    HANDLE hContact;
    WORD wItemID;

    hContact = FindFirstContact();

    while (hContact)
    {
        // search all contacts
        if (wGroupID == getSettingWord(hContact, "SrvGroupId", 0))
        {
            // add only buddys from specified group
            wItemID = getSettingWord(hContact, "ServerId", 0);

            if (wItemID)
            {
                // valid ID, add
                cnt++;
                buf = (WORD*)mir_realloc(buf, cnt*sizeof(WORD));
                buf[cnt-1] = wItemID;
            }
        }

        hContact = FindNextContact(hContact);
    }

    *count = cnt<<1; // we return size in bytes
    return buf;
}



// Look thru DB and collect all GroupIDs
void* collectGroups(int *count)
{
    WORD* buf = NULL;
    int cnt = 0;
    int i;
    HANDLE hContact;
    WORD wGroupID;

    hContact = FindFirstContact();

    while (hContact)
    {
        // search all contacts
        if (wGroupID = getSettingWord(hContact, "SrvGroupId", 0))
        {
            // add only valid IDs
            for (i = 0; i<cnt; i++)
            {
                // check for already added ids
                if (buf[i] == wGroupID) break;
            }

            if (i == cnt)
            {
                // not preset, add
                cnt++;
                buf = (WORD*)mir_realloc(buf, cnt*sizeof(WORD));
                buf[i] = wGroupID;
            }
        }

        hContact = FindNextContact(hContact);
    }

    *count = cnt<<1;
    return buf;
}



static int GroupLinksEnumProc(const char *szSetting,LPARAM lParam)
{
    // check link target, add if match
    if (DBGetContactSettingWord(NULL, ((char**)lParam)[2], szSetting, 0) == (WORD)((char**)lParam)[1])
    {
        char** block = (char**)icq_alloc_zero(2*sizeof(char*));
        block[1] = null_strdup(szSetting);
        block[0] = ((char**)lParam)[0];
        ((char**)lParam)[0] = (char*)block;
    }
    return 0;
}



void removeGroupPathLinks(WORD wGroupID)
{
    // remove miranda grouppath links targeting to this groupid
    DBCONTACTENUMSETTINGS dbces;
    char szModule[MAX_PATH+6];
    char* pars[3];

    strcpy(szModule, ICQ_PROTOCOL_NAME);
    strcat(szModule, "Groups");

    pars[0] = NULL;
    pars[1] = (char*)wGroupID;
    pars[2] = szModule;

    dbces.pfnEnumProc = &GroupLinksEnumProc;
    dbces.szModule = szModule;
    dbces.lParam = (LPARAM)pars;

    if (!CallService(MS_DB_CONTACT_ENUMSETTINGS, (WPARAM)NULL, (LPARAM)&dbces))
    {
        // we found some links, remove them
        char** list = (char**)pars[0];
        while (list)
        {
            void* bet;

            DBDeleteContactSetting(NULL, szModule, list[1]);
            mir_free(list[1]);
            bet = list;
            list = (char**)list[0];
            mir_free(bet);
        }
    }
    return;
}



char* getServerGroupNameUtf(WORD wGroupID)
{
    char szModule[MAX_PATH+9];
    char szGroup[16];

    strcpy(szModule, ICQ_PROTOCOL_NAME);
    strcat(szModule, "SrvGroups");
    _itoa(wGroupID, szGroup, 0x10);

    if (!CheckServerID(wGroupID, 0))
    {
        // check if valid id, if not give empty and remove
        NetLog_Server("Removing group %u from cache...", wGroupID);
        DBDeleteContactSetting(NULL, szModule, szGroup);
        return NULL;
    }

    return getSettingStringUtf(NULL, szModule, szGroup, NULL);
}



void setServerGroupNameUtf(WORD wGroupID, const char* szGroupNameUtf)
{
    char szModule[MAX_PATH+9];
    char szGroup[16];

    strcpy(szModule, ICQ_PROTOCOL_NAME);
    strcat(szModule, "SrvGroups");
    _itoa(wGroupID, szGroup, 0x10);

    if (szGroupNameUtf)
        setSettingStringUtf(NULL, szModule, szGroup, (char*)szGroupNameUtf);
    else
    {
        DBDeleteContactSetting(NULL, szModule, szGroup);
        removeGroupPathLinks(wGroupID);
    }
    return;
}



WORD getServerGroupIDUtf(const char* szPath)
{
    char szModule[MAX_PATH+6];
    WORD wGroupId;

    strcpy(szModule, ICQ_PROTOCOL_NAME);
    strcat(szModule, "Groups");

    wGroupId = DBGetContactSettingWord(NULL, szModule, szPath, 0);

    if (wGroupId && !CheckServerID(wGroupId, 0))
    {
        // known, check if still valid, if not remove
        NetLog_Server("Removing group \"%s\" from cache...", szPath);
        DBDeleteContactSetting(NULL, szModule, szPath);
        wGroupId = 0;
    }

    return wGroupId;
}



void setServerGroupIDUtf(const char* szPath, WORD wGroupID)
{
    char szModule[MAX_PATH+6];

    strcpy(szModule, ICQ_PROTOCOL_NAME);
    strcat(szModule, "Groups");

    if (wGroupID)
        DBWriteContactSettingWord(NULL, szModule, szPath, wGroupID);
    else
        DBDeleteContactSetting(NULL, szModule, szPath);

    return;
}



// copied from groups.c - horrible, but only possible as this is not available as service
int GroupNameExistsUtf(const char *name,int skipGroup)
{
    char idstr[33];
    char* szGroup = NULL;
    int i;

    if (name == NULL) return 1; // no group always exists
    for(i=0;; i++)
    {
        if(i==skipGroup) continue;
        itoa(i,idstr,10);
        szGroup = getSettingStringUtf(NULL, "CListGroups", idstr, "");
        if (!strlennull(szGroup)) break;
        if (!strcmpnull(szGroup+1, name))
        {
            // caution, this can be false - with ansi clist
            mir_free(szGroup);
            return 1;
        }
        mir_free(szGroup);
    }
    mir_free(szGroup);
    return 0;
}



// utility function which counts > on start of a server group name
static int countGroupNameLevel(const char *szGroupName)
{
    int nNameLen = strlennull(szGroupName);
    int i = 0;

    while (i<nNameLen)
    {
        if (szGroupName[i] != '\\')
            return i;

        i++;
    }
    return -1;
}



int countGroupLevel(WORD wGroupId)
{
    char* szGroupName = getServerGroupNameUtf(wGroupId);
    int cnt = -1;

    if (szGroupName)
        cnt = countGroupNameLevel(szGroupName);

    return cnt;
}



static int countClistGroupLevel(const char *szClistName)
{
    int nNameLen = strlennull(szClistName);
    int i, level = 0;

    for (i = 0; i < nNameLen; i++)
        if (szClistName[i] == '\\') level++;

    return level;
}



int CreateCListGroup(const char* szGroupName)
{
    int hGroup;
    CLIST_INTERFACE *clint = NULL;

    if (ServiceExists(MS_CLIST_RETRIEVE_INTERFACE))
        clint = (CLIST_INTERFACE*)CallService(MS_CLIST_RETRIEVE_INTERFACE, 0, 0);

    hGroup = CallService(MS_CLIST_GROUPCREATE, 0, 0);

    if (gbUnicodeCore_dep && clint && clint->version >= 1)
    {
        // we've got unicode interface, use it
        wchar_t* usTmp = mir_utf8decodeW(szGroupName);

        clint->pfnRenameGroup(hGroup, (TCHAR*)usTmp);
        mir_free(usTmp);
    }
    else
    {
        // old ansi version - no other way
        int size = strlennull(szGroupName) + 2;
        char* szTmp = mir_utf8decodeA(szGroupName);
        CallService(MS_CLIST_GROUPRENAME, hGroup, (LPARAM)szTmp);
    }

    return hGroup;
}



// demangle group path
char* makeGroupPathUtf(WORD wGroupId)
{
    char* szGroup = NULL;

    if (szGroup = getServerGroupNameUtf(wGroupId))
    {
        // this groupid is not valid
        /*    while (strstr(szGroup, "\\")!=NULL) *strstr(szGroup, "\\") = '_'; */ // remove invalid char
        if (getServerGroupIDUtf(szGroup) == wGroupId)
        {
            // this grouppath is known and is for this group, set user group
            return szGroup;
        }
        else
        {
            if (strlennull(szGroup) && (szGroup[0] == '\\'))
            {
                // it is probably a sub-group
                WORD wId = wGroupId-1;
                int level = countGroupLevel(wGroupId);
                int levnew = countGroupLevel(wId);
                char* szTempGroup;

                if (level == -1)
                {
                    // this is just an ordinary group
                    int hGroup;

                    if (!GroupNameExistsUtf(szGroup, -1))
                    {
                        // if the group does not exist, create it
                        hGroup = CreateCListGroup(szGroup);
                    }
                    setServerGroupIDUtf(szGroup, wGroupId); // set grouppath id
                    return szGroup;
                }
                while ((levnew >= level) && (levnew != -1))
                {
                    // we look for parent group
                    wId--;
                    levnew = countGroupLevel(wId);
                }
                if (levnew == -1)
                {
                    // that was not a sub-group, it was just a group starting with >
                    if (!GroupNameExistsUtf(szGroup, -1))
                    {
                        // if the group does not exist, create it
                        int hGroup = CreateCListGroup(szGroup);
                    }
                    setServerGroupIDUtf(szGroup, wGroupId); // set grouppath id
                    return szGroup;
                }

                szTempGroup = makeGroupPathUtf(wId);

                szTempGroup = (char*)mir_realloc(szTempGroup, strlennull(szGroup)+strlennull(szTempGroup)+2);
                strcat(szTempGroup, "\\");
                strcat(szTempGroup, szGroup+level);
                mir_free(szGroup);
                szGroup = szTempGroup;

                if (getServerGroupIDUtf(szGroup) == wGroupId)
                {
                    // known path, give
                    return szGroup;
                }
                else
                {
                    // unknown path, create
                    if (!GroupNameExistsUtf(szGroup, -1))
                    {
                        // if the group does not exist, create it
                        int hGroup = CreateCListGroup(szGroup);
                    }
                    setServerGroupIDUtf(szGroup, wGroupId); // set grouppath id
                    return szGroup;
                }
            }
            else
            {
                // create that group
                int hGroup;

                if (!GroupNameExistsUtf(szGroup, -1))
                {
                    // if the group does not exist, create it
                    hGroup = CreateCListGroup(szGroup);
                }
                setServerGroupIDUtf(szGroup, wGroupId); // set grouppath id
                return szGroup;
            }
        }
    }
    return NULL;
}



// this is the second pard of recursive event-driven procedure
void madeMasterGroupId(WORD wGroupID, LPARAM lParam)
{
    servlistcookie* clue = (servlistcookie*)lParam;
    char* szGroup = clue->szGroupName;
    GROUPADDCALLBACK ofCallback = clue->ofCallback;
    servlistcookie* param = (servlistcookie*)clue->lParam;
    int level;

    if (wGroupID) // if we got an id count level
        level = countGroupLevel(wGroupID);
    else
        level = -1;

    mir_free(clue);

    if (level == -1)
    {
        // something went wrong, give the id and go away
        if (ofCallback) ofCallback(wGroupID, (LPARAM)param);

        mir_free(szGroup);
        return;
    }
    level++; // we are a sub

    // check if on that id is not group of the same or greater level, if yes, try next
    while (CheckServerID((WORD)(wGroupID+1),0) && (countGroupLevel((WORD)(wGroupID+1)) >= level))
    {
        wGroupID++;
    }

    if (!CheckServerID((WORD)(wGroupID+1), 0))
    {
        // the next id is free, so create our group with that id
        servlistcookie* ack = NULL;
        DWORD dwCookie;
        char* szSubGroup = (char*)icq_alloc_zero(strlennull(szGroup)+level+1);

        if (szSubGroup)
        {
            int i;

            for (i=0; i<level; i++)
            {
                szSubGroup[i] = '\\';
            }
            strcpy(szSubGroup+level, szGroup);
            szSubGroup[strlennull(szGroup)+level] = '\0';

            ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));

            if (ack)
            {
                // we have cookie good, go on
                ReserveServerID((WORD)(wGroupID+1), SSIT_GROUP, 0);

                ack->wGroupId = wGroupID+1;
                ack->szGroupName = szSubGroup; // we need that name
                ack->dwAction = SSA_GROUP_ADD;
                ack->ofCallback = ofCallback;
                ack->lParam = (LPARAM)param;
                dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_ADDTOLIST, 0, ack);

                sendAddStart(0);
                icq_sendGroupUtf(dwCookie, ICQ_LISTS_ADDTOLIST, ack->wGroupId, szSubGroup, NULL, 0);

                mir_free(szGroup);
                return;
            }
            mir_free(szSubGroup);
        }
    }
    // we failed to create sub-group give parent groupid
    if (ofCallback) ofCallback(wGroupID, (LPARAM)param);

    mir_free(szGroup);
    return;
}



// create group with this path, a bit complex task
// this supposes that all server groups are known
WORD makeGroupId(const char* szGroupPath, GROUPADDCALLBACK ofCallback, servlistcookie* lParam)
{
    WORD wGroupID = 0;
    char* szGroup = (char*)szGroupPath;

    if (!szGroup || szGroup[0]=='\0') szGroup = DEFAULT_SS_GROUP;

    if (wGroupID = getServerGroupIDUtf(szGroup))
    {
        if (ofCallback) ofCallback(wGroupID, (LPARAM)lParam);
        return wGroupID; // if the path is known give the id
    }

    if (!strstr(szGroup, "\\"))
    {
        // a root group can be simply created without problems
        servlistcookie* ack = NULL;
        DWORD dwCookie;

        if (ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie)))
        {
            // we have cookie good, go on
            ack->wGroupId = GenerateServerID(SSIT_GROUP, 0, 0);
            ack->szGroupName = null_strdup(szGroup); // we need that name
            ack->dwAction = SSA_GROUP_ADD;
            ack->ofCallback = ofCallback;
            ack->lParam = (LPARAM)lParam;
            dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_ADDTOLIST, 0, ack);

            sendAddStart(0);
            icq_sendGroupUtf(dwCookie, ICQ_LISTS_ADDTOLIST, ack->wGroupId, szGroup, NULL, 0);

            return 0;
        }
    }
    else
    {
        // this is a sub-group
        char* szSub = null_strdup(szGroup); // create subgroup, recursive, event-driven, possibly relocate
        servlistcookie* ack = NULL;
        char *szLast;

        if (strstr(szSub, "\\") != NULL)
        {
            // determine parent group
            szLast = strstr(szSub, "\\")+1;

            while (strstr(szLast, "\\") != NULL)
                szLast = strstr(szLast, "\\")+1; // look for last backslash
            szLast[-1] = '\0';
        }
        // make parent group id
        ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));
        if (ack)
        {
            WORD wRes;
            ack->lParam = (LPARAM)lParam;
            ack->ofCallback = ofCallback;
            ack->szGroupName = null_strdup(szLast); // groupname
            wRes = makeGroupId(szSub, madeMasterGroupId, ack);
            mir_free(szSub);

            return wRes;
        }

        mir_free(szSub);
    }

    if (strstr(szGroup, "\\") != NULL)
    {
        // we failed to get grouppath, trim it by one group
        WORD wRes;
        char *szLast = null_strdup(szGroup);
        char *szLess = szLast;

        while (strstr(szLast, "\\") != NULL)
            szLast = strstr(szLast, "\\")+1; // look for last backslash
        szLast[-1] = '\0';
        wRes = makeGroupId(szLess, ofCallback, lParam);
        mir_free(szLess);

        return wRes;
    }

    wGroupID = 0; // everything failed, let callback handle error
    if (ofCallback) ofCallback(wGroupID, (LPARAM)lParam);

    return wGroupID;
}


/*****************************************
 *
 *    --- Server-List Operations ---
 *
 */

void addServContactReady(WORD wGroupID, LPARAM lParam)
{
    WORD wItemID;
    DWORD dwUin;
    uid_str szUid;
    servlistcookie* ack;
    DWORD dwCookie;

    ack = (servlistcookie*)lParam;

    if (!ack || !wGroupID) // something went wrong
    {
        if (ack) RemovePendingOperation(ack->hContact, 0);
        return;
    }

    wItemID = getSettingWord(ack->hContact, "ServerId", 0);

    if (wItemID)
    {
        // Only add the contact if it doesnt already have an ID
        RemovePendingOperation(ack->hContact, 0);
        NetLog_Server("Failed to add contact to server side list (%s)", "already there");
        return;
    }

    // Get UID
    if (getContactUid(ack->hContact, &dwUin, &szUid))
    {
        // Could not do anything without uid
        RemovePendingOperation(ack->hContact, 0);
        NetLog_Server("Failed to add contact to server side list (%s)", "no UID");
        return;
    }

    wItemID = GenerateServerID(SSIT_ITEM, 0, 0);

    ack->dwAction = SSA_CONTACT_ADD;
    ack->dwUin = dwUin;
    ack->wGroupId = wGroupID;
    ack->wContactId = wItemID;

    dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_ADDTOLIST, ack->hContact, ack);

    sendAddStart(0);
    icq_sendServerContact(ack->hContact, dwCookie, ICQ_LISTS_ADDTOLIST, wGroupID, wItemID);
    setSettingByte(ack->hContact,"CheckSelfRemove", 0);//checkselfremove
}



// Called when contact should be added to server list, if group does not exist, create one
DWORD addServContact(HANDLE hContact, const char *pszGroup)
{
    servlistcookie* ack = NULL;
    ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));
    if (!ack)
    {
        // Could not do anything without cookie
        NetLog_Server("Failed to add contact to server side list (%s)", "malloc failed");
        return 0;
    }
    else
    {
        ack->hContact = hContact;

        if (AddPendingOperation(hContact, pszGroup, ack, addServContactReady))
            makeGroupId(pszGroup, addServContactReady, ack);

        return 1;
    }
}



// Called when contact should be removed from server list, remove group if it remain empty
DWORD removeServContact(HANDLE hContact)
{
    WORD wGroupID;
    WORD wItemID;
    DWORD dwUin;
    uid_str szUid;
    servlistcookie* ack = NULL;
    DWORD dwCookie;

    // Get the contact's group ID
    if (!(wGroupID = getSettingWord(hContact, "SrvGroupId", 0)))
    {
        // Could not find a usable group ID
        NetLog_Server("Failed to remove contact from server side list (%s)", "no group ID");
        return 0;
    }

    // Get the contact's item ID
    if (!(wItemID = getSettingWord(hContact, "ServerId", 0)))
    {
        // Could not find usable item ID
        NetLog_Server("Failed to remove contact from server side list (%s)", "no item ID");
        return 0;
    }

    // Get UID
    if (getContactUid(hContact, &dwUin, &szUid))
    {
        // Could not do anything without uid
        NetLog_Server("Failed to remove contact from server side list (%s)", "no UID");
        return 0;
    }
    ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));
    if (!ack)
    {
        // Could not do anything without cookie
        NetLog_Server("Failed to remove contact from server side list (%s)", "malloc failed");
        return 0;
    }
    else
    {
        ack->dwAction = SSA_CONTACT_REMOVE;
        ack->dwUin = dwUin;
        ack->hContact = hContact;
        ack->wGroupId = wGroupID;
        ack->wContactId = wItemID;

        dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_REMOVEFROMLIST, hContact, ack);
    }

    sendAddStart(0);
    icq_sendServerContact(hContact, dwCookie, ICQ_LISTS_REMOVEFROMLIST, wGroupID, wItemID);

    return 0;
}



void moveServContactReady(WORD wNewGroupID, LPARAM lParam)
{
    WORD wItemID;
    WORD wGroupID;
    DWORD dwUin;
    uid_str szUid;
    servlistcookie* ack;
    DWORD dwCookie, dwCookie2;

    ack = (servlistcookie*)lParam;

    if (!ack || !wNewGroupID) // something went wrong
    {
        if (ack) RemovePendingOperation(ack->hContact, 0);
        return;
    }

    if (!ack->hContact) return; // we do not move us, caused our uin was wrongly added to list

    wItemID = getSettingWord(ack->hContact, "ServerId", 0);
    wGroupID = getSettingWord(ack->hContact, "SrvGroupId", 0);

    if (!wItemID)
    {
        // We have no ID, so try to simply add the contact to serv-list
        NetLog_Server("Unable to move contact (no ItemID) -> trying to add");
        // we know the GroupID, so directly call add
        addServContactReady(wNewGroupID, lParam);
        return;
    }

    if (!wGroupID)
    {
        // Only move the contact if it had an GroupID
        RemovePendingOperation(ack->hContact, 0);
        NetLog_Server("Failed to move contact to group on server side list (%s)", "no Group");
        return;
    }

    if (wGroupID == wNewGroupID)
    {
        // Only move the contact if it had different GroupID
        RemovePendingOperation(ack->hContact, 1);
        NetLog_Server("Contact not moved to group on server side list (same Group)");
        return;
    }

    // Get UID
    if (getContactUid(ack->hContact, &dwUin, &szUid))
    {
        // Could not do anything without uin
        RemovePendingOperation(ack->hContact, 0);
        NetLog_Server("Failed to move contact to group on server side list (%s)", "no UID");
        return;
    }

    ack->szGroupName = NULL;
    ack->dwAction = SSA_CONTACT_SET_GROUP;
    ack->dwUin = dwUin;
    ack->wGroupId = wGroupID;
    ack->wContactId = wItemID;
    ack->wNewContactId = GenerateServerID(SSIT_ITEM, 0, 0); // icq5 recreates also this, imitate
    ack->wNewGroupId = wNewGroupID;
    ack->lParam = 0; // we use this as a sign

    dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_REMOVEFROMLIST, ack->hContact, ack);
    dwCookie2 = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_ADDTOLIST, ack->hContact, ack);

    sendAddStart(0);
    // imitate icq5, previously here was different order, but AOL changed and it ceased to work
    icq_sendServerContact(ack->hContact, dwCookie, ICQ_LISTS_REMOVEFROMLIST, wGroupID, wItemID);
    icq_sendServerContact(ack->hContact, dwCookie2, ICQ_LISTS_ADDTOLIST, wNewGroupID, ack->wNewContactId);
}



// Called when contact should be moved from one group to another, create new, remove empty
DWORD moveServContactGroup(HANDLE hContact, const char *pszNewGroup)
{
    servlistcookie* ack;

    if (!GroupNameExistsUtf(pszNewGroup, -1) && (pszNewGroup != NULL) && (pszNewGroup[0]!='\0'))
    {
        // the contact moved to non existing group, do not do anything: MetaContact hack
        NetLog_Server("Contact not moved - probably hiding by MetaContacts.");
        return 0;
    }

    if (!getSettingWord(hContact, "ServerId", 0))
    {
        // the contact is not stored on the server, check if we should try to add
        if (!getSettingByte(NULL, "ServerAddRemove", DEFAULT_SS_ADDSERVER) ||
                DBGetContactSettingByte(hContact, "CList", "Hidden", 0))
            return 0;
    }
    ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));
    if (!ack)
    {
        // Could not do anything without cookie
        NetLog_Server("Failed to add contact to server side list (%s)", "malloc failed");
        return 0;
    }
    else
    {
        ack->hContact = hContact;

        if (AddPendingOperation(hContact, pszNewGroup, ack, moveServContactReady))
            makeGroupId(pszNewGroup, moveServContactReady, ack);
        return 1;
    }
}



// Is called when a contact' details has been changed locally to update
// the server side details.
static DWORD updateServContact(HANDLE hContact)
{
    WORD wGroupID;
    WORD wItemID;
    DWORD dwUin;
    uid_str szUid;
    servlistcookie* ack;
    DWORD dwCookie;

    // Get the contact's group ID
    if (!(wGroupID = getSettingWord(hContact, "SrvGroupId", 0)))
    {
        // Could not find a usable group ID
        NetLog_Server("Failed to update contact's details on server side list (%s)", "no group ID");
        RemovePendingOperation(hContact, 0);
        return 0;
    }

    // Get the contact's item ID
    if (!(wItemID = getSettingWord(hContact, "ServerId", 0)))
    {
        // Could not find usable item ID
        NetLog_Server("Failed to update contact's details on server side list (%s)", "no item ID");
        RemovePendingOperation(hContact, 0);
        return 0;
    }

    // Get UID
    if (getContactUid(hContact, &dwUin, &szUid))
    {
        // Could not set nickname on server without uid
        NetLog_Server("Failed to update contact's details on server side list (%s)", "no UID");
        RemovePendingOperation(hContact, 0);
        return 0;
    }
    ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));
    if (!ack)
    {
        // Could not allocate cookie - use old fake
        NetLog_Server("Failed to allocate cookie");
        dwCookie = GenerateCookie(ICQ_LISTS_UPDATEGROUP);
    }
    else
    {
        ack->dwAction = SSA_CONTACT_UPDATE;
        ack->wContactId = wItemID;
        ack->wGroupId = wGroupID;
        ack->dwUin = dwUin;
        ack->hContact = hContact;

        dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_UPDATEGROUP, hContact, ack);
    }

    // There is no need to send ICQ_LISTS_CLI_MODIFYSTART or
    // ICQ_LISTS_CLI_MODIFYEND when just changing nick name
    icq_sendServerContact(hContact, dwCookie, ICQ_LISTS_UPDATEGROUP, wGroupID, wItemID);

    return dwCookie;
}



void renameServGroup(WORD wGroupId, char* szGroupName)
{
    servlistcookie* ack;
    DWORD dwCookie;
    char* szGroup, *szLast;
    int level = countGroupLevel(wGroupId);
    int i;
    void* groupData;
    int groupSize;	// FIXME: It was DWORD

    if (IsGroupRenamed(wGroupId)) return; // the group was already renamed

    if (level == -1) return; // we failed to prepare group

    szLast = szGroupName;
    i = level;
    while (i)
    {
        // find correct part of grouppath
        szLast = strstr(szLast, "\\")+1;
        i--;
    }
    szGroup = (char*)icq_alloc_zero(strlennull(szLast)+1+level);
    if (!szGroup) return;

    for (i=0; i<level; i++)
    {
        // create level prefix
        szGroup[i] = '\\';
    }
    strcat(szGroup, szLast);
    // truncate other possible sub-groups
    szLast = strstr(szGroup, "\\");
    if (szLast)
        szLast[0] = '\0';

    if (!(ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie))))
    {
        // cookie failed, use old fake
        dwCookie = GenerateCookie(ICQ_LISTS_UPDATEGROUP);
    }
    if (groupData = collectBuddyGroup(wGroupId, &groupSize))
    {
        ack->dwAction = SSA_GROUP_RENAME;
        ack->wGroupId = wGroupId;
        ack->szGroupName = szGroup; // we need this name

        dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_UPDATEGROUP, 0, ack);

        AddGroupRename(wGroupId);

        icq_sendGroupUtf(dwCookie, ICQ_LISTS_UPDATEGROUP, wGroupId, szGroup, groupData, groupSize);
        mir_free(groupData);
    }
}



void resetServContactAuthState(HANDLE hContact, DWORD dwUin)
{
    WORD wContactId = getSettingWord(hContact, "ServerId", 0);
    WORD wGroupId = getSettingWord(hContact, "SrvGroupId", 0);

    if (wContactId && wGroupId)
    {
        DWORD dwCookie;
        servlistcookie* ack = NULL;
        ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));
        if (ack)
        {
            // we have cookie good, go on
            ack->hContact = hContact;
            ack->wContactId = wContactId;
            ack->wGroupId = wGroupId;
            ack->dwAction = SSA_CONTACT_FIX_AUTH;
            ack->dwUin = dwUin;
            dwCookie = AllocateCookie(CKT_SERVERLIST, 0, hContact, ack);

            sendAddStart(0);
            icq_sendServerContact(hContact, dwCookie, ICQ_LISTS_REMOVEFROMLIST, wGroupId, wContactId);
            deleteSetting(hContact, "ServerData");
            icq_sendServerContact(hContact, dwCookie, ICQ_LISTS_ADDTOLIST, wGroupId, wContactId);
            sendAddEnd();
        }
    }
}



/*****************************************
 *
 *   --- Miranda Contactlist Hooks ---
 *
 */



static int ServListDbSettingChanged(WPARAM wParam, LPARAM lParam)
{
    DBCONTACTWRITESETTING* cws = (DBCONTACTWRITESETTING*)lParam;
    DBVARIANT dbv = {0};

    // We can't upload changes to NULL contact
    if ((HANDLE)wParam == NULL)
        return 0;

    // TODO: Queue changes that occur while offline
    if (!icqOnline || !m_bSsiEnabled || bIsSyncingCL)
        return 0;

    {
        // only our contacts will be handled
        if (IsICQContact((HANDLE)wParam))
            ;// our contact, fine; otherwise return
        else
            return 0;
    }

    if (!strcmpnull(cws->szModule, "CList"))
    {
        // Has a temporary contact just been added permanently?
        if(DBGetContactSettingString((HANDLE)wParam, "CList", "Group", &dbv) && dbv.pszVal == "Not In List")
            ;
        else if (!strcmpnull(cws->szSetting, "NotOnList") &&
                 (cws->value.type == DBVT_DELETED || (cws->value.type == DBVT_BYTE && cws->value.bVal == 0)) &&
                 getSettingByte(NULL, "ServerAddRemove", DEFAULT_SS_ADDSERVER) &&
                 !DBGetContactSettingByte((HANDLE)wParam, "CList", "Hidden", 0))
        {
            // Add to server-list
            IcqAddServerContact(wParam, 0);
        }

        // Has contact been renamed?
        if (!strcmpnull(cws->szSetting, "MyHandle") &&
                getSettingByte(NULL, "StoreServerDetails", DEFAULT_SS_STORE))
        {
            if (AddPendingOperation((HANDLE)wParam, NULL, (servlistcookie*)1, NULL))
                updateServContact((HANDLE)wParam);
        }

        // Has contact been moved to another group?
        if (!strcmpnull(cws->szSetting, "Group") &&
                getSettingByte(NULL, "StoreServerDetails", DEFAULT_SS_STORE))
        {
            // Test if group was not renamed...
            WORD wGroupId = getSettingWord((HANDLE)wParam, "SrvGroupId", 0);
            char* szGroup = makeGroupPathUtf(wGroupId);
            char* szNewGroup;
            int bRenamed = 0;
            int bMoved = 1;

            // Read group from DB
            szNewGroup = getSettingStringUtf((HANDLE)wParam, "CList", "Group", NULL);

            if (szNewGroup && wGroupId && !GroupNameExistsUtf(szGroup, -1))
            {
                // if we moved from non-existing group, it can be rename
                if (!getServerGroupIDUtf(szNewGroup))
                {
                    // the target group is not known - it is probably rename
                    if (getServerGroupIDUtf(szGroup))
                    {
                        // source group not known -> already renamed
                        if (countClistGroupLevel(szNewGroup) == countGroupLevel(wGroupId))
                        {
                            // renamed groups can be only in the same level, if not it is move
                            if (!IsGroupRenamed(wGroupId))
                            {
                                // is rename in progress ?
                                bRenamed = 1; // TODO: we should really check if group was not moved to sub-group
                                NetLog_Server("Group %x renamed to ""%s"".", wGroupId, szNewGroup);
                            }
                            else // if rename in progress do not move contacts
                                bMoved = 0;
                        }
                    }
                }
            }
            mir_free(szGroup);
            if (bRenamed)
                renameServGroup(wGroupId, szNewGroup);
            else if (bMoved) // TODO: this is bad, we badly need rate management
                moveServContactGroup((HANDLE)wParam, szNewGroup);

            mir_free(szNewGroup);
        }
    }
    if (!strcmpnull(cws->szModule, "UserInfo"))
    {
        if (!strcmpnull(cws->szSetting, "MyNotes") &&
                getSettingByte(NULL, "StoreServerDetails", DEFAULT_SS_STORE))
        {
            if (AddPendingOperation((HANDLE)wParam, NULL, (servlistcookie*)1, NULL))
                updateServContact((HANDLE)wParam);
        }
    }
    return 0;
}



static int ServListDbContactDeleted(WPARAM wParam, LPARAM lParam)
{
    DeleteFromCache((HANDLE)wParam);

    if (!icqOnline && m_bSsiEnabled)
    {
        // contact was deleted only locally - retrieve full list on next connect
        setSettingWord((HANDLE)wParam, "SrvRecordCount", 0);
    }

    if (!icqOnline || !m_bSsiEnabled)
        return 0;

    {
        // we need all server contacts on local buddy list
        WORD wContactID;
        WORD wGroupID;
        WORD wVisibleID;
        WORD wInvisibleID;
        WORD wIgnoreID;
        DWORD dwUIN;
        uid_str szUID;

        wContactID = getSettingWord((HANDLE)wParam, "ServerId", 0);
        wGroupID = getSettingWord((HANDLE)wParam, "SrvGroupId", 0);
        wVisibleID = getSettingWord((HANDLE)wParam, "SrvPermitId", 0);
        wInvisibleID = getSettingWord((HANDLE)wParam, "SrvDenyId", 0);
        wIgnoreID = getSettingWord((HANDLE)wParam, "SrvIgnoreId", 0);
        if (getContactUid((HANDLE)wParam, &dwUIN, &szUID))
            return 0;

        // Close all opened peer connections
        CloseContactDirectConns((HANDLE)wParam);

        if ((wGroupID && wContactID) || wVisibleID || wInvisibleID || wIgnoreID)
        {
            if (wContactID)
            {
                // delete contact from server
                removeServContact((HANDLE)wParam);
            }

            if (wVisibleID)
            {
                // delete permit record
                icq_removeServerPrivacyItem((HANDLE)wParam, dwUIN, szUID, wVisibleID, SSI_ITEM_PERMIT);
            }

            if (wInvisibleID)
            {
                // delete deny record
                icq_removeServerPrivacyItem((HANDLE)wParam, dwUIN, szUID, wInvisibleID, SSI_ITEM_DENY);
            }

            if (wIgnoreID)
            {
                // delete ignore record
                icq_removeServerPrivacyItem((HANDLE)wParam, dwUIN, szUID, wIgnoreID, SSI_ITEM_IGNORE);
            }
        }
    }

    return 0;
}



void InitServerLists(void)
{
    InitializeCriticalSection(&servlistMutex);

    hHookSettingChanged = HookEvent(ME_DB_CONTACT_SETTINGCHANGED, ServListDbSettingChanged);
    hHookContactDeleted = HookEvent(ME_DB_CONTACT_DELETED, ServListDbContactDeleted);
}



void UninitServerLists(void)
{
    if (hHookSettingChanged)
        UnhookEvent(hHookSettingChanged);

    if (hHookContactDeleted)
        UnhookEvent(hHookContactDeleted);

    FlushServerIDs();
    FlushPendingOperations();

    DeleteCriticalSection(&servlistMutex);
}
