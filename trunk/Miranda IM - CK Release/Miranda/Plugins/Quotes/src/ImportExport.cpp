/*
Quotes (Forex) plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) Dioksin

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

#include "StdAfx.h"
#include "ImportExport.h"
#include "ModuleInfo.h"
#include "QuotesProviders.h"
#ifdef TEST_IMPORT_EXPORT
#include "m_Quotes.h"
#endif
#include "IXMLEngine.h"
#include "Base64.h"
#include "EconomicRateInfo.h"
#include "IQuotesProvider.h"
#include "QuotesProviderVisitor.h"
#include "QuotesProviderDukasCopy.h"
#include "QuotesProviderGoogle.h"
#include "QuotesProviderGoogleFinance.h"
#include "Locale.h"

namespace
{
	LPCTSTR g_pszXmlValue = _T("Value");
	LPCTSTR g_pszXmlName = _T("Name");
	LPCTSTR g_pszXmlSetting = _T("Setting");
	LPCTSTR g_pszXmlModule = _T("Module");
	LPCTSTR g_pszXmlContact = _T("Contact");
	LPCTSTR g_pszXmlContacts = _T("Contacts");
	LPCTSTR g_pszXmlType = _T("type");
	LPCTSTR g_pszXmlTypeByte = _T("byte");
	LPCTSTR g_pszXmlTypeWord = _T("word");
	LPCTSTR g_pszXmlTypeDword = _T("dword");
	LPCTSTR g_pszXmlTypeAsciiz = _T("asciiz");
	LPCTSTR g_pszXmlTypeWchar = _T("wchar");
	LPCTSTR g_pszXmlTypeUtf8 = _T("utf8");
	LPCTSTR g_pszXmlTypeBlob = _T("blob");

	struct CEnumContext
	{
		CModuleInfo::TXMLEnginePtr m_pXmlEngine;
		IXMLNode::TXMLNodePtr m_pNode;
		HANDLE m_hContact;
		LPCSTR m_pszModule;
	};

	struct mir_safety_dbvar
	{
		mir_safety_dbvar(DBVARIANT* p) : m_p(p){}
		~mir_safety_dbvar(){DBFreeVariant(m_p);}
		DBVARIANT* m_p;
	};

	static int enum_contact_settings(const char* szSetting,LPARAM lp)
	{
// 		USES_CONVERSION;
		CEnumContext* ctx = reinterpret_cast<CEnumContext*>(lp);

		DBVARIANT dbv;
		DBCONTACTGETSETTING cgs;

		cgs.szModule = ctx->m_pszModule;
		cgs.szSetting = szSetting;
		cgs.pValue = &dbv;
		if(0 == CallService(MS_DB_CONTACT_GETSETTING,
							reinterpret_cast<WPARAM>(ctx->m_hContact),
							reinterpret_cast<LPARAM>(&cgs)))
		{
			mir_safety_dbvar sdbvar(&dbv);

			tstring sType;
			tostringstream sValue;
			sValue.imbue(GetSystemLocale());

			switch(dbv.type)
			{
			case DBVT_BYTE:
				sValue << dbv.bVal;
				sType = g_pszXmlTypeByte;
				break;
			case DBVT_WORD:
				sValue << dbv.wVal;
				sType = g_pszXmlTypeWord;
				break;
			case DBVT_DWORD:
				sValue << dbv.dVal;
				sType = g_pszXmlTypeDword;
				break;
			case DBVT_ASCIIZ:
				sType = g_pszXmlTypeAsciiz;
				if(dbv.pszVal)
				{
					sValue << dbv.pszVal;
// 					mir_safe_string<char> mss(mir_utf8encode(dbv.pszVal));
// 					if(mss.m_p)
// 					{
// 						sValue << mss.m_p;
// 					}
				}
				break;
			case DBVT_WCHAR:
				sType = g_pszXmlTypeWchar;
				if(dbv.pwszVal)
				{
					sValue << dbv.pwszVal;
// 					mir_safe_string<char> mss(mir_utf8encodeW(dbv.pwszVal));
// 					if(mss.m_p)
// 					{
// 						sValue << mss.m_p;
// 					}
				}
				break;
			case DBVT_UTF8:
				sType = g_pszXmlTypeUtf8;
				if(dbv.pszVal)
				{
					sValue << dbv.pszVal;
				}
				break;
			case DBVT_BLOB:
				sType = g_pszXmlTypeBlob;
				if(dbv.pbVal)
				{
					std::vector<char> buf;
					if(true == base64::encode(dbv.pbVal,dbv.cpbVal,buf))
					{
						buf.push_back('\0');
						sValue << &*buf.begin();
					}
				}
				break;
			}

// 			mir_safe_string<char> mssSetting(mir_utf8encode(szSetting));
// 			if(mssSetting.m_p)
			{
				IXMLNode::TXMLNodePtr pXmlSet = ctx->m_pXmlEngine->CreateNode(g_pszXmlSetting,tstring());
				if(pXmlSet)
				{
					IXMLNode::TXMLNodePtr pXmlName = ctx->m_pXmlEngine->CreateNode(g_pszXmlName,quotes_a2t(szSetting));

					IXMLNode::TXMLNodePtr pXmlValue = ctx->m_pXmlEngine->CreateNode(g_pszXmlValue,sValue.str());
					if(pXmlName && pXmlValue)
					{
						pXmlValue->AddAttribute(g_pszXmlType,sType);

						pXmlSet->AddChild(pXmlName);
						pXmlSet->AddChild(pXmlValue);
						ctx->m_pNode->AddChild(pXmlSet);
					}
				}
			}
		}		

		return 0;
	}

	int EnumDbModules(const char* szModuleName, DWORD ofsModuleName, LPARAM lp)
	{
// 		USES_CONVERSION;
		CEnumContext* ctx = reinterpret_cast<CEnumContext*>(lp);
		IXMLNode::TXMLNodePtr pXml = ctx->m_pNode;
		IXMLNode::TXMLNodePtr pModule = ctx->m_pXmlEngine->CreateNode(g_pszXmlModule,quotes_a2t(szModuleName)/*A2CT(szModuleName)*/);
		if(pModule)
		{
			ctx->m_pszModule = szModuleName;
			ctx->m_pNode = pModule;

			DBCONTACTENUMSETTINGS dbces;
			dbces.pfnEnumProc = &enum_contact_settings;
			dbces.szModule = szModuleName;
			dbces.lParam = reinterpret_cast<LPARAM>(ctx);

			CallService(MS_DB_CONTACT_ENUMSETTINGS,reinterpret_cast<WPARAM>(ctx->m_hContact),reinterpret_cast<LPARAM>(&dbces));
			if(pModule->GetChildCount() > 0)
			{
				pXml->AddChild(pModule);
			}
			ctx->m_pNode = pXml;
		}

		return 0;
	}

	IXMLNode::TXMLNodePtr export_contact(HANDLE hContact,const CModuleInfo::TXMLEnginePtr& pXmlEngine)
	{
		IXMLNode::TXMLNodePtr pNode = pXmlEngine->CreateNode(g_pszXmlContact,tstring());
		if(pNode)
		{
			CEnumContext ctx;
			ctx.m_pXmlEngine = pXmlEngine;
			ctx.m_pNode = pNode;
			ctx.m_hContact = hContact;

			CallService(MS_DB_MODULES_ENUM,reinterpret_cast<WPARAM>(&ctx),reinterpret_cast<LPARAM>(EnumDbModules));
		}
		return pNode;
	}

	LPCTSTR prepare_filter(LPTSTR pszBuffer,size_t cBuffer)
	{
		LPTSTR p = pszBuffer;
		LPCTSTR pszXml = TranslateT("Xml File (*.xml)");
		lstrcpyn(p,pszXml, (int)cBuffer);
		size_t nLen = (int)lstrlen(pszXml)+1;
		p+= nLen;
		if(nLen < cBuffer)
		{
			lstrcpyn(p,_T("*.xml"),(int)(cBuffer-nLen));
			p+= 6;
			nLen += 6;
		}

		if(nLen < cBuffer)
		{
			LPCTSTR pszAll = TranslateT("All files (*.*)");
			lstrcpyn(p,pszAll,(int)(cBuffer-nLen));
			size_t n = lstrlen(pszAll)+1;
			nLen += n;
			p+= n;
		}

		if(nLen < cBuffer)
		{
			lstrcpyn(p,_T("*.*"),(int)(cBuffer-nLen));
			p+= 4;
			nLen += 4;
		}

		if(nLen < cBuffer)
		{
			*p = _T('\0');
		}

		return pszBuffer;
	}

	bool show_open_file_dialog(bool bOpen,tstring& rsFile)
	{
		TCHAR szBuffer[MAX_PATH];
		TCHAR szFilter[MAX_PATH];
		OPENFILENAME ofn;
		memset(&ofn,0,sizeof(ofn));

		ofn.lStructSize       = sizeof(OPENFILENAME);

		ofn.hwndOwner         = NULL;
		ofn.lpstrFilter       = prepare_filter(szFilter,MAX_PATH);
		ofn.Flags             = OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|OFN_EXPLORER;
		ofn.lpstrDefExt		  = _T("xml");
		if(true == bOpen)
		{
			ofn.Flags |= OFN_FILEMUSTEXIST;
		}
		else
		{
			ofn.Flags |= OFN_OVERWRITEPROMPT;
		}
		ofn.nMaxFile          = MAX_PATH;
		ofn.lpstrFile         = szBuffer;
		ofn.lpstrFile[0]	  =	_T('\0');

		if(bOpen)
		{
			if(FALSE == GetOpenFileName(&ofn))
			{
				return false;
			}
		}
		else
		{
			if(FALSE == GetSaveFileName(&ofn))
			{
				return false;
			}
		}

		rsFile = szBuffer;

		return true;
	}
}

