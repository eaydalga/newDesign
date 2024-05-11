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
 
#ifndef EDIParse_INCLUDED
#define EDIParse_INCLUDED

#include "common.h"

struct EDI_ParserStruct {
	void                            *userData;
	EDI_Memory_Handling_Suite       *memsuite;
	char                            *bufReadPtr;
	char                            *bufEndPtr;
	char                            *dataBuffer;
	char                            *dataBufEnd;
	enum EDI_Error                   errorCode;
	enum EDI_ParsingState            state;
	EDI_DocumentStartHandler         documentStartHandler;
	EDI_DocumentEndHandler           documentEndHandler;
	EDI_SegmentStartHandler          segmentStartHandler;
	EDI_SegmentEndHandler            segmentEndHandler;
	EDI_CompositeStartHandler        compositeStartHandler;
	EDI_CompositeEndHandler          compositeEndHandler;
	EDI_ElementHandler               elementHandler;
	EDI_BinaryBufferHandler          binBufferHandler;
	long long                        binaryElementSize;
	long long                        bytesHandled;
	char                            *binBuffer;
	EDI_RepeatHandler                repeatHandler;
	EDI_NonEDIDataHandler            nonEDIDataHandler;
	/* Internally used by the parsers */
	EDI_StateHandler                 seekHeader;
	EDI_StateHandler                 process;
	EDI_StateHandler                 error;
	void                            *machine;
	void                            *child;
	void                            *(*freeChild)(EDI_Parser);
	enum EDI_DocumentType            docType;
	EDI_Schema                       schema;
	EDI_Bool                         validate;
	EDI_Bool                         final;
};

#endif /* EDIParse_INCLUDED */
