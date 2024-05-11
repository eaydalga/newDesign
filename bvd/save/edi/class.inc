<?php
class EDI_SchemaNodeStruct
{
    var $adrnodeID;
    var $type; // enum EDI_NodeType
    var $refCount;
    var $schema = EDI_SchemaNodeStruct;
};
/******************************************************************************/
class EDI_ChildNodeStruct
{
        var $node = EDI_SchemaNodeStruct;
        var $previousSibling = EDI_ChildNodeStruct;
        var $nextSibling = EDI_ChildNodeStruct;
        var $min_occurs;
        var $max_occurs;
        var $count;
};
/******************************************************************************/
class EDI_ComplexTypeStruct
{
      var $nheader = EDI_SchemaNodeStruct;  
      var $firstChild = EDI_ChildNodeStruct;
      var $lastChild = EDI_ChildNodeStruct;
      var $childCount;
      var $firstNote = EDI_SyntaxNote;
      var $finalNote = EDI_SyntaxNote;
};

class EDI_LoopNodeStruct
{
        var $node = EDI_ComplexTypeStruct;
        var $startID;
};
/******************************************************************************/
class EDI_SyntaxNoteStruct
{
        var $type = EDI_SyntaxType;
        var $count;
        var $position;
        var $next = EDI_SyntaxNote;
};

/******************************************************************************/
class tok {
	var $token;
	var $length;
	var $capacity;
};
class EDIFACT_ParserStruct
{
	var $parent;
	var $delimiters = Array();
	var $savedTag;
        var $tok =tok;
	var $savedElementPosition;
	var $savedComponentPosition;
	var $error;       // EDIFACT_ParseError;
        var $segmentError; // enum EDI_SegmentValidationError
        var $previous;     // enum EDIFACT_Delimiter
        var $data;         // EDI_DataElement
};
/******************************************************************************/
class token
{
    var $type = EDIFACT_Delimiter;
    var $token;
    var $length;
};
/******************************************************************************/

class EDI_Parser
{
	var $userData;
	var $memsuite = EDI_Memory_Handling_Suite;
	var $bufReadPtr;
	var $bufEndPtr;
	var $dataBuffer;
	var $dataBufEnd;
	var $errorCode;  // enum EDI_Error
	var $state;      // enum EDI_ParsingState
        var $documentStartHandler = EDI_DocumentStartHandler;
        var $documentEndHandler = EDI_DocumentEndHandler;
        var $segmentStartHandler = EDI_SegmentStartHandler;
        var $segmentEndHandler = EDI_SegmentEndHandler;
        var $compositeStartHandler = EDI_CompositeStartHandler;
        var $compositeEndHandler = EDI_CompositeEndHandler;
        var $elementHandler = EDI_ElementHandler;
        var $binBufferHandler = EDI_BinaryBufferHandler;
	var $binaryElementSize;
	var $bytesHandled;
	var $binBuffer;
        var $repeatHandler = EDI_RepeatHandler;
        var $nonEDIDataHandler = EDI_NonEDIDataHandler;
	/* Internally used by the parsers */
        var $seekHeader = EDI_StateHandler;
        var $process = EDI_StateHandler;
        var $error = EDI_StateHandler;
	var $machine;
	var $child;
	var $freeChild = EDI_Parser;
	var $docTyep;   // enum EDI_DocumentType
        var $schema = EDI_Schema;
        var $validate = EDI_Bool;
        var $final = EDI_Bool;
};

/*******************************************************************************
 * Struct for passing parsed element data back through the EDI_ElementHandler
 * callback function.  All elements will be a "string" except EDI_DATA_INTEGER
 * and EDI_DATA_BINARY_SIZE will be an "integer" and EDI_DATA_DECIMAL will be 
 * a "decimal".  
 ******************************************************************************/
class EDI_DataElement
{
	var $type = EDI_PrimitiveDataType;
        var $data; // union of integer, char etc
};

class EDI_Memory_Handling_Suite
{
  function malloc_fcn ($size) {}
  function realloc_fcn($ptr, $size){}
  function free_fcn   ($ptr){}
};

class entry
{
    var $adrk, $adrv;
    var $h;
    var $next = entry;
};

class hashtable
{
    var $tablelength;
    var $adrtable = entry;
    var $entrycount;
    var $loadlimit;
    var $primeindex;
};

class EDI_StateMachineStruct
{
    var $startState = EDI_StateHandler;
    var $resumeState = EDI_StateHandler;
    var $runStates = Array(); // EDI_StateHandler;
    var $stateCount;
    var $endStates = Array(); // EDI_StateHandler;
    var $endCount;
};
/******************************************************************************/

class X12_ParserStruct
{
    var $parent = EDI_Parser;
    var $delimiters = Array(); // 5
    var $savedTag;
    var $compositeTag;
    var $savedElementPosition;
    var $savedComponentPosition;
    var $error;        // enum X12_ParseError
    var $segmentError; // enum EDI_SegmentValidationError
    var $previous;     // enum X12_Delimiter
    var $data = EDI_DataElement;
};
/******************************************************************************/

class EDI_SchemaStruct
{
    var $adridentifier;
    var $documentType = EDI_DocumentType;
    var $adrelements = hashtable;
    var $adrcomplexNodes = hashtable;
    var $root = EDI_ComplexTypeStruct;
    var $stack = Array(); // EDI_ChildNode[50];
    var $depth;
    var $prevElementNode = EDI_ChildNode;
    var $prevElementIndex = prevElementIndex;
    var $adrmemsuite = EDI_Memory_Handling_Suite;
    var $parser = EDI_Parser;
    var $loopStartHandler = EDI_LoopStartHandler;
    var $loopEndHandler = EDI_LoopEndHandler;
    var $segmentErrorHandler = EDI_SegmentErrorHandler;
    var $elementErrorHandler = EDI_ElementErrorHandler;
};
/******************************************************************************/
?>
