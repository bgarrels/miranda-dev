/*
Basic History plugin
Copyright (C) 2011 Krzysztof Kral

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "StdAfx.h"
#include "HistoryWindow.h"
#include "resource.h"
#include "Options.h"
#include "HotkeyHelper.h"
#include "ImageDataObject.h"
#include "ExportManeger.h"

extern HINSTANCE hInst;
extern HCURSOR     hCurSplitNS, hCurSplitWE;
extern HANDLE *hEventIcons;
extern int iconsNum;
extern HANDLE hPlusIcon, hMinusIcon, hFindNextIcon, hFindPrevIcon;
extern bool g_SmileyAddAvail;
extern char* metaContactProto;
#define DM_HREBUILD  (WM_USER+11)
#define DM_SPLITTERMOVED     (WM_USER+15)

#define MIN_PANELHEIGHT 40

HistoryWindow::HistoryWindow(HANDLE _hContact)
	: isDestroyed(true),
	OldSplitterProc(0),
	splitterY(0),
	splitterOrgY(0),
	splitterX(0),
	splitterOrgX(0),
	plusIco(NULL),
	minusIco(NULL),
	findNextIco(NULL),
	findPrevIco(NULL),
	configIco(NULL),
	deleteIco(NULL),
	isContactList(false),
	isLoading(false),
	searcher(*this), 
	isGroupImages(false),
	allIconNumber(0), 
	eventIcoms(NULL),
	bkBrush(NULL)
{
	hContact = _hContact;
	selected = -1;
	searcher.SetMatchCase(Options::instance->searchMatchCase);
	searcher.SetMatchWholeWords(Options::instance->searchMatchWhole);
	searcher.SetOnlyIn(Options::instance->searchOnlyIn);
	searcher.SetOnlyOut(Options::instance->searchOnlyOut);
	searcher.SetOnlyGroup(Options::instance->searchOnlyGroup);
	searcher.SetSearchForInLG(Options::instance->searchForInList);
	searcher.SetSearchForInMes(Options::instance->searchForInMess);
}


HistoryWindow::~HistoryWindow()
{
	if(eventIcoms != NULL)
	{
		for(int i = 0; i < iconsNum; ++i)
		{
			if(eventIcoms[i] != NULL)
			{
				CallService(MS_SKIN2_RELEASEICON, (LPARAM)eventIcoms[i], 0);
			}
		}

		delete[] eventIcoms;
	}

	if(plusIco != NULL)
	{
		CallService(MS_SKIN2_RELEASEICON, (LPARAM)plusIco, 0);
	}
	if(minusIco != NULL)
	{
		CallService(MS_SKIN2_RELEASEICON, (LPARAM)minusIco, 0);
	}
	if(findNextIco != NULL)
	{
		CallService(MS_SKIN2_RELEASEICON, (LPARAM)findNextIco, 0);
	}
	if(findPrevIco != NULL)
	{
		CallService(MS_SKIN2_RELEASEICON, (LPARAM)findPrevIco, 0);
	}
	if(himlSmall != NULL)
	{
		ImageList_Destroy(himlSmall);
	}
	if(himlNone != NULL)
	{
		ImageList_Destroy(himlNone);
	}
	if(bkBrush != NULL)
	{
		DeleteObject(bkBrush);
	}
}

std::map<HANDLE, HistoryWindow*> HistoryWindow::windows;
std::vector<HistoryWindow*> HistoryWindow::freeWindows;

void HistoryWindow::Deinit()
{
	bool destroyed = true;
	std::vector<HANDLE> keys;
	for(std::map<HANDLE, HistoryWindow*>::iterator it = windows.begin(); it != windows.end(); ++it)
	{
		if(!it->second->isDestroyed)
		{
			keys.push_back(it->first);
		}
	}
	for(std::vector<HANDLE>::iterator it = keys.begin(); it != keys.end(); ++it)
	{
		std::map<HANDLE, HistoryWindow*>::iterator it1 = windows.find(*it);
		if(it1 != windows.end())
		{
			DestroyWindow(it1->second->hWnd);
			it1 = windows.find(*it);
			destroyed &= it1 == windows.end();
		}
	}
	
	std::vector<HistoryWindow*> keys1;
	for(std::vector<HistoryWindow*>::iterator it = freeWindows.begin(); it != freeWindows.end(); ++it)
	{
		if(!(*it)->isDestroyed)
		{
			keys1.push_back(*it);
		}
	}
	for(std::vector<HistoryWindow*>::iterator it = keys1.begin(); it != keys1.end(); ++it)
	{
		DestroyWindow((*it)->hWnd);
	}
	for(std::vector<HistoryWindow*>::iterator it = freeWindows.begin(); it != freeWindows.end(); ++it)
	{
		if(!(*it)->isDestroyed)
		{
			destroyed = false;
			break;
		}
	}

	if(destroyed)
	{
		for(std::map<HANDLE, HistoryWindow*>::iterator it = windows.begin(); it != windows.end(); ++it)
		{
			delete it->second;
		}

		windows.clear();

		for(std::vector<HistoryWindow*>::iterator it = freeWindows.begin(); it != freeWindows.end(); ++it)
		{
			delete *it;
		}

		freeWindows.clear();
	}
}

void HistoryWindow::Open(HANDLE hContact)
{
	if(hContact == NULL)
	{
		HistoryWindow *hw = new HistoryWindow(hContact);
		freeWindows.push_back(hw);
		hw->Show();
	}
	else
	{
		std::map<HANDLE, HistoryWindow*>::iterator it = windows.find(hContact);
		if(it != windows.end())
		{
			it->second->Focus();
		}
		else
		{
			windows[hContact] = new HistoryWindow(hContact);
			windows[hContact]->Show();
		}
	}
}

void HistoryWindow::Close(HANDLE hContact)
{
	std::map<HANDLE, HistoryWindow*>::iterator it = windows.find(hContact);
	if(it != windows.end())
	{
		if(it->second->isDestroyed)
		{
			delete it->second;
			windows.erase(it);
		}
		else
		{
			DestroyWindow(it->second->hWnd);
		}
	}
}

void  HistoryWindow::Close(HistoryWindow* historyWindow)
{
	if(!historyWindow->isDestroyed)
	{
		DestroyWindow(historyWindow->hWnd);
		return;
	}
	std::map<HANDLE, HistoryWindow*>::iterator it = windows.find(historyWindow->hContact);
	if(it != windows.end() && it->second == historyWindow)
	{
		delete it->second;
		windows.erase(it);
	}
	else
	{
		for(std::vector<HistoryWindow*>::iterator it = freeWindows.begin(); it != freeWindows.end(); ++it)
		{
			if(*it == historyWindow)
			{
				freeWindows.erase(it);
				delete historyWindow;
				return;
			}
		}
	}
}

void HistoryWindow::ChangeToFreeWindow(HistoryWindow* historyWindow)
{
	std::map<HANDLE, HistoryWindow*>::iterator it = windows.find(historyWindow->hContact);
	if(it != windows.end() && it->second == historyWindow)
	{
		windows.erase(it);
		freeWindows.push_back(historyWindow);
	}
}

void HistoryWindow::Show()
{
	CreateDialogParam(hInst,MAKEINTRESOURCE(IDD_HISTORY),NULL,HistoryWindow::DlgProcHistory,(LPARAM)this);
}

void HistoryWindow::Focus()
{
	if(IsIconic(hWnd))
	{
		ShowWindow(hWnd, SW_RESTORE);
	}
	else
	{
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);
	}
	SendMessage(hWnd,DM_HREBUILD,0,0);
}

int HistoryWindow::FontsChanged(WPARAM wParam, LPARAM lParam)
{
	for(std::map<HANDLE, HistoryWindow*>::iterator it = windows.begin(); it != windows.end(); ++it)
	{
		if(!it->second->isDestroyed)
		{
			it->second->FontsChanged();
		}
	}

	for(std::vector<HistoryWindow*>::iterator it = freeWindows.begin(); it != freeWindows.end(); ++it)
	{
		if(!(*it)->isDestroyed)
		{
			(*it)->FontsChanged();
		}
	}

	return 0;
}

void HistoryWindow::FontsChanged()
{
	if(bkBrush != NULL)
	{
		DeleteObject(bkBrush);
	}

	bkBrush = CreateSolidBrush(Options::instance->GetColor(Options::WindowBackground));

	COLORREF bkColor = Options::instance->GetColor(Options::GroupListBackground);
	ListView_SetBkColor(listWindow, bkColor);
	ListView_SetTextBkColor(listWindow, bkColor);
	LOGFONT font;
	ListView_SetTextColor(listWindow, Options::instance->GetFont(Options::GroupList, &font));
	InvalidateRect(listWindow, NULL, TRUE);
	InvalidateRect(hWnd, NULL, TRUE);
	SelectEventGroup(selected);
}

void OptionsGroupChanged()
{
	HistoryWindow::OptionsGroupChanged();
}

void HistoryWindow::OptionsGroupChanged()
{
	for(std::map<HANDLE, HistoryWindow*>::iterator it = windows.begin(); it != windows.end(); ++it)
	{
		if(!it->second->isDestroyed)
		{
			it->second->GroupImagesChanged();
			SendMessage(it->second->hWnd,DM_HREBUILD,0,0);
		}
	}

	for(std::vector<HistoryWindow*>::iterator it = freeWindows.begin(); it != freeWindows.end(); ++it)
	{
		if(!(*it)->isDestroyed)
		{
			(*it)->GroupImagesChanged();
			SendMessage((*it)->hWnd,DM_HREBUILD,0,0);
		}
	}
}

void OptionsMainChanged()
{
	HistoryWindow::OptionsMainChanged();
}

void HistoryWindow::OptionsMainChanged()
{
	for(std::map<HANDLE, HistoryWindow*>::iterator it = windows.begin(); it != windows.end(); ++it)
	{
		if(!it->second->isDestroyed)
		{
			it->second->ReloadMainOptions();
		}
	}

	for(std::vector<HistoryWindow*>::iterator it = freeWindows.begin(); it != freeWindows.end(); ++it)
	{
		if(!(*it)->isDestroyed)
		{
			(*it)->ReloadMainOptions();
		}
	}
}

void HistoryWindow::ReloadMainOptions()
{
	SendDlgItemMessage(hWnd, IDC_LIST_CONTACTS, CLM_SETUSEGROUPS, Options::instance->showContactGroups, 0);
	SendMessage(hWnd,DM_HREBUILD,0,0);
}

void OptionsMessageChanged()
{
	HistoryWindow::FontsChanged(0, 0);
}

void OptionsSearchingChanged()
{
	HistoryWindow::OptionsSearchingChanged();
}

void HistoryWindow::OptionsSearchingChanged()
{
	for(std::map<HANDLE, HistoryWindow*>::iterator it = windows.begin(); it != windows.end(); ++it)
	{
		if(!it->second->isDestroyed)
		{
			it->second->searcher.SetSearchForInLG(Options::instance->searchForInList);
			it->second->searcher.SetSearchForInMes(Options::instance->searchForInMess);
		}
	}

	for(std::vector<HistoryWindow*>::iterator it = freeWindows.begin(); it != freeWindows.end(); ++it)
	{
		if(!(*it)->isDestroyed)
		{
			(*it)->searcher.SetSearchForInLG(Options::instance->searchForInList);
			(*it)->searcher.SetSearchForInMes(Options::instance->searchForInMess);
		}
	}
}

INT_PTR HistoryWindow::DeleteAllUserHistory(WPARAM wParam, LPARAM lParam)
{
	HANDLE hContact = (HANDLE)wParam;
	HWND hWnd = NULL;
	int toDelete = 1;
	int start = 0;
	int end = 0;
	int count = CallService(MS_DB_EVENT_GETCOUNT,wParam,0);
	if(!count)
		return FALSE;
	
	for(std::map<HANDLE, HistoryWindow*>::iterator it = windows.begin(); it != windows.end(); ++it)
	{
		if(!it->second->isDestroyed)
		{
			if(it->second->hContact == hContact)
			{
				if(hWnd == NULL)
				{
					hWnd = it->second->hWnd;
				}
				else if(GetForegroundWindow() == it->second->hWnd)
				{
					hWnd = it->second->hWnd;
				}
			}
		}
	}

	for(std::vector<HistoryWindow*>::iterator it = freeWindows.begin(); it != freeWindows.end(); ++it)
	{
		if(!(*it)->isDestroyed)
		{
			if((*it)->hContact == hContact)
			{
				if(hWnd == NULL)
				{
					hWnd = (*it)->hWnd;
				}
				else if(GetForegroundWindow() == (*it)->hWnd)
				{
					hWnd = (*it)->hWnd;
				}
			}
		}
	}

	TCHAR *message = TranslateT("This operation will PERMANENTLY REMOVE all history for this contact.\nAre you sure you want to do this?");
	if(MessageBox(hWnd, message, TranslateT("Are You sure?"), MB_OKCANCEL | MB_ICONERROR) != IDOK)
		return FALSE;

	std::deque<HANDLE> toRemove;
	HANDLE hDbEvent=(HANDLE)CallService(MS_DB_EVENT_FINDFIRST,(WPARAM)hContact,0);
	while ( hDbEvent != NULL ) 
	{
		toRemove.push_back(hDbEvent);
		hDbEvent=(HANDLE)CallService(MS_DB_EVENT_FINDNEXT,(WPARAM)hDbEvent,0);
	}
	
	for(std::deque<HANDLE>::iterator it = toRemove.begin(); it != toRemove.end(); ++it)
	{
		CallService(MS_DB_EVENT_DELETE,(WPARAM)hContact,(LPARAM)(HANDLE)*it);
	}
		
	for(std::map<HANDLE, HistoryWindow*>::iterator it = windows.begin(); it != windows.end(); ++it)
	{
		if(!it->second->isDestroyed)
		{
			if(it->second->hContact == hContact)
			{
				SendMessage(it->second->hWnd,DM_HREBUILD,0,0);
			}
		}
	}

	for(std::vector<HistoryWindow*>::iterator it = freeWindows.begin(); it != freeWindows.end(); ++it)
	{
		if(!(*it)->isDestroyed)
		{
			if((*it)->hContact == hContact)
			{
				SendMessage((*it)->hWnd,DM_HREBUILD,0,0);
			}
		}
	}

	return TRUE;
}

bool HistoryWindow::IsInList(HWND hWnd)
{
	for(std::map<HANDLE, HistoryWindow*>::iterator it = windows.begin(); it != windows.end(); ++it)
	{
		if(!it->second->isDestroyed)
		{
			if(it->second->hWnd == hWnd)
			{
				return true;
			}
		}
	}

	for(std::vector<HistoryWindow*>::iterator it = freeWindows.begin(); it != freeWindows.end(); ++it)
	{
		if(!(*it)->isDestroyed)
		{
			if((*it)->hWnd == hWnd)
			{
				return true;
			}
		}
	}
	
	return false;
}

void ClickLink(HWND hwnd, ENLINK *penLink)
{
	TCHAR buf[1024];
	if(penLink->msg != WM_LBUTTONUP)
		return;
	if(penLink->chrg.cpMin >= 0 && penLink->chrg.cpMax > penLink->chrg.cpMin)
	{
		// selection
		int len = penLink->chrg.cpMax - penLink->chrg.cpMin;
		if(len < 1023)
		{
			TEXTRANGE tr;
			CHARRANGE sel;
			char* pszUrl;

			SendMessage(hwnd, EM_EXGETSEL, 0, (LPARAM) & sel);
			if (sel.cpMin != sel.cpMax)
				return;
			tr.chrg = penLink->chrg;
			tr.lpstrText = buf;
			SendMessage(hwnd, EM_GETTEXTRANGE, 0, (LPARAM) & tr);
			pszUrl = mir_t2a( tr.lpstrText );
			CallService(MS_UTILS_OPENURL, penLink->nmhdr.code == IDM_OPENNEW ? 1 : 0, (LPARAM) pszUrl);
			mir_free(pszUrl);
		}
	}
}

void ConvertSize(HWND hwndSrc, HWND hwndDest, RECT& rc)
{
	POINT pt;
	pt.x = rc.left;
	pt.y = rc.top;
	ClientToScreen(hwndSrc, &pt);
	ScreenToClient(hwndDest, &pt);
	rc.left = pt.x;
	rc.top = pt.y;

	pt.x = rc.right;
	pt.y = rc.bottom;
	ClientToScreen(hwndSrc, &pt);
	ScreenToClient(hwndDest, &pt);
	rc.right = pt.x;
	rc.bottom = pt.y;
}

void OpenOptions(char* group, char* page, char* tab = NULL)
{
	OPENOPTIONSDIALOG op;
	op.cbSize = sizeof(OPENOPTIONSDIALOG);
	op.pszGroup = group;
	op.pszPage = page;
	op.pszTab = tab;
	CallService(MS_OPT_OPENOPTIONS, 0, (LPARAM)&op);
}

#define DlgReturn(ret){\
	SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, (ret));\
	return (ret);\
}

class ShowMessageData
{
public:
	ShowMessageData(HANDLE _hContact)
		:hContact(_hContact)
	{
	}

	ShowMessageData(HANDLE _hContact, const std::wstring &_str)
		:hContact(_hContact),
		str(_str)
	{
	}

	HANDLE hContact;
	std::wstring str;
};

void __stdcall ShowMessageWindow(void* arg)
{
	ShowMessageData* dt = (ShowMessageData*)arg;
	if(dt->str.empty())
		CallService(MS_MSG_SENDMESSAGE, (WPARAM)dt->hContact, 0);
	else
		CallService(MS_MSG_SENDMESSAGET, (WPARAM)dt->hContact, (LPARAM)dt->str.c_str());
	delete dt;
}

INT_PTR CALLBACK HistoryWindow::DlgProcHistory(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x=500;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y=380;

	case WM_SIZE:
		{
			UTILRESIZEDIALOG urd={0};
			urd.cbSize=sizeof(urd);
			urd.hwndDlg=hwndDlg;
			urd.hInstance=hInst;
			urd.lpTemplate=MAKEINTRESOURCEA(IDD_HISTORY);
			urd.lParam=(LPARAM)NULL;
			urd.pfnResizer=HistoryWindow::HistoryDlgResizer;
			CallService(MS_UTILS_RESIZEDIALOG,0,(LPARAM)&urd);
			ListView_SetColumnWidth(GetDlgItem(hwndDlg,IDC_LIST), 0, LVSCW_AUTOSIZE_USEHEADER);
			DlgReturn(TRUE);
		}
	case WM_COMMAND:
		switch ( LOWORD( wParam )) {
		case IDOK:
		case IDCANCEL:
			DestroyWindow(hwndDlg);
			DlgReturn(TRUE);

		case IDM_FIND:
			{
				HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
				historyWindow->searcher.Find();
				DlgReturn(TRUE);
			}

		case IDM_CONFIG:
			{
				OPENOPTIONSDIALOG opd = {0};
				opd.cbSize = sizeof(OPENOPTIONSDIALOG);
				opd.pszPage = LPGEN("History");
				CallService(MS_OPT_OPENOPTIONS, 0, (LPARAM)&opd);
				DlgReturn(TRUE);
			}

		case IDM_DELETE:
			{
				HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
				historyWindow->Delete(0);
				DlgReturn(TRUE);
			}

		case IDC_FIND_TEXT:
			if(HIWORD(wParam) == EN_CHANGE)
			{
				HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
				historyWindow->searcher.ClearFind();
			}
			
			DlgReturn(TRUE);

		case IDC_SHOWHIDE:
			{
				if(HIWORD( wParam ) == BN_CLICKED)
				{
					HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
					if(Button_GetCheck(GetDlgItem(hwndDlg,IDC_SHOWHIDE)) & BST_CHECKED)
					{
						SendDlgItemMessage( hwndDlg, IDC_SHOWHIDE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)historyWindow->minusIco);
						SendDlgItemMessage( hwndDlg, IDC_SHOWHIDE, BUTTONADDTOOLTIP, (WPARAM)LPGENT("Hide Contacts"), BATF_TCHAR);
						historyWindow->isContactList = true;
						ShowWindow(GetDlgItem(hwndDlg,IDC_LIST_CONTACTS), SW_SHOW);
						ShowWindow(GetDlgItem(hwndDlg,IDC_SPLITTERV), SW_SHOW);
					}
					else
					{
						SendDlgItemMessage( hwndDlg, IDC_SHOWHIDE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)historyWindow->plusIco);
						SendDlgItemMessage( hwndDlg, IDC_SHOWHIDE, BUTTONADDTOOLTIP, (WPARAM)LPGENT("Show Contacts"), BATF_TCHAR);
						historyWindow->isContactList = false;
						ShowWindow(GetDlgItem(hwndDlg,IDC_LIST_CONTACTS), SW_HIDE);
						ShowWindow(GetDlgItem(hwndDlg,IDC_SPLITTERV), SW_HIDE);
					}

					SendMessage(hwndDlg, WM_SIZE, 0, 0);
				}

				DlgReturn(TRUE);
			}
		}
		break;
	case WM_NOTIFY:
		{
			LPNMHDR pNmhdr;

			pNmhdr = (LPNMHDR)lParam;
			switch(pNmhdr->idFrom)
			{
				case IDC_LIST_CONTACTS:
					if(pNmhdr->code == CLN_LISTREBUILT)// || pNmhdr->code == CLN_CONTACTMOVED || pNmhdr->code == CLN_NEWCONTACT)
					{
						HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
						historyWindow->ReloadContacts();
						DlgReturn(TRUE);
					}
					else if(pNmhdr->code == CLN_MYSELCHANGED)
					{
						HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
						if(historyWindow->ContactChanged())
						{
							MSGFILTER* msgFilter = (MSGFILTER *) lParam;
							if(msgFilter->msg == WM_LBUTTONDOWN)
							{
								SendMessage(pNmhdr->hwndFrom, WM_LBUTTONUP, msgFilter->wParam, msgFilter->lParam);
							}
						}

						DlgReturn(TRUE);
					}

					//fall through
				//case IDC_LIST_CONTACTS:
				case IDC_SHOWHIDE:
				case IDC_FIND_TEXT:
				case IDC_EDIT:
					if ( pNmhdr->code == EN_LINK ) 
					{
						ClickLink(GetDlgItem(hwndDlg, IDC_EDIT), (ENLINK *) lParam);
						return FALSE;
					}
					else if( pNmhdr->code == EN_SELCHANGE)
					{
						HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
						historyWindow->searcher.ClearFind();
					}
					else if(pNmhdr->code == EN_MSGFILTER)
					{
						MSGFILTER* msgFilter = (MSGFILTER *) lParam;
						if (msgFilter->msg == WM_KEYDOWN || msgFilter->msg == WM_SYSKEYDOWN) 
						{
							HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
							if(historyWindow->DoHotkey(msgFilter->msg, msgFilter->lParam,  msgFilter->wParam, pNmhdr->idFrom))
								DlgReturn(TRUE);
						}
						else if (msgFilter->msg == WM_RBUTTONDOWN || msgFilter->msg == WM_RBUTTONDBLCLK || msgFilter->msg == WM_NCRBUTTONUP || msgFilter->msg == WM_NCRBUTTONDBLCLK || msgFilter->msg == WM_NCRBUTTONDOWN) 
						{
							DlgReturn(TRUE);
						}
						else if (msgFilter->msg == WM_RBUTTONUP) 
						{
							HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
							POINT clicked;
							LPNMITEMACTIVATE nmlv = (LPNMITEMACTIVATE)lParam;
							HWND window = historyWindow->editWindow;
							POINTL p;
							POINT scrool;
							LVHITTESTINFO info = {0};
							p.x = clicked.x = info.pt.x = GET_X_LPARAM(msgFilter->lParam);
							p.y = clicked.y = info.pt.y = GET_Y_LPARAM(msgFilter->lParam);
							ClientToScreen(window, &clicked);
							SetFocus(window);
							int selChar = SendMessage(window, EM_CHARFROMPOS, 0, (LPARAM)&p);
							CHARRANGE chrg;
							SendMessage(window,EM_EXGETSEL,0,(LPARAM)&chrg);
							SendMessage(window,EM_GETSCROLLPOS,0,(LPARAM)&scrool);
							if(selChar < chrg.cpMin || selChar > chrg.cpMax)
							{
								chrg.cpMin = chrg.cpMax = selChar;
							}

							if(chrg.cpMin == chrg.cpMax)
							{
								CHARRANGE chrgNew;
								chrgNew.cpMin = chrg.cpMin;
								chrgNew.cpMax = chrg.cpMax + 1;
								SendMessage(window,EM_EXSETSEL,0,(LPARAM)&chrgNew);
							}
							CHARFORMAT2 chf;
							memset(&chf, 0, sizeof(CHARFORMAT2));
							chf.cbSize = sizeof(CHARFORMAT2);
							chf.dwMask = CFM_LINK;
							SendMessage(window, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&chf);
							if(chrg.cpMin == chrg.cpMax)
								SendMessage(window,EM_EXSETSEL,0,(LPARAM)&chrg);

							HMENU hPopupMenu = CreatePopupMenu();
							if(hPopupMenu != NULL)
							{
								if(chf.dwEffects & CFE_LINK)
								{
									AppendMenu(hPopupMenu, MF_STRING, IDM_OPENNEW, TranslateT("Open in &new window"));
									AppendMenu(hPopupMenu, MF_STRING, IDM_OPENEXISTING, TranslateT("&Open in existing window"));
									AppendMenu(hPopupMenu, MF_STRING, IDM_COPYLINK, TranslateT("&Copy link"));
								}
								else
								{
									AppendMenu(hPopupMenu, MF_STRING, IDM_COPY, TranslateT("Copy"));
									AppendMenu(hPopupMenu, MF_STRING, IDM_DELETE, TranslateT("Delete"));
									AppendMenu(hPopupMenu, MFT_SEPARATOR, 0, NULL);
									AppendMenu(hPopupMenu, MF_STRING, IDM_MESSAGE, TranslateT("Send Message"));
									AppendMenu(hPopupMenu, MF_STRING, IDM_QUOTE, TranslateT("Reply &Quoted"));
									AppendMenu(hPopupMenu, MF_STRING, IDM_DELETEGROUP, TranslateT("Delete Group"));
									AppendMenu(hPopupMenu, MF_STRING, IDM_DELETEUSER, TranslateT("Delete All User History"));
								}

								int selected = TrackPopupMenu(hPopupMenu, TPM_RETURNCMD, clicked.x, clicked.y, 0, hwndDlg, 0);
								switch (selected)
								{
								case IDM_COPY:
									{
										if(chrg.cpMax == chrg.cpMin && historyWindow->currentGroup.size() > 0)
										{
											int start = 0;
											while(start < historyWindow->currentGroup.size() && chrg.cpMin >= historyWindow->currentGroup[start].endPos) ++start;
											if(start < historyWindow->currentGroup.size())
											{
												CHARRANGE chrgNew;
												chrgNew.cpMin = 0;
												if(start > 0)
													chrgNew.cpMin = historyWindow->currentGroup[start - 1].endPos;
												chrgNew.cpMax = historyWindow->currentGroup[start].endPos;
												SendMessage(window,EM_EXSETSEL,0,(LPARAM)&chrgNew);
												SendMessage(window,WM_COPY,0,0);
												SendMessage(window,EM_EXSETSEL,0,(LPARAM)&chrg);
											}
										}
										else
										{
											SendMessage(window,WM_COPY,0,0);
										}
									}
									break;
								case IDM_MESSAGE:
									//CallService(MS_MSG_SENDMESSAGE, (WPARAM)historyWindow->hContact, 0);
									CallFunctionAsync(ShowMessageWindow, new ShowMessageData(historyWindow->hContact));
									break;
								case IDM_QUOTE:
									{
										if(historyWindow->currentGroup.size() > 0)
										{
											std::wstring quote;
											if(chrg.cpMax == chrg.cpMin)
											{
												int start = 0;
												while(start < historyWindow->currentGroup.size() && chrg.cpMin >= historyWindow->currentGroup[start].endPos) ++start;
												if(start < historyWindow->currentGroup.size())
												{
													historyWindow->FormatQuote(quote, historyWindow->currentGroup[start], historyWindow->currentGroup[start].description);
												}
											}
											else
											{
												int start = 0;
												while(start < historyWindow->currentGroup.size() && chrg.cpMin >= historyWindow->currentGroup[start].endPos) ++start;
												int end = 0;
												while(end < historyWindow->currentGroup.size() && chrg.cpMax > historyWindow->currentGroup[end].endPos) ++end;
												if(end >= historyWindow->currentGroup.size())
													end = historyWindow->currentGroup.size() - 1;
												if(start == end && start < historyWindow->currentGroup.size())
												{
													int iStart = historyWindow->currentGroup[start].startPos;
													if(chrg.cpMin > iStart)
														iStart = chrg.cpMin;
													int iEnd = historyWindow->currentGroup[start].endPos;
													if(chrg.cpMax < iEnd)
														iEnd = chrg.cpMax;
													if(iEnd > iStart)
													{
														TEXTRANGE tr;
														tr.chrg.cpMin = iStart;
														tr.chrg.cpMax = iEnd;
														tr.lpstrText = new TCHAR[iEnd - iStart + 1];
														SendMessage(historyWindow->editWindow, EM_GETTEXTRANGE, 0, (LPARAM) & tr);
														historyWindow->FormatQuote(quote, historyWindow->currentGroup[start], tr.lpstrText);
														delete [] tr.lpstrText;
													}
												}
												else
												{
													while(start <= end)
													{
														historyWindow->FormatQuote(quote, historyWindow->currentGroup[start], historyWindow->currentGroup[start].description);
														++start;
													}
												}
											}

											if(!quote.empty())
											{
												CallFunctionAsync(ShowMessageWindow, new ShowMessageData(historyWindow->hContact, quote));
											}
										}
									}
									break;
								case IDM_DELETE:
									historyWindow->Delete(0);
									break;
								case IDM_DELETEGROUP:
									historyWindow->Delete(1);
									break;
								case IDM_DELETEUSER:
									historyWindow->Delete(2);
									break;
								case IDM_OPENNEW:
								case IDM_OPENEXISTING:
								case IDM_COPYLINK:
									{
										int start = chrg.cpMin, end = chrg.cpMin;
										CHARRANGE chrgNew;
										chrgNew.cpMin = start-1;
										chrgNew.cpMax = start;
										do
										{
											memset(&chf, 0, sizeof(CHARFORMAT2));
											chf.cbSize = sizeof(CHARFORMAT2);
											chf.dwMask = CFM_LINK;
											int sel = SendMessage(window,EM_EXSETSEL,0,(LPARAM)&chrgNew);
											if(sel != chrgNew.cpMax)
												break;
											SendMessage(window, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&chf);
											--chrgNew.cpMin;
											--chrgNew.cpMax;
											--start;
										} while(start >= 0 && chf.dwEffects & CFE_LINK);

										++start;
										chrgNew.cpMin = end;
										chrgNew.cpMax = end + 1;
										do
										{
											memset(&chf, 0, sizeof(CHARFORMAT2));
											chf.cbSize = sizeof(CHARFORMAT2);
											chf.dwMask = CFM_LINK;
											int sel = SendMessage(window,EM_EXSETSEL,0,(LPARAM)&chrgNew);
											if(sel != chrgNew.cpMax)
												break;
											SendMessage(window, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&chf);
											++chrgNew.cpMin;
											++chrgNew.cpMax;
											++end;
										} while(chf.dwEffects & CFE_LINK);

										--end;
										if(selected == IDM_COPYLINK)
										{
											chrgNew.cpMin = start;
											chrgNew.cpMax = end;
											SendMessage(window,EM_EXSETSEL,0,(LPARAM)&chrgNew);
											SendMessage(window,WM_COPY,0,0);
											SendMessage(window,EM_EXSETSEL,0,(LPARAM)&chrg);
										}
										else
										{
											ENLINK link;
											link.chrg.cpMin = start;
											link.chrg.cpMax = end;
											link.msg = WM_LBUTTONUP;
											link.nmhdr.code = selected;
											SendMessage(window,EM_EXSETSEL,0,(LPARAM)&chrg);
											ClickLink(window, &link);
										}
									}
									break;
								}

								DestroyMenu(hPopupMenu);
							}
							SendMessage(window,EM_SETSCROLLPOS,0,(LPARAM)&scrool);
							DlgReturn(TRUE);
						}
					}
				break;
				case IDC_LIST:
					if( pNmhdr->code == LVN_ITEMCHANGED)
					{
						NMLISTVIEW  *nmlv = (NMLISTVIEW*)lParam;
						HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
						if((nmlv->uChanged & LVIF_STATE) && (nmlv->uNewState & LVIS_SELECTED) && historyWindow->selected != nmlv->iItem && nmlv->iItem >= 0)
						{
							historyWindow->SelectEventGroup(nmlv->iItem);
							DlgReturn(TRUE);
						}
					}
					else if( pNmhdr->code == LVN_KEYDOWN)
					{
						LPNMLVKEYDOWN  nmlv = (LPNMLVKEYDOWN)lParam;
						HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
						if(historyWindow->DoHotkey(WM_KEYDOWN, 0, nmlv->wVKey, IDC_LIST))
							DlgReturn(TRUE);
					}
					else if(pNmhdr->code ==  NM_RCLICK)
					{
						HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
						POINT clicked;
						LPNMITEMACTIVATE nmlv = (LPNMITEMACTIVATE)lParam;
						HWND window = historyWindow->listWindow;
						LVHITTESTINFO info = {0};
						clicked.x = info.pt.x = nmlv->ptAction.x;
						clicked.y = info.pt.y = nmlv->ptAction.y;
						ClientToScreen(window, &clicked);
						int newSel = SendMessage(window, LVM_SUBITEMHITTEST, 0, (LPARAM)&info);
						int curSel = historyWindow->selected;

						if(newSel >= 0)
						{ 
							HMENU hPopupMenu = CreatePopupMenu();
							if(hPopupMenu != NULL)
							{
								AppendMenu(hPopupMenu, MF_STRING, IDM_COPY, TranslateT("Copy"));
								AppendMenu(hPopupMenu, MF_STRING, IDM_DELETEGROUP, TranslateT("Delete Group"));
								AppendMenu(hPopupMenu, MFT_SEPARATOR, 0, NULL);
								AppendMenu(hPopupMenu, MF_STRING, IDM_MESSAGE, TranslateT("Send Message"));
								AppendMenu(hPopupMenu, MF_STRING, IDM_DELETEUSER, TranslateT("Delete All User History"));

								int selected = TrackPopupMenu(hPopupMenu, TPM_RETURNCMD, clicked.x, clicked.y, 0, hwndDlg, 0);
								switch (selected)
								{
								case IDM_COPY:
									{
										CHARRANGE chrg;
										SendMessage(historyWindow->editWindow,EM_EXGETSEL,0,(LPARAM)&chrg);
										CHARRANGE chrgNew;
										chrgNew.cpMin = 0;
										chrgNew.cpMax = -1;
										SendMessage(historyWindow->editWindow,EM_EXSETSEL,0,(LPARAM)&chrgNew);
										SendMessage(historyWindow->editWindow,WM_COPY,0,0);
										SendMessage(historyWindow->editWindow,EM_EXSETSEL,0,(LPARAM)&chrg);
									}
									break;
								case IDM_MESSAGE:
									CallService(MS_MSG_SENDMESSAGE, (WPARAM)historyWindow->hContact, 0);
									break;
								case IDM_DELETEGROUP:
									historyWindow->Delete(1);
									break;
								case IDM_DELETEUSER:
									historyWindow->Delete(2);
									break;
								}

								DestroyMenu(hPopupMenu);
							}
						}
						
						DlgReturn(TRUE);
					}

					break;
				case IDC_TOOLBAR:        
					if( pNmhdr->code == TBN_DROPDOWN)
					{   
						LPNMTOOLBAR lpnmTB= (LPNMTOOLBAR)lParam;
						HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
						if(lpnmTB->iItem == IDM_FIND)
						{
							historyWindow->FindToolbarClicked(lpnmTB);
						}
						else if(lpnmTB->iItem == IDM_CONFIG)
						{
							historyWindow->ConfigToolbarClicked(lpnmTB);
						}
						else if(lpnmTB->iItem == IDM_DELETE)
						{
							historyWindow->DeleteToolbarClicked(lpnmTB);
						}
						
						DlgReturn(TBDDRET_DEFAULT);
					}
					else if( pNmhdr->code == NM_KEYDOWN)
					{
						LPNMKEY  nmlv = (LPNMKEY)lParam;
						HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
						if(historyWindow->DoHotkey(WM_KEYDOWN, 0, nmlv->nVKey, IDC_TOOLBAR))
							DlgReturn(TRUE);
					}
					break;
			}
			break;
		}
	case WM_CTLCOLORDLG:
		{
			HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
			DlgReturn((LONG_PTR)historyWindow->bkBrush);
		}
	case DM_SPLITTERMOVED: 
		{
			HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
			historyWindow->SplitterMoved((HWND)lParam, wParam, true);
			break;
		}
	case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwndDlg);
			HistoryWindow* historyWindow = (HistoryWindow*)lParam;
			historyWindow->hWnd = hwndDlg;
			historyWindow->isWnd = true;
			SetWindowLongPtr(hwndDlg,GWLP_USERDATA,(LONG_PTR)lParam);
			historyWindow->Initialise();
		}
		DlgReturn(TRUE);

	case DM_HREBUILD:
		{
			HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
			if(!historyWindow->isLoading)
			{
				historyWindow->isLoading = true;
				historyWindow->ReloadContacts();
				mir_forkthread(HistoryWindow::FillHistoryThread, historyWindow);
			}
		}
		DlgReturn(TRUE);

	case WM_DESTROY:
		{
			HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwndDlg,GWLP_USERDATA);
			historyWindow->Destroy();
		}
		DlgReturn(TRUE);
	}
	return FALSE;
}

void HistoryWindow::Initialise()
{
	OldSplitterProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(hWnd, IDC_SPLITTER), GWLP_WNDPROC, (LONG_PTR) SplitterSubclassProc);
	SetWindowLongPtr(GetDlgItem(hWnd, IDC_SPLITTERV), GWLP_WNDPROC, (LONG_PTR) SplitterSubclassProc);

	editWindow = GetDlgItem(hWnd, IDC_EDIT);
	findWindow = GetDlgItem(hWnd, IDC_FIND_TEXT);
	toolbarWindow = GetDlgItem(hWnd, IDC_TOOLBAR);
	listWindow = GetDlgItem(hWnd, IDC_LIST);

	RECT rc;
	POINT pt;
	GetWindowRect(GetDlgItem(hWnd, IDC_SPLITTER), &rc);
	pt.y = (rc.top + rc.bottom) / 2;
	pt.x = 0;
	ScreenToClient(hWnd, &pt);
	splitterOrgY = pt.y;
	splitterY = pt.y;
	GetWindowRect(GetDlgItem(hWnd, IDC_SPLITTERV), &rc);
	pt.y = 0;
	pt.x = (rc.left + rc.right) / 2;
	ScreenToClient(hWnd, &pt);
	splitterOrgX = pt.x;
	splitterX = pt.x;
	GetWindowRect(GetDlgItem(hWnd, IDC_LIST_CONTACTS), &rc);
	pt.y = rc.top;
	pt.x = rc.left;
	ScreenToClient(hWnd, &pt);
	listOryginalPos = pt.x;

	plusIco = (HICON)CallService(MS_SKIN2_GETICONBYHANDLE, 1, (LPARAM)hPlusIcon);
	minusIco = (HICON)CallService(MS_SKIN2_GETICONBYHANDLE, 1, (LPARAM)hMinusIcon);
	SendDlgItemMessage( hWnd, IDC_SHOWHIDE, BUTTONSETASPUSHBTN, 0, 0 );
	SendDlgItemMessage( hWnd, IDC_SHOWHIDE, BUTTONSETASFLATBTN, 0, 0 );
	if(hContact == NULL || Options::instance->showContacts)
	{
		SendDlgItemMessage( hWnd, IDC_SHOWHIDE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)minusIco);
		SendDlgItemMessage( hWnd, IDC_SHOWHIDE, BUTTONADDTOOLTIP, (WPARAM)LPGENT("Hide Contacts"), BATF_TCHAR);
		Button_SetCheck(GetDlgItem(hWnd,IDC_SHOWHIDE), BST_CHECKED);
		isContactList = true;
	}
	else
	{
		SendDlgItemMessage( hWnd, IDC_SHOWHIDE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)plusIco);
		SendDlgItemMessage( hWnd, IDC_SHOWHIDE, BUTTONADDTOOLTIP, (WPARAM)LPGENT("Show Contacts"), BATF_TCHAR);
		Button_SetCheck(GetDlgItem(hWnd,IDC_SHOWHIDE), BST_UNCHECKED);
		ShowWindow(GetDlgItem(hWnd,IDC_LIST_CONTACTS), SW_HIDE);
		ShowWindow(GetDlgItem(hWnd,IDC_SPLITTERV), SW_HIDE);
		isContactList = false;
	}
	RegisterHotkeyControl(GetDlgItem(hWnd, IDC_SHOWHIDE));
	RegisterHotkeyControl(GetDlgItem(hWnd, IDC_LIST_CONTACTS));

	SendDlgItemMessage(hWnd, IDC_LIST_CONTACTS, CLM_SETUSEGROUPS, Options::instance->showContactGroups, 0);
			
	RestorePos();
	SendMessage(hWnd, WM_SETICON, ICON_BIG,   ( LPARAM )LoadSkinnedIconBig( SKINICON_OTHER_HISTORY ));
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, ( LPARAM )LoadSkinnedIcon( SKINICON_OTHER_HISTORY ));
	SendMessage(editWindow,EM_AUTOURLDETECT,TRUE,0);
	SendMessage(editWindow,EM_SETEVENTMASK,0,ENM_LINK | ENM_SELCHANGE | ENM_KEYEVENTS | ENM_MOUSEEVENTS);
	SendMessage(editWindow,EM_SETEDITSTYLE,SES_EXTENDBACKCOLOR,SES_EXTENDBACKCOLOR);
			
	himlSmall = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 2);
	himlNone = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 2);
	ImageList_SetIconSize(himlNone, 0, 16);
	if(himlSmall)
	{
		allIconNumber = iconsNum + 3;
		eventIcoms = new HICON[allIconNumber];
		for(int i = 0; i < iconsNum; ++i)
		{
			eventIcoms[i] = hEventIcons[i] == NULL ? NULL : (HICON)CallService(MS_SKIN2_GETICONBYHANDLE, 0, (LPARAM)hEventIcons[i]);
			ImageList_AddIcon(himlSmall, eventIcoms[i]);
		}

		int id = iconsNum;
		eventIcoms[id] = LoadSkinnedIcon(SKINICON_EVENT_FILE);
		ImageList_AddIcon(himlSmall, eventIcoms[id]);

		eventIcoms[++id] = LoadSkinnedIcon(SKINICON_EVENT_URL);
		ImageList_AddIcon(himlSmall, eventIcoms[id]);

		eventIcoms[++id] = LoadSkinnedIcon(SKINICON_OTHER_WINDOWS);
		ImageList_AddIcon(himlSmall, eventIcoms[id]);

		if((isGroupImages = Options::instance->groupShowEvents) != false)
			ListView_SetImageList(listWindow, himlSmall, LVSIL_SMALL);
	}
	
	bkBrush = CreateSolidBrush(Options::instance->GetColor(Options::WindowBackground));

	LVCOLUMN col = {0};
	col.mask = LVCF_WIDTH | LVCF_TEXT;
	col.cx = 470;
	col.pszText = _T("");
	ListView_InsertColumn(listWindow, 0, &col);
	ListView_SetColumnWidth(listWindow, 0, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetExtendedListViewStyleEx(listWindow, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	COLORREF bkColor = Options::instance->GetColor(Options::GroupListBackground);
	ListView_SetBkColor(listWindow, bkColor);
	ListView_SetTextBkColor(listWindow, bkColor);
	LOGFONT font;
	ListView_SetTextColor(listWindow, Options::instance->GetFont(Options::GroupList, &font));

	Edit_LimitText(findWindow, 100);
	RegisterHotkeyControl(findWindow);
			
	HIMAGELIST himlButtons = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 3, 3);
	if(himlButtons)
	{
		findNextIco = (HICON)CallService(MS_SKIN2_GETICONBYHANDLE, 0, (LPARAM)hFindNextIcon);
		ImageList_AddIcon(himlButtons, findNextIco);
		findPrevIco = (HICON)CallService(MS_SKIN2_GETICONBYHANDLE, 0, (LPARAM)hFindPrevIcon);
		ImageList_AddIcon(himlButtons, findPrevIco);
		configIco = LoadSkinnedIcon(SKINICON_OTHER_OPTIONS);
		ImageList_AddIcon(himlButtons, configIco);
		deleteIco = LoadSkinnedIcon(SKINICON_OTHER_DELETE);
		ImageList_AddIcon(himlButtons, deleteIco);
				
		// Set the image list.
		SendMessage(toolbarWindow, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)himlButtons);

		// Load the button images.
		SendMessage(toolbarWindow, TB_LOADIMAGES, (WPARAM)IDB_STD_SMALL_COLOR, (LPARAM)HINST_COMMCTRL);
	}

	TBBUTTON tbButtons[] = 
	{
		{ 0, IDM_FIND,  TBSTATE_ENABLED, BTNS_DROPDOWN, {0}, 0, (INT_PTR)TranslateT("Find Next") },
		{ 3, IDM_DELETE, TBSTATE_ENABLED,  BTNS_DROPDOWN, {0}, 0, (INT_PTR)TranslateT("Delete")},
		{ 2, IDM_CONFIG, TBSTATE_ENABLED,  BTNS_DROPDOWN, {0}, 0, (INT_PTR)TranslateT("Options")},
	};    
	SendMessage(toolbarWindow, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(toolbarWindow, TB_ADDBUTTONS,       (WPARAM)SIZEOF(tbButtons),       (LPARAM)&tbButtons);
	SendMessage(toolbarWindow, TB_SETBUTTONSIZE, 0, MAKELPARAM(16, 16));
	SendMessage(toolbarWindow, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);
	SendMessage(toolbarWindow, TB_SETMAXTEXTROWS, 0, 0);

	SetDefFilter(Options::instance->defFilter);
			
	SendMessage(hWnd, DM_SETDEFID, IDM_FIND, 0);
	SendMessage(hWnd, WM_SIZE, 0, 0);
	SendMessage(hWnd,DM_HREBUILD,0,0);
	isDestroyed = false;
}

void HistoryWindow::Destroy()
{
	HICON hIcon = (HICON)SendMessage(hWnd, WM_SETICON, ICON_BIG, 0);
	CallService(MS_SKIN2_RELEASEICON, (WPARAM)hIcon, 0);
	
	hIcon = (HICON)SendMessage(hWnd, WM_SETICON, ICON_SMALL, 0);
	CallService(MS_SKIN2_RELEASEICON, (WPARAM)hIcon, 0);
			
	UnregisterHotkeyControl(GetDlgItem(hWnd, IDC_SHOWHIDE));
	UnregisterHotkeyControl(GetDlgItem(hWnd, IDC_LIST_CONTACTS));
	UnregisterHotkeyControl(findWindow);

	isDestroyed = true;
	HistoryWindow::Close(this);
}

void HistoryWindow::SplitterMoved(HWND splitter, LONG pos, bool screenPos)
{
	POINT pt;
	RECT rc1;
	RECT rc2;
	POINT pt1;
	POINT pt2;
			
	if(splitter == GetDlgItem(hWnd, IDC_SPLITTER))
	{
		GetWindowRect(listWindow, &rc1);
		GetWindowRect(editWindow, &rc2);
		pt.x = 0;
		pt.y = pos;
		pt1.x = rc1.left;
		pt1.y = rc1.top;
		pt2.x = rc2.right;
		pt2.y = rc2.bottom;
		if(screenPos)
			ScreenToClient(hWnd, &pt);
		ScreenToClient(hWnd, &pt1);
		ScreenToClient(hWnd, &pt2);
		if ((pt.y >= pt1.y + MIN_PANELHEIGHT) && (pt.y < pt2.y - MIN_PANELHEIGHT))
		{
			splitterY = pt.y;
			if(!screenPos)
				SendMessage(hWnd, WM_SIZE, 0, 0);
			//if(M->isAero())
			//	InvalidateRect(GetParent(hwndDlg), NULL, FALSE);
		}
	}
	else
	{
		GetWindowRect(GetDlgItem(hWnd, IDC_LIST_CONTACTS), &rc1);
		GetWindowRect(listWindow, &rc2);
		pt.x = pos;
		pt.y = 0;
		pt1.x = rc1.left;
		pt1.y = rc1.top;
		pt2.x = rc2.right;
		pt2.y = rc2.bottom;
		if(screenPos)
			ScreenToClient(hWnd, &pt);
		ScreenToClient(hWnd, &pt1);
		ScreenToClient(hWnd, &pt2);
		if ((pt.x >= pt1.x + MIN_PANELHEIGHT) && (pt.x < pt2.x - MIN_PANELHEIGHT))
		{
			splitterX = pt.x;
			if(!screenPos)
				SendMessage(hWnd, WM_SIZE, 0, 0);
			//if(M->isAero())
			//	InvalidateRect(GetParent(hwndDlg), NULL, FALSE);
		}
	}
}

int HistoryWindow::HistoryDlgResizer(HWND hwnd, LPARAM, UTILRESIZECONTROL *urc)
{
	HistoryWindow* historyWindow =(HistoryWindow*)GetWindowLongPtr(hwnd,GWLP_USERDATA);
	switch(urc->wId) {
	case IDC_LIST:
		{
			urc->rcItem.bottom += historyWindow->splitterY - historyWindow->splitterOrgY;
			urc->rcItem.left += historyWindow->splitterX - historyWindow->splitterOrgX;
			if(!historyWindow->isContactList)
				urc->rcItem.left = historyWindow->listOryginalPos;
			return RD_ANCHORX_WIDTH|RD_ANCHORY_TOP;
		}
	case IDC_LIST_CONTACTS:
		{
			urc->rcItem.right += historyWindow->splitterX - historyWindow->splitterOrgX;
			return RD_ANCHORX_LEFT|RD_ANCHORY_HEIGHT;
		}
	case IDC_SPLITTER:
		{
			urc->rcItem.top += historyWindow->splitterY - historyWindow->splitterOrgY;
			urc->rcItem.bottom += historyWindow->splitterY - historyWindow->splitterOrgY;
			urc->rcItem.left += historyWindow->splitterX - historyWindow->splitterOrgX;
			if(!historyWindow->isContactList)
				urc->rcItem.left = 0;
			return RD_ANCHORX_WIDTH|RD_ANCHORY_TOP;
		}
	case IDC_SPLITTERV:
		{
			urc->rcItem.left += historyWindow->splitterX - historyWindow->splitterOrgX;
			urc->rcItem.right += historyWindow->splitterX - historyWindow->splitterOrgX;
			return RD_ANCHORX_LEFT|RD_ANCHORY_HEIGHT;
		}
	case IDC_EDIT:
		{
			urc->rcItem.top += historyWindow->splitterY - historyWindow->splitterOrgY;
			urc->rcItem.left += historyWindow->splitterX - historyWindow->splitterOrgX;
			if(!historyWindow->isContactList)
				urc->rcItem.left = historyWindow->listOryginalPos;
			return RD_ANCHORX_WIDTH|RD_ANCHORY_HEIGHT;
		}
	case IDC_FIND_TEXT:
		return RD_ANCHORX_WIDTH|RD_ANCHORY_TOP;
	case IDC_SHOWHIDE:
		return RD_ANCHORX_LEFT|RD_ANCHORY_TOP;
	case IDC_TOOLBAR:
		return RD_ANCHORX_RIGHT|RD_ANCHORY_TOP;
	}
	return RD_ANCHORX_LEFT|RD_ANCHORY_TOP;
}

void HistoryWindow::FillHistoryThread(void* param)
{
	HistoryWindow *hInfo = ( HistoryWindow* )param;
	HWND hwndList = hInfo->listWindow;
	ListView_DeleteAllItems(hwndList);
	hInfo->SelectEventGroup(-1);
	hInfo->EnableWindows(FALSE);
	int i=CallService(MS_DB_EVENT_GETCOUNT,(WPARAM)hInfo->hContact,0);
	ListView_SetItemCount(hwndList, i);
	bool isNewOnTop = Options::instance->groupNewOnTop;

	hInfo->RefreshEventList();

	LVITEM item = {0};
	item.mask = LVIF_STATE;
	item.iItem = 0;
	item.state = LVIS_SELECTED;
	item.stateMask = LVIS_SELECTED;
	if(!isNewOnTop)
	{
		item.iItem = ListView_GetItemCount(hwndList) - 1;
		if(item.iItem < 0)
			item.iItem = 0;
	}

	ListView_SetItem(hwndList, &item);
	ListView_SetColumnWidth(hwndList, 0, LVSCW_AUTOSIZE_USEHEADER);
	ListView_EnsureVisible(hwndList, item.iItem, FALSE);
	hInfo->EnableWindows(TRUE);
	SetFocus(hwndList);
}

void HistoryWindow::AddGroup(bool isMe, const std::wstring &time, const std::wstring &user, const std::wstring &eventText, int ico)
{
	TCHAR msg[256];
	msg[0] = 0;
	if(Options::instance->groupShowTime)
	{
		_tcscpy_s(msg, time.c_str());
	}
	if(Options::instance->groupShowName)
	{
		if(msg[0] != 0)
			_tcscat_s(msg, _T(" "));
		_tcscat_s(msg, user.c_str());
	}

	if(Options::instance->groupShowMessage)
	{
		if(msg[0] != 0)
			_tcscat_s(msg, _T(" "));
		_tcscat_s(msg, eventText.c_str());
	}

	LVITEM item = {0};
	item.mask = LVIF_TEXT | LVIF_IMAGE;
	item.iItem = MAXINT;
	item.pszText = msg;
	item.iImage = ico;
	ListView_InsertItem(listWindow, &item);
}

void HistoryWindow::ReplaceIcons(HWND hwndDlg, int selStart, BOOL isSent)
{
	if(g_SmileyAddAvail)
	{
		CHARRANGE sel;
		SMADD_RICHEDIT3 smadd = {0};

		sel.cpMin = selStart;
		sel.cpMax = -1;

		smadd.cbSize = sizeof(smadd);
		smadd.hwndRichEditControl = hwndDlg;
		smadd.Protocolname = (char *)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
		smadd.hContact = hContact;
		smadd.flags = isSent ? SAFLRE_OUTGOING : 0;
		if (selStart > 0)
			smadd.rangeToReplace = &sel;
		CallService(MS_SMILEYADD_REPLACESMILEYS, 0, (LPARAM)&smadd);
	}
}

void SetFontFromOptions(ITextFont *TextFont, int caps, Options::Fonts fontId)
{
	COLORREF fontColor;
	LOGFONT font;
	fontColor = Options::instance->GetFont(fontId, &font);
	BSTR bstrFont = SysAllocString(font.lfFaceName);
	TextFont->SetName(bstrFont);
	SysFreeString(bstrFont);
	TextFont->SetForeColor(fontColor);
	TextFont->SetWeight(font.lfWeight);
	font.lfHeight = (font.lfHeight * 72) / caps;
	TextFont->SetSize(font.lfHeight < 0 ? -font.lfHeight : font.lfHeight);
	TextFont->SetItalic(font.lfItalic ? tomTrue : tomFalse);
	TextFont->SetItalic(font.lfUnderline ? tomTrue : tomFalse);
	TextFont->SetItalic(font.lfStrikeOut ? tomTrue : tomFalse);
}

void HistoryWindow::SelectEventGroup(int sel)
{
	SendMessage(editWindow, WM_SETTEXT, 0, (LPARAM)_T(""));
	currentGroup.clear();
	selected = sel;
	if(sel < 0 || sel >= eventList.size())
		return;

#define MAXSELECTSTR 8184
	TCHAR _str[MAXSELECTSTR + 8]; // for safety reason
	TCHAR *str = _str + sizeof(int)/sizeof(TCHAR);
	BSTR pStr = str;
	int *strLen = (int*)_str;
	DBEVENTINFO dbei = {0};
	DWORD newBlobSize,oldBlobSize;
	dbei.cbSize=sizeof(dbei);
	oldBlobSize = 0;
	str[0] = 0;
	tm lastTime;
	bool isFirst = true;
	bool lastMe = false;
	long startAt, endAt;
	long cnt;
	std::wstring strStl;
	IRichEditOle* RichEditOle;
	if (SendMessage(editWindow, EM_GETOLEINTERFACE, 0, (LPARAM)&RichEditOle) == 0)
		return;
	ITextDocument* TextDocument;
	if (RichEditOle->QueryInterface(IID_ITextDocument, (void**)&TextDocument) != S_OK)
	{
		RichEditOle->Release();
		return;
	}
	ITextSelection* TextSelection;
	ITextFont *TextFont;
	SendMessage(editWindow, EM_SETREADONLY, FALSE, 0);
	TextDocument->Freeze(&cnt);
	TextDocument->GetSelection(&TextSelection);
	HDC hDC =  GetDC(NULL);
	int caps = GetDeviceCaps(hDC, LOGPIXELSY);
	std::deque<HANDLE> revDeq;
	std::deque<HANDLE>& deq = eventList[sel];
	if(Options::instance->messagesNewOnTop)
	{
		revDeq.insert(revDeq.begin(), deq.rbegin(), deq.rend());
		deq = revDeq;
	}
	COLORREF backColor = GetSysColor(COLOR_WINDOW);
	for(std::deque<HANDLE>::iterator it = deq.begin(); it != deq.end(); ++it)
	{
		HANDLE hDbEvent = *it;
		newBlobSize=CallService(MS_DB_EVENT_GETBLOBSIZE,(WPARAM)hDbEvent,0);
		if ((int)dbei.cbBlob != -1)
		{
			if(newBlobSize>oldBlobSize) 
			{
				dbei.pBlob=(PBYTE)mir_realloc(dbei.pBlob,newBlobSize);
				oldBlobSize=newBlobSize;
			}
			dbei.cbBlob = oldBlobSize;
			if (CallService(MS_DB_EVENT_GET,(WPARAM)hDbEvent,(LPARAM)&dbei) == 0)
			{
				bool isUser = Options::instance->messagesShowName && (isFirst || (!lastMe && (dbei.flags & DBEF_SENT)) || (lastMe && !(dbei.flags & DBEF_SENT)));
				lastMe = dbei.flags & DBEF_SENT;
				backColor = Options::instance->GetColor(lastMe ? Options::OutBackground : Options::InBackground);
				if(Options::instance->messagesShowEvents)
				{
					str[0] = _T('>');
					str[1] = 0;
					*strLen = 1 * sizeof(TCHAR);
					TextSelection->SetStart(MAXLONG);
					TextSelection->GetFont(&TextFont);
					TextFont->SetBackColor(backColor);
					TextSelection->SetText(pStr);
					TextFont->Release();
					int imId;
					HICON ico;
					if(GetEventIcon(lastMe, dbei.eventType, imId))
					{
						ico = eventIcoms[imId];
					}
					else
					{
						ico = (HICON)CallService(MS_DB_EVENT_GETICON, LR_SHARED, (LPARAM)&dbei);
						HICON icoMsg = LoadSkinnedIcon(SKINICON_EVENT_MESSAGE);
						if(ico == NULL || icoMsg == ico)
						{
							ico = eventIcoms[imId];
						}
					}

					ImageDataObject::InsertIcon(RichEditOle, ico, backColor, 16, 16);
				}

				TCHAR* formatDate = Options::instance->messagesShowSec ? (isUser ? _T("d s ") : _T("d s\n")) : (isUser ? _T("d t ") : _T("d t\n"));
				if(!Options::instance->messagesShowDate)
				{
					if(isFirst)
					{
						isFirst = false;
						formatDate = Options::instance->messagesShowSec ? (isUser ? _T("s ") : _T("s\n")) : (isUser ? _T("t ") : _T("t\n"));
						time_t tt = dbei.timestamp;
						lastTime = *localtime(&tt);
					}
					else
					{
						time_t tt = dbei.timestamp;
						tm* t = localtime(&tt);
						if(lastTime.tm_yday == t->tm_yday && lastTime.tm_year == t->tm_year)
							formatDate = Options::instance->messagesShowSec ? (isUser ? _T("s ") : _T("s\n")) : (isUser ? _T("t ") : _T("t\n"));
					}
				}

				tmi.printTimeStamp(NULL, dbei.timestamp, formatDate, str , MAXSELECTSTR, 0);
				*strLen = _tcslen(str) * sizeof(TCHAR);
				TextSelection->SetStart(MAXLONG);
				TextSelection->GetFont(&TextFont);
				SetFontFromOptions(TextFont, caps, lastMe ? Options::OutTimestamp : Options::InTimestamp);
				TextFont->SetBackColor(backColor);
				TextSelection->SetText(pStr);
				TextFont->Release();

				if(isUser)
				{
					if(lastMe)
						mir_sntprintf( str, MAXSELECTSTR, _T("%s\n"), myName );
					else
						mir_sntprintf( str, MAXSELECTSTR, _T("%s\n"), contactName );
					*strLen = _tcslen(str) * sizeof(TCHAR);
					TextSelection->SetStart(MAXLONG);
					TextSelection->GetFont(&TextFont);
					SetFontFromOptions(TextFont, caps, lastMe ? Options::OutName : Options::InName);
					TextSelection->SetText(pStr);
					TextFont->Release();
				}
				
				GetObjectDescription(&dbei,str, MAXSELECTSTR);
				strStl = str;
				int i = strStl.length();
				if(i + 1 >= MAXSELECTSTR)
					continue;
				str[i++] = _T('\n');
				str[i] = 0;
				*strLen = i * sizeof(TCHAR);
				TextSelection->SetStart(MAXLONG);
				TextSelection->GetFont(&TextFont);
				SetFontFromOptions(TextFont, caps, lastMe ? Options::OutMessages : Options::InMessages);
				TextFont->SetBackColor(backColor);
				TextSelection->GetStart(&startAt);
				TextSelection->SetText(pStr);
				TextFont->Release();

				if(Options::instance->messagesUseSmileys)
					ReplaceIcons(editWindow, startAt, lastMe);
				TextSelection->SetStart(MAXLONG);
				TextSelection->GetStart(&endAt);
				currentGroup.push_back(MessageData(strStl, startAt, endAt, lastMe, dbei.timestamp));
			}
		}
	}
	
	TextSelection->SetRange(0, 0);
	TextSelection->Release();
	TextDocument->Unfreeze(&cnt);
	TextDocument->Release();
	RichEditOle->Release();
	SendMessage(editWindow, EM_SETREADONLY, TRUE, 0);
	SendMessage(editWindow,EM_SETBKGNDCOLOR,0, backColor);
	if (cnt == 0) 
	{
		UpdateWindow(editWindow);
	}
	
	mir_free(dbei.pBlob);
}

LRESULT CALLBACK HistoryWindow::SplitterSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hwndParent = GetParent(hwnd);
	HistoryWindow *dat = (HistoryWindow*)GetWindowLongPtr(hwndParent, GWLP_USERDATA);

	switch (msg) {
		case WM_NCHITTEST:
			return HTCLIENT;
		case WM_SETCURSOR: {
			RECT rc;
			GetClientRect(hwnd, &rc);
			SetCursor(rc.right > rc.bottom ? hCurSplitNS : hCurSplitWE);
			return TRUE;
		}
		case WM_LBUTTONDOWN: {
			SetCapture(hwnd);
			return 0;
		}
		case WM_MOUSEMOVE:
			if (GetCapture() == hwnd) {
				RECT rc;
				GetClientRect(hwnd, &rc);
				SendMessage(hwndParent, DM_SPLITTERMOVED, rc.right > rc.bottom ? (short) HIWORD(GetMessagePos()) + rc.bottom / 2 : (short) LOWORD(GetMessagePos()) + rc.right / 2, (LPARAM) hwnd);
			}
			return 0;
		case WM_ERASEBKGND:
			return(1);
		case WM_LBUTTONUP: {
			HWND hwndCapture = GetCapture();

			ReleaseCapture();
			SendMessage(hwndParent, WM_SIZE, 0, 0);
			RedrawWindow(hwndParent, NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW);
			return 0;
		}
	}
	return CallWindowProc(dat->OldSplitterProc, hwnd, msg, wParam, lParam);
}

void HistoryWindow::EnableWindows(BOOL enable)
{
	EnableWindow(GetDlgItem(hWnd,IDC_LIST_CONTACTS), enable);
	EnableWindow(listWindow, enable);
	EnableWindow(findWindow, enable);
	EnableWindow(toolbarWindow, enable);
	isLoading = !enable;
}

void HistoryWindow::ReloadContacts()
{
	//ListBox_ResetContent(GetDlgItem(hWnd,IDC_LIST_CONTACTS));
	HWND contactList = GetDlgItem(hWnd,IDC_LIST_CONTACTS);
	HANDLE _hContact = (HANDLE) CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
	bool isEmpty = true;
	while(_hContact)
	{
		if(CallService(MS_DB_EVENT_GETCOUNT,(WPARAM) _hContact,0) && (metaContactProto == NULL || DBGetContactSettingByte(_hContact, metaContactProto, "IsSubcontact", 0) == 0))
		{
			/*TCHAR* d = (TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM) _hContact, GCDNF_TCHAR );
			int pos = ListBox_AddString(GetDlgItem(hWnd,IDC_LIST_CONTACTS), d);
			if(pos != LB_ERR && pos != LB_ERRSPACE)
			{
				isEmpty = false;
				ListBox_SetItemData(GetDlgItem(hWnd,IDC_LIST_CONTACTS), pos, _hContact);
			}*/

			HANDLE hItem = (HANDLE)SendMessage(contactList, CLM_FINDCONTACT, (WPARAM)_hContact, 0);
			if(hItem == NULL)
			{
				SendMessage(contactList, CLM_ADDCONTACT, (WPARAM)_hContact, 0);
			}
		}
		else
		{
			HANDLE hItem = (HANDLE)SendMessage(contactList, CLM_FINDCONTACT, (WPARAM)_hContact, 0);
			if(hItem != NULL)
			{
				SendMessage(contactList, CLM_DELETEITEM, (WPARAM)_hContact, 0);
			}
		}

		_hContact = (HANDLE) CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)_hContact, 0);
	}

	if(hContact != NULL)
	{
		HANDLE hItem = (HANDLE)SendMessage(contactList, CLM_FINDCONTACT, (WPARAM)hContact, 0);
		if(hItem != NULL)
		{
			SendMessage(contactList, CLM_ENSUREVISIBLE, (WPARAM)hItem, 0);
			SendMessage(contactList, CLM_SELECTITEM, (WPARAM)hItem, 0);
		}
	}

	/*
	int count = ListBox_GetCount(GetDlgItem(hWnd,IDC_LIST_CONTACTS));
	for(int i = 0; i < count; ++i)
	{
		_hContact = (HANDLE)ListBox_GetItemData(GetDlgItem(hWnd,IDC_LIST_CONTACTS), i);
		if(_hContact == hContact)
		{
			ListBox_SetCurSel(GetDlgItem(hWnd,IDC_LIST_CONTACTS), i);
			break;
		}
	}*/
}

