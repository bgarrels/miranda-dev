/*
Name_day plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
Copyright (c) 2001-2004 Luca Santarelli
Copyright (c) 2005-2007 Vasilich
Copyright (c) 2007-2011 yaho
Copyright (c) 2011-2012 Mataes

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

#define IDD_OPT_XPOPUP                  101
#define IDD_INFO_SOUNDS                 102
#define IDD_OPT_GENERAL                 103
#define IDI_RESET                       109
#define IDI_VARIABLES                   110
#define IDD_AUTODISABLE                 113
#define IDD_OPT_FILTERS                 114
#define IDD_OPT_POPUP                   115
#define IDD_OPT_XLOG                    116
#define IDI_POPUP                       117
#define IDD_OPT_SMPOPUP                 117
#define IDI_SOUND                       118
#define IDI_LOGGING                     119
#define IDI_XSTATUS                     120
#define IDI_DISABLEALL                  121
#define IDI_ENABLEALL                   122
#define IDI_NOTIFICATION_OFF            123
#define IDI_NOTIFICATION_ON             124
#define IDI_STATUS_MESSAGE              125
#define IDC_STATUS_STATIC_INFINITE      1002
#define IDC_STATUS_STATIC_DEFAULT       1003
#define IDC_TIMEOUT_VALUE_SPIN          1004
#define IDC_STATUS_STATIC_SEC           1005
#define IDC_STATUS_LC                   1006
#define IDC_USEWINCOLORS                1007
#define IDC_STATUS_RC                   1008
#define IDC_READAWAYMSG                 1011
#define IDC_CHK_DISABLEMUSIC            1011
#define IDC_SHOWPREVIOUSSTATUS          1012
#define IDC_CHK_CUTMSG                  1012
#define IDC_USEPOPUPCOLORS              1013
#define IDC_ED_MSGLEN                   1013
#define IDC_ED_TOPENING                 1018
#define IDC_ED_TCHANGE                  1024
#define IDC_ED_TCHANGEMSG               1025
#define IDC_BT_VARIABLES                1028
#define IDC_BT_RESET                    1029
#define IDC_PREVIEW                     1030
#define IDC_CHANGE                      1031
#define IDC_CHK_MSGCHANGE               1031
#define IDC_DELETE                      1032
#define IDC_CHK_XSTATUSCHANGE           1032
#define IDC_CHK_OPENING                 1033
#define IDC_UD_MSGLEN                   1034
#define IDC_CHK_PGLOBAL                 1035
#define IDC_CHK_SGLOBAL                 1036
#define IDC_CHK_ONLYGLOBAL              1037
#define IDC_HYPERLINK                   1038
#define IDC_CHK_REMOVE                  1039
#define IDC_ED_TREMOVE                  1040
#define IDC_ED_TDELIMITER               1042
#define IDC_INDSNDLIST                  1044
#define IDC_HIDDENCONTACTSTOO           1052
#define IDC_USEINDIVSOUNDS              1053
#define IDC_OK                          1055
#define IDC_CANCEL                      1056
#define IDC_CONFIGUREAUTODISABLE        1057
#define IDC_AUTODISABLE                 1060
#define IDC_LOG                         1061
#define IDC_TIMEOUT_VALUE               1066
#define IDC_BLINKICON                   1067
#define IDC_BLINKICON_STATUS            1068
#define IDC_SOUNDICON                   1069
#define IDC_POPUPICON                   1070
#define IDC_CHECK_NOTIFYSOUNDS          1071
#define IDC_CHECK_NOTIFYPOPUPS          1073
#define IDC_TEXT_ENABLE_IS              1074
#define IDC_USEOWNCOLORS                1076
#define IDC_SHOWALTDESCS                1077
#define IDC_LOGGINGICON                 1078
#define IDC_SHOWSTATUS                  1078
#define IDC_XSTATUSICON                 1079
#define IDC_DISABLEALLICON              1080
#define IDC_ENABLEALLICON               1081
#define IDC_SHOWGROUP                   1082
#define IDC_STATUSMESSAGEICON           1082
#define IDC_ONCONNECT                   1101
#define IDC_PUIGNOREREMOVE              1102
#define IDC_POPUPTEXT                   1115
#define IDC_PROTOCOLLIST                1207
#define IDC_LOGFILE                     2029
#define IDC_BT_CHOOSELOGFILE            2030
#define IDC_BT_VIEWLOG                  2031
#define IDC_CHK_LOGGING                 2034
#define IDC_CHK_SAVETOHISTORY           2039
#define IDC_CHK_PREVENTIDENTICAL        2040
#define IDC_CHK_OFFLINE                 40071
#define IDC_CHK_ONLINE                  40072
#define IDC_CHK_AWAY                    40073
#define IDC_CHK_DND                     40074
#define IDC_CHK_NA                      40075
#define IDC_CHK_OCCUPIED                40076
#define IDC_CHK_FREECHAT                40077
#define IDC_CHK_INVISIBLE               40078
#define IDC_CHK_ONTHEPHONE              40079
#define IDC_CHK_OUTTOLUNCH              40080
#define IDC_CHK_XSTATUS                 40081
#define IDC_CHK_STATUS_MESSAGE          40082
#define IDC_CHK_FROMOFFLINE             40083
#define IDC_OFFLINE_TX                  41071
#define IDC_ONLINE_TX                   41072
#define IDC_AWAY_TX                     41073
#define IDC_DND_TX                      41074
#define IDC_NA_TX                       41075
#define IDC_OCCUPIED_TX                 41076
#define IDC_FREEFORCHAT_TX              41077
#define IDC_INVISIBLE_TX                41078
#define IDC_ONTHEPHONE_TX               41079
#define IDC_OUTTOLUNCH_TX               41080
#define IDC_XSTATUS_TX                  41081
#define IDC_STATUSMESSAGE_TX            41082
#define IDC_OFFLINE_BG                  42071
#define IDC_ONLINE_BG                   42072
#define IDC_CHK_ONLINE2                 42072
#define IDC_AWAY_BG                     42073
#define IDC_CHK_AWAY2                   42073
#define IDC_DND_BG                      42074
#define IDC_CHK_DND2                    42074
#define IDC_NA_BG                       42075
#define IDC_CHK_NA2                     42075
#define IDC_OCCUPIED_BG                 42076
#define IDC_CHK_OCCUPIED2               42076
#define IDC_FREEFORCHAT_BG              42077
#define IDC_CHK_FREECHAT2               42077
#define IDC_INVISIBLE_BG                42078
#define IDC_CHK_INVISIBLE2              42078
#define IDC_ONTHEPHONE_BG               42079
#define IDC_CHK_ONTHEPHONE2             42079
#define IDC_OUTTOLUNCH_BG               42080
#define IDC_CHK_OUTTOLUNCH2             42080
#define IDC_XSTATUS_BG                  42081
#define IDC_STATUSMESSAGE_BG            42082

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        111
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1044
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif
