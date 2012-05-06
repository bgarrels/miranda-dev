/*
Historysweeper plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2002-2003  Sergey V. Gershovich
			Copyright (C) 2006-2009  Boris Krasnovskiy


Copyright 2000-2012 Miranda IM project,
all portions of this codebase are copyrighted to the people
listed in contributors.txt.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

===============================================================================

File name      : $HeadURL: 
Revision       : $Revision: 
Last change on : $Date: 
Last change by : $Author:
$Id$		   : $Id$:

===============================================================================
*/

#include "historysweeper.h"
#include <time.h>

// build criteria
time_t BuildCriteria(int dwti)
{
	time_t tim =  time(NULL);
	struct tm gtim = *gmtime(&tim);

	switch (dwti)
	{
		case 0:
			return 0;
		case 1:
			gtim.tm_mday -= 1;
			break;
		case 2:
			gtim.tm_mday -= 3;
			break;
		case 3:
			gtim.tm_mday -= 7;
			break;
		case 4:
			gtim.tm_mday -= 14;
			break;
		case 5:
			gtim.tm_mon -= 1;
			break;
		case 6:
			gtim.tm_mon -= 3;
			break;
		case 7:
			gtim.tm_mon -= 6;
			break;
		case 8:
			gtim.tm_year -= 1;
			break;
	}
	return mktime(&gtim);
}

typedef struct
{
	HANDLE hDBEvent;
	DWORD CRC32;
	DWORD Timestamp;
} BEventData;

static int CompareBookmarks( const void* p1, const void* p2 )
{
	return (BYTE*)((BEventData*)p2)->hDBEvent - (BYTE*)((BEventData*)p1)->hDBEvent;
}

static void GetBookmarks(HANDLE hContact, time_t tm, BEventData** books, size_t* bookcnt )
{
	DBVARIANT dbv;
	DBCONTACTGETSETTING cgs;

	*books = NULL;
	*bookcnt = 0;

	cgs.szModule = "HistoryPlusPlus";
	cgs.szSetting = "Bookmarks";
	cgs.pValue = &dbv;
	if (CallService(MS_DB_CONTACT_GETSETTING, (WPARAM)hContact, (LPARAM)&cgs) == 0)
	{
		if (dbv.cpbVal > 2 && *(WORD*)dbv.pbVal >= sizeof(BEventData))
		{
			size_t recSize = *(WORD*)dbv.pbVal;
			size_t numb = (dbv.cpbVal - sizeof(WORD)) / recSize;

			BEventData* tbooks = mir_alloc(sizeof(BEventData)*numb);
			size_t tbookcnt = 0;

			size_t i;
			BYTE* evs = dbv.pbVal + sizeof(WORD);

			for (i=0; i<numb; ++i)
			{
				if (((BEventData*)evs)->Timestamp <= (unsigned)tm)
					tbooks[tbookcnt++] = *(BEventData*)evs;
				evs += recSize;
			}
			if (tbookcnt != 0)
			{
				qsort(tbooks, tbookcnt, sizeof(BEventData), CompareBookmarks);
				*bookcnt = tbookcnt;
				*books = tbooks;
			}
			else
				mir_free(tbooks);
		}
	
		DBFreeVariant(&dbv);
	}
}


