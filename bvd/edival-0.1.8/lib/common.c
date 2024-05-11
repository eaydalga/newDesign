/*
 *  Copyright (C) 2006, 2007 Michael Edgar
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "common.h"
/******************************************************************************/
size_t EDI_strnlen(const char *string, size_t maxlen)
{
  const char *end = memchr(string, '\0', maxlen);
  return end ? (size_t) (end - string) : maxlen;
}
/******************************************************************************/
char *EDI_strndup(const char *s, size_t n, const EDI_Memory_Handling_Suite *memsuite)
{
    size_t len = EDI_strnlen(s, n);
    char *new = memsuite->malloc_fcn(len + 1);

    if(new == NULL){
        return NULL;
    }
    new[len] = '\0';
    return (char *) memcpy(new, s, len);
}
/******************************************************************************/
inline int string_eq(const char* str1, const char* str2)
{
	while(!((*str1++) ^ (*str2++))){
		if(!(*str1) && !(*str2)){
			return 1;
		}
	}
	return 0;
}
/******************************************************************************/
char *EDI_strdup(const char *s)
{
	char *new = (char *)malloc(strlen(s) + 1);
	if(new){
		strcpy(new, s);
	}
	return new;
}