INT_PTR Quotes_Export(WPARAM wp,LPARAM lp)
{
// 	USES_CONVERSION;

	tstring sFileName;
	const char* pszFile = reinterpret_cast<const char*>(lp);
	if(NULL == pszFile)
	{
		if(false == show_open_file_dialog(false,sFileName))
		{
			return -1;
		}
	}
	else
	{		
		sFileName = quotes_a2t(pszFile);//A2CT(pszFile);
	}	

	CModuleInfo::TXMLEnginePtr pXmlEngine = CModuleInfo::GetInstance().GetXMLEnginePtr();
	CModuleInfo::TQuotesProvidersPtr pProviders = CModuleInfo::GetInstance().GetQuoteProvidersPtr();
	IXMLNode::TXMLNodePtr pRoot = pXmlEngine->CreateNode(g_pszXmlContacts,tstring());
	HANDLE hContact = reinterpret_cast<HANDLE>(wp);
	if(hContact)
	{
		CQuotesProviders::TQuotesProviderPtr pProvider = pProviders->GetContactProviderPtr(hContact);
		if(pProvider)
		{
			IXMLNode::TXMLNodePtr pNode = export_contact(hContact,pXmlEngine);
			if(pNode)
			{
				pRoot->AddChild(pNode);
			}
		}
	}
	else
	{
		for(hContact = reinterpret_cast<HANDLE>(CallService(MS_DB_CONTACT_FINDFIRST,0,0));hContact;hContact = reinterpret_cast<HANDLE>(CallService(MS_DB_CONTACT_FINDNEXT,reinterpret_cast<WPARAM>(hContact),0)))
		{
			CQuotesProviders::TQuotesProviderPtr pProvider = pProviders->GetContactProviderPtr(hContact);
			if(pProvider)
			{
				IXMLNode::TXMLNodePtr pNode = export_contact(hContact,pXmlEngine);
				if(pNode)
				{
					pRoot->AddChild(pNode);
				}
			}
		}
	}

	return ((true == pXmlEngine->SaveFile(sFileName,pRoot)) ? 0 : 1);
}

