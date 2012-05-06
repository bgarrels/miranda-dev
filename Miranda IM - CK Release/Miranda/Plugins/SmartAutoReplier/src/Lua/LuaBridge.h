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

class CLuaBridge
{
public:
	CLuaBridge(void);
	~CLuaBridge(void);

public:
	static int LuaDebugMessage(lua_State * pContext);
	static void HandleLuaAsssert(lua_State * pContext);

	bool RunScript(const char *szScript, size_t nLength, const char *szFuncName = NULL);	
	bool ExecuteFunction(int nArgs, int nReturns  = 0);
	operator lua_State *();

protected:
	lua_State *m_pLuaContext;
};

class CLuaStack
{
public:
	CLuaStack(CLuaBridge & luaBridge) : m_pState(NULL), m_nTopIndex(0)
	{
		m_pState	 = (lua_State*)luaBridge;
		m_nTopIndex	 = lua_gettop(m_pState);
	}

	virtual ~CLuaStack()
	{
		lua_settop(m_pState, m_nTopIndex);
	}

protected:

	lua_State *m_pState;
	int m_nTopIndex;
};

class CLuaTableThis
{
public:
	CLuaTableThis(CLuaBridge & luaBridge, int nRef) : m_nOldRef(0), m_luaBridge(luaBridge)
	{
		lua_State *state = (lua_State*)m_luaBridge;

		lua_getglobal(state, "this");
		m_nOldRef = luaL_ref(state, LUA_REGISTRYINDEX);

		lua_rawgeti(state, LUA_REGISTRYINDEX, nRef);
		lua_setglobal(state, "this");
	}

	virtual ~CLuaTableThis()
	{
		lua_State *state = (lua_State*)m_luaBridge;

		if (m_nOldRef > 0)
		{
			lua_rawgeti(state, LUA_REGISTRYINDEX, m_nOldRef);
			lua_setglobal (state, "this");
			luaL_unref(state, LUA_REGISTRYINDEX, m_nOldRef);
		}
	}

protected:
   int m_nOldRef;
   CLuaBridge & m_luaBridge;
};

