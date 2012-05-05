#ifndef name_day_core_h
#define name_day_core_h

/*
Name_day plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2005 Tibor Szabo

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

#include "../calendar/calendar.h"

// WInd00z stuff
#include <windows.h>


class name_day_core_t
{
	public:
							name_day_core_t				();
							~name_day_core_t			();
		
			void			perform_name_day_test		(void);
			void			create_menu					(void);

			int				perform_command				(const unsigned calendar_idx);

	private:				

			void			create_name_day_event		(HANDLE &handle, const string &contact_name, const string &first_name, const string &country);			
			bool			has_name_day				(const string &name_day, const string &first_name);
			void			create_sub_menu				(const calendar_t &calendar, const string &function_name);			

				vector<calendar_t>		calendars;

};


#endif // calendar_h