/*
SecureIM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 	2003 Johell
							2005-2009 Baloo

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


HINSTANCE hRtfconv = NULL;
RTFCONVSTRING pRtfconvString = NULL;


BOOL load_rtfconv () {

    hRtfconv = LoadLibrary( "rtfconv.dll" ) ;
    if ( hRtfconv == NULL ) {
   	    hRtfconv = LoadLibrary( "plugins\\rtfconv.dll" ) ;
	    if ( hRtfconv == NULL )
    		return FALSE;
    }

    pRtfconvString = (RTFCONVSTRING) GetProcAddress( hRtfconv, "RtfconvString" ) ;
    if ( pRtfconvString == NULL ) {
        FreeLibrary( hRtfconv ) ;
        return FALSE;
    }

    return TRUE;
}

void free_rtfconv () {

	if ( hRtfconv )
	    FreeLibrary( hRtfconv ) ;
	pRtfconvString = NULL;
	hRtfconv = NULL;
}


void rtfconvA(LPCSTR rtf, LPWSTR plain) {

    pRtfconvString( rtf, plain, 0, 1200, CONVMODE_USE_SYSTEM_TABLE, (strlen(rtf)+1)*sizeof(WCHAR) );
}


void rtfconvW(LPCWSTR rtf, LPWSTR plain) {

    pRtfconvString( rtf, plain, 0, 1200, CONVMODE_USE_SYSTEM_TABLE, (wcslen(rtf)+1)*sizeof(WCHAR) );
}

// EOF
