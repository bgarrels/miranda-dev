#include "StdAfx.h"
#include "QuotesProviderGoogleFinance.h"
#include "QuotesProviderVisitor.h"
#include "EconomicRateInfo.h"
#include "DBUtils.h"
#include "resource.h"
#include "ModuleInfo.h"
#include "QuotesProviders.h"
#include "CommonOptionDlg.h"
#include "WinCtrlHelper.h"
#include "IHTMLParser.h"
#include "IHTMLEngine.h"
#include "HTTPSession.h"
#include "Log.h"
#include "Locale.h"

CQuotesProviderGoogleFinance::CQuotesProviderGoogleFinance()
{
}

CQuotesProviderGoogleFinance::~CQuotesProviderGoogleFinance()
{
}

namespace
{
	tstring build_url(HANDLE hContact,const tstring& rsURL)
	{
		tostringstream o;
		o << rsURL << _T("?q=") << Quotes_DBGetStringT(hContact,QUOTES_MODULE_NAME,DB_STR_QUOTE_ID);
		return o.str();
	}

	struct CGoogleInfo
	{
		enum
		{
			giRate = 0x0001,
			giOpen = 0x0002,
			giPercentChangeAfterHours = 0x0004,
			giPercentChangeToYesterdayClose = 0x0008
		};
		CGoogleInfo() 
			: m_dRate(0.0),m_dOpenValue(0.0),m_dPercentChangeAfterHours(0.0),m_dPercentChangeToYersterdayClose(0.0),m_nFlags(0){}
// 		tstring m_sCmpID;
		tstring m_sCmpName;
		double m_dRate;
		double m_dOpenValue;
		double m_dPercentChangeAfterHours;
		double m_dPercentChangeToYersterdayClose;

// 		tstring m_sRateID;
// 		tstring m_sDiffID;
		byte m_nFlags;
	};

	tstring make_rate_id_value(const tstring& rsCmpID,int nFlags)
	{
		tostringstream o;
		o << _T("ref_") << rsCmpID;
		switch(nFlags)
		{
		default:
			assert(!"Unknown type of value");
		case CGoogleInfo::giRate:
			o << _T("_l");
			break;
		case CGoogleInfo::giPercentChangeAfterHours:
			o << _T("_ecp");
			break;
		case CGoogleInfo::giPercentChangeToYesterdayClose:
			o << _T("_cp");
			break;
		}
		
		return o.str();
	}

	tstring get_var_value(const tstring& rsHTML,LPCTSTR pszVarName,size_t cVarNameLength)
	{
		tstring sResult;
		tstring::size_type n = rsHTML.find(pszVarName);
		if(tstring::npos != n)
		{
			size_t cLengthHTML = rsHTML.size();
			for(size_t i = n + cVarNameLength;i < cLengthHTML;++i)
			{
				TCHAR c = rsHTML[i];
				if(_T(';') == c)
				{
					break;
				}
				else
				{
					sResult.push_back(c);
				}
			}
		}

		return sResult;
	}

	tstring get_company_id(const tstring& rsHTML)
	{
		static LPCTSTR pszVarName = _T("var _companyId = ");
		static size_t cVarNameLength = _tcslen(pszVarName);

		return get_var_value(rsHTML,pszVarName,cVarNameLength);		
	}

	tstring get_company_name(const tstring& rsHTML)
	{
		static LPCTSTR pszVarName = _T("var _companyName = ");
		static size_t cVarNameLength = _tcslen(pszVarName);

		tstring s = get_var_value(rsHTML,pszVarName,cVarNameLength);
		if(s.size() > 0 && _T('\'') == s[0])
		{
			s.erase(s.begin());
		}

		if(s.size() > 0 && _T('\'') == s[s.size()-1])
		{
			s.erase(s.rbegin().base()-1);
		}

		return s;
	}

	bool get_double_value(const tstring& rsText,double& rdValue)
	{
		tistringstream input(rsText);
		input.imbue(std::locale("English_United States.1252"));
		input >> rdValue;

		if((true == input.bad()) || (true == input.fail()))
		{
			tistringstream inputSys(rsText);
			input.imbue(GetSystemLocale());
			input >> rdValue;
			return (false == inputSys.bad()) && (false == inputSys.fail());
		}
		else
		{
			return true;
		}
	}

