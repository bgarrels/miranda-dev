/*
SmileyAdd Plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author 
			Copyright (C) 2006 - 2012 Boris Krasnovskiy

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

#include "imagecache.h"
#include "options.h"

typedef BOOL (WINAPI *tAlphaBlend)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
static tAlphaBlend pAlphaBlend;

static FI_INTERFACE *fei;

static HANDLE g_hMutexIm;
static OBJLIST<ImageBase> g_imagecache(25, ImageType::CompareImg);

static bkstring lastdllname;
static HMODULE lastmodule;
static time_t laststamp;
static UINT_PTR timerId;

static void CALLBACK timerProc(HWND, UINT, UINT_PTR, DWORD) 
{
	WaitForSingleObject(g_hMutexIm, 3000);
	const time_t ts = time(NULL) - 10;
	if ( lastmodule && ts > laststamp)
	{
		FreeLibrary(lastmodule);
		lastmodule = NULL;
		lastdllname.clear();
	}

	for (int i=g_imagecache.getCount(); i--; )
		g_imagecache[i].ProcessTimerTick(ts);

	if (g_imagecache.getCount() == 0) 
	{
		g_imagecache.destroy();
		if (timerId && (timerId+1) && lastmodule == NULL) 
		{
			KillTimer(NULL, timerId);
			timerId = 0;
		}
	}

	ReleaseMutex(g_hMutexIm);
}


static void CALLBACK sttMainThreadCallback( PVOID )
{
	if (timerId == 0xffffffff) 
		timerId = SetTimer(NULL, 0, 10000, (TIMERPROC)timerProc);
}


static HMODULE LoadDll(const bkstring& file)
{
	WaitForSingleObject(g_hMutexIm, 3000);

	if (lastdllname != file) 
	{
		FreeLibrary(lastmodule);
		lastdllname = file;
#if (defined _UNICODE || defined UNICODE)
		lastmodule = LoadLibraryEx(file.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
#else
		lastmodule = LoadLibraryEx(file.c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES);
#endif
	}
	laststamp = time(NULL);

	ReleaseMutex(g_hMutexIm);
	return lastmodule;
}



ImageBase::ImageBase(unsigned id)
{
	m_id = id;
	m_lRefCount = 1;
	m_timestamp = 0;
}

long ImageBase::AddRef(void) 
{ 
	WaitForSingleObject(g_hMutexIm, 3000);
	long cnt = ++m_lRefCount;
	ReleaseMutex(g_hMutexIm);
	return cnt;
}

long ImageBase::Release(void)
{
	WaitForSingleObject(g_hMutexIm, 3000);

	long cnt = m_lRefCount;
	if (cnt) m_lRefCount = --cnt;
	if (cnt == 0) m_timestamp = time(NULL);

	ReleaseMutex(g_hMutexIm);
	return cnt;
}

void ImageBase::ProcessTimerTick(time_t ts)
{
	WaitForSingleObject(g_hMutexIm, 3000);
	if (m_lRefCount == 0 && m_timestamp < ts ) 
	{
		if (!g_imagecache.remove(this)) 
			delete this; 
	}
	ReleaseMutex(g_hMutexIm);
}

int ImageBase::CompareImg(const ImageBase* p1, const ImageBase* p2)
{
	unsigned id1 = p1->m_id;
	unsigned id2 = p2->m_id;

	if (id1 == id2) return 0;
	else return id1 < id2 ? -1 : 1;
}

void ImageBase::Draw(HDC hdc, RECT& rc, bool clip)
{
	HRGN hrgn = NULL;
	if (clip)
	{
		hrgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
		SelectClipRgn(hdc, hrgn);
	}

	SIZE iSize;
	GetSize(iSize);

	const int sizeX = rc.right - rc.left;
	const int sizeY = rc.bottom - rc.top;

	const int x = rc.left + (sizeX > iSize.cx || clip ? (sizeX - iSize.cx) / 2 : 0);
	const int y = rc.top  + (sizeY > iSize.cy || clip ? (sizeY - iSize.cy) / 2 : 0);

	const int scaleX = sizeX > iSize.cx || clip ? iSize.cx : sizeX;
	const int scaleY = sizeY > iSize.cy || clip ? iSize.cy : sizeY;

	DrawInternal(hdc, x, y, scaleX, scaleY);

	if (clip)
	{
		SelectClipRgn(hdc, NULL);
		DeleteObject(hrgn);
	}
}


HBITMAP ImageBase::GetBitmap(COLORREF bkgClr, int sizeX, int sizeY)
{
	RECT rc = { 0, 0, sizeX, sizeY };

	if (sizeX == 0 || sizeY == 0)
	{
		SIZE iSize;
		GetSize(iSize);

		if (sizeX == 0) rc.right  = iSize.cx;
		if (sizeY == 0) rc.bottom = iSize.cy;
	}

	HBRUSH hBkgBrush = CreateSolidBrush(bkgClr);
	HDC hdc = GetDC(NULL);
	HBITMAP hBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
	HDC hdcMem = CreateCompatibleDC(hdc);
	SelectObject(hdcMem, hBmp);

	FillRect(hdcMem, &rc, hBkgBrush);

	Draw(hdcMem, rc, false);

	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdc);
	DeleteObject(hBkgBrush);

	return hBmp;
}

int ImageBase::SelectNextFrame(const int frame)
{
	int res = (frame + 1) % GetFrameCount();
	SelectFrame(res);
	return res;
}



IconType::IconType(const unsigned id, const bkstring& file, const int index, const IcoTypeEnum type)
	: ImageBase(id)
{
	m_SmileyIcon = NULL;

	switch (type)
	{
	case icoDll:
		{
			const HMODULE hModule = LoadDll(file);
			if (hModule != NULL) 
				m_SmileyIcon = (HICON) LoadImage(hModule, MAKEINTRESOURCE(-index), IMAGE_ICON, 0, 0, 0);
		}
		break;

	case icoFile:
		m_SmileyIcon = (HICON) LoadImage(NULL, file.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
		break;

	default:
		ExtractIconEx(file.c_str(), index, NULL, &m_SmileyIcon, 1);
		break;
	}
}

IconType::~IconType()
{
	DestroyIcon(m_SmileyIcon);
}

void IconType::DrawInternal(HDC hdc, int x, int y, int sizeX, int sizeY)
{
	if (m_SmileyIcon != NULL)
		DrawIconEx(hdc, x, y, m_SmileyIcon, sizeX, sizeY, 0, NULL, DI_NORMAL);
}

HICON IconType::GetIcon(void) 
{ 
	return (HICON)CopyImage(m_SmileyIcon, IMAGE_ICON, 0, 0, 0); 
}

void IconType::GetSize(SIZE& size)
{
	if (m_SmileyIcon != NULL)
	{
		ICONINFO ii;
		BITMAP bm;
		GetIconInfo(m_SmileyIcon, &ii);
		GetObject(ii.hbmColor, sizeof(bm), &bm);
		size.cx = bm.bmWidth;
		size.cy = bm.bmHeight;
		DeleteObject(ii.hbmMask);
		DeleteObject(ii.hbmColor);
	}
}



ImageListItemType::ImageListItemType(const unsigned id, HIMAGELIST hImList, int index)
	: ImageBase(id)
{
	m_index = index;
	m_hImList = hImList;
}

void ImageListItemType::DrawInternal(HDC hdc, int x, int y, int sizeX, int sizeY) 
{
	SIZE iSize;
	GetSize(iSize);

	if (sizeX >= iSize.cx && sizeY >= iSize.cy)
		ImageList_Draw(m_hImList, m_index, hdc, x, y, ILD_TRANSPARENT);
	else
	{
		HICON hIcon = ImageList_GetIconFixed(m_hImList, m_index, ILD_TRANSPARENT);
		DrawIconEx(hdc, x, y, hIcon, sizeX, sizeY, 0, NULL, DI_NORMAL);
		DestroyIcon(hIcon);
	}
}

HICON ImageListItemType::GetIcon(void) 
{ 
	return ImageList_GetIconFixed(m_hImList, m_index, ILD_TRANSPARENT);
}

void ImageListItemType::GetSize(SIZE& size)
{
	ImageList_GetIconSize(m_hImList, (int*)&size.cx, (int*)&size.cy);
}

ImageType::ImageType(const unsigned id, const bkstring& file, IStream* pStream)
	: ImageBase(id)
{
	m_bmp           = NULL;
	m_pPropertyItem = NULL;
	m_nCurrentFrame = 0;
	m_nFrameCount = 0;

	if (!InitGdiPlus()) return;

	if (pStream)
		m_bmp = new Gdiplus::Bitmap(pStream);
	else
		m_bmp = new Gdiplus::Bitmap(T2W_SM(file.c_str()));

	if (m_bmp->GetLastStatus() != Gdiplus::Ok)
	{
		delete m_bmp;
		m_bmp = NULL;
		return;
	}

	GUID pageGuid = Gdiplus::FrameDimensionTime;
	m_nFrameCount = m_bmp->GetFrameCount(&pageGuid);

	if (IsAnimated())
	{
		int nSize = m_bmp->GetPropertyItemSize(PropertyTagFrameDelay);
		m_pPropertyItem = (Gdiplus::PropertyItem*) new char[nSize];
		m_bmp->GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);
	}
}

ImageType::ImageType(const unsigned id, const bkstring& file, const int index, const IcoTypeEnum type)
	: ImageBase(id)
{
	m_bmp           = NULL;
	m_pPropertyItem = NULL;
	m_nCurrentFrame = 0;
	m_nFrameCount = 0;

	if (!InitGdiPlus()) return;

	switch (type)
	{
	case icoDll:
		{
			const HMODULE hModule = LoadDll(file);
			if (hModule != NULL) 
			{
				HICON hIcon = (HICON) LoadImage(hModule, MAKEINTRESOURCE(-index), IMAGE_ICON, 0, 0, 0);
				m_bmp = new Gdiplus::Bitmap(hIcon);
				DestroyIcon(hIcon);
			}
		}
		break;

	case icoFile:
		m_bmp = new Gdiplus::Bitmap(T2W_SM(file.c_str()));
		break;

	default:
		HICON hIcon = NULL;
		ExtractIconEx(file.c_str(), index, NULL, &hIcon, 1);
		m_bmp = new Gdiplus::Bitmap(hIcon);
		DestroyIcon(hIcon);
		break;
	}

	if (m_bmp->GetLastStatus() != Gdiplus::Ok)
	{
		delete m_bmp;
		m_bmp = NULL;
		return;
	}
}


ImageType::~ImageType(void)
{
	if (m_pPropertyItem) delete[] m_pPropertyItem;
	if (m_bmp) delete m_bmp;
}

void ImageType::SelectFrame(int frame)
{
	if ((unsigned)frame >= (unsigned)m_nFrameCount) frame = 0;
	if (IsAnimated() && frame != m_nCurrentFrame)
	{
		m_nCurrentFrame = frame;
		GUID pageGuid = Gdiplus::FrameDimensionTime;
		m_bmp->SelectActiveFrame(&pageGuid, frame);
	}
}


void ImageType::DrawInternal(HDC hdc, int x, int y, int sizeX, int sizeY)
{
	if (m_bmp == NULL) return;

	WaitForSingleObject(g_hMutexIm, 3000);
	{
		Gdiplus::Graphics grp(hdc);
//		if (opt.HQScaling) grp.SetInterpolationMode(Gdiplus::InterpolationModeBicubic);
		grp.DrawImage(m_bmp, x, y, sizeX, sizeY);
	}
	ReleaseMutex(g_hMutexIm);
}

int  ImageType::GetFrameDelay(void) const
{ 
	return ((long*) m_pPropertyItem->value)[m_nCurrentFrame]; 
}

HICON ImageType::GetIcon(void) 
{
	if (m_bmp == NULL) return NULL;

	HICON hIcon = NULL;
	WaitForSingleObject(g_hMutexIm, 3000);

	m_bmp->GetHICON(&hIcon);

	ReleaseMutex(g_hMutexIm);
	return hIcon;
}


void ImageType::GetSize(SIZE& size)
{
	if (m_bmp)
	{
		size.cx = m_bmp->GetWidth();
		size.cy = m_bmp->GetHeight();
	}
	else
	{
		size.cx = 0;
		size.cy = 0;
	}
}


ImageFType::ImageFType(const unsigned id)
	: ImageBase(id)
{
	m_bmp = NULL;
}

ImageFType::ImageFType(const unsigned id, const bkstring& file)
	: ImageBase(id)
{
	m_bmp = NULL;

	FREE_IMAGE_FORMAT fif = fei->FI_GetFileTypeT(file.c_str(), 0);
	if (fif == FIF_UNKNOWN)
		fif = fei->FI_GetFIFFromFilenameT(file.c_str());
	if (fif == FIF_UNKNOWN) return;

	FIBITMAP *dib = fei->FI_LoadT(fif, file.c_str(), 0);
	if (dib == NULL) return;

	bool transp = fei->FI_IsTransparent(dib) != 0;
	FREE_IMAGE_TYPE imt = fei->FI_GetImageType(dib);
	unsigned bpp = fei->FI_GetBPP(dib);

	if (transp && bpp != 32 || imt == FIT_RGBA16)  
	{
		FIBITMAP *tdib = fei->FI_ConvertTo32Bits(dib);
		fei->FI_Unload(dib);
		dib = tdib;
	}
	else if (!transp && bpp > 24)
	{
		FIBITMAP *tdib = fei->FI_ConvertTo24Bits(dib);
		fei->FI_Unload(dib);
		dib = tdib;
	}

	m_bmp = fei->FI_CreateHBITMAPFromDIB(dib);
	fei->FI_Unload(dib);

	if (transp)
		fei->FI_Premultiply(m_bmp);
}

ImageFType::~ImageFType()
{
	DeleteObject(m_bmp);
}

void ImageFType::DrawInternal(HDC hdc, int x, int y, int sizeX, int sizeY)
{
	if (m_bmp == NULL) return;

	HDC hdcImg = CreateCompatibleDC(hdc);
	HBITMAP oldBmp = (HBITMAP) SelectObject(hdcImg, m_bmp);

	BITMAP bm;
	GetObject(m_bmp, sizeof(bm), &bm);

	if (bm.bmBitsPixel == 32 && pAlphaBlend)
	{
		BLENDFUNCTION bf = {0};
		bf.SourceConstantAlpha = 255;
		bf.AlphaFormat = AC_SRC_ALPHA;
		pAlphaBlend(hdc, x, y, sizeX, sizeY, hdcImg, 0, 0, bm.bmWidth, bm.bmHeight, bf);
	}
	else
	{
		BitBlt(hdc, x, y, sizeX, sizeY, hdcImg, 0, 0, SRCCOPY);
	}

	SelectObject(hdcImg, oldBmp);
	DeleteDC(hdcImg);
}

HICON ImageFType::GetIcon(void)
{
	if (m_bmp == NULL) return NULL;

	HICON hIcon;

	BITMAP bm;
	GetObject(m_bmp, sizeof(bm), &bm);

	ICONINFO ii;
	ii.fIcon = TRUE;
	ii.xHotspot = 0;
	ii.yHotspot = 0;

	if (bm.bmBitsPixel == 32 && GetWinVer() < 0x0501)
	{
		int slen = bm.bmWidth * 4;
		int len  = bm.bmHeight * slen;

		BYTE* p         = (BYTE*)mir_alloc(len);
		BYTE* maskBits  = (BYTE*)mir_calloc(len);
		BYTE* colorBits = (BYTE*)mir_calloc(len);

		GetBitmapBits(m_bmp, len, p);

		for (int y = 0; y < bm.bmHeight; ++y) 
		{
			int shift = y * slen;
			BYTE *px = p + shift;
			BYTE *color = colorBits + shift;
			BYTE *mask = maskBits + shift;

			for (int x = 0; x < bm.bmWidth; ++x) 
			{
				for(int i = 0; i < 4; i++)
				{
					mask[i] = px[3];
					color[i] = px[i];
				}

				px += 4;
				mask += 4;
				color += 4;
			}
		}

		ii.hbmMask  = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 32, maskBits);
		ii.hbmColor = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 32, colorBits);

		hIcon = CreateIconIndirect(&ii);

		DeleteObject(ii.hbmMask);
		DeleteObject(ii.hbmColor);

		mir_free(p);
		mir_free(maskBits);
		mir_free(colorBits);
	}
	else
	{
		ii.hbmMask  = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
		ii.hbmColor = m_bmp;
		hIcon = CreateIconIndirect(&ii);
		DeleteObject(ii.hbmMask);
	}
	return hIcon;
}

void ImageFType::GetSize(SIZE& size)
{
	if (m_bmp)
	{
		BITMAP bm;
		GetObject(m_bmp, sizeof(bm), &bm);
		size.cx = bm.bmWidth;
		size.cy = bm.bmHeight;
	}
	else
	{
		size.cx = 0;
		size.cy = 0;
	}
}
/*
ImageFAniType::ImageFAniType(const unsigned id, const bkstring& file)
: ImageFType(id)
{
	m_fmbmp = NULL;
	m_nCurrentFrame = -1;
	m_FrameDelay = NULL;

	FREE_IMAGE_FORMAT fif = fei->FI_GetFileTypeT(file.c_str(), 0);
	if (fif == FIF_UNKNOWN)
		fif = fei->FI_GetFIFFromFilenameT(file.c_str());

	m_fmbmp = fei->FI_OpenMultiBitmap(fif, T2A_SM(file.c_str()), FALSE, TRUE, TRUE, GIF_PLAYBACK);
	if (m_fmbmp == NULL) return;

	m_nFrameCount = fei->FI_GetPageCount(m_fmbmp);
	m_bmpl = (HBITMAP*)mir_calloc(m_nFrameCount*sizeof(HBITMAP));
	m_FrameDelay = (int*)mir_calloc(m_nFrameCount*sizeof(int));
	SelectFrame(0);
}

ImageFAniType::~ImageFAniType()
{
	if (m_fmbmp) fei->FI_CloseMultiBitmap(m_fmbmp, 0);
	for (int i=0; i<m_nFrameCount; ++i)
		if (m_bmp) DeleteObject(m_bmpl[i]);

	mir_free(m_bmpl);
	mir_free(m_FrameDelay);
}

void ImageFAniType::SelectFrame(int frame)
{
	if ((unsigned)frame >= (unsigned)m_nFrameCount) frame = 0;
	if (frame == m_nCurrentFrame) return;
	m_nCurrentFrame = frame;

	if (m_bmpl[frame]) 
	{
		m_bmp = m_bmpl[frame];
		return;
	}

	FITAG *tag = NULL;

	FIBITMAP *dib = fei->FI_LockPage(m_fmbmp, frame);
	if (dib == NULL)
		return;

	if (fei->FI_GetMetadata(FIMD_ANIMATION, dib, "FrameTime", &tag))
		m_FrameDelay[frame] = *(LONG *)fei->FI_GetTagValue(tag) / 10;

	m_bmpl[frame] = m_bmp = fei->FI_CreateHBITMAPFromDIB(dib);

	if (fei->FI_IsTransparent(dib))
		fei->FI_Premultiply(m_bmp);

	fei->FI_UnlockPage(m_fmbmp, dib, FALSE);
}
*/

