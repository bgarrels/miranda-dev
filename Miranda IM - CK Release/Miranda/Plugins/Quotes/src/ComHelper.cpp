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
#include "ComHelper.h"
#include "Log.h"
#include "WinCtrlHelper.h"

tstring ComException2Msg(_com_error& e,const tstring& rsAdditionalInfo)
{
	HRESULT hError = e.Error();
	tostringstream o;
	if(false == rsAdditionalInfo.empty())
	{
		o << rsAdditionalInfo << "\n";
	}

	o << e.ErrorMessage() << _T(" (") << std::hex << hError << _T(")");

	IErrorInfo* p = e.ErrorInfo();
	CComPtr<IErrorInfo> pErrorInfo(p);
	if(NULL != p)
	{
		p->Release();
	}

	if(pErrorInfo)
	{
		o << _T("\n") << e.Description();
	}

	return o.str();
}

void ShowComError(_com_error& e,const tstring& rsAdditionalInfo)
{
	tstring sErrorMsg = ComException2Msg(e,rsAdditionalInfo);
	LogIt(Error,sErrorMsg);
	Quotes_MessageBox(NULL,sErrorMsg.c_str(),MB_OK|MB_ICONERROR);
}


