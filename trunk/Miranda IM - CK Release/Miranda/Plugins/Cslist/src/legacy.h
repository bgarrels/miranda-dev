/*
Custom Status List for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
                Copyright (C) 2006-2008 HANAX
                Copyright (C) 2007-2009 jarvis

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

* Legacy functions of older Custom Status List versions.
*/

int cslist_import_v2_status_items( );
int cslist_parse_row( TCHAR *row, int ord );

int cslist_import_v2_status_items( )
{
  // get DB string, parse to statuses, add by helpItem
  DBVARIANT dbv = { DBVT_TCHAR };
  int parseResult;
  int dbLoadResult;
  const TCHAR* rowDelim = _T( "" ); // new line
  TCHAR *row = NULL;

  dbLoadResult = getTString( "listhistory", &dbv );
  if ( dbv.ptszVal )
  {
    int i = getByte( "ItemsCount", DEFAULT_ITEMS_COUNT );
    row = _tcstok( dbv.ptszVal, rowDelim );
    while( row != NULL ) {
	  i++;
      // process current row..
      parseResult = cslist_parse_row( row, i );
      // ..and go to the other, while some remains
      row = _tcstok( NULL, rowDelim );
    }
	setByte( "ItemsCount", i );
  }
  //free( rowDelim );
  //free( row );
  return 0;
}

int cslist_parse_row( TCHAR *row, int ord ) // parse + helpItem
{
  int pIconInt;
  TCHAR pIcon[4], pTitle[EXTRASTATUS_TITLE_LIMIT+2], pMsg[EXTRASTATUS_MESSAGE_LIMIT+2], pFav[4];
  TCHAR scanfFormat[64];

  lstrcpy( pTitle, _T( "" ) );
  lstrcpy( pMsg, _T( "" ) );

  // database row format: "%2[^]%64[^]%2048[^]%2[^]"
  
  mir_sntprintf(
    scanfFormat,
    sizeof( scanfFormat ),
    _T( "%%%d[^]%%%d[^]%%%d[^]%%%d[^]" ), // %% makes %, %d makes number :)
    2,
    EXTRASTATUS_TITLE_LIMIT,
    EXTRASTATUS_MESSAGE_LIMIT,
    2
  );

  _tcscanf( row, scanfFormat, pIcon, pTitle, pMsg, pFav );

  if ( lstrlen( pTitle ) + lstrlen( pMsg ) > 0 )
  {
    char dbSetting[32];
    pIconInt = _ttoi( pIcon );
    mir_snprintf( dbSetting, sizeof( dbSetting ), "Item%dIcon", ord );
    setByte( dbSetting, pIconInt );
    mir_snprintf( dbSetting, sizeof( dbSetting ), "Item%dTitle", ord );
    setTString( dbSetting, pTitle );
    mir_snprintf( dbSetting, sizeof( dbSetting ), "Item%dMessage", ord );
    setTString( dbSetting, pMsg );
    mir_snprintf( dbSetting, sizeof( dbSetting ), "Item%dFavourite", ord );
    setByte( dbSetting, 0 );
  }
  else {
    return FALSE;
  }
  return TRUE;
}


void MessageBoxInt( int value1, int value2 )
{
	TCHAR buf1[64], buf2[64];
	mir_sntprintf( buf1, SIZEOF( buf1 ), TEXT( "%d" ), value1 );
	mir_sntprintf( buf2, SIZEOF( buf2 ), TEXT( "%d" ), value2 );
	MessageBox( NULL, buf2, buf1, MB_OK );
}
