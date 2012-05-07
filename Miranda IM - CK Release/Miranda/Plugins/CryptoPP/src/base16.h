#ifndef __BASE_16__
#define __BASE_16__

/*
Crypto++ plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
Copyright (C) 2006-2009 Baloo

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

#define BERR		0xFF	/* Illegal char marker */
#define BEOF		0x7F	/* EOF marker (padding char or EOL) */

typedef unsigned char byte;

static const byte asciiToBin16[] =
	{ BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BEOF, BERR, BERR, BEOF, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	  0x08, 0x09, BERR, BERR, BERR, BEOF, BERR, BERR,
	  BERR, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR,
	  BERR, BERR, BERR, BERR, BERR, BERR, BERR, BERR
	};

static const byte binToAscii16[] = "0123456789ABCDEF";

char *base16encode(const char *, const int);
char *base16decode(const char *, int *);
char *base16decode(const char *);

#define encode16(data) binToAscii16[data]
#define decode16(data) asciiToBin16[data]

#endif
