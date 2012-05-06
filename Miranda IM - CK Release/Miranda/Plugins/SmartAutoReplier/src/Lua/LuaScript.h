/*
Smart Auto Replier (SAR) for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2004 - 2012 by Volodymyr M. Shcherbyna <volodymyr@shcherbyna.com>

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

#pragma once

#include "luainc.h"
#include "LuaBridge.h"

///	
#define DEFAULT_INDEX_INITIALIZER	0

class CLuaScript
{
public:
	CLuaScript(CLuaBridge & luaBridge);
	virtual ~CLuaScript();

public:

	static int LuaCallback (lua_State *lua);
	bool CompileScript(const char *szScript, size_t nLength);
	int  RegisterFunction(const char *szFuncName);   
	bool SelectScriptFunction(const char *szFunction);
	bool ScriptHasFunction(const char *szFunction);
	bool Run(int nRetValues = 0);

public:
	void AddParam(int nValue);
	void AddParam(float fValue);
	void AddParam(char *szValue);
	CLuaBridge & Bridge();

	int MethodsCount();
	
public:
	virtual int ScriptCalling(CLuaBridge & luaBridge, int nFunction) = 0;
	virtual void HandleReturns(CLuaBridge & luaBridge, const char * szFunction) = 0;

protected:

	CLuaBridge & m_luaBridge;

	const char * m_szFunction;

	int m_nMethods,
		m_nThisRef,
		m_nArgs;
};