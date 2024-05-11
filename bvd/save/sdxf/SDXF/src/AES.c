/* doc-----------------------------------------------------------------------------
:Project.   SDXF
:Program.   AES.c
:PrType.    Library
:Title.     SDXF Interface for AES/Rijndael ciphering
:Version.   
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI-C
:Compiler.
:Created.   2001-06-21
:Changes.
   #43 2000-05-03 Word alignment for type 'Word'. Important for HP maschines 
                  (only non-FB routines!)
---------------------------------------------------------------------------------*/ 

//#define OLDCODE


#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "tools.h"
#include <time.h>

#include "SDXF.h"       /* imports SDXE_BLKSIZE    */ 
#include "rijndael.h"   /* imports Byte, u16, Word */ 

#define printf     BT_printf
#define HEX16(x)   BT_hexan((Byte *)x,16)

#define WORDBLOCKSIZE (SDXE_BLKSIZE / 4)

//typedef unsigned char  Byte;
typedef unsigned long  Word;

/*  The structure for key information: */
typedef struct 
{
  int          keyLen;               /* Length of the key                        */ 
  int          Nr;                   /* key-length-dependent number of rounds    */ 
  Word         IV [WORDBLOCKSIZE];   /* initial vector                        #43*/ 
  unsigned int rk [4*(MAXNR+1)];     /* key schedule                             */ 
} keyInstance;


/*  Function prototypes:  */
/*    length preserving ("normal") functions:  */
int blockEncrypt   (keyInstance *key, Byte *input, int inputLen);
int blockDecrypt   (keyInstance *key, Byte *input, int inputLen);
/*    fixed block functions:                   */
int blockEncryptFB (keyInstance *key, Byte *input, int inputLen);
int blockDecryptFB (keyInstance *key, Byte *input, int inputLen);
 
/************************************************************************/
/* AES interface for SDXF: */ 
#define EN_VERSION "AES alg-fst 3.0 "

long SDXE_AES 
  ( Tcrypt_mode  p_mode      /* encrypt, decrypt, SET_IV or GET_VERSION */ 
  , Byte        *p_buffer    /* content to encrypt  / IV                */ 
  , long         p_len       /* length of buffer                        */ 
  , char        *p_passw     /* encryption key                          */ 
  )
