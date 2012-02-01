/*
UserinfoEx plugin for Miranda IM

Copyright:
� 2006-2010 DeathAxe, Yasnovidyashii, Merlin, K. Romanov, Kreol

part of this code based on:
Miranda IM Country Flags Plugin Copyright �2006-2007 H. Herkenrath

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

===============================================================================

File name      : $HeadURL: http://userinfoex.googlecode.com/svn/trunk/Flags/svc_flags.cpp $
Revision       : $Revision: 187 $
Last change on : $Date: 2010-09-08 16:05:54 +0400 (Ср, 08 сен 2010) $
Last change by : $Author: ing.u.horn $

===============================================================================
*/

#include "..\src\commonheaders.h"
#include "svc_flags.h"
#include "svc_flagsicons.h"
#include "svc_countrylistext.h"

#define M_ENABLE_SUBCTLS			(WM_APP+1)

FLAGSOPTIONS	gFlagsOpts;

/* Misc */
int		nCountriesCount;
struct	CountryListEntry *countries;
static	HANDLE *phExtraImageList	= NULL;		//return value(s) from MS_CLIST_EXTRA_ADD_ICON
static	HANDLE hExtraIconSvc		= INVALID_HANDLE_VALUE;
/* hook */
static HANDLE hRebuildIconsHook		= NULL;
static HANDLE hApplyIconHook		= NULL;
static HANDLE hMsgWndEventHook		= NULL;
static HANDLE hIconsChangedHook		= NULL;
static HANDLE hSettingChangedHook	= NULL;

static HANDLE hExtraIconSvcHook		= NULL;
static HANDLE hOptInitHook			= NULL;

static int OnContactSettingChanged(WPARAM wParam,LPARAM lParam);

/***********************************************************************************************************
 * Buffered functions
 ***********************************************************************************************************/

struct BufferedCallData {
	DWORD startTick;
	UINT uElapse;
	BUFFEREDPROC pfnBuffProc;
	LPARAM lParam;
	#ifdef _DEBUG
	const char *pszProcName;
	#endif
};

static UINT_PTR idBufferedTimer;
static struct BufferedCallData *callList;
static int nCallListCount;

// always gets called in main message loop
static void CALLBACK BufferedProcTimer(HWND hwnd, UINT msg, UINT idTimer, DWORD currentTick) {
	int i;
	struct BufferedCallData *buf;
	UINT uElapsed, uElapseNext = USER_TIMER_MAXIMUM;
	BUFFEREDPROC pfnBuffProc;
	LPARAM lParam;
	#ifdef _DEBUG
	char szDbgLine[256];
	const char *pszProcName;
	#endif
	UNREFERENCED_PARAMETER(msg);

	for(i=0;i<nCallListCount;++i) {
		/* find elapsed procs */
		uElapsed=currentTick-callList[i].startTick; /* wraparound works */
		if((uElapsed+USER_TIMER_MINIMUM)>=callList[i].uElapse) { 
			/* call elapsed proc */
			pfnBuffProc=callList[i].pfnBuffProc;
			lParam=callList[i].lParam;
			#ifdef _DEBUG
				pszProcName=callList[i].pszProcName;
			#endif
			/* resize storage array */
			if((i+1)<nCallListCount)
				MoveMemory(&callList[i],&callList[i+1],((nCallListCount-i-1)*sizeof(struct BufferedCallData)));
			--nCallListCount;
			--i; /* reiterate current */
			if(nCallListCount) {
				buf=(struct BufferedCallData*)mir_realloc(callList,nCallListCount*sizeof(struct BufferedCallData));
				if(buf!=NULL) callList=buf;
			} else {
				mir_free(callList);
				callList=NULL;
			}
			#ifdef _DEBUG
				mir_snprintf(szDbgLine,sizeof(szDbgLine),"buffered call: %s(0x%X)\n",pszProcName,lParam); /* all ascii */
				OutputDebugStringA(szDbgLine);
			#endif
			CallFunctionAsync((void (CALLBACK *)(void*))pfnBuffProc,(void*)lParam); /* compatible */
		}
		/* find next timer delay */
		else if((callList[i].uElapse-uElapsed)<uElapseNext)
			uElapseNext=callList[i].uElapse-uElapsed;
	}

	/* set next timer */
	if(nCallListCount) {
		#ifdef _DEBUG
			mir_snprintf(szDbgLine,sizeof(szDbgLine),"next buffered timeout: %ums\n",uElapseNext); /* all ascii */
			OutputDebugStringA(szDbgLine);
		#endif
		idBufferedTimer=SetTimer(hwnd, idBufferedTimer, uElapseNext, (TIMERPROC)BufferedProcTimer); /* will be reset */
	} else {
		KillTimer(hwnd,idTimer);
		idBufferedTimer=0;
		#ifdef _DEBUG
			OutputDebugStringA("empty buffered queue\n");
		#endif
	}
}