namespace
{
	bool set_contact_settings(HANDLE hContact,DBCONTACTWRITESETTING& dbs)
	{
		assert(DBVT_DELETED != dbs.value.type);
		return (0 == CallService(MS_DB_CONTACT_WRITESETTING,reinterpret_cast<WPARAM>(hContact),
								 reinterpret_cast<LPARAM>(&dbs)));
	}

	bool handle_module(HANDLE hContact,const IXMLNode::TXMLNodePtr& pXmlModule,UINT nFlags)
	{
// 		USES_CONVERSION;

		size_t cCreatedRecords = 0;
		tstring sModuleName = pXmlModule->GetText();
		if(false == sModuleName.empty())
		{
			DBCONTACTWRITESETTING dbs;
			std::string s = quotes_t2a(sModuleName.c_str());
			dbs.szModule = s.c_str();//T2CA(sModuleName.c_str());

			bool bCListModule = 0 == quotes_stricmp(sModuleName.c_str(),_T("CList"));

			size_t cChild = pXmlModule->GetChildCount();
			for(size_t i = 0;i < cChild;++i)
			{
				IXMLNode::TXMLNodePtr pSetting = pXmlModule->GetChildNode(i);
				tstring sSetting = pSetting->GetName();
				if(0 == quotes_stricmp(g_pszXmlSetting,sSetting.c_str()))
				{
					size_t cSetChild = pSetting->GetChildCount();
					if(cSetChild >= 2)
					{
						tstring sName;
						tstring sValue;
						tstring sType;
						for(size_t i = 0;i < cSetChild;++i)
						{
							IXMLNode::TXMLNodePtr pNode = pSetting->GetChildNode(i);
							tstring sNode = pNode->GetName();
							if(0 == quotes_stricmp(g_pszXmlName,sNode.c_str()))
							{
								sName = pNode->GetText();
							}
							else if(0 == quotes_stricmp(g_pszXmlValue,sNode.c_str()))
							{
								sValue = pNode->GetText();
								sType = pNode->GetAttributeValue(g_pszXmlType);
							}
						}

						if((false == sName.empty()) && (false == sType.empty()))
						{
							std::string s = quotes_t2a(sName.c_str());
							dbs.szSetting = s.c_str();//T2CA(sName.c_str());
							if(0 == quotes_stricmp(g_pszXmlTypeByte,sType.c_str()))
							{
								tistringstream in(sValue.c_str());
								in.imbue(GetSystemLocale());
								dbs.value.cVal = in.get();
								if(in.good() && in.eof())
								{
									dbs.value.type = DBVT_BYTE;
									if(set_contact_settings(hContact,dbs))
									{
										++cCreatedRecords;
									}
								}
							}
							else if(0 == quotes_stricmp(g_pszXmlTypeWord,sType.c_str()))
							{	
								tistringstream in(sValue.c_str());
								in.imbue(GetSystemLocale());
								in >> dbs.value.wVal;
								if(in.good() || in.eof())
								{
									dbs.value.type = DBVT_WORD;
									if(set_contact_settings(hContact,dbs))
									{
										++cCreatedRecords;
									}
								}
							}
							else if(0 == quotes_stricmp(g_pszXmlTypeDword,sType.c_str()))
							{		
								tistringstream in(sValue.c_str());
								in.imbue(GetSystemLocale());
								in >> dbs.value.dVal;
								if(in.good() || in.eof())
								{
									dbs.value.type = DBVT_DWORD;
									if(set_contact_settings(hContact,dbs))
									{
										++cCreatedRecords;
									}
								}
							}
							else if(0 == quotes_stricmp(g_pszXmlTypeAsciiz,sType.c_str()))
							{	
								CT2A v(sValue.c_str());
								dbs.value.pszVal = v;
								dbs.value.type = DBVT_ASCIIZ;
								if(set_contact_settings(hContact,dbs))
								{
									++cCreatedRecords;
								}
							}
							else if(0 == quotes_stricmp(g_pszXmlTypeUtf8,sType.c_str()))
							{					
								dbs.value.pszVal = mir_utf8encodeT(sValue.c_str());
								dbs.value.type = DBVT_UTF8;
								if(set_contact_settings(hContact,dbs))
								{
									++cCreatedRecords;
								}
								mir_free(dbs.value.pszVal);
							}
							else if(0 == quotes_stricmp(g_pszXmlTypeWchar,sType.c_str()))
							{			
								CT2W val(sValue.c_str());
								dbs.value.pwszVal = val;
								dbs.value.type = DBVT_WCHAR;
								if(set_contact_settings(hContact,dbs))
								{
									++cCreatedRecords;
								}
								mir_free(dbs.value.pwszVal);
							}
							else if(0 == quotes_stricmp(g_pszXmlTypeBlob,sType.c_str()))
							{
								std::vector<BYTE> blob_buf;
								std::string p = quotes_t2a(sValue.c_str());//T2A(sValue.c_str());
								if(true == base64::decode(p.c_str(),lstrlenA(p.c_str()),blob_buf))
								{
									dbs.value.pbVal = &*blob_buf.begin();
									dbs.value.cpbVal = (WORD)blob_buf.size();
									dbs.value.type = DBVT_BLOB;

									if(set_contact_settings(hContact,dbs))
									{
										++cCreatedRecords;
									}
								}
							}

							if((true == bCListModule) && (0 == quotes_stricmp(sName.c_str(),_T("Group"))))
							{
								CallService(MS_CLIST_GROUPCREATE,NULL,reinterpret_cast<LPARAM>(sValue.c_str()));
							}
						}
					}
				}		
			}
		}

		return true;
	}