/************************************************************
results: length of output / SDXE_BLKSIZE / -1 when error
*************************************************************/
{
  /* globals: cipher object */
  static bool  s_blocking = false;                                  /*#28#30*/ 
  static const Word c_defIV [WORDBLOCKSIZE];                           /*#30*/ 
  
  keyInstance  m_keyInstance;
  Word        *IV_encP = (Word *)c_defIV, *IV_decP = (Word *)c_defIV; /*#30#43*/  
  int          ix;

  memset (&m_keyInstance, 0, sizeof m_keyInstance);

  switch (p_mode)
  {
    /*------------------------------------------------*/
  case  CM_ENCRYPT:       
    printf (":7AES---ENCRYPT: using IV: %s\n", HEX16 (IV_encP));

    if (IV_encP) memcpy (m_keyInstance.IV, IV_encP, SDXE_BLKSIZE);
      
    printf (":7AES---ENCRYPT: keysetup\n");
    m_keyInstance.Nr = 
      rijndaelKeySetupEnc (m_keyInstance.rk, p_passw, SDXE_BLKSIZE << 3);

    printf (":7AES---ENCRYPT: blockEncrypt blocking=%d \n", s_blocking);
    if (s_blocking)
      return blockEncryptFB (&m_keyInstance, p_buffer, p_len);         /*#28*/ 
    else
      return blockEncrypt   (&m_keyInstance, p_buffer, p_len);

    /*------------------------------------------------*/
  case  CM_DECRYPT:       
    printf (":7AES---DECRYPT: using IV: %s\n", HEX16 (IV_decP));

    if (IV_decP) memcpy (m_keyInstance.IV, IV_decP, SDXE_BLKSIZE);
      
    if (p_len < SDXE_BLKSIZE && ! s_blocking)
      m_keyInstance.Nr = 
        rijndaelKeySetupEnc (m_keyInstance.rk, p_passw, SDXE_BLKSIZE << 3);
    else
      m_keyInstance.Nr = 
        rijndaelKeySetupDec (m_keyInstance.rk, p_passw, SDXE_BLKSIZE << 3);

    if (s_blocking)
      return blockDecryptFB (&m_keyInstance, p_buffer, p_len);         /*#28*/ 
    else
      return blockDecrypt   (&m_keyInstance, p_buffer, p_len);

      /*------------------------------------------------*/
    case  CM_GET_BLOCKSIZE:         
      return s_blocking ? SDXE_BLKSIZE : 0;

      /*------------------------------------------------*/
    case  CM_SET_IV:          /* params: SET_IV, &IVenc, 0, &IVdec */  /*#13*/ 
      IV_encP = (Word *)p_buffer;
      IV_decP = (Word *)p_passw;

      printf (":7AES---SET_IV:    enc:%s\n", HEX16 (IV_encP));
      printf (":7                 dec:%s\n", HEX16 (IV_decP));

      return 0;
 
      /*------------------------------------------------*/
    case  CM_SHIFTnMODIFY_IV:  /* params: CM_SHIFTnMODIFY_IV, &IVenc, 0, &IVdec */ 
      IV_encP = (Word *)p_buffer;
      IV_decP = (Word *)p_passw;

      memcpy (IV_decP, IV_encP, SDXE_BLKSIZE);

      for (ix = 0; ix < SDXE_BLKSIZE; ++ix)  ++ *((Byte *)IV_encP + ix);    /*#43*/
      
      printf (":7AES---SHnMOD_IV: enc:%s\n", HEX16 (IV_encP));
      printf (":7                 dec:%s\n", HEX16 (IV_decP));

      return 0;

      /*------------------------------------------------*/
    case  CM_GET_VERSION:    /* params: CM_GET_VERSION, buffer, 0, NULL */ 
      strncpy (p_buffer, EN_VERSION, p_len);
      strncat (p_buffer, s_blocking ? "(blocked)" : "(variable)", p_len); /*#28*/ 
      return 0;

      /*------------------------------------------------*/
    case  CM_SET_BLOCKING:    /* params: CM_SET_BLOCKING, NULL, 0, NULL *#28*/ 
      s_blocking = true;      /* no multithread conflict */            /*#30*/ 
      return 0;

      /*------------------------------------------------*/
    default:
      return -1;
  }
} /*end- SDXE_AES */ 

#ifdef OLDCODE
/***********************************************************************/
int blockEncrypt (keyInstance *p_key, Byte *p_buffer, int p_bufferLen)
{
  int   i, numBlocks, lastBytes;
  Byte  block [SDXE_BLKSIZE];
  Byte *iv    = (Byte *)&p_key->IV;

  if (p_key == NULL || p_buffer == NULL || p_bufferLen <= 0) 
    return 0;  /* nothing to do */ 

  numBlocks = p_bufferLen / SDXE_BLKSIZE;
  lastBytes = p_bufferLen - numBlocks * SDXE_BLKSIZE;

  if (numBlocks == 0)   /* CTS not possible: XOR with enc IV */ 
  {
    rijndaelEncrypt (p_key->rk, p_key->Nr, iv, block);

    ((Word*)block) [0] ^= ((Word*)p_buffer) [0];
    ((Word*)block) [1] ^= ((Word*)p_buffer) [1];
    ((Word*)block) [2] ^= ((Word*)p_buffer) [2];
    ((Word*)block) [3] ^= ((Word*)p_buffer) [3];

    memcpy (p_buffer, block, lastBytes); 
    return p_bufferLen;
  }
 
   /*case MODE_CBC_CTS: chain block cipher with ciphertext stealing  */ 

  for (i = numBlocks; i > 0; i--) 
  {
    ((Word*)block) [0] = ((Word*)p_buffer) [0] ^ ((Word*)iv) [0];
    ((Word*)block) [1] = ((Word*)p_buffer) [1] ^ ((Word*)iv) [1];
    ((Word*)block) [2] = ((Word*)p_buffer) [2] ^ ((Word*)iv) [2];
    ((Word*)block) [3] = ((Word*)p_buffer) [3] ^ ((Word*)iv) [3];

    rijndaelEncrypt (p_key->rk, p_key->Nr, block, p_buffer);

    iv         = p_buffer;
    p_buffer  += SDXE_BLKSIZE;
  } /* next i */ 
 
  if (lastBytes == 0) return p_bufferLen;

  /* ciphertext stealing:  */ 

  memcpy (block,  p_buffer, lastBytes);                 /*  Pn  */ 
  memset (block + lastBytes, 0, SDXE_BLKSIZE - lastBytes);
  memcpy (p_buffer, iv, lastBytes);                     /*  Cn  */ 

  ((Word*)block) [0] ^= ((Word*)iv) [0];
  ((Word*)block) [1] ^= ((Word*)iv) [1];
  ((Word*)block) [2] ^= ((Word*)iv) [2];
  ((Word*)block) [3] ^= ((Word*)iv) [3];


  rijndaelEncrypt (p_key->rk, p_key->Nr, block, p_buffer-SDXE_BLKSIZE);
  return p_bufferLen;

} /*end- blockEncrypt */ 


