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

#include "headers.h"

HWND hwnd2mainWindow;
int Order;
HIMAGELIST himl2;
int Hex;

#define GC_SPLITTERMOVED		(WM_USER+101)

extern BOOL bServiceMode;

static WNDPROC SettingListSubClass, ModuleTreeSubClass, SplitterSubClass;

void moduleListWM_NOTIFY(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
void SettingsListWM_NOTIFY(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

int DialogResize(HWND hwnd,LPARAM lParam,UTILRESIZECONTROL *urc)
{
   switch(urc->wId)
   {
       case IDC_MODULES:
           urc->rcItem.right = lParam-3;
           urc->rcItem.top = 0;
           urc->rcItem.left = 0;
           urc->rcItem.bottom = urc->dlgNewSize.cy;
           return RD_ANCHORX_CUSTOM|RD_ANCHORY_CUSTOM;
       case IDC_SPLITTER:
           urc->rcItem.top = 0;
           urc->rcItem.bottom = urc->dlgNewSize.cy;
           urc->rcItem.right = lParam;
           urc->rcItem.left =  lParam-3;
           return RD_ANCHORX_CUSTOM|RD_ANCHORY_CUSTOM;
       case IDC_SETTINGS:
           urc->rcItem.top = 0;
           urc->rcItem.bottom = urc->dlgNewSize.cy;
           urc->rcItem.left = lParam;
           urc->rcItem.right = urc->dlgNewSize.cx;
           return RD_ANCHORX_CUSTOM|RD_ANCHORY_CUSTOM;
       case IDC_VARS:
           urc->rcItem.top = 0;
           urc->rcItem.bottom = urc->dlgNewSize.cy;
           urc->rcItem.left = 0;
           urc->rcItem.right = urc->dlgNewSize.cx;
           return RD_ANCHORY_CUSTOM|RD_ANCHORX_CUSTOM;
   }
   return RD_ANCHORX_LEFT|RD_ANCHORY_TOP;

}

static LRESULT CALLBACK SplitterSubclassProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg) {
		case WM_NCHITTEST:
			return HTCLIENT;
		case WM_SETCURSOR:
		{	RECT rc;
			GetClientRect(hwnd,&rc);
			SetCursor(rc.right>rc.bottom?LoadCursor(NULL, IDC_SIZENS):LoadCursor(NULL, IDC_SIZEWE));
			return TRUE;
		}
		case WM_LBUTTONDOWN:
			SetCapture(hwnd);
			return 0;
		case WM_MOUSEMOVE:
			if(GetCapture()==hwnd) {
				RECT rc;
				GetClientRect(hwnd,&rc);
				SendMessage(GetParent(hwnd),GC_SPLITTERMOVED,rc.right>rc.bottom?(short)HIWORD(GetMessagePos())+rc.bottom/2:(short)LOWORD(GetMessagePos())+rc.right/2,(LPARAM)hwnd);
			}
			return 0;
		case WM_LBUTTONUP:
			ReleaseCapture();
			return 0;
	}
	return CallWindowProc(SplitterSubClass,hwnd,msg,wParam,lParam);
}
LRESULT CALLBACK ModuleTreeSubclassProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg) {
		case WM_RBUTTONDOWN:
		{
			TVHITTESTINFO hti;
			hti.pt.x=(short)LOWORD(GetMessagePos());
			hti.pt.y=(short)HIWORD(GetMessagePos());
			ScreenToClient(hwnd,&hti.pt);

			if(TreeView_HitTest(hwnd,&hti))
			{
				if(hti.flags&TVHT_ONITEM)
					TreeView_SelectItem(hwnd, hti.hItem);
			}
		}
		break;
		case WM_CHAR:
			if (GetKeyState(VK_CONTROL)&0x8000 && wParam == 6)
				CreateDialog(hInst, MAKEINTRESOURCE(IDD_FIND), hwnd, FindWindowDlgProc);
			break;
		case WM_KEYUP:
		{
			if (wParam == VK_DELETE ||
				wParam == VK_F2 ||
				//wParam == VK_UP ||
				//wParam == VK_DOWN ||
				wParam == VK_F5 ||
				wParam == VK_F3)

			{
				TVITEM tvi;
				char module[256];
				tvi.mask=TVIF_HANDLE|TVIF_PARAM|TVIF_TEXT;
				tvi.hItem=TreeView_GetSelection(hwnd);
				tvi.pszText = module;
				tvi.cchTextMax = 255;
				if (TreeView_GetItem(hwnd,&tvi) && tvi.lParam)
				{
					ModuleTreeInfoStruct *mtis = (ModuleTreeInfoStruct*)tvi.lParam;
					HANDLE hContact = mtis->hContact;
					if (wParam == VK_DELETE)
					{
						if ((mtis->type) & MODULE)
						{
							if (deleteModule(module, hContact,0))
							{
								mir_free(mtis);
								TreeView_DeleteItem(hwnd,tvi.hItem);
							}
						}
						else if ((mtis->type == CONTACT) && hContact)
						{
							char msg[1024];
							mir_snprintf(msg, SIZEOF(msg), Translate("Are you sure you want to delete contact \"%s\"?"), module);
							if (DBGetContactSettingByte(NULL,"CList", "ConfirmDelete",1))
							{
								if (MessageBox(0,msg, Translate("Confirm Contact Delete"), MB_YESNO|MB_ICONEXCLAMATION) == IDNO)
									break;
							}
							CallService(MS_DB_CONTACT_DELETE, (WPARAM)hContact,0);
							freeTree(hwnd,mtis->hContact);
							TreeView_DeleteItem(hwnd,tvi.hItem);
						}
					}
					else if (wParam == VK_F2 && (mtis->type == MODULE || mtis->type == UNKNOWN_MODULE))
						TreeView_EditLabel(hwnd,tvi.hItem);
					else if (wParam == VK_F5)
					{
						refreshTree(1);
						break;
					}

					else if (wParam == VK_F3)
					{
						CreateDialog(hInst, MAKEINTRESOURCE(IDD_FIND), hwnd, FindWindowDlgProc);
						break;
					}
				}
			}
		}
		break;
		default:break;
	}
	return CallWindowProc(ModuleTreeSubClass,hwnd,msg,wParam,lParam);
}
static LRESULT CALLBACK SettingListSubclassProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg) {
		case WM_CHAR:
			if (GetKeyState(VK_CONTROL)&0x8000 && wParam == 6)
				CreateDialog(hInst, MAKEINTRESOURCE(IDD_FIND), hwnd, FindWindowDlgProc);
			break;
		case WM_KEYDOWN:
			if (wParam == VK_DELETE || wParam == VK_F5 || (wParam == VK_F2 && ListView_GetSelectedCount(hwnd) == 1))
			{
				char *module, setting[256];
				HANDLE hContact;
				SettingListInfo* sli = (SettingListInfo*)GetWindowLongPtr(hwnd,GWLP_USERDATA);
				if (!sli) break;
				hContact = sli->hContact;
				module = sli->module;
				ListView_GetItemText(hwnd, ListView_GetSelectionMark(hwnd), 0, setting, 256);

				if (wParam == VK_F2)
					editSetting(hContact,module, setting);
				else if (wParam == VK_F5)
				{
					char *szModule = mir_tstrdup(module); // need to do this, otheriwse the setlist stays empty
					PopulateSettings(hwnd,hContact,szModule);
					mir_free(szModule);
				}
				else if (wParam == VK_DELETE)
					DeleteSettingsFromList(hwnd, hContact, module, setting);

				return 0;
			}
			else if (wParam == VK_F3)
				CreateDialog(hInst, MAKEINTRESOURCE(IDD_FIND), hwnd, FindWindowDlgProc);
		break;

		default: break;
	}
	return CallWindowProc(SettingListSubClass,hwnd,msg,wParam,lParam);
}

