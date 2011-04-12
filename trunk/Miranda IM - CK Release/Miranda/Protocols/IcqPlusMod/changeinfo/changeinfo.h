// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright � 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright � 2001,2002 Jon Keating, Richard Hughes
// Copyright � 2002,2003,2004 Martin �berg, Sam Kothari, Robert Rainwater
// Copyright � 2004,2005,2006,2007 Joe Kucera
// Copyright � 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// -----------------------------------------------------------------------------
//
// File name      : $Source$
// Revision       : $Revision: 36 $
// Last change on : $Date: 2007-08-05 03:45:10 +0300 (Вс, 05 авг 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  ChangeInfo Plugin stuff
//
// -----------------------------------------------------------------------------


#ifndef __CHANGEINFO_H
#define __CHANGEINFO_H


#ifndef AW_SLIDE
#define SPI_GETCOMBOBOXANIMATION 0x1004
#ifndef AW_SLIDE
#define AW_SLIDE      0x40000
#endif
#ifndef AW_ACTIVATE
#define AW_ACTIVATE      0x20000
#endif
#define AW_VER_POSITIVE    0x4
#define UDM_SETPOS32            (WM_USER+113)
#define UDM_GETPOS32            (WM_USER+114)
#endif


#define LI_DIVIDER       0
#define LI_STRING        1
#define LI_LIST          2
#define LI_LONGSTRING    3
#define LI_NUMBER        4
#define LIM_TYPE         0x0000FFFF
#define LIF_ZEROISVALID  0x80000000
#define LIF_SIGNED       0x40000000
#define LIF_PASSWORD     0x20000000
#define LIF_CHANGEONLY   0x10000000

extern char Password[10];
extern HANDLE hUpload[2];
extern HWND hwndList;
extern HFONT hListFont;
extern int iEditItem;

typedef struct
{
    char *szDescription;
    unsigned displayType;    //LI_ constant
    int dbType;              //DBVT_ constant
    char *szDbSetting;
    void *pList;
    int listCount;
    LPARAM value;
    int changed;
} SettingItem;

typedef struct
{
    int id;
    char *szValue;
} ListTypeDataItem;

// contants.c
extern SettingItem setting[];
extern const int settingCount;

//main.c
int InitChangeDetails(WPARAM wParam,LPARAM lParam);

//dlgproc.c
INT_PTR CALLBACK ChangeInfoDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
char* GetItemSettingText(int i, char *buf, size_t bufsize);

//db.c
void LoadSettingsFromDb(int keepChanged);
void FreeStoredDbSettings(void);
void ClearChangeFlags(void);
int ChangesMade(void);
int SaveSettingsToDb(HWND hwndDlg);

//editstring.c
void BeginStringEdit(int iItem,RECT *rc,int i,WORD wVKey);
void EndStringEdit(int save);
int IsStringEditWindow(HWND hwnd);
char *BinaryToEscapes(char *str);

//editlist.c
void BeginListEdit(int iItem,RECT *rc,int i,WORD wVKey);
void EndListEdit(int save);
int IsListEditWindow(HWND hwnd);

//upload.c
int StringToListItemId(const char *szSetting,int def);
int UploadSettings(HWND hwndParent);


#endif /* __CHANGEINFO_H */
