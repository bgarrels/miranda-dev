#ifndef __BASS_INTERFACE_H__
#define __BASS_INTERFACE_H__

/*
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C) 2010, 2011 tico-tico
			Copyright (c) 1999-2012 Un4seen Developments Ltd.

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

#define MIRANDA_VER    0x0A00

#include <windows.h>
#include <newpluginapi.h>
#include <m_langpack.h>
#include <m_skin.h>
#include <m_database.h>
#include <m_options.h>
#include <m_clist.h>
#include <win2k.h>
#include <m_icolib.h>
#include <commctrl.h>

#include "m_toolbar.h"

#include "resource.h"
#include "../version.h"

#define BASSDEF(f) (WINAPI *f)
#include "../bass.h"

#define LOADBASSFUNCTION(f) (*((void**)&f)=(void*)GetProcAddress(hBass,#f))

#define MIID_BASSINT     {0x2f07ea05, 0x05b5, 0x4ff0, { 0x87, 0x5d, 0xc5, 0x90, 0xda, 0x2d, 0xda, 0xc1 }} //2f07ea05-05b5-4ff0-875d-c590da2ddac1

#define ModuleName "Bass Interface"

#define OPT_VOLUME		"Volume"
#define OPT_BASSPATH	"PathToDll"
#define OPT_MAXCHAN		"MaxChannels"
#define OPT_OUTDEVICE	"OutDevice"
#define OPT_QUIETTIME	"QuietTime"
#define OPT_TIME1		"Time1"
#define OPT_TIME2		"Time2"
#define OPT_STATUS		"StatMask"
#define OPT_PREVIEW		"Preview"
#define OPT_DEVOPEN		"DevOpen"

#endif
