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
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"


#define LISTSIZE 100
static CRITICAL_SECTION listmutex;
static HANDLE hInfoQueueEvent = NULL;
static int nInfoUserCount = 0;
static int bInfoPendingUsers = 0;
static BOOL bInfoUpdateEnabled = FALSE;
static BOOL bInfoUpdateRunning = FALSE;
static HANDLE hInfoThread = NULL;
static DWORD dwUpdateThreshold, dwInfoActiveRequest;
typedef struct s_userinfo
{
    DWORD dwUin;
    HANDLE hContact;
    time_t queued;
} userinfo;
userinfo m_infoUpdateList[LISTSIZE];


// Retrieve users' info
unsigned __stdcall icq_InfoUpdateThread(void* arg);



void icq_InitInfoUpdate(void)
{


    // Create wait objects
    hInfoQueueEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (hInfoQueueEvent)
    {
        int i = 0;
        // Init mutexes
        InitializeCriticalSection(&listmutex);

        // Init list
        for (i = 0; i<LISTSIZE; i++)
        {
            m_infoUpdateList[i].dwUin = 0;
            m_infoUpdateList[i].hContact = NULL;
            m_infoUpdateList[i].queued = 0;
        }

        hInfoThread = ICQCreateThreadEx(icq_InfoUpdateThread, NULL, NULL);
    }

    bInfoPendingUsers = 0;
    dwInfoActiveRequest = 0;
}

// Returns TRUE if user was queued
// Returns FALSE if the list was full
BOOL icq_QueueUser(HANDLE hContact)
{
    if (nInfoUserCount < LISTSIZE)
    {
        int i, nChecked = 0, nFirstFree = -1;
        BOOL bFound = FALSE;

        EnterCriticalSection(&listmutex);

        // Check if in list
        for (i = 0; (i<LISTSIZE && nChecked < nInfoUserCount); i++)
        {
            if (m_infoUpdateList[i].hContact)
            {
                nChecked++;
                if (m_infoUpdateList[i].hContact == hContact)
                {
                    bFound = TRUE;
                    break;
                }
            }
            else if (nFirstFree == -1)
            {
                nFirstFree = i;
            }
        }
        if (nFirstFree == -1)
            nFirstFree = i;

        // Add to list
        if (!bFound)
        {
            DWORD dwUin = ICQGetContactSettingUIN(hContact);

            if (dwUin)
            {
                m_infoUpdateList[nFirstFree].dwUin = dwUin;
                m_infoUpdateList[nFirstFree].hContact = hContact;
                m_infoUpdateList[nFirstFree].queued = time(NULL);
                nInfoUserCount++;
#ifdef _DEBUG
                NetLog_Server("Queued user %u, place %u, count %u", dwUin, nFirstFree, nInfoUserCount);
#endif
                // Notify worker thread
                if (hInfoQueueEvent && bInfoUpdateEnabled)
                    SetEvent(hInfoQueueEvent);
            }
        }

        LeaveCriticalSection(&listmutex);

        return TRUE;
    }

    return FALSE;
}



void icq_DequeueUser(DWORD dwUin)
{
    if (nInfoUserCount > 0)
    {
        int i = 0, nChecked = 0;
        // Check if in list
        EnterCriticalSection(&listmutex);
        for (i = 0; (i < LISTSIZE && nChecked < nInfoUserCount); i++)
        {
            if (m_infoUpdateList[i].dwUin)
            {
                nChecked++;
                // Remove from list
                if (m_infoUpdateList[i].dwUin == dwUin)
                {
#ifdef _DEBUG
                    NetLog_Server("Dequeued user %u", m_infoUpdateList[i].dwUin);
#endif
                    m_infoUpdateList[i].dwUin = 0;
                    m_infoUpdateList[i].hContact = NULL;
                    m_infoUpdateList[i].queued = 0;
                    nInfoUserCount--;
                    break;
                }
            }
        }
        LeaveCriticalSection(&listmutex);
    }
}



void icq_RescanInfoUpdate()
{
    HANDLE hContact = NULL;
    BOOL bOldEnable = bInfoUpdateEnabled;

    bInfoPendingUsers = 0;
    /* This is here, cause we do not want to emit large number of reuqest at once,
      fill queue, and let thread deal with it */
    bInfoUpdateEnabled = 0; // freeze thread
    // Queue all outdated users
    hContact = ICQFindFirstContact();

    while (hContact != NULL)
    {
        if (IsMetaInfoChanged(hContact))
        {
            // Queue user
            if (!icq_QueueUser(hContact))
            {
                // The queue is full, pause queuing contacts
                bInfoPendingUsers = 1;
                break;
            }
        }
        hContact = ICQFindNextContact(hContact);
    }
    icq_EnableUserLookup(bOldEnable); // wake up thread
}



