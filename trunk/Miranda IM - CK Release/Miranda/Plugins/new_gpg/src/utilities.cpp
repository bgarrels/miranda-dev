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


TCHAR* __stdcall UniGetContactSettingUtf(HANDLE hContact, const char *szModule,const char* szSetting, TCHAR* szDef)
{
  DBVARIANT dbv = {DBVT_DELETED};
  TCHAR* szRes;
  if (DBGetContactSettingTString(hContact, szModule, szSetting, &dbv))
	 return _tcsdup(szDef);
  if(dbv.pszVal)
	  szRes = _tcsdup(dbv.ptszVal);
  DBFreeVariant(&dbv);
  return szRes;
}

char* __stdcall UniGetContactSettingUtf(HANDLE hContact, const char *szModule,const char* szSetting, char* szDef)
{
  DBVARIANT dbv = {DBVT_DELETED};
  char* szRes;
  if (DBGetContactSettingString(hContact, szModule, szSetting, &dbv))
	 return _strdup(szDef);
  if(dbv.pszVal)
	  szRes = _strdup(dbv.pszVal);
  DBFreeVariant(&dbv);
  return szRes;
}


char *date()
{
	setlocale( LC_ALL, "C" );
	static char d[11];	
	char *tmp = __DATE__, m[4], mn[3] = "01";
	m[0]=tmp[0];
	m[1]=tmp[1];
	m[2]=tmp[2];
	if(strstr(m,"Jan"))
		strcpy(mn,"01");
	else if(strstr(m,"Feb"))
		strcpy(mn,"02");
	else if(strstr(m,"Mar"))
		strcpy(mn,"03");
	else if(strstr(m,"Apr"))
		strcpy(mn,"04");
	else if(strstr(m,"May"))
		strcpy(mn,"05");
	else if(strstr(m,"Jun"))
		strcpy(mn,"06");
	else if(strstr(m,"Jul"))
		strcpy(mn,"07");
	else if(strstr(m,"Aug"))
		strcpy(mn,"08");
	else if(strstr(m,"Sep"))
		strcpy(mn,"09");
	else if(strstr(m,"Oct"))
		strcpy(mn,"10");
	else if(strstr(m,"Nov"))
		strcpy(mn,"11");
	else if(strstr(m,"Dec"))
		strcpy(mn,"12");
	d[0]=tmp[7];
	d[1]=tmp[8];
	d[2]=tmp[9];
	d[3]=tmp[10];
	d[4]='.';
	d[5]=mn[0];
	d[6]=mn[1];
	d[7]='.';
	if (tmp[4] == ' ')
		d[8] = '0';
	else
		d[8]=tmp[4];
	d[9]=tmp[5];
	return d;
}

void GetFilePath(TCHAR *WindowTittle, char *szSetting, TCHAR *szExt, TCHAR *szExtDesc)
{
	TCHAR str[MAX_PATH+2] = {0}, *tmp;
	OPENFILENAME ofn={0};
	TCHAR filter[512], *pfilter;
	ofn.lStructSize=CDSIZEOF_STRUCT(OPENFILENAME,lpTemplateName);
	ofn.Flags=OFN_EXPLORER;
	ofn.lpstrTitle=TranslateW(WindowTittle);
	_tcscpy(filter,TranslateW(szExtDesc));
	pfilter=filter+_tcslen(filter)+1;
	_tcscpy(pfilter, szExt);
	pfilter[_tcslen(pfilter)+1] = '\0';
	pfilter[_tcslen(pfilter)+2] = '\0';
	ofn.lpstrFilter=filter;
	tmp = UniGetContactSettingUtf(0, szGPGModuleName, szSetting, _T(""));
	_tcscpy(str, tmp);
	mir_free(tmp);
	if(_tcslen(str)< 2)
		str[0] = '\0';
	ofn.lpstrFile=str;
	ofn.nMaxFile=_MAX_PATH;
	ofn.nMaxFileTitle=MAX_PATH;
	if(!GetOpenFileName(&ofn)) 
		return;
	DBWriteContactSettingTString(0, szGPGModuleName, szSetting, str);
}

TCHAR *GetFilePath(TCHAR *WindowTittle, TCHAR *szExt, TCHAR *szExtDesc, bool save_file)
{
	TCHAR *str = new TCHAR [MAX_PATH+2];
	OPENFILENAME ofn={0};
	TCHAR filter[512], *pfilter;
	ofn.lStructSize=CDSIZEOF_STRUCT(OPENFILENAME,lpTemplateName);
	ofn.Flags=OFN_EXPLORER;
	ofn.lpstrTitle=TranslateW(WindowTittle);
	_tcscpy(filter,TranslateW(szExtDesc));
	pfilter=filter+_tcslen(filter)+1;
	_tcscpy(pfilter, szExt);
	pfilter[_tcslen(pfilter)+1] = '\0';
	pfilter[_tcslen(pfilter)+2] = '\0';
	ofn.lpstrFilter=filter;
	_tcscpy(str, _T(""));
	if(_tcslen(str)< 2)
		str[0] = '\0';
	ofn.lpstrFile=str;
	ofn.nMaxFile=_MAX_PATH;
	ofn.nMaxFileTitle=MAX_PATH;
	if(!save_file)
	{
		if(!GetOpenFileName(&ofn)) 
		{
			delete [] str;
			return NULL;
		}
	}
	else
	{
		if(!GetSaveFileName(&ofn)) 
		{
			delete [] str;
			return NULL;
		}
	}
	return str;
}

