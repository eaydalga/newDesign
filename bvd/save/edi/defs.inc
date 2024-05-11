<?php
define("EDIFACT_ERROR_NONE","0");
define("EDIFACT_CNTL_NUM_MATCH","1");
define("EDIFACT_UNSUPPORTED_STANDARD","2");
define("EDIFACT_UNSUPPORTED_VERSION","3");
define("EDIFACT_INVALID_SEG_TERM","4");
define("EDIFACT_INVALID_STANDARD","5");
define("EDIFACT_INVALID_VERSION","6");
define("EDIFACT_INVALID_CNTL_NUM","7");
define("EDIFACT_INVALID_CONTENT","8");
define("EDIFACT_INVALID_ELEM_SEP","9");
define("EDIFACT_INVALID_COMP_SEP","10");
define("EDIFACT_INVALID_RELEASE","11");
define("EDIFACT_INVALID_DECIMAL","12");
/******************************************************************************/
define("DEFAULT_ELEMENT",'+');
define("DEFAULT_COMPONENT",':');
define("DEFAULT_DECIMAL",'.');
define("DEFAULT_SEGMENT",'\'');    /*'*/
define("DEFAULT_RELEASE",'?');

define("EDI_MAJOR_VERSION","0");
define("EDI_MINOR_VERSION","1");
define("EDI_MICRO_VERSION","8");

define("EDI_TRUE",  "1");
define("EDI_FALSE", "0");

define("MAX_RUN_STATES","25");
/****************************************************************************/

// define("EDIFACT_PARSER",$parser->child);
// define("EDI_PARSER",$parser);

// define("X12_PARSER",$parser->child);
// define("EDI_PARSER",$parser);
/******************************************************************************/
$primes[] = Array(
	53,        97,        193,       389,
	769,       1543,      3079,      6151,
	12289,     24593,     49157,     98317,
	196613,    393241,    786433,    1572869,
	3145739,   6291469,   12582917,  25165843,
	50331653,  100663319, 201326611, 402653189,
	805306457, 1610612741
);

