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

// gzip data
BYTE *cpp_gzip(BYTE *pData, int nLen, int& nCompressedLen) {

	string zipped;
	Gzip gzip(new StringSink(zipped),5);    // 1 is fast, 9 is slow
	gzip.Put(pData, nLen);
	gzip.MessageEnd();

	nCompressedLen = (int) zipped.length();
	PBYTE pCompressed = (PBYTE) malloc(nCompressedLen+1);
	memcpy(pCompressed,zipped.data(),nCompressedLen);

	return pCompressed;
}

// gunzip data
BYTE *cpp_gunzip(BYTE *pCompressedData, int nCompressedLen, int& nLen) {

	string unzipped;
	Gunzip gunzip(new StringSink(unzipped));
	gunzip.Put((PBYTE)pCompressedData,nCompressedLen);
	gunzip.MessageEnd();

	nLen = (int) unzipped.length();
	PBYTE pData = (PBYTE) malloc(nLen+1);
	memcpy(pData,unzipped.data(),nLen);

	return pData;
}

// zlibc data
string cpp_zlibc(string& pData) {

	string zipped;

	ZlibCompressor zlib(new StringSink(zipped),5);    // 1 is fast, 9 is slow
	zlib.Put((PBYTE)pData.data(), pData.length());
	zlib.MessageEnd();

	return zipped;
}

// zlibd data
string cpp_zlibd(string& pData) {

	string unzipped;

	ZlibDecompressor zlib(new StringSink(unzipped));
	zlib.Put((PBYTE)pData.data(),pData.length());
	zlib.MessageEnd();

	return unzipped;
}

// EOF
