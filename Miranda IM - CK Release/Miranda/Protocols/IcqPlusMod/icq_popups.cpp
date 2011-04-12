// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin Öberg, Sam Kothari, Robert Rainwater
// Copyright © 2004,2005,2006,2007 Joe Kucera
// Copyright © 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
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
// Revision       : $Revision: 50 $
// Last change on : $Date: 2007-08-28 02:57:00 +0300 (Ð’Ñ‚, 28 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  PopUp Plugin stuff
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"


static INT_PTR CALLBACK DlgProcIcqPopupOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);



static void LoadDBCheckState(HWND hwndDlg, int idCtrl, const char* szSetting, BYTE bDef)
{
    CheckDlgButton(hwndDlg, idCtrl, ICQGetContactSettingByte(NULL, szSetting, bDef));
}

static void StoreDBCheckState(HWND hwndDlg, int idCtrl, const char* szSetting)
{
    ICQWriteContactSettingByte(NULL, szSetting, (BYTE)IsDlgButtonChecked(hwndDlg, idCtrl));
}




static int CALLBACK PopupDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_COMMAND:
        if (HIWORD(wParam) == STN_CLICKED)
        {
            // HANDLE hContact = PUGetContact(hWnd);
            // if (hContact)
            //          NotifyEventHooks(hHookSendMessage, (WPARAM)hContact, 0);
            PUDeletePopUp(hWnd);
            return 1;
        }
        break;
    case WM_CONTEXTMENU:
        PUDeletePopUp(hWnd);
        return 1;
    case UM_INITPOPUP:
        WindowList_Add(hPopUpsList,hWnd,(HANDLE)CallService(MS_POPUP_GETCONTACT,(WPARAM)hWnd,0));
        return 1;
    case UM_FREEPLUGINDATA:
        WindowList_Remove(hPopUpsList,hWnd);
        return 1;
    default:
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}