#else   // new code:
/**********************************************************************/
int blockEncrypt (keyInstance *p_key, Byte *p_buffer, int p_bufferLen)
{
  int   i, w, numBlocks, lastBytes;
  Word  block  [WORDBLOCKSIZE];                                    /*#43*/
  Word  buffer [WORDBLOCKSIZE];                                    /*#43*/
  Word  iv     [WORDBLOCKSIZE];                                    /*#43*/
  /**old:  Word *iv  = (Word *)&p_key->IV;           */

  printf (":7AES---blockENCRYPT\n");
  if (p_key == NULL || p_buffer == NULL || p_bufferLen <= 0) 
    return 0;  /* nothing to do */ 

  memcpy (iv, p_key->IV, SDXE_BLKSIZE);
  numBlocks = p_bufferLen / SDXE_BLKSIZE;
  lastBytes = p_bufferLen - numBlocks * SDXE_BLKSIZE;

  printf (":7AES---blockENCRYPT nr blocks = %d\n", numBlocks);
  if (numBlocks == 0)   /* CTS not possible: XOR with enc IV */ 
  {
    printf (":7AES---ENCRYPT: rijndael #1\n");
    
    memcpy (buffer, p_buffer, SDXE_BLKSIZE);            /* word alignment #43*/

    rijndaelEncrypt (p_key->rk, p_key->Nr, (Byte *)iv, (Byte *)block);  /*#43*/    
    
    for ( w = 0; w < WORDBLOCKSIZE; ++w ) block [w] ^= buffer [w];      /*#43*/

    /***** old:
    ((Word*)block) [0] ^= ((Word*)p_buffer) [0];
    ((Word*)block) [1] ^= ((Word*)p_buffer) [1];
    ((Word*)block) [2] ^= ((Word*)p_buffer) [2];
    ((Word*)block) [3] ^= ((Word*)p_buffer) [3];
    ******/

    memcpy (p_buffer, block, lastBytes);                                /*#43*/
    return p_bufferLen;
  }

   /*case MODE_CBC_CTS: chain block cipher with ciphertext stealing  */ 

  for ( i = numBlocks; i > 0; i-- ) 
  {
    printf (":7AES---ENCRYPT: CBC\n");
    
    memcpy (buffer, p_buffer, SDXE_BLKSIZE);               /* word alignment #43*/
    for ( w = 0; w < WORDBLOCKSIZE; ++w ) block [w] = buffer [w] ^ iv [w]; /*#43*/

    /***** old:
    ((Word*)block) [0] = ((Word*)p_buffer) [0] ^ ((Word*)iv) [0];
    ((Word*)block) [1] = ((Word*)p_buffer) [1] ^ ((Word*)iv) [1];
    ((Word*)block) [2] = ((Word*)p_buffer) [2] ^ ((Word*)iv) [2];
    ((Word*)block) [3] = ((Word*)p_buffer) [3] ^ ((Word*)iv) [3];
    ******/

    printf (":7AES---ENCRYPT: rijndael #2\n");
    rijndaelEncrypt (p_key->rk, p_key->Nr, (Byte *)block, (Byte *)buffer);   /*#43*/

    memcpy (iv,       buffer, SDXE_BLKSIZE);                 /* word alignment #43*/
    memcpy (p_buffer, buffer, SDXE_BLKSIZE);                 /* word alignment #43*/
    p_buffer  += SDXE_BLKSIZE;
  } /* next i */ 

  if (lastBytes == 0) return p_bufferLen;

  /* ciphertext stealing:  */ 

  memcpy (block,  p_buffer, lastBytes);                 /*  Pn  */ 
  memset ((Byte *)block + lastBytes, 0, SDXE_BLKSIZE - lastBytes);
  memcpy (p_buffer, iv, lastBytes);                     /*  Cn  */ 
  
  for ( w = 0; w < WORDBLOCKSIZE; ++w ) block [w] ^= iv [w];                 /*#43*/


  /***** old:
  ((Word*)block) [0] ^= ((Word*)iv) [0];
  ((Word*)block) [1] ^= ((Word*)iv) [1];
  ((Word*)block) [2] ^= ((Word*)iv) [2];
  ((Word*)block) [3] ^= ((Word*)iv) [3];
  ******/

  printf (":7AES---ENCRYPT: rijndael #3\n");

  rijndaelEncrypt (p_key->rk, p_key->Nr, (Byte *)block, p_buffer - SDXE_BLKSIZE);
  return p_bufferLen;

} /*end- blockEncrypt */ 
#endif

