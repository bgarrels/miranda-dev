/*
Stopspam for 
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
$Id$		   : $Id$:

===============================================================================
*/

#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include <malloc.h>
#include <time.h>
#include <string>
#include <sstream>
#include<fstream>


#include <newpluginapi.h>
#include <m_database.h>
#include <m_protosvc.h>
#include <m_options.h>
#include <m_utils.h>
#include <m_langpack.h>
#include <m_clistint.h>
#include <m_skin.h>
#include <m_button.h>
#include <m_contacts.h>

//ExternalApi
#include <m_variables.h>
#include <m_folders.h>

#include "globals.h"
#include "../stopspam.h"
#include "options.h"
#include "eventhooker.h"
#include "../version.h"
#include "../resource.h"
#include "utilities.h"

//boost
#include <boost/thread.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/nondet_random.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_int.hpp>

//utf8cpp
#include <utf8.h>

//only for dos-plugin
#include <m_dos.h>