	bool get_rate(const IHTMLNode::THTMLNodePtr& pRate,CGoogleInfo& rInfo)
	{
		tstring sRate = pRate->GetText();

		if(true == get_double_value(sRate,rInfo.m_dRate))
		{
			rInfo.m_nFlags |= CGoogleInfo::giRate;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool get_inline_data(const IHTMLNode::THTMLNodePtr& pNode,CGoogleInfo& rInfo)
	{
		size_t cChild = pNode->GetChildCount();
		for(size_t i = 0;i < cChild;++i)
		{
			IHTMLNode::THTMLNodePtr pChild = pNode->GetChildPtr(i);
			size_t c = pChild->GetChildCount();
			assert(2 == c);
			if(c >= 2)
			{
				IHTMLNode::THTMLNodePtr pName = pChild->GetChildPtr(0);

				tstring sName = pName->GetText();
				if(0 == quotes_stricmp(sName.c_str(),_T("Open")))
				{
					IHTMLNode::THTMLNodePtr pValue = pChild->GetChildPtr(1);
					tstring sValue = pValue->GetText();
					if(true == get_double_value(sValue,rInfo.m_dOpenValue))
					{
						rInfo.m_nFlags |= CGoogleInfo::giOpen;
					}
					return true;
				}
			}
		}

		return false;
	}

	bool get_dif_value(const IHTMLNode::THTMLNodePtr& pNode,CGoogleInfo& rInfo,int nItem)
	{
		tstring sDiff = pNode->GetText();
		// this value is in brackets and it has percentage sign. 
		// Remove these symbols.
		for(tstring::iterator i = sDiff.begin();i != sDiff.end();)
		{
			TCHAR s = *i;
			if(_T('(') == s || _T(')') == s || _T('%') == s)
			{
				i = sDiff.erase(i);
			}
			else
			{
				++i;
			}
		}

		double* pValue = NULL;
		switch(nItem)
		{
		case CGoogleInfo::giPercentChangeAfterHours:
			pValue = &rInfo.m_dPercentChangeAfterHours;
			break;
		case CGoogleInfo::giPercentChangeToYesterdayClose:
			pValue = &rInfo.m_dPercentChangeToYersterdayClose;
			break;
		}

		assert(pValue);

		if((pValue) && (true == get_double_value(sDiff,*pValue)))
		{
			rInfo.m_nFlags |= nItem;
			return true;
		}
		else
		{
			return false;
		}

	}

	bool parse_responce(const tstring& rsHTML,CGoogleInfo& rInfo)
	{
		IHTMLEngine::THTMLParserPtr pHTMLParser = CModuleInfo::GetHTMLEngine()->GetParserPtr();
		IHTMLNode::THTMLNodePtr pRoot = pHTMLParser->ParseString(rsHTML);
		if(pRoot)
		{
			tstring sCmpID = get_company_id(rsHTML);
			if(false == sCmpID.empty())
			{
				tstring sRateID = make_rate_id_value(sCmpID,CGoogleInfo::giRate);
				IHTMLNode::THTMLNodePtr pRate = pRoot->GetElementByID(sRateID);
				if(pRate && get_rate(pRate,rInfo))
				{
					rInfo.m_sCmpName = get_company_name(rsHTML);

					IHTMLNode::THTMLNodePtr pInline = pRoot->GetElementByID(_T("snap-data"));
					if(pInline)
					{
						get_inline_data(pInline,rInfo);
					}

					tstring sDiffID = make_rate_id_value(sCmpID,CGoogleInfo::giPercentChangeAfterHours);
					IHTMLNode::THTMLNodePtr pDiff = pRoot->GetElementByID(sDiffID);
					if(pDiff)
					{
						get_dif_value(pDiff,rInfo,CGoogleInfo::giPercentChangeAfterHours);
					}

					sDiffID = make_rate_id_value(sCmpID,CGoogleInfo::giPercentChangeToYesterdayClose);
					pDiff = pRoot->GetElementByID(sDiffID);
					if(pDiff)
					{
						get_dif_value(pDiff,rInfo,CGoogleInfo::giPercentChangeToYesterdayClose);
					}

					return true;
				}

				//return (true == parse_html_node(pRoot,rInfo));
			}
		}

		return false;
	}
}

void CQuotesProviderGoogleFinance::RefreshQuotes(TContracts& anContacts)
{
	CHTTPSession http;
	tstring sURL = GetURL();
	bool bUseExtendedStatus = CModuleInfo::GetInstance().GetExtendedStatusFlag();

	for(TContracts::const_iterator i = anContacts.begin();i != anContacts.end() && IsOnline();++i)
	{
		HANDLE hContact = *i;

		if(bUseExtendedStatus)
		{
			SetContactStatus(hContact,ID_STATUS_OCCUPIED);
		}

		tstring sFullURL = build_url(hContact,sURL);
// 		LogIt(Info,sFullURL);
		if((true == http.OpenURL(sFullURL)) && (true == IsOnline()))
		{
			tstring sHTML;
			if((true == http.ReadResponce(sHTML)) && (true == IsOnline()))
			{
// 				LogIt(Info,sHTML);

				CGoogleInfo Info;
				parse_responce(sHTML,Info);
				if(true == IsOnline())
				{
					if(Info.m_nFlags&CGoogleInfo::giRate)
					{
						if(Info.m_nFlags&CGoogleInfo::giOpen)
						{
							Quotes_DBWriteDouble(hContact,QUOTES_MODULE_NAME,DB_STR_GOOGLE_FINANCE_OPEN_VALUE,Info.m_dOpenValue);
						}
						if(Info.m_nFlags&CGoogleInfo::giPercentChangeAfterHours)
						{
							Quotes_DBWriteDouble(hContact,QUOTES_MODULE_NAME,DB_STR_GOOGLE_FINANCE_DIFF,Info.m_dPercentChangeAfterHours);
						}
						if(Info.m_nFlags&CGoogleInfo::giPercentChangeToYesterdayClose)
						{
							Quotes_DBWriteDouble(hContact,QUOTES_MODULE_NAME,DB_STR_GOOGLE_FINANCE_PERCENT_CHANGE_TO_YERSTERDAY_CLOSE,Info.m_dPercentChangeToYersterdayClose);
						}
						if(false == Info.m_sCmpName.empty())
						{
							DBWriteContactSettingTString(hContact,QUOTES_MODULE_NAME,DB_STR_QUOTE_DESCRIPTION,Info.m_sCmpName.c_str());
						}

						WriteContactRate(hContact,Info.m_dRate);
						continue;
					}
				}
			}
		}

		SetContactStatus(hContact,ID_STATUS_NA);
	}

}

namespace
{
	CQuotesProviderGoogleFinance* get_provider()
	{
		CModuleInfo::TQuotesProvidersPtr& pProviders = CModuleInfo::GetQuoteProvidersPtr();
		const CQuotesProviders::TQuotesProviders& rapQuotesProviders = pProviders->GetProviders();
		for(CQuotesProviders::TQuotesProviders::const_iterator i = rapQuotesProviders.begin();i != rapQuotesProviders.end();++i)
		{
			const CQuotesProviders::TQuotesProviderPtr& pProvider = *i;
			CQuotesProviderGoogleFinance* pGoogle = dynamic_cast<CQuotesProviderGoogleFinance*>(pProvider.get());
			if(pGoogle)
			{
				return pGoogle;
			}
		}

		assert(!"We should never get here!");
		return NULL;
	}

	inline tstring make_quote_name(const CQuotesProviderBase::CQuote& rQuote)
	{
		const tstring& rsDesc = rQuote.GetName();
		return((false == rsDesc.empty()) ? rsDesc : rQuote.GetSymbol());
	}


	inline void add_quote_to_wnd(const CQuotesProviderBase::CQuote& rQuote,HWND hwnd)
	{
		tstring sName = make_quote_name(rQuote);
		::SendMessage(hwnd,LB_ADDSTRING,0,reinterpret_cast<LPARAM>(sName.c_str()));
	}

	typedef CQuotesProviderGoogleFinance::TQuotes TQuotes;
	TQuotes g_aWatchedQuotes;

	inline bool cmp_quotes(const tstring& rsQuoteId,const CQuotesProviderBase::CQuote& quote)
	{
		return (0 == quotes_stricmp(rsQuoteId.c_str(),quote.GetID().c_str()));
	}

	INT_PTR CALLBACK  GoogleFinanceOptDlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
	{
		CQuotesProviderGoogleFinance* pProvider = get_provider();

		CCommonDlgProcData d(pProvider);
		CommonOptionDlgProc(hDlg,message,wParam,lParam,d);

		switch(message)
		{
		case WM_INITDIALOG:
			{
				TranslateDialogDefault(hDlg);

				g_aWatchedQuotes.clear();
				pProvider->GetWatchedQuotes(g_aWatchedQuotes);

				HWND hwndList = GetDlgItem(hDlg,IDC_LIST_RATES);
				std::for_each(g_aWatchedQuotes.begin(),g_aWatchedQuotes.end(),
					boost::bind(add_quote_to_wnd,_1,hwndList));

				::EnableWindow(::GetDlgItem(hDlg,IDC_BUTTON_ADD),FALSE);
				::EnableWindow(::GetDlgItem(hDlg,IDC_BUTTON_REMOVE),FALSE);
			}
			return (TRUE);
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_EDIT_QUOTE:
				if(EN_CHANGE == HIWORD(wParam))
				{
					::EnableWindow(::GetDlgItem(hDlg,IDC_BUTTON_ADD),GetWindowTextLength(GetDlgItem(hDlg,IDC_EDIT_QUOTE)) > 0);
				}
				return (TRUE);
			case IDC_BUTTON_ADD:
				if(BN_CLICKED == HIWORD(wParam))
				{
					HWND hEdit = GetDlgItem(hDlg,IDC_EDIT_QUOTE);
					tstring sQuoteSymbol = get_window_text(hEdit);
					assert(false == sQuoteSymbol.empty());
					if((false == sQuoteSymbol.empty()) 
						&& (g_aWatchedQuotes.end() == std::find_if(g_aWatchedQuotes.begin(),g_aWatchedQuotes.end(),
							boost::bind(cmp_quotes,boost::cref(sQuoteSymbol),_1))))
					{
						CQuotesProviderBase::CQuote quote(sQuoteSymbol,sQuoteSymbol);
						add_quote_to_wnd(quote,GetDlgItem(hDlg,IDC_LIST_RATES));
						g_aWatchedQuotes.push_back(quote);

						SetDlgItemText(hDlg,IDC_EDIT_QUOTE,_T(""));
						SetFocus(hEdit);
						PropSheet_Changed(::GetParent(hDlg),hDlg);
					}
				}
				return (TRUE);
			case IDC_BUTTON_REMOVE:
				if(BN_CLICKED == HIWORD(wParam))
				{
					HWND hWnd = ::GetDlgItem(hDlg,IDC_LIST_RATES);
					int nSel = ::SendMessage(hWnd,LB_GETCURSEL,0,0);
					if(LB_ERR != nSel)
					{
						if(nSel < static_cast<int>(g_aWatchedQuotes.size()))
						{
							TQuotes::iterator i = g_aWatchedQuotes.begin();
							std::advance(i,nSel);
							g_aWatchedQuotes.erase(i);
						}

						if(LB_ERR != ::SendMessage(hWnd,LB_DELETESTRING,nSel,0))
						{
							PropSheet_Changed(::GetParent(hDlg),hDlg);
						}
					}

					nSel = ::SendMessage(hWnd,LB_GETCURSEL,0,0);
					::EnableWindow(::GetDlgItem(hDlg,IDC_BUTTON_REMOVE),(LB_ERR != nSel));
				}
				return (TRUE);
			case IDC_LIST_RATES:
				if(CBN_SELCHANGE == HIWORD(wParam))
				{
					int nSel = ::SendMessage(::GetDlgItem(hDlg,IDC_LIST_RATES),LB_GETCURSEL,0,0);
					::EnableWindow(::GetDlgItem(hDlg,IDC_BUTTON_REMOVE),(LB_ERR != nSel));
				}
				return (TRUE);
			}
			return (FALSE);

		case WM_NOTIFY:
			{
				LPNMHDR pNMHDR = reinterpret_cast<LPNMHDR>(lParam);
				switch(pNMHDR->code)
				{
				case PSN_APPLY:
					if(pProvider)
					{
						TQuotes aTemp;
						pProvider->GetWatchedQuotes(aTemp);

						TQuotes aRemove;
						BOOST_FOREACH(const CQuotesProviderBase::CQuote& quote,aTemp)
						{
							if(g_aWatchedQuotes.end() == 
								std::find_if(g_aWatchedQuotes.begin(),g_aWatchedQuotes.end(),
									boost::bind(cmp_quotes,boost::cref(quote.GetID()),_1)))
							{
								aRemove.push_back(quote);
							}
						}

						TQuotes aNew;
						BOOST_FOREACH(const CQuotesProviderBase::CQuote& quote,g_aWatchedQuotes)
						{
							if(aTemp.end() == 
								std::find_if(aTemp.begin(),aTemp.end(),
								boost::bind(cmp_quotes,boost::cref(quote.GetID()),_1)))
							{
								aNew.push_back(quote);
							}
						}

						std::for_each(aRemove.begin(),aRemove.end(),boost::bind(&CQuotesProviderGoogleFinance::WatchForQuote,pProvider,_1,false));
						std::for_each(aNew.begin(),aNew.end(),boost::bind(&CQuotesProviderGoogleFinance::WatchForQuote,pProvider,_1,true));

						pProvider->RefreshAll();
					}

					return (TRUE);
				}
			}
		}
		return (FALSE);
	}
}

void CQuotesProviderGoogleFinance::ShowPropertyPage(WPARAM wp,OPTIONSDIALOGPAGE& odp)
{
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_DIALOG_OPT_GOOGLE_FINANCE);
	odp.pfnDlgProc = GoogleFinanceOptDlgProc;
// #if MIRANDA_VER >= 0x0600
	//odp.ptszTab = TranslateTS(const_cast<LPTSTR>(GetInfo().m_sName.c_str()));
	odp.ptszTab = const_cast<LPTSTR>(GetInfo().m_sName.c_str());
// #else
// 	tostringstream o;
// 	o << TranslateTS(QUOTES_PROTOCOL_NAME) << _T(" - ") << TranslateTS(GetInfo().m_sName.c_str());
// 	tstring sTitle = o.str();
// 	odp.ptszTitle = TranslateTS(const_cast<LPTSTR>(sTitle.c_str()));
// #endif
	CallService(MS_OPT_ADDPAGE,wp,reinterpret_cast<LPARAM>(&odp));	
}

