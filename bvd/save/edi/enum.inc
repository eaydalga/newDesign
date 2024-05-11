<?php
$EDI_Error = array (
	'EDI_ERROR_NONE' => 0,
	'EDI_ERROR_NO_MEM' => 1,
	'EDI_ERROR_UNKNOWN' => 2,
	'EDI_ERROR_INVALID_STATE' => 3,
	'EDI_ERROR_SUSPENDED' => 4,
	'EDI_ERROR_ABORTED' => 5,
	'EDI_ERROR_BUFFER_END' => 6,
	'EDI_ERROR_FINISHED' => 7
);

$EDI_Status = array (
	'EDI_STATUS_OK' => 0,
	'EDI_STATUS_SUSPENDED' => 1,
	'EDI_STATUS_ERROR' => 2
);

$EDI_ParsingState = array (
	'EDI_INITIALIZED' => 0,
	'EDI_PARSING' => 1,
	'EDI_FINISHED' => 2,
	'EDI_SUSPENDED' => 3
);

/*******************************************************************************
 *  Enumeration of all supported document types.
 ******************************************************************************/
$EDI_DocumentType = array (
	'EDI_UNKNOWN_DOC' => 0,
	'EDI_ANSI_X12' => 1,
	'EDI_EDIFACT' => 2
);

/*******************************************************************************
 *  Enumeration of all supported basic datatypes.  Binary data allows any data
 *  stream to validate, assuming size constraints are met (X12 BIN02 element).
 ******************************************************************************/
$EDI_PrimitiveDataType = array (
    'EDI_DATA_STRING' => 1, /* Alphanumeric string & predefined identifiers */
    'EDI_DATA_INTEGER' => 2,
    'EDI_DATA_DECIMAL' => 3,
    'EDI_DATA_DATE' => 4,       /* CCYYMMDD */
    'EDI_DATA_TIME' => 5,       /* HHMMSSdd */
    'EDI_DATA_BINARY' => 6,     /* Binary data */
    'EDI_DATA_BINARY_SIZE' => 7 /* ONLY for a numeric element that gives the */
                             /* size of a binary element following next.  */
);

$EDI_NodeType = array (
	'EDITYPE_DOCUMENT' => 1,
	'EDITYPE_LOOP' => 2,
	'EDITYPE_SEGMENT' => 3,
	'EDITYPE_COMPOSITE' => 4,
	'EDITYPE_ELEMENT' => 5
);

/*******************************************************************************
 * Enumeration of errors that can signal an invalid string for a given element
 * in the data stream.
 ******************************************************************************/
$EDI_ElementValidationError = array (
    'VAL_VALID_ELEMENT' => 0,
    'VAL_RANGE_HIGH' => 1,
    'VAL_RANGE_LOW' => 2,
    'VAL_CHAR_ERROR' => 3,
    'VAL_CODE_ERROR' => 4,
    'VAL_DATE_ERROR' => 5,
    'VAL_TIME_ERROR' => 6,
    'VAL_UNKNOWN_ELEMENT' => 7,
    'VAL_MANDATORY_ELEMENT' => 8,
    'VAL_TOO_MANY_ELEMENTS' => 9,
    'VAL_MISSING_CONDITIONAL' => 10,
    'VAL_EXCLUSION_VIOLATED' => 11,
    'VAL_TOO_MANY_COMPONENTS' => 12,
    'VAL_REPETITION_EXCEEDED' => 13,
    'VAL_INVALID_SEGMENT' => 14
);

/*******************************************************************************
 * Enumeration of errors that can signal an invalid occurance for a segment
 * in the data stream.
 ******************************************************************************/
$EDI_SegmentValidationError = array (
    'SEGERR_NONE' => 0,
    'SEGERR_UNEXPECTED' => 1,
    'SEGERR_MANDATORY' => 2,
    'SEGERR_LOOP_EXCEEDED' => 3,
    'SEGERR_EXCEED_REPEAT' => 4,
    'SEGERR_UNDEFINED' => 5,
    'SEGERR_SEQUENCE' => 6,
    'SEGERR_UNKNOWN' => 7,
    'SEGERR_LOOP_SEEK' => 8
);

/*******************************************************************************
 * Enumeration of element syntax restrictions that can be enforced
 * These are taken from the X12 standards.  Other syntax types could be added
 * as required.
 ******************************************************************************/
