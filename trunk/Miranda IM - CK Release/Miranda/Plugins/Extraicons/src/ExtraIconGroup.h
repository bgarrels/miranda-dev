#ifndef __EXTRAICONGROUP_H__
#define __EXTRAICONGROUP_H__

/*
Extraicons service plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			 Copyright (C) 2009-2012 Ricardo Pescuma Domenecci

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

#include <vector>
#include "ExtraIcon.h"

class BaseExtraIcon;

class ExtraIconGroup : public ExtraIcon
{
public:
	ExtraIconGroup(const char *name);
	virtual ~ExtraIconGroup();

	virtual void addExtraIcon(BaseExtraIcon *extra);

	virtual void rebuildIcons();
	virtual void applyIcon(HANDLE hContact);
	virtual void onClick(HANDLE hContact);

	virtual int setIcon(int id, HANDLE hContact, void *icon);
	virtual void storeIcon(HANDLE hContact, void *icon);

	virtual const char *getDescription() const;
	virtual const char *getDescIcon() const;
	virtual int getType() const;

	virtual int getPosition() const;
	virtual void setSlot(int slot);

	std::vector<BaseExtraIcon*> items;

	virtual int ClistSetExtraIcon(HANDLE hContact, HANDLE hImage);

protected:
	std::string description;
	bool setValidExtraIcon;
	bool insideApply;

	virtual ExtraIcon * getCurrentItem(HANDLE hContact) const;
};

#endif // __EXTRAICONGROUP_H__
