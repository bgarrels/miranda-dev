
#ifndef __FLTCONT_H__
#define __FLTCONT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
//

#define FLT_FONTID_CONTACTS    0
#define FLT_FONTID_INVIS       1
#define FLT_FONTID_OFFLINE     2
#define FLT_FONTID_OFFINVIS    3
#define FLT_FONTID_NOTONLIST   4

#define FLT_FONTIDS 5

#define DBFONTF_BOLD       1
#define DBFONTF_ITALIC     2
#define DBFONTF_UNDERLINE  4

/////////////////////////////////////////////////////////////////////////////
//

#define FLT_DEFAULT_DRAWBORDER			TRUE
#define FLT_DEFAULT_LTEDGESCOLOR		GetSysColor(COLOR_3DHILIGHT)
#define FLT_DEFAULT_RBEDGESCOLOR		GetSysColor(COLOR_3DDKSHADOW)
#define FLT_DEFAULT_BKGNDCOLOR			GetSysColor(COLOR_3DFACE)
#define FLT_DEFAULT_BKGNDUSEBITMAP		FALSE
#define FLT_DEFAULT_BKGNDBITMAPOPT		CLB_STRETCH

/////////////////////////////////////////////////////////////////////////////
//

extern char szModuleDB[];

extern HINSTANCE	hInst;
extern BOOL			bNT;
extern BOOL			bHideOffline;
extern BOOL			bHideAll;
extern BOOL			bHideWhenFullscreen;
extern BOOL			bMoveTogether;
extern BOOL			bFixedWidth;
extern int			nThumbWidth;
extern BYTE			thumbAlpha;

extern BOOL (WINAPI *pSetLayeredWindowAttributes)(HWND,COLORREF,BYTE,DWORD);

/////////////////////////////////////////////////////////////////////////////
//

void
GetFontSetting
	( IN BOOL bFltContacts
	, IN int nFontId
	, IN LOGFONT* lf
	, IN COLORREF* colour
	);

void
ApplyOptionsChanges();

void
OnStatusChanged();

void
SetThumbsOpacity
	( IN BYTE btAlpha
	);

int
OnOptionsInitialize
	( IN WPARAM wParam
	, IN LPARAM lParam
	);

/////////////////////////////////////////////////////////////////////////////

#endif	// #ifndef __FLTCONT_H__

/////////////////////////////////////////////////////////////////////////////
// End Of File fltcont.h
