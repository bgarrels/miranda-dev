// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright � 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright � 2001,2002 Jon Keating, Richard Hughes
// Copyright � 2002,2003,2004 Martin �berg, Sam Kothari, Robert Rainwater
// Copyright � 2004,2005,2006,2007 Joe Kucera
// Copyright � 2006,2007,2008 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
// Copyright � 2008 [sss], chaos.persei, nullbie, baloo, jarvis
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
// Last change on : $Date: 2007-08-20 01:51:06 +0300 (Пн, 20 авг 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"


extern WORD wListenPort;

extern void setUserInfo();
extern int GroupNameExistsUtf(const char *name,int skipGroup);

BOOL bIsSyncingCL = FALSE;

static HANDLE HContactFromRecordName(char* szRecordName, int *bAdded);

static int getServerDataFromItemTLV(oscar_tlv_chain* pChain, unsigned char *buf);

static int unpackServerListItem(unsigned char** pbuf, WORD* pwLen, char* pszRecordName, WORD* pwGroupId, WORD* pwItemId, WORD* pwItemType, WORD* pwTlvLength);

static void handleServerCListAck(servlistcookie* sc, WORD wError);
static void handleServerCList(unsigned char *buf, WORD wLen, WORD wFlags, serverthread_info *info);
static void handleRecvAuthRequest(unsigned char *buf, WORD wLen);
static void handleRecvAuthResponse(unsigned char *buf, WORD wLen);
static void handleRecvAdded(unsigned char *buf, WORD wLen);
static void handleRecvRemoved(unsigned char *buf, WORD wLen);
void sendRosterAck(void);


static WORD swapWord(WORD val)
{
    return (val & 0xFF)<<8 | (val>>8);
}



void handleServClistFam(unsigned char *pBuffer, WORD wBufferLength, snac_header* pSnacHeader, serverthread_info *info)
{
    switch (pSnacHeader->wSubtype)
    {

    case ICQ_LISTS_ACK: // UPDATE_ACK
        if (wBufferLength >= 2)
        {
            WORD wError;
            servlistcookie* sc;

            unpackWord(&pBuffer, &wError);

            if (FindCookie(pSnacHeader->dwRef, NULL, (void**)&sc))
            {
                // look for action cookie
#ifdef _DEBUG
                NetLog_Server("Received expected server list ack, action: %d, result: %d", sc->dwAction, wError);
#endif
                FreeCookie(pSnacHeader->dwRef); // release cookie

                handleServerCListAck(sc, wError);
            }
            else
            {
                NetLog_Server("Received unexpected server list ack %u", wError);
            }
        }
        break;

    case ICQ_LISTS_SRV_REPLYLISTS:
    {
        /* received list rights, we just skip them */

        oscar_tlv_chain* chain;

        if (chain = readIntoTLVChain(&pBuffer, wBufferLength, 0))
        {
            oscar_tlv* pTLV;

            if ((pTLV = getTLV(chain, 0x04, 1)) && pTLV->wLen>=30)
            {
                // limits for item types
                WORD* pMax = (WORD*)pTLV->pData;

                NetLog_Server("SSI: Max %d contacts, %d groups, %d permit, %d deny, %d ignore items.", swapWord(pMax[0]), swapWord(pMax[1]), swapWord(pMax[2]), swapWord(pMax[3]), swapWord(pMax[14]));
            }

            disposeChain(&chain);
        }
#ifdef _DEBUG
        NetLog_Server("Server sent SNAC(x13,x03) - SRV_REPLYLISTS");
#endif
    }
    break;

    case ICQ_LISTS_LIST: // SRV_REPLYROSTER
    {
        servlistcookie* sc;
        BOOL blWork;

        blWork = bIsSyncingCL;
        bIsSyncingCL = TRUE; // this is not used if cookie takes place

        if (FindCookie(pSnacHeader->dwRef, NULL, (void**)&sc))
        {
            // we do it by reliable cookie
            if (!sc->dwUin)
            {
                // is this first packet ?
                ResetSettingsOnListReload();
                sc->dwUin = 1;
            }
            handleServerCList(pBuffer, wBufferLength, pSnacHeader->wFlags, info);
            if (!(pSnacHeader->wFlags & 0x0001))
            {
                // was that last packet ?
                ReleaseCookie(pSnacHeader->dwRef); // yes, release cookie
            }
        }
        else
        {
            // use old fake
            if (!blWork)
            {
                // this can fail on some crazy situations
                ResetSettingsOnListReload();
            }
            handleServerCList(pBuffer, wBufferLength, pSnacHeader->wFlags, info);
        }
        break;
    }

    case ICQ_LISTS_UPTODATE: // SRV_REPLYROSTEROK
    {
        servlistcookie* sc;

        bIsSyncingCL = FALSE;

        if (FindCookie(pSnacHeader->dwRef, NULL, (void**)&sc))
        {
            // we requested servlist check
#ifdef _DEBUG
            NetLog_Server("Server stated roster is ok.");
#endif
            ReleaseCookie(pSnacHeader->dwRef);
            LoadServerIDs();
        }
        else
            NetLog_Server("Server sent unexpected SNAC(x13,x0F) - SRV_REPLYROSTEROK");

        // This will activate the server side list
        sendRosterAck(); // this must be here, cause of failures during cookie alloc
        handleServUINSettings(wListenPort, info);
        break;
    }

    case ICQ_LISTS_CLI_MODIFYSTART:
        NetLog_Server("Server sent SNAC(x13,x%02x) - %s", 0x11, "Server is modifying contact list");
        break;

    case ICQ_LISTS_CLI_MODIFYEND:
        NetLog_Server("Server sent SNAC(x13,x%02x) - %s", 0x12, "End of server modification");
        break;

    case ICQ_LISTS_UPDATEGROUP:
        if (wBufferLength >= 10)
        {
            WORD wGroupId, wItemId, wItemType, wTlvLen;
            uid_str szUID;

            if (unpackServerListItem(&pBuffer, &wBufferLength, szUID, &wGroupId, &wItemId, &wItemType, &wTlvLen))
            {
                HANDLE hContact = HContactFromRecordName(szUID, NULL);

                if (wBufferLength >= wTlvLen && hContact != INVALID_HANDLE_VALUE && wItemType == SSI_ITEM_BUDDY)
                {
                    // a contact was updated on server
                    if (wTlvLen > 0)
                    {
                        // parse details
                        oscar_tlv_chain *pChain = readIntoTLVChain(&pBuffer, (WORD)(wTlvLen), 0);

                        if (pChain)
                        {
                            oscar_tlv* pAuth = getTLV(pChain, SSI_TLV_AWAITING_AUTH, 1);
                            BYTE bAuth = getSettingByte(hContact, "Auth", 0);

                            if (bAuth && !pAuth)
                            {
                                // server authorized our contact
                                char str[MAX_PATH];
                                char msg[MAX_PATH];
                                char *nick = NickFromHandleUtf(hContact);

                                setSettingByte(hContact, "Auth", 0);
                                null_snprintf(str, MAX_PATH, ICQTranslateUtfStatic("Contact \"%s\" was authorized in the server list.", msg, MAX_PATH), nick);
                                icq_LogMessage(LOG_WARNING, str);
                                mir_free(nick);
                            }
                            else if (!bAuth && pAuth)
                            {
                                // server took away authorization of our contact
                                char str[MAX_PATH];
                                char msg[MAX_PATH];
                                char *nick = NickFromHandleUtf(hContact);

                                setSettingByte(hContact, "Auth", 1);
                                null_snprintf(str, MAX_PATH, ICQTranslateUtfStatic("Contact \"%s\" lost its authorization in the server list.", msg, MAX_PATH), nick);
                                icq_LogMessage(LOG_WARNING, str);
                                mir_free(nick);
                            }

                            {
                                // update server's data - otherwise consequent operations can fail with 0x0E
                                unsigned char* data = (unsigned char*)icq_alloc_zero(wTlvLen);
                                int datalen = getServerDataFromItemTLV(pChain, data);

                                if (datalen > 0)
                                    setSettingBlob(hContact, "ServerData", data, datalen);
                                else
                                    deleteSetting(hContact, "ServerData");
                            }

                            disposeChain(&pChain);

                            break;
                        }
                    }
                }
            }
        }
        NetLog_Server("Server sent SNAC(x13,x%02x) - %s", 0x09, "Server updated our contact on list");
        break;

    case ICQ_LISTS_REMOVEFROMLIST:
        if (wBufferLength >= 10)
        {
            WORD wGroupId, wItemId, wItemType;
            uid_str szUID;

            if (unpackServerListItem(&pBuffer, &wBufferLength, szUID, &wGroupId, &wItemId, &wItemType, NULL))
            {
                HANDLE hContact = HContactFromRecordName(szUID, NULL);

                if (hContact != INVALID_HANDLE_VALUE && wItemType == SSI_ITEM_BUDDY)
                {
                    // a contact was removed from our list
                    deleteSetting(hContact, "ServerId");
                    deleteSetting(hContact, "SrvGroupId");
                    deleteSetting(hContact, "Auth");
                    icq_sendNewContact(0, szUID); // add to CS to see him
                    {
                        CHECKCONTACT chk = {0};
                        chk.hContact=hContact;
                        chk.popup=chk.historyevent=chk.logtofile=TRUE;
                        chk.icqeventtype=ICQEVENTTYPE_SELF_REMOVE;
                        chk.popuptype=POPTYPE_SELFREMOVE;
                        chk.msg="removed himself from your Serverlist!";
                        chk.dbeventflag=DBEF_READ;
                        CheckContact(chk);
                    }
                }
            }
        }
        NetLog_Server("Server sent SNAC(x13,x%02x) - %s", 0x0A, "Server removed something from our list");
        break;

    case ICQ_LISTS_ADDTOLIST:
        if (wBufferLength >= 10)
        {
            WORD wGroupId, wItemId, wItemType, wTlvLen;

            if (unpackServerListItem(&pBuffer, &wBufferLength, NULL, &wGroupId, &wItemId, &wItemType, &wTlvLen))
            {
                if (wBufferLength >= wTlvLen && wItemType == SSI_ITEM_IMPORTTIME)
                {
                    if (wTlvLen > 0)
                    {
                        // parse timestamp
                        oscar_tlv_chain *pChain = readIntoTLVChain(&pBuffer, (WORD)(wTlvLen), 0);

                        if (pChain)
                        {
                            setSettingDword(NULL, "ImportTS", getDWordFromChain(pChain, SSI_TLV_TIMESTAMP, 1));
                            setSettingWord(NULL, "SrvImportID", wItemId);
                            disposeChain(&pChain);

                            NetLog_Server("Server added Import timestamp to list");

                            break;
                        }
                    }
                }
            }
        }
        NetLog_Server("Server sent SNAC(x13,x%02x) - %s", 0x08, "Server added something to our list");
        break;

    case ICQ_LISTS_AUTHREQUEST:
        handleRecvAuthRequest(pBuffer, wBufferLength);
        break;

    case ICQ_LISTS_SRV_AUTHRESPONSE:
        handleRecvAuthResponse(pBuffer, wBufferLength);
        break;

    case ICQ_LISTS_AUTHGRANTED:
        NetLog_Server("Server sent SNAC(x13,x%02x) - %s", 0x15, "User granted us future authorization");
        break;

    case ICQ_LISTS_YOUWEREADDED:
        handleRecvAdded(pBuffer, wBufferLength);
        break;

    case ICQ_LISTS_ERROR:
        if (wBufferLength >= 2)
        {
            WORD wError;
            servlistcookie* sc;

            unpackWord(&pBuffer, &wError);

            if (FindCookie(pSnacHeader->dwRef, NULL, (void**)&sc))
            {
                // look for action cookie
#ifdef _DEBUG
                NetLog_Server("Received server list error, action: %d, result: %d", sc->dwAction, wError);
#endif
                FreeCookie(pSnacHeader->dwRef); // release cookie

                if (sc->dwAction==SSA_CHECK_ROSTER)
                {
                    // the serv-list is unavailable turn it off
                    icq_LogMessage(LOG_ERROR, "Server contact list is unavailable, Miranda will use local contact list.");
                    m_bSsiEnabled = 0;
                    handleServUINSettings(wListenPort, info);
                }
                mir_free(sc);
            }
            else
            {
                LogFamilyError(ICQ_LISTS_FAMILY, wError);
            }
        }
        break;

    default:
        NetLog_Server("Warning: Ignoring SNAC(x%02x,x%02x) - Unknown SNAC (Flags: %u, Ref: %u)", ICQ_LISTS_FAMILY, pSnacHeader->wSubtype, pSnacHeader->wFlags, pSnacHeader->dwRef);
        break;
    }
}



