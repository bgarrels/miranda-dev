/*
Miranda Floating Contacts plugin, created by Iavor Vajarov ( iavor.vajarov@maverick-bg.com )
http://miranda-icq.sourceforge.net/

Miranda fonts and colors settings by Ranger.
Extended fonts and backgrounds settings by Oleksiy Shurubura


This file is placed in the public domain. Anybody is free to use or
modify it as they wish with no restriction.

This plugin has been tested to work under Windows ME, 2000 and XP.

No warranty for any misbehaviour.
*/

#include "stdhdr.h"
#include "shlwapi.h"

#include "resource.h"
#include "fltcont.h"

#pragma warning ( disable : 4100 ) // Unreferenced formal parameter
#pragma comment ( lib, "comctl32.lib" )
#pragma comment ( lib, "shlwapi.lib" )

#pragma warning ( default : 4201 )
#define ME_SYSTEM_MODULESLOADED		"Miranda/System/ModulesLoaded"

#define WND_CLASS					"MirandaThumbsWnd"
#define	USERNAME_LEN				50
#define	WM_REFRESH_CONTACT			WM_USER + 0x100

#ifndef WS_EX_LAYERED
	#define WS_EX_LAYERED           0x00080000
#endif

#ifndef LWA_ALPHA
	#define LWA_ALPHA               0x00000002
#endif

#if WINVER < 0x0500
	#define SM_XVIRTUALSCREEN       76
	#define SM_YVIRTUALSCREEN       77
	#define SM_CXVIRTUALSCREEN      78
	#define SM_CYVIRTUALSCREEN      79
#endif

#define MIID_FLTCONTACTSPLUGIN    { 0xa90bc742, 0x84f0, 0x451a, { 0x8b, 0x99, 0x1e, 0x8, 0x8e, 0x51, 0x82, 0x33 } }

char szModuleDB[]	= "Floating contacts";


typedef struct _CDropTarget 
{
	IDropTargetVtbl *lpVtbl;
	unsigned		refCount;
} CDropTarget;


typedef struct _DockOpt
{
	HWND	hwndLeft;
	HWND	hwndRight;
}
DockOpt;

	
typedef struct _ThumbInfo
{
	HWND		hwnd;
	char		szName[ USERNAME_LEN ];
	HANDLE		hContact;
	int			iIcon;
	CDropTarget dropTarget;
	DockOpt		dockOpt;
	
	struct _ThumbInfo	*pNext;
} 
ThumbInfo;


#include "filedrop.h"


typedef struct _ContactDB
{
	WORD	nXPos;
	WORD	nYPos;
} ContactDB;


BOOL (WINAPI *pSetLayeredWindowAttributes)(HWND,COLORREF,BYTE,DWORD);


// Forward function declarations
static ThumbInfo* AddThumb			( HWND hwnd, char *szName, HANDLE hContact );
static void	RemoveThumb				( ThumbInfo *pThumb );
static ThumbInfo* FindThumb			( HWND hwnd );
static ThumbInfo* FindThumbByContact( HANDLE hContact );

static void	SendMsgDialog			( HWND hwnd, char *pText );
static void	PopUpMessageDialog		( HWND hwnd );
static void	RepaintWindow			( HWND hwnd );	
static void	PositionThumb			( ThumbInfo *pThumb, short nX, short nY ); 
static void	ResizeThumb				( ThumbInfo *pThumb );
static void	RefreshContactIcon		( HANDLE hContact, int iIcon );
static void	RefreshContactStatus	( ThumbInfo *pThumb, int idStatus );
static void	LoadContacts			( void );
static void	SaveContactsPos			( void );
static void	DeleteContactPos		( ThumbInfo *pThumb );
static void	LoadMenus				();
static void	CreateThumbWnd			( char *szName, HANDLE hContact, int nX, int nY );
static void	RegisterWindowClass		( void );
static void	UnregisterWindowClass	( void );
static void	CleanUp					( void );
static void	OnLButtonDown			( ThumbInfo *pThumb, short nX, short nY );
static void	OnLButtonUp				( ThumbInfo *pThumb );
static void	OnMouseMove				( ThumbInfo *pThumb, short nX, short nY );
static BOOL GetOSPlatform			( void );
static void ThumbSelect				( ThumbInfo *pThumb, BOOL bMouse );
static void ThumbDeselect			( ThumbInfo *pThumb, BOOL bMouse );
static void LoadContact				( HANDLE hContact );
static void LoadDBSettings			( void );
static void DockThumbs				( ThumbInfo *pThumbLeft, ThumbInfo *pThumbRight, BOOL bMoveLeft );
static void CreateThumbsFont		( void );
static void CreateBackgroundBrush	( void );
static int	GetContactStatus		( HANDLE hContact );
static void	SnapToScreen			( RECT rcThumb, int nX, int nY, int* pX, int* pY );
static void	GetScreenRect			( void );
extern void SetThumbsOpacity		( BYTE btAlpha );
static void SetThumbOpacity			( ThumbInfo *pThumb, BYTE btAlpha );
static int	ClcStatusToPf2			( int status );
static BOOL IsStatusVisible			( int status );
static void RegHotkey				( char* szName, HWND hwnd );
static BOOL HideOnFullScreen		();

static LRESULT __stdcall CommWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

static int OnContactDeleted			( WPARAM wParam,LPARAM lParam );
static int OnContactIconChanged		( WPARAM wParam, LPARAM lParam );
static int OnContactDrag			( WPARAM wParam, LPARAM lParam );
static int OnContactDrop			( WPARAM wParam, LPARAM lParam );
static int OnContactDragStop		( WPARAM wParam, LPARAM lParam );
static int OnSkinIconsChanged		( WPARAM wParam, LPARAM lParam );
static int OnContactSettingChanged	( WPARAM wParam, LPARAM lParam );
static int OnStatusModeChange		( WPARAM wParam, LPARAM lParam );
static int OnModulesLoded			( WPARAM wParam, LPARAM lParam );
static int OnPrebuildContactMenu	( WPARAM wParam, LPARAM lParam );

static int OnContactMenu_Remove		( WPARAM wParam,LPARAM lParam );
//static int OnContactMenu_HideAll	( WPARAM wParam,LPARAM lParam );
static int OnMainMenu_HideAll		( WPARAM wParam,LPARAM lParam );

HINSTANCE	hInst				= NULL;
HMODULE		hUserDll			= NULL;
PLUGINLINK	*pluginLink			= NULL;
HFONT		hFont[FLT_FONTIDS]	= {NULL};
COLORREF	tColor[FLT_FONTIDS]	= {0};
HIMAGELIST	himl				= NULL;
HANDLE		hevContactIcon		= NULL;
HANDLE		hevContactDrop		= NULL;
HANDLE		hevContactDragStop	= NULL;
HANDLE		hevSkinIcons		= NULL;
HANDLE		hevContactDrag		= NULL;
HANDLE		hevContactSetting	= NULL;
HANDLE		hevContactDeleted	= NULL;
HANDLE		hevOptionsInit		= NULL;
HANDLE		hevStatusMode		= NULL;
HANDLE		hevModules			= NULL;
HANDLE		hevPrebuildMenu		= NULL;
HANDLE		hNewContact			= NULL;
HPEN		hLTEdgesPen			= NULL;
HPEN		hRBEdgesPen			= NULL;
HBRUSH		hBkBrush			= NULL;
HBITMAP		hBmpBackground		= NULL;
WORD		nBackgroundBmpUse	= CLB_STRETCH;
HWND		hwndMiranda			= NULL;
BOOL		bNT					= FALSE;
BOOL		bHideOffline		= FALSE;
BOOL		bHideAll			= FALSE;
BOOL		bHideWhenFullscreen	= FALSE;
BOOL		bMoveTogether		= FALSE;
BOOL		bDockHorz			= TRUE;
BOOL		bFixedWidth			= FALSE;
int			nThumbWidth			= 0;
UINT		nStatus				= 0;
HMENU		hContactMenu		= NULL;
HANDLE		hMenuItemRemove		= NULL;
HANDLE		hMenuItemHideAll	= NULL;
HANDLE		hMainMenuItemHideAll	= NULL;
RECT		rcScreen;
DWORD		dwOfflineModes		= 0;

// Window movement consts
POINT	ptOld;
BOOL	bMouseDown		= FALSE;
BOOL	bMouseIn		= FALSE;
BOOL	bMouseMoved		= FALSE;
short	nLeft			= 0;
short	nTop			= 0;
BYTE	thumbAlpha		= 255;
int		nOffs			= 5;

