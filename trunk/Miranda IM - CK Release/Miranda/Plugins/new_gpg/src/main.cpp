/*
New_gpg plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author 
			Copyright (C) 2010-2011 sss

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

#include "commonheaders.h"


HWND hwndFirstRun = NULL, hwndSetDirs = NULL, hwndNewKey = NULL, hwndKeyGen = NULL, hwndSelectExistingKey = NULL;

int itemnum = 0;

HWND hwndList_g = NULL;
BOOL CheckStateStoreDB(HWND hwndDlg, int idCtrl, const char* szSetting);

static BOOL CALLBACK DlgProcFirstRun(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
	HWND hwndList=GetDlgItem(hwndDlg, IDC_KEY_LIST);
	hwndList_g = hwndList;
	LVCOLUMN col = {0};
	LVITEM item = {0};
	NMLISTVIEW * hdr = (NMLISTVIEW *) lParam;
	TCHAR fp[16] = {0};
  switch (msg)
  {
  case WM_INITDIALOG:
    {
		SetWindowPos(hwndDlg, 0, firstrun_rect.left, firstrun_rect.top, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
		TranslateDialogDefault(hwndDlg);
		SetWindowText(hwndDlg, TranslateW(_T("Set own key")));
		col.pszText = _T("Key ID");
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.cx = 50;
		ListView_InsertColumn(hwndList, 0, &col);
		ZeroMemory(&col,sizeof(col));
		col.pszText = TranslateW(_T("Email"));
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.cx = 30;
		ListView_InsertColumn(hwndList, 1, &col);
		ZeroMemory(&col,sizeof(col));
		col.pszText = TranslateW(_T("Name"));
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.cx = 250;
		ListView_InsertColumn(hwndList, 2, &col);
		ZeroMemory(&col,sizeof(col));
		col.pszText = TranslateW(_T("Creation date"));
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.cx = 30;
		ListView_InsertColumn(hwndList, 3, &col);
		ZeroMemory(&col,sizeof(col));
		col.pszText = TranslateW(_T("Key length"));
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.cx = 30;
		ListView_InsertColumn(hwndList, 4, &col);
		ListView_SetExtendedListViewStyleEx(hwndList, 0, LVS_EX_FULLROWSELECT);
		int i = 1, iRow = 0;
		{ 
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.iSubItem = 0;
			item.pszText = _T("");
			{//parse gpg output
				string out;
				DWORD code;
				pxResult result;
				wstring::size_type p = 0, p2 = 0, stop = 0;
				{
					gpg_execution_params params;
					wstring cmd = _T("--batch --list-secret-keys");
					params.cmd = &cmd;
					params.useless = "";
					params.out = &out;
					params.code = &code;
					params.result = &result;
					boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
					if(!gpg_thread.timed_join(boost::posix_time::seconds(10)))
					{
						gpg_thread.~thread();
						TerminateProcess(params.hProcess, 1);
						params.hProcess = NULL;
						debuglog<<time_str()<<": GPG execution timed out, aborted\n";
						break;
					}
					if(result == pxNotFound)
						break;
				}
				while(p != string::npos)
				{
					if((p = out.find("sec  ", p)) == string::npos)
						break;
					p += 5;
					if(p < stop)
						break;
					stop = p;
					p2 = out.find("/", p) - 1;
					TCHAR *tmp = mir_wstrdup(toUTF16(out.substr(p,p2-p)).c_str());
					item.pszText = tmp;
					iRow = ListView_InsertItem(hwndList, &item);
					ListView_SetItemText(hwndList, iRow, 4, tmp);
					mir_free(tmp);
					p2+=2;
					p = out.find(" ", p2);
					tmp = mir_wstrdup(toUTF16(out.substr(p2,p-p2)).c_str());
					ListView_SetItemText(hwndList, iRow, 0, tmp);
					mir_free(tmp);
					p = out.find("uid  ", p);
					p2 = out.find_first_not_of(" ", p+5);
					p = out.find("<", p2);
					tmp = mir_wstrdup(toUTF16(out.substr(p2,p-p2)).c_str());
					ListView_SetItemText(hwndList, iRow, 2, tmp);
					mir_free(tmp);
					p++;
					p2 = out.find(">", p);
					tmp = mir_wstrdup(toUTF16(out.substr(p,p2-p)).c_str());
					ListView_SetItemText(hwndList, iRow, 1, tmp);
					mir_free(tmp);
					p = out.find("ssb  ", p2) + 6;
					p = out.find(" ", p) + 1;
					p2 = out.find("\n", p);
					tmp = mir_wstrdup(toUTF16(out.substr(p,p2-p-1)).c_str());
					ListView_SetItemText(hwndList, iRow, 3, tmp);
					mir_free(tmp);
					ListView_SetColumnWidth(hwndList, 0, LVSCW_AUTOSIZE);// not sure about this
					ListView_SetColumnWidth(hwndList, 1, LVSCW_AUTOSIZE);
					ListView_SetColumnWidth(hwndList, 2, LVSCW_AUTOSIZE);
					ListView_SetColumnWidth(hwndList, 3, LVSCW_AUTOSIZE);
					ListView_SetColumnWidth(hwndList, 4, LVSCW_AUTOSIZE);
					i++;
				}
			}
		}

		return TRUE;
    }
    
 
  case WM_COMMAND:
    {
      switch (LOWORD(wParam))
      {
	  case IDC_GENERATE_KEY:
		  void ShowKeyGenDialog();
		  ShowKeyGenDialog();
		  break;
	  case ID_OK:
		  {
			  ListView_GetItemText(hwndList, itemnum, 0, fp, 16);
			  TCHAR *name = new TCHAR [64];
			  ListView_GetItemText(hwndList, itemnum, 2, name, 64);
			  {
				  if(_tcschr(name, _T('(')))
				  {
					  wstring str = name;
					  wstring::size_type p = str.find(_T("("))-1;
					  _tcscpy(name, str.substr(0, p).c_str());
				  }
			  }
			  string out;
			  DWORD code;
			  wstring cmd = _T("--batch -a --export ");
			  cmd += fp;
//			  cmd += _T("\"");
			  gpg_execution_params params;
			  pxResult result;
			  params.cmd = &cmd;
			  params.useless = "";
			  params.out = &out;
			  params.code = &code;
			  params.result = &result;
			  boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
			  if(!gpg_thread.timed_join(boost::posix_time::seconds(10)))
			  {
				  gpg_thread.~thread();
				  TerminateProcess(params.hProcess, 1);
				  params.hProcess = NULL;
				  debuglog<<time_str()<<": GPG execution timed out, aborted\n";
				  break;
			  }
			  if(result == pxNotFound)
				  break;
			  string::size_type s = 0;
			  while((s = out.find("\r", s)) != string::npos)
			  {
				  out.erase(s, 1);
			  }
			  DBWriteContactSettingString(NULL, szGPGModuleName, "GPGPubKey", out.c_str());
			  DBWriteContactSettingTString(NULL, szGPGModuleName, "KeyMainName", name);
			  DBWriteContactSettingTString(NULL, szGPGModuleName, "KeyID", fp);
			  TCHAR passwd[64];
			  GetDlgItemText(hwndDlg, IDC_KEY_PASSWORD, passwd, 64);
			  if(passwd[0])
			  {
				  string dbsetting = "szKey_";
				  char *keyid = mir_t2a(fp);
				  dbsetting += keyid;
				  mir_free(keyid);
				  dbsetting += "_Password";
				  DBWriteContactSettingTString(NULL, szGPGModuleName, dbsetting.c_str(), passwd);
			  }
			  {
				  wstring keyinfo = TranslateT("Current private key id");
				  keyinfo += _T(": ");
				  keyinfo += (fp[0])?fp:_T("not set");
				  extern HWND hwndCurKey_p;
				  SetWindowText(hwndCurKey_p, keyinfo.c_str());
			  }
			  delete [] name;
		  }
		  bAutoExchange = CheckStateStoreDB(hwndDlg, IDC_AUTO_EXCHANGE, "bAutoExchange");
		  gpg_configured = isGPGConfigured();
		  DestroyWindow(hwndDlg);
		  break;
	  case IDC_OTHER:
		  {
			  void ShowLoadPublicKeyDialog();
			  extern map<int, HANDLE> user_data;
			  extern int item_num;
			  item_num = 0;		 //black magic here
			  user_data[1] = 0;
			  ShowLoadPublicKeyDialog();
			  ListView_DeleteAllItems(hwndList);
			  { 
				  int i = 1, iRow = 0;
				  item.mask = LVIF_TEXT;
				  item.iItem = i;
				  item.iSubItem = 0;
				  item.pszText = _T("");
				  {//parse gpg output
					  string out;
					  DWORD code;
					  wstring::size_type p = 0, p2 = 0, stop = 0;
					  {
						  wstring cmd = _T("--batch --list-secret-keys");
						  gpg_execution_params params;
						  pxResult result;
						  params.cmd = &cmd;
						  params.useless = "";
						  params.out = &out;
						  params.code = &code;
						  params.result = &result;
						  boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
						  if(!gpg_thread.timed_join(boost::posix_time::seconds(10)))
						  {
							  gpg_thread.~thread();
							  TerminateProcess(params.hProcess, 1);
							  params.hProcess = NULL;
							  debuglog<<time_str()<<": GPG execution timed out, aborted\n";
							  break;
						  }
						  if(result == pxNotFound)
							  break;
					  }
					  while(p != string::npos)
					  {
						  if((p = out.find("sec  ", p)) == string::npos)
							  break;
						  p += 5;
						  if(p < stop)
							  break;
						  stop = p;
						  p2 = out.find("/", p) - 1;
						  TCHAR *tmp = mir_wstrdup(toUTF16(out.substr(p,p2-p)).c_str());
						  item.pszText = tmp;
						  iRow = ListView_InsertItem(hwndList, &item);
						  ListView_SetItemText(hwndList, iRow, 4, tmp);
						  mir_free(tmp);
						  p2+=2;
						  p = out.find(" ", p2);
						  tmp = mir_wstrdup(toUTF16(out.substr(p2,p-p2)).c_str());
						  ListView_SetItemText(hwndList, iRow, 0, tmp);
						  mir_free(tmp);
						  p = out.find("uid  ", p);
						  p2 = out.find_first_not_of(" ", p+5);
						  p = out.find("<", p2);
						  tmp = mir_wstrdup(toUTF16(out.substr(p2,p-p2)).c_str());
						  ListView_SetItemText(hwndList, iRow, 2, tmp);
						  mir_free(tmp);
						  p++;
						  p2 = out.find(">", p);
						  tmp = mir_wstrdup(toUTF16(out.substr(p,p2-p)).c_str());
						  ListView_SetItemText(hwndList, iRow, 1, tmp);
						  mir_free(tmp);
						  p = out.find("ssb  ", p2) + 6;
						  p = out.find(" ", p) + 1;
						  p2 = out.find("\n", p);
						  tmp = mir_wstrdup(toUTF16(out.substr(p,p2-p-1)).c_str());
						  ListView_SetItemText(hwndList, iRow, 3, tmp);
						  mir_free(tmp);
						  ListView_SetColumnWidth(hwndList, 0, LVSCW_AUTOSIZE);// not sure about this
						  ListView_SetColumnWidth(hwndList, 1, LVSCW_AUTOSIZE);
						  ListView_SetColumnWidth(hwndList, 2, LVSCW_AUTOSIZE);
						  ListView_SetColumnWidth(hwndList, 3, LVSCW_AUTOSIZE);
						  ListView_SetColumnWidth(hwndList, 4, LVSCW_AUTOSIZE);
						  i++;
					  }
				  }
			  }
		  }
		  break;
	  case IDC_DELETE_KEY:
		  ListView_GetItemText(hwndList, itemnum, 0, fp, 16);
		  {
			  string out;
			  DWORD code;
			  wstring cmd = _T("--batch --fingerprint ");
			  cmd += fp;
			  gpg_execution_params params;
			  pxResult result;
			  params.cmd = &cmd;
			  params.useless = "";
			  params.out = &out;
			  params.code = &code;
			  params.result = &result;			
			  boost::thread *gpg_thread = new boost::thread(boost::bind(&pxEexcute_thread, &params));
			  if(!gpg_thread->timed_join(boost::posix_time::seconds(10)))
			  {
				  delete gpg_thread;
				  TerminateProcess(params.hProcess, 1);
				  params.hProcess = NULL;
				  debuglog<<time_str()<<": GPG execution timed out, aborted\n";
				  break;
			  }
			  if(result == pxNotFound)
				  break;
			  string::size_type s = out.find("Key fingerprint = ");
			  s += strlen("Key fingerprint = ");
			  string::size_type s2 = out.find("\n", s);
			  TCHAR *fp = NULL;
			  {
				  string tmp = out.substr(s, s2-s-1).c_str();
				  string::size_type p = 0;
				  while((p = tmp.find(" ", p)) != string::npos)
				  {
					  tmp.erase(p, 1);
				  }
				  fp = mir_a2t(tmp.c_str());
			  }
			  cmd.clear();
			  out.clear();
			  cmd += _T("--batch --delete-secret-and-public-key --fingerprint ");
			  cmd += fp;
			  mir_free(fp);
			  gpg_thread = new boost::thread(boost::bind(&pxEexcute_thread, &params));
			  if(!gpg_thread->timed_join(boost::posix_time::seconds(10)))
			  {
				  delete gpg_thread;
				  TerminateProcess(params.hProcess, 1);
				  params.hProcess = NULL;
				  debuglog<<time_str()<<": GPG execution timed out, aborted\n";
				  break;
			  }
			  if(result == pxNotFound)
				  break;
		  }
		  DBDeleteContactSetting(NULL, szGPGModuleName, "GPGPubKey");
		  DBDeleteContactSetting(NULL, szGPGModuleName, "KeyID");
		  DBDeleteContactSetting(NULL, szGPGModuleName, "KeyComment");
		  DBDeleteContactSetting(NULL, szGPGModuleName, "KeyMainName");
		  DBDeleteContactSetting(NULL, szGPGModuleName, "KeyMainEmail");
		  DBDeleteContactSetting(NULL, szGPGModuleName, "KeyType");
		  ListView_DeleteItem(hwndList, itemnum);
		  break;
	  case IDC_GENERATE_RANDOM:
		  {
			  wstring path;
			  { //generating key file
				  TCHAR *tmp = UniGetContactSettingUtf(NULL, szGPGModuleName, "szHomePath", _T(""));
				  path = tmp;
				  mir_free(tmp);
				  path.append(_T("\\new_key"));
				  wfstream f(path.c_str(), std::ios::out);
				  if(!f.is_open())
				  {
					  MessageBox(0, TranslateT("Failed to open file"), TranslateT("Error"), MB_OK);
					  break;
				  }
				  f<<"Key-Type: RSA";
				  f<<"\n";
				  f<<"Key-Length: 2048";
				  f<<"\n";
				  f<<"Subkey-Type: RSA";
				  f<<"\n";
				  f<<"Name-Real: ";
				  f<<get_random(6).c_str();
				  f<<"\n";
				  f<<"Name-Email: ";
				  f<<get_random(5).c_str();
				  f<<"@";
				  f<<get_random(5).c_str();
				  f<<".";
				  f<<get_random(3).c_str();
				  f<<"\n";
				  f.close();
			  }
			  { //gpg execution
				  DWORD code;
				  string out;
				  wstring cmd;
				  cmd += _T("--batch --yes --gen-key \"");
				  cmd += path;
				  cmd += _T("\"");
				  gpg_execution_params params;
				  pxResult result;
				  params.cmd = &cmd;
				  params.useless = "";
				  params.out = &out;
				  params.code = &code;
				  params.result = &result;
				  boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
				  if(!gpg_thread.timed_join(boost::posix_time::minutes(10)))
				  {
					  gpg_thread.~thread();
					  TerminateProcess(params.hProcess, 1);
					  params.hProcess = NULL;
					  debuglog<<time_str()<<": GPG execution timed out, aborted";
					  break;
				  }
				  if(result == pxNotFound)
					  break;

				  DeleteFile(path.c_str());
				  string::size_type p1 = 0;
				  if((p1 = out.find("key ")) != string::npos)
					  path = toUTF16(out.substr(p1+4, 8));
				  else
					  path.clear();
			  }
			  if(!path.empty())
			  {
				  string out;
				  DWORD code;
				  wstring cmd = _T("--batch -a --export ");
				  cmd += path;
				  gpg_execution_params params;
				  pxResult result;
				  params.cmd = &cmd;
				  params.useless = "";
				  params.out = &out;
				  params.code = &code;
				  params.result = &result;
				  boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
				  if(!gpg_thread.timed_join(boost::posix_time::seconds(10)))
				  {
					  gpg_thread.~thread();
					  TerminateProcess(params.hProcess, 1);
					  params.hProcess = NULL;
					  debuglog<<time_str()<<"GPG execution timed out, aborted\n";
					  break;
				  }
				  if(result == pxNotFound)
					  break;
				  string::size_type s = 0;
				  while((s = out.find("\r", s)) != string::npos)
				  {
					  out.erase(s, 1);
				  }
				  DBWriteContactSettingString(NULL, szGPGModuleName, "GPGPubKey", out.c_str());
				  DBWriteContactSettingTString(NULL, szGPGModuleName, "KeyID", path.c_str());
				  extern HWND hwndCurKey_p;
				  SetWindowText(hwndCurKey_p, path.c_str());
			  }
		  }
		  DestroyWindow(hwndDlg);
		  break;
	  }
	  break;
	}
    
  case WM_NOTIFY:
    {
		if(hdr && IsWindowVisible(hdr->hdr.hwndFrom) && hdr->iItem != (-1))
		{
			if(hdr->hdr.code == NM_CLICK)
			{				
				EnableWindow(GetDlgItem(hwndDlg, ID_OK), 1);
				itemnum = hdr->iItem;
			}
		}
//      switch (((LPNMHDR)lParam)->code)
  //    {
//	  default:
//		  break;
        
//      }
	}
	break;
	case WM_CLOSE:
		DestroyWindow(hwndDlg);
		break;
	case WM_DESTROY:
		{
			GetWindowRect(hwndDlg, &firstrun_rect);
			DBWriteContactSettingDword(NULL, szGPGModuleName, "FirstrunWindowX", firstrun_rect.left);
			DBWriteContactSettingDword(NULL, szGPGModuleName, "FirstrunWindowY", firstrun_rect.top);
		}
		hwndFirstRun = NULL;
		break;

  }

  return FALSE;
}

void ShowFirstRunDialog();

static BOOL CALLBACK DlgProcGpgBinOpts(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	TCHAR *tmp = NULL;
  switch (msg)
  {
  case WM_INITDIALOG:
    {
		TranslateDialogDefault(hwndDlg);
		TCHAR *path = new TCHAR [MAX_PATH];
		bool gpg_exists = false, lang_exists = false;
		{
			char *mir_path = new char [MAX_PATH];
			CallService(MS_UTILS_PATHTOABSOLUTE, (WPARAM)"\\", (LPARAM)mir_path);
			SetCurrentDirectoryA(mir_path);
			tmp = mir_a2t(mir_path);
			mir_free(mir_path);
			mir_realloc(path, (_tcslen(path)+128)*sizeof(TCHAR));
			TCHAR *gpg_path = new TCHAR [MAX_PATH], *gpg_lang_path = new TCHAR [MAX_PATH];
			_tcscpy(gpg_path, tmp);
			_tcscat(gpg_path, _T("\\GnuPG\\gpg.exe"));
			_tcscpy(gpg_lang_path, tmp);
			_tcscat(gpg_lang_path, _T("\\GnuPG\\gnupg.nls\\en@quot.mo"));
			mir_free(tmp);
			if(_waccess(gpg_path, 0) != -1)
			{
				gpg_exists = true;
				_tcscpy(path, _T("GnuPG\\gpg.exe"));
			}
			if(_waccess(gpg_lang_path, 0) != -1)
				lang_exists = true;
			if(gpg_exists && !lang_exists) //TODO: check gpg version
				MessageBox(0, TranslateT("gpg binary found in miranda folder, but english locale does not exists.\nit's highly recommended to place \\gnupg.nls\\en@quot.mo in gnupg folder under miranda root.\nwithout this file you may expirense many problem with gpg output on non english systems.\nand plugin may completely do not work.\nyou have beed warned."), TranslateT("Warning"), MB_OK);
			mir_free(gpg_path);
			mir_free(gpg_lang_path);
		}
		DWORD len = MAX_PATH;
		if(!gpg_exists)
		{
			tmp = UniGetContactSettingUtf(NULL, szGPGModuleName, "szGpgBinPath", (SHGetValue(HKEY_CURRENT_USER, _T("Software\\GNU\\GnuPG"), _T("gpgProgram"), 0, path, &len) == ERROR_SUCCESS)?path:_T(""));
			if(tmp[0])
			{
				char *mir_path = new char [MAX_PATH];
				CallService(MS_UTILS_PATHTOABSOLUTE, (WPARAM)"\\", (LPARAM)mir_path);
				SetCurrentDirectoryA(mir_path);
				delete [] mir_path;
				if(_waccess(tmp, 0) == -1)
				{
					if(errno == ENOENT)
					{
						MessageBox(0, TranslateT("wrong gpg binary location found in system.\nplease choose another location"), TranslateT("Warning"), MB_OK);
					}
				}
			}
		}
		else
			tmp = mir_wstrdup(path);
		delete [] path;
		SetDlgItemText(hwndDlg, IDC_BIN_PATH, tmp);
		bool bad_version = false;
		if(gpg_exists && lang_exists)
		{
			DBWriteContactSettingTString(NULL, szGPGModuleName, "szGpgBinPath", tmp);
			string out;
			DWORD code;
			wstring cmd = _T("--version");
			gpg_execution_params params;
			pxResult result;
			params.cmd = &cmd;
			params.useless = "";
			params.out = &out;
			params.code = &code;
			params.result = &result;
			gpg_configured = true;
			boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
			if(!gpg_thread.timed_join(boost::posix_time::seconds(10)))
			{
				gpg_thread.~thread();
				TerminateProcess(params.hProcess, 1);
				params.hProcess = NULL;
				debuglog<<time_str()<<": GPG execution timed out, aborted\n";
			}
			gpg_configured = false;
			DBDeleteContactSetting(NULL, szGPGModuleName, "szGpgBinPath");
			string::size_type p1 = out.find("(GnuPG) ");
			if(p1 != string::npos)
			{
				p1 += strlen("(GnuPG) ");
				if(out[p1] != '1')
					bad_version = true;
			}
			else
			{
				bad_version = false;
				MessageBox(0, TranslateT("This is not gnupg binary !\nrecommended to use GnuPG v1.x.x with this plugn."), TranslateT("Warning"), MB_OK);
			}
			if(bad_version)
				MessageBox(0, TranslateT("Unsupported gnupg version found, use at you own risk!\nrecommended to use GnuPG v1.x.x with this plugn."), TranslateT("Warning"), MB_OK);
		}
		mir_free(tmp);
		if(!gpg_exists)
		{
			wstring path_ = _wgetenv(_T("APPDATA"));
			path_ += _T("\\GnuPG");
			tmp = UniGetContactSettingUtf(NULL, szGPGModuleName, "szHomePath", (TCHAR*)path_.c_str());
		}
		SetDlgItemText(hwndDlg, IDC_HOME_DIR, !gpg_exists?tmp:_T("gpg"));
		mir_free(tmp);
		if(gpg_exists && lang_exists && !bad_version)
			MessageBox(0, TranslateT("Your GPG version is supported. The language file was found.\nGPG plugin should work fine.\nPress OK to continue."), TranslateT("Info"), MB_OK);
		extern bool bIsMiranda09;
		EnableWindow(GetDlgItem(hwndDlg, IDC_AUTO_EXCHANGE), bIsMiranda09);
      return TRUE;
    }
    
 
  case WM_COMMAND:
    {
      switch (LOWORD(wParam))
      {
	  case IDC_SET_BIN_PATH:
		  {
			  GetFilePath(_T("Choose gpg.exe"), "szGpgBinPath", _T("*.exe"), _T("EXE Executables"));
			  tmp = UniGetContactSettingUtf(NULL, szGPGModuleName, "szGpgBinPath", _T("gpg.exe"));
			  SetDlgItemText(hwndDlg, IDC_BIN_PATH, tmp);
			  char mir_path[MAX_PATH];
			  char *atmp = mir_t2a(tmp);
			  mir_free(tmp);
			  CallService(MS_UTILS_PATHTOABSOLUTE, (WPARAM)"\\", (LPARAM)mir_path);
			  char* p_path = NULL;
			  if(StriStr(atmp, mir_path))
			  {
				  p_path = atmp + strlen(mir_path);
				  tmp = mir_a2t(p_path);
				  SetDlgItemText(hwndDlg, IDC_BIN_PATH, tmp);
			  }
		  }
		  break;
	  case IDC_SET_HOME_DIR:
		  {
			  GetFolderPath(_T("Set home diractory"), "szHomePath");
			  tmp = UniGetContactSettingUtf(NULL, szGPGModuleName, "szHomePath", _T(""));
			  SetDlgItemText(hwndDlg, IDC_HOME_DIR, tmp);
			  char mir_path[MAX_PATH];
			  char *atmp = mir_t2a(tmp);
			  mir_free(tmp);
			  CallService(MS_UTILS_PATHTOABSOLUTE, (WPARAM)"\\", (LPARAM)mir_path);
			  char* p_path = NULL;
			  if(StriStr(atmp, mir_path))
			  {
				  p_path = atmp + strlen(mir_path);
				  tmp = mir_a2t(p_path);
				  SetDlgItemText(hwndDlg, IDC_HOME_DIR, tmp);
			  }
		  }
		  break;
      case ID_OK:
        {
		  TCHAR tmp[512];
		  GetDlgItemText(hwndDlg, IDC_BIN_PATH, tmp, 512);
		  if(tmp[0])
		  {
			  char *mir_path = new char [MAX_PATH];
			  CallService(MS_UTILS_PATHTOABSOLUTE, (WPARAM)"\\", (LPARAM)mir_path);
			  SetCurrentDirectoryA(mir_path);
			  delete [] mir_path;
			  if(_waccess(tmp, 0) == -1)
			  {
				  if(errno == ENOENT)
				  {
					  MessageBox(0, TranslateT("gpg binary does not exists.\nplease choose another location"), TranslateT("Warning"), MB_OK);
					  break;
				  }
			  }
		  }
		  else
		  {
			  MessageBox(0, TranslateT("please choose gpg binary location"), TranslateT("Warning"), MB_OK);
			  break;
		  }
		  {
			  bool bad_version = false;
			  DBWriteContactSettingTString(NULL, szGPGModuleName, "szGpgBinPath", tmp);
			  string out;
			  DWORD code;
			  wstring cmd = _T("--version");
			  gpg_execution_params params;
			  pxResult result;
			  params.cmd = &cmd;
			  params.useless = "";
			  params.out = &out;
			  params.code = &code;
			  params.result = &result;
			  gpg_configured = true;
			  boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
			  if(!gpg_thread.timed_join(boost::posix_time::seconds(10)))
			  {
				  TerminateProcess(params.hProcess, 1);
				  params.hProcess = NULL;
				  gpg_thread.~thread();
				  debuglog<<time_str()<<": GPG execution timed out, aborted\n";
			  }
			  gpg_configured = false;
			  DBDeleteContactSetting(NULL, szGPGModuleName, "szGpgBinPath");
			  string::size_type p1 = out.find("(GnuPG) ");
			  if(p1 != string::npos)
			  {
				  p1 += strlen("(GnuPG) ");
				  if(out[p1] != '1')
					  bad_version = true;
			  }
			  else
			  {
				  bad_version = false;
				  MessageBox(0, TranslateT("This is not gnupg binary !\nrecommended to use GnuPG v1.x.x with this plugn."), TranslateT("Warning"), MB_OK);
			  }
			  if(bad_version)
				  MessageBox(0, TranslateT("Unsupported gnupg version found, use at you own risk!\nrecommended to use GnuPG v1.x.x with this plugn."), TranslateT("Warning"), MB_OK);
		  }
		  DBWriteContactSettingTString(NULL, szGPGModuleName, "szGpgBinPath", tmp);
		  GetDlgItemText(hwndDlg, IDC_HOME_DIR, tmp, 512);
		  while(tmp[_tcslen(tmp)-1] == '\\')
			  tmp[_tcslen(tmp)-1] = '\0';
		  if(!tmp[0])
		  {
			  MessageBox(0, TranslateT("please set keyring's home directory"), TranslateT("Warning"), MB_OK);
			  break;
		  }
		  DBWriteContactSettingTString(NULL, szGPGModuleName, "szHomePath", tmp);
		  {
			  TCHAR *path = UniGetContactSettingUtf(NULL, szGPGModuleName, "szHomePath", _T(""));
			  DWORD dwFileAttr = GetFileAttributes(path);
			  if (dwFileAttr != INVALID_FILE_ATTRIBUTES)
			  {
				  dwFileAttr &=~ FILE_ATTRIBUTE_READONLY;
				  SetFileAttributes(path, dwFileAttr);
			  }
			  mir_free(path);
		  }
		  gpg_configured = true;
		  DBWriteContactSettingByte(NULL, szGPGModuleName, "FirstRun", 0);
		  DestroyWindow(hwndDlg);
		  ShowFirstRunDialog();
        }
		break;
	  case IDC_GENERATE_RANDOM:
        {
			TCHAR tmp[512];
			GetDlgItemText(hwndDlg, IDC_BIN_PATH, tmp, 512);
			if(tmp[0])
			{
				char *mir_path = new char [MAX_PATH];
				CallService(MS_UTILS_PATHTOABSOLUTE, (WPARAM)"\\", (LPARAM)mir_path);
				SetCurrentDirectoryA(mir_path);
				delete [] mir_path;
				if(_waccess(tmp, 0) == -1)
				{
					if(errno == ENOENT)
					{
						MessageBox(0, TranslateT("gpg binary does not exists.\nplease choose another location"), TranslateT("Warning"), MB_OK);
						break;
					}
				}
			}
			else
			{
				MessageBox(0, TranslateT("please choose gpg binary location"), TranslateT("Warning"), MB_OK);
				break;
			}
			{
				bool bad_version = false;
				DBWriteContactSettingTString(NULL, szGPGModuleName, "szGpgBinPath", tmp);
				string out;
				DWORD code;
				wstring cmd = _T("--version");
				gpg_execution_params params;
				pxResult result;
				params.cmd = &cmd;
				params.useless = "";
				params.out = &out;
				params.code = &code;
				params.result = &result;
				gpg_configured = true;
				boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
				if(!gpg_thread.timed_join(boost::posix_time::seconds(10)))
				{
					TerminateProcess(params.hProcess, 1);
					params.hProcess = NULL;
					gpg_thread.~thread();
					debuglog<<time_str()<<": GPG execution timed out, aborted\n";
				}
				gpg_configured = false;
				DBDeleteContactSetting(NULL, szGPGModuleName, "szGpgBinPath");
				string::size_type p1 = out.find("(GnuPG) ");
				if(p1 != string::npos)
				{
					p1 += strlen("(GnuPG) ");
					if(out[p1] != '1')
						bad_version = true;
				}
				else
				{
					bad_version = false;
					MessageBox(0, TranslateT("This is not gnupg binary !\nrecommended to use GnuPG v1.x.x with this plugn."), TranslateT("Warning"), MB_OK);
				}
				if(bad_version)
					MessageBox(0, TranslateT("Unsupported gnupg version found, use at you own risk!\nrecommended to use GnuPG v1.x.x with this plugn."), TranslateT("Warning"), MB_OK);
			}
			DBWriteContactSettingTString(NULL, szGPGModuleName, "szGpgBinPath", tmp);
			GetDlgItemText(hwndDlg, IDC_HOME_DIR, tmp, 512);
			while(tmp[_tcslen(tmp)-1] == '\\')
				tmp[_tcslen(tmp)-1] = '\0';
			if(!tmp[0])
			{
				MessageBox(0, TranslateT("please set keyring's home directory"), TranslateT("Warning"), MB_OK);
				break;
			}
			DBWriteContactSettingTString(NULL, szGPGModuleName, "szHomePath", tmp);
			{
				TCHAR *path = UniGetContactSettingUtf(NULL, szGPGModuleName, "szHomePath", _T(""));
				DWORD dwFileAttr = GetFileAttributes(path);
				if (dwFileAttr != INVALID_FILE_ATTRIBUTES)
				{
					dwFileAttr &=~ FILE_ATTRIBUTE_READONLY;
					SetFileAttributes(path, dwFileAttr);
				}
				mir_free(path);
			}
		}
		  {
			  wstring path;
			  { //generating key file
				  TCHAR *tmp = UniGetContactSettingUtf(NULL, szGPGModuleName, "szHomePath", _T(""));
				  path = tmp;
				  mir_free(tmp);
				  path.append(_T("\\new_key"));
				  wfstream f(path.c_str(), std::ios::out);
				  if(!f.is_open())
				  {
					  MessageBox(0, TranslateT("Failed to open file"), TranslateT("Error"), MB_OK);
					  break;
				  }
				  f<<"Key-Type: RSA";
				  f<<"\n";
				  f<<"Key-Length: 2048";
				  f<<"\n";
				  f<<"Subkey-Type: RSA";
				  f<<"\n";
				  f<<"Name-Real: ";
				  f<<get_random(6).c_str();
				  f<<"\n";
				  f<<"Name-Email: ";
				  f<<get_random(5).c_str();
				  f<<"@";
				  f<<get_random(5).c_str();
				  f<<".";
				  f<<get_random(3).c_str();
				  f<<"\n";
				  f.close();
			  }
			  { //gpg execution
				  DWORD code;
				  string out;
				  wstring cmd;
				  cmd += _T("--batch --yes --gen-key \"");
				  cmd += path;
				  cmd += _T("\"");
				  gpg_execution_params params;
				  pxResult result;
				  params.cmd = &cmd;
				  params.useless = "";
				  params.out = &out;
				  params.code = &code;
				  params.result = &result;
				  gpg_configured = true;
				  boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
				  if(!gpg_thread.timed_join(boost::posix_time::minutes(10)))
				  {
					  gpg_thread.~thread();
					  TerminateProcess(params.hProcess, 1);
					  params.hProcess = NULL;
					  debuglog<<time_str()<<": GPG execution timed out, aborted";
					  gpg_configured = false;
					  break;
				  }
				  gpg_configured = false;
				  if(result == pxNotFound)
					  break;
				  DeleteFile(path.c_str());
				  string::size_type p1 = 0;
				  if((p1 = out.find("key ")) != string::npos)
					  path = toUTF16(out.substr(p1+4, 8));
				  else
					  path.clear();
			  }
			  if(!path.empty())
			  {
				  string out;
				  DWORD code;
				  wstring cmd = _T("--batch -a --export ");
				  cmd += path;
				  gpg_execution_params params;
				  pxResult result;
				  params.cmd = &cmd;
				  params.useless = "";
				  params.out = &out;
				  params.code = &code;
				  params.result = &result;
				  gpg_configured = true;
				  boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
				  if(!gpg_thread.timed_join(boost::posix_time::seconds(10)))
				  {
					  gpg_thread.~thread();
					  TerminateProcess(params.hProcess, 1);
					  params.hProcess = NULL;
					  debuglog<<time_str()<<"GPG execution timed out, aborted\n";
					  gpg_configured = false;
					  break;
				  }
				  gpg_configured = false;
				  if(result == pxNotFound)
					  break;
				  string::size_type s = 0;
				  while((s = out.find("\r", s)) != string::npos)
				  {
					  out.erase(s, 1);
				  }
				  DBWriteContactSettingString(NULL, szGPGModuleName, "GPGPubKey", out.c_str());
				  DBWriteContactSettingTString(NULL, szGPGModuleName, "KeyID", path.c_str());
				  extern HWND hwndCurKey_p;
				  SetWindowText(hwndCurKey_p, path.c_str());
			  }
		  }
		  bAutoExchange = CheckStateStoreDB(hwndDlg, IDC_AUTO_EXCHANGE, "bAutoExchange");
		  gpg_configured = true;
		  DBWriteContactSettingByte(NULL, szGPGModuleName, "FirstRun", 0);
		  DestroyWindow(hwndDlg);
		  break;
	  default:
		break;
      }
      
      break;
    }
    
  case WM_NOTIFY:
    {
/*      switch (((LPNMHDR)lParam)->code)
      {
	  default:
		  break;
      }*/
	}
    break;
  case WM_CLOSE:
	  DestroyWindow(hwndDlg);
	  break;
  case WM_DESTROY:
	  hwndSetDirs = NULL;
	  break;

  }
  return FALSE;
}

