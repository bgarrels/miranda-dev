/*
Popup Plus plugin for Miranda IM

Copyright	� 2002 Luca Santarelli,
			� 2004-2007 Victor Pavlychko
			� 2010 MPK
			� 2010 Merlin_de

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

File name      : $HeadURL: http://svn.miranda.im/mainrepo/popup/trunk/src/skin.cpp $
Revision       : $Revision: 1651 $
Last change on : $Date: 2010-07-15 20:31:06 +0300 (Чт, 15 июл 2010) $
Last change by : $Author: Merlin_de $

===============================================================================
*/

#include "headers.h"

#include <stdio.h>
#include <fstream>
#include <sstream>

// PopupSkin
PopupSkin::PopupSkin(LPCTSTR aName)
{
	for (int i = 0; i < 32; i++)
		m_flag_names[i] = NULL;
	m_elements = 0;
	m_name = aName ? mir_tstrdup(aName) : NULL;
}

PopupSkin::~PopupSkin()
{
	if (m_name) mir_free(m_name);
	for (int i = 0; i < 32; i++)
		mir_free(m_flag_names[i]);
	freeSkin(m_elements);
}

SIZE PopupSkin::measureAction(HDC hdc, POPUPACTION *act) const
{
	SIZE sz = {0};
	if (!(PopUpOptions.actions&ACT_ENABLE))
		return sz;

	if (PopUpOptions.actions&ACT_LARGE)
	{
		sz.cx = sz.cy = 32;
	} else
	{
		sz.cx = sz.cy = 16;
	}

	if (PopUpOptions.actions&ACT_TEXT)
	{
		char *name = strchr(act->lpzTitle, '/');
		if (!name) name = act->lpzTitle;
		else ++name;

		SIZE szText, szSpace;

#if defined(_UNICODE)
		LPWSTR wname = mir_a2u(name);
		WCHAR *str = TranslateW(wname);
		GetTextExtentPoint32W(hdc, str, lstrlenW(str), &szText);
		mir_free(wname);
		GetTextExtentPoint32W(hdc, L" ", 1, &szSpace);
#else
		if (g_popup.isOsUnicode && MyGetTextExtentPoint32W) {
			LPWSTR wname = mir_a2u(name);
			WCHAR *str = TranslateW(wname);
			MyGetTextExtentPoint32W(hdc, str, lstrlenW(str), &szText);
			mir_free(wname);
		}
		else {
			char *str = Translate(name);
			GetTextExtentPoint32A(hdc, str, lstrlenA(str), &szText);
		}
		GetTextExtentPoint32A(hdc, " ", 1, &szSpace);
#endif
		sz.cy = max(sz.cy, szText.cy);
		sz.cx += szSpace.cx;
		sz.cx += szText.cx;
		sz.cy += 2;
	} else
	{
		sz.cx += 4;
		sz.cy += 4;
	}

	return sz;
}

SIZE PopupSkin::measureActionBar(HDC hdc, PopupWnd2 *wnd) const
{
	SIZE sz = {0};
	HFONT hFntSave = (HFONT)SelectObject(hdc, fonts.action);
	for (int i = 0; i < wnd->getActionCount(); ++i)
	{
		SIZE szAction = measureAction(hdc, &wnd->getActions()[i].actionA);

		if (PopUpOptions.actions & ACT_TEXT)
		{
			sz.cx = max(sz.cx, szAction.cx);
			sz.cy += szAction.cy;
		} else
		{
			sz.cx += szAction.cx;
			sz.cy = max(sz.cy, szAction.cy);
		}
	}
	SelectObject(hdc, hFntSave);
	return sz;
}

void PopupSkin::drawAction(MyBitmap *bmp, POPUPACTION *act, int x, int y, bool hover) const
{
	if (!(PopUpOptions.actions&ACT_ENABLE))
		return;

	bmp->DrawIcon(act->lchIcon,
		(PopUpOptions.actions&ACT_TEXT) ? x : (x+2),
		y+2,
		(PopUpOptions.actions&ACT_LARGE) ? 32 : 16,
		(PopUpOptions.actions&ACT_LARGE) ? 32 : 16);

	if (PopUpOptions.actions&ACT_TEXT)
	{
		char *name = strchr(act->lpzTitle, '/');
		if (!name) name = act->lpzTitle;
		else ++name;

		SIZE szText, szSpace;

		HFONT hFntSave = (HFONT)SelectObject(bmp->getDC(), hover ? fonts.actionHover : fonts.action);
		SetTextColor(bmp->getDC(), hover ? fonts.clActionHover : fonts.clAction);
		SetBkMode(bmp->getDC(), TRANSPARENT);

		GetTextExtentPoint32A(bmp->getDC(), " ", 1, &szSpace);

#if defined(_UNICODE)
		LPWSTR wname = mir_a2u(name);
		WCHAR *str = TranslateW(wname);
		GetTextExtentPoint32W(bmp->getDC(), str, lstrlenW(str), &szText);
		bmp->Draw_TextW(str,
			(PopUpOptions.actions&ACT_LARGE) ? (x+szSpace.cx+32) : (x+szSpace.cx+16),
			max(
				y+2,
				y+2 + (((PopUpOptions.actions&ACT_LARGE) ? 32 : 16) - szText.cy)/2
			));
		mir_free(wname);
#else
		if (g_popup.isOsUnicode && MyGetTextExtentPoint32W)
		{
			LPWSTR wname = mir_a2u(name);
			WCHAR *str = TranslateW(wname);
			MyGetTextExtentPoint32W(bmp->getDC(), str, lstrlenW(str), &szText);
			bmp->Draw_TextW(str,
				(PopUpOptions.actions&ACT_LARGE) ? (x+szSpace.cx+32) : (x+szSpace.cx+16),
				max(
					y+2,
					y+2 + (((PopUpOptions.actions&ACT_LARGE) ? 32 : 16) - szText.cy)/2
				));
			mir_free(wname);
		}
		else {
			char *str = Translate(name);
			GetTextExtentPoint32A(bmp->getDC(), str, lstrlenA(str), &szText);
			bmp->Draw_TextA(str,
				(PopUpOptions.actions&ACT_LARGE) ? (x+szSpace.cx+32) : (x+szSpace.cx+16),
				max(
					y+2,
					y+2 + (((PopUpOptions.actions&ACT_LARGE) ? 32 : 16) - szText.cy)/2
				));
		}
#endif
		SelectObject(bmp->getDC(), hFntSave);
	}
	else {
		if (hover)
		{
			RECT rc;
			rc.left = x;
			rc.top = y;
			rc.right = x + ((PopUpOptions.actions&ACT_LARGE) ? 32 : 16) + 4;
			rc.bottom = y + ((PopUpOptions.actions&ACT_LARGE) ? 32 : 16) + 4;
			bmp->saveAlpha(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top);
			HBRUSH hbr = CreateSolidBrush(fonts.clActionHover);
			FrameRect(bmp->getDC(), &rc, hbr);
			DeleteObject(hbr);
			bmp->restoreAlpha(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top);
		}
	}
}