$EDI_SyntaxType = array(
    'EDI_SYNTAX_PAIRED' => 1,
    'EDI_SYNTAX_REQUIRED' => 2,
    'EDI_SYNTAX_EXCLUSION' => 3,
    'EDI_SYNTAX_CONDITION' => 4,
    'EDI_SYNTAX_LIST' => 5
);
/******************************************************************************/
$EDIFACT_ParseError = array (
	'EDIFACT_ERROR_NONE' => 0,
	'EDIFACT_CNTL_NUM_MATCH' => 1,
	'EDIFACT_UNSUPPORTED_STANDARD' => 2,
	'EDIFACT_UNSUPPORTED_VERSION' => 3,
	'EDIFACT_INVALID_SEG_TERM' => 4,
	'EDIFACT_INVALID_STANDARD' => 5,
	'EDIFACT_INVALID_VERSION' => 6,
	'EDIFACT_INVALID_CNTL_NUM' => 7,
	'EDIFACT_INVALID_CONTENT' => 8,
	'EDIFACT_INVALID_ELEM_SEP' => 9,
	'EDIFACT_INVALID_COMP_SEP' => 10,
	'EDIFACT_INVALID_RELEASE' => 11,
	'EDIFACT_INVALID_DECIMAL' => 12
);
/******************************************************************************/
$EDIFACT_Delimiter = array (
	'UNKNOWN' => 0,
	'ELEMENT' => 1,
	'COMPONENT' => 2,
	'DECIMAL' => 3,
	'SEGMENT' => 4,
	'RELEASE' => 5
);
/******************************************************************************/
$X12_ParseError = array (
	'X12_ERROR_NONE' => 0,
	'X12_CNTL_NUM_MATCH' => 1,
	'X12_UNSUPPORTED_STANDARD' => 2,
	'X12_UNSUPPORTED_VERSION' => 3,
	'X12_INVALID_SEG_TERM' => 4,
	'X12_INVALID_STANDARD' => 5,
	'X12_INVALID_VERSION' => 6,
	'X12_INVALID_CNTL_NUM' => 7,
	'X12_INVALID_CONTENT' => 8,
	'X12_INVALID_ELEM_SEP' => 9,
	'X12_INVALID_COMP_SEP' => 10,
	'X12_INVALID_REP_SEP' => 11
);
/******************************************************************************/
$X12_Delimiter = array (
	'UNKNOWN' => 0,
	'ELEMENT' => 1,
	'COMPONENT' => 2,
	'REPEAT' => 3,
	'SEGMENT' => 4
);
/******************************************************************************/
/*
enum EDI_Status       EDI_Parse(EDI_Parser, const char *, int, EDI_Bool);
enum EDI_Status       EDI_ParseBuffer(EDI_Parser, int, EDI_Bool);
enum EDI_ParsingState EDI_GetParserState(EDI_Parser);
enum EDI_Status       EDI_StopParser(EDI_Parser, EDI_Bool);
enum EDI_Status       EDI_ResumeParser(EDI_Parser);
enum EDI_Error      EDI_GetErrorCode(EDI_Parser);
enum EDI_DocumentType EDI_GetSchemaType(EDI_Schema);
enum EDI_SegmentValidationError 
     EDI_ValidateSegmentPosition(EDI_Schema, const char *);
enum EDI_ElementValidationError 
     EDI_ValidateElement(EDI_Schema,int,int*,const char *,int,EDI_DataElement);
enum EDI_ElementValidationError 
     EDI_ValidateSyntax(EDI_Schema, int, int);
enum EDI_ElementValidationError 
     EDI_CheckElementConstraints(EDI_SimpleType *, const char*, int, EDI_DataElement );
enum EDIFACT_ParseError         error;
enum EDI_SegmentValidationError segmentError;
enum EDIFACT_Delimiter          previous;
enum EDIFACT_Delimiter  type;
enum EDIFACT_Delimiter type     = UNKNOWN;
enum EDI_DocumentType EDI_GetSchemaType(EDI_Schema schema)
enum EDI_SegmentValidationError EDI_ValidateSegmentPosition(EDI_Schema  schema,
enum EDI_ElementValidationError EDI_ValidateElement(
enum EDI_ElementValidationError EDI_ValidateSyntax(EDI_Schema schema       ,
enum EDI_ElementValidationError EDI_AddElementValue(EDI_SchemaNode  node ,
enum EDI_ElementValidationError EDI_CheckElementConstraints(EDI_SimpleType *element,
*/
/******************************************************************************/
