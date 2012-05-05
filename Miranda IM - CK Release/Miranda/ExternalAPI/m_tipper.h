/*
TipperYM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
            Copyright (C) 2006-2009 Scott Ellis
            Copyright (C) 2007-2011 Jan Holub

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

* Tipper is internally unicode and requires unicows.dll to function correctly on
  Windows 95/98/ME so you'll find a lot of wchar_t stuff in here.
*/


/*		Translation function type
 *		Use hContact, module and setting to read your db value(s) and put the resulting string into buff
 *		return buff if the translation was successful, or return 0 for failure
 */
typedef TCHAR *(TranslateFunc)(HANDLE hContact, const char *module, const char *setting_or_prefix, TCHAR *buff, int bufflen);

typedef struct {
	TranslateFunc *transFunc;	// address of your translation function (see typedef above)
	const TCHAR *swzName;		// make sure this is unique, and DO NOT translate it
	DWORD id;					// will be overwritten by Tipper - do not use
} DBVTranslation;

/*		add a translation to tipper
 *		wParam not used
 *		lParam = (DBVTranslation *)translation
 */
#define MS_TIPPER_ADDTRANSLATION	 "Tipper/AddTranslation"

/*		unicode extension to the basic functionality
 *		wParam - optional (wchar_t *)text for text-only tips
 *		lParam - (CLCINFOTIP *)infoTip
 */
#define MS_TIPPER_SHOWTIPW				"mToolTip/ShowTipW"