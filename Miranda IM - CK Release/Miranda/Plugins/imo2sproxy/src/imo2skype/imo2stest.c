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

* Simple test application for imo2skypeapi
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "imo2skypeapi.h"

// -----------------------------------------------------------------------------
// Static
// -----------------------------------------------------------------------------
static void Recv(char *pszMsg, void *pUser)
{
	printf ("< %s\n", pszMsg);
}

// -----------------------------------------------------------------------------
// EIP
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	int iRet = -1;
	IMOSAPI *pInst;
	char *pszError = NULL;

	if (argc<3)
	{
		printf ("Usage: %s [username] [password]\n", argv[0]);
		return -1;
	}

	if (!(pInst = Imo2S_Init(Recv, NULL,0)))
	{
		fprintf (stderr, "Failed initialization.\n");
		return -1;
	}

	/* Logon */
	if (Imo2S_Login(pInst, argv[1], argv[2], &pszError) == 1)
	{
		/* Dispatch loop */
		char szLine[4096]={0};

		while (gets(szLine))
		{
			Imo2S_Send (pInst, szLine);
			memset (szLine, 0, sizeof(szLine));
		}
	}
	else
	{
		fprintf (stderr, "Login failed: %s\n", pszError);
		return -1;
	}

	Imo2S_Exit (pInst);
	return iRet;
}
