/*
Author Artem Shpynov aka FYR

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

#pragma once

typedef signed char sbyte;
typedef struct _MODERNEFFECTMATRIX
{
	sbyte matrix[25];
	BYTE  topEffect;
	BYTE  leftEffect;
	BYTE  rightEffect;
	BYTE  bottomEffect;
	BYTE  cycleCount;	  //low 7 bits
}MODERNEFFECTMATRIX;

typedef  struct _MODERNEFFECT
{
	BYTE EffectID;
	MODERNEFFECTMATRIX EffectMatrix;
	DWORD EffectColor1;
	DWORD EffectColor2;
}MODERNEFFECT;

#ifdef _EFFECTENUM_FULL_H
  TCHAR * ModernEffectNames[]=
#else
  TCHAR * _ModernEffectNames[]=
#endif
{
  _T("Shadow at left"),
  _T("Shadow at right"),
  _T("Outline"),
  _T("Outline smooth"),
  _T("Smooth bump"),
  _T("Contour thin"),
  _T("Contour heavy"),
};

#ifdef _EFFECTENUM_FULL_H
MODERNEFFECTMATRIX ModernEffectsEnum[]={
	{   //Shadow at Left
		{	0,	0,	0,	0,	 0,
			0,	4,  16,  4,   4,
			0,	16,	64, 32, 16,
			0,	4,	32,	32,	16,
			0,	4,	16,	16,	16   },	2,2,2,2,1},
    {   //Shadow at Right
		{	0,	0,	0,	0,	 0,
			4,	4,  16,  4,   0,
		    16,	32,	64, 16,   0,
			16,	32,	32,	4,	 0,
			16,	16,	16,	4,	0   },	2,2,2,2,1},
	{   //Outline
		{	0,	0,	0,	0,	 0,
			0,	16, 16, 16, 0,
			0,	16,	32, 16, 0,
			0,	16,	16,	16,	 0,
			0,	0,	0,	0,	 0   },	1,1,1,1,1},
	
	{  //Outline smooth
		{	4,	4,	4,	4,	 4,
			4,	8,  8,  8,   4,
			4,	8,	32, 8,   4,
			4,	8,	8,	8,	 4,
			4,	4,	4,	4,	 4   },	2,2,2,2,1},
	
	{  //Smooth bump
		{	-2,   2,  2,  2,  2,
			-2,	-16, 16, 16,  2,
			-2,	-16, 48, 16,  2,
			-2,	-16,-16, 16,  2,
			-2,	 -2, -2, -2, -2 },	2,2,2,2,1+0x80},
    {  //Contour thin
        {	0,	0,  0,	0,	 0,
            0, 48, 64, 48,   0,
            0, 64, 64, 64,   0,
            0, 48, 64, 48,	 0,
            0,	0,  0,	0,	 0   },	1,1,1,1,1},
    {  //Contour heavy
        {   8, 16, 16, 16,  8,
           16, 64, 64, 64, 16,
           16, 64, 64, 64, 16,
           16, 64, 64, 64, 16,
            8, 16, 16, 16,  8   },	2,2,2,2,1},

};
#endif
#ifdef _EFFECTENUM_FULL_H
  #define MAXPREDEFINEDEFFECTS sizeof(ModernEffectNames)/sizeof(ModernEffectNames[0])
#else
  #define MAXPREDEFINEDEFFECTS sizeof(_ModernEffectNames)/sizeof(_ModernEffectNames[0])
  extern TCHAR * ModernEffectNames[];
#endif
extern BOOL SkinEngine_ResetTextEffect(HDC);
extern BOOL SkinEngine_SelectTextEffect(HDC hdc, BYTE EffectID, DWORD FirstColor, DWORD SecondColor);