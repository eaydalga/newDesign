/* doc----------------------------------------------------------------
:Project.   SDXF: Structured Data Exchange Format
:Program.   SDXtxt.h
:PrType.    header file
:Title.     SDXF message texts
:Version.   1.0
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI-C
:Compiler.
:Created.   2001-12-08 

------------------------------------------------------------------*/
    
/*00*/ #define RC_TEXT_ok               "ok"
/*01*/ #define RC_TEXT_failed           "failed/warning"
/*02*/ #define RC_TEXT_illegalOperation "illegal Operation"
/*03*/ #define RC_TEXT_dataError        "dataError"
/*04*/ #define RC_TEXT_parameterError   "parameter Error"
/*05*/ #define RC_TEXT_programError     "program Error"
/*--*/ #define RC_TEXT_other            "unknown rc"

/*01*/ #define EC_TEXT_end_of_data      "end of data"
/*02*/ #define EC_TEXT_notFound         "not found"
/*03*/ #define EC_TEXT_dataCutted       "data cutted"
/*04*/ #define EC_TEXT_overflow         "overflow"
/*05*/ #define EC_TEXT_initError        "wrong dataType for Init or Init not given"
/*06*/ #define EC_TEXT_comprerr         "compression error"
/*07*/ #define EC_TEXT_forbidden        "oper. forbidden"
/*08*/ #define EC_TEXT_unknown          "unknown compr.typ"
/*09*/ #define EC_TEXT_levelOvflw       "level overflow"
/*10*/ #define EC_TEXT_paramMissing     "parameter missing"
/*11*/ #define EC_TEXT_magicError       "wrong magic value"
/*12*/ #define EC_TEXT_not_consistent   "SDX structure not consistent"
/*13*/ #define EC_TEXT_wrongDataType    "data type not allowed for array"
/*14*/ #define EC_TEXT_noMemory         "no memory"
/*15*/ #define EC_TEXT_malformed        "malformed UTF8"
/*16*/ #define EC_TEXT_crypterror       "encrypt error"
/*17*/ #define EC_TEXT_wrongLength      "wrong data length"
/*18*/ #define EC_TEXT_FTP_inexact	    "result of floating conversion is inexact"
/*19*/ #define EC_TEXT_FTP_invalid	    "result of floating conversion is invalid"
/*20*/ #define EC_TEXT_FTP_underflow	  "underflow while floating conversion"
/*21*/ #define EC_TEXT_FTP_overflow	    "overflow while floating conversion"
/*22*/ #define EC_TEXT_unknown_function "unknown_function"
/*23*/ #define EC_TEXT_cont_too_small   "(rest of) container too small "
/*24*/ #define EC_TEXT_not_supported	  "function not supported"
/*25*/ #define EC_TEXT_data_missing	    "data missing"
/*26*/ #define EC_TEXT_not_structured   "chunk not structured"

/*--*/ #define EC_TEXT_other            "unspecified error"
