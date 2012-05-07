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

#include "commonheaders.h"


char *base16encode(const char *inBuffer, int count) {

	char *outBuffer = (char *) malloc(count*2+1);
	char *outBufferPtr = outBuffer;
	BYTE *inBufferPtr = (BYTE *) inBuffer;

	while(count){
		*outBufferPtr++ = encode16(((*inBufferPtr)>>4)&0x0F);
		*outBufferPtr++ = encode16((*inBufferPtr++)&0x0F);
		count--;
	}
	*outBufferPtr = '\0';

	return outBuffer;
}


char *base16decode(const char *inBuffer, int *count) {

	char *outBuffer = (char *) malloc(*count);
	BYTE *outBufferPtr = (BYTE *) outBuffer;
	bool big_endian = false;

	if(*inBuffer == '0' && *(inBuffer+1) == 'x') {
		inBuffer += *count;
		big_endian = true;
		*count -= 2;
	}
	while(*count>1){
		BYTE c0,c1;
		if(big_endian) {
			c1 = decode16(*--inBuffer);
			c0 = decode16(*--inBuffer);
		}
		else {
			c0 = decode16(*inBuffer++);
			c1 = decode16(*inBuffer++);
		}
		if((c0 | c1) == BERR) {
			free(outBuffer);
			*count = 0;
			return(NULL);
		}
		*outBufferPtr++ = (c0<<4) | c1;
		*count -= 2;
	}
	*outBufferPtr = '\0';
	*count = (int)(outBufferPtr-(BYTE *)outBuffer);

	return outBuffer;
}


char *base16decode(const char *inBuffer) {

    int count = (int)strlen(inBuffer);
	return base16decode(inBuffer, &count);
}


// EOF
