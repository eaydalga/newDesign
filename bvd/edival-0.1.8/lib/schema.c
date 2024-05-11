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

#include "complextype.h"
#include "schema.h"
#include "simpletype.h"
/******************************************************************************/
#define SCHEMA_PUSH(segment) (schema->stack[++(schema->depth)] = segment)
#define SCHEMA_SAVE(segment) (schema->stack[schema->depth] = segment)
#define SCHEMA_POP()         (schema->stack[--(schema->depth)])
#define SCHEMA_READ(depth)   (schema->stack[depth])
#define PARENT_NODE          ((schema->depth > 0) ? \
                                ((EDI_ComplexType) \
                                   (schema->stack[schema->depth - 1])->node \
                                ) : schema->root)
/******************************************************************************/
static EDI_Schema schemaCreate(
	enum EDI_DocumentType      ,
	const char *               , 
	EDI_Memory_Handling_Suite *);
static void schemaInit(EDI_Schema);
/******************************************************************************/
EDI_Schema EDI_SchemaCreate(enum EDI_DocumentType type)
{
	return schemaCreate(type, NULL, NULL);	
}
/******************************************************************************/
EDI_Schema EDI_SchemaCreateNamed(enum EDI_DocumentType type, const char *name)
{
	return schemaCreate(type, name, NULL);	
}
/******************************************************************************/
EDI_Schema EDI_SchemaCreate_MM(enum EDI_DocumentType type         , 
                               EDI_Memory_Handling_Suite *memsuite)
{
	return schemaCreate(type, NULL, memsuite);
}
/******************************************************************************/
EDI_Schema EDI_SchemaCreateNamed_MM(enum EDI_DocumentType      type    ,
                                    const char                *name    ,
                                    EDI_Memory_Handling_Suite *memsuite)
{
	return schemaCreate(type, name, memsuite);
}
/******************************************************************************/
static EDI_Schema schemaCreate(enum EDI_DocumentType      type    ,
                               const char                *name    ,
                               EDI_Memory_Handling_Suite *memsuite)
{
	EDI_Schema schema = NULL;
	
	if(memsuite){
        EDI_Memory_Handling_Suite *mtemp;
        schema = (EDI_Schema)\
            memsuite->malloc_fcn(sizeof(struct EDI_SchemaStruct));
        if (schema != NULL) {
				schema->memsuite = (EDI_Memory_Handling_Suite *)\
        			memsuite->malloc_fcn(sizeof(EDI_Memory_Handling_Suite));
            mtemp = (EDI_Memory_Handling_Suite *)schema->memsuite;
            mtemp->malloc_fcn = memsuite->malloc_fcn;
            mtemp->realloc_fcn = memsuite->realloc_fcn;
            mtemp->free_fcn = memsuite->free_fcn;
        }
	} else {
        EDI_Memory_Handling_Suite *mtemp;
        schema = (EDI_Schema)malloc(sizeof(struct EDI_SchemaStruct));
        if (schema != NULL) {
				schema->memsuite = (EDI_Memory_Handling_Suite *)\
        			malloc(sizeof(EDI_Memory_Handling_Suite));
            mtemp = (EDI_Memory_Handling_Suite *)schema->memsuite;
            mtemp->malloc_fcn = malloc;
            mtemp->realloc_fcn = realloc;
            mtemp->free_fcn = free;
        }
	}
	if(schema){
		schemaInit(schema);
		if(name){
			schema->identifier = EDI_strndup(name, strlen(name), schema->memsuite);
		}
		if(type){
			schema->documentType = type;
		}
	}
	return schema;
}
/******************************************************************************/
static void schemaInit(EDI_Schema schema)
{
	schema->identifier       = NULL;
	schema->documentType     = EDI_UNKNOWN_DOC;
	schema->elements         = create_hashtable(20);
	schema->complexNodes     = create_hashtable(20);
	schema->root             = NULL;
	schema->stack[0]         = NULL;
	schema->depth            = 0;
	schema->prevElementNode  = NULL;
	schema->prevElementIndex = 0;
	schema->parser           = NULL;
	schema->loopStartHandler = NULL;
	schema->loopEndHandler   = NULL;
	schema->segmentErrorHandler = NULL;
	schema->elementErrorHandler = NULL;
}
/******************************************************************************/
void EDI_SetDocumentRoot(EDI_Schema            schema,
                         EDI_SchemaNode        node  )
{
	if(schema->stack[0]){
		FREE(schema, schema->stack[0]);
		schema->stack[0] = NULL;
	}
	if(node->type == EDITYPE_DOCUMENT){
		schema->root = (EDI_ComplexType)node;
		EDI_ChildNode root = MALLOC(schema, sizeof(struct EDI_ChildNodeStruct));
		root->node = node;
		root->previousSibling = NULL;
		root->nextSibling = NULL;
		root->min_occurs = 0;
		root->max_occurs = 999999999;
		root->count = 0;
		schema->stack[0] = root;
	}
	return;
}
/******************************************************************************/
EDI_SchemaNode EDI_GetDocumentRoot(EDI_Schema schema)
{
	return &(schema->root->header);
}
/******************************************************************************/
enum EDI_DocumentType EDI_GetSchemaType(EDI_Schema schema)
{
	enum EDI_DocumentType type = EDI_UNKNOWN_DOC;
	if(schema){
		type = schema->documentType;
	}
	return type;
}
/******************************************************************************/
void EDI_SetSchemaType(EDI_Schema schema, enum EDI_DocumentType type)
{
	if(schema){
		schema->documentType = type;
	}
}
/******************************************************************************/
char *EDI_GetSchemaId(EDI_Schema schema)
{
	char *id = "";
	if(schema){
		id = schema->identifier;
	}
	return id;
}
/******************************************************************************/
void EDI_SetSchemaId(EDI_Schema schema, const char *id)
{
	if(schema){
		if(schema->identifier){
			FREE(schema, schema->identifier);
		}
		schema->identifier = EDI_strndup(id, strlen(id), schema->memsuite);
	}
}
/******************************************************************************/
void EDI_SetLoopStartHandler(EDI_Schema schema, EDI_LoopStartHandler h)
{
    schema->loopStartHandler = h;
}
/******************************************************************************/
void EDI_SetLoopEndHandler(EDI_Schema schema, EDI_LoopEndHandler h)
{
    schema->loopEndHandler = h;
}
/******************************************************************************/
void EDI_SetSegmentErrorHandler(EDI_Schema schema, EDI_SegmentErrorHandler h)
{
    schema->segmentErrorHandler = h;
}
/******************************************************************************/
void EDI_SetElementErrorHandler(EDI_Schema schema, EDI_ElementErrorHandler h)
{
    schema->elementErrorHandler = h;
}
/******************************************************************************/
enum EDI_SegmentValidationError EDI_ValidateSegmentPosition(EDI_Schema  schema,
                                                            const char *nodeID)
{
	unsigned int                    startDepth    = schema->depth;
	unsigned int                    startCount    = 0;
	unsigned int                    mCount        = 0;
	EDI_ChildNode                   startNode     = NULL;
	EDI_ChildNode                   current       = NULL;
	enum EDI_SegmentValidationError error         = SEGERR_NONE;
	char                           *mandatory[20];
	
	startNode = schema->stack[startDepth];
	if(!startNode){
		error = SEGERR_UNDEFINED;
	} else {
		startCount = startNode->count;
		current = startNode;
		while(current){
			if(IS_SEGMENT(current->node)){
				if(string_eq(nodeID, current->node->nodeID)){
					if(PARENT_NODE->firstChild == current && current->count > 0){
						while(current){
							if(current->count < current->min_occurs){
								error = SEGERR_MANDATORY;
								if(schema->segmentErrorHandler){
									schema->segmentErrorHandler(
										schema->parser->userData,
										(current->node->type == EDITYPE_LOOP) ? 
											((EDI_LoopNode)current->node)->startID :
											current->node->nodeID,
										error
									);
								}
								error = SEGERR_NONE;
							}
							current->count = 0;
							current = current->nextSibling;
						}
						current = PARENT_NODE->firstChild;
						if(PARENT_NODE->header.type == EDITYPE_LOOP && schema->loopEndHandler){
							schema->loopEndHandler(
								schema->parser->userData,
								PARENT_NODE->header.nodeID
							);
							if(schema->loopStartHandler){
								schema->loopStartHandler(
									schema->parser->userData,
									PARENT_NODE->header.nodeID
								);
							}
						}
					}
					if(schema->depth < startDepth && schema->loopEndHandler){
						int closed;
						for(closed = startDepth; closed > schema->depth; closed--){
							schema->loopEndHandler(
								schema->parser->userData,
								SCHEMA_READ(closed - 1)->node->nodeID
							);
						}
					}
					current->count++;
					if(current->count > current->max_occurs){
							error = SEGERR_EXCEED_REPEAT;
					}
					SCHEMA_SAVE(current);
					break;
				}
			} else if(IS_LOOP(current->node)){
				EDI_LoopNode loop = (EDI_LoopNode)(current->node);
				if(string_eq(nodeID, loop->startID)){
					/* Reset the node usage counts before entering the loop */
					EDI_ChildNode clear = ((EDI_ComplexType)loop)->firstChild;
					while(clear){
						clear->count = 0;
						clear = clear->nextSibling;
					}
					if(schema->depth < startDepth && schema->loopEndHandler){
						int closed;
						for(closed = startDepth; closed > schema->depth; closed--){
							schema->loopEndHandler(
								schema->parser->userData,
								SCHEMA_READ(closed - 1)->node->nodeID
							);
						}
					}
					if(schema->loopStartHandler){
						schema->loopStartHandler(
							schema->parser->userData,
							current->node->nodeID
						);
					}
					SCHEMA_SAVE(current);
					current->count++;
					if(current->count > current->max_occurs){
						error = SEGERR_LOOP_EXCEEDED;
					}
					loop->node.firstChild->count++;
					SCHEMA_PUSH(loop->node.firstChild);
					break;
				}
			}
			if(current->count < current->min_occurs){
				mandatory[mCount++] = current->node->nodeID;
			}
			if(current->nextSibling){
				current = current->nextSibling;
			} else {
				if(schema->depth == startDepth){	
					current = startNode->previousSibling;
					while(current){
						if(string_eq(nodeID, current->node->nodeID) &&
							PARENT_NODE->firstChild != current){
							error = SEGERR_SEQUENCE;
							current->count++;
							if(current->count > current->max_occurs){
								if(schema->segmentErrorHandler){
									schema->segmentErrorHandler(
										schema->parser->userData,
										current->node->nodeID,
										error
									);
								}
								error = SEGERR_EXCEED_REPEAT;
							}
						}
						current = current->previousSibling;
					}
					if(error) break;
				} 
				if(schema->depth > 0){
					current = SCHEMA_POP();
				} else {
					current = schema->root->firstChild;
					if(!string_eq(nodeID, current->node->nodeID)){
						/* Unexpected segment... must reset our position! */
						schema->depth = startDepth;
						startNode->count = startCount;
						if(EDI_GetComplexNodeByID(schema, nodeID)){
							error = SEGERR_UNEXPECTED;
						} else {
							error = SEGERR_UNDEFINED;
						}
						mCount = 0;
						break; /* Wasn't found; cut our losses and go back. */
					}
				}
			}
		}
	}
	if(error && schema->segmentErrorHandler){
		schema->segmentErrorHandler(schema->parser->userData,	nodeID, error);
	}
	if(mCount && schema->segmentErrorHandler){
		int i;
		for(i = 0; i < mCount; i++){
			schema->segmentErrorHandler(
				schema->parser->userData,
				mandatory[i],
				SEGERR_MANDATORY
			);
		}
	}
	return error;
}
/******************************************************************************/
enum EDI_ElementValidationError EDI_ValidateElement(
	EDI_Schema                  schema        ,
   int                         elementIndex  , 
	int                        *componentIndex, 
	const char                 *value         ,
	int                         length        ,
	EDI_DataElement             results       )
{
	int                             index   = 0;
	EDI_ChildNode                   clear   = NULL;
	EDI_ChildNode                   segment = NULL;
	EDI_ChildNode                   element = NULL;
	enum EDI_ElementValidationError error   = VAL_VALID_ELEMENT;
	
	if(elementIndex > 1){
		element = schema->prevElementNode;
	} else {
		segment = schema->stack[schema->depth];
		if(segment){
			clear = element = ((EDI_ComplexType)segment->node)->firstChild;
			while(clear){
				clear->count = 0;
				clear = clear->nextSibling;
			}
			schema->prevElementIndex = 1;	
		} else {
			error = VAL_INVALID_SEGMENT;
		}
	}
	if(!error){
		for(index = schema->prevElementIndex; index < elementIndex; index++){
			if(element->nextSibling){
				element = element->nextSibling;
			} else {
				return VAL_TOO_MANY_ELEMENTS;
			}
		}
		schema->prevElementNode = element;
		schema->prevElementIndex = index;
		if(element->node->type == EDITYPE_COMPOSITE){
			if(*componentIndex == 0 && length > 0){
				*componentIndex = 1;
			}
		}
		if(*componentIndex > 0){
			if(element->node->type != EDITYPE_COMPOSITE){
				return VAL_TOO_MANY_COMPONENTS;
			} else {
				EDI_ChildNode c = ((EDI_ComplexType)element->node)->firstChild;
				if(*componentIndex == 1){
					clear = c;
					while(clear){
						clear->count = 0;
						clear = clear->nextSibling;
					}
				}
				for(index = 1; index < *componentIndex; index++){
					if(c->nextSibling){
						c = c->nextSibling;
					} else {
						return VAL_TOO_MANY_COMPONENTS;
					}
				}
				element = c;
			}
		}
		if(length){
			element->count++;
			if(element->count <= element->max_occurs){
				error = EDI_CheckElementConstraints(
					(EDI_SimpleType *)(element->node),
					value, 
					length,
					results
				);
			} else {
				error = VAL_REPETITION_EXCEEDED;
			}
		} else if(element->min_occurs > 0){
			error = VAL_MANDATORY_ELEMENT;
		}
	}
	if(error && schema->elementErrorHandler){
		schema->elementErrorHandler(
			schema->parser->userData, 
			elementIndex, 
			*componentIndex, 
			error
		);
	}
	return error;
}
/******************************************************************************/
enum EDI_ElementValidationError EDI_ValidateSyntax(EDI_Schema schema       ,
                                                   int        element      ,
                                                   int        finalReceived)
{
	unsigned int                    i, j;
	int                             found     = 0;
	EDI_Bool                        anchor    = EDI_FALSE;
	EDI_ComplexType                 parent    = NULL;
	EDI_SyntaxNote                  note      = NULL;
	EDI_ChildNode                   child     = NULL;
	enum EDI_ElementValidationError error     = VAL_VALID_ELEMENT;

	parent = (EDI_ComplexType)(schema->stack[schema->depth])->node;
	if(parent){
		i = element;
		if(i-- > 0){
			child = parent->firstChild;
			while(i--){
				child = child->nextSibling;
			}
			parent = (EDI_ComplexType)child->node;
		}
		for(child = parent->firstChild, i = 1; i <= parent->childCount; i++){
			if(i > finalReceived && child->count < child->min_occurs){
				if(schema->elementErrorHandler){
					schema->elementErrorHandler(
						schema->parser->userData,
						(element > 0) ? element : i, 
						(element > 0) ? i : 0, 
						VAL_MANDATORY_ELEMENT
					);
				}
			}
			child = child->nextSibling;
		} 
		note = parent->firstNote;
		while(note && !error){
			found = i = j = 0;
			anchor = EDI_FALSE;
			child = parent->firstChild;
			while(child && j < note->count){
				if(++i == note->positions[j]){
					j++;
					if(child->count > 0){
						found++;
						if(j == 1){
							anchor = EDI_TRUE;
						}
					}
				}
				child = child->nextSibling;
			}
			switch(note->type){
			case EDI_SYNTAX_PAIRED:
				if(found && found != note->count){
					error = VAL_MISSING_CONDITIONAL;
					if(schema->elementErrorHandler){
						child = parent->firstChild;
						i = j = 0;
						while(child && j < note->count){
							if(++i == note->positions[j]){
								if(child->count == 0){
									schema->elementErrorHandler(
										schema->parser->userData,
										(element > 0) ? element : note->positions[j],
										(element > 0) ? note->positions[j] : 0,
										error
									);
								}
								j++;
							}
							child = child->nextSibling;
						}
					}
				}
				break;
			case EDI_SYNTAX_REQUIRED:
				if(!found){
					error = VAL_MISSING_CONDITIONAL;
					if(schema->elementErrorHandler){
						schema->elementErrorHandler(
							schema->parser->userData,
							(element > 0) ? element : note->positions[0],
							(element > 0) ? note->positions[0] : 0,
							error
						);
					}
				}
				break;
			case EDI_SYNTAX_EXCLUSION:
				if(found > 1){
					error = VAL_EXCLUSION_VIOLATED;
					if(schema->elementErrorHandler){
						child = parent->firstChild;
						found = i = j = 0;
						while(child && j < note->count){
							if(++i == note->positions[j]){
								if(child->count > 0){
									if(++found > 1){
										schema->elementErrorHandler(
											schema->parser->userData,
											(element > 0) ? element : note->positions[j],
											(element > 0) ? note->positions[j] : 0,
											error
										);
									}
								}
								j++;
							}
							child = child->nextSibling;
						}
					}
				}
				break;
			case EDI_SYNTAX_CONDITION:
				if(anchor && found < note->count){
					error = VAL_MISSING_CONDITIONAL;
					if(schema->elementErrorHandler){
						child = parent->firstChild;
						found = i = j = 0;
						while(child && j < note->count){
							if(++i == note->positions[j]){
								if(child->count == 0){
									schema->elementErrorHandler(
										schema->parser->userData,
										(element > 0) ? element : note->positions[j],
										(element > 0) ? note->positions[j] : 0,
										error
									);
								}
								j++;
							}
							child = child->nextSibling;
						}
					}
				}
				break;
			case EDI_SYNTAX_LIST:
				if(anchor && found == 1){
					error = VAL_MISSING_CONDITIONAL;
					if(schema->elementErrorHandler){
						schema->elementErrorHandler(
							schema->parser->userData,
							(element > 0) ? element : note->positions[1],
							(element > 0) ? note->positions[1] : 0,
							error
						);
					}
				}
				break;
			}
			note = note->next;
		}
	} else {
		error = VAL_INVALID_SEGMENT;
	}
	return error;
}
/******************************************************************************/
void EDI_SchemaFree(EDI_Schema schema)
{
	void (*free_fcn)(void *ptr);

	if(schema->parser && EDI_GetSchemaId(schema->parser->schema) && 
	   schema->identifier &&
	   strcmp(EDI_GetSchemaId(schema->parser->schema), schema->identifier) == 0){
		schema->parser->schema = NULL;
		schema->parser->validate = EDI_FALSE;
	}
	if(schema->identifier){
		FREE(schema, schema->identifier);
	}
	if(schema->root){
		EDI_DisposeNode((EDI_SchemaNode)schema->root);
	}
	if(schema->stack[0]){
		FREE(schema, schema->stack[0]);
	}
	hashtable_destroy(schema->complexNodes, 0);
	hashtable_destroy(schema->elements, 0);
	free_fcn = schema->memsuite->free_fcn;
	free_fcn((void *)schema->memsuite);
	free_fcn(schema);
	schema = NULL;
	return;
}
/******************************************************************************/
void EDI_DisposeNode(EDI_SchemaNode node)
{
	if(node->type == EDITYPE_ELEMENT){
		EDI_DisposeSimpleType(node);
	} else {
		EDI_DisposeComplexType((EDI_ComplexType)node);
	}
}