void GetFolderPath(TCHAR *WindowTittle, char *szSetting)
{
	BROWSEINFO pbi = {0};
	pbi.lpszTitle = WindowTittle;
	pbi.ulFlags = BIF_EDITBOX|BIF_NEWDIALOGSTYLE|BIF_SHAREABLE;
	LPITEMIDLIST pidl = SHBrowseForFolder(&pbi);
	if (pidl != 0)
	{
		TCHAR path[MAX_PATH];
		if (SHGetPathFromIDList(pidl, path))
		{
			DBWriteContactSettingTString(NULL, szGPGModuleName, "szHomePath", path);
		}
		IMalloc * imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(pidl);
			imalloc->Release();
		}
	}
}

int LoadKey(WPARAM w, LPARAM l)
{
	void ShowLoadPublicKeyDialog();
	extern map<int, HANDLE> user_data;
	extern int item_num;
	item_num = 0;		 //black magic here
	user_data[1] = (HANDLE)w;
	ShowLoadPublicKeyDialog();
	return 0;
}

int SendKey(WPARAM w, LPARAM l)
{
	HANDLE hContact = (HANDLE)w;
	if(metaIsProtoMetaContacts(hContact))
		hContact = metaGetMostOnline(hContact);
	char *szMessage = UniGetContactSettingUtf(NULL, szGPGModuleName, "GPGPubKey", "");
	if(strlen(szMessage) > 1)
	{
		BYTE enc = DBGetContactSettingByte(hContact, szGPGModuleName, "GPGEncryption", 0);
		DBWriteContactSettingByte(hContact, szGPGModuleName, "GPGEncryption", 0);
		CallContactService(hContact, PSS_MESSAGE, (WPARAM)PREF_UTF, (LPARAM)szMessage);
		HistoryLog(hContact, db_event("Public key sent", 0, 0, DBEF_SENT));
		DBWriteContactSettingByte(hContact, szGPGModuleName, "GPGEncryption", enc);
	}
	mir_free(szMessage);
	return 0;
}

extern HANDLE hLoadPublicKey, hToggleEncryption;

int ToggleEncryption(WPARAM w, LPARAM l)
{
	HANDLE hContact = (HANDLE)w;
	BYTE enc = 0;
	if(metaIsProtoMetaContacts(hContact))
		enc = DBGetContactSettingByte(metaGetMostOnline(hContact), szGPGModuleName, "GPGEncryption", 0);
	else
		enc = DBGetContactSettingByte(hContact, szGPGModuleName, "GPGEncryption", 0);
	if(metaIsProtoMetaContacts(hContact))
	{
		HANDLE hcnt = NULL;
		if(MessageBox(0, _T("Do you want to toggle encryption for all subcontacts ?"), _T("Metacontact detected"), MB_YESNO) == IDYES)
		{
			int count = metaGetContactsNum(hContact);
			for(int i = 0; i < count; i++)
			{
				hcnt = metaGetSubcontact(hContact, i);
				if(hcnt)
					DBWriteContactSettingByte(hcnt, szGPGModuleName, "GPGEncryption", enc?0:1);
			}
			DBWriteContactSettingByte(hContact, szGPGModuleName, "GPGEncryption", enc?0:1);
		}
	}
	else
		DBWriteContactSettingByte(metaGetMostOnline(hContact), szGPGModuleName, "GPGEncryption", enc?0:1);
	void setSrmmIcon(HANDLE hContact);
	void setClistIcon(HANDLE hContact);
	setSrmmIcon(hContact);
	setClistIcon(hContact);
	enc = enc?0:1;
	CLISTMENUITEM mi = {0};
	mi.cbSize=sizeof(mi);
	mi.flags = CMIM_NAME;
	enc?mi.pszName="Turn off GPG encryption":mi.pszName="Turn on GPG encryption";
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hToggleEncryption, (LPARAM)&mi);
	return 0;
}

int OnPreBuildContactMenu(WPARAM w, LPARAM l)
{
	HANDLE hContact = (HANDLE)w;
	if(metaIsProtoMetaContacts(hContact))
		hContact = metaGetMostOnline(hContact);
	CLISTMENUITEM mi = {0};
	mi.cbSize=sizeof(mi);
	mi.flags = CMIM_NAME;
	TCHAR *tmp = UniGetContactSettingUtf(hContact, szGPGModuleName, "GPGPubKey", _T(""));
	if(_tcslen(tmp) < 1)
	{
		DBDeleteContactSetting(hContact, szGPGModuleName, "GPGEncryption");
		mi.flags += CMIM_FLAGS | CMIF_GRAYED;
	}
	else
		mi.flags = CMIM_NAME | CMIM_FLAGS;
	mi.pszName = DBGetContactSettingByte(hContact, szGPGModuleName, "GPGEncryption", 0)?"Turn off GPG encryption":"Turn on GPG encryption";
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)hToggleEncryption, (LPARAM)&mi);
	return 0;
}


list<wstring> transfers;
extern bool bFileTransfers;

