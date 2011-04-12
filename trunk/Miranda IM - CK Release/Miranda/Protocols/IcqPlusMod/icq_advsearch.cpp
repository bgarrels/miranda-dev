// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright � 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright � 2001,2002 Jon Keating, Richard Hughes
// Copyright � 2002,2003,2004 Martin �berg, Sam Kothari, Robert Rainwater
// Copyright � 2004,2005,2006,2007 Joe Kucera
// Copyright � 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
// Copyright � 2008,2009,2010 sss
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
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"



static void InitComboBox(HWND hwndCombo, FieldNamesItem *names)
{
    int iItem;
    int i;

    iItem = ComboBoxAddStringUtf(hwndCombo, NULL, 0);
    SendMessage(hwndCombo, CB_SETCURSEL, iItem, 0);

    for (i = 0; ; i++)
    {
        if (names[i].text == NULL)
            break;

        iItem = ComboBoxAddStringUtf(hwndCombo, names[i].text, names[i].code);
    }
}


INT_PTR CALLBACK AdvancedSearchDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_INITDIALOG:
        ICQTranslateDialog(hwndDlg);
        InitComboBox(GetDlgItem(hwndDlg, IDC_GENDER), (FieldNamesItem*)genderField);
        InitComboBox(GetDlgItem(hwndDlg, IDC_AGERANGE), (FieldNamesItem*)agesField);
        InitComboBox(GetDlgItem(hwndDlg, IDC_MARITALSTATUS), (FieldNamesItem*)maritalField);
        InitComboBox(GetDlgItem(hwndDlg, IDC_WORKFIELD), (FieldNamesItem*)occupationField);
        InitComboBox(GetDlgItem(hwndDlg, IDC_ORGANISATION), (FieldNamesItem*)affiliationField);
        InitComboBox(GetDlgItem(hwndDlg, IDC_LANGUAGE), (FieldNamesItem*)languageField);
        InitComboBox(GetDlgItem(hwndDlg, IDC_COUNTRY), (FieldNamesItem*)countryField);
        InitComboBox(GetDlgItem(hwndDlg, IDC_INTERESTSCAT), (FieldNamesItem*)interestsField);
        InitComboBox(GetDlgItem(hwndDlg, IDC_PASTCAT), (FieldNamesItem*)pastField);

        return TRUE;

    case WM_COMMAND:
    {
        switch(LOWORD(wParam))
        {

        case IDOK:
            SendMessage(GetParent(hwndDlg), WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), (LPARAM)GetDlgItem(GetParent(hwndDlg), IDOK));
            break;

        case IDCANCEL:
            //          CheckDlgButton(GetParent(hwndDlg),IDC_ADVANCED,BST_UNCHECKED);
            //          SendMessage(GetParent(hwndDlg),WM_COMMAND,MAKEWPARAM(IDC_ADVANCED,BN_CLICKED),(LPARAM)GetDlgItem(GetParent(hwndDlg),IDC_ADVANCED));
            break;

        default:
            break;

        }
        break;
    }

    default:
        break;
    }

    return FALSE;
}



static DWORD getCurItemData(HWND hwndDlg, UINT iCtrl)
{
    return SendDlgItemMessage(hwndDlg, iCtrl, CB_GETITEMDATA, SendDlgItemMessage(hwndDlg, iCtrl, CB_GETCURSEL, 0, 0), 0);
}



static void searchPackTLVLNTS(PBYTE *buf, int *buflen, HWND hwndDlg, UINT idControl, WORD wType)
{
    char str[512];

    GetDlgItemTextA(hwndDlg, idControl, str, sizeof(str));

    ppackTLVLNTS(buf, buflen, str, wType, 0);
}



static void searchPackTLVWordLNTS(PBYTE *buf, int *buflen, HWND hwndDlg, UINT idControl, WORD w, WORD wType)
{
    char str[512];

    GetDlgItemTextA(hwndDlg, idControl, str, sizeof(str));

    ppackTLVWordLNTS(buf, buflen, w, str, wType, 0);
}