ThumbInfo *pThumbsList	= NULL;	// Singly linked list of thumbs




PLUGININFOEX pluginInfo ={	sizeof( PLUGININFOEX ),
							"Floating contacts",
							PLUGIN_MAKE_VERSION( 1,0,0,10 ),
							"Floating contacts",
							"Iavor Vajarov",
							"ivajarov@maverick-bg.com",
							"© 2002 - 2010 I. Vajarov",
							"",
							0,		//not transient
							0,		//doesn't replace anything built-in,
							// {A90BC742-84F0-451a-8B99-1E088E518233}
							MIID_FLTCONTACTSPLUGIN
						};



///////////////////////////////////////////////////////
// Thumbs info handling


// Add a thumb to the the list
static ThumbInfo* AddThumb( HWND hwnd, char *szName, HANDLE hContact )
{
	ThumbInfo *pThumb	= NULL;

	if ( szName == NULL )	return( NULL );
	if ( hContact == NULL ) return( NULL );
	if ( hwnd == NULL )		return( NULL );

	pThumb = (ThumbInfo*) malloc( sizeof( ThumbInfo ) );
	 
	if ( pThumb != NULL )
	{
		strncpy( pThumb->szName, szName, USERNAME_LEN - 1 );
		pThumb->hContact	= hContact;
		pThumb->hwnd		= hwnd;
		
		pThumb->dockOpt.hwndLeft	= NULL;
		pThumb->dockOpt.hwndRight	= NULL;

		RegHotkey( szName, hwnd );
	}
	
	pThumb->pNext	= pThumbsList;
	pThumbsList		= pThumb;

	return( pThumb );
}


static void RemoveThumb( ThumbInfo *pThumb )
{
	ThumbInfo *pPrev = NULL;

	if ( pThumb == NULL ) return;
		
	if ( pThumbsList == pThumb )
	{
		// First entry in the list
		pThumbsList = pThumb->pNext;
		
		UnregisterFileDropping( pThumb->hwnd );
		DestroyWindow( pThumb->hwnd );
		free( pThumb );
	}
	else
	{
		pPrev = pThumbsList;

		while( ( pPrev != NULL ) && ( pPrev->pNext != pThumb ) )
		{
			pPrev = pPrev->pNext;
		}

		if ( pPrev != NULL )
		{
			pPrev->pNext = pThumb->pNext;
			
			UnregisterFileDropping( pThumb->hwnd );
			DestroyWindow( pThumb->hwnd );
			free( pThumb );
		}
	}
}



static ThumbInfo* FindThumb( HWND hwnd )
{
	ThumbInfo	*pThumb = pThumbsList;
	BOOL		bFound	= FALSE;

	if ( hwnd == NULL ) return( NULL );

	while( pThumb != NULL && !bFound )
	{
		bFound = ( hwnd == pThumb->hwnd );

		if ( !bFound )
		{
			pThumb = pThumb->pNext;
		}
	}

	return( pThumb );
}



static ThumbInfo* FindThumbByContact( HANDLE hContact )
{
	ThumbInfo	*pThumb = pThumbsList;
	BOOL		bFound	= FALSE;

	if ( hContact == NULL ) { return NULL; }

	while( pThumb != NULL && !bFound )
	{
		bFound = ( hContact == pThumb->hContact );

		if ( !bFound )
		{
			pThumb = pThumb->pNext;
		}
	}

	return( pThumb );
}



///////////////////////////////////////////////////////
// Load / unload
BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
	hInst = hinstDLL;
	
	switch ( fdwReason )
	{
		case DLL_PROCESS_ATTACH:	
			break;

		case DLL_PROCESS_DETACH:
			break;
	}
		
	return( TRUE );
}



__declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx( DWORD mirandaVersion )
{
	return &pluginInfo;
}


static const MUUID interfaces[] = {MIID_FLTCONTACTSPLUGIN, MIID_LAST};
__declspec(dllexport) const MUUID* MirandaPluginInterfaces(void)
{
	return interfaces;
}


int __declspec(dllexport) Load( PLUGINLINK *link )
{
	SetLastError( 0 );
	pluginLink	= link;

	hevModules		= HookEvent( ME_SYSTEM_MODULESLOADED,  OnModulesLoded );
	bNT				= GetOSPlatform();

	hUserDll = LoadLibrary("user32.dll");
	pSetLayeredWindowAttributes =
		(hUserDll != NULL) 
			? (BOOL (WINAPI *)(HWND,COLORREF,BYTE,DWORD))GetProcAddress(hUserDll, "SetLayeredWindowAttributes")
			: NULL;
	return 0;
}



int __declspec(dllexport) Unload()
{
	CleanUp();
	return 0;
}



static void CleanUp()
{
	int nFontId;

	UnhookEvent( hevContactIcon );
	UnhookEvent( hevContactDrag );
	UnhookEvent( hevContactDrop );
	UnhookEvent( hevContactDragStop );
	UnhookEvent( hevSkinIcons );
	UnhookEvent( hevContactDeleted );
	UnhookEvent( hevContactSetting );
	UnhookEvent( hevOptionsInit );
	UnhookEvent( hevStatusMode );
	UnhookEvent( hevModules );
	UnhookEvent( hevPrebuildMenu );

	while( pThumbsList != NULL )
		RemoveThumb( pThumbsList );

	if (NULL != hLTEdgesPen)
		DeleteObject(hLTEdgesPen);
	if (NULL != hRBEdgesPen)
		DeleteObject(hRBEdgesPen);
	if (NULL != hBmpBackground)
		DeleteObject(hBmpBackground);
	if (NULL != hBkBrush)
		DeleteObject(hBkBrush);

	for (nFontId = 0; nFontId < FLT_FONTIDS; nFontId++)
		if (NULL != hFont[nFontId])
			DeleteObject(hFont[nFontId]);

	UnregisterWindowClass();

	FreeFileDropping();

	if ( hUserDll != NULL )
	{
		FreeLibrary( hUserDll );
	}
}



///////////////////////////////////////////////////////
// Hooked events
static int OnModulesLoded( WPARAM wParam, LPARAM lParam )
{
	hevOptionsInit		= HookEvent( ME_OPT_INITIALISE, OnOptionsInitialize );
	hevContactIcon		= HookEvent( ME_CLIST_CONTACTICONCHANGED,  OnContactIconChanged );
	hevSkinIcons		= HookEvent( ME_SKIN_ICONSCHANGED,  OnSkinIconsChanged );
	hevContactDrag		= HookEvent( ME_CLUI_CONTACTDRAGGING,  OnContactDrag );
	hevContactDrop		= HookEvent( ME_CLUI_CONTACTDROPPED,  OnContactDrop );
	hevContactDragStop	= HookEvent( ME_CLUI_CONTACTDRAGSTOP,  OnContactDragStop );
	hevContactSetting	= HookEvent( ME_DB_CONTACT_SETTINGCHANGED, OnContactSettingChanged );
	hevContactDeleted	= HookEvent( ME_DB_CONTACT_DELETED, OnContactDeleted );
	hevStatusMode		= HookEvent( ME_CLIST_STATUSMODECHANGE, OnStatusModeChange );
	hevPrebuildMenu		= HookEvent( ME_CLIST_PREBUILDCONTACTMENU, OnPrebuildContactMenu );
	hwndMiranda			= (HWND)CallService( MS_CLUI_GETHWND, 0, 0 );

	
	
	// No thumbs yet
	pThumbsList	= NULL;
	
	RegisterWindowClass();
	InitFileDropping();
	GetScreenRect();
	LoadDBSettings();
	CreateBackgroundBrush();
	CreateThumbsFont();
	LoadContacts();
	LoadMenus();

	return( 0 );
}


static int OnContactDeleted( WPARAM wParam, LPARAM lParam )
{
	HANDLE		hContact	= ( HANDLE )wParam;
	ThumbInfo	*pThumb		= FindThumbByContact( hContact );

	DeleteContactPos( pThumb );
	RemoveThumb( pThumb );
	
	return 0;
}


static int OnContactIconChanged( WPARAM wParam, LPARAM lParam )
{
	HANDLE		hContact	= ( HANDLE )wParam;
	ThumbInfo	*pThumb		= FindThumbByContact( hContact );

	if ( pThumb != NULL )
	{
		RefreshContactIcon( hContact, ( int )lParam );
		ThumbSelect( pThumb, FALSE );
		SetTimer( pThumb->hwnd, 1, 1000, NULL ); 
	}

	return 0;
}


