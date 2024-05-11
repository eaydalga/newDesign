<?php
/******************************************************************************/
function seekHeader($parser)
{
    $i       = 0;
    $tag[4]  = Array("   ");
    $previous = '';
    $bufIter = '';
    $found = EDI_FALSE;

    $bufIter = $parser->bufReadPtr;
    if($previous == $parser->bufReadPtr){
        $parser->bufReadPtr -= 3;
    }
    while($bufIter < $parser->bufEndPtr){
        for($i = 0; $i < 3; $i++){
            $tag[$i] = $bufIter++;
        }
        if(string_eq($tag, "ISA")){
            if($parser->docType != EDI_ANSI_X12){
                if($parser->docType != EDI_UNKNOWN_DOC){
                    $parser->freeChild($parser);
                }
                $parser->child = X12_ParserCreate($parser);
                $parser->docType = EDI_ANSI_X12;
            }
            $found = EDI_TRUE;
        } else if(string_eq($tag, "UNA") || string_eq($tag, "UNB")){
            if($parser->docType != EDI_EDIFACT){
                if($parser->docType != EDI_UNKNOWN_DOC){
                    $parser->freeChild($parser);
                }
                $parser->child = EDIFACT_ParserCreate($parser);
                $parser->docType = EDI_EDIFACT;
            }
            $found = EDI_TRUE;
        }
        if($found){
            $previous = $bufIter;
            if(($bufIter - $parser->bufReadPtr) > 3){
                $prefix = $bufIter - $parser->bufReadPtr - 3;
                if($prefix > 1 || !isspace($parser->bufReadPtr)){
                    $invalid = EDI_strndup($parser->bufReadPtr, $prefix, $parser->memsuite);
                    if(!$invalid){
                        $parser->errorCode = EDI_ERROR_NO_MEM;
                        return $parser->error;
                    }
                    $parser->nonEDIDataHandler($parser->userData, $invalid);
                    FREE($parser, $invalid);
                }
            }
            $parser->bufReadPtr = $bufIter - 3;
            return $parser->process;
        } else {
            $bufIter -= 2;
        }
    }
    if($parser->bufReadPtr < $parser->bufEndPtr && $bufIter == $parser->bufEndPtr){
        $prefix = $bufIter - $parser->bufReadPtr;
        $invalid = EDI_strndup($parser->bufReadPtr, $prefix, $parser->memsuite);
        if(!$invalid){
            $parser->errorCode = EDI_ERROR_NO_MEM;
            return $parser->error;
        }
        $parser->nonEDIDataHandler($parser->userData, $invalid);
        FREE($parser, $invalid);
        $parser->$bufReadPtr = $bufIter;
    }
    $parser->errorCode = EDI_ERROR_BUFFER_END;
    return $parser->error;
}
/******************************************************************************/
function handleError($parser)
{
    //fprintf(stderr, "Error! %d\n", EDI_GetErrorCode(parser));
    return '';
}
/******************************************************************************/
function EDI_ParserCreate()
{
    return parserCreate('');
}
/******************************************************************************/
function EDI_ParserCreate_MM($memsuite)
{
    return parserCreate($memsuite);
}   
/******************************************************************************/
function parserCreate($memsuite)
{
    $parser = EDI_Parser;
    
    if($memsuite){
        $mtemp = EDI_Memory_Handling_Suite;
        $parser = 
            $memsuite->malloc_fcn(sizeof(EDI_Parser));
        if ($parser != NULL) {
            $parser->memsuite = 
                $memsuite->malloc_fcn(sizeof(EDI_Memory_Handling_Suite));
            $mtemp = $parser->memsuite;
            $mtemp->malloc_fcn = $memsuite->malloc_fcn;
            $mtemp->realloc_fcn = $memsuite->realloc_fcn;
            $mtemp->free_fcn = $memsuite->free_fcn;
        }
    } else {
        $mtemp = EDI_Memory_Handling_Suite;
        $parser = '';  //malloc(sizeof(struct EDI_Parser));
        if ($parser != NULL) {
            $parser->memsuite = '';
                // malloc(sizeof(EDI_Memory_Handling_Suite));
            $mtemp = $parser->memsuite;
            $mtemp->malloc_fcn = $malloc;
            $mtemp->realloc_fcn = $realloc;
            $mtemp->free_fcn = $free;
        }
    }
    if(!$parser){
       return NULL;
    }
    $parser->dataBuffer = MALLOC(parser, INIT_DATA_BUF_SIZE * sizeof(char));
    if(!$parser->dataBuffer){
       FREE($parser, $parser);
       return NULL;
    }
    $parser->dataBufEnd = $parser->dataBuffer + INIT_DATA_BUF_SIZE;
    parserInit($parser);
    return $parser;
}
/******************************************************************************/
function parserInit($parser)
{
    $parser->userData              = '';
    $parser->bufReadPtr            = $parser->dataBuffer;
    $parser->bufEndPtr             = $parser->dataBuffer;
    $parser->state                 = EDI_INITIALIZED;
    $parser->errorCode             = EDI_ERROR_NONE;
    $parser->segmentStartHandler   = '';
    $parser->segmentEndHandler     = '';
    $parser->compositeStartHandler = '';
    $parser->compositeEndHandler   = '';
    $parser->elementHandler        = '';
    $parser->binBufferHandler      = '';
    $parser->binaryElementSize     = 0;
    $parser->bytesHandled          = 0;
    $parser->binBuffer             = '';
    $parser->nonEDIDataHandler     = '';
    $parser->seekHeader            = $seekHeader;
    $parser->error                 = $handleError;
    $parser->process               = '';
    $parser->machine               = '';
    $parser->child                 = '';
    $parser->freeChild             = '';
    $parser->docType               = EDI_UNKNOWN_DOC;
    $parser->schema                = '';
    $parser->validate              = EDI_FALSE;
    $parser->final                 = EDI_FALSE;
    return;
}
/******************************************************************************/
function EDI_SetUserData($parser, $p)
{
    $parser->userData = $p;
}
/******************************************************************************/
function EDI_SetDocumentStartHandler($parser, $h)
{
    $parser->documentStartHandler = $h;
}
/******************************************************************************/
function EDI_SetDocumentEndHandler($parser, $h)
{
    $parser->documentEndHandler = $h;
}
/******************************************************************************/
function EDI_SetSegmentStartHandler($parser, $h)
{
    $parser->segmentStartHandler = $h;
}
/******************************************************************************/
function EDI_SetSegmentEndHandler($parser, $h)
{
    $parser->segmentEndHandler = $h;
}
/******************************************************************************/
function EDI_SetCompositeStartHandler($parser, $h)
{
    $parser->compositeStartHandler = $h;
}
/******************************************************************************/
function EDI_SetCompositeEndHandler($parser, $h)
{
    $parser->compositeEndHandler = $h;
}
/******************************************************************************/
function EDI_SetElementHandler($parser, $h)
{
    $parser->elementHandler = $h;
}
/******************************************************************************/
function EDI_SetBinaryElementHandler($parser, $bbh   )
{
    $parser->binBufferHandler    = $bbh;
}
/******************************************************************************/
function EDI_SetRepeatHandler($parser, $h)
{
    $parser->repeatHandler = $h;
}
/******************************************************************************/
function EDI_SetNonEDIDataHandler($parser, $h)
{
    $parser->nonEDIDataHandler = $h;
}
/******************************************************************************/
function EDI_ParserReset($parser)
{
    FREE($parser, $parser->dataBuffer);
    FREE($parser, $parser->machine);
    if($parser->child){
        $parser->freeChild($parser);
    }
    $parser->dataBuffer = ''; // MALLOC($parser, INIT_DATA_BUF_SIZE * sizeof(char *));
    if(!$parser->dataBuffer){
       FREE($parser, $parser);
       return EDI_FALSE;
    }
    $parser->dataBufEnd = $parser->dataBuffer + INIT_DATA_BUF_SIZE;
    parserInit($parser);
    return EDI_TRUE;
}
/******************************************************************************/
function EDI_GetBuffer($parser, $len)
{

    switch ($parser->state) {
        case EDI_SUSPENDED:
            $parser->errorCode = EDI_ERROR_SUSPENDED;
            return NULL;
        case EDI_FINISHED:
            $parser->errorCode = EDI_ERROR_FINISHED;
            return NULL;
        default: ;
    }
    if ($len > $parser->dataBufEnd - $parser->bufEndPtr) {
        $neededSize = $len + ($parser->bufEndPtr - $parser->bufReadPtr);
        if ($neededSize <= $parser->dataBufEnd - $parser->dataBuffer){
            memmove($parser->dataBuffer, 
                    $parser->bufReadPtr, 
                    $parser->bufEndPtr - $parser->bufReadPtr);
            $parser->bufEndPtr = $parser->dataBuffer +
                                ($parser->bufEndPtr - $parser->bufReadPtr);
            $parser->bufReadPtr = $parser->dataBuffer;
        } else {
            $size = $parser->dataBufEnd - $parser->bufReadPtr;
            $size += $neededSize;
            $newBuf = MALLOC($parser, $size);
            if(!$newBuf){
                $parser->errorCode = EDI_ERROR_NO_MEM;
                return NULL;
            }
            $parser->dataBufEnd = $newBuf + $size;
            if($parser->bufReadPtr){
                memcpy($newBuf, 
                       $parser->bufReadPtr, 
                       $parser->bufEndPtr - $parser->bufReadPtr);
                FREE($parser, $parser->dataBuffer);
            }
            $parser->bufEndPtr = $newBuf + 
                                ($parser->bufEndPtr - $parser->bufReadPtr);
            $parser->bufReadPtr = $newBuf;
            $parser->dataBuffer = $newBuf;
        }
    }
    return $parser->bufEndPtr;
}
/******************************************************************************/
function EDI_GetSchema($parser)
{
    $schema = new EDI_Schema;
    if($parser){
        $schema = $parser->schema;
    }
    return $schema;
}
/******************************************************************************/
function EDI_SetSchema($parser, $schema)
{
    if($schema){
        $parser->schema = $schema;
        $parser->validate = EDI_TRUE;
        $schema->parser = $parser;
    }
}
/******************************************************************************/
function EDI_RemoveSchema($parser)
{
    $schema = new EDI_Schema;
    if($parser){
        $schema = $parser->schema;
        $parser->schema = NULL;
        $parser->validate = EDI_FALSE;
        if($schema){
            $schema->parser = NULL;
        }
    }
    return $schema;
}
/******************************************************************************/
function EDI_Parse($parser, $s, $len, $final)
{
    $buffer = '';

    switch ($parser->state) {
        case EDI_SUSPENDED:
            $parser->errorCode = EDI_ERROR_SUSPENDED;
            return EDI_STATUS_ERROR;
        case EDI_FINISHED:
            $parser->errorCode = EDI_ERROR_FINISHED;
            return EDI_STATUS_ERROR;
        default:
            $parser->state = EDI_PARSING;
    }
    $buffer = EDI_GetBuffer($parser, $len);
    if ($buffer == ''){
        return EDI_STATUS_ERROR;
    } else {
        $buffer = substr($s,0, $len);
        return EDI_ParseBuffer($parser, $len, $final);
    }
}
/******************************************************************************/
function EDI_ParseBuffer($parser, $len, $final)
{
    $error = EDI_ERROR_NONE;

    if(!$parser->documentStartHandler){
        echo( "FATAL (edival): No callback registered, event: Document Start\n");
        $error = $parser->errorCode = EDI_ERROR_ABORTED;
    }
    if(!$parser->documentEndHandler){
        echo( "FATAL (edival): No callback registered, event: Document End\n");
        $error = $parser->errorCode = EDI_ERROR_ABORTED;
    }
    if(!$parser->segmentStartHandler){
        echo( "FATAL (edival): No callback registered, event: Segment Start\n");
        $error = $parser->errorCode = EDI_ERROR_ABORTED;
    }
    if(!$parser->segmentEndHandler){
        echo( "FATAL (edival): No callback registered, event: Segment End\n");
        $error = $parser->errorCode = EDI_ERROR_ABORTED;
    }
    if(!$parser->compositeStartHandler){
        echo( "FATAL (edival): No callback registered, event: Composite Start\n");
        $error = $parser->errorCode = EDI_ERROR_ABORTED;
    }
    if(!$parser->compositeEndHandler){
        echo( "FATAL (edival): No callback registered, event: Composite End\n");
        $error = $parser->errorCode = EDI_ERROR_ABORTED;
    }
    if(!$parser->elementHandler){
        echo( "FATAL (edival): No callback registered, event: Element\n");
        $error = $parser->errorCode = EDI_ERROR_ABORTED;
    }
    if($error){
        return EDI_STATUS_ERROR;
    }
    switch ($parser->state) {
        case EDI_SUSPENDED:
            $parser->errorCode = EDI_ERROR_SUSPENDED;
            return EDI_STATUS_ERROR;
        case EDI_FINISHED:
            $parser->errorCode = EDI_ERROR_FINISHED;
            return EDI_STATUS_ERROR;
        default:
            $parser->state = EDI_PARSING;
    }
    if(!($parser->machine)){
        $parser->machine = EDI_StateMachineCreate($parser->memsuite);
        if(!($parser->machine)){
            $parser->errorCode = EDI_ERROR_NO_MEM;
            return EDI_STATUS_ERROR;
        }
        EDI_AddState($parser->machine, $parser->seekHeader, 0);
        EDI_AddState($parser->machine, $parser->error, 1);
        EDI_SetStartState($parser->machine, $parser->seekHeader);
    }
    $parser->bufEndPtr += len;
    $parser->bufEndPtr[0] = '\0';
    $parser->final = $final;
    $error = EDI_StateMachineRun($parser->machine, $parser);
    switch($error){
        case EDI_ERROR_NONE:
            $parser->errorCode = EDI_ERROR_NONE;
            return EDI_STATUS_OK;
        case EDI_ERROR_FINISHED:
            $parser->errorCode = EDI_ERROR_FINISHED;
            return EDI_STATUS_OK;
        case EDI_ERROR_SUSPENDED:
            $parser->errorCode = EDI_ERROR_NONE;
            return EDI_STATUS_SUSPENDED;
        default:
            $parser->errorCode = error;
            return EDI_STATUS_ERROR;
    }
}
/******************************************************************************/
function EDI_StopParser($parser, $resumable)
{
    echo( "FIXME: EDI_StopParser is not yet implemented.\n");
    $parser->errorCode = EDI_ERROR_UNKNOWN;
    return EDI_STATUS_ERROR;
}
/******************************************************************************/
function EDI_ResumeParser($parser)
{
    echo( "FIXME: EDI_ResumeParser is not yet implemented.\n");
    $parser->errorCode = EDI_ERROR_UNKNOWN;
    return EDI_STATUS_ERROR;
}
/******************************************************************************/
function EDI_GetParserState($parser)
{
    return $parser->state;
}
/******************************************************************************/
function EDI_GetErrorCode($parser)
{
    return $parser->errorCode;
}
/******************************************************************************/
function EDI_MemMalloc($parser, $size)
{
    return MALLOC(parser, size);
}
/******************************************************************************/
function EDI_MemRealloc($parser, $pointer, $size)
{
    return REALLOC($parser, $pointer, $size);
}
/******************************************************************************/
function EDI_MemFree($parser, $pointer)
{
    return FREE($parser, $pointer);
}
/******************************************************************************/
function EDI_ParserFree($parser)
{
    // void (*free_fcn)(void *ptr);

    if($parser->schema){
        EDI_RemoveSchema($parser);
    }
    if($parser->child){
        $parser->freeChild($parser);
    }
    if($parser->binBuffer){
        FREE($parser, $parser->binBuffer);
    }
    FREE($parser, $parser->dataBuffer);
    FREE($parser, $parser->machine);
    $free_fcn = $parser->memsuite->free_fcn;
    free_fcn($parser->memsuite);
    free_fcn($parser);
}
?>
