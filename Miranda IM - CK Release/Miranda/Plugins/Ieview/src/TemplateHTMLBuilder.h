/*
IEView Plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author 
			Copyright (C) 2005-2010  Piotr Piastucki

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

class TemplateHTMLBuilder;

#ifndef TEMPLATEHTMLBUILDER_INCLUDED
#define TEMPLATEHTMLBUILDER_INCLUDED

#include "HTMLBuilder.h"
#include "Template.h"

class TemplateHTMLBuilder:public HTMLBuilder
{
protected:
		char *timestampToString(DWORD dwFlags, time_t check, int mode);
		time_t 		startedTime;
		time_t 		getStartedTime();
		const char *groupTemplate;
		time_t 		flashAvatarsTime[2];
		char *		flashAvatars[2];
		const char *getFlashAvatar(const TCHAR *file, int index);
		char *getAvatar(HANDLE hContact, const char *szProto);
		void buildHeadTemplate(IEView *, IEVIEWEVENT *event, ProtocolSettings* protoSettings);
		void appendEventTemplate(IEView *, IEVIEWEVENT *event, ProtocolSettings* protoSettings);
		virtual TemplateMap *getTemplateMap(ProtocolSettings *);
		virtual int getFlags(ProtocolSettings *);
public:
		TemplateHTMLBuilder();
		virtual ~TemplateHTMLBuilder();
//	void buildHead(IEView *, IEVIEWEVENT *event);
};

#endif