static int unpackServerListItem(unsigned char** pbuf, WORD* pwLen, char* pszRecordName, WORD* pwGroupId, WORD* pwItemId, WORD* pwItemType, WORD* pwTlvLength)
{
    WORD wRecordNameLen;

    // The name of the entry. If this is a group header, then this
    // is the name of the group. If it is a plain contact list entry,
    // then it's the UIN of the contact.
    unpackWord(pbuf, &wRecordNameLen);
    if (*pwLen < 10 + wRecordNameLen || wRecordNameLen >= MAX_PATH)
        return 0; // Failure

    unpackString(pbuf, pszRecordName, wRecordNameLen);
    if (pszRecordName)
        pszRecordName[wRecordNameLen] = '\0';

    // The group identifier this entry belongs to. If 0, this is meta information or
    // a contact without a group
    unpackWord(pbuf, pwGroupId);

    // The ID of this entry. Group headers have ID 0. Otherwise, this
    // is a random number generated when the user is added to the
    // contact list, or when the user is ignored. See CLI_ADDBUDDY.
    unpackWord(pbuf, pwItemId);

    // This field indicates what type of entry this is
    unpackWord(pbuf, pwItemType);

    // The length in bytes of the following TLV chain
    unpackWord(pbuf, pwTlvLength);

    *pwLen -= wRecordNameLen + 10;

    return 1; // Success
}



static DWORD updateServerGroupData(WORD wGroupId, void *groupData, int groupSize)
{
    DWORD dwCookie;
    servlistcookie* ack = NULL;

    ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));
    if (!ack)
    {
        NetLog_Server("Updating of group on server list failed (malloc error)");
        return 0;
    }
    ack->dwAction = SSA_GROUP_UPDATE;
    ack->szGroupName = getServerGroupNameUtf(wGroupId);
    ack->wGroupId = wGroupId;
    dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_UPDATEGROUP, 0, ack);

    return icq_sendGroupUtf(dwCookie, ICQ_LISTS_UPDATEGROUP, ack->wGroupId, ack->szGroupName, groupData, groupSize);
}