#ifdef OLDCODE
/***********************************************************************/
int blockDecrypt (keyInstance *p_key, Byte *p_buffer, int p_bufferLen) 
{
  int   i, numBlocks;
  Byte  block [SDXE_BLKSIZE], wbuff [SDXE_BLKSIZE], Cn_2 [SDXE_BLKSIZE], Pn [SDXE_BLKSIZE];
  int   lastBytes;  
  Byte *outbuffer, *iv = (Byte *)&p_key->IV;


  if (p_key == NULL || p_buffer == NULL  || p_bufferLen <= 0) 
    return 0; /* nothing to do */ 

  numBlocks = p_bufferLen / SDXE_BLKSIZE;
  lastBytes = p_bufferLen - numBlocks * SDXE_BLKSIZE;

  if (numBlocks == 0)   /* CTS not possible: XOR with enc IV */ 
  {
    rijndaelEncrypt (p_key->rk, p_key->Nr, iv, block);

    ((Word*)block) [0] ^= ((Word*)p_buffer) [0];
    ((Word*)block) [1] ^= ((Word*)p_buffer) [1];
    ((Word*)block) [2] ^= ((Word*)p_buffer) [2];
    ((Word*)block) [3] ^= ((Word*)p_buffer) [3];

    memcpy (p_buffer, block, lastBytes); 
    return p_bufferLen;
  }

  /* case MODE_CBC_CTS: with ciphertext stealing  */ 
  if (lastBytes > 0) 
  {
    --numBlocks;
    memcpy (Cn_2, p_buffer + (numBlocks - 1) * SDXE_BLKSIZE, SDXE_BLKSIZE);
  }

  memcpy (wbuff, iv, SDXE_BLKSIZE);

  for (i = numBlocks; i > 0; i--) 
  {
    rijndaelDecrypt (p_key->rk, p_key->Nr, p_buffer, block);

    ((Word*)block) [0] ^= ((Word*)wbuff) [0];
    ((Word*)block) [1] ^= ((Word*)wbuff) [1];
    ((Word*)block) [2] ^= ((Word*)wbuff) [2];
    ((Word*)block) [3] ^= ((Word*)wbuff) [3];
 
    memcpy (wbuff,    p_buffer, SDXE_BLKSIZE);
    memcpy (p_buffer, block,    SDXE_BLKSIZE);

    p_buffer  += SDXE_BLKSIZE;
  }

  if (lastBytes == 0) return p_bufferLen;

  /* ciphertext stealing:  */ 

  rijndaelDecrypt (p_key->rk, p_key->Nr, p_buffer, block);

  outbuffer = p_buffer;
  p_buffer += SDXE_BLKSIZE;                           /* Cn   */ 
                                      /* outBuffer -> Pn-1 */ 
  i = lastBytes;
  memcpy (wbuff, p_buffer, i);
  memset (wbuff + i, 0, SDXE_BLKSIZE - i);            /* Cn, 0 */ 

  ((Word*)block) [0] ^= ((Word*)wbuff) [0];
  ((Word*)block) [1] ^= ((Word*)wbuff) [1]; 
  ((Word*)block) [2] ^= ((Word*)wbuff) [2];
  ((Word*)block) [3] ^= ((Word*)wbuff) [3];      /* Pn, C' */ 

  memcpy (Pn, block, SDXE_BLKSIZE);                 /* Pn     */ 
  memcpy (block, p_buffer, i);                   /* Cn, C' */ 
  memcpy (outbuffer + SDXE_BLKSIZE, Pn, i);         /* Pn     */ 

  rijndaelDecrypt (p_key->rk, p_key->Nr, block, outbuffer);

  p_buffer = (numBlocks > 0) ? Cn_2 : iv;        /* Cn-2   */ 

  ((Word*)outbuffer) [0] ^= ((Word*)p_buffer) [0];
  ((Word*)outbuffer) [1] ^= ((Word*)p_buffer) [1];
  ((Word*)outbuffer) [2] ^= ((Word*)p_buffer) [2];
  ((Word*)outbuffer) [3] ^= ((Word*)p_buffer) [3];

  return p_bufferLen;

} /*end- blockDecrypt */ 