int onProtoAck(WPARAM w, LPARAM l)
{
	ACKDATA *ack=(ACKDATA*)l;
	CCSDATA *ccs=(CCSDATA*)ack->lParam;
		
	if(ack->type == ACKTYPE_FILE && bFileTransfers)
	{
		switch(ack->result)
		{
		case ACKRESULT_DENIED:	case ACKRESULT_FAILED: 
			break;
		case ACKRESULT_SUCCESS:
			{
				PROTOFILETRANSFERSTATUS *f = (PROTOFILETRANSFERSTATUS*) ack->hProcess;
				TCHAR *filename = NULL;
				if(f->flags & PFTS_UNICODE)
				{
					if(f->tszCurrentFile && f->tszCurrentFile[0])
						filename = mir_wstrdup(f->tszCurrentFile);
					if(!filename)
						return 0;
				}
				else
				{
					if(f->szCurrentFile && f->szCurrentFile[0])
						filename = mir_utf8decodeT(f->szCurrentFile);
					if(!filename)
						return 0;
				}
				if(_tcsstr(filename, _T(".gpg"))) //decrypt it
				{ //process encrypted file
					if(_waccess(f->tszCurrentFile, 0) == -1)
					{
						if(errno == ENOENT)
							return 0;
					}
					string out;
					DWORD code;
					pxResult result;
					wstring cmd = _T("-o ");
					wstring file = filename;
					wstring::size_type p1 = file.rfind(_T(".gpg"));
					file.erase(p1, _tcslen(_T(".gpg")));
					if(_waccess(file.c_str(), 0) != -1)
					{
						if(MessageBox(0, _T("Target file exists, do you want to replace it ?"), _T("Warning"), MB_YESNO) == IDNO)
							return 0;
					}
					DeleteFile(file.c_str());
					file.insert(0, _T("\""));
					file.insert(file.length(), _T("\" "));
					cmd += file;
					cmd += _T(" -d \"");
					cmd += filename;
					cmd += _T("\"");
					extern TCHAR *password;
					{ // password
						TCHAR *pass = NULL;
						char *keyid = UniGetContactSettingUtf(ccs->hContact, szGPGModuleName, "KeyID", "");
						if(strlen(keyid) > 0)
						{
							string dbsetting = "szKey_";
							dbsetting += keyid;
							dbsetting += "_Password";
							pass = UniGetContactSettingUtf(NULL, szGPGModuleName, dbsetting.c_str(), _T(""));
							if(_tcslen(pass) > 0)
								debuglog<<time_str()<<": info: found password in database for key id: "<<keyid<<", trying to decrypt message from "<<(TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)ccs->hContact, GCDNF_TCHAR)<<" with password\n";
						}
						else
						{
							pass = UniGetContactSettingUtf(NULL, szGPGModuleName, "szKeyPassword", _T(""));
							if(_tcslen(pass) > 0)
								debuglog<<time_str()<<": info: found password for all keys in database, trying to decrypt message from "<<(TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)ccs->hContact, GCDNF_TCHAR)<<" with password\n";
						}
						if(_tcslen(pass) > 0)
						{
							cmd += _T("--passphrase \"");
							cmd += pass;
							cmd += _T("\" ");
						}
						else if(password)
						{
							debuglog<<time_str()<<": info: found password in memory, trying to decrypt message from "<<(TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)ccs->hContact, GCDNF_TCHAR)<<" with password\n";
							cmd += _T("--passphrase \"");
							cmd += password;
							cmd += _T("\" ");
						}
						else
							debuglog<<time_str()<<": info: passwords not found in database or memory, trying to decrypt message from "<<(TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)ccs->hContact, GCDNF_TCHAR)<<" with out password\n";
						mir_free(pass);
						mir_free(keyid);
					}
					gpg_execution_params params;
					params.cmd = &cmd;
					params.useless = "";
					params.out = &out;
					params.code = &code;
					params.result = &result;
					boost::thread *gpg_thread = new boost::thread(boost::bind(&pxEexcute_thread, &params));
					if(!gpg_thread->timed_join(boost::posix_time::minutes(15)))
					{
						delete gpg_thread;
						TerminateProcess(params.hProcess, 1);
						params.hProcess = NULL;
						debuglog<<time_str()<<": GPG execution timed out, aborted\n";
						return 0;
					}
					while(out.find("public key decryption failed: bad passphrase") != string::npos)
					{
						extern bool _terminate;
						extern HANDLE new_key_hcnt;
						extern boost::mutex new_key_hcnt_mutex;
						debuglog<<time_str()<<": info: failed to decrypt messaage from "<<(TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)ccs->hContact, GCDNF_TCHAR)<<" password needed, trying to get one\n";
						if(_terminate)
							break;
						{ //save inkey id
							string::size_type s = out.find(" encrypted with ");
							s = out.find(" ID ", s);
							s += strlen(" ID ");
							string::size_type s2 = out.find(",",s);
							DBWriteContactSettingString(metaGetMostOnline(ccs->hContact), szGPGModuleName, "InKeyID", out.substr(s, s2-s).c_str());
						}
						void ShowLoadKeyPasswordWindow();
						new_key_hcnt_mutex.lock();
						new_key_hcnt = ccs->hContact;
						ShowLoadKeyPasswordWindow();
						wstring cmd2 = cmd;
						if(password)
						{
							debuglog<<time_str()<<": info: found password in memory, trying to decrypt message from "<<(TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)ccs->hContact, GCDNF_TCHAR)<<"\n";
							wstring tmp = _T("--passphrase \"");
							tmp += password;
							tmp += _T("\" ");
							cmd2.insert(0, tmp);
						}
						out.clear();
						gpg_execution_params params;
						pxResult result;
						params.cmd = &cmd2;
						params.useless = "";
						params.out = &out;
						params.code = &code;
						params.result = &result;
						gpg_thread = gpg_thread = new boost::thread(boost::bind(&pxEexcute_thread, &params));
						if(!gpg_thread->timed_join(boost::posix_time::seconds(15)))
						{
							delete gpg_thread;
							TerminateProcess(params.hProcess, 1);
							params.hProcess = NULL;
							debuglog<<time_str()<<": GPG execution timed out, aborted\n";
							DeleteFile(filename);
							return 0;
						}
						if(result == pxNotFound)
						{
							DeleteFile(filename);
							return 0;
						}
					}
					DeleteFile(filename);
					mir_free(filename);
			}
		}
		break;
	}
	}
	else if(ack->type == ACKTYPE_MESSAGE)
	{
		extern std::list<HANDLE> sent_msgs;
		if(!sent_msgs.empty())
		{
			if(ack->result == ACKRESULT_FAILED)
			{
				std::list<HANDLE>::iterator it = std::find(sent_msgs.begin(), sent_msgs.end(), ack->hProcess);
				if(it != sent_msgs.end())
				{
					HistoryLog(ack->hContact, db_event("Failed to send encrypted message", 0,0, 0));
					
				}
			}
			else if(ack->result == ACKRESULT_SUCCESS)
			{
				std::list<HANDLE>::iterator it = std::find(sent_msgs.begin(), sent_msgs.end(), ack->hProcess);
				if(it != sent_msgs.end())
					sent_msgs.erase(it);
			}
		}
	}
	return 0;
}

std::wstring encrypt_file(HANDLE hContact, TCHAR *filename)
{
	string out;
	DWORD code;
	pxResult result;
	wstring cmd = _T("--batch --yes -r ");
	char *keyid = UniGetContactSettingUtf(hContact, szGPGModuleName, "KeyID", "");
	if(DBGetContactSettingByte(hContact, szGPGModuleName, "bAlwaysTrust", 0))
		cmd += _T("--trust-model always ");
	TCHAR *szKeyid = mir_a2t(keyid);
	TCHAR *name = _tcsrchr(filename,_T('\\'));
	if( !name )
		name = filename;
	else
		name++;
	TCHAR *file_out =  new TCHAR [_tcslen(filename)+4];
	mir_sntprintf(file_out, _tcslen(name)+7, _T("%s.gpg"), name);
	cmd += szKeyid;
	mir_free(szKeyid);
	mir_free(keyid);
	cmd += _T(" -o \"");
	TCHAR *temp = _tgetenv(_T("TEMP"));
	cmd += temp;
	cmd += _T("\\");
	cmd += file_out;
	wstring path_out = temp;
	path_out += _T("\\");
	path_out += file_out;
	DeleteFile(path_out.c_str());
	cmd += _T("\" ");
	mir_free(temp);
	cmd += _T(" -e \"");
	cmd += filename;
	cmd += _T("\" ");
	gpg_execution_params params;
	params.cmd = &cmd;
	params.useless = "";
	params.out = &out;
	params.code = &code;
	params.result = &result;
	mir_free(keyid);
	delete [] file_out;
	boost::thread *gpg_thread = new boost::thread(boost::bind(&pxEexcute_thread, &params));
	if(!gpg_thread->timed_join(boost::posix_time::seconds(180)))
	{
		delete gpg_thread;
		TerminateProcess(params.hProcess, 1);
		params.hProcess = NULL;
		debuglog<<time_str()<<": GPG execution timed out, aborted\n";
		return 0;
	}
	if(out.find("There is no assurance this key belongs to the named user") != string::npos)
	{
		out.clear();
		if(MessageBox(0, _T("We trying to encrypt with untrusted key, do you want to trust this key permanently ?"), _T("Warning"), MB_YESNO) == IDYES)
		{
			DBWriteContactSettingByte(hContact, szGPGModuleName, "bAlwaysTrust", 1);
			cmd.insert(0, _T("--trust-model always "));
			gpg_thread = new boost::thread(boost::bind(&pxEexcute_thread, &params));
			if(!gpg_thread->timed_join(boost::posix_time::seconds(180)))
			{
				delete gpg_thread;
				TerminateProcess(params.hProcess, 1);
				params.hProcess = NULL;
				debuglog<<time_str()<<": GPG execution timed out, aborted\n";
				return 0;
			}
		}
		else
			return 0;
	}
	return path_out;
}

//from secureim partially
int onSendFile(WPARAM w, LPARAM l)
{
	if(!bFileTransfers)
		return CallService(MS_PROTO_CHAINSEND, w, l);
	CCSDATA *ccs=(CCSDATA*)l;
	if(isContactSecured(ccs->hContact))
	{
		
		DWORD flags = (DWORD)ccs->wParam; //check for PFTS_UNICODE here
		int i;
		if(flags & PFTS_UNICODE)
		{
			TCHAR **file=(TCHAR **)ccs->lParam;
			for(i = 0; file[i]; i++)
			{
				if(_waccess(file[i], 0) == -1)
					if(errno == ENOENT)
						return 0; //we do not want to send file unencrypted (sometimes ack have wrong info)
				if (_tcsstr(file[i],_T(".gpg")))
					continue;
				std::wstring path_out = encrypt_file(ccs->hContact, file[i]);
				mir_free(file[i]);
				file[i] = mir_tstrdup(path_out.c_str());
				transfers.push_back(path_out);
			}
		}
		else
		{
			char **file = (char**) ccs->lParam;
			for(i = 0; file[i]; i++)
			{
				if(_access(file[i], 0) == -1)
					if(errno == ENOENT)
						return 0; //we do not want to send file unencrypted (sometimes ack have wrong info)
				if (strstr(file[i],".gpg"))
					continue;
				TCHAR *tmp = mir_utf8decodeT(file[i]);
				std::wstring path_out = encrypt_file(ccs->hContact, tmp);
				mir_free(tmp);
				char* tmp2 = mir_utf8encodeW(path_out.c_str());
				mir_free(file[i]);
				file[i] = tmp2;
				transfers.push_back(path_out);

			}
		}
	}
	return CallService(MS_PROTO_CHAINSEND, w, l);
}

