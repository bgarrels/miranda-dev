#pragma warning( disable : 4100 )

#include "common.h"
#include "res\version.h"
#include "res\resource.h"
#include "UseActions.h"

#define TABTOOLBAR_INITPOS 350

struct LIST_INTERFACE li = { 0 };
struct MM_INTERFACE mmi;
int hLangpack;

SortedList ActionItemsList = {0};
TCHAR tszMyPath[MAX_PATH];

HANDLE hEventOptInit;

HANDLE hModulesLoaded;
HANDLE hPreBuildMMenu;
HANDLE hPreBuildCMenu;
HANDLE hActionsChanged;
HANDLE hMTBHook;
HANDLE hTTBHook;
HANDLE hTabBBLoadHook;
HANDLE hTabBBPressHook;
HANDLE hServiceWithLParam;

BOOL bActionsDefined;
BOOL bTTBPresents;
BOOL bTabBBPresents;
BOOL bMTBPresents;
BOOL bCoreHotkeyPresents;
BOOL bVariablePresents;
BOOL bTrayMenuPresents;

DWORD MMenuSeparator, CMenuSeparator;

int ModulesLoaded(WPARAM wParam, LPARAM lParam);

int ReadSetting(MyActionItem* ActionItem);
void WriteSetting(MyActionItem* ActionItem);
void SetAllActionUsers(MyActionItem* ActionItem);
static int CompareActions(MyActionItem* c1, MyActionItem* c2 );

HANDLE AddMainMenuItem(MyActionItem* ActionItem);
void DeleteMainMenuItem(MyActionItem* ActionItem);
int PreBuildMainMenu(WPARAM wParam,LPARAM lParam);
HANDLE AddContactMenuItem(MyActionItem* ActionItem);
void DeleteContactMenuItem(MyActionItem* ActionItem);
int PreBuildContactMenu(WPARAM wParam,LPARAM lParam);
HANDLE AddTrayMenuItem(MyActionItem* ActionItem);
void DeleteTrayMenuItem(MyActionItem* ActionItem);

HANDLE AddRootMenuIcon(TCHAR* tszCMenuPopup);

HANDLE AddMTBButton(MyActionItem* ActionItem);
void DeleteMTBButton(MyActionItem* ActionItem);
int OnMTBLoaded(WPARAM wParam, LPARAM lParam);
HANDLE AddTTBButton(MyActionItem* ActionItem);
void DeleteTTBButton(MyActionItem* ActionItem);
int OnTTBLoaded(WPARAM wParam, LPARAM lParam);
BOOL AddTabBBButton(MyActionItem* ActionItem);
void DeleteTabBBButton(MyActionItem* ActionItem);
int OnTabBBLoaded(WPARAM wParam, LPARAM lParam);
int OnTabButtonPressed(WPARAM wParam,LPARAM lParam);
HANDLE AddIcolibIcon (MyActionItem* ActionItem);
void DeleteIcolibIcon(MyActionItem* ActionItem);
int AddCoreHotkey(MyActionItem* ActionItem);
void DeleteCoreHotkey(MyActionItem* ActionItem);

INT_PTR ServiceCallWithLParam (WPARAM wParam, LPARAM lParam);
INT_PTR ServiceCallWithFParam (WPARAM wParam, LPARAM lParam, LPARAM fParam);
int ActionsChanged(WPARAM wParam, LPARAM lParam);
void AddActionItem(TChain NewAction, int Index);
void SetActionDesc(MyActionItem* ActionItem, TChain NewAction);
void DeleteActionItem(MyActionItem* ActionItem, int ActionIndex);

int OptInit(WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgProcOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void FillListViewWithActions();
int OptActiveItem;
HWND hListView;
HIMAGELIST hImageListView;
HWND hDialogWIndow;
BOOL bRefreshing;

///////////////////////////////////////////////
// Common Plugin Stuff
///////////////////////////////////////////////
HINSTANCE hInst;
PLUGINLINK *pluginLink;

PLUGININFOEX pluginInfo={
	sizeof(PLUGININFOEX),
	__PLUGIN_NAME,
	PLUGIN_MAKE_VERSION(__MAJOR_VERSION, __MINOR_VERSION, __RELEASE_NUM, __BUILD_NUM),
	__DESC,
	__AUTHOR,
	__AUTHOREMAIL,
	__COPYRIGHT,
	__AUTHORWEB,
	UNICODE_AWARE,
	0,
	MIID_USEACTIONS
};


BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved) {
	hInst=hinstDLL;
	return TRUE;
}

__declspec (dllexport) int Load(PLUGINLINK *link) {
	pluginLink = link;
	mir_getLP(&pluginInfo);
	mir_getLI(&li);
	mir_getMMI( &mmi );
	ActionItemsList = *(li.List_Create( 0, 1));
	ActionItemsList.sortFunc = CompareActions;

	hModulesLoaded = HookEvent(ME_SYSTEM_MODULESLOADED, ModulesLoaded);

	GetModuleFileName(hInst, tszMyPath, MAX_PATH);

	return 0;
}

__declspec (dllexport) int Unload(void) {
	int i;
	UnhookEvent(hModulesLoaded);
	UnhookEvent(hActionsChanged);
	UnhookEvent(hPreBuildMMenu);
	UnhookEvent(hPreBuildCMenu);
	UnhookEvent(hMTBHook);
	UnhookEvent(hTabBBLoadHook);
	UnhookEvent(hTTBHook);
	UnhookEvent(hEventOptInit);
	UnhookEvent(hTabBBPressHook);
	DestroyServiceFunction(hServiceWithLParam);
	for (i=ActionItemsList.realCount-1; i>=0;i--) {
		MyActionItem* ActItem = ActionItemsList.items[i];
		DestroyServiceFunction(ActItem->hMenuService);
		mir_free(ActItem);
	}
	li.List_Destroy(&ActionItemsList);
	return 0;
}

__declspec (dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion) {
	return &pluginInfo;
}

/*
// uncomment this for pre 0.7 compatibility
extern "C" __declspec (dllexport) PLUGININFO* MirandaPluginInfo(DWORD mirandaVersion) {
	pluginInfo.cbSize = sizeof(PLUGININFO);
	return (PLUGININFO*)&pluginInfo;
}
*/

static const MUUID interfaces[] = {MIID_USEACTIONS, MIID_LAST};
__declspec(dllexport) const MUUID* MirandaPluginInterfaces(void){
	return interfaces;
}

static int CompareActions(MyActionItem* c1, MyActionItem* c2 ){
	return c1->dwActID - c2->dwActID;
}

void AddActionItem(TChain NewAction, int Index) {
	MyActionItem *ActItem = mir_calloc(sizeof(MyActionItem));
	ActItem->dwActID = NewAction.ID;
	ActItem->wSortIndex = Index;
	//li.List_Insert(&ActionItemsList, ActItem, ActionItemsList.realCount);
	li.List_InsertPtr(&ActionItemsList, ActItem);

	SetActionDesc(ActItem, NewAction);
	mir_snprintf(ActItem->szNameID,32,"%s/Action_%u", MODULE,NewAction.ID);

	// register all for test
	/*ActItem->wFlags = 0xFFFF;
	_tcsncpy(ActItem->tszMMenuPopup, _T("TestMMenuPopup"), MAX_ACT_DESC_LEN);
	_tcsncpy(ActItem->tszCMenuPopup, _T("TestCMenuPopup"), MAX_ACT_DESC_LEN);
	_tcsncpy(ActItem->tszCMenuVarStr, _T("?if(?strcmp(?dbsetting(%subject%,CList,MyHandle),TestNick),1,0)"), MAX_ACT_DESC_LEN);
	*/
	ReadSetting(ActItem);
	SetAllActionUsers(ActItem);
}

