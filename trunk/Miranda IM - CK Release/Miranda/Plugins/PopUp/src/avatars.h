#ifndef __avatars_h__
#define __avatars_h__

/*
Popup Plus plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	2002 Luca Santarelli
							2004-2007 Victor Pavlychko
							2010-2012 Merlin_de, MPK

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

class PopupAvatar
{
protected:
	int width, height;
	bool bIsAnimated, bIsValid;

public:
	PopupAvatar(HANDLE hContact) {}
	virtual ~PopupAvatar() {}
	virtual int activeFrameDelay() = 0;
	virtual void draw(MyBitmap *bmp, int x, int y, int w, int h, POPUPOPTIONS *options) = 0;

	inline bool isValid() { return bIsValid; }
	inline void invalidate() { bIsValid = false; }
	inline bool isAnimated() { return bIsAnimated; }
	inline int getWidth() { return width; }
	inline int getHeight() { return height; }
	
	static PopupAvatar *create(HANDLE hContact);
};

#endif // __avatars_h__