#else   // new code:
/******************************************************************************************************/
int blockDecrypt (keyInstance *p_key, Byte *p_buffer, int p_bufferLen) 
{
  int   i, w, numBlocks, lastBytes;
  Word  buffer [WORDBLOCKSIZE];                                    /*#43*/
  Word  block  [WORDBLOCKSIZE];                                    /*#43*/
  Word  wbuff  [WORDBLOCKSIZE];                                    /*#43*/
  Word  Cn_2   [WORDBLOCKSIZE];
  Word  Pn     [WORDBLOCKSIZE];
  Word *iv     = p_key->IV;                                        /*#43*/
  Word *outbuffer;


  if (p_key == NULL || p_buffer == NULL  || p_bufferLen <= 0) 
    return 0; /* nothing to do */ 

  numBlocks = p_bufferLen / SDXE_BLKSIZE;
  lastBytes = p_bufferLen - numBlocks * SDXE_BLKSIZE;
 
  if (numBlocks == 0)   /* CTS not possible: XOR with enc IV */ 
  {
    rijndaelEncrypt (p_key->rk, p_key->Nr, (Byte *)iv, (Byte *)block);

    memcpy (buffer, p_buffer, SDXE_BLKSIZE);            /* word alignment #43*/

    for ( w = 0; w < WORDBLOCKSIZE; ++w ) block [w] ^= buffer [w];      /*#43*/

    /***** old:
    ((Word*)block) [0] ^= ((Word*)p_buffer) [0];
    ((Word*)block) [1] ^= ((Word*)p_buffer) [1];
    ((Word*)block) [2] ^= ((Word*)p_buffer) [2];
    ((Word*)block) [3] ^= ((Word*)p_buffer) [3];
    ******/

    memcpy (p_buffer, block, lastBytes); 
    return p_bufferLen;
  }

  /* case MODE_CBC_CTS: with ciphertext stealing  */ 
  if (lastBytes > 0) 
  {
    --numBlocks;
    memcpy (Cn_2, p_buffer + (numBlocks - 1) * SDXE_BLKSIZE, SDXE_BLKSIZE);
  }

  memcpy (wbuff, iv, SDXE_BLKSIZE);

  for (i = numBlocks; i > 0; i--) 
  {
    rijndaelDecrypt (p_key->rk, p_key->Nr, (Byte *)p_buffer, (Byte *)block);
    
    for ( w = 0; w < WORDBLOCKSIZE; ++w ) block [w] ^= wbuff [w];      /*#43*/

    /***** old:
    ((Word*)block) [0] ^= ((Word*)wbuff) [0];
    ((Word*)block) [1] ^= ((Word*)wbuff) [1];
    ((Word*)block) [2] ^= ((Word*)wbuff) [2];
    ((Word*)block) [3] ^= ((Word*)wbuff) [3];
    ******/
 
    memcpy (wbuff,    p_buffer, SDXE_BLKSIZE);
    memcpy (p_buffer, block,    SDXE_BLKSIZE);

    p_buffer  += SDXE_BLKSIZE;
  }

  if (lastBytes == 0) return p_bufferLen;

  /* ciphertext stealing:  */ 

  rijndaelDecrypt (p_key->rk, p_key->Nr, (Byte *)p_buffer, (Byte *)block);

  outbuffer = (Word *)p_buffer;
  p_buffer += SDXE_BLKSIZE;                                         /* Cn   */ 
                                                       /* outBuffer -> Pn-1 */ 
  i = lastBytes;
  memcpy (wbuff,             p_buffer, i);
  memset ((Byte *)wbuff + i, 0,        SDXE_BLKSIZE - i);          /* Cn, 0 */ 

  for ( w = 0; w < WORDBLOCKSIZE; ++w ) block [w] ^= wbuff [w];   /* Pn, C' */ 

  /***** old:
  ((Word*)block) [0] ^= ((Word*)wbuff) [0];
  ((Word*)block) [1] ^= ((Word*)wbuff) [1]; 
  ((Word*)block) [2] ^= ((Word*)wbuff) [2];
  ((Word*)block) [3] ^= ((Word*)wbuff) [3];     
  ******/

  memcpy (Pn,                        block,    SDXE_BLKSIZE);      /* Pn     */ 
  memcpy (block,                     p_buffer, i);                 /* Cn, C' */ 
//  memcpy (outbuffer + WORDBLOCKSIZE, Pn,       i);                 /* Pn     */ 
  memcpy ((Byte *)outbuffer + SDXE_BLKSIZE, Pn,       i);                 /* Pn     */ 

  rijndaelDecrypt (p_key->rk, p_key->Nr, (Byte *)block, (Byte *)outbuffer);

  p_buffer = (Byte *)((numBlocks > 0) ? Cn_2 : iv);               /* Cn-2   */ 

  memcpy (buffer, p_buffer,  SDXE_BLKSIZE);                            /*#43*/
  memcpy (wbuff,  outbuffer, SDXE_BLKSIZE);                            /*#43*/

  for ( w = 0; w < WORDBLOCKSIZE; ++w ) wbuff [w] ^= buffer [w]; 

  memcpy (outbuffer, wbuff, SDXE_BLKSIZE);                             /*#43*/

  /***** old:
  ((Word*)outbuffer) [0] ^= ((Word*)p_buffer) [0];
  ((Word*)outbuffer) [1] ^= ((Word*)p_buffer) [1];
  ((Word*)outbuffer) [2] ^= ((Word*)p_buffer) [2];
  ((Word*)outbuffer) [3] ^= ((Word*)p_buffer) [3];
  ******/

  return p_bufferLen;

} /*end- blockDecrypt */ 
#endif



