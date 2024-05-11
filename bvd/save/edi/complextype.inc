<?php
function EDI_CreateComplexType($schema, $type, $id)
{
   $node = EDI_ComplexTypeStruct;
	
   if($type == EDITYPE_LOOP || $type == EDITYPE_DOCUMENT)
      $node = EDI_LoopNodeStruct;
   $node->hheader->type =  $type;
   $node->hheader->nodeID = EDI_strndup($id, strlen($id), $schema.memsuite);
   $node->hheader->refCount = 0;
   $node->hheader->schema = $schema;

   $node->firstChild = NULL;
   $node->lastChild  = NULL;
   $node->childCount = 0;
   $node->firstNote  = NULL;
   $node->finalNote  = NULL;
   return $node;
}
/******************************************************************************/
function EDI_StoreComplexNode($schema, $node)
{
	if($node && $node->type != EDITYPE_ELEMENT){
		hashtable_insert($schema->complexNodes,
		                 EDI_strdup($node->nodeID), 
		                 $node);
	}
}
/******************************************************************************/
function EDI_GetComplexNodeByID($schema, $nodeID)
{
	$node = new EDI_SchemaNode;
	$table = new hashtable;
	
	$table = $schema->complexNodes;
	if($node = hashtable_search($table, $nodeID)){
		return $node;
	} else {
		return "";
	}
}
/******************************************************************************/
function EDI_AddSyntaxNote($node, $type, $count, $elements)
{
	$cNode = new EDI_ComplexType;
	$note = new EDI_SyntaxNote;

	if($node->type == EDITYPE_SEGMENT || $node-> type == EDITYPE_COMPOSITE){
		$cNode = $node;
		$note = new EDI_SyntaxNoteStruct;
		$positions = Array();
		if($note && $positions){
			$note->type = $type;
			for($i = 0; $i < $count; $i++){
				$positions[$i] = $elements[$i];
			}
			$note->positions = $positions;
			$note->count = $count;
			$note->next = NULL;
			if($cNode->firstNote){
				$cNode->finalNote->next = $cNode->finalNote = $note;
			} else {
				$cNode->firstNote = $cNode->finalNote = $note;
			}
		}
		return EDI_TRUE;
	} else {
		echo("Illegal operation - attempted to add syntax note to schema node of type $node->type\n");
		return EDI_FALSE;
	}
}
/******************************************************************************/
function EDI_RemoveChild($parent, $child )
{
    $detached = new EDI_SchemaNode;

    if($child){
	    $detached = $child->node;
	    if($parent->firstChild == $child){
	        $parent->firstChild = $child->nextSibling;
	        if($parent->firstChild){
	            $parent->firstChild->previousSibling = NULL;
	        }
	    } else if($child->nextSibling){
	        $child->nextSibling->previousSibling = $child->previousSibling;
	    }
	    if($parent->lastChild == $child){
	        $parent->lastChild = $child->previousSibling;
	        if($parent->lastChild){
	            $parent->lastChild->nextSibling = NULL;
	        }
	    } else if($child->previousSibling){
	        $child->previousSibling->nextSibling = $child->nextSibling;
	    }
	    $parent->childCount--;
	    $detached->refCount--;
    }
    return $detached;
}
/******************************************************************************/
function EDI_AppendType($parent, $new, $min_occurs, $max_occurs)
{
    $childMeta = new EDI_ChildNode;
    $cParent = new EDI_ComplexType;
    
    if(!$parent || !$new){
        return NULL;
    }
    switch($parent->type){
    	case EDITYPE_DOCUMENT:
    		if($new->type != EDITYPE_LOOP && 
    		   $new->type != EDITYPE_SEGMENT){
    			return NULL;
    		}
    		if($cParent->childCount == 0){
    			$loop = new EDI_LoopNode;
                        $loop = $parent;
    			$loop->startID = EDI_strdup($new->nodeID);
    		}
    		break;
    	case EDITYPE_LOOP:
    		if($new->type != EDITYPE_LOOP && 
    		   $new->type != EDITYPE_SEGMENT){
    			return NULL;
    		}
    		if($cParent->childCount == 0){
    			$loop = new EDI_LoopNode;
                        $loop = $parent;
    			$loop->startID = EDI_strdup($new->nodeID);
    		}
    		break;
    	case EDITYPE_SEGMENT:
    		if($new->type != EDITYPE_COMPOSITE && 
    		   $new->type != EDITYPE_ELEMENT){
    			return NULL;
    		}
    		break;
    	case EDITYPE_COMPOSITE:
    		if($new->type != EDITYPE_ELEMENT){
    			return NULL;
    		}
    		break;
    	default:
    		return NULL;
    }
    $childMeta = new EDI_ChildNodeStruct;
    if(!$childMeta){
    	$parent->schema->parser->errorCode = EDI_ERROR_NO_MEM;
    	return NULL;
    }
    $childMeta->min_occurs = $min_occurs;
    $childMeta->max_occurs = $max_occurs;
    $childMeta->node       = $new;
    $childMeta->count      = 0;
    
    if($cParent->lastChild){
        $cParent->lastChild->nextSibling = $childMeta;
    } else {
        $cParent->firstChild = $childMeta;
    }
	$childMeta->previousSibling = $cParent->lastChild;
	$cParent->lastChild = $childMeta;
	$childMeta->nextSibling = NULL;
	$cParent->childCount++;
	$new->refCount++;

	return $parent;
}
/******************************************************************************/
function EDI_InsertType($parent, $child, $position, $min_occurs, $max_occurs)
{
	$index;
	$chilMeta = new EDI_ChildNode;
	$sibling = new EDI_ChildNode;
	$cParent = new EDI_ComplexType;
        $cParent  = $parent;
    
	if(!$parent || !$child){
		return NULL;
	}
	switch($parent->type){
		case EDITYPE_DOCUMENT:
			if($child->type != EDITYPE_LOOP && 
    		   $child->type != EDITYPE_SEGMENT){
    			return NULL;
    		}
    		if($cParent->childCount == 0){
    			$loop = new EDI_LoopNode;
                        $loop = $parent;
    			$loop->startID = EDI_strdup($child->nodeID);
    		}
    		break;
    	case EDITYPE_LOOP:
    		if($child->type != EDITYPE_LOOP && 
    		   $child->type != EDITYPE_SEGMENT){
    			return NULL;
    		}
    		if($cParent->childCount == 0){
    			$loop = new EDI_LoopNode;
                        $loop = $parent;
    			$loop->startID = EDI_strdup($child->nodeID);
    		}
    		break;
    	case EDITYPE_SEGMENT:
    		if($child->type != EDITYPE_COMPOSITE && 
    		   $child->type != EDITYPE_ELEMENT){
    			return NULL;
    		}
    		break;
    	case EDITYPE_COMPOSITE:
    		if($child->type != EDITYPE_ELEMENT){
    			return NULL;
    		}
    		break;
    	default:
    		return NULL;
    }
    $childMeta = new EDI_ChildNodeStruct;
    if(!$childMeta){
    	$parent->schema->parser->errorCode = EDI_ERROR_NO_MEM;
    	return NULL;
    }
    $childMeta->min_occurs = $min_occurs;
    $childMeta->max_occurs = $max_occurs;
    $childMeta->node       = $child;
    $childMeta->count      = 0;

	$sibling = $cParent->firstChild;
	for($index = 1; $sibling && $index < $position; $index++){
		$sibling = $sibling->nextSibling;
	}
	if($index == $position){
		if($sibling){
			if($sibling->previousSibling){
				$sibling->previousSibling->nextSibling = $childMeta;
			}
			$childMeta->previousSibling = $sibling->previousSibling;
			$sibling->previousSibling = $childMeta;
			$childMeta->nextSibling = $sibling;
		} else {
			if($cParent->$lastChild){
				$cParent->lastChild->nextSibling = childMeta;
			} else {
				$cParent->firstChild = childMeta;
			}
			$childMeta->previousSibling = $cParent->lastChild;
			$cParent->lastChild = $childMeta;
			$childMeta->nextSibling = NULL;
		}
		$cParent->childCount++;
		$child->refCount++;
	} else {
		return NULL;
	}
	return $parent;
}
/******************************************************************************/
function EDI_GetChildNodes($node, $argc, $min, $max)
{
	$children = new EDI_SchemaNode;
        $children = NULL;
	
	$argc = 0;
	if($node->type !=EDITYPE_ELEMENT){
                $parent = new EDI_ComplexType;
                $parent = $node;
		if($parent && $parent->childCount > 0){
			$children = Array();
			$min_occurs = '';
			$max_occurs = '';
			$child = $parent->firstChild;
			for($i = 0; $child; $i++){
				$children[$i] = $child->node;
				$min_occurs[$i] = $child->min_occurs;
				$max_occurs[i] = $child->max_occurs;
				$child = $child->nextSibling;
			}
			$argc = $parent->childCount;
			$min = $min_occurs;
			$max = $max_occurs;
		}
	}
	return $children;
}
/******************************************************************************/
function EDI_DisposeComplexType($node)
{
	$child = EDI_SchemaNode;
	if($node && $node->header->refCount == 0){
		while($node->childCount > 0){
			$child = EDI_RemoveChild($node, $node->firstChild);
			EDI_DisposeNode($child);
		}
		if($node->header.nodeID){
			FREE($node->header->schema, $node->header->nodeID);
		}
		if($node->firstNote){
			$note = $node->firstNote;
			while($note){
				$next = $note->next;
				FREE($node->header->schema, $note->positions);
				FREE($node->header->schema, $note);
				$note = $next;
			}
		}		
		if($node->header->type == EDITYPE_LOOP || $node->header->type == EDITYPE_DOCUMENT){
			$l = $node;
			FREE($node->header->schema, $l->startID);
		}
		FREE($node->header->schema, $node);
	}
	return;
}
?>
