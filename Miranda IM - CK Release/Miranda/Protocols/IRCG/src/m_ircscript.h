/*
IRC plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright (C) 2003-2005 Jurgen Persson
			Copyright (C) 2007-2009 George Hazan
			Copyright (C) 2010-2012 Borkra

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

/*
	This file is a description of the interface between the Miranda Scripting 
	Plugin (MSP) and IRC. It is mainly an internal draft.
	
	The MSP is a powerful PHP based scripting engine 
	which can automate many tasks in Miranda IM. With the implementation of 
	this interface IRC can also benefit from the advantages MSP brings. The
	interface has been constructed by the respective authors of IRC and MSP.

	The idea is that MSP and IRC should communicate via the interface outlined herein, 
	but MSP should "package" the data in attractive functions that can be easily 
	used by eager scripters.

	Note that m_chat.h should be #included as the events defined therein is used in this interface
	
	The IRC plugin communicates with MSP mainly through 8 (2 x 4) services which are 
	illustrated by the following matrix (MSP has 4 services and IRC 4 services):


	--------------------------------------------------
	|						|                         |
	|  Incoming raw data	|		Data going        |
	|     from server		|		to GUI		      |
	|	(before processing)	|    (after processing)   |
	|						|                         |
	--------------------------------------------------
	|						|                         |
	|  Outgoing raw data	|		Data going        |
	|     to server			|		from GUI	      |
	|	(after processing)	|    (before processing)  |
	|						|                         |
	--------------------------------------------------
	  
	(processing refers to the IRC plugin reacting to different commands)
	
*/


//------------------------- HANDLING RAW MESSAGES ------------------------
/*	

	The two leftmost boxes are related to data sent to and from the server in RAW form.
	(a typical RAW message looks like this: ":nick!user@host COMMAND PARAMETERS")

	It is very useful for scripters to be able to manipulate and add RAW streams of data.
	
	The below structure is used when RAW data is generated and IRC must trigger an 
	event in MSP so the scripts can do their magic.

	If the data is modified make sure to use MS_SYSTEM_GET_MMI and then mmi_realloc() to
	allocate from the same heap. Do not free the data.

  	wParam=  Module name
	lParam=  (LPARAM)( char* )pszRaw
	returns nonzero if the message should be dropped
*/

/*
	NOTE! IRC will implement two services which also uses the same parameters:

	"<Module m_name>/InsertRawIn"			//used for inserting a "fake" incoming RAW event
	"<Module m_name>/InsertRawOut"			//used for inserting a "fake" outgoing RAW event

*/


#define MS_MBOT_IRC_RAW_IN  "MBot/IrcRawIn"							
#define MS_MBOT_IRC_RAW_OUT  "MBot/IrcRawOut"						


//------------------------- HANDLING GUI MESSAGES 1 ------------------------
/*	

	The top rightmost box is related to data sent to the gui 
	(currently chat.dll) in the form of GCEVENT's. (see m_chat.h)

	While maybe not as useful to the scripter, it can be useful to be able to 
	"tap into" messages to the GUI from the IRC plugin. 

	If the data is modified make sure to realloc() or similar.

  	wParam=  (WPARAM) (WPARAM_GUI_IN) &wgi
	lParam=  (LPARAM)(GCEVENT *)&gce     // see m_chat.h
	returns nonzero if the message should be dropped
*/

/*
	NOTE! IRC will implement a service which also uses the same parameters:

	"<Module m_name>/InsertGuiIn"		//used for inserting a "fake" event to the GUI

*/
typedef struct MBOT_GUI_WPARAM_IN_STRUCT
{
	const char* pszModule;
	WPARAM wParam;
}WPARAM_GUI_IN;

#define MS_MBOT_IRC_GUI_IN  "MBot/IrcGuiIn"				


//------------------------- HANDLING GUI MESSAGES 2 ------------------------
/*	

	The bottom rightmost box is related to data sent from the gui 
	(currently chat.dll) in the form of GCHOOKS's. Originally this is carried out in a hook,
	but for MSP this is done via a service instead.

	It can be useful for scripters to simulate user actions, like entering text, closing the window etc

	If the data is modified make sure to realloc() or similar.

  	wParam=  Module name
	lParam=  (LPARAM)(GCHOOK *)gch     // see m_chat.h
	returns nonzero if the message should be dropped
*/

/*
	NOTE! IRC will implement a service which also uses the same parameters:

	"<Module m_name>/InsertGuiOut"		//used for inserting a "fake" outgoing event to the GUI

*/

#define MS_MBOT_IRC_GUI_OUT  "MBot/IrcGuiOut"				

#define MS_MBOT_REGISTERIRC "MBot/RegisterIRCModule"