bool HistoryWindow::DoHotkey(UINT msg, LPARAM lParam, WPARAM wParam, int window)
{
	MSG		message;
	message.hwnd = hWnd;
	message.message = msg;
	message.lParam = lParam;
	message.wParam = wParam;
	LRESULT mim_hotkey_check = CallService(MS_HOTKEY_CHECK, (WPARAM)&message, (LPARAM)("History"));
	switch(mim_hotkey_check)
	{
	case HISTORY_HK_FIND:
		SetFocus(findWindow);
		Edit_SetSel(findWindow, 0, -1);
		break;
	case HISTORY_HK_FINDNEXT:
		searcher.ChangeFindDirection(false);
		break;
	case HISTORY_HK_FINDPREV:
		searcher.ChangeFindDirection(true);
		break;
	case HISTORY_HK_MATCHCASE:
		searcher.SetMatchCase(!searcher.IsMatchCase());
		break;
	case HISTORY_HK_MATCHWHOLE:
		searcher.SetMatchWholeWords(!searcher.IsMatchWholeWords());
		break;
	case HISTORY_HK_SHOWCONTACTS:
		Button_SetCheck(GetDlgItem(hWnd, IDC_SHOWHIDE), Button_GetCheck(GetDlgItem(hWnd, IDC_SHOWHIDE)) & BST_CHECKED ? BST_UNCHECKED : BST_CHECKED);
        SendMessage(hWnd, WM_COMMAND, MAKELONG(IDC_SHOWHIDE, BN_CLICKED), NULL);
		break;
	case HISTORY_HK_ONLYIN:
		searcher.SetOnlyIn(!searcher.IsOnlyIn());
		break;
	case HISTORY_HK_ONLYOUT:
		searcher.SetOnlyOut(!searcher.IsOnlyOut());
		break;
	case HISTORY_HK_ONLYGROUP:
		searcher.SetOnlyGroup(!searcher.IsOnlyGroup());
		break;
	case HISTORY_HK_EXRHTML:
		{
			ExportManeger exp(hContact, GetFilterNr());
			exp.Export(IExport::RichHtml);
		}
		break;
	case HISTORY_HK_EXPHTML:
		{
			ExportManeger exp(hContact, GetFilterNr());
			exp.Export(IExport::PlainHtml);
		}
		break;
	case HISTORY_HK_EXTXT:
		{
			ExportManeger exp(hContact, GetFilterNr());
			exp.Export(IExport::Txt);
		}
		break;
	case HISTORY_HK_DELETE:
		{
			int what = window == IDC_EDIT ? 0 : (window == IDC_LIST ? 1 : (window == IDC_LIST_CONTACTS ? 2 : -1));
			Delete(what);
			return what != -1;
		}
		break;
	default:
		return false;
	}

	return true;
}

