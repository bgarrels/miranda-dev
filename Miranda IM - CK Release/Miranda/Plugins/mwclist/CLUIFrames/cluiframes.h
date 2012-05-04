/*
Copyright (C) 2000-2  Richard Hughes, Roland Rabien & Tristan Van de Vreede

Miranda IM: the free IM client for Microsoft* Windows*

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

int LoadCLUIFramesModule(void);
int UnLoadCLUIFramesModule(void);
int CLUIFramesGetMinHeight();
int CLUIFramesOnClistResize(WPARAM wParam,LPARAM lParam);

typedef struct tagProtocolData {
	char *RealName;
	int protopos;
	boolean show;
} ProtocolData;

#define OFFSET_PROTOPOS 200
#define OFFSET_VISIBLE 400

#define CLUIFrameTitleBarClassName				_T("CLUIFrameTitleBar")
#define CLUIFrameModule							"CLUIFrames"

//integrated menu module
#define MS_INT_MENUMEASUREITEM "CLUIFrames/IntMenuMeasureItem"
#define MS_INT_MENUDRAWITEM "CLUIFrames/IntMenuDrawItem"
#define MS_INT_MENUPROCESSCOMMAND "CLUIFrames/IntMenuProcessCommand"
#define MS_INT_MODIFYMENUITEM "CLUIFrames/IntModifyMenuItem"

	

