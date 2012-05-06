#ifndef __ICustomDestinationList_INTERFACE_DEFINED__
#define __ICustomDestinationList_INTERFACE_DEFINED__

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

typedef /* [v1_enum] */ enum tagKNOWNDESTCATEGORY {
  KDC_FREQUENT = 1,
  KDC_RECENT = (KDC_FREQUENT + 1)
} KNOWNDESTCATEGORY;

MIDL_INTERFACE("6332debf-87b5-4670-90c0-5e57b408a49e")
ICustomDestinationList : public IUnknown {
 public:
  virtual HRESULT STDMETHODCALLTYPE SetAppID(
      /* [string][in] */__RPC__in_string LPCWSTR pszAppID) = 0;
  virtual HRESULT STDMETHODCALLTYPE BeginList(
      /* [out] */ __RPC__out UINT *pcMaxSlots,
      /* [in] */ __RPC__in REFIID riid,
      /* [iid_is][out] */ __RPC__deref_out_opt void **ppv) = 0;
  virtual HRESULT STDMETHODCALLTYPE AppendCategory(
      /* [string][in] */ __RPC__in_string LPCWSTR pszCategory,
      /* [in] */ __RPC__in_opt IObjectArray *poa) = 0;
  virtual HRESULT STDMETHODCALLTYPE AppendKnownCategory(
      /* [in] */ KNOWNDESTCATEGORY category) = 0;
  virtual HRESULT STDMETHODCALLTYPE AddUserTasks(
      /* [in] */ __RPC__in_opt IObjectArray *poa) = 0;
  virtual HRESULT STDMETHODCALLTYPE CommitList(void) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetRemovedDestinations(
      /* [in] */ __RPC__in REFIID riid,
      /* [iid_is][out] */ __RPC__deref_out_opt void **ppv) = 0;
  virtual HRESULT STDMETHODCALLTYPE DeleteList(
      /* [string][in] */ __RPC__in_string LPCWSTR pszAppID) = 0;
  virtual HRESULT STDMETHODCALLTYPE AbortList(void) = 0;
};

#endif  // __ICustomDestinationList_INTERFACE_DEFINED__