int ShowPopUpMsg(HANDLE hContact, DWORD dwUin, const char* szTitle, const char* szMsg, BYTE bType)
{
    if (bPopUpService && bPopUpsEnabled)
    {
        POPUPDATAEX ppd = {0};
        POPUPDATAW ppdw = {0};
        LPCTSTR rsIcon;
        HINSTANCE hIcons = NULL;
        HWND hPopUpWnd = NULL;
        char szPrefix[64], szSetting[32], szText [64];

        strcpy(szPrefix, "Popups");
        ppd.iSeconds = 0;

        switch(bType)
        {
        case LOG_NOTE:
            rsIcon = MAKEINTRESOURCE(IDI_INFORMATION);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "0");
            break;

        case LOG_WARNING:
            rsIcon = MAKEINTRESOURCE(IDI_WARNING);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "1");
            break;

        case LOG_ERROR:
            rsIcon = MAKEINTRESOURCE(IDI_ERROR);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "2");
            break;

        case LOG_FATAL:
            rsIcon = MAKEINTRESOURCE(IDI_ERROR);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "3");
            break;

        case POPTYPE_SPAM:
            rsIcon = MAKEINTRESOURCE(IDI_WARNING);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "Spam");
            break;

        case POPTYPE_UNKNOWN:
            rsIcon = MAKEINTRESOURCE(IDI_WARNING);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "Unknown");
            break;

        case POPTYPE_VIS:
            rsIcon = MAKEINTRESOURCE(IDI_INFORMATION);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "Vis");
            break;


        case POPTYPE_FOUND:
            hIcons = hInst;
            rsIcon = MAKEINTRESOURCE(IDI_HIDDEN);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "Found");
            break;

        case POPTYPE_SCAN:
            hIcons = hInst;
            rsIcon = gbScan?MAKEINTRESOURCE(IDI_START):MAKEINTRESOURCE(IDI_STOP);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "Finished");
            break;
        case POPTYPE_CLIENT_CHANGE:
            rsIcon = MAKEINTRESOURCE(IDI_INFORMATION);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "ClientChange");
            break;
        case POPTYPE_INFO_REQUEST:
            rsIcon = MAKEINTRESOURCE(IDI_INFORMATION);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "InfoRequest");
            break;
        case POPTYPE_IGNORE_CHECK:
            rsIcon = MAKEINTRESOURCE(IDI_INFORMATION);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "IgnoreCheck");
            break;
        case POPTYPE_SELFREMOVE:
            rsIcon = MAKEINTRESOURCE(IDI_INFORMATION);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "RemoveHimself");
            break;
        case POPTYPE_AUTH:
            rsIcon = MAKEINTRESOURCE(IDI_INFORMATION);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "Auth");
            break;
        case POPTYPE_XUPDATER:
            rsIcon = MAKEINTRESOURCE(IDI_INFORMATION);
            ppd.colorBack = RGB(0,0,0);
            ppd.colorText = RGB(255,255,255);
            strcat(szPrefix, "XUpdater");
            break;
        default:
            return -1;
        }
        if (!ICQGetContactSettingByte(NULL, "PopupsSysIcons", 1))
        {
            hIcons = hInst;
            rsIcon = MAKEINTRESOURCE(IDI_ICQ);
        }
        ppd.lchIcon = (HICON)LoadImage(hIcons, rsIcon, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_SHARED);
        if (ICQGetContactSettingByte(NULL, "PopupsWinColors", 0))
        {
            ppd.colorText = GetSysColor(COLOR_WINDOWTEXT);
            ppd.colorBack = GetSysColor(COLOR_WINDOW);
        }
        else
        {
            strcpy(szSetting, szPrefix);
            strcat(szSetting, "TextColor");
            ppd.colorText = ICQGetContactSettingDword(NULL, szSetting, ppd.colorText);
            strcpy(szSetting, szPrefix);
            strcat(szSetting, "BackColor");
            ppd.colorBack = ICQGetContactSettingDword(NULL, szSetting, ppd.colorBack);
        }
        strcpy(szSetting, szPrefix);
        strcat(szSetting, "Timeout");
        ppd.iSeconds = ICQGetContactSettingDword(NULL, szSetting, ppd.iSeconds);

        //by chaos.persei @ 9.12.2006 15:25
        //bug - unhiding popups - fix
        if (ppd.iSeconds == 0) ppd.iSeconds = 10;

        hPopUpWnd = WindowList_Find(hPopUpsList,hContact);
        if ((bType == POPTYPE_VIS) && hPopUpWnd)
            return -1;
        if(dwUin && bUinPopup)
            mir_snprintf(szText, sizeof(szText), "%s, %s: (%u)",  szTitle,  ICQTranslateUtf("UIN"), dwUin);
        else
            strcpy(szText,szTitle);

        if (gbUnicodeAPI_dep && ServiceExists(MS_POPUP_ADDPOPUPW))
        {
            // call unicode popup module - only on unicode OS otherwise it will not work properly :(
            // due to Popup Plug bug in ADDPOPUPW implementation
            char str[MAX_PATH];
            wchar_t * tmp = mir_utf8decodeW(ICQTranslateUtfStatic(szText, str, MAX_PATH));
            wcscpy(ppdw.lpwzContactName, tmp);
            mir_free(tmp);
            tmp = mir_utf8decodeW(ICQTranslateUtfStatic(szMsg, str, MAX_PATH));
            wcscpy(ppdw.lpwzText, tmp);
            mir_free(tmp);

            ppdw.lchContact = hContact;
            ppdw.lchIcon = ppd.lchIcon;
            ppdw.colorBack = ppd.colorBack;
            ppdw.colorText = ppd.colorText;
            ppdw.PluginWindowProc = (WNDPROC)PopupDlgProc;
            ppdw.PluginData = NULL;
            ppdw.iSeconds = ppd.iSeconds;

            return CallService(MS_POPUP_ADDPOPUPW, (WPARAM)&ppdw, 0);
        }
        else
        {
            char str[MAX_PATH];

            char * tmp = mir_utf8decodeA(ICQTranslateUtfStatic(szText, str, MAX_PATH));
            strcpy(ppd.lpzContactName, tmp);
            mir_free(tmp);
            tmp = mir_utf8decodeA(ICQTranslateUtfStatic(szMsg, str, MAX_PATH));
            strcpy(ppd.lpzText, tmp);
            mir_free(tmp);

            ppd.lchContact = hContact;
            ppd.PluginWindowProc = (WNDPROC)PopupDlgProc;
            ppd.PluginData = NULL;

            return CallService(MS_POPUP_ADDPOPUPEX, (WPARAM)&ppd, 0);
        }
    }
    return -1; // Failure
}
