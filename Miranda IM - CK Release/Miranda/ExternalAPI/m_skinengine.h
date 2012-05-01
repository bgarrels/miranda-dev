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

struct ISkinBackend;
struct ISkinElement;

struct SkinRenderParams
{
	HDC hdc;
	RECT rc;
};

struct ISkinDataSource
{
	virtual LPCTSTR GetText(const TCHAR *key) = 0;
	virtual HICON GetIcon(const TCHAR *key) = 0;
	virtual HBITMAP GetBitmap(const TCHAR *key) = 0;
	virtual ISkinBackend *GetObject(const TCHAR *key) = 0;
};

struct ISkinElement
{
	// general manadgement
	virtual void SetParent(ISkinElement *parent) = 0;
	virtual void LoadFromXml(HXML hXml) = 0;
	virtual void SetId(const TCHAR *id) = 0;
	virtual void SetDataSource(ISkinDataSource *ds) = 0;
	virtual void Destroy() = 0;

	// rendering and layouting
	virtual void Measure(SkinRenderParams *params) = 0;
	virtual void Layout(SkinRenderParams *params) = 0;
	virtual void Paint(SkinRenderParams *params) = 0;

	// element tree
	virtual bool IsComplexObject() = 0;
	virtual ISkinElement *GetParent() = 0;
	virtual int GetChildCount() = 0;
	virtual ISkinElement *GetChild(int index) = 0;
	virtual bool AppendChild(ISkinElement *child) = 0;
	virtual bool InsertChild(ISkinElement *child, int index) = 0;
	virtual void RemoveChild(ISkinElement *child) = 0;

	// element properties
	virtual void SetPropText(const TCHAR *key, const TCHAR *value) = 0;
	virtual const TCHAR *GetPropText(const TCHAR *key, const TCHAR *value) = 0;
	virtual void SetPropInt(const TCHAR *key, int value) = 0;
	virtual void SetPropIntText(const TCHAR *key, const TCHAR *value) = 0;
	virtual int GetPropInt(const TCHAR *key) = 0;
};

struct ISkinBackend
{
	virtual LPCTSTR GetText(const TCHAR *key) = 0;
	virtual HICON GetIcon(const TCHAR *key) = 0;
	virtual HBITMAP GetBitmap(const TCHAR *key) = 0;
	virtual ISkinBackend *GetObject(const TCHAR *key) = 0;
};
