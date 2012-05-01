#ifndef __DYNSTUFF_H
#define __DYNSTUFF_H

/*
GaduGadu plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			(C) 2001-2002 Wojtek Kaniewski <wojtekka@irc.pl>
			(C)	Dawid Jarosz <dawjar@poczta.onet.pl>

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

/* listy */

struct list {
	void *data;
	struct list *next;
};

typedef struct list * list_t;

void *list_add(list_t *list, void *data, int alloc_size);
void *list_add_sorted(list_t *list, void *data, int alloc_size, int (*comparision)(void *, void *));
int list_remove(list_t *list, void *data, int free_data);
int list_count(list_t list);
int list_destroy(list_t list, int free_data);

/* stringi */

struct string {
	char *str;
	int len, size;
};

typedef struct string * string_t;

string_t string_init(const char *str);
int string_append(string_t s, const char *str);
int string_append_n(string_t s, const char *str, int count);
int string_append_c(string_t s, char ch);
void string_insert(string_t s, int index, const char *str);
void string_insert_n(string_t s, int index, const char *str, int count);
void string_clear(string_t s);
char *string_free(string_t s, int free_string);

/* tablice string�w */

char **array_make(const char *string, const char *sep, int max, int trim, int quotes);
char *array_join(char **array, const char *sep);
void array_add(char ***array, char *string);
int array_count(char **array);
int array_contains(char **array, const char *string, int casesensitive);
void array_free(char **array);

/* rozszerzenia libc�w */

const char *ditoa(long int i);

#endif /* __DYNSTUFF_H */