static void handleServerCListAck(servlistcookie* sc, WORD wError)
{
    switch (sc->dwAction)
    {
    case SSA_VISIBILITY:
    {
        if (wError)
            NetLog_Server("Server visibility update failed, error %d", wError);
        break;
    }
    case SSA_CONTACT_UPDATE:
    {
        RemovePendingOperation(sc->hContact, 1);
        if (wError)
        {
            NetLog_Server("Updating of server contact failed, error %d", wError);
            icq_LogMessage(LOG_WARNING, "Updating of server contact failed.");
        }
        break;
    }
    case SSA_IGNORE_ADD:
    {
        if (wError)
        {
            NetLog_Server("Ignoring contact failed, error %d, contact %d", wError, sc->hContact);
            icq_LogMessage(LOG_WARNING, LPGEN("Updating of server contact failed."));
        }
        else
        {
            // Set contact server-ignore bit
            setSettingByte(sc->hContact, "SrvIgnore", 1);
            setSettingDword(sc->hContact, "SrvIgnoreTS", (DWORD)sc->lParam);
        }
        break;
    }
    case SSA_IGNORE_REMOVE:
    {
        if (wError)
        {
            NetLog_Server("Ignoring contact failed, error %d, contact %d", wError, sc->hContact);
            icq_LogMessage(LOG_WARNING, LPGEN("Updating of server contact failed."));
        }
        else
        {
            // set contact server-ignore bit
            setSettingByte (sc->hContact, "SrvIgnore", 0);
            deleteSetting(sc->hContact, "SrvIgnoreTS");
            // releasing unused server id
            FreeServerID(sc->wContactId, SSIT_ITEM);
            // and forgiving him
            deleteSetting(sc->hContact, "SrvIgnoreId");
        }
        break;
    }
    case SSA_PRIVACY_ADD:
    {
        if (wError)
        {
            NetLog_Server("Adding of privacy item to server list failed, error %d", wError);
            icq_LogMessage(LOG_WARNING, "Adding of privacy item to server list failed.");
        }
        break;
    }
    case SSA_PRIVACY_REMOVE:
    {
        if (wError)
        {
            NetLog_Server("Removing of privacy item from server list failed, error %d", wError);
            icq_LogMessage(LOG_WARNING, "Removing of privacy item from server list failed.");
        }
        FreeServerID(sc->wContactId, SSIT_ITEM); // release server id
        break;
    }
    case SSA_CONTACT_ADD:
    {
        if (wError)
        {
            if (wError == 0xE) // server refused to add contact w/o auth, add with
            {
                DWORD dwCookie;

                NetLog_Server("Contact could not be added without authorization, add with await auth flag.");

                setSettingByte(sc->hContact, "Auth", 1); // we need auth
                dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_ADDTOLIST, sc->hContact, sc);
                icq_sendServerContact(sc->hContact, dwCookie, ICQ_LISTS_ADDTOLIST, sc->wGroupId, sc->wContactId);

                sc = NULL; // we do not want it to be freed now
                break;
            }
            FreeServerID(sc->wContactId, SSIT_ITEM);
            sendAddEnd(); // end server modifications here
            RemovePendingOperation(sc->hContact, 0);

            NetLog_Server("Adding of contact to server list failed, error %d", wError);
            icq_LogMessage(LOG_WARNING, "Adding of contact to server list failed.");
        }
        else
        {
            void* groupData;
            int groupSize;
            HANDLE hPend = sc->hContact;

            setSettingWord(sc->hContact, "ServerId", sc->wContactId);
            setSettingWord(sc->hContact, "SrvGroupId", sc->wGroupId);

            if (groupData = collectBuddyGroup(sc->wGroupId, &groupSize))
            {
                // the group is not empty, just update it
                updateServerGroupData(sc->wGroupId, groupData, groupSize);
                mir_free(groupData);
            }
            else
            {
                // this should never happen
                NetLog_Server("Group update failed.");
            }
            sendAddEnd(); // end server modifications here

            if (hPend) RemovePendingOperation(hPend, 1);
        }
        break;
    }
    case SSA_GROUP_ADD:
    {
        if (wError)
        {
            FreeServerID(sc->wGroupId, SSIT_GROUP);
            NetLog_Server("Adding of group to server list failed, error %d", wError);
            icq_LogMessage(LOG_WARNING, "Adding of group to server list failed.");
        }
        else // group added, we need to update master group
        {
            void* groupData;
            int groupSize;
            servlistcookie* ack = NULL;
            DWORD dwCookie;

            setServerGroupNameUtf(sc->wGroupId, sc->szGroupName); // add group to namelist
            setServerGroupIDUtf(makeGroupPathUtf(sc->wGroupId), sc->wGroupId); // add group to known

            groupData = collectGroups(&groupSize);
            groupData = mir_realloc(groupData, groupSize+2);
            *(((WORD*)groupData)+(groupSize>>1)) = sc->wGroupId; // add this new group id
            groupSize += 2;

            ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));
            if (ack)
            {
                ack->dwAction = SSA_GROUP_UPDATE;
                dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_UPDATEGROUP, 0, ack);

                icq_sendGroupUtf(dwCookie, ICQ_LISTS_UPDATEGROUP, 0, ack->szGroupName, groupData, groupSize);
            }
            sendAddEnd(); // end server modifications here

            mir_free(groupData);

            if (sc->ofCallback) // is add contact pending
            {
                sc->ofCallback(sc->wGroupId, (LPARAM)sc->lParam);
                // sc = NULL; // we do not want to be freed here
            }
        }
        mir_free(sc->szGroupName);
        break;
    }
    case SSA_CONTACT_REMOVE:
    {
        if (!wError)
        {
            void* groupData;
            int groupSize;

            setSettingWord(sc->hContact, "ServerId", 0); // clear the values
            setSettingWord(sc->hContact, "SrvGroupId", 0);

            FreeServerID(sc->wContactId, SSIT_ITEM);

            if (groupData = collectBuddyGroup(sc->wGroupId, &groupSize))
            {
                // the group is still not empty, just update it
                updateServerGroupData(sc->wGroupId, groupData, groupSize);

                sendAddEnd(); // end server modifications here
            }
            else // the group is empty, delete it if it does not have sub-groups
            {
                DWORD dwCookie;

                if (!CheckServerID((WORD)(sc->wGroupId+1), 0) || countGroupLevel((WORD)(sc->wGroupId+1)) == 0)
                {
                    // is next id an sub-group, if yes, we cannot delete this group
                    sc->dwAction = SSA_GROUP_REMOVE;
                    sc->wContactId = 0;
                    sc->hContact = NULL;
                    sc->szGroupName = getServerGroupNameUtf(sc->wGroupId);
                    dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_REMOVEFROMLIST, 0, sc);

                    icq_sendGroupUtf(dwCookie, ICQ_LISTS_REMOVEFROMLIST, sc->wGroupId, sc->szGroupName, NULL, 0);
                    // here the modifications go on
                    sc = NULL; // we do not want it to be freed now
                }
            }
            mir_free(groupData); // free the memory
        }
        else
        {
            NetLog_Server("Removing of contact from server list failed, error %d", wError);
            icq_LogMessage(LOG_WARNING, "Removing of contact from server list failed.");
            sendAddEnd(); // end server modifications here
        }
        break;
    }
    case SSA_GROUP_UPDATE:
    {
        if (wError)
        {
            NetLog_Server("Updating of group on server list failed, error %d", wError);
            icq_LogMessage(LOG_WARNING, "Updating of group on server list failed.");
        }
        mir_free(sc->szGroupName);
        break;
    }
    case SSA_GROUP_REMOVE:
    {
        mir_free(sc->szGroupName);
        if (wError)
        {
            NetLog_Server("Removing of group from server list failed, error %d", wError);
            icq_LogMessage(LOG_WARNING, "Removing of group from server list failed.");
        }
        else // group removed, we need to update master group
        {
            void* groupData;
            int groupSize;
            DWORD dwCookie;

            setServerGroupNameUtf(sc->wGroupId, NULL); // clear group from namelist
            FreeServerID(sc->wGroupId, SSIT_GROUP);
            removeGroupPathLinks(sc->wGroupId);

            groupData = collectGroups(&groupSize);
            sc->wGroupId = 0;
            sc->dwAction = SSA_GROUP_UPDATE;
            sc->szGroupName = NULL;
            dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_UPDATEGROUP, 0, sc);

            icq_sendGroupUtf(dwCookie, ICQ_LISTS_UPDATEGROUP, 0, sc->szGroupName, groupData, groupSize);
            sendAddEnd(); // end server modifications here

            sc = NULL; // we do not want to be freed here

            mir_free(groupData);
        }
        break;
    }
    case SSA_CONTACT_SET_GROUP:
    {
        // we moved contact to another group
        if (sc->lParam == -1)
        {
            // the first was an error
            break;
        }
        if (wError)
        {
            if (wError == 0x0E && sc->lParam == 1)
            {
                // second ack - adding failed with error 0x0E, try to add with AVAIT_AUTH flag
                DWORD dwCookie;

                if (!getSettingByte(sc->hContact, "Auth", 0))
                {
                    // we tried without AWAIT_AUTH, try again with it
                    NetLog_Server("Contact could not be added without authorization, add with await auth flag.");

                    setSettingByte(sc->hContact, "Auth", 1); // we need auth
                }
                else
                {
                    // we tried with AWAIT_AUTH, try again without
                    NetLog_Server("Contact count not be added awaiting authorization, try authorized.");

                    setSettingByte(sc->hContact, "Auth", 0);
                }
                dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_ADDTOLIST, sc->hContact, sc);
                icq_sendServerContact(sc->hContact, dwCookie, ICQ_LISTS_ADDTOLIST, sc->wNewGroupId, sc->wNewContactId);

                sc->lParam = 2; // do not cycle
                sc = NULL; // we do not want to be freed here
                break;
            }
            RemovePendingOperation(sc->hContact, 0);
            NetLog_Server("Moving of user to another group on server list failed, error %d", wError);
            icq_LogMessage(LOG_ERROR, "Moving of user to another group on server list failed.");
            if (!sc->lParam) // is this first ack ?
            {
                sc->lParam = -1;
                sc = NULL; // this can't be freed here
            }
            break;
        }
        if (sc->lParam) // is this the second ack ?
        {
            void* groupData;
            int groupSize;
            int bEnd = 1; // shall we end the sever modifications

            setSettingWord(sc->hContact, "ServerId", sc->wNewContactId);
            setSettingWord(sc->hContact, "SrvGroupId", sc->wNewGroupId);
            FreeServerID(sc->wContactId, SSIT_ITEM); // release old contact id

            if (groupData = collectBuddyGroup(sc->wGroupId, &groupSize)) // update the group we moved from
            {
                // the group is still not empty, just update it
                updateServerGroupData(sc->wGroupId, groupData, groupSize);
                mir_free(groupData); // free the memory
            }
            else if (!CheckServerID((WORD)(sc->wGroupId+1), 0) || countGroupLevel((WORD)(sc->wGroupId+1)) == 0)
            {
                // the group is empty and is not followed by sub-groups, delete it
                DWORD dwCookie;
                servlistcookie* ack = NULL;

                ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));
                if (!ack)
                {
                    NetLog_Server("Updating of group on server list failed (malloc error)");
                    break;
                }
                ack->dwAction = SSA_GROUP_REMOVE;
                ack->szGroupName = getServerGroupNameUtf(sc->wGroupId);
                ack->wGroupId = sc->wGroupId;
                dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_REMOVEFROMLIST, 0, ack);

                icq_sendGroupUtf(dwCookie, ICQ_LISTS_REMOVEFROMLIST, ack->wGroupId, ack->szGroupName, NULL, 0);
                bEnd = 0; // here the modifications go on
            }

            groupData = collectBuddyGroup(sc->wNewGroupId, &groupSize); // update the group we moved to
            updateServerGroupData(sc->wNewGroupId, groupData, groupSize);
            mir_free(groupData);

            if (bEnd) sendAddEnd();
            if (sc->hContact) RemovePendingOperation(sc->hContact, 1);
        }
        else // contact was deleted from server-list
        {
            deleteSetting(sc->hContact, "ServerId");
            deleteSetting(sc->hContact, "SrvGroupId");
            sc->lParam = 1;
            sc = NULL; // wait for second ack
        }
        break;
    }
    case SSA_CONTACT_FIX_AUTH:
    {
        if (wError)
        {
            // FIXME: something failed, we should handle it properly
        }
        break;
    }
    case SSA_GROUP_RENAME:
    {
        if (wError)
        {
            NetLog_Server("Renaming of server group failed, error %d", wError);
            icq_LogMessage(LOG_WARNING, "Renaming of server group failed.");
        }
        else
        {
            setServerGroupNameUtf(sc->wGroupId, sc->szGroupName);
            removeGroupPathLinks(sc->wGroupId);
            setServerGroupIDUtf(makeGroupPathUtf(sc->wGroupId), sc->wGroupId);
        }
        RemoveGroupRename(sc->wGroupId);
        mir_free(sc->szGroupName);
        break;
    }
    case SSA_SETAVATAR:
    {
        if (wError)
        {
            NetLog_Server("Uploading of avatar hash failed.");
            if (sc->wGroupId) // is avatar added or updated?
            {
                FreeServerID(sc->wContactId, SSIT_ITEM);
                deleteSetting(NULL, "SrvAvatarID"); // to fix old versions
            }
        }
        else
        {
            setSettingWord(NULL, "SrvAvatarID", sc->wContactId);
        }
        break;
    }
    case SSA_REMOVEAVATAR:
    {
        if (wError)
            NetLog_Server("Removing of avatar hash failed.");
        else
        {
            deleteSetting(NULL, "SrvAvatarID");
            FreeServerID(sc->wContactId, SSIT_ITEM);
        }
        break;
    }
    case SSA_SERVLIST_ACK:
    {
        BroadcastAck(sc->hContact, ICQACKTYPE_SERVERCLIST, wError?ACKRESULT_FAILED:ACKRESULT_SUCCESS, (HANDLE)sc->lParam, wError);
        break;
    }
    case SSA_IMPORT:
    {
        if (wError)
            NetLog_Server("Re-starting import sequence failed, error %d", wError);
        else
        {
            setSettingWord(NULL, "SrvImportID", 0);
            deleteSetting(NULL, "ImportTS");
        }
        break;
    }
    default:
        NetLog_Server("Server ack cookie type (%d) not recognized.", sc->dwAction);
    }
    mir_free(sc); // free the memory

    return;
}