void PopupSkin::drawActionBar(MyBitmap *bmp, PopupWnd2 *wnd, int x, int y) const
{
	for (int i = 0; i < wnd->getActionCount(); ++i)
	{
		SIZE szAction = measureAction(bmp->getDC(), &wnd->getActions()[i].actionA);
		drawAction(bmp, &wnd->getActions()[i].actionA, x, y, wnd->getActions()[i].hover);

		wnd->getActions()[i].rc.left = x;
		wnd->getActions()[i].rc.top = y;
		wnd->getActions()[i].rc.right = x + szAction.cx;
		wnd->getActions()[i].rc.bottom = y + szAction.cy;

		if (PopUpOptions.actions & ACT_TEXT)
		{
			y += szAction.cy;
		} else
		{
			x += szAction.cx;
		}
	}
}

void PopupSkin::measure(HDC hdc, PopupWnd2 *wnd, int maxw, POPUPOPTIONS *options) const
{
	if (!m_elements) return;

	SKINELEMENT *head = NULL;

	int STYLE_SZ_CLOCK = 0;
	if (!this->useInternalClock())
		for (head = m_elements; head; head = head->next)
			if ((head->type&ST_TYPEMASK) == ST_CLOCK)
				break;
	if (head && head->myBmp)
	{//layerd clock
		SIZE szNew;
		szNew.cx = head->clocksize[CLOCK_LEFT]+head->clocksize[CLOCK_RIGHT];
		szNew.cy = head->myBmp->getHeight();
		for (char *p = wnd->getTime(); *p; p++)
		{
			if (*p == ':')
			{
				szNew.cx += head->clocksize[CLOCK_SEPARATOR];
			} else
			if ((*p >= '0') && (*p <= '9'))
			{
				szNew.cx += head->clocksize[CLOCK_DIGITS+*p-'0'];
			}
		}
		wnd->getArgs()->add("clock.width",szNew.cx);
		wnd->getArgs()->add("clock.height",szNew.cy);
		STYLE_SZ_CLOCK = szNew.cx;
	} else
	{//normal clock
		HFONT hfnSave = (HFONT)SelectObject(hdc, fonts.clock);
		SIZE sz; GetTextExtentPoint32A(hdc, wnd->getTime(), lstrlenA(wnd->getTime()), &sz);
		SelectObject(hdc, hfnSave);
		wnd->getArgs()->add("clock.width",sz.cx+2*STYLE_SZ_GAP);
		wnd->getArgs()->add("clock.height",sz.cy);
		STYLE_SZ_CLOCK = sz.cx+2*STYLE_SZ_GAP;
	}

	wnd->getArgs()->clear();
	wnd->getArgs()->add("options.avatarsize", PopUpOptions.avatarSize);
	wnd->getArgs()->add("window.width", maxw);
	wnd->getArgs()->add("window.maxwidth", maxw);

	for (int i = 0; i < 32; ++i)
	{
		char buf[10];
		wsprintfA(buf, "opt%d", i);
		wnd->getArgs()->add(buf, (m_flags&(1L<<i)) ? 1 : 0);
	}

	SIZE sz;
	sz.cx = sz.cy = 0;
	head = m_elements;
	while (head)
	{
		if ((m_flags & head->flag_mask) != (head->flags & head->flag_mask))
		{
			head = head->next;
			continue;
		}

		SIZE szNew = {0,0};
		switch (head->type & ST_TYPEMASK)
		{
		case ST_TEXT:
		{
			int tmp = head->fw.eval(wnd->getArgs());
			// this is used to measure and layout text
			wnd->getRenderInfo()->textw = tmp ? tmp : (maxw - head->fx.eval(wnd->getArgs()));
			szNew.cx = wnd->getRenderInfo()->textw;
			if (wnd->isTextEmpty())
			{
				szNew.cx = szNew.cy = 0;
			} else
			{
				HFONT hFntSave = (HFONT)SelectObject(hdc, fonts.text);
				switch (wnd->getTextType())
				{
					case PopupWnd2::TT_ANSI:
					{
						RECT rc; SetRect(&rc, 0, 0, szNew.cx, 0);
						DrawTextExA(hdc, wnd->getTextA(), lstrlenA(wnd->getTextA()), &rc,
							DT_CALCRECT|DT_EXPANDTABS|DT_LEFT|DT_NOPREFIX|DT_TOP|DT_WORDBREAK/*|DT_RTLREADING*/, NULL);
						szNew.cx = rc.right;
						szNew.cy = rc.bottom;
						break;
					}
					case PopupWnd2::TT_UNICODE:
					{
#if defined(_UNICODE)
						RECT rc; SetRect(&rc, 0, 0, szNew.cx, 0);
						DrawTextExW(hdc, wnd->getTextW(), lstrlenW(wnd->getTextW()), &rc,
							DT_CALCRECT|DT_EXPANDTABS|DT_LEFT|DT_NOPREFIX|DT_TOP|DT_WORDBREAK/*|DT_RTLREADING*/, NULL);
						szNew.cx = rc.right;
						szNew.cy = rc.bottom;
#else
						if (g_popup.isOsUnicode && MyDrawTextExW)
						{
							RECT rc; SetRect(&rc, 0, 0, szNew.cx, 0);
							MyDrawTextExW(hdc, wnd->getTextW(), lstrlenW(wnd->getTextW()), &rc,
								DT_CALCRECT|DT_EXPANDTABS|DT_LEFT|DT_NOPREFIX|DT_TOP|DT_WORDBREAK/*|DT_RTLREADING*/, NULL);
							szNew.cx = rc.right;
							szNew.cy = rc.bottom;
						}
#endif
						break;
					}
					case PopupWnd2::TT_MTEXT:
					{
						MText.Measure(hdc, &szNew, wnd->getTextM());
						break;
					}
				}
				SelectObject(hdc, hFntSave);
			}

			wnd->getRenderInfo()->texth = szNew.cy;

			SIZE szActions = measureActionBar(hdc, wnd);
			wnd->getRenderInfo()->actw = szActions.cx;
			if (szActions.cy)
			{
				szNew.cx = max(szNew.cx, szActions.cx);
				szNew.cy += szActions.cy;
				szNew.cy += 3;
			}

			wnd->getRenderInfo()->realtextw = szNew.cx;

			if (szNew.cx > maxw - head->fx.eval(wnd->getArgs()))
				szNew.cx = maxw - head->fx.eval(wnd->getArgs());
			wnd->getArgs()->add("text.width", szNew.cx);
			wnd->getArgs()->add("text.height", szNew.cy);

			break;
		}

		case ST_TITLE:
		{
			int tmp = head->fw.eval(wnd->getArgs());
			// this is used to measure and layout text
			wnd->getRenderInfo()->titlew = tmp ? tmp : (maxw - head->fx.eval(wnd->getArgs()) - STYLE_SZ_CLOCK);
			szNew.cx = wnd->getRenderInfo()->titlew;
			HFONT hFntSave = (HFONT)SelectObject(hdc, fonts.title);
			switch (wnd->getTextType())
			{
				case PopupWnd2::TT_ANSI:
				{
					RECT rc; SetRect(&rc, 0, 0, szNew.cx, 0);
					DrawTextExA(hdc, wnd->getTitleA(), lstrlenA(wnd->getTitleA()), &rc,
						DT_CALCRECT|DT_EXPANDTABS|DT_LEFT|DT_NOPREFIX|DT_TOP|DT_WORDBREAK/*|DT_RTLREADING*/, NULL);
					szNew.cx = rc.right;
					szNew.cy = rc.bottom;
					break;
				}
				case PopupWnd2::TT_UNICODE:
				{
#if defined(_UNICODE)
					RECT rc; SetRect(&rc, 0, 0, szNew.cx, 0);
					DrawTextExW(hdc, wnd->getTitleW(), lstrlenW(wnd->getTitleW()), &rc,
						DT_CALCRECT|DT_EXPANDTABS|DT_LEFT|DT_NOPREFIX|DT_TOP|DT_WORDBREAK/*|DT_RTLREADING*/, NULL);
					szNew.cx = rc.right;
					szNew.cy = rc.bottom;
#else
					if (g_popup.isOsUnicode && MyDrawTextExW)
					{
						RECT rc; SetRect(&rc, 0, 0, szNew.cx, 0);
						MyDrawTextExW(hdc, wnd->getTitleW(), lstrlenW(wnd->getTitleW()), &rc,
							DT_CALCRECT|DT_EXPANDTABS|DT_LEFT|DT_NOPREFIX|DT_TOP|DT_WORDBREAK/*|DT_RTLREADING*/, NULL);
						szNew.cx = rc.right;
						szNew.cy = rc.bottom;
					}
#endif
					break;
				}
				case PopupWnd2::TT_MTEXT:
				{
					MText.Measure(hdc, &szNew, wnd->getTitleM());
					break;
				}
			}
			SelectObject(hdc, hFntSave);
			if (szNew.cx > maxw - head->fx.eval(wnd->getArgs()) - STYLE_SZ_CLOCK)
				szNew.cx = maxw - head->fx.eval(wnd->getArgs()) - STYLE_SZ_CLOCK;
			szNew.cx += STYLE_SZ_CLOCK;
			wnd->getArgs()->add("title.width", szNew.cx);
			wnd->getArgs()->add("title.height", szNew.cy);
			break;
		}

		case ST_ICON:
			szNew.cx = szNew.cy = 16;
			wnd->getArgs()->add("icon.width",16);
			wnd->getArgs()->add("icon.height",16);
			break;

		case ST_BITMAP:
//			szNew.cx = szNew.cy = 0;
			GetBmpSize(head->hbm, &szNew);
			break;

		case ST_MYBITMAP:
			szNew.cx = head->myBmp->getWidth();
			szNew.cy = head->myBmp->getHeight();
			break;

		case ST_AVATAR:
			wnd->getRenderInfo()->hasAvatar = false;
			szNew.cx = szNew.cy = 0;
			if (wnd->getAvatar())
			{
				szNew.cx = wnd->getAvatar()->getWidth();
				szNew.cy = wnd->getAvatar()->getHeight();
				wnd->getArgs()->add("avatarbitmap.width",max(1,szNew.cx));
				wnd->getArgs()->add("avatarbitmap.height",max(1,szNew.cy));
				wnd->getRenderInfo()->hasAvatar = true;
			}
			break;

		case ST_CLOCK:
			if (head && head->myBmp)
			{
				szNew.cx = head->clocksize[CLOCK_LEFT]+head->clocksize[CLOCK_RIGHT];
				szNew.cy = head->myBmp->getHeight();
				for (char *p = wnd->getTime(); *p; p++)
				{
					if (*p == ':')
					{
						szNew.cx += head->clocksize[CLOCK_SEPARATOR];
					} else
					if ((*p >= '0') && (*p <= '9'))
					{
						szNew.cx += head->clocksize[CLOCK_DIGITS+*p-'0'];
					}
				}
				wnd->getArgs()->add("clock.width",szNew.cx);
				wnd->getArgs()->add("clock.height",szNew.cy);
			} else
			{
				HFONT hfnSave = (HFONT)SelectObject(hdc, fonts.clock);
				SIZE sz; GetTextExtentPoint32A(hdc, wnd->getTime(), lstrlenA(wnd->getTime()), &sz);
				SelectObject(hdc, hfnSave);
				wnd->getArgs()->add("clock.width",sz.cx+2*STYLE_SZ_GAP);
				wnd->getArgs()->add("clock.height",sz.cy);
			}
			break;
		}

		if (head->proportional && szNew.cx && szNew.cy && (((head->fw.eval(wnd->getArgs()) > 0) && !head->fh.eval(wnd->getArgs())) || (!head->fw.eval(wnd->getArgs()) && (head->fh.eval(wnd->getArgs()) > 0))))
		{
			if (head->fw.eval(wnd->getArgs()))
			{
				szNew.cy = szNew.cy * head->fw.eval(wnd->getArgs()) / szNew.cx;
				szNew.cx = head->fw.eval(wnd->getArgs());
			} else
			{
				szNew.cx = szNew.cx * head->fh.eval(wnd->getArgs()) / szNew.cy;
				szNew.cy = head->fh.eval(wnd->getArgs());
//				szNew.cx = head->fx.eval(wnd->getArgs()) + szNew.cx * head->fh.eval(wnd->getArgs()) / szNew.cy;
//				szNew.cy = head->fy.eval(wnd->getArgs()) + head->fh.eval(wnd->getArgs());
			}
		}

		if ((head->type & ST_TYPEMASK) == ST_AVATAR)
		{
			wnd->getArgs()->add("avatar.width",szNew.cx);
			wnd->getArgs()->add("avatar.height",szNew.cy);

			szNew.cy += 2;
		}

		if (!head->fw.eval(wnd->getArgs()))
			szNew.cx += head->fx.eval(wnd->getArgs());
		else
		if (head->fw.eval(wnd->getArgs()) > 0)
			szNew.cx = head->fx.eval(wnd->getArgs()) + head->fw.eval(wnd->getArgs());
		else
			szNew.cx = 0;

		if (!head->fh.eval(wnd->getArgs()))
			szNew.cy += head->fy.eval(wnd->getArgs());
		else
		if (head->fh.eval(wnd->getArgs()) > 0)
			szNew.cy = head->fy.eval(wnd->getArgs()) + head->fh.eval(wnd->getArgs());
		else
			szNew.cy = 0;

		if (head->fx.eval(wnd->getArgs()) >= 0 && szNew.cx > sz.cx) sz.cx = szNew.cx;
		if (head->fy.eval(wnd->getArgs()) >= 0 && szNew.cy > sz.cy) sz.cy = szNew.cy;

		head = head->next;
	}
//	if (pw->title_height > 20)
//		sz.cy += pw->title_height - 20;

	sz.cx += m_right_gap;
	sz.cy += m_bottom_gap;

	int tmp;

	if (tmp = m_fw.eval(wnd->getArgs())) sz.cx = tmp;
	if (tmp = m_fh.eval(wnd->getArgs())) sz.cy = tmp;

	if (options->DynamicResize)
	{
		if (options->UseMinimumWidth && sz.cx < options->MinimumWidth)
			sz.cx = options->MinimumWidth;
	} else
	{
		sz.cx = options->MaximumWidth;
	}

	wnd->getArgs()->add("window.width",sz.cx);
	wnd->getArgs()->add("window.height",sz.cy);
	wnd->setSize(sz);
}

