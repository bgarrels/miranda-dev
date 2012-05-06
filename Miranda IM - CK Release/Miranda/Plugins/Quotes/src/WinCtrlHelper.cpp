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

#include "stdafx.h"
#include "QuotesProviderVisitorFormatSpecificator.h"
#include "IQuotesProvider.h"
#include "resource.h"
#include "ModuleInfo.h"

namespace
{
	INT_PTR CALLBACK VariableListDlgProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp) 
	{
		switch(msg)
		{
		case WM_INITDIALOG:
			{
				TranslateDialogDefault(hWnd);
				const IQuotesProvider* pProvider = reinterpret_cast<const IQuotesProvider*>(lp);
				CQuotesProviderVisitorFormatSpecificator visitor;
				pProvider->Accept(visitor);

				tostringstream o;
				const CQuotesProviderVisitorFormatSpecificator::TFormatSpecificators& raSpec = visitor.GetSpecificators();
				std::for_each(raSpec.begin(),raSpec.end(),
					[&o](const CQuotesProviderVisitorFormatSpecificator::CFormatSpecificator& spec)
					{
						o << spec.m_sSymbol << _T('\t') << spec.m_sDesc << _T("\r\n");
					});
				::SetDlgItemText(hWnd,IDC_EDIT_VARIABLE,o.str().c_str());
			}
			break;
		case WM_COMMAND:
			if(BN_CLICKED == HIWORD(wp) && (IDOK == LOWORD(wp) || IDCANCEL == LOWORD(wp)))
			{
				::EndDialog(hWnd,IDOK);
			}
			break;
		}

		return FALSE;
	}
}

void show_variable_list(HWND hwndParent,const IQuotesProvider* pProvider)
{
	::DialogBoxParam(CModuleInfo::GetModuleHandle(),
		MAKEINTRESOURCE(IDD_DIALOG_VARIABLE_LIST),
		hwndParent,
		VariableListDlgProc,
		reinterpret_cast<LPARAM>(pProvider));
}