/* blocked encrytion "classic":  */                                      /*#28*/ 

/***********************************************************************/
int blockEncryptFB (keyInstance *p_key, Byte *p_buffer, int p_bufferLen)
{
  int   i, numBlocks, lastBytes;
  Byte  block [SDXE_BLKSIZE];
  Byte *iv        = (Byte *)&p_key->IV;

  if (p_key == NULL || p_buffer == NULL || p_bufferLen <= 0) 
    return 0;  /* nothing to do */ 

  numBlocks = p_bufferLen / SDXE_BLKSIZE;  /* complete blocks */ 
  lastBytes = p_bufferLen - numBlocks * SDXE_BLKSIZE;


  /*case MODE_CBC: chain block cipher */ 

  for (i = numBlocks; i > 0; i--)  /* cond is tested before execution */ 
  {
    ((Word*)block) [0] = ((Word*)p_buffer) [0] ^ ((Word*)iv) [0];
    ((Word*)block) [1] = ((Word*)p_buffer) [1] ^ ((Word*)iv) [1];
    ((Word*)block) [2] = ((Word*)p_buffer) [2] ^ ((Word*)iv) [2];
    ((Word*)block) [3] = ((Word*)p_buffer) [3] ^ ((Word*)iv) [3];

    rijndaelEncrypt (p_key->rk, p_key->Nr, block, p_buffer);

    iv         = p_buffer;
    p_buffer  += SDXE_BLKSIZE;
  } /* next i */ 

  /* preparing the last block */ 

  {
    time_t  random   = time(NULL);  /* assuming time_t == long  */ 
    time_t *blockPtr = (time_t *)block; 
    int     ix, bdim = SDXE_BLKSIZE / sizeof (time_t);

    for (ix = 0; ix < bdim; ++ix) blockPtr [ix] = (random << (3 * ix)) + ix;
  }
  
  memcpy (block, p_buffer, lastBytes); 
  block [SDXE_BLKSIZE - 1] &= 0xF0;
  block [SDXE_BLKSIZE - 1] |= SDXE_BLKSIZE - lastBytes - 1;

  ((Word*)block) [0] = ((Word*)block) [0] ^ ((Word*)iv) [0];
  ((Word*)block) [1] = ((Word*)block) [1] ^ ((Word*)iv) [1];
  ((Word*)block) [2] = ((Word*)block) [2] ^ ((Word*)iv) [2];
  ((Word*)block) [3] = ((Word*)block) [3] ^ ((Word*)iv) [3];

  rijndaelEncrypt (p_key->rk, p_key->Nr, block, p_buffer);
  return ++numBlocks * SDXE_BLKSIZE;

} /*end- blockEncryptFB */ 