void CQuotesProviderGoogleFinance::Accept(CQuotesProviderVisitor& visitor)const
{
	CQuotesProviderBase::Accept(visitor);
	visitor.Visit(*this);
}

void CQuotesProviderGoogleFinance::GetWatchedQuotes(TQuotes& raQuotes)const
{
	raQuotes.clear();
	BOOST_FOREACH(HANDLE hContact,m_aContacts)
	{
		tstring sID = Quotes_DBGetStringT(hContact,QUOTES_MODULE_NAME,DB_STR_QUOTE_ID);
		tstring sSymbol = Quotes_DBGetStringT(hContact,QUOTES_MODULE_NAME,DB_STR_QUOTE_SYMBOL,sID.c_str());
		tstring sDescr = Quotes_DBGetStringT(hContact,QUOTES_MODULE_NAME,DB_STR_QUOTE_DESCRIPTION);
		CQuotesProviderBase::CQuote quote(sID,sSymbol,sDescr);

		raQuotes.push_back(quote);
	}
}

namespace
{
	inline tstring get_quote_id(HANDLE hContact)
	{
		return Quotes_DBGetStringT(hContact,QUOTES_MODULE_NAME,DB_STR_QUOTE_ID);
	}

	inline bool is_quote_id_equal(HANDLE hContact,const tstring& sID)
	{
		return sID == get_quote_id(hContact);
	}
}

