/*
Popup Plus plugin for Miranda IM

Copyright	� 2002 Luca Santarelli,
			� 2004-2007 Victor Pavlychko
			� 2010 MPK
			� 2010 Merlin_de

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

File name      : $HeadURL: http://svn.miranda.im/mainrepo/popup/trunk/src/avatars_flash.h $
Revision       : $Revision: 1610 $
Last change on : $Date: 2010-06-23 00:55:13 +0300 (Ср, 23 июн 2010) $
Last change by : $Author: Merlin_de $

===============================================================================
*/

#ifndef __avatars_flash_h__
#define __avatars_flash_h__

class FlashAvatar: public PopupAvatar
{
public:
	FlashAvatar(HANDLE hContact);
	virtual ~FlashAvatar();
	virtual int activeFrameDelay() = 0;
	virtual void draw(MyBitmap *bmp, int x, int y, int w, int h, POPUPOPTIONS *options) = 0;
};

#endif // __avatars_flash_h__