//Sweep history from specified contact
void SweepHistoryFromContact(HANDLE hContact, time_t Criteria, BOOL BatchJob, BOOL keepUnread)
{
	//Get first event
	HANDLE hDBEvent = (HANDLE)CallService(MS_DB_EVENT_FINDFIRST, (WPARAM)hContact, 0);
	if (hDBEvent != NULL) 
	{ 
		BOOL unsafe;
		BEventData* books; 
		size_t bookcnt;

		// ask user (if not batchjob)
		if (!BatchJob)
		{
			HWND hwndCLC = (HWND)CallService(MS_CLUI_GETHWND, 0, 0);
			if (MessageBox(hwndCLC, TranslateT("Sweep history messages?"), TranslateT(ModuleName),
				MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES) return;
		}

		SetCursor(LoadCursor(0, IDC_WAIT));

		// switch off SAFETYMODE if necessary
		unsafe = DBGetContactSettingByte(NULL, ModuleName, "UnsafeMode", 0);
		if (unsafe)	CallService(MS_DB_SETSAFETYMODE, 0, 0); 

		GetBookmarks(hContact, Criteria, &books, &bookcnt);

		while (hDBEvent != NULL) 
		{
			BEventData* item = NULL;
			BOOL doDelete = TRUE;
			HANDLE hDBEventNext;

			if (bookcnt != 0)
			{
				BEventData ev = { hDBEvent, 0, 0 };
				item = (BEventData*)bsearch( &ev, books, bookcnt, sizeof(BEventData), CompareBookmarks);
			}

			// if it approach to criteria - delete it
			if (Criteria != 0 || item != NULL || keepUnread)
			{
				DBEVENTINFO dbei = {0}; 
				dbei.cbSize = sizeof(dbei);

				CallService(MS_DB_EVENT_GET, (WPARAM)hDBEvent, (LPARAM)&dbei);
				if (Criteria != 0 && (unsigned)Criteria < dbei.timestamp) break;

				if (item != NULL)
				{
					size_t btshift = (--bookcnt - (item - books))*sizeof(BEventData);
					doDelete = item->Timestamp != dbei.timestamp;
					if (btshift) memmove(item, item+1, btshift);
				}
				doDelete &= (dbei.flags & (DBEF_SENT | DBEF_READ)) || !keepUnread;
			}
		
			// find next event
			hDBEventNext = (HANDLE)CallService(MS_DB_EVENT_FINDNEXT, (WPARAM)hDBEvent, 0);

			if (doDelete)
				CallService(MS_DB_EVENT_DELETE, (WPARAM)hContact, (LPARAM)hDBEvent);
			
			hDBEvent = hDBEventNext;
		}

		mir_free(books);

		// switch ON safety mode as fast as we can
		// to avoid  DB corruption
		if (unsafe)	CallService(MS_DB_SETSAFETYMODE, 1, 0);

		SetCursor(LoadCursor(0, IDC_ARROW));
	}
}

// sweep system history
static void SweepSystemHistory(time_t dwCriteria, BOOL bBatchJob)
{
	SweepHistoryFromContact(NULL, dwCriteria, bBatchJob, FALSE);
}

// sweep histroy from selected contacts
static void SweepHistoryFromSelectedContacts(time_t dwCriteria, BOOL bBatchJob, BOOL keepUnread)
{
	int gPolicy = DBGetContactSettingByte(NULL, ModuleName, "SweepHistory", 0);

	//find first contact
	HANDLE hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
	while ( hContact != NULL )
	{
		int lPolicy = DBGetContactSettingByte(hContact, "CList", "SweepHistory", gPolicy);

		// if it ticked in options page
		if (lPolicy)
		{
			// sweep history from it
			SweepHistoryFromContact(hContact, lPolicy == 2 ? 0 : dwCriteria, bBatchJob, keepUnread);
		}

		hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0);
	}
}

static void SweepWholeHistory(time_t dwCriteria, BOOL bBatchJob)
{
	// find first contact
	HANDLE hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
	while ( hContact != NULL )
	{
		//sweep history from it
		SweepHistoryFromContact (hContact, dwCriteria, bBatchJob, FALSE);
		hContact = (HANDLE)CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)hContact, 0);
	}

	// sweep system history
	SweepSystemHistory(dwCriteria, bBatchJob);
}

// stratup/shutdown action
void SSAction(void)
{ 
	time_t gCriteria = BuildCriteria(DBGetContactSettingByte(NULL, ModuleName, "StartupShutdownOlder", 0));

	if (DBGetContactSettingByte(NULL, ModuleName, "SweepSHistory", 0))
		SweepSystemHistory(gCriteria, TRUE);

	SweepHistoryFromSelectedContacts(gCriteria, TRUE, TRUE);
}

// Main menu action
INT_PTR MainMenuAction(WPARAM wParam, LPARAM lParam)
{
	time_t dwCriteria = BuildCriteria(DBGetContactSettingByte(NULL, ModuleName, "MainMenuOlder", 0));
	HWND hwndCL = (HWND)CallService(MS_CLUI_GETHWND, 0, 0);
	switch (DBGetContactSettingByte(NULL, ModuleName, "MainMenuAction", 0))
	{
		case 0:
			if (MessageBox (hwndCL,
				TranslateT("Sweep system history?"), TranslateT(ModuleName),
				MB_YESNO |MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
				SweepSystemHistory(dwCriteria, TRUE);
			break;

		case 1:
			if (MessageBox (hwndCL,
				TranslateT("Sweep whole history?"), TranslateT(ModuleName),
				MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
				SweepWholeHistory(dwCriteria, TRUE);
			break;

		case 2:
			if (MessageBox (hwndCL,
				TranslateT("Sweep history from selected contacts?"), TranslateT(ModuleName),
				MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
				SweepHistoryFromSelectedContacts(dwCriteria, TRUE, FALSE);
			break;
	}
	return 0;
}

// Contact menu action
INT_PTR ContactMenuAction(WPARAM wParam, LPARAM lParam)
{
	SweepHistoryFromContact ((HANDLE)wParam, 0, FALSE, FALSE);
	return 0;
}

int OnWindowEvent(WPARAM wParam, LPARAM lParam)
{
	MessageWindowEventData* msgEvData  = (MessageWindowEventData*)lParam;

	if (msgEvData->uType == MSG_WINDOW_EVT_CLOSE && DBGetContactSettingByte(NULL, ModuleName, "SweepOnClose", 0))
	{
		int gPolicy = DBGetContactSettingByte(NULL, ModuleName, "SweepHistory", 0);
		int lPolicy = DBGetContactSettingByte(msgEvData->hContact, "CList", "SweepHistory", gPolicy);

		if (lPolicy == 2) SweepHistoryFromContact(msgEvData->hContact, 0, TRUE, TRUE);
	}

	return 0;
}
