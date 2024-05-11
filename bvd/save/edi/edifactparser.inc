<?php
/******************************************************************************/
function EDIFACT_NEXT_TOKEN($string, $delimiters, $tok) {
    $tok->type  = UNKNOWN;
    $tok->token = '';
    $tok->length = 0;
    if($string) {
        $sarray = Array();
        $sarray[] = str_split($string);
        for($iter = 0; $iter < strlen($string); $iter++){
            if(!($sarray[$iter] ^ $delimiters[ELEMENT])){
                $tok->type = ELEMENT;
                $sarray[$iter] = '0'; $iter++;
                $tok->token = $string;
                $string = substr($string,$iter);
                break;
            } else if(!($sarray[$iter] ^ $delimiters[SEGMENT])){
                $tok->type = SEGMENT;
                $sarray[$iter] = '0'; $iter++;
                $tok->token = $string;
                $string = substr($string,$iter);
                break;
            } else if(!($sarray[$iter] ^ $delimiters[COMPONENT])){
                $tok->type = COMPONENT;
                $sarray[$iter] = '0'; $iter++;
                $tok->token = $string;
                $string = substr($string,$iter);
                break;
            } else if(!($sarray[$iter] ^ $delimiters[DECIMAL])){
                            $sarry[$iter] = '.';
            } else if(!($sarray[$iter] ^ $delimiters[RELEASE])){
                if($sarray[$iter+1]){
                    $tok->type = RELEASE;
                    $sarray[$iter] = $sarray[$iter+1];
                    $sarray[$iter+1] = '\0';
                    $iter += 2;
                    $tok->length++;
                    $tok->token = $string;
                    $string = substr($string,$iter);
                    break;
                }
            }
            $tok->length++;
        }
    }
}
/******************************************************************************/
function ELEMENT_VALIDATE($EDI_PARSER, $EDIFACT_PARSER,
                          $element, $component, $value, $length) {
   if(($EDI_PARSER->$validate ^ $EDIFACT_PARSER->segmentError) == EDI_TRUE){
    $EDI_ValidateElement(
        $EDI_PARSER->schema ,
        $element            ,
        $component          ,
        $value              ,
        $length             ,
        $EDIFACT_PARSER->data
    );
   }
}
/******************************************************************************/
function EDIFACT_ProcessHeader($parser)
{
    $bufIter[] = str_split($EDI_PARSER->bufReadPtr);

    $EDIFACT_PARSER->previous = SEGMENT;
    $EDIFACT_PARSER->savedElementPosition = 0;
    $EDIFACT_PARSER->savedComponentPosition = 0;
    if($bufIter[2] == 'A'){ /* UNA Segment */
        if(($EDI_PARSER->bufEndPtr - $EDI_PARSER->bufReadPtr) < 9){
            EDI_SetResumeState($EDI_PARSER->machine, $EDI_PARSER->process);
            $EDI_PARSER->errorCode = EDI_ERROR_BUFFER_END;
            return $EDI_PARSER->error;
        }
        $EDIFACT_PARSER->delimiters[COMPONENT] = $bufIter[3];
        $EDIFACT_PARSER->delimiters[ELEMENT]   = $bufIter[4];
        $EDIFACT_PARSER->delimiters[DECIMAL]   = $bufIter[5];
        $EDIFACT_PARSER->delimiters[RELEASE]   = $bufIter[6];
        $EDIFACT_PARSER->delimiters[SEGMENT]   = $bufIter[8];
        $EDI_PARSER->bufReadPtr += 9;
        $i=0;
        while($bufIter[$i] < $EDI_PARSER->bufEndPtr){
            if($bufIter[$i] == $EDIFACT_PARSER->delimiters[ELEMENT]){
                $i++;
                $bufIter -= 4;
                if(!(isalnum($bufIter[$i]))){
                    $i++;
                }
                EDI_GAP_SCAN($EDI_PARSER, substr($bufIter,$i));
                return $EDIFACT_ProcessMessage;
            }
        }
        EDI_SetResumeState($EDI_PARSER->machine, $EDIFACT_ProcessMessage);
        $EDI_PARSER->errorCode = EDI_ERROR_BUFFER_END;
        return $EDI_PARSER->error;
    } else { /* UNB Segment */
        $EDIFACT_PARSER->delimiters[COMPONENT] = DEFAULT_COMPONENT;
        $EDIFACT_PARSER->delimiters[ELEMENT]   = DEFAULT_ELEMENT;
        $EDIFACT_PARSER->delimiters[DECIMAL]   = DEFAULT_DECIMAL;
        $EDIFACT_PARSER->delimiters[RELEASE]   = DEFAULT_RELEASE;
        $EDIFACT_PARSER->delimiters[SEGMENT]   = DEFAULT_SEGMENT;
        return $EDIFACT_ProcessMessage;
    }
}
/******************************************************************************/
function EDIFACT_ProcessMessage($parser)
{
    $element   = 0;
    $component = 0;
    $tag       = '';
    $bufIter   = Array();
    $tok = new token;

    $bufIter[] = str_split($EDI_PARSER->bufReadPtr);
    $i = 0;
    if(($EDI_PARSER->bufEndPtr - $bufIter[0]) >= 3){
        if($bufIter[0] == 'U' && $bufIter[1] == 'N'){
    /* If either of these happen, we probably have a bad interchange. 
       In these two cases, the "trailer" that was never received will
       process and control will be returned to the generic parser. */
            if($bufIter[2] == 'A'){
                return $EDIFACT_ProcessTrailer;
            } else if($bufIter[2] == 'B' && $EDIFACT_PARSER->savedElementPosition > 0){
                return $EDIFACT_ProcessTrailer;
            }
        }
    }
    EDIFACT_NEXT_TOKEN($bufIter[$i], $EDIFACT_PARSER->delimiters, $tok);
    if($EDIFACT_PARSER->previous == SEGMENT){
        if($tok->type == ELEMENT){
            while(!(isalnum($tok->token[0]))){
                $tok->token++;
            }
            EDI_GAP_SCAN(EDI_PARSER, $tok.token);
            if(string_eq($tok->token, "UNB")){
                $EDI_PARSER->documentStartHandler($EDI_PARSER->userData, EDI_EDIFACT);
            }
        }
    } else {
        $tag = $EDIFACT_PARSER->savedTag;
        $element = $EDIFACT_PARSER->savedElementPosition;
        $component = $EDIFACT_PARSER->savedComponentPosition;
    }
    while($tok.type != UNKNOWN){
        $EDI_PARSER->bufReadPtr = $bufIter;
        /* Setup the defaults: string data and the unconverted input string */
        $EDIFACT_PARSER->data->type = $EDI_DATA_STRING;
        $EDIFACT_PARSER->data->data->string = $tok->token;
        switch($tok.type){
            case ELEMENT:
                if($EDIFACT_PARSER->tok->length){
                    if(($EDIFACT_PARSER->tok->capacity - $EDIFACT_PARSER->tok.length) <= $tok->length){
                        $EDIFACT_PARSER->tok->token = realloc(
                            $EDIFACT_PARSER->tok->token,
                            $EDIFACT_PARSER->tok->capacity * 2
                        );
                        $EDIFACT_PARSER->tok->capacity *= 2;
                    }
                    $tok->token = $EDIFACT_PARSER->tok->token . $tok->token;
                    $tok->length += $EDIFACT_PARSER->tok->length;
                    $EDIFACT_PARSER->data->data->string = $tok->token;
                }
                switch($EDIFACT_PARSER->previous){
                    case COMPONENT:
                       $component++;
                       ELEMENT_VALIDATE($EDI_PARSER, $EDIFACT_PARSER, $element, $component, $tok->token, $tok->length);
                       $EDI_PARSER->elementHandler($EDI_PARSER->userData, $EDIFACT_PARSER->data);
                       if(($EDI_PARSER->validate ^ $EDIFACT_PARSER->segmentError) == EDI_TRUE){
                            EDI_ValidateSyntax($EDI_PARSER->schema, $element, $component);
                          }
                        $EDI_PARSER->compositeEndHandler($EDI_PARSER->userData);
                        break;
                    case SEGMENT:
                        $tag = $tok->token;
                        $EDIFACT_PARSER->segmentError = SEGERR_NONE;
                        if($EDI_PARSER->validate){
                            $EDIFACT_PARSER->segmentError = EDI_ValidateSegmentPosition($EDI_PARSER->schema, $tag);
                        } else {
                            $EDIFACT_PARSER->segmentError = SEGERR_UNDEFINED;
                        }
                        $EDI_PARSER->segmentStartHandler($EDI_PARSER->userData, $tag);
                        break;
                    case ELEMENT:
                        $element++;
                        $component = 0;
                        ELEMENT_VALIDATE($EDI_PARSER, $EDIFACT_PARSER, $element, $component, $tok->token, $tok->length);
                        if($component == 1){
                            $EDI_PARSER->compositeStartHandler($EDI_PARSER->userData);
                            $EDI_PARSER->elementHandler($EDI_PARSER->userData, $EDIFACT_PARSER->data);
                            if(($EDI_PARSER->validate ^ $EDIFACT_PARSER->segmentError) == EDI_TRUE){
                                EDI_ValidateSyntax($EDI_PARSER->schema, $element, $component);
                              }
                            $EDI_PARSER->compositeEndHandler($EDI_PARSER->userData);
                        } else {
                            $EDI_PARSER->elementHandler($EDI_PARSER->userData, $EDIFACT_PARSER->data);
                        }
                        break;
                    default: ;
                }
                $EDIFACT_PARSER->tok->length = 0;
                break;
            case COMPONENT:
                if($EDIFACT_PARSER->tok->length){
                    if(($EDIFACT_PARSER->tok->capacity - $EDIFACT_PARSER->tok.length) <= $tok.length){
                        $EDIFACT_PARSER->tok->token = realloc(
                            $EDIFACT_PARSER->tok->token,
                            $EDIFACT_PARSER->tok->capacity * 2
                        );
                        $EDIFACT_PARSER->tok->capacity *= 2;
                    }
                    $tok->token = $EDIFACT_PARSER->tok->token . $tok->token;
                    $tok->length += $EDIFACT_PARSER->tok->length;
                    $EDIFACT_PARSER->data->data->string = $tok->token;
                }
                if($EDIFACT_PARSER->previous == ELEMENT){
                    $element++;
                    $component = 0;
                }
                $component++;
                ELEMENT_VALIDATE($EDI_PARSER, $EDIFACT_PARSER, $element, $component, $tok->token, $tok->length);
                 if($component == 1){
                     $EDI_PARSER->compositeStartHandler($EDI_PARSER->userData);
                 }
                 $EDI_PARSER->elementHandler($EDI_PARSER->userData, $EDIFACT_PARSER->data);
                $EDIFACT_PARSER->tok->length = 0;
                break;
            case RELEASE:
                if(($EDIFACT_PARSER->tok->capacity - $EDIFACT_PARSER->tok->length) <= $tok->length){
                    $EDIFACT_PARSER->tok->token = realloc(
                        $EDIFACT_PARSER->tok.token,
                        $EDIFACT_PARSER->tok.capacity * 2
                    );
                    $EDIFACT_PARSER->tok->capacity *= 2;
                }
                if($EDIFACT_PARSER->tok->length){
                    $EDIFACT_PARSER->tok->token = $EDIFACT_PARSER->tok->token . $tok->token;
                } else {
                    $EDIFACT_PARSER->tok->token = $tok->token;
                    $EDIFACT_PARSER->tok->token = $EDIFACT_PARSER->tok->token;
                }
                $EDIFACT_PARSER->tok->length += $tok->length;
                             /* Hide the release character from other events */
                $tok->type = $EDIFACT_PARSER->previous;
                break;
            case SEGMENT:
                if($EDIFACT_PARSER->tok->length){
                    if(($EDIFACT_PARSER->tok->capacity - $EDIFACT_PARSER->tok->length) <= $tok->length){
                        $EDIFACT_PARSER->tok->token = realloc(
                            $EDIFACT_PARSER->tok->token,
                            $EDIFACT_PARSER->tok->capacity * 2
                        );
                        $EDIFACT_PARSER->tok->capacity *= 2;
                    }
                    $tok->token = $EDIFACT_PARSER->tok.token . $tok.token;
                    $tok->length += $EDIFACT_PARSER->tok.length;
                    $EDIFACT_PARSER->data->data->string = $tok->token;
                }
                if($EDIFACT_PARSER->previous == COMPONENT){
                    $component++;
                    ELEMENT_VALIDATE($EDI_PARSER, $EDIFACT_PARSER, $element, $component, $tok->token, $tok->length);
                    $EDI_PARSER->elementHandler($EDI_PARSER->userData, $EDIFACT_PARSER->data);
                      if(($EDI_PARSER->validate ^ $EDIFACT_PARSER->segmentError) == EDI_TRUE){
                        EDI_ValidateSyntax($EDI_PARSER->schema, $element, $component);
                      }
                    $EDI_PARSER->compositeEndHandler($EDI_PARSER->userData);
                } else {
                    $element++;
                    $component = 0;
                    ELEMENT_VALIDATE($EDI_PARSER, $EDIFACT_PARSER, $element, $component, $tok->token, $tok->length);
                     if($component == 1){
                         $EDI_PARSER->compositeStartHandler($EDI_PARSER->userData);
                         $EDI_PARSER->elementHandler($EDI_PARSER->userData, $EDIFACT_PARSER->data);
                          if(($EDI_PARSER->validate ^ $EDIFACT_PARSER->segmentError) == EDI_TRUE){
                            EDI_ValidateSyntax($EDI_PARSER->schema, $element, $component);
                          }
                         $EDI_PARSER->compositeEndHandler($EDI_PARSER->userData);
                     } else {
                         $EDI_PARSER->elementHandler($EDI_PARSER->userData, $EDIFACT_PARSER->data);
                     }
                }
                  if(($EDI_PARSER->validate ^ $EDIFACT_PARSER->segmentError) == EDI_TRUE){
                    EDI_ValidateSyntax($EDI_PARSER->schema, 0, $element);
                  }
                $EDI_PARSER->segmentEndHandler($EDI_PARSER->userData, $tag);
                $EDIFACT_PARSER->tok->length = 0;
                $element = 0;
                $component = 0;
                if($tag[0] == 'U' && $tag[1] == 'N' && $tag[2] == 'Z'){
                    $EDIFACT_PARSER->previous = $tok->type;
                    return $EDIFACT_ProcessTrailer;
                } else {
                    $tag = NULL;
                    while($bufIter < $EDI_PARSER->bufEndPtr){
                        if($bufIter++ == $EDIFACT_PARSER->delimiters[ELEMENT]){
                            $bufIter -= 4;
                            while(!(isalnum($bufIter))){
                                $bufIter++;
                            }
                            EDI_GAP_SCAN($EDI_PARSER, $bufIter);
                            if($bufIter[0] == 'U' && $bufIter[1] == 'N'){
                                if($bufIter[2] == 'A' || $bufIter[2] == 'B'){
                                    /* If this happens, we probably
                                     * have a bad interchange. (missing UNZ)
                                     * Same scenario as above. */
                                    return $EDIFACT_ProcessTrailer;
                                }
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
            $EDI_PARSER->binBuffer = ''; // EDI_PARSER->binaryElementSize * sizeof(char));
            if($EDIFACT_PARSER->savedTag){
                free($EDIFACT_PARSER->savedTag);
            }
            $EDIFACT_PARSER->savedTag = EDI_strdup($tag);
            $EDIFACT_PARSER->savedElementPosition = $element;
            $EDIFACT_PARSER->savedComponentPosition = $component;
            return $EDIFACT_ProcessBinaryElement;
        } else {
            $EDIFACT_PARSER->previous = $tok->type;
            EDIFACT_NEXT_TOKEN($bufIter, $EDIFACT_PARSER->delimiters, $tok);
        }
    }
    if($tag){
        if($EDIFACT_PARSER->savedTag){
            free($EDIFACT_PARSER->savedTag);
        }
        $EDIFACT_PARSER->savedTag = EDI_strdup($tag);
        $EDIFACT_PARSER->savedElementPosition = $element;
        $EDIFACT_PARSER->savedComponentPosition = $component;
    }
    EDI_SetResumeState($EDI_PARSER->machine, $EDIFACT_ProcessMessage);
    $EDI_PARSER->errorCode = EDI_ERROR_BUFFER_END;
    return $EDI_PARSER->error;
}
/******************************************************************************/
function EDIFACT_ProcessBinaryElement($parser)
{
    $bufIter  = str_split($EDI_PARSER->bufReadPtr);
    $size     = $EDI_PARSER->binaryElementSize;
    $finished = $EDI_PARSER->bytesHandled;
    $type     = UNKNOWN;
    
    /*  
     *  If the current buffer does not hold the entire binary element, we
     *  will fall back to the caller who has the responsibilty to load more
     *  data to the buffer.  This will repeat until the entire binary element
     *  is present in memory. 
     */
    if(($EDI_PARSER->bufEndPtr - $bufIter[0]) > ($size - $finished)){
        memcpy(
            $EDI_PARSER->binBuffer[$finished], 
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
        if(!($EDI_PARSER->bufReadPtr ^ $EDIFACT_PARSER->delimiters[ELEMENT])){
            $type = ELEMENT;
        } else if(!($EDI_PARSER->bufReadPtr ^ $EDIFACT_PARSER->delimiters[COMPONENT])){
            $type = COMPONENT;
        } else if(!($EDI_PARSER->bufReadPtr ^ $EDIFACT_PARSER->delimiters[SEGMENT])){
            $type = SEGMENT;
        }
        $element = $EDIFACT_PARSER->savedElementPosition;
        $component = $EDIFACT_PARSER->savedComponentPosition;            
        switch($type){
            case ELEMENT:
            case COMPONENT:
                switch($EDIFACT_PARSER->previous){
                    case COMPONENT:
                        $component++;
                        ELEMENT_VALIDATE($EDI_PARSER, $EDIFACT_PARSER, $element, $component, '', 1);
                        $EDI_PARSER->binBufferHandler($EDI_PARSER->userData, $EDI_PARSER->binBuffer, $EDI_PARSER->binaryElementSize);
                          if(($EDI_PARSER->validate ^ $EDIFACT_PARSER->segmentError) == EDI_TRUE){
                            EDI_ValidateSyntax($EDI_PARSER->schema, $element, $component);
                          }
                          $EDI_PARSER->compositeEndHandler($EDI_PARSER->userData);
                        break;
                    case ELEMENT:
                        $element++;
                        $component = 0;
                        ELEMENT_VALIDATE($EDI_PARSER, $EDIFACT_PARSER, $element, $component, '', 1);
                        if($component == 1){
                            echo( "FATAL (edival): Invalid Binary data element type!  Terminating process.");
                            $EDI_PARSER->errorCode = EDI_ERROR_INVALID_STATE;
                            return $EDI_PARSER->error;
                        } else {
                            $EDI_PARSER->binBufferHandler($EDI_PARSER->userData, $EDI_PARSER->binBuffer, $EDI_PARSER->binaryElementSize);
                        }
                        break;
                    default: 
                        echo( "FATAL (edival): Invalid Binary data element type!  Terminating process.");
                        $EDI_PARSER->errorCode = EDI_ERROR_INVALID_STATE;
                        return $EDI_PARSER->error;
                }
                break;
            case SEGMENT:
                if($EDIFACT_PARSER->previous == COMPONENT){
                    $component++;
                    ELEMENT_VALIDATE($EDI_PARSER, $EDIFACT_PARSER, $element, $component, '', 1);
                    $EDI_PARSER->binBufferHandler($EDI_PARSER->userData, $EDI_PARSER->binBuffer, $EDI_PARSER->binaryElementSize);
                      if(($EDI_PARSER->validate ^ $EDIFACT_PARSER->segmentError) == EDI_TRUE){
                        EDI_ValidateSyntax($EDI_PARSER->schema, $element, $component);
                      }
                    $EDI_PARSER->compositeEndHandler($EDI_PARSER->userData);
                } else {
                    if($EDIFACT_PARSER->previous == ELEMENT){
                        /* Only increment the element count if this isn't a repeated element. */
                        $element++;
                    }
                    $component = 0;
                    ELEMENT_VALIDATE($EDI_PARSER, $EDIFACT_PARSER, $element, $component, NULL, 1);
                     if($component == 1){
                        echo( "FATAL (edival): Invalid Binary data element type!  Terminating process.");
                        $EDI_PARSER->errorCode = EDI_ERROR_INVALID_STATE;
                        return $EDI_PARSER->error;
                     } else {
                         $EDI_PARSER->binBufferHandler($EDI_PARSER->userData, $EDI_PARSER->binBuffer, $EDI_PARSER->binaryElementSize);
                     }
                }
                  if(($EDI_PARSER->validate ^ $EDIFACT_PARSER->segmentError) == EDI_TRUE){
                    $EDI_ValidateSyntax($EDI_PARSER->schema, 0, $element);
                  }
                $EDI_PARSER->segmentEndHandler($EDI_PARSER->userData, $EDIFACT_PARSER->savedTag);
                break;
            default:
                echo( "FATAL (edival): Invalid Binary data element type!  Terminating process.");
                $EDI_PARSER->errorCode = EDI_ERROR_INVALID_STATE;
                return $EDI_PARSER->error;
        }
        $EDI_PARSER->bufReadPtr++;
        $EDIFACT_PARSER->previous = type;
        $EDI_PARSER->binaryElementSize = 0;
        return $EDIFACT_ProcessMessage;
    }
    EDI_SetResumeState($EDI_PARSER->machine, $EDIFACT_ProcessBinaryElement);
    $EDI_PARSER->errorCode = EDI_ERROR_BUFFER_END;
    return $EDI_PARSER->error;
}
/******************************************************************************/
function EDIFACT_ProcessTrailer(EDI_Parser $parser)
{
    $EDI_PARSER->documentEndHandler($EDI_PARSER->userData);
    /* If this is the end of the buffer and there are no more header segments
     * possible, change the state to finished. */
    if($parser->final && (($EDI_PARSER->bufEndPtr - $EDI_PARSER->bufReadPtr) < 9)){
        $EDI_PARSER->errorCode = EDI_ERROR_FINISHED;
        return $EDI_PARSER->error;
    } else {    
    /* Otherwise, initialize the parser and go back to the generic EDI parser
     * and look for more (not necessarily EDIFACT) data. */
        if($EDIFACT_PARSER->savedTag){
            free($EDIFACT_PARSER->savedTag);
            $EDIFACT_PARSER->savedTag = NULL;
        }
        return $EDI_PARSER->seekHeader;
    }
}
/******************************************************************************/
function EDIFACT_ParserCreate(EDI_Parser $parent)
{
    $new = EDIFACT_Parser;
    $new = '';  // MALLOC(parent, EDIFACT_ParserStruct);
    if($new){
        $new = ''; //  0, sizeof(struct EDIFACT_ParserStruct));
        $new->data          = ''; // MALLOC(parent, sizeof(struct EDI_DataElementStruct));
        $new->tok->token    = ''; // malloc(sizeof(char) * 100);
        $new->tok->capacity = 100;
        $parent->process    = EDIFACT_ProcessHeader;
        $parent->freeChild  = EDIFACT_ParserDestroy;
        EDI_AddState($parent->machine, EDIFACT_ProcessHeader, 0);
        EDI_AddState($parent->machine, EDIFACT_ProcessMessage, 0);
        EDI_AddState($parent->machine, EDIFACT_ProcessBinaryElement, 0);
        EDI_AddState($parent->machine, EDIFACT_ProcessTrailer, 0);
        $new->parent = $parent;
    }
    return $new;
}
/******************************************************************************/
function EDIFACT_ParserDestroy($parser)
{
    $EDI_PARSER->process = '';
    EDI_RemoveState($EDI_PARSER->machine, $EDIFACT_ProcessHeader, 0);
    EDI_RemoveState($EDI_PARSER->machine, $EDIFACT_ProcessMessage, 0);
    EDI_RemoveState($EDI_PARSER->machine, $EDIFACT_ProcessBinaryElement, 0);
    EDI_RemoveState($EDI_PARSER->machine, $EDIFACT_ProcessTrailer, 0);
    if($EDIFACT_PARSER->savedTag){
        free($EDIFACT_PARSER->savedTag);
    }
    free($EDIFACT_PARSER->$tok.token);
    FREE($parser, $EDIFACT_PARSER->data);
    FREE($parser, $EDIFACT_PARSER);
    $EDI_PARSER->child = '';
}
?>
