/* doc----------------------------------------------------------------
:Project.   SDXF
:Program.   compress.h
:PrType.
:Title.     Common compression interface
:Version.   1.0
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI C
:Compiler.  
:Created.   1996-02-14 adapted from Oberon source

------------------------------------------------------------------
*/
#ifndef compress_h
#define compress_h

#include "sysdef.h"

#define  CMP_THRESHOLD        8
#define  CMP_RC_ok            0
#define  CMP_RC_failed        1
#define  CMP_RC_overflow      2
#define  CMP_RC_wrong_header  3
#define  CMP_RC_unknown_type  4


/* short names */ 
#ifdef _SYS_MAINFRAME
  #define  CMP_orgLength  CMorglen
  #define  CMP_compress   CMcompr 
  #define  CMP_decompress CMdecmpr 
#endif /* _SYS_MAINFRAME */ 

long CMP_orgLength (Byte *str);
 
long RL1_compress   (Byte *out, Byte *in, long inlen); 
int  RL1_decompress (Byte *out, Byte *in, long inlen);

#define DEF_compress   compress2
#define DEF_decompress uncompress

DllExport int compress2 
  ( char           *tgt       /* target buffer          */ 
  , unsigned long  *tgtsize   /* size of target buffer  */ 
  , char           *src       /* source buffer          */ 
  , unsigned long   srcsize   /* size of source buffer  */ 
  , int             level     /* compression level      */ 
  );
    /* returns: 0  if ok     */ 
    /*          <0 if error  */ 

DllExport int uncompress
  ( char *tgt
  , long *tgtsize
  , char *src
  , long  srcsize
  );
    /* returns: decompressed length if ok  */ 
    /*          < 0 as error code          */ 


#endif /* compress_h */ 
/*****************  end of source   ****************/