// assumes to be called in context of main thread
#ifdef _DEBUG
void _CallFunctionBuffered(BUFFEREDPROC pfnBuffProc,const char *pszProcName,LPARAM lParam,BOOL fAccumulateSameParam,UINT uElapse)
#else
void _CallFunctionBuffered(BUFFEREDPROC pfnBuffProc,LPARAM lParam,BOOL fAccumulateSameParam,UINT uElapse)
#endif
{
	struct BufferedCallData *data=NULL;
	int i;

	/* find existing */
	for(i=0;i<nCallListCount;++i)
		if(callList[i].pfnBuffProc==pfnBuffProc)
			if(!fAccumulateSameParam || callList[i].lParam==lParam) {
				data=&callList[i];
				break;
			}
	/* append new */
	if(data==NULL) {
		/* resize storage array */
		data=(struct BufferedCallData*)mir_realloc(callList,(nCallListCount+1)*sizeof(struct BufferedCallData));
		if(data==NULL) return;
		callList=data;
		data=&callList[nCallListCount];
		++nCallListCount;
	}
	/* set delay */
	data->startTick=GetTickCount();
	data->uElapse=uElapse;
	data->lParam=lParam;
	data->pfnBuffProc=pfnBuffProc;
	#ifdef _DEBUG
		{	char szDbgLine[256];
			data->pszProcName=pszProcName;
			mir_snprintf(szDbgLine,sizeof(szDbgLine),"buffered queue: %s(0x%X)\n",pszProcName,lParam); /* all ascii */
			OutputDebugStringA(szDbgLine);
			if(!idBufferedTimer) {
				mir_snprintf(szDbgLine,sizeof(szDbgLine),"next buffered timeout: %ums\n",uElapse); /* all ascii */
				OutputDebugStringA(szDbgLine);
			}
		}
	#endif
	/* set next timer */
	if(idBufferedTimer) uElapse=USER_TIMER_MINIMUM; /* will get recalculated */
	idBufferedTimer=SetTimer(NULL,idBufferedTimer,uElapse,(TIMERPROC)BufferedProcTimer);
}

// assumes to be called in context of main thread
void PrepareBufferedFunctions()
{
	idBufferedTimer=0;
	nCallListCount=0;
	callList=NULL;
}

// assumes to be called in context of main thread
void KillBufferedFunctions()
{
	if(idBufferedTimer) KillTimer(NULL,idBufferedTimer);
	nCallListCount=0;
	mir_free(callList); /* does NULL check */
}

/***********************************************************************************************************
 * service functions
 ***********************************************************************************************************/

static INT_PTR ServiceDetectContactOriginCountry(WPARAM wParam,LPARAM lParam)
{
	WORD countryNumber;
	char *pszProto = 
	pszProto=(char*)CallService(MS_PROTO_GETCONTACTBASEPROTO,wParam,0);
	/* UserinfoEx */
	if (countryNumber = (int)DB::Setting::GetWord((HANDLE)wParam,USERINFO,SET_CONTACT_ORIGIN_COUNTRY,0))
		return (INT_PTR)countryNumber;
	else if (countryNumber = (int)DB::Setting::GetWord((HANDLE)wParam,USERINFO,SET_CONTACT_COUNTRY,0))
		return (INT_PTR)countryNumber;
	else if (countryNumber = (int)DB::Setting::GetWord((HANDLE)wParam,USERINFO,SET_CONTACT_COMPANY_COUNTRY,0))
		return (INT_PTR)countryNumber;
	/* fallback proto settings */
	else if (countryNumber = (int)DB::Setting::GetWord((HANDLE)wParam,pszProto,"Country",0))
		return (INT_PTR)countryNumber;
	else if (countryNumber = (int)DB::Setting::GetWord((HANDLE)wParam,pszProto,"CompanyCountry",0))
		return (INT_PTR)countryNumber;
	/* fallback ip detect
	else if(countryNumber==0xFFFF && DBGetContactSettingByte(NULL,"Flags","UseIpToCountry",SETTING_USEIPTOCOUNTRY_DEFAULT)){
		countryNumber=ServiceIpToCountry(DBGetContactSettingDword((HANDLE)wParam,pszProto,"RealIP",0),0);
	}*/

	return (INT_PTR)0xFFFF;
}

