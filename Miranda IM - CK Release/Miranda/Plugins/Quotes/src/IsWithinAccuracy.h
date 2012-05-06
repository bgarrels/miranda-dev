#ifndef __C8C6FB80_D66B_4382_8FAB_E92C83F77BB8_IsWithinAcuracy_h__
#define __C8C6FB80_D66B_4382_8FAB_E92C83F77BB8_IsWithinAcuracy_h__

/*
Quotes (Forex) plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
Copyright (C) Dioksin

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

inline bool IsWithinAccuracy(double dValue1, double dValue2, double dAccuracy = 1e-4)
{
	double dDifference = dValue1 - dValue2 ;

	if((-dAccuracy <= dDifference) && (dDifference <= dAccuracy))
		return true ;
	else
		return false ;
}


#endif //__C8C6FB80_D66B_4382_8FAB_E92C83F77BB8_IsWithinAcuracy_h__