void HistoryWindow::RestorePos()
{
	HANDLE contactToLoad = hContact;
	if(hContact == NULL)
	{
		Utils_RestoreWindowPosition(hWnd,NULL,"BasicHistory","history_");
		contactToLoad = NULL;
	}
	else if(Utils_RestoreWindowPosition(hWnd,hContact,"BasicHistory","history_") != 0)
	{
		Utils_RestoreWindowPosition(hWnd,NULL,"BasicHistory","history_");
		contactToLoad = NULL;
	}
	if(DBGetContactSettingByte(contactToLoad, "BasicHistory", "history_ismax", 0))
	{
		ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	}

	LONG pos = DBGetContactSettingDword(contactToLoad, "BasicHistory", "history_splitterv", 0);
	if(pos > 0)
	{
		SplitterMoved(GetDlgItem(hWnd, IDC_SPLITTERV), pos, false);
	}

	pos = DBGetContactSettingDword(contactToLoad, "BasicHistory", "history_splitter", 0);
	if(pos > 0)
	{
		SplitterMoved(GetDlgItem(hWnd, IDC_SPLITTER), pos, false);
	}
}

void HistoryWindow::SavePos(bool all)
{
	HANDLE contactToSave = hContact;
	if(all)
	{
		HANDLE _hContact = (HANDLE) CallService(MS_DB_CONTACT_FINDFIRST, 0, 0);
		while(_hContact)
		{
			DBDeleteContactSetting(_hContact, "BasicHistory", "history_x");
			DBDeleteContactSetting(_hContact, "BasicHistory", "history_y");
			DBDeleteContactSetting(_hContact, "BasicHistory", "history_width");
			DBDeleteContactSetting(_hContact, "BasicHistory", "history_height");
			DBDeleteContactSetting(_hContact, "BasicHistory", "history_ismax");
			DBDeleteContactSetting(_hContact, "BasicHistory", "history_splitterv");
			DBDeleteContactSetting(_hContact, "BasicHistory", "history_splitter");
			_hContact = (HANDLE) CallService(MS_DB_CONTACT_FINDNEXT, (WPARAM)_hContact, 0);
		}

		contactToSave = NULL;
	}
	
	Utils_SaveWindowPosition(hWnd,contactToSave,"BasicHistory","history_");
	WINDOWPLACEMENT wp;
	wp.length=sizeof(wp);
	GetWindowPlacement(hWnd,&wp);
	DBWriteContactSettingByte(contactToSave, "BasicHistory", "history_ismax", wp.showCmd == SW_MAXIMIZE ? 1 : 0);
	DBWriteContactSettingDword(contactToSave, "BasicHistory", "history_splitterv", splitterX);
	DBWriteContactSettingDword(contactToSave, "BasicHistory", "history_splitter", splitterY);
}