#pragma optimize("gt", on)

// MurmurHash2, by Austin Appleby
unsigned int __fastcall hash( const void * key, unsigned int len )
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.
	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value
	unsigned int h = len;

	// Mix 4 bytes at a time into the hash
	const unsigned char * data = (const unsigned char *)key;

	while(len >= 4)
	{
		unsigned int k = *(unsigned int *)data;

		k *= m; 
		k ^= k >> r; 
		k *= m; 

		h *= m; 
		h ^= k;

		data += 4;
		len -= 4;
	}

	// Handle the last few bytes of the input array
	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
		h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
} 
#pragma optimize("", on)


void InitImageCache(void)
{
	g_hMutexIm = CreateMutex(NULL, FALSE, NULL);
	CallService(MS_IMG_GETINTERFACE, FI_IF_VERSION, (LPARAM) &fei);

	pAlphaBlend = (tAlphaBlend) GetProcAddress(GetModuleHandleA("gdi32"), "GdiAlphaBlend");
	if (pAlphaBlend == NULL)
		pAlphaBlend = (tAlphaBlend) GetProcAddress(LoadLibraryA("msimg32"), "AlphaBlend");
}

void DestroyImageCache(void)
{
	WaitForSingleObject(g_hMutexIm, 3000);

	if (timerId) KillTimer(NULL, timerId);
	if (lastmodule) FreeLibrary(lastmodule);

	g_imagecache.destroy();

	ReleaseMutex(g_hMutexIm);
	CloseHandle(g_hMutexIm);
}


