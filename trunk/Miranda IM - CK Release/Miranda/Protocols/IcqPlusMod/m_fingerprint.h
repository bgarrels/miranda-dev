/*
Based on Miranda plugin template, originally by Richard Hughes
http://miranda-icq.sourceforge.net/

Miranda IM: the free IM client for Microsoft Windows

Copyright 2000-2006 Miranda ICQ/IM project,
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
*/

/************************************************************************/
/*  Author: Artem Shpynov aka FYR     mailto:shpynov@nm.ru              */
/*  icons by Angeli-Ka                                                  */
/*  January 12, 2006													*/
/************************************************************************/


/*
 *   FINGERPRINT PLUGIN SERVICES HEADER
 */

/*
 *   Service SameClients MS_FP_SAMECLIENTS
 *	 wParam - char * first MirVer value
 *   lParam - char * second MirVer value
 *	 return pointer to char string - client desription (DO NOT DESTROY) if clients are same otherwise NULL
 */
#define MS_FP_SAMECLIENTS "Fingerprint/SameClients"

/*
 *   ServiceGetClientIcon MS_FP_GETCLIENTICON
 *	 wParam - char * MirVer value to get client for.
 *   lParam - int noCopy - if wParam is equal to "1"  will return icon handler without copiing icon.
 *   the returned in this case handler is static valid  only till next service call.
 */
#define MS_FP_GETCLIENTICON "Fingerprint/GetClientIcon"


// Plugin UUIDs for New plugin loader
// req. 0.7.18+ core
#define MIID_FINGERPRINT				{0xFFF4B77A, 0xCE40, 0x11DB, { 0xA5, 0xCD, 0x06, 0xA7, 0x55, 0xD8, 0x95, 0x93 }} //FFF4B77A-CE40-11DB-A5CD-06A755D89593
#define MIID_FINGERPRINT_MOD			{0xBAC0BBBE, 0xCE40, 0x11DB, { 0xA1, 0x1E, 0x72, 0xA6, 0x55, 0xD8, 0x95, 0x93 }} //BAC0BBBE-CE40-11DB-A11E-72A655D89593
#define MIID_FINGERPRINT_MOD_FULL		{0x0afe5bbb, 0xce62, 0x11db, { 0x83, 0x14, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 }} //0afe5bbb-ce62-11db-8314-0800200c9a66
#define MIID_FINGERPRINT_MOD_STANDARD	{0x0afe5abc, 0xce62, 0x11db, { 0x83, 0x14, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 }} //0afe5abc-ce62-11db-8314-0800200c9a66
#define MIID_FINGERPRINT_MOD_LITE		{0x0afe5def, 0xce62, 0x11db, { 0x83, 0x14, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 }} //0afe5def-ce62-11db-8314-0800200c9a66
#define MIID_FINGERPRINT_MOD_CUSTOM		{0x0afe5bad, 0xce62, 0x11db, { 0x83, 0x14, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 }} //0afe5bad-ce62-11db-8314-0800200c9a66


