#ifndef __9d0dac0c_12e4_46ce_809a_db6dc7d6f269_ExtraImages_h__
#define __9d0dac0c_12e4_46ce_809a_db6dc7d6f269_ExtraImages_h__

/*
Quotes (Forex) plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
Copyright (C) Dioksin

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

#include "LightMutex.h"

class CExtraImages : private boost::noncopyable
{
public:
	enum EImageIndex
	{
		eiUp = 0,
		eiDown = 1,
		eiNotChanged = 2,
		eiEmpty = 3,
		ImageCount = 3
	};

private:
	CExtraImages();
	~CExtraImages();

public:
	static CExtraImages& GetInstance();

	void RebuildExtraImages();
	bool SetContactExtraImage(HANDLE hContact,EImageIndex nIndex)const;

private:
	mutable CLightMutex m_lmExtraImages;
	HANDLE m_ahExtraImages[ImageCount];
	HANDLE m_hExtraIcons;
	bool m_bExtraImagesInit;
	int m_nSlot;
};

int QuotesEventFunc_onExtraImageListRebuild(WPARAM wp,LPARAM lp);
int QuotesEventFunc_onExtraImageApply(WPARAM wp,LPARAM lp);

#endif //__9d0dac0c_12e4_46ce_809a_db6dc7d6f269_ExtraImages_h__
