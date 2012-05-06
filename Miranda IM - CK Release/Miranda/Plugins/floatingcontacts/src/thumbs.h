/*
FltContacts plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Authors
Copyright (C) Ranger (fonts and colors settings)
Copyright (C) Iavor Vajarov ( ivajarov@code.bg )


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

* This file is a modification of clcfiledrop.h originally  written by Richard Hughes.
*/

#define	USERNAME_LEN				50
class CDropTarget;

typedef struct _DockOpt
{
	HWND	hwndLeft;
	HWND	hwndRight;
}
DockOpt;

struct ThumbInfo
{
public:				// TODO: make it private
	HWND			hwnd;
	TCHAR			ptszName[ USERNAME_LEN ];
	HANDLE			hContact;
	int				iIcon;
	CDropTarget *	dropTarget;
	DockOpt			dockOpt;
	BOOL			fTipActive;
	BOOL			fTipTimerActive;
	POINT			ptTipSt;

	BYTE			btAlpha;
	MyBitmap		bmpContent;

	POINT			ptPos;
	SIZE			szSize;

public:
	ThumbInfo();
	~ThumbInfo();

	void GetThumbRect			(RECT *rc);
	void PositionThumb			(short nX, short nY); 
	void PositionThumbWorker	(short nX, short nY, POINT *rcNewPos); 
	void ResizeThumb			();
	void RefreshContactIcon		(int iIcon);
	void RefreshContactStatus	(int idStatus);
	void DeleteContactPos		();
	void OnLButtonDown			(short nX, short nY);
	void OnLButtonUp			();
	void OnMouseMove			(short nX, short nY, WPARAM wParam);
	void ThumbSelect			(BOOL bMouse);
	void ThumbDeselect			(BOOL bMouse);
	void SetThumbOpacity		(BYTE btAlpha);
	void KillTip				();
	void UpdateContent			();
	void PopUpMessageDialog		();
	void OnTimer				(BYTE idTimer);
};

void UndockThumbs	( ThumbInfo *pThumb1, ThumbInfo *pThumb2 );
void DockThumbs		( ThumbInfo *pThumbLeft, ThumbInfo *pThumbRight, BOOL bMoveLeft );

class ThumbList: public LIST<ThumbInfo>
{
public:
	ThumbList();
	~ThumbList();

	ThumbInfo*	AddThumb			(HWND hwnd, TCHAR *ptszName, HANDLE hContact);
	void		RemoveThumb			(ThumbInfo *pThumb);

	ThumbInfo*	FindThumb			(HWND hwnd);
	ThumbInfo*	FindThumbByContact	(HANDLE hContact);

private:
	static int cmp(const ThumbInfo *p1, const ThumbInfo *p2);
};

extern ThumbList thumbList;
