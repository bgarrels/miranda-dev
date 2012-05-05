/*
Spellchecker plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2009-2010 Ricardo Pescuma Domenecci

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

* RichEdit part for spellchecker
*/

#include "commons.h"


RichEdit::RichEdit(HWND hwnd) 
	: hwnd(NULL), ole(NULL), textDocument(NULL), stopped(0), undoEnabled(TRUE)
{
	SetHWND(hwnd);
}

RichEdit::~RichEdit()
{
	SetHWND(NULL);
}

bool RichEdit::IsValid() const
{
	return ole != NULL;
}

HWND RichEdit::GetHWND() const
{
	return hwnd;
}

void RichEdit::SetHWND(HWND hwnd)
{
	if (textDocument != NULL)
	{
		textDocument->Release();
		textDocument = NULL;
	}
	if (ole != NULL)
	{
		ole->Release();
		ole = NULL;
	}

	this->hwnd = hwnd;

	if (hwnd == NULL)
		return;

	SendMessage(EM_GETOLEINTERFACE, 0, (LPARAM) &ole);
	if (ole == NULL)
		return;

	if (ole->QueryInterface(IID_ITextDocument, (void**) &textDocument) != S_OK)
		textDocument = NULL;
}

LRESULT RichEdit::SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam) const
{
	return ::SendMessage(hwnd, Msg, wParam, lParam);
}

bool RichEdit::IsReadOnly() const
{
	return (GetWindowLong(hwnd, GWL_STYLE) & ES_READONLY) == ES_READONLY;
}

void RichEdit::SuspendUndo()
{
	if (textDocument != NULL)
	{
		textDocument->Undo(tomSuspend, NULL);
		undoEnabled = FALSE;
	}
}

void RichEdit::ResumeUndo()
{
	if (textDocument != NULL)
	{
		textDocument->Undo(tomResume, NULL);
		undoEnabled = TRUE;
	}
}

void RichEdit::Stop()
{
	stopped++;
	if (stopped != 1)
		return;

	SuspendUndo();

//	HideCaret(hwnd);
	SendMessage(WM_SETREDRAW, FALSE, 0);

	SendMessage(EM_GETSCROLLPOS, 0, (LPARAM) &old_scroll_pos);
	SendMessage(EM_EXGETSEL, 0, (LPARAM) &old_sel);
	GetCaretPos(&caretPos);

	old_mask = SendMessage(EM_GETEVENTMASK, 0, 0);
	SendMessage(EM_SETEVENTMASK, 0, old_mask & ~ENM_CHANGE);

	inverse = (old_sel.cpMin >= LOWORD(SendMessage(EM_CHARFROMPOS, 0, (LPARAM) &caretPos)));
}

void RichEdit::Start()
{
	stopped--;

	if (stopped < 0)
	{
		stopped = 0;
		return;
	}
	else if (stopped > 0)
		return;

	if (inverse)
	{
		LONG tmp = old_sel.cpMin;
		old_sel.cpMin = old_sel.cpMax;
		old_sel.cpMax = tmp;
	}

	SendMessage(EM_SETEVENTMASK, 0, old_mask);
	SendMessage(EM_EXSETSEL, 0, (LPARAM) &old_sel);
	SendMessage(EM_SETSCROLLPOS, 0, (LPARAM) &old_scroll_pos);

	SendMessage(WM_SETREDRAW, TRUE, 0);
	InvalidateRect(hwnd, NULL, FALSE);
//	ShowCaret(hwnd);

	ResumeUndo();
}

BOOL RichEdit::IsStopped()
{
	return stopped > 0;
}

int RichEdit::GetCharFromPos(const POINT &pt)
{
	return LOWORD(SendMessage(EM_CHARFROMPOS, 0, (LPARAM) &pt));
}

int RichEdit::GetLineCount() const
{
	return SendMessage(EM_GETLINECOUNT, 0, 0);
}

void RichEdit::GetLine(int line, TCHAR *text, size_t text_len) const
{
	*((WORD*)text) = text_len - 1;
	unsigned size = (unsigned) SendMessage(EM_GETLINE, (WPARAM) line, (LPARAM) text);
	// Sometimes it likes to return size = lineLen+1, adding an \n at the end, so we remove it here
	// to make both implementations return same size
	int lineLen = GetLineLength(line);
	size = (unsigned) max(0, min((int)text_len - 1, min((int) size, lineLen)));
	text[size] = _T('\0');

/*
	int len = GetLineLength(line);
	if (len < 1)
	{
		text[0] = 0;
		return;
	}

	int first = GetFirstCharOfLine(line);
	TCHAR *tmp = GetText(first, first + GetLineLength(line));
	lstrcpyn(text, tmp, text_len);
	mir_free(tmp);
*/
}