void DeleteActionItem(MyActionItem* ActionItem, int ActionIndex) {
	/*ActionItem->wFlags = UAF_NONE;
	SetAllActionUsers(ActionItem);
	DeleteIcolibIcon(ActionItem);*/
	char text[200], *szDest;
	mir_snprintf(text, 32, "Act%u_", ActionItem->dwActID);
	szDest = text + strlen(text);

	strcpy(szDest, "Flags");
	DBDeleteContactSetting(NULL, MODULE, text);

	// delete  icon for action to icolib
	DeleteIcolibIcon(ActionItem);

	// core hotkeys
	if (ActionItem->wFlags & UAF_REGHOTKEY) {
		DeleteCoreHotkey(ActionItem);
	}

	// CList_Modern toolbar buttons
	if (ActionItem->wFlags & UAF_REGMTBB) {
		DeleteMTBButton(ActionItem);
	}

	// TopToolBar buttons
	if (ActionItem->wFlags & UAF_REGTTBB) {
		DeleteTTBButton(ActionItem);
	}

	// TabButtonBar buttons
	if (ActionItem->wFlags & UAF_REGTABB) {
		DeleteTabBBButton(ActionItem);
	}

	// MainMenu items
	if (ActionItem->wFlags & UAF_REGMMENU) {
		DeleteMainMenuItem(ActionItem);
		strcpy(szDest, "MMenuPopup");
		DBDeleteContactSetting(NULL, MODULE, text);
		strcpy(szDest, "MMenuName");
		DBDeleteContactSetting(NULL, MODULE, text);
		strcpy(szDest, "MMenuVarStr");
		DBDeleteContactSetting(NULL, MODULE, text);
	}

	// ContactMenu items
	if (ActionItem->wFlags & UAF_REGCMENU) {
		DeleteContactMenuItem(ActionItem);
		strcpy(szDest, "CMenuPopup");
		DBDeleteContactSetting(NULL, MODULE, text);
		strcpy(szDest, "CMenuName");
		DBDeleteContactSetting(NULL, MODULE, text);
		strcpy(szDest, "CMenuVarStr");
		DBDeleteContactSetting(NULL, MODULE, text);
	}
	// TrayMenu items
	if (ActionItem->wFlags & UAF_REGTMENU) {
		DeleteTrayMenuItem(ActionItem);
		strcpy(szDest, "TMenuPopup");
		DBDeleteContactSetting(NULL, MODULE, text);
	}

	if (!(ActionItem->wFlags & (UAF_REGMMENU | UAF_REGCMENU | UAF_REGTMENU)) && (ActionItem->hMenuService != NULL)) {
		DestroyServiceFunction(ActionItem->hMenuService);
		ActionItem->hMenuService = NULL;
	}

	// FirstRun
	strcpy(szDest, "FirstRun");
	DBDeleteContactSetting(NULL, MODULE, text);

	li.List_Remove(&ActionItemsList, ActionIndex);
}

int ModulesLoaded(WPARAM wParam, LPARAM lParam) {
	int ActionsCount;
	DWORD* pActionStructLen = {0};
	PChain pActionsList = {0};
	//ACTION* pActionsList2 = {0};
	bActionsDefined = FALSE;
	if (ServiceExists(MS_ACT_GETLIST)) {
		hEventOptInit   = HookEvent(ME_OPT_INITIALISE, OptInit);
		hPreBuildMMenu  = HookEvent(ME_CLIST_PREBUILDMAINMENU, PreBuildMainMenu);
		hPreBuildCMenu  = HookEvent(ME_CLIST_PREBUILDCONTACTMENU, PreBuildContactMenu);

		hActionsChanged = HookEvent(ME_ACT_CHANGED, ActionsChanged);
		hMTBHook        = HookEvent(ME_TB_MODULELOADED, OnMTBLoaded);
		hTTBHook        = HookEvent(ME_TTB_MODULELOADED, OnTTBLoaded);
		hTabBBLoadHook  = HookEvent(ME_MSG_TOOLBARLOADED, OnTabBBLoaded);

		if (ServiceExists(MS_TB_ADDBUTTON))
			OnMTBLoaded(0,0);
		if (ServiceExists(MS_BB_ADDBUTTON))
			OnTabBBLoaded(0,0);
//		ActionsCount = CallService(MS_ACT_GETLIST, (WPARAM)0, (LPARAM)&pActionsList);
		ActionsCount = CallService(MS_ACT_GETLIST, (WPARAM)0, (LPARAM)&pActionStructLen);
		if (ActionsCount > 0) {
			int i;
			DWORD dwActionStructLen = *pActionStructLen;
			pActionsList = (PChain)(pActionStructLen + 1);
			bActionsDefined = TRUE;
//			pActionsList2 = pActionsList;

			//bMTBPresents = ServiceExists(MS_TB_ADDBUTTON) && bActionsDefined;
			bTTBPresents  = ServiceExists(MS_TTB_ADDBUTTON) && bActionsDefined;
			bCoreHotkeyPresents = ServiceExists(MS_HOTKEY_REGISTER);
			bVariablePresents = ServiceExists(MS_VARS_FORMATSTRING);
			bTrayMenuPresents = ServiceExists(MS_CLIST_ADDTRAYMENUITEM);

			hServiceWithLParam = CreateServiceFunction(SERVICE_WITH_LPARAM_NAME, &ServiceCallWithLParam);
			for (i=0;i<ActionsCount;i++) {
				AddActionItem(*pActionsList, i);
				pActionsList = (PChain)((BYTE*)pActionsList + dwActionStructLen);
			}
//			CallService(MS_ACT_FREELIST, (WPARAM)0, (LPARAM)pActionsList2);
			CallService(MS_ACT_FREELIST, (WPARAM)0, (LPARAM)pActionStructLen);
		}


		if(ServiceExists(MS_UPDATE_REGISTER)) {
			// register with updater
			Update update = {0};
			char szVersion[16];
			update.cbSize               = sizeof(Update);
			update.szComponentName      = pluginInfo.shortName;
			update.pbVersion            = (BYTE *)CreateVersionString(pluginInfo.version, szVersion);
			update.cpbVersion           = (int)strlen((char *)update.pbVersion);
			update.szBetaUpdateURL      = "http://vasilich.org/mim/Updater/UseActions/UseActions.zip";
			update.szBetaChangelogURL   = "http://vasilich.org/mim/Updater/UseActions/Changelog.txt";
			update.szBetaVersionURL     = "http://vasilich.org/mim/Updater/UseActions/Changelog.txt";
			update.pbBetaVersionPrefix  = (BYTE *)"UseActions Plugin, version ";
			update.cpbBetaVersionPrefix = (int)strlen((char *)update.pbBetaVersionPrefix);
			CallService(MS_UPDATE_REGISTER, 0, (LPARAM)&update);
		}
		CallService(MS_ACT_RUNBYNAME, (WPARAM)_T("#Autostart"), 0);
	}
	else {
		UnhookEvent(hEventOptInit);
		MessageBox(NULL, 
			TranslateT("No actions manager (ActMan) plugin found.\n\rYou can't use UseActions plugin without ActMan.\n\rUseActions will not be loaded."), 
			TranslateT("Error loading actions"), 
			MB_OK + MB_ICONERROR);
	}
	return 0;
}

void SetAllActionUsers(MyActionItem* ActionItem) {
	// add icon for action to icolib
	ActionItem->hIcolibIcon = AddIcolibIcon(ActionItem);
	// core hotkeys
	if (ActionItem->wFlags & UAF_REGHOTKEY) {
		ActionItem->bReggedHotkey = AddCoreHotkey(ActionItem);
	}
	else {
		DeleteCoreHotkey(ActionItem);
	}
	// CList_Modern toolbar buttons
	if (ActionItem->wFlags & UAF_REGMTBB) {
		ActionItem->hMTBButton = AddMTBButton(ActionItem);
	}
	else {
		DeleteMTBButton(ActionItem);
	}
	// TopToolBar buttons
	if (ActionItem->wFlags & UAF_REGTTBB) {
		ActionItem->hTTBButton = AddTTBButton(ActionItem);
	}
	else {
		DeleteTTBButton(ActionItem);
	}
	// MainMenu items
	if (ActionItem->wFlags & UAF_REGMMENU) {
		ActionItem->hMainMenuItem = AddMainMenuItem(ActionItem);
	}
	else {
		DeleteMainMenuItem(ActionItem);
	}
	// ContactMenu items
	if (ActionItem->wFlags & UAF_REGCMENU) {
		ActionItem->hContactMenuItem = AddContactMenuItem(ActionItem);
	}
	else {
		DeleteContactMenuItem(ActionItem);
	}
	// TrayMenu items
	if (ActionItem->wFlags & UAF_REGTMENU) {
		ActionItem->hTrayMenuItem = AddTrayMenuItem(ActionItem);
	}
	else {
		DeleteTrayMenuItem(ActionItem);
	}

	if (!(ActionItem->wFlags & (UAF_REGMMENU | UAF_REGCMENU | UAF_REGTMENU)) && (ActionItem->hMenuService != NULL)) {
		DestroyServiceFunction(ActionItem->hMenuService);
		ActionItem->hMenuService = NULL;
	}

	// TabButtonBar buttons
	if (ActionItem->wFlags & UAF_REGTABB) {
		ActionItem->wReggedTabButton = AddTabBBButton(ActionItem);
	}
	else {
		DeleteTabBBButton(ActionItem);
	}

	{// FirstRun
		char szFirstRunName[32];
		mir_snprintf(szFirstRunName, 32, "Act%u_FirstRun", ActionItem->dwActID);
		if (DBGetContactSettingByte(0, MODULE, szFirstRunName, 0)) {
			CallService(SERVICE_WITH_LPARAM_NAME, 0, ActionItem->dwActID);
			DBDeleteContactSetting(0, MODULE, szFirstRunName);
		}
	}
}

INT_PTR ServiceCallWithLParam (WPARAM wParam, LPARAM lParam){
	CallService(MS_ACT_RUNBYID, lParam, 0);
	return 0;
}

INT_PTR ServiceCallWithFParam (WPARAM wParam, LPARAM lParam, LPARAM fParam){
	CallService(MS_ACT_RUNBYID, fParam, wParam);
	return 0;
}