static int OnContactDrag( WPARAM wParam, LPARAM lParam )
{
	ThumbInfo	*pNewThumb	= NULL;
	HANDLE		hContact	= ( HANDLE )wParam;
	ThumbInfo	*pThumb		= FindThumbByContact( hContact );
	char		*pName		= (char*)CallService( MS_CLIST_GETCONTACTDISPLAYNAME, wParam, (LPARAM)0 );
	int			idStatus	= ID_STATUS_OFFLINE;
	
	if ( pThumb == NULL )
	{
		idStatus = GetContactStatus( hContact );
		
		if ( !bHideAll && !HideOnFullScreen() && ( !bHideOffline || IsStatusVisible( idStatus ) ) )
		{
			CreateThumbWnd( pName, hContact, LOWORD( lParam ), HIWORD( lParam ) );
			pNewThumb = FindThumbByContact( hContact );
			ShowWindow( pNewThumb->hwnd, SW_SHOWNA );
			hNewContact = hContact;

			OnStatusChanged();
		}
	}
	else if ( hContact == hNewContact )
	{
		PositionThumb( pThumb, (short)( LOWORD( lParam ) - 5 ), (short)( HIWORD( lParam ) - 5 ) );
	}
	
	return( hNewContact != NULL ? 1 : 0 );
}



static int OnContactDrop( WPARAM wParam, LPARAM lParam )
{
	RECT	rcMiranda;
	RECT	rcThumb;
	RECT	rcOverlap;

	HANDLE	hContact	= ( HANDLE )wParam;
	ThumbInfo *pThumb	= FindThumbByContact( hContact );

	if ( ( hNewContact == hContact ) && ( pThumb != NULL ) )
	{
		hNewContact = NULL;
		
		GetWindowRect( hwndMiranda, &rcMiranda );
		GetWindowRect( pThumb->hwnd, &rcThumb );
		
		if ( IntersectRect( &rcOverlap, &rcThumb, &rcMiranda ) )
		{
			OnLButtonDown( pThumb, (short)(rcThumb.left + 5), (short)(rcThumb.top + 5) );
		}
	}

	SaveContactsPos();

	return( 1 );
}


static int OnContactDragStop( WPARAM wParam, LPARAM lParam )
{
	HANDLE	hContact = ( HANDLE )wParam;
	ThumbInfo *pThumb = FindThumbByContact( hContact );

	if ( ( pThumb != NULL ) && ( hNewContact == hContact ) )
	{
		RemoveThumb( pThumb );
		hNewContact = NULL;
	}

	return( 0 );
}


static int OnSkinIconsChanged( WPARAM wParam, LPARAM lParam )
{
	ThumbInfo	*pThumb		= pThumbsList;
	
	// Get handle to the image list
	himl = ( HIMAGELIST )CallService( MS_CLIST_GETICONSIMAGELIST, 0, 0 );
	
	// Update thumbs
	while( pThumb != NULL )
	{
		RepaintWindow( pThumb->hwnd );
		pThumb = pThumb->pNext;
	}

	return( 0 );
}



static int OnContactSettingChanged( WPARAM wParam, LPARAM lParam )
{
	HANDLE		hContact	= ( HANDLE )wParam;
	ThumbInfo	*pThumb		= FindThumbByContact( hContact );
	int			idStatus	= ID_STATUS_OFFLINE;
	BOOL		bRefresh	= TRUE;
	DBCONTACTWRITESETTING* pdbcws = ( DBCONTACTWRITESETTING* )lParam;
	

	if ( hContact == NULL )
	{
		return( 0 );
	}

	if ( pThumb == NULL ) return( 0 );

	// Only on these 2 events we need to refresh
	if( 0 == _stricmp( pdbcws->szSetting, "Status" ) ) 
	{
		idStatus = pdbcws->value.wVal;
	}
	else if ( ( 0 == _stricmp( pdbcws->szSetting, "Nick" ) ) || ( 0 == _stricmp( pdbcws->szSetting, "MyHandle" ) ) )
	{
		idStatus = GetContactStatus( hContact );
	}
	else
	{
		bRefresh = FALSE;
	}

	if ( bRefresh )
	{
		// Detach call
		PostMessage( pThumb->hwnd, WM_REFRESH_CONTACT, 0, idStatus );
	}

	return( 0 );
}


static int OnStatusModeChange( WPARAM wParam, LPARAM lParam )
{
	ThumbInfo	*pThumb = pThumbsList;
	int			idStatus;
	
	nStatus = (int)wParam;
	
	while( pThumb != NULL )
	{
		idStatus = GetContactStatus( pThumb->hContact );
		RefreshContactStatus( pThumb, idStatus );

		pThumb = pThumb->pNext;
	}
	if ( wParam == ID_STATUS_OFFLINE )
	{
		// Floating status window will use this
	}

	return( 0 );
}



static int OnPrebuildContactMenu( WPARAM wParam, LPARAM lParam )
{
	ThumbInfo *pThumb = FindThumbByContact( (HANDLE) wParam );
	CLISTMENUITEM clmi;

	ZeroMemory( &clmi, sizeof( clmi ) );
	clmi.cbSize = sizeof( clmi );

	clmi.flags = ( pThumb == NULL ) ? CMIM_FLAGS | CMIF_HIDDEN : CMIM_FLAGS &~CMIF_HIDDEN;
	CallService( MS_CLIST_MODIFYMENUITEM, (WPARAM)hMenuItemRemove, (LPARAM)&clmi );

	clmi.flags = bHideAll ? CMIM_FLAGS | CMIF_HIDDEN : CMIM_FLAGS &~CMIF_HIDDEN;
	CallService( MS_CLIST_MODIFYMENUITEM, (WPARAM)hMenuItemHideAll, (LPARAM)&clmi );

	return( 0 );
}




///////////////////////////////////////////////////////
// General functions

static void LoadDBSettings()
{
	thumbAlpha			= (BYTE)((double)DBGetContactSettingByte( NULL, szModuleDB, "Opacity", 100 ) * 2.55 );
	bHideOffline		= (BOOL)DBGetContactSettingByte( NULL, szModuleDB, "HideOffline", 0 );
	bHideAll			= (BOOL)DBGetContactSettingByte( NULL, szModuleDB, "HideAll", 0 );
	bHideWhenFullscreen	= (BOOL)DBGetContactSettingByte( NULL, szModuleDB, "HideWhenFullscreen", 0 );
	bMoveTogether		= (BOOL)DBGetContactSettingByte( NULL, szModuleDB, "MoveTogether", 0 );
	bFixedWidth			= (BOOL)DBGetContactSettingByte( NULL, szModuleDB, "FixedWidth", 0 );
	nThumbWidth			= (DWORD)DBGetContactSettingDword( NULL, szModuleDB, "Width", 0 );
	dwOfflineModes		= DBGetContactSettingDword( NULL, "CLC","OfflineModes", MODEF_OFFLINE );
}



static void PopUpMessageDialog( HWND hwnd )
{
	ThumbInfo *pThumb = FindThumb( hwnd );

	if ( pThumb != NULL )
	{
		CallService( MS_CLIST_CONTACTDOUBLECLICKED, (WPARAM)pThumb->hContact, (LPARAM)0 );
	}
}



static void SendMsgDialog( HWND hwnd, char *pText )
{
	ThumbInfo *pThumb = FindThumb( hwnd );

	if ( pThumb != NULL )
	{
		CallService( MS_MSG_SENDMESSAGE, (WPARAM)pThumb->hContact, (LPARAM)pText );
	}
}


static void ShowContactMenu( HWND hwnd, POINT pt )
{
	ThumbInfo	*pThumb		= FindThumb( hwnd );
	int			idCommand	= 0;

	if ( pThumb != NULL )
	{
		hContactMenu = (HMENU)CallService( MS_CLIST_MENUBUILDCONTACT, (WPARAM)pThumb->hContact, (LPARAM)0 );
		
		if ( hContactMenu == NULL ) return;
		
		idCommand = TrackPopupMenu( hContactMenu, TPM_RIGHTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, pt.x, pt.y, 0 , hwnd, NULL );
		CallService( MS_CLIST_MENUPROCESSCOMMAND, MAKEWPARAM( idCommand ,MPCF_CONTACTMENU), (LPARAM)pThumb->hContact );
	}
}

