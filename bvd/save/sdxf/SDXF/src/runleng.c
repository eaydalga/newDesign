/* doc------------------------------------------------------------------------------
:Project.   SDXF
:Program.   runleng.c
:PrType.    function lib
:Title.     Compression & decompression with RunLength1 algorithm. 
:Version.   1.1
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI C
:Compiler.  
:Created.   1996-02-14 adapted from Oberon source

:Description. The compressed data consists of several sections of various
   length. Every section starts with a "counter" byte, a signed "tiny" 
   (8 bit) integer, which contains a length information.

   If this byte contains the value "n",
   with n >= 0 (and n <128), the next n+1 bytes will be taken unchanged;
   with n <  0 (and n > -128), the next byte will be replicated -n+1 times;
   n = -128 will be ignored.

------------------------------------------------------------------------------------*/

#include <string.h>
#include "compress.h"

#define  CMP_MAXCNT       128

void CMP_dmy() {}  /* dummy for MVS */ 

#define  PROGERR_1       91
#define  PROGERR_2       92



/****************************************************************/
long  RL1_compress   
		 ( Byte *p_buffout   /* -> out */ 
		 , Byte *p_buffin    /* in     */ 
		 , long  p_bufflen   /* in     */ 
		 )
{
  Byte    *endbuffer, *target, *cb, *source;
  int      cnt, status, rep, error;
  long     srcCnt, trgCnt;
  Byte     oldChar;
  
  source    = p_buffin;
  srcCnt    = 0;
  trgCnt    = 0;           /* length compression header ??*/ 
  target    = p_buffout;
  
  if ( p_bufflen < CMP_THRESHOLD ) return 0;

  error     = CMP_RC_overflow;
  endbuffer = target + p_bufflen;

  status  = 1;

  for (;;)
  {
    switch (status)
    {
    case 1 :
      cb      = target;
      ++target;
      if ( target > endbuffer) return -error;
      oldChar = *source;
      cnt     = 0;
      rep     = 0;
      status  = 2;
      break;

    case 2 :
      *target = *source;
      ++target;
      if ( target > endbuffer) return -error;
      ++source;
      ++cnt;
      ++srcCnt;

      if ( srcCnt > p_bufflen ) 
      {
        trgCnt += cnt;
        *cb     = (Byte)(cnt - 2);
        return trgCnt;
      } 

      if ( cnt >= CMP_MAXCNT )
      {
        trgCnt += cnt + 1;
        *cb     = (Byte)(cnt - 1);
        status  = 1;
      }
      else
        status  = 3;

      break;

    case 3 :
      if ( *source == oldChar )
      {
        ++rep;
        switch (rep)
        {
        case 0 :
          return -PROGERR_1;

        case 1 :
          status = 2;
          break;

        case 2 :
          if ( cnt > 2 ) status = 4;
          else           status = 5;

          if (status == 5) cnt = 3;
          break;

        default:
          status = 5;

        } /* end switch rep */ 

      } /* end if oldChar */ 

      else
      {
        if ( rep > 1 )
          status = 6;
        else
        {
          oldChar = *source;
          rep     = 0;
          status  = 2;
        } /* end else */;

      } /* end else *source = oldChar */ 
      break;

    case 4 :
      trgCnt += cnt - 1;
      *cb     = (Byte)(cnt - 3);
      cnt     = 3;
      cb      = target - 2;
      status  = 5;
      break;

    case 5 :
      ++source;
      ++srcCnt;
      ++cnt;

      if ( srcCnt >= p_bufflen ) 
      {
        trgCnt += 2;
        *cb     = (Byte)(-cnt + 2);
        return trgCnt;
      } 

      if ( cnt > CMP_MAXCNT )  status = 6;
      else                     status = 3;

      break;

    case 6 :
      trgCnt += 2;
      *cb     = (Byte)(-cnt + 2);
      target  = cb + 2;
      if ( target > endbuffer) return -error;
      status = 1;
      break;

    default:
      return -PROGERR_2;
    } /* end switch status */ 

  } /* end while */ 

} /* END RL1_compress */ 


/****************************************************************/

int  RL1_decompress                                          /*#55*/ 
     ( Byte *p_buffout   /* in  */ 
     , Byte *p_buffin    /* in: raw data: no header */ 
     , long  p_bufflen   /* in  */ 
     )
{
  Byte   *target, *source;
  long    trgCnt;
  Small   cnt;               /*?? or better int ? **/ 
  int     cntx;

  source  = p_buffin;
  target  = p_buffout;
  trgCnt  = 0;

  while ( trgCnt < p_bufflen)
  {
    cnt = *source;

    if ( cnt >= 0 )
    {
      cntx = cnt + 1;
      trgCnt += cntx;

      if ( trgCnt > p_bufflen ) return CMP_RC_overflow;
     
      ++source;
      memcpy (target, source, cntx);
      source += cntx;
      target += cntx;
   
    } /* end if cnt >= 0 */ 

    else if ( cnt > -CMP_MAXCNT )
    {
      cntx = -cnt + 1;
      ++source;
      trgCnt += cntx;

      if ( trgCnt > p_bufflen ) return CMP_RC_overflow;

      memset (target, *source, cntx);
      ++source;
      target += cntx;

    }  /* end else if cnt < 0 */ 

  } /* END WHILE*/ 

  return CMP_RC_ok;
} /* END RL1_decompress */ 



/** END Compression.**/ 

/** -----------------------   end of source   ----------------------- **/
