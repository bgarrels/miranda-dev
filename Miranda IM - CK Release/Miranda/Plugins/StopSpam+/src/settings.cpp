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

#include "../headers.h"

//reading from database-------------
tstring db_usage::DBGetPluginSetting(std::string const &name, tstring const &defValue)
{
	DBVARIANT dbv;
	if(DBGetContactSettingTString(NULL, pluginName, name.c_str(), &dbv))
		return defValue;
	tstring value = dbv.ptszVal;
	DBFreeVariant(&dbv);
	return value;
}

#ifdef _UNICODE
std::string db_usage::DBGetPluginSetting(std::string const &name, std::string const &defValue)
{
	DBVARIANT dbv;
	if(DBGetContactSettingString(NULL, pluginName, name.c_str(), &dbv))
		return defValue;
	std::string value = dbv.pszVal;
	DBFreeVariant(&dbv);
	return value;
}
#endif

bool db_usage::DBGetPluginSetting(std::string const &name, bool const &defValue)
{
	return(0 != DBGetContactSettingByte(NULL, pluginName, name.c_str(), defValue?1:0));
}

DWORD db_usage::DBGetPluginSetting(std::string const &name, DWORD const &defValue)
{
	return DBGetContactSettingDword(NULL, pluginName, name.c_str(), defValue);
}

//writting to database--------------
void db_usage::DBSetPluginSetting(std::string const &name, tstring const &value)
{
	DBWriteContactSettingTString(NULL, pluginName, name.c_str(), value.c_str());
}

#ifdef _UNICODE
void db_usage::DBSetPluginSetting(std::string const &name, std::string const &value)
{
	DBWriteContactSettingString(NULL, pluginName, name.c_str(), value.c_str());
}
#endif

void db_usage::DBSetPluginSetting(std::string const &name, bool const &value)
{
	DBWriteContactSettingByte(NULL, pluginName, name.c_str(), value?1:0);
}

void db_usage::DBSetPluginSetting(std::string const &name, DWORD const &value)
{
	DBWriteContactSettingDword(NULL, pluginName, name.c_str(),value);
}

Settings *plSets;
