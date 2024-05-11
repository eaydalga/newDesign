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
#include "simpletype.h"
/******************************************************************************/
EDI_SchemaNode EDI_CreateComplexType(EDI_Schema         schema,
                                     enum EDI_NodeType  type  ,
                                     const char        *id    )
{
	EDI_ComplexType node = NULL;
	
	if(type == EDITYPE_LOOP || type == EDITYPE_DOCUMENT){
		node = MALLOC(schema, sizeof(struct EDI_LoopNodeStruct));
	} else {
		node = MALLOC(schema, sizeof(struct EDI_ComplexTypeStruct));
	}
	if(!node){
		return NULL;
	}
	node->header.type = type;
	node->header.nodeID = EDI_strndup(id, strlen(id), schema->memsuite);
	node->header.refCount = 0;
	node->header.schema = schema;

	node->firstChild = NULL;
	node->lastChild  = NULL;
	node->childCount = 0;
	node->firstNote  = NULL;
	node->finalNote  = NULL;
	return (EDI_SchemaNode)node;
}
/******************************************************************************/
void EDI_StoreComplexNode(EDI_Schema     schema,
                          EDI_SchemaNode node  )
{
	if(node && node->type != EDITYPE_ELEMENT){
		hashtable_insert(schema->complexNodes,
		                 (void *)EDI_strdup(node->nodeID), 
		                 (void *)node);
	}
	return;
}
/******************************************************************************/
EDI_SchemaNode EDI_GetComplexNodeByID(EDI_Schema      schema,
                                      const char     *nodeID)
{
	EDI_SchemaNode    node  = NULL;
	struct hashtable *table = NULL;
	
	table = schema->complexNodes;
	if((node = (EDI_SchemaNode)hashtable_search(table, (void *)nodeID))){
		return node;
	} else {
		return NULL;
	}
}
/******************************************************************************/
EDI_Bool EDI_AddSyntaxNote(EDI_SchemaNode       node    ,
                           enum EDI_SyntaxType  type    ,
                           unsigned int         count   ,
                           unsigned int        *elements)
{
	int             i;
	unsigned int   *positions;
	EDI_ComplexType cNode;
	EDI_SyntaxNote  note;

	if(node->type == EDITYPE_SEGMENT || node-> type == EDITYPE_COMPOSITE){
		cNode = (EDI_ComplexType)node;
		note = MALLOC(node->schema, sizeof(struct EDI_SyntaxNoteStruct));
		positions = MALLOC(node->schema, sizeof(unsigned int) * count);
		if(note && positions){
			note->type = type;
			for(i = 0; i < count; i++){
				positions[i] = elements[i];
			}
			note->positions = positions;
			note->count = count;
			note->next = NULL;
			if(cNode->firstNote){
				cNode->finalNote->next = cNode->finalNote = note;
			} else {
				cNode->firstNote = cNode->finalNote = note;
			}
		}
		return EDI_TRUE;
	} else {
		fprintf(stderr, "Illegal operation - attempted to add syntax note to schema node of type %d\n", node->type);
		return EDI_FALSE;
	}
}
/******************************************************************************/
static EDI_SchemaNode EDI_RemoveChild(EDI_ComplexType parent,
                                      EDI_ChildNode   child )
{
	EDI_SchemaNode detached = NULL;

    if(child){
	    detached = child->node;
	    if(parent->firstChild == child){
	        parent->firstChild = child->nextSibling;
	        if(parent->firstChild){
	            parent->firstChild->previousSibling = NULL;
	        }
	    } else if(child->nextSibling){
	        child->nextSibling->previousSibling = child->previousSibling;
	    }
	    if(parent->lastChild == child){
	        parent->lastChild = child->previousSibling;
	        if(parent->lastChild){
	            parent->lastChild->nextSibling = NULL;
	        }
	    } else if(child->previousSibling){
	        child->previousSibling->nextSibling = child->nextSibling;
	    }
	    FREE(parent->header.schema, child);
	    parent->childCount--;
	    detached->refCount--;
    }
    return detached;
}
/******************************************************************************/
EDI_SchemaNode EDI_AppendType(EDI_SchemaNode parent    ,
                              EDI_SchemaNode new       ,
                              unsigned int   min_occurs,
                              unsigned int   max_occurs)
{
    EDI_ChildNode   childMeta  = NULL;
    EDI_ComplexType cParent    = (EDI_ComplexType)parent;
    
    if(!parent || !new){
        return NULL;
    }
    switch(parent->type){
    	case EDITYPE_DOCUMENT:
    		if(new->type != EDITYPE_LOOP && 
    		   new->type != EDITYPE_SEGMENT){
    			return NULL;
    		}
    		if(cParent->childCount == 0){
    			EDI_LoopNode loop = (EDI_LoopNode)parent;
    			loop->startID = EDI_strdup(new->nodeID);
    		}
    		break;
    	case EDITYPE_LOOP:
    		if(new->type != EDITYPE_LOOP && 
    		   new->type != EDITYPE_SEGMENT){
    			return NULL;
    		}
    		if(cParent->childCount == 0){
    			EDI_LoopNode loop = (EDI_LoopNode)parent;
    			loop->startID = EDI_strdup(new->nodeID);
    		}
    		break;
    	case EDITYPE_SEGMENT:
    		if(new->type != EDITYPE_COMPOSITE && 
    		   new->type != EDITYPE_ELEMENT){
    			return NULL;
    		}
    		break;
    	case EDITYPE_COMPOSITE:
    		if(new->type != EDITYPE_ELEMENT){
    			return NULL;
    		}
    		break;
    	default:
    		return NULL;
    }
    childMeta = MALLOC(parent->schema, sizeof(struct EDI_ChildNodeStruct));
    if(!childMeta){
    	parent->schema->parser->errorCode = EDI_ERROR_NO_MEM;
    	return NULL;
    }
    childMeta->min_occurs = min_occurs;
    childMeta->max_occurs = max_occurs;
    childMeta->node       = new;
    childMeta->count      = 0;
    
    if(cParent->lastChild){
        cParent->lastChild->nextSibling = childMeta;
    } else {
        cParent->firstChild = childMeta;
    }
	childMeta->previousSibling = cParent->lastChild;
	cParent->lastChild = childMeta;
	childMeta->nextSibling = NULL;
	cParent->childCount++;
	new->refCount++;

	return parent;
}
/******************************************************************************/
EDI_SchemaNode EDI_InsertType(EDI_SchemaNode parent    ,
                              EDI_SchemaNode child     ,
                              unsigned int   position  ,
                              unsigned int   min_occurs,
                              unsigned int   max_occurs)
{
	int index;
	EDI_ChildNode   childMeta  = NULL;
	EDI_ChildNode   sibling    = NULL;
	EDI_ComplexType cParent    = (EDI_ComplexType)parent;
    
	if(!parent || !child){
		return NULL;
	}
	switch(parent->type){
		case EDITYPE_DOCUMENT:
			if(child->type != EDITYPE_LOOP && 
    		   child->type != EDITYPE_SEGMENT){
    			return NULL;
    		}
    		if(cParent->childCount == 0){
    			EDI_LoopNode loop = (EDI_LoopNode)parent;
    			loop->startID = EDI_strdup(child->nodeID);
    		}
    		break;
    	case EDITYPE_LOOP:
    		if(child->type != EDITYPE_LOOP && 
    		   child->type != EDITYPE_SEGMENT){
    			return NULL;
    		}
    		if(cParent->childCount == 0){
    			EDI_LoopNode loop = (EDI_LoopNode)parent;
    			loop->startID = EDI_strdup(child->nodeID);
    		}
    		break;
    	case EDITYPE_SEGMENT:
    		if(child->type != EDITYPE_COMPOSITE && 
    		   child->type != EDITYPE_ELEMENT){
    			return NULL;
    		}
    		break;
    	case EDITYPE_COMPOSITE:
    		if(child->type != EDITYPE_ELEMENT){
    			return NULL;
    		}
    		break;
    	default:
    		return NULL;
    }
    childMeta = MALLOC(parent->schema, sizeof(struct EDI_ChildNodeStruct));
    if(!childMeta){
    	parent->schema->parser->errorCode = EDI_ERROR_NO_MEM;
    	return NULL;
    }
    childMeta->min_occurs = min_occurs;
    childMeta->max_occurs = max_occurs;
    childMeta->node       = child;
    childMeta->count      = 0;

	sibling = cParent->firstChild;
	for(index = 1; sibling && index < position; index++){
		sibling = sibling->nextSibling;
	}
	if(index == position){
		if(sibling){
			if(sibling->previousSibling){
				sibling->previousSibling->nextSibling = childMeta;
			}
			childMeta->previousSibling = sibling->previousSibling;
			sibling->previousSibling = childMeta;
			childMeta->nextSibling = sibling;
		} else {
			if(cParent->lastChild){
				cParent->lastChild->nextSibling = childMeta;
			} else {
				cParent->firstChild = childMeta;
			}
			childMeta->previousSibling = cParent->lastChild;
			cParent->lastChild = childMeta;
			childMeta->nextSibling = NULL;
		}
		cParent->childCount++;
		child->refCount++;
	} else {
		return NULL;
	}
	return parent;
}
/******************************************************************************/
EDI_SchemaNode* EDI_GetChildNodes(EDI_SchemaNode  node,
                                  unsigned int   *argc,
                                  unsigned int   **min,
                                  unsigned int   **max)
{
	EDI_SchemaNode *children = NULL;
	unsigned int *min_occurs, *max_occurs;
	
	*argc = 0;
	if(node->type !=EDITYPE_ELEMENT){
		EDI_ComplexType parent = (EDI_ComplexType)node;
		if(parent && parent->childCount > 0){
			children = MALLOC(
				node->schema, 
				sizeof(struct EDI_SchemaNodeStruct) * parent->childCount
			);
			min_occurs = MALLOC(node->schema, sizeof(int) * parent->childCount);
			max_occurs = MALLOC(node->schema, sizeof(int) * parent->childCount);
			int i;
			EDI_ChildNode child = parent->firstChild;
			for(i = 0; child; i++){
				children[i] = child->node;
				min_occurs[i] = child->min_occurs;
				max_occurs[i] = child->max_occurs;
				child = child->nextSibling;
			}
			*argc = parent->childCount;
			*min = min_occurs;
			*max = max_occurs;
		}
	}
	return children;
}
/******************************************************************************/
void EDI_DisposeComplexType(EDI_ComplexType node)
{
	EDI_SchemaNode child;
	if(node && node->header.refCount == 0){
		while(node->childCount > 0){
			child = EDI_RemoveChild(node, node->firstChild);
			EDI_DisposeNode(child);
		}
		if(node->header.nodeID){
			FREE(node->header.schema, node->header.nodeID);
		}
		if(node->firstNote){
			EDI_SyntaxNote note = node->firstNote;
			while(note){
				EDI_SyntaxNote next = note->next;
				FREE(node->header.schema, note->positions);
				FREE(node->header.schema, note);
				note = next;
			}
		}		
		if(node->header.type == EDITYPE_LOOP || node->header.type == EDITYPE_DOCUMENT){
			EDI_LoopNode l = (EDI_LoopNode)node;
			FREE(node->header.schema, l->startID);
		}
		FREE(node->header.schema, node);
	}
	return;
}