void icq_EnableUserLookup(BOOL bEnable)
{
    bInfoUpdateEnabled = bEnable;

    // Notify worker thread
    if (bInfoUpdateEnabled && hInfoQueueEvent)
        SetEvent(hInfoQueueEvent);
}



unsigned __stdcall icq_InfoUpdateThread(void* arg)
{
    int i;
    DWORD dwWait = WAIT_OBJECT_0;
    WORD wGroup = 0;
    userinfo *hContactList[LISTSIZE];
    int nListIndex = 0, nRequestSize = 0;
    BYTE *pRequestData = NULL;

    NetLog_Server("%s thread starting.", "Info-Update");

    bInfoUpdateRunning = TRUE;


    while (bInfoUpdateRunning)
    {
        // Wait for a while
        ResetEvent(hInfoQueueEvent);

        if (!nInfoUserCount && bInfoPendingUsers) // whole queue processed, check if more users needs updating
            icq_RescanInfoUpdate();

        if (!nInfoUserCount || !bInfoUpdateEnabled || !icqOnline)
        {
            dwWait = WAIT_TIMEOUT;
            while (bInfoUpdateRunning && dwWait == WAIT_TIMEOUT)
            {
                // wait for new work or until we should end
                dwWait = WaitForSingleObjectEx(hInfoQueueEvent, 10000, TRUE);
            }
        }
        if (!bInfoUpdateRunning) break;

        switch (dwWait)
        {
        case WAIT_IO_COMPLETION:
            // Possible shutdown in progress
            break;

        case WAIT_OBJECT_0:
        case WAIT_TIMEOUT:
            // Time to check for new users
            if (!bInfoUpdateEnabled) continue; // we can't send requests now

            if (nInfoUserCount && icqOnline)
            {
                time_t now = time(NULL);
                BOOL bNotReady = FALSE, bTimeOuted = FALSE;

                // Check the list, take only users that were there for at least 5sec
                // wait if any user is there shorter than 5sec and not a single user is there longer than 20sec
                EnterCriticalSection(&listmutex);
                for (i = 0; i<LISTSIZE; i++)
                {
                    if (m_infoUpdateList[i].hContact)
                    {
                        if (m_infoUpdateList[i].queued + 20 < now)
                        {
                            bTimeOuted = TRUE;
                            break;
                        }
                        else if (m_infoUpdateList[i].queued + 5 >= now)
                            bNotReady = TRUE;
                    }
                }
                LeaveCriticalSection(&listmutex);

                if (!bTimeOuted && bNotReady)
                {
                    SleepEx(1000, TRUE);
                    if (!bInfoUpdateRunning)
                    {
                        // need to end as fast as possible
                        NetLog_Server("%s thread ended.", "Info-Update");
                        return 0;
                    }
                    continue;
                }

                if (FindCookie(dwInfoActiveRequest, NULL, NULL))
                {
                    // only send another request, when the previous is completed
#ifdef _DEBUG
                    NetLog_Server("Info-Update: Request 0x%x still in progress.", dwInfoActiveRequest);
#endif
                    SleepEx(1000, TRUE);
                    if (!bInfoUpdateRunning)
                    {
                        // need to end as fast as possible
                        NetLog_Server("%s thread ended.", "Info-Update");
                        return 0;
                    }
                    continue;
                }

#ifdef _DEBUG
                NetLog_Server("Info-Update: Users %u in queue.", nInfoUserCount);
#endif
                // Either some user is waiting long enough, or all users are ready (waited at least the minimum time)
                EnterCriticalSection(&ratesMutex);
                if (!gRates)
                {
                    // we cannot send info request - icq is offline
                    LeaveCriticalSection(&ratesMutex);
                    break;
                }
                wGroup = ratesGroupFromSNAC(gRates, ICQ_EXTENSIONS_FAMILY, ICQ_META_CLI_REQ);
                while (ratesNextRateLevel(gRates, wGroup) < ratesGetLimitLevel(gRates, wGroup, RML_IDLE_30))
                {
                    // we are over rate, need to wait before sending
                    int nDelay = ratesDelayToLevel(gRates, wGroup, RML_IDLE_50);

                    LeaveCriticalSection(&ratesMutex);
#ifdef _DEBUG
                    NetLog_Server("Rates: InfoUpdate delayed %dms", nDelay);
#endif
                    SleepEx(nDelay, TRUE); // do not keep things locked during sleep
                    if (!bInfoUpdateRunning)
                    {
                        // need to end as fast as possible
                        NetLog_Server("%s thread ended.", "Info-Update");
                        return 0;
                    }
                    EnterCriticalSection(&ratesMutex);
                    if (!gRates) // we lost connection when we slept, go away
                        break;
                }
                if (!gRates)
                {
                    // we cannot send info request - icq is offline
                    LeaveCriticalSection(&ratesMutex);
                    break;
                }
                LeaveCriticalSection(&ratesMutex);

                EnterCriticalSection(&listmutex);
                nListIndex = 0;
                nRequestSize = 0;
                for (i = 0; i<LISTSIZE; i++)
                {
                    if (m_infoUpdateList[i].hContact)
                    {
                        // check TS again, maybe it has been updated while we slept
                        if (IsMetaInfoChanged(m_infoUpdateList[i].hContact))
                        {
                            if (m_infoUpdateList[i].queued + 5 < now)
                            {
                                BYTE *pItem = NULL;
                                int nItemSize = 0;
                                DBVARIANT dbv = {DBVT_DELETED};

                                if (!ICQGetContactSetting(m_infoUpdateList[i].hContact, DBSETTING_METAINFO_TOKEN, &dbv))
                                {
                                    // retrieve user details using privacy token
                                    ppackTLV(&pItem, &nItemSize, 0x96, dbv.cpbVal, dbv.pbVal);
                                    ICQFreeVariant(&dbv);
                                }
                                // last updated time
                                ppackTLVDouble(&pItem, &nItemSize, 0x64, ICQGetContactSettingDouble(m_infoUpdateList[i].hContact, DBSETTING_METAINFO_TIME, 0));

                                ppackTLVUID(&pItem, &nItemSize, 0x32, m_infoUpdateList[i].dwUin, NULL);
                                ppackWord(&pRequestData, &nRequestSize, (WORD)nItemSize);
                                ppackBuffer(&pRequestData, &nRequestSize, nItemSize, pItem);
                                // take a reference
                                mir_free(pItem);
                                hContactList[nListIndex++] = &m_infoUpdateList[i];
                            }
                        }
                        else
                        {
#ifdef _DEBUG
                            NetLog_Server("Dequeued absolete user %u", m_infoUpdateList[i].dwUin);
#endif
                            // Dequeue user and find another one
                            m_infoUpdateList[i].dwUin = 0;
                            m_infoUpdateList[i].hContact = NULL;
                            nInfoUserCount--;
                            // continue for loop
                        }
                    }
                }

#ifdef _DEBUG
                NetLog_Server("Request info for %u user(s).", nListIndex);
#endif
                if (!nListIndex)
                {
                    // no users to request info for
                    LeaveCriticalSection(&listmutex);
                    break;
                }
                if (!(dwInfoActiveRequest = sendUserInfoMultiRequest(pRequestData, nRequestSize, nListIndex)))
                {
                    // sending data packet failed
                    mir_free(&pRequestData);
                    LeaveCriticalSection(&listmutex);
                    break;
                }
                mir_free(&pRequestData);

                for (i = 0; i<nListIndex; i++)
                {
                    // Dequeue users and go back to sleep
                    hContactList[i]->dwUin = 0;
                    hContactList[i]->hContact = NULL;
                    hContactList[i]->queued = 0;
                    nInfoUserCount--;
                }
                LeaveCriticalSection(&listmutex);
            }
            break;

        default:
            // Something strange happened. Exit
            bInfoUpdateRunning = FALSE;
            break;
        }
    }
    NetLog_Server("%s thread ended.", "Info-Update");
    return 0;
}



// Clean up before exit
void icq_InfoUpdateCleanup(void)
{
    bInfoUpdateRunning = FALSE;
    SetEvent(hInfoQueueEvent); // break queue loop
    if (hInfoThread) WaitForSingleObjectEx(hInfoThread, INFINITE, TRUE);
    // Uninit mutex
    DeleteCriticalSection(&listmutex);
    CloseHandle(hInfoQueueEvent);
    CloseHandle(hInfoThread);
}