/***********************************************************************************************************
 * Clist Extra Image functions
 ***********************************************************************************************************/

static void CALLBACK SetExtraImage(LPARAM lParam) {
	/* get contact's country */
	int countryNumber = ServiceDetectContactOriginCountry((WPARAM)lParam,0);;
	//use ExtraIconsService ?
	if (myGlobals.ExtraIconsServiceExist) {
		EXTRAICON ico;
		ico.cbSize		= sizeof(ico);
		ico.hContact	= (HANDLE)lParam;
		ico.hExtraIcon	= hExtraIconSvc;
		ico.icoName		= (char*)0;		//preset
		if(countryNumber!=0xFFFF || gFlagsOpts.bUseUnknownFlag) {
			char szId[20];
			mir_snprintf(szId, SIZEOF(szId), (countryNumber==0xFFFF)?"%s_0x%X":"%s_%i","flags",countryNumber); /* buffer safe */
			ico.icoName	= szId;
		}
		CallService(MS_EXTRAICON_SET_ICON, (WPARAM)&ico, 0);
	}
	//use Clist ExtraImageService
	else if(gFlagsOpts.bShowExtraImgFlag) {
		int index;
		IconExtraColumn iec;
		iec.cbSize		= sizeof(iec);
		iec.ColumnType	= gFlagsOpts.idExtraColumn;
		iec.hImage		= INVALID_HANDLE_VALUE;		//preset
		/* get icon for contact */
		if(phExtraImageList!=NULL) { /* too early? */
			if(countryNumber!=0xFFFF || gFlagsOpts.bUseUnknownFlag) {
				index = CountryNumberToIndex(countryNumber);
				/* icon not add to clist extra image list? */
				if(phExtraImageList[index]==INVALID_HANDLE_VALUE) {
					HICON hIcon=LoadFlag(countryNumber);	// Returned HICON SHOULDN'T be destroyed, it is managed by IcoLib
					if(hIcon!=NULL) {
						phExtraImageList[index]=(HANDLE)CallService(MS_CLIST_EXTRA_ADD_ICON,(WPARAM)hIcon,0);
						CallService(MS_SKIN2_RELEASEICON,(WPARAM)hIcon,0); /* does NULL check */
					}
				}
				iec.hImage=phExtraImageList[index];
			}
		}
		//Set icon for contact at needed column
		CallService(MS_CLIST_EXTRA_SET_ICON,(WPARAM)lParam,(LPARAM)&iec);
	}
}

static void CALLBACK RemoveExtraImages(LPARAM lParam) {
	register HANDLE hContact;
	//use ExtraIconsService ?
	if (myGlobals.ExtraIconsServiceExist) {
		EXTRAICON ico;
		ico.cbSize			= sizeof(ico);
		ico.hExtraIcon		= hExtraIconSvc;
		ico.icoName			= (char *)0;		/* invalidate icon for contact*/
		/* enum all contacts */
		for (hContact = DB::Contact::FindFirst(); hContact != NULL; hContact = DB::Contact::FindNext(hContact)) {
			ico.hContact	= hContact;
			CallService(MS_EXTRAICON_SET_ICON, (WPARAM)&ico, 0);
		}
	}
	//use Clist ExtraImageService
	else {
		IconExtraColumn iec;
		iec.cbSize		= sizeof(iec);
		iec.ColumnType	= gFlagsOpts.idExtraColumn;
		iec.hImage		= INVALID_HANDLE_VALUE;
		/* enum all contacts */
		for (hContact = DB::Contact::FindFirst(); hContact != NULL; hContact = DB::Contact::FindNext(hContact)) {
			/* invalidate icon for contact*/
			CallService(MS_CLIST_EXTRA_SET_ICON,(WPARAM)hContact,(LPARAM)&iec);
		}
	}
}