static LRESULT __stdcall CommWndProc(	HWND	hwnd, 
										UINT	uMsg, 
										WPARAM	wParam, 
										LPARAM  lParam	)
{
	LRESULT		lResult		= 0;
	ThumbInfo	*pThumb		= FindThumb( hwnd );

	switch( uMsg )
	{
	case WM_RBUTTONUP:
		{
			POINT pt;
			pt.x = LOWORD( lParam ); 
			pt.y = HIWORD( lParam ); 

			ThumbDeselect( pThumb, TRUE );

			ClientToScreen(hwnd,&pt);
			ShowContactMenu( hwnd, pt );
		}

		break;

	case WM_PAINT:
		RepaintWindow( hwnd );
		ValidateRect( hwnd, NULL );
		break;

	case WM_ACTIVATEAPP:
		if ( NULL != pThumb )
		{
			SetWindowPos( pThumb->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
		}
		break;


	case WM_MEASUREITEM:
		lResult = CallService( MS_CLIST_MENUMEASUREITEM,wParam,lParam );
		break;

	case WM_DRAWITEM:
		lResult = CallService( MS_CLIST_MENUDRAWITEM,wParam,lParam );
		break;

	case WM_LBUTTONDOWN:
		OnLButtonDown( pThumb, (short)LOWORD( lParam ), (short)HIWORD( lParam ) );
		break;

	case WM_MOUSEMOVE:
		OnMouseMove( pThumb, (short)LOWORD( lParam ), (short)HIWORD( lParam ) );
		break;

	case WM_LBUTTONUP:
		OnLButtonUp( pThumb );
		if (bMouseMoved || !DBGetContactSettingByte(NULL, "CList", "Tray1Click", SETTING_TRAY1CLICK_DEFAULT))
			break;
		// FALL THRU

	case WM_LBUTTONDBLCLK:
		// Popup message dialog
		ThumbDeselect( pThumb, TRUE );
		PopUpMessageDialog( hwnd );
		break;

	case WM_REFRESH_CONTACT:
		{
			strncpy( pThumb->szName, (char*)CallService( MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)pThumb->hContact, (LPARAM)0 ), USERNAME_LEN - 1 );
			RefreshContactStatus( pThumb, (int)lParam );
			ResizeThumb( pThumb );
		}
		break;

	case WM_TIMER:
		KillTimer( pThumb->hwnd, 1 );
		ThumbDeselect( pThumb, FALSE );
		break;

	case WM_HOTKEY:
		{
			ThumbInfo *pThumb = FindThumb( ( HWND )wParam );

			if ( NULL != pThumb )
			{
				PopUpMessageDialog( ( HWND )wParam );
			}
		}

	default:
		break;
	}

	lResult = DefWindowProc( hwnd, uMsg, wParam, lParam );

	return( lResult );
}



static void OnLButtonDown( ThumbInfo *pThumb, short nX, short nY )
{
	RECT rc;

	if ( pThumb == NULL ) return;

	ptOld.x = nX;
	ptOld.y = nY;
	
	ClientToScreen( pThumb->hwnd, &ptOld );
	GetWindowRect( pThumb->hwnd, &rc );
	
	nLeft	= (short)rc.left;
	nTop	= (short)rc.top;
	
	bMouseIn	= FALSE;
	bMouseDown	= TRUE;
	bMouseMoved	= FALSE;

	SetCapture( pThumb->hwnd );
}


static void OnLButtonUp( ThumbInfo *pThumb )
{
	RECT	rcMiranda;
	RECT	rcThumb;
	RECT	rcOverlap;
	
	if ( pThumb == NULL ) return;

	ThumbDeselect( pThumb, TRUE );

	if ( bMouseDown )
	{
		bMouseDown = FALSE;
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	
		// Check whether we shoud remove the window
		GetWindowRect( hwndMiranda, &rcMiranda );
		GetWindowRect( pThumb->hwnd, &rcThumb );

		if ( IntersectRect( &rcOverlap, &rcMiranda, &rcThumb ) )
		{
			if( IsWindowVisible( hwndMiranda ) )
			{
				DeleteContactPos( pThumb );

				RemoveThumb( pThumb );
			}
		}
	}

	SaveContactsPos();
}



static void OnMouseMove( ThumbInfo *pThumb, short nX, short nY )
{
	int		dX;
	int		dY;
	POINT	ptNew;

	// Position thumb
	if ( pThumb == NULL ) return;
	
	if( bMouseDown )
	{
		bMouseMoved	= TRUE;

		ptNew.x = nX;
		ptNew.y = nY;

		ClientToScreen( pThumb->hwnd, &ptNew );				
		
		dX = ptNew.x - ptOld.x;
		dY = ptNew.y - ptOld.y;
		
		nLeft	+= (short)dX;
		nTop	+= (short)dY;
		
		PositionThumb( pThumb, nLeft, nTop );

		ptOld = ptNew;
	}
	else
	{
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	}

	// Update selection status
	if ( !bMouseIn )
	{
		ThumbSelect( pThumb, TRUE );
	}
	else
	{
		POINT pt;
		RECT rc;
				
		pt.x	= nX;
		pt.y	= nY;

		GetClientRect( pThumb->hwnd, &rc );
		
		if ( !PtInRect( &rc, pt ) && !bMouseDown )
		{
			ThumbDeselect( pThumb, TRUE );
		}
	}
}

extern void SetThumbsOpacity( BYTE btAlpha )
{
	ThumbInfo *pThumb = pThumbsList;

	while( pThumb != NULL )
	{
		SetThumbOpacity( pThumb, btAlpha );
		
		pThumb = pThumb->pNext;
	}
}



static void SetThumbOpacity( ThumbInfo *pThumb, BYTE btAlpha )
{
	if ( ( NULL != pSetLayeredWindowAttributes ) && ( pThumb != NULL ) )
	{
		LONG prevStyle = GetWindowLong( pThumb->hwnd, GWL_EXSTYLE );

		if ( btAlpha == 255 )
		{
			SetWindowLong( pThumb->hwnd, GWL_EXSTYLE, prevStyle & ~WS_EX_LAYERED );
		}
		else
		{
			SetWindowLong( pThumb->hwnd, GWL_EXSTYLE, prevStyle | WS_EX_LAYERED );
			pSetLayeredWindowAttributes( pThumb->hwnd, 0, btAlpha, LWA_ALPHA );
		}
	}
}


