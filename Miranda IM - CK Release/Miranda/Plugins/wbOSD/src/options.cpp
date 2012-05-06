/*
Wannabe OSD plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2005 Andrej Krutak

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

#include "wbOSD.h"

COLORREF pencustcolors[16];

const static osdmsg defstr={_T(""), 0, RGB(0, 0, 0), 0, 0};

void FillCheckBoxTree(HWND hwndTree,const struct CheckBoxValues_t *values,int nValues,DWORD style)
{
	logmsg("FillCheckBoxTree");

	TVINSERTSTRUCT tvis;
	tvis.hParent = NULL;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_PARAM|TVIF_TEXT|TVIF_STATE;
	for ( int i=0; i < nValues; i++ ) {
		tvis.item.lParam = values[i].style - ID_STATUS_OFFLINE;
		tvis.item.pszText = TranslateTS( values[i].szDescr );
		tvis.item.stateMask = TVIS_STATEIMAGEMASK;
		tvis.item.state = INDEXTOSTATEIMAGEMASK(( style & ( 1 << tvis.item.lParam )) != 0 ? 2 : 1 );
		TreeView_InsertItem( hwndTree, &tvis );
	}
}

DWORD MakeCheckBoxTreeFlags(HWND hwndTree)
{
	DWORD flags=0;
	TVITEM tvi;

	logmsg("MakeCheckBoxTreeFlags");

	tvi.mask=TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
	tvi.hItem=TreeView_GetRoot(hwndTree);
	while(tvi.hItem) {
		TreeView_GetItem(hwndTree,&tvi);
		if(((tvi.state&TVIS_STATEIMAGEMASK)>>12==2)) flags|=1<<tvi.lParam;
		tvi.hItem=TreeView_GetNextSibling(hwndTree,tvi.hItem);
	}
	return flags;
}

int selectColor(HWND hwnd, COLORREF *clr)
{
	CHOOSECOLOR cc;

	logmsg("SelectColor");

	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hwnd;
	cc.hInstance = (HWND)hI;
	cc.rgbResult = *clr;
	cc.lpCustColors = pencustcolors;
	cc.Flags = CC_FULLOPEN|CC_RGBINIT;
	if (!ChooseColor(&cc))
		return 1;
	
	*clr=cc.rgbResult;
	return 0;
}

int selectFont(HWND hDlg, LOGFONTA *lf)
{
	CHOOSEFONTA cf;
	HDC hDC;
	COLORREF color=RGB(0, 0, 0);

	logmsg("SelectFont");

	ZeroMemory(&cf, sizeof(CHOOSEFONT));
	hDC = GetDC(hDlg);
	cf.lStructSize = sizeof(cf);
	cf.hwndOwner = hDlg;
	cf.hDC = hDC;
	cf.lpLogFont = lf;
	cf.rgbColors = 0;
	cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_EFFECTS | CF_BOTH | CF_FORCEFONTEXIST;
	cf.nFontType = 0;
	cf.rgbColors=color;
	
	if (!ChooseFontA(&cf)) {
		if (cf.hDC)
			DeleteDC(cf.hDC);
		
		ReleaseDC(hDlg, hDC);
		return 1;
	}
	
	if (cf.hDC)
		DeleteDC(cf.hDC);
	
	ReleaseDC(hDlg, hDC);
	return 0;
}

void loadDBSettings(plgsettings *ps)
{
	DBVARIANT dbv;

	logmsg("loadDBSettings");

	ps->align=DBGetContactSettingByte(NULL,THIS_MODULE, "align", DEFAULT_ALIGN);
	ps->salign=DBGetContactSettingByte(NULL,THIS_MODULE, "salign", DEFAULT_SALIGN);
	ps->altShadow=DBGetContactSettingByte(NULL,THIS_MODULE, "altShadow", DEFAULT_ALTSHADOW);
	ps->transparent=DBGetContactSettingByte(NULL,THIS_MODULE, "transparent", DEFAULT_TRANPARENT);
	ps->showShadow=DBGetContactSettingByte(NULL,THIS_MODULE, "showShadow", DEFAULT_SHOWSHADOW);
	ps->messages=DBGetContactSettingByte(NULL,THIS_MODULE, "messages", DEFAULT_ANNOUNCEMESSAGES);
	ps->a_user=DBGetContactSettingByte(NULL,THIS_MODULE, "a_user", DEFAULT_ANNOUNCESTATUS);
	ps->distance=DBGetContactSettingByte(NULL,THIS_MODULE, "distance", DEFAULT_DISTANCE);
	ps->winx=DBGetContactSettingDword(NULL,THIS_MODULE, "winx", DEFAULT_WINX);
	ps->winy=DBGetContactSettingDword(NULL,THIS_MODULE, "winy", DEFAULT_WINY);
	ps->winxpos=DBGetContactSettingDword(NULL,THIS_MODULE, "winxpos", DEFAULT_WINXPOS);
	ps->winypos=DBGetContactSettingDword(NULL,THIS_MODULE, "winypos", DEFAULT_WINYPOS);
	ps->alpha=DBGetContactSettingByte(NULL,THIS_MODULE, "alpha", DEFAULT_ALPHA);
	ps->showmystatus=DBGetContactSettingByte(NULL,THIS_MODULE, "showMyStatus", DEFAULT_SHOWMYSTATUS);
	ps->timeout=DBGetContactSettingDword(NULL,THIS_MODULE, "timeout", DEFAULT_TIMEOUT);
	ps->clr_msg=DBGetContactSettingDword(NULL,THIS_MODULE, "clr_msg", DEFAULT_CLRMSG);
	ps->clr_status=DBGetContactSettingDword(NULL,THIS_MODULE, "clr_status", DEFAULT_CLRSTATUS);
	ps->clr_shadow=DBGetContactSettingDword(NULL,THIS_MODULE, "clr_shadow", DEFAULT_CLRSHADOW);
	ps->bkclr=DBGetContactSettingDword(NULL,THIS_MODULE, "bkclr", DEFAULT_BKCLR);

	ps->showMsgWindow=DBGetContactSettingByte(NULL,THIS_MODULE, "showMessageWindow", DEFAULT_SHOWMSGWIN);
	ps->showWhen=DBGetContactSettingDword(NULL,THIS_MODULE,"showWhen", DEFAULT_SHOWWHEN);

	_tcscpy(ps->msgformat, DEFAULT_MESSAGEFORMAT);
	if ( !DBGetContactSettingTString( NULL, THIS_MODULE, "message_format", &dbv )) {
		_tcscpy(ps->msgformat, dbv.ptszVal);
		DBFreeVariant(&dbv);
	}

	ps->announce=DBGetContactSettingDword(NULL,THIS_MODULE,"announce", DEFAULT_ANNOUNCE);

	ps->lf.lfHeight=DBGetContactSettingDword(NULL,THIS_MODULE, "fntHeight", DEFAULT_FNT_HEIGHT);
	ps->lf.lfWidth=DBGetContactSettingDword(NULL,THIS_MODULE, "fntWidth", DEFAULT_FNT_WIDTH);
	ps->lf.lfEscapement=DBGetContactSettingDword(NULL,THIS_MODULE, "fntEscapement", DEFAULT_FNT_ESCAPEMENT);
	ps->lf.lfOrientation=DBGetContactSettingDword(NULL,THIS_MODULE, "fntOrientation", DEFAULT_FNT_ORIENTATION);
	ps->lf.lfWeight=DBGetContactSettingDword(NULL,THIS_MODULE, "fntWeight", DEFAULT_FNT_WEIGHT);
	ps->lf.lfItalic=DBGetContactSettingByte(NULL,THIS_MODULE, "fntItalic", DEFAULT_FNT_ITALIC);
	ps->lf.lfUnderline=DBGetContactSettingByte(NULL,THIS_MODULE, "fntUnderline", DEFAULT_FNT_UNDERLINE);
	ps->lf.lfStrikeOut=DBGetContactSettingByte(NULL,THIS_MODULE, "fntStrikeout", DEFAULT_FNT_STRIKEOUT);
	ps->lf.lfCharSet=DBGetContactSettingByte(NULL,THIS_MODULE, "fntCharSet", DEFAULT_FNT_CHARSET);
	ps->lf.lfOutPrecision=DBGetContactSettingByte(NULL,THIS_MODULE, "fntOutPrecision", DEFAULT_FNT_OUTPRECISION);
	ps->lf.lfClipPrecision=DBGetContactSettingByte(NULL,THIS_MODULE, "fntClipPrecision", DEFAULT_FNT_CLIPRECISION);
	ps->lf.lfQuality=DBGetContactSettingByte(NULL,THIS_MODULE, "fntQuality", DEFAULT_FNT_QUALITY);
	ps->lf.lfPitchAndFamily=DBGetContactSettingByte(NULL,THIS_MODULE, "fntPitchAndFamily", DEFAULT_FNT_PITCHANDFAM);
	strcpy(ps->lf.lfFaceName, DEFAULT_FNT_FACENAME);
	if(!DBGetContactSetting(NULL,THIS_MODULE,"fntFaceName",&dbv)) {
		strcpy(ps->lf.lfFaceName, dbv.pszVal);
		DBFreeVariant(&dbv);
	}
}

void saveDBSettings(plgsettings *ps)
{
	logmsg("saveDBSettings");

	DBWriteContactSettingByte(NULL,THIS_MODULE,"showShadow", ps->showShadow);
	DBWriteContactSettingByte(NULL,THIS_MODULE,"altShadow",ps->altShadow);
	DBWriteContactSettingByte(NULL,THIS_MODULE,"distance",ps->distance);
	
	DBWriteContactSettingDword(NULL,THIS_MODULE,"winx",ps->winx);
	DBWriteContactSettingDword(NULL,THIS_MODULE,"winy",ps->winy);
	DBWriteContactSettingDword(NULL,THIS_MODULE,"winxpos", ps->winxpos);
	DBWriteContactSettingDword(NULL,THIS_MODULE,"winypos", ps->winypos);
	
	DBWriteContactSettingByte(NULL,THIS_MODULE,"alpha",ps->alpha);
	DBWriteContactSettingDword(NULL,THIS_MODULE,"timeout", ps->timeout);

	DBWriteContactSettingByte(NULL,THIS_MODULE,"transparent",ps->transparent); 
	DBWriteContactSettingByte(NULL,THIS_MODULE,"messages",ps->messages); 
	DBWriteContactSettingByte(NULL,THIS_MODULE,"a_user",ps->a_user); 
	DBWriteContactSettingTString(NULL,THIS_MODULE, "message_format", ps->msgformat);

	DBWriteContactSettingByte(NULL,THIS_MODULE,"align",ps->align); 
	DBWriteContactSettingByte(NULL,THIS_MODULE,"salign",ps->salign); 

	DBWriteContactSettingByte(NULL,THIS_MODULE,"showMyStatus",ps->showmystatus); 

	DBWriteContactSettingDword(NULL,THIS_MODULE,"clr_msg", ps->clr_msg); 
	DBWriteContactSettingDword(NULL,THIS_MODULE,"clr_shadow", ps->clr_shadow); 
	DBWriteContactSettingDword(NULL,THIS_MODULE,"clr_status", ps->clr_status); 
	DBWriteContactSettingDword(NULL,THIS_MODULE,"bkclr", ps->bkclr); 

	DBWriteContactSettingDword(NULL,THIS_MODULE, "fntHeight", ps->lf.lfHeight);
	DBWriteContactSettingDword(NULL,THIS_MODULE, "fntWidth", ps->lf.lfWidth);
	DBWriteContactSettingDword(NULL,THIS_MODULE, "fntEscapement", ps->lf.lfEscapement);
	DBWriteContactSettingDword(NULL,THIS_MODULE, "fntOrientation", ps->lf.lfOrientation);
	DBWriteContactSettingDword(NULL,THIS_MODULE, "fntWeight", ps->lf.lfWeight);
	DBWriteContactSettingByte(NULL,THIS_MODULE, "fntItalic", ps->lf.lfItalic);
	DBWriteContactSettingByte(NULL,THIS_MODULE, "fntUnderline", ps->lf.lfUnderline);
	DBWriteContactSettingByte(NULL,THIS_MODULE, "fntStrikeout", ps->lf.lfStrikeOut);
	DBWriteContactSettingByte(NULL,THIS_MODULE, "fntCharSet", ps->lf.lfCharSet);
	DBWriteContactSettingByte(NULL,THIS_MODULE, "fntOutPrecision", ps->lf.lfOutPrecision);
	DBWriteContactSettingByte(NULL,THIS_MODULE, "fntClipPrecision", ps->lf.lfClipPrecision);
	DBWriteContactSettingByte(NULL,THIS_MODULE, "fntQuality", ps->lf.lfQuality);
	DBWriteContactSettingByte(NULL,THIS_MODULE, "fntPitchAndFamily", ps->lf.lfPitchAndFamily);
	DBWriteContactSettingString(NULL,THIS_MODULE, "fntFaceName", ps->lf.lfFaceName);
	
	DBWriteContactSettingDword(NULL,THIS_MODULE,"announce", ps->announce);

	DBWriteContactSettingByte(NULL,THIS_MODULE, "showMessageWindow", ps->showMsgWindow);
	DBWriteContactSettingDword(NULL,THIS_MODULE,"showWhen", ps->showWhen);
}

BOOL CALLBACK OptDlgProc(HWND hDlg,UINT msg,WPARAM wparam,LPARAM lparam)
{
	RECT rect;
	plgsettings *ps; //0: current; 1: original

	logmsg("OptDlgProc");

	switch(msg){
		case WM_INITDIALOG:
			logmsg("OptDlgProc::INITDIALOG");
			TranslateDialogDefault(hDlg);

			ps=(plgsettings*)malloc(sizeof(plgsettings)*2);
			loadDBSettings(&ps[0]);
			ps[1]=ps[0];
			SetWindowLong(hDlg, GWL_USERDATA, (LONG)ps);
			SetWindowLong(hwnd, GWL_STYLE, (LONG)(pSetLayeredWindowAttributes?0:WS_CLIPSIBLINGS)|WS_POPUP|WS_SIZEBOX);
			SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_FRAMECHANGED);

			SetWindowLong(GetDlgItem(hDlg,IDC_TREE1),GWL_STYLE,GetWindowLong(GetDlgItem(hDlg,IDC_TREE1),GWL_STYLE)|TVS_NOHSCROLL|TVS_CHECKBOXES);
			SetWindowLong(GetDlgItem(hDlg,IDC_TREE2),GWL_STYLE,GetWindowLong(GetDlgItem(hDlg,IDC_TREE1),GWL_STYLE)|TVS_NOHSCROLL|TVS_CHECKBOXES);

			CheckDlgButton(hDlg, IDC_RADIO1+ps->align-1, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_RADIO10+9-ps->salign, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_CHECK1, ps->altShadow);
			CheckDlgButton(hDlg, IDC_CHECK2, ps->showMsgWindow);
			CheckDlgButton(hDlg, IDC_CHECK3, ps->transparent);
			CheckDlgButton(hDlg, IDC_CHECK4, ps->showShadow);
			CheckDlgButton(hDlg, IDC_CHECK5, ps->messages);
			
			SetDlgItemText(hDlg, IDC_EDIT2, ps->msgformat);

			CheckDlgButton(hDlg, IDC_CHECK6, ps->a_user);
			CheckDlgButton(hDlg, IDC_CHECK7, ps->showmystatus);
			SetDlgItemInt(hDlg, IDC_EDIT1, ps->distance, 0);

			SendDlgItemMessage(hDlg, IDC_SLIDER1, TBM_SETRANGE, (WPARAM)0, MAKELONG(0, 255));
			SendDlgItemMessage(hDlg, IDC_SLIDER1, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)ps->alpha);

			{
				TCHAR buf[20];
				mir_sntprintf(buf, SIZEOF(buf), _T("%d %%"), ps->alpha*100/255);
				SetDlgItemText(hDlg, IDC_ALPHATXT, buf);
			}

			SetDlgItemInt(hDlg, IDC_EDIT5, ps->timeout, 0);
			FillCheckBoxTree(GetDlgItem(hDlg, IDC_TREE1), statusValues, SIZEOF(statusValues), ps->announce);
			FillCheckBoxTree(GetDlgItem(hDlg, IDC_TREE2), statusValues, SIZEOF(statusValues), ps->showWhen);
			return 0;

		case WM_HSCROLL:
			if (LOWORD(wparam)==SB_ENDSCROLL||LOWORD(wparam)==SB_THUMBPOSITION||LOWORD(wparam)==SB_ENDSCROLL)
				return 0;
			ps=(plgsettings*)GetWindowLong(hDlg, GWL_USERDATA);
			ps->alpha=SendDlgItemMessage(hDlg, IDC_SLIDER1, TBM_GETPOS, 0, 0);
			{
				TCHAR buf[20];
				mir_sntprintf(buf, SIZEOF(buf), _T("%d %%"), ps->alpha*100/255);
				SetDlgItemText(hDlg, IDC_ALPHATXT, buf);
			}
			goto xxx;
		case WM_DESTROY:
			logmsg("OptDlgProc::DESTROY");
			ps=(plgsettings*)GetWindowLong(hDlg, GWL_USERDATA);
			ps[0]=ps[1];
			saveDBSettings(&ps[0]);

			SetWindowLong(hwnd, GWL_STYLE, (LONG)(pSetLayeredWindowAttributes?0:WS_CLIPSIBLINGS)|WS_POPUP);
			SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_FRAMECHANGED);

			SetWindowPos(hwnd, 0, ps->winxpos, ps->winypos, ps->winx, ps->winy, SWP_NOZORDER|SWP_NOACTIVATE);
			if (pSetLayeredWindowAttributes)
				pSetLayeredWindowAttributes(hwnd, ps->bkclr, ps->alpha, (ps->transparent?LWA_COLORKEY:0)|LWA_ALPHA);

			free((void*)GetWindowLong(hDlg, GWL_USERDATA));
			return 0;
		case WM_COMMAND:
			logmsg("OptDlgProc::COMMAND");
			ps=(plgsettings*)GetWindowLong(hDlg, GWL_USERDATA);
			switch (LOWORD(wparam)) {
			case IDC_BUTTON7:
				MessageBox(hDlg, _T("Variables:\n  %n : Nick\n  %m : Message\n  %l : New line"), _T("Help"), MB_ICONINFORMATION|MB_OK);
				return 0;
			case IDC_BUTTON5:
				SendMessage(hwnd, WM_USER+1, (WPARAM)_T("miranda is gr8 and this is a long message ;-)"), 0);
				break;
			case IDC_BUTTON1:
				selectFont(hDlg, &(ps->lf));
				break;
			case IDC_BUTTON2:
				selectColor(hDlg, &ps->clr_status);
				break;
			case IDC_BUTTON6:
				selectColor(hDlg, &ps->clr_msg);
				break;
			case IDC_BUTTON3:
				selectColor(hDlg, &ps->clr_shadow);
				break;
			case IDC_BUTTON4:
				selectColor(hDlg, &ps->bkclr);
				break;
			case IDC_CHECK4:
				ps->showShadow=IsDlgButtonChecked(hDlg, IDC_CHECK4);
				break;
			case IDC_CHECK1:
				ps->altShadow=IsDlgButtonChecked(hDlg, IDC_CHECK1);
				break;
			case IDC_CHECK2:
				ps->showMsgWindow=IsDlgButtonChecked(hDlg, IDC_CHECK2);
			case IDC_EDIT1:
				ps->distance=GetDlgItemInt(hDlg, IDC_EDIT1, 0, 0);
				break;
			case IDC_EDIT5:
				ps->timeout=GetDlgItemInt(hDlg, IDC_EDIT5, 0, 0);
				break;
			case IDC_CHECK3:
				ps->transparent=IsDlgButtonChecked(hDlg, IDC_CHECK3);
				break;
			case IDC_CHECK5:
				ps->messages=IsDlgButtonChecked(hDlg, IDC_CHECK5);
				break;
			case IDC_CHECK6:
				ps->a_user=IsDlgButtonChecked(hDlg, IDC_CHECK6);
				break;
			case IDC_CHECK7:
				ps->showmystatus=IsDlgButtonChecked(hDlg, IDC_CHECK7);
				break;
			case IDC_RADIO1:
			case IDC_RADIO2:
			case IDC_RADIO3:
			case IDC_RADIO4:
			case IDC_RADIO5:
			case IDC_RADIO6:
			case IDC_RADIO7:
			case IDC_RADIO8:
			case IDC_RADIO9:
				if (IsDlgButtonChecked(hDlg, LOWORD(wparam)))
					ps->align=LOWORD(wparam)-IDC_RADIO1+1;
				break;
			case IDC_RADIO10:
			case IDC_RADIO11:
			case IDC_RADIO12:
			case IDC_RADIO13:
			case IDC_RADIO14:
			case IDC_RADIO15:
			case IDC_RADIO16:
			case IDC_RADIO17:
			case IDC_RADIO18:
				if (IsDlgButtonChecked(hDlg, LOWORD(wparam)))
					ps->salign=10-(LOWORD(wparam)-IDC_RADIO10+1);
				break;
			}
xxx:
			saveDBSettings(ps);
			SetWindowPos(hwnd, 0, 0, 0, ps->winx, ps->winy, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
			if (pSetLayeredWindowAttributes)
				pSetLayeredWindowAttributes(hwnd, DBGetContactSettingDword(NULL,THIS_MODULE, "bkclr", DEFAULT_BKCLR), DBGetContactSettingByte(NULL,THIS_MODULE, "alpha", DEFAULT_ALPHA), (DBGetContactSettingByte(NULL,THIS_MODULE, "transparent", DEFAULT_TRANPARENT)?LWA_COLORKEY:0)|LWA_ALPHA);
			InvalidateRect(hwnd, 0, TRUE);
			SendMessage(GetParent(hDlg),PSM_CHANGED,0,0);

			return 0;

		case WM_NOTIFY:
			logmsg("OptDlgProc::NOTIFY");
			switch(((LPNMHDR)lparam)->code){
				case TVN_SETDISPINFO:
				case NM_CLICK:
				case NM_RETURN:
				case TVN_SELCHANGED:
					if (((LPNMHDR)lparam)->idFrom==IDC_TREE1)
						SendMessage(GetParent(hDlg),PSM_CHANGED,0,0);
					break;
				case PSN_APPLY:
					ps=(plgsettings*)GetWindowLong(hDlg, GWL_USERDATA);
					
					GetWindowRect(hwnd, &rect);
					ps->winx=rect.right-rect.left;
					ps->winy=rect.bottom-rect.top;
					ps->winxpos=rect.left;
					ps->winypos=rect.top;
					ps->announce=MakeCheckBoxTreeFlags(GetDlgItem(hDlg, IDC_TREE1));
					ps->showWhen=MakeCheckBoxTreeFlags(GetDlgItem(hDlg, IDC_TREE2));
					GetDlgItemText(hDlg, IDC_EDIT2, ps->msgformat, 255);
					ps[1]=ps[0]; //apply current settings at closing

					saveDBSettings(ps);
					if (pSetLayeredWindowAttributes)
						pSetLayeredWindowAttributes(hwnd, DBGetContactSettingDword(NULL,THIS_MODULE, "bkclr", DEFAULT_BKCLR), DBGetContactSettingByte(NULL,THIS_MODULE, "alpha", DEFAULT_ALPHA), (DBGetContactSettingByte(NULL,THIS_MODULE, "transparent", DEFAULT_TRANPARENT)?LWA_COLORKEY:0)|LWA_ALPHA);
					InvalidateRect(hwnd, 0, TRUE);
					break;
			}
			break;
	}

	return 0;
}

int OptionsInit(WPARAM wparam,LPARAM lparam)
{
	OPTIONSDIALOGPAGE odp;

	logmsg("OptionsInit");
	ZeroMemory(&odp,sizeof(odp));
	odp.cbSize=sizeof(odp);
	odp.position=150000000;
	odp.pszGroup="Plugins";
	odp.groupPosition=950000000;
	odp.hInstance=hI;
	odp.pszTemplate=MAKEINTRESOURCEA(IDD_DIALOG1);
	odp.pszTitle="OSD";
	odp.pfnDlgProc=(DLGPROC)OptDlgProc;
	odp.flags=ODPF_BOLDGROUPS;
	CallService(MS_OPT_ADDPAGE,wparam,(LPARAM)&odp);
	return 0;
}