// always call in context of main thread
void EnsureExtraImages()		 //garantieren - sicherstellen - updaten
{
	register HANDLE hContact;
	//use Clist ExtraImageService?
	if(!myGlobals.ExtraIconsServiceExist) {
		BYTE idExtraColumnNew = DB::Setting::GetByte(MODNAMEFLAGS,"ExtraImgFlagColumn",SETTING_EXTRAIMGFLAGCOLUMN_DEFAULT);
		if(idExtraColumnNew != gFlagsOpts.idExtraColumn) {
			/* clear previous column */
			RemoveExtraImages(0);
			gFlagsOpts.idExtraColumn = idExtraColumnNew;
			/* clear new column */
			RemoveExtraImages(0);
		}
	}
	/* enum all contacts */
	for (hContact = DB::Contact::FindFirst(); hContact != NULL; hContact = DB::Contact::FindNext(hContact)) {
		/* update icon */
		CallFunctionBuffered(SetExtraImage,(LPARAM)hContact,TRUE,EXTRAIMAGE_REFRESHDELAY);
	}
}

static void CALLBACK UpdateExtraImages(LPARAM lParam) {
	if(!lParam)
		 RemoveExtraImages(0);
	else EnsureExtraImages();

/*	if(!myGlobals.ExtraIconsServiceExist && !gFlagsOpts.bShowExtraImgFlag)
		 RemoveExtraImages();
	else EnsureExtraImages();  */
}

//hookProc ME_CLIST_EXTRA_LIST_REBUILD
static int OnCListRebuildIcons(WPARAM wParam,LPARAM lParam) {
	OutputDebugStringA("REBUILD EXTRA\n");
	//use ExtraIconsService ?
	if(myGlobals.ExtraIconsServiceExist) return 0;			//delete ?
	//use Clist ExtraImageService
	if(phExtraImageList!=NULL) {
		/* invalidate icons */
		for(int i=0;i<nCountriesCount;++i)
			phExtraImageList[i]=INVALID_HANDLE_VALUE;
	}
	/* update column */
	gFlagsOpts.idExtraColumn = DB::Setting::GetByte(MODNAMEFLAGS,"ExtraImgFlagColumn",SETTING_EXTRAIMGFLAGCOLUMN_DEFAULT);
	return 0;
}

//hookProc ME_CLIST_EXTRA_IMAGE_APPLY
static int OnCListApplyIcons(WPARAM wParam,LPARAM lParam) {
	//OutputDebugStringA("APPLY EXTRA\n");
	if(myGlobals.ExtraIconsServiceExist || gFlagsOpts.bShowExtraImgFlag) 
		SetExtraImage((LPARAM)wParam); /* unbuffered */
	return 0;
}

//hookProc (ME_DB_CONTACT_SETTINGCHANGED) - workaround for missing event from ExtraIconSvc
static int OnExtraIconSvcChanged(WPARAM wParam,LPARAM lParam) {
	DBCONTACTWRITESETTING *dbcws=(DBCONTACTWRITESETTING*)lParam;
	if((HANDLE)wParam!=NULL)return 0;
	if(!lstrcmpA(dbcws->szModule, "ExtraIcons") &&
	   !lstrcmpA(dbcws->szSetting,"Slot_Flags") ){
			BOOL bEnable;
			switch (dbcws->value.type) {
				case DBVT_BYTE:
					bEnable = dbcws->value.bVal != (BYTE)-1;
					break;
				case DBVT_WORD:
					bEnable = dbcws->value.wVal != (WORD)-1;
					break;
				case DBVT_DWORD:
					bEnable = dbcws->value.dVal != (DWORD)-1;
					break;
				default:
					bEnable = -1;
					break;
			}
			if(bEnable == -1) {
				return 0;
			}
			else if(bEnable && !hApplyIconHook) {
					hApplyIconHook = HookEvent(ME_CLIST_EXTRA_IMAGE_APPLY, OnCListApplyIcons);
			}
			else if(!bEnable && hApplyIconHook) {
				UnhookEvent(hApplyIconHook); hApplyIconHook = NULL;
			}
			CallFunctionBuffered(UpdateExtraImages,(LPARAM)bEnable,FALSE,EXTRAIMAGE_REFRESHDELAY);
	}
	return 0;
}

