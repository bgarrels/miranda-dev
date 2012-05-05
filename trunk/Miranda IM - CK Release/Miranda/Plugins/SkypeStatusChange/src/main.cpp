/*
Skype Status Change plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2008-2010 Dioksin 

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

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <atlbase.h>
#include <atlconv.h>
#include <commctrl.h>

#define MIRANDA_CUSTOM_LP

#pragma warning(disable: 4996)
#include <newpluginapi.h>
#include <m_utils.h>

#include "m_updater.h"
#include <m_protocols.h>
#include <m_protosvc.h>
#include <cassert>
#include <m_awaymsg.h>
#include <m_database.h>
#include <m_options.h>
#include <m_langpack.h>
#pragma warning(default: 4996)
#include "resource.h"
#include <map>
#include <string>


PLUGINLINK* pluginLink = NULL;
MM_INTERFACE    mmi;
UTF8_INTERFACE  utfi;
int hLangpack = 0;


namespace
{
	PLUGININFOEX g_pluginInfo =
	{
		sizeof(PLUGININFOEX),
#ifdef WIN64
		"Skype Status Change (x64)",
#else
		"Skype Status Change",
#endif
		PLUGIN_MAKE_VERSION(0,0,0,18),
		"Skype Status Change according to miranda-Status",
		"Dioksin",
		"dioksin@ua.fm",
		"Don't worry!",
		"http://www.miranda-im.org",
		UNICODE_AWARE,		//not transient
		0		//doesn't replace anything built-in
#ifdef WIN64
		,{ 0x2925520b, 0x6677, 0x4658, { 0x8b, 0xad, 0x56, 0x61, 0xd1, 0x3e, 0x46, 0x92 } }
#elif _UNICODE
		,{0x2ec6238e, 0xb08f, 0x449b, { 0x9c, 0x96, 0xcb, 0x51, 0x36, 0x02, 0x9e, 0xfc } }
#else
		,{0xc57528e7, 0x3d38, 0x4c4e, { 0xb3, 0x48, 0x3f, 0x59, 0x44, 0x96, 0x62, 0x85 } }
#endif
	};

	UINT g_MsgIDSkypeControlAPIAttach = 0;
	UINT g_MsgIDSkypeControlAPIDiscover = 0;
	HINSTANCE g_hModule = NULL;
	HWND g_wndMainWindow = NULL;
// 	bool g_bStopThread = false;
	HANDLE g_hThread = NULL;
	HANDLE g_hEventShutdown = NULL;
	HANDLE g_hOptInitialize = NULL;
	bool g_bMirandaIsShutdount = false;
	LPCSTR g_pszPluginDbSection = "Change Skype Status";

	enum
	{
		SKYPECONTROLAPI_ATTACH_SUCCESS = 0,								// Client is successfully attached and API window handle can be found in wParam parameter
		SKYPECONTROLAPI_ATTACH_PENDING_AUTHORIZATION = 1,	// Skype has acknowledged connection request and is waiting for confirmation from the user.
		// The client is not yet attached and should wait for SKYPECONTROLAPI_ATTACH_SUCCESS message
		SKYPECONTROLAPI_ATTACH_REFUSED = 2,								// User has explicitly denied access to client
		SKYPECONTROLAPI_ATTACH_NOT_AVAILABLE = 3,					// API is not available at the moment. For example, this happens when no user is currently logged in.
		// Client should wait for SKYPECONTROLAPI_ATTACH_API_AVAILABLE broadcast before making any further
		// connection attempts.
		SKYPECONTROLAPI_ATTACH_API_AVAILABLE = 0x8001
	};

	LPCTSTR g_pszSkypeWndClassName = _T("SkypeHelperWindow{155198f0-8749-47b7-ac53-58f2ac70844c}");

	struct CMirandaStatus2SkypeStatus
	{
		int m_nMirandaStatus;
		LPCSTR m_pszSkypeStatus;
		LPCTSTR m_ptszStatusName;
	};

	const CMirandaStatus2SkypeStatus g_aStatusCode[] = 
	{
		{ID_STATUS_AWAY, "AWAY",_T("Away")},
		{ID_STATUS_NA, "AWAY",_T("NA")}, // removed in Skype 5
		{ID_STATUS_DND, "DND",_T("DND")},
		{ID_STATUS_ONLINE, "ONLINE",_T("Online")},
		{ID_STATUS_FREECHAT, "ONLINE",_T("Free for chat")},	// SKYPEME status doesn't work in Skype 4!
		{ID_STATUS_OFFLINE, "OFFLINE",_T("Offline")},
		{ID_STATUS_INVISIBLE, "INVISIBLE",_T("Invisible")},
// 		{ID_STATUS_CONNECTING, "CONNECTING"},
		{ID_STATUS_OCCUPIED,"DND",_T("Occupied")},
		{ID_STATUS_ONTHEPHONE,"DND",_T("On the phone")},
		{ID_STATUS_OUTTOLUNCH,"DND",_T("Out to lunch")}
	};

	const size_t g_cStatusCode = sizeof(g_aStatusCode)/sizeof(g_aStatusCode[0]);
	enum{INVALID_INDEX = 0xFFFFFFFF};
	
	class CLightMutex
	{
	public:
		CLightMutex(){::InitializeCriticalSection(&m_section);}
		~CLightMutex(){::DeleteCriticalSection(&m_section);}

		void Lock(){::EnterCriticalSection(&m_section);}
		void Unlock(){::LeaveCriticalSection(&m_section);}

	private:
		CRITICAL_SECTION m_section;
	};

	class CStatusInfo
	{
	public:
		CStatusInfo() : m_nStatusIndex(INVALID_INDEX){m_szModule[0] = '\0';}

		size_t StatusIndex()const{return m_nStatusIndex;}
		void StatusIndex(size_t val){m_nStatusIndex = val;}

		const char* Module()const{return m_szModule;}
		void Module(const char* val)
		{
			if(val)
			{
				strncpy_s(m_szModule,val,strlen(val));
			}
			else
			{
				m_szModule[0] = '\0';
			}
		}

	private:
		char m_szModule[MAXMODULELABELLENGTH];
		size_t m_nStatusIndex;
	};

	template<class TSyncObject>class CGuard
	{
	public:
		CGuard(TSyncObject& so) : m_so(so)
		{
			m_so.Lock();
		}
		~CGuard()
		{
			m_so.Unlock();
		}

	private:
		TSyncObject& m_so;
	};

	CStatusInfo g_CurrStatusInfo;
	CLightMutex g_mxStatusInfo;

	class COptions
	{
		enum
		{
			cssOnline		= 0x00000001,
			cssOffline		= 0x00000002,
			cssInvisible	= 0x00000004,
			cssShortAway	= 0x00000008,
			cssLongAway		= 0x00000010,
			cssLightDND		= 0x00000020,
			cssHeavyDND		= 0x00000040,
			cssFreeChart	= 0x00000080,
			cssOutToLunch	= 0x00000100,
			cssOnThePhone	= 0x00000200,
			cssIdle			= 0x00000400,
			cssAll			= 0x80000000
		};

		static unsigned long Status2Flag(int status)
		{
			switch(status)
			{
				case ID_STATUS_ONLINE: return cssOnline;
				case ID_STATUS_OFFLINE: return cssOffline;
				case ID_STATUS_INVISIBLE: return cssInvisible;
				case ID_STATUS_OUTTOLUNCH: return cssOutToLunch;
				case ID_STATUS_ONTHEPHONE: return cssOnThePhone;
				case ID_STATUS_AWAY: return cssShortAway;
				case ID_STATUS_NA: return cssLongAway;
				case ID_STATUS_OCCUPIED: return cssLightDND;
				case ID_STATUS_DND: return cssHeavyDND;
				case ID_STATUS_FREECHAT: return cssFreeChart;
				case ID_STATUS_IDLE: return cssIdle;
			}
			return 0;
		}

	public:
		COptions(){}

		bool IsProtocolExcluded(const char* pszProtocol)const
		{
			DWORD dwSettings = DBGetContactSettingDword(NULL,pszProtocol,"ChangeSkypeStatus_Exclusions",0);
			return ((dwSettings&cssAll) ? true : false);
		}

		bool IsProtocolStatusExcluded(const char* pszProtocol,int nStatus)const
		{
			DWORD dwSettings = DBGetContactSettingDword(NULL,pszProtocol,"ChangeSkypeStatus_Exclusions",0);
			return ((dwSettings&Status2Flag(nStatus)) ? true : false);
		}

		void ExcludeProtocol(const char* pszProtocol,bool bExclude)
		{
			DWORD dwSettings = DBGetContactSettingDword(NULL,pszProtocol,"ChangeSkypeStatus_Exclusions",0);
			if(bExclude)
			{
				dwSettings |=cssAll;
			}
			else
			{
				dwSettings &= ~cssAll;
			}
			DBWriteContactSettingDword(NULL,pszProtocol,"ChangeSkypeStatus_Exclusions",dwSettings);
		}

		void ExcludeProtocolStatus(const char* pszProtocol,int nStatus,bool bExclude)
		{
			DWORD dwSettings = DBGetContactSettingDword(NULL,pszProtocol,"ChangeSkypeStatus_Exclusions",0);
			if(bExclude)
			{
				dwSettings |=Status2Flag(nStatus);
			}
			else
			{
				dwSettings &= ~Status2Flag(nStatus);
			}
			DBWriteContactSettingDword(NULL,pszProtocol,"ChangeSkypeStatus_Exclusions",dwSettings);
		}

		bool GetSyncStatusMsgFlag()const
		{
			return DBGetContactSettingByte(NULL,g_pszPluginDbSection,"SyncStatusMsg",false) > 0;
		}

		bool GetSyncStatusStateFlag()const
		{
			return DBGetContactSettingByte(NULL,g_pszPluginDbSection,"SyncStatusState",false) > 0;
		}

		void SetSyncStatusMsgFlag(bool b)
		{
			DBWriteContactSettingByte(NULL,g_pszPluginDbSection,"SyncStatusMsg",b);
		}

		void SetSyncStatusStateFlag(bool b)
		{
			DBWriteContactSettingByte(NULL,g_pszPluginDbSection,"SyncStatusState",b);
		}

		bool GetPreviousStatus(const char* pszProtocol,int& nStatus)const
		{
			TProtocolToStatus::const_iterator i = m_aProtocol2Status.find(pszProtocol);
			if(i != m_aProtocol2Status.end())
			{
				 nStatus = i->second;
				 return true;
			}
			else
			{
				return false;
			}
		}

		void SetPreviousStatus(const char* pszProtocol,int nStatus)
		{
			m_aProtocol2Status[pszProtocol] = nStatus;
		}

	private:
		typedef std::map<std::string,int> TProtocolToStatus;
		TProtocolToStatus m_aProtocol2Status;
	};

	COptions g_Options;

	int SSC_OnProtocolAck(WPARAM/* wParam*/,LPARAM lParam)
	{
		ACKDATA* pAckData = reinterpret_cast<ACKDATA*>(lParam);

		if((false == g_bMirandaIsShutdount)
			&& (ACKTYPE_STATUS == pAckData->type) 
			&& (ACKRESULT_SUCCESS == pAckData->result) 
			&& (NULL != pAckData->szModule)
			&& (false == g_Options.IsProtocolExcluded(pAckData->szModule)))
		{
			int nStatus = CallProtoService(pAckData->szModule,PS_GETSTATUS,0,0);
			for(size_t i = 0;i < g_cStatusCode;++i)
			{
				const CMirandaStatus2SkypeStatus& ms = g_aStatusCode[i];
				if(ms.m_nMirandaStatus == nStatus)
				{
					int nPrevStatus;
					if((false == g_Options.IsProtocolStatusExcluded(pAckData->szModule,nStatus))
						&& ((false == g_Options.GetSyncStatusStateFlag()) 
							|| (false == g_Options.GetPreviousStatus(pAckData->szModule,nPrevStatus)) 
							|| (nPrevStatus != nStatus)))
					{
						{
							CGuard<CLightMutex> guard(g_mxStatusInfo);
							g_CurrStatusInfo.StatusIndex(i);
							g_CurrStatusInfo.Module(pAckData->szModule);
						}
						if(0 == ::PostMessage(HWND_BROADCAST,g_MsgIDSkypeControlAPIDiscover,(WPARAM)g_wndMainWindow,0))
						{
							CGuard<CLightMutex> guard(g_mxStatusInfo);
							g_CurrStatusInfo.StatusIndex(INVALID_INDEX);
							g_CurrStatusInfo.Module(NULL);
						}
						else
						{
							g_Options.SetPreviousStatus(pAckData->szModule,nStatus);
						}
					}
					break;
				}
			}
		}

		return 0;
	}

	unsigned __stdcall ThreadFunc(void*) 
	{ 
		for(;;)
		{
			MSG msg;
			if(TRUE == ::PeekMessage(&msg,g_wndMainWindow,0,0,PM_NOREMOVE))
			{
				while(::GetMessage(&msg,g_wndMainWindow,0,0)) 
				{ 
					TranslateMessage(&msg); 
					DispatchMessage(&msg); 
				} 
			}
			else
			{
				DWORD dwResult = ::MsgWaitForMultipleObjects(1,&g_hEventShutdown,FALSE,INFINITE,QS_ALLEVENTS);
				assert(WAIT_FAILED != dwResult);
				if(WAIT_OBJECT_0 == dwResult)
				{
					break;
				}
			}
		}

		return 0; 
	}   

	bool ProtoServiceExists(const char *szModule,const char *szService)
	{
		char str[MAXMODULELABELLENGTH * 2];
		strncpy_s(str,szModule,strlen(szModule));
		strncat_s(str,szService,strlen(szService));

		return (ServiceExists(str) > 0);
	}


	LRESULT APIENTRY SkypeAPI_WindowProc(HWND hWnd,
										 UINT msg,
										 WPARAM wp,
										 LPARAM lp)
	{
		LRESULT lReturnCode = 0;
		bool bIssueDefProc = false;

		switch(msg)
		{
		case WM_DESTROY:
			g_wndMainWindow = NULL;
			break;
		case WM_COPYDATA:
// 			if( hGlobal_SkypeAPIWindowHandle==(HWND)uiParam )
// 			{
// 				PCOPYDATASTRUCT poCopyData=(PCOPYDATASTRUCT)ulParam;
// 				printf( "Message from Skype(%u): %.*s\n", poCopyData->dwData, poCopyData->cbData, poCopyData->lpData);
// 				lReturnCode=1;
// 			}
			break;
		default:
			if(msg == g_MsgIDSkypeControlAPIAttach)
			{
				switch(lp)
				{
				case SKYPECONTROLAPI_ATTACH_SUCCESS:
					{
						CStatusInfo si;
						{
							CGuard<CLightMutex> guard(g_mxStatusInfo);
							si = g_CurrStatusInfo;
						}
						if(INVALID_INDEX != si.StatusIndex() && si.StatusIndex() < g_cStatusCode)
						{
							const CMirandaStatus2SkypeStatus& ms = g_aStatusCode[si.StatusIndex()];
							HWND wndSkypeAPIWindow = reinterpret_cast<HWND>(wp);
// 							if(TRUE == ::IsWindow(wndSkypeAPIWindow))
							{
								enum{BUFFER_SIZE = 256};
								char szSkypeCmd[BUFFER_SIZE];
								const char szSkypeCmdSetStatus[] = "SET USERSTATUS ";
								::strncpy_s(szSkypeCmd,szSkypeCmdSetStatus,sizeof(szSkypeCmdSetStatus)/sizeof(szSkypeCmdSetStatus[0]));
								::strncat_s(szSkypeCmd,ms.m_pszSkypeStatus,strlen(ms.m_pszSkypeStatus));
								DWORD cLength = static_cast<DWORD>(strlen(szSkypeCmd));

								COPYDATASTRUCT oCopyData;

								oCopyData.dwData=0;
								oCopyData.lpData = szSkypeCmd;
								oCopyData.cbData = cLength+1;
								SendMessage(wndSkypeAPIWindow,WM_COPYDATA,(WPARAM)hWnd,(LPARAM)&oCopyData);
								if(g_Options.GetSyncStatusMsgFlag())
								{
									TCHAR* pszStatusMsg = NULL;
									 if(true == ProtoServiceExists(si.Module(), PS_GETMYAWAYMSG)) 
									 {
										 pszStatusMsg =  reinterpret_cast<TCHAR*>(CallProtoService(si.Module(),PS_GETMYAWAYMSG,(WPARAM)ms.m_nMirandaStatus,SGMA_TCHAR));
									 }
									if((NULL == pszStatusMsg) || (CALLSERVICE_NOTFOUND == reinterpret_cast<int>(pszStatusMsg)))
									{
										pszStatusMsg = reinterpret_cast<TCHAR*>(CallService(MS_AWAYMSG_GETSTATUSMSGT,(WPARAM)ms.m_nMirandaStatus,0));
									}
									
									if(pszStatusMsg && reinterpret_cast<int>(pszStatusMsg) != CALLSERVICE_NOTFOUND)
									{
										char* pMsg = mir_utf8encodeT(pszStatusMsg);
										mir_free(pszStatusMsg);

										const char szSkypeCmdSetStatusMsg[] = "SET PROFILE MOOD_TEXT ";
										::strncpy_s(szSkypeCmd,szSkypeCmdSetStatusMsg,sizeof(szSkypeCmdSetStatusMsg)/sizeof(szSkypeCmdSetStatusMsg[0]));
										::strncat_s(szSkypeCmd,pMsg,strlen(pMsg));										
										mir_free(pMsg);
										
										DWORD cLength = static_cast<DWORD>(strlen(szSkypeCmd));

										oCopyData.dwData=0;
										oCopyData.lpData = szSkypeCmd;
										oCopyData.cbData = cLength+1;
										SendMessage(wndSkypeAPIWindow,WM_COPYDATA,(WPARAM)hWnd,(LPARAM)&oCopyData);
									}
								}
							}
						}
					}
					break;
				case SKYPECONTROLAPI_ATTACH_PENDING_AUTHORIZATION:
// 					printf("!!! Pending authorization\n");
					break;
				case SKYPECONTROLAPI_ATTACH_REFUSED:
// 					printf("!!! Connection refused\n");
					break;
				case SKYPECONTROLAPI_ATTACH_NOT_AVAILABLE:
// 					printf("!!! Skype API not available\n");
					break;
				case SKYPECONTROLAPI_ATTACH_API_AVAILABLE:
// 					printf("!!! Try connect now (API available); issue #connect\n");
					break;
				}
				lReturnCode=1;
			}
			else
			{
				bIssueDefProc = true;
			}
			break;
		}
		
		if(true == bIssueDefProc)
		{
			lReturnCode = DefWindowProc(hWnd, msg, wp, lp);
		}

		return(lReturnCode);
	}

	bool Init()
	{
		g_MsgIDSkypeControlAPIAttach = ::RegisterWindowMessage(_T("SkypeControlAPIAttach"));
		g_MsgIDSkypeControlAPIDiscover = ::RegisterWindowMessage(_T("SkypeControlAPIDiscover"));
		if((0 == g_MsgIDSkypeControlAPIAttach)|| (0 == g_MsgIDSkypeControlAPIDiscover))
		{
			return false;
		}

		WNDCLASS oWindowClass;
		oWindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
		oWindowClass.lpfnWndProc = (WNDPROC)&SkypeAPI_WindowProc;
		oWindowClass.cbClsExtra = 0;
		oWindowClass.cbWndExtra = 0;
		oWindowClass.hInstance = g_hModule;
		oWindowClass.hIcon = NULL;
		oWindowClass.hCursor = NULL;
		oWindowClass.hbrBackground = NULL;
		oWindowClass.lpszMenuName = NULL;
		oWindowClass.lpszClassName = g_pszSkypeWndClassName;

		if(0 == RegisterClass(&oWindowClass))
		{
			return false;
		}

		g_wndMainWindow = CreateWindowEx( WS_EX_APPWINDOW|WS_EX_WINDOWEDGE,
			g_pszSkypeWndClassName,_T(""),
			WS_BORDER|WS_SYSMENU|WS_MINIMIZEBOX,
			CW_USEDEFAULT, CW_USEDEFAULT, 128, 128, NULL, 0, g_hModule, 0);
		
		return(NULL != g_wndMainWindow);
	}

}

