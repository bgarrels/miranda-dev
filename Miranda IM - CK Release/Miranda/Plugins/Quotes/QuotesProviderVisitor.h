#ifndef __7fca59e7_17b2_4849_bd7a_02c7675f2d76_QuotesProviderVisitor_h__
#define __7fca59e7_17b2_4849_bd7a_02c7675f2d76_QuotesProviderVisitor_h__

class CQuotesProviderBase;
class CQuotesProviderDukasCopy;
class CQuotesProviderGoogle;
class CQuotesProviderGoogleFinance;

class CQuotesProviderVisitor
{
public:
	CQuotesProviderVisitor(){}
	virtual ~CQuotesProviderVisitor(){}

	virtual void Visit(const CQuotesProviderBase& rProvider){}
	virtual void Visit(const CQuotesProviderDukasCopy& rProvider){}
	virtual void Visit(const CQuotesProviderGoogle& rProvider){}
	virtual void Visit(const CQuotesProviderGoogleFinance& rProvider){}
};

#endif //__7fca59e7_17b2_4849_bd7a_02c7675f2d76_QuotesProviderVisitor_h__
