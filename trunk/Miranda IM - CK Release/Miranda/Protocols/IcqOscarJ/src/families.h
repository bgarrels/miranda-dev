#ifndef __FAMILIES_H
#define __FAMILIES_H

/*
ICQ plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright © 2001-2004 Richard Hughes
			Copyright © 2002-2004 Martin Öberg, Sam Kothari, Robert Rainwater
			Copyright © 2004-2010 Joe Kucera, Bio
			Copyright © 2010-2012 Borkra, Georg Hazan

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

*  Declaration for handlers of Channel 2 SNAC Families
*/

struct message_ack_params 
{
  BYTE bType;
  DWORD dwUin;
  DWORD dwMsgID1;
  DWORD dwMsgID2;
  directconnect *pDC;
  WORD wCookie;
  int msgType;
  BYTE bFlags;
};

#define MAT_SERVER_ADVANCED 0
#define MAT_DIRECT          1


/* handleMessageTypes(): mMsgFlags constants */
#define MTF_DIRECT          1
#define MTF_PLUGIN          2
#define MTF_STATUS_EXTENDED 4


struct UserInfoRecordItem 
{
  WORD wTLV;
  int dbType;
  char *szDbSetting;
};

/*---------* Functions *---------------*/

int getPluginTypeIdLen(int nTypeID);
void packPluginTypeId(icq_packet *packet, int nTypeID);

#define BUL_ALLCONTACTS   0
#define BUL_VISIBLE       1
#define BUL_INVISIBLE     2
#define BUL_TEMPVISIBLE   4


#endif /* __FAMILIES_H */