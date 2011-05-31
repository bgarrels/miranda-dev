/* 
ListeningTo plugin for Miranda IM
==========================================================================
Copyright	(C) 2005-2011 Ricardo Pescuma Domenecci
			(C) 2010-2011 Merlin_de
==========================================================================

in case you accept the pre-condition,
this is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the
Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#pragma once

int m_log(const TCHAR *function, const TCHAR *fmt, ...);

class GenericPlayer : public Player
{
protected:
	ATOM cWndclass;

public:
	GenericPlayer(int index);
	virtual ~GenericPlayer();

	WCHAR received[1024];
	WCHAR last_received[1024];
	void ProcessReceived();
	void NewData(const WCHAR *data, size_t len);
};
