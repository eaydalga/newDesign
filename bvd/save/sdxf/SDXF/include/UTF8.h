/* doc----------------------------------------------------------------
:Project.   Structured Data eXchange Format (SDXF)
:Program.   UTF8.h
:Title.     Definitions for en/de-coding char string to/from UTF8
:PrType.    Header File
:Created.   2000-12-09
:Author.    Max Wildgrube
:Copyright. Max Wildgrube
:Language.  C (ANSI)
----------------------------------------------------------------------*/
/*
  
UTF-8 encoding

bytes bits representation 
1        7 0bbbbbbb 
2       11 110bbbbb 10bbbbbb 
3       16 1110bbbb 10bbbbbb 10bbbbbb 
4       21 11110bbb 10bbbbbb 10bbbbbb 10bbbbbb (UCS4)

Each b represents a bit that can be used to store character data. 
*/

#ifndef _UTF8
#define _UTF8

#include "sysdef.h"

#define UCS_HIGH_BYTE(x)  (x >> 8)
#define UCS_LOW_BYTE(x)   (x & 0xFF)

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/ 

/* basic UTF-8 definitions: */ 

typedef enum  {UTF8_TO = 1, UTF8_FROM = 2} UTF8_cvtMode; 

typedef bool UTF8_TCvtProc
                  ( UTF8_cvtMode mode                       /* direction */ 
                  , Byte *target,      long *targetlength   /* output    */ 
                  , Byte *source,      long  sourcelength   /* input     */ 
                   );
    /* targetlength contains maximal size as input param. */ 
    /*                   (or == 0 if no check is needed.) */  
    /* returns success: true = ok, false = no conversion  */ 



/* UTF-8 tools:   --------------------------------------------------- */ 

DllExport UTF8_TCvtProc UTF8_vsANSI1;
  /*  converts UTF-8 to ANSI Latin-1 character set and v.v.           */ 
  /*  non Latin-1 characters from UTF-8 will be coded as 0x7F (del)   */ 

DllExport UTF8_TCvtProc UTF8_vsHost;
  /*  converts UTF-8 to system dependent (host) character set and v.v. */ 
  /*  Assuming ANSI Latin-1 is the Network character set, translation  */ 
  /*  will be done to / from ANSI1 to Host                             */ 

  /* hint: input will be overwritten in TO_UTF8 mode !!                */ 



/* elementary UTF-8 functions, used by the UTF-8 tools: */ 
 
DllExport int UTF8_toUCS4   (/*out*/ long *, /*in*/ char *, /*in*/ long); 
DllExport int UTF8_fromUCS4 (/*out*/ char *, /*in*/ long );

/* returns: -1 on error                           */ 
/*           0 end of string (output unchanged)   */ 
/*          >0 number of chars used or generated  */ 


#ifdef __cplusplus
}//end-extern-C
#endif /*__cplusplus*/ 

#endif
