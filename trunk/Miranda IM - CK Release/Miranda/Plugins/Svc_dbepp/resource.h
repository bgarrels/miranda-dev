/*
Database Editor++ for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2003-2011 Bio, Jonathan Gordon

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

#define ICO_REGEDIT                     1
#define ICO_UNICODE                     2
#define ICO_DBE_BUTT                    3
#define ICO_REGUSER                     4
#define ICO_STRING                      5
#define ICO_BINARY                      6
#define ICO_DWORD                       7
#define ICO_BYTE                        8
#define ICO_WORD                        9
#define ICO_HANDLE                      10
#define IDD_MAIN                        101
#define IDR_MAINMENU                    103
#define IDR_CONTEXTMENU                 104
#define ICO_CONTACTS                    106
#define ICO_OFFLINE                     107
#define IDD_ADD_MODULE                  108
#define IDD_EDIT_SETTING                109
#define IDD_WATCH_DIAG                  110
#define IDR_WATCHWINDOWMENU             112
#define ICO_UNLOADED                    115
#define IDD_COPY_MOD                    116
#define IDD_IMPORT                      118
#define IDD_CHANGE_ARRAYSIZE            120
#define IDD_FIND                        121
#define IDD_OPTIONS                     122
#define ICO_KNOWN                       124
#define ICO_KNOWNOPEN                   125
#define ICO_UNKNOWNOPEN                 126
#define ICO_UNKNOWN                     127
#define ICO_SETTINGS                    128
#define ICO_ONLINE                      129
#define IDC_MODULES                     1000
#define IDC_SETTINGS                    1001
#define IDC_MENU                        1002
#define IDC_MODNAME                     1004
#define CHK_ADD2ALL                     1005
#define IDC_MODNAME2                    1005
#define IDC_SETTINGNAME                 1006
#define IDC_SPLITTER                    1006
#define IDC_STRING                      1007
#define IDC_SETTINGNAME2                1007
#define GRP_BASE                        1008
#define CHK_HEX                         1009
#define CHK_DECIMAL                     1010
#define IDC_SETTINGVALUE                1011
#define GRP_TYPE                        1012
#define IDC_SETTINGVALUE2               1012
#define CHK_BYTE                        1013
#define IDC_SETTINGVALUE3               1013
#define CHK_WORD                        1014
#define CHK_DWORD                       1015
#define CHK_STRING                      1016
#define IDC_VARS                        1017
#define IDC_ADDMODNAMESTATIC            1020
#define IDC_CONTACTS                    1021
#define CHK_COPY2ALL                    1022
#define IDC_TEXT                        1025
#define IDC_FIND                        1026
#define IDC_REPLACE                     1027
#define IDC_CASE_SENSITIVE              1027
#define IDC_REQUIRED                    1028
#define IDC_ARRAYSIZE                   1030
#define IDC_CURRENTSIZE                 1032
#define IDC_INFOTEXT                    1033
#define IDC_CRLF                        1035
#define IDC_CASESENSITIVE               1040
#define IDC_LIST                        1041
#define IDC_SEARCH                      1042
#define IDC_EXACT                       1043
#define IDC_EXPANDSETTINGS              1044
#define IDC_USEKNOWNMODS                1045
#define IDC_WARNONDEL                   1047
#define IDC_POPUPS                      1048
#define IDC_POPUPTIMEOUT                1049
#define IDC_COLOUR                      1050
#define IDC_RESTORESETTINGS             1051
#define IDC_BLOB                        1052
#define IDC_FOUND                       1056
#define IDC_SBAR                        1057
#define IDC_ENTIRELY                    1058
#define MENU_REFRESH_MODS               40001
#define MENU_REFRESH_SETS               40002
#define MENU_EXIT                       40003
#define MENU_CHANGE2UNICODE             40004
#define MENU_ADD_UNICODE                40005
#define MENU_RENAME_SET                 40006
#define MENU_EDIT_SET                   40007
#define MENU_CHANGE2BYTE                40008
#define MENU_CHANGE2WORD                40009
#define MENU_CHANGE2DWORD               40010
#define MENU_CHANGE2STRING              40011
#define MENU_DELETE_SET                 40012
#define MENU_DELETE_MOD                 40013
#define MENU_CREATE_MOD                 40014
#define MENU_VIEW_WATCHES               40016
#define MENU_REMALL_WATCHES             40017
#define MENU_ADD_WATCH                  40018
#define MENU_WATCH_ITEM                 40018
#define MENU_WATCH_MOD                  40019
#define MENU_ADD_BLOB                   40020
#define MENU_CLONE_CONTACT              40021
#define MENU_DELETE_CONTACT             40022
#define MENU_ADD_MODULE                 40023
#define MENU_ADD_BYTE                   40024
#define MENU_ADD_WORD                   40025
#define MENU_ADD_DWORD                  40026
#define MENU_ADD_STRING                 40027
#define MENU_SAVE_WATCHES               40028
#define MENU_RENAME_MOD                 40030
#define MENU_COPY_MOD                   40032
#define MENU_USE_MODLIST                40033
#define MENU_USE_POPUPS                 40034
#define MENU_SORT_ORDER                 40035
#define MENU_EXPORTDB                   40036
#define MENU_EXPORTMODULE               40037
#define MENU_EXPORTCONTACT              40038
#define MENU_SAVE_POSITION              40039
#define MENU_IMPORTSETTINGS             40040
#define MENU_ADDCONTACT                 40041
#define MENU_WARNONDEL                  40042
#define MENU_LOAD_WATCHES               40043
#define MENU_WORD_HEX                   40044
#define MENU_DECRYPT                    40045
#define MENU_ENCRYPT                    40046
#define MENU_VIEWDECRYPT                40047
#define MENU_VIEWENCRYPT                40048
#define MENU_LOGTODISK                  40049
#define MENU_FINDANDREPLACE             40050
#define MENU_FINDMODSETTING             40051
#define MENU_DWORD_HEX                  40052
#define MENU_IMPORTFROMFILE             40053
#define MENU_IMPORTFROMTEXT             40054
#define MENU_CHANGEARRAYSIZE            40055
#define MENU_OPTIONS                    40056
#define MENU_DELETE                     40057
#define MENU_REFRESH                    40058
#define MENU_ADDKNOWN                   40059
#define MENU_FILTER_ALL                 40060
#define MENU_FILTER_LOADED              40061
#define MENU_FILTER_UNLOADED            40062
#define MENU_BYTE_HEX                   40063

// Next default values for new objects
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        131
#define _APS_NEXT_COMMAND_VALUE         40064
#define _APS_NEXT_CONTROL_VALUE         1059
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif
