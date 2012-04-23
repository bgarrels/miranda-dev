/* $Id$ */

/*
(C) Copyright 2001-2003 Wojtek Kaniewski <wojtekka@irc.pl>
						Dawid Jarosz <dawjar@poczta.onet.pl>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License Version 2 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "gg.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *list_add_sorted(list_t *list, void *data, int alloc_size, int (*comparision)(void *, void *))
{
	list_t new, tmp;

	if (!list) {
		errno = EFAULT;
		return NULL;
	}

	new = malloc(sizeof(struct list));

	new->data = data;
	new->next = NULL;

	if (alloc_size) {
		new->data = malloc(alloc_size);
		memcpy(new->data, data, alloc_size);
	}

	if (!(tmp = *list)) {
		*list = new;
	} else {
		if (!comparision) {
			while (tmp->next)
				tmp = tmp->next;
			tmp->next = new;
		} else {
			list_t prev = NULL;

			while (comparision(new->data, tmp->data) > 0) {
				prev = tmp;
				tmp = tmp->next;
				if (!tmp)
					break;
			}

			if (!prev) {
				tmp = *list;
				*list = new;
				new->next = tmp;
			} else {
				prev->next = new;
				new->next = tmp;
			}
		}
	}

	return new->data;
}

void *list_add(list_t *list, void *data, int alloc_size)
{
	return list_add_sorted(list, data, alloc_size, NULL);
}

int list_remove(list_t *list, void *data, int free_data)
{
	list_t tmp, last = NULL;

	if (!list || !*list) {
		errno = EFAULT;
		return -1;
	}

	tmp = *list;
	if (tmp->data == data) {
		*list = tmp->next;
	} else {
		for (; tmp && tmp->data != data; tmp = tmp->next)
			last = tmp;
		if (!tmp) {
			errno = ENOENT;
			return -1;
		}
		last->next = tmp->next;
	}

	if (free_data)
		free(tmp->data);
	free(tmp);

	return 0;
}

int list_count(list_t list)
{
	int count = 0;

	for (; list; list = list->next)
		count++;

	return count;
}

int list_destroy(list_t list, int free_data)
{
	list_t tmp;

	while (list) {
		if (free_data)
			free(list->data);

		tmp = list->next;

		free(list);

		list = tmp;
	}

	return 0;
}

static void string_realloc(string_t s, int count)
{
	char *tmp;

	if (s->str && count + 1 <= s->size)
		return;

	tmp = realloc(s->str, count + 81);
	if (!s->str)
		*tmp = 0;
	tmp[count + 80] = 0;
	s->size = count + 81;
	s->str = tmp;
}

int string_append_c(string_t s, char c)
{
	if (!s) {
		errno = EFAULT;
		return -1;
	}

	string_realloc(s, s->len + 1);

	s->str[s->len + 1] = 0;
	s->str[s->len++] = c;

	return 0;
}

int string_append_n(string_t s, const char *str, int count)
{
	if (!s || !str) {
		errno = EFAULT;
		return -1;
	}

	if (count == -1)
		count = (int)strlen(str);

	string_realloc(s, s->len + count);

	s->str[s->len + count] = 0;
	strncpy(s->str + s->len, str, count);

	s->len += count;

	return 0;
}

int string_append(string_t s, const char *str)
{
	return string_append_n(s, str, -1);
}

void string_insert_n(string_t s, int index, const char *str, int count)
{
	if (!s || !str)
		return;

	if (count == -1)
		count = (int)strlen(str);

	if (index > s->len)
		index = s->len;

	string_realloc(s, s->len + count);

	memmove(s->str + index + count, s->str + index, s->len + 1 - index);
	memmove(s->str + index, str, count);

	s->len += count;
}

void string_insert(string_t s, int index, const char *str)
{
	string_insert_n(s, index, str, -1);
}

string_t string_init(const char *value)
{
	string_t tmp = malloc(sizeof(struct string));

	if (!value)
		value = "";

	tmp->str = _strdup(value);
	tmp->len = (int)strlen(value);
	tmp->size = (int)strlen(value) + 1;

	return tmp;
}

void string_clear(string_t s)
{
	if (!s)
		return;

	if (s->size > 160) {
		s->str = realloc(s->str, 80);
		s->size = 80;
	}

	s->str[0] = 0;
	s->len = 0;
}

char *string_free(string_t s, int free_string)
{
	char *tmp = NULL;

	if (!s)
		return NULL;

	if (free_string)
		free(s->str);
	else
		tmp = s->str;

	free(s);

	return tmp;
}

const char *ditoa(long int i)
{
	static char bufs[10][16];
	static int index = 0;
	char *tmp = bufs[index++];

	if (index > 9)
		index = 0;

	mir_snprintf(tmp, 16, "%ld", i);

	return tmp;
}

char **array_make(const char *string, const char *sep, int max, int trim, int quotes)
{
	const char *p, *q;
	char **result = NULL;
	int items = 0, last = 0;

	if (!string || !sep)
		goto failure;

	for (p = string; ; ) {
		int len = 0;
		char *token = NULL;

		if (max && items >= max - 1)
			last = 1;

		if (trim) {
			while (*p && strchr(sep, *p))
				p++;
			if (!*p)
				break;
		}

		if (!last && quotes && (*p == '\'' || *p == '\"')) {
			char sep = *p;

			for (q = p + 1, len = 0; *q; q++, len++) {
				if (*q == '\\') {
					q++;
					if (!*q)
						break;
				} else if (*q == sep)
					break;
			}

			if ((token = calloc(1, len + 1))) {
				char *r = token;

				for (q = p + 1; *q; q++, r++) {
					if (*q == '\\') {
						q++;

						if (!*q)
							break;

						switch (*q) {
							case 'n':
								*r = '\n';
								break;
							case 'r':
								*r = '\r';
								break;
							case 't':
								*r = '\t';
								break;
							default:
								*r = *q;
						}
					} else if (*q == sep) {
						break;
					} else
						*r = *q;
				}

				*r = 0;
			}

			p = (*q) ? q + 1 : q;

		} else {
			for (q = p, len = 0; *q && (last || !strchr(sep, *q)); q++, len++);
			token = calloc(1, len + 1);
			strncpy(token, p, len);
			token[len] = 0;
			p = q;
		}

		result = realloc(result, (items + 2) * sizeof(char*));
		result[items] = token;
		result[++items] = NULL;

		if (!*p)
			break;

		p++;
	}

failure:
	if (!items)
		result = calloc(1, sizeof(char*));

	return result;
}

int array_count(char **array)
{
	int result = 0;

	if (!array)
		return 0;

	while (*array) {
		result++;
		array++;
	}

	return result;
}

void array_add(char ***array, char *string)
{
	int count = array_count(*array);

	*array = realloc(*array, (count + 2) * sizeof(char*));
	(*array)[count + 1] = NULL;
	(*array)[count] = string;
}

char *array_join(char **array, const char *sep)
{
	string_t s = string_init(NULL);
	int i;

	if (!array)
		return _strdup("");

	for (i = 0; array[i]; i++) {
		if (i)
			string_append(s, sep);

		string_append(s, array[i]);
	}

	return string_free(s, 0);
}

int array_contains(char **array, const char *string, int casesensitive)
{
	int i;

	if (!array || !string)
		return 0;

	for (i = 0; array[i]; i++) {
		if (casesensitive && !strcmp(array[i], string))
			return 1;
		if (!casesensitive && !strcasecmp(array[i], string))
			return 1;
	}

	return 0;
}

void array_free(char **array)
{
	char **tmp;

	if (!array)
		return;

	for (tmp = array; *tmp; tmp++)
		free(*tmp);

	free(array);
}
