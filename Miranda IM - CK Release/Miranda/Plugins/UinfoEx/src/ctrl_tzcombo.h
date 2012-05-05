#ifndef _UI_CTRL_TZ_COMBO_INCLUDE_
#define _UI_CTRL_TZ_COMBO_INCLUDE_

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

#include "ctrl_base.h"
#include "svc_timezone.h"
#include "svc_timezone_old.h"

/**
 *
 **/
class CTzCombo : public CBaseCtrl
{
	INT _curSel;											//selectet combo index
	
	CTzCombo();
	CTzCombo(HWND hDlg, WORD idCtrl, LPCSTR pszSetting);

	INT Find(CTimeZone *pTimeZone) const;					//old UIEX method
	INT Find(LPTIME_ZONE_INFORMATION pTimeZone) const;		//new core tz interface

public:

	static FORCEINLINE CTzCombo* GetObj(HWND hCtrl) 
		{ return (CTzCombo*) GetUserData(hCtrl); }
	static FORCEINLINE CTzCombo* GetObj(HWND hDlg, WORD idCtrl)
		{ return GetObj(GetDlgItem(hDlg, idCtrl)); }

	static CBaseCtrl* CreateObj(HWND hDlg, WORD idCtrl, LPCSTR pszSetting);

	virtual VOID	Release();
//	virtual VOID	OnReset(){};
	virtual BOOL	OnInfoChanged(HANDLE hContact, LPCSTR pszProto);
	virtual VOID	OnApply(HANDLE hContact, LPCSTR pszProto);
	virtual VOID	OnChangedByUser(WORD wChangedMsg);

	VOID			GetTime(LPTSTR szTime, WORD cchTime);
};

#endif /* _UI_CTRL_TZ_COMBO_INCLUDE_ */