VOID SvcFlagsEnableExtraIcons(BYTE bColumn, BOOLEAN bUpdateDB) {
	if (!myGlobals.HaveCListExtraIcons) return;
	gFlagsOpts.bShowExtraImgFlag = (bColumn!=((BYTE)-1));
	if (bUpdateDB) {
		if(gFlagsOpts.bShowExtraImgFlag) DB::Setting::WriteByte(MODNAMEFLAGS,"ExtraImgFlagColumn", bColumn);
		DB::Setting::WriteByte(MODNAMEFLAGS,"ShowExtraImgFlag", bColumn!=(BYTE)-1);
	}

	// Flags is on
	if (gFlagsOpts.bShowExtraImgFlag) {
		//use ExtraIconsService ?
		if(myGlobals.ExtraIconsServiceExist) {
			if(hExtraIconSvc == INVALID_HANDLE_VALUE) {
				char  szId[20];
				//get local langID for descIcon (try to use user local Flag as icon)
				DWORD langid = 0;
				int r = GetLocaleInfo(
					LOCALE_USER_DEFAULT,
					LOCALE_ICOUNTRY | LOCALE_RETURN_NUMBER ,
					(LPTSTR)&langid, sizeof(langid)/sizeof(TCHAR));
				if(!CallService(MS_UTILS_GETCOUNTRYBYNUMBER,langid,0)) langid = 1;

				EXTRAICON_INFO ico = {0};
				ico.cbSize		= sizeof(ico);
				ico.type		= EXTRAICON_TYPE_ICOLIB;
				ico.name		= "Flags";
				ico.description	= "Flags (uinfoex)";
				mir_snprintf(szId, SIZEOF(szId), (langid==0xFFFF)?"%s_0x%X":"%s_%i","flags",langid); /* buffer safe */
				ico.descIcon	= szId;
				hExtraIconSvc=(HANDLE)CallService(MS_EXTRAICON_REGISTER, (WPARAM)&ico, 0);
				if(hExtraIconSvc)
					hExtraIconSvcHook	= HookEvent(ME_DB_CONTACT_SETTINGCHANGED,		OnExtraIconSvcChanged);

			}
		}
		//use Clist ExtraImageService
		else {
			if(phExtraImageList == NULL){
				phExtraImageList = (HANDLE*)mir_alloc(nCountriesCount*sizeof(HANDLE));
				/* invalidate icons */
				if(phExtraImageList!=NULL)
					for(int i=0;i<nCountriesCount;++i)
						phExtraImageList[i]=INVALID_HANDLE_VALUE;
			}
			if(!hRebuildIconsHook) {
				hRebuildIconsHook	= HookEvent(ME_CLIST_EXTRA_LIST_REBUILD,	OnCListRebuildIcons);
			}
		}
		//init hooks
		if(!hApplyIconHook) {
			hApplyIconHook		= HookEvent(ME_CLIST_EXTRA_IMAGE_APPLY,		OnCListApplyIcons);
		}
		if(!hSettingChangedHook) {
			hSettingChangedHook	= HookEvent(ME_DB_CONTACT_SETTINGCHANGED,	OnContactSettingChanged);
		}
	}
	// Flags is off
	else {
		//use ExtraIconsService ?
		if (myGlobals.ExtraIconsServiceExist) {
			//nothing to do, until plugin has a hookable event for status
			return;
		}
		//use Clist ExtraImageService
		//unhook
		if (hRebuildIconsHook) {
			UnhookEvent(hRebuildIconsHook);		hRebuildIconsHook = NULL;
		}
		if (hApplyIconHook) {
			UnhookEvent(hApplyIconHook);		hApplyIconHook = NULL;
		}
		if (hSettingChangedHook && !gFlagsOpts.bShowStatusIconFlag) {
			UnhookEvent(hSettingChangedHook);	hSettingChangedHook = NULL;
		}
			//SvcFlagsApplyCListIcons();
			//RemoveExtraImages();
			CallFunctionBuffered(RemoveExtraImages,0,FALSE,EXTRAIMAGE_REFRESHDELAY);
	}
}