int ActionsChanged(WPARAM wParam, LPARAM lParam) {
	DWORD* pActionStructLen = {0};
	PChain pActionsList = {0};
	PChain pActionsList2 = {0};
	DWORD dwActionStructLen;
//	int ActionsCount = CallService(MS_ACT_GETLIST, (WPARAM)0, (LPARAM)&pActionsList);
	int ActionsCount = CallService(MS_ACT_GETLIST, (WPARAM)0, (LPARAM)&pActionStructLen);
	if (ActionsCount > 0) {
		int i;
		dwActionStructLen = *pActionStructLen;
		pActionsList = (PChain)(pActionStructLen + 1);
		bActionsDefined = TRUE;
		pActionsList2 = pActionsList;
		//bMTBPresents = ServiceExists(MS_TB_ADDBUTTON) && bActionsDefined;
		bTTBPresents  = ServiceExists(MS_TTB_ADDBUTTON) && bActionsDefined;
		bCoreHotkeyPresents = ServiceExists(MS_HOTKEY_REGISTER);
		bVariablePresents = ServiceExists(MS_VARS_FORMATSTRING);
		bTrayMenuPresents = ServiceExists(MS_CLIST_ADDTRAYMENUITEM);

		if(hServiceWithLParam == NULL)
			hServiceWithLParam = CreateServiceFunction(SERVICE_WITH_LPARAM_NAME, &ServiceCallWithLParam);
		for (i=0;i<ActionsCount;i++) {
			if(wParam & (ACTM_NEW | ACTM_RENAME | ACTM_SORT | ACTM_DELETE)) {
				MyActionItem *pFoundListItem = NULL;
				MyActionItem pTestingItem;
				pTestingItem.dwActID = pActionsList->ID;
				pTestingItem.wSortIndex = i;
				pFoundListItem = li.List_Find(&ActionItemsList, &pTestingItem);
				// if we have no item in list for this action - then add new one
				if (pFoundListItem == NULL) {
					AddActionItem(*pActionsList, i);
				}
				else {
					if(wParam & ACTM_RENAME) {
						if (pActionsList->Descr == NULL)
							pActionsList->Descr = _T("");
						SetActionDesc(pFoundListItem, *pActionsList);
					}
					if (wParam & (ACTM_SORT | ACTM_DELETE | ACTM_NEW)) {
						pFoundListItem->wSortIndex = i;
					}
				}
			}
//			pActionsList++;
			pActionsList = (PChain)((BYTE*)pActionsList + dwActionStructLen);

		}
	}
	if(wParam & ACTM_DELETE) {
		int i,j;
		for (j=ActionItemsList.realCount-1; j>=0;j--) {
			BOOL bFound = FALSE;
			PChain pActionsList3 = pActionsList2;
			MyActionItem* ActItem = ActionItemsList.items[j];
			for (i=0;i<ActionsCount;i++) {
				if(pActionsList3->ID == ActItem->dwActID) {
					bFound = TRUE;
					break;
				}
				pActionsList3++;
			}
			if(!bFound) {
				DeleteActionItem(ActionItemsList.items[j], j);
			}
		}
	}
	if (pActionStructLen != NULL)
		CallService(MS_ACT_FREELIST, (WPARAM)0, (LPARAM)pActionStructLen);

	ListView_DeleteAllItems(hListView);
	ListView_DeleteColumn(hListView, 0);
	ImageList_RemoveAll(hImageListView);
	FillListViewWithActions();
	return 0;
}

void SetActionDesc( MyActionItem* ActionItem, TChain NewAction ) {
	WideCharToMultiByte(CallService(MS_LANGPACK_GETCODEPAGE,0,0), 0, NewAction.Descr, -1, ActionItem->szActDescr, MAX_ACT_DESC_LEN, NULL, NULL);
#ifdef UNICODE
	_tcsncpy(ActionItem->tszActDescr, NewAction.Descr, MAX_ACT_DESC_LEN);
#else
	strncpy(ActionItem->tszActDescr, ActionItem->szActDescr, MAX_ACT_DESC_LEN);
#endif
}
int ReadSetting(MyActionItem* ActionItem){
	DBVARIANT dbv, dbv2;
	int result = 1;
	char text[200], *szDest;
	mir_snprintf(text, 32, "Act%u_", ActionItem->dwActID);
	szDest = text + strlen(text);

	strcpy(szDest, "Flags");
	ActionItem->wFlags = DBGetContactSettingWord(NULL, MODULE, text,0);

	strcpy(szDest, "MMenuPopup");
	if(!DBGetContactSettingTString(NULL, MODULE, text, &dbv)) {
		_tcscpy(ActionItem->tszMMenuPopup, dbv.ptszVal);
		DBFreeVariant(&dbv);
		result++;
	}
	strcpy(szDest, "MMenuName");
	if(!DBGetContactSettingTString(NULL, MODULE, text, &dbv)) {
		_tcscpy(ActionItem->tszMMenuNameVars, dbv.ptszVal);
		DBFreeVariant(&dbv);
		result++;
	}
	strcpy(szDest, "MMenuVarStr");
	if(!DBGetContactSettingTString(NULL, MODULE, text, &dbv2)) {
		_tcscpy(ActionItem->tszMMenuShowWhenVars, dbv2.ptszVal);
		DBFreeVariant(&dbv2);
		result++;
	}
	strcpy(szDest, "CMenuName");
	if(!DBGetContactSettingTString(NULL, MODULE, text, &dbv)) {
		_tcscpy(ActionItem->tszCMenuNameVars, dbv.ptszVal);
		DBFreeVariant(&dbv);
		result++;
	}
	strcpy(szDest, "CMenuPopup");
	if(!DBGetContactSettingTString(NULL, MODULE, text, &dbv)) {
		_tcscpy(ActionItem->tszCMenuPopup, dbv.ptszVal);
		DBFreeVariant(&dbv);
		result++;
	}
	strcpy(szDest, "CMenuVarStr");
	if(!DBGetContactSettingTString(NULL, MODULE, text, &dbv2)) {
		_tcscpy(ActionItem->tszCMenuShowWhenVars, dbv2.ptszVal);
		DBFreeVariant(&dbv2);
		result++;
	}
	strcpy(szDest, "TMenuPopup");
	if(!DBGetContactSettingTString(NULL, MODULE, text, &dbv)) {
		_tcscpy(ActionItem->tszTMenuPopup, dbv.ptszVal);
		DBFreeVariant(&dbv);
		result++;
	}
	return result;
}

void WriteSetting(MyActionItem* ActionItem){
	char text[200], *szDest;
	mir_snprintf(text, 32, "Act%u_", ActionItem->dwActID);
	szDest = text + strlen(text);

	strcpy(szDest, "Flags");
	if(ActionItem->wFlags == 0)
		DBDeleteContactSetting(NULL, MODULE, text);
	else
		DBWriteContactSettingWord(NULL, MODULE, text, ActionItem->wFlags);
	strcpy(szDest, "MMenuPopup");
	if(ActionItem->tszMMenuPopup[0] == 0)
		DBDeleteContactSetting(NULL, MODULE, text);
	else
		DBWriteContactSettingTString(NULL, MODULE, text, ActionItem->tszMMenuPopup);
	strcpy(szDest, "MMenuName");
	if(ActionItem->tszMMenuNameVars[0] == 0)
		DBDeleteContactSetting(NULL, MODULE, text);
	else
		DBWriteContactSettingTString(NULL, MODULE, text, ActionItem->tszMMenuNameVars);
	strcpy(szDest, "MMenuVarStr");
	if(ActionItem->tszMMenuShowWhenVars[0] == 0)
		DBDeleteContactSetting(NULL, MODULE, text);
	else
		DBWriteContactSettingTString(NULL, MODULE, text, ActionItem->tszMMenuShowWhenVars);
	strcpy(szDest, "CMenuName");
	if(ActionItem->tszCMenuNameVars[0] == 0)
		DBDeleteContactSetting(NULL, MODULE, text);
	else
		DBWriteContactSettingTString(NULL, MODULE, text, ActionItem->tszCMenuNameVars);
	strcpy(szDest, "CMenuPopup");
	if(ActionItem->tszCMenuPopup[0] == 0)
		DBDeleteContactSetting(NULL, MODULE, text);
	else
		DBWriteContactSettingTString(NULL, MODULE, text, ActionItem->tszCMenuPopup);
	strcpy(szDest, "CMenuVarStr");
	if(ActionItem->tszCMenuShowWhenVars[0] == 0)
		DBDeleteContactSetting(NULL, MODULE, text);
	else
		DBWriteContactSettingTString(NULL, MODULE, text, ActionItem->tszCMenuShowWhenVars);
	strcpy(szDest, "TMenuPopup");
	if(ActionItem->tszTMenuPopup[0] == 0)
		DBDeleteContactSetting(NULL, MODULE, text);
	else
		DBWriteContactSettingTString(NULL, MODULE, text, ActionItem->tszTMenuPopup);
}