#define DEF_FILTERS_START 50000
void HistoryWindow::FindToolbarClicked(LPNMTOOLBAR lpnmTB)
{
	RECT rc;
	SendMessage(lpnmTB->hdr.hwndFrom, TB_GETRECT, (WPARAM)lpnmTB->iItem, (LPARAM)&rc); 
	MapWindowPoints(lpnmTB->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rc, 2);         
	HMENU hPopupMenu = CreatePopupMenu();
	if(hPopupMenu != NULL)
	{
		AppendMenu(hPopupMenu, MF_STRING, IDM_FINDNEXT, TranslateT("Find Next"));
		AppendMenu(hPopupMenu, MF_STRING, IDM_FINDPREV, TranslateT("Find Previous"));
		AppendMenu(hPopupMenu, MFT_SEPARATOR, 0, NULL);
		AppendMenu(hPopupMenu, searcher.IsMatchCase() ? MF_STRING | MF_CHECKED : MF_STRING, IDM_MATCHCASE, TranslateT("Match Case"));
		AppendMenu(hPopupMenu, searcher.IsMatchWholeWords() ? MF_STRING | MF_CHECKED : MF_STRING, IDM_MATCHWHOLE, TranslateT("Match Whole Word"));
		AppendMenu(hPopupMenu, searcher.IsOnlyIn() ? MF_STRING | MF_CHECKED : MF_STRING, IDM_ONLYIN, TranslateT("Only Incomming Messages"));
		AppendMenu(hPopupMenu, searcher.IsOnlyOut() ? MF_STRING | MF_CHECKED : MF_STRING, IDM_ONLYOUT, TranslateT("Only Outgoing Messages"));
		AppendMenu(hPopupMenu, searcher.IsOnlyGroup() ? MF_STRING | MF_CHECKED : MF_STRING, IDM_ONLYGROUP, TranslateT("Only Selected Group"));
		AppendMenu(hPopupMenu, MFT_SEPARATOR, 0, NULL);
		HMENU hFilterMenu = CreatePopupMenu();
		int filter = GetFilterNr();
		AppendMenu(hFilterMenu, filter == 0 ? MF_STRING | MF_CHECKED : MF_STRING, IDM_FILTERDEF, TranslateT("Default history events"));
		AppendMenu(hFilterMenu, filter == 1 ? MF_STRING | MF_CHECKED : MF_STRING, IDM_FILTERALL, TranslateT("All events"));
		for(int i = 0 ; i < Options::instance->customFilters.size(); ++i)
		{
			UINT flags = MF_STRING;
			if(filter - 2 == i)
				flags |= MF_CHECKED;

			AppendMenu(hFilterMenu, flags, DEF_FILTERS_START + i, Options::instance->customFilters[i].name.c_str());
		}
		AppendMenu(hPopupMenu, MF_STRING | MF_POPUP, (UINT_PTR)hFilterMenu, TranslateT("Filters"));
		if(searcher.IsFindBack())
			SetMenuDefaultItem(hPopupMenu, IDM_FINDPREV, FALSE);
		else
			SetMenuDefaultItem(hPopupMenu, IDM_FINDNEXT, FALSE);

		int selected = TrackPopupMenu(hPopupMenu, TPM_RETURNCMD, rc.left, rc.bottom, 0, hWnd, 0);
		switch (selected)
		{
		case IDM_FINDNEXT:
			searcher.ChangeFindDirection(false);
			break;
		case IDM_FINDPREV:
			searcher.ChangeFindDirection(true);
			break;
		case IDM_MATCHCASE:
			searcher.SetMatchCase(!searcher.IsMatchCase());
			break;
		case IDM_MATCHWHOLE:
			searcher.SetMatchWholeWords(!searcher.IsMatchWholeWords());
			break;
		case IDM_ONLYIN:
			searcher.SetOnlyIn(!searcher.IsOnlyIn());
			break;
		case IDM_ONLYOUT:
			searcher.SetOnlyOut(!searcher.IsOnlyOut());
			break;
		case IDM_ONLYGROUP:
			searcher.SetOnlyGroup(!searcher.IsOnlyGroup());
			break;
		case IDM_FILTERDEF:
			SetDefFilter(0);
			SendMessage(hWnd,DM_HREBUILD,0,0);
			break;
		case IDM_FILTERALL:
			SetDefFilter(1);
			SendMessage(hWnd,DM_HREBUILD,0,0);
			break;
		default:
			if(selected >= DEF_FILTERS_START)
			{
				SetDefFilter(selected - DEF_FILTERS_START + 2);
				SendMessage(hWnd,DM_HREBUILD,0,0);
			}
			break;
		}
		
		DestroyMenu(hFilterMenu);
		DestroyMenu(hPopupMenu);
	}
}

