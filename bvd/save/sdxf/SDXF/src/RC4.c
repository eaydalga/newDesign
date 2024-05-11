/* doc----------------------------------------------------------------
:Project.   SDXF
:Program.   RC4.c
:PrType.    function lib
:Title.     A symetric encryption routine
:Version.   1.1
:Author.
:Copyright.
:Language.  ANSI C
:Created.   1997-10-12 adapted from fabian @ steganos
:Changed.   2000-11-12 including permutation of buffer 
----------------------------------------------------------------------*/

#define EN_VERSION "RC4 Steganos (DEMCOM) 1997"
 
#include <string.h>
#include "SDXF.h"
#include <stdio.h>
//#include "tools.h"

#define BYTELIM  256

enum
{
  CRYPT_START  = 99
, CRYPT_ADD    = 77
};

void EN_dmy() {}  /* dummy for MVS */

/******************************************************************/
long SDXE_RC4 
       ( enum Tcrypt_mode  p_mode
       , Byte             *p_buffer
       , long              p_len
       , char             *p_passw
       )
{
  int  CryptI, CryptJ;
  Byte sbox [BYTELIM], pbox [BYTELIM], s_buffer [BYTELIM], key [BYTELIM]; 
  int  ix, iy;
  int  passlen = SDXE_PASSLEN;                                            /*#07*/ 
  Byte buf, CryptModify;

  Byte cr_start = (CRYPT_START + 6) % BYTELIM;
  Byte cr_add   = (CRYPT_ADD   + 4) % BYTELIM;

  /* the cr_... are all odd, so they cannot be 0 */ 

  switch (p_mode)
  {
  case  CM_GET_VERSION:
    strncpy (p_buffer, EN_VERSION, p_len);
    return 0;

  case CM_ENCRYPT :
  case CM_DECRYPT :
    break;
  
  default:
    return 0;
  }

  if (p_buffer == NULL || p_len <= 0) return 0;
  if (p_passw  == NULL              ) return 0;

#ifdef test
  printf ("SDXE_RC4 mode: %d, using passw:\n", p_mode);
  BT_hexedit (0, p_passw, 1);
#endif

  for (ix = 0; ix < BYTELIM; ix++) key [ix] = p_passw [ix % passlen];

  CryptModify = CRYPT_START;

  /* initialize the s-box:  */ 

  for (ix = 0; ix < BYTELIM; ix++) sbox [ix] = (Byte)ix;

  CryptJ=0;

  for (CryptI = 0; CryptI < BYTELIM; CryptI++)
  {
    CryptJ = (CryptJ + sbox [CryptI] + key [CryptI]) % BYTELIM;

    buf           = sbox [CryptI];
    sbox [CryptI] = sbox [CryptJ];
    sbox [CryptJ] = buf;

  } /*end-for CryptI */ 

  CryptI=0;
  CryptJ=0;

  for (ix = 0; ix < BYTELIM; ix++) pbox [ix] = sbox [ix];

  for (ix=0; ix < p_len; ix += BYTELIM)
  {
    int diff = p_len - ix;
     
    if (diff < BYTELIM)
    {
      int i = 0, j = 0;

      while (i < diff)
      {
        if (sbox [j] < (Byte)diff)
        {
          pbox [i] = sbox [j];
          ++i;
        }
        ++j;
      } /* end-while */ 
    }
    else
      diff = BYTELIM;
    /* end-if diff */ 

    for (iy = 0; iy < BYTELIM && ix + iy < p_len; ++iy)
      s_buffer [iy] = p_buffer [ix + iy];

    for (iy = 0; iy < BYTELIM && ix + iy < p_len; ++iy)
    {
      Byte t;

      /* generation of a random byte:  */ 

      CryptI = (CryptI + 1) % BYTELIM;
      CryptJ = (CryptJ + sbox [CryptI]) % BYTELIM;

      buf           = sbox [CryptI];
      sbox [CryptI] = sbox [CryptJ];
      sbox [CryptJ] = buf;

      t = (Byte)((sbox [CryptI] + sbox [CryptJ]) % BYTELIM);

      /*  END of generation of a random byte  */ 

      CryptModify = (Byte)((CryptModify + CRYPT_ADD) % BYTELIM);

      switch (p_mode)
      {
      case CM_ENCRYPT :
        p_buffer [ix + iy] = s_buffer [pbox [iy]] ^ sbox [t] ^ CryptModify;
        break;
      case CM_DECRYPT :
        p_buffer [ix + pbox [iy]] = s_buffer [iy] ^ sbox [t] ^ CryptModify;
        break;
      default:
        return 0;
      }
    }/*end-for iy */ 
  }/*end-for ix */ 

  return p_len;

}/* end-EnCrypt*/ 