//================= Menu items =============================
//----------------- MainMenu -------------------------------
HANDLE AddMainMenuItem(MyActionItem* ActionItem){
	if (!ActionItem->hMainMenuItem) {
		//register MainMenu items
		CLISTMENUITEM clmi = {0};
		clmi.cbSize=sizeof(clmi);
		clmi.flags = CMIF_TCHAR | CMIF_ICONFROMICOLIB;
		if (ActionItem->tszMMenuPopup[0] != 0) {
			// first search if we have already created root popup with same name
			int i;
			for (i=0; i<ActionItemsList.realCount;i++) {
				// try to find root popup with same name (if we already created one)
				MyActionItem* ActItem = ActionItemsList.items[i];
				if ( (ActItem->tszMMenuPopup[0] != 0) &&
					( (!_tcscmp(ActItem->tszMMenuPopup, ActionItem->tszMMenuPopup) ) && (ActItem != ActionItem) ) ) {
						clmi.ptszPopupName = (TCHAR*)ActItem->hMMenuRoot;
						clmi.flags = CMIF_TCHAR | CMIF_ICONFROMICOLIB | CMIF_CHILDPOPUP;
						//mi.position = -1999901010;
						ActionItem->hMMenuRoot = ActItem->hMMenuRoot;
						break;
				}
			}
			if (clmi.ptszPopupName == NULL) {
				//haven't found - then make one!
				clmi.icolibItem = AddRootMenuIcon(ActionItem->tszMMenuPopup);
				//then register root popup itself
				clmi.ptszName = ActionItem->tszMMenuPopup;
				clmi.flags |= CMIF_ROOTPOPUP;
				clmi.pszPopupName = (char *)-1;
				ActionItem->hMMenuRoot = (HANDLE)CallService(MS_CLIST_ADDMAINMENUITEM,0,(LPARAM)&clmi);
				clmi.ptszPopupName = (TCHAR*)ActionItem->hMMenuRoot;
				clmi.flags &= ~CMIF_ROOTPOPUP;
			}
			clmi.flags |= CMIF_CHILDPOPUP;
		}
		clmi.pszService = ActionItem->szNameID;
		if (!ActionItem->hMenuService) 
			ActionItem->hMenuService = (HANDLE)CreateServiceFunctionParam(clmi.pszService, ServiceCallWithFParam, ActionItem->dwActID);
		clmi.ptszName = ActionItem->tszActDescr;
		//clmi.ptszPopupName = ActionItem->tszMMenuPopup[0] == 0 ? NULL : ActionItem->tszMMenuPopup;
		clmi.icolibItem = ActionItem->hIcolibIcon;
		if (ActionItem->wFlags & UAF_MMENUSEP) {
			MMenuSeparator += 100000;
		}
		clmi.position = MMenuSeparator;
		return (HANDLE)CallService(MS_CLIST_ADDMAINMENUITEM,0,(LPARAM)&clmi);
	}
	else {
		return ActionItem->hMainMenuItem;
	}
}

void DeleteMainMenuItem(MyActionItem* ActionItem) {
	if(ActionItem->hMainMenuItem) {
		CallService(MS_CLIST_REMOVEMAINMENUITEM, (WPARAM)ActionItem->hMainMenuItem, 0);
		ActionItem->hMainMenuItem = NULL;
	}
}

int PreBuildMainMenu(WPARAM wParam,LPARAM lParam){
	int i;
	TCHAR* tszVarResult;
	TCHAR tszVarResultCopy[MAX_ACT_DESC_LEN];
	//	HANDLE hContact = (HANDLE) wParam;
	//	char* szProto = ( char* )CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM) hContact, 0);
	for (i=0; i<ActionItemsList.realCount;i++) {
		MyActionItem* ActItem = ActionItemsList.items[i];
		if(ActItem->hMainMenuItem) {
			CLISTMENUITEM clmi = {0};
			clmi.cbSize = sizeof(clmi);
			clmi.flags = CMIM_FLAGS | CMIF_HIDDEN;
			if ( bVariablePresents && 
				(ActItem->wFlags & UAF_REGMMENU) &&
				(ActItem->wFlags & UAF_MMENU_VARS)  && 
				(ActItem->tszMMenuShowWhenVars[0] != 0) ) {
					tszVarResult = variables_parse(ActItem->tszMMenuShowWhenVars, (TCHAR *)NULL, (HANDLE) wParam);
					if (_tcscmp(tszVarResult, _T("1")) == 0) {
						//show menuitem
						clmi.flags = CMIM_FLAGS;
					}
					variables_free(tszVarResult);
			}
			else if( (ActItem->wFlags & UAF_REGMMENU) &&  !(ActItem->wFlags & UAF_MMENU_VARS) ) {
				//show menuitem	
				clmi.flags = CMIM_FLAGS;
			}
			
			// Name through Vars
			if ( bVariablePresents && 
				(ActItem->wFlags & UAF_REGMMENU) &&
				(ActItem->tszMMenuNameVars[0] != 0) ) {
					tszVarResult = variables_parse(ActItem->tszMMenuNameVars, (TCHAR *)NULL, (HANDLE) wParam);
					if (tszVarResult[0] != 0) {
						_tcsncpy(tszVarResultCopy, tszVarResult, MAX_ACT_DESC_LEN);
						clmi.ptszName = tszVarResultCopy;
					}
					else
						clmi.ptszName = ActItem->tszActDescr;
					clmi.flags |= CMIM_NAME | CMIF_TCHAR;
					variables_free(tszVarResult);
				}
			if (!_tcscmp(ActItem->tszMMenuNameVars, _T("-----"))) {
				//clmi.flags |= CMIF_HIDDEN;
			}
			CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM) ActItem->hMainMenuItem, (LPARAM)&clmi);
		}
	}
	return 0;
}


//----------------- ContactMenu ----------------------------
HANDLE AddContactMenuItem(MyActionItem* ActionItem){
	if(!ActionItem->hContactMenuItem) {
		//register ContactMenu items
		CLISTMENUITEM clmi = {0};
		clmi.cbSize=sizeof(clmi);
		clmi.flags = CMIF_TCHAR | CMIF_ICONFROMICOLIB;
		
		//note: this code works only with patched modules/clist/clistmenus.c lines 308-312
		//mi.ptszPopupName = ActionItem->tszCMenuPopup;

		if (ActionItem->tszCMenuPopup[0] != 0) {
			// first search if we have already created root popup with same name
			int i;
			for (i=0; i<ActionItemsList.realCount;i++) {
				// try to find root popup with same name (if we already created one)
				MyActionItem* ActItem = ActionItemsList.items[i];
				if ( (ActItem->tszCMenuPopup[0] != 0) &&
					 ( (!_tcscmp(ActItem->tszCMenuPopup, ActionItem->tszCMenuPopup) ) && (ActItem != ActionItem) ) ) {
					clmi.ptszPopupName = (TCHAR*)ActItem->hCMenuRoot;
//					mi.flags = CMIF_TCHAR | CMIF_ICONFROMICOLIB | CMIF_CHILDPOPUP;
					//mi.position = -1999901010;
					ActionItem->hCMenuRoot = ActItem->hCMenuRoot;
					break;
				}
			}
			if (clmi.ptszPopupName == NULL) {
				//haven't found - then make one!
				clmi.icolibItem = AddRootMenuIcon(ActionItem->tszCMenuPopup);
				//then register root popup itself
				clmi.ptszName = ActionItem->tszCMenuPopup;
				clmi.flags |= CMIF_ROOTPOPUP;
				clmi.pszPopupName = (char *)-1;
				ActionItem->hCMenuRoot = (HANDLE)CallService(MS_CLIST_ADDCONTACTMENUITEM,0,(LPARAM)&clmi);
				clmi.ptszPopupName = (TCHAR*)ActionItem->hCMenuRoot;
				clmi.flags &= ~CMIF_ROOTPOPUP;
				//mi.position = -1999901010;
			}
			clmi.flags |= CMIF_CHILDPOPUP;
		}
		clmi.pszService = ActionItem->szNameID;
		if (!ActionItem->hMenuService)
			ActionItem->hMenuService = (HANDLE)CreateServiceFunctionParam(clmi.pszService, ServiceCallWithFParam, ActionItem->dwActID);
		clmi.ptszName = ActionItem->tszActDescr;
		clmi.icolibItem = ActionItem->hIcolibIcon;
		/*if (!_tcscmp(ActionItem->tszCMenuName, _T("-----"))) {
			//mi.position += 100000;
		}*/
		if (ActionItem->wFlags & UAF_CMENUSEP) {
			CMenuSeparator += 100000;
		}
		clmi.position = CMenuSeparator;

		return (HANDLE)CallService(MS_CLIST_ADDCONTACTMENUITEM,0,(LPARAM)&clmi);
	}
	else
		return ActionItem->hContactMenuItem;
}

void DeleteContactMenuItem(MyActionItem* ActionItem) {
	if(ActionItem->hContactMenuItem) {
		CallService(MS_CLIST_REMOVECONTACTMENUITEM, (WPARAM)ActionItem->hContactMenuItem, 0);
		ActionItem->hContactMenuItem = NULL;
	}
}

