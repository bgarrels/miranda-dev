/*
Basic History plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2011-2012 Krzysztof Kral

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

#define IDD_HISTORY                     101
#define IDD_OPT_MAIN                    102
#define IDD_OPT_GROUPLIST               103
#define IDD_OPT_MESSAGES                104
#define IDD_OPT_SEARCHING               105
#define IDD_OPT_EXPORT                  106
#define IDD_OPT_SCHEDULER               107
#define IDD_DLG_TASK                    108
#define IDC_EDIT                        1001
#define IDC_LIST                        1002
#define IDC_DELETEHISTORY               1004
#define IDC_SPLITTER                    1005
#define IDC_FIND_TEXT                   1006
#define IDC_LIST_CONTACTS               1007
#define IDC_SPLITTERV                   1008
#define IDC_SHOWHIDE                    1009
#define IDC_TOOLBAR                     1010
#define IDC_SHOWCONTACTS                1011
#define IDC_NEWONTOP                    1012
#define IDC_SHOWEVENTS                  1013
#define IDC_SHOWTIME                    1014
#define IDC_SHOWNAME                    1015
#define IDC_SHOWMESSAGE                 1016
#define IDC_MESSAGELEN                  1017
#define IDC_MESSAGELEN_DESC             1018
#define IDC_GROUPTIME                   1019
#define IDC_LIMITMESSAGES               1020
#define IDC_SHOWDATE                    1021
#define IDC_SHOWSECOND                  1022
#define IDC_SHOWSMILEYS                 1023
#define IDC_FORLIST                     1024
#define IDC_FORMES                      1025
#define IDC_MATCHCASE                   1026
#define IDC_MATCHWHOLE                  1027
#define IDC_ONLYIN                      1028
#define IDC_ONLYOUT                     1029
#define IDC_ONLYGROUP                   1030
#define IDC_DEFFILTER                   1031
#define IDC_LIST_FILTERS                1032
#define IDC_LIST_EVENTS                 1033
#define IDC_ADD_FILTER                  1034
#define IDC_DELETE_FILTER               1035
#define IDC_FILTER_NAME                 1036
#define IDC_EVENT                       1037
#define IDC_ADD_EVENT                   1038
#define IDC_DELETE_EVENT                1039
#define IDC_SHOWCONTACTGROUPS           1040
#define IDC_TXTENC                      1041
#define IDC_HTML1ENC                    1042
#define IDC_HTML1DATE                   1043
#define IDC_HTML2ENC                    1044
#define IDC_HTML2DATE                   1045
#define IDC_HTML2SHOWSMILEYS            1046
#define IDC_HTML2EXTCSS                 1047
#define IDC_HTML2EXTCSSFILE             1048
#define IDC_LIST_TASKS                  1049
#define IDC_ADD_TASK                    1050
#define IDC_EDIT_TASK                   1051
#define IDC_DELETE_TASK                 1052
#define IDC_TASK_TYPE                   1053
#define IDC_TASK_FILTER                 1054
#define IDC_LIST_CONTACTSEX             1055
#define IDC_EVENT_TIME                  1056
#define IDC_EVENT_UNIT                  1057
#define IDC_TRIGER_TYPE                 1058
#define IDC_EXPORT_TYPE                 1059
#define IDC_EXPORT_TYPE_LABEL           1060
#define IDC_COMPRESS                    1061
#define IDC_EXPORT_PATH                 1062
#define IDC_EXPORT_PATH_LABEL           1063
#define IDC_TASK_STAR                   1064
#define IDC_FTP                         1065
#define IDC_UPLOAD                      1066
#define IDC_FTP_LABEL                   1067
#define IDC_TRIGER_TIME                 1068
#define IDC_TRIGER_TIME_LABEL           1069
#define IDC_TRIGER_WEEK                 1070
#define IDC_TRIGER_WEEK_LABEL           1071
#define IDC_TRIGER_DAY                  1072
#define IDC_TRIGER_DAY_LABEL            1073
#define IDC_TRIGER_DELTA_TIME           1074
#define IDC_TRIGER_DELTA_TIME_LABEL     1075
#define IDC_WINSCP                      1076
#define IDC_WINSCPLOG                   1077
#define IDC_TASK_NAME                   1078
#define IDC_TASK_ACTIVE                 1079
#define IDC_TASK_FILTER_LABEL           1080
#define IDC_EVENT_LABEL                 1081
#define IDC_IMPORT_TYPE                 1082
#define IDC_PASSWORD                    1083
#define IDC_PASSWORD_LABEL              1084
#define IDC_EXPIMP                      1085
#define IDI_INM                         20000
#define IDI_OUTM                        20001
#define IDI_SHOW                        20002
#define IDI_HIDE                        20003
#define IDI_FINDNEXT                    20004
#define IDI_FINDPREV                    20005
#define IDI_STATUS                      20008
#define IDR_CSS                         20009
#define IDR_JS                          20010
#define IDI_PLUSEX                      20011
#define IDI_MINUSEX                     20012
#define IDM_FIND                        40001
#define IDM_CONFIG                      40002
#define IDM_FINDNEXT                    40003
#define IDM_FINDPREV                    40004
#define IDM_MATCHCASE                   40005
#define IDM_MATCHWHOLE                  40006
#define IDM_OPTIONS                     40007
#define IDM_FONTS                       40008
#define IDM_ICONS                       40009
#define IDM_HOTKEYS                     40010
#define IDM_SAVEPOS                     40011
#define IDM_SAVEPOSALL                  40012
#define IDM_ONLYIN                      40013
#define IDM_ONLYOUT                     40014
#define IDM_DELETE                      40015
#define IDM_DELETEGROUP                 40016
#define IDM_DELETEUSER                  40017
#define IDM_MESSAGE                     40018
#define IDM_COPY                        40019
#define IDM_OPENNEW                     40020
#define IDM_OPENEXISTING                40021
#define IDM_COPYLINK                    40022
#define IDM_ONLYGROUP                   40023
#define IDM_QUOTE                       40024
#define IDM_FILTERDEF                   40025
#define IDM_FILTERALL                   40026
#define IDM_EXPORTRHTML                 40027
#define IDM_EXPORTPHTML                 40028
#define IDM_EXPORTTXT                   40029
#define IDM_EXPORTBINARY                40030
#define IDM_IMPORTBINARY                40031
#define IDM_EXPORTDAT                   40032
#define IDM_IMPORTDAT                   40033

#define CUSTOMRES                       300

// Next default values for new objects
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        106
#define _APS_NEXT_COMMAND_VALUE         40024
#define _APS_NEXT_CONTROL_VALUE         1030
#define _APS_NEXT_SYMED_VALUE           106
#endif
#endif
