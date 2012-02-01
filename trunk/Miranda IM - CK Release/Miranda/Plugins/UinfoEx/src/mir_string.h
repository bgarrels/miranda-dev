/*
UserinfoEx plugin for Miranda IM

Copyright:
� 2006-2010 DeathAxe, Yasnovidyashii, Merlin, K. Romanov, Kreol

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

===============================================================================

File name      : $HeadURL: http://userinfoex.googlecode.com/svn/trunk/mir_string.h $
Revision       : $Revision: 187 $
Last change on : $Date: 2010-09-08 16:05:54 +0400 (Ср, 08 сен 2010) $
Last change by : $Author: ing.u.horn $

===============================================================================
*/

#ifndef _MIR_STRING_H_INCLUDED_
#define _MIR_STRING_H_INCLUDED_

class _A2T
{
	public:
	_A2T(const char* s) :
		buf(mir_a2t(s))
		{}

	_A2T(const char* s, int cp) :
		buf(mir_a2t_cp(s, cp))
		{}

	~_A2T()
	{	mir_free(buf);
	}

	__inline operator TCHAR*() const
	{	return buf;
	}

	private: TCHAR* buf;
};

#define mir_wcsdup		mir_wstrdup

#ifdef _UNICODE
 #define mir_tcslen		mir_wcslen
 #define mir_tcscpy		mir_wcscpy
 #define mir_tcsncpy	mir_wcsncpy
 #define mir_tcsncat	mir_wcsncat
 #define mir_tcsdup		mir_wcsdup
 #define mir_tcscmp		mir_wcscmp
 #define mir_tcsncmp	mir_wcsncmp
 #define mir_tcsicmp	mir_wcsicmp
 #define mir_tcsnicmp	mir_wcsnicmp
 #define mir_tcschr		mir_wcschr
 #define mir_tcsrchr	mir_wcsrchr
 #define mir_tcsncat_c	mir_wcsncat_c
 #define mir_IsEmpty	mir_IsEmptyW
#else
 #define mir_tcslen		mir_strlen
 #define mir_tcscpy		mir_strcpy
 #define mir_tcsncpy	mir_strncpy
 #define mir_tcsncat	mir_strncat
 #define mir_tcsdup		mir_strdup
 #define mir_tcscmp		mir_strcmp
 #define mir_tcsncmp	mir_strncmp
 #define mir_tcsicmp	mir_stricmp
 #define mir_tcsnicmp	mir_strnicmp
 #define mir_tcschr		mir_strchr
 #define mir_tcsrchr	mir_strrchr
 #define mir_tcsncat_c	mir_strncat_c
 #define mir_IsEmpty	mir_IsEmptyA
#endif


#define mir_strlen(s)			(((s)!=0)?strlen(s):0)
#define mir_strcpy(d,s)			(((s)!=0&&(d)!=0)?strcpy(d,s):0)
#define mir_strcmp(s1,s2)		((s1)==0||(s2)==0||strcmp((s1),(s2)))
#define mir_strncmp(s1,s2,n)	((s1)==0||(s2)==0||strncmp((s1),(s2),(n)))
#define mir_stricmp(s1,s2)		((s1)==0||(s2)==0||_stricmp((s1),(s2)))
#define mir_strnicmp(s1,s2,n)	((s1)==0||(s2)==0||_strnicmp((s1),(s2),(n)))
#define mir_strchr(s,c)			(((s)!=0)?strchr((s),(c)):0)
#define mir_strrchr(s,c)		(((s)!=0)?strrchr((s),(c)):0)

#define mir_wcslen(s)			(((s)!=0)?wcslen(s):0)
#define mir_wcscpy(d,s)			(((s)!=0&&(d)!=0)?wcscpy(d,s):0)
#define mir_wcscmp(s1,s2)		((s1)==0||(s2)==0||wcscmp((s1),(s2)))
#define mir_wcsncmp(s1,s2,n)	((s1)==0||(s2)==0||wcsncmp((s1),(s2),(n)))
#define mir_wcsicmp(s1,s2)		((s1)==0||(s2)==0||_wcsicmp((s1),(s2)))
#define mir_wcsnicmp(s1,s2,n)	((s1)==0||(s2)==0||_wcsnicmp((s1),(s2),(n)))
#define mir_wcschr(s,c)			(((s)!=0)?wcschr((s),(c)):0)
#define mir_wcsrchr(s,c)		(((s)!=0)?wcsrchr((s),(c)):0)

char *		mir_strncpy(char *pszDest, const char *pszSrc, const size_t cchDest);
wchar_t *	mir_wcsncpy(wchar_t *pszDest, const wchar_t *pszSrc, const size_t cchDest);

char *		mir_strncat(char *pszDest, const char *pszSrc, const size_t cchDest);
wchar_t *	mir_wcsncat(wchar_t *pszDest, const wchar_t *pszSrc, const size_t cchDest);

char *		mir_strncat_c(char *pszDest, const char cSrc);
char *		mir_wcsncat_c(char *pszDest, const char cSrc);

size_t		mir_vsnwprintf(wchar_t *pszDest, const size_t cchDest, const wchar_t *pszFormat, va_list& argList);
size_t		mir_snwprintf (wchar_t *pszDest, const size_t cchDest, const wchar_t *pszFormat, ...);

char *		mir_strnerase(char *pszDest, size_t sizeFrom, size_t sizeTo);

int			mir_IsEmptyA(char *str);
int			mir_IsEmptyW(wchar_t *str);

#endif /* _MIR_STRING_H_INCLUDED_ */