#ifndef __m_notify_popup_h__
#define __m_notify_popup_h__

/*
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

#define NFOPT_POPUP2_BACKCOLOR    "Popup2/BackColor"
#define NFOPT_POPUP2_TEXTCOLOR    "Popup2/TextColor"
#define NFOPT_POPUP2_TIMEOUT      "Popup2/Timeout"
#define NFOPT_POPUP2_LCLICKSVC    "Popup2/LClickSvc"
#define NFOPT_POPUP2_LCLICKCOOKIE "Popup2/LClickCookie"
#define NFOPT_POPUP2_RCLICKSVC    "Popup2/RClickSvc"
#define NFOPT_POPUP2_RCLICKCOOKIE "Popup2/RClickCookie"
#define NFOPT_POPUP2_STATUSMODE   "Popup2/StatusMode"
#define NFOPT_POPUP2_PLUGINDATA   "Popup2/PluginData"
#define NFOPT_POPUP2_WNDPROC      "Popup2/WndProc"

#define NFOPT_POPUP2_BACKCOLOR_S  "Popup2/BackColor/Save"
#define NFOPT_POPUP2_TEXTCOLOR_S  "Popup2/TextColor/Save"
#define NFOPT_POPUP2_TIMEOUT_S    "Popup2/Timeout/Save"

#define MS_POPUP2_SHOW   "Popup2/Show"
#define MS_POPUP2_UPDATE "Popup2/Update"
#define MS_POPUP2_REMOVE "Popup2/Remove"

#ifndef POPUP2_NOHELPERS
	#define MPopup2Show(a)   (CallService(MS_POPUP2_SHOW, 0, (LPARAM)(a)))
	#define MPopup2Update(a) (CallService(MS_POPUP2_UPDATE, 0, (LPARAM)(a)))
	#define MPopup2Remove(a) (CallService(MS_POPUP2_REMOVE, 0, (LPARAM)(a)))
#endif

#endif // __m_notify_popup_h__
