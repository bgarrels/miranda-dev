/*
Skype lugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
Copyright (C) 2009-2012 leecher

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

* IO Layer for Internet communication using libcurl
*/

#include <curl/curl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "fifo.h"
#include "io_layer.h"

typedef struct 
{
	IOLAYER vtbl;
	CURL *hCurl;
	TYP_FIFO *hResult;
	char szErrorBuf[CURL_ERROR_SIZE+1];
} IOLAYER_INST;

static void IoLayer_Exit (IOLAYER *hPIO);
static char *IoLayer_Post(IOLAYER *hPIO, char *pszUrl, char *pszPostFields, unsigned int cbPostFields, unsigned int *pdwLength);
static char *IoLayer_Get(IOLAYER *hPIO, char *pszUrl, unsigned int *pdwLength);
static void IoLayer_Cancel(IOLAYER *hIO);
static char *IoLayer_GetLastError(IOLAYER *hIO);
static char *IoLayer_EscapeString(IOLAYER *hIO, char *pszData);
static void IoLayer_FreeEscapeString(char *pszData);
static size_t add_data(char *data, size_t size, size_t nmemb, void *ctx);

// -----------------------------------------------------------------------------
// Interface
// -----------------------------------------------------------------------------

IOLAYER *IoLayerCURL_Init(void)
{
	IOLAYER_INST *hIO;
	
	if (!(hIO = calloc(1, sizeof(IOLAYER_INST))))
		return NULL;
		
	if (!(hIO->hCurl = curl_easy_init()))
	{
		free (hIO);
		return NULL;
	}
	
	if (!(hIO->hResult = Fifo_Init(1024)))
	{
		IoLayer_Exit((IOLAYER*)hIO);
		return NULL;
	}

	curl_easy_setopt(hIO->hCurl, CURLOPT_USERAGENT, "XMLHttpRequest/1.0");
	curl_easy_setopt(hIO->hCurl, CURLOPT_HEADER, 0);
	curl_easy_setopt(hIO->hCurl, CURLOPT_AUTOREFERER, 1);
//	curl_easy_setopt(hRq->hCurl, CURLOPT_RETURNTRANSFER, 1);
	curl_easy_setopt(hIO->hCurl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(hIO->hCurl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(hIO->hCurl, CURLOPT_ERRORBUFFER, hIO->szErrorBuf);
	curl_easy_setopt(hIO->hCurl, CURLOPT_COOKIEFILE, "cookies.txt");
	curl_easy_setopt(hIO->hCurl, CURLOPT_COOKIEJAR, "cookies.txt");
	curl_easy_setopt(hIO->hCurl, CURLOPT_WRITEFUNCTION, add_data);
	curl_easy_setopt(hIO->hCurl, CURLOPT_FILE, hIO);

	// Init Vtbl
	hIO->vtbl.Exit = IoLayer_Exit;
	hIO->vtbl.Post = IoLayer_Post;
	hIO->vtbl.Get = IoLayer_Get;
	hIO->vtbl.Cancel = IoLayer_Cancel;
	hIO->vtbl.GetLastError = IoLayer_GetLastError;
	hIO->vtbl.EscapeString = IoLayer_EscapeString;
	hIO->vtbl.FreeEscapeString = IoLayer_FreeEscapeString;
	
	return (IOLAYER*)hIO;
}

// -----------------------------------------------------------------------------

static void IoLayer_Exit (IOLAYER *hPIO)
{
	IOLAYER_INST *hIO = (IOLAYER_INST*)hPIO;

	if (hIO->hCurl) curl_easy_cleanup(hIO->hCurl);
	if (hIO->hResult) Fifo_Exit(hIO->hResult);
	free (hIO);
}

// -----------------------------------------------------------------------------

static char *IoLayer_Post(IOLAYER *hPIO, char *pszUrl, char *pszPostFields, unsigned int cbPostFields, unsigned int *pdwLength)
{
	IOLAYER_INST *hIO = (IOLAYER_INST*)hPIO;

	curl_easy_setopt(hIO->hCurl, CURLOPT_POST, 1);
	curl_easy_setopt(hIO->hCurl, CURLOPT_URL, pszUrl);
	curl_easy_setopt(hIO->hCurl, CURLOPT_POSTFIELDS, pszPostFields);
	curl_easy_setopt(hIO->hCurl, CURLOPT_POSTFIELDSIZE, cbPostFields);
	if (curl_easy_perform(hIO->hCurl)) return NULL;
	if (!pdwLength)
	{
		// Get string
		Fifo_Add (hIO->hResult, "", 1);
		return Fifo_Get (hIO->hResult, NULL);
	}
	else
	{
		// Get binary, return size of buffer
		*pdwLength = (unsigned int)-1;
		return Fifo_Get (hIO->hResult, pdwLength);
	}
}

// -----------------------------------------------------------------------------

static char *IoLayer_Get(IOLAYER *hPIO, char *pszUrl, unsigned int *pdwLength)
{
	IOLAYER_INST *hIO = (IOLAYER_INST*)hPIO;

	curl_easy_setopt(hIO->hCurl, CURLOPT_POST, 0);
	curl_easy_setopt(hIO->hCurl, CURLOPT_URL, pszUrl);
	if (curl_easy_perform(hIO->hCurl)) return NULL;
	if (!pdwLength)
	{
		// Get string
		Fifo_Add (hIO->hResult, "", 1);
		return Fifo_Get (hIO->hResult, NULL);
	}
	else
	{
		// Get binary, return size of buffer
		*pdwLength = (unsigned int)-1;
		return Fifo_Get (hIO->hResult, pdwLength);
	}
}

// -----------------------------------------------------------------------------

static void IoLayer_Cancel(IOLAYER *hIO)
{
	// FIXME: Currently not implemented
}

// -----------------------------------------------------------------------------

static char *IoLayer_GetLastError(IOLAYER *hIO)
{
	return ((IOLAYER_INST*)hIO)->szErrorBuf;
}

// -----------------------------------------------------------------------------

static char *IoLayer_EscapeString(IOLAYER *hIO, char *pszData)
{
	return curl_easy_escape(((IOLAYER_INST*)hIO)->hCurl, pszData, 0);
}

// -----------------------------------------------------------------------------

static void IoLayer_FreeEscapeString(char *pszData)
{
	curl_free(pszData);
}

// -----------------------------------------------------------------------------
// Static
// -----------------------------------------------------------------------------

static size_t add_data(char *data, size_t size, size_t nmemb, void *ctx) 
{
	IOLAYER_INST *hIO = (IOLAYER_INST*)ctx;

	if (Fifo_Add(hIO->hResult, data, size * nmemb)) return size * nmemb;
	return 0;
}

