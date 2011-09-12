#ifndef __3e6cb4ec_fc47_468f_a2c8_a77941176bc9_QuotesProviderBase_h__
#define __3e6cb4ec_fc47_468f_a2c8_a77941176bc9_QuotesProviderBase_h__

#include "iquotesprovider.h"
#include <vector>
#include "LightMutex.h"

class CQuotesProviderBase : public IQuotesProvider
{
public:
	class CQuote
	{
	public:
		CQuote(const tstring& rsID = _T(""),const tstring& rsSymbol = _T(""),const tstring& rsName = _T(""))
			: m_sSymbol(rsSymbol),m_sName(rsName),m_sID(rsID){}

		const tstring& GetSymbol()const{return m_sSymbol;}
		const tstring& GetName()const{return m_sName;}
		const tstring& GetID()const{return m_sID;}

	private:
		tstring m_sSymbol;
		tstring m_sName;
		tstring m_sID;
	};

	class CQuoteSection
	{
	public:
		typedef std::vector<CQuoteSection> TSections;
		typedef std::vector<CQuote> TQuotes;

	public:
		CQuoteSection(const tstring& rsName = _T(""),const TSections& raSections = TSections(),const TQuotes& raQuotes = TQuotes())
			: m_sName(rsName),m_aSections(raSections),m_aQuotes(raQuotes){}

		const tstring& GetName()const
		{return m_sName;}

		size_t GetSectionCount()const
		{return m_aSections.size();}
		CQuoteSection GetSection(size_t nIndex)const
		{return ((nIndex < m_aSections.size()) ? m_aSections[nIndex] : CQuoteSection());}

		size_t GetQuoteCount()const
		{return m_aQuotes.size();}
		CQuote GetQuote(size_t nIndex)const
		{return ((nIndex < m_aQuotes.size()) ? m_aQuotes[nIndex] : CQuote());}

	private:
		tstring m_sName;
		TSections m_aSections;
		TQuotes m_aQuotes;
	};

protected:
	typedef std::vector<HANDLE> TContracts;

public:
	struct CXMLFileInfo;

public:
	CQuotesProviderBase(void);
	~CQuotesProviderBase(void);

	const CQuoteSection& GetQuotes()const;
// 	void SetSettingsEvent();

	virtual const CProviderInfo& GetInfo()const;
	virtual void AddContact(HANDLE hContact);
	virtual void DeleteContact(HANDLE hContact);
	virtual void Run();
	virtual void Accept(CQuotesProviderVisitor& visitor)const;
	virtual void RefreshAll();
	virtual void RefreshContact(HANDLE hContact);
	virtual void SetContactExtraIcon(HANDLE hContact)const;

protected:
	const tstring& GetURL()const;
	HANDLE CreateNewContact(const tstring& rsName);
	static bool IsOnline();
	static void SetContactStatus(HANDLE hContact,int nNewStatus);
	void WriteContactRate(HANDLE hContact,double dRate,const tstring& rsSymbol = _T(""));

private:
	virtual void RefreshQuotes(TContracts& anContacts) = 0;

private:
	virtual void OnEndRun();

private:
	CXMLFileInfo* GetXMLFileInfo()const;

protected:
	TContracts m_aContacts;
	mutable CLightMutex m_cs;

private:
	typedef boost::scoped_ptr<CXMLFileInfo> TXMLFileInfoPtr;
	mutable TXMLFileInfoPtr m_pXMLInfo;
	HANDLE m_hEventSettingsChanged;
	HANDLE m_hEventRefreshContact;
	tstring m_sContactListFormat;
	tstring m_sStatusMsgFormat;
	tstring m_sTendencyFormat;
	TContracts m_aRefreshingContacts;
	bool m_bRefreshInProgress;
};

#endif //__3e6cb4ec_fc47_468f_a2c8_a77941176bc9_QuotesProviderBase_h__
