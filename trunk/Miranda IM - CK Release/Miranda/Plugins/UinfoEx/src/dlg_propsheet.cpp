/*
UserinfoEx plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2006-2010 DeathAxe, Yasnovidyashii, Merlin_de, K. Romanov, Kreol

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

/**
 * System & local includes:
 **/
#include "commonheaders.h"
#include "dlg_propsheet.h"
#include "psp_base.h"
#include "svc_ExImport.h"
#include "svc_reminder.h"

/**
 * Miranda includes:
 **/
#include <m_protocols.h>
#include <m_icq.h>

#define UPDATEANIMFRAMES		20

// internal dialog message handler
#define M_CHECKONLINE			(WM_USER+10)
#define HM_PROTOACK				(WM_USER+11)
#define HM_SETTING_CHANGED		(WM_USER+12)
#define HM_RELOADICONS			(WM_USER+13)
#define HM_SETWINDOWTITLE		(WM_USER+14)

#define TIMERID_UPDATING		1
#ifndef TIMERID_RENAME
	#define TIMERID_RENAME		2
#endif

// flags for the PS structure
#define PSF_CHANGED				0x00000100
#define PSF_LOCKED				0x00000200
#define PSF_INITIALIZED			0x00000400

#define INIT_ICONS_NONE			0
#define INIT_ICONS_OWNER		1
#define INIT_ICONS_CONTACT		2
#define INIT_ICONS_ALL			(INIT_ICONS_OWNER|INIT_ICONS_CONTACT)

/***********************************************************************************************************
 * internal variables
 ***********************************************************************************************************/

static BOOLEAN bInitIcons			= INIT_ICONS_NONE;
static HANDLE ghWindowList			= NULL;
static HANDLE ghDetailsInitEvent	= NULL;
HANDLE hContactDel, hUserInfo;

static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

CPsHdr::CPsHdr() :
_ignore(10, (LIST<TCHAR>::FTSortFunc)_tcscmp)
{
	_dwSize = sizeof(*this);
	_hContact = NULL;
	_pszProto = NULL;
	_pszPrefix = NULL;
	_pPages = NULL;
	_numPages = 0;
	_dwFlags = 0;
	_hImages = NULL;
}

CPsHdr::~CPsHdr() 
{
	INT i;

	// delete data
	for (i = 0 ; i < _ignore.getCount(); i++)
	{
		if (_ignore[i]) mir_free(_ignore[i]);
	}
	// delete the list
	_ignore.destroy();
}

VOID CPsHdr::Free_pPages() 
{
	for ( int i = 0 ; i < _numPages; i++)
		if (_pPages[i]) delete _pPages[i];
	_numPages = 0;
	MIR_FREE(_pPages);
}

/***********************************************************************************************************
 * class CPsUpload
 ***********************************************************************************************************/

class CPsUpload {
public:
	enum EPsUpReturn {
		UPLOAD_CONTINUE = 0,
		UPLOAD_FINISH = 1,
		UPLOAD_FINISH_CLOSE = 2
	};

private:
	PROTOCOLDESCRIPTOR **_pPd;
	INT			_numProto;
	BOOLEAN	_bExitAfterUploading;
	HANDLE	_hUploading;
	LPPS		_pPs;
	
	/**
	 * @class	Upload
	 * @class	CPsUpload
	 * @desc	start upload process for the current protocol
	 * @param	none
	 * @return	0 on success
	 * @return	1 otherwise
	 **/
	INT Upload()
	{
		CPsTreeItem *pti;

		// check if icq is online
		if (!IsProtoOnline((*_pPd)->szName)) {
			TCHAR		szMsg[MAX_PATH];
			LPTSTR	ptszProto;

			ptszProto = mir_a2t((*_pPd)->szName);
			mir_sntprintf(szMsg, SIZEOF(szMsg), TranslateT("Protocol '%s' is offline"), ptszProto);
			mir_free(ptszProto);

			MsgBox(_pPs->hDlg, MB_ICON_WARNING, TranslateT("Upload Details"), szMsg,
				TranslateT("You are not currently connected to the ICQ network.\nYou must be online in order to update your information on the server.\n\nYour changes will be saved to database only."));
		}
		// start uploading process
		else {
			_hUploading = (HANDLE)CallProtoService((*_pPd)->szName, PS_CHANGEINFOEX, CIXT_FULL, NULL);
			if (_hUploading && _hUploading != (HANDLE)CALLSERVICE_NOTFOUND) {
				EnableWindow(_pPs->pTree->Window(), FALSE);
				if (pti = _pPs->pTree->CurrentItem()) {
					EnableWindow(pti->Wnd(), FALSE);
				}
				EnableWindow(GetDlgItem(_pPs->hDlg, IDOK), FALSE);
				EnableWindow(GetDlgItem(_pPs->hDlg, IDAPPLY), FALSE);
				mir_snprintf(_pPs->szUpdating, SIZEOF(_pPs->szUpdating), "%s (%s)", Translate("Uploading"), (*_pPd)->szName);
				ShowWindow(GetDlgItem(_pPs->hDlg, TXT_UPDATING), SW_SHOW);
				SetTimer(_pPs->hDlg, TIMERID_UPDATING, 100, NULL);
				return 0;
			}
		}
		return 1;
	}

public:
	/**
	 * @name	CPsUpload
	 * @class	CPsUpload
	 * @desc	retrieves the list of installed protocols and initializes the class
	 * @param	pPs			- the owning propertysheet
	 * @param	bExitAfter	- whether the dialog is to close after upload or not
	 * @return	nothing
	 **/
	CPsUpload(LPPS pPs, BOOLEAN bExitAfter)
	{
		_pPs = pPs;
		_pPd = NULL;
		_numProto = 0;
		_hUploading = NULL;
		_bExitAfterUploading = bExitAfter;
	}

	INT UploadFirst() {
		// create a list of all protocols which support uploading contact information
		if (CallService(MS_PROTO_ENUMPROTOCOLS, (WPARAM)&_numProto, (LPARAM)&_pPd)) {
			return _bExitAfterUploading ? UPLOAD_FINISH_CLOSE : UPLOAD_FINISH;
		}
		return UploadNext();
	}

	/**
	 * @name	~CPsUpload
	 * @class	CPsUpload
	 * @desc	clear pointer to the upload object
	 * @param	none
	 * @return	nothing
	 **/
	~CPsUpload()
		{ _pPs->pUpload = NULL; }

	/**
	 * @name	Handle
	 * @class	CPsUpload
	 * @desc	returns the handle of the current upload process
	 * @param	none
	 * @return	handle of the current upload process
	 **/
	__inline HANDLE Handle() const
		{ return _hUploading; };

	/**
	 * @name	UploadNext
	 * @class	CPsUpload
	 * @desc	Search the next protocol which supports uploading contact information
	 *			and start uploading. Delete the object if ready
	 * @param	none
	 * @return	nothing
	 **/
	INT	UploadNext()
	{
		CHAR str[MAXMODULELABELLENGTH];
		while (_pPd && *_pPd && _numProto-- > 0) {
			if ((*_pPd)->type == PROTOTYPE_PROTOCOL) {
				mir_strncpy(str, (*_pPd)->szName, MAXMODULELABELLENGTH);
				mir_strncat(str, PS_CHANGEINFOEX, MAXMODULELABELLENGTH);
				if (ServiceExists(str) && !Upload()) {
					_pPd++;
					return UPLOAD_CONTINUE;
				}
			}
			_pPd++;
		}
		return _bExitAfterUploading ? UPLOAD_FINISH_CLOSE : UPLOAD_FINISH;
	}
};

/***********************************************************************************************************
 * propertysheet
 ***********************************************************************************************************/

/**
 * @name	SortProc()
 * @desc	used for sorting the tab pages
 *
 * @return	-1 or 0 or 1
 **/
static INT SortProc(CPsTreeItem** item1, CPsTreeItem** item2)
{
	if (*item1 && *item2) {
		if ((*item2)->Pos() > (*item1)->Pos()) return -1;
		if ((*item2)->Pos() < (*item1)->Pos()) return 1;
	}
	return 0;
}

/**
 * This service routine creates the DetailsDialog
 * @param	wParam		- handle to contact
 * @param	lParam		- not used
 *
 * @retval	0 on success
 * @retval	1 on failure
 **/
