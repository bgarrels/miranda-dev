#define MIRANDA_VER 0x0800
#include "headers.h"

char * pluginDescription = "No more spam! Robots can't go! Only human beings invited!\r\n\r\n"
"This plugin works pretty simple:\r\n"
"While messages from users on your contact list go as there is no any anti-spam software, "
"messages from unknown users are not delivered to you. "
"But also they are not ignored, this plugin replies with a simple question, "
"and if user gives the right answer plugin adds him to your contact list "
"so that he can contact you.";
TCHAR const * defQuestion = 
_T("Spammers made me to install small anti-spam system you are now speaking with.\r\n")
_T("Please reply \"nospam\" without quotes and spaces if you want to contact me.");


INT_PTR CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch(msg)
	{
	case WM_INITDIALOG:
		{
			SetDlgItemTextA(hwnd, ID_DESCRIPTION, pluginDescription);
			TranslateDialogDefault(hwnd);
			SetDlgItemInt(hwnd, ID_MAXQUESTCOUNT, gbMaxQuestCount, FALSE);
			SendDlgItemMessage(hwnd, ID_INFTALKPROT, BM_SETCHECK, gbInfTalkProtection ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, ID_ADDPERMANENT, BM_SETCHECK, gbAddPermanent ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, ID_HANDLEAUTHREQ, BM_SETCHECK, gbHandleAuthReq ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, ID_HIDECONTACTS, BM_SETCHECK, gbHideContacts ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, ID_IGNORESPAMMERS, BM_SETCHECK, gbIgnoreContacts ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, ID_LOGSPAMTOFILE, BM_SETCHECK, gbLogToFile ? BST_CHECKED : BST_UNCHECKED, 0);
		}
		return TRUE;
	case WM_COMMAND:{
		switch (LOWORD(wParam))
		{
		case ID_MAXQUESTCOUNT:
			{
				if (EN_CHANGE != HIWORD(wParam) || (HWND)lParam != GetFocus())
					return FALSE;
				break;
			}
		}
		SendMessage(GetParent(hwnd), PSM_CHANGED, 0, 0);
					}
		break;
	case WM_NOTIFY:
		{
			NMHDR* nmhdr = (NMHDR*)lParam;
			switch (nmhdr->code)
			{
			case PSN_APPLY:
				{
					DBWriteContactSettingDword(NULL, pluginName, "maxQuestCount", gbMaxQuestCount =
						GetDlgItemInt(hwnd, ID_MAXQUESTCOUNT, NULL, FALSE));
					DBWriteContactSettingByte(NULL, pluginName, "infTalkProtection", gbInfTalkProtection =  
						BST_CHECKED == SendDlgItemMessage(hwnd, ID_INFTALKPROT, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "addPermanent", gbAddPermanent = 
						BST_CHECKED == SendDlgItemMessage(hwnd, ID_ADDPERMANENT, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "handleAuthReq", gbHandleAuthReq = 
						BST_CHECKED == SendDlgItemMessage(hwnd, ID_HANDLEAUTHREQ, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "HideContacts",  gbHideContacts = 
						BST_CHECKED == SendDlgItemMessage(hwnd, ID_HIDECONTACTS, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "IgnoreContacts",  gbIgnoreContacts = 
						BST_CHECKED == SendDlgItemMessage(hwnd, ID_IGNORESPAMMERS, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "LogSpamToFile",  gbLogToFile = 
						BST_CHECKED == SendDlgItemMessage(hwnd, ID_LOGSPAMTOFILE, BM_GETCHECK, 0, 0));
				}
				return TRUE;
			}
		}
		break;			
	}
	return FALSE;
}

INT_PTR CALLBACK MessagesDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch(msg)
	{
	case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwnd);
			SetDlgItemText(hwnd, ID_QUESTION, gbQuestion.c_str());
			SetDlgItemText(hwnd, ID_ANSWER, gbAnswer.c_str());			
			SetDlgItemText(hwnd, ID_CONGRATULATION, gbCongratulation.c_str());
			SetDlgItemText(hwnd, ID_AUTHREPL, gbAuthRepl.c_str());
			EnableWindow(GetDlgItem(hwnd, ID_ANSWER), !gbMathExpression);
			variables_skin_helpbutton(hwnd, IDC_VARS);
			gbVarsServiceExist?EnableWindow(GetDlgItem(hwnd, IDC_VARS),1):EnableWindow(GetDlgItem(hwnd, IDC_VARS),0);
		}
		return TRUE;
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case ID_QUESTION:
			case ID_ANSWER:
			case ID_AUTHREPL:
			case ID_CONGRATULATION:
				{
					if (EN_CHANGE != HIWORD(wParam) || (HWND)lParam != GetFocus())
						return FALSE;
					break;
				}
			case ID_RESTOREDEFAULTS:
				SetDlgItemText(hwnd, ID_QUESTION, TranslateTS(defQuestion));
				SetDlgItemText(hwnd, ID_ANSWER,  TranslateTS(_T("nospam")));
				SetDlgItemText(hwnd, ID_AUTHREPL, TranslateTS(_T("StopSpam: send a message and reply to a anti-spam bot question.")));
				SetDlgItemText(hwnd, ID_CONGRATULATION, TranslateTS(_T("Congratulations! You just passed human/robot test. Now you can write me a message.")));
				SendMessage(GetParent(hwnd), PSM_CHANGED, 0, 0);
				return TRUE;
			case IDC_VARS:
				variables_showhelp(hwnd, msg, VHF_FULLDLG|VHF_SETLASTSUBJECT, NULL, NULL);
				return TRUE;
			}
			SendMessage(GetParent(hwnd), PSM_CHANGED, 0, 0);
		}
		break;
	case WM_NOTIFY:
		{
			NMHDR* nmhdr = (NMHDR*)lParam;
			switch (nmhdr->code)
			{
			case PSN_APPLY:
				{
					DBWriteContactSettingTString(NULL, pluginName, "question", 
						GetDlgItemString(hwnd, ID_QUESTION).c_str());
					gbQuestion = DBGetContactSettingStringPAN(NULL, pluginName, "question", defQuestion);
					DBWriteContactSettingTString(NULL, pluginName, "answer",
						GetDlgItemString(hwnd, ID_ANSWER).c_str());
					gbAnswer = DBGetContactSettingStringPAN(NULL, pluginName, "answer", _T("nospam"));
					DBWriteContactSettingTString(NULL, pluginName, "authrepl", 
						GetDlgItemString(hwnd, ID_AUTHREPL).c_str());
					gbAuthRepl = DBGetContactSettingStringPAN(NULL, pluginName, "authrepl", _T("StopSpam: send a message and reply to a anti-spam bot question."));
					DBWriteContactSettingTString(NULL, pluginName, "congratulation", 
						GetDlgItemString(hwnd, ID_CONGRATULATION).c_str());
					gbCongratulation = DBGetContactSettingStringPAN(NULL, pluginName, "congratulation", _T("Congratulations! You just passed human/robot test. Now you can write me a message."));
				}
				return TRUE;
			}
		}
		break;			
	}
	return FALSE;
}

