/*
Traffic Counter plugin for Miranda IM 
Copyright 2007-2011 Mironych.

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
*/

#include "commonheaders.h"

// Функция заменяет фигурные скобки в строке string на пробелы
void ReplaceBrackets(TCHAR *string)
{
	WORD i;
	for (i = 0; string[i]; i++)
		if ((string[i] == '{') || (string[i] == '}'))
			string[i] = ' ';
}

// Функция принимает строку с фигурными скобками и возвращает строки, предназначенные для
// вывода с выравниванием по левому/центру/по правому
void GetFormattedStrings(TCHAR *src, TCHAR *la, TCHAR *ca, TCHAR *ra)
{
	signed short int BracketLeft, BracketRight;
	BracketLeft = !_tcschr(src, '{') ? -1 : _tcschr(src, '{') - src;
	BracketRight = !_tcschr(src, '}') ? -1 : _tcschr(src, '}') - src;
	SecureZeroMemory(la, 512);
	SecureZeroMemory(ca, 512);
	SecureZeroMemory(ra, 512);
	// Если присутствуют обе скобки в правильном порядке
	if ((BracketLeft != -1) && (BracketRight != -1) && (BracketLeft < BracketRight))
	{
		_tcsncpy(la, src, BracketLeft);
		_tcsncpy(ca, src + BracketLeft + 1, BracketRight - BracketLeft - 1);
		_tcscpy(ra, src + BracketRight + 1);
	}
	// иначе весь текст выравнивается по правому краю
	else _tcscpy(ra, src);
	// Избавляемся от оставшихся скобок
	ReplaceBrackets(la);
	ReplaceBrackets(ca);
	ReplaceBrackets(ra);
}

/* Функция возвращает количество дней в указанном месяце указанного года. */
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

// Функция определяет день недели по дате
// 7 - ВС, 1 - ПН и т. д.
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

/*
Аргументы:
Value - количество байт;
Unit - единицы измерения (0 - байты, 1 - килобайты, 2 - мегабайты, 3 - автоматически);
Buffer - адрес строки для записи результата
*/
void GetFormattedTraffic(DWORD Value, BYTE Unit, TCHAR *Buffer)
{
	TCHAR Str1[32], szUnit[4] = {' ', 0}, *pChar;
	DWORD Divider;
	NUMBERFMT nf = {0, 1, 3, _T(","), _T(" "), 0};

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
	mir_sntprintf(Str1, 32, _T("%.2f"), 1.0 * Value / Divider);
	pChar = _tcsrchr(Str1, ',');
	*pChar = '.';
	if (!GetNumberFormat(LOCALE_USER_DEFAULT, 0, Str1, &nf, Buffer, 32))
		mir_sntprintf(Buffer, 32, _T("?? ??"));
	_tcscat(Buffer, szUnit);
}

/* Преобразование интервала времени в его строковое представление
Аргументы:
Duration: интервал времени в секундах;
Format: строка формата;
Buffer: адрес буфера, куда функция помещает результат.
Size - размер буфера. */
void GetDurationFormatM(DWORD Duration, TCHAR *Format, TCHAR *Buffer, WORD Size)
{
	WORD i, j;
	DWORD Values[4]; // Значения интервалов времени.
	BYTE Flags[4]; // Признаки наличия токенов в строке формата.

	for (i = 0; i < Size; Buffer[i++] = 0);

	Flags[0] = NULL != _tcsstr(Format, _T("d"));
	Flags[1] = NULL != _tcsstr(Format, _T("h"));
	Flags[2] = NULL != _tcsstr(Format, _T("m"));
	Flags[3] = NULL != _tcsstr(Format, _T("s"));
	
	Values[0] = Duration / (60 * 60 * 24);
	Values[1] = Duration / (60 * 60) 
		- (Flags[0] ? Values[0] * 24 : 0);
	Values[2] = Duration / 60 
		- (Flags[1] ? Values[1] * 60 : 0) 
		- (Flags[0] ? Values[0] * 24 * 60 : 0);
	Values[3] = Duration 
		- (Flags[2] ? Values[2] * 60 : 0)
		- (Flags[1] ? Values[1] * 60 * 60 : 0) 
		- (Flags[0] ? Values[0] * 24 * 60 * 60 : 0);

	for (i = j = 0; Format[i]; i++)
	{
		switch (Format[i])
		{
			case 'd':
				mir_sntprintf(Buffer, Size, _T("%s%d"), Buffer, Values[0]);
				break;
			case 'h':
				if (Format[i+1] == 'h')
				{
					mir_sntprintf(Buffer, Size, _T("%s%02d"), Buffer, Values[1]);
					i++;
				}
				else mir_sntprintf(Buffer, Size, _T("%s%d"), Buffer, Values[1]);
				break;
			case 'm':
				if (Format[i+1] == 'm')
				{
					mir_sntprintf(Buffer, Size, _T("%s%02d"), Buffer, Values[2]);
					i++;
				}
				else mir_sntprintf(Buffer, Size, _T("%s%d"), Buffer, Values[2]);
				break;
			case 's':
				if (Format[i+1] == 's')
				{
					mir_sntprintf(Buffer, Size, _T("%s%02d"), Buffer, Values[3]);
					i++;
				}
				else mir_sntprintf(Buffer, Size, _T("%s%d"), Buffer, Values[3]);
				break;
			default:
				mir_sntprintf(Buffer, Size, _T("%s%c"), Buffer, Format[i]); 
				break;
		}
	}
}

/* Результат:
-1 - st1 < st2
 0 - st1 = st2
+1 - st1 > st2
*/
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