static BOOL CALLBACK DlgProcNewKeyDialog(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	extern HANDLE new_key_hcnt;
	extern boost::mutex new_key_hcnt_mutex;
	HANDLE hContact = INVALID_HANDLE_VALUE;
	void ImportKey();
	TCHAR *tmp = NULL;
  switch (msg)
  {
  case WM_INITDIALOG:
    {
		hContact = new_key_hcnt;
		new_key_hcnt_mutex.unlock();
		SetWindowPos(hwndDlg, 0, new_key_rect.left, new_key_rect.top, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
		TranslateDialogDefault(hwndDlg);
		TCHAR *tmp = UniGetContactSettingUtf(hContact, szGPGModuleName, "GPGPubKey", _T(""));
		SetDlgItemText(hwndDlg, IDC_MESSAGE, (_tcslen(tmp) > 0)?TranslateT("There is existing key for contact, would you like to replace with new key ?"):TranslateT("New public key was received, do you want to import it?"));
		EnableWindow(GetDlgItem(hwndDlg, IDC_IMPORT_AND_USE), DBGetContactSettingByte(hContact, szGPGModuleName, "GPGEncryption", 0)?0:1);
		SetDlgItemText(hwndDlg, ID_IMPORT, (_tcslen(tmp) > 0)?TranslateT("Replace"):_T("Accept"));
		mir_free(tmp);
		tmp = new TCHAR [256];
		_tcscpy(tmp, TranslateT("Received key from "));
		_tcscat(tmp, (TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)hContact, (LPARAM)GCDNF_TCHAR));
		SetDlgItemText(hwndDlg, IDC_KEY_FROM, tmp);
		delete [] tmp;
      return TRUE;
    }
    
 
  case WM_COMMAND:
    {
      switch (LOWORD(wParam))
      {
	  case ID_IMPORT:
		  ImportKey();
		  DestroyWindow(hwndDlg);
		  break;
	  case IDC_IMPORT_AND_USE:
		  ImportKey();
		  DBWriteContactSettingByte(hContact, szGPGModuleName, "GPGEncryption", 1);
		  void setSrmmIcon(HANDLE hContact);
		  void setClistIcon(HANDLE hContact);
		  setSrmmIcon(hContact);
		  setClistIcon(hContact);
		  DestroyWindow(hwndDlg);
		  break;
	  case IDC_IGNORE_KEY:
		  DestroyWindow(hwndDlg);
		  break;
	  default:
		break;
      }
      
      break;
    }
    
  case WM_NOTIFY:
    {
/*      switch (((LPNMHDR)lParam)->code)
      {
	  default:
		  break;
      }*/
	}
    break;
  case WM_CLOSE:
	  DestroyWindow(hwndDlg);
	  break;
  case WM_DESTROY:
	  {
		  GetWindowRect(hwndDlg, &new_key_rect);
		  DBWriteContactSettingDword(NULL, szGPGModuleName, "NewKeyWindowX", new_key_rect.left);
		  DBWriteContactSettingDword(NULL, szGPGModuleName, "NewKeyWindowY", new_key_rect.top);
	  }
	  hwndNewKey = NULL;
	  break;

  }
  return FALSE;
}
static BOOL CALLBACK DlgProcKeyGenDialog(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_INITDIALOG:
    {
		SetWindowPos(hwndDlg, 0, key_gen_rect.left, key_gen_rect.top, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
		TranslateDialogDefault(hwndDlg);
		SetWindowText(hwndDlg, TranslateT("Key Generation dialog"));
		ComboBoxAddStringUtf(GetDlgItem(hwndDlg, IDC_KEY_TYPE), _T("RSA"), 0);
		ComboBoxAddStringUtf(GetDlgItem(hwndDlg, IDC_KEY_TYPE), _T("DSA"), 1);
		SendDlgItemMessage(hwndDlg, IDC_KEY_TYPE, CB_SETCURSEL, (WPARAM)1, 0);
		SetDlgItemInt(hwndDlg, IDC_KEY_EXPIRE_DATE, 0, 0);
		SetDlgItemInt(hwndDlg, IDC_KEY_LENGTH, 2048, 0);
      return TRUE;
    }
    
 
  case WM_COMMAND:
    {
      switch (LOWORD(wParam))
      {
	  case IDCANCEL:
		  DestroyWindow(hwndDlg);
		  break;
	  case IDOK:
		  {
			  wstring path;
			  { //data sanity checks
				  TCHAR *tmp = new TCHAR [5];
				  GetDlgItemText(hwndDlg, IDC_KEY_TYPE, tmp, 5);
				  if(_tcslen(tmp) < 3)
				  {
					  mir_free(tmp);
					  MessageBox(0, TranslateT("You must set encryption algorythm first"), TranslateT("Error"), MB_OK);
					  break;
				  }
				  mir_free(tmp);
				  tmp = new TCHAR [5];
				  GetDlgItemText(hwndDlg, IDC_KEY_LENGTH, tmp, 5);
				  int length = _ttoi(tmp);
				  mir_free(tmp);
				  if(length < 1024 || length > 4096)
				  {
					  MessageBox(0, TranslateT("Key length must be of length from 1024 to 4096 bits"), TranslateT("Error"), MB_OK);
					  break;
				  }
				  tmp = new TCHAR [12];
				  GetDlgItemText(hwndDlg, IDC_KEY_EXPIRE_DATE, tmp, 12);
				  if(_tcslen(tmp) != 10 && tmp[0] != '0')
				  {
					  MessageBox(0, TranslateT("Invalid date"), TranslateT("Error"), MB_OK);
					  delete [] tmp;
					  break;
				  }
				  delete [] tmp;
				  tmp = new TCHAR [128];
				  GetDlgItemText(hwndDlg, IDC_KEY_REAL_NAME, tmp, 128);
				  if(_tcslen(tmp) < 5)
				  {
					  MessageBox(0, TranslateT("Name must contain at least 5 characters"), TranslateT("Error"), MB_OK);
					  delete [] tmp;
					  break;
				  }
				  else if (_tcschr(tmp, _T('(')) || _tcschr(tmp, _T(')')))
				  {
					  MessageBox(0, TranslateT("Name cannot contain '(' or ')'"), TranslateT("Error"), MB_OK);
					  delete [] tmp;
					  break;
				  }
				  delete [] tmp;
				  tmp = new TCHAR [128];
				  GetDlgItemText(hwndDlg, IDC_KEY_EMAIL, tmp, 128);
				  if((_tcslen(tmp)) < 5 || (!_tcschr(tmp, _T('@'))) || (!_tcschr(tmp, _T('.'))))
				  {
					  MessageBox(0, TranslateT("Invalid Email"), TranslateT("Error"), MB_OK);
					  delete [] tmp;
					  break;
				  }
				  delete [] tmp;
			  }		  
			  { //generating key file
				  TCHAR *tmp = UniGetContactSettingUtf(NULL, szGPGModuleName, "szHomePath", _T(""));
				  char  *tmp2;// = mir_t2a(tmp);
				  path = tmp;
				  mir_free(tmp);
				  //			  mir_free(tmp2);
				  path.append(_T("\\new_key"));
				  wfstream f(path.c_str(), std::ios::out);
				  if(!f.is_open())
				  {
					  MessageBox(0, TranslateT("Failed to open file"), TranslateT("Error"), MB_OK);
					  break;
				  }
				  f<<"Key-Type: ";
				  tmp = new TCHAR [5];
				  GetDlgItemText(hwndDlg, IDC_KEY_TYPE, tmp, 5);
				  tmp2 = mir_t2a(tmp);
				  delete [] tmp;
				  char *subkeytype = new char [6];
				  if(strstr(tmp2, "RSA"))
					  strcpy(subkeytype, "RSA");
				  else if(strstr(tmp2, "DSA")) //this is useless check for now, but it will be required if someone add another key types support
					  strcpy(subkeytype, "ELG-E");
				  f<<tmp2;
				  mir_free(tmp2);
				  f<<"\n";
				  f<<"Key-Length: ";
				  tmp = new TCHAR [5]; 
				  GetDlgItemText(hwndDlg, IDC_KEY_LENGTH, tmp, 5);
				  int length = _ttoi(tmp);
				  delete [] tmp;
				  f<<length;
				  f<<"\n";
				  f<<"Subkey-Length: ";
				  f<<length;
				  f<<"\n";
				  f<<"Subkey-Type: ";
				  f<<subkeytype;
				  delete [] subkeytype;
				  f<<"\n";
				  tmp = new TCHAR [64]; //i hope this is enough for password
				  GetDlgItemText(hwndDlg, IDC_KEY_PASSWD, tmp, 64);
				  if(tmp[0])
				  {
					  f<<"Passphrase: ";
					  tmp2 = mir_strdup(toUTF8(tmp).c_str());
					  f<<tmp2;
					  f<<"\n";
					  mir_free(tmp2);
				  }
				  delete [] tmp;
				  f<<"Name-Real: ";
				  tmp = new TCHAR [128];
				  GetDlgItemText(hwndDlg, IDC_KEY_REAL_NAME, tmp, 128);
				  tmp2 = mir_strdup(toUTF8(tmp).c_str());
				  f<<tmp2;
				  mir_free(tmp2);
				  delete [] tmp;
				  f<<"\n";
				  tmp = new TCHAR [512];
				  GetDlgItemText(hwndDlg, IDC_KEY_COMMENT, tmp, 512);
				  if(tmp[0])
				  {
					  tmp2 = mir_strdup(toUTF8(tmp).c_str());
					  f<<"Name-Comment: ";
					  f<<tmp2;
					  f<<"\n";
				  }
				  mir_free(tmp2);
				  delete [] tmp;
				  f<<"Name-Email: ";
				  tmp = new TCHAR [128];
				  GetDlgItemText(hwndDlg, IDC_KEY_EMAIL, tmp, 128);
				  tmp2 = mir_strdup(toUTF8(tmp).c_str());
				  f<<tmp2;
				  mir_free(tmp2);
				  delete [] tmp;
				  f<<"\n";
				  f<<"Expire-Date: ";
				  tmp = new TCHAR [12];
				  GetDlgItemText(hwndDlg, IDC_KEY_EXPIRE_DATE, tmp, 12);
				  tmp2 = mir_strdup(toUTF8(tmp).c_str());
				  f<<tmp2;
				  mir_free(tmp2);
				  delete [] tmp;
				  f<<"\n";
				  f.close();
			  }
			  { //gpg execution
				  DWORD code;
				  string out;
				  wstring cmd;
				  cmd += _T("--batch --yes --gen-key \"");
				  cmd += path;
				  cmd += _T("\"");
				  gpg_execution_params params;
				  pxResult result;
				  params.cmd = &cmd;
				  params.useless = "";
				  params.out = &out;
				  params.code = &code;
				  params.result = &result;
				  boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
				  if(!gpg_thread.timed_join(boost::posix_time::minutes(10)))
				  {
					  gpg_thread.~thread();
					  TerminateProcess(params.hProcess, 1);
					  params.hProcess = NULL;
					  debuglog<<time_str()<<": GPG execution timed out, aborted\n";
					  break;
				  }
				  if(result == pxNotFound)
					  break;
			  }
			  DeleteFile(path.c_str());
			  DestroyWindow(hwndDlg);
			{//parse gpg output
				LVITEM item = {0};
				int i = 1, iRow = 0;
				item.mask = LVIF_TEXT;
				item.iItem = i;
				item.iSubItem = 0;
				item.pszText = _T("");				
				string out;
				DWORD code;
				string::size_type p = 0, p2 = 0, stop = 0;
				{
					wstring cmd = _T("--list-secret-keys");
					gpg_execution_params params;
					pxResult result;
					params.cmd = &cmd;
					params.useless = "";
					params.out = &out;
					params.code = &code;
					params.result = &result;
					boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
					if(!gpg_thread.timed_join(boost::posix_time::seconds(10)))
					{
						gpg_thread.~thread();
						TerminateProcess(params.hProcess, 1);
						params.hProcess = NULL;
						debuglog<<time_str()<<": GPG execution timed out, aborted\n";
						break;
					}
					if(result == pxNotFound)
						break;
				}
				ListView_DeleteAllItems(hwndList_g);
				while(p != string::npos)
				{
					if((p = out.find("sec  ", p)) == string::npos)
						break;
					p += 5;
					if(p < stop)
						break;
					stop = p;
					p2 = out.find("/", p) - 1;
					TCHAR *tmp = mir_wstrdup(toUTF16(out.substr(p,p2-p)).c_str());
					item.pszText = tmp;
					iRow = ListView_InsertItem(hwndList_g, &item);
					ListView_SetItemText(hwndList_g, iRow, 4, tmp);
					mir_free(tmp);
					p2+=2;
					p = out.find(" ", p2);
					tmp = mir_wstrdup(toUTF16(out.substr(p2,p-p2)).c_str());
					ListView_SetItemText(hwndList_g, iRow, 0, tmp);
					mir_free(tmp);
					p = out.find("uid  ", p);
					p2 = out.find_first_not_of(" ", p+5);
					p = out.find("<", p2);
					tmp = mir_wstrdup(toUTF16(out.substr(p2,p-p2)).c_str());
					ListView_SetItemText(hwndList_g, iRow, 2, tmp);
					mir_free(tmp);
					p++;
					p2 = out.find(">", p);
					tmp = mir_wstrdup(toUTF16(out.substr(p,p2-p)).c_str());
					ListView_SetItemText(hwndList_g, iRow, 1, tmp);
					mir_free(tmp);
					p = out.find("ssb  ", p2) + 6;
					p = out.find(" ", p) + 1;
					p2 = out.find("\n", p);
					tmp = mir_wstrdup(toUTF16(out.substr(p,p2-p-1)).c_str());
					ListView_SetItemText(hwndList_g, iRow, 3, tmp);
					mir_free(tmp);
					ListView_SetColumnWidth(hwndList_g, 0, LVSCW_AUTOSIZE);// not sure about this
					ListView_SetColumnWidth(hwndList_g, 1, LVSCW_AUTOSIZE);
					ListView_SetColumnWidth(hwndList_g, 2, LVSCW_AUTOSIZE);
					ListView_SetColumnWidth(hwndList_g, 3, LVSCW_AUTOSIZE);
					ListView_SetColumnWidth(hwndList_g, 4, LVSCW_AUTOSIZE);
					i++;
				}
			}
		  }
		  break;
	  default:
		break;
      }
      
      break;
    }
    
  case WM_NOTIFY:
    {
/*      switch (((LPNMHDR)lParam)->code)
      {
	  default:
		  break;
      } */
	}
    break;
  case WM_CLOSE:
	  DestroyWindow(hwndDlg);
	  break;
  case WM_DESTROY:
	  {
		  GetWindowRect(hwndDlg, &key_gen_rect);
		  DBWriteContactSettingDword(NULL, szGPGModuleName, "KeyGenWindowX", key_gen_rect.left);
		  DBWriteContactSettingDword(NULL, szGPGModuleName, "KeyGenWindowY", key_gen_rect.top);
	  }
	  hwndKeyGen = NULL;
	  break;

  }
  return FALSE;
}

