#ifndef __7D8F07A4_72AF_4838_9C5C_6FDFF57D0DC6_m_Quotes_h__
#define __7D8F07A4_72AF_4838_9C5C_6FDFF57D0DC6_m_Quotes_h__

/*
Miranda IM: the free IM client for Microsoft* Windows*

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

/*
 *	Export the contact (or all contacts) to xml file
 *		wParam = (WPARAM)(HANDLE)hContact to export or 0 to export all contacts
 *		lParam = (LPARAM)(const char*)pszFileName - pointer to file name to export or
 *				  0 in this case the dialog to select a file to export would be shown
 *		returns 0 if export was successfull, 
 *			    -1 if user canceled export and 
 *				value greater than zero if error occurred during exporting
 */
#define MS_QUOTES_EXPORT "Quotes/Export"

/*
 *	Import the contact (or all contacts) from xml file
 *		wParam = flags
 *		lParam = (LPARAM)(const char*)pszFileName - pointer to file name to import or
 *				  0 in this case the dialog to select a file to import would be shown
 *		returns 0 if import was successfull, 
 *			    -1 if user canceled import and 
 *				value greater than zero if error occurred during importing
 */

// if contact(s) exists user would be asked to overwrite these contacts
// #define QUOTES_IMPORT_PROMPT_TO_OVERWRITE_EXISTING_CONTACTS 0x0000
// if contact(s) exists it would be overwrite without any prompt
// #define QUOTES_IMPORT_SILENT_OVERWRITE_EXISTING_CONTACTS 0x0001
// if contact(s) exists during importing it would be ignored
#define QUOTES_IMPORT_SKIP_EXISTING_CONTACTS 0x0002

#define MS_QUOTES_IMPORT "Quotes/Import"


#endif //__7D8F07A4_72AF_4838_9C5C_6FDFF57D0DC6_m_Quotes_h__