void HistoryWindow::ConfigToolbarClicked(LPNMTOOLBAR lpnmTB)
{
	RECT rc;
	SendMessage(lpnmTB->hdr.hwndFrom, TB_GETRECT, (WPARAM)lpnmTB->iItem, (LPARAM)&rc); 
	MapWindowPoints(lpnmTB->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rc, 2);         
	HMENU hPopupMenu = CreatePopupMenu();
	if(hPopupMenu != NULL)
	{
		AppendMenu(hPopupMenu, MF_STRING, IDM_OPTIONS, TranslateT("Options"));
		AppendMenu(hPopupMenu, MF_STRING, IDM_FONTS, TranslateT("Fonts & Colors"));
		AppendMenu(hPopupMenu, MF_STRING, IDM_ICONS, TranslateT("Icons"));
		AppendMenu(hPopupMenu, MF_STRING, IDM_HOTKEYS, TranslateT("Hotkeys"));
		AppendMenu(hPopupMenu, MFT_SEPARATOR, 0, NULL);
		
		HMENU hExportMenu = CreatePopupMenu();
		AppendMenu(hExportMenu, MF_STRING, IDM_EXPORTRHTML, TranslateT("Rich Html"));
		AppendMenu(hExportMenu, MF_STRING, IDM_EXPORTPHTML, TranslateT("Plain Html"));
		AppendMenu(hExportMenu, MF_STRING, IDM_EXPORTTXT, TranslateT("Txt"));
		AppendMenu(hPopupMenu, MF_STRING | MF_POPUP, (UINT_PTR)hExportMenu, TranslateT("Export"));

		AppendMenu(hPopupMenu, MFT_SEPARATOR, 0, NULL);
		AppendMenu(hPopupMenu, MF_STRING, IDM_SAVEPOS, TranslateT("Save window position as default"));
		AppendMenu(hPopupMenu, MF_STRING, IDM_SAVEPOSALL, TranslateT("Save window position for all contacts"));
		SetMenuDefaultItem(hPopupMenu, IDM_OPTIONS, FALSE);

		int selected = TrackPopupMenu(hPopupMenu, TPM_RETURNCMD, rc.left, rc.bottom, 0, hWnd, 0);
		switch (selected)
		{
		case IDM_OPTIONS:
			SendMessage(hWnd, WM_COMMAND, IDM_CONFIG, 0);
			break;
		case IDM_FONTS:
			OpenOptions("Customize", "Fonts & Colors");
			break;
		case IDM_ICONS:
			OpenOptions("Customize", "Icons");
			break;
		case IDM_HOTKEYS:
			OpenOptions("Customize", "Hotkeys");
			break;
		case IDM_SAVEPOS:
			SavePos(false);
			break;
		case IDM_SAVEPOSALL:
			SavePos(true);
			break;
		case IDM_EXPORTRHTML:
			{
				ExportManeger exp(hContact, GetFilterNr());
				exp.Export(IExport::RichHtml);
			}

			break;
		case IDM_EXPORTPHTML:
			{
				ExportManeger exp(hContact, GetFilterNr());
				exp.Export(IExport::PlainHtml);
			}

			break;
		case IDM_EXPORTTXT:
			{
				ExportManeger exp(hContact, GetFilterNr());
				exp.Export(IExport::Txt);
			}

			break;
		}

		DestroyMenu(hPopupMenu);
	}
}

