/*
TipperYM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
            Copyright (C) 2006-2009 Scott Ellis
            Copyright (C) 2007-2011 Jan Holub

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

#define IDD_OPT_APPEARANCE              101
#define IDD_SUBST                       103
#define IDD_ITEM                        104
#define IDD_OPT_CONTENT                 105
#define IDD_OPT_SKIN                    106
#define IDI_ITEM                        107
#define IDI_ITEM_ALL                    108
#define IDD_OPT_EXTRA                   109
#define IDD_OPT_TRAYTIP                 110
#define IDD_FAVCONTACTS                 111
#define IDI_UP                          112
#define IDI_DOWN                        113
#define IDI_SEPARATOR                   114
#define IDI_RELOAD                      116
#define IDI_APPLY                       117
#define IDC_ED_WIDTH                    1005
#define IDC_ED_MAXHEIGHT                1006
#define IDC_SPIN_WIDTH                  1007
#define IDC_SPIN_MAXHEIGHT              1008
#define IDC_ED_INDENT                   1009
#define IDC_SPIN_INDENT                 1010
#define IDC_ED_PADDING                  1011
#define IDC_SPIN_PADDING                1012
#define IDC_ED_TRANS                    1013
#define IDC_SPIN_TRANS                  1014
#define IDC_CHK_BORDER                  1015
#define IDC_CHK_ROUNDCORNERS            1016
#define IDC_ED_MINWIDTH                 1017
#define IDC_ED_SPEED                    1017
#define IDC_CHK_AEROGLASS               1018
#define IDC_SPIN_SPEED                  1019
#define IDC_CHK_SHADOW                  1020
#define IDC_SPIN_MINWIDTH               1021
#define IDC_ED_MINHEIGHT                1022
#define IDC_SPIN_MINHEIGHT              1023
#define IDC_ED_SBWIDTH                  1024
#define IDC_SPIN_SBWIDTH                1025
#define IDC_ED_TEXTPADDING              1026
#define IDC_ED_AVSIZE                   1027
#define IDC_SPIN_AVSIZE                 1028
#define IDC_ED_HOVER                    1029
#define IDC_SPIN_HOVER                  1030
#define IDC_SPIN_TEXTPADDING            1031
#define IDC_CMB_ICON                    1032
#define IDC_CMB_AV                      1033
#define IDC_CMB_POS                     1034
#define IDC_ED_OUTAVPADDING             1035
#define IDC_BTN_ADD                     1036
#define IDC_SPIN_OUTAVPADDING           1036
#define IDC_BTN_REMOVE                  1037
#define IDC_CHK_ROUNDCORNERSAV          1037
#define IDC_BTN_UP                      1038
#define IDC_CHK_ORIGINALAVSIZE          1038
#define IDC_BTN_DOWN                    1039
#define IDC_CHK_AVBORDER                1039
#define IDC_CHK_NOFOCUS                 1040
#define IDC_BTN_ADD2                    1040
#define IDC_BTN_EDIT                    1041
#define IDC_ED_TITLEINDENT              1041
#define IDC_LST_SUBST                   1042
#define IDC_SPIN_TITLEINDENT            1042
#define IDC_ED_VALUEINDENT              1043
#define IDC_BTN_REMOVE2                 1043
#define IDC_ED_MODULE                   1044
#define IDC_SPIN_VALUEINDENT            1044
#define IDC_BTN_EDIT2                   1044
#define IDC_CHK_PROTOMOD                1045
#define IDC_ED_INAVPADDING              1045
#define IDC_BTN_SEPARATOR               1045
#define IDC_ED_SETTING                  1046
#define IDC_SPIN_INAVPADDING            1046
#define IDC_CMB_TRANSLATE               1047
#define IDC_ED_LABEL                    1048
#define IDC_CHK_SBAR                    1048
#define IDC_ED_VALUE                    1050
#define IDC_CHK_LINEABOVE               1051
#define IDC_CHK_VALNEWLINE              1052
#define IDC_CMB_LV                      1053
#define IDC_CHK_PARSETIPPERFIRST        1053
#define IDC_CMB_VV                      1054
#define IDC_CMB_LH                      1055
#define IDC_CMB_VH                      1056
#define IDC_CHK_DISABLEINVISIBLE        1059
#define IDC_CHK_RETRIEVEXSTATUS         1060
#define IDC_TREE_EXTRAICONS             1061
#define IDC_CHK_ENABLESMILEYS           1062
#define IDC_CHK_RESIZESMILEYS           1063
#define IDC_CHK_GETSTATUSMSG            1064
#define IDC_CHK_WAITFORCONTENT          1065
#define IDC_STATIC_AVATARSIZE           1085
#define IDC_CMB_EFFECT                  1086
#define IDC_TREE_FIRST_PROTOS           1087
#define IDC_CHK_ENABLETRAYTIP           1088
#define IDC_CHK_HANDLEBYTIPPER          1089
#define IDC_TREE_SECOND_PROTOS          1090
#define IDC_TREE_SECOND_ITEMS           1094
#define IDC_TREE_FIRST_ITEMS            1095
#define IDC_BTN_FAVCONTACTS             1096
#define IDC_CHK_EXPAND                  1097
#define IDC_ED_EXPANDTIME               1098
#define IDC_SPIN_EXPANDTIME             1099
#define IDC_CLIST                       1101
#define IDC_BTN_OK                      1102
#define IDC_BTN_CANCEL                  1103
#define IDC_CHK_HIDEOFFLINE             1104
#define IDC_CHK_APPENDPROTO             1105
#define IDC_CHK_USEPROTOSMILEYS         1106
#define IDC_CHK_ONLYISOLATED            1107
#define IDC_CMB_PRESETITEMS             1111
#define IDC_CHK_LIMITMSG                1113
#define IDC_ED_CHARCOUNT                1114
#define IDC_SPIN_CHARCOUNT              1115
#define IDC_LB_SKINS                    1116
#define IDC_PIC_PREVIEW                 1117
#define IDC_ST_PREVIEW                  1119
#define IDC_CHK_LOADFONTS               1120
#define IDC_CHK_ENABLECOLORING          1121
#define IDC_CHK_LOADPROPORTIONS         1122
#define IDC_BTN_RELOADLIST              1123
#define IDC_BTN_APPLYSKIN               1124
#define IDC_BTN_VARIABLE                1125
#define IDC_BTN_GETSKINS                1125

// Next default values for new objects
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        118
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1127
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif
