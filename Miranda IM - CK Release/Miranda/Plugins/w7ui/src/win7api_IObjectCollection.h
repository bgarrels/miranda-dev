#ifndef __IObjectCollection_INTERFACE_DEFINED__
#define __IObjectCollection_INTERFACE_DEFINED__

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

MIDL_INTERFACE("5632b1a4-e38a-400a-928a-d4cd63230295")
IObjectCollection : public IObjectArray {
 public:
  virtual HRESULT STDMETHODCALLTYPE AddObject(
      /* [in] */ __RPC__in_opt IUnknown *punk) = 0;
  virtual HRESULT STDMETHODCALLTYPE AddFromArray(
      /* [in] */ __RPC__in_opt IObjectArray *poaSource) = 0;
  virtual HRESULT STDMETHODCALLTYPE RemoveObjectAt(
      /* [in] */ UINT uiIndex) = 0;
  virtual HRESULT STDMETHODCALLTYPE Clear(void) = 0;
};

#endif  // __IObjectCollection_INTERFACE_DEFINED__