ImageBase* AddCacheImage(const bkstring& file, int index)
{
	bkstring tmpfile(file); tmpfile.appendfmt(_T("#%d"), index);
	unsigned id = hash(tmpfile.c_str(), (unsigned int)tmpfile.size() * sizeof(TCHAR));

	WaitForSingleObject(g_hMutexIm, 3000);

	ImageBase srch(id);
	ImageBase *img = g_imagecache.find(&srch);
	if (img == NULL)
	{
		bkstring::size_type ind = file.find_last_of('.');
		if (ind == file.npos) return NULL;
		bkstring ext = file.substr(ind+1);

		if (ext.comparei(_T("dll")) == 0 || ext.comparei(_T("exe")) == 0)
			img = opt.HQScaling ? (ImageBase*)new ImageType(id, file, index, icoDll) : (ImageBase*)new IconType(id, file, index, icoDll);
		else if (ext.comparei(_T("ico")) == 0)
			img = opt.HQScaling ? (ImageBase*)new ImageType(id, file, 0, icoFile) : (ImageBase*)new IconType(id, file, 0, icoFile);
		else if (ext.comparei(_T("icl")) == 0)
			img = opt.HQScaling ? (ImageBase*)new ImageType(id, file, index, icoIcl) : (ImageBase*)new IconType(id, file, index, icoIcl);
		else if (ext.comparei(_T("gif")) == 0)
			img = new ImageType(id, file, NULL);
		else if (fei == NULL || ext.comparei(_T("tif")) == 0 || ext.comparei(_T("tiff")) == 0)
			img = new ImageType(id, file, NULL);
		else
			img = opt.HQScaling ? (ImageBase*)new ImageType(id, file, NULL) : (ImageBase*)new ImageFType(id, file);

		g_imagecache.insert(img);

		if (timerId == 0) 
		{
			timerId = 0xffffffff;
			CallFunctionAsync(sttMainThreadCallback, NULL);
		}
	}
	else
		img->AddRef();

	ReleaseMutex(g_hMutexIm);

	return img;
}
