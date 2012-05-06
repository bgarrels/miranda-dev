#ifndef __ITaskbarList3_FWD_DEFINED__
#define __ITaskbarList3_FWD_DEFINED__

/*
W7ui plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2009 nullbie, persei

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

typedef interface ITaskbarList3 ITaskbarList3;
#endif 	/* __ITaskbarList3_FWD_DEFINED__ */

/* interface ITaskbarList3 */
/* [object][uuid] */ 
   
MIDL_INTERFACE("ea1afb91-9e28-4b86-90e9-9e9f8a5eefaf")
ITaskbarList3 : public ITaskbarList2
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetProgressValue( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ ULONGLONG ullCompleted,
        /* [in] */ ULONGLONG ullTotal) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetProgressState( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ TBPFLAG tbpFlags) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE RegisterTab( 
        /* [in] */ __RPC__in HWND hwndTab,
        /* [in] */ __RPC__in HWND hwndMDI) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE UnregisterTab( 
        /* [in] */ __RPC__in HWND hwndTab) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetTabOrder( 
        /* [in] */ __RPC__in HWND hwndTab,
        /* [in] */ __RPC__in HWND hwndInsertBefore) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetTabActive( 
        /* [in] */ __RPC__in HWND hwndTab,
        /* [in] */ __RPC__in HWND hwndMDI,
        /* [in] */ TBATFLAG tbatFlags) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE ThumbBarAddButtons( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ UINT cButtons,
        /* [size_is][in] */ __RPC__in_ecount_full(cButtons) LPTHUMBBUTTON pButton) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE ThumbBarUpdateButtons( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ UINT cButtons,
        /* [size_is][in] */ __RPC__in_ecount_full(cButtons) LPTHUMBBUTTON pButton) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE ThumbBarSetImageList( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ __RPC__in_opt HIMAGELIST himl) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetOverlayIcon( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ __RPC__in HICON hIcon,
        /* [string][in] */ __RPC__in_string LPCWSTR pszDescription) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetThumbnailTooltip( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [string][in] */ __RPC__in_string LPCWSTR pszTip) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetThumbnailClip( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ __RPC__in RECT *prcClip) = 0;

};