/******************************* INSTALLATION PROCEDURES *****************************/


// DLL stuff

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	if(DLL_PROCESS_ATTACH == fdwReason)
	{
		g_hModule = hinstDLL;
	}

	return TRUE;
}

extern "C"__declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
	return &g_pluginInfo;
}

#define MIID_SKYPE_STATUS_CHANGE	{0xf3c066de, 0xc2c2, 0x40d6, { 0xbc, 0xb4, 0xd5, 0xf4, 0xd5, 0xe7, 0x79, 0x62}}

extern "C" __declspec(dllexport) const MUUID* MirandaPluginInterfaces()
{
	static const MUUID interfaces[] = {MIID_SKYPE_STATUS_CHANGE,MIID_LAST};
	return interfaces;
}

namespace
{
	HANDLE g_hEventProtocolAck = NULL;
	HANDLE g_hEventModulesLoaded = NULL;
	HANDLE g_hEventPreShutdown = NULL;

	int SSC_OnModulesLoaded(WPARAM/* wParam*/, LPARAM/* lParam*/) 
	{
		if(ServiceExists(MS_UPDATE_REGISTER)) 
		{
			Update update = {0};
			char szVersion[16];

			update.cbSize = sizeof(update);
			update.szComponentName = g_pluginInfo.shortName;
			update.szUpdateURL = UPDATER_AUTOREGISTER;

			update.szBetaVersionURL = "http://code.google.com/p/dioksin/downloads/list";
#ifdef WIN64
			update.szBetaUpdateURL = "http://dioksin.googlecode.com/files/SkypeStatusChange64_%VERSION%.zip";
			update.pbBetaVersionPrefix = (BYTE*) "Skype Status Change (x64) ";
#else
			update.szBetaUpdateURL = "http://dioksin.googlecode.com/files/SkypeStatusChange_%VERSION%.zip";
			update.pbBetaVersionPrefix = (BYTE*) "Skype Status Change ";
#endif
			update.cpbBetaVersionPrefix = static_cast<int>(strlen((char *)update.pbBetaVersionPrefix));

			update.pbVersion = (BYTE*)CreateVersionStringPluginEx(&g_pluginInfo, szVersion);
			update.cpbVersion = static_cast<int>(strlen((char *)update.pbVersion));

			update.szBetaChangelogURL = "http://code.google.com/p/dioksin/source/list";

			CallService(MS_UPDATE_REGISTER,0,(WPARAM)&update);
		}
		return 0;
	}


