/*
Author Ricardo Pescuma Domenecci

Miranda IM: the free IM client for Microsoft* Windows*

Copyright 2005-2012 Miranda IM project,
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

#ifndef modern_row_h__
#define modern_row_h__

#pragma once


// Types of cells
#define TC_TEXT1 1
#define TC_TEXT2 2
#define TC_TEXT3 3
#define TC_STATUS 4
#define TC_AVATAR 5
#define TC_EXTRA 6
#define TC_EXTRA1 7
#define TC_EXTRA2 8
#define TC_EXTRA3 9
#define TC_EXTRA4 10
#define TC_EXTRA5 11
#define TC_EXTRA6 12
#define TC_EXTRA7 13
#define TC_EXTRA8 14
#define TC_EXTRA9 15
#define TC_TIME 16
#define TC_SPACE 17
#define TC_FIXED 18


#define TC_ELEMENTSCOUNT 18

// Containers
#define TC_ROOT 50
#define TC_ROW 51
#define TC_COL 52
#define TC_FLOAT 53

// Alignes
#define TC_LEFT 0
#define TC_HCENTER 100
#define TC_RIGHT 101

#define TC_TOP 0
#define TC_VCENTER 102
#define TC_BOTTOM 103

// Sizes
#define TC_WIDTH 104
#define TC_HEIGHT 105



// ���������, ����������� ��������� �������� ��������
//
typedef struct tagRowCell
{
	int cont;				// ��� ���������� - �������, �������, �������
	int type;				// ��� ��������, ������������� � ����������, ���� 0 - ������ ���������
	int halign;				// �������������� ������������ ������ ����������
	int valign;				// ������������ ������������ ������ ����������

	int	w;					// ������ �������� ��������, ��� ��������� ����� ������������
	int	h;					// ������ �������� ��������

	BOOL sizing;			// ��������, ������������ ������� ��������� ����� � �������� �����������
	BOOL layer;				// ��������, ������������, ��� ��������� �������� ����� ����
  
  BOOL hasfixed;    // �������� ������������ ��� ���� ��������� ������������� ��������
  BOOL fitwidth;    // �������� ����������� ��� ��������� ������� ��������� ��� ���������� 
                    // ������������ (������������ ��������.�������)

  int fixed_width;
  int full_width;

	RECT r;					// ������������� ��� ��������� ��������
	struct tagRowCell * next;		// ���� ����� 
	struct tagRowCell * child;		// ���� ����� ��. ���� ��������
} ROWCELL, *pROWCELL;

// ��������� ��� ������� � ����������� �������� �������� ������ ������ ��������
#ifndef _CPPCODE
  int cppCalculateRowHeight(ROWCELL	*RowRoot);
  void cppCalculateRowItemsPos(ROWCELL	*RowRoot, int width);
  ROWCELL *cppInitModernRow(ROWCELL	** tabAccess);
  void cppDeleteTree(ROWCELL	* RowRoot);
#endif

#endif // modern_row_h__