INT_PTR CALLBACK ProtoDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch(msg)
	{
	case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwnd);
			int n;
			PROTOCOLDESCRIPTOR** pppd;
			if(!CallService(MS_PROTO_ENUMPROTOCOLS, (LPARAM)&n, (WPARAM)&pppd))
				for(int i = 0; i < n; ++i)
				{
					SendDlgItemMessageA(hwnd, (ProtoInList(pppd[i]->szName) ? ID_USEDPROTO : ID_ALLPROTO), 
						LB_ADDSTRING, 0, (LPARAM)pppd[i]->szName);
				}
		}
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_ADD:
			{
				WPARAM n = (WPARAM)SendDlgItemMessage(hwnd, ID_ALLPROTO, LB_GETCURSEL, 0, 0);
				if(LB_ERR != n)
				{
					size_t len = SendDlgItemMessage(hwnd, ID_ALLPROTO, LB_GETTEXTLEN, n, 0);
					if(LB_ERR != len)
					{
						TCHAR * buf = new TCHAR[len + 1];
						SendDlgItemMessage(hwnd, ID_ALLPROTO, LB_GETTEXT, n, (LPARAM)buf);
						SendDlgItemMessage(hwnd, ID_USEDPROTO, LB_ADDSTRING, 0, (LPARAM)buf);
						delete []buf;
						SendDlgItemMessage(hwnd, ID_ALLPROTO, LB_DELETESTRING, n, 0);
					}
				}
			}
			break;
		case ID_REMOVE:
			{
				WPARAM n = (WPARAM)SendDlgItemMessage(hwnd, ID_USEDPROTO, LB_GETCURSEL, 0, 0);
				if(LB_ERR != n)
				{
					size_t len = SendDlgItemMessage(hwnd, ID_USEDPROTO, LB_GETTEXTLEN, n, 0);
					if(LB_ERR != len)
					{
						TCHAR * buf = new TCHAR[len + 1];
						SendDlgItemMessage(hwnd, ID_USEDPROTO, LB_GETTEXT, n, (LPARAM)buf);
						SendDlgItemMessage(hwnd, ID_ALLPROTO, LB_ADDSTRING, 0, (LPARAM)buf);
						delete []buf;
						SendDlgItemMessage(hwnd, ID_USEDPROTO, LB_DELETESTRING, n, 0);
					}
				}
			}
			break;
		case ID_ADDALL:
			for(;;)
			{
				LRESULT count = SendDlgItemMessage(hwnd, ID_ALLPROTO, LB_GETCOUNT, 0, 0);
				if(!count || LB_ERR == count)
					break;
				SendDlgItemMessage(hwnd, ID_ALLPROTO, LB_SETCURSEL, 0, 0);
				SendMessage(hwnd, WM_COMMAND, ID_ADD, 0);
			}
			break;
		case ID_REMOVEALL:
			for(;;)
			{
				LRESULT count = SendDlgItemMessage(hwnd, ID_USEDPROTO, LB_GETCOUNT, 0, 0);
				if(!count || LB_ERR == count)
					break;
				SendDlgItemMessage(hwnd, ID_USEDPROTO, LB_SETCURSEL, 0, 0);
				SendMessage(hwnd, WM_COMMAND, ID_REMOVE, 0);
			}
			break;
		default:
			return FALSE;
		}
		SendMessage(GetParent(hwnd), PSM_CHANGED, 0, 0);
		return TRUE;
	case WM_NOTIFY:
		{
			NMHDR* nmhdr = (NMHDR*)lParam;
			switch (nmhdr->code)
			{
			case PSN_APPLY:
				{
					LRESULT count = SendDlgItemMessage(hwnd, ID_USEDPROTO, LB_GETCOUNT, 0, 0);
					std::ostringstream out;
					for(int i = 0; i < count; ++i)
					{
						size_t len = SendDlgItemMessageA(hwnd, ID_USEDPROTO, LB_GETTEXTLEN, i, 0);
						if(LB_ERR != len)
						{
							char * buf = new char[len + 1];
							SendDlgItemMessageA(hwnd, ID_USEDPROTO, LB_GETTEXT, i, (LPARAM)buf);
							out << buf << "\r\n";
							delete []buf;
						}
					}
					DBWriteContactSettingString(NULL, pluginName, "protoList", out.str().c_str());
				}
				return TRUE;
			}			
		}
		break;			
	}
	return FALSE;
}

