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


#ifndef __MUSIC_H__
# define __MUSIC_H__

// First non polling ones
#define WATRACK			0
#define GENERIC			1

#define FIRST_PLAYER	2

#define WLM				2
#define WMP				3
#define WINAMP			4
#define ITUNES			5
#define FOOBAR			6
#define MRADIO			7
//#define VIDEOLAN		8
#define NUM_PLAYERS		8


void InitMusic();
void FreeMusic();
void EnableDisablePlayers();

int ChangedListeningInfo();
LISTENINGTOINFO * GetListeningInfo();

// Helper functions to players
void FreeListeningInfo(LISTENINGTOINFO *lti);
void CopyListeningInfo(LISTENINGTOINFO *dest, const LISTENINGTOINFO * const src);


#endif // __MUSIC_H__
