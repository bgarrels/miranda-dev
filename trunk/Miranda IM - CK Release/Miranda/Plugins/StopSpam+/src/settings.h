/*
StopSpam+ plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
            Copyright (C) 2004-2011 Roman Miklashevsky
                                    A. Petkevich
                                    Kosh&chka
                                    persei


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

#include "../headers.h"

class db_usage
{
public:
	//reading from database
	static tstring DBGetPluginSetting(std::string const &name, tstring const &defValue);
#ifdef _UNICODE
	static std::string DBGetPluginSetting(std::string const &name, std::string const &defValue);
#endif
	static bool DBGetPluginSetting(std::string const &name, bool const &defValue);
	static DWORD DBGetPluginSetting(std::string const &name, DWORD const &defValue);
	//writting to database
	static void DBSetPluginSetting(std::string const &name, tstring const &value);
#ifdef _UNICODE
	static void DBSetPluginSetting(std::string const &name, std::string const &value);
#endif
	static void DBSetPluginSetting(std::string const &name, bool const &value);
	static void DBSetPluginSetting(std::string const &name, DWORD const &value);

};

template <typename T>
class db_setting
{
	std::string m_name;
	T m_defValue;
	T m_value;
public:
	db_setting(std::string const &name, T const &defValue):m_name(name),m_defValue(defValue)
	{
		m_value=db_usage::DBGetPluginSetting(m_name, m_defValue);
	}
	const T & GetDefault()const{return m_defValue;}
	void Set(T const &value)
	{
		m_value=value;
		db_usage::DBSetPluginSetting(m_name, m_value);
	}
	const T & Get()const{return m_value;}
	db_setting<T>& operator=(T const &value)
	{
		m_value=value;
		db_usage::DBSetPluginSetting(m_name, m_value);
		return *this;
	}
	operator T(){return m_value;}
	void SetResident(BOOL bResident){
		CallService(MS_DB_SETSETTINGRESIDENT, bResident, (LPARAM)(pluginName m_name.c_str()));
	}
};

class Settings
{
public:
	db_setting<tstring> Question;
	db_setting<tstring> AuthRepl;
	db_setting<tstring> Answer;
	db_setting<tstring> Congratulation;
	db_setting<std::string> DisabledProtoList;
	db_setting<bool> InfTalkProtection;
	db_setting<bool> AddPermanent;
	db_setting<DWORD> MaxQuestCount;
	db_setting<bool> HandleAuthReq;
	db_setting<bool> AnswNotCaseSens;
	db_setting<tstring> AnswSplitString;
	db_setting<bool> RemTmpAll;
	db_setting<bool> HistLog;

	Settings():Question("Question",TranslateTS(_T("Spammers made me to install small anti-spam system you are now speaking with. ")
		_T("Please reply \"nospam\" without quotes and spaces if you want to contact me.\r\n")))
	,AuthRepl("AuthReply",TranslateTS(_T("StopSpam: send a message and reply to a anti-spam bot question.\r\n")))
	,Answer("Answer",TranslateT("nospam"))
	,Congratulation("Congratulation",TranslateTS(_T("Congratulations! You just passed human/robot test. Now you can write me a message.\r\n")))
	,DisabledProtoList("DisabledProtoList","MetaContacts RSSNews")
	,InfTalkProtection("InfTalkProtection", 1)
	,AddPermanent("AddPermanent", 0)
	,HandleAuthReq("HandleAuthReq", 0)
	,MaxQuestCount("MaxQuestCount", 2)
	,AnswNotCaseSens("AnswNotCaseSens", 1)
	,AnswSplitString("AnswSplitString",_T("|"))
	,RemTmpAll("RemTmpAll", 1)
	,HistLog("HistLog", 0)
	{
		const std::string& str = DisabledProtoList.Get();
		if ( !str.empty() && *(str.rbegin()) != ' ' )
			DisabledProtoList=DisabledProtoList.Get()+' ';
	}
	bool ProtoDisabled(std::string proto)
	{
		return std::string::npos != DisabledProtoList.Get().find(proto + " ");
	}
};

extern Settings *plSets;
