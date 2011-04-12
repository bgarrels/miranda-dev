// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin Öberg, Sam Kothari, Robert Rainwater
// Copyright © 2004,2005,2006,2007 Joe Kucera
// Copyright © 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// -----------------------------------------------------------------------------
//
// File name      : $Source$
// Revision       : $Revision: 43 $
// Last change on : $Date: 2007-08-20 01:51:06 +0300 (ÐŸÐ½, 20 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Describe me here please...
//
// -----------------------------------------------------------------------------

typedef struct fieldnames_t
{
    int code;
    char *text;
} FieldNamesItem;

extern const FieldNamesItem countryField[];
extern const FieldNamesItem interestsField[];
extern const FieldNamesItem languageField[];
extern const FieldNamesItem pastField[];
extern const FieldNamesItem genderField[];
extern const FieldNamesItem studyLevelField[];
extern const FieldNamesItem industryField[];
extern const FieldNamesItem agesField[];
extern const FieldNamesItem affiliationField[];
extern const FieldNamesItem maritalField[];
extern const FieldNamesItem occupationField[];

char *LookupFieldNameUtf(struct fieldnames_t *table, int code, char *str, size_t strsize);
