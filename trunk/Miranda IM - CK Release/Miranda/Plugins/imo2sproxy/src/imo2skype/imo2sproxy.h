/*
Skype lugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2009-2012 leecher

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

#include <stdio.h>
#include "imo2skypeapi.h"
#ifndef WIN32
#define __cdecl
#endif

typedef struct
{
	int bVerbose;
	FILE *fpLog;
	int iFlags;
	int (__cdecl *logerror)( FILE *stream, const char *format, ...);
// FIXME: user+pass from cmdline, until there is a possibility for 
// a client to authenticate
	char *pszUser;
	char *pszPass;
} IMO2SPROXY_CFG;

typedef struct tag_imo2sproxy IMO2SPROXY;

struct tag_imo2sproxy
{
	int  (*Open)(IMO2SPROXY*);
	void (*Loop)(IMO2SPROXY*);
	void (*Exit)(IMO2SPROXY*);
};

void Imo2sproxy_Defaults (IMO2SPROXY_CFG *pCfg);