void storeOutput(HANDLE ahandle, string *output)
{
	BOOL success;
	char readbuffer[4096] = {0};
	unsigned long transfered, available;
	
	do {
		PeekNamedPipe(ahandle,NULL,0,NULL,&available,NULL);
		if (!available)
			continue;
		success=ReadFile(ahandle,readbuffer,sizeof(readbuffer),&transfered,NULL);
		if (success && transfered)
			output->append(readbuffer, 4096);
	} while (available>0);
}

void HistoryLog(HANDLE hContact, db_event evt)
{
	DBEVENTINFO Event = {0};
	Event.cbSize = sizeof(Event);
	Event.szModule = szGPGModuleName;
	Event.eventType = evt.eventType;
	Event.flags = evt.flags;
	if(!evt.timestamp)
		Event.timestamp = (DWORD)time(NULL);
	else
		Event.timestamp = evt.timestamp;
	Event.cbBlob = strlen((char*)evt.pBlob)+1;
	Event.pBlob = (PBYTE)_strdup((char*)evt.pBlob);
	CallService(MS_DB_EVENT_ADD, (WPARAM)(HANDLE)hContact,(LPARAM)&Event);
}

static int ControlAddStringUtf(HWND ctrl, DWORD msg, const TCHAR *szString)
{
	int item = -1;
	item = SendMessage(ctrl, msg, 0, (LPARAM)szString);
	return item;
}

int ComboBoxAddStringUtf(HWND hCombo, const TCHAR *szString, DWORD data)
{
	int item = ControlAddStringUtf(hCombo, CB_ADDSTRING, szString);
	SendMessage(hCombo, CB_SETITEMDATA, item, data);

	return item;
}


int GetJabberInterface(WPARAM w, LPARAM l) //get interface for all jabber accounts, options later
{
	extern list <JabberAccount*> Accounts;
	void AddHandlers();
	int count = 0;
	PROTOACCOUNT **accounts;
	ProtoEnumAccounts(&count, &accounts);
	list <JabberAccount*>::iterator p;
	Accounts.clear();
	Accounts.push_back(new JabberAccount);
	p = Accounts.begin();
	(*p)->setAccountNumber(0);
	for(int i = 0; i < count; i++) //get only jabber accounts from all accounts
	{
		IJabberInterface *JIftmp = getJabberApi(accounts[i]->szModuleName);
		int a = 0;
		if(JIftmp)
		{
			(*p)->setJabberInterface(JIftmp);
			if(accounts[i]->tszAccountName)
			{
				TCHAR* tmp = mir_tstrdup(accounts[i]->tszAccountName);
				(*p)->setAccountName(tmp);
			}
			else
			{
				TCHAR *tmp = mir_a2t(accounts[i]->szModuleName);
				(*p)->setAccountName(tmp);
			}
			(*p)->setAccountNumber(a);
			a++;
			Accounts.push_back(new JabberAccount);
			p++;
		}
	}
	Accounts.pop_back();
	AddHandlers();
	return 0;
}

