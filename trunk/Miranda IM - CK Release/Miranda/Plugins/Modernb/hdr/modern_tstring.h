/*
Author Artem Shpynov aka FYR

Miranda IM: the free IM client for Microsoft* Windows*

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

===============================================================================
*/

#ifndef UNICODE	//FIXME Build without UNICODE flag
#define _AtlGetConversionACP() CP_THREAD_ACP
#endif

#include "MString.h"

#ifdef UNICODE
typedef CMStringA astring;
typedef CMStringW wstring;

class mbstring : public astring
{
	// It is prohibited to initialize by char* outside, use L"xxx"
private:
	mbstring( const char * pChar )		: astring( pChar ) {};
	mbstring& operator=( const char * pChar  )	{ this->operator =( pChar ); return *this; }

public:
	mbstring()							: astring() {};
	mbstring( const mbstring& uStr )	: astring( uStr ) {};

		
	mbstring( const wstring&  tStr )	{ *this = tStr.c_str(); }
	mbstring& operator=( const wstring&  tStr  )	{ this->operator =( tStr.c_str() ); return *this; }

	mbstring( const wchar_t * wChar );
	mbstring& operator=( const astring&  aStr  );
	mbstring& operator=( const wchar_t * wChar );
	operator wstring();
	operator astring();
};


class tstring : public wstring
{
public:
	tstring()							: wstring() {};
	tstring(const wchar_t * pwChar)		: wstring( pwChar ) {};
	
	
	tstring(const astring& aStr)		{	*this = aStr.c_str(); }
	tstring(const mbstring& utfStr)		{	*this = utfStr; }

	tstring(const char * pChar);
	tstring& operator=( const char * pChar );
	tstring& operator=( const astring& aStr );
	tstring& operator=( const mbstring& uStr );
	operator astring();
	operator mbstring()					{  return mbstring( this->c_str() ); }
};

#else
typedef CMStringA		astring;
typedef CMStringA		wstring;
typedef CMStringA		tstring;
typedef CMStringA		mbstring;

#endif