void PopupSkin::display(MyBitmap *bmp, PopupWnd2 *wnd, int maxw, POPUPOPTIONS *options, DWORD drawFlags) const
{
	if (!m_elements) return;

	HDC hdc = bmp->getDC();
	SKINELEMENT *head = NULL;

	int STYLE_SZ_CLOCK = 0;
	if (!this->useInternalClock())
		for (head = m_elements; head; head = head->next)
			if ((head->type&ST_TYPEMASK) == ST_CLOCK)
				break;
	if (head && head->myBmp)
	{
		SIZE szNew;
		szNew.cx = head->clocksize[CLOCK_LEFT]+head->clocksize[CLOCK_RIGHT];
		szNew.cy = head->myBmp->getHeight();
		for (char *p = wnd->getTime(); *p; p++)
		{
			if (*p == ':')
			{
				szNew.cx += head->clocksize[CLOCK_SEPARATOR];
			} else
			if ((*p >= '0') && (*p <= '9'))
			{
				szNew.cx += head->clocksize[CLOCK_DIGITS+*p-'0'];
			}
		}
		STYLE_SZ_CLOCK = szNew.cx;
	} else
	{
		HFONT hfnSave = (HFONT)SelectObject(hdc, fonts.clock);
		SIZE sz; GetTextExtentPoint32A(hdc, wnd->getTime(), lstrlenA(wnd->getTime()), &sz);
		SelectObject(hdc, hfnSave);
		STYLE_SZ_CLOCK = sz.cx + 2*STYLE_SZ_GAP;
	}

	
	head = m_elements;

	SIZE sz;
	SIZE szWnd = wnd->getSize();
	SIZE pos;
	bool actionsRendered = false;
	while (head)
	{
		if ((head->type&ST_TYPEMASK) == ST_AVATAR)
		{
			if (wnd->getAvatar()->isAnimated() && !(drawFlags&DF_ANIMATE))
			{
				head = head->next;
				continue;
			}
			if (!wnd->getAvatar()->isAnimated() && !(drawFlags&DF_STATIC))
			{
				head = head->next;
				continue;
			}
		} else
		if ((head->type&ST_TYPEMASK) == ST_TEXT)
		{
			if (!wnd->getActionCount() && !(drawFlags&DF_STATIC))
			{
				head = head->next;
				continue;
			}
		} else
		if (!(drawFlags&DF_STATIC))
		{
			head = head->next;
			continue;
		}

		if ((head->type&ST_BADPOS) || ((m_flags & head->flag_mask) != (head->flags & head->flag_mask)))
		{
			head = head->next;
			continue;
		}

		int hShift = 0;
		switch (head->type & ST_TYPEMASK)
		{
		case ST_TEXT:
		{
			if (head->textColor != (COLORREF)0xffffffff)
				SetTextColor(hdc, head->textColor);
			else
				SetTextColor(hdc, wnd->getTextColor());

			POINT pos;
			pos.x = head->fx.eval(wnd->getArgs());
			pos.y = head->fy.eval(wnd->getArgs());
			SIZE sz;
			sz.cx = wnd->getRenderInfo()->textw;
			sz.cy = 1000;

			SetRect(&wnd->getRenderInfo()->textRect, pos.x, pos.y,
				pos.x + wnd->getRenderInfo()->realtextw,
				pos.y + wnd->getRenderInfo()->texth);

			if ((drawFlags&DF_STATIC) && !wnd->isTextEmpty())
			{
				HFONT hFntSave = (HFONT)SelectObject(hdc, fonts.text);
				bmp->saveAlpha();
				switch (wnd->getTextType())
				{
					case PopupWnd2::TT_ANSI:
					{
						RECT rc; SetRect(&rc, pos.x, pos.y, pos.x+sz.cx, pos.y+sz.cy);
						DrawTextExA(hdc, wnd->getTextA(), lstrlenA(wnd->getTextA()), &rc,
							DT_EXPANDTABS|DT_LEFT|DT_NOPREFIX|DT_TOP|DT_WORDBREAK/*|DT_RTLREADING*/, NULL);
						break;
					}
					case PopupWnd2::TT_UNICODE:
					{
#if defined(_UNICODE)
						RECT rc; SetRect(&rc, pos.x, pos.y, pos.x+sz.cx, pos.y+sz.cy);
						DrawTextExW(hdc, wnd->getTextW(), lstrlenW(wnd->getTextW()), &rc,
							DT_EXPANDTABS|DT_LEFT|DT_NOPREFIX|DT_TOP|DT_WORDBREAK/*|DT_RTLREADING*/, NULL);
#else
						if (g_popup.isOsUnicode && MyDrawTextExW)
						{
							RECT rc; SetRect(&rc, pos.x, pos.y, pos.x+sz.cx, pos.y+sz.cy);
							MyDrawTextExW(hdc, wnd->getTextW(), lstrlenW(wnd->getTextW()), &rc,
								DT_EXPANDTABS|DT_LEFT|DT_NOPREFIX|DT_TOP|DT_WORDBREAK/*|DT_RTLREADING*/, NULL);
						}
#endif
						break;
					}
					case PopupWnd2::TT_MTEXT:
					{
						MText.Display(hdc, pos, sz, wnd->getTextM()); 
						break;
					}
				}
				SelectObject(hdc, hFntSave);
				bmp->restoreAlpha();
			}

			if (!actionsRendered && (drawFlags&DF_ANIMATE))
			{
				int textAreaWidth = head->fw.eval(wnd->getArgs());
				if (textAreaWidth <= 0) textAreaWidth = wnd->getRenderInfo()->realtextw;

				drawActionBar(bmp, wnd,
					DBGetContactSettingByte(NULL, MODULNAME, "CenterActions", 0) ?
						(pos.x + (textAreaWidth - wnd->getRenderInfo()->actw)/2) :
					(PopUpOptions.actions&ACT_RIGHTICONS) ?
						(pos.x + textAreaWidth - wnd->getRenderInfo()->actw) :
					// else
						pos.x,
					pos.y + wnd->getRenderInfo()->texth + 3);

				actionsRendered = true;
			}


			break;
		}

		case ST_TITLE:
		{
			if (head->textColor != (COLORREF)0xffffffff)
				SetTextColor(hdc, head->textColor);
			else
				SetTextColor(hdc, wnd->getTitleColor());

			bmp->saveAlpha();
			POINT pos;
			pos.x = head->fx.eval(wnd->getArgs());
			pos.y = head->fy.eval(wnd->getArgs());
			SIZE sz;
			sz.cx = wnd->getRenderInfo()->titlew;
			sz.cy = 1000;

			switch (wnd->getTextType())
			{
				case PopupWnd2::TT_ANSI:
				{
					HFONT hFntSave = (HFONT)SelectObject(hdc, fonts.title);
					RECT rc; SetRect(&rc, pos.x, pos.y, pos.x+sz.cx, pos.y+sz.cy);
					DrawTextExA(hdc, wnd->getTitleA(), lstrlenA(wnd->getTitleA()), &rc,
						DT_EXPANDTABS|DT_LEFT|DT_NOPREFIX|DT_TOP|DT_WORDBREAK/*|DT_RTLREADING*/, NULL);
					SelectObject(hdc, hFntSave);
					break;
				}
				case PopupWnd2::TT_UNICODE:
				{
#if defined(_UNICODE)
					HFONT hFntSave = (HFONT)SelectObject(hdc, fonts.title);
					RECT rc; SetRect(&rc, pos.x, pos.y, pos.x+sz.cx, pos.y+sz.cy);
					DrawTextExW(hdc, wnd->getTitleW(), lstrlenW(wnd->getTitleW()), &rc,
						DT_EXPANDTABS|DT_LEFT|DT_NOPREFIX|DT_TOP|DT_WORDBREAK/*|DT_RTLREADING*/, NULL);
					SelectObject(hdc, hFntSave);
#else
					if (g_popup.isOsUnicode && MyDrawTextExW)
					{
						HFONT hFntSave = (HFONT)SelectObject(hdc, fonts.title);
						RECT rc; SetRect(&rc, pos.x, pos.y, pos.x+sz.cx, pos.y+sz.cy);
						MyDrawTextExW(hdc, wnd->getTitleW(), lstrlenW(wnd->getTitleW()), &rc,
							DT_EXPANDTABS|DT_LEFT|DT_NOPREFIX|DT_TOP|DT_WORDBREAK/*|DT_RTLREADING*/, NULL);
						SelectObject(hdc, hFntSave);
					}
#endif
					break;
				}
				case PopupWnd2::TT_MTEXT:
				{
					HFONT hFntSave = (HFONT)SelectObject(hdc, fonts.title);
					MText.Display(hdc, pos, sz, wnd->getTitleM()); 
					SelectObject(hdc, hFntSave);
					break;
				}
			}

			bmp->restoreAlpha();
			break;
		}

		case ST_ICON:
			bmp->DrawIcon(wnd->getIcon(), head->fx.eval(wnd->getArgs()), head->fy.eval(wnd->getArgs()), 16, 16);
			break;

		case ST_MYBITMAP:
			sz.cx = head->myBmp->getWidth();
			sz.cy = head->myBmp->getHeight();
/*
			if (head->fh.eval(wnd->getArgs()) > 0)
				sz.cy = head->fh.eval(wnd->getArgs());
			else
			if (head->fh.eval(wnd->getArgs()) < 0)
				sz.cy = szWnd.cy - head->fy.eval(wnd->getArgs()) + head->fh.eval(wnd->getArgs());

			if (head->fw.eval(wnd->getArgs()) > 0)
				sz.cx = head->fw.eval(wnd->getArgs());
			else
			if (head->fw.eval(wnd->getArgs()) < 0)
				sz.cx = szWnd.cx - head->fx.eval(wnd->getArgs()) + head->fw.eval(wnd->getArgs());
*/
			if (head->proportional && sz.cx && sz.cy && (((head->fw.eval(wnd->getArgs()) > 0) && !head->fh.eval(wnd->getArgs())) || (!head->fw.eval(wnd->getArgs()) && (head->fh.eval(wnd->getArgs()) > 0))))
			{
				if (head->fw.eval(wnd->getArgs()))
				{
					sz.cy = sz.cy * head->fw.eval(wnd->getArgs()) / sz.cx;
					sz.cx = head->fw.eval(wnd->getArgs());
				} else
				{
					sz.cx = sz.cx * head->fh.eval(wnd->getArgs()) / sz.cy;
					sz.cy = head->fh.eval(wnd->getArgs());
				}
			} else
			{
				if (head->fh.eval(wnd->getArgs()) > 0)
					sz.cy = head->fh.eval(wnd->getArgs());
				else
				if (head->fh.eval(wnd->getArgs()) < 0)
					sz.cy = szWnd.cy - head->fy.eval(wnd->getArgs()) + head->fh.eval(wnd->getArgs()) + 1;
				if (head->fw.eval(wnd->getArgs()) > 0)
					sz.cx = head->fw.eval(wnd->getArgs());
				else
				if (head->fw.eval(wnd->getArgs()) < 0)
					sz.cx = szWnd.cx - head->fx.eval(wnd->getArgs()) + head->fw.eval(wnd->getArgs()) + 1;
			}

			bool vars;
			pos.cx = ((head->fx.eval(wnd->getArgs(), &vars) < 0) && !vars) ? szWnd.cx + head->fx.eval(wnd->getArgs()) : head->fx.eval(wnd->getArgs());
			pos.cy = ((head->fy.eval(wnd->getArgs(), &vars) < 0) && !vars) ? szWnd.cy + head->fy.eval(wnd->getArgs()) : head->fy.eval(wnd->getArgs());

			if (head->type & ST_MONO)
			{
				COLORREF back = wnd->getBackColor();
				if (head->type & ST_BLEND)
				{
					bmp->BlendColorized(head->myBmp, pos.cx, pos.cy, sz.cx, sz.cy, back);
				} else
				{
					bmp->DrawColorized(head->myBmp, pos.cx, pos.cy, sz.cx, sz.cy, back);
				}
			} else
			{
				if (head->type & ST_BLEND)
				{
					bmp->Blend(head->myBmp, pos.cx, pos.cy, sz.cx ,sz.cy);
				} else
				{
					bmp->Draw(head->myBmp, pos.cx, pos.cy, sz.cx ,sz.cy);
				}
			}
			break;

		case ST_AVATAR:
		{
			if (!wnd->getAvatar()) break;

			sz.cx = wnd->getAvatar()->getWidth();
			sz.cy = wnd->getAvatar()->getHeight();

			if (head->proportional && sz.cx && sz.cy && (((head->fw.eval(wnd->getArgs()) > 0) && !head->fh.eval(wnd->getArgs())) || (!head->fw.eval(wnd->getArgs()) && (head->fh.eval(wnd->getArgs()) > 0))))
			{
				if (head->fw.eval(wnd->getArgs()))
				{
					sz.cy = sz.cy * head->fw.eval(wnd->getArgs()) / sz.cx;
					sz.cx = head->fw.eval(wnd->getArgs());
				} else
				{
					sz.cx = sz.cx * head->fh.eval(wnd->getArgs()) / sz.cy;
					sz.cy = head->fh.eval(wnd->getArgs());
				}
			} else
			{
				if (head->fh.eval(wnd->getArgs()) > 0)
					sz.cy = head->fh.eval(wnd->getArgs());
				else
				if (head->fh.eval(wnd->getArgs()) < 0)
					sz.cy = szWnd.cy - head->fy.eval(wnd->getArgs()) + head->fh.eval(wnd->getArgs());

				if (head->fw.eval(wnd->getArgs()) > 0)
					sz.cx = head->fw.eval(wnd->getArgs());
				else
				if (head->fw.eval(wnd->getArgs()) < 0)
					sz.cx = szWnd.cx - head->fx.eval(wnd->getArgs()) + head->fw.eval(wnd->getArgs());
			}

			pos.cx = head->fx.eval(wnd->getArgs()) < 0 ? szWnd.cx + head->fx.eval(wnd->getArgs()) - sz.cx : head->fx.eval(wnd->getArgs());
			pos.cy = head->fy.eval(wnd->getArgs()) < 0 ? szWnd.cy + head->fy.eval(wnd->getArgs()) - sz.cy : head->fy.eval(wnd->getArgs());

			wnd->getAvatar()->draw(bmp, pos.cx, pos.cy, sz.cx, sz.cy, options);
			break;
		}

		case ST_CLOCK:
		{
			if (!options->DisplayTime)
				break;

			COLORREF back = wnd->getBackColor();
			int x = head->fx.eval(wnd->getArgs());
			int y = head->fy.eval(wnd->getArgs());
			if (head->myBmp)
			{
				int sy = head->myBmp->getHeight();
				if (head->type & ST_MONO)
					bmp->BlendPartColorized(head->myBmp, head->clockstart[CLOCK_LEFT], 0, head->clocksize[CLOCK_LEFT], sy, x, y, head->clocksize[CLOCK_LEFT], sy, back);
				else
					bmp->BlendPart(head->myBmp, head->clockstart[CLOCK_LEFT], 0, head->clocksize[CLOCK_LEFT], sy, x, y, head->clocksize[CLOCK_LEFT], sy);
				x += head->clocksize[CLOCK_LEFT];
				for (char *p = wnd->getTime(); *p; p++)
				{
					int clock_idx = -1;
					if (*p == ':')
					{
						clock_idx = CLOCK_SEPARATOR;
					} else
					if ((*p >= '0') && (*p <= '9'))
					{
						clock_idx = CLOCK_DIGITS + *p - '0';
					}

					if (clock_idx >= 0)
					{
						if (head->type & ST_MONO)
							bmp->BlendPartColorized(head->myBmp, head->clockstart[clock_idx], 0, head->clocksize[clock_idx], sy, x, y, head->clocksize[clock_idx], sy, back);
						else
							bmp->BlendPart(head->myBmp, head->clockstart[clock_idx], 0, head->clocksize[clock_idx], sy, x, y, head->clocksize[clock_idx], sy);
						x += head->clocksize[clock_idx];
					}
				}
				if (head->type & ST_MONO)
					bmp->BlendPartColorized(head->myBmp, head->clockstart[CLOCK_RIGHT], 0, head->clocksize[CLOCK_RIGHT], sy, x, y, head->clocksize[CLOCK_RIGHT], sy, back);
				else
					bmp->BlendPart(head->myBmp, head->clockstart[CLOCK_RIGHT], 0, head->clocksize[CLOCK_RIGHT], sy, x, y, head->clocksize[CLOCK_RIGHT], sy);
				x += head->clocksize[CLOCK_RIGHT];
			} else
			{
				SetTextColor(hdc, wnd->getClockColor());
				HFONT hfnSave = (HFONT)SelectObject(bmp->getDC(), fonts.clock);
				SIZE sz; GetTextExtentPoint32A(bmp->getDC(), wnd->getTime(), lstrlenA(wnd->getTime()), &sz);
				bmp->Draw_TextA(wnd->getTime(), x, y);
				SelectObject(bmp->getDC(), hfnSave);
			}
			break;
		}
		}
		head = head->next;
	}

}