static HANDLE HContactFromRecordName(char* szRecordName, int *bAdded)
{
    HANDLE hContact = INVALID_HANDLE_VALUE;

    if (!IsStringUIN(szRecordName))
    {
        // probably AIM contact
        hContact = HContactFromUID(0, szRecordName, bAdded);
    }
    else
    {
        // this should be ICQ number
        DWORD dwUin;

        dwUin = atoi(szRecordName);
        hContact = HContactFromUIN(dwUin, bAdded);
    }
    return hContact;
}



static int getServerDataFromItemTLV(oscar_tlv_chain* pChain, unsigned char *buf)
{
    // get server-list item's TLV data
    oscar_tlv_chain* list = pChain;
    int datalen = 0;
    icq_packet pBuf;

    // Initialize our handy data buffer
    pBuf.wPlace = 0;
    pBuf.pData = buf;

    while (list)
    {
        // collect non-standard TLVs and save them to DB
        if (list->tlv.wType != SSI_TLV_AWAITING_AUTH &&
                list->tlv.wType != SSI_TLV_NAME &&
                list->tlv.wType != SSI_TLV_COMMENT)
        {
            // only TLVs which we do not handle on our own
            packTLV(&pBuf, list->tlv.wType, list->tlv.wLen, (char*)list->tlv.pData);

            datalen += list->tlv.wLen + 4;
        }
        list = list->next;
    }
    return datalen;
}