void HistoryWindow::DeleteToolbarClicked(LPNMTOOLBAR lpnmTB)
{
	RECT rc;
	SendMessage(lpnmTB->hdr.hwndFrom, TB_GETRECT, (WPARAM)lpnmTB->iItem, (LPARAM)&rc); 
	MapWindowPoints(lpnmTB->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rc, 2);         
	HMENU hPopupMenu = CreatePopupMenu();
	if(hPopupMenu != NULL)
	{
		AppendMenu(hPopupMenu, MF_STRING, IDM_DELETE, TranslateT("Delete"));
		AppendMenu(hPopupMenu, MF_STRING, IDM_DELETEGROUP, TranslateT("Delete Group"));
		AppendMenu(hPopupMenu, MF_STRING, IDM_DELETEUSER, TranslateT("Delete All User History"));
		SetMenuDefaultItem(hPopupMenu, IDM_DELETE, FALSE);

		int selected = TrackPopupMenu(hPopupMenu, TPM_RETURNCMD, rc.left, rc.bottom, 0, hWnd, 0);
		switch (selected)
		{
		case IDM_DELETE:
			Delete(0);
			break;
		case IDM_DELETEGROUP:
			Delete(1);
			break;
		case IDM_DELETEUSER:
			Delete(2);
			break;
		}

		DestroyMenu(hPopupMenu);
	}
}