	int SSC_OnPreShutdown(WPARAM/* wParam*/, LPARAM/* lParam*/)
	{
		g_bMirandaIsShutdount = true;
		BOOL b = SetEvent(g_hEventShutdown);
		assert(b && "SetEvent failed");

		DWORD dwResult = ::WaitForSingleObject(g_hThread,INFINITE);
		assert(WAIT_FAILED != dwResult);

// 		if(WAIT_FAILED != dwResult)
// 			::MessageBox(NULL,_T("Ok"),_T("Skype Status Changed"),MB_OK);
// 		else
// 			::MessageBox(NULL,_T("Wrong"),_T("Skype Status Changed"),MB_OK);

		b = ::CloseHandle(g_hEventShutdown);
		assert(b && "CloseHandle event");
		b = ::CloseHandle(g_hThread);
		assert(b && "CloseHandle thread");

		if(NULL != g_wndMainWindow)
		{
			b = DestroyWindow(g_wndMainWindow);
			assert(b && "DestoryWindow");
			g_wndMainWindow = NULL;
		}

		UnregisterClass(g_pszSkypeWndClassName,g_hModule);

		return 0;
	}

	struct CTreeItemData
	{
		enum EType
		{
			Protocol,
			Status
		};

		EType m_nType;
		char* m_pszModule;
		int m_nStatus;

	};