static PBYTE createAdvancedSearchStructureTLV(HWND hwndDlg, int *length)
{
    PBYTE buf = NULL;
    int buflen = 0;
    WORD w;
    BYTE b;

    ppackLEWord(&buf, &buflen, META_SEARCH_GENERIC);       /* subtype: full search */

    searchPackTLVLNTS(&buf, &buflen, hwndDlg, IDC_FIRSTNAME, TLV_FIRSTNAME);
    searchPackTLVLNTS(&buf, &buflen, hwndDlg, IDC_LASTNAME, TLV_LASTNAME);
    searchPackTLVLNTS(&buf, &buflen, hwndDlg, IDC_NICK, TLV_NICKNAME);
    searchPackTLVLNTS(&buf, &buflen, hwndDlg, IDC_EMAIL, TLV_EMAIL);
    searchPackTLVLNTS(&buf, &buflen, hwndDlg, IDC_CITY, TLV_CITY);
    searchPackTLVLNTS(&buf, &buflen, hwndDlg, IDC_STATE, TLV_STATE);
    searchPackTLVLNTS(&buf, &buflen, hwndDlg, IDC_COMPANY, TLV_COMPANY);
    searchPackTLVLNTS(&buf, &buflen, hwndDlg, IDC_DEPARTMENT, TLV_DEPARTMENT);
    searchPackTLVLNTS(&buf, &buflen, hwndDlg, IDC_POSITION, TLV_POSITION);
    searchPackTLVLNTS(&buf, &buflen, hwndDlg, IDC_KEYWORDS, TLV_KEYWORDS);

    ppackTLVDWord(&buf, &buflen, (DWORD)getCurItemData(hwndDlg, IDC_AGERANGE),      TLV_AGERANGE,  0);
    b = (BYTE)getCurItemData(hwndDlg,  IDC_GENDER);
    switch (b)
    {
    case 'F':
        b = 1;
        break;
    case 'M':
        b = 2;
        break;
    default:
        b = 0;
    };
    ppackTLVByte(&buf,  &buflen, b,        TLV_GENDER,    0);
    ppackTLVByte(&buf,  &buflen, (BYTE)getCurItemData(hwndDlg,  IDC_MARITALSTATUS), TLV_MARITAL,   0);
    ppackTLVWord(&buf,  &buflen, (WORD)getCurItemData(hwndDlg,  IDC_LANGUAGE),      TLV_LANGUAGE,  0);
    ppackTLVWord(&buf,  &buflen, (WORD)getCurItemData(hwndDlg,  IDC_COUNTRY),       TLV_COUNTRY,   0);
    ppackTLVWord(&buf,  &buflen, (WORD)getCurItemData(hwndDlg,  IDC_WORKFIELD),     TLV_OCUPATION, 0);

    w = (WORD)getCurItemData(hwndDlg, IDC_PASTCAT);
    searchPackTLVWordLNTS(&buf, &buflen, hwndDlg, IDC_PASTKEY, w, TLV_PASTINFO);

    w = (WORD)getCurItemData(hwndDlg, IDC_INTERESTSCAT);
    searchPackTLVWordLNTS(&buf, &buflen, hwndDlg, IDC_INTERESTSKEY, w, TLV_INTERESTS);

    w = (WORD)getCurItemData(hwndDlg, IDC_ORGANISATION);
    searchPackTLVWordLNTS(&buf, &buflen, hwndDlg, IDC_ORGKEYWORDS, w, TLV_AFFILATIONS);

    w = (WORD)getCurItemData(hwndDlg, IDC_HOMEPAGECAT);;
    if (w != 0xFFFF)
        searchPackTLVWordLNTS(&buf, &buflen, hwndDlg, IDC_HOMEPAGEKEY, w, TLV_HOMEPAGE);
    if (IsDlgButtonChecked(hwndDlg, IDC_ONLINEONLY))
        ppackTLVByte(&buf, &buflen, 1, TLV_ONLINEONLY, 1);


    if (length)
        *length = buflen;

    return buf;
}



PBYTE createAdvancedSearchStructure(HWND hwndDlg, int *length)
{
    if (!hwndDlg)
        return NULL;

    return createAdvancedSearchStructureTLV(hwndDlg, length);
}