static void RepaintWindow( HWND hwnd )
{
	HFONT	hOldFont;
	SIZE	size;
	RECT	rc;
	RECT	rcText;
	DWORD	oldColor;
	int		oldBkMode, index = 0;// nStatus;
	
	HDC			hdc		= GetWindowDC( hwnd );
	ThumbInfo	*pThumb	= FindThumb( hwnd );
	HDC			hdcDraw	= CreateCompatibleDC( hdc );
	HBITMAP		hbmDraw = NULL;
	HBITMAP		hbmOld	= NULL;

	GetClientRect( hwnd, &rc );
	hbmDraw = CreateCompatibleBitmap( hdc, rc.right - rc.left, rc.bottom - rc.top );
	hbmOld	= (HBITMAP)SelectObject( hdcDraw, hbmDraw );
    
	if ( hdc == NULL )		return;
	if ( pThumb == NULL )	return;
	

	if ( NULL != hBmpBackground )
	{
		RECT rcBkgnd;
		BITMAP bmp;
		HDC hdcBmp;
		HBITMAP hbmTmp;
		int x,y;
		int maxx,maxy;
		int destw,desth;
		int width;
		int height;

		rcBkgnd = rc;
		if (NULL != hLTEdgesPen)
			InflateRect(&rcBkgnd, -1, -1);
		width = rcBkgnd.right - rcBkgnd.left;
		height = rcBkgnd.bottom - rcBkgnd.top;

		GetObject(hBmpBackground, sizeof(bmp), &bmp);
		hdcBmp	= CreateCompatibleDC( hdcDraw );
		hbmTmp = (HBITMAP)SelectObject( hdcBmp, hBmpBackground );

		maxx = (0 != (nBackgroundBmpUse & CLBF_TILEH) ? rcBkgnd.right : rcBkgnd.left + 1);
		maxy = (0 != (nBackgroundBmpUse & CLBF_TILEV) ? rcBkgnd.bottom : rcBkgnd.top + 1);
		switch (nBackgroundBmpUse & CLBM_TYPE)
		{
		case CLB_STRETCH:
			if (0 != (nBackgroundBmpUse & CLBF_PROPORTIONAL))
			{
				if (width * bmp.bmHeight < height * bmp.bmWidth)
				{
					desth = height;
					destw = desth * bmp.bmWidth / bmp.bmHeight;
				}
				else
				{
					destw = width;
					desth = destw * bmp.bmHeight / bmp.bmWidth;
				}
			}
			else
			{
				destw = width;
				desth = height;
			}
			break;

		case CLB_STRETCHH:
			destw = width;
			if (0 != (nBackgroundBmpUse & CLBF_PROPORTIONAL))
				desth = destw * bmp.bmHeight / bmp.bmWidth;
			else
				desth = bmp.bmHeight;
			break;

		case CLB_STRETCHV:
			desth = height;
			if (0 != (nBackgroundBmpUse & CLBF_PROPORTIONAL))
				destw = desth * bmp.bmWidth / bmp.bmHeight;
			else
				destw = bmp.bmWidth;
			break;

		default:    //clb_topleft
			destw = bmp.bmWidth;
			desth = bmp.bmHeight;
			break;
		}
		SetStretchBltMode(hdc, STRETCH_HALFTONE);

		for (x = rcBkgnd.left; x < maxx; x += destw)
		{
			for (y = rcBkgnd.top; y < maxy; y += desth)
			{
				StretchBlt( hdcDraw, x, y, destw, desth, hdcBmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY );
			}
		}

		SelectObject( hdc, hbmTmp );
		DeleteDC( hdcBmp );
	}
	else
	{
		FillRect( hdcDraw, &rc, hBkBrush );
	}

	ImageList_GetIconSize( himl, &(int)size.cx, &(int)size.cy );

	oldBkMode	= SetBkMode( hdcDraw, TRANSPARENT );

	if (!DBGetContactSettingByte(pThumb->hContact, "CList", "NotOnList", 0))
	{
		int nStatus;
		int nContactStatus;
		int nApparentMode;
		char* szProto;

		szProto = (char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)pThumb->hContact, 0);
		
		if ( NULL != szProto )
		{
			nStatus			= CallProtoService(szProto, PS_GETSTATUS, 0, 0);
			nContactStatus	= DBGetContactSettingWord(pThumb->hContact, szProto, "Status", ID_STATUS_OFFLINE);
			nApparentMode	= DBGetContactSettingWord(pThumb->hContact, szProto, "ApparentMode", 0);
			
			if (	(nStatus == ID_STATUS_INVISIBLE && nApparentMode == ID_STATUS_ONLINE) ||
					(nStatus != ID_STATUS_INVISIBLE && nApparentMode == ID_STATUS_OFFLINE) )
			{
				if (ID_STATUS_OFFLINE == nContactStatus)
				{
					index = FLT_FONTID_OFFINVIS;
				}
				else
				{
					index =	FLT_FONTID_INVIS;
				}
			}
			else if (ID_STATUS_OFFLINE == nContactStatus)
			{
				index = FLT_FONTID_OFFLINE;
			}
			else
			{
				index = FLT_FONTID_CONTACTS;
			}
		}
	}
	else
	{
		index = FLT_FONTID_NOTONLIST;
	}

	oldColor = SetTextColor( hdcDraw, tColor[ index ] );
	
	if (NULL != hLTEdgesPen)
	{
		HPEN  hOldPen = SelectObject( hdcDraw, hLTEdgesPen );

		MoveToEx(hdcDraw, 0, 0, NULL);
		LineTo(hdcDraw, rc.right, 0);
		MoveToEx(hdcDraw, 0, 0, NULL);
		LineTo(hdcDraw, 0, rc.bottom);

		SelectObject(hdcDraw, hRBEdgesPen);

		MoveToEx(hdcDraw, 0, rc.bottom - 1, NULL);
		LineTo(hdcDraw, rc.right - 1, rc.bottom - 1);
		MoveToEx(hdcDraw, rc.right - 1, rc.bottom - 1, NULL);
		LineTo(hdcDraw, rc.right - 1, 0);

		SelectObject(hdcDraw, hOldPen);
		//InflateRect(&rc, -1, -1);
	}

	ImageList_DrawEx(	himl, 
						pThumb->iIcon, 
						hdcDraw, 
						2, 
						( rc.bottom - rc.top - size.cx ) / 2, 
						0, 
						0, 
						CLR_NONE, 
						CLR_NONE, 
						ILD_NORMAL );
	
	rcText = rc;
	rcText.left += size.cx + 4;

	hOldFont = SelectObject( hdcDraw, hFont[ index ] );
	
	DrawText( hdcDraw, pThumb->szName, strlen( pThumb->szName ), &rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER );
	
	SelectObject( hdcDraw, hOldFont );
	
	SetTextColor( hdcDraw, oldColor );
	SetBkMode( hdcDraw, oldBkMode );

	BitBlt( hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdcDraw, 0, 0, SRCCOPY );
	
	hbmDraw = ( HBITMAP )SelectObject( hdcDraw, hbmOld );
    DeleteDC( hdcDraw );
	DeleteObject( hbmDraw );

	ReleaseDC( hwnd, hdc );
}


static void DockThumbs( ThumbInfo *pThumbLeft, ThumbInfo *pThumbRight, BOOL bMoveLeft )
{
	if ( ( pThumbRight->dockOpt.hwndLeft == NULL ) && ( pThumbLeft->dockOpt.hwndRight == NULL ) )
	{
		pThumbRight->dockOpt.hwndLeft	= pThumbLeft->hwnd;
		pThumbLeft->dockOpt.hwndRight	= pThumbRight->hwnd;
	}
}


static void UndockThumbs( ThumbInfo *pThumb1, ThumbInfo *pThumb2 )
{
	if ( ( pThumb1 == NULL ) || ( pThumb2 == NULL ) )
	{
		return;
	}

	if ( pThumb1->dockOpt.hwndRight == pThumb2->hwnd )
	{
		pThumb1->dockOpt.hwndRight = NULL;
	}

	if ( pThumb1->dockOpt.hwndLeft == pThumb2->hwnd )
	{
		pThumb1->dockOpt.hwndLeft = NULL;
	}
	
	if ( pThumb2->dockOpt.hwndRight == pThumb1->hwnd )
	{
		pThumb2->dockOpt.hwndRight = NULL;
	}

	if ( pThumb2->dockOpt.hwndLeft == pThumb1->hwnd )
	{
		pThumb2->dockOpt.hwndLeft = NULL;
	}
}


