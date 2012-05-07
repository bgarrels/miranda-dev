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

* Displays a window with an embedded browser in order to display the Voicechat
* flash from imo.im
* This example is bsaed on the work of Jeff Glatt (http://www.codeproject.com/KB/COM/cwebpage.aspx?msg=2852721)
* Credits go to Jeff Glatt for his excellent work.
*/

int W32Browser_Init(int bInitBrowser);
void W32Browser_Exit(void);

void *W32Browser_ShowHTMLStr(char *pszHTMLStr, int width, int height, char *pszTitle);
void W32Browser_CloseWindow (void *pWnd);