INT_PTR CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
		{
			int i;
			HMENU hMenu = GetMenu(hwnd);

			hwnd2mainWindow = hwnd;
			// do the icon
			SendMessage(hwnd,WM_SETICON,ICON_BIG,(LPARAM)LoadIcon(hInst,MAKEINTRESOURCE(ICO_REGEDIT)));
			if (UOS)
				SetWindowText(hwnd, Translate("Database Editor++ (unicode mode)"));
			else
				SetWindowText(hwnd, Translate("Database Editor++ (ansi mode)"));
			// setup the splitter
			SetWindowLongPtr(GetDlgItem(hwnd,IDC_SPLITTER),GWLP_USERDATA,(LPARAM)DBGetContactSettingWord(NULL, modname, "Splitter", 300));
			SendMessage(hwnd, GC_SPLITTERMOVED, 0,0);
			SplitterSubClass=(WNDPROC)SetWindowLongPtr(GetDlgItem(hwnd,IDC_SPLITTER),GWLP_WNDPROC,(LONG)SplitterSubclassProc);
			// module tree
			TreeView_SetUnicodeFormat(GetDlgItem(hwnd,IDC_MODULES), UOS);
			ModuleTreeSubClass=(WNDPROC)SetWindowLongPtr(GetDlgItem(hwnd,IDC_MODULES),GWLP_WNDPROC,(LONG)ModuleTreeSubclassProc);
			//setting list
			setupSettingsList(GetDlgItem(hwnd,IDC_SETTINGS));
			SettingListSubClass=(WNDPROC)SetWindowLongPtr(GetDlgItem(hwnd,IDC_SETTINGS),GWLP_WNDPROC,(LONG)SettingListSubclassProc);
			// traslation stuff
			TranslateDialogDefault(hwnd);
			CallService(MS_LANGPACK_TRANSLATEMENU,(WPARAM)hMenu,0);

			for (i=0;i<6;i++)
			{
				CallService(MS_LANGPACK_TRANSLATEMENU,(WPARAM)GetSubMenu(hMenu,i),0);
			}

			// move the dialog to the users position
			MoveWindow(hwnd,DBGetContactSettingDword(NULL,modname,"x",0),DBGetContactSettingDword(NULL,modname,"y",0),DBGetContactSettingDword(NULL,modname,"width",500),DBGetContactSettingDword(NULL,modname,"height",250),0);
			if (DBGetContactSettingByte(NULL,modname,"Maximised",0))
			{
				WINDOWPLACEMENT wp;
				wp.length = sizeof(WINDOWPLACEMENT);
				wp.flags = WPF_RESTORETOMAXIMIZED;
				wp.showCmd = SW_SHOWMAXIMIZED;

				SetWindowPlacement(hwnd,&wp);
			}
			SetCursor(LoadCursor(NULL,IDC_ARROW));

			Mode = MODE_ALL;
			CheckMenuItem(GetSubMenu(hMenu,5),MENU_FILTER_ALL,MF_BYCOMMAND|MF_CHECKED);

			Hex = DBGetContactSettingByte(NULL,modname,"HexMode",0);
			CheckMenuItem(GetSubMenu(hMenu,5),MENU_BYTE_HEX,MF_BYCOMMAND|((Hex & HEX_BYTE)?MF_CHECKED:MF_UNCHECKED));
			CheckMenuItem(GetSubMenu(hMenu,5),MENU_WORD_HEX,MF_BYCOMMAND|((Hex & HEX_WORD)?MF_CHECKED:MF_UNCHECKED));
			CheckMenuItem(GetSubMenu(hMenu,5),MENU_DWORD_HEX,MF_BYCOMMAND|((Hex & HEX_DWORD)?MF_CHECKED:MF_UNCHECKED));

			CheckMenuItem(GetSubMenu(GetMenu(hwnd),5),MENU_SAVE_POSITION,MF_BYCOMMAND|(DBGetContactSettingByte(NULL,modname,"RestoreOnOpen",1)?MF_CHECKED:MF_UNCHECKED));

			Order = DBGetContactSettingByte(NULL,modname,"SortMode",1);
			CheckMenuItem(GetSubMenu(GetMenu(hwnd),5),MENU_SORT_ORDER,MF_BYCOMMAND|(Order?MF_CHECKED:MF_UNCHECKED));



			// image list
			{
				int numberOfIcons = 0;
				himl = NULL;

				if (himl = ImageList_Create(16, 16, IsWinVerXPPlus() ? ILC_COLOR32 | ILC_MASK : ILC_COLOR8 | ILC_MASK, 10, 0))
				{
					if (AddIconToList(himl, LoadSkinnedDBEIcon(ICO_SETTINGS)))
						numberOfIcons++;
					if (AddIconToList(himl, LoadSkinnedDBEIcon(ICO_KNOWN)))
						numberOfIcons++;
					if (AddIconToList(himl, LoadSkinnedDBEIcon(ICO_KNOWNOPEN)))
						numberOfIcons++;
					if (AddIconToList(himl, LoadSkinnedDBEIcon(ICO_CONTACTS)))
						numberOfIcons++;
					if (AddIconToList(himl, LoadSkinnedDBEIcon(ICO_OFFLINE)))
						numberOfIcons++;
					if (AddIconToList(himl, LoadSkinnedDBEIcon(ICO_UNKNOWN)))
						numberOfIcons++;
					if (AddIconToList(himl, LoadSkinnedDBEIcon(ICO_UNKNOWNOPEN)))
						numberOfIcons++;
					if (AddIconToList(himl, LoadSkinnedDBEIcon(ICO_ONLINE)))
						numberOfIcons++;

					if (numberOfIcons < DEF_ICON + 1)
					{
						if (numberOfIcons)
							ImageList_Destroy(himl);
						himl = NULL;
					}

					AddProtoIconsToList(himl, numberOfIcons);

				}

				himl2 = NULL;
				numberOfIcons = 0;

				if (himl2 = ImageList_Create(16, 16, IsWinVerXPPlus() ? ILC_COLOR32 | ILC_MASK : ILC_COLOR8 | ILC_MASK, 5, 0))
				{

					if (AddIconToList(himl2, LoadIcon(hInst, MAKEINTRESOURCE(ICO_BINARY))))
						numberOfIcons++;
					if (AddIconToList(himl2, LoadIcon(hInst, MAKEINTRESOURCE(ICO_BYTE))))
						numberOfIcons++;
					if (AddIconToList(himl2, LoadIcon(hInst, MAKEINTRESOURCE(ICO_WORD))))
						numberOfIcons++;
					if (AddIconToList(himl2, LoadIcon(hInst, MAKEINTRESOURCE(ICO_DWORD))))
						numberOfIcons++;
					if (AddIconToList(himl2, LoadIcon(hInst, MAKEINTRESOURCE(ICO_STRING))))
						numberOfIcons++;
					if (AddIconToList(himl2, LoadIcon(hInst, MAKEINTRESOURCE(ICO_UNICODE))))
						numberOfIcons++;
					if (AddIconToList(himl2, LoadIcon(hInst, MAKEINTRESOURCE(ICO_HANDLE))))
						numberOfIcons++;

					if (numberOfIcons < 7)
					{
						if (numberOfIcons)
							ImageList_Destroy(himl2);
						himl2 = NULL;
					}
				}
			}

			{
				int restore;

				if (hRestore)
					restore = 3;
				else
				if (DBGetContactSettingByte(NULL,modname,"RestoreOnOpen",1))
					restore = 2;
				else
					restore = 0;

				refreshTree(restore);
			}

		}
		return TRUE;
		case GC_SPLITTERMOVED:
		{
			POINT pt;
			RECT rc;
			RECT rc2;

			int splitterPos = GetWindowLongPtr(GetDlgItem(hwnd,IDC_SPLITTER),GWLP_USERDATA);

			GetWindowRect(hwnd,&rc2);

			if((HWND)lParam==GetDlgItem(hwnd,IDC_SPLITTER))
			{
				GetClientRect(hwnd,&rc);
				pt.x=wParam; pt.y=0;
				ScreenToClient(hwnd,&pt);

				splitterPos=rc.left+pt.x+1;
				if (splitterPos<65) splitterPos=65;
				if (splitterPos > rc2.right-rc2.left-65) splitterPos=rc2.right-rc2.left-65;
				SetWindowLongPtr(GetDlgItem(hwnd,IDC_SPLITTER),GWLP_USERDATA, splitterPos);
				DBWriteContactSettingWord(NULL, modname, "Splitter",(WORD)splitterPos);
			}
			PostMessage(hwnd,WM_SIZE,0,0);
		}
		break;
		case WM_GETMINMAXINFO:
		{
			MINMAXINFO *mmi=(MINMAXINFO*)lParam;
			int splitterPos = GetWindowLongPtr(GetDlgItem(hwnd,IDC_SPLITTER),GWLP_USERDATA);

			if(splitterPos+40 > 200)
			   mmi->ptMinTrackSize.x=splitterPos+65;
			else
			   mmi->ptMinTrackSize.x=200;
			mmi->ptMinTrackSize.y=150;
			return 0;
		}
		case WM_MOVE:
		case WM_SIZE:
		{
			UTILRESIZEDIALOG urd;

			ZeroMemory(&urd,sizeof(urd));
			urd.cbSize=sizeof(urd);
			urd.hInstance=hInst;
			urd.hwndDlg=hwnd;
			urd.lParam=(LPARAM)GetWindowLongPtr(GetDlgItem(hwnd,IDC_SPLITTER),GWLP_USERDATA);
			urd.lpTemplate=MAKEINTRESOURCE(IDD_MAIN);
			urd.pfnResizer=DialogResize;
			CallService(MS_UTILS_RESIZEDIALOG,0,(LPARAM)&urd);

			if (msg == WM_SIZE && wParam == SIZE_MAXIMIZED || wParam == SIZE_MINIMIZED)
			{
				DBWriteContactSettingByte(NULL,modname,"Maximised",1);
			}
			else if (msg == WM_SIZE && wParam == SIZE_RESTORED)
			{
				DBWriteContactSettingByte(NULL,modname,"Maximised",0);
			}

		}
		break;
		case WM_DESTROY: // free our shit!

			if (DBGetContactSettingByte(NULL,modname,"RestoreOnOpen",1))
			{
				HTREEITEM item;
				HWND hwnd2Tree = GetDlgItem(hwnd,IDC_MODULES);
				char module[256] = {0};
				if (item = TreeView_GetSelection(hwnd2Tree))
				{
    				int type = MODULE;
    				TVITEM tvi = {0};
    				HANDLE hContact = NULL;
					tvi.mask=TVIF_HANDLE|TVIF_PARAM|TVIF_TEXT;
					tvi.pszText = module;
					tvi.cchTextMax = 255;
    				tvi.hItem=item;

					if (TreeView_GetItem(hwnd2Tree, &tvi))
					{
						if (tvi.lParam)
						{
							ModuleTreeInfoStruct *mtis = (ModuleTreeInfoStruct *)tvi.lParam;
							hContact = mtis->hContact;
							type = mtis->type;
						}

						DBWriteContactSettingDword(NULL,modname,"LastContact",(DWORD)hContact);

						if (type == CONTACT)
							DBWriteContactSettingString(NULL,modname,"LastModule","");
						else
							DBWriteContactSettingString(NULL,modname,"LastModule",module);
					}
					else
					{
						DBDeleteContactSetting(NULL,modname,"LastContact");
						DBDeleteContactSetting(NULL,modname,"LastModule");
					}

					// setting list
					{
					    HWND hwnd2Settings = GetDlgItem(hwnd, IDC_SETTINGS);
					    int pos = ListView_GetSelectionMark(hwnd2Settings);

					    if (pos != -1)
					    {
							char text[256];

							ListView_GetItemText(hwnd2Settings, pos, 0, text, SIZEOF(text));

							DBWriteContactSettingString(NULL,modname,"LastSetting",text);
						}
						else
							DBDeleteContactSetting(NULL,modname,"LastSetting");
					}
				}
			}
			DBWriteContactSettingByte(NULL,modname,"HexMode",(byte)Hex);
			DBWriteContactSettingByte(NULL,modname,"SortMode",(byte)Order);
			saveListSettings(GetDlgItem(hwnd,IDC_SETTINGS));
			hwnd2mainWindow = 0;
			ClearListview(GetDlgItem(hwnd,IDC_SETTINGS));
			freeTree(GetDlgItem(hwnd,IDC_MODULES),0);
			if (himl)
				ImageList_Destroy(himl);
			if (himl2)
				ImageList_Destroy(himl2);
			SetWindowLongPtr(GetDlgItem(hwnd,IDC_SETTINGS),GWLP_WNDPROC,(LONG)SettingListSubClass);
			SetWindowLongPtr(GetDlgItem(hwnd,IDC_MODULES),GWLP_WNDPROC,(LONG)ModuleTreeSubClass);
			SetWindowLongPtr(GetDlgItem(hwnd,IDC_SPLITTER),GWLP_WNDPROC,(LONG)SplitterSubClass);
			if (!DBGetContactSettingByte(NULL,modname,"Maximised",0))
			{
				RECT rc;
				GetWindowRect(hwnd,&rc);
				DBWriteContactSettingDword(NULL,modname,"x",rc.left);
				DBWriteContactSettingDword(NULL,modname,"y",rc.top);
				DBWriteContactSettingDword(NULL,modname,"width",rc.right-rc.left);
				DBWriteContactSettingDword(NULL,modname,"height",rc.bottom-rc.top);
			}
			if (hwnd2importWindow) {
				DestroyWindow(hwnd2importWindow);
				hwnd2importWindow = 0;
			}
			if ( bServiceMode ) {
				PostQuitMessage(0);
			}

		return 0;
		case WM_COMMAND:
			if (GetKeyState(VK_ESCAPE) & 0x8000) return TRUE; // this needs to be changed to c if htere is a label edit happening..
			switch(LOWORD(wParam))
			{
				case MENU_REFRESH_MODS:
					refreshTree(1);
				break;
				case MENU_REFRESH_SETS:
				{
					TVITEM tvi;
					ModuleTreeInfoStruct *mtis;
					char module[256];
					tvi.mask=TVIF_HANDLE|TVIF_PARAM|TVIF_TEXT;
					tvi.hItem=TreeView_GetSelection(GetDlgItem(hwnd, IDC_MODULES));
					tvi.pszText = module;
					tvi.cchTextMax = 255;
					if (!TreeView_GetItem(GetDlgItem(hwnd, IDC_MODULES),&tvi)) break;
					if (tvi.lParam)
					{
						mtis = (ModuleTreeInfoStruct *)tvi.lParam;
						if (mtis->type  == MODULE || mtis->type == UNKNOWN_MODULE)
							PopulateSettings(GetDlgItem(hwnd, IDC_SETTINGS), mtis->hContact, module);
						else ClearListview(GetDlgItem(hwnd, IDC_SETTINGS));
					}
					else ClearListview(GetDlgItem(hwnd, IDC_SETTINGS));
				}
				break;
				///////////////////////// // watches
				case MENU_VIEW_WATCHES:
				{
					if (!hwnd2watchedVarsWindow) // so only opens 1 at a time
						hwnd2watchedVarsWindow = CreateDialog(hInst, MAKEINTRESOURCE(IDD_WATCH_DIAG), 0, WatchDlgProc);
					else SetForegroundWindow(hwnd2watchedVarsWindow);
				}
				break;
				case MENU_REMALL_WATCHES:
					freeAllWatches();
				break;
				case MENU_EXPORTDB: // all db
					exportDB(INVALID_HANDLE_VALUE,0);
				break;
				case MENU_EXPORTCONTACT: // all contacts
					exportDB(INVALID_HANDLE_VALUE, "");
				break;
				case MENU_EXPORTMODULE: // all settings
					exportDB(NULL, 0);
					break;
				case MENU_IMPORTFROMFILE:
					ImportSettingsFromFileMenuItem(NULL, "");
				break;
				case MENU_IMPORTFROMTEXT:
					ImportSettingsMenuItem(NULL);
				break;
				case MENU_EXIT:
				case IDCANCEL:
					DestroyWindow(hwnd);
				break;
				case MENU_DELETE:
					deleteModuleGui();
				break;
				case MENU_FINDANDREPLACE:
					CreateDialog(hInst, MAKEINTRESOURCE(IDD_FIND), hwnd, FindWindowDlgProc);
				break;
				case MENU_FILTER_ALL:
				if (Mode != MODE_ALL)
				{
					HMENU hMenu = GetMenu(hwnd);
					CheckMenuItem(GetSubMenu(hMenu,5),MENU_FILTER_ALL,MF_BYCOMMAND|MF_CHECKED);
					CheckMenuItem(GetSubMenu(hMenu,5),MENU_FILTER_LOADED,MF_BYCOMMAND|MF_UNCHECKED);
					CheckMenuItem(GetSubMenu(hMenu,5),MENU_FILTER_UNLOADED,MF_BYCOMMAND|MF_UNCHECKED);
					Mode = MODE_ALL;
					refreshTree(1);
				}
				break;
				case MENU_FILTER_LOADED:
				if (Mode != MODE_LOADED)
				{
					HMENU hMenu = GetMenu(hwnd);
					CheckMenuItem(GetSubMenu(hMenu,5),MENU_FILTER_ALL,MF_BYCOMMAND|MF_UNCHECKED);
					CheckMenuItem(GetSubMenu(hMenu,5),MENU_FILTER_LOADED,MF_BYCOMMAND|MF_CHECKED);
					CheckMenuItem(GetSubMenu(hMenu,5),MENU_FILTER_UNLOADED,MF_BYCOMMAND|MF_UNCHECKED);
					Mode = MODE_LOADED;
					refreshTree(1);
				}
				break;
				case MENU_FILTER_UNLOADED:
				if (Mode != MODE_UNLOADED)
				{
					HMENU hMenu = GetMenu(hwnd);
					CheckMenuItem(GetSubMenu(hMenu,5),MENU_FILTER_ALL,MF_BYCOMMAND|MF_UNCHECKED);
					CheckMenuItem(GetSubMenu(hMenu,5),MENU_FILTER_LOADED,MF_BYCOMMAND|MF_UNCHECKED);
					CheckMenuItem(GetSubMenu(hMenu,5),MENU_FILTER_UNLOADED,MF_BYCOMMAND|MF_CHECKED);
					Mode = MODE_UNLOADED;
					refreshTree(1);
				}
				break;
				case MENU_BYTE_HEX:
				{
					Hex ^= HEX_BYTE;
					CheckMenuItem(GetSubMenu(GetMenu(hwnd),5),MENU_BYTE_HEX,MF_BYCOMMAND|((Hex & HEX_BYTE)?MF_CHECKED:MF_UNCHECKED));
				}
				break;
				case MENU_WORD_HEX:
				{
					Hex ^= HEX_WORD;
					CheckMenuItem(GetSubMenu(GetMenu(hwnd),5),MENU_WORD_HEX,MF_BYCOMMAND|((Hex & HEX_WORD)?MF_CHECKED:MF_UNCHECKED));
				}
				break;
				case MENU_DWORD_HEX:
				{
					Hex ^= HEX_DWORD;
					CheckMenuItem(GetSubMenu(GetMenu(hwnd),5),MENU_DWORD_HEX,MF_BYCOMMAND|((Hex & HEX_DWORD)?MF_CHECKED:MF_UNCHECKED));
				}
				break;
				case MENU_SAVE_POSITION:
				{
					BOOL save = !DBGetContactSettingByte(NULL,modname,"RestoreOnOpen",1);
					CheckMenuItem(GetSubMenu(GetMenu(hwnd),5),MENU_SAVE_POSITION,MF_BYCOMMAND|(save?MF_CHECKED:MF_UNCHECKED));
					DBWriteContactSettingByte(NULL,modname,"RestoreOnOpen", (byte)save);
				}
				break;
				case MENU_SORT_ORDER:
					Order = !Order;
					CheckMenuItem(GetSubMenu(GetMenu(hwnd),5),MENU_SORT_ORDER,MF_BYCOMMAND|(Order?MF_CHECKED:MF_UNCHECKED));
					refreshTree(1);
				break;
			}
		return TRUE; // case WM_COMMAND
		case WM_NOTIFY:
			switch(LOWORD(wParam))
			{
				case IDC_MODULES:
					moduleListWM_NOTIFY(hwnd,msg,wParam,lParam);
				break;
				case IDC_SETTINGS:
					SettingsListWM_NOTIFY(hwnd,msg,wParam,lParam);
				break;
			}
		return TRUE; // case WM_NOTIFY
		case WM_FINDITEM:
		{
			ItemInfo *ii = (ItemInfo*)wParam;
			HWND hwnd2Settings = GetDlgItem(hwnd,IDC_SETTINGS);
			int hItem = findItemInTree(GetDlgItem(hwnd,IDC_MODULES),ii->hContact,ii->module);

			if (hItem != -1)
			{
				TreeView_SelectItem(GetDlgItem(hwnd,IDC_MODULES), (HTREEITEM)hItem);
				if (ii->type != FW_MODULE)
				{
					LVITEM lvItem;
					LVFINDINFO lvfi;

					lvfi.flags = LVFI_STRING;
					lvfi.psz = ii->setting;
					lvfi.vkDirection = VK_DOWN;

					lvItem.mask = LVIF_TEXT|LVIF_IMAGE;
					lvItem.iItem = ListView_FindItem(hwnd2Settings,-1,&lvfi);
					lvItem.iSubItem = 0;
					if (lvItem.iItem >= 0)
						ListView_SetItemState(hwnd2Settings,lvItem.iItem, LVIS_SELECTED, LVIS_SELECTED);
				}
			}
		}
		break;
	}	// switch(msg)
	return 0;
}
