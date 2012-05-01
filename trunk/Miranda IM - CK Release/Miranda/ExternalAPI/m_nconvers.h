#ifndef __m_nconvers_h__
#define __m_nconvers_h__

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

//replace smiley tags in a rich edit control... 
//wParam = (WPARAM) 0; not used
//lParam = (LPARAM) (NCONVERS_GETICON*) &ncgi;
//return: TRUE if found, FALSE if not

typedef struct 
{
  int cbSize;             // = sizeof(NCONVERS_GETSMILEY)
  char* Protocolname;     // NULL means 'default'
  char* SmileySequence;   // character string containing the smiley 
  HICON SmileyIcon;       // RETURN VALUE: this is filled with the icon handle... 
                          // do not destroy!
  int Smileylength;       //l ength of the smiley that is found.
} NCONVERS_GETICON;

#define MS_NCONVERS_GETSMILEYICON "nConvers/GetSmileyIcon"


#endif // __m_nconvers_h__