int itemnum2 = 0;

static BOOL CALLBACK DlgProcLoadExistingKey(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
	HWND hwndList=GetDlgItem(hwndDlg, IDC_EXISTING_KEY_LIST);
	hwndList_g = hwndList;
	LVCOLUMN col = {0};
	LVITEM item = {0};
	NMLISTVIEW * hdr = (NMLISTVIEW *) lParam;
	TCHAR id[16] = {0};
  switch (msg)
  {
  case WM_INITDIALOG:
    {
		SetWindowPos(hwndDlg, 0, load_existing_key_rect.left, load_existing_key_rect.top, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
		TranslateDialogDefault(hwndDlg);
		col.pszText = _T("Key ID");
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.cx = 50;
		ListView_InsertColumn(hwndList, 0, &col);
		ZeroMemory(&col,sizeof(col));
		col.pszText = _T("Email");
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.cx = 30;
		ListView_InsertColumn(hwndList, 1, &col);
		ZeroMemory(&col,sizeof(col));
		col.pszText = _T("Name");
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.cx = 250;
		ListView_InsertColumn(hwndList, 2, &col);
		ZeroMemory(&col,sizeof(col));
		col.pszText = _T("Creation date");
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.cx = 30;
		ListView_InsertColumn(hwndList, 3, &col);
		ZeroMemory(&col,sizeof(col));
		col.pszText = _T("Expiration date");
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.cx = 30;
		ListView_InsertColumn(hwndList, 4, &col);
		ZeroMemory(&col,sizeof(col));
		col.pszText = _T("Key length");
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.cx = 30;
		ListView_InsertColumn(hwndList, 5, &col);
		ListView_SetExtendedListViewStyleEx(hwndList, 0, LVS_EX_FULLROWSELECT);
		int i = 1, iRow = 0;
		{ 
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.iSubItem = 0;
			item.pszText = _T("");
			{//parse gpg output
				string out;
				DWORD code;
				string::size_type p = 0, p2 = 0, stop = 0;
				{
					wstring cmd = _T("--batch --list-keys");
					gpg_execution_params params;
					pxResult result;
					params.cmd = &cmd;
					params.useless = "";
					params.out = &out;
					params.code = &code;
					params.result = &result;
					boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
					if(!gpg_thread.timed_join(boost::posix_time::seconds(10)))
					{
						gpg_thread.~thread();
						TerminateProcess(params.hProcess, 1);
						params.hProcess = NULL;
						debuglog<<time_str()<<": GPG execution timed out, aborted\n";
						break;
					}
					if(result == pxNotFound)
						break;
				}
				while(p != string::npos)
				{
					if((p = out.find("pub  ", p)) == string::npos)
						break;
					p += 5;
					if(p < stop)
						break;
					stop = p;
					p2 = out.find("/", p) - 1;
					TCHAR *tmp = mir_wstrdup(toUTF16(out.substr(p,p2-p)).c_str());
					item.pszText = tmp;
					iRow = ListView_InsertItem(hwndList, &item);
					ListView_SetItemText(hwndList, iRow, 5, tmp);
					mir_free(tmp);
					p2+=2;
					p = out.find(" ", p2);
					tmp = mir_wstrdup(toUTF16(out.substr(p2,p-p2)).c_str());
					ListView_SetItemText(hwndList, iRow, 0, tmp);
					mir_free(tmp);
					p++;
					p2 = out.find("\n", p);
					string::size_type p3 = out.substr(p, p2-p).find("[");
					if(p3 != string::npos)
					{
						p3+=p;
						p2 = p3;
						p2--;
						p3++;
						p3+=strlen("expires: ");
						string::size_type p4 = out.find("]", p3);
						tmp = mir_wstrdup(toUTF16(out.substr(p3,p4-p3)).c_str());
						ListView_SetItemText(hwndList, iRow, 4, tmp);
						mir_free(tmp);
					}
					else
						p2--;
					tmp = mir_wstrdup(toUTF16(out.substr(p,p2-p)).c_str());
					ListView_SetItemText(hwndList, iRow, 3, tmp);
					mir_free(tmp);
					p = out.find("uid  ", p);
					p+= strlen("uid ");
					p2 = out.find("\n", p);
					p3 = out.substr(p, p2-p).find("<");
					if(p3 != string::npos)
					{
						p3+=p;
						p2=p3;
						p2--;
						p3++;
						string::size_type p4 = out.find(">", p3);
						tmp = mir_wstrdup(toUTF16(out.substr(p3,p4-p3)).c_str());
						ListView_SetItemText(hwndList, iRow, 1, tmp);
						mir_free(tmp);
					}
					else
						p2--;
					p = out.find_first_not_of(" ", p);
					tmp = mir_wstrdup(toUTF16(out.substr(p,p2-p)).c_str());
					ListView_SetItemText(hwndList, iRow, 2, tmp);
					mir_free(tmp);
//					p = out.find("sub  ", p2) + 6;
//					p = out.find(" ", p) + 1;
//					p2 = out.find("\n", p);
//					tmp = mir_wstrdup(toUTF16(out.substr(p,p2-p-1)).c_str());
//					ListView_SetItemText(hwndList, iRow, 3, tmp);
//					mir_free(tmp);
					ListView_SetColumnWidth(hwndList, 0, LVSCW_AUTOSIZE);// not sure about this
					ListView_SetColumnWidth(hwndList, 1, LVSCW_AUTOSIZE);
					ListView_SetColumnWidth(hwndList, 2, LVSCW_AUTOSIZE);
					ListView_SetColumnWidth(hwndList, 3, LVSCW_AUTOSIZE);
					ListView_SetColumnWidth(hwndList, 4, LVSCW_AUTOSIZE);
					ListView_SetColumnWidth(hwndList, 5, LVSCW_AUTOSIZE);
					i++;
				}
			}
		}
		return TRUE;
    } 
  case WM_COMMAND:
    {
      switch (LOWORD(wParam))
      {
	  case IDOK:
		  {
			  ListView_GetItemText(hwndList, itemnum2, 0, id, 16);
			  extern HWND hPubKeyEdit;
			  string out;
			  DWORD code;
			  wstring cmd = _T("--batch -a --export ");
			  cmd += id;
			  gpg_execution_params params;
			  pxResult result;
			  params.cmd = &cmd;
			  params.useless = "";
			  params.out = &out;
			  params.code = &code;
			  params.result = &result;
			  boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
			  if(!gpg_thread.timed_join(boost::posix_time::seconds(10)))
			  {
				  gpg_thread.~thread();
				  TerminateProcess(params.hProcess, 1);
				  params.hProcess = NULL;
				  debuglog<<time_str()<<": GPG execution timed out, aborted\n";
				  break;
			  }
			  if(result == pxNotFound)
				  break;
			  string::size_type s = 0;
			  while((s = out.find("\r", s)) != string::npos)
			  {
				  out.erase(s, 1);
			  }
			  std::string::size_type p1 = 0, p2 = 0;
			  p1 = out.find("-----BEGIN PGP PUBLIC KEY BLOCK-----");
			  if(p1 != std::string::npos)
			  {
				  p2 = out.find("-----END PGP PUBLIC KEY BLOCK-----", p1);
				  if(p2 != std::string::npos)
				  {
					  p2 += strlen("-----END PGP PUBLIC KEY BLOCK-----");
					  out = out.substr(p1, p2-p1);
					  TCHAR *tmp = mir_a2t(out.c_str());
					  SetWindowText(hPubKeyEdit, tmp);
					  mir_free(tmp);
				  }
				  else
					  MessageBox(NULL, TranslateT("Failed to export public key."), TranslateT("Error"), MB_OK);
			  }
			  else
				  MessageBox(NULL, TranslateT("Failed to export public key."), TranslateT("Error"), MB_OK);
//			  SetDlgItemText(hPubKeyEdit, IDC_PUBLIC_KEY_EDIT, tmp);
		  }
		  DestroyWindow(hwndDlg);
		  break;
	  case IDCANCEL:
		  DestroyWindow(hwndDlg);
		  break;
      }
      break;
    }
    
  case WM_NOTIFY:
    {
		if(hdr && IsWindowVisible(hdr->hdr.hwndFrom) && hdr->iItem != (-1))
		{
			if(hdr->hdr.code == NM_CLICK)
			{				
				EnableWindow(GetDlgItem(hwndDlg, IDOK), 1);
				itemnum2 = hdr->iItem;
			}
		}

      switch (((LPNMHDR)lParam)->code)
      {
        
      case PSN_APPLY:
        {
          return TRUE;
        }
      }
	}
	break;
	case WM_CLOSE:
		DestroyWindow(hwndDlg);
		break;
	case WM_DESTROY:
		{
			GetWindowRect(hwndDlg, &load_existing_key_rect);
			DBWriteContactSettingDword(NULL, szGPGModuleName, "LoadExistingKeyWindowX", load_existing_key_rect.left);
			DBWriteContactSettingDword(NULL, szGPGModuleName, "LoadExistingKeyWindowY", load_existing_key_rect.top);
		}
		hwndSelectExistingKey = NULL;
		break;

  }

  return FALSE;
}
static BOOL CALLBACK DlgProcImportKeyDialog(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  extern HANDLE new_key_hcnt;
  extern boost::mutex new_key_hcnt_mutex;
  HANDLE hContact = INVALID_HANDLE_VALUE;
  switch (msg)
  {
  case WM_INITDIALOG:
    {
		hContact = new_key_hcnt;
		new_key_hcnt_mutex.unlock();
		SetWindowPos(hwndDlg, 0 , import_key_rect.left, import_key_rect.top, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
		TranslateDialogDefault(hwndDlg);
		ComboBoxAddStringUtf(GetDlgItem(hwndDlg, IDC_KEYSERVER), _T("subkeys.pgp.net"), 0);
		ComboBoxAddStringUtf(GetDlgItem(hwndDlg, IDC_KEYSERVER), _T("keys.gnupg.net"), 0);
      return TRUE;
    }
    
 
  case WM_COMMAND:
    {
      switch (LOWORD(wParam))
      {
	  case IDC_IMPORT:
		  {
			  string out;
			  DWORD code;
			  wstring cmd = _T(" --keyserver \"");
			  TCHAR *server= new TCHAR [128];
			  GetDlgItemText(hwndDlg, IDC_KEYSERVER, server, 128);
			  cmd += server;
			  delete [] server;
			  cmd += _T("\" --recv-keys ");
//			  char *tmp = UniGetContactSettingUtf(hContact, szGPGModuleName, "KeyID_Prescense", "");
//			  TCHAR *tmp2 = mir_a2t(tmp);
//			  mir_free(tmp);
			  cmd += toUTF16(hcontact_data[hContact].key_in_prescense);
//			  mir_free(tmp2);
			  gpg_execution_params params;
			  pxResult result;
			  params.cmd = &cmd;
			  params.useless = "";
			  params.out = &out;
			  params.code = &code;
			  params.result = &result;
			  boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
			  if(!gpg_thread.timed_join(boost::posix_time::seconds(10)))
			  {
				  gpg_thread.~thread();
				  TerminateProcess(params.hProcess, 1);
				  params.hProcess = NULL;
				  debuglog<<time_str()<<": GPG execution timed out, aborted\n";
			  }
			  MessageBoxA(0, out.c_str(), "GPG output", MB_OK);
		  }
		  break;
	  default:
		break;
      }
      break;
    }
    
  case WM_NOTIFY:
    {
/*      switch (((LPNMHDR)lParam)->code)
      {
	  default:
		  break;
      } */
	}
    break;
  case WM_CLOSE:
	  DestroyWindow(hwndDlg);
	  break;
  case WM_DESTROY:
	  {
		  GetWindowRect(hwndDlg, &import_key_rect);
		  DBWriteContactSettingDword(NULL, szGPGModuleName, "ImportKeyWindowX", import_key_rect.left);
		  DBWriteContactSettingDword(NULL, szGPGModuleName, "ImportKeyWindowY", import_key_rect.top);
	  }
	  break;

  }
  return FALSE;
}


extern HINSTANCE hInst;


void ShowFirstRunDialog()
{
	if (hwndFirstRun == NULL)
	{
		hwndFirstRun = CreateDialog(hInst, MAKEINTRESOURCE(IDD_FIRST_RUN), NULL, (DLGPROC)DlgProcFirstRun);
	}
	SetForegroundWindow(hwndFirstRun);
}


void ShowSetDirsDialog()
{
	if (hwndSetDirs == NULL)
	{
		hwndSetDirs = CreateDialog(hInst, MAKEINTRESOURCE(IDD_BIN_PATH), NULL, (DLGPROC)DlgProcGpgBinOpts);
	}
	SetForegroundWindow(hwndSetDirs);
}

void ShowNewKeyDialog()
{
	hwndNewKey = CreateDialog(hInst, MAKEINTRESOURCE(IDD_NEW_KEY), NULL, (DLGPROC)DlgProcNewKeyDialog);
	SetForegroundWindow(hwndNewKey);
}

void ShowKeyGenDialog()
{
	if (hwndKeyGen == NULL)
	{
		hwndKeyGen = CreateDialog(hInst, MAKEINTRESOURCE(IDD_KEY_GEN), NULL, (DLGPROC)DlgProcKeyGenDialog);
	}
	SetForegroundWindow(hwndKeyGen);
}

void ShowSelectExistingKeyDialog()
{
	if (hwndSelectExistingKey == NULL)
	{
		hwndSelectExistingKey = CreateDialog(hInst, MAKEINTRESOURCE(IDD_LOAD_EXISTING_KEY), NULL, (DLGPROC)DlgProcLoadExistingKey);
	}
	SetForegroundWindow(hwndSelectExistingKey);
}

void ShowImportKeyDialog()
{
	CreateDialog(hInst, MAKEINTRESOURCE(IDD_IMPORT_KEY), NULL, (DLGPROC)DlgProcImportKeyDialog);
}




void FirstRun()
{
	DWORD pid = 0;
	if(!DBGetContactSettingByte(NULL, szGPGModuleName, "FirstRun", 1))
		return;
	ShowSetDirsDialog();
}

void InitCheck()
{
	{//parse gpg output
		gpg_configured = isGPGConfigured();
		if(!gpg_configured)
			return;
		string out;
		DWORD code;
		pxResult result;
		wstring::size_type p = 0, p2 = 0, stop = 0;
		{
			gpg_execution_params params;
			wstring cmd = _T("--batch --list-secret-keys");
			params.cmd = &cmd;
			params.useless = "";
			params.out = &out;
			params.code = &code;
			params.result = &result;
			boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
			if(!gpg_thread.timed_join(boost::posix_time::seconds(10)))
			{
				gpg_thread.~thread();
				TerminateProcess(params.hProcess, 1);
				params.hProcess = NULL;
				debuglog<<time_str()<<": GPG execution timed out, aborted\n";
				return;
			}
			if(result == pxNotFound)
				return;
		}
		TCHAR *home_dir = UniGetContactSettingUtf(NULL, szGPGModuleName, "szHomePath", _T(""));
		wstring tmp_dir = home_dir;
		mir_free(home_dir);
		tmp_dir += _T("\\tmp");
		_wmkdir(tmp_dir.c_str());
		string question = Translate("Your secret key whith id: ");
		char *keyid = UniGetContactSettingUtf(NULL, szGPGModuleName, "KeyID", "");
		question += keyid;
		mir_free(keyid);
		question += Translate(" deleted from gpg secret keyring\nDo you want to set another key ?");
		void ShowFirstRunDialog();
		if(out.find(keyid) == string::npos)
			if(MessageBoxA(0, question.c_str(), Translate("Own secret key warning"), MB_YESNO) == IDYES)
				ShowFirstRunDialog();
	}
	{
		TCHAR *path = UniGetContactSettingUtf(NULL, szGPGModuleName, "szHomePath", _T(""));
		DWORD dwFileAttr = GetFileAttributes(path);
		if (dwFileAttr != INVALID_FILE_ATTRIBUTES)
		{
			dwFileAttr &=~ FILE_ATTRIBUTE_READONLY;
			SetFileAttributes(path, dwFileAttr);
		}
		mir_free(path);
	}
	extern bool bAutoExchange;
	if(bAutoExchange && (ServiceExists("ICQ"PS_ICQ_ADDCAPABILITY))) //work only for one icq instance
	{
		ICQ_CUSTOMCAP cap;
		cap.cbSize = sizeof(ICQ_CUSTOMCAP);
		cap.hIcon = 0;
		strcpy(cap.name, "GPG Key AutoExchange");
		strcpy(cap.caps, "GPG AutoExchange");
		CallService("ICQ"PS_ICQ_ADDCAPABILITY, 0, (LPARAM)&cap);
	}
}

void ImportKey()
{
	extern wstring new_key;
	extern HANDLE new_key_hcnt;
	extern boost::mutex new_key_hcnt_mutex;
	HANDLE hContact = new_key_hcnt;
	new_key_hcnt_mutex.unlock();
	bool for_all_sub = false;
	if(metaIsProtoMetaContacts(hContact))
		if(MessageBox(0, TranslateT("Do you want load key for all subcontacts ?"), TranslateT("Metacontact detected"), MB_YESNO) == IDYES)
			for_all_sub = true;
	if(metaIsProtoMetaContacts(hContact))
	{
		HANDLE hcnt = NULL;
		if(for_all_sub)
		{
			int count = metaGetContactsNum(hContact);
			for(int i = 0; i < count; i++)
			{
				hcnt = metaGetSubcontact(hContact, i);
				if(hcnt)
					DBWriteContactSettingTString(hcnt, szGPGModuleName, "GPGPubKey", new_key.c_str());
			}
		}
		else
			DBWriteContactSettingTString(metaGetMostOnline(hContact), szGPGModuleName, "GPGPubKey", new_key.c_str());
	}
	else
		DBWriteContactSettingTString(hContact, szGPGModuleName, "GPGPubKey", new_key.c_str());
	new_key.clear();
	{ //gpg execute block
		wstring cmd;
		TCHAR tmp2[MAX_PATH] = {0};
		TCHAR *ptmp;
		string output;
		DWORD exitcode;
		{
			ptmp = UniGetContactSettingUtf(NULL, szGPGModuleName, "szHomePath", _T(""));
			_tcscpy(tmp2, ptmp);
			mir_free(ptmp);
			_tcscat(tmp2, _T("\\"));
			_tcscat(tmp2, _T("temporary_exported.asc"));
			DeleteFile(tmp2);
			wfstream f(tmp2, std::ios::out);
			if(metaIsProtoMetaContacts(hContact))
				ptmp = UniGetContactSettingUtf(metaGetMostOnline(hContact), szGPGModuleName, "GPGPubKey", _T(""));
			else
				ptmp = UniGetContactSettingUtf(hContact, szGPGModuleName, "GPGPubKey", _T(""));
			wstring new_key = ptmp;
			mir_free(ptmp);
			f<<new_key.c_str();
			f.close();
			cmd += _T(" --batch ");
			cmd += _T(" --import \"");
			cmd += tmp2;
			cmd += _T("\"");
		}
		gpg_execution_params params;
		pxResult result;
		params.cmd = &cmd;
		params.useless = "";
		params.out = &output;
		params.code = &exitcode;
		params.result = &result;
		boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
		if(!gpg_thread.timed_join(boost::posix_time::seconds(10)))
		{
			gpg_thread.~thread();
			TerminateProcess(params.hProcess, 1);
			params.hProcess = NULL;
			debuglog<<time_str()<<": GPG execution timed out, aborted\n";
			return;
		}
		if(result == pxNotFound)
			return;
		{
			if(metaIsProtoMetaContacts(hContact))
			{
				HANDLE hcnt = NULL;
				if(for_all_sub)
				{
					int count = metaGetContactsNum(hContact);
					for(int i = 0; i < count; i++)
					{
						hcnt = metaGetSubcontact(hContact, i);
						if(hcnt)
						{
							char *tmp = NULL;
							string::size_type s = output.find("gpg: key ") + strlen("gpg: key ");
							string::size_type s2 = output.find(":", s);
							DBWriteContactSettingString(hcnt, szGPGModuleName, "KeyID", output.substr(s,s2-s).c_str());
							s2+=2;
							s = output.find("“", s2);
							if(s == string::npos)
							{
								s = output.find("\"", s2);
								s += 1;
							}
							else
								s += 3;
							if((s2 = output.find("(", s)) == string::npos)
								s2 = output.find("<", s);
							else if(s2 > output.find("<", s))
								s2 = output.find("<", s);
							tmp = new char [output.substr(s,s2-s-1).length()+1];
							strcpy(tmp, output.substr(s,s2-s-1).c_str());
							mir_utf8decode(tmp, 0);
							DBWriteContactSettingString(hcnt, szGPGModuleName, "KeyMainName", tmp);
							mir_free(tmp);
							if((s = output.find(")", s2)) == string::npos)
								s = output.find(">", s2);
							else if(s > output.find(">", s2))
								s = output.find(">", s2);
							s2++;
							if(output[s] == ')')
							{
								tmp = new char [output.substr(s2,s-s2).length()+1];
								strcpy(tmp, output.substr(s2,s-s2).c_str());
								mir_utf8decode(tmp, 0);
								DBWriteContactSettingString(hcnt, szGPGModuleName, "KeyComment", tmp);
								mir_free(tmp);
								s+=3;
								s2 = output.find(">", s);
								tmp = new char [output.substr(s,s2-s).length()+1];
								strcpy(tmp, output.substr(s,s2-s).c_str());
								mir_utf8decode(tmp, 0);
								DBWriteContactSettingString(hcnt, szGPGModuleName, "KeyMainEmail", tmp);
								mir_free(tmp);
							}
							else
							{
								tmp = new char [output.substr(s2,s-s2).length()+1];
								strcpy(tmp, output.substr(s2,s-s2).c_str());
								mir_utf8decode(tmp, 0);
								DBWriteContactSettingString(hcnt, szGPGModuleName, "KeyMainEmail", output.substr(s2,s-s2).c_str());
								mir_free(tmp);
							}
							DBDeleteContactSetting(hcnt, szGPGModuleName, "bAlwatsTrust");
						}
					}
				}
				else
				{
					char *tmp = NULL;
					string::size_type s = output.find("gpg: key ") + strlen("gpg: key ");
					string::size_type s2 = output.find(":", s);
					DBWriteContactSettingString(metaGetMostOnline(hContact), szGPGModuleName, "KeyID", output.substr(s,s2-s).c_str());
					s2+=2;
					s = output.find("“", s2);
					if(s == string::npos)
					{
						s = output.find("\"", s2);
						s += 1;
					}
					else
						s += 3;
					if((s2 = output.find("(", s)) == string::npos)
						s2 = output.find("<", s);
					else if(s2 > output.find("<", s))
						s2 = output.find("<", s);
					tmp = new char [output.substr(s,s2-s-1).length()+1];
					strcpy(tmp, output.substr(s,s2-s-1).c_str());
					mir_utf8decode(tmp, 0);
					DBWriteContactSettingString(metaGetMostOnline(hContact), szGPGModuleName, "KeyMainName", tmp);
					mir_free(tmp);
					if((s = output.find(")", s2)) == string::npos)
						s = output.find(">", s2);
					else if(s > output.find(">", s2))
						s = output.find(">", s2);
					s2++;
					if(output[s] == ')')
					{
						tmp = new char [output.substr(s2,s-s2).length()+1];
						strcpy(tmp, output.substr(s2,s-s2).c_str());
						mir_utf8decode(tmp, 0);
						DBWriteContactSettingString(metaGetMostOnline(hContact), szGPGModuleName, "KeyComment", tmp);
						mir_free(tmp);
						s+=3;
						s2 = output.find(">", s);
						tmp = new char [output.substr(s,s2-s).length()+1];
						strcpy(tmp, output.substr(s,s2-s).c_str());
						mir_utf8decode(tmp, 0);
						DBWriteContactSettingString(metaGetMostOnline(hContact), szGPGModuleName, "KeyMainEmail", tmp);
						mir_free(tmp);
					}
					else
					{
						tmp = new char [output.substr(s2,s-s2).length()+1];
						strcpy(tmp, output.substr(s2,s-s2).c_str());
						mir_utf8decode(tmp, 0);
						DBWriteContactSettingString(metaGetMostOnline(hContact), szGPGModuleName, "KeyMainEmail", output.substr(s2,s-s2).c_str());
						mir_free(tmp);
					}
					DBDeleteContactSetting(metaGetMostOnline(hContact), szGPGModuleName, "bAlwatsTrust");
				}
			}
			else
			{
				char *tmp = NULL;
				string::size_type s = output.find("gpg: key ") + strlen("gpg: key ");
				string::size_type s2 = output.find(":", s);
				DBWriteContactSettingString(hContact, szGPGModuleName, "KeyID", output.substr(s,s2-s).c_str());
				s2+=2;
				s = output.find("“", s2);
				if(s == string::npos)
				{
					s = output.find("\"", s2);
					s += 1;
				}
				else
					s += 3;
				if((s2 = output.find("(", s)) == string::npos)
					s2 = output.find("<", s);
				else if(s2 > output.find("<", s))
					s2 = output.find("<", s);
				tmp = new char [output.substr(s,s2-s-1).length()+1];
				strcpy(tmp, output.substr(s,s2-s-1).c_str());
				mir_utf8decode(tmp, 0);
				DBWriteContactSettingString(hContact, szGPGModuleName, "KeyMainName", tmp);
				mir_free(tmp);
				if((s = output.find(")", s2)) == string::npos)
					s = output.find(">", s2);
				else if(s > output.find(">", s2))
					s = output.find(">", s2);
				s2++;
				if(output[s] == ')')
				{
					tmp = new char [output.substr(s2,s-s2).length()+1];
					strcpy(tmp, output.substr(s2,s-s2).c_str());
					mir_utf8decode(tmp, 0);
					DBWriteContactSettingString(hContact, szGPGModuleName, "KeyComment", tmp);
					mir_free(tmp);
					s+=3;
					s2 = output.find(">", s);
					tmp = new char [output.substr(s,s2-s).length()+1];
					strcpy(tmp, output.substr(s,s2-s).c_str());
					mir_utf8decode(tmp, 0);
					DBWriteContactSettingString(hContact, szGPGModuleName, "KeyMainEmail", tmp);
					mir_free(tmp);
				}
				else
				{
					tmp = new char [output.substr(s2,s-s2).length()+1];
					strcpy(tmp, output.substr(s2,s-s2).c_str());
					mir_utf8decode(tmp, 0);
					DBWriteContactSettingString(hContact, szGPGModuleName, "KeyMainEmail", output.substr(s2,s-s2).c_str());
					mir_free(tmp);
				}
				DBDeleteContactSetting(hContact, szGPGModuleName, "bAlwatsTrust");
			}
		}
		ptmp = mir_wstrdup(toUTF16(output).c_str());
		MessageBox(0, ptmp, _T(""), MB_OK);
		mir_free(ptmp);
		DeleteFile(tmp2);
	}
}