bool PopupSkin::onMouseMove(PopupWnd2 *wnd, int x, int y) const
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	bool res = false;
	bool hovered = false;

	for (int i = 0; i < wnd->getActionCount(); ++i)
	{
		bool hover = PtInRect(&wnd->getActions()[i].rc, pt) ? true : false;
		hovered |= hover;
		if (wnd->getActions()[i].hover != hover)
		{
			wnd->getActions()[i].hover = hover;
			res = true;
		}
	}

	SetCursor(LoadCursor(NULL, hovered ? IDC_HAND : IDC_ARROW));

	return res;
}

bool PopupSkin::onMouseLeave(PopupWnd2 *wnd) const
{
	bool res = false;

	for (int i = 0; i < wnd->getActionCount(); ++i)
	{
		if (wnd->getActions()[i].hover)
		{
			wnd->getActions()[i].hover = false;
			res = true;
		}
	}

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	return res;
}

void PopupSkin::loadOptions(std::istream &f)
{
	char *buf = new char[1024];
	while (!f.eof())
	{
		f >> buf;
		if (*buf == '#')
		{
			f.ignore(1024, '\n');
			continue;
		}
		if (!strcmp(buf, "option"))
		{
			int id, val;
			f >> id >> val;
			f.getline(buf, 1024);
			id--;
			if (m_flag_names[id])
				mir_free(m_flag_names[id]);
			char *p = buf;
			while (isspace(*p))
				p++;
			char *q = p + lstrlenA(p) - 1;
			while ((q >= p) && isspace(*q))
				*q-- = 0;
			m_flag_names[id] = mir_strdup(p);
			if (val)
				m_flags |= 1 << id;
			else
				m_flags &= ~(1 << id);
		} else
		if (!strcmp(buf, "end"))
		{
			break;
		}
	}
	delete [] buf;
}