static void handleServerCList(unsigned char *buf, WORD wLen, WORD wFlags, serverthread_info *info)
{
    BYTE bySSIVersion;
    WORD wRecordCount;
    WORD wRecord;
    WORD wGroupId;
    WORD wItemId;
    WORD wTlvType;
    WORD wTlvLength;
    BOOL bIsLastPacket;
    uid_str szRecordName;
    oscar_tlv_chain* pChain = NULL;
    oscar_tlv* pTLV = NULL;
    BOOL AddClist = (BOOL)getSettingByte(NULL, "ServerAddClist", 0);


    // If flag bit 1 is set, this is not the last
    // packet. If it is 0, this is the last packet
    // and there will be a timestamp at the end.
    if (wFlags & 0x0001)
        bIsLastPacket = FALSE;
    else
        bIsLastPacket = TRUE;

    if (wLen < 3)
        return;

    // Version number of SSI protocol?
    unpackByte(&buf, &bySSIVersion);
    wLen -= 1;

    // Total count of following entries. This is the size of the server
    // side contact list and should be saved and sent with CLI_CHECKROSTER.
    // NOTE: When the entries are split up in several packets, each packet
    // has it's own count and they must be added to get the total size of
    // server list.
    unpackWord(&buf, &wRecordCount);
    wLen -= 2;
    NetLog_Server("SSI: number of entries is %u, version is %u", wRecordCount, bySSIVersion);


    // Loop over all items in the packet
    for (wRecord = 0; wRecord < wRecordCount; wRecord++)
    {
        NetLog_Server("SSI: parsing record %u", wRecord + 1);

        if (wLen < 10)
        {
            // minimum: name length (zero), group ID, item ID, empty TLV
            NetLog_Server("Warning: SSI parsing error (%d)", 0);
            break;
        }

        if (!unpackServerListItem(&buf, &wLen, szRecordName, &wGroupId, &wItemId, &wTlvType, &wTlvLength))
        {
            // unpack basic structure
            NetLog_Server("Warning: SSI parsing error (%d)", 1);
            break;
        }

        NetLog_Server("Name: '%s', GroupID: %u, EntryID: %u, EntryType: %u, TLVlength: %u",
                      szRecordName, wGroupId, wItemId, wTlvType, wTlvLength);

        if (wLen < wTlvLength)
        {
            NetLog_Server("Warning: SSI parsing error (%d)", 2);
            break;
        }

        // Initialize the tlv chain
        if (wTlvLength > 0)
        {
            pChain = readIntoTLVChain(&buf, wTlvLength, 0);
            wLen -= wTlvLength;
        }
        else
        {
            pChain = NULL;
        }


        switch (wTlvType)
        {

        case SSI_ITEM_BUDDY:
        {
            /* this is a contact */
            HANDLE hContact;
            int bAdded;

            hContact = HContactFromRecordName(szRecordName, &bAdded);

            if (hContact != INVALID_HANDLE_VALUE)
            {
                int bRegroup = 0;
                int bNicked = 0;
                WORD wOldGroupId;

                if (bAdded)
                {
                    // Not already on list: added
                    char* szGroup;

                    NetLog_Server("SSI added new %s contact '%s'", "ICQ", szRecordName);

                    if (szGroup = makeGroupPathUtf(wGroupId))
                    {
                        // try to get Miranda Group path from groupid, if succeeded save to db
                        setSettingStringUtf(hContact, "CList", "Group", szGroup);

                        mir_free(szGroup);
                    }
                    AddJustAddedContact(hContact);
                }
                else
                {
                    // we should add new contacts and this contact was just added, show it
                    if (IsContactJustAdded(hContact))
                    {
                        setContactHidden(hContact, 0);
                        bAdded = 1; // we want details for new contacts
                    }
                    else
                        NetLog_Server("SSI ignoring existing contact '%s'", szRecordName);
                    // Contact on server is always on list
                    if(AddClist)
                        DBWriteContactSettingByte(hContact, "CList", "NotOnList", 0);
                }

                wOldGroupId = getSettingWord(hContact, "SrvGroupId", 0);
                // Save group and item ID
                setSettingWord(hContact, "ServerId", wItemId);
                setSettingWord(hContact, "SrvGroupId", wGroupId);
                ReserveServerID(wItemId, SSIT_ITEM, 0);

                if (!bAdded && (wOldGroupId != wGroupId) && getSettingByte(NULL, "LoadServerDetails", DEFAULT_SS_LOAD))
                {
                    // contact has been moved on the server
                    char* szOldGroup = getServerGroupNameUtf(wOldGroupId);
                    char* szGroup = getServerGroupNameUtf(wGroupId);

                    if (!szOldGroup)
                    {
                        // old group is not known, most probably not created subgroup
                        char* szTmp = getSettingStringUtf(hContact, "CList", "Group", "");

                        if (strlennull(szTmp))
                        {
                            // got group from CList
                            mir_free(szOldGroup);
                            szOldGroup = szTmp;
                        }
                        else
                            mir_free(szTmp);

                        if (!szOldGroup) szOldGroup = null_strdup(DEFAULT_SS_GROUP);
                    }

                    if (!szGroup || strlennull(szGroup)>=strlennull(szOldGroup) || strnicmp(szGroup, szOldGroup, strlennull(szGroup)))
                    {
                        // contact moved to new group or sub-group or not to master group
                        bRegroup = 1;
                    }
                    if (bRegroup && !stricmp(DEFAULT_SS_GROUP, szGroup) && !GroupNameExistsUtf(szGroup, -1))
                    {
                        // is it the default "General" group ? yes, does it exists in CL ?
                        bRegroup = 0; // if no, do not move to it - cause it would hide the contact
                    }
                    mir_free(szGroup);
                    mir_free(szOldGroup);
                }

                if (bRegroup || bAdded)
                {
                    // if we should load server details or contact was just added, update its group
                    char* szGroup;

                    if (szGroup = makeGroupPathUtf(wGroupId))
                    {
                        // try to get Miranda Group path from groupid, if succeeded save to db
                        setSettingStringUtf(hContact, "CList", "Group", szGroup);

                        mir_free(szGroup);
                    }
                }

                if (pChain)
                {
                    // Look for nickname TLV and copy it to the db if necessary
                    if (pTLV = getTLV(pChain, SSI_TLV_NAME, 1))
                    {
                        if (pTLV->pData && (pTLV->wLen > 0))
                        {
                            char* pszNick;
                            WORD wNickLength;

                            wNickLength = pTLV->wLen;

                            pszNick = (char*)icq_alloc_zero(wNickLength + 1);
                            // Copy buffer to utf-8 buffer
                            memcpy(pszNick, pTLV->pData, wNickLength);
                            pszNick[wNickLength] = 0; // Terminate string

                            NetLog_Server("Nickname is '%s'", pszNick);

                            bNicked = 1;

                            // Write nickname to database
                            if (getSettingByte(NULL, "LoadServerDetails", DEFAULT_SS_LOAD) || bAdded)
                            {
                                // if just added contact, save details always - does no harm
                                char *szOldNick;

                                if (szOldNick = getSettingStringUtf(hContact,"CList","MyHandle",""))
                                {
                                    if ((strcmpnull(szOldNick, pszNick)) && (strlennull(pszNick) > 0))
                                    {
                                        // check if the truncated nick changed, i.e. do not overwrite locally stored longer nick
                                        if (strlennull(szOldNick) <= strlennull(pszNick) || strncmp(szOldNick, pszNick, null_strcut(szOldNick, MAX_SSI_TLV_NAME_SIZE)))
                                        {
                                            // Yes, we really do need to delete it first. Otherwise the CLUI nick
                                            // cache isn't updated (I'll look into it)
                                            DBDeleteContactSetting(hContact,"CList","MyHandle");
                                            setSettingStringUtf(hContact, "CList", "MyHandle", pszNick);
                                        }
                                    }
                                    mir_free(szOldNick);
                                }
                                else if (strlennull(pszNick) > 0)
                                {
                                    DBDeleteContactSetting(hContact,"CList","MyHandle");
                                    setSettingStringUtf(hContact, "CList", "MyHandle", pszNick);
                                }
                            }
                            mir_free(pszNick);
                        }
                        else
                        {
                            NetLog_Server("Invalid nickname");
                        }
                    }
                    if (bAdded && !bNicked)
                        icq_QueueUser(hContact); // queue user without nick for fast auto info update

                    // Look for comment TLV and copy it to the db if necessary
                    if (pTLV = getTLV(pChain, SSI_TLV_COMMENT, 1))
                    {
                        if (pTLV->pData && (pTLV->wLen > 0))
                        {
                            char* pszComment;
                            WORD wCommentLength;


                            wCommentLength = pTLV->wLen;

                            pszComment = (char*)icq_alloc_zero(wCommentLength + 1);
                            // Copy buffer to utf-8 buffer
                            memcpy(pszComment, pTLV->pData, wCommentLength);
                            pszComment[wCommentLength] = 0; // Terminate string

                            NetLog_Server("Comment is '%s'", pszComment);

                            // Write comment to database
                            if (getSettingByte(NULL, "LoadServerDetails", DEFAULT_SS_LOAD) || bAdded)
                            {
                                // if just added contact, save details always - does no harm
                                char *szOldComment;

                                if (szOldComment = getSettingStringUtf(hContact,"UserInfo","MyNotes",""))
                                {
                                    if ((strcmpnull(szOldComment, pszComment)) && (strlennull(pszComment) > 0))
                                    {
                                        // check if the truncated comment changed, i.e. do not overwrite locally stored longer comment
                                        if (strlennull(szOldComment) <= strlennull(pszComment) || strncmp(szOldComment, pszComment, null_strcut(szOldComment, MAX_SSI_TLV_COMMENT_SIZE)))
                                        {
                                            setSettingStringUtf(hContact, "UserInfo", "MyNotes", pszComment);
                                        }
                                    }
                                    mir_free(szOldComment);
                                }
                                else if (strlennull(pszComment) > 0)
                                {
                                    setSettingStringUtf(hContact, "UserInfo", "MyNotes", pszComment);
                                }
                            }
                            mir_free(pszComment);
                        }
                        else
                        {
                            NetLog_Server("Invalid comment");
                        }
                    }

                    // Look for need-authorization TLV
                    if (getTLV(pChain, SSI_TLV_AWAITING_AUTH, 1))
                    {
                        setSettingByte(hContact, "Auth", 1);
                        NetLog_Server("SSI contact need authorization");
                    }
                    else
                    {
                        setSettingByte(hContact, "Auth", 0);
                    }

                    {
                        // store server-list item's TLV data
                        unsigned char* data = (unsigned char*)icq_alloc_zero(wTlvLength);
                        int datalen = getServerDataFromItemTLV(pChain, data);

                        if (datalen > 0)
                            setSettingBlob(hContact, "ServerData", data, datalen);
                        else
                            deleteSetting(hContact, "ServerData");

                        mir_free(data);
                    }
                }
            }
            else
            {
                // failed to add or other error
                NetLog_Server("SSI failed to handle %s Item '%s'", "Buddy", szRecordName);
                ReserveServerID(wItemId, SSIT_ITEM, SSIF_UNHANDLED);
            }
        }
        break;

        case SSI_ITEM_GROUP:
            if ((wGroupId == 0) && (wItemId == 0))
            {
                /* list of groups. wTlvType=1, data is TLV(C8) containing list of WORDs which */
                /* is the group ids */
                /* we don't need to use this. Our processing is on-the-fly */
                /* this record is always sent first in the first packet only, */
            }
            else if (wGroupId != 0)
            {
                /* wGroupId != 0: a group record */
                if (wItemId == 0)
                {
                    /* no item ID: this is a group */
                    /* pszRecordName is the name of the group */
                    char* pszName = NULL;

                    ReserveServerID(wGroupId, SSIT_GROUP, 0);

                    setServerGroupNameUtf(wGroupId, szRecordName);

                    NetLog_Server("Group %s added to known groups.", szRecordName);

                    /* demangle full grouppath, create groups, set it to known */
                    pszName = makeGroupPathUtf(wGroupId);
                    mir_free(pszName);

                    /* TLV contains a TLV(C8) with a list of WORDs of contained contact IDs */
                    /* our processing is good enough that we don't need this duplication */
                }
                else
                {
                    NetLog_Server("Unhandled type 0x01, wItemID != 0");
                }
            }
            else
            {
                NetLog_Server("Unhandled type 0x01");
            }
            break;

        case SSI_ITEM_PERMIT:
        {
            /* item on visible list */
            /* wItemId not related to contact ID */
            /* pszRecordName is the UIN */
            HANDLE hContact;
            int bAdded;

            hContact = HContactFromRecordName(szRecordName, &bAdded);

            if (hContact != INVALID_HANDLE_VALUE)
            {
                if (bAdded)
                {
                    NetLog_Server("SSI added new %s contact '%s'", "Permit", szRecordName);
                    // It wasn't previously in the list, we hide it so it only appears in the visible list
                    setContactHidden(hContact, 1);
                    // Add it to the list, so it can be added properly if proper contact
                    AddJustAddedContact(hContact);
                }
                else
                    NetLog_Server("SSI %s contact already exists '%s'", "Permit", szRecordName);

                // Save permit ID
                setSettingWord(hContact, "SrvPermitId", wItemId);
                ReserveServerID(wItemId, SSIT_ITEM, 0);
                // Set apparent mode
                setSettingWord(hContact, "ApparentMode", ID_STATUS_ONLINE);
                NetLog_Server("Visible-contact (%s)", szRecordName);
            }
            else
            {
                // failed to add or other error
                NetLog_Server("SSI failed to handle %s Item '%s'", "Permit", szRecordName);
                ReserveServerID(wItemId, SSIT_ITEM, SSIF_UNHANDLED);
            }
        }
        break;

        case SSI_ITEM_DENY:
        {
            /* Item on invisible list */
            /* wItemId not related to contact ID */
            /* pszRecordName is the UIN */
            HANDLE hContact;
            int bAdded;

            hContact = HContactFromRecordName(szRecordName, &bAdded);

            if (hContact != INVALID_HANDLE_VALUE)
            {
                if (bAdded)
                {
                    /* not already on list: added */
                    NetLog_Server("SSI added new %s contact '%s'", "Deny", szRecordName);
                    ReserveServerID(wItemId, SSIT_ITEM, SSIF_UNHANDLED);
                    // It wasn't previously in the list, we hide it so it only appears in the visible list
                    setContactHidden(hContact, 1);
                    // Add it to the list, so it can be added properly if proper contact
                    AddJustAddedContact(hContact);
                }
                else
                    NetLog_Server("SSI %s contact already exists '%s'", "Deny", szRecordName);

                // Save Deny ID
                setSettingWord(hContact, "SrvDenyId", wItemId);
                ReserveServerID(wItemId, SSIT_ITEM, 0);

                // Set apparent mode
                setSettingWord(hContact, "ApparentMode", ID_STATUS_OFFLINE);
                NetLog_Server("Invisible-contact (%s)", szRecordName);
            }
            else
            {
                // failed to add or other error
                NetLog_Server("SSI failed to handle %s Item '%s'", "Deny", szRecordName);
            }
        }
        break;

        case SSI_ITEM_VISIBILITY: /* My visibility settings */
        {
            BYTE bVisibility;

            // Look for visibility TLV
            if (bVisibility = getByteFromChain(pChain, SSI_TLV_VISIBILITY, 1))
            {
                // found it, store the id, we do not need current visibility - we do not rely on it
                ReserveServerID(wItemId, SSIT_ITEM, 0);
                setSettingWord(NULL, "SrvVisibilityID", wItemId);
                NetLog_Server("Visibility is %u", bVisibility);
            }
            else
                ReserveServerID(wItemId, SSIT_ITEM, SSIF_UNHANDLED);

        }
        break;

        case SSI_ITEM_IGNORE:
        {
            /* item on ignore list */
            /* wItemId not related to contact ID */
            /* pszRecordName is the UIN */
            HANDLE hContact;
            int bAdded;
            oscar_tlv *tschain = &pChain->tlv;
            char szIgnoreDate[255] = "date not saved";


            hContact = HContactFromRecordName(szRecordName, &bAdded);

            if (hContact != INVALID_HANDLE_VALUE)
            {
                if (bAdded)
                {
                    /* not already on list: add */
                    NetLog_Server("SSI added new %s contact '%s'", "Ignore", szRecordName);
                    // It wasn't previously in the list, we hide it
                    setContactHidden(hContact, 1);
                    // Add it to the list, so it can be added properly if proper contact
                    AddJustAddedContact(hContact);
                }
                else
                    NetLog_Server("SSI %s contact already exists '%s'", "Ignore", szRecordName);

                // Save Ignore ID
                setSettingWord(hContact, "SrvIgnoreId", wItemId);
                ReserveServerID(wItemId, SSIT_ITEM, 0);

//          // Set apparent mode & ignore
//          ICQWriteContactSettingWord(hContact, "ApparentMode", ID_STATUS_OFFLINE);
//          // set ignore all events
//          CallService(MS_IGNORE_IGNORE, (WPARAM)hContact, IGNOREEVENT_ALL);
                setSettingByte(hContact, "SrvIgnore", 1);

                if(tschain != NULL && tschain->wLen == 4)
                {
                    DWORD dwIgnoreTS;
                    DBTIMETOSTRING tts;
                    unpackDWord(&(tschain->pData), &dwIgnoreTS);
                    tschain->pData -= 4;
                    setSettingDword(hContact, "SrvIgnoreTS", dwIgnoreTS);

                    tts.szDest = szIgnoreDate;
                    tts.cbDest = 254;
                    tts.szFormat = "D s";
                    CallService(MS_DB_TIME_TIMESTAMPTOSTRING, (WPARAM)dwIgnoreTS, (LPARAM)&tts);
                }
                NetLog_Server("Ignore-contact (%s), added to ignore-list: %s", szRecordName);
            }
            else
            {
                // failed to add or other error
                NetLog_Server("SSI failed to handle %s Item '%s'", "Ignore", szRecordName);
                ReserveServerID(wItemId, SSIT_ITEM, SSIF_UNHANDLED);
            }
        }
        break;

        case SSI_ITEM_UNKNOWN2:
            NetLog_Server("SSI unknown type 0x11");
            ReserveServerID(wItemId, SSIT_ITEM, SSIF_UNHANDLED);
            break;

        case SSI_ITEM_IMPORTTIME:
            if (wGroupId == 0)
            {
                /* time our list was first imported */
                /* pszRecordName is "Import Time" */
                /* data is TLV(13) {TLV(D4) {time_t importTime}} */
                setSettingDword(NULL, "ImportTS", getDWordFromChain(pChain, SSI_TLV_TIMESTAMP, 1));
                setSettingWord(NULL, "SrvImportID", wItemId);
                ReserveServerID(wItemId, SSIT_ITEM, 0);
                NetLog_Server("SSI %s item recognized", "first import");
            }
            break;

        case SSI_ITEM_BUDDYICON:
            if (wGroupId == 0)
            {
                /* our avatar MD5-hash */
                /* pszRecordName is "1" */
                /* data is TLV(D5) hash */
                /* we ignore this, just save the id */
                /* cause we get the hash again after login */
                if (!strcmpnull(szRecordName, "12"))
                {
                    // need to handle Photo Item separately
                    setSettingWord(NULL, "SrvPhotoID", wItemId);
                    NetLog_Server("SSI %s item recognized", "Photo");
                }
                else
                {
                    setSettingWord(NULL, "SrvAvatarID", wItemId);
                    NetLog_Server("SSI %s item recognized", "Avatar");
                }
                ReserveServerID(wItemId, SSIT_ITEM, 0);
            }
            break;

        case SSI_ITEM_UNKNOWN1:
            if (wGroupId == 0)
            {
                /* ICQ2k ShortcutBar Items */
                /* data is TLV(CD) text */
                if (wItemId)
                    ReserveServerID(wItemId, SSIT_ITEM, SSIF_UNHANDLED);
            }

        default:
            if (wItemId)
                ReserveServerID(wItemId, SSIT_ITEM, SSIF_UNHANDLED);
            NetLog_Server("SSI unhandled item %2x", wTlvType);
            break;
        }

        if (pChain)
            disposeChain(&pChain);

    } // end for

    NetLog_Server("Bytes left: %u", wLen);

    setSettingWord(NULL, "SrvRecordCount", (WORD)(wRecord + getSettingWord(NULL, "SrvRecordCount", 0)));

    if (bIsLastPacket)
    {
        // No contacts left to sync
        bIsSyncingCL = FALSE;

        StoreServerIDs();

        icq_RescanInfoUpdate();

        if (wLen >= 4)
        {
            DWORD dwLastUpdateTime;

            /* finally we get a time_t of the last update time */
            unpackDWord(&buf, &dwLastUpdateTime);
            setSettingDword(NULL, "SrvLastUpdate", dwLastUpdateTime);
            NetLog_Server("Last update of server list was (%u) %s", dwLastUpdateTime, time2text(dwLastUpdateTime));
            ReserveServerID(wItemId, SSIT_ITEM, 0);

            sendRosterAck();
            handleServUINSettings(wListenPort, info);
        }
        else
        {
            NetLog_Server("Last packet missed update time...");
        }
        if (getSettingWord(NULL, "SrvRecordCount", 0) == 0)
        {
            // we got empty serv-list, create master group
            servlistcookie* ack = NULL;
            ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));

            if (ack)
            {
                DWORD seq;

                ack->dwAction = SSA_GROUP_UPDATE;
                ack->szGroupName = "";
                seq = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_ADDTOLIST, 0, ack);
                icq_sendGroupUtf(seq, ICQ_LISTS_ADDTOLIST, 0, ack->szGroupName, NULL, 0);
            }
        }
        // serv-list sync finished, clear just added contacts
        FlushJustAddedContacts();
    }
    else
    {
        NetLog_Server("Waiting for more packets");
    }
}



