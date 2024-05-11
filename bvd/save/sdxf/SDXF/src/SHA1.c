/* doc-----------------------------------------------------------------------------
:Project.   SDXF: Structured Data Exchange Format
:Program.   SHA1.c
:PrType.    function lib
:Title.     SHA1 (Secure Hash Algorithm), algorithm by NIST
:Version.   
:Author.    Michael Neumann.
:Language.  ANSI-C
:Compiler.
:Created.   1998
---------------------------------------------------------------------------------*/ 
/*
 *  SHA1 (Secure Hash Algorithm), algorithm by NIST
 *  This program is Copyright (c) 1998 by Michael Neumann.
 *
 *  This One-Way-Hashfunction produces a 160 bit hashvalue,
 *  and is very equal like MD5.
 *
 *  1998-06-01 created
 *  2001-04-13 some changes by max, marked by /*# 
 */

#include <string.h>
#include "tools.h"     /*# added by max; IMPORTS BT_adaptL */ 

#define inline

typedef unsigned long ULONG;


#define  SHA_BITBLOCK         512
#define  SHA_BYTEBLOCK        (SHA_BITBLOCK/8)   /*  = 64  */ 
#define  SHA_HASHVALUE_LEN    (sizeof(ULONG)*5)  /*  = 20  */ 


// To initialise A,B,C,D,E
ULONG magic[5] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0 };


// rotates x n-bits to left
inline ULONG ROTATE_LEFT(ULONG x, int n)  { return ((x << n) | (x >> (32-n))); }


// four constants t: 0-19, 20-39, 40-59, 60-79  ==> t/20 = index
ULONG K[4] = { 0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6 };

// four non-linear functions
ULONG f0 (ULONG X, ULONG Y, ULONG Z) { return ((X & Y) | (~X & Z)); }
ULONG f13(ULONG X, ULONG Y, ULONG Z) { return (X ^ Y ^ Z);          }
ULONG f2 (ULONG X, ULONG Y, ULONG Z) { return ((X&Y)|(X&Z)|(Y&Z));  }

typedef ULONG (*fun)(ULONG,ULONG,ULONG);

fun f[4] = {f0,f13,f2,f13};



//
//  In ABCDE are A,B,C,D,E which contains at the end the hashvalue.
//  In M must be a 512 bit block to calculate by 'sha_hashblock'
//
struct sha_struc 
{
    ULONG ABCDE[5], M[16];
    ULONG len, sum_length;  /*# added by max. */ 
};


//
//  This function calculates the 512 bit block given in M[16]
//  and adds it to the ABCDE variables.
//  At the end, the hash value of the whole message is in ABCDE,
//  it's a 160 bit value.
//  ==> This function had to be called for every 512 bit block
//      of the message.
//
void sha_hashblock(struct sha_struc *m) 
{
    ULONG   a=m->ABCDE[0], b=m->ABCDE[1], c=m->ABCDE[2], 
            d=m->ABCDE[3], e=m->ABCDE[4],
            W[80], *M = m->M, TEMP ;
    int t,i;

    // transforms M to W
    for(t= 0;t<16;++t) W[t] = BT_adaptL (M[t]);      /*# modified by max. */ 
    for(t=16;t<80;++t) W[t] = ROTATE_LEFT(W[t-3]^W[t-8]^W[t-14]^W[t-16],1);

    for(t=0;t<80;++t) 
    {
        i    = t/20;
        TEMP = ROTATE_LEFT(a,5) + f[i](b,c,d) + e + W[t] + K[i];
        e    = d;
        d    = c;
        c    = ROTATE_LEFT(b,30);
        b    = a;
        a    = TEMP;
    }

    m->ABCDE[0] += a; m->ABCDE[1] += b; m->ABCDE[2] += c;
    m->ABCDE[3] += d; m->ABCDE[4] += e;
}

//
//  This function calculates the hashvalue of the message 'mem' which
//  is 'length' bytes long, into 'hashvalue' which had to be 16 bytes
//  long (128 bits).
//
void sha_hash_mem(char *mem, ULONG length, ULONG *hashvalue) 
{

    struct sha_struc m;
    int len=length;
    int i;
    char *m_M = (char *) m.M;
 
    memcpy (m.ABCDE, magic, SHA_HASHVALUE_LEN);

    // this hashs all available 512 bit blocks
    while (len >= SHA_BYTEBLOCK) 
    {        // 64 = 512 bits divided by 8
      memcpy (m_M, mem, SHA_BYTEBLOCK);

      sha_hashblock (&m);
      len -= SHA_BYTEBLOCK;
      mem += SHA_BYTEBLOCK;
    }

    //
    // message-len to hash is not dividable through 64 (512 bits)
    // without a rest
    //

    memcpy (m_M, mem, len);  //  the 'restliche' bytes
    i = len;
    m_M [i++] = (char)128;       //  2**7 (1 0*)

    if (i == SHA_BYTEBLOCK) sha_hashblock (&m) , i = 0;

    if (i > SHA_BYTEBLOCK - 8) 
    {
      memset (&m_M [i], 0, SHA_BYTEBLOCK - i);
      sha_hashblock (&m);
      memset (m_M, 0, SHA_BYTEBLOCK - 8);
      i = 0;
    }
    else
    {
      memset (&m_M [i], 0, SHA_BYTEBLOCK - 8 - i);
    }

    m.M[14] = 0;
    m.M[15] = BT_adaptL (length << 3);        /*# modified by max. */ 
    sha_hashblock (&m);



    // copies the calculated hashvalue ABCDE into the array hashvalue,
    // which must be 20 bytes long or 160 bits.
    // memcpy (hashvalue, m.ABCDE, SHA_HASHVALUE_LEN);
    for (i = 0; i < 5; ++i) hashvalue [i] = BT_adaptL (m.ABCDE [i]); /*# modified by max. */ 

}

