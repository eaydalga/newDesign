/* doc-----------------------------------------------------------------------------
:Project.   SDXF: Structured Data Exchange Format
:Program.   UTF8.c
:PrType.    function lib
:Title.     en/de-codes char string to/from UTF8
:Version.   
:Author.    Max Wildgrube, Ken Thompson 
:Language.  ANSI-C
:Compiler.
:Created.   2000-12-09
:Description.
  
UTF-8 encoding

bytes bits  representation 
    1    7  0bbbbbbb 
    2   11  110bbbbb 10bbbbbb 
    3   16  1110bbbb 10bbbbbb 10bbbbbb 
    4   21  11110bbb 10bbbbbb 10bbbbbb 10bbbbbb (UCS4)

Each b represents a bit that can be used to store character data. 


---------------------------------------------------------------------------------*/ 

#include "SDXF.h"
#include "tools.h"
#include <string.h>

/****************************************************************************/
bool UTF8_vsHost
              ( UTF8_cvtMode   p_mode     /* 1 = to, 2 = from UTF-8 */
              , Byte *p_target,    long *p_targetlength   /* output */ 
              , Byte *p_source,    long  p_sourcelength   /* input  */ 
               )
  /* targetlength contains maximal size as input param.      */ 
  /*                   (or == 0 if no check is needed.)      */  
  /* returns success: 1 = ok, 0 = no conversion              */ 
  /* hint: input will be overwritten in BT_UTF8_TO mode!!    */ 
{
  SDX_TOptions *opt = SDX_getOptions ();

  switch (p_mode)
  {
  case UTF8_TO:
    BT_convert (p_source, p_source, p_sourcelength, opt->toNet); 
    return UTF8_vsANSI1 
             (UTF8_TO, p_target, p_targetlength, p_source, p_sourcelength);
  case UTF8_FROM:
    if (! UTF8_vsANSI1 
            (UTF8_FROM, p_target, p_targetlength, p_source, p_sourcelength)
       ) return false;
    BT_convert (p_target, p_target, *p_targetlength, opt->toHost); 
    return true;
  default:
    return false;
  }
}/*end- UTF8_vsHost */ 

/****************************************************************************/
bool UTF8_vsANSI1
              ( UTF8_cvtMode p_mode           /* 1 = to, 2 = from UTF-8 */ 
              , Byte *p_target,  long *p_targetlength             /* output */ 
              , Byte *p_source,  long  p_sourcelength             /* input  */ 
              )
  /* targetlength contains maximal size as input param. */ 
  /*                   (or == 0 if no check is needed.) */  
  /* returns success: 1 = ok, 0 = no conversion         */ 
{
  long maxlength = *p_targetlength;
  char buffer [8];

  *p_targetlength = 0;

  switch (p_mode)
  {
  case UTF8_TO:
    while (p_sourcelength--)
    {
      long ansichar = *(p_source++);
      int  numchar;

      numchar = UTF8_fromUCS4 (/*out*/ buffer, /*in*/ ansichar);
      if (numchar < 0) return false;
      *p_targetlength += numchar; 
      if (*p_targetlength >= maxlength) return false;
      strncpy (p_target, buffer, numchar);
      p_target += numchar;
    }/*end-while*/ 
    *p_target = 0;
    break;

  case UTF8_FROM: 
    while (p_sourcelength)
    {
      long ansichar;
      int  numchar;

      numchar = UTF8_toUCS4 (/*out*/ &ansichar, 
                             /*in*/ p_source, /*in*/ p_sourcelength); 
      p_sourcelength -= numchar; 
      ++*p_targetlength;
      p_source += numchar; 

      if (p_sourcelength < 0) 
        return false;
      if (*p_targetlength > maxlength) 
        return false;
      
      *(p_target++) = (ansichar < 256) ? (char)ansichar : (char)127;
    }/*end-while*/
    
    *p_target = 0;
    break;
  
  default:
    return false;
  }/*end-switch mode */ 

  return true;

}/*end- UTF8_vsANSI1 */ 

/****************************************************************************/