/***********************************************************************************************************
 * message winsow status icon functions
 ***********************************************************************************************************/

MsgWndData::MsgWndData(HWND hwnd, HANDLE hContact) {
	m_hwnd		= hwnd;
	m_hContact	= hContact;
	m_contryID	= (int)ServiceDetectContactOriginCountry((WPARAM)m_hContact,0);
	FlagsIconUpdate();
}

MsgWndData::~MsgWndData() {
	FlagsIconUnset();			//check if realy need
}

void
MsgWndData::FlagsIconSet() {
	HICON hIcon				= NULL;
	StatusIconData sid		= {0};
	sid.cbSize				= sizeof(sid);
	sid.szModule			= MODNAMEFLAGS;
	/* ensure status icon is registered */
	if(	m_contryID!=0xFFFF || gFlagsOpts.bUseUnknownFlag) {
		/* copy icon as status icon API will call DestroyIcon() on it */
		hIcon = LoadFlagIcon(m_contryID);
		sid.hIcon			= (hIcon!=NULL)?CopyIcon(hIcon):NULL;
		CallService(MS_SKIN2_RELEASEICON,(WPARAM)hIcon,0); /* does NULL check */
		hIcon = sid.hIcon;
		sid.dwId			= (DWORD)m_contryID;
		sid.hIconDisabled	= sid.hIcon/*NULL*/;
		sid.szTooltip		= Translate((char*)CallService(MS_UTILS_GETCOUNTRYBYNUMBER,m_contryID,0));
		sid.flags			= 0;
		if(CallService(MS_MSG_MODIFYICON,(WPARAM)m_hContact,(LPARAM)&sid) !=0) /* not yet registered? */
			CallService(MS_MSG_ADDICON,0,(LPARAM)&sid);
	}	
	sid.hIcon				= NULL;
	sid.szTooltip			= NULL;
	sid.hIconDisabled		= NULL;
	for(int i=0;i<nCountriesCount;++i) {
		sid.dwId			= (DWORD)countries[i].id;
		sid.flags			= (m_contryID==countries[i].id && hIcon!=NULL)? 0:MBF_HIDDEN;
		CallService(MS_MSG_MODIFYICON,(WPARAM)m_hContact,(LPARAM)&sid);
	}
}

void
MsgWndData::FlagsIconUnset() {
	StatusIconData sid		= {0};
	sid.cbSize				= sizeof(sid);
	sid.szModule			= MODNAMEFLAGS;
	sid.dwId				= (DWORD)m_contryID;
	sid.flags				= MBF_HIDDEN;
	CallService(MS_MSG_MODIFYICON,(WPARAM)m_hContact,(LPARAM)&sid);
	/* can't call MS_MSG_REMOVEICON here as the icon might be
	 * in use by other contacts simultanously, removing them all at exit */
}

static int CompareMsgWndData(const MsgWndData* p1, const MsgWndData* p2)
{
	return (int)((INT_PTR)p1->m_hContact - (INT_PTR)p2->m_hContact);
}
static LIST<MsgWndData> gMsgWndList(10, CompareMsgWndData);

static int CompareIconListData(const IconList* p1, const IconList* p2)
{
	return (int)((INT_PTR)p1->m_ID - (INT_PTR)p2->m_ID);
}
static OBJLIST<IconList> gIListMW(10, CompareIconListData);