static void PositionThumb( ThumbInfo *pThumb, short nX, short nY )
{
	ThumbInfo	*pCurThumb	= pThumbsList;
	ThumbInfo	*pDockThumb	= pThumb;
	RECT		rc;
	RECT		rcThumb;
	RECT		rcOld;
	int			nNewX;
	int			nNewY;
	int			nWidth;
	int			nHeight;
	POINT		pt;
	RECT		rcLeft;
	RECT		rcTop;
	RECT		rcRight;
	RECT		rcBottom;
	BOOL		bDocked;
	BOOL		bDockedLeft;
	BOOL		bDockedRight;
	BOOL		bLeading;
	
	if ( pThumb == NULL ) return;

	// Get thumb dimnsions
	GetWindowRect( pThumb->hwnd, &rcThumb );
	nWidth	= rcThumb.right - rcThumb.left;
	nHeight = rcThumb.bottom - rcThumb.top;

	// Docking and screen boundaries check
	SnapToScreen( rcThumb, nX, nY, &nNewX, &nNewY );
	
	bLeading = pThumb->dockOpt.hwndRight != NULL;

	if ( bMoveTogether )
	{
		UndockThumbs( pThumb, FindThumb( pThumb->dockOpt.hwndLeft ) );
		GetWindowRect( pThumb->hwnd, &rcOld );
	}


	while( pCurThumb != NULL )
	{
		if ( pCurThumb != pThumb )
		{
			GetWindowRect( pThumb->hwnd, &rcThumb );
			OffsetRect( &rcThumb, nX - rcThumb.left, nY - rcThumb.top );
			
			GetWindowRect( pCurThumb->hwnd, &rc );

			// These are rects we will dock into
					
			rcLeft.left		= rc.left - nOffs;
			rcLeft.top		= rc.top - nOffs;
			rcLeft.right	= rc.left + nOffs;
			rcLeft.bottom	= rc.bottom + nOffs;
						
			rcTop.left		= rc.left - nOffs;
			rcTop.top		= rc.top - nOffs;
			rcTop.right		= rc.right + nOffs;
			rcTop.bottom	= rc.top + nOffs;

			rcRight.left	= rc.right - nOffs;
			rcRight.top		= rc.top - nOffs;
			rcRight.right	= rc.right + nOffs;
			rcRight.bottom	= rc.bottom + nOffs;

			rcBottom.left	= rc.left - nOffs;
			rcBottom.top	= rc.bottom - nOffs;
			rcBottom.right	= rc.right + nOffs;
			rcBottom.bottom = rc.bottom + nOffs;

			
			bDockedLeft		= FALSE;
			bDockedRight	= FALSE;

			// Upper-left
			pt.x	= rcThumb.left;
			pt.y	= rcThumb.top;
			bDocked	= FALSE;
			
			if ( PtInRect( &rcRight, pt ) )
			{
				nNewX	= rc.right;
				bDocked = TRUE;
			}
			
			if ( PtInRect( &rcBottom, pt ) )
			{
				nNewY = rc.bottom;

				if ( PtInRect( &rcLeft, pt ) )
				{
					nNewX = rc.left;
				}
			}

			if ( PtInRect( &rcTop, pt ) )
			{
				nNewY		= rc.top;
				bDockedLeft	= bDocked;
			}
			
			// Upper-right
			pt.x	= rcThumb.right;
			pt.y	= rcThumb.top;
			bDocked	= FALSE;

			if ( !bLeading && PtInRect( &rcLeft, pt ) )
			{
				if ( !bDockedLeft )
				{
					nNewX	= rc.left - nWidth;
					bDocked	= TRUE;
				}
				else if ( rc.right == rcThumb.left )
				{
					bDocked = TRUE;
				}
			}
			

			if ( PtInRect( &rcBottom, pt ) )
			{
				nNewY = rc.bottom;

				if ( PtInRect( &rcRight, pt ) )
				{
					nNewX = rc.right - nWidth;
				}
			}

			if ( !bLeading && PtInRect( &rcTop, pt ) )
			{
				nNewY			= rc.top;
				bDockedRight	= bDocked;
			}
			
			if ( bMoveTogether )
			{
				if ( bDockedRight )
				{
					DockThumbs( pThumb, pCurThumb, TRUE );
				}
				
				if ( bDockedLeft )
				{
					DockThumbs( pCurThumb, pThumb, FALSE );
				}
			}									

			// Lower-left
			pt.x = rcThumb.left;
			pt.y = rcThumb.bottom;

			if ( PtInRect( &rcRight, pt ) )
			{
				nNewX = rc.right;
			}

			if ( PtInRect( &rcTop, pt ) )
			{
				nNewY = rc.top - nHeight;

				if ( PtInRect( &rcLeft, pt ) )
				{
					nNewX = rc.left;
				}
			}
			

			// Lower-right
			pt.x = rcThumb.right;
			pt.y = rcThumb.bottom;

			if ( !bLeading && PtInRect( &rcLeft, pt ) )
			{
				nNewX = rc.left - nWidth;
			}

			if ( !bLeading && PtInRect( &rcTop, pt ) )
			{
				nNewY = rc.top - nHeight;

				if ( PtInRect( &rcRight, pt ) )
				{
					nNewX = rc.right - nWidth;
				}
			}
		}
		
		pCurThumb = pCurThumb->pNext;

	}

	// Adjust coords once again
	SnapToScreen( rcThumb, nNewX, nNewY, &nNewX, &nNewY );
	
	SetWindowPos(	pThumb->hwnd, 
					HWND_TOPMOST, 
					nNewX, 
					nNewY, 
					0, 
					0,
					SWP_NOSIZE /* | SWP_NOZORDER */ | SWP_NOACTIVATE );


	// OK, move all docked thumbs
	if ( bMoveTogether )
	{
		pDockThumb = FindThumb( pDockThumb->dockOpt.hwndRight );
		
		PositionThumb( pDockThumb, (short)( nNewX + nWidth ), (short)nNewY );
	}
}



static void GetScreenRect()
{
	rcScreen.left	= GetSystemMetrics( SM_XVIRTUALSCREEN );
	rcScreen.top	= GetSystemMetrics( SM_YVIRTUALSCREEN );
	rcScreen.right	= GetSystemMetrics( SM_CXVIRTUALSCREEN ) + rcScreen.left;
	rcScreen.bottom	= GetSystemMetrics( SM_CYVIRTUALSCREEN ) + rcScreen.top;
}


static void SnapToScreen( RECT rcThumb, int nX, int nY, int *pX, int *pY )
{
	int nWidth;
	int nHeight;

	assert( NULL != pX );
	assert( NULL != pY );
	
	nWidth	= rcThumb.right - rcThumb.left;
	nHeight = rcThumb.bottom - rcThumb.top;

	*pX = nX < ( nOffs + rcScreen.left ) ? rcScreen.left : nX;
	*pY = nY < ( nOffs + rcScreen.top ) ? rcScreen.top : nY;
	*pX = *pX > ( rcScreen.right - nWidth - nOffs ) ? ( rcScreen.right - nWidth ) : *pX;
	*pY = *pY > ( rcScreen.bottom - nHeight - nOffs ) ? ( rcScreen.bottom - nHeight ) : *pY;
}



static void RefreshContactIcon( HANDLE hContact, int iIcon )
{
	ThumbInfo *pThumb	= FindThumbByContact( hContact );	

	if ( ( hContact != NULL ) && ( pThumb != NULL ) )
	{
		if ( iIcon == 0xFFFFFFFF )
		{
			pThumb->iIcon = CallService( MS_CLIST_GETCONTACTICON, (WPARAM)pThumb->hContact, 0 );	
		}
		else
		{
			pThumb->iIcon = iIcon;
			RepaintWindow( pThumb->hwnd );
		}
	}
}



void OnStatusChanged()
{
	ThumbInfo	*pThumb		= pThumbsList;
	int			idStatus	= ID_STATUS_OFFLINE;

	while( pThumb != NULL )
	{
		idStatus = GetContactStatus( pThumb->hContact );
		RefreshContactStatus( pThumb, idStatus );

		pThumb = pThumb->pNext;
	}
}



static void RefreshContactStatus( ThumbInfo *pThumb, int idStatus )
{
	if ( pThumb != NULL )
	{
		if ( IsStatusVisible( idStatus ) )
		{
			RegisterFileDropping( pThumb->hwnd, &pThumb->dropTarget );
		}
		else
		{
			UnregisterFileDropping( pThumb->hwnd );
		}

		ShowWindow( pThumb->hwnd, bHideAll || HideOnFullScreen() || ( bHideOffline && ( !IsStatusVisible( idStatus ) ) ) ? SW_HIDE : SW_SHOWNA );
	}
}



void ApplyOptionsChanges()
{
	ThumbInfo	*pThumb		= pThumbsList;

	CreateThumbsFont();
	CreateBackgroundBrush();

	dwOfflineModes	= DBGetContactSettingDword( NULL, "CLC","OfflineModes", MODEF_OFFLINE );

	OnStatusChanged();

	while( pThumb != NULL )
	{
		ResizeThumb( pThumb );
		RepaintWindow( pThumb->hwnd );

		pThumb = pThumb->pNext;
	}
}



///////////////////////////////////////////////////////
// Window creation
static void RegisterWindowClass()
{
	WNDCLASSEX	wcx;
	ZeroMemory( &wcx, sizeof( wcx ) );
		
	wcx.cbSize			=	sizeof( WNDCLASSEX );
	wcx.style			=	CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wcx.lpfnWndProc		=	CommWndProc;
	wcx.cbClsExtra		=	0;
	wcx.cbWndExtra		=	0;
	wcx.hInstance		=	hInst;
	wcx.hIcon			=	NULL;
	wcx.hCursor			=	NULL;
	wcx.hbrBackground	=	GetSysColorBrush(COLOR_3DFACE);
	wcx.lpszMenuName	=	NULL;
	wcx.lpszClassName	=	WND_CLASS;
	wcx.hIconSm			=	NULL;
	
	SetLastError( 0 );
	
	RegisterClassEx( &wcx );
}



static void UnregisterWindowClass()
{
	UnregisterClass( WND_CLASS, hInst );
}