int PreBuildContactMenu(WPARAM wParam,LPARAM lParam){
	int i;
	TCHAR* tszVarResult;
	TCHAR tszVarResultCopy[MAX_ACT_DESC_LEN];
	//	HANDLE hContact = (HANDLE) wParam;
	//	char* szProto = ( char* )CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM) hContact, 0);
	for (i=0; i<ActionItemsList.realCount;i++) {
		MyActionItem* ActItem = ActionItemsList.items[i];
		if(ActItem->hContactMenuItem) {
			CLISTMENUITEM clmi = {0};
			clmi.cbSize = sizeof(clmi);
			clmi.flags = CMIM_FLAGS | CMIF_HIDDEN;
			if ( bVariablePresents && 
				(ActItem->wFlags & UAF_REGCMENU) &&
				(ActItem->wFlags & UAF_CMENU_VARS)  && 
				(ActItem->tszCMenuShowWhenVars[0] != 0) ) {
					tszVarResult = variables_parse(ActItem->tszCMenuShowWhenVars, (TCHAR *)NULL, (HANDLE) wParam);
					if (_tcscmp(tszVarResult, _T("1")) == 0) {
						//show menuitem
						clmi.flags = CMIM_FLAGS;
					}
					variables_free(tszVarResult);
			}
			else if( (ActItem->wFlags & UAF_REGCMENU) &&  !(ActItem->wFlags & UAF_CMENU_VARS) ) {
				//show menuitem
				clmi.flags = CMIM_FLAGS;
			}
			// Name through Vars
			if ( bVariablePresents && 
				(ActItem->wFlags & UAF_REGCMENU) &&
				(ActItem->tszCMenuNameVars[0] != 0) ) {
					tszVarResult = variables_parse(ActItem->tszCMenuNameVars, (TCHAR *)NULL, (HANDLE) wParam);
					if (tszVarResult[0] != 0) {
						_tcsncpy(tszVarResultCopy, tszVarResult, MAX_ACT_DESC_LEN);
						clmi.ptszName = tszVarResultCopy;
					}
					else
						clmi.ptszName = ActItem->tszActDescr;
					clmi.flags |= CMIM_NAME | CMIF_TCHAR;
					variables_free(tszVarResult);
				}	
			if (!_tcscmp(ActItem->tszCMenuNameVars, _T("-----"))) {
				//clmi.flags |= CMIF_HIDDEN;
			}
			CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM) ActItem->hContactMenuItem, (LPARAM)&clmi);
		}
	}
	return 0;
}

//----------------- TrayMenu -------------------------------
HANDLE AddTrayMenuItem(MyActionItem* ActionItem){
	if(bTrayMenuPresents) {
		if (!ActionItem->hTrayMenuItem) {
			CLISTMENUITEM clmi = {0};
			clmi.cbSize=sizeof(clmi);
			clmi.flags = CMIF_TCHAR | CMIF_ICONFROMICOLIB;
			//register TrayMenu items
			if (ActionItem->tszTMenuPopup[0] != 0) {
				// first search if we have already created root popup with same name
				int i;
				for (i=0; i<ActionItemsList.realCount;i++) {
					// try to find root popup with same name (if we already created one)
					MyActionItem* ActItem = ActionItemsList.items[i];
					if ( (ActItem->tszTMenuPopup[0] != 0) &&
						( (!_tcscmp(ActItem->tszTMenuPopup, ActionItem->tszTMenuPopup) ) && (ActItem != ActionItem) ) ) {
							clmi.ptszPopupName = (TCHAR*)ActItem->hTMenuRoot;
							//mi.position = -1999901010;
							ActionItem->hTMenuRoot = ActItem->hTMenuRoot;
							break;
					}
				}
				if (clmi.ptszPopupName == NULL) {
					//haven't found - then make one!
					clmi.icolibItem = AddRootMenuIcon(ActionItem->tszTMenuPopup);
					//then register root popup itself
					clmi.ptszName = ActionItem->tszTMenuPopup;
					clmi.flags |= CMIF_ROOTPOPUP;
					clmi.pszPopupName = (char *)-1;
					ActionItem->hTMenuRoot = (HANDLE)CallService(MS_CLIST_ADDTRAYMENUITEM,0,(LPARAM)&clmi);
					clmi.ptszPopupName = (TCHAR*)ActionItem->hTMenuRoot;
					clmi.flags &= ~CMIF_ROOTPOPUP;
					//mi.position = -1999901010;
				}
				clmi.flags |= CMIF_CHILDPOPUP;
			}
			clmi.pszService = ActionItem->szNameID;
			if (!ActionItem->hMenuService)
				ActionItem->hMenuService = (HANDLE)CreateServiceFunctionParam(clmi.pszService, ServiceCallWithFParam, ActionItem->dwActID);
			clmi.ptszName = ActionItem->tszActDescr;
			clmi.icolibItem = ActionItem->hIcolibIcon;
			//if (!_tcscmp(ActionItem->tszCMenuName, _T("-----"))) {
				//mi.position += 100000;
			return (HANDLE)CallService(MS_CLIST_ADDTRAYMENUITEM,0,(LPARAM)&clmi);
		}
		else {
			//show menuitem
			//clmi.flags = CMIM_FLAGS;
			//CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM) ActionItem->hTrayMenuItem, (LPARAM)&clmi);
			return ActionItem->hTrayMenuItem;
		}
	}
	return NULL;
}

void DeleteTrayMenuItem(MyActionItem* ActionItem) {
	if(ActionItem->hTrayMenuItem) {
		CallService(MS_CLIST_REMOVETRAYMENUITEM, (WPARAM)ActionItem->hTrayMenuItem, 0);
		ActionItem->hTrayMenuItem = NULL;
	}
}

HANDLE AddRootMenuIcon(TCHAR* tszPopupName){
	//first - register icon for root popup
	SKINICONDESC sid = {0};
	sid.cbSize = sizeof(sid);
	sid.ptszSection = _T(ICOLIB_MNUSECTION);
	sid.flags = SIDF_ALL_TCHAR;
	sid.cx= sid.cy = 16;
	sid.ptszDescription = tszPopupName;
	sid.pszName = mir_t2a(tszPopupName);
	sid.ptszDefaultFile = tszMyPath;
	sid.iDefaultIndex = -IDI_ACTION;
	return (HANDLE)CallService(MS_SKIN2_ADDICON,0,(LPARAM)&sid);
}

//============== Toolbar Buttons ===========================
//-------------- CList_Modern Toolbar ----------------------
HANDLE AddMTBButton(MyActionItem* ActionItem){
	//register CList_Modern toolbar buttons
	if (bMTBPresents) {
		if(!ActionItem->hMTBButton) {
			TBButton mtButton = {0};
			mtButton.cbSize = sizeof(mtButton);
			mtButton.defPos = 1000;
			mtButton.tbbFlags = TBBF_SHOWTOOLTIP|TBBF_VISIBLE;
			mtButton.pszButtonID = ActionItem->szNameID;
			mtButton.pszServiceName = SERVICE_WITH_LPARAM_NAME;
			mtButton.lParam = ActionItem->dwActID;
			mtButton.pszButtonName = mtButton.pszTooltipUp = mtButton.pszTooltipUp = ActionItem->szActDescr;
			mtButton.hSecondaryIconHandle = mtButton.hPrimaryIconHandle = ActionItem->hIcolibIcon;
			return (HANDLE)CallService(MS_TB_ADDBUTTON, 0, (LPARAM)&mtButton);
		}
		else {
			return ActionItem->hMTBButton;
		}
	}
	else
		return NULL;
}

void DeleteMTBButton(MyActionItem* ActionItem){
	//register CList_Modern toolbar buttons
	if (bMTBPresents && ActionItem->hMTBButton) {
		CallService(MS_TB_REMOVEBUTTON, (WPARAM)ActionItem->hMTBButton, 0);
		ActionItem->hMTBButton = NULL;
		// ask FYR to fix his CList_Modern - right now (22.03.2008) it does nothing when calling service for remove toolbar button!
	}
}

int OnMTBLoaded(WPARAM wParam, LPARAM lParam){
	int i;
	bMTBPresents = TRUE;//bActionsDefined;
//	for (i=0; i<ActionItemsList.realCount;i++) {
	for (i=ActionItemsList.realCount-1;i>=0;i--) {
		MyActionItem* ActItem = ActionItemsList.items[i];
		if (ActItem->wFlags & UAF_REGMTBB)
			ActItem->hMTBButton = AddMTBButton(ActItem);
	}
	return 0;
}

