<?php
/******************************************************************************/
function X12_NEXT_TOKEN($string, $delimiters, $tok)
{
$tok->type  = UNKNOWN;
$tok->token = '';
$tok->length = 0;
if($string){
    for($iter = $string; $iter; $iter++){
        if(!($iter ^ $delimiters[ELEMENT])){
            $tok->type = ELEMENT;
            $iter++;
            $iter = '0';
            $tok->token = $string;
            $string = $iter;
            break;
        } else if(!($iter ^ $delimiters[SEGMENT])){
            $tok->type = SEGMENT;
            $iter++;
            $iter = '0';
            $tok->token = $string;
            $string = $iter;
            break;
        } else if(!($iter ^ $delimiters[COMPONENT])){
            $tok->type = COMPONENT;
            $iter++;
            $iter = '0';
            $tok->token = $string;
            $string = $iter;
            break;
        } else if(!($iter ^ $delimiters[REPEAT])){
            $tok->type = REPEAT;
            $iter++;
            $iter = '0';
            $tok->token = $string;
            $string = $iter;
            break;
        }
        $tok->length++;
      }
    }
}
/******************************************************************************/
function x12ELEMENT_VALIDATE($EDI_PARSER, $X12_PARSER, $element, $component, $value, $length) 
{
if(($EDI_PARSER->validate ^ $X12_PARSER->segmentError) == EDI_TRUE)
    EDI_ValidateElement(  
        $EDI_PARSER->schema,
        $element           ,
        $component         ,
        $value             ,
        $length            ,
        $X12_PARSER->data   
    );                    
}
/******************************************************************************/
function X12_ProcessISA($parser)
{
    $ISA_seps[16] = Array(  3,  6, 17, 20, 31, 34,  50,  53,
                           69, 76, 81, 83, 89, 99, 101, 103 );
    $i = 0;  $j = 0;
    $elesep       = 0;
    $bufIter      = '';
    $componentSep = '';
    $repeatSep    = '';
    $tok = token;

    if(($EDI_PARSER->bufEndPtr - $EDI_PARSER->bufReadPtr) < 106){
        EDI_SetResumeState($EDI_PARSER->machine, $EDI_PARSER->process);
        $EDI_PARSER->errorCode = EDI_ERROR_BUFFER_END;
        return $EDI_PARSER->error;
    }
    $bufIter = $EDI_PARSER->bufReadPtr;
    $elesep = $bufIter[3];
    for($i = 0; $i < 16; $i++){
        for($j = 0; $j < 106; $j++){
            if($j == $ISA_seps[$i]){
                if($bufIter[$j] == $elesep){
                    $j=$ISA_seps[$i]+1;
                    $i++;
                } else {
                    $EDI_PARSER->bufReadPtr += 3;
                    return $EDI_PARSER->seekHeader;
                }
            } else if($bufIter[$j] == $elesep){
                $EDI_PARSER->bufReadPtr += 3;
                return $EDI_PARSER->seekHeader;
            }
        }
    }
    $EDI_PARSER->bufReadPtr += 106;
    $X12_PARSER->delimiters[ELEMENT] = $bufIter[3];
    $X12_PARSER->delimiters[COMPONENT] = '\0';
    $X12_PARSER->delimiters[REPEAT] = '\0';
    $X12_PARSER->delimiters[SEGMENT] = $bufIter[105];
    $repeatSep                       = $bufIter[82];
    $componentSep                    = $bufIter[104];
    $EDI_PARSER->documentStartHandler($EDI_PARSER->userData, $EDI_ANSI_X12);    
    $X12_NEXT_TOKEN(bufIter, $X12_PARSER->delimiters, $tok);
    if($EDI_PARSER->schema && $EDI_PARSER->schema->documentType == EDI_ANSI_X12){
        $EDI_PARSER->validate = EDI_TRUE;
        $X12_PARSER->segmentError = EDI_ValidateSegmentPosition($EDI_PARSER->schema, "ISA");
    } else {
        $EDI_PARSER->validate = EDI_FALSE;
    }
    $EDI_PARSER->segmentStartHandler($EDI_PARSER->userData, "ISA");
    $i = $j = 0;
    while($tok->type != SEGMENT){
        X12_NEXT_TOKEN($bufIter, $X12_PARSER->delimiters, $tok);
        $i++;
        /* Setup the defaults: string data and the unconverted input string */
        $X12_PARSER->data->type = EDI_DATA_STRING;
        $X12_PARSER->data->data->string = $tok->token;
        switch($tok->type){
            case ELEMENT:
            {
                x12ELEMENT_VALIDATE($EDI_PARSER, $X12_PARSER, $i, $j, $tok->token, $tok->length);
                if($i == 12){
                    if(strcmp($tok->token, "00402") >= 0){
                        $X12_PARSER->delimiters[REPEAT] = $repeatSep;
                    }
                }
                $EDI_PARSER->elementHandler($EDI_PARSER->userData, $X12_PARSER->data);
                break;
            }
            case SEGMENT:
                x12ELEMENT_VALIDATE($EDI_PARSER, $X12_PARSER, $i, $j, $tok->token, $tok->length);
                $EDI_PARSER->elementHandler($EDI_PARSER->userData, $X12_PARSER->data);
                $EDI_PARSER->segmentEndHandler($EDI_PARSER->userData, "ISA");
                break;
            default:
                break;
        }
    }
    $X12_PARSER->delimiters[COMPONENT] = $componentSep;
    $X12_PARSER->savedElementPosition = 0;
    $X12_PARSER->savedComponentPosition = 0;
    $X12_PARSER->previous = SEGMENT;
    while($bufIter < $EDI_PARSER->bufEndPtr){
        if($bufIter++ == $X12_PARSER->delimiters[ELEMENT]){
            $bufIter -= 4;
            if(!(isalnum($bufIter))){
                $bufIter++;
            }
            EDI_GAP_SCAN(EDI_PARSER, bufIter);
            return $X12_ProcessMessage;
        }
    }
    EDI_SetResumeState($EDI_PARSER->machine, $X12_ProcessMessage);
    $EDI_PARSER->errorCode = EDI_ERROR_BUFFER_END;
    return $EDI_PARSER->error;
}
/******************************************************************************/
function X12_ProcessMessage(EDI_Parser $parser)
{
    $element   = 0;
    $component = 0;
    $tag       = '';
    $bufIter   = '';
    $tok = token;

    $bufIter = $EDI_PARSER->bufReadPtr;
    X12_NEXT_TOKEN($bufIter, $X12_PARSER->delimiters, $tok);
    if($X12_PARSER->previous == SEGMENT){
        if($tok->type == ELEMENT){
            while(!(isalnum($tok->token[0]))){
                $tok->token++;
            }
            EDI_GAP_SCAN($EDI_PARSER, $tok->token);
            if(string_eq($tok->token, "ISA")){
                /* If this happens, we had an incomplete interchange (missing IEA) 
                 * and we were lucky enough that the next interchange used the same
                 * delimiter. */
                $bufIter = $X12_PARSER->delimiters[ELEMENT];
                --$bufIter;
                $bufIter -= 3;
                $EDI_PARSER->bufReadPtr = $bufIter;
                return $X12_ProcessIEA;
            }
        }
    } else {
        $tag = $X12_PARSER->savedTag;
        $element = $X12_PARSER->savedElementPosition;
        $component = $X12_PARSER->savedComponentPosition;
    }
    while($tok->type != UNKNOWN){
        $EDI_PARSER->bufReadPtr = $bufIter;
        /* Setup the defaults: string data and the unconverted input string */
        $X12_PARSER->data->type = EDI_DATA_STRING;
        $X12_PARSER->data->data->string = $tok->token;
        switch($tok->type){
            case REPEAT:
            case ELEMENT:
                switch($X12_PARSER->previous){
                    case COMPONENT:
                        $component++;
                        x12ELEMENT_VALIDATE($EDI_PARSER, $X12_PARSER, $element, $component, $tok->token, $tok->length);
                        $EDI_PARSER->elementHandler($EDI_PARSER->userData, $X12_PARSER->data);
                          if(($EDI_PARSER->validate ^ $X12_PARSER->segmentError) == EDI_TRUE){
                            EDI_ValidateSyntax($EDI_PARSER->schema, $element, $component);
                          }
                          $EDI_PARSER->compositeEndHandler($EDI_PARSER->userData);
                        break;
                    case SEGMENT:
                        $tag = $tok->token;
                        $X12_PARSER->segmentError = SEGERR_NONE;
                        if($EDI_PARSER->validate){
                            $X12_PARSER->segmentError = EDI_ValidateSegmentPosition($EDI_PARSER->schema, $tag);
                        } else {
                            $X12_PARSER->segmentError = SEGERR_UNDEFINED;
                        }
                        $EDI_PARSER->segmentStartHandler($EDI_PARSER->userData, $tag);
                        break;
                    case ELEMENT:
                        $element++;
                    case REPEAT:
                        $component = 0;
                        x12ELEMENT_VALIDATE($EDI_PARSER, $X12_PARSER, $element, $component, $tok->token, $tok->length);
                        if($component == 1){
                            $EDI_PARSER->compositeStartHandler($EDI_PARSER->userData);
                            $EDI_PARSER->elementHandler($EDI_PARSER->userData, $X12_PARSER->data);
                              if(($EDI_PARSER->validate ^ $X12_PARSER->segmentError) == EDI_TRUE){
                                EDI_ValidateSyntax($EDI_PARSER->schema, $element, $component);
                              }
                            $EDI_PARSER->compositeEndHandler($EDI_PARSER->userData);
                        } else {
                            $EDI_PARSER->elementHandler($EDI_PARSER->userData, $X12_PARSER->data);
                        }
                        if($tok->type == REPEAT){
                            if($EDI_PARSER->repeatHandler){
                                $EDI_PARSER->repeatHandler($EDI_PARSER->userData);
                            } else {
                                echo( "FATAL (edival): A repeated element was found in the data stream but no \n\
                                callback function has been registered for the event using EDI_SetRepeatHandler()\n");
                                $EDI_PARSER->errorCode = $EDI_ERROR_INVALID_STATE;
                                return $EDI_PARSER->error;
                            }
                        }
                        break;
                    default: ;
                }
                break;
            case COMPONENT:
                if($X12_PARSER->previous == ELEMENT){
                    $element++;
                    $component = 0;
                } else if($X12_PARSER->previous == REPEAT){
                    $component = 0;
                }
                $component++;
                x12ELEMENT_VALIDATE($EDI_PARSER, $X12_PARSER, $element, $component, $tok->token, $tok->length);
                 if($component == 1){
                     $EDI_PARSER->compositeStartHandler($EDI_PARSER->userData);
                 }
                 $EDI_PARSER->elementHandler($EDI_PARSER->userData, $X12_PARSER->data);
                break;
            case SEGMENT:
                if($X12_PARSER->previous == COMPONENT){
                    $component++;
                    x12ELEMENT_VALIDATE($EDI_PARSER, $X12_PARSER, $element, $component, $tok->token, $tok->length);
                    $EDI_PARSER->elementHandler($EDI_PARSER->userData, $X12_PARSER->data);
                      if(($EDI_PARSER->validate ^ $X12_PARSER->segmentError) == EDI_TRUE){
                        EDI_ValidateSyntax($EDI_PARSER->schema, $element, $component);
                      }
                    $EDI_PARSER->compositeEndHandler($EDI_PARSER->userData);
                } else {
                    if($X12_PARSER->previous == ELEMENT){
                        /* Only increment the element count if this isn't a repeated element. */
                        $element++;
                    }
                    $component = 0;
                    x12ELEMENT_VALIDATE($EDI_PARSER, $X12_PARSER, $element, $component, $tok->token, $tok->length);
                     if($component == 1){
                         $EDI_PARSER->compositeStartHandler($EDI_PARSER->userData);
                         $EDI_PARSER->elementHandler($EDI_PARSER->userData, $X12_PARSER->data);
                          if(($EDI_PARSER->validate ^ $X12_PARSER->segmentError) == EDI_TRUE){
                            EDI_ValidateSyntax($EDI_PARSER->schema, $element, $component);
                          }
                         $EDI_PARSER->compositeEndHandler($EDI_PARSER->userData);
                     } else {
                         $EDI_PARSER->elementHandler($EDI_PARSER->userData, $X12_PARSER->data);
                     }
                }
                  if(($EDI_PARSER->validate ^ $X12_PARSER->segmentError) == EDI_TRUE){
                    EDI_ValidateSyntax($EDI_PARSER->schema, 0, $element);
                  }
                $EDI_PARSER->segmentEndHandler($EDI_PARSER->userData, $tag);
                $element = 0;
                $component = 0;
                if($tag[0] == 'I' && $tag[1] == 'E' && $tag[2] == 'A'){
                    $X12_PARSER->previous = $tok->type;
                    return $X12_ProcessIEA;
                } else {
                    $tag = '';
                    while($bufIter < $EDI_PARSER->bufEndPtr){
                        if($bufIter++ == $X12_PARSER->delimiters[ELEMENT]){
                            $bufIter -= 4;
                            while(!(isalnum($bufIter))){
                                $bufIter++;
                            }
                            EDI_GAP_SCAN($EDI_PARSER, $bufIter);
                            if($bufIter[0] == 'I' && $bufIter[1] == 'S' && $bufIter[2] == 'A'){
                                /* If this happens, we probably have a bad interchange. (missing IEA)
                                 * Same scenario as above. */
                                $X12_PARSER->previous = $tok->type;
                                return $X12_ProcessIEA;
                            }
                            break;
                        }
                    }
                }
                break;
            default: ;
        }
        if($EDI_PARSER->binaryElementSize){
            if($EDI_PARSER->binBuffer){
                FREE($parser, $EDI_PARSER->binBuffer);
            }
            $EDI_PARSER->binBuffer = MALLOC($parser, $EDI_PARSER->binaryElementSize * sizeof(char));
            if($X12_PARSER->savedTag){
                free($X12_PARSER->savedTag);
            }
            $X12_PARSER->savedTag = EDI_strdup($tag);
            $X12_PARSER->savedElementPosition = $element;
            $X12_PARSER->savedComponentPosition = $component;
            return $X12_ProcessBinaryElement;
        } else {
            $X12_PARSER->previous = $tok->type;
            X12_NEXT_TOKEN($bufIter, $X12_PARSER->delimiters, $tok);
        }
    }
    if($tag){
        if($X12_PARSER->savedTag){
            free($X12_PARSER->savedTag);
        }
        $X12_PARSER->savedTag = EDI_strdup($tag);
        $X12_PARSER->savedElementPosition = $element;
        $X12_PARSER->savedComponentPosition = $component;
    }
    EDI_SetResumeState($EDI_PARSER->machine, $X12_ProcessMessage);
    $EDI_PARSER->errorCode = EDI_ERROR_BUFFER_END;
    return $EDI_PARSER->error;
}
/******************************************************************************/
function X12_ProcessBinaryElement($parser)
{
    $bufIter  = $EDI_PARSER->bufReadPtr;
    $size     = $EDI_PARSER->binaryElementSize;
    $finished = $EDI_PARSER->bytesHandled;
    $type = X12_Delimiter;
    
    /*  
     *  If the current buffer does not hold the entire binary element, we
     *  will fall back to the caller who has the responsibilty to load more
     *  data to the buffer.  This will repeat until the entire binary element
     *  is present in memory. 
     */
    if(($EDI_PARSER->bufEndPtr - $bufIter) > ($size - $finished)){
        memcpy(
            ($EDI_PARSER->binBuffer[$finished]), 
            $bufIter, 
            ($size - $finished)
        );
        $finished = $size;
        $EDI_PARSER->bufReadPtr += $size;
    } else {
        memcpy(
            ($EDI_PARSER->binBuffer[$finished]), 
            $bufIter,
            ($EDI_PARSER->bufEndPtr - $bufIter)
        );
        $finished += ($EDI_PARSER->bufEndPtr - $bufIter);
        $EDI_PARSER->bufReadPtr = $EDI_PARSER->bufEndPtr;
    }
    if($finished == $size){
        if(!($EDI_PARSER->bufReadPtr ^ $X12_PARSER->delimiters[REPEAT])){
            $type = REPEAT;
        } else if(!($EDI_PARSER->bufReadPtr ^ $X12_PARSER->delimiters[ELEMENT])){
            $type = ELEMENT;
        } else if(!($EDI_PARSER->bufReadPtr ^ $X12_PARSER->delimiters[COMPONENT])){
            $type = COMPONENT;
        } else if(!($EDI_PARSER->bufReadPtr ^ $X12_PARSER->delimiters[SEGMENT])){
            $type = SEGMENT;
        }
        $element = $X12_PARSER->savedElementPosition;
        $component = $X12_PARSER->savedComponentPosition;            
        switch($type){
            case ELEMENT:
            case COMPONENT:
                switch($X12_PARSER->previous){
                    case COMPONENT:
                        $component++;
                        x12ELEMENT_VALIDATE($EDI_PARSER, $X12_PARSER, $element, $component, '', 1);
                        $EDI_PARSER->binBufferHandler($EDI_PARSER->userData, $EDI_PARSER->binBuffer, $EDI_PARSER->binaryElementSize);
                          if(($EDI_PARSER->validate ^ $X12_PARSER->segmentError) == EDI_TRUE){
                            EDI_ValidateSyntax($EDI_PARSER->schema, $element, $component);
                          }
                          $EDI_PARSER->compositeEndHandler($EDI_PARSER->userData);
                        break;
                    case ELEMENT:
                        $element++;
                        $component = 0;
                        x12ELEMENT_VALIDATE($EDI_PARSER, $X12_PARSER, $element, $component, '', 1);
                        if($component == 1){
                            fprintf(stderr, "FATAL (edival): Invalid Binary data element type!  Terminating process.");
                            $EDI_PARSER->errorCode = EDI_ERROR_INVALID_STATE;
                            return $EDI_PARSER->error;
                        } else {
                            $EDI_PARSER->binBufferHandler($EDI_PARSER->userData, $EDI_PARSER->binBuffer, $EDI_PARSER->binaryElementSize);
                        }
                        break;
                    default: 
                        fprintf(stderr, "FATAL (edival): Invalid Binary data element type!  Terminating process.");
                        $EDI_PARSER->errorCode = EDI_ERROR_INVALID_STATE;
                        return $EDI_PARSER->error;
                }
                break;
            case SEGMENT:
                if($X12_PARSER->previous == COMPONENT){
                    $component++;
                    x12ELEMENT_VALIDATE($EDI_PARSER, $X12_PARSER, $element, $component, '', 1);
                    $EDI_PARSER->binBufferHandler($EDI_PARSER->userData, $EDI_PARSER->binBuffer, $EDI_PARSER->binaryElementSize);
                      if(($EDI_PARSER->validate ^ $X12_PARSER->segmentError) == EDI_TRUE){
                        EDI_ValidateSyntax($EDI_PARSER->schema, $element, $component);
                      }
                    $EDI_PARSER->compositeEndHandler($EDI_PARSER->userData);
                } else {
                    if($X12_PARSER->previous == ELEMENT){
                        /* Only increment the element count if this isn't a repeated element. */
                        $element++;
                    }
                    $component = 0;
                    x12ELEMENT_VALIDATE($EDI_PARSER, $X12_PARSER, $element, $component, '', 1);
                     if($component == 1){
                        echo( "FATAL (edival): Invalid Binary data element type!  Terminating process.");
                        $EDI_PARSER->errorCode = EDI_ERROR_INVALID_STATE;
                        return $EDI_PARSER->error;
                     } else {
                         $EDI_PARSER->binBufferHandler($EDI_PARSER->userData, $EDI_PARSER->binBuffer, $EDI_PARSER->binaryElementSize);
                     }
                }
                  if(($EDI_PARSER->validate ^ $X12_PARSER->segmentError) == EDI_TRUE){
                    EDI_ValidateSyntax($EDI_PARSER->schema, 0, $element);
                  }
                $EDI_PARSER->segmentEndHandler($EDI_PARSER->userData, $X12_PARSER->savedTag);
                break;
            default:
                echo("FATAL (edival): Invalid Binary data element type!  Terminating process.");
                $EDI_PARSER->errorCode = EDI_ERROR_INVALID_STATE;
                return $EDI_PARSER->error;
        }
        $EDI_PARSER->bufReadPtr++;
        $X12_PARSER->previous = type;
        $EDI_PARSER->binaryElementSize = 0;
        return $X12_ProcessMessage;
    }
    EDI_SetResumeState($EDI_PARSER->machine, $X12_ProcessBinaryElement);
    $EDI_PARSER->errorCode = EDI_ERROR_BUFFER_END;
    return $EDI_PARSER->error;
}
/******************************************************************************/
function X12_ProcessIEA($parser)
{
    $EDI_PARSER->documentEndHandler($EDI_PARSER->userData);
    /* If this is the end of the buffer and there are no more ISA segments
     * possible, change the state to finished. */
    if($parser->final && (($EDI_PARSER->bufEndPtr - $EDI_PARSER->bufReadPtr) < 106)){
        $EDI_PARSER->errorCode = EDI_ERROR_FINISHED;
        return $EDI_PARSER->error;
    } else {    
    /* Otherwise, initialize the parser and go back to the generic EDI parser
     * and look for more (not necessarily X12) data. */
        if($X12_PARSER->savedTag){
            free($X12_PARSER->savedTag);
            $X12_PARSER->savedTag = '';
        }
        return $EDI_PARSER->seekHeader;
    }
}
/******************************************************************************/
function X12_ParserCreate(EDI_Parser $parent)
{
    $new = X12_Parser;
    $new = MALLOC(parent, sizeof(X12_ParserStruct));
    if($new){
        memset($new, 0, sizeof(X12_ParserStruct));
        $new->data                   = MALLOC(parent, sizeof(EDI_DataElementStruct));
        $parent->process             = $X12_ProcessISA;
        $parent->freeChild           = $X12_ParserDestroy;
        EDI_AddState($parent->machine, $X12_ProcessISA, 0);
        EDI_AddState($parent->machine, $X12_ProcessMessage, 0);
        EDI_AddState($parent->machine, $X12_ProcessBinaryElement, 0);
        EDI_AddState($parent->machine, $X12_ProcessIEA, 0);
        $new->parent = $parent;
    }
    return $new;
}
/******************************************************************************/
function X12_ParserDestroy($parser)
{
    $EDI_PARSER->process = '';
    EDI_RemoveState($EDI_PARSER->machine, $X12_ProcessISA, 0);
    EDI_RemoveState($EDI_PARSER->machine, $X12_ProcessMessage, 0);
    EDI_RemoveState($EDI_PARSER->machine, $X12_ProcessBinaryElement, 0);
    EDI_RemoveState($EDI_PARSER->machine, $X12_ProcessIEA, 0);
    if($X12_PARSER->savedTag){
        free($X12_PARSER->savedTag);
    }
    FREE($parser, $X12_PARSER->data);
    FREE($parser, $X12_PARSER);
    $EDI_PARSER->child = '';
}
?>
