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
 
#ifndef EDISimpleType_INCLUDED
#define EDISimpleType_INCLUDED

#include "schema.h"
/******************************************************************************/
#define IS_LEAP_YEAR(Y) \
	( ((Y)>0) && !((Y)%4) && ( ((Y)%100) || !((Y)%400) ) )

#define DATE_IS_VALID(Y, M, D) \
(\
	(M < 8 && \
		(\
				(M==2 && (D<=28 || (IS_LEAP_YEAR(Y) && D<=29)))\
			|| \
				(!(M%2) && (M!=2) && (D<=30))\
			|| \
				( (M%2) && (D<=31))\
		)\
	) || (M>7 && \
		(\
			(!(M%2) && (D<=31))\
			|| \
			( (M%2) && (D<=30))\
		)\
	)\
)\
/******************************************************************************/
struct EDI_SimpleTypeStruct {
	struct EDI_SchemaNodeStruct header;
	enum EDI_PrimitiveDataType  type;
	unsigned int                min;
	unsigned int                max;
	struct hashtable           *values;
};
/******************************************************************************/
void EDI_DisposeSimpleType(EDI_SchemaNode);

enum EDI_ElementValidationError 
EDI_CheckElementConstraints(EDI_SimpleType *, /* The element */
                            const char     *, /* String/element value  */
                            int             , /* String/element length */
                            EDI_DataElement );/* Reference to a conversion element (output param) */

#endif /* EDISimpleType_INCLUDED */