static void CreateThumbWnd( char *szName, HANDLE hContact, int nX, int nY )
{
	HWND		hwnd		= NULL;
	ThumbInfo	*pThumb		= FindThumbByContact( hContact );

	// Prepare for window creation
	if ( pThumb == NULL )
	{
		hwnd = CreateWindowEx(	WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
								WND_CLASS,
								szName,
								WS_POPUP,
								nX,
								nY,
								50,
								20,
								NULL, // ?? hwndMiranda
								NULL,
								hInst,
								NULL
								 );

		if ( hwnd != NULL ) 
		{
			//SetFocus( hwnd );
			//??SetWindowPos( hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

			pThumb = AddThumb( hwnd, szName, hContact );
			SetWindowLong( hwnd, GWL_USERDATA, (long)pThumb );
			ResizeThumb( pThumb );
			
			SetThumbOpacity( pThumb, thumbAlpha );
			ShowWindow( hwnd, bHideOffline ? SW_HIDE : SW_SHOWNA );
			PositionThumb( pThumb, (short)nX, (short)nY );
		}
	}
}



static void ResizeThumb( ThumbInfo *pThumb )
{
	HDC		hdc			= NULL;
	HFONT	hOldFont	= NULL;
	POINT	ptText;
	SIZEL	sizeIcon;
	SIZEL	sizeText;
	RECT	rcThumb;
	int		index		= FLT_FONTID_NOTONLIST;

	ThumbInfo *pNextThumb = NULL;
	
	himl = ( HIMAGELIST )CallService( MS_CLIST_GETICONSIMAGELIST, 0, 0 );
	
	if ( himl == NULL ) return;
	
	if ( !ImageList_GetIconSize( himl, &(int)sizeIcon.cx, &(int)sizeIcon.cy ) ) return;
	if ( pThumb == NULL ) return;

	hdc = GetWindowDC( pThumb->hwnd );

	if (!DBGetContactSettingByte(pThumb->hContact, "CList", "NotOnList", 0))
	{
		int nStatus;
		int nContactStatus;
		int nApparentMode;
		char* szProto;

		szProto = (char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)pThumb->hContact, 0);
		
		if ( NULL != szProto )
		{
			nStatus = CallProtoService(szProto, PS_GETSTATUS, 0, 0);
			nContactStatus = DBGetContactSettingWord(pThumb->hContact, szProto, "Status", ID_STATUS_OFFLINE);
			nApparentMode = DBGetContactSettingWord(pThumb->hContact, szProto, "ApparentMode", 0);
			
			if (	(nStatus == ID_STATUS_INVISIBLE && nApparentMode == ID_STATUS_ONLINE)
				||	(nStatus != ID_STATUS_INVISIBLE && nApparentMode == ID_STATUS_OFFLINE)
				)
			{
				if (ID_STATUS_OFFLINE == nContactStatus)
				{
					index = FLT_FONTID_OFFINVIS;
				}
				else
				{
					index =	FLT_FONTID_INVIS;
				}
			}
			else if (ID_STATUS_OFFLINE == nContactStatus)
			{
				index = FLT_FONTID_OFFLINE;
			}
			else
			{
				index = FLT_FONTID_CONTACTS;
			}
		}
	}
	else
	{
		index = FLT_FONTID_NOTONLIST;
	}

	hOldFont = SelectObject( hdc, hFont[ index ] );
	
	// Get text and icon sizes
	GetTextExtentPoint32( hdc, pThumb->szName, strlen( pThumb->szName ), &sizeText );
	
	
	SelectObject( hdc, hOldFont );
	
	// Transform text size
	ptText.x = sizeText.cx;
	ptText.y = sizeText.cy;
	LPtoDP( hdc, &ptText, 1 );

	SetWindowPos(	pThumb->hwnd, 
					HWND_TOPMOST, 
					0, 
					0, 
					bFixedWidth ? nThumbWidth : sizeIcon.cx + ptText.x + 10, 
					( ( sizeIcon.cy > ptText.y ) ? sizeIcon.cy : ptText.y ) + 4, 
					SWP_NOMOVE /* | SWP_NOZORDER */ | SWP_NOACTIVATE );
	
	RefreshContactIcon( pThumb->hContact, 0xFFFFFFFF );

	
	// Move the docked widnow if needed
	pNextThumb = FindThumb( pThumb->dockOpt.hwndRight );

	if ( pNextThumb != NULL )
	{
		GetWindowRect( pThumb->hwnd, &rcThumb );
		PositionThumb( pNextThumb, (short)rcThumb.right, (short)rcThumb.top );
	}

	ReleaseDC( pThumb->hwnd, hdc );
}



static BOOL GetOSPlatform()
{
	OSVERSIONINFO	VersionInfo;
	
	// Get OS platform
	ZeroMemory( &VersionInfo, sizeof( VersionInfo ) );
	VersionInfo.dwOSVersionInfoSize = sizeof( VersionInfo );
	
	GetVersionEx( &VersionInfo );
	return( VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT );
}



static void CreateThumbsFont()
{
	int nFontId;
	LOGFONT lf;

	for (nFontId = 0; nFontId < FLT_FONTIDS; nFontId++)
	{
		if (NULL != hFont[nFontId])
		{
			DeleteObject(hFont[nFontId]);
			hFont[nFontId] = NULL;
		}
		GetFontSetting(TRUE, nFontId, &lf, &tColor[nFontId]);
		hFont[nFontId] = CreateFontIndirect(&lf);
	}
}



static void CreateBackgroundBrush()
{
	LOGBRUSH	lb;
	DWORD		bkColor = DBGetContactSettingDword(NULL, szModuleDB, "BkColor", FLT_DEFAULT_BKGNDCOLOR);


	if (NULL != hLTEdgesPen)
	{
		DeleteObject(hLTEdgesPen);
		hLTEdgesPen = NULL;
	}
	if (NULL != hRBEdgesPen)
	{
		DeleteObject(hRBEdgesPen);
		hRBEdgesPen = NULL;
	}
	if (NULL != hBmpBackground)
	{
		DeleteObject(hBmpBackground);
		hBmpBackground = NULL;
	}
	if (NULL != hBkBrush)
	{
		DeleteObject( hBkBrush );
		hBkBrush = NULL;
	}

	if (DBGetContactSettingByte(NULL, szModuleDB, "DrawBorder", FLT_DEFAULT_DRAWBORDER))
	{
		COLORREF cr;

		cr = (COLORREF)DBGetContactSettingDword(NULL, szModuleDB, "LTEdgesColor", FLT_DEFAULT_LTEDGESCOLOR);
		hLTEdgesPen = CreatePen(PS_SOLID, 1, cr);
		cr = (COLORREF)DBGetContactSettingDword(NULL, szModuleDB, "RBEdgesColor", FLT_DEFAULT_RBEDGESCOLOR);
		hRBEdgesPen = CreatePen(PS_SOLID, 1, cr);
	}
	if (DBGetContactSettingByte(NULL, szModuleDB, "BkUseBitmap", FLT_DEFAULT_BKGNDUSEBITMAP))
	{
		DBVARIANT dbv;

		if (!DBGetContactSetting(NULL, szModuleDB, "BkBitmap", &dbv))
		{
			hBmpBackground = (HBITMAP)CallService(MS_UTILS_LOADBITMAP, 0, (LPARAM)dbv.pszVal);
			DBFreeVariant(&dbv);
		}
	}
	nBackgroundBmpUse = (WORD)DBGetContactSettingWord(NULL, szModuleDB, "BkBitmapOpt", FLT_DEFAULT_BKGNDBITMAPOPT);

	// Create brush
	lb.lbColor	= bkColor;
	lb.lbStyle	= BS_SOLID;
	lb.lbHatch	= 0;
	hBkBrush	= CreateBrushIndirect( &lb );
}


static int GetContactStatus( HANDLE hContact )
{
	char	*szProto	= NULL;
	int		idStatus	= ID_STATUS_OFFLINE;

	if ( hContact == NULL )
	{
		assert( !"Contact is NULL" );
		return( 0 );
	}

	szProto	= ( char* )CallService( "Proto/GetContactBaseProto", (WPARAM)hContact, 0 );
	
	if ( NULL != szProto )
	{
		idStatus	= DBGetContactSettingWord( hContact, szProto, "Status", ID_STATUS_OFFLINE );
	}
	

	return( idStatus );
}


static int ClcStatusToPf2( int status )
{
	switch(status) {
		case ID_STATUS_ONLINE: return PF2_ONLINE;
		case ID_STATUS_AWAY: return PF2_SHORTAWAY;
		case ID_STATUS_DND: return PF2_HEAVYDND;
		case ID_STATUS_NA: return PF2_LONGAWAY;
		case ID_STATUS_OCCUPIED: return PF2_LIGHTDND;
		case ID_STATUS_FREECHAT: return PF2_FREECHAT;
		case ID_STATUS_INVISIBLE: return PF2_INVISIBLE;
		case ID_STATUS_ONTHEPHONE: return PF2_ONTHEPHONE;
		case ID_STATUS_OUTTOLUNCH: return PF2_OUTTOLUNCH;
		case ID_STATUS_OFFLINE: return MODEF_OFFLINE;
	}
	return 0;
}