static INT_PTR ShowDialog(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd;

	// update some cached settings
	myGlobals.ShowPropsheetColours = DB::Setting::GetByte(SET_PROPSHEET_SHOWCOLOURS, TRUE);
	myGlobals.WantAeroAdaption = DB::Setting::GetByte(SET_PROPSHEET_AEROADAPTION, TRUE);

	// allow only one dialog per user
	if (hWnd = WindowList_Find(ghWindowList, (HANDLE)wParam)) {
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);
	}
	else {
		CPsHdr psh;
		POINT metrics;
		BOOLEAN bScanMetaSubContacts = FALSE;
		HICON hDefIcon;

		// init the treeview options
		if (DB::Setting::GetByte(SET_PROPSHEET_SORTITEMS, FALSE))
			psh._dwFlags |= PSTVF_SORTTREE;
		if (DB::Setting::GetByte(SET_PROPSHEET_GROUPS, TRUE))
			psh._dwFlags |= PSTVF_GROUPS;
		// create imagelist
		metrics.x = GetSystemMetrics(SM_CXSMICON);
		metrics.y = GetSystemMetrics(SM_CYSMICON);
		if ((psh._hImages = ImageList_Create(metrics.x, metrics.y, (IsWinVerXPPlus() ? ILC_COLOR32 : ILC_COLOR16) | ILC_MASK, 0, 1)) == NULL) {
			MsgErr(NULL, LPGENT("Creating the imagelist failed!"));
			return 1;
		}

		hDefIcon = IcoLib_GetIcon(ICO_TREE_DEFAULT);
		if (!hDefIcon)
		{
			hDefIcon = (HICON) LoadImage(ghInst, MAKEINTRESOURCE(IDI_DEFAULT), IMAGE_ICON, metrics.x, metrics.y, 0);
		}
		// add the default icon to imagelist
		ImageList_AddIcon(psh._hImages, hDefIcon);

		// init contact
		psh._hContact = (HANDLE)wParam;
		if (psh._hContact == NULL) {
			// mark owner icons as initiated
			bInitIcons |= INIT_ICONS_OWNER;
			psh._pszProto = NULL;
			psh._pszPrefix = NULL;
		}
		else {
			// get contact's protocol
			psh._pszPrefix = psh._pszProto = DB::Contact::Proto((HANDLE)wParam);
			if (psh._pszProto == NULL) {
				MsgErr(NULL, LPGENT("Could not find contact's protocol. Maybe it is not active!"));
				return 1;
			}
			// prepare scanning for metacontact's subcontact's pages
			if (bScanMetaSubContacts = DB::Module::IsMetaAndScan(psh._pszProto))
				psh._dwFlags |= PSF_PROTOPAGESONLY_INIT;
		}
		// add the pages
		NotifyEventHooks(ghDetailsInitEvent, (WPARAM)&psh, wParam);
		if (!psh._pPages || !psh._numPages) {
			MsgErr(NULL, LPGENT("No pages have been added. Canceling dialog creation!"));
			return 1;
		}
		// metacontacts sub pages
		if (bScanMetaSubContacts)
		{
			INT numSubs = DB::MetaContact::SubCount((HANDLE)wParam);

			psh._dwFlags &= ~PSF_PROTOPAGESONLY_INIT;
			psh._dwFlags |= PSF_PROTOPAGESONLY;
			for (INT i = 0; i < numSubs; i++) 
			{
				psh._hContact = DB::MetaContact::Sub((HANDLE)wParam, i);
				psh._nSubContact = i;
				if (psh._hContact) 
				{
					psh._pszProto = DB::Contact::Proto(psh._hContact);
					if ((INT_PTR)psh._pszProto != CALLSERVICE_NOTFOUND)
						NotifyEventHooks(ghDetailsInitEvent, (WPARAM)&psh, (LPARAM)psh._hContact);
				}
			}
			psh._hContact = (HANDLE)wParam;
		}

		// sort the pages by the position read from database
		if (!(psh._dwFlags & PSTVF_SORTTREE)) {
			 qsort(psh._pPages, psh._numPages, sizeof(CPsTreeItem*), 
				(INT (*)(const VOID*, const VOID*))SortProc);
		}
		// create the dialog itself
		hWnd = CreateDialogParam(ghInst, MAKEINTRESOURCE(IDD_DETAILS), NULL, DlgProc, (LPARAM)&psh);
		if (!hWnd) {
			MsgErr(NULL, LPGENT("Details dialog failed to be created. Returning error is %d."), GetLastError());
		}
		else
			MagneticWindows_AddWindow(hWnd);
	}
	return 0;
}

/**
 * @name	AddPage()
 * @desc	this adds a new pages
 * @param	wParam		- The List of pages we want to add the new one to
 * @param	lParam		- it's the page to add
 *
 * @return	0
 **/
static INT_PTR AddPage(WPARAM wParam, LPARAM lParam)
{
	CPsHdr				*pPsh	= (CPsHdr*)wParam;
	OPTIONSDIALOGPAGE	*odp	= (OPTIONSDIALOGPAGE*)lParam;

	// check size of the handled structures
	if (pPsh == NULL || odp == NULL || pPsh->_dwSize != sizeof(CPsHdr))
	{
		return 1;
	}
	if (odp->cbSize != sizeof(OPTIONSDIALOGPAGE) &&
			odp->cbSize != OPTIONPAGE_OLD_SIZE2 &&
			odp->cbSize != OPTIONPAGE_OLD_SIZE3) 
	{
		MsgErr(NULL, LPGENT("The Page to add has invalid size %d bytes!"), odp->cbSize);
		return 1;
	}

	// try to check whether the flag member is initialized or not
	odp->flags = odp->flags > (ODPF_UNICODE|ODPF_BOLDGROUPS|ODPF_ICON|PSPF_PROTOPREPENDED) ? 0 : odp->flags;

	if (pPsh->_dwFlags & (PSF_PROTOPAGESONLY|PSF_PROTOPAGESONLY_INIT))
	{
		BOOLEAN bIsUnicode = (odp->flags & ODPF_UNICODE) == ODPF_UNICODE;
		TCHAR* ptszTitle = bIsUnicode ? mir_tstrdup(odp->ptszTitle) : mir_a2t(odp->pszTitle);

		// avoid adding pages for a meta subcontact, which have been added for a metacontact.
		if (pPsh->_dwFlags & PSF_PROTOPAGESONLY) 
		{	
			if (pPsh->_ignore.getIndex(ptszTitle) != -1)
			{
				mir_free(ptszTitle);
				return 0;
			}
		}
		// init ignore list with pages added by metacontact
		else if (pPsh->_dwFlags & PSF_PROTOPAGESONLY_INIT) 
		{
			pPsh->_ignore.insert(mir_tstrdup(ptszTitle));
		}
		mir_free(ptszTitle);
	}

	// create the new tree item
	CPsTreeItem *pNew = new CPsTreeItem();
	if (pNew) 
	{
		if (pNew->Create(pPsh, odp))
		{
			MIR_DELETE(pNew);
			return 1;
		}

		// resize the array
		pPsh->_pPages = (CPsTreeItem**)mir_realloc(pPsh->_pPages, (pPsh->_numPages + 1) * sizeof(CPsTreeItem*));
		if (pPsh->_pPages != NULL) {
			pPsh->_pPages[pPsh->_numPages++] = pNew;
			return 0;
		}
		pPsh->_numPages = 0;
	}
	return 1;
}

/**
 * @name	OnDeleteContact()
 * @desc	a user was deleted, so need to close its details dialog, if one open
 *
 * @return	0
 **/
static INT OnDeleteContact(WPARAM wParam, LPARAM lParam)
{
	 HWND hWnd = WindowList_Find(ghWindowList, (HANDLE)wParam);
	 if (hWnd != NULL)
		 DestroyWindow(hWnd);
	 return 0;
}

/**
 * @name	OnShutdown()
 * @desc	we need to emptify the windowlist
 *
 * @return	0
 **/
static INT OnShutdown(WPARAM wParam, LPARAM lParam)
{
	UnhookEvent(hContactDel);
	UnhookEvent(hPreShutDown);
	UnhookEvent(hUserInfo);
	WindowList_BroadcastAsync(ghWindowList, WM_DESTROY, 0, 0);
	return 0;
}

/**
 * @name	AddProtocolPages()
 * @desc	is called by Miranda if user selects to display the userinfo dialog
 * @param	odp			- optiondialogpage structure to use
 * @param	wParam		- the propertysheet init structure to pass
 * @param	pszProto	- the protocol name to prepend as item name (can be NULL)
 *
 * @return	0
 **/