	size_t count_contacts(const IXMLNode::TXMLNodePtr& pXmlRoot,bool bInContactsGroup)
	{
		size_t cContacts = 0;
		size_t cChild = pXmlRoot->GetChildCount();
		for(size_t i = 0;i < cChild;++i)
		{
			IXMLNode::TXMLNodePtr pNode = pXmlRoot->GetChildNode(i);
			tstring sName = pNode->GetName();
			if(false == bInContactsGroup)
			{
				if(0 == quotes_stricmp(g_pszXmlContacts,sName.c_str()))
				{
					cContacts += count_contacts(pNode,true);
				}
				else
				{
					cContacts += count_contacts(pNode,false);
				}
			}
			else
			{
				if(0 == quotes_stricmp(g_pszXmlContact,sName.c_str()))
				{
					++ cContacts;
				}
			}
		}

		return cContacts;
	}

	struct CImportContext
	{
		CImportContext(size_t cTotalContacts) : m_cTotalContacts(cTotalContacts),m_cHandledContacts(0),m_nFlags(0){}

		size_t m_cTotalContacts;
		size_t m_cHandledContacts;
		UINT m_nFlags;
	};

	struct CContactState
	{
		CContactState() : m_hContact(NULL),m_bNewContact(false){}
		HANDLE m_hContact;
		CQuotesProviders::TQuotesProviderPtr m_pProvider;
		bool m_bNewContact;
	};