static BOOL IsStatusVisible( int status )
{
	return ( 0 == ( dwOfflineModes & ClcStatusToPf2( status ) ) );
}



static void RegHotkey( char* szName, HWND hwnd )
{
	char szBuf[ MAX_PATH ]= {0};
	char szPath[ MAX_PATH ]= {0};

	GetModuleFileName( NULL, szPath, MAX_PATH );
	PathRemoveFileSpec( szPath );
	strcat( szPath, "\\Thumbs.ini" );
	GetPrivateProfileString( "Hotkeys", szName, "", szBuf, MAX_PATH - 1, szPath );

	if ( 0 != strlen( szBuf ) )
	{
		UINT nModifiers			= 0;
		char chKey				= 0;
		char szMod[ 2 ][ 20 ]	= {0};
		char szKey[ 20 ]		= {0};
		int i = 0;

		sscanf( szBuf, "%[^'+']+%[^'+']+%[^'+']", szMod[ 0 ], szMod[ 1 ], szKey );

		for (  i = 0; i < 2; ++i )
		{
			if( 0 == strncmp( szMod[ i ], "ALT", 19 ) )
			{
				nModifiers = nModifiers | MOD_ALT;
			}
			else if( 0 == strncmp( szMod[ i ], "CTRL", 19 ) )
			{
				nModifiers = nModifiers | MOD_CONTROL;
			}
			else if( 0 == strncmp( szMod[ i ], "SHIFT", 19 ) )
			{
				nModifiers = nModifiers | MOD_SHIFT;
			}
		}

		chKey = szKey[ 0 ];

		RegisterHotKey( hwnd, (int)hwnd, nModifiers, VkKeyScan( chKey ) );
	}
}



static void ThumbSelect( ThumbInfo *pThumb, BOOL bMouse )
{

	if ( pThumb == NULL ) return;

	if ( bMouse )
	{
		bMouseIn = TRUE;
		SetCapture( pThumb->hwnd );
	}

	SetThumbOpacity( pThumb, 255 );
}


static void ThumbDeselect( ThumbInfo *pThumb, BOOL bMouse )
{
	if ( pThumb == NULL ) return;

	if ( bMouse )
	{
		bMouseIn = FALSE;
		ReleaseCapture();
	}
	
	SetThumbOpacity( pThumb, thumbAlpha );
}

///////////////////////////////////////////////////////
// Contact sttings

static void SaveContactsPos()
{	
	ContactDB				contact;
	RECT					rc;
	ThumbInfo				*pThumb = NULL;
	
	pThumb = pThumbsList;

	while( pThumb != NULL )
	{
		SetLastError( 0 );

		GetWindowRect( pThumb->hwnd, &rc );
		
		if( 0 == GetLastError() )
		{
			contact.nXPos	= (WORD)rc.left;
			contact.nYPos	= (WORD)rc.top;
			
			DBWriteContactSettingDword( pThumb->hContact, "FloatingContacts", "ThumbsPos", *(DWORD*)&contact );
		}
		
		pThumb = pThumb->pNext;
	}
}


static void LoadContacts()
{
	HANDLE	hContact = (HANDLE)CallService( MS_DB_CONTACT_FINDFIRST, 0, 0) ;

	while( hContact != NULL )
	{
		LoadContact( hContact );
		
		hContact = (HANDLE)CallService( MS_DB_CONTACT_FINDNEXT, ( WPARAM ) hContact, 0 );
	}
}



static void LoadMenus()
{
	CLISTMENUITEM mi;

		// Remove thumb menu item
		CreateServiceFunction( "FloatingContacts/RemoveThumb",OnContactMenu_Remove );
		ZeroMemory( &mi,sizeof( mi ) );
		
		mi.cbSize		= sizeof( mi );
		mi.position		= 0xFFFFF;
		mi.flags		= 0;
		mi.hIcon		= LoadIcon( hInst, MAKEINTRESOURCE( IDI_HIDE ) );
		mi.pszName		= Translate("Remove thumb");
		mi.pszService	= "FloatingContacts/RemoveThumb";
		hMenuItemRemove	= (HANDLE)CallService( MS_CLIST_ADDCONTACTMENUITEM,0, ( LPARAM )&mi );

		// Hide all thumbs main menu item
		CreateServiceFunction( "FloatingContacts/MainHideAllThumbs",OnMainMenu_HideAll );
		ZeroMemory( &mi,sizeof( mi ) );
		
		mi.cbSize		= sizeof( mi );
		mi.position		= 0xFFFFF;
		mi.flags		= 0;
		mi.hIcon		= LoadIcon( hInst, MAKEINTRESOURCE( bHideAll ? IDI_SHOW : IDI_HIDE ) );
		mi.pszName		= bHideAll ? Translate("Show all thumbs") : Translate("Hide all thumbs");
		mi.pszService	= "FloatingContacts/MainHideAllThumbs";
		hMainMenuItemHideAll	= (HANDLE)CallService( MS_CLIST_ADDMAINMENUITEM,0, ( LPARAM )&mi );
}


static void LoadContact( HANDLE hContact )
{
	DWORD	dwPos		= (DWORD)-1;
	char	*pName		= NULL;
	ThumbInfo	*pThumb	= FindThumbByContact( hContact );
	int		nX, nY;
	

	if ( hContact == NULL ) return;

	dwPos = DBGetContactSettingDword( hContact, "FloatingContacts", "ThumbsPos", (DWORD)-1 );

	if ( dwPos != -1 )
	{
		pName = (char*)CallService( MS_CLIST_GETCONTACTDISPLAYNAME, ( WPARAM )hContact, 0 );

		if ( pName != NULL )
		{
			nX = LOWORD( dwPos );
			nY = HIWORD( dwPos );
			
			CreateThumbWnd( pName, hContact, nX, nY );
			PositionThumb( pThumb, (short)nX, (short)nY );
		}
	}
}


static void DeleteContactPos( ThumbInfo *pThumb )
{
	if ( pThumb == NULL )	return;

	DBDeleteContactSetting( pThumb->hContact, "FloatingContacts", "ThumbsPos" );
}



BOOL HideOnFullScreen()
{
	BOOL bFullscreen = FALSE;
	HWND hWnd = 0;
	
	if ( bHideWhenFullscreen )
	{
		int w = GetSystemMetrics(SM_CXSCREEN);
		int h = GetSystemMetrics(SM_CYSCREEN);
	
		hWnd = GetForegroundWindow();

		while (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST)
		{
			RECT WindowRect;
			GetWindowRect(hWnd, &WindowRect);
			
			if ( (w == (WindowRect.right - WindowRect.left) ) &&
				(h == (WindowRect.bottom - WindowRect.top)))
			{	
				bFullscreen = TRUE;
				break;
			}
		
			hWnd = GetNextWindow( hWnd, GW_HWNDNEXT );
		}
	}
	
	return bFullscreen && bHideWhenFullscreen;
}


static int OnContactMenu_Remove( WPARAM wParam,LPARAM lParam )
{
	HANDLE		hContact	= ( HANDLE )wParam;
	ThumbInfo	*pThumb		= FindThumbByContact( hContact );

	DeleteContactPos( pThumb );
	RemoveThumb( pThumb );

	DestroyMenu( hContactMenu );

	return 0;
}




static int OnMainMenu_HideAll( WPARAM wParam,LPARAM lParam )
{
	CLISTMENUITEM clmi = {0};
	int b;

	bHideAll = !bHideAll;
	DBWriteContactSettingByte(NULL, szModuleDB, "HideAll", (BYTE)bHideAll);
	OnStatusChanged();

	clmi.cbSize		= sizeof( clmi );
	clmi.flags		= CMIM_NAME | CMIM_ICON;
	clmi.hIcon		= LoadIcon( hInst, MAKEINTRESOURCE( bHideAll ? IDI_SHOW : IDI_HIDE ) );
	clmi.pszName	= bHideAll ? Translate("Show all thumbs") : Translate("Hide all thumbs");
	b = CallService( MS_CLIST_MODIFYMENUITEM, ( WPARAM )hMainMenuItemHideAll, ( LPARAM )&clmi );
	return 0;
}