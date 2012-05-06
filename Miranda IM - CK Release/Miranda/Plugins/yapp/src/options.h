#ifndef _OPTIONS_INC
#define _OPTIONS_INC

/*
YAPP for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
            Copyright (C) 2005-2006 Scott Ellis

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

typedef enum {PL_BOTTOMRIGHT=0, PL_BOTTOMLEFT=1, PL_TOPRIGHT=2, PL_TOPLEFT=3} PopupLocation;
typedef enum {PAV_NONE=0, PAV_LEFT=1, PAV_RIGHT=2} PopupAvLayout;
typedef enum {PT_NONE=0, PT_LEFT=1, PT_RIGHT=2, PT_WITHAV=3} PopupTimeLayout;
typedef struct {
	int win_width, win_max_height, av_size; //tweety
	int default_timeout;
	PopupLocation location;
	int opacity;
	bool border;
	bool round, av_round;
	bool animate;
	bool trans_bg;
	bool use_mim_monitor;
	bool right_icon;
	PopupAvLayout av_layout;
	bool disable_status[10];
	int text_indent;
	bool global_hover;
	PopupTimeLayout time_layout;
	bool disable_full_screen;
	bool drop_shadow;
	int sb_width;
	int padding, av_padding;
} Options;

extern Options options;

void InitOptions();
void LoadOptions();
void LoadModuleDependentOptions();
void DeinitOptions();

#endif