	enum ETreeCheckBoxState
	{
		// tree check box state
		TCBS_NOSTATEBOX = 0,
		TCBS_UNCHECKED = 1,
		TCBS_CHECKED = 2,
		TCBS_DISABLE_UNCHECKED = 3,
		TCBS_DISABLE_CHECKED = 4,
// 		TREE_VIEW_CHECK_STATE_CHANGE = WM_USER + 100,
	};

	HTREEITEM tree_insert_item(HWND hDlg,
							   HWND hwndTree,
							   TCHAR* pName,
							   HTREEITEM htiParent,
							   ETreeCheckBoxState nState,
							   CTreeItemData* pData)
	{
		USES_CONVERSION;

		TVINSERTSTRUCT tvi;
		ZeroMemory(&tvi,sizeof(tvi));

		tvi.hParent = htiParent;
		tvi.hInsertAfter = TVI_LAST;
		tvi.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_STATE;
		tvi.item.pszText = pName;
		tvi.item.lParam = reinterpret_cast<LPARAM>(pData);
		tvi.item.stateMask = TVIS_STATEIMAGEMASK;
		tvi.item.state = INDEXTOSTATEIMAGEMASK(nState);

		HTREEITEM hti = TreeView_InsertItem(hwndTree,&tvi);
// 		if(hti && (TCBS_UNCHECKED != nState) && (TCBS_NOSTATEBOX != nState))
// 		{
// 			::PostMessage(hDlg,TREE_VIEW_CHECK_STATE_CHANGE,MAKEWPARAM(0,nState),reinterpret_cast<LPARAM>(hti));
// 		}
		return hti;

	}

