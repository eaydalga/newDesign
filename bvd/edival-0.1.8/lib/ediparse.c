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

#include "edifactparser.h"
#include "ediparse.h"
#include "schema.h"
#include "statemachine.h"
#include "x12parser.h"
/******************************************************************************/
#define INIT_DATA_BUF_SIZE 1024  /* 1KB default buffer size */
/******************************************************************************/
static EDI_Parser  parserCreate(EDI_Memory_Handling_Suite *);
static void        parserInit(EDI_Parser);
/******************************************************************************/
EDI_StateHandler seekHeader(EDI_Parser parser)
{
	int          i       = 0;
	char         tag[4]  = "   ";
	static char *previous;
	char        *bufIter = NULL;
	EDI_Bool     found = EDI_FALSE;

	bufIter = parser->bufReadPtr;
	if(previous == parser->bufReadPtr){
		parser->bufReadPtr -= 3;
	}
	while(bufIter < parser->bufEndPtr){
		for(i = 0; i < 3; i++){
			tag[i] = *bufIter++;
		}
		if(string_eq(tag, "ISA")){
			if(parser->docType != EDI_ANSI_X12){
				if(parser->docType != EDI_UNKNOWN_DOC){
					parser->freeChild(parser);
				}
				parser->child = X12_ParserCreate(parser);
				parser->docType = EDI_ANSI_X12;
			}
			found = EDI_TRUE;
		} else if(string_eq(tag, "UNA") || string_eq(tag, "UNB")){
			if(parser->docType != EDI_EDIFACT){
				if(parser->docType != EDI_UNKNOWN_DOC){
					parser->freeChild(parser);
				}
				parser->child = EDIFACT_ParserCreate(parser);
				parser->docType = EDI_EDIFACT;
			}
			found = EDI_TRUE;
		}
		if(found){
			previous = bufIter;
			if((bufIter - parser->bufReadPtr) > 3){
				int prefix = bufIter - parser->bufReadPtr - 3;
				if(prefix > 1 || !isspace(*(parser->bufReadPtr))){
					char *invalid = EDI_strndup(parser->bufReadPtr, prefix, parser->memsuite);
					if(!invalid){
						parser->errorCode = EDI_ERROR_NO_MEM;
						return parser->error;
					}
					parser->nonEDIDataHandler(parser->userData, invalid);
					FREE(parser, invalid);
				}
			}
			parser->bufReadPtr = bufIter - 3;
			return parser->process;
		} else {
			bufIter -= 2;
		}
	}
	if(parser->bufReadPtr < parser->bufEndPtr && bufIter == parser->bufEndPtr){
		int prefix = bufIter - parser->bufReadPtr;
		char *invalid = EDI_strndup(parser->bufReadPtr, prefix, parser->memsuite);
		if(!invalid){
			parser->errorCode = EDI_ERROR_NO_MEM;
			return parser->error;
		}
		parser->nonEDIDataHandler(parser->userData, invalid);
		FREE(parser, invalid);
		parser->bufReadPtr = bufIter;
	}
	parser->errorCode = EDI_ERROR_BUFFER_END;
	return parser->error;
}
/******************************************************************************/
EDI_StateHandler handleError(EDI_Parser parser)
{
    //fprintf(stderr, "Error! %d\n", EDI_GetErrorCode(parser));
	return NULL;
}
/******************************************************************************/
EDI_Parser EDI_ParserCreate(void)
{
    return parserCreate(NULL);
}
/******************************************************************************/
EDI_Parser EDI_ParserCreate_MM(EDI_Memory_Handling_Suite *memsuite)
{
    return parserCreate(memsuite);
}   
/******************************************************************************/
static EDI_Parser parserCreate(EDI_Memory_Handling_Suite *memsuite)
{
	EDI_Parser parser;
	
	if(memsuite){
        EDI_Memory_Handling_Suite *mtemp;
        parser = (EDI_Parser)\
            memsuite->malloc_fcn(sizeof(struct EDI_ParserStruct));
        if (parser != NULL) {
        	parser->memsuite = (EDI_Memory_Handling_Suite *)\
        		memsuite->malloc_fcn(sizeof(EDI_Memory_Handling_Suite));
            mtemp = (EDI_Memory_Handling_Suite *)parser->memsuite;
            mtemp->malloc_fcn = memsuite->malloc_fcn;
            mtemp->realloc_fcn = memsuite->realloc_fcn;
            mtemp->free_fcn = memsuite->free_fcn;
        }
	} else {
        EDI_Memory_Handling_Suite *mtemp;
        parser = (EDI_Parser)malloc(sizeof(struct EDI_ParserStruct));
        if (parser != NULL) {
        	parser->memsuite = (EDI_Memory_Handling_Suite *)\
        		malloc(sizeof(EDI_Memory_Handling_Suite));
            mtemp = (EDI_Memory_Handling_Suite *)parser->memsuite;
            mtemp->malloc_fcn = malloc;
            mtemp->realloc_fcn = realloc;
            mtemp->free_fcn = free;
        }
	}
	if(!parser){
	   return NULL;
	}
	parser->dataBuffer = MALLOC(parser, INIT_DATA_BUF_SIZE * sizeof(char));
	if(!parser->dataBuffer){
	   FREE(parser, parser);
	   return NULL;
	}
	parser->dataBufEnd = parser->dataBuffer + INIT_DATA_BUF_SIZE;
	parserInit(parser);
	return parser;
}
/******************************************************************************/
static void parserInit(EDI_Parser parser)
{
	parser->userData              = NULL;
	parser->bufReadPtr            = parser->dataBuffer;
	parser->bufEndPtr             = parser->dataBuffer;
	parser->state                 = EDI_INITIALIZED;
	parser->errorCode             = EDI_ERROR_NONE;
	parser->segmentStartHandler   = NULL;
	parser->segmentEndHandler     = NULL;
	parser->compositeStartHandler = NULL;
	parser->compositeEndHandler   = NULL;
	parser->elementHandler        = NULL;
	parser->binBufferHandler      = NULL;
	parser->binaryElementSize     = 0;
	parser->bytesHandled          = 0;
	parser->binBuffer             = NULL;
	parser->nonEDIDataHandler     = NULL;
	parser->seekHeader            = (void *)seekHeader;
	parser->error                 = (void *)handleError;
	parser->process               = NULL;
	parser->machine               = NULL;
	parser->child                 = NULL;
	parser->freeChild             = NULL;
	parser->docType               = EDI_UNKNOWN_DOC;
	parser->schema                = NULL;
	parser->validate              = EDI_FALSE;
	parser->final                 = EDI_FALSE;
	return;
}
/******************************************************************************/
void EDI_SetUserData(EDI_Parser parser, void *p)
{
    parser->userData = p;
}
/******************************************************************************/
void EDI_SetDocumentStartHandler(EDI_Parser parser, EDI_DocumentStartHandler h)
{
    parser->documentStartHandler = h;
}
/******************************************************************************/
void EDI_SetDocumentEndHandler(EDI_Parser parser, EDI_DocumentEndHandler h)
{
    parser->documentEndHandler = h;
}
/******************************************************************************/
void EDI_SetSegmentStartHandler(EDI_Parser parser, EDI_SegmentStartHandler h)
{
    parser->segmentStartHandler = h;
}
/******************************************************************************/
void EDI_SetSegmentEndHandler(EDI_Parser parser, EDI_SegmentEndHandler h)
{
    parser->segmentEndHandler = h;
}
/******************************************************************************/
void EDI_SetCompositeStartHandler(EDI_Parser parser, EDI_CompositeStartHandler h)
{
    parser->compositeStartHandler = h;
}
/******************************************************************************/
void EDI_SetCompositeEndHandler(EDI_Parser parser, EDI_CompositeEndHandler h)
{
    parser->compositeEndHandler = h;
}
/******************************************************************************/
void EDI_SetElementHandler(EDI_Parser parser, EDI_ElementHandler h)
{
    parser->elementHandler = h;
}
/******************************************************************************/
void EDI_SetBinaryElementHandler(EDI_Parser              parser, 
                                 EDI_BinaryBufferHandler bbh   )
{
    parser->binBufferHandler    = bbh;
}
/******************************************************************************/
void EDI_SetRepeatHandler(EDI_Parser parser, EDI_RepeatHandler h)
{
    parser->repeatHandler = h;
}
/******************************************************************************/
void EDI_SetNonEDIDataHandler(EDI_Parser parser, EDI_NonEDIDataHandler h)
{
    parser->nonEDIDataHandler = h;
}
/******************************************************************************/
EDI_Bool EDI_ParserReset(EDI_Parser parser)
{
    FREE(parser, parser->dataBuffer);
    FREE(parser, parser->machine);
	if(parser->child){
		parser->freeChild(parser);
	}
	parser->dataBuffer = MALLOC(parser, INIT_DATA_BUF_SIZE * sizeof(char *));
	if(!parser->dataBuffer){
	   FREE(parser, parser);
	   return EDI_FALSE;
	}
	parser->dataBufEnd = parser->dataBuffer + INIT_DATA_BUF_SIZE;
	parserInit(parser);
	return EDI_TRUE;
}
/******************************************************************************/
void *EDI_GetBuffer(EDI_Parser parser, int len)
{

    switch (parser->state) {
        case EDI_SUSPENDED:
            parser->errorCode = EDI_ERROR_SUSPENDED;
            return NULL;
        case EDI_FINISHED:
            parser->errorCode = EDI_ERROR_FINISHED;
            return NULL;
        default: ;
    }
    if (len > parser->dataBufEnd - parser->bufEndPtr) {
        int neededSize = len + (parser->bufEndPtr - parser->bufReadPtr);
        if (neededSize <= parser->dataBufEnd - parser->dataBuffer){
            memmove((void *)(parser->dataBuffer), 
                    (const void *)(parser->bufReadPtr), 
                    parser->bufEndPtr - parser->bufReadPtr);
            parser->bufEndPtr = parser->dataBuffer +
                                (parser->bufEndPtr - parser->bufReadPtr);
            parser->bufReadPtr = parser->dataBuffer;
        } else {
            char *newBuf;
            int size = parser->dataBufEnd - parser->bufReadPtr;
            size += neededSize;
            newBuf = (char *)MALLOC(parser, size);
            if(!newBuf){
                parser->errorCode = EDI_ERROR_NO_MEM;
                return NULL;
            }
            parser->dataBufEnd = newBuf + size;
            if(parser->bufReadPtr){
                memcpy(newBuf, 
                       parser->bufReadPtr, 
                       parser->bufEndPtr - parser->bufReadPtr);
                FREE(parser, parser->dataBuffer);
            }
            parser->bufEndPtr = newBuf + 
                                (parser->bufEndPtr - parser->bufReadPtr);
            parser->bufReadPtr = newBuf;
            parser->dataBuffer = newBuf;
        }
    }
    return (void *)parser->bufEndPtr;
}
/******************************************************************************/
EDI_Schema EDI_GetSchema(EDI_Parser parser)
{
	EDI_Schema schema = NULL;
	if(parser){
		schema = parser->schema;
	}
	return schema;
}
/******************************************************************************/
void EDI_SetSchema(EDI_Parser parser, EDI_Schema schema)
{
	if(schema){
		parser->schema = schema;
		parser->validate = EDI_TRUE;
		schema->parser = parser;
	}
}
/******************************************************************************/
EDI_Schema EDI_RemoveSchema(EDI_Parser parser)
{
	EDI_Schema schema = NULL;
	if(parser){
		schema = parser->schema;
		parser->schema = NULL;
		parser->validate = EDI_FALSE;
		if(schema){
			schema->parser = NULL;
		}
	}
	return schema;
}
/******************************************************************************/
enum EDI_Status EDI_Parse(EDI_Parser parser, const char *s, int len, EDI_Bool final)
{
    void *buffer = NULL;

