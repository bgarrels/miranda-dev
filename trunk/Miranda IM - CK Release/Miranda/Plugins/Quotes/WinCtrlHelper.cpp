#include "stdafx.h"
#include "QuotesProviderVisitorFormatSpecificator.h"
#include "IQuotesProvider.h"

namespace
{
	inline void format_spec(tostream& o,const CQuotesProviderVisitorFormatSpecificator::CFormatSpecificator& spec)
	{
		o << spec.m_sSymbol << _T('\t') << spec.m_sDesc << _T('\n');
	}
}

void show_variable_list(HWND hwndParent,const IQuotesProvider* pProvider)
{
	CQuotesProviderVisitorFormatSpecificator visitor;
	pProvider->Accept(visitor);

	tostringstream o;
	const CQuotesProviderVisitorFormatSpecificator::TFormatSpecificators& raSpec = visitor.GetSpecificators();
	std::for_each(raSpec.begin(),raSpec.end(),boost::bind(format_spec,boost::ref(o),_1));
	MessageBox(hwndParent,o.str().c_str(),TranslateT("Variable List"),MB_OK|MB_ICONINFORMATION);
}
