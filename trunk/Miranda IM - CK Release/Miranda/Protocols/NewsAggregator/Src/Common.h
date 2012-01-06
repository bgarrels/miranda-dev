/* 
Copyright (C) 2012 Mataes

This is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this file; see the file license.txt.  If
not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  
*/

#define MIRANDA_VER    0x0900
#define MIRANDA_CUSTOM_LP

// Windows Header Files:
#include <windows.h>
#include <commctrl.h>
#include <time.h>

// Miranda header files
#include <newpluginapi.h>
#include <m_clist.h>
#include <m_skin.h>
#include <m_langpack.h>
#include <m_options.h>
#include <m_database.h>
#include <m_utils.h>
#include <m_system.h>
#include <m_popup.h>
#include <m_hotkeys.h>
#include <m_netlib.h>
#include <m_icolib.h>
#include <win2k.h>
#include <m_protocols.h>
#include <m_protomod.h>
#include <m_protosvc.h>
#include <m_xml.h>

#include <m_folders.h>
#include <m_popup2.h>

#include "..\version.h"
#include "..\resource.h"

#define MODULE	"NewsAggr"
extern HINSTANCE hInst;
extern HWND hAddFeedDlg;
extern UINT_PTR timerId;
// check if Feeds is currently updating
extern BOOL ThreadRunning;

struct ItemInfo
{
	HWND hwndList;
	HANDLE hContact;
	int SelNumber;
	TCHAR nick[MAX_PATH];
	TCHAR url[MAX_PATH];
};

INT_PTR NewsAggrInit(WPARAM wParam,LPARAM lParam);
INT OptInit(WPARAM wParam, LPARAM lParam);
INT_PTR NewsAggrPreShutdown(WPARAM wParam,LPARAM lParam);
VOID NetlibInit();
VOID NetlibUnInit();
VOID InitMenu();
VOID InitIcons();
HICON LoadIconEx(const char* name, BOOL big);
HANDLE  GetIconHandle(const char* name);
INT_PTR NewsAggrGetName(WPARAM wParam, LPARAM lParam);
INT_PTR NewsAggrGetCaps(WPARAM wp,LPARAM lp);
INT_PTR NewsAggrSetStatus(WPARAM wp,LPARAM /*lp*/);
INT_PTR NewsAggrGetStatus(WPARAM/* wp*/,LPARAM/* lp*/);
INT_PTR NewsAggrLoadIcon(WPARAM wParam,LPARAM lParam);
INT_PTR NewsAggrGetInfo(WPARAM wParam,LPARAM lParam);

INT_PTR CheckAllFeeds(WPARAM wParam,LPARAM lParam);
INT_PTR AddFeed(WPARAM wParam,LPARAM lParam);
INT_PTR ImportFeeds(WPARAM wParam,LPARAM lParam);
INT_PTR ExportFeeds(WPARAM wParam,LPARAM lParam);
INT_PTR CheckFeed(WPARAM wParam,LPARAM lParam);
INT_PTR CALLBACK DlgProcAddFeedOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgProcChangeFeedOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
VOID CALLBACK timerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
VOID CALLBACK timerProc2(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

BOOL IsMyContact(HANDLE hContact);
VOID GetNewsData(TCHAR *szUrl, char** szData);
VOID UpdateList (HWND hwndList);
VOID DeleteAllItems(HWND hwndList);

// ===============  NewsAggr SERVICES  ================
// Check all Feeds info
// WPARAM = LPARAM = NULL
#define MS_NEWSAGGR_CHECKALLFEEDS	"NEWSAGGR/CheckAllFeeds"

// Add new Feed channel
// WPARAM = LPARAM = NULL
#define MS_NEWSAGGR_ADDFEED	"NEWSAGGR/AddNewsFeed"

// Import Feed chanels from file
// WPARAM = LPARAM = NULL
#define MS_NEWSAGGR_IMPORTFEEDS	"NEWSAGGR/ImportFeeds"

// Export Feed chanels to file
// WPARAM = LPARAM = NULL
#define MS_NEWSAGGR_EXPORTFEEDS	"NEWSAGGR/ExportFeeds"

// Check Feed info
// WPARAM = LPARAM = NULL
#define MS_NEWSAGGR_CHECKFEED	"NEWSAGGR/CheckFeed"