static void handleRecvAuthRequest(unsigned char *buf, WORD wLen)
{
    WORD wReasonLen;
    DWORD dwUin;
    uid_str szUid;
    HANDLE hcontact;
    CCSDATA ccs;
    PROTORECVEVENT pre;
    char* szReason;
    int nReasonLen;
    char* szNick;
    int nNickLen;
    char* szBlob;
    char* pCurBlob;
    DBVARIANT dbv;
    int bAdded;
    if (!bAuthIgnore)
        if (!unpackUID(&buf, &wLen, &dwUin, &szUid)) return;

    if (dwUin && IsOnSpammerList(dwUin))
    {
        NetLog_Server("Ignored Message from known Spammer");
        return;
    }

    unpackWord(&buf, &wReasonLen);
    wLen -= 2;
    if (wReasonLen > wLen)
        return;

    hcontact = HContactFromUID(dwUin, szUid, &bAdded);

    ccs.szProtoService=PSR_AUTH;
    ccs.hContact=hcontact;
    ccs.wParam=0;
    ccs.lParam=(LPARAM)&pre;
    pre.flags=0;
    pre.timestamp=time(NULL);
    pre.lParam=sizeof(DWORD)+sizeof(HANDLE)+wReasonLen+5;
    szReason = (char*)icq_alloc_zero(wReasonLen+1);
    if (szReason)
    {
        memcpy(szReason, buf, wReasonLen);
        szReason[wReasonLen] = '\0';
        szReason = detect_decode_utf8(szReason); // detect & decode UTF-8
    }
    nReasonLen = strlennull(szReason);
    // Read nick name from DB
    if (dwUin)
    {
        if (getSettingString(hcontact, "Nick", &dbv))
            nNickLen = 0;
        else
        {
            szNick = dbv.pszVal;
            nNickLen = strlennull(szNick);
        }
    }
    else
        nNickLen = strlennull(szUid);
    pre.lParam += nNickLen + nReasonLen;

    setSettingByte(ccs.hContact, "Grant", 1);

    /*blob is: uin(DWORD), hcontact(HANDLE), nick(ASCIIZ), first(ASCIIZ), last(ASCIIZ), email(ASCIIZ), reason(ASCIIZ)*/
    pCurBlob=szBlob=(char *)icq_alloc_zero(pre.lParam);
    memcpy(pCurBlob,&dwUin,sizeof(DWORD));
    pCurBlob+=sizeof(DWORD);
    memcpy(pCurBlob,&hcontact,sizeof(HANDLE));
    pCurBlob+=sizeof(HANDLE);
    if (nNickLen && dwUin)
    {
        // if we have nick we add it, otherwise keep trailing zero
        memcpy(pCurBlob, szNick, nNickLen);
        pCurBlob+=nNickLen;
    }
    else
    {
        memcpy(pCurBlob, szUid, nNickLen);
        pCurBlob+=nNickLen;
    }
    *(char *)pCurBlob = 0;
    pCurBlob++;
    *(char *)pCurBlob = 0;
    pCurBlob++;
    *(char *)pCurBlob = 0;
    pCurBlob++;
    *(char *)pCurBlob = 0;
    pCurBlob++;
    if (nReasonLen)
    {
        memcpy(pCurBlob, szReason, nReasonLen);
        pCurBlob += nReasonLen;
    }
    else
    {
        memcpy(pCurBlob, buf, wReasonLen);
        pCurBlob += wReasonLen;
    }
    *(char *)pCurBlob = 0;
    pre.szMessage=(char *)szBlob;

// TODO: Change for new auth system, include all known informations
    CallService(MS_PROTO_CHAINRECV,0,(LPARAM)&ccs);

    mir_free(szReason);
    ICQFreeVariant(&dbv);

    if(!DBGetContactSettingDword(hcontact, "Ignore", "Mask1", 0x0000007F))
        SkinPlaySound("AuthRequest"); // Added by BM
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hcontact;
        chk.dwUin=dwUin;
        chk.PSD=26;
        chk.popup=chk.historyevent=chk.logtofile=TRUE;
        chk.popuptype=POPTYPE_AUTH;
        chk.msg="Authorization requested";
        chk.icqeventtype=ICQEVENTTYPE_AUTH_REQUESTED;
        chk.dbeventflag=DBEF_READ;
        CheckContact(chk);
    }

    return;
}



static void handleRecvAdded(unsigned char *buf, WORD wLen)
{
    DWORD dwUin;
    uid_str szUid;
    DWORD cbBlob;
    PBYTE pBlob,pCurBlob;
    HANDLE hContact;
    int bAdded;
    char* szNick;
    int nNickLen;
    BOOL NotOnList = FALSE;
    DBVARIANT dbv = {0};


    if (!unpackUID(&buf, &wLen, &dwUin, &szUid)) return;


    if (dwUin && IsOnSpammerList(dwUin))
    {
        NetLog_Server("Ignored Message from known Spammer");
        return;
    }


    hContact=HContactFromUID(dwUin, szUid, &bAdded);



    if (DBGetContactSettingByte(hContact, "CList", "NotOnList", 1))
    {
        NotOnList = TRUE;
    }

    if (NotOnList)
    {
        CHECKCONTACT chk = {0};
        chk.hContact=hContact;
        chk.dwUin=dwUin;
        chk.popup=chk.logtofile=chk.historyevent=TRUE;
        chk.popuptype=POPTYPE_AUTH;
        chk.msg="sent you 'You were added' notice";
        chk.icqeventtype=ICQEVENTTYPE_YOU_ADDED;
        chk.dbeventflag=DBEF_READ;
        CheckContact(chk);
    }


    deleteSetting(hContact, "Grant");

    cbBlob=sizeof(DWORD)+sizeof(HANDLE)+4;

    if (dwUin)
    {
        if (getSettingString(hContact, "Nick", &dbv))
            nNickLen = 0;
        else
        {
            szNick = dbv.pszVal;
            nNickLen = strlennull(szNick);
        }
    }
    else
        nNickLen = strlennull(szUid);

    cbBlob += nNickLen;

    pCurBlob=pBlob=(PBYTE)icq_alloc_zero(cbBlob);
    /*blob is: uin(DWORD), hContact(HANDLE), nick(ASCIIZ), first(ASCIIZ), last(ASCIIZ), email(ASCIIZ) */
    memcpy(pCurBlob,&dwUin,sizeof(DWORD));
    pCurBlob+=sizeof(DWORD);
    memcpy(pCurBlob,&hContact,sizeof(HANDLE));
    pCurBlob+=sizeof(HANDLE);
    if (nNickLen && dwUin)
    {
        // if we have nick we add it, otherwise keep trailing zero
        memcpy(pCurBlob, szNick, nNickLen);
        pCurBlob+=nNickLen;
    }
    else
    {
        memcpy(pCurBlob, szUid, nNickLen);
        pCurBlob+=nNickLen;
    }
    *(char *)pCurBlob = 0;
    pCurBlob++;
    *(char *)pCurBlob = 0;
    pCurBlob++;
    *(char *)pCurBlob = 0;
    pCurBlob++;
    *(char *)pCurBlob = 0;
// TODO: Change for new auth system

    AddEvent(NULL, EVENTTYPE_ADDED, time(NULL), 0, cbBlob, pBlob);
    if(bLogAuthHistory)
        HistoryLog(hContact,dwUin, "you added",0,DBEF_READ);
// logtofile(hContact, DWORD dwUin, char *string, int event_type);
    SkinPlaySound("YouWereAdded"); // Added by BM
}



