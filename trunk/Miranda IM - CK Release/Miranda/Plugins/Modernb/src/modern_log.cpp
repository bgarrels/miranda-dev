/*
Author Artem Shpynov aka FYR

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

#include "../hdr/modern_commonheaders.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>


void Log(const char *file,int line,const char *fmt,...)
{

	
	va_list vararg;
	const char *file_tmp;
	char str[1024];
	char buf[1024];

	file_tmp = strrchr(file, '\\');
	if (file_tmp == NULL)
		file_tmp = file;
	else
		file_tmp++;

	va_start(vararg,fmt);
	mir_vsnprintf(str,SIZEOF(str),fmt,vararg);
	va_end(vararg);
	{
		char * tmp=str;
		while(*tmp!='\0')
		{
		   if (*tmp=='\n') *tmp=' ';
		   tmp++;
		}        
	}
	mir_snprintf(buf,SIZEOF(buf),"clist_modern:[%u - %u]: %s \t\t(%s Ln %d)\n",GetCurrentThreadId(),GetTickCount(),str,file_tmp,line);
#ifdef _FILELOG_
	{
		FILE *fp;
		fp=fopen(_FILELOG_,"at");	
		fprintf(fp,buf);
		fclose(fp);
	}
#else
	OutputDebugStringA(buf);
#endif
}

