#ifndef M_STOPSPAM_H__
#define M_STOPSPAM_H__

/*
Stopspam plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors Roman Miklashevsky, A. Petkevich, Kosh&chka, persei

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

===============================================================================
*/

#define CS_NOTPASSED					0
#define CS_PASSED						1

//check is contact pass the stopspam
//wParam=(HANDLE)hContact
//lParam=0
//returns a "Contact Stae" flag
#define MS_STOPSPAM_CONTACTPASSED		"StopSpam/IsContactPassed"

//remove all temporary contacts from db
//wParam=0
//lParam=0
//returns 0
#define MS_STOPSPAM_REMTEMPCONTACTS		"StopSpam/RemoveTempContacts"

#endif