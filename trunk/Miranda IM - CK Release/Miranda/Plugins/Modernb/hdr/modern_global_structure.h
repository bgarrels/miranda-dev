/*
Author Artem Shpynov aka FYR

Miranda IM: the free IM client for Microsoft* Windows*

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

===============================================================================
*/

#pragma once

#ifndef modern_global_structure_h__
#define modern_global_structure_h__

typedef struct tagCLUIDATA
{
	/************************************ 
	**         Global variables       **
	************************************/

	/*         NotifyArea menu          */
	HMENU		hMenuNotify;             
	WORD		wNextMenuID;	
	int			iIconNotify;
	BOOL		bEventAreaEnabled;
	BOOL		bNotifyActive;
	DWORD       dwFlags;
	TCHAR *     szNoEvents;
	int         hIconNotify;
	HANDLE      hUpdateContact;

	/*         Contact List View Mode          */
	TCHAR	groupFilter[2048];
	char	protoFilter[2048];
	char	varFilter[2048];
	DWORD	lastMsgFilter;
	char	current_viewmode[256], old_viewmode[256];
	BYTE	boldHideOffline;
	BYTE	bOldUseGroups;
	DWORD	statusMaskFilter;
	DWORD	stickyMaskFilter;
	DWORD	filterFlags;
	DWORD	bFilterEffective;
	BOOL	bMetaAvail;
	DWORD	t_now;

	// Modern Global Variables
	BOOL	fDisableSkinEngine;
	BOOL	fOnDesktop;
	BOOL	fSmoothAnimation;
	BOOL	fLayered;
	BOOL	fDocked;
	BOOL	fGDIPlusFail;
	BOOL	fSortNoOfflineBottom;
	BOOL	fAutoSize;
	BOOL	fAeroGlass;
	HRGN	hAeroGlassRgn;

	BOOL	mutexPreventDockMoving;
	BOOL    mutexOnEdgeSizing;
	BOOL    mutexPaintLock;

	BYTE	bCurrentAlpha;
	BYTE	bSTATE;
	BYTE	bBehindEdgeSettings;
	BYTE	bSortByOrder[3];

	signed char nBehindEdgeState;

	DWORD	dwKeyColor;

	HWND	hwndEventFrame;

	int		LeftClientMargin;
	int		RightClientMargin; 
	int		TopClientMargin;
	int		BottomClientMargin;

	BOOL	bInternalAwayMsgDiscovery;
	BOOL	bRemoveAwayMessageForOffline;

	//hEventHandles

	HANDLE hEventExtraImageListRebuilding;
	HANDLE hEventExtraImageApplying;
	HANDLE hEventExtraClick;
	HANDLE hEventBkgrChanged;
	HANDLE hEventPreBuildTrayMenu;
	HANDLE hEventPreBuildFrameMenu;
	HANDLE hEventPreBuildGroupMenu;
	HANDLE hEventPreBuildSubGroupMenu;
	HANDLE hEventStatusBarShowToolTip;
	HANDLE hEventStatusBarHideToolTip;
	HANDLE hEventToolBarModuleLoaded;
	HANDLE hEventSkinServicesCreated;

	int	   nGapBetweenTitlebar;
} CLUIDATA;

EXTERN_C CLUIDATA g_CluiData;

#endif // modern_global_structure_h__