	void InitProtocolTree(HWND hDlg,HWND hwndTreeCtrl)
	{
		int cAccounts = 0;
		PROTOACCOUNT** ppAccount;

		enum{OFFLINE_STATUS_INDEX = 5};

		ProtoEnumAccounts(&cAccounts, &ppAccount);
		for(int i = 0; i < cAccounts;++i)
		{
			PROTOACCOUNT* pAccount = ppAccount[i];
			CTreeItemData* pItemData = new CTreeItemData;
			pItemData->m_nType = CTreeItemData::Protocol;
			pItemData->m_pszModule = pAccount->szModuleName;

			bool bProtocolExcluded = g_Options.IsProtocolExcluded(pAccount->szModuleName);
			HTREEITEM hti = tree_insert_item(hDlg,hwndTreeCtrl,pAccount->tszAccountName,TVI_ROOT,((true == bProtocolExcluded) ? TCBS_CHECKED : TCBS_UNCHECKED),pItemData);
			if(hti)
			{
				int nStatusBits = CallProtoService(pAccount->szModuleName,PS_GETCAPS,PFLAGNUM_2,0);
				int nStatusExcluded = CallProtoService(pAccount->szModuleName,PS_GETCAPS,PFLAGNUM_5,0);
				pItemData = new CTreeItemData;
				pItemData->m_nType = CTreeItemData::Status;
				pItemData->m_pszModule = pAccount->szModuleName;
				pItemData->m_nStatus = ID_STATUS_OFFLINE;
				bool bStatusExcluded = g_Options.IsProtocolStatusExcluded(pAccount->szModuleName,pItemData->m_nStatus);
				ETreeCheckBoxState nState = TCBS_UNCHECKED;
				if(bProtocolExcluded)
				{
					if(bStatusExcluded)
					{
						nState = TCBS_DISABLE_CHECKED;
					}
					else
					{
						nState = TCBS_DISABLE_UNCHECKED;
					}
				}
				else
				{
					if(bStatusExcluded)
					{
						nState = TCBS_CHECKED;
					}
					else
					{
						nState = TCBS_UNCHECKED;
					}
				}
				tree_insert_item(hDlg,hwndTreeCtrl,TranslateTS(g_aStatusCode[OFFLINE_STATUS_INDEX].m_ptszStatusName),hti,nState,pItemData);
				for(size_t k = 0;k < g_cStatusCode;++k)
				{
					const CMirandaStatus2SkypeStatus& m2s = g_aStatusCode[k];
					unsigned long statusFlags = Proto_Status2Flag(m2s.m_nMirandaStatus);
					if((m2s.m_nMirandaStatus != ID_STATUS_OFFLINE) && (nStatusBits & statusFlags) && !(nStatusExcluded & statusFlags))
					{
						pItemData = new CTreeItemData;
						pItemData->m_nType = CTreeItemData::Status;
						pItemData->m_pszModule = pAccount->szModuleName;
						pItemData->m_nStatus = m2s.m_nMirandaStatus;
						bool bStatusExcluded = g_Options.IsProtocolStatusExcluded(pAccount->szModuleName,pItemData->m_nStatus);
						if(bProtocolExcluded)
						{
							if(bStatusExcluded)
							{
								nState = TCBS_DISABLE_CHECKED;
							}
							else
							{
								nState = TCBS_DISABLE_UNCHECKED;
							}
						}
						else
						{
							if(bStatusExcluded)
							{
								nState = TCBS_CHECKED;
							}
							else
							{
								nState = TCBS_UNCHECKED;
							}
						}

						tree_insert_item(hDlg,hwndTreeCtrl,TranslateTS(m2s.m_ptszStatusName),hti,nState,pItemData);
					}
				}

				TreeView_Expand(hwndTreeCtrl,hti,TVE_EXPAND);
			}
		}
	}