/*
 * The following was provided by Ken Thompson of AT&T Bell Laboratories,
 * <ken@research.att.com>, on Tue, 8 Sep 92 03:22:07 EDT, to the X/Open
 * Joint Internationalization Group.  Some minor formatting changes have
 * been made by Glenn Adams, <glenn@metis.com>.
 * The proposed UCS transformation format encodes UCS values in the range
 * [0,0x7fffffff] using multibyte characters of lengths 1, 2, 3, 4, 5,
 * and 6 bytes.  For all encodings of more than one byte, the initial
 * byte determines the number of bytes used and the high-order bit in
 * each byte is set.  Every byte that does not start 10xxxxxx is the
 * start of a UCS character sequence.
 *
 * An easy way to remember this transformation format is to note that the
 * number of high-order 1's in the first byte signifies the number of
 * bytes in the multibyte character:
 *
 * Bits  Hex Min  Hex Max  Byte Sequence in Binary
 *   7  00000000 0000007f 0vvvvvvv
 *  11  00000080 000007FF 110vvvvv 10vvvvvv
 *  16  00000800 0000FFFF 1110vvvv 10vvvvvv 10vvvvvv
 *  21  00010000 001FFFFF 11110vvv 10vvvvvv 10vvvvvv 10vvvvvv
 *  26  00200000 03FFFFFF 111110vv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
 *  31  04000000 7FFFFFFF 1111110v 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
 *
 * The UCS value is just the concatenation of the v bits in the multibyte
 * encoding.  When there are multiple ways to encode a value, for example
 * UCS 0, only the shortest encoding is legal.
 *
 * Below are sample implementations of the C standard wctomb() and
 * mbtowc() functions which demonstrate the algorithms for converting
 * from UCS to the transformation format and converting from the
 * transformation format to UCS. The sample implementations include error
 * checks, some of which may not be necessary for conformance:
 *
 */

typedef struct
{
  int     cmask;
  int     cval;
  int     shift;
  long    lmask;
  long    lval;
} Tab;

static Tab tab[] =
{
  0x80,  0x00,   0*6,    0x7F,           0,              /* 1 byte sequence */ 
  0xE0,  0xC0,   1*6,    0x7FF,          0x80,           /* 2 byte sequence */ 
  0xF0,  0xE0,   2*6,    0xFFFF,         0x800,          /* 3 byte sequence */ 
  0xF8,  0xF0,   3*6,    0x1FFFFF,       0x10000,        /* 4 byte sequence */ 
  0xFC,  0xF8,   4*6,    0x3FFFFFF,      0x200000,       /* 5 byte sequence */ 
  0xFE,  0xFC,   5*6,    0x7FFFFFFF,     0x4000000,      /* 6 byte sequence */ 
  0,                                                     /* end of table    */ 
};

/*************************************************************************/ 
int UTF8_toUCS4 (/*out*/ long *p_wc,/*in*/ char *p_mb,/*in*/ long p_n ) 

/* returns: -1 on error                           */ 
/*           0 end of string (output unchanged)   */ 
/*          >0 number of chars used               */ 
 
{
  long wc;
  int c0, c, nc;
  Tab *t;
  
  if ( p_mb == 0 )  return 0;
  
  nc = 0;
  if ( p_n <= nc ) return -1;

  c0 = *p_mb & 0xff;
  wc = c0;
  
  for ( t = tab; t->cmask; t++ ) 
  {
    nc++;
    if ( ( c0 & t->cmask ) == t->cval ) 
    {
      wc &= t->lmask;
      if ( wc < t->lval )	return -1;
      *p_wc = wc;
      return nc;
    }/*end-if*/ 
    if ( p_n <= nc )  return -1;

    p_mb++;
    c = ( *p_mb ^ 0x80 ) & 0xFF;
    if ( c & 0xC0 )  return -1;
    wc = ( wc << 6 ) | c;
  }/*end-for*/ 
  return -1;
}/*end- mbtowc */ 

/**********************************************************************/ 
int UTF8_fromUCS4 (/*out*/ char *p_mb, /*in*/ long p_wc ) 

/* returns: -1 on error                           */ 
/*           0 end of string (output unchanged)   */ 
/*          >0 number of chars generated          */ 

{
  long wc;
  int c, nc;
  Tab *t;
  
  if (p_mb == 0 ) return 0;
  
  wc = p_wc;
  nc = 0;
  for ( t = tab; t->cmask; t++ ) 
  {
    nc++;
    if ( wc <= t->lmask ) 
    {
      c = t->shift;
      *p_mb = (char)(t->cval | ( wc >> c ));

      while ( c > 0 ) 
      {
	      c -= 6;
	      p_mb++;
	      *p_mb = 0x80 | ( ( wc >> c ) & 0x3F );
      }
      return nc;
    }/*end-if*/ 
  }/*end-for*/ 
  return -1;
}/*end- wctomb */ 