	IXMLNode::TXMLNodePtr find_quotes_module(const IXMLNode::TXMLNodePtr& pXmlContact)
	{
// 		USES_CONVERSION;
// 		LPCTSTR pszQuotes = A2T(QUOTES_MODULE_NAME);
		static const tstring g_sQuotes = quotes_a2t(QUOTES_MODULE_NAME);
		size_t cChild = pXmlContact->GetChildCount();
		for(size_t i = 0;i < cChild;++i)
		{
			IXMLNode::TXMLNodePtr pNode = pXmlContact->GetChildNode(i);
			tstring sName = pNode->GetName();
			if((0 == quotes_stricmp(g_pszXmlModule,sName.c_str()))
				&& (0 == quotes_stricmp(g_sQuotes.c_str(),pNode->GetText().c_str())))
			{
				return pNode;
			}
		}

		return IXMLNode::TXMLNodePtr();
	}

	typedef std::pair<tstring,tstring> TNameValue;//first is name,second is value
	TNameValue parse_setting_node(const IXMLNode::TXMLNodePtr& pXmlSetting)
	{
		assert(pXmlSetting);
		
		tstring sName,sValue;
		size_t cSettingChildItems = pXmlSetting->GetChildCount();
		for(size_t j = 0;j < cSettingChildItems;++j)
		{
			IXMLNode::TXMLNodePtr pXMLSetChild = pXmlSetting->GetChildNode(j);
			if(pXMLSetChild)
			{
				if(0 == quotes_stricmp(g_pszXmlName,pXMLSetChild->GetName().c_str()))
				{
					sName = pXMLSetChild->GetText();
				}
				else if(0 == quotes_stricmp(g_pszXmlValue,pXMLSetChild->GetName().c_str()))
				{
					sValue = pXMLSetChild->GetText();
				}
			}
		}

		return std::make_pair(sName,sValue);
	}

	CQuotesProviders::TQuotesProviderPtr find_provider(const IXMLNode::TXMLNodePtr& pXmlQuotesModule)
	{
// 		USES_CONVERSION;
		static const tstring g_sQuotesProvider = quotes_a2t(DB_STR_QUOTE_PROVIDER);//A2CT(DB_STR_QUOTE_PROVIDER);
		size_t cChild = pXmlQuotesModule->GetChildCount();
		for(size_t i = 0;i < cChild;++i)
		{
			IXMLNode::TXMLNodePtr pXMLSetting = pXmlQuotesModule->GetChildNode(i);
			if(pXMLSetting && (0 == quotes_stricmp(g_pszXmlSetting,pXMLSetting->GetName().c_str())))
			{
				TNameValue Item = parse_setting_node(pXMLSetting);
				if((0 == quotes_stricmp(g_sQuotesProvider.c_str(),Item.first.c_str())) && (false == Item.second.empty()))
				{
					return CModuleInfo::GetInstance().GetQuoteProvidersPtr()->FindProvider(Item.second);
				}
			}
		}

		return CQuotesProviders::TQuotesProviderPtr();
	}