	inline HTREEITEM tree_get_child_item(HWND hwndTreeCtrl,HTREEITEM hti)
	{
		return TreeView_GetChild(hwndTreeCtrl,hti);
	}

	inline HTREEITEM tree_get_next_sibling_item(HWND hwndTreeCtrl,HTREEITEM hti)
	{
		return TreeView_GetNextSibling(hwndTreeCtrl,hti);
	}

	const CTreeItemData* get_item_data(HWND hwndTreeCtrl,HTREEITEM hti)
	{
		TVITEM tvi = {0};
		tvi.hItem = hti;
		tvi.mask = TVIF_PARAM|TVIF_HANDLE;
		if(TRUE == ::SendMessage(hwndTreeCtrl,TVM_GETITEM,0,reinterpret_cast<LPARAM>(&tvi)))
		{
			CTreeItemData* pData = reinterpret_cast<CTreeItemData*>(tvi.lParam);
			return pData;
		}
		else
		{
			return NULL;
		}
	}


	inline ETreeCheckBoxState tree_get_state_image(HWND hwndTree,HTREEITEM hti)
	{
		TVITEM   tvi;
		tvi.hItem = hti;
		tvi.mask = TVIF_STATE|TVIF_HANDLE;
		tvi.stateMask = TVIS_STATEIMAGEMASK;
		if(TRUE == ::SendMessage(hwndTree,TVM_GETITEM,0,reinterpret_cast<LPARAM>(&tvi)))
		{
			UINT nState = (tvi.state >> 12);
			return static_cast<ETreeCheckBoxState>(nState);
		}
		else
		{
			assert(!"we should never get here!");
			return TCBS_UNCHECKED;
		}
	}



