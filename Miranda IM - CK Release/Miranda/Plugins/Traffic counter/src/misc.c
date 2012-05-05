/*
Traffic Counter plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) Copyright 2007-2011 Mironych

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

#include "commonheaders.h"


WORD GetRowItems(TCHAR *InputString, RowItemInfo **RowItemsList)
{
	TCHAR *begin, *end;
	WORD c = 0;

	begin = _tcschr(InputString, '{');
	if (begin)
	{
		*RowItemsList = (RowItemInfo*)mir_alloc(sizeof(RowItemInfo));
	}
	else return 0;

	do
	{
		end = _tcschr(begin, '}');

		*RowItemsList = (RowItemInfo*)mir_realloc(*RowItemsList, sizeof(RowItemInfo) * (c + 1));

		_stscanf(begin + 1, _T("%c%d"),
				&((*RowItemsList)[c].Alignment),
				&((*RowItemsList)[c].Interval));

		begin = _tcschr(end, '{');

		if (begin)
		{
			(*RowItemsList)[c].String = (TCHAR*)mir_alloc(sizeof(TCHAR) * (begin - end));
			_tcsncpy((*RowItemsList)[c].String, end + 1, begin - end - 1);
			(*RowItemsList)[c].String[begin - end - 1] = 0;
		}
		else
		{
			(*RowItemsList)[c].String = (TCHAR*)mir_alloc(sizeof(TCHAR) * _tcslen(end));
			_tcsncpy((*RowItemsList)[c].String, end + 1, _tcslen(end));
		}

		c++;
	} while (begin);

	return c;
}

BYTE DaysInMonth(BYTE Month, WORD Year)
{
	switch (Month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12: return 31;
		case 4:
		case 6:
		case 9:
		case 11: return 30;
		case 2: return 28 + (BYTE)!((Year % 4) && ( (Year % 100) || !(Year % 400) ));
	}
	return 0;
}

BYTE DayOfWeek(BYTE Day, BYTE Month, WORD Year)
{
	WORD a, y, m;

	a = (14 - Month) / 12;
	y = Year - a;
	m = Month + 12 * a - 2;

	a = (7000 + (Day + y + (y >> 2) - y / 100 + y / 400 + (31 * m) / 12)) % 7;
	if (!a) a = 7;

	return a;
}

WORD GetFormattedTraffic(DWORD Value, BYTE Unit, TCHAR *Buffer, WORD Size)
{
	TCHAR Str1[32], szUnit[4] = {' ', 0};
	DWORD Divider;
	NUMBERFMT nf = {0, 1, 3, _T(","), _T(" "), 0};
	TCHAR *Res; 
	WORD l;

	switch (Unit)
	{
		case 0: //bytes
			Divider = 1;
			nf.NumDigits = 0;
			szUnit[0] = 0;
			break;
		case 1: // KB
			Divider = 0x400;
			nf.NumDigits = 2;
			break;
		case 2: // MB
			Divider = 0x100000;
			nf.NumDigits = 2;
			break;
		case 3: // Adaptive
			nf.NumDigits = 2;
			if (Value < 0x100000) { Divider = 0x400; szUnit[1] = 'K'; szUnit[2] = 'B'; }
			else { Divider = 0x100000; szUnit[1] = 'M'; szUnit[2] = 'B'; }
			break;
	}

	mir_sntprintf(Str1, 32, _T("%d.%d"), Value / Divider, Value % Divider);
	l = GetNumberFormat(LOCALE_USER_DEFAULT, 0, Str1, &nf, NULL, 0);
	if (!l) return 0;
	l += _tcslen(szUnit) + 1;
	Res = (TCHAR*)malloc(l * sizeof(TCHAR));
	if (!Res) return 0;
	GetNumberFormat(LOCALE_USER_DEFAULT, 0, Str1, &nf, Res, l);
	_tcscat(Res, szUnit);

	if (Size && Buffer)
	{
		_tcscpy(Buffer, Res);
		l = _tcslen(Buffer);
	}
	else
	{
		l = _tcslen(Res) + 1;
	}

	free(Res);
	return l;
}

WORD GetDurationFormatM(DWORD Duration, TCHAR *Format, TCHAR *Buffer, WORD Size)
{
	DWORD q;
	WORD TokenIndex, FormatIndex, Length;
	TCHAR Token[256], 
		*Res; 

	Res = (TCHAR*)malloc(sizeof(TCHAR)); 
	SecureZeroMemory(Res, sizeof(TCHAR));

	for (FormatIndex = 0; Format[FormatIndex];)
	{
		TokenIndex = 0;
		q = _istalpha(Format[FormatIndex]);
		do
		{
			Token[TokenIndex++] = Format[FormatIndex++];
		} while (q == _istalpha(Format[FormatIndex]));
		Token[TokenIndex] = 0;

		if (!_tcscmp(Token, _T("d")))
		{
			q = Duration / (60 * 60 * 24);
			mir_sntprintf(Token, 256, _T("%d"), q);
			Duration -= q * 60 * 60 * 24;
		}
		else
		if (!_tcscmp(Token, _T("h")))
		{
			q = Duration / (60 * 60);
			mir_sntprintf(Token, 256, _T("%d"), q);
			Duration -= q * 60 * 60;
		}
		else
		if (!_tcscmp(Token, _T("hh")))
		{
			q = Duration / (60 * 60);
			mir_sntprintf(Token, 256, _T("%02d"), q);
			Duration -= q * 60 * 60;
		}
		else
		if (!_tcscmp(Token, _T("m")))
		{
			q = Duration / 60;
			mir_sntprintf(Token, 256, _T("%d"), q);
			Duration -= q * 60;
		}
		else
		if (!_tcscmp(Token, _T("mm")))
		{
			q = Duration / 60;
			mir_sntprintf(Token, 256, _T("%02d"), q);
			Duration -= q * 60;
		}
		else
		if (!_tcscmp(Token, _T("s")))
		{
			q = Duration;
			mir_sntprintf(Token, 256, _T("%d"), q);
			Duration -= q;
		}
		else
		if (!_tcscmp(Token, _T("ss")))
		{
			q = Duration;
			mir_sntprintf(Token, 256, _T("%02d"), q);
			Duration -= q;
		}

		Length = _tcslen(Res) + _tcslen(Token) + 1;
		Res = (TCHAR*)realloc(Res, Length * sizeof(TCHAR));
		_tcscat(Res, Token);
	}

	if (Size && Buffer)
	{
		_tcsncpy(Buffer, Res, Size);
		Length = _tcslen(Buffer);
	}
	else
	{
		Length = _tcslen(Res) + 1;
	}

	free(Res);
	return Length;
}

signed short int TimeCompare(SYSTEMTIME st1, SYSTEMTIME st2)
{
	signed short int a, b, c, d;

	a = st1.wYear - st2.wYear;
	b = st1.wMonth - st2.wMonth;
	c = st1.wDay - st2.wDay;
	d = st1.wHour - st2.wHour;

	if (a < 0) return -1;
	if (a > 0) return +1;

	if (b < 0) return -1;
	if (b > 0) return +1;

	if (c < 0) return -1;
	if (c > 0) return +1;

	if (d < 0) return -1;
	if (d > 0) return +1;

	return 0;
}