bool PopupSkin::load(LPCTSTR dir)
{
	for (int i = 0; i < 32; i++)
	{
		if (m_flag_names[i])
		{
			mir_free(m_flag_names[i]);
			m_flag_names[i] = NULL;
		}
	}
	m_flags = 0;

	if (!_tcsncmp(_T("res:"), dir, 4))
	{ // resource
		loadSkin(dir+4, _T("Skin"));
	} else
	{ // filesystem
		// skin info
		TCHAR dir_save[1024];
		GetCurrentDirectory(1024, dir_save);
		SetCurrentDirectory(dir);
		
		std::ifstream theFile;
		theFile.open("popupskin.config", std::ios::in);
		if (theFile)
		{
			loadOptions(theFile);
			theFile.close();
		}

		WIN32_FIND_DATA ffd;
		HANDLE hFind = FindFirstFile(_T("*.popupskin"), &ffd);
		while (hFind != INVALID_HANDLE_VALUE)
		{
			loadSkin(ffd.cFileName);
			break;
			if (!FindNextFile(hFind, &ffd))
				break;
		}
		FindClose(hFind);

		SetCurrentDirectory(dir_save);
	}

	loadOpts();

	return true;
}

void PopupSkin::loadSkin(std::istream &f)
{
	m_bottom_gap = m_right_gap = 0;
	m_legacy_region_opacity = 64;
	m_shadow_region_opacity = 1;
	m_popup_version = 0;
	m_internalClock = true;

	SKINELEMENT *head = new SKINELEMENT;
	m_elements = head;
	head->type = ST_NOTHING;
	head->next = NULL;

	while (!f.eof())
	{
		char buf[1024];
		f >> buf;
//		fscanf(f, "%s", buf);

		if (!*buf)
			continue;

		if (*buf == '#' || *buf == ';')
		{
			f.ignore(1024, '\n');
			continue;
		}

		if (!lstrcmpA(buf,"popup-version"))
		{
			f >> m_popup_version;
			m_popup_version = PLUGIN_MAKE_VERSION((m_popup_version/1000000)%100, (m_popup_version/10000)%100, (m_popup_version/100)%100, (m_popup_version/1)%100);
			if (!isCompatible())
				break;
		} else
		if (!lstrcmpA(buf,"padding-right"))
		{
			f >> m_right_gap;
		} else
		if (!lstrcmpA(buf,"padding-bottom"))
		{
			f >> m_bottom_gap;
		} else
		if (!lstrcmpA(buf,"shadow-region-opacity"))
		{
			f >> m_shadow_region_opacity;
		} else
		if (!lstrcmpA(buf,"legacy-region-opacity"))
		{
			f >> m_legacy_region_opacity;
		} else
		if (!lstrcmpA(buf,"w"))
		{
			f.getline(buf, 1024);
			m_fw.set(buf);
		} else
		if (!lstrcmpA(buf,"h"))
		{
			f.getline(buf, 1024);
			m_fh.set(buf);
		} else
		if (!lstrcmpA(buf,"object"))
		{
			head->next = loadObject(f);
			if (head->next && ((head->next->type & ST_TYPEMASK) == ST_CLOCK))
				m_internalClock = false;
			head = head->next;
			head->next = NULL;
		} else
		if (!lstrcmpA(buf,"options"))
		{
			loadOptions(f);
		}
	}


	head = m_elements;
	m_elements = m_elements->next;
	delete head;
}

