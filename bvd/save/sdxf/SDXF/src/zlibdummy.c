/* doc-----------------------------------------------------------------------------
:Project.   SDXF: Structured Data Exchange Format
:Program.   zlibdummy.c
:PrType.    function lib
:Title.     A fake for the zlib compression functions
:Version.   
:Author.    Max Wildgrube
:Language.  ANSI-C
:Compiler.
:Created.   
---------------------------------------------------------------------------------

purpose: if the global macro constant DEFLATE_IMPLEMENTED is NOT set this 
  modul define dummy entries for the related externals in zlib.lib.
  This avoids "unresolved externals" errors.

---------------------------------------------------------------------------------*/ 
#include "compress.h"

#ifndef DEFLATE_IMPLEMENTED

int compress2 
  ( char           *tgt       /* target buffer          */ 
  , unsigned long  *tgtsize   /* size of target buffer  */ 
  , char           *src       /* source buffer          */ 
  , unsigned long   srcsize   /* size of source buffer  */ 
  , int             level     /* compression level      */ 
  )
    /* returns: < 0 if error  */ 
    /*          ==0 if ok     */ 
 
{
  return -99;  
}

  
int uncompress
  ( char *tgt
  , long *tgtsize
  , char *src
  , long  srcsize
  )
    /* returns: decompressed length if ok  */ 
    /*          < 0 as error code          */ 
{
  return -99;  
}

#endif