//----------------- TopToolbar -----------------------------
HANDLE AddTTBButton(MyActionItem* ActionItem){
	if (bTTBPresents) {
		if(!ActionItem->hTTBButton) {
			// register TopToolBar button
			TTBButtonV2 ttbb = {0};
			HANDLE hTTBButt;
			ttbb.cbSize = sizeof(ttbb);
			ttbb.dwFlags = TTBBF_VISIBLE | TTBBF_SHOWTOOLTIP;
			ttbb.pszServiceDown = ttbb.pszServiceUp = SERVICE_WITH_LPARAM_NAME;
			ttbb.lParamDown = ttbb.lParamUp =  ActionItem->dwActID;
			ttbb.name = Translate(ActionItem->szActDescr);
			ttbb.hIconDn = ttbb.hIconUp = (HICON)CallService(MS_SKIN2_GETICONBYHANDLE,0, (LPARAM)ActionItem->hIcolibIcon);
			ttbb.tooltipDn = ttbb.tooltipUp = Translate(ActionItem->szActDescr);
			hTTBButt = (HANDLE)CallService(MS_TTB_ADDBUTTON, (WPARAM)&ttbb, 0);
			if (hTTBButt)
				CallService(MS_TTB_SETBUTTONOPTIONS, MAKEWPARAM(TTBO_TIPNAME, hTTBButt), (LPARAM)(Translate(ActionItem->szActDescr)));
			return hTTBButt;
		}
		else
			return ActionItem->hTTBButton;
	}
	else
		return NULL;
}

void DeleteTTBButton(MyActionItem* ActionItem){
	if(bTTBPresents && ActionItem->hTTBButton) {
		// delete TopToolBar button
		CallService(MS_TTB_REMOVEBUTTON, (WPARAM)ActionItem->hTTBButton, 0);
		ActionItem->hTTBButton = NULL;
	}
}

int OnTTBLoaded(WPARAM wParam, LPARAM lParam){
	int i;
	bTTBPresents = TRUE;//bActionsDefined;
//	for (i=0; i<ActionItemsList.realCount;i++) {
	for (i=ActionItemsList.realCount-1;i>=0;i--) {
		MyActionItem* ActItem = ActionItemsList.items[i];
		if (ActItem->wFlags & UAF_REGTTBB)
			ActItem->hTTBButton = AddTTBButton(ActItem);
	}
	return 0;
}

//----------------- TabSRMM ButtonBar ----------------------
BOOL AddTabBBButton(MyActionItem* ActionItem){
	if (bTabBBPresents) {
		if(!(ActionItem->wReggedTabButton & 0x8000) ) {
			// register Tab ButtonBar button
			BBButton tabb = {0};
			tabb.cbSize = sizeof(tabb);
			tabb.dwButtonID = ActionItem->dwActID;
			tabb.pszModuleName = MODULE;
			tabb.ptszTooltip = TranslateTS(ActionItem->tszActDescr);
			tabb.dwDefPos = (TABTOOLBAR_INITPOS + ActionItem->wSortIndex*10) & 0x7FFF;
			tabb.iButtonWidth = 0;
			tabb.bbbFlags = BBBF_ISIMBUTTON | BBBF_ISLSIDEBUTTON | BBBF_ISCHATBUTTON;
			tabb.hIcon = ActionItem->hIcolibIcon;

			CallService(MS_BB_ADDBUTTON, 0, (LPARAM)&tabb);
			return tabb.dwDefPos | 0x8000;
		}
		else
			return ActionItem->wReggedTabButton;
	}
	else
		return FALSE;
}

void DeleteTabBBButton(MyActionItem* ActionItem){
	if(bTabBBPresents && (ActionItem->wReggedTabButton & 0x8000)) {
		// delete Tab ButtonBar button
		BBButton tabb = {0};
		tabb.dwButtonID = ActionItem->dwActID;
		tabb.pszModuleName = MODULE;
		CallService(MS_BB_REMOVEBUTTON, 0, (LPARAM)&tabb);
		ActionItem->wReggedTabButton = ActionItem->wReggedTabButton & 0x7FFF;
	}
}

int OnTabBBLoaded(WPARAM wParam, LPARAM lParam){
	int i;
	bTabBBPresents = TRUE;//bActionsDefined;
//	for (i=0; i<ActionItemsList.realCount;i++) {
	for (i=ActionItemsList.realCount-1;i>=0;i--) {
		MyActionItem* ActItem = ActionItemsList.items[i];
		if (ActItem->wFlags & UAF_REGTABB)
			ActItem->wReggedTabButton = AddTabBBButton(ActItem);
	}
	hTabBBPressHook = HookEvent(ME_MSG_BUTTONPRESSED, OnTabButtonPressed);
	return 0;
}

int OnTabButtonPressed(WPARAM wParam,LPARAM lParam) {
	CustomButtonClickData *cbcd=(CustomButtonClickData *)lParam;
	if(strcmp(cbcd->pszModule, MODULE)) return 0;
	ServiceCallWithFParam((WPARAM)cbcd->hContact, 0, cbcd->dwButtonId);
	return 1;
}


//=============== Other ====================================
HANDLE AddIcolibIcon (MyActionItem* ActionItem){
	if(!ActionItem->hIcolibIcon) {
		// add icon for action to icolib
		SKINICONDESC sid = {0};
		sid.cbSize = sizeof(sid);
		sid.ptszSection = _T(ICOLIB_ACTSECTION);
		sid.flags = SIDF_ALL_TCHAR;
		sid.cx= sid.cy = 16;
		sid.ptszDescription = ActionItem->tszActDescr;
		sid.pszName = ActionItem->szNameID;
		sid.ptszDefaultFile = tszMyPath;
		sid.iDefaultIndex = -IDI_ACTION;
		return (HANDLE)CallService(MS_SKIN2_ADDICON,0,(LPARAM)&sid);
	}
	else
		return ActionItem->hIcolibIcon;
}
void DeleteIcolibIcon(MyActionItem* ActionItem) {
	//will work only in MIM0.8.0.13+ (Rev7555+) - otherwise it will crash !
	DWORD dwMirVer = CallService(MS_SYSTEM_GETVERSION, 0, 0);
	if (dwMirVer >= 0x0008000D)
		CallService(MS_SKIN2_REMOVEICON, 0, (LPARAM)ActionItem->szNameID);
}

int AddCoreHotkey(MyActionItem* ActionItem){
//register core hotkeys
if (bCoreHotkeyPresents) {
	if (!ActionItem->bReggedHotkey) {
		HOTKEYDESC hkd = {0};
		hkd.cbSize = sizeof(hkd);
		hkd.pszName = ActionItem->szNameID;
		hkd.pszDescription = ActionItem->szActDescr;
		hkd.pszService = SERVICE_WITH_LPARAM_NAME;
		hkd.pszSection = MODULE;
		hkd.lParam = ActionItem->dwActID;
		return (CallService(MS_HOTKEY_REGISTER, 0, (LPARAM)&hkd) != 0) ? TRUE : FALSE;
	}
	return TRUE;
}
else
	return FALSE;
}

void DeleteCoreHotkey(MyActionItem* ActionItem){
	//delete core hotkeys
	//will work only from 0.8.0.15+ (Rev.7786+)
	// and bug with crash when hotkey synonims were used is fixed in 0.8.0.27 (Rev.8831)
	if (bCoreHotkeyPresents && ServiceExists(MS_HOTKEY_UNREGISTER) && ActionItem->bReggedHotkey) {
		CallService(MS_HOTKEY_UNREGISTER, 0, (LPARAM)ActionItem->szNameID);
		ActionItem->bReggedHotkey = FALSE;
	}
}

//================== GUI ===================================
int OptInit(WPARAM wParam, LPARAM lParam) {
	OPTIONSDIALOGPAGE odp = { 0 };
	odp.cbSize	= sizeof(odp);
	odp.flags	= ODPF_BOLDGROUPS | ODPF_TCHAR;
	//	odp.position = -790000000;
	odp.hInstance = hInst;
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPTIONS);
	odp.ptszTitle = _T("Actions");
	odp.ptszGroup = _T("Services");
	odp.ptszTab	= _T(MODULE);
	odp.nIDBottomSimpleControl = 0;
	odp.pfnDlgProc = DlgProcOpts;
	CallService( MS_OPT_ADDPAGE, wParam,( LPARAM )&odp );

	return 0;
}