	void FreeMemory(HWND hwndTreeCtrl,HTREEITEM hti)
	{
		for(HTREEITEM h = tree_get_child_item(hwndTreeCtrl,hti);h;h = tree_get_next_sibling_item(hwndTreeCtrl,h))
		{
			FreeMemory(hwndTreeCtrl,h);
			const CTreeItemData* pData = get_item_data(hwndTreeCtrl,h);
			if(pData)
			{
				delete pData;
			}
		}
	}

	bool tree_set_item_state(HWND hwndTree,HTREEITEM hti,ETreeCheckBoxState nState)
	{
		TVITEM tvi;
		ZeroMemory(&tvi,sizeof(tvi));

		tvi.mask = TVIF_STATE|TVIF_HANDLE;
		tvi.hItem = hti;

		tvi.stateMask = TVIS_STATEIMAGEMASK;
		tvi.state = INDEXTOSTATEIMAGEMASK(nState);

		return TRUE == ::SendMessage(hwndTree,TVM_SETITEM,0,reinterpret_cast<LPARAM>(&tvi));
	}

	void disable_children(HWND hwndTree,HTREEITEM htiParent,bool bDisable)
	{
		for(HTREEITEM hti = tree_get_child_item(hwndTree,htiParent);hti;hti = tree_get_next_sibling_item(hwndTree,hti))
		{
			ETreeCheckBoxState nState = tree_get_state_image(hwndTree,hti);
			if(bDisable)
			{
				if(TCBS_CHECKED == nState)
				{
					nState = TCBS_DISABLE_CHECKED;
				}
				else if(TCBS_UNCHECKED == nState)
				{
					nState = TCBS_DISABLE_UNCHECKED;
				}
			}
			else
			{
				if(TCBS_DISABLE_CHECKED == nState)
				{
					nState = TCBS_CHECKED;
				}
				else if(TCBS_DISABLE_UNCHECKED == nState)
				{
					nState = TCBS_UNCHECKED;
				}
			}
			tree_set_item_state(hwndTree,hti,nState);
		}
	}

	void save_exclusion_list(HWND hwndTree,HTREEITEM htiParent)
	{
		for(HTREEITEM hti = tree_get_child_item(hwndTree,htiParent);hti;hti = tree_get_next_sibling_item(hwndTree,hti))
		{
			const CTreeItemData* pData = get_item_data(hwndTree,hti);
			ETreeCheckBoxState nState = tree_get_state_image(hwndTree,hti);
			if(CTreeItemData::Protocol == pData->m_nType)
			{
				g_Options.ExcludeProtocol(pData->m_pszModule,TCBS_CHECKED == nState);
				save_exclusion_list(hwndTree,hti);
			}
			else
			{
				g_Options.ExcludeProtocolStatus(pData->m_pszModule,pData->m_nStatus,((TCBS_CHECKED == nState) || (TCBS_DISABLE_CHECKED == nState)));
			}
		}			
	}

	class CImageListWrapper
	{
	public:
		CImageListWrapper()
			: m_hImageList(ImageList_LoadImage(g_hModule,MAKEINTRESOURCE(IDB_TREE_STATE),16,0,RGB(255,255,255),IMAGE_BITMAP,LR_DEFAULTCOLOR))
		{
			
		}

		~CImageListWrapper()
		{
			if(m_hImageList)
			{
				ImageList_Destroy(m_hImageList);
			}
		}

		operator HIMAGELIST()const
		{
			return m_hImageList;
		}

	private:
		HIMAGELIST m_hImageList;
	};

	HIMAGELIST get_image_list()
	{
		static CImageListWrapper wrapper;
		return wrapper;
	}