static INT AddProtocolPages(OPTIONSDIALOGPAGE& odp, WPARAM wParam, LPSTR pszProto = NULL)
{
	TCHAR szTitle[MAX_PATH];
	const BYTE ofs = (pszProto) ? mir_sntprintf(szTitle, SIZEOF(szTitle), _T(TCHAR_STR_PARAM) _T("\\"), pszProto) : 0;

	odp.ptszTitle = szTitle;
	
	odp.pszTemplate	= MAKEINTRESOURCEA(IDD_CONTACT_GENERAL);
	odp.position	= 0x8000000;
	odp.pfnDlgProc	= PSPProcGeneral;
	odp.hIcon		= (HICON)ICONINDEX(IDI_TREE_GENERAL);
	mir_tcsncpy(szTitle + ofs, LPGENT("General"), SIZEOF(szTitle) - ofs);
	AddPage(wParam, (LPARAM)&odp);

	odp.pszTemplate	= MAKEINTRESOURCEA(IDD_CONTACT_ADDRESS);
	odp.position	= 0x8000001;
	odp.pfnDlgProc	= PSPProcContactHome;
	odp.hIcon		= (HICON)ICONINDEX(IDI_TREE_ADDRESS);
	mir_tcsncpy(szTitle + ofs, LPGENT("General") _T("\\") LPGENT("Contact (private)"), SIZEOF(szTitle) - ofs);
	AddPage(wParam, (LPARAM)&odp);

	odp.pszTemplate	= MAKEINTRESOURCEA(IDD_CONTACT_ORIGIN);
	odp.position	= 0x8000002;
	odp.pfnDlgProc	= PSPProcOrigin;
	odp.hIcon		= (HICON)ICONINDEX(IDI_TREE_ADVANCED);
	mir_tcsncpy(szTitle + ofs, LPGENT("General") _T("\\") LPGENT("Origin"), SIZEOF(szTitle) - ofs);
	AddPage(wParam, (LPARAM)&odp);
		
	odp.pszTemplate	= MAKEINTRESOURCEA(IDD_CONTACT_ANNIVERSARY);
	odp.position	= 0x8000003;
	odp.pfnDlgProc	= PSPProcAnniversary;
	odp.hIcon		= (HICON)ICONINDEX(IDI_BIRTHDAY);
	mir_tcsncpy(szTitle + ofs,  LPGENT("General") _T("\\") LPGENT("Anniversaries"), SIZEOF(szTitle) - ofs);
	AddPage(wParam, (LPARAM)&odp);

	odp.pszTemplate	= MAKEINTRESOURCEA(IDD_CONTACT_COMPANY);
	odp.position	= 0x8000004;
	odp.pfnDlgProc	= PSPProcCompany;
	odp.hIcon		= (HICON)ICONINDEX(IDI_TREE_COMPANY);
	mir_tcsncpy(szTitle + ofs, LPGENT("Work"), SIZEOF(szTitle) - ofs);
	AddPage(wParam, (LPARAM)&odp);

	odp.pszTemplate	= MAKEINTRESOURCEA(IDD_CONTACT_ADDRESS);
	odp.position	= 0x8000005;
	odp.pfnDlgProc	= PSPProcContactWork;
	odp.hIcon		= (HICON)ICONINDEX(IDI_TREE_ADDRESS);
	mir_tcsncpy(szTitle + ofs, LPGENT("Work") _T("\\") LPGENT("Contact (Work)"), SIZEOF(szTitle) - ofs);
	AddPage(wParam, (LPARAM)&odp);
		
	odp.pszTemplate	= MAKEINTRESOURCEA(IDD_CONTACT_ABOUT);
	odp.position	= 0x8000006;
	odp.pfnDlgProc	= PSPProcAbout;
	odp.hIcon		= (HICON)ICONINDEX(IDI_TREE_ABOUT);
	mir_tcsncpy(szTitle + ofs, LPGENT("About"), SIZEOF(szTitle) - ofs);
	AddPage(wParam, (LPARAM)&odp);

	odp.pszTemplate	= MAKEINTRESOURCEA(IDD_CONTACT_PROFILE);
	odp.position	= 0x8000007;
	odp.pfnDlgProc	= PSPProcContactProfile;
	odp.hIcon		= (HICON)ICONINDEX(IDI_TREE_PROFILE);
	mir_tcsncpy(szTitle + ofs, LPGENT("About") _T("\\") LPGENT("Profile"), SIZEOF(szTitle) - ofs);
	AddPage(wParam, (LPARAM)&odp);
	return 0;
}

/**
 * @name	InitDetails
 * @desc	is called by Miranda if user selects to display the userinfo dialog
 * @param	wParam		- the propertysheet init structure to pass
 * @param	lParam		- handle to contact whose information are read
 *
 * @return	0
 **/
static INT InitDetails(WPARAM wParam, LPARAM lParam)
{
	CPsHdr* pPsh = (CPsHdr*)wParam;

	if (!(pPsh->_dwFlags & PSF_PROTOPAGESONLY)) {

		OPTIONSDIALOGPAGE odp;
		BOOLEAN bChangeDetailsEnabled = myGlobals.CanChangeDetails && DB::Setting::GetByte(SET_PROPSHEET_CHANGEMYDETAILS, FALSE);
		
		// important to avoid craches!!
		ZeroMemory(&odp, sizeof(odp));

		if (lParam || bChangeDetailsEnabled) {
			odp.cbSize = sizeof(odp);
			odp.hInstance = ghInst;
			odp.flags = /*ODPF_BOLDGROUPS|*/ODPF_ICON|ODPF_TCHAR;
			odp.ptszGroup = IcoLib_GetDefaultIconFileName();

			if (lParam) {

				// ignore common pages for weather contacts
				if (!pPsh->_pszProto || stricmp(pPsh->_pszProto, "weather")) 
				{
					AddProtocolPages(odp, wParam);
					odp.ptszTitle = LPGENT("About") _T("\\") LPGENT("Notes");
				}
				else {
					odp.ptszTitle = LPGENT("Notes");
				}
				odp.pszTemplate = MAKEINTRESOURCEA(IDD_CONTACT_ABOUT);
				odp.position = 0x8000008;
				odp.pfnDlgProc = PSPProcMyNotes;
				odp.hIcon = (HICON)ICONINDEX(IDI_TREE_NOTES);
				AddPage(wParam, (LPARAM)&odp);
			}
			/* Editing owner details no longer supported due to leak of common interface for all protocols.
			else 
			if (!(pPsh->_dwFlags & PSTVF_INITICONS))
			{
				PROTOCOLDESCRIPTOR **pd;
				INT ProtoCount, i;
				CHAR str[MAXMODULELABELLENGTH];

				odp.flags |= PSPF_PROTOPREPENDED;

				// create a list of all protocols which support uploading contact information
				if (!CallService(MS_PROTO_ENUMPROTOCOLS, (WPARAM)&ProtoCount, (LPARAM)&pd)) {
					for (i = 0; i < ProtoCount; i++) {
						if (pd[i]->type == PROTOTYPE_PROTOCOL) {
							pPsh->_pszProto = pd[i]->szName;
							mir_snprintf(str, MAXMODULELABELLENGTH, "%s"PS_CHANGEINFOEX, pd[i]->szName);
							if (ServiceExists(str)) AddProtocolPages(odp, wParam, pd[i]->szName);
						}
					}
				}
			}
			*/
		}
	}
	return 0;
}

/**
 * @name	InitTreeIcons()
 * @desc	initalize all treeview icons
 * @param	none
 *
 * @return	nothing
 **/
VOID DlgContactInfoInitTreeIcons()
{
	// make sure this is run only once
	if (!(bInitIcons & INIT_ICONS_ALL)) {
		CPsHdr	psh;
		POINT	metrics	= {0};
		INT		i		= 0;

		psh._dwFlags = PSTVF_INITICONS;

		metrics.x = GetSystemMetrics(SM_CXSMICON);
		metrics.y = GetSystemMetrics(SM_CYSMICON);
		if (psh._hImages = ImageList_Create(metrics.x, metrics.y, (IsWinVerXPPlus() ? ILC_COLOR32 : ILC_COLOR16) | ILC_MASK, 0, 1)) {
			HICON hDefIcon = IcoLib_GetIcon(ICO_TREE_DEFAULT);
			if (!hDefIcon)
			{
				hDefIcon = (HICON) LoadImage(ghInst, MAKEINTRESOURCE(IDI_DEFAULT), IMAGE_ICON, metrics.x, metrics.y, 0);
			}
			// add the default icon to imagelist
			ImageList_AddIcon(psh._hImages, hDefIcon);
		}

		// avoid pages from loading doubled
		if (!(bInitIcons & INIT_ICONS_CONTACT)) {
			LPCSTR pszContactProto = NULL;
			PROTOCOLDESCRIPTOR **pd;
			INT ProtoCount = 0;

			psh._dwFlags |= PSF_PROTOPAGESONLY_INIT;
			
			// enumerate all protocols
			if (!CallService(MS_PROTO_ENUMPROTOCOLS, (WPARAM)&ProtoCount, (LPARAM)&pd)) {
				for (i = 0; i < ProtoCount; i++) {
					if (pd[i]->type == PROTOTYPE_PROTOCOL) {
						// enumerate all contacts
						for (psh._hContact = DB::Contact::FindFirst();
							psh._hContact != NULL;
							psh._hContact = DB::Contact::FindNext(psh._hContact))
						{
							// compare contact's protocol to the current one, to add
							pszContactProto = DB::Contact::Proto(psh._hContact);
							if ((INT_PTR)pszContactProto != CALLSERVICE_NOTFOUND && !mir_strcmp(pd[i]->szName, pszContactProto)) {
								// call a notification for the contact to retrieve all protocol specific tree items
								NotifyEventHooks(ghDetailsInitEvent, (WPARAM)&psh, (LPARAM)psh._hContact);
								if (psh._pPages) {
									psh.Free_pPages();
									psh._dwFlags = PSTVF_INITICONS|PSF_PROTOPAGESONLY;
								}
								break;
							}
						}
					}
				}
			}
			bInitIcons |= INIT_ICONS_CONTACT;
		}
		// load all treeitems for owner contact
		if (!(bInitIcons & INIT_ICONS_OWNER)) {
			psh._hContact = NULL;
			psh._pszProto = NULL;
			NotifyEventHooks(ghDetailsInitEvent, (WPARAM)&psh, (LPARAM)psh._hContact);
			if (psh._pPages) {
				psh.Free_pPages();
			}
			bInitIcons |= INIT_ICONS_OWNER;
		}
		ImageList_Destroy(psh._hImages);
	}
}

/**
 * @name	UnLoadModule()
 * @desc	unload the UserInfo Module
 *
 * @return	nothing
 **/