INT_PTR CALLBACK DlgProcOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	LOGFONT lf;
	HFONT hf;
	switch ( msg ) {
		case WM_INITDIALOG: {
			hDialogWIndow = hwndDlg;
			hListView = GetDlgItem(hwndDlg, IDC_ACTIONSLIST);
			hImageListView = ImageList_Create(GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),(IsWinVerXPPlus()?ILC_COLOR32:ILC_COLOR16)|ILC_MASK,3,3);
			FillListViewWithActions();

			// variables help button
			if (bVariablePresents) {
				HICON hIcon=(HICON)CallService(MS_VARS_GETSKINITEM, 0, (LPARAM)VSI_HELPICON);
				char *szTipInfo = (char *)CallService(MS_VARS_GETSKINITEM, 0, (LPARAM)VSI_HELPTIPTEXT);
				if (hIcon != NULL) {
					SendMessage(GetDlgItem(hwndDlg, IDC_BTMMNVARSHELP), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);
					SendMessage(GetDlgItem(hwndDlg, IDC_BTMMSVARSHELP), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);
					SendMessage(GetDlgItem(hwndDlg, IDC_BTCMNVARSHELP), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);
					SendMessage(GetDlgItem(hwndDlg, IDC_BTCMSVARSHELP), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);
				}
				else {
					SetDlgItemText(hwndDlg, IDC_BTMMNVARSHELP, _T("V"));
					SetDlgItemText(hwndDlg, IDC_BTMMSVARSHELP, _T("V"));
					SetDlgItemText(hwndDlg, IDC_BTCMNVARSHELP, _T("V"));
					SetDlgItemText(hwndDlg, IDC_BTCMSVARSHELP, _T("V"));
				}
				if (szTipInfo == NULL) {
					SendMessage(GetDlgItem(hwndDlg, IDC_BTMMNVARSHELP), BUTTONADDTOOLTIP, (WPARAM)Translate("Open string formatting help for MainMenuitem name"), 0);
					SendMessage(GetDlgItem(hwndDlg, IDC_BTMMSVARSHELP), BUTTONADDTOOLTIP, (WPARAM)Translate("Open string formatting help for MainMenuitem showing"), 0);
					SendMessage(GetDlgItem(hwndDlg, IDC_BTCMNVARSHELP), BUTTONADDTOOLTIP, (WPARAM)Translate("Open string formatting help for ContactMenuitem name"), 0);
					SendMessage(GetDlgItem(hwndDlg, IDC_BTCMSVARSHELP), BUTTONADDTOOLTIP, (WPARAM)Translate("Open string formatting help for ContactMenuitem showing"), 0);
				}
				else {
					SendMessage(GetDlgItem(hwndDlg, IDC_BTMMNVARSHELP), BUTTONADDTOOLTIP, (WPARAM)szTipInfo, 0);
					SendMessage(GetDlgItem(hwndDlg, IDC_BTMMSVARSHELP), BUTTONADDTOOLTIP, (WPARAM)szTipInfo, 0);
					SendMessage(GetDlgItem(hwndDlg, IDC_BTCMNVARSHELP), BUTTONADDTOOLTIP, (WPARAM)szTipInfo, 0);
					SendMessage(GetDlgItem(hwndDlg, IDC_BTCMSVARSHELP), BUTTONADDTOOLTIP, (WPARAM)szTipInfo, 0);
				}
				SendDlgItemMessage(hwndDlg, IDC_BTMMNVARSHELP, BUTTONSETASFLATBTN, 0, 0);
				SendDlgItemMessage(hwndDlg, IDC_BTMMSVARSHELP, BUTTONSETASFLATBTN, 0, 0);
				SendDlgItemMessage(hwndDlg, IDC_BTCMNVARSHELP, BUTTONSETASFLATBTN, 0, 0);
				SendDlgItemMessage(hwndDlg, IDC_BTCMSVARSHELP, BUTTONSETASFLATBTN, 0, 0);
			}

			GetObject((HFONT)SendMessage(GetDlgItem(hwndDlg, IDC_STTOOLBAR), WM_GETFONT, 0, 0), sizeof(lf), &lf);
			lf.lfWeight = FW_BOLD;
			hf = CreateFontIndirect(&lf);
			
			// For Old GUI 
			SendMessage(GetDlgItem(hwndDlg, IDC_STTOOLBAR),	WM_SETFONT, (WPARAM)hf, TRUE);
			SendMessage(GetDlgItem(hwndDlg, IDC_STMMI),		WM_SETFONT, (WPARAM)hf, TRUE);
			SendMessage(GetDlgItem(hwndDlg, IDC_STCMI),		WM_SETFONT, (WPARAM)hf, TRUE);
			SendMessage(GetDlgItem(hwndDlg, IDC_STTMI),		WM_SETFONT, (WPARAM)hf, TRUE);
			SendMessage(GetDlgItem(hwndDlg, IDC_STOTHER),	WM_SETFONT, (WPARAM)hf, TRUE);

			// For New GUI
			// SendMessage(GetDlgItem(hwndDlg, IDC_STTOOLBAR),	WM_SETFONT, (WPARAM)hf, TRUE);
			// SendMessage(GetDlgItem(hwndDlg, IDC_CHBMMENUITEM),		WM_SETFONT, (WPARAM)hf, TRUE);
			// SendMessage(GetDlgItem(hwndDlg, IDC_CHBCMENUITEM),		WM_SETFONT, (WPARAM)hf, TRUE);
			// SendMessage(GetDlgItem(hwndDlg, IDC_CHBTMENUITEM),		WM_SETFONT, (WPARAM)hf, TRUE);
			// SendMessage(GetDlgItem(hwndDlg, IDC_STOTHER),	WM_SETFONT, (WPARAM)hf, TRUE);
			
			//DeleteObject(hf);

			TranslateDialogDefault( hwndDlg );
			return FALSE;		
		}
		case WM_COMMAND: {
			MyActionItem* ActItem = ActionItemsList.items[OptActiveItem];
			switch (LOWORD(wParam)) {
				case IDC_CHBHOTKEY:
					ActItem->wFlags ^= UAF_REGHOTKEY;
					break;
				case IDC_CHBMTBB:
					ActItem->wFlags ^= UAF_REGMTBB;
					break;
				case IDC_CHBTABBB:
					ActItem->wFlags ^= UAF_REGTABB;
					break;
				case IDC_CHBMMENUITEM:
					ActItem->wFlags ^= UAF_REGMMENU;
					break;
				case IDC_CHBMMENUSEP:
					ActItem->wFlags ^= UAF_MMENUSEP;
					break;
				case IDC_CHBCMENUSEP:
					ActItem->wFlags ^= UAF_CMENUSEP;
					break;
				case IDC_EDMMENUPN:
					if ((HIWORD(wParam) == EN_CHANGE) && !bRefreshing)
						GetDlgItemText(hwndDlg, IDC_EDMMENUPN, ActItem->tszMMenuPopup, MAX_ACT_DESC_LEN);
					break;
				case IDC_EDMMENUN:
					if ((HIWORD(wParam) == EN_CHANGE) && !bRefreshing)
						GetDlgItemText(hwndDlg, IDC_EDMMENUN, ActItem->tszMMenuNameVars, MAX_VARSCRIPT_LEN);
					break;
				case IDC_EDCMENUN:
					if ((HIWORD(wParam) == EN_CHANGE) && !bRefreshing)
						GetDlgItemText(hwndDlg, IDC_EDCMENUN, ActItem->tszCMenuNameVars, MAX_VARSCRIPT_LEN);
					break;
				case IDC_CHBCMENUITEM:
					ActItem->wFlags ^= UAF_REGCMENU;
					break;
				case IDC_EDCMENUPN:
					if ((HIWORD(wParam) == EN_CHANGE) && !bRefreshing)
						GetDlgItemText(hwndDlg, IDC_EDCMENUPN, ActItem->tszCMenuPopup, MAX_ACT_DESC_LEN);
					break;
				case IDC_CHBMMVARS:
					ActItem->wFlags ^= UAF_MMENU_VARS;
					break;
				case IDC_CHBCMVARS:
					ActItem->wFlags ^= UAF_CMENU_VARS;
					break;
				case IDC_EDMMENUVARSTR:
					if ((HIWORD(wParam) == EN_CHANGE) && !bRefreshing)
						GetDlgItemText(hwndDlg, IDC_EDMMENUVARSTR, ActItem->tszMMenuShowWhenVars, MAX_VARSCRIPT_LEN);
					break;
				case IDC_EDCMENUVARSTR:
					if ((HIWORD(wParam) == EN_CHANGE) && !bRefreshing)
						GetDlgItemText(hwndDlg, IDC_EDCMENUVARSTR, ActItem->tszCMenuShowWhenVars, MAX_VARSCRIPT_LEN);
					break;
				case IDC_CHBTMENUITEM:
					ActItem->wFlags ^= UAF_REGTMENU;
					break;
				case IDC_EDTMENUPN:
					if ((HIWORD(wParam) == EN_CHANGE) && !bRefreshing)
						GetDlgItemText(hwndDlg, IDC_EDTMENUPN, ActItem->tszTMenuPopup, MAX_ACT_DESC_LEN);
					break;
				case IDC_CHBTTBB:
					ActItem->wFlags ^= UAF_REGTTBB;
					break;
				case IDC_BTMMNVARSHELP:
					variables_showhelp(hwndDlg, IDC_EDMMENUN, VHF_FULLDLG|VHF_SETLASTSUBJECT, NULL, NULL);
					break;
				case IDC_BTMMSVARSHELP:
					variables_showhelp(hwndDlg, IDC_EDMMENUVARSTR, VHF_FULLDLG|VHF_SETLASTSUBJECT, NULL, NULL);
					break;
				case IDC_BTCMNVARSHELP:
					variables_showhelp(hwndDlg, IDC_EDCMENUN, VHF_FULLDLG|VHF_SETLASTSUBJECT, NULL, NULL);
					break;
				case IDC_BTCMSVARSHELP:
					variables_showhelp(hwndDlg, IDC_EDCMENUVARSTR, VHF_FULLDLG|VHF_SETLASTSUBJECT, NULL, NULL);
					break;
			}
			// enable the 'apply' button
			if(!bRefreshing)
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			break;
		}
		case WM_NOTIFY:
			if(ActionItemsList.realCount == 0) break;
			else {
			//MyActionItem* ActItem = ActionItemsList.items[OptActiveItem];
			if ((LPNMHDR)lParam == NULL) return TRUE;
			switch(((LPNMHDR)lParam)->idFrom) {
				case 0:
					switch (((LPNMHDR)lParam)->code) {
						case PSN_APPLY: {
							int i;
							for (i=0; i<ActionItemsList.realCount;i++) {
								WriteSetting(ActionItemsList.items[i]);
								SetAllActionUsers(ActionItemsList.items[i]);
							}
							break;
						}
					}
					break;
				}
			switch (LOWORD(wParam)) {
				case IDC_ACTIONSLIST: {
					switch (((LPNMHDR)lParam)->code) {
						case LVN_ITEMCHANGED:{
							NMITEMACTIVATE* LVItemAct = (LPNMITEMACTIVATE)lParam;
							if ((LVItemAct->iItem >= 0) && (LVItemAct->uChanged == LVIF_STATE)) {
								if (LVItemAct->uNewState == (LVIS_SELECTED | LVIS_FOCUSED)) {
									MyActionItem* ActItem = ActionItemsList.items[LVItemAct->lParam/*iItem*/];
									OptActiveItem = LVItemAct->lParam;//iItem;
									bRefreshing = TRUE;
									CheckDlgButton(hwndDlg, IDC_CHBHOTKEY, (ActItem->wFlags & UAF_REGHOTKEY) ? BST_CHECKED : BST_UNCHECKED);
									CheckDlgButton(hwndDlg, IDC_CHBMTBB, (ActItem->wFlags & UAF_REGMTBB) ? BST_CHECKED : BST_UNCHECKED);

									CheckDlgButton(hwndDlg, IDC_CHBMMENUITEM, (ActItem->wFlags & UAF_REGMMENU) ? BST_CHECKED : BST_UNCHECKED);
									CheckDlgButton(hwndDlg, IDC_CHBMMENUSEP, (ActItem->wFlags & UAF_MMENUSEP) ? BST_CHECKED : BST_UNCHECKED);
									CheckDlgButton(hwndDlg, IDC_CHBCMENUSEP, (ActItem->wFlags & UAF_CMENUSEP) ? BST_CHECKED : BST_UNCHECKED);
									SetDlgItemText(hwndDlg, IDC_EDMMENUPN, ActItem->tszMMenuPopup);
									SetDlgItemText(hwndDlg, IDC_EDMMENUN, ActItem->tszMMenuNameVars);

									CheckDlgButton(hwndDlg, IDC_CHBCMENUITEM, (ActItem->wFlags & UAF_REGCMENU) ? BST_CHECKED : BST_UNCHECKED);
									SetDlgItemText(hwndDlg, IDC_EDCMENUN, ActItem->tszCMenuNameVars);
									SetDlgItemText(hwndDlg, IDC_EDCMENUPN, ActItem->tszCMenuPopup);
									CheckDlgButton(hwndDlg, IDC_CHBMMVARS, (ActItem->wFlags & UAF_MMENU_VARS) ? BST_CHECKED : BST_UNCHECKED);
									SetDlgItemText(hwndDlg, IDC_EDMMENUVARSTR, ActItem->tszMMenuShowWhenVars);
									CheckDlgButton(hwndDlg, IDC_CHBCMVARS, (ActItem->wFlags & UAF_CMENU_VARS) ? BST_CHECKED : BST_UNCHECKED);
									SetDlgItemText(hwndDlg, IDC_EDCMENUVARSTR, ActItem->tszCMenuShowWhenVars);

									CheckDlgButton(hwndDlg, IDC_CHBTMENUITEM, (ActItem->wFlags & UAF_REGTMENU) ? BST_CHECKED : BST_UNCHECKED);
									SetDlgItemText(hwndDlg, IDC_EDTMENUPN, ActItem->tszTMenuPopup);

									CheckDlgButton(hwndDlg, IDC_CHBTTBB, (ActItem->wFlags & UAF_REGTTBB) ? BST_CHECKED : BST_UNCHECKED);
									CheckDlgButton(hwndDlg, IDC_CHBTABBB, (ActItem->wFlags & UAF_REGTABB) ? BST_CHECKED : BST_UNCHECKED);
									bRefreshing = FALSE;
								}
							}
							return TRUE;
						} //LVN_ITEMCHANGED
					} //switch (((LPNMHDR)lParam)->code) 
				} //IDC_ACTIONSLIST
			} //switch (LOWORD(wParam))
			break;
		} // case WM_NOTIFY
	default:
		break;
	} //switch ( msg )
	return 0;
}