void PopupSkin::loadSkin(LPCTSTR fn)
{
	if(!fn) return;
#if defined (_UNICODE) && _MSC_VER <= 1200
	char* temp= mir_t2a(fn);
	std::ifstream theFile;
	theFile.open(temp, std::ios::in);
	mir_free(temp);
#else
	std::ifstream theFile;
	theFile.open(fn, std::ios::in);
#endif
	if (!theFile) return;
	loadSkin(theFile);
	theFile.close();
}

void PopupSkin::loadSkin(LPCTSTR lpName, LPCTSTR lpType)
{
	HRSRC hRes = FindResource(hInst, lpName, lpType);
//	DWORD ResSize   = SizeofResource(hInst,hRes); 
	HRSRC hResLoad  = (HRSRC)LoadResource(hInst, hRes);
	char *lpResLock = (char *)LockResource(hResLoad);
//	lpResLock [ResSize] = 0;
	std::istringstream stream(lpResLock);
	loadSkin(stream);
	UnlockResource(lpResLock);
	FreeResource(hRes);
}

PopupSkin::SKINELEMENT *PopupSkin::loadObject(std::istream &f)
{
//	char pth[1024];
//	GetSkinPath(pth,1024);

	SKINELEMENT *element = new SKINELEMENT;
//	element->x = element->y = element->w = element->h = 0;
	element->proportional = 0;
	element->type = ST_NOTHING|ST_BADPOS;
	element->next = NULL;
	element->flag_mask = 0;
	element->flags = 0;
	element->myBmp = NULL;

	while (!f.eof())
	{
		char buf[1024];
		f >> buf;

		if (!*buf)
			continue;

		if (*buf == '#' || *buf == ';')
		{
			f.ignore(1024, '\n');
			continue;
		}

		if (!lstrcmpA(buf,"type"))
		{
			f >> buf;
			if (!lstrcmpA(buf,"icon"))
				element->type = ( element->type & ~ST_TYPEMASK ) | ST_ICON;
			else
			if (!lstrcmpA(buf,"bitmap"))
				element->type = ( element->type & ~ST_TYPEMASK ) | ST_MYBITMAP;
			else
			if (!lstrcmpA(buf,"text"))
			{
				element->type = ( element->type & ~ST_TYPEMASK ) | ST_TEXT;
				element->textColor = (COLORREF)0xffffffff;
				element->hfn = 0;
			}
			else
			if (!lstrcmpA(buf,"title"))
			{
				element->type = ( element->type & ~ST_TYPEMASK ) | ST_TITLE;
				element->textColor = (COLORREF)0xffffffff;
				element->hfn = 0;
			} else
			if (!lstrcmpA(buf,"avatar"))
			{
				element->type = ( element->type & ~ST_TYPEMASK ) | ST_AVATAR;
			} else
			if (!lstrcmpA(buf,"clock"))
			{
				element->type = ( element->type & ~ST_TYPEMASK ) | ST_CLOCK;
			}
		} else
		if (!lstrcmpA(buf,"source"))
		{
			f >> buf;
//			fscanf(f, "%*[\x01-\x20]%[^\n]", buf);
//			if ( (element->type & ST_TYPEMASK ) == ST_BITMAP)
//				element->hbm = LoadImageFromFile(buf);
//			else
			if (((element->type & ST_TYPEMASK) == ST_MYBITMAP) || ((element->type & ST_TYPEMASK) == ST_CLOCK))
			{
				char *alpha = mir_strdup(buf);
				alpha[strlen(alpha)-1] = 'a';
				element->myBmp = new MyBitmap(buf,alpha);
				mir_free(alpha);
			}
		} else
		if (!lstrcmpA(buf,"mono"))
		{
			element->type |= ST_MONO;
		} else
		if (!lstrcmpA(buf,"layer"))
		{
			element->type |= ST_BLEND;
		} else
		if (!lstrcmpA(buf,"proportional"))
		{
			f >> element->proportional;
		} else
		if (!lstrcmpA(buf,"x"))
		{
			f.getline(buf, 1024);
			element->fx.set(buf);
			element->type &= ~ST_BADPOS;
		} else
		if (!lstrcmpA(buf,"y"))
		{
			f.getline(buf, 1024);
			element->fy.set(buf);
			element->type &= ~ST_BADPOS;
		} else
		if (!lstrcmpA(buf,"w"))
		{
			f.getline(buf, 1024);
			element->fw.set(buf);
		} else
		if (!lstrcmpA(buf,"h"))
		{
			f.getline(buf, 1024);
			element->fh.set(buf);
		} else
		if (!lstrcmpA(buf,"ifset"))
		{
			int id;
			f >> id; id--;
			element->flag_mask |= 1 << id;
			element->flags |= 1 << id;
		} else
		if (!lstrcmpA(buf,"ifnotset"))
		{
			int id;
			f >> id; id--;
			element->flag_mask |= 1 << id;
			element->flags &= ~(1 << id);
		} else
		if (!lstrcmpA(buf,"color"))
		{
			if (((element->type & ST_TYPEMASK) != ST_TEXT) &&
			    ((element->type & ST_TYPEMASK) != ST_TITLE)) continue;

			int r,g,b;
			f >> r >> g >> b;
			element->textColor = RGB(r,g,b);
		} else
		if (!lstrcmpA(buf,"clocksize"))
		{
			element->clockstart[0]=0;
			f >> element->clocksize[0];
			for (int i = 1; i < CLOCK_ITEMS; i++)
			{
				element->clockstart[i]=element->clockstart[i-1]+element->clocksize[i-1];
				f >> element->clocksize[i];
			}
		} else
		if (!lstrcmpA(buf,"end"))
		{
			break;
		}
	}
	return element;
}