void HistoryWindow::Delete(int what)
{
	int toDelete = 1;
	int start = 0;
	int end = 0;
	if(selected < 0 || selected >= eventList.size() || what > 2 || what < 0)
		return;
	if(what == 0)
	{
		CHARRANGE chrg;
		SendMessage(editWindow,EM_EXGETSEL,0,(LPARAM)&chrg);
		if(chrg.cpMin == 0 && chrg.cpMax == -1)
		{
			toDelete = currentGroup.size();
		}
		else
		{
			while(start < currentGroup.size() && chrg.cpMin >= currentGroup[start].endPos) ++start;
			end = start;
			while(end < currentGroup.size() && chrg.cpMax > currentGroup[end].endPos) ++end;
			if(start >= currentGroup.size())
				return;
			if(end < currentGroup.size())
				++end;
			toDelete = end - start;
		}
	}
	else if(what == 1)
	{
		end = toDelete = currentGroup.size();
	}
	else
	{
		if(eventList.size() == 0)
			return;
		toDelete = eventList[0].size();
	}

	if(toDelete == 0)
		return;
	TCHAR message[256];
	if(what == 2)
		_tcscpy_s(message, TranslateT("This operation will PERMANENTLY REMOVE all history for this contact.\nAre you sure you want to do this?"));
	else
		_stprintf_s(message, TranslateT("Number of history items to delete: %d.\nAre you sure you want to do this?"), toDelete);
	if(MessageBox(hWnd, message, TranslateT("Are You sure?"), MB_OKCANCEL | MB_ICONERROR) != IDOK)
		return;

	if(what == 2)
	{
		for(int j = 0; j < eventList.size(); ++j)
		{
			for(int i = 0; i < eventList[j].size(); ++i)
			{
				CallService(MS_DB_EVENT_DELETE,(WPARAM)hContact,(LPARAM)(HANDLE)eventList[j][i]);
			}
		}
		
		SendMessage(hWnd,DM_HREBUILD,0,0);
	}
	else
	{
		for(int i = start; i < end; ++i)
		{
			CallService(MS_DB_EVENT_DELETE,(WPARAM)hContact,(LPARAM)(HANDLE)eventList[selected][i]);
		}

		if(start == 0 && end == currentGroup.size())
		{
			SendMessage(hWnd,DM_HREBUILD,0,0);
		}
		else
		{
			std::deque<HANDLE> newGroup;
			for(int i = 0; i < eventList[selected].size(); ++i)
			{
				if(CallService(MS_DB_EVENT_GETBLOBSIZE,(WPARAM)(HANDLE)eventList[selected][i],0) >= 0)
				{
					// If event exist, we add it to new group
					newGroup.push_back(eventList[selected][i]);
				}
			}
			eventList[selected].clear();
			eventList[selected].insert(eventList[selected].begin(), newGroup.begin(), newGroup.end());
			SelectEventGroup(selected);
		}
	}
}

