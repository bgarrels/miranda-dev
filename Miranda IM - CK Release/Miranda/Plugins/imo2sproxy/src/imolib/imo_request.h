#ifndef _IMO_REQUEST_H_
#define _IMO_REQUEST_H_

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
*/

#include "cJSON.h"

struct _tagIMORQ;
typedef struct _tagIMORQ IMORQ;

#ifdef _IOLAYER_H_
void ImoRq_SetIOLayer(IOLAYER *(*fp_Init)(void));
#endif

IMORQ *ImoRq_Init(void);
IMORQ *ImoRq_Clone (IMORQ *hRq);
void ImoRq_Cancel (IMORQ *hRq);
void ImoRq_Exit (IMORQ *hRq);

char *ImoRq_SessId(IMORQ *hRq);
char *ImoRq_GetLastError(IMORQ *hRq);
char *ImoRq_PostAmy(IMORQ *hRq, char *pszMethod, cJSON *data);
void ImoRq_CreateID(char *pszID, int cbID);
char *ImoRq_PostSystem(IMORQ *hRq, char *pszMethod, char *pszSysTo, char *pszSysFrom, cJSON *data, int bFreeData);
char *ImoRq_PostToSys(IMORQ *hRq, char *pszMethod, char *pszSysTo, cJSON *data, int bFreeData);
void ImoRq_UpdateAck(IMORQ *hRq, unsigned long lAck);
unsigned long ImoRq_GetSeq(IMORQ *hRq);
char *ImoRq_UserActivity(IMORQ *hRq);
char *ImoRq_ResetRPC(IMORQ *hRq);
char *ImoRq_Reui_Session(IMORQ *hRq);
char *ImoRq_Echo(IMORQ *hRq);
char *ImoRq_HTTPGet(IMORQ *hRq, char *pszURL, unsigned int *pdwLength);
#endif