	bool get_contact_state(const IXMLNode::TXMLNodePtr& pXmlContact,CContactState& cst)
	{
		class visitor : public CQuotesProviderVisitor
		{
		public:
			visitor(const IXMLNode::TXMLNodePtr& pXmlQuotes) 
				: m_hContact(NULL),m_pXmlQuotes(pXmlQuotes){}

			HANDLE GetContact()const{return m_hContact;}

		private:
			virtual void Visit(const CQuotesProviderDukasCopy& rProvider)
			{
				tstring sQuoteID = GetXMLNodeValue(DB_STR_QUOTE_ID);
				if(false == sQuoteID.empty())
				{
					m_hContact = rProvider.GetContactByQuoteID(sQuoteID);
				}
			}

			virtual void Visit(const CQuotesProviderGoogle& rProvider)
			{
// 				USES_CONVERSION;
				static const tstring g_sFromID = quotes_a2t(DB_STR_FROM_ID);//A2CT(DB_STR_FROM_ID);
				static const tstring g_sToID = quotes_a2t(DB_STR_TO_ID);//A2CT(DB_STR_TO_ID);

				tstring sFromID;
				tstring sToID;
				size_t cChild = m_pXmlQuotes->GetChildCount();
				for(size_t i = 0;i < cChild;++i)
				{
					IXMLNode::TXMLNodePtr pNode = m_pXmlQuotes->GetChildNode(i);
					if(pNode && (0 == quotes_stricmp(g_pszXmlSetting, pNode->GetName().c_str())))
					{
						TNameValue Item = parse_setting_node(pNode);
						if(0 == quotes_stricmp(g_sFromID.c_str(),Item.first.c_str()))
						{
							sFromID = Item.second;
						}
						else if(0 == quotes_stricmp(g_sToID.c_str(),Item.first.c_str()))
						{
							sToID = Item.second;
						}
					}
				}

				if((false == sFromID.empty()) && (false == sToID.empty()))
				{
					m_hContact = rProvider.GetContactByID(sFromID,sToID);
				}
			}

			virtual void Visit(const CQuotesProviderFinance& rProvider)
			{
				tstring sQuoteID = GetXMLNodeValue(DB_STR_QUOTE_ID);
				if(false == sQuoteID.empty())
				{
					m_hContact = rProvider.GetContactByQuoteID(sQuoteID);
				}
			}

			tstring GetXMLNodeValue(const char* pszXMLNodeName)const
			{
// 				USES_CONVERSION;
				tstring sXMLNodeName = quotes_a2t(pszXMLNodeName);//A2CT(pszXMLNodeName);

				tstring sValue;
				size_t cChild = m_pXmlQuotes->GetChildCount();
				for(size_t i = 0;i < cChild;++i)
				{
					IXMLNode::TXMLNodePtr pNode = m_pXmlQuotes->GetChildNode(i);
					if(pNode && (0 == quotes_stricmp(g_pszXmlSetting, pNode->GetName().c_str())))
					{
						TNameValue Item = parse_setting_node(pNode);
						if(0 == quotes_stricmp(Item.first.c_str(),sXMLNodeName.c_str()))
						{
							sValue = Item.second;
							break;
						}
					}
				}

				return sValue;
			}

		private:
			HANDLE m_hContact;
			IXMLNode::TXMLNodePtr m_pXmlQuotes;
		};

		IXMLNode::TXMLNodePtr pXmlQuotes = find_quotes_module(pXmlContact);
		if(pXmlQuotes)
		{
			cst.m_pProvider = find_provider(pXmlQuotes);
			if(cst.m_pProvider)
			{
				visitor vs(pXmlQuotes);
				cst.m_pProvider->Accept(vs);
				cst.m_hContact = vs.GetContact();
				return true;
			}
		}

		return false;
	}