static JABBER_HANDLER_FUNC SendHandler(IJabberInterface *ji, HXML node, void *pUserData)
{
	HXML local_node = node;
	for(int n = 0; n <= xi.getChildCount(node); n++)
	{
		LPCTSTR str = xi.getText(local_node); 
		LPCTSTR nodename = xi.getName(local_node);
		if(str)
		{
			if(_tcsstr(str, _T("-----BEGIN PGP MESSAGE-----")) && _tcsstr(str, _T("-----END PGP MESSAGE-----")))
			{
				wstring data = str;
				xi.setText(local_node, _T("This message is encrypted."));
				wstring::size_type p1 = data.find(_T("-----BEGIN PGP MESSAGE-----")) + _tcslen(_T("-----BEGIN PGP MESSAGE-----"));
				while(data.find(_T("Version: "), p1) != wstring::npos)
				{
					p1 = data.find(_T("Version: "), p1);
					p1 = data.find(_T("\n"), p1);
				}
				while(data.find(_T("Comment: "), p1) != wstring::npos)
				{
					p1 = data.find(_T("Comment: "), p1);
					p1 = data.find(_T("\n"), p1);
				}
				while(data.find(_T("Encoding: "), p1) != wstring::npos)
				{
					p1 = data.find(_T("Encoding: "), p1);
					p1 = data.find(_T("\n"), p1);
				}
				p1+=2;
				wstring::size_type p2 = data.find(_T("-----END PGP MESSAGE-----"));
				HXML encrypted_data = xi.addChild(node, _T("x"), data.substr(p1, p2-p1).c_str());
				xi.addAttr(encrypted_data, _T("xmlns"), _T("jabber:x:encrypted"));
				return FALSE;
			}
		}
		if(nodename)
		{
			if(_tcsstr(nodename, _T("status")))
			{
				TCHAR *path_c = UniGetContactSettingUtf(NULL, szGPGModuleName, "szHomePath", _T(""));
				wstring path_out = path_c;
				wstring file = toUTF16(get_random(10));
				mir_free(path_c);
				path_out += _T("\\tmp\\");
				path_out += file;
				DeleteFile(path_out.c_str());
				wfstream f(path_out.c_str(), std::ios::out);
				f<<toUTF8(str).c_str();
				f.close();
				if(_waccess(path_out.c_str(), 0) == -1)
				{
					if(errno == ENOENT)
					{
						debuglog<<time_str()<<": info: Failed to write prescense in file\n";
						return FALSE;
					}
				}
				{
					extern TCHAR *password;
					string out;
					DWORD code;
					wstring cmd;
					{
						char *inkeyid = UniGetContactSettingUtf(NULL, szGPGModuleName, "KeyID", "");
						TCHAR *pass = NULL;
						if(inkeyid[0])
						{
							string dbsetting = "szKey_";
							dbsetting += inkeyid;
							dbsetting += "_Password";
							pass = UniGetContactSettingUtf(NULL, szGPGModuleName, dbsetting.c_str(), _T(""));
							if(pass[0])
								debuglog<<time_str()<<": info: found password in database for key id: "<<inkeyid<<", trying to encrypt message from self with password\n";
						}
						else
						{
							pass = UniGetContactSettingUtf(NULL, szGPGModuleName, "szKeyPassword", _T(""));
							if(pass[0])
								debuglog<<time_str()<<": info: found password for all keys in database, trying to encrypt message from self with password\n";
						}
						if(pass[0])
						{
							cmd += _T("--passphrase \"");
							cmd += pass;
							cmd += _T("\" ");
						}
						else if(password)
						{
							debuglog<<time_str()<<": info: found password in memory, trying to encrypt message from self with password\n";
							cmd += _T("--passphrase \"");
							cmd += password;
							cmd += _T("\" ");
						}
						else
							debuglog<<time_str()<<": info: passwords not found in database or memory, trying to encrypt message from self with out password\n";
						mir_free(pass);
						mir_free(inkeyid);
					}
					cmd += _T("--local-user ");
					path_c = UniGetContactSettingUtf(NULL, szGPGModuleName, "KeyID", _T(""));
					cmd += path_c;
					cmd += _T(" --default-key ");
					cmd += path_c;
					mir_free(path_c);
					cmd += _T(" --batch --yes -a -s \"");
					cmd += path_out;
					cmd += _T("\" ");
					gpg_execution_params params;
					pxResult result;
					params.cmd = &cmd;
					params.useless = "";
					params.out = &out;
					params.code = &code;
					params.result = &result;
					boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
					if(!gpg_thread.timed_join(boost::posix_time::seconds(15)))
					{
						gpg_thread.~thread();
						TerminateProcess(params.hProcess, 1);
						params.hProcess = NULL;
						debuglog<<time_str()<<"GPG execution timed out, aborted\n";
					}
					DeleteFile(path_out.c_str());
					path_out += _T(".asc");
					f.open(path_out.c_str(), std::ios::in | std::ios::ate | std::ios::binary);
					wstring data;
					if(f.is_open())
					{
						std::wifstream::pos_type size = f.tellg();
						TCHAR *tmp = new TCHAR [(std::ifstream::pos_type)size+(std::ifstream::pos_type)1];
						f.seekg(0, std::ios::beg);
						f.read(tmp, size);
						tmp[size]= '\0';
						data.append(tmp);
						delete [] tmp;
						f.close();
						DeleteFile(path_out.c_str());
					}
					if(data.empty())
					{
						debuglog<<time_str()<<": info: Failed to read prescense sign from file\n";
						return FALSE;
					}
					if(data.find(_T("-----BEGIN PGP MESSAGE-----")) != wstring::npos && data.find(_T("-----END PGP MESSAGE-----")) != wstring::npos)
					{
						wstring::size_type p1 = data.find(_T("-----BEGIN PGP MESSAGE-----")) + _tcslen(_T("-----BEGIN PGP MESSAGE-----"));
						if(data.find(_T("Version: "), p1) != wstring::npos)
						{
							p1 = data.find(_T("Version: "), p1);
							p1 = data.find(_T("\n"), p1);
							if(data.find(_T("Version: "), p1) != wstring::npos)
							{
								p1 = data.find(_T("Version: "), p1);
								p1 = data.find(_T("\n"), p1)+1;
							}
							else
								p1 += 1;
						}
						if(data.find(_T("Comment: "), p1) != wstring::npos)
						{
							p1 = data.find(_T("Comment: "), p1);
							p1 = data.find(_T("\n"), p1);
							if(data.find(_T("Comment: "), p1) != wstring::npos)
							{
								p1 = data.find(_T("Comment: "), p1);
								p1 = data.find(_T("\n"), p1)+1;
							}
							else
								p1 += 1;
						}
						else
							p1+=1;
						wstring::size_type p2 = data.find(_T("-----END PGP MESSAGE-----"));
						HXML encrypted_data = xi.addChild(node, _T("x"), data.substr(p1, p2-p1).c_str());
						xi.addAttr(encrypted_data, _T("xmlns"), _T("jabber:x:signed"));
					}
					return FALSE;
				}
			}
		}
		local_node = xi.getChild(node, n);
	}
	return FALSE;
}

//boost::mutex sign_file_mutex;

