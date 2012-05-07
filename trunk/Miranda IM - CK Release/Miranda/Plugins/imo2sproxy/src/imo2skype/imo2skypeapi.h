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

Simple wrapper for imo.im Webservice to SKYPE API to maintain compatibility with Skype-Plugins
*/

typedef void(*IMO2SCB)(char *pszMsg, void *pUser);
struct _tagIMOSAPI;
typedef struct _tagIMOSAPI IMOSAPI;

#define IMO2S_FLAG_CURRTIMESTAMP	1	// Use current timestamp on MSG receive
#define IMO2S_FLAG_ALLOWINTERACT	2	// Allow GUI interaction (CALL)

IMOSAPI *Imo2S_Init(IMO2SCB Callback, void *pUser, int iFlags);
void Imo2S_Exit (IMOSAPI *pInst);
void Imo2S_SetLog (IMOSAPI *pInst, FILE *fpLog);
int Imo2S_Login (IMOSAPI *pInst, char *pszUser, char *pszPass, char **ppszError);
void Imo2S_Logout(IMOSAPI *pInst);
int Imo2S_Send (IMOSAPI *pInst, char *pszMsg);