int RichEdit::GetLineLength(int line) const
{
	return SendMessage(EM_LINELENGTH, GetFirstCharOfLine(line), 0);
}

int RichEdit::GetFirstCharOfLine(int line) const
{
	return SendMessage(EM_LINEINDEX, (WPARAM) line, 0);
}

int RichEdit::GetLineFromChar(int charPos) const
{
	return SendMessage(EM_LINEFROMCHAR, charPos, 0);
}

CHARRANGE RichEdit::GetSel() const
{
	CHARRANGE sel;
	SendMessage(EM_EXGETSEL, 0, (LPARAM) &sel);
	return sel;
}

void RichEdit::SetSel(int start, int end)
{
	CHARRANGE sel = { start, end };
	SetSel(sel);
}

void RichEdit::SetSel(const CHARRANGE &sel)
{
	SendMessage(EM_EXSETSEL, 0, (LPARAM) &sel);
}

int RichEdit::GetTextLength() const
{
	return GetWindowTextLength(hwnd);
}

TCHAR *RichEdit::GetText(int start, int end) const
{
	if (end <= start)
		end = GetTextLength();

	if (textDocument != NULL)
	{
		ITextRange *range;
		if (textDocument->Range(start, end, &range) != S_OK) 
			return mir_tstrdup(_T(""));

		BSTR text = NULL;
		if (range->GetText(&text) != S_OK || text == NULL)
		{
			range->Release();
			return mir_tstrdup(_T(""));
		}

		TCHAR *ret = mir_u2t(text);

		SysFreeString(text);

		range->Release();

		return ret;
	}
	else
	{
		int len = GetTextLength();
		TCHAR *tmp = (TCHAR *) mir_alloc(len * sizeof(TCHAR));
		GetWindowText(hwnd, tmp, len);
		tmp[len] = 0;

		TCHAR *ret = (TCHAR *) mir_alloc((end - start + 1) * sizeof(TCHAR));
		memmove(ret, &tmp[start], (end - start) * sizeof(TCHAR));
		ret[end - start] = 0;

		mir_free(tmp);
		return ret;
	}
}

void RichEdit::ReplaceSel(const TCHAR *new_text)
{
	if (stopped)
	{
		CHARRANGE sel = GetSel();

		ResumeUndo();

		SendMessage(EM_REPLACESEL, undoEnabled, (LPARAM) new_text);

		SuspendUndo();

		FixSel(&old_sel, sel, lstrlen(new_text));

		SendMessage(WM_SETREDRAW, FALSE, 0);
		SendMessage(EM_SETEVENTMASK, 0, old_mask & ~ENM_CHANGE);
	}
	else
	{
		SendMessage(EM_REPLACESEL, undoEnabled, (LPARAM) new_text);
	}
}

int RichEdit::Replace(int start, int end, const TCHAR *new_text)
{
	CHARRANGE sel = GetSel();
	CHARRANGE replace_sel = { start, end };
	SetSel(replace_sel);
	
	ReplaceSel(new_text);

	int dif = FixSel(&sel, replace_sel, lstrlen(new_text));
	SetSel(sel);
	return dif;
}

int RichEdit::Insert(int pos, const TCHAR *text)
{
	CHARRANGE sel = GetSel();
	CHARRANGE replace_sel = { pos, pos };
	SetSel(replace_sel);
	
	ReplaceSel(text);

	int dif = FixSel(&sel, replace_sel, lstrlen(text));
	SetSel(sel);
	return dif;
}

int RichEdit::Delete(int start, int end)
{
	CHARRANGE sel = GetSel();
	CHARRANGE replace_sel = { start, end };
	SetSel(replace_sel);
	
	ReplaceSel(_T(""));

	int dif = FixSel(&sel, replace_sel, 0);
	SetSel(sel);
	return dif;
}

int RichEdit::FixSel(CHARRANGE *to_fix, CHARRANGE sel_changed, int new_len)
{
	int dif = new_len - (sel_changed.cpMax - sel_changed.cpMin);
	
	if (to_fix->cpMax <= sel_changed.cpMin)
		return dif;

	int newMax = sel_changed.cpMax + dif;

	if (to_fix->cpMin >= sel_changed.cpMax)
		to_fix->cpMin += dif;
	else if (to_fix->cpMin >= newMax) // For dif < 0, pos beetween sel_changed.cpMax + dif and sel_changed.cpMax
		to_fix->cpMin = newMax;

	if (to_fix->cpMax >= sel_changed.cpMax)
		to_fix->cpMax += dif;
	else if (to_fix->cpMax >= newMax) // For dif < 0, pos beetween sel_changed.cpMax + dif and sel_changed.cpMax
		to_fix->cpMax = newMax;

	return dif;
}