IconList::IconList(StatusIconData *sid) {
	m_StatusIconData.cbSize			= sid->cbSize;
	m_StatusIconData.szModule		= mir_strdup(sid->szModule);
	m_StatusIconData.dwId			= sid->dwId;
	m_StatusIconData.hIcon			= CopyIcon(sid->hIcon);
	m_StatusIconData.hIconDisabled	= sid->hIconDisabled;
	m_StatusIconData.flags			= sid->flags;
	m_StatusIconData.szTooltip		= mir_strdup(sid->szTooltip);

	m_ID = sid->dwId;
	CallService(MS_MSG_ADDICON,0,(LPARAM)sid);

}
IconList::~IconList() {
	mir_free(m_StatusIconData.szModule);
	mir_free(m_StatusIconData.szTooltip);
}
// const char *pszName;			// [Optional] Name of an icon registered with icolib to be used in GUI.
static __inline int MessageAPI_AddIcon(const char* pszName, const char* szModul/*StatusIconData *sid*/,int ID, int flags, const char* szTooltip)
{
	HICON hIcon				= (HICON)CallService(MS_SKIN2_GETICON,(WPARAM)0/* =small 1=big*/,(LPARAM)pszName);

	StatusIconData sid		= {0};
	sid.cbSize				= sizeof(sid);
	sid.szModule			= (char*)szModul;
	sid.dwId				= (DWORD)ID;
	sid.hIcon				= (hIcon!=NULL)?CopyIcon(hIcon):NULL;
	CallService(MS_SKIN2_RELEASEICON,(WPARAM)hIcon,0); /* does NULL check */
//	sid.hIconDisabled		= sid.hIcon/*NULL*/;
	sid.flags				= 0;
	sid.szTooltip			= Translate((char*)CallService(MS_UTILS_GETCOUNTRYBYNUMBER,ID,0));

	int res = -1;
	IconList* p = new IconList(&sid);
	if(!p->m_ID)delete p;
	else res = gIListMW.insert(p);
	if(res == -1)delete p;
	return res;
}

void CALLBACK UpdateStatusIcons(LPARAM lParam) {
	if(!lParam) {
		/* enum all opened message windows */
		for (int i = 0; i < gMsgWndList.getCount(); i++)
			gMsgWndList[i]->FlagsIconUpdate();
	}
	else {
		int i = gMsgWndList.getIndex((MsgWndData*)&lParam);
		if (i!= -1) gMsgWndList[i]->FlagsIconUpdate();
	}
}

//hookProc ME_MSG_WINDOWEVENT
static int OnMsgWndEvent(WPARAM wParam,LPARAM lParam) {
	MessageWindowEventData *msgwe=(MessageWindowEventData*)lParam;
	/* sanity check */
	if(msgwe->hContact==NULL)
		return 0;

	switch(msgwe->uType) {
		case MSG_WINDOW_EVT_OPENING:
			{
				MsgWndData* msgwnd = gMsgWndList.find((MsgWndData*)&msgwe->hContact);
				if(msgwnd == NULL) {
					msgwnd = new MsgWndData(msgwe->hwndWindow, msgwe->hContact);
					gMsgWndList.insert(msgwnd);
				}
			}
			break;

		case MSG_WINDOW_EVT_CLOSE:
			{
				int i = gMsgWndList.getIndex((MsgWndData*)&msgwe->hContact);
				if (i != -1) {
					delete gMsgWndList[i];
					gMsgWndList.remove(i);
				}
			}
			break;
	}
	return 0;
}

//hookProc ME_SKIN2_ICONSCHANGED
static int OnStatusIconsChanged(WPARAM wParam,LPARAM lParam) {
	if(myGlobals.MsgAddIconExist && gFlagsOpts.bShowStatusIconFlag)
		CallFunctionBuffered(UpdateStatusIcons,0,FALSE,STATUSICON_REFRESHDELAY);
	return 0;
}

/***********************************************************************************************************
 * option page (not used yet)
 ***********************************************************************************************************/
static INT_PTR CALLBACK ExtraImgOptDlgProc(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
	return FALSE;
}

//hookProc ME_OPT_INITIALISE
static int ExtraImgOptInit(WPARAM wParam,LPARAM lParam)
{
	OPTIONSDIALOGPAGE odp = { 0 };
	return 0;
}

/***********************************************************************************************************
 * misc functions
 ***********************************************************************************************************/
//hookProc ME_DB_CONTACT_SETTINGCHANGED
static int OnContactSettingChanged(WPARAM wParam,LPARAM lParam) {
	if((HANDLE)wParam==NULL) return 0;
	DBCONTACTWRITESETTING *dbcws=(DBCONTACTWRITESETTING*)lParam;

	/* user details update */
	if(/*!lstrcmpA(dbcws->szSetting,"RealIP") || */
	    !lstrcmpA(dbcws->szSetting,SET_CONTACT_COUNTRY) ||
	    !lstrcmpA(dbcws->szSetting,SET_CONTACT_ORIGIN_COUNTRY) ||
	    !lstrcmpA(dbcws->szSetting,SET_CONTACT_COMPANY_COUNTRY))
		{
		/* Extra Image */
		if(myGlobals.HaveCListExtraIcons)
			CallFunctionBuffered(SetExtraImage,(LPARAM)wParam,TRUE,EXTRAIMAGE_REFRESHDELAY);
		/* Status Icon */
		if(hMsgWndEventHook) {
			int i = gMsgWndList.getIndex((MsgWndData*)&wParam);
			if (i != -1) {
				gMsgWndList[i]->ContryIDchange((int)ServiceDetectContactOriginCountry(wParam,0));
				gMsgWndList[i]->FlagsIconUpdate();
			}
		}
	}
	return 0;
}


