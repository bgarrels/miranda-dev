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

* eventhooker.cpp - Copyright (C) Miklashevsky Roman
*/

#include <list>
#include "eventhooker.h"

namespace miranda
{
	namespace
	{
		std::list<EventHooker*> eventHookerList;
	}

	EventHooker::EventHooker(std::string name, MIRANDAHOOK fun) : name_(name), fun_(fun), handle_(0) 
	{
		eventHookerList.push_back(this);
	}

	EventHooker::~EventHooker()
	{
		eventHookerList.remove(this);
	}

	void EventHooker::Hook() 
	{
		handle_ = HookEvent(name_.c_str(), fun_);
	}

	void EventHooker::Unhook()
	{
		if(handle_)
		{
			UnhookEvent(handle_);
			handle_ = 0;
		}
	}

	void EventHooker::HookAll()
	{
		for(std::list<EventHooker*>::iterator it = eventHookerList.begin(); it != eventHookerList.end(); ++it)
		{
			(*it)->Hook();
		}
	}

	void EventHooker::UnhookAll()
	{
		for(std::list<EventHooker*>::iterator it = eventHookerList.begin(); it != eventHookerList.end(); ++it)
		{
			(*it)->Unhook();
		}
	}
}
