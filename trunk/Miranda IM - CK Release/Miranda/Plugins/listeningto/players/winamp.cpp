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


#include "..\commons.h"


static TCHAR *wcs[] = {
		_T("Winamp v1.x")
};

Winamp::Winamp(int index)
: CodeInjectionPlayer(index)
{ 
	m_name					= _T("Winamp");
	m_window_classes		= wcs;
	m_window_classes_num	= MAX_REGS(wcs);
	m_message_window_class	= MIRANDA_WINDOWCLASS _T(".Winamp");
	m_dll_name				= "gen_mlt";
}
