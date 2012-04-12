/*
Facebook plugin for Miranda Instant Messenger
_____________________________________________

Copyright � 2009-2011 Michal Zelinka, 2011-2012 Robert P�sel

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

File name      : $HeadURL$
Revision       : $Revision$
Last change by : $Author$
Last change on : $Date$
*/

#pragma once

INT_PTR CALLBACK FBAccountProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK FBMindProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK FBOptionsProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK FBOptionsAdvancedProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );
INT_PTR CALLBACK FBEventsProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );
