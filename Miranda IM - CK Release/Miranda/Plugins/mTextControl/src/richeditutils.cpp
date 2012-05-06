/*
Miranda Text Control for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
				Copyright (C) 2005 Victor Pavlychko (nullbie@gmail.com),
							  2010 Merlin_de

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

#include "headers.h"

class CREOleCallback: public IRichEditOleCallback
{
private:
	unsigned refCount;
	IStorage *pictStg;
	int nextStgId;
public:
	CREOleCallback()
	{
		refCount = 1;
		pictStg = 0;
		nextStgId = 0;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID * ppvObj)
	{
		if (IsEqualIID(riid, IID_IRichEditOleCallback))
		{
			*ppvObj = this;
			this->AddRef();
			return S_OK;
		}
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{
		if (this->refCount == 0) {
			if (S_OK != StgCreateDocfile(NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DELETEONRELEASE, 0, &this->pictStg))
	            this->pictStg = NULL;
			this->nextStgId = 0;
	    }
	    return ++this->refCount;
	}

	ULONG STDMETHODCALLTYPE Release()
	{
	    if (--this->refCount == 0) {
	        if (this->pictStg)
				this->pictStg->Release();
	    }
	    return this->refCount;
	}

	HRESULT STDMETHODCALLTYPE  ContextSensitiveHelp(BOOL fEnterMode)
	{
	    return S_OK;
	}

	HRESULT STDMETHODCALLTYPE  DeleteObject(LPOLEOBJECT lpoleobj)
	{
	    return S_OK;
	}

	HRESULT STDMETHODCALLTYPE  GetClipboardData(CHARRANGE * lpchrg, DWORD reco, LPDATAOBJECT * lplpdataobj)
	{
	    return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE  GetContextMenu(WORD seltype, LPOLEOBJECT lpoleobj, CHARRANGE * lpchrg, HMENU * lphmenu)
	{
	    return E_INVALIDARG;
	}

	HRESULT STDMETHODCALLTYPE  GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
	{
	    return S_OK;
	}

	HRESULT STDMETHODCALLTYPE  GetInPlaceContext(LPOLEINPLACEFRAME * lplpFrame, LPOLEINPLACEUIWINDOW * lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
	{
	    return E_INVALIDARG;
	}

	HRESULT STDMETHODCALLTYPE  GetNewStorage(LPSTORAGE * lplpstg)
	{
		WCHAR szwName[64];
		char szName[64];
		wsprintfA(szName, "s%u", this->nextStgId);
		MultiByteToWideChar(CP_ACP, 0, szName, -1, szwName, sizeof(szwName) / sizeof(szwName[0]));
		if (this->pictStg == NULL)
			return STG_E_MEDIUMFULL;
#ifdef UNICODE
	    return this->pictStg->CreateStorage(szwName, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE, 0, 0, lplpstg);
#else
	    return this->pictStg->CreateStorage(szName, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE, 0, 0, lplpstg);
#endif
	}

	HRESULT STDMETHODCALLTYPE  QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT * lpcfFormat, DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE  QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
	{
	    return S_OK;
	}

	HRESULT STDMETHODCALLTYPE  ShowContainerUI(BOOL fShow)
	{
	    return S_OK;
	}
};

IRichEditOleCallback *reOleCallback = 0;

void InitRichEdit(ITextServices *ts)
{
	LRESULT lResult;
	ts->TxSendMessage(EM_SETOLECALLBACK, 0, (LPARAM)reOleCallback, &lResult);
}

LRESULT CALLBACK RichEditProxyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ITextServices *ts = (ITextServices *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (ts && (msg != WM_DESTROY))
	{
		LRESULT lResult;
		ts->TxSendMessage(msg, wParam, lParam, &lResult);
		return lResult;
	}
	return 1;
}

void LoadRichEdit()
{
	OleInitialize(NULL);
//	LoadLibrary("riched20.dll");
	reOleCallback = new CREOleCallback;
//	CoInitializeEx(NULL,COINIT_MULTITHREADED);


	WNDCLASSEX wcl;
	wcl.cbSize = sizeof(wcl);
	wcl.lpfnWndProc = RichEditProxyWndProc;
	wcl.style = CS_GLOBALCLASS;
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;
	wcl.hInstance = hInst;
	wcl.hIcon = NULL;
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wcl.lpszMenuName = NULL;
	wcl.lpszClassName = _T("NBRichEditProxyWndClass");
	wcl.hIconSm = 0; 
	RegisterClassEx(&wcl);
}

void UnloadRichEdit()
{
	delete reOleCallback;
	OleUninitialize();
}

HWND CreateProxyWindow(ITextServices *ts)
{
	HWND hwnd = CreateWindow(_T("NBRichEditProxyWndClass"), _T(""), 0, 0, 0, 0, 0, 0, 0, hInst, 0);
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)ts);
	return hwnd;
}