VOID DlgContactInfoUnLoadModule()
{
	DestroyHookableEvent(ghDetailsInitEvent);
}

/**
 * @name	LoadModule()
 * @desc	load the UserInfo Module
 *
 * @return	nothing
 **/
VOID DlgContactInfoLoadModule()
{
	ghDetailsInitEvent = CreateHookableEvent(ME_USERINFO_INITIALISE);

	myCreateServiceFunction(MS_USERINFO_SHOWDIALOG, ShowDialog);
	myCreateServiceFunction(MS_USERINFO_ADDPAGE, AddPage);

	hContactDel = HookEvent(ME_DB_CONTACT_DELETED, OnDeleteContact);
	hPreShutDown = HookEvent(ME_SYSTEM_PRESHUTDOWN, OnShutdown);
	hUserInfo = HookEvent(ME_USERINFO_INITIALISE, InitDetails);
	ghWindowList = (HANDLE)CallService(MS_UTILS_ALLOCWINDOWLIST, 0, 0);

	// check whether changing my details via UserInfoEx is basically possible
	{
		PROTOACCOUNT **pAcc;
		INT i, nAccCount;
		
		myGlobals.CanChangeDetails = FALSE;
		if (MIRSUCCEEDED(ProtoEnumAccounts(&nAccCount, &pAcc)))
		{
			for (i = 0; (i < nAccCount) && !myGlobals.CanChangeDetails; i++) 
			{
				if (IsProtoAccountEnabled(pAcc[i])) 
				{
					// update my contact information on icq server
					myGlobals.CanChangeDetails = MIREXISTS(CallProtoService(pAcc[i]->szModuleName, PS_CHANGEINFOEX, NULL, NULL));
				}
			}
		}
	}
}

static void ResetUpdateInfo(LPPS pPs)
{
	INT i;

	// free the array of accomblished acks
	for (i = 0; i < (INT)pPs->nSubContacts; i++)
		MIR_FREE(pPs->infosUpdated[i].acks);
	MIR_FREE(pPs->infosUpdated);
	pPs->nSubContacts = 0;
}

/*
============================================================================================
	PropertySheet's Dialog Procedures
============================================================================================
*/

/**
 * @name	DlgProc()
 * @desc	dialog procedure for the main propertysheet dialog box
 *
 * @return	0 or 1
 **/