	bool import_contact(const IXMLNode::TXMLNodePtr& pXmlContact,CImportContext& impctx)
	{
		++ impctx.m_cHandledContacts;

		CContactState cst;
		bool bResult = get_contact_state(pXmlContact,cst);
		if(bResult)
		{
			if(NULL == cst.m_hContact)
			{
				cst.m_hContact = reinterpret_cast<HANDLE>(CallService(MS_DB_CONTACT_ADD,0,0));
				cst.m_bNewContact = true;
			}
			else if(impctx.m_nFlags&QUOTES_IMPORT_SKIP_EXISTING_CONTACTS)
			{
				return true;
			}

			if(cst.m_hContact)
			{
				size_t cChild = pXmlContact->GetChildCount();
				for(size_t i = 0;i < cChild && bResult;++i)
				{
					IXMLNode::TXMLNodePtr pNode = pXmlContact->GetChildNode(i);
					tstring sName = pNode->GetName();
					if(0 == quotes_stricmp(g_pszXmlModule,sName.c_str()))
					{
						bResult &= handle_module(cst.m_hContact,pNode,impctx.m_nFlags);
					}
				}

				if(cst.m_bNewContact && bResult)
				{
					cst.m_pProvider->AddContact(cst.m_hContact);
					cst.m_pProvider->RefreshContact(cst.m_hContact);
				}
			}
			else
			{
				bResult = false;
			}
		}

		return bResult;

	}

	size_t import_contacts(const IXMLNode::TXMLNodePtr& pXmlContacts,CImportContext& impctx)
	{
		size_t cContacts = 0;
		size_t cChild = pXmlContacts->GetChildCount();
		for(size_t i = 0;i < cChild;++i)
		{
			IXMLNode::TXMLNodePtr pNode = pXmlContacts->GetChildNode(i);
			tstring sName = pNode->GetName();
			if(0 == quotes_stricmp(g_pszXmlContact,sName.c_str()))
			{
				if(true == import_contact(pNode,impctx))
				{
					++ cContacts;
				}
			}
		}

		return cContacts;

	}

	size_t handle_contacts_node(const IXMLNode::TXMLNodePtr& pXmlRoot,CImportContext& impctx)
	{
		size_t cContacts = 0;
		size_t cChild = pXmlRoot->GetChildCount();
		for(size_t i = 0;i < cChild;++i)
		{
			IXMLNode::TXMLNodePtr pNode = pXmlRoot->GetChildNode(i);
			tstring sName = pNode->GetName();
			if(0 == quotes_stricmp(g_pszXmlContacts,sName.c_str()))
			{
				cContacts += import_contacts(pNode,impctx);
			}
			else
			{
				cContacts += handle_contacts_node(pNode,impctx);
			}
		}

		return cContacts;

	}

	bool do_import(const IXMLNode::TXMLNodePtr& pXmlRoot,UINT nFlags)
	{
		CImportContext imctx(count_contacts(pXmlRoot,false));
		imctx.m_cHandledContacts = 0;
		imctx.m_nFlags = nFlags;

		return (handle_contacts_node(pXmlRoot,imctx) > 0);
	}
}

INT_PTR Quotes_Import(WPARAM wp,LPARAM lp)
{
// 	USES_CONVERSION;

	tstring sFileName;
	const char* pszFile = reinterpret_cast<const char*>(lp);
	if(NULL == pszFile)
	{
		if(false == show_open_file_dialog(true,sFileName))
		{
			return -1;
		}
	}
	else
	{		
		sFileName = quotes_a2t(pszFile);//A2CT(pszFile);
	}	
	CModuleInfo::TXMLEnginePtr pXmlEngine = CModuleInfo::GetInstance().GetXMLEnginePtr();
	IXMLNode::TXMLNodePtr pXmlRoot = pXmlEngine->LoadFile(sFileName);
	if(pXmlRoot)
	{
		return ((true == do_import(pXmlRoot,wp)) ? 0 : 1);
	}

	return 1;
}

#ifdef TEST_IMPORT_EXPORT
INT_PTR QuotesMenu_ImportAll(WPARAM wp,LPARAM lp)
{
	return CallService(MS_QUOTES_IMPORT,0,0);
}

INT_PTR QuotesMenu_ExportAll(WPARAM wp,LPARAM lp)
{
	return CallService(MS_QUOTES_EXPORT,0,0);
}
#endif