static void handleRecvAuthResponse(unsigned char *buf, WORD wLen)
{
    BYTE bResponse;
    DWORD dwUin;
    uid_str szUid;
    HANDLE hContact;
//  char* szNick;
    WORD nReasonLen;
    char* szReason;
    int bAdded;

//  DBEVENTINFO dbei = {0};
//  PBYTE pCurBlob;
//  char szText[MAX_PATH];
    char szPopup[MAX_PATH];
//  WORD wTextLen;
    BOOL InDB = FALSE;

    bResponse = 0xFF;

    if (!unpackUID(&buf, &wLen, &dwUin, &szUid)) return;

    if (dwUin && IsOnSpammerList(dwUin))
    {
        NetLog_Server("Ignored Message from known Spammer");
        return;
    }

    hContact = HContactFromUID(dwUin, szUid, &bAdded);


    InDB = CallService(MS_DB_CONTACT_IS, (WPARAM)hContact, 0);
    if (hContact != INVALID_HANDLE_VALUE)
    {
        if (wLen > 0)
        {
            unpackByte(&buf, &bResponse);
            wLen -= 1;
        }
        if (wLen >= 2)
        {
            unpackWord(&buf, &nReasonLen);
            wLen -= 2;
            if (wLen >= nReasonLen)
            {
                szReason = (char*)icq_alloc_zero(nReasonLen+1);
                unpackString(&buf, szReason, nReasonLen);
                szReason[nReasonLen] = '\0';
            }
        }
    }
    switch (bResponse)
    {

    case 0:
        SkinPlaySound("AuthDenied"); // Added by BM
#ifdef _DEBUG
        NetLog_Server("Authorization request %s by %s", "denied", strUID(dwUin, szUid));
#endif
        if (!InDB)
        {
            CHECKCONTACT chk = {0};
            chk.hContact=hContact;
            chk.dwUin=dwUin;
            chk.popup=chk.historyevent=chk.logtofile=chk.fornotonlist=TRUE;
            chk.popuptype=POPTYPE_VIS;
            chk.dbeventflag=DBEF_READ;
            chk.icqeventtype=ICQEVENTTYPE_CHECK_STATUS;
            chk.msg="...probably detected your invisibility (auth denied)";
            CheckContact(chk);
        }
        else
        {
            mir_snprintf(szPopup, sizeof(szPopup), ICQTranslateUtf("User \"%s\" was denied your authorization request.(may be checking for invisible status)"), NickFromHandleUtf(hContact));
            if(bLogAuthHistory)
                HistoryLog(hContact,dwUin, "was denied your authorization request", ICQEVENTTYPE_AUTH_DENIED, DBEF_READ);
            else
                HistoryLog(0,0, "was denied your authorization request", ICQEVENTTYPE_AUTH_DENIED, 0);
            LogToFile(hContact, dwUin, 0, ICQEVENTTYPE_AUTH_DENIED);
        }
        break;

    case 1:
        SkinPlaySound("AuthGranted"); // Added by BM
        setSettingByte(hContact, "Auth", 0);
#ifdef _DEBUG
        NetLog_Server("Authorization request %s by %s", "granted", strUID(dwUin, szUid));
#endif
        if (!InDB)
        {
            CHECKCONTACT chk = {0};
            chk.dwUin=dwUin;
            chk.hContact=hContact;
            chk.popup=chk.logtofile=chk.historyevent=chk.fornotonlist=TRUE;
            chk.popuptype=POPTYPE_VIS;
            chk.dbeventflag=DBEF_READ;
            chk.icqeventtype=ICQEVENTTYPE_CHECK_STATUS;
            chk.msg="...probably detected your invisibility (auth granted)";
            CheckContact(chk);
        }
        else
        {
            mir_snprintf(szPopup, sizeof(szPopup), ICQTranslateUtf("User \"%s\" was granted your authorization request.(may be checking for invisible status)"), NickFromHandleUtf(hContact));
            if(bLogAuthHistory)
                HistoryLog(hContact,dwUin, "Authorization request granted", ICQEVENTTYPE_AUTH_GRANTED, DBEF_READ);
            else
                HistoryLog(0,0, "Authorization request granted", ICQEVENTTYPE_AUTH_GRANTED, 0);
            LogToFile(hContact, dwUin, 0, ICQEVENTTYPE_AUTH_GRANTED);
        }
        break;

    default:
        if (!InDB)
        {
            CHECKCONTACT chk = {0};
            chk.dwUin=dwUin;
            chk.hContact=hContact;
            chk.popup=chk.logtofile=chk.historyevent=chk.fornotonlist=TRUE;
            chk.popuptype=POPTYPE_VIS;
            chk.dbeventflag=DBEF_READ;
            chk.icqeventtype=ICQEVENTTYPE_CHECK_STATUS;
            chk.msg="Uknown authorization request response from contact not in your list";
            CheckContact(chk);

        }
        NetLog_Server("Unknown Authorization request response (%u) from %s", bResponse, strUID(dwUin, szUid));
        break;

    }

    {
        CHECKCONTACT chk = {0};
        chk.dwUin=dwUin;
        chk.hContact=hContact;
        chk.PSD=28;
        CheckContact(chk);
    }

}


static void handleRecvRemoved(unsigned char *buf, WORD wLen)
{

    if (wLen >= 10)
    {
        WORD wGroupId, wItemId, wItemType;
        uid_str szUid;
        DWORD dwUin;

        if (!unpackUID(&buf, &wLen, &dwUin, &szUid)) return;

        if (unpackServerListItem(&buf, &wLen, szUid, &wGroupId, &wItemId, &wItemType, NULL))
        {
            HANDLE hContact = HContactFromRecordName(szUid, NULL);

            if (hContact != INVALID_HANDLE_VALUE && wItemType == SSI_ITEM_BUDDY)
                ResetUserSSISettings(hContact);
            {
                // a contact was removed from our list
                deleteSetting(hContact, "ServerId");
                deleteSetting(hContact, "SrvGroupId");
                deleteSetting(hContact, "Auth");
                icq_sendNewContact(0, szUid); // add to CS to see him
                {
                    CHECKCONTACT chk = {0};
                    chk.hContact=hContact;
                    chk.dwUin=dwUin;
                    chk.PSD=29;
                    chk.historyevent=chk.logtofile=TRUE;
                    chk.icqeventtype=ICQEVENTTYPE_SELF_REMOVE;
                    chk.dbeventflag=DBEF_READ;
                    chk.msg="User was removed from server list";
                    CheckContact(chk);
                }
            }
        }
    }
    NetLog_Server("Server sent SNAC(x13,x%02x) - %s", 0x0A, "Server removed something from our list");
}



// Updates the visibility code used while in SSI mode. If a server ID is
// not stored in the local DB, a new ID will be added to the server list.
//
// Possible values are:
//   01 - Allow all users to see you
//   02 - Block all users from seeing you
//   03 - Allow only users in the permit list to see you
//   04 - Block only users in the invisible list from seeing you
//   05 - Allow only users in the buddy list to see you
//	 06 - Allow only users in the Contact list to see you, except Invisible list users

void updateServVisibilityCode(BYTE bCode)
{
    icq_packet packet;
    WORD wVisibilityID;
    WORD wCommand;
    int special = 0;


    servlistcookie* ack = NULL;
    DWORD dwCookie;


    if (gbVisibility)
        bCode = gbVisibility;

    // add by Se7ven
    if (bCode == 6)
    {
        bCode = 3;
        special = 1;
    }

//
    if ((bCode > 0) && (bCode < 6))
    {
// add by Se7ven
        if (special)
            sendVisListServ(1);
//
        if (bVisibility == bCode) // if no change was made, not necescary to update that
            return;
        setSettingByte(NULL, "SrvVisibility", bCode);
        bVisibility = bCode;

        // Do we have a known server visibility ID? We should, unless we just subscribed to the serv-list for the first time
        if ((wVisibilityID = getSettingWord(NULL, "SrvVisibilityID", 0)) == 0)
        {
            // No, create a new random ID
            wVisibilityID = GenerateServerID(SSIT_ITEM, 0, 0);
            setSettingWord(NULL, "SrvVisibilityID", wVisibilityID);
            wCommand = ICQ_LISTS_ADDTOLIST;
            NetLog_Server("Made new srvVisibilityID, id is %u, code is %u", wVisibilityID, bCode);
        }
        else
        {
            NetLog_Server("Reused srvVisibilityID, id is %u, code is %u", wVisibilityID, bCode);
            wCommand = ICQ_LISTS_UPDATEGROUP;
        }

        ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));
        if (!ack)
        {
            NetLog_Server("Cookie alloc failure.");
            return; // out of memory, go away
        }
        ack->dwAction = SSA_VISIBILITY; // update visibility
        dwCookie = AllocateCookie(CKT_SERVERLIST, wCommand, 0, ack); // take cookie

        // Build and send packet
        serverPacketInit(&packet, 25);
        packFNACHeaderFull(&packet, ICQ_LISTS_FAMILY, wCommand, 0, dwCookie);
        packWord(&packet, 0);                   // Name (null)
        packWord(&packet, 0);                   // GroupID (0 if not relevant)
        packWord(&packet, wVisibilityID);       // EntryID
        packWord(&packet, SSI_ITEM_VISIBILITY); // EntryType
        packWord(&packet, 5);                   // Length in bytes of following TLV
        packTLV(&packet, SSI_TLV_VISIBILITY, 1, (char*)&bCode);  // TLV (Visibility)
        sendServPacket(&packet);
        // There is no need to send ICQ_LISTS_CLI_MODIFYSTART or
        // ICQ_LISTS_CLI_MODIFYEND when modifying the visibility code