/***********************************************************************/
int blockDecryptFB (keyInstance *p_key, Byte *p_buffer, int p_bufferLen) 
{
  int   i, numBlocks;
  Byte  block [SDXE_BLKSIZE], wbuff [SDXE_BLKSIZE];
  int   lastBytes;  
  Byte *iv = (Byte *)&p_key->IV;


  if (p_key == NULL || p_buffer == NULL  || p_bufferLen <= 0) 
    return 0; /* nothing to do */ 

  numBlocks = p_bufferLen / SDXE_BLKSIZE;
  lastBytes = p_bufferLen - numBlocks * SDXE_BLKSIZE;

  if (lastBytes > 0) return -1; 
                   /* error ! - p_bufferLen must be a multiple of BLKSIZE */ 

  /* case MODE_CBC */ 

  memcpy (wbuff, iv, SDXE_BLKSIZE);

  for (i = numBlocks; i > 0; i--) 
  {
    rijndaelDecrypt (p_key->rk, p_key->Nr, p_buffer, block);

    ((Word*)block) [0] ^= ((Word*)wbuff) [0];
    ((Word*)block) [1] ^= ((Word*)wbuff) [1];
    ((Word*)block) [2] ^= ((Word*)wbuff) [2];
    ((Word*)block) [3] ^= ((Word*)wbuff) [3];
 
    memcpy (wbuff,    p_buffer, SDXE_BLKSIZE);
    memcpy (p_buffer, block,    SDXE_BLKSIZE);

    p_buffer  += SDXE_BLKSIZE;
  }

  
  /* last Bytes contains length of fill bytes - 1 */ 

  return numBlocks * SDXE_BLKSIZE - (0x0F & *--p_buffer) - 1;
  

} /*end- blockDecryptFB */ 

/*** end of source **************************************************/