	INT_PTR CALLBACK SettingsDlgProc(HWND hdlg,UINT msg,WPARAM wp,LPARAM lp)
	{
		switch(msg)
		{
		case WM_INITDIALOG:
			{
				TranslateDialogDefault(hdlg);
				HIMAGELIST hImage = get_image_list();
				HWND hwndTreeCtrl = GetDlgItem(hdlg,IDC_TREE_PROTOCOLS);
				if(hImage)
				{
					//int cImages = ImageList_GetImageCount(hImage);
					TreeView_SetImageList(hwndTreeCtrl,hImage,TVSIL_STATE);
				}
				InitProtocolTree(hdlg,hwndTreeCtrl);
				CheckDlgButton(hdlg,IDC_CHECK_SYNCK_STATUS_MSG,(true == g_Options.GetSyncStatusMsgFlag()) ? 1 : 0);
				CheckDlgButton(hdlg,IDC_CHECK_STATUSES,(true == g_Options.GetSyncStatusStateFlag()) ? 1 : 0);				
			}
			return TRUE;
		case WM_NOTIFY:
			{
				LPNMHDR pNMHDR = reinterpret_cast<LPNMHDR>(lp);
				switch(pNMHDR->code)
				{
				case PSN_APPLY:
					{
						save_exclusion_list(GetDlgItem(hdlg,IDC_TREE_PROTOCOLS),TVI_ROOT);
						g_Options.SetSyncStatusMsgFlag(1 == IsDlgButtonChecked(hdlg,IDC_CHECK_SYNCK_STATUS_MSG));
						g_Options.SetSyncStatusStateFlag(1 == IsDlgButtonChecked(hdlg,IDC_CHECK_STATUSES));
					}
					break;
				case NM_CLICK:
					if(IDC_TREE_PROTOCOLS == wp)
					{
						DWORD pos = ::GetMessagePos();

						HWND hwndTree = ::GetDlgItem(hdlg,IDC_TREE_PROTOCOLS);

						TVHITTESTINFO tvhti;
						tvhti.pt.x = LOWORD(pos);
						tvhti.pt.y = HIWORD(pos);
						::ScreenToClient(hwndTree,&(tvhti.pt));

						HTREEITEM hti = reinterpret_cast<HTREEITEM>(::SendMessage(hwndTree,TVM_HITTEST,0,reinterpret_cast<LPARAM>(&tvhti)));
						if(hti && (tvhti.flags&(TVHT_ONITEMSTATEICON|TVHT_ONITEMICON)))
						{
							ETreeCheckBoxState nState = tree_get_state_image(hwndTree,hti);
							if(TCBS_CHECKED == nState || TCBS_UNCHECKED == nState)
							{
								if(TCBS_CHECKED == nState)
								{
									nState = TCBS_UNCHECKED;
								}
								else
								{
									nState = TCBS_CHECKED;
								}
								tree_set_item_state(hwndTree,hti,nState);
								disable_children(hwndTree,hti,TCBS_CHECKED == nState);
								PropSheet_Changed(::GetParent(hdlg),hdlg);
							}
						}
					}
					break;
				}
			}
			break;
		case WM_COMMAND:
			if(BN_CLICKED == HIWORD(wp) 
				&& ((IDC_CHECK_SYNCK_STATUS_MSG == LOWORD(wp))
				|| (IDC_CHECK_STATUSES == LOWORD(wp))))
			{
				PropSheet_Changed(::GetParent(hdlg),hdlg);
			}
			break;
		case WM_DESTROY:
			FreeMemory(GetDlgItem(hdlg,IDC_TREE_PROTOCOLS),TVI_ROOT);
			break;
// 		case TREE_VIEW_CHECK_STATE_CHANGE:
// 			if(1 == LOWORD(wp))
// 			{
// 				HWND hwndTree = ::GetDlgItem(hdlg,IDC_TREE_PROTOCOLS);
// 				HTREEITEM hti = reinterpret_cast<HTREEITEM>(lp);
// 				const CTreeItemData* pData = get_item_data(hwndTree,hti);
// 				if(CTreeItemData::Protocol == pData->m_nType)
// 				{
// 					ETreeCheckBoxState nState = tree_get_state_image(hwndTree,hti);
// 					disable_children(hwndTree,hti,TCBS_CHECKED == nState);
// 				}
// 			}
// 			else
// 			{
// 				HTREEITEM hti = reinterpret_cast<HTREEITEM>(lp);
// 				ETreeCheckBoxState nState = static_cast<ETreeCheckBoxState>(HIWORD(wp));
// 				tree_set_item_state(::GetDlgItem(hdlg,IDC_TREE_PROTOCOLS),hti,nState);
// 			}
// 			break;
		}
		return FALSE;
	}

	int SSC_OptInitialise(WPARAM wp, LPARAM lp)
	{
		OPTIONSDIALOGPAGE odp;
		ZeroMemory(&odp,sizeof(odp));

		odp.cbSize = sizeof(odp);
		odp.position = 910000000;
		odp.hInstance = g_hModule;
		odp.ptszTitle = LPGENT("Change Skype Status");
		odp.ptszGroup = LPGENT("Plugins");
		odp.hIcon = NULL;
		odp.flags = ODPF_TCHAR;
		odp.pszTemplate = MAKEINTRESOURCEA(IDD_DIALOG_SETTINGS);
		odp.pfnDlgProc = SettingsDlgProc;
		CallService(MS_OPT_ADDPAGE,wp,reinterpret_cast<LPARAM>(&odp));
	
		return 0;
	}
}

extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	pluginLink = link;
	mir_getMMI( &mmi );
	mir_getUTFI( &utfi );
	mir_getLP(&g_pluginInfo);
	if(false == Init())
	{

		return 1;
	}

	g_bMirandaIsShutdount = false;
	g_hEventShutdown = ::CreateEvent(NULL,TRUE,FALSE,NULL);

	g_hThread = reinterpret_cast<HANDLE>(::_beginthreadex(NULL,0,ThreadFunc,NULL,0,NULL));
		//reinterpret_cast<HANDLE>(mir_forkthread(ThreadFunc,0));
	g_hEventProtocolAck = HookEvent(ME_PROTO_ACK,SSC_OnProtocolAck);
	g_hEventModulesLoaded = HookEvent(ME_SYSTEM_MODULESLOADED,SSC_OnModulesLoaded);
	g_hEventPreShutdown = HookEvent(ME_SYSTEM_PRESHUTDOWN,SSC_OnPreShutdown);
	g_hOptInitialize = HookEvent(ME_OPT_INITIALISE,SSC_OptInitialise);
    return 0;
}

extern "C" __declspec(dllexport) int Unload(void)		// Executed on DLL unload
{
	UnhookEvent(g_hEventProtocolAck);
	UnhookEvent(g_hEventPreShutdown);
	UnhookEvent(g_hEventPreShutdown);
	UnhookEvent(g_hOptInitialize);


	return 0;
}