INT_PTR CALLBACK AdvancedDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch(msg)
	{
	case WM_INITDIALOG:
		{
			TranslateDialogDefault(hwnd);
			SendDlgItemMessage(hwnd, IDC_INVIS_DISABLE, BM_SETCHECK, gbInvisDisable ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, IDC_CASE_INSENSITIVE, BM_SETCHECK, gbCaseInsensitive ? BST_CHECKED : BST_UNCHECKED, 0);
			gbDosServiceExist?EnableWindow(GetDlgItem(hwnd, ID_DOS_INTEGRATION),1):EnableWindow(GetDlgItem(hwnd, ID_DOS_INTEGRATION),0);
			SendDlgItemMessage(hwnd, ID_DOS_INTEGRATION, BM_SETCHECK, gbDosServiceIntegration ? BST_CHECKED : BST_UNCHECKED, 0);
			SetDlgItemText(hwnd, ID_SPECIALGROUPNAME, gbSpammersGroup.c_str());
			SendDlgItemMessage(hwnd, ID_SPECIALGROUP, BM_SETCHECK, gbSpecialGroup ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, ID_EXCLUDE, BM_SETCHECK, gbExclude ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, ID_REMOVE_TMP, BM_SETCHECK, gbDelExcluded ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, ID_REMOVE_TMP_ALL, BM_SETCHECK, gbDelAllTempory ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, ID_IGNOREURL, BM_SETCHECK, gbIgnoreURL ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, IDC_AUTOAUTH, BM_SETCHECK, gbAutoAuth ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, IDC_ADDTOSRVLST, BM_SETCHECK, gbAutoAddToServerList ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, IDC_REQAUTH, BM_SETCHECK, gbAutoReqAuth ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, IDC_REGEX, BM_SETCHECK, gbRegexMatch ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, IDC_HISTORY_LOG, BM_SETCHECK, gbHistoryLog ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, IDC_MATH_QUESTION, BM_SETCHECK, gbMathExpression ? BST_CHECKED : BST_UNCHECKED, 0 );

			SetDlgItemText(hwnd, IDC_AUTOADDGROUP, gbAutoAuthGroup.c_str());
		}
		return TRUE;
	case WM_COMMAND:{
		switch (LOWORD(wParam))
		{
		case IDC_MATH_DETAILS:
			{
				MessageBox(NULL, TranslateT("If math expression is turned on you can use following expression in message text:\nXX+XX-X/X*X\neach X will be replaced by one ruandom number and answer will be expression result\nMessage must contain only one expression without spaces"), _T("Info"), MB_OK);
			}
			break;
		case IDC_INVIS_DISABLE: 
		case IDC_CASE_INSENSITIVE: 
		case ID_DOS_INTEGRATION:
		case ID_SPECIALGROUPNAME: 
		case ID_SPECIALGROUP: 
		case ID_EXCLUDE: 
		case ID_REMOVE_TMP: 
		case ID_REMOVE_TMP_ALL:
		case ID_IGNOREURL:
		case IDC_AUTOAUTH:
		case IDC_ADDTOSRVLST:
		case IDC_REQAUTH:
		case IDC_AUTOADDGROUP:
		case IDC_REGEX:
		case IDC_HISTORY_LOG:
			SendMessage(GetParent(hwnd), PSM_CHANGED, 0, 0);
			break;

		}
					}
		break;
	case WM_NOTIFY:
		{
			NMHDR* nmhdr = (NMHDR*)lParam;
			switch (nmhdr->code)
			{
			case PSN_APPLY:
				{
					DBWriteContactSettingByte(NULL, pluginName, "CaseInsensitive", gbCaseInsensitive = 
						BST_CHECKED == SendDlgItemMessage(hwnd, IDC_CASE_INSENSITIVE, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "DisableInInvis", gbInvisDisable = 
						BST_CHECKED == SendDlgItemMessage(hwnd, IDC_INVIS_DISABLE, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "DOSIntegration",  gbDosServiceIntegration = 
						BST_CHECKED == SendDlgItemMessage(hwnd, ID_DOS_INTEGRATION, BM_GETCHECK, 0, 0));
					{
						static tstring NewGroupName, CurrentGroupName;
						NewGroupName = GetDlgItemString(hwnd, ID_SPECIALGROUPNAME);
						CurrentGroupName = gbSpammersGroup = DBGetContactSettingStringPAN(NULL, pluginName, "SpammersGroup", _T("0"));
						if(wcscmp(CurrentGroupName.c_str(), NewGroupName.c_str()) != 0)
						{
							int GroupNumber = 0;
							BYTE GroupExist = 0;
							TCHAR szValue[96] = {0};
							char szNumber[32] = {0};
							extern int CreateCListGroup(TCHAR* szGroupName);
							strcpy(szNumber, "0");
							while(strcmp(DBGetContactSettingStringPAN_A(NULL, "CListGroups", szNumber, "0").c_str(), "0") != 0)
							{
								_itoa(GroupNumber, szNumber, 10);
								wcscpy(szValue, DBGetContactSettingStringPAN(NULL, "CListGroups", szNumber, _T("0")).c_str());
								if(wcscmp(NewGroupName.c_str(), szValue + 1) == 0)
								{
									GroupExist = 1;
									break;
								}
								GroupNumber++;
							}
							DBWriteContactSettingTString(NULL,pluginName, "SpammersGroup", NewGroupName.c_str());
							gbSpammersGroup = DBGetContactSettingStringPAN(NULL,pluginName,"SpammersGroup", _T("Spammers"));
							if(!GroupExist && gbSpecialGroup)
								CreateCListGroup((TCHAR*)gbSpammersGroup.c_str());
						}
					}
					DBWriteContactSettingByte(NULL, pluginName, "SpecialGroup",  gbSpecialGroup = 
						BST_CHECKED == SendDlgItemMessage(hwnd, ID_SPECIALGROUP, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "ExcludeContacts",  gbExclude = 
						BST_CHECKED == SendDlgItemMessage(hwnd, ID_EXCLUDE, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "DelExcluded",  gbDelExcluded = 
						BST_CHECKED == SendDlgItemMessage(hwnd, ID_REMOVE_TMP, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "DelAllTempory",  gbDelAllTempory = 
						BST_CHECKED == SendDlgItemMessage(hwnd, ID_REMOVE_TMP_ALL, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "IgnoreURL",  gbIgnoreURL =
						BST_CHECKED == SendDlgItemMessage(hwnd, ID_IGNOREURL, BM_GETCHECK, 0, 0));

					DBWriteContactSettingByte(NULL, pluginName, "AutoAuth",  gbAutoAuth =
						BST_CHECKED == SendDlgItemMessage(hwnd, IDC_AUTOAUTH, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "AutoAddToServerList",  gbAutoAddToServerList =
						BST_CHECKED == SendDlgItemMessage(hwnd, IDC_ADDTOSRVLST, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "AutoReqAuth",  gbAutoReqAuth =
						BST_CHECKED == SendDlgItemMessage(hwnd, IDC_REQAUTH, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "RegexMatch",  gbRegexMatch =
						BST_CHECKED == SendDlgItemMessage(hwnd, IDC_REGEX, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "HistoryLog",  gbHistoryLog =
						BST_CHECKED == SendDlgItemMessage(hwnd, IDC_HISTORY_LOG, BM_GETCHECK, 0, 0));
					DBWriteContactSettingByte(NULL, pluginName, "MathExpression",  gbMathExpression =
						BST_CHECKED == SendDlgItemMessage(hwnd, IDC_MATH_QUESTION, BM_GETCHECK, 0, 0));

					{
						static tstring NewAGroupName, CurrentAGroupName;
						NewAGroupName = GetDlgItemString(hwnd, IDC_AUTOADDGROUP);
						CurrentAGroupName = gbAutoAuthGroup = DBGetContactSettingStringPAN(NULL, pluginName, "AutoAuthGroup", _T("0"));
						if(wcscmp(CurrentAGroupName.c_str(), NewAGroupName.c_str()) != 0)
						{
							int GroupNumber = 0;
							BYTE GroupExist = 0;
							TCHAR szValue[96] = {0};
							char szNumber[32] = {0};
							extern int CreateCListGroup(TCHAR* szGroupName);
							strcpy(szNumber, "0");
							while(strcmp(DBGetContactSettingStringPAN_A(NULL, "CListGroups", szNumber, "0").c_str(), "0") != 0)
							{
								_itoa(GroupNumber, szNumber, 10);
								wcscpy(szValue, DBGetContactSettingStringPAN(NULL, "CListGroups", szNumber, _T("0")).c_str());
								if(wcscmp(NewAGroupName.c_str(), szValue + 1) == 0)
								{
									GroupExist = 1;
									break;
								}
								GroupNumber++;
							}
							DBWriteContactSettingTString(NULL,pluginName, "AutoAuthGroup", NewAGroupName.c_str());
							gbAutoAuthGroup = DBGetContactSettingStringPAN(NULL,pluginName,"AutoAuthGroup", _T("Not Spammers"));
							if(!GroupExist && gbAutoAddToServerList)
								CreateCListGroup((TCHAR*)gbAutoAuthGroup.c_str());
						}
					}

				}
				return TRUE;
			}
		}
		break;			
	}
	return FALSE;
}


HINSTANCE hInst;
MIRANDA_HOOK_EVENT(ME_OPT_INITIALISE, w, l)
{
	OPTIONSDIALOGPAGE odp = {0};
	odp.cbSize = sizeof(odp);
	odp.ptszGroup = _T("Message Sessions");
	odp.ptszTitle = _T(pluginName);
	odp.position = -1;
	odp.hInstance = hInst;
	odp.flags = ODPF_TCHAR;

	odp.ptszTab = _T("Main");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_MAIN);
	odp.pfnDlgProc = MainDlgProc;
	CallService(MS_OPT_ADDPAGE, w, (LPARAM)&odp);


	odp.ptszTab = _T("Messages");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_MESSAGES);
	odp.pfnDlgProc = MessagesDlgProc;
	CallService(MS_OPT_ADDPAGE, w, (LPARAM)&odp);

	odp.ptszTab = _T("Protocols");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_PROTO);
	odp.pfnDlgProc = ProtoDlgProc;
	CallService(MS_OPT_ADDPAGE, w, (LPARAM)&odp);

	odp.ptszTab = _T("Advanced");
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_ADVANCED);
	odp.pfnDlgProc = AdvancedDlgProc;
	odp.flags = odp.flags|ODPF_EXPERTONLY;
	CallService(MS_OPT_ADDPAGE, w, (LPARAM)&odp);

	return 0;
}