static JABBER_HANDLER_FUNC PrescenseHandler(IJabberInterface *ji, HXML node, void *pUserData)
{
	HXML local_node = node;
	for(int n = 0; n <= xi.getChildCount(node); n++)
	{
		LPCTSTR str = xi.getText(local_node); 
		LPCTSTR nodename = xi.getName(local_node);
		if(nodename)
		{
			if(_tcsstr(nodename, _T("x")))
			{
				for(int n = 0; n < xi.getAttrCount(local_node); n++)
				{
					LPCTSTR name = xi.getAttrName(local_node, n);
					LPCTSTR value = xi.getAttrValue(local_node, name);
					if(_tcsstr(value, _T("jabber:x:signed")))
					{
						LPCTSTR data = xi.getText(local_node);
						wstring sign = _T("-----BEGIN PGP MESSAGE-----\n\n");
						wstring file = toUTF16(get_random(10));
						sign += data;
						sign += _T("\n-----END PGP MESSAGE-----\n");
						TCHAR *path_c = UniGetContactSettingUtf(NULL, szGPGModuleName, "szHomePath", _T(""));
						wstring path_out = path_c;
						mir_free(path_c);
						path_out += _T("\\tmp\\");
						path_out += file;
//						sign_file_mutex.lock();
						DeleteFile(path_out.c_str());
						wfstream f(path_out.c_str(), std::ios::out);
						while(!f.is_open())
							f.open(path_out.c_str(), std::ios::out);
						f<<toUTF8(sign).c_str();
						f.close();
						if(_waccess(path_out.c_str(), 0) == -1)
						{
							if(errno == ENOENT)
							{
//								sign_file_mutex.unlock();
								debuglog<<time_str()<<": info: Failed to write sign in file\n";
								return FALSE;
							}
						}
						{ //gpg
							string out;
							DWORD code;
							wstring cmd = _T(" --verify -a \"");
							cmd += path_out;
							cmd += _T("\"");
							gpg_execution_params params;
							pxResult result;
							params.cmd = &cmd;
							params.useless = "";
							params.out = &out;
							params.code = &code;
							params.result = &result;
							boost::thread gpg_thread(boost::bind(&pxEexcute_thread, &params));
							if(!gpg_thread.timed_join(boost::posix_time::seconds(15)))
							{
								gpg_thread.~thread();
								TerminateProcess(params.hProcess, 1);
								params.hProcess = NULL;
								debuglog<<time_str()<<": GPG execution timed out, aborted\n";
								return FALSE;
							}
							if(result == pxNotFound)
							{
								return FALSE;
							}
							DeleteFile(path_out.c_str());
							if(out.find("key ID ") != string::npos)
							{
								//need to get hcontact here, i can get jid from hxml, and get handle from jid, maybe exists better way ?
								string::size_type p1 = out.find("key ID ") + strlen("key ID ");
								string::size_type p2 = out.find("\n", p1);
								if(p1 != string::npos && p2 != string::npos)
								{
									HANDLE hContact = NULL;
									{
										extern list <JabberAccount*> Accounts;
										list <JabberAccount*>::iterator p = Accounts.begin();	
										for(unsigned int i = 0; i < Accounts.size(); i++, p++)
										{
											if(!(*p))
												break;
											hContact = (*p)->getJabberInterface()->Sys()->ContactFromJID(xi.getAttrValue(node, _T("from")));
											if(hContact)
												hcontact_data[hContact].key_in_prescense = out.substr(p1, p2-p1-1).c_str();
										}
									}
								}
							}
						}
						return FALSE;
					}
				}
			}
		}
		local_node = xi.getChild(node, n);
	}
	return FALSE;
}

static JABBER_HANDLER_FUNC MessageHandler(IJabberInterface *ji, HXML node, void *pUserData)
{
	return FALSE;
}




void AddHandlers()
{
	extern list<JabberAccount*> Accounts;
	list<JabberAccount*>::iterator end = Accounts.end();
	for(list<JabberAccount*>::iterator p = Accounts.begin(); p != end; p++)
	{
		if(!(*p))
			break;
		if((*p)->getSendHandler() == INVALID_HANDLE_VALUE)
			(*p)->setSendHandler((*p)->getJabberInterface()->Net()->AddSendHandler((JABBER_HANDLER_FUNC)SendHandler));
		if((*p)->getPrescenseHandler() == INVALID_HANDLE_VALUE)
			(*p)->setPrescenseHandler((*p)->getJabberInterface()->Net()->AddPresenceHandler((JABBER_HANDLER_FUNC)PrescenseHandler));
		if((*p)->getMessageHandler() == INVALID_HANDLE_VALUE)
			(*p)->setMessageHandler((*p)->getJabberInterface()->Net()->AddMessageHandler((JABBER_HANDLER_FUNC)MessageHandler, JABBER_MESSAGE_TYPE_ERROR,0 ,0));
		if(bAutoExchange)
		{
			(*p)->getJabberInterface()->Net()->RegisterFeature(_T("GPG_Key_Auto_Exchange:0"), _T("Indicates that gpg installed and configured to public key auto exchange (currently implemented in new_gpg Miranda IM plugin)"));
			(*p)->getJabberInterface()->Net()->AddFeatures(_T("GPG_Key_Auto_Exchange:0\0\0"));
		}
	}
}

bool isContactSecured(HANDLE hContact)
{
	BYTE gpg_enc = DBGetContactSettingByte(hContact, szGPGModuleName, "GPGEncryption", 0);
	if(!gpg_enc)
	{
		debuglog<<time_str()<<": encryption is turned off for "<<(TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)hContact, GCDNF_TCHAR)<<"\n";
		return false;
	}
	if(!metaIsProtoMetaContacts(hContact))
	{
		TCHAR *key = UniGetContactSettingUtf(hContact, szGPGModuleName, "GPGPubKey", _T(""));
		if(!key[0])
		{
			mir_free(key);
			debuglog<<time_str()<<": encryption is turned off for "<<(TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)hContact, GCDNF_TCHAR)<<"\n";
			return false;
		}
		mir_free(key);
	}
	debuglog<<time_str()<<": encryption is turned on for "<<(TCHAR*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)hContact, GCDNF_TCHAR)<<"\n";
	return true;
}

bool isContactHaveKey(HANDLE hContact)
{
	TCHAR *key = UniGetContactSettingUtf(hContact, szGPGModuleName, "GPGPubKey", _T(""));
	if(_tcslen(key) > 0)
	{
		mir_free(key);
		return true;
	}
	mir_free(key);
	return false;
}