//
        if (!special)
            sendVisListServ(0);
//
    }
}

void setContactIgnore(HANDLE hContact, BOOL bIgnore)
{
    icq_packet packet;
    servlistcookie* ack = NULL;
    DWORD dwCookie, dwTimestamp;
    WORD wCommand, wIgnoreId;
    char szUin[255];

    if(bIgnore)
        wCommand = ICQ_LISTS_ADDTOLIST;
    else
        wCommand = ICQ_LISTS_REMOVEFROMLIST;

    dwTimestamp = (DWORD)time(NULL);

    ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));
    if (!ack)
    {
        NetLog_Server("Cookie alloc failure.");
        return; // out of memory, go away
    }

    if(bIgnore)
        ack->dwAction = SSA_IGNORE_ADD;
    else
        ack->dwAction = SSA_IGNORE_REMOVE;

    ack->hContact = hContact;
    ack->lParam = (LPARAM)dwTimestamp;
    dwCookie = AllocateCookie(CKT_SERVERLIST, wCommand, hContact, ack); // take cookie

    _ltoa( getContactUin(hContact), szUin, 10);

    if ((wIgnoreId = getSettingWord(hContact, "SrvIgnoreId", 0)) == 0)
    {
        wIgnoreId = GenerateServerID(SSIT_ITEM, 0, 0);
        setSettingWord(hContact, "SrvIgnoreId", wIgnoreId);
    }

    // Build and send packet
    serverPacketInit(&packet, strlen(szUin) + 28);
    packFNACHeaderFull(&packet, ICQ_LISTS_FAMILY, wCommand, 0, dwCookie);
    packWord(&packet, strlen(szUin));       // Name
    packBuffer(&packet, szUin, strlen(szUin));
    packWord(&packet, 0);                   // GroupID (0 if not relevant)
    packWord(&packet, wIgnoreId);       // EntryID
    packWord(&packet, SSI_ITEM_IGNORE); // EntryType
    packWord(&packet, 8);                   // Length in bytes of following TLV
    packWord(&packet, SSI_TLV_IGNORE_TIMESTAMP);	//Timestamp TLV
    packWord(&packet, 4);
    packDWord(&packet, dwTimestamp); // Timestamp

    sendServPacket(&packet);
}



// Updates the avatar hash used while in SSI mode. If a server ID is
// not stored in the local DB, a new ID will be added to the server list.
void updateServAvatarHash(char* pHash, int size)
{
    icq_packet packet;
    WORD wAvatarID;
    WORD wCommand;
    DBVARIANT dbvHash;
    int bResetHash = 0;
    BYTE bName = 0;

    if (!getSetting(NULL, "AvatarHash", &dbvHash))
    {
        bName = 0x30 + dbvHash.pbVal[1];

        if (memcmp(pHash, dbvHash.pbVal, 2) != 0)
        {
            /** add code to remove old hash from server */
            bResetHash = 1;
        }
        ICQFreeVariant(&dbvHash);
    }

    if (bResetHash) // start update session
        sendAddStart(FALSE);

    if (bResetHash || !pHash)
    {
        servlistcookie* ack = NULL;
        DWORD dwCookie;

        // Do we have a known server avatar ID?
        if (wAvatarID = getSettingWord(NULL, "SrvAvatarID", 0))
        {
            ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));
            if (!ack)
            {
                NetLog_Server("Cookie alloc failure.");
                return; // out of memory, go away
            }
            ack->dwAction = SSA_REMOVEAVATAR; // update avatar hash
            ack->wContactId = wAvatarID;
            dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_REMOVEFROMLIST, 0, ack); // take cookie

            // Build and send packet
            serverPacketInit(&packet, (WORD)(20 + (bName ? 1 : 0)));
            packFNACHeaderFull(&packet, ICQ_LISTS_FAMILY, ICQ_LISTS_REMOVEFROMLIST, 0, dwCookie);
            if (bName)
            {
                // name
                packWord(&packet, 1);
                packByte(&packet, bName);             // Name
            }
            else
                packWord(&packet, 0);                 // Name (null)
            packWord(&packet, 0);                   // GroupID (0 if not relevant)
            packWord(&packet, wAvatarID);           // EntryID
            packWord(&packet, SSI_ITEM_BUDDYICON);  // EntryType
            packWord(&packet, 0);                   // Length in bytes of following TLV
            sendServPacket(&packet);
        }
    }

    if (pHash)
    {
        servlistcookie* ack = NULL;
        DWORD dwCookie;
        WORD hashsize = size - 2;

        // Do we have a known server avatar ID? We should, unless we just subscribed to the serv-list for the first time
        if (bResetHash || (wAvatarID = getSettingWord(NULL, "SrvAvatarID", 0)) == 0)
        {
            // No, create a new random ID
            wAvatarID = GenerateServerID(SSIT_ITEM, 0, 0);
            wCommand = ICQ_LISTS_ADDTOLIST;
            NetLog_Server("Made new srvAvatarID, id is %u", wAvatarID);
        }
        else
        {
            NetLog_Server("Reused srvAvatarID, id is %u", wAvatarID);
            wCommand = ICQ_LISTS_UPDATEGROUP;
        }

        ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));
        if (!ack)
        {
            NetLog_Server("Cookie alloc failure.");
            return; // out of memory, go away
        }
        ack->dwAction = SSA_SETAVATAR; // update avatar hash
        ack->wContactId = wAvatarID;
        dwCookie = AllocateCookie(CKT_SERVERLIST, wCommand, 0, ack); // take cookie

        // Build and send packet
        serverPacketInit(&packet, (WORD)(29 + hashsize));
        packFNACHeaderFull(&packet, ICQ_LISTS_FAMILY, wCommand, 0, dwCookie);
        packWord(&packet, 1);                       // Name length
        packByte(&packet, (BYTE)(0x30 + pHash[1])); // Name
        packWord(&packet, 0);                       // GroupID (0 if not relevant)
        packWord(&packet, wAvatarID);               // EntryID
        packWord(&packet, SSI_ITEM_BUDDYICON);      // EntryType
        packWord(&packet, (WORD)(0x8 + hashsize));  // Length in bytes of following TLV
        packTLV(&packet, SSI_TLV_NAME, 0, NULL);                    // TLV (Name)
        packTLV(&packet, SSI_TLV_AVATARHASH, hashsize, pHash + 2);  // TLV (Hash)
        sendServPacket(&packet);
        // There is no need to send ICQ_LISTS_CLI_MODIFYSTART or
        // ICQ_LISTS_CLI_MODIFYEND when modifying the avatar hash
    }

    if (bResetHash) // finish update session
        sendAddEnd();
}



// Should be called before the server list is modified. When all
// modifications are done, call sendAddEnd().
void sendAddStart(int bImport)
{
    icq_packet packet;
    WORD wImportID = getSettingWord(NULL, "SrvImportID", 0);

    if (bImport && wImportID)
    {
        // we should be importing, check if already have import item
        if (getSettingDword(NULL, "ImportTS", 0) + 604800 < getSettingDword(NULL, "LogonTS", 0))
        {
            // is the timestamp week older, clear it and begin new import
            DWORD dwCookie;
            servlistcookie* ack = NULL;
            ack = (servlistcookie*)icq_alloc_zero(sizeof(servlistcookie));
            if (ack)
            {
                // we have cookie good, go on
                ack->dwAction = SSA_IMPORT;
                dwCookie = AllocateCookie(CKT_SERVERLIST, ICQ_LISTS_REMOVEFROMLIST, 0, ack);

                icq_sendSimpleItem(dwCookie, ICQ_LISTS_REMOVEFROMLIST, 0, "ImportTime", 0, wImportID, SSI_ITEM_IMPORTTIME);
            }
        }
    }

    serverPacketInit(&packet, (WORD)(bImport?14:10));
    packFNACHeader(&packet, ICQ_LISTS_FAMILY, ICQ_LISTS_CLI_MODIFYSTART);
    if (bImport) packDWord(&packet, 1<<0x10);
    sendServPacket(&packet);
}



// Should be called after the server list has been modified to inform
// the server that we are done.
void sendAddEnd(void)
{
    icq_packet packet;

    serverPacketInit(&packet, 10);
    packFNACHeader(&packet, ICQ_LISTS_FAMILY, ICQ_LISTS_CLI_MODIFYEND);
    sendServPacket(&packet);
}



// Sent when the last roster packet has been received
void sendRosterAck(void)
{
    icq_packet packet;

    serverPacketInit(&packet, 10);
    packFNACHeader(&packet, ICQ_LISTS_FAMILY, ICQ_LISTS_GOTLIST);
    sendServPacket(&packet);

#ifdef _DEBUG
    NetLog_Server("Sent SNAC(x13,x07) - CLI_ROSTERACK");
#endif
}