/***********************************************************************************************************
 * module loading & unloading
 ***********************************************************************************************************/

/**
 * This function initially loads all required stuff for Flags.
 *
 * @param	none
 *
 * @return	nothing
 **/
void SvcFlagsLoadModule()
{
	PrepareBufferedFunctions();
	InitCountryListExt();	/* hack to replace core country list */
	if(CallService(MS_UTILS_GETCOUNTRYLIST,(WPARAM)&nCountriesCount,(LPARAM)&countries))
		nCountriesCount=0;
	InitIcons();			/* load in iconlib */
	//InitIpToCountry();	/* not implementet */
	myCreateServiceFunction(MS_FLAGS_DETECTCONTACTORIGINCOUNTRY,ServiceDetectContactOriginCountry);
	//init settings
	gFlagsOpts.bShowExtraImgFlag	= DB::Setting::GetByte(MODNAMEFLAGS,"ShowExtraImgFlag",		SETTING_SHOWEXTRAIMGFLAG_DEFAULT);
	gFlagsOpts.bUseUnknownFlag		= DB::Setting::GetByte(MODNAMEFLAGS,"UseUnknownFlag",		SETTING_USEUNKNOWNFLAG_DEFAULT);
	gFlagsOpts.idExtraColumn		= DB::Setting::GetByte(MODNAMEFLAGS,"ExtraImgFlagColumn",	SETTING_EXTRAIMGFLAGCOLUMN_DEFAULT);
	gFlagsOpts.bShowStatusIconFlag	= DB::Setting::GetByte(MODNAMEFLAGS,"ShowStatusIconFlag",	SETTING_SHOWSTATUSICONFLAG_DEFAULT);

	hOptInitHook		= HookEvent(ME_OPT_INITIALISE,ExtraImgOptInit);
	hIconsChangedHook	= HookEvent(ME_SKIN2_ICONSCHANGED,OnStatusIconsChanged);
}

/**
 * This function is called by Miranda just after loading all system modules.
 *
 * @param	none
 *
 * @return	nothing
 **/
void SvcFlagsOnModulesLoaded() {
	//use ExtraIconsService ?
	if ( myGlobals.ExtraIconsServiceExist) {
		SvcFlagsEnableExtraIcons(DB::Setting::GetByte(SET_CLIST_EXTRAICON_FLAGS2, 0), FALSE);
	}
	//use Clist ExtraImageService
	else {
		SvcFlagsEnableExtraIcons(gFlagsOpts.bShowExtraImgFlag? gFlagsOpts.idExtraColumn : -1, FALSE);
	}
	/* Status Icon */
	if(myGlobals.MsgAddIconExist)
		hMsgWndEventHook = HookEvent(ME_MSG_WINDOWEVENT, OnMsgWndEvent);
}

/**
 * This function unloads the module.
 *
 * @param	none
 *
 * @return	nothing
 **/
void SvcFlagsUnloadModule() {
	KillBufferedFunctions();
	//Uninit ExtraImg
	UnhookEvent(hRebuildIconsHook);
	UnhookEvent(hApplyIconHook);
	UnhookEvent(hIconsChangedHook);
	UnhookEvent(hExtraIconSvcHook);
	mir_free(phExtraImageList);		/* does NULL check */
	//Uninit message winsow
	UnhookEvent(hMsgWndEventHook);
	for(int i = 0; i < gMsgWndList.getCount(); i++) {
		//this should not happen
		delete gMsgWndList[i];
		gMsgWndList.remove(i);
	}
	gMsgWndList.destroy();
	gIListMW.destroy();
	//Uninit misc
	UnhookEvent(hSettingChangedHook);
	UnhookEvent(hOptInitHook);
	UninitIcons();

	//UninitIpToCountry();			/* not implementet */
}

