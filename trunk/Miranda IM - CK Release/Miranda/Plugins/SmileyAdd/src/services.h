#ifndef SMILEYADD_SERVICES_H_
#define SMILEYADD_SERVICES_H_

/*
SmileyAdd Plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author 
			Copyright (C) 2006 - 2012 Boris Krasnovskiy

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

#include "m_smileyadd.h"
#include "m_smileyadd_deprecated.h"

// service commands
INT_PTR ReplaceSmileysCommand(WPARAM wParam, LPARAM lParam);
INT_PTR GetSmileyIconCommand(WPARAM wParam, LPARAM lParam);
INT_PTR ShowSmileySelectionCommand(WPARAM wParam, LPARAM lParam);
INT_PTR GetInfoCommand(WPARAM wParam, LPARAM);
INT_PTR GetInfoCommand2(WPARAM wParam, LPARAM);
INT_PTR ParseText(WPARAM wParam, LPARAM lParam);
INT_PTR ParseTextW(WPARAM wParam, LPARAM lParam);
INT_PTR RegisterPack(WPARAM wParam, LPARAM lParam);
INT_PTR ParseTextBatch(WPARAM wParam, LPARAM lParam);
INT_PTR FreeTextBatch(WPARAM wParam, LPARAM lParam);
INT_PTR CustomCatMenu(WPARAM, LPARAM lParam);
int RebuildContactMenu(WPARAM wParam, LPARAM);
INT_PTR ReloadPack(WPARAM, LPARAM lParam);
INT_PTR LoadContactSmileys(WPARAM, LPARAM lParam);
int AccountListChanged(WPARAM wParam, LPARAM lParam);
int DbSettingChanged(WPARAM wParam, LPARAM lParam);

SmileyPackType* GetSmileyPack(const char* proto, HANDLE hContact = NULL, SmileyPackCType** smlc = NULL);


#endif // SMILEYADD_SERVICES_H_