/*# added by max:

//  These functions calculates the hashvalue of messages 'mem' which
//  are 'length' bytes long, into 'hashvalue' which had to be 20 bytes
//  long (160 bits).
//
// the calculation is done in three steps with these 3 functions:

/**
void sha_hash_init ();
void sha_hash_update (char *mem, ULONG length);
void sha_hash_finish (ULONG *hashvalue);
  and a helper to adapt little-endian maschines: 
long BT_adaptL (long parm)  // out off tools.c

and a common used structure, the "context": 
**/


/*-----------------------------------------------------------------------*/
void sha_hash_init (struct sha_struc *p_ctx)
{
  memcpy (p_ctx->ABCDE, magic, SHA_HASHVALUE_LEN);
  p_ctx->len = 0;
  p_ctx->sum_length = 0;
} /*end- sha_hash_init */ 


/*-----------------------------------------------------------------------*/
void sha_hash_update (struct sha_struc *p_ctx, char *p_mem, ULONG p_length)
{
  p_ctx->sum_length += p_length;

  if (p_ctx->len + p_length < SHA_BYTEBLOCK)
  {
    memcpy ((char *)p_ctx->M + p_ctx->len, p_mem, p_length);
    p_ctx->len += p_length;
    return;
  }
  else
  {
    int i = SHA_BYTEBLOCK - p_ctx->len;

    memcpy ((char *)p_ctx->M + p_ctx->len, p_mem, i);
    sha_hashblock (p_ctx);
    p_mem   += i;
    p_ctx->len += p_length - SHA_BYTEBLOCK;
  }
  

  // this hashs all available 512 bit = 64 byte blocks
  while (p_ctx->len >= SHA_BYTEBLOCK) 
  {        
    memcpy (p_ctx->M, p_mem, SHA_BYTEBLOCK);
    sha_hashblock (p_ctx);
    p_ctx->len -= SHA_BYTEBLOCK;
    p_mem      += SHA_BYTEBLOCK;
  }

  //
  // message-len to hash is not dividable through 64 (512 bits)
  // without a rest
  //
  memcpy (p_ctx->M, p_mem, p_ctx->len);  //  the 'restliche' bytes

} /*end- sha_hash_update */ 

/*-----------------------------------------------------------------------*/
void sha_hash_finish (struct sha_struc *p_ctx, ULONG *p_hashvalue)
{
  int i;
  char *ctx_M = (char *) p_ctx->M;

  i = p_ctx->len;
  ctx_M [i++] = (char)128;       //  2**7 (1 0*)

  if (i == SHA_BYTEBLOCK) sha_hashblock (p_ctx) , i = 0;

  if (i > SHA_BYTEBLOCK - 8) 
  {
    memset (&ctx_M [i], 0, SHA_BYTEBLOCK - i);
    sha_hashblock (p_ctx);
    memset (ctx_M, 0, SHA_BYTEBLOCK - 8);
    i = 0;
  }
  else
  {
    memset (&ctx_M [i], 0, SHA_BYTEBLOCK - 8 - i);
  }

  p_ctx->M[14] = 0;
  p_ctx->M[15] = BT_adaptL (p_ctx->sum_length << 3);      /*# modified by max. */ 
  sha_hashblock (p_ctx);


  // copies the calculated hashvalue ABCDE into the array hashvalue,
  // which must be 20 bytes long or 160 bits.
  // memcpy (hashvalue, m.ABCDE, SHA_HASHVALUE_LEN);
  for (i = 0; i < 5; ++i) p_hashvalue [i] = BT_adaptL (p_ctx->ABCDE [i]); /*# modified by max. */ 

} /*end- sha_hash_finish */ 

/*****************************************************************/
/* a simple implementation of keyed hashing  */

DllExport Byte *SDXE_keyhash 
  ( Byte          *p_hash       /* out : the hash value       */ 
  , long           hlen         /* in  : length of output     */  
  , char          *p_str1       /* in  : the key              */ 
  , long           p_len1       /* in  : length of key        */ 
  , char          *p_str2       /* in  : the text             */ 
  , long           p_len2       /* in  : length of text       */ 
  )
{
  ULONG            hash [5];
  struct sha_struc ctx; 

  if (hlen == 0) return NULL;

  sha_hash_init (&ctx);
  if (p_str1 && p_len1) sha_hash_update (&ctx, p_str1, p_len1);
  if (p_str2 && p_len2) sha_hash_update (&ctx, p_str2, p_len2);
  sha_hash_finish (&ctx, hash);

  if (hash [0] == 0) hash [0] = 1;
  memcpy (p_hash, hash, hlen);
  return p_hash;


} /*end- SDXE_keyhash */ 

/*****************************************************************/
short SDXE_hash_length () {return SHA_HASHVALUE_LEN / 2;}
 

/*# end by max*/