bool CQuotesProviderGoogleFinance::WatchForQuote(const CQuote& rQuote,bool bWatch)
{
	const tstring& sQuoteID = rQuote.GetID();
	TContracts::iterator i = std::find_if(m_aContacts.begin(),m_aContacts.end(),
		boost::bind(is_quote_id_equal,_1,sQuoteID));

	if((false == bWatch) && (i != m_aContacts.end()))
	{
		HANDLE hContact = *i;
		{// for CCritSection
			CGuard<CLightMutex> cs(m_cs);
			m_aContacts.erase(i);
		}

		CallService(MS_DB_CONTACT_DELETE,reinterpret_cast<WPARAM>(hContact),0);
		return true;
	}
	else if((true == bWatch) && (i == m_aContacts.end()))
	{
		HANDLE hContact = CreateNewContact(rQuote.GetSymbol());
		if(hContact)
		{
			DBWriteContactSettingTString(hContact,QUOTES_PROTOCOL_NAME,DB_STR_QUOTE_ID,sQuoteID.c_str());
			if(false == rQuote.GetName().empty())
			{
				DBWriteContactSettingTString(hContact,QUOTES_PROTOCOL_NAME,DB_STR_QUOTE_DESCRIPTION,rQuote.GetName().c_str());
			}

			return true;
		}
	}

	return false;
}

HANDLE CQuotesProviderGoogleFinance::GetContactByQuoteID(const tstring& rsQuoteID)const
{
	CGuard<CLightMutex> cs(m_cs);

	TContracts::const_iterator i = std::find_if(m_aContacts.begin(),m_aContacts.end(),
		boost::bind(std::equal_to<tstring>(),rsQuoteID,boost::bind(get_quote_id,_1)));
	if(i != m_aContacts.end())
	{
		return *i;
	}
	else
	{
		return NULL;
	}
}