void PopupSkin::freeSkin(SKINELEMENT *head)
{
	while (head)
	{
		SKINELEMENT *next = head->next;

		if ((head->type & ST_TYPEMASK) == ST_BITMAP)
			DeleteObject(head->hbm);
		if ((head->type & ST_TYPEMASK) == ST_MYBITMAP)
			delete head->myBmp;
		if (((head->type & ST_TYPEMASK) == ST_CLOCK) && head->myBmp)
			delete head->myBmp;

		delete head;
		head = next;
	}
}

void PopupSkin::saveOpts() const
{
	char buf[128];
#if defined( _UNICODE )
	mir_snprintf(buf, sizeof(buf), "skin.%.120S", m_name);
#else
	mir_snprintf(buf, sizeof(buf), "skin.%.120s", m_name);
#endif
	DBWriteContactSettingDword(NULL, MODULNAME, buf, m_flags);
}

void PopupSkin::loadOpts() const
{
	char buf[128];
#if defined( _UNICODE )
	mir_snprintf(buf, sizeof(buf), "skin.%.120S", m_name);
#else
	mir_snprintf(buf, sizeof(buf), "skin.%.120s", m_name);
#endif
	m_flags = DBGetContactSettingDword(NULL, MODULNAME, buf, m_flags);
}

