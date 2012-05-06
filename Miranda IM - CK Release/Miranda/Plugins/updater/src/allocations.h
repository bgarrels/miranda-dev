#ifndef _ALLOCATION_INC
#define _ALLOCATION_INC

/*
Updater plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	2005-2006 Scott Ellis
							2009-2012 Boris Krasnovskiy

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

// store allocated memory pointers for deallocation when shutting down
class Allocations {
public:
	Allocations(): head(0) {}
	virtual ~Allocations() { free_all(); }

	void push_back(void *pt) { Node *n = new Node; n->value = pt; n->next = head; head = n;}

protected:
	
	class Node {
	public:
		void *value;
		Node *next;
	};

	Node *head;

	void free_all() {
		Node *current;
		while(head) {
			current = head;
			head = head->next;
			free(current->value);
			delete current;
		}
	}
};

extern Allocations allocations;

__inline static void *safe_alloc(size_t bytes) {
	if(bytes == 0) return 0;
	void *ret = malloc(bytes);
	allocations.push_back(ret);
	return ret;
}

__inline static char *safe_strdup(const char *s) {
	if(!s) return 0;
	char *ret = _strdup(s);
	allocations.push_back(ret);
	return ret;
}

__inline static wchar_t *safe_wstrdup(const wchar_t *s) {
	if(!s) return 0;
	wchar_t *ret = _wcsdup(s);
	allocations.push_back(ret);
	return ret;
}

#ifdef _UNICODE
#define safe_tstrdup(x)		safe_wstrdup(x)
#else
#define sage_tstrdup(x)		safe_strdup(x)
#endif

__inline static BYTE *safe_bytedup(BYTE *bytes, int size) {
	if(!bytes || size == 0) return 0;

	BYTE *ret = (BYTE *)malloc(size + 1);
	memcpy(ret, bytes, size);
	*(ret + size) = 0;
	allocations.push_back(ret);
	return ret;
}

#endif
