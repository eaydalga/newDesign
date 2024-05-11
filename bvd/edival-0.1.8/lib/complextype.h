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
 
#ifndef EDIComplexType_INCLUDED
#define EDIComplexType_INCLUDED

#include "schema.h"
/******************************************************************************/
struct EDI_ChildNodeStruct {
	EDI_SchemaNode    node;
	EDI_ChildNode     previousSibling;
	EDI_ChildNode     nextSibling;
	unsigned int      min_occurs;
	unsigned int      max_occurs;
	unsigned int      count;
};
/******************************************************************************/
struct EDI_ComplexTypeStruct {
	struct EDI_SchemaNodeStruct  header;
	EDI_ChildNode                firstChild;
	EDI_ChildNode                lastChild;
	unsigned int                 childCount;
	EDI_SyntaxNote               firstNote;
	EDI_SyntaxNote               finalNote;
};
/******************************************************************************/
struct EDI_LoopNodeStruct {
	struct EDI_ComplexTypeStruct node;
	char                        *startID;
};
/******************************************************************************/
struct EDI_SyntaxNoteStruct {
	enum EDI_SyntaxType  type;
	unsigned int         count;
	unsigned int        *positions;
	EDI_SyntaxNote       next;
};
/******************************************************************************/
void EDI_DisposeComplexType(EDI_ComplexType);
#endif /* EDIComplexType_INCLUDED */