    switch (parser->state) {
        case EDI_SUSPENDED:
            parser->errorCode = EDI_ERROR_SUSPENDED;
            return EDI_STATUS_ERROR;
        case EDI_FINISHED:
            parser->errorCode = EDI_ERROR_FINISHED;
            return EDI_STATUS_ERROR;
        default:
            parser->state = EDI_PARSING;
    }
    buffer = EDI_GetBuffer(parser, len);
    if (buffer == NULL){
        return EDI_STATUS_ERROR;
    } else {
        memcpy(buffer, s, len);
        return EDI_ParseBuffer(parser, len, final);
    }
}
/******************************************************************************/
enum EDI_Status EDI_ParseBuffer(EDI_Parser parser, int len, EDI_Bool final)
{
	enum EDI_Error error = EDI_ERROR_NONE;

	if(!parser->documentStartHandler){
		fprintf(stderr, "FATAL (edival): No callback registered, event: Document Start\n");
		error = parser->errorCode = EDI_ERROR_ABORTED;
	}
	if(!parser->documentEndHandler){
		fprintf(stderr, "FATAL (edival): No callback registered, event: Document End\n");
		error = parser->errorCode = EDI_ERROR_ABORTED;
	}
	if(!parser->segmentStartHandler){
		fprintf(stderr, "FATAL (edival): No callback registered, event: Segment Start\n");
		error = parser->errorCode = EDI_ERROR_ABORTED;
	}
	if(!parser->segmentEndHandler){
		fprintf(stderr, "FATAL (edival): No callback registered, event: Segment End\n");
		error = parser->errorCode = EDI_ERROR_ABORTED;
	}
	if(!parser->compositeStartHandler){
		fprintf(stderr, "FATAL (edival): No callback registered, event: Composite Start\n");
		error = parser->errorCode = EDI_ERROR_ABORTED;
	}
	if(!parser->compositeEndHandler){
		fprintf(stderr, "FATAL (edival): No callback registered, event: Composite End\n");
		error = parser->errorCode = EDI_ERROR_ABORTED;
	}
	if(!parser->elementHandler){
		fprintf(stderr, "FATAL (edival): No callback registered, event: Element\n");
		error = parser->errorCode = EDI_ERROR_ABORTED;
	}
	if(error){
		return EDI_STATUS_ERROR;
	}
	switch (parser->state) {
		case EDI_SUSPENDED:
			parser->errorCode = EDI_ERROR_SUSPENDED;
			return EDI_STATUS_ERROR;
		case EDI_FINISHED:
			parser->errorCode = EDI_ERROR_FINISHED;
			return EDI_STATUS_ERROR;
		default:
			parser->state = EDI_PARSING;
	}
	if(!(parser->machine)){
		parser->machine = EDI_StateMachineCreate(parser->memsuite);
		if(!(parser->machine)){
			parser->errorCode = EDI_ERROR_NO_MEM;
			return EDI_STATUS_ERROR;
		}
		EDI_AddState(parser->machine, parser->seekHeader, 0);
		EDI_AddState(parser->machine, parser->error, 1);
		EDI_SetStartState(parser->machine, parser->seekHeader);
	}
	parser->bufEndPtr += len;
	*(parser->bufEndPtr) = '\0';
	parser->final = final;
	error = EDI_StateMachineRun(parser->machine, parser);
	switch(error){
		case EDI_ERROR_NONE:
			parser->errorCode = EDI_ERROR_NONE;
			return EDI_STATUS_OK;
		case EDI_ERROR_FINISHED:
			parser->errorCode = EDI_ERROR_FINISHED;
			return EDI_STATUS_OK;
		case EDI_ERROR_SUSPENDED:
			parser->errorCode = EDI_ERROR_NONE;
			return EDI_STATUS_SUSPENDED;
		default:
			parser->errorCode = error;
			return EDI_STATUS_ERROR;
	}
}
/******************************************************************************/
enum EDI_Status EDI_StopParser(EDI_Parser parser, EDI_Bool resumable)
{
	fprintf(stderr, "FIXME: EDI_StopParser is not yet implemented.\n");
	parser->errorCode = EDI_ERROR_UNKNOWN;
	return EDI_STATUS_ERROR;
}
/******************************************************************************/
enum EDI_Status EDI_ResumeParser(EDI_Parser parser)
{
	fprintf(stderr, "FIXME: EDI_ResumeParser is not yet implemented.\n");
	parser->errorCode = EDI_ERROR_UNKNOWN;
	return EDI_STATUS_ERROR;
}
/******************************************************************************/
enum EDI_ParsingState EDI_GetParserState(EDI_Parser parser)
{
	return parser->state;
}
/******************************************************************************/
enum EDI_Error EDI_GetErrorCode(EDI_Parser parser)
{
    return parser->errorCode;
}
/******************************************************************************/
void *EDI_MemMalloc(EDI_Parser parser, size_t size)
{
	return MALLOC(parser, size);
}
/******************************************************************************/
void *EDI_MemRealloc(EDI_Parser parser, void *pointer, size_t size)
{
	return REALLOC(parser, pointer, size);
}
/******************************************************************************/
void EDI_MemFree(EDI_Parser parser, void *pointer)
{
	return FREE(parser, pointer);
}
/******************************************************************************/
void EDI_ParserFree(EDI_Parser parser)
{
	void (*free_fcn)(void *ptr);

	if(parser->schema){
		EDI_RemoveSchema(parser);
	}
	if(parser->child){
		parser->freeChild(parser);
	}
	if(parser->binBuffer){
		FREE(parser, parser->binBuffer);
	}
	FREE(parser, parser->dataBuffer);
	FREE(parser, parser->machine);
	free_fcn = parser->memsuite->free_fcn;
	free_fcn((void *)parser->memsuite);
	free_fcn(parser);
}