bool HistoryWindow::ContactChanged(bool sync)
{
	if(!isLoading)
	{
		HANDLE hItem = (HANDLE)SendDlgItemMessage(hWnd, IDC_LIST_CONTACTS, CLM_GETSELECTION, 0, 0);
		if(hItem != NULL)
		{
			int typeOf = SendDlgItemMessage(hWnd, IDC_LIST_CONTACTS, CLM_GETITEMTYPE,(WPARAM)hItem,0);
			if(typeOf == CLCIT_CONTACT)
			{
				if(hContact != hItem)
				{
					ChangeToFreeWindow(this);
					isLoading = true;
					hContact = hItem;
					ReloadContacts();
					if(sync)
						FillHistoryThread(this);
					else
						mir_forkthread(HistoryWindow::FillHistoryThread, this);
					return true;
				}
			}
		}
	}

	return false;
}

void HistoryWindow::GroupImagesChanged()
{
	if(isGroupImages != Options::instance->groupShowEvents)
	{
		isGroupImages = Options::instance->groupShowEvents;
		if(isGroupImages)
		{
			ListView_SetImageList(listWindow, himlSmall, LVSIL_SMALL);
		}
		else
		{
			ListView_SetImageList(listWindow, himlNone, LVSIL_SMALL);
		}
	}
}

void HistoryWindow::FormatQuote(std::wstring& quote, const MessageData& md, const std::wstring& msg)
{
	if(md.isMe)
		quote += myName;
	else
		quote += contactName;
	TCHAR str[32];
	tmi.printTimeStamp(NULL, md.timestamp, _T("d t"), str , 32, 0);
	quote += _T(", ");
	quote += str;
	quote += _T("\n");
	int f = 0;
	do
	{
		int nf = msg.find_first_of(_T("\r\n"), f);
		if(nf >= 0 && nf < msg.length())
		{
			if(nf - f >= 0 )
			{
				quote += _T(">");
				quote += msg.substr(f, nf - f);
				quote += _T("\n");
			}

			f = nf + 1;
			if(msg[nf] == _T('\r') && f < msg.length() && msg[f] == _T('\n'))
				++f;
		}
		else if(msg.length() - f > 0)
		{
			quote += _T(">");
			quote += msg.substr(f, msg.length() - f);
			quote += _T("\n");
			f = -1;
		}
		else
			f = -1;
	}
	while(f > 0 && f < msg.length());
}
