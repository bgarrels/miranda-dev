/*
UserinfoEx plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2006-2010 DeathAxe, Yasnovidyashii, Merlin_de, K. Romanov, Kreol

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

#define IDSKIP                          20
#define IDALL                           21
#define IDAPPLY                         22
#define IDI_MAIN                        110
#define IDD_EXPORT                      601
#define IDB_FLAGSPNG                    602
#define IDD_ANNIVERSARY_EDITOR          603
#define IDD_ANNIVERSARY_LIST            604
#define CURSOR_ADDGROUP                 605
#define IDR_VERSION2                    608
#define IDD_CHOSE_PROTOCOL              614
#define IDI_DEFAULT                     618
#define IDD_DETAILS                     1000
#define IDD_CONTACT_GENERAL             1001
#define IDD_CONTACT_ORIGIN              1002
#define IDD_CONTACT_COMPANY             1003
#define IDD_CONTACT_ABOUT               1004
#define CHECK_OPT_FLAGSUNKNOWN          1005
#define IDD_ADDPHONE                    1006
#define CHECK_OPT_FLAGSMSGSTATUS        1006
#define IDD_ADDEMAIL                    1007
#define IDD_MSGBOX                      1008
#define IDD_MSGBOX2                     1008
#define IDD_OPT_ADVANCED                1009
#define IDD_OPT_POPUP                   1010
#define IDD_CONTACT_HOME                1011
#define IDD_CONTACT_ADDRESS             1011
#define IDD_CONTACT_PROFILE             1012
#define IDD_OPT_REMINDER                1015
#define IDD_OPT_T                       1015
#define IDD_OPT_DETAILSDLG              1016
#define TXT_TIME                        1017
#define IDD_CONTACT_HISTORY             1017
#define TXT_ABOUT                       1018
#define IDD_COPYPROGRESS                1019
#define IDD_EXPORT_DATAHISTORY          1020
#define IDD_OPT_CLIST                   1021
#define IDD_OPT_COMMON                  1021
#define IDD_CONTACT_ANNIVERSARY         1022
#define IDD_REFRESHDETAILS              1023
#define IDD_MSGBOXDUMMI                 1024
#define STATIC_WHITERECT                1100
#define STATIC_TREE                     1101
#define STATIC_LINE1                    1102
#define STATIC_LINE2                    1103
#define ICO_DLGLOGO                     1105
#define ICO_DLGLOGO2                    1106
#define ICO_MSGDLG                      1106
#define ICO_ADDRESS                     1107
#define ICO_FEMALE                      1108
#define ICO_MALE                        1109
#define ICO_MARITAL                     1110
#define ICO_BIRTHDAY                    1111
#define ICO_PASSWORD                    1112
#define ICO_CLOCK                       1113
#define TXT_NAME                        1114
#define TXT_DESCRIPTION                 1115
#define TXT_UPDATING                    1116
#define TXT_NICK                        1119
#define TXT_PASSWORD                    1120
#define TXT_AGE                         1121
#define TXT_REMIND                      1122
#define TXT_FEMALE                      1122
#define TXT_REMIND2                     1123
#define TXT_MALE                        1123
#define TXT_REMIND3                     1124
#define TXT_NUMCONTACT                  1124
#define TXT_NUMBIRTH                    1125
#define TXT_REMIND4                     1125
#define TXT_REMIND5                     1126
#define TXT_MESSAGE                     1126
#define TXT_OPT_CLR_NORMAL              1127
#define TXT_REMIND6                     1127
#define TXT_OPT_CLR_USER                1128
#define TXT_REMIND_LASTCHECK            1128
#define TXT_OPT_CLR_BOTH                1129
#define TXT_REMIND7                     1129
#define TXT_OPT_CLR_CHANGED             1130
#define TXT_REMIND8                     1130
#define TXT_OPT_POPUP_CLR_BACK          1131
#define TXT_OPT_CLR_META                1131
#define TXT_REMIND9                     1131
#define TXT_OPT_POPUP_CLR_TEXT          1132
#define TXT_OPT_POPUP_CLR_ABACK         1133
#define TXT_OPT_POPUP_CLR_ATEXT         1134
#define EDIT_TITLE                      1201
#define EDIT_FIRSTNAME                  1202
#define EDIT_SECONDNAME                 1203
#define EDIT_LASTNAME                   1204
#define EDIT_PREFIX                     1205
#define EDIT_NICK                       1206
#define EDIT_DISPLAYNAME                1207
#define EDIT_PASSWORD                   1208
#define EDIT_STREET                     1209
#define EDIT_ZIP                        1210
#define EDIT_CITY                       1211
#define EDIT_STATE                      1212
#define EDIT_COUNTRY                    1213
#define EDIT_POSITION                   1214
#define EDIT_SUPERIOR                   1215
#define EDIT_ASSISTENT                  1216
#define EDIT_COMPANY                    1217
#define EDIT_DEPARTMENT                 1218
#define EDIT_ANNIVERSARY_DATE           1219
#define EDIT_DEPARTMENT2                1219
#define EDIT_POSITION2                  1219
#define EDIT_OFFICE                     1219
#define EDIT_AGE                        1220
#define EDIT_REMIND                     1221
#define EDIT_REMIND2                    1222
#define EDIT_LANG1                      1223
#define EDIT_REMIND_SOUNDOFFSET         1223
#define EDIT_LANG2                      1224
#define EDIT_LANG3                      1225
#define EDIT_MARITAL                    1226
#define EDIT_PARTNER                    1227
#define EDIT_TIMEZONE                   1228
#define EDIT_HOMEPAGE                   1229
#define EDIT_CATEGORY                   1230
#define EDIT_AREA                       1231
#define EDIT_NUMBER                     1232
#define EDIT_PHONE                      1233
#define EDIT_EMAIL                      1234
#define EDIT_ABOUT                      1235
#define EDIT_NOTES                      1236
#define EDIT_OCCUPATION                 1237
#define EDIT_DELAY                      1238
#define LIST_PROFILE                    1239
#define LIST_PHONE                      1240
#define LIST_EMAIL                      1241
#define EDIT_TABAPPEAREANCE             1242
#define SPIN_AGE                        1301
#define SPIN_REMIND                     1302
#define SPIN_REMIND2                    1303
#define SPIN_REMIND_SOUNDOFFSET         1304
#define CLR_NORMAL                      1400
#define CLR_USER                        1401
#define CLR_BOTH                        1402
#define CLR_CHANGED                     1403
#define CLR_BBACK                       1404
#define CLR_META                        1404
#define CLR_BTEXT                       1405
#define CLR_ATEXT                       1407
#define IDC_CUSTOM1                     1502
#define BTN_DELETE                      1502
#define BTN_SEARCH                      1502
#define IDC_HEADERBAR                   1502
#define IDC_CUSTOM2                     1503
#define IDC_TREE1                       1504
#define IDC_TREE                        1504
#define IDC_COMBO1                      1506
#define EDIT_SORT                       1506
#define EDIT_EXTRAICON                  1506
#define EDIT_METASUBCONTACTS            1506
#define EDIT_HISTORY_GROUPING           1506
#define COMBO_VIEW                      1506
#define EDIT_BIRTHMODULE                1507
#define EDIT_PATH                       1512
#define BTN_BROWSE                      1513
#define BTN_CHECK                       1514
#define BTN_UNCHECK                     1515
#define BTN_OPT_RESET                   1515
#define TEXT_ZODIAC                     1516
#define LIST_DROPDOWN                   1517
#define TXT_CHANGED                     1522
#define TXT_MODULE                      1523
#define CLR_ABACK                       1523
#define IDC_LINE                        1526
#define EDIT_REMIND_ENABLED             1528
#define IDC_ZODIAC                      1529
#define STATIC_ADDRESS                  1530
#define LIST_META                       1531
#define BTN_DEFAULT                     1533
#define EDIT_MESSAGE                    1538
#define BTN_HISTORY_PASSWORD            1548
#define CHECK_HISTORY_ENABLED           1550
#define TXT_HISTORY1                    1551
#define GROUP_STATS                     1553
#define BTN_BROWSEDIR                   1555
#define CHECK_OPT_GETCONTACTINFO_ENABLED 1560
#define CHECK_OPT_AUTOTIMEZONE          1561
#define CHECK_OPT_GETCONTACTINFO_ENABLED2 1562
#define CHECK_OPT_SREMAIL_ENABLED       1562
#define CHECK_OPT_SENDSMS_MENUITEMS3    1563
#define IDNONE                          1565
#define CHECK_OPT_READONLYLABEL         1566
#define TXT_DATEADDED                   1566
#define IDC_PROGRESS                    1567
#define IDC_CHECK1                      1567
#define IDC_PROGRESS2                   1568
#define IDC_CHECK2                      1568
#define STATIC_OPT_METAGROUP            1568
#define TXT_CONTACT                     1569
#define EDIT_DAYS                       1569
#define TXT_SETTING                     1570
#define CHECK_DAYS                      1570
#define IDC_INFO                        1571
#define TXT_DAYS                        1571
#define CHECK_POPUP                     1572
#define GROUP_FILTER                    1573
#define GROUP_REMINDER                  1574
#define COMBO_OPT_GENDER                1575
#define COMBO_OPT_FLAGS                 1576
#define TXT_OPT_GENDER                  1577
#define GROUP_OPT_EXTRAICONS            1578
#define TXT_OPT_DEFAULTICONS            1579
#define TXT_OPT_DEFAULTICONS2           1580
#define ICO_COUNTRY                     1580
#define GROUP_OPT_EXTRAICONS2           1581
#define IDC_PERCENT                     1581
#define TITLE_ZODIAC                    1582
#define TXT_OPT_GENDER2                 1582
#define TXT_OPT_FLAGS                   1582
#define TITLE_AGE                       1583
#define FRAME_AGE                       1584
#define IDC_STATIC_GROUP                1585
#define CHECK_OPT_GENDER                1586
#define TXT_OPT_EXTRAICONS              1587
#define BTN_PREVIEW                     1588
#define IDC_PAGETITLE                   1589
#define RADIO_REMIND1                   1590
#define RADIO_REMIND2                   1591
#define IDC_PAGETITLEBG                 1591
#define RADIO_REMIND3                   1592
#define IDC_PAGETITLEBG2                1592
#define CHECK_OPT_MI_MAIN               1610
#define TXT_OPT_MI_MAIN                 1611
#define RADIO_OPT_MI_MAIN_NONE          1612
#define RADIO_OPT_MI_MAIN_ALL           1613
#define RADIO_OPT_MI_MAIN_EXIMPORT      1614
#define CHECK_OPT_MI_CONTACT            1620
#define TXT_OPT_MI_CONTACT              1621
#define RADIO_OPT_MI_CONTACT_NONE       1622
#define RADIO_OPT_MI_CONTACT_ALL        1623
#define RADIO_OPT_MI_CONTACT_EXIMPORT   1624
#define CHECK_OPT_MI_GROUP              1630
#define TXT_OPT_MI_GROUP                1631
#define RADIO_OPT_MI_GROUP_NONE         1632
#define RADIO_OPT_MI_GROUP_ALL          1633
#define RADIO_OPT_MI_GROUP_EXIMPORT     1634
#define CHECK_OPT_MI_SUBGROUP           1640
#define TXT_OPT_MI_SUBGROUP             1641
#define RADIO_OPT_MI_SUBGROUP_NONE      1642
#define RADIO_OPT_MI_SUBGROUP_ALL       1643
#define RADIO_OPT_MI_SUBGROUP_EXIMPORT  1644
#define CHECK_OPT_MI_STATUS             1650
#define TXT_OPT_MI_STATUS               1651
#define RADIO_OPT_MI_STATUS_NONE        1652
#define RADIO_OPT_MI_STATUS_ALL         1653
#define RADIO_OPT_MI_STATUS_EXIMPORT    1654
#define CHECK_OPT_MI_ACCOUNT            1660
#define TXT_OPT_MI_ACCOUNT              1661
#define RADIO_OPT_MI_ACCOUNT_NONE       1662
#define RADIO_OPT_MI_ACCOUNT_ALL        1663
#define RADIO_OPT_MI_ACCOUNT_EXIMPORT   1664
#define CHECK_OPT_MI_RESTART            1690
#define BTN_UPDATE                      40001
#define BTN_ADD                         40002
#define BTN_IMPORT                      40002
#define BTN_ADD_PHONE                   40003
#define BTN_IMPORT2                     40003
#define BTN_EXPORT                      40003
#define BTN_MENU                        40003
#define BTN_ADD_intEREST                40004
#define BTN_EDIT                        40004
#define BTN_ADD_AFFLIATION              40005
#define BTN_ADD_PAST                    40006
#define BTN_EDIT_CAT                    40007
#define BTN_EDIT_VAL                    40008
#define BTN_EDIT_MAIL                   40009
#define BTN_EDIT_PHONE                  40010
#define BTN_DEL                         40011
#define BTN_DEL_MAIL                    40012
#define BTN_DELALL                      40012
#define BTN_DEL_PHONE                   40013
#define BTN_COPY_MAIL                   40014
#define BTN_COPY_PHONE                  40015
#define BTN_GOTO                        40016
#define BTN_OK                          40017
#define BTN_CANCEL                      40018
#define BTN_APPLY                       40019
#define CHECK_REMIND                    40101
#define CHECK_REMIND_MI                 40102
#define CHECK_REMIND_FLASHICON          40103
#define CHECK_SMS                       40104
#define CHECK_REMIND_STARTUP            40104
#define CHECK_OPT_DETECTUTF             40105
#define CHECK_REMIND_SECURED            40105
#define CHECK_REMIND_HIDDEN             40106
#define CHECK_REMIND_VISIBLEONLY        40106
#define CHECK_OPT_ICOVERSION            40108
#define CHECK_OPT_HOMEPAGEICON          40108
#define CHECK_OPT_CLR                   40109
#define CHECK_OPT_REPLACECONTACTS       40109
#define CHECK_OPT_GROUPS                40110
#define CHECK_OPT_EMAILICON             40110
#define CHECK_OPT_SORTTREE              40111
#define CHECK_OPT_PHONEICON             40111
#define CHECK_OPT_READONLY              40112
#define CHECK_OPT_ZODIACAVATAR          40112
#define CHECK_OPT_CHANGEMYDETAILS       40113
#define CHECK_OPT_METACPY               40114
#define CHECK_OPT_AEROADAPTION          40114
#define CHECK_OPT_METASCAN              40115
#define CHECK_OPT_POPUP_WINCLR          40116
#define CHECK_OPT_BUTTONICONS           40116
#define CHECK_OPT_POPUP_DEFCLR          40117
#define CHECK_OPT_POPUP_ENABLED         40118
#define CHECK_OPT_POPUP_AWINCLR         40119
#define CHECK_OPT_POPUP_ADEFCLR         40120
#define RADIO_OPT_POPUP_DEFAULT         40121
#define RADIO_OPT_POPUP_CUSTOM          40122
#define RADIO_OPT_POPUP_PERMANENT       40123
#define RADIO_MALE                      40124
#define CHECK_OPT_POPUP_ENABLED2        40124
#define CHECK_OPT_POPUP_PROGRESS        40124
#define RADIO_FEMALE                    40125
#define CHECK_OPT_POPUP_PROGRESS2       40125
#define CHECK_OPT_POPUP_MSGBOX          40125

// Next default values for new objects
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NO_MFC                     1
#define _APS_NEXT_RESOURCE_VALUE        623
#define _APS_NEXT_COMMAND_VALUE         40201
#define _APS_NEXT_CONTROL_VALUE         1589
#define _APS_NEXT_SYMED_VALUE           601
#endif
#endif