void FillListViewWithActions() {
	LVCOLUMN lvc = { 0 };
	RECT aRect = { 0 };
	int iCurrentSortIndex = 0;
	int iCurrentArrayIndex = 0;
	int i, j;
	MyActionItem* ActItemTmp;

	ListView_SetImageList(hListView, hImageListView, LVSIL_SMALL);
	lvc.mask = LVCF_TEXT;
	ListView_SetExtendedListViewStyleEx(hListView, LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP, LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	lvc.pszText = TranslateT("Actions");
	ListView_InsertColumn(hListView, 0, &lvc);
	
	iCurrentSortIndex = -1;
	iCurrentArrayIndex = 0;

	for (i=0; i<ActionItemsList.realCount;i++) {
		LVITEM lvi = { 0 };

		for (j=0; j<ActionItemsList.realCount;j++) {
			MyActionItem* ActItemTmp2 = ActionItemsList.items[j];
			if (ActItemTmp2->wSortIndex == iCurrentSortIndex + 1) {
				iCurrentArrayIndex = j;
				iCurrentSortIndex = ActItemTmp2->wSortIndex;
				break;
			}
		}
		ActItemTmp = ActionItemsList.items[iCurrentArrayIndex];

		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvi.iImage = lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.lParam = iCurrentArrayIndex;
		lvi.pszText = ActItemTmp->tszActDescr;
		lvi.iItem = ListView_InsertItem(hListView, &lvi);
		ImageList_AddIcon(hImageListView, (HICON)CallService(MS_SKIN2_GETICONBYHANDLE, 0, (LPARAM)ActItemTmp->hIcolibIcon));
		if (i == 0)
			OptActiveItem = iCurrentArrayIndex;
	}
	if (ActionItemsList.realCount > 0) {
		ListView_SetItemState(hListView, 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		//OptActiveItem = 0;
	}
	else {
		//EnableWindow(GetDlgItem(hwndDlg, IDC_GB), FALSE);
	}
	GetClientRect(hListView, &aRect);
	ListView_SetColumnWidth(hListView, 0, aRect.right - aRect.left);

	ShowWindow(GetDlgItem(hDialogWIndow,   IDC_BTMMNVARSHELP), ((ActionItemsList.realCount>0) && bVariablePresents)? SW_SHOW : SW_HIDE);
	EnableWindow(GetDlgItem(hDialogWIndow,   IDC_BTCMNVARSHELP), (ActionItemsList.realCount>0) && bVariablePresents);
	ShowWindow(GetDlgItem(hDialogWIndow,   IDC_BTMMSVARSHELP), (ActionItemsList.realCount>0) && bVariablePresents);
	ShowWindow(GetDlgItem(hDialogWIndow,   IDC_BTCMSVARSHELP), (ActionItemsList.realCount>0) && bVariablePresents);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_EDMMENUN), (ActionItemsList.realCount>0) && bVariablePresents);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_EDCMENUN), (ActionItemsList.realCount>0) && bVariablePresents);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_EDMMENUVARSTR), (ActionItemsList.realCount>0) && bVariablePresents);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_EDCMENUVARSTR), (ActionItemsList.realCount>0) && bVariablePresents);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_CHBMMVARS), (ActionItemsList.realCount>0) && bVariablePresents);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_CHBCMVARS), (ActionItemsList.realCount>0) && bVariablePresents);

	EnableWindow(GetDlgItem(hDialogWIndow, IDC_CHBTTBB), (ActionItemsList.realCount>0) && bTTBPresents);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_CHBMTBB), (ActionItemsList.realCount>0) && bMTBPresents);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_CHBTABBB),(ActionItemsList.realCount>0) && bTabBBPresents);

	EnableWindow(GetDlgItem(hDialogWIndow, IDC_CHBMMENUITEM), ActionItemsList.realCount>0);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_CHBMMENUSEP), ActionItemsList.realCount>0);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_CHBCMENUITEM), ActionItemsList.realCount>0);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_CHBCMENUSEP), ActionItemsList.realCount>0);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_CHBTMENUITEM), (ActionItemsList.realCount>0) && bTrayMenuPresents);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_EDMMENUPN), ActionItemsList.realCount>0);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_EDCMENUPN), ActionItemsList.realCount>0);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_EDTMENUPN), (ActionItemsList.realCount>0) && bTrayMenuPresents);
	EnableWindow(GetDlgItem(hDialogWIndow, IDC_CHBHOTKEY), (ActionItemsList.realCount>0) && bCoreHotkeyPresents);
}
