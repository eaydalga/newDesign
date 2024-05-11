<?php
/******************************************************************************/
function SCHEMA_PUSH($segment) {
++$schema->depth;
$schema->stack[$schema->depth] = $segments;
}
function SCHEMA_SAVE($segment) {
$schema->stack[$schema->depth] = $segment;
}
function SCHEMA_POP() {
--$schema->depth;
$schema->stack[$schema->depth];
}
function SCHEMA_READ($depth) {
$schema->stack[$depth];
}
function PARENT_NODE()
{
  return ($schema->depth > 0) ?
     ($schema->stack[$schema->depth - 1]->node) : $schema->root;
}
/******************************************************************************/
function EDI_SchemaCreate($type)
{
    return schemaCreate($type, '', '');    
}
/******************************************************************************/
function EDI_SchemaCreateNamed($type, $name)
{
    return schemaCreate($type, $name, '');    
}
/******************************************************************************/
function EDI_SchemaCreate_MM($type, $memsuite)
{
    return schemaCreate($type, '', $memsuite);
}
/******************************************************************************/
function EDI_SchemaCreateNamed_MM($type, $name, $memsuite)
{
    return schemaCreate($type, $name, $memsuite);
}
/******************************************************************************/
function schemaCreate($type, $name, $memsuite)
{
    $schema = EDI_Schema;
    
    if($memsuite){
        $mtemp = EDI_Memory_Handling_Suite;
        $schema = 
            $memsuite->malloc_fcn(sizeof(EDI_SchemaStruct));
        if ($schema != '') {
                $schema->memsuite = 
                    $memsuite->malloc_fcn(sizeof(EDI_Memory_Handling_Suite));
            $mtemp = $schema->memsuite;
            $mtemp->malloc_fcn = $memsuite->malloc_fcn;
            $mtemp->realloc_fcn = $memsuite->realloc_fcn;
            $mtemp->free_fcn = $memsuite->free_fcn;
        }
    } else {
        $mtemp = EDI_Memory_Handling_Suite;
        $schema = malloc(sizeof(EDI_SchemaStruct));
        if ($schema != '') {
                $schema->memsuite = 
                    malloc(sizeof(EDI_Memory_Handling_Suite));
            $mtemp = $schema->memsuite;
            $mtemp->malloc_fcn = "malloc";
            $mtemp->realloc_fcn = "realloc";
            $mtemp->free_fcn = "free";
        }
    }
    if($schema){
        schemaInit($schema);
        if($name){
            $schema->identifier = EDI_strndup($name, strlen($name), $schema->memsuite);
        }
        if($type){
            $schema->documentType = $type;
        }
    }
    return $schema;
}
/******************************************************************************/
function schemaInit($schema)
{
    $schema->identifier       = '';
    $schema->documentType     = EDI_UNKNOWN_DOC;
    $schema->elements         = create_hashtable(20);
    $schema->complexNodes     = create_hashtable(20);
    $schema->root             = '';
    $schema->stack[0]         = '';
    $schema->depth            = 0;
    $schema->prevElementNode  = '';
    $schema->prevElementIndex = 0;
    $schema->parser           = '';
    $schema->loopStartHandler = '';
    $schema->loopEndHandler   = '';
    $schema->segmentErrorHandler = '';
    $schema->elementErrorHandler = '';
}
/******************************************************************************/
function EDI_SetDocumentRoot($schema, $node  )
{
    if($schema->stack[0]){
        FREE($schema, $schema->stack[0]);
        $schema->stack[0] = '';
    }
    if($node->type == EDITYPE_DOCUMENT){
        $schema->root = $node;
        $root = MALLOC(schema, sizeof(EDI_ChildNodeStruct));
        $root->node = node;
        $root->previousSibling = '';
        $root->nextSibling = '';
        $root->min_occurs = 0;
        $root->max_occurs = 999999999;
        $root->count = 0;
        $schema->stack[0] = $root;
    }
    return;
}
/******************************************************************************/
function EDI_GetDocumentRoot($schema)
{
    return $schema->root->header;
}
/******************************************************************************/
function EDI_GetSchemaType($schema)
{
    $type = EDI_UNKNOWN_DOC;
    if($schema){
        $type = $schema->documentType;
    }
    return $type;
}
/******************************************************************************/
function EDI_SetSchemaType($schema, $type)
{
    if($schema){
        $schema->documentType = $type;
    }
}
/******************************************************************************/
function EDI_GetSchemaId($schema)
{
    if($schema){
        $id = $schema->identifier;
    }
    return $id;
}
/******************************************************************************/
function EDI_SetSchemaId($schema, $id)
{
    if($schema){
        if($schema->identifier){
            FREE($schema, $schema->identifier);
        }
        $schema->identifier = EDI_strndup($id, strlen($id), $schema->memsuite);
    }
}
/******************************************************************************/
function EDI_SetLoopStartHandler($schema, $h)
{
    $schema->loopStartHandler = $h;
}
/******************************************************************************/
function EDI_SetLoopEndHandler($schema, $h)
{
    $schema->loopEndHandler = $h;
}
/******************************************************************************/
function EDI_SetSegmentErrorHandler($schema, $h)
{
    $schema->segmentErrorHandler = $h;
}
/******************************************************************************/
function EDI_SetElementErrorHandler($schema, $h)
{
    $schema->elementErrorHandler = $h;
}
/******************************************************************************/
function EDI_ValidateSegmentPosition($schema, $nodeID)
{
    $startDepth    = $schema->depth;
    $startCount    = 0;
    $mCount        = 0;
    $startNode     = '';
    $current       = '';
    $error         = SEGERR_NONE;
    $mandatory[20];
    
    $startNode = $schema->stack[$startDepth];
    if(!$startNode){
        $error = SEGERR_UNDEFINED;
    } else {
        $startCount = $startNode->count;
        $current = $startNode;
        while($current){
            if(IS_SEGMENT($current->node)){
                if(string_eq($nodeID, $current->node->nodeID)){
                    if($PARENT_NODE->firstChild == $current && $current->count > 0){
                        while($current){
                            if($current->count < $current->min_occurs){
                                $error = SEGERR_MANDATORY;
                                if($schema->segmentErrorHandler){
                                    $schema->segmentErrorHandler(
                                        $schema->parser->userData,
                                        ($current->node->type == EDITYPE_LOOP) ? 
                                            $current->node->startID :
                                            $current->node->nodeID,
                                        $error
                                    );
                                }
                                $error = SEGERR_NONE;
                            }
                            $current->count = 0;
                            $current = $current->nextSibling;
                        }
                        $current = $PARENT_NODE->firstChild;
                        if($PARENT_NODE->header->type == EDITYPE_LOOP && $schema->loopEndHandler){
                            $schema->loopEndHandler(
                                $schema->parser->userData,
                                $PARENT_NODE->header->nodeID
                            );
                            if($schema->loopStartHandler){
                                $schema->loopStartHandler(
                                    $schema->parser->userData,
                                    $PARENT_NODE->header->nodeID
                                );
                            }
                        }
                    }
                    if($schema->depth < $startDepth && $schema->loopEndHandler){
                        for($closed = $startDepth; $closed > $schema->depth; $closed--){
                            $schema->loopEndHandler(
                                $schema->parser->userData,
                                $SCHEMA_READ($closed - 1)->node->nodeID
                            );
                        }
                    }
                    $current->count++;
                    if($current->count > $current->max_occurs){
                            $error = SEGERR_EXCEED_REPEAT;
                    }
                    SCHEMA_SAVE($current);
                    break;
                }
            } else if(IS_LOOP($current->node)){
                $loop = $current->node;
                if(string_eq($nodeID, $loop->startID)){
                    /* Reset the node usage counts before entering the loop */
                    $clear = $loop->firstChild;
                    while($clear){
                        $clear->count = 0;
                        $clear = $clear->nextSibling;
                    }
                    if($schema->depth < $startDepth && $schema->loopEndHandler){
                        for($closed = $startDepth; $closed > $schema->depth; $closed--){
                            $schema->loopEndHandler(
                                $schema->parser->userData,
                                $SCHEMA_READ($closed - 1)->node->nodeID
                            );
                        }
                    }
                    if($schema->loopStartHandler){
                        $schema->loopStartHandler(
                            $schema->parser->userData,
                            $current->node->nodeID
                        );
                    }
                    SCHEMA_SAVE($current);
                    $current->count++;
                    if($current->count > $current->max_occurs){
                        $error = SEGERR_LOOP_EXCEEDED;
                    }
                    $loop->node->firstChild->count++;
                    SCHEMA_PUSH($loop->node->firstChild);
                    break;
                }
            }
            if($current->count < $current->min_occurs){
                $mandatory[$mCount++] = $current->node->nodeID;
            }
            if($current->nextSibling){
                $current = $current->nextSibling;
            } else {
                if($schema->depth == $startDepth){    
                    $current = $startNode->previousSibling;
                    while($current){
                        if(string_eq($nodeID, $current->node->nodeID) &&
                            $PARENT_NODE->firstChild != $current){
                            $error = SEGERR_SEQUENCE;
                            $current->count++;
                            if($current->count > $current->max_occurs){
                                if($schema->segmentErrorHandler){
                                    $schema->segmentErrorHandler(
                                        $schema->parser->userData,
                                        $current->node->nodeID,
                                        $error
                                    );
                                }
                                $error = SEGERR_EXCEED_REPEAT;
                            }
                        }
                        $current = $current->previousSibling;
                    }
                    if($error) break;
                } 
                if($schema->depth > 0){
                    $current = SCHEMA_POP();
                } else {
                    $current = $schema->root->firstChild;
                    if(!string_eq($nodeID, $current->node->nodeID)){
                        /* Unexpected segment... must reset our position! */
                        $schema->depth = $startDepth;
                        $startNode->count = $startCount;
                        if(EDI_GetComplexNodeByID($schema, $nodeID)){
                            $error = SEGERR_UNEXPECTED;
                        } else {
                            $error = SEGERR_UNDEFINED;
                        }
                        $mCount = 0;
                        break; /* Wasn't found; cut our losses and go back. */
                    }
                }
            }
        }
    }
    if($error && $schema->segmentErrorHandler){
        $schema->segmentErrorHandler($schema->parser->userData,    $nodeID, $error);
    }
    if($mCount && $schema->segmentErrorHandler){
        for($i = 0; $i < $mCount; $i++){
            $schema->segmentErrorHandler(
                $schema->parser->userData,
                $mandatory[$i],
                SEGERR_MANDATORY
            );
        }
    }
    return $error;
}
/******************************************************************************/
function EDI_ValidateElement( $schema, $elementIndex, $componentIndex, $value, $length, $results)
{
    $index   = 0;
    $clear   = '';
    $segmet = new EDI_ChildNode;
    $element = new EDI_ChildNode;
    $error   = VAL_VALID_ELEMENT;
    
    if($elementIndex > 1){
        $element = $schema->prevElementNode;
    } else {
        $segment = $schema->stack[$schema->depth];
        if($segment){
            $clear = $element = $segment->node->firstChild;
            while($clear){
                $clear->count = 0;
                $clear = $clear->nextSibling;
            }
            $schema->prevElementIndex = 1;    
        } else {
            $error = VAL_INVALID_SEGMENT;
        }
    }
    if(!$error){
        for($index = $schema->prevElementIndex; $index < $elementIndex; $index++){
            if($element->nextSibling){
                $element = $element->nextSibling;
            } else {
                return VAL_TOO_MANY_ELEMENTS;
            }
        }
        $schema->prevElementNode = $element;
        $schema->prevElementIndex = $index;
        if($element->node->type == EDITYPE_COMPOSITE){
            if($componentIndex == 0 && $length > 0){
                $componentIndex = 1;
            }
        }
        if($componentIndex > 0){
            if($element->node->type != EDITYPE_COMPOSITE){
                return VAL_TOO_MANY_COMPONENTS;
            } else {
                $c = $element->node->firstChild;
                if($componentIndex == 1){
                    $clear = $c;
                    while($clear){
                        $clear->count = 0;
                        $clear = $clear->nextSibling;
                    }
                }
                for($index = 1; $index < $componentIndex; $index++){
                    if($c->nextSibling){
                        $c = $c->nextSibling;
                    } else {
                        return VAL_TOO_MANY_COMPONENTS;
                    }
                }
                $element = $c;
            }
        }
        if($length){
            $element->count++;
            if($element->count <= $element->max_occurs){
                $error = EDI_CheckElementConstraints(
                    $element->node,
                    $value, 
                    $length,
                    $results
                );
            } else {
                $error = VAL_REPETITION_EXCEEDED;
            }
        } else if($element->min_occurs > 0){
            $error = VAL_MANDATORY_ELEMENT;
        }
    }
    if($error && $schema->elementErrorHandler){
        $schema->elementErrorHandler(
            $schema->parser->userData, 
            $elementIndex, 
            $componentIndex, 
            $error
        );
    }
    return $error;
}
/******************************************************************************/
function EDI_ValidateSyntax($schema, $element, $finalReceived)
{
    $found     = 0;
    $anchor    = EDI_FALSE;
    $parent    = '';
    $note      = '';
    $child     = '';
    $error = new EDI_ElementValidationError;
    $error     = VAL_VALID_ELEMENT;

    $parent = $schema->stack[$schema->depth]->node;
    if($parent){
        $i = $element;
        if($i-- > 0){
            $child = $parent->firstChild;
            while($i--){
                $child = $child->nextSibling;
            }
            $parent = $child->node;
        }
        for($child = $parent->firstChild, $i = 1; $i <= $parent->childCount; $i++){
            if($i > $finalReceived && $child->count < $child->$min_occurs){
                if($schema->elementErrorHandler){
                    $schema->elementErrorHandler(
                        $schema->parser->userData,
                        ($element > 0) ? $element : $i, 
                        ($element > 0) ? $i : 0, 
                        VAL_MANDATORY_ELEMENT
                    );
                }
            }
            $child = $child->nextSibling;
        } 
        $note = $parent->firstNote;
        while($note && !$error){
            $found = $i = $j = 0;
            $anchor = EDI_FALSE;
            $child = $parent->firstChild;
            while($child && $j < $note->count){
                if(++$i == $note->positions[j]){
                    $j++;
                    if($child->count > 0){
                        $found++;
                        if($j == 1){
                            $anchor = EDI_TRUE;
                        }
                    }
                }
                $child = $child->nextSibling;
            }
            switch($note->type){
            case EDI_SYNTAX_PAIRED:
                if($found && $found != $note->count){
                    $error = VAL_MISSING_CONDITIONAL;
                    if($schema->elementErrorHandler){
                        $child = $parent->firstChild;
                        $i = $j = 0;
                        while($child && $j < $note->count){
                            if(++$i == $note->positions[j]){
                                if($child->count == 0){
                                    $schema->elementErrorHandler(
                                        $schema->parser->userData,
                                        ($element > 0) ? $element : $note->positions[j],
                                        ($element > 0) ? $note->positions[j] : 0,
                                        $error
                                    );
                                }
                                $j++;
                            }
                            $child = $child->nextSibling;
                        }
                    }
                }
                break;
            case EDI_SYNTAX_REQUIRED:
                if(!$found){
                    $error = VAL_MISSING_CONDITIONAL;
                    if($schema->elementErrorHandler){
                        $schema->elementErrorHandler(
                            $schema->parser->userData,
                            ($element > 0) ? $element : $note->positions[0],
                            ($element > 0) ? $note->positions[0] : 0,
                            $error
                        );
                    }
                }
                break;
            case EDI_SYNTAX_EXCLUSION:
                if($found > 1){
                    $error = VAL_EXCLUSION_VIOLATED;
                    if($schema->elementErrorHandler){
                        $child = $parent->firstChild;
                        $found = $i = $j = 0;
                        while($child && $j < $note->count){
                            if(++$i == $note->positions[j]){
                                if($child->count > 0){
                                    if(++$found > 1){
                                        $schema->elementErrorHandler(
                                            $schema->parser->userData,
                                            ($element > 0) ? $element : $note->positions[j],
                                            ($element > 0) ? $note->positions[j] : 0,
                                            $error
                                        );
                                    }
                                }
                                $j++;
                            }
                            $child = $child->nextSibling;
                        }
                    }
                }
                break;
            case EDI_SYNTAX_CONDITION:
                if($anchor && $found < $note->count){
                    $error = VAL_MISSING_CONDITIONAL;
                    if($schema->elementErrorHandler){
                        $child = $parent->firstChild;
                        $found = $i = $j = 0;
                        while($child && $j < $note->count){
                            if(++$i == $note->positions[j]){
                                if($child->count == 0){
                                    $schema->elementErrorHandler(
                                        $schema->parser->userData,
                                        ($element > 0) ? $element : $note->positions[$j],
                                        ($element > 0) ? $note->positions[$j] : 0,
                                        $error
                                    );
                                }
                                $j++;
                            }
                            $child =$child->nextSibling;
                        }
                    }
                }
                break;
            case EDI_SYNTAX_LIST:
                if($anchor && $found == 1){
                    $error = VAL_MISSING_CONDITIONAL;
                    if($schema->elementErrorHandler){
                        $schema->elementErrorHandler(
                            $schema->parser->userData,
                            ($element > 0) ? $element : $note->positions[1],
                            ($element > 0) ? $note->positions[1] : 0,
                            $error
                        );
                    }
                }
                break;
            }
            $note = $note->next;
        }
    } else {
        $error = VAL_INVALID_SEGMENT;
    }
    return $error;
}
/******************************************************************************/
function EDI_SchemaFree($schema)
{
    // void (*free_fcn)(void *ptr);

    if($schema->parser && EDI_GetSchemaId($schema->parser->schema) && 
       $schema->identifier &&
       strcmp(EDI_GetSchemaId($schema->parser->schema), $schema->identifier) == 0){
        $schema->parser->schema = '';
        $schema->parser->validate = EDI_FALSE;
    }
    if($schema->identifier){
        FREE($schema, $schema->identifier);
    }
    if($schema->root){
        EDI_DisposeNode($schema->root);
    }
    if($schema->stack[0]){
        FREE($schema, $schema->stack[0]);
    }
    hashtable_destroy($schema->complexNodes, 0);
    hashtable_destroy($schema->elements, 0);
    $free_fcn = $schema->memsuite->free_fcn;
    free_fcn($schema->memsuite);
    free_fcn($schema);
    $schema = '';
    return;
}
/******************************************************************************/
function EDI_DisposeNode($node)
{
    if($node->type == EDITYPE_ELEMENT){
        EDI_DisposeSimpleType($node);
    } else {
        EDI_DisposeComplexType($node);
    }
}