bool isGPGConfigured()
{
	TCHAR *id = UniGetContactSettingUtf(NULL, szGPGModuleName, "KeyID", _T(""));
	char *key = UniGetContactSettingUtf(NULL, szGPGModuleName, "GPGPubKey", "");
	if(id[0] && key[0])
	{
		mir_free(id);
		mir_free(key);
		return true;
	}
	mir_free(id);
	mir_free(key);
	return false;
}

#define NEWTSTR_MALLOC(A) (A==NULL)?NULL:strcpy((char*)mir_alloc(sizeof(char)*(strlen(A)+1)),A)

const bool StriStr(const char *str, const char *substr)
{
	bool i = false;
	char *str_up = NEWTSTR_MALLOC(str);
	char *substr_up = NEWTSTR_MALLOC(substr);

	CharUpperBuffA(str_up, strlen(str_up));
	CharUpperBuffA(substr_up, strlen(substr_up));

	if(strstr (str_up, substr_up))
		i = true;

	mir_free(str_up);
	mir_free(substr_up);
	
	return i;
}

bool IsOnline(HANDLE hContact)
{
	if(DBGetContactSettingByte(hContact, szGPGModuleName, "Status", 0) == ID_STATUS_OFFLINE)
		return false;
	return true;
}

//from secureim
#include <process.h>

struct TFakeAckParams {
	inline TFakeAckParams( HANDLE p1, HANDLE p2, LONG p3, LPCSTR p4 ) :
		hEvent( p1 ),
		hContact( p2 ),
		id( p3 ),
		msg( p4 )
		{}

	HANDLE	hEvent;
	HANDLE	hContact;
	LONG	id;
	LPCSTR	msg;
};

int SendBroadcast( HANDLE hContact, int type, int result, HANDLE hProcess, LPARAM lParam ) {
	ACKDATA ack;
	memset(&ack,0,sizeof(ack));
	ack.cbSize = sizeof( ACKDATA );
	ack.szModule = 	(char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
	ack.hContact = hContact;
	ack.type = type;
	ack.result = result;
	ack.hProcess = hProcess;
	ack.lParam = lParam;
	return CallService( MS_PROTO_BROADCASTACK, 0, ( LPARAM )&ack );
}



unsigned __stdcall sttFakeAck( LPVOID param ) {

	TFakeAckParams* tParam = ( TFakeAckParams* )param;
	WaitForSingleObject( tParam->hEvent, INFINITE );

	Sleep( 100 );
	if ( tParam->msg == NULL )
		SendBroadcast( tParam->hContact, ACKTYPE_MESSAGE, ACKRESULT_SUCCESS, ( HANDLE )tParam->id, 0 );
	else
		SendBroadcast( tParam->hContact, ACKTYPE_MESSAGE, ACKRESULT_FAILED, ( HANDLE )tParam->id, LPARAM( tParam->msg ));

	CloseHandle( tParam->hEvent );
	delete tParam;

	return 0;
}


int returnNoError(HANDLE hContact) {
	HANDLE hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	unsigned int tID;
	CloseHandle( (HANDLE) _beginthreadex(NULL, 0, sttFakeAck, new TFakeAckParams(hEvent,hContact,777,0), 0, &tID) );
	SetEvent( hEvent );
	return 777;
}
// end from secureim


string toUTF8(wstring str)
{
	string ustr;
	try{
	utf8::utf16to8(str.begin(), str.end(), back_inserter(ustr));
	}
	catch(const utf8::exception& e)
	{
		debuglog<<std::string("utf8cpp encoding exception: ")+(char*)e.what();
	    //TODO
	}
	return ustr;
}



wstring toUTF16(string str) //convert as much as possible
{
	wstring ustr;
	string tmpstr;
	try{
	utf8::replace_invalid(str.begin(), str.end(), back_inserter(tmpstr));
	utf8::utf8to16(tmpstr.begin(), tmpstr.end(), back_inserter(ustr));
	}
	catch(const utf8::exception& e)
	{
		debuglog<<std::string("utf8cpp decoding exception: ")+(char*)e.what();
	    //TODO
	}
	return ustr;
}

string get_random(int length)
{
	string chars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
	string data;
	boost::random_device rng;
	boost::variate_generator<boost::random_device&, boost::uniform_int<>> gen(rng, boost::uniform_int<>(0, chars.length()-1));
	for(int i = 0; i < length; ++i) 
        data += chars[gen()];
	return data;
}

void send_encrypted_msgs_thread(HANDLE hContact)
{
	while(true)
	{
		char *key = UniGetContactSettingUtf(hContact, szGPGModuleName, "GPGPubKey", "");
		while(!isContactSecured(hContact))
			boost::this_thread::sleep(boost::posix_time::seconds(1));
		if(!hcontact_data[hContact].msgs_to_send.empty())
		{
			boost::this_thread::sleep(boost::posix_time::seconds(1));
			list<string>::iterator end = hcontact_data[hContact].msgs_to_send.end();
			extern std::list<HANDLE> sent_msgs;
			for(list<string>::iterator p = hcontact_data[hContact].msgs_to_send.begin(); p != end; ++p)
			{
				sent_msgs.push_back((HANDLE)CallContactService(hContact, PSS_MESSAGE, (WPARAM)PREF_UTF, (LPARAM)p->c_str()));
				HistoryLog(hContact, db_event((char*)p->c_str(),0,0, DBEF_SENT));
				boost::this_thread::sleep(boost::posix_time::seconds(1));
			}
			hcontact_data[hContact].msgs_to_send.clear();
			return;
		}
		else
			return;
	}
}

string time_str()
{
	boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
	return (string)boost::posix_time::to_simple_string(now);
}