static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPPS pPs = (LPPS)GetUserData(hDlg);
		 
	// do not process any message if pPs is no longer existent
	if (!PtrIsValid(pPs) && uMsg != WM_INITDIALOG)
		return FALSE;

	switch (uMsg)
	{
	/**
	 * @class	WM_INITDIALOG
	 * @desc	initiates all dialog controls
	 * @param	wParam - not used
	 *				lParam - pointer to a PSHDR structure, which contains all information to create the dialog
	 *
	 * @return	TRUE if everything is ok, FALSE if dialog creation should fail
	 **/
	case WM_INITDIALOG:
		{
			CPsHdr*	pPsh = (CPsHdr*)lParam;
			WORD	needWidth = 0;
			RECT	rc;
			
			if (!pPsh || pPsh->_dwSize != sizeof(CPsHdr))
				return FALSE;

			TranslateDialogDefault(hDlg);

			//
			// create data structures
			//
			if (!(pPs = (LPPS)mir_alloc(sizeof(PS))))
				return FALSE;
			ZeroMemory(pPs, sizeof(PS));

			if (!(pPs->pTree = new CPsTree(pPs)))
				return FALSE;
			if (!(pPs->pTree->Create(GetDlgItem(hDlg, STATIC_TREE), pPsh))) {
				return FALSE;
			}
			SetUserData(hDlg, pPs);
			pPs->hDlg = hDlg;
			pPs->dwFlags |= PSF_LOCKED;
			pPs->hContact = pPsh->_hContact;
			pPs->hProtoAckEvent = HookEventMessage(ME_PROTO_ACK, hDlg, HM_PROTOACK);
			pPs->hSettingChanged = HookEventMessage(ME_DB_CONTACT_SETTINGCHANGED, hDlg, HM_SETTING_CHANGED);
			pPs->hIconsChanged = HookEventMessage(ME_SKIN2_ICONSCHANGED, hDlg, HM_RELOADICONS);

			ShowWindow(GetDlgItem(hDlg, IDC_PAGETITLEBG),IsAeroMode());
			ShowWindow(GetDlgItem(hDlg, IDC_PAGETITLEBG2),!IsAeroMode());
			
			//
			// set icons
			//
			SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(ghInst, MAKEINTRESOURCE(IDI_MAIN)));
			DlgProc(hDlg, HM_RELOADICONS, NULL, NULL);

			//
			// load basic protocol for current contact (for faster load later on and better handling for owner protocol)
			//
			if (pPs->hContact) mir_strncpy(pPs->pszProto, pPsh->_pszPrefix, MAXMODULELABELLENGTH);

			// set the windowtitle
			DlgProc(hDlg, HM_SETWINDOWTITLE, NULL, NULL);
			
			// translate Userinfo buttons
			{
				SendDlgItemMessage(hDlg, BTN_UPDATE, BUTTONTRANSLATE, NULL, NULL);
				SendDlgItemMessage(hDlg, IDOK, BUTTONTRANSLATE, NULL, NULL);
				SendDlgItemMessage(hDlg, IDCANCEL, BUTTONTRANSLATE, NULL, NULL);
				SendDlgItemMessage(hDlg, IDAPPLY, BUTTONTRANSLATE, NULL, NULL);
				SendDlgItemMessage(hDlg, BTN_EXPORT, BUTTONADDTOOLTIP, (WPARAM)TranslateT("Export to file"), MBF_TCHAR);
				SendDlgItemMessage(hDlg, BTN_IMPORT, BUTTONADDTOOLTIP, (WPARAM)TranslateT("Import from file"), MBF_TCHAR);
			}

			//
			// set bold font for name in description area
			//
			{
				LOGFONT lf;
				HFONT hNormalFont = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);

				GetObject(hNormalFont, sizeof(lf), &lf);
				lf.lfHeight = 22;
				mir_tcscpy(lf.lfFaceName, _T("Segoe UI"));
				pPs->hCaptionFont = CreateFontIndirect(&lf);
				SendDlgItemMessage(hDlg, IDC_PAGETITLE, WM_SETFONT, (WPARAM)pPs->hCaptionFont, 0);

				GetObject(hNormalFont, sizeof(lf), &lf);
				lf.lfWeight = FW_BOLD;
				pPs->hBoldFont = CreateFontIndirect(&lf);
			}

			//
			// initialize the optionpages and tree control
			//
			if (!pPs->pTree->InitTreeItems((LPWORD)&needWidth))
				return FALSE;

			//
			// move and resize dialog and its controls
			//
			{
				HWND hCtrl;
				RECT rcTree;
				POINT pt = { 0, 0 };
				INT addWidth = 0;

				// at least add width of scrollbar
				needWidth += 8 + GetSystemMetrics(SM_CXVSCROLL);

				// get tree rectangle
				GetWindowRect(hDlg, &pPs->rcDisplay);
				GetWindowRect(pPs->pTree->Window(), &rcTree);
				ClientToScreen(hDlg, &pt);
				OffsetRect(&rcTree, -pt.x, -pt.y);

				// calculate the amout of pixels to resize the dialog by?
				if (needWidth > rcTree.right - rcTree.left) {
					RECT rcMax = { 0, 0, 0, 0 };

					rcMax.right = 280;
					MapDialogRect(hDlg, &rcMax);
					addWidth = min(needWidth, rcMax.right) - rcTree.right + rcTree.left;
					rcTree.right += addWidth;
					// resize tree
					MoveWindow(pPs->pTree->Window(), rcTree.left, rcTree.top, rcTree.right - rcTree.left, rcTree.bottom - rcTree.top, FALSE);

					pPs->rcDisplay.right += addWidth;
					MoveWindow(hDlg, pPs->rcDisplay.left, pPs->rcDisplay.top, 
						pPs->rcDisplay.right - pPs->rcDisplay.left, 
						pPs->rcDisplay.bottom - pPs->rcDisplay.top, FALSE);

				}
				GetClientRect(GetDlgItem(hDlg, IDC_PAGETITLEBG), &rc);
				// calculate dislpay area for pages
				OffsetRect(&pPs->rcDisplay, -pt.x, -pt.y);
				pPs->rcDisplay.bottom = rcTree.bottom;
				pPs->rcDisplay.left = rcTree.right + 2;
				pPs->rcDisplay.top = rcTree.top+rc.bottom;
				pPs->rcDisplay.right -= 2;

				// move and resize the rest of the controls
				if (addWidth > 0) {
					const WORD	idResize[] = { IDC_HEADERBAR, STATIC_LINE2 };
					const WORD	idMove[] = { IDC_PAGETITLE, IDC_PAGETITLEBG, IDC_PAGETITLEBG2, IDOK, IDCANCEL, IDAPPLY };
					WORD i;

					for (i = 0; i < SIZEOF(idResize); i++) {
						if (hCtrl = GetDlgItem(hDlg, idResize[i])) {
							GetWindowRect(hCtrl, &rc);
							OffsetRect(&rc, -pt.x, -pt.y);
							MoveWindow(hCtrl, rc.left, rc.top, rc.right - rc.left + addWidth, rc.bottom - rc.top, FALSE);
						}
					}						
					for (i = 0; i < SIZEOF(idMove); i++) {
						if (hCtrl = GetDlgItem(hDlg, idMove[i])) {
							GetWindowRect(hCtrl, &rc);
							OffsetRect(&rc, -pt.x, -pt.y);
							MoveWindow(hCtrl, rc.left + addWidth, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);
						}
					}
				}
				// restore window position and add required size
				Utils_RestoreWindowPositionNoSize(hDlg, NULL, MODNAME, "DetailsDlg");
			}

			//
			// show the first propsheetpage
			//
			// finally add the dialog to the window list
			WindowList_Add(ghWindowList, hDlg, pPs->hContact);

			// show the dialog
			pPs->dwFlags &= ~PSF_LOCKED;
			pPs->dwFlags |= PSF_INITIALIZED;


			//
			// initialize the "updating" button and statustext and check for online status
			//
			pPs->updateAnimFrame = 0;
			if (pPs->hContact && *pPs->pszProto) 
			{
				GetDlgItemTextA(hDlg, TXT_UPDATING, pPs->szUpdating, SIZEOF(pPs->szUpdating));
				ShowWindow(GetDlgItem(hDlg, TXT_UPDATING), SW_HIDE);
				
				if (DlgProc(hDlg, M_CHECKONLINE, NULL, NULL)) 
				{
					DlgProc(hDlg, WM_COMMAND, MAKEWPARAM(BTN_UPDATE, BN_CLICKED), (LPARAM)GetDlgItem(hDlg, BTN_UPDATE));
				}
			}


			{
				INT nPage = pPs->pTree->CurrentItemIndex();
				if (!pPs->pTree->IsIndexValid(nPage))
					nPage = 0;
				TreeView_Select(pPs->pTree->Window(), NULL, TVGN_CARET);
				TreeView_Select(pPs->pTree->Window(), pPs->pTree->TreeItemHandle(nPage), TVGN_CARET);
				ShowWindow(hDlg, SW_SHOW);
			}
		}
		return TRUE;

	/**
	 * @class	WM_TIMER
	 * @desc	is called to display the "updating" text in the status area
	 * @param	wParam - not used
	 *			lParam - not used
	 *
	 * @return	always FALSE
	 **/
	case WM_TIMER:
		switch (wParam) 
		{
		case TIMERID_UPDATING:
			{
				CHAR str[84];

				mir_snprintf(str, SIZEOF(str), "%.*s%s%.*s", pPs->updateAnimFrame%10, ".........", pPs->szUpdating, pPs->updateAnimFrame%10, ".........");
				SetDlgItemTextA(hDlg, TXT_UPDATING, str);
				if (++pPs->updateAnimFrame == UPDATEANIMFRAMES)
					pPs->updateAnimFrame = 0;
				return FALSE;
			}
		}
		break;

	case 0x031E: /*WM_DWMCOMPOSITIONCHANGED:*/
		{
			ShowWindow(GetDlgItem(hDlg, IDC_PAGETITLEBG),IsAeroMode());
			InvalidateRect(hDlg, NULL, TRUE);
		}
		break;

	/**
	 * @class	WM_CTLCOLORSTATIC
	 * @desc	sets the colour of some of the dialog's static controls
	 * @param	wParam - HWND of the contrls
	 *			lParam - HDC for drawing
	 *
	 * @return	StockObject
	 **/
	case WM_CTLCOLORSTATIC:
		switch (GetWindowLongPtr((HWND)lParam, GWLP_ID)) 
		{
		case TXT_UPDATING:
			{
				COLORREF textCol, bgCol, newCol;
				INT ratio;

				textCol = GetSysColor(COLOR_BTNTEXT);
				bgCol = GetSysColor(COLOR_3DFACE);
				ratio = abs(UPDATEANIMFRAMES/2 - pPs->updateAnimFrame) * 510 / UPDATEANIMFRAMES;
				newCol = RGB(GetRValue(bgCol) + (GetRValue(textCol) - GetRValue(bgCol)) * ratio / 256,
						GetGValue(bgCol) + (GetGValue(textCol) - GetGValue(bgCol)) * ratio / 256,
						GetBValue(bgCol) + (GetBValue(textCol) - GetBValue(bgCol)) * ratio / 256);
				SetTextColor((HDC)wParam, newCol);
				SetBkColor((HDC)wParam, GetSysColor(COLOR_3DFACE));
			}
			return (INT_PTR)GetSysColorBrush(COLOR_3DFACE);
		case IDC_PAGETITLE:
		case IDC_PAGETITLEBG:
			{
				if (IsAeroMode())
				{
					SetTextColor((HDC)wParam, RGB(0,90,180));
					SetBkColor((HDC)wParam, RGB(255, 255, 255));
					return (INT_PTR)GetStockObject(WHITE_BRUSH);
				}
				else
				{
					SetBkColor((HDC)wParam, GetSysColor(COLOR_3DFACE));
					return (INT_PTR)GetSysColorBrush(COLOR_3DFACE);
				}
			}
		}
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (INT_PTR)GetStockObject(NULL_BRUSH);

	/**
	 * @class	PSM_CHANGED
	 * @desc	indicates the propertysheet and the current selected page as changed
	 * @param	wParam - not used
	 *			lParam - not used
	 *
	 * @return	TRUE if successful FALSE if the dialog is locked at the moment
	 **/
	case PSM_CHANGED:
		if (!(pPs->dwFlags & PSF_LOCKED)) 
		{	 
			pPs->dwFlags |= PSF_CHANGED;
			pPs->pTree->CurrentItem()->AddFlags(PSPF_CHANGED);
			EnableWindow(GetDlgItem(hDlg, IDAPPLY), TRUE);
			return TRUE;
		}
		break;

	/**
	 * @class	PSM_GETBOLDFONT
	 * @desc	returns the bold font
	 * @param	wParam - not used
	 *			lParam - pointer to a HFONT, which takes the boldfont
	 *
	 * @return	TRUE if successful, FALSE otherwise
	 **/
	case PSM_GETBOLDFONT:
		if (pPs->hBoldFont && lParam) 
		{
			*(HFONT*)lParam = pPs->hBoldFont;
			SetWindowLongPtr(hDlg, DWLP_MSGRESULT, (LONG_PTR)pPs->hBoldFont);
			return TRUE;
		}
		*(HFONT*)lParam = NULL;
		SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0l);
		break;

	/**
	 * @class	PSM_GETCONTACT
	 * @desc	returns the handle to the contact, associated with this propertysheet
	 * @param	wParam - index or -1 for current item
	 *			lParam - pointer to a HANDLE, which takes the contact handle
	 *
	 * @return	TRUE if successful, FALSE otherwise
	 **/
	case PSM_GETCONTACT:
		if (lParam) 
		{
			CPsTreeItem *pti = ((INT)wParam != -1) 
				? pPs->pTree->TreeItem((INT)wParam)
				: pPs->pTree->CurrentItem();

			// prefer to return the contact accociated with the current page
			if (pti && pti->hContact() != INVALID_HANDLE_VALUE) {
				*(HANDLE*)lParam = pti->hContact();
				SetWindowLongPtr(hDlg, DWLP_MSGRESULT, (LONG_PTR)pti->hContact());
				return TRUE;
			}
			
			// return contact who owns the details dialog
			if (pPs->hContact != INVALID_HANDLE_VALUE) {
				*(HANDLE*)lParam = pPs->hContact;
				SetWindowLongPtr(hDlg, DWLP_MSGRESULT, (LONG_PTR)pPs->hContact);
				return TRUE;
			}
			*(HANDLE*)lParam = NULL;
			SetWindowLongPtr(hDlg, DWLP_MSGRESULT, NULL);
		}
		break;

	/**
	 * @class	PSM_GETBASEPROTO
	 * @desc	returns the basic protocol module for the associated contact
	 * @param	wParam - index or -1 for current item
	 *			lParam - pointer to a LPCSTR which takes the protocol string pointer
	 *
	 * @return	TRUE if successful, FALSE otherwise
	 **/
	case PSM_GETBASEPROTO:
		if (lParam) {
			CPsTreeItem *pti = ((INT)wParam != -1) 
				? pPs->pTree->TreeItem((INT)wParam)
				: pPs->pTree->CurrentItem();
			
			if (pti && pti->Proto()) {
				// return custom protocol for the current page
				*(LPCSTR*)lParam = pti->Proto();
				SetWindowLongPtr(hDlg, DWLP_MSGRESULT, (LONG_PTR)pti->Proto());
				return TRUE;
			}

			if (*pPs->pszProto) {
				// return global protocol
				*(LPSTR*)lParam = pPs->pszProto;
				SetWindowLongPtr(hDlg, DWLP_MSGRESULT, (LONG_PTR)pPs->pszProto);
				return TRUE;
			}
		}
		*(LPCSTR*)lParam = NULL;
		SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0l);
		break;

	/**
	 * @class	PSM_ISLOCKED
	 * @desc	returns the lock state of the propertysheetpage
	 * @param	wParam - not used
	 *			lParam - not used
	 *
	 * @return	TRUE if propertysheet is locked, FALSE if not
	 **/
	case PSM_ISLOCKED:
	{
		BOOLEAN bLocked = (pPs->dwFlags & PSF_LOCKED) == PSF_LOCKED;

		SetWindowLongPtr(hDlg, DWLP_MSGRESULT, bLocked);
		return bLocked;
	}

	/**
	 * @class	PSM_FORCECHANGED
	 * @desc	force all propertysheetpages to update their controls with new values from the database
	 * @param	wParam - whether to replace changed settings too or not
		*		lParam - not used
	 *
	 * @return	always FALSE
	 **/
	case PSM_FORCECHANGED:
		if (!(pPs->dwFlags & PSF_LOCKED)) { 
			BOOLEAN bChanged;

			pPs->dwFlags |= PSF_LOCKED;
			if (bChanged = pPs->pTree->OnInfoChanged())
				pPs->dwFlags |= PSF_CHANGED;
			else
				pPs->dwFlags &= ~PSF_CHANGED;
			pPs->dwFlags &= ~PSF_LOCKED;
			EnableWindow(GetDlgItem(hDlg, IDAPPLY), bChanged);
		}
		break;

	/**
	 * @class	PSM_DLGMESSAGE
	 * @desc	Sends a message to a specified propertysheetpage
	 * @param	wParam - not used
	 *			lParam - LPDLGCOMMAND structure, which contains information about the message to forward
	 *
	 * @return	E_FAIL if the page was not found
	 **/
	case PSM_DLGMESSAGE:
	{
		LPDLGCOMMAND pCmd = (LPDLGCOMMAND)lParam;
		CPsTreeItem *pti;
		
		if (pCmd && (pti = pPs->pTree->FindItemByResource(pCmd->hInst, pCmd->idDlg)) &&	pti->Wnd()) {
			if (!pCmd->idDlgItem)
				return SendMessage(pti->Wnd(), pCmd->uMsg, pCmd->wParam, pCmd->lParam);
			else
				return SendDlgItemMessage(pti->Wnd(), pCmd->idDlgItem, pCmd->uMsg, pCmd->wParam, pCmd->lParam);
		}
		return E_FAIL;
	}

	/**
	 * @class	PSM_GETPAGEHWND
	 * @desc	get the window handle for a specified propertysheetpage
	 * @param	wParam - recource id of the dialog recource
	 *			lParam - hinstance of the plugin, which created the dialog box
	 *
	 * @return	TRUE if handle was found and dialog was created before, false otherwise
	 **/
	case PSM_GETPAGEHWND:
		{
			CPsTreeItem *pti;
			if (pti = pPs->pTree->FindItemByResource((HINSTANCE)lParam, wParam)) {
				SetWindowLongPtr(hDlg, DWLP_MSGRESULT, (LONG_PTR)pti->Wnd());
				return (pti->Wnd() != NULL);
			}
		}
		return FALSE;

	case PSM_ISAEROMODE:
		{
			BOOLEAN bIsAeroMode = IsAeroMode();
			if (lParam)
			{
				*(BOOLEAN*)lParam = bIsAeroMode;
			}
			return (INT_PTR)bIsAeroMode;
		}

	/**
	 * @class	HM_SETWINDOWTITLE
	 * @desc	set the window title and text of the infobar
	 * @param	wParam - not used
	 *			lParam - DBCONTACTWRITESETTING structure if called by HM_SETTING_CHANGED message handler
	 *
	 * @return	FALSE
	 **/
	case HM_SETWINDOWTITLE:
	{
		LPCTSTR pszName = NULL; 
		TCHAR	newTitle[MAX_PATH];
		RECT	rc;
		POINT	pt = { 0, 0 };
		HWND	hName = GetDlgItem(hDlg, TXT_NAME);
		DBCONTACTWRITESETTING* pdbcws = (DBCONTACTWRITESETTING*)lParam;

		if (!pPs->hContact)
			pszName = TranslateT("Owner");
		else
		if (pdbcws && pdbcws->value.type == DBVT_TCHAR)
			pszName = pdbcws->value.ptszVal;
		else
			pszName = DB::Contact::DisplayName(pPs->hContact);

		SetWindowText(hName, pszName);
		mir_sntprintf(newTitle, MAX_PATH, _T("%s - %s"), pszName, TranslateT("Edit Contact Information"));
		SetWindowText(hDlg, newTitle);
		mir_sntprintf(newTitle, MAX_PATH, _T("%s\n%s"), TranslateT("Edit Contact Information"), pszName);
		SetDlgItemText(hDlg, IDC_HEADERBAR, newTitle);

		// redraw the name control
		ScreenToClient(hDlg, &pt);
		GetWindowRect(hName, &rc);
		OffsetRect(&rc, pt.x, pt.y);
		InvalidateRect(hDlg, &rc, TRUE);
		break;
	}

	/**
	 * @class	HM_RELOADICONS
	 * @desc	handles the changed icon event from the icolib plugin and reloads all icons
	 * @param	wParam - not used
	 *			lParam - not used
	 *
	 * @return	FALSE
	 **/
	case HM_RELOADICONS:
	{
		HWND	hCtrl;
		HICON	hIcon;
		const ICONCTRL idIcon[] = {
			{ ICO_DLG_DETAILS,	STM_SETIMAGE,	ICO_DLGLOGO	},
			{ ICO_BTN_UPDATE,	BM_SETIMAGE,	BTN_UPDATE	},
			{ ICO_BTN_OK,		BM_SETIMAGE,	IDOK		},
			{ ICO_BTN_CANCEL,	BM_SETIMAGE,	IDCANCEL	},
			{ ICO_BTN_APPLY,	BM_SETIMAGE,	IDAPPLY		}
		};
		
		const INT numIconsToSet = DB::Setting::GetByte(SET_ICONS_BUTTONS, 1) ? SIZEOF(idIcon) : 1;
		
		IcoLib_SetCtrlIcons(hDlg, idIcon, numIconsToSet);
		
		if (hCtrl = GetDlgItem(hDlg, BTN_IMPORT)) {
			hIcon = IcoLib_GetIcon(ICO_BTN_IMPORT);
			SendMessage(hCtrl, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			SetWindowText(hCtrl, hIcon ? _T("") : _T("I"));
		}
		if (hCtrl = GetDlgItem(hDlg, BTN_EXPORT)) {
			hIcon = IcoLib_GetIcon(ICO_BTN_EXPORT);
			SendMessage(hCtrl, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			SetWindowText(hCtrl, hIcon ? _T("") : _T("E"));
		}
		// update page icons
		if (PtrIsValid(pPs) && (pPs->dwFlags & PSF_INITIALIZED))
			pPs->pTree->OnIconsChanged();
		break;
	}

	/**
	 * @class	M_CHECKONLINE
	 * @desc	determines whether miranda is online or not
	 * @param	wParam - not used
	 *			lParam - not used
	 *
	 * @return	TRUE if online, FALSE if offline
	 **/
	case M_CHECKONLINE:
	{
		if (IsProtoOnline(pPs->pszProto))
		{
			EnableWindow(GetDlgItem(hDlg, BTN_UPDATE), !IsWindowVisible(GetDlgItem(hDlg, TXT_UPDATING)));
			return TRUE;
		}

		EnableWindow(GetDlgItem(hDlg, BTN_UPDATE), FALSE);
		EnableWindow(GetDlgItem(hDlg, TXT_UPDATING), FALSE);
		break;
	}

	/**
	 * @class	HM_PROTOACK
	 * @desc	handles all acks from the protocol plugin
	 * @param	wParam - not used
	 *			lParam - pointer to a ACKDATA structure
	 *
	 * @return	FALSE
	 **/
	case HM_PROTOACK:
	{
		ACKDATA *ack = (ACKDATA*)lParam;
		INT i, iSubContact;
		CPsTreeItem *pti;

		if (!ack->hContact && ack->type == ACKTYPE_STATUS)
			return DlgProc(hDlg, M_CHECKONLINE, NULL, NULL);
		
		switch (ack->type) {
		
			case ACKTYPE_SETINFO:
			{
				if (ack->hContact != pPs->hContact || !pPs->pUpload || pPs->pUpload->Handle() != ack->hProcess)
					break;
				if (ack->result == ACKRESULT_SUCCESS) {
					ShowWindow(GetDlgItem(hDlg, TXT_UPDATING), SW_HIDE);
					KillTimer(hDlg, TIMERID_UPDATING);
					// upload next protocols contact information
					switch (pPs->pUpload->UploadNext()) {
						case CPsUpload::UPLOAD_FINISH_CLOSE:
							MIR_DELETE(pPs->pUpload);
							DestroyWindow(hDlg);
						case CPsUpload::UPLOAD_CONTINUE:
							return FALSE;
						case CPsUpload::UPLOAD_FINISH:
							MIR_DELETE(pPs->pUpload);
							break;
					}
					DlgProc(hDlg, M_CHECKONLINE, NULL, NULL);
					EnableWindow(pPs->pTree->Window(), TRUE);
					if (pti = pPs->pTree->CurrentItem()) {
						EnableWindow(pti->Wnd(), TRUE);
					}
					EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
					pPs->dwFlags &= ~PSF_LOCKED;
				}
				else 
				if (ack->result == ACKRESULT_FAILED)	{
					MsgBox(hDlg, MB_ICON_WARNING, 
						LPGENT("Upload ICQ Details"),
						LPGENT("Upload failed"),
						LPGENT("Your details were not uploaded successfully.\nThey were written to database only."));
					KillTimer(hDlg, TIMERID_UPDATING);
					ShowWindow(GetDlgItem(hDlg, TXT_UPDATING), SW_HIDE);
					DlgProc(hDlg, M_CHECKONLINE, NULL, NULL);

					// upload next protocols contact information
					switch (pPs->pUpload->UploadNext()) {
						case CPsUpload::UPLOAD_FINISH_CLOSE:
							MIR_DELETE(pPs->pUpload);
							DestroyWindow(hDlg);
						case CPsUpload::UPLOAD_CONTINUE:
							return 0;
						case CPsUpload::UPLOAD_FINISH:
							MIR_DELETE(pPs->pUpload);
							break;
					}
					if (pti = pPs->pTree->CurrentItem()) {
						EnableWindow(pti->Wnd(), TRUE);
					}
					// activate all controls again
					EnableWindow(pPs->pTree->Window(), TRUE);
					EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
					pPs->dwFlags &= ~PSF_LOCKED;
				}
				break;
			}

			case ACKTYPE_GETINFO:

				// is contact the owner of the dialog or any metasubcontact of the owner? skip handling otherwise!
				if (ack->hContact != pPs->hContact) {
					
					if (!myGlobals.szMetaProto)
						break;
					
					if (!DB::Setting::GetByte(SET_META_SCAN, TRUE))
						break;
					
					for (i = 0; i < pPs->nSubContacts; i++) {
						if (pPs->infosUpdated[i].hContact == ack->hContact) {
							iSubContact = i;
							break;
						}
					}
					if (i == pPs->nSubContacts)
						break;
				}
				else {
					iSubContact = 0;
				}

				// if they're not gonna send any more ACK's don't let that mean we should crash
				if (!pPs->infosUpdated || (!ack->hProcess && !ack->lParam)) {
					ResetUpdateInfo(pPs);

					ShowWindow(GetDlgItem(hDlg, TXT_UPDATING), SW_HIDE);
					KillTimer(hDlg, TIMERID_UPDATING);
					DlgProc(hDlg, M_CHECKONLINE, NULL, NULL);
					break;
				}

				if (iSubContact < pPs->nSubContacts) {

					// init the acks structure for a sub contact
					if (pPs->infosUpdated[iSubContact].acks == NULL) {
						pPs->infosUpdated[iSubContact].acks	= (LPINT)mir_calloc(sizeof(INT) * (INT)(INT_PTR)ack->hProcess);
						pPs->infosUpdated[iSubContact].count = (INT)(INT_PTR)ack->hProcess;
					}

					if (ack->result == ACKRESULT_SUCCESS || ack->result == ACKRESULT_FAILED)
						pPs->infosUpdated[iSubContact].acks[ack->lParam] = 1;

					// check for pending tasks
					for (iSubContact = 0; iSubContact < pPs->nSubContacts; iSubContact++) {
						for (i = 0; i < pPs->infosUpdated[iSubContact].count; i++) {
							if (pPs->infosUpdated[iSubContact].acks[i] == 0)
								break;
						}
						if (i < pPs->infosUpdated[iSubContact].count)
							break;
					}
				}

				// all acks are done, finish updating
				if (iSubContact >= pPs->nSubContacts) {
					ResetUpdateInfo(pPs);
					ShowWindow(GetDlgItem(hDlg, TXT_UPDATING), SW_HIDE);
					KillTimer(hDlg, TIMERID_UPDATING);
					DlgProc(hDlg, M_CHECKONLINE, NULL, NULL);
				}
		}
		break;
	}

	/**
	 * @class	HM_SETTING_CHANGED
	 * @desc	This message is called by the ME_DB_CONTACT_SETTINGCHANGED event and forces all
	 *			unedited settings in the propertysheetpages to be updated
	 * @param	wParam	- handle to the contact whose settings are to be changed
	 *			lParam	- DBCONTACTWRITESETTING structure that identifies the changed setting
	 * @return	FALSE
	 **/
	case HM_SETTING_CHANGED:
		if (!(pPs->dwFlags & PSF_LOCKED)) {
			HANDLE hContact = (HANDLE)wParam;
			DBCONTACTWRITESETTING* pdbcws = (DBCONTACTWRITESETTING*)lParam;

			if (hContact != pPs->hContact) {
				if (!myGlobals.szMetaProto)
					break;
				if (pPs->hContact != (HANDLE)CallService(MS_MC_GETMETACONTACT, (WPARAM)hContact, NULL))
					break;
				if (!DB::Setting::GetByte(SET_META_SCAN, TRUE))
					break;
			}

			if (
				!mir_stricmp(pdbcws->szSetting, SET_CONTACT_MYHANDLE) ||
				!mir_stricmp(pdbcws->szSetting, SET_CONTACT_NICK)
			 )
			{
				// force the update of all propertysheetpages
				DlgProc(hDlg, PSM_FORCECHANGED, NULL, NULL);
				// update the windowtitle
				DlgProc(hDlg, HM_SETWINDOWTITLE, NULL, lParam);
			}
			else if (
				!mir_stricmp(pdbcws->szModule, USERINFO) ||
				!mir_stricmp(pdbcws->szModule, pPs->pszProto) ||
				!mir_stricmp(pdbcws->szModule, MOD_MBIRTHDAY)
			 )
			{
				// force the update of all propertysheetpages
				DlgProc(hDlg, PSM_FORCECHANGED, NULL, NULL);
			}
		}
		break;

	case WM_NOTIFY:
		switch (wParam) {

			//
			// Notification Messages sent by the TreeView
			//
			case STATIC_TREE:
				switch (((LPNMHDR)lParam)->code) {
					case TVN_SELCHANGING:
					{	 
						pPs->dwFlags |= PSF_LOCKED;
						pPs->pTree->OnSelChanging();
						pPs->dwFlags &= ~PSF_LOCKED;
						break;
					}

					case TVN_SELCHANGED:
						if (pPs->dwFlags & PSF_INITIALIZED) {
							pPs->dwFlags |= PSF_LOCKED;
							pPs->pTree->OnSelChanged((LPNMTREEVIEW)lParam);
							if (pPs->pTree->CurrentItem())
							{
								RECT rc;
								POINT pt = { 0, 0 };

								GetWindowRect(GetDlgItem(hDlg, IDC_PAGETITLE), &rc);
								ScreenToClient(hDlg, &pt);
								OffsetRect(&rc, pt.x, pt.y);
								SetDlgItemText(hDlg, IDC_PAGETITLE, pPs->pTree->CurrentItem()->Label());
								InvalidateRect(GetDlgItem(hDlg, IDC_PAGETITLEBG), &rc, TRUE);
								InvalidateRect(hDlg, &rc, TRUE);
							}
							pPs->dwFlags &= ~PSF_LOCKED;
						}
						break;

					case TVN_BEGINDRAG:
					{
						LPNMTREEVIEW nmtv = (LPNMTREEVIEW)lParam;

						if (nmtv->itemNew.hItem == TreeView_GetSelection(nmtv->hdr.hwndFrom)) {
							SetCapture(hDlg);
							pPs->pTree->BeginDrag(nmtv->itemNew.hItem);
						}
						TreeView_SelectItem(nmtv->hdr.hwndFrom, nmtv->itemNew.hItem);
						break;
					}

					case TVN_ITEMEXPANDED:
						pPs->pTree->AddFlags(PSTVF_STATE_CHANGED);
						break;

					case NM_KILLFOCUS:
						KillTimer(hDlg, TIMERID_RENAME);
						break;

					case NM_CLICK:
					{
						TVHITTESTINFO hti;

						GetCursorPos(&hti.pt);
						ScreenToClient(pPs->pTree->Window(), &hti.pt);
						TreeView_HitTest(pPs->pTree->Window(), &hti);
						if ((hti.flags & (TVHT_ONITEM|TVHT_ONITEMRIGHT)) && hti.hItem == TreeView_GetSelection(pPs->pTree->Window())) 
							SetTimer(hDlg, TIMERID_RENAME, 500, NULL);
						break;
					}

					case NM_RCLICK:
						pPs->pTree->PopupMenu();
						return 0;

				}
				break;
		}
		break;
	
	case WM_MOUSEMOVE:
		if (pPs->pTree->IsDragging()) {
			TVHITTESTINFO hti;

			hti.pt.x = (SHORT)LOWORD(lParam);
			hti.pt.y = (SHORT)HIWORD(lParam);
			MapWindowPoints(hDlg, pPs->pTree->Window(), &hti.pt, 1);
			TreeView_HitTest(pPs->pTree->Window(), &hti);

			if (hti.flags & (TVHT_ONITEM|TVHT_ONITEMRIGHT)) {
				RECT rc;
				BYTE height;

				// check where over the item, the pointer is
				if (TreeView_GetItemRect(pPs->pTree->Window(), hti.hItem, &rc, FALSE)) {
					height = (BYTE)(rc.bottom - rc.top);

					if (hti.pt.y - (height / 3) < rc.top) {
						SetCursor(LoadCursor(NULL, IDC_ARROW));
						TreeView_SetInsertMark(pPs->pTree->Window(), hti.hItem, 0);
					}
					else
					if (hti.pt.y + (height / 3) > rc.bottom) {
						SetCursor(LoadCursor(NULL, IDC_ARROW));
						TreeView_SetInsertMark(pPs->pTree->Window(), hti.hItem, 1);
					}
					else {
						TreeView_SetInsertMark(pPs->pTree->Window(), NULL, 0);
						SetCursor(LoadCursor(ghInst, MAKEINTRESOURCE(CURSOR_ADDGROUP)));
					}
				}
			}
			else {
				if (hti.flags & TVHT_ABOVE) SendMessage(pPs->pTree->Window(), WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), 0);
				if (hti.flags & TVHT_BELOW) SendMessage(pPs->pTree->Window(), WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0), 0);
				TreeView_SetInsertMark(pPs->pTree->Window(), NULL, 0);
			}
		}
		break;
	
	case WM_LBUTTONUP:

		// drop item
		if (pPs->pTree->IsDragging()) {
			TVHITTESTINFO hti;
			RECT rc;
			BYTE height;
			BOOLEAN bAsChild = FALSE;

			TreeView_SetInsertMark(pPs->pTree->Window(), NULL, 0);
			ReleaseCapture();
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			
			hti.pt.x = (SHORT)LOWORD(lParam);
			hti.pt.y = (SHORT)HIWORD(lParam);
			MapWindowPoints(hDlg, pPs->pTree->Window(), &hti.pt, 1);
			TreeView_HitTest(pPs->pTree->Window(), &hti);

			if (hti.hItem == pPs->pTree->DragItem()) {
				pPs->pTree->EndDrag();
				break;
			}

			if (hti.flags & TVHT_ABOVE) {
				hti.hItem = TVI_FIRST;
			}
			else
			if (hti.flags & (TVHT_NOWHERE|TVHT_BELOW)) {
				hti.hItem = TVI_LAST;
			}
			else
			if (hti.flags & (TVHT_ONITEM|TVHT_ONITEMRIGHT)) {
				// check where over the item, the pointer is
				if (!TreeView_GetItemRect(pPs->pTree->Window(), hti.hItem, &rc, FALSE)) {
					pPs->pTree->EndDrag();
					break;
				}
				height = (BYTE)(rc.bottom - rc.top);

				if (hti.pt.y - (height / 3) < rc.top) {
					HTREEITEM hItem = hti.hItem;

					if (!(hti.hItem = TreeView_GetPrevSibling(pPs->pTree->Window(), hItem))) {
						if (!(hti.hItem = TreeView_GetParent(pPs->pTree->Window(), hItem)))
							hti.hItem = TVI_FIRST;
						else
							bAsChild = TRUE;
					}
				}
				else 
				if (hti.pt.y + (height / 3) <= rc.bottom) {
					bAsChild = TRUE;
				}
			}	
			pPs->pTree->MoveItem(pPs->pTree->DragItem(), hti.hItem, bAsChild);
			pPs->pTree->EndDrag();

		}
		break; 
	
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case IDCANCEL:
			{	 
				pPs->pTree->OnCancel();
				DestroyWindow(hDlg);
				break;
			}

			/**
			 *	name:	IDOK / IDAPPLY
			 *	desc:	user clicked on apply or ok button in order to save changes
			 **/
			case IDOK:
			case IDAPPLY:
				if (pPs->dwFlags & PSF_CHANGED) {	 
					// kill focus from children to make sure all data can be saved (ComboboxEx)
					SetFocus(hDlg);

					pPs->dwFlags |= PSF_LOCKED;
					if (pPs->pTree->OnApply()) {
						pPs->dwFlags &= ~(PSF_LOCKED|PSF_CHANGED);
						break;
					}

					pPs->dwFlags &= ~PSF_CHANGED;
					EnableWindow(GetDlgItem(hDlg, IDAPPLY), FALSE);
					CallService(MS_CLIST_INVALIDATEDISPLAYNAME, (WPARAM)pPs->hContact, NULL);

					// need to upload owners settings
					if (!pPs->hContact && myGlobals.CanChangeDetails && DB::Setting::GetByte(SET_PROPSHEET_CHANGEMYDETAILS, FALSE)) {
						if (pPs->pUpload = new CPsUpload(pPs, LOWORD(wParam) == IDOK)) {
							if (pPs->pUpload->UploadFirst() == CPsUpload::UPLOAD_CONTINUE)
								break;
							MIR_DELETE(pPs->pUpload);
						}
					}
					pPs->dwFlags &= ~PSF_LOCKED;
				}
				if (LOWORD(wParam) == IDOK) 
					DestroyWindow(hDlg);
				break;

			case BTN_UPDATE:
				{
					if (pPs->hContact != NULL) 
					{
						ResetUpdateInfo(pPs);

						mir_snprintf(pPs->szUpdating, SIZEOF(pPs->szUpdating), 
							"%s (%s)", Translate("Updating"), pPs->pszProto);
						
						// need meta contact's subcontact information
						if (DB::Module::IsMetaAndScan(pPs->pszProto)) 
						{
							HANDLE hSubContact;
							CHAR szService[MAXSETTING];
							INT	i, numSubs;
							
							numSubs = DB::MetaContact::SubCount(pPs->hContact);
							
							// count valid subcontacts whose protocol supports the PSS_GETINFO service to update the information
							for (i = 0; i < numSubs; i++) 
							{
								hSubContact = DB::MetaContact::Sub(pPs->hContact, i);
								if (hSubContact != NULL) 
								{
									mir_snprintf(szService, SIZEOF(szService), "%s%s", 
										DB::Contact::Proto(hSubContact), PSS_GETINFO);
									
									if (ServiceExists(szService)) 
									{
										pPs->infosUpdated = (TAckInfo*)mir_realloc(pPs->infosUpdated, sizeof(TAckInfo) * (pPs->nSubContacts + 1));
										pPs->infosUpdated[pPs->nSubContacts].hContact = hSubContact;
										pPs->infosUpdated[pPs->nSubContacts].acks = NULL;
										pPs->infosUpdated[pPs->nSubContacts].count = 0;
										pPs->nSubContacts++;
									}
								}
							}

							if (pPs->nSubContacts != 0)
							{
								BOOLEAN bDo = FALSE;

								// call the services
								for (i = 0; i < pPs->nSubContacts; i++) 
								{
									if (!CallContactService(pPs->infosUpdated[pPs->nSubContacts].hContact, PSS_GETINFO, NULL, NULL))
									{
										bDo = TRUE;
									}
								}
								if (bDo)
								{
									EnableWindow(GetDlgItem(hDlg, BTN_UPDATE), FALSE);
									ShowWindow(GetDlgItem(hDlg, TXT_UPDATING), SW_SHOW);
									SetTimer(hDlg, TIMERID_UPDATING, 100, NULL);
								}
							}
						}
						else if (!CallContactService(pPs->hContact, PSS_GETINFO, NULL, NULL)) 
						{
							pPs->infosUpdated = (TAckInfo*)mir_calloc(sizeof(TAckInfo));
							pPs->infosUpdated[0].hContact = pPs->hContact;
							pPs->nSubContacts = 1;

							EnableWindow(GetDlgItem(hDlg, BTN_UPDATE), FALSE);
							ShowWindow(GetDlgItem(hDlg, TXT_UPDATING), SW_SHOW);
							SetTimer(hDlg, TIMERID_UPDATING, 100, NULL);
						}
					}
				}
				break;

			case BTN_IMPORT:
				svcExIm_ContactImport_Service((WPARAM) pPs->hContact, 0);
				break;

			case BTN_EXPORT:
				// save changes before exporting data
				DlgProc(hDlg, WM_COMMAND, MAKEWPARAM(IDAPPLY, BN_CLICKED), (LPARAM)GetDlgItem(hDlg, IDAPPLY));
				// do the exporting stuff
				svcExIm_ContactExport_Service((WPARAM) pPs->hContact, 0);
				break;
		}
		break;

	case WM_CLOSE:
		DlgProc(hDlg, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), (LPARAM)GetDlgItem(hDlg, IDCANCEL));
		break;

	case WM_DESTROY:
		{
			INT i = 0;

			// hide before destroy
			ShowWindow(hDlg, SW_HIDE);

			ResetUpdateInfo(pPs);

			// avoid any further message processing for this dialog page
			WindowList_Remove(ghWindowList, hDlg);
			SetUserData(hDlg, NULL);

			// unhook events and stop timers
			KillTimer(hDlg, TIMERID_RENAME);
			UnhookEvent(pPs->hProtoAckEvent);
			UnhookEvent(pPs->hSettingChanged);
			UnhookEvent(pPs->hIconsChanged);
			
			// save my window position
			Utils_SaveWindowPosition(hDlg, NULL, MODNAME, "DetailsDlg");

			// save current tree and destroy it
			if (pPs->pTree != NULL) {
				// save tree's current look
				pPs->pTree->SaveState();
				delete pPs->pTree;
				pPs->pTree = NULL;
			}

			DeleteObject(pPs->hCaptionFont);
			DeleteObject(pPs->hBoldFont);
			mir_free(pPs); pPs = NULL;
			MagneticWindows_RemoveWindow(hDlg);
		}
	}
	return FALSE;
}

