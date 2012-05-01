#ifndef __M_VOICESERVICE_H__
#define __M_VOICESERVICE_H__

/*
Voiceservice plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author Ricardo Pescuma Domenecci

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

#include "m_voice.h"

#define MIID_VOICESERVICE  { 0x7d64437, 0xef2e, 0x4f60, { 0xbb, 0x2d, 0x3c, 0x51, 0x8f, 0xe2, 0x4d, 0x63 } }


/*
This services are a mirror of the services/notifications in m_voice.h,
with the difference that that ones are to be used by protocols, and this ones
are to be used by plugins that can make calls to contacts in multiple protocols.
*/


/*
Notifies that a voice call changed state

wParam: const VOICE_CALL *
lParam: ignored
return: 0 on success
*/
#define MS_VOICESERVICE_STATE			"VoiceService/State"



struct VOICE_MODULE
{
	int cbSize;			// sizeof(VOICE_MODULE)
	char *name;			// The internal name of the plugin. All PS_* serivces (except PS_VOICE_GETINFO)
						// defined in m_voide.h need to be created based in this name. For example, 
						// PS_VOICE_CALL (/Voice/Call) need to be created as <name>/Voice/Call
	int flags;			// VOICE_* from m_voice.h
};
/*
Register a new plugin that can make/receive voice calls.

wParam: const VOICE_MODULE *
lParam: ignored
return: 0 on success
*/
#define MS_VOICESERVICE_REGISTER		"VoiceService/Register"


/*
Request a voice call to hContact.

wParam: (HANDLE) hContact
lParam: ignored
return: the number of option calls for a contact. If > 0, it can be called
*/
#define MS_VOICESERVICE_CAN_CALL			"VoiceService/CanCall"

/*
Request a voice call to hContact.

wParam: (HANDLE) hContact
lParam: (char *) Protocol or NULL to use any proto avaiable
return: 0 on success
*/
#define MS_VOICESERVICE_CALL				"VoiceService/Call"



#endif // __M_VOICESERVICE_H__
