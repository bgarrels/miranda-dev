/*
Skype protocol for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright © 2008-2012 leecher, tweety, jls17,
						Laurent Marechal (aka Peorth)
						Gennady Feldman (aka Gena01) 

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

int RegisterOptions(WPARAM wParam, LPARAM lParam);
/*
 * OptionsDlgProc
 *
 * This callback function is called, when the options dialog in Miranda is shown
 * The function contains all necessary stuff to process the options in the dialog
 * and store them in the database, when changed, and fill out the settings-dialog
 * correctly according to the current settings
 */
INT_PTR CALLBACK OptionsDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK OptionsDefaultDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK OptionsAdvancedDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK OptionsProxyDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK OptPopupDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*
* Procedure to call when the option page is asked
*
*/
int OnDetailsInit( WPARAM wParam, LPARAM lParam );

/*
* Dialog to change avatar in user details.
*
*
*/
INT_PTR CALLBACK AvatarDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

/*
* Dialog to change infos in user details.
*
*
*/
INT_PTR CALLBACK DetailsDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

/*
 * Helper functions
 *
 */
void DoAutoDetect(HWND dlg);

;