#ifndef UTILITY_H
#define UTILITY_H

/*
AIM plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			(C) 2008-2012 Boris Krasnovskiy
			(C) 2005-2006 Aaron Myles Landwehr

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

char *normalize_name(const char *s);
char* trim_str(char* s);
void create_group(const char *group);
void set_extra_icon(HANDLE hContact, HANDLE hImage, int column_type);
unsigned int aim_oft_checksum_file(TCHAR *filename, unsigned __int64 size = -1);
char* long_ip_to_char_ip(unsigned long host, char* ip);
unsigned long char_ip_to_long_ip(char* ip);
unsigned short get_random(void);

inline int cap_cmp(const char* cap, const char* cap2) { return memcmp(cap, cap2, 16); }
inline const char* alpha_cap_str(char ver) { return (ver & 0x80) ? " Alpha" : ""; }
inline const char* secure_cap_str(char* ver) { return (*(int*)ver == 0xDEC0FE5A) ? " + SecureIM" : ""; }


struct BdListItem
{
	char* name;
	unsigned short item_id;

	BdListItem() { name = NULL; item_id = 0; }
	BdListItem(const char* snt, unsigned short id) { name = mir_strdup(snt); item_id = id; }
	~BdListItem() { mir_free(name); }
};

struct BdList : public OBJLIST<BdListItem>
{
	BdList() : OBJLIST<BdListItem>(5) {}

	void add(const char* snt, unsigned short id)
	{ insert(new BdListItem(snt, id)); }

	unsigned short add(const char* snt)
	{ 
		unsigned short id = get_free_id();
		insert(new BdListItem(snt, id));
		return id;
	}

	unsigned short get_free_id(void);
	unsigned short find_id(const char* name);
	char* find_name(unsigned short id);
	void remove_by_id(unsigned short id);
};

#endif