// Skins
Skins skins;

Skins::Skins()
{
	m_skins = 0;
}

Skins::~Skins()
{
	while (m_skins)
	{
		SKINLIST *next = m_skins->next;
		delete m_skins->skin;
		delete [] m_skins->dir;
		mir_free(m_skins->name); // this is allocated with mir_strdup()
		delete m_skins;
		m_skins = next;
	}
}

bool Skins::load(LPCTSTR dir1)
{
	while (m_skins)
	{
		SKINLIST *next = m_skins->next;
		mir_free(m_skins->name);
		delete [] m_skins->dir;
		delete m_skins->skin;
		delete m_skins;
		m_skins = next;
	}

	SKINLIST *skin = new SKINLIST;
	skin->next = m_skins;
	m_skins = skin;
	m_skins->name = mir_tstrdup(_T("* Popup Classic"));
	m_skins->dir = new TCHAR[1024];
	lstrcpy(m_skins->dir, _T("res:classic.popupskin"));
	m_skins->skin = 0;

	TCHAR dir[1024]={'\0'};

	if (ServiceExists(MS_FOLDERS_GET_PATH)) {
		if (FoldersGetCustomPathT(folderId, dir, 1024, NULL) != 0)
			return false;
	}
	else {
		GetModuleFileName(hInst,dir,1024);
		dir[lstrlen(dir)-18] = 0;
		lstrcat(dir,_T("\\skins\\popup"));
		DWORD fa = GetFileAttributes(dir);
		if ((fa==INVALID_FILE_ATTRIBUTES) || !(fa&FILE_ATTRIBUTE_DIRECTORY))
			return false;
	}

	TCHAR dir_save[1024];
	GetCurrentDirectory(1024, dir_save);
	SetCurrentDirectory(dir);

	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile(_T("*.*"), &ffd);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && lstrcmp(_T("."), ffd.cFileName) && lstrcmp(_T(".."), ffd.cFileName))
		{
			SetCurrentDirectory(ffd.cFileName);

			SKINLIST *skin = new SKINLIST;
			skin->next = m_skins;
			m_skins = skin;
			m_skins->name = mir_tstrdup(ffd.cFileName);
			m_skins->dir = new TCHAR[1024];
			GetCurrentDirectory(1024, m_skins->dir);
			m_skins->skin = 0;

			SetCurrentDirectory(_T(".."));
		}

		if (!FindNextFile(hFind, &ffd))
			break;
	}
	FindClose(hFind);

	SetCurrentDirectory(dir_save);

	return true;
}

const PopupSkin *Skins::getSkin(LPCTSTR name)
{
	SKINLIST *any = 0;
	for (SKINLIST *p = m_skins; p; p = p->next)
	{
		if (!lstrcmp(p->name, _T("* Popup Classic")) || !any)
			any = p;
		if (!lstrcmpi(p->name, name))
		{
			any = p;
			break;
		}
	}

	if (any && any->skin)
		return any->skin;
	if (!any)
		return 0;

	any->skin = new PopupSkin(any->name);
	any->skin->load(any->dir);

	if (!any->skin->isCompatible())
		MessageBox(NULL,
			TranslateTS(
				_T("The skin you are trying to load is designed\r\n")
				_T("for newer version of Popup Plus. And will not\r\n")
				_T("display properly.\r\n")
				_T("\r\n")
				_T("Please choose another skin.") ),
			_T(MODULNAME_LONG), MB_ICONSTOP|MB_OK);

	return any->skin;
}

void Skins::loadActiveSkin()
{
	for (SKINLIST *p = m_skins; p; p = p->next)
		if (!lstrcmpi(p->name, PopUpOptions.SkinPack))
		{
			if (p->skin)
				break;

			p->skin = new PopupSkin(p->name);
			p->skin->load(p->dir);
			break;
		}
}

void Skins::freeAllButActive()
{
	for (SKINLIST *p = m_skins; p; p = p->next)
		if (lstrcmpi(p->name, PopUpOptions.SkinPack))
		{
			delete p->skin;
			p->skin = NULL;
		}
}
