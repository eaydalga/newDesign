/* doc-----------------------------------------------------------------------------
:Project.   SDXF: Structured Data Exchange Format
:Program.   SDXF.c
:PrType.    Function Lib
:Title.     SDXF kernel
:Version.   4
:Author.    Max Wildgrube
:Copyright. Max Wildgrube
:Language.  ANSI-C

:Changes.
  #01 2003-10-28 corr SDX_init: first initialize, magic
---------------------------------------------------------------------------------*/

/* global settings */

#define SDX_VERSION  "SDXF V:1.0 R:34 B:"  __DATE__ " " _SYStext

#define CUTTING_TRAILING_BLANKS
#define ENCRYPT_BLOCKING  false  /* true: experimental, false: normal     */
#define NATIVE_KEY        0      /* 1 = passwords in chars of host format */

/* defaults: */
#define  DEF_FILLER         NIL
#define  DEF_KEY            "structured data exchange format"
#define  DEF_MAX_LEVEL      20
#define  DEF_MAX_ARRAY_DIM  10000


/* How to handle 1-Byte numerics in an array: */
#define Tiny signed char;
    /* ??? or unsigned char --- see R159 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define  printf BT_printf
//old: #include "#changes.h"   /* imports SDX_VERSION                                     #31*/

/***** in release mode:
#define  SDX_PRIVATE                      \
struct TPrivate                           \
{                                         \
    SDX_Chunk *newChunk;                  \
    ChunkID    oldChunkID;                \
    Byte       iflags;                    \
    short      arr_index;                 \
    long       workbufflen;               \
    Byte      *workbuffer;                \
                                          \
    struct     T_stack                    \
    {                                     \
      SDX_Chunk *chunk;                   \
      long       length, rest;            \
    } stack [];                           \
}
*******************/
// doesn't work:   #define  SDX_PRIVATE  struct TPrivate;
// and typedef TPrivate later

#include "SDXF.h"
#include "SDXtxt.h"
#include "float.h"
#include "tools.h"
#include "compress.h"
#include "transtab.h"


#define  LenLength         3
#define  ShortLength       LenLength
#define  COMPRTHRESHOLD    5
#define  CMP_HEADERLENGTH  (sizeof (long))
#define  ARRAYHEADERLENGTH 2

#define  ERROR_noparam          ":0SDXF: severe error: no param\n"
#define  ERROR_TRANS_TABLE      ":0SDX-ERROR: Translate Tables wrong!\n"
#define  ERROR_NO_MEM_TR_TABLE  ":0SDX-ERROR: No memory for Translate Tables!\n"
#define  ERROR_NO_ZLIB          ":0SDXF/compression: ZLIB not installed!\n"
#define  TEXT_INIT              ":4SDXF: Translate Tables initialized\n"

#define  DT(x)             (0xE0 & (x))>>5                                              /*#92*/


/* internal flags: */
#define  IFLAGS_encryption_pending   1
#define  IFLAGS_compression_pending  2
//#define  IFLAGS_creating             4



/* special internal Type as a header for decompressed structured SDX_Chunk */
/* must be: sizeof TptrChunk == sizeof SDX_Chunk == SDX_HEADERLENGTH                 */

typedef struct TptrChunk
{
  ChunkID    id;
  SDX_Chunk *ptr;

} TptrChunk;
#define LptrChunk sizeof (TptrChunk)

/*     Prototypes     */

static void setChunkLength (SDX_Chunk *p_chunk, long pLength);
static void getShortData   (SDX_Chunk *p_chunk, Byte *pstr);
static void setShortData   (SDX_Chunk *p_chunk, Byte *pstr, long plen);
bool SDX_getTablesFromFile (Byte **p_toNet, Byte **p_toHost);

static char g_defkey [SDX_MAXKEYLEN]; /* no multithread conflict */                     /*#30*/


static SDX_TOptions g_xoptions =
{ NULL                   /* Byte           *toHost;         */
, NULL                   /* Byte           *toNet;          */
, DEF_MAX_LEVEL          /* int             maxLevel;       */
, true                   /* int             translation;    */
, EN_DEFAULT_ROUTINE     /* TEncryptProc   *encryptProc;    */
, SDX_getTablesFromFile  /* TGetTablesProc *getTablesProc;  */
, NULL                   /* TCvtUTF8Proc   *cvtUTF8Proc;    */
, SDX_VERSION            /* version string                  */
, false                  /* int             adaptFTPto390;  */
, ENCRYPT_BLOCKING       /* int             encryptBlocking */
, DEF_MAX_ARRAY_DIM      /* int             max_array_dim   */
};

void SDX_dmy() {}  /* dummy for MVS */

/****************************************************************/
/*                                                              */
/*                        Help routines                         */
/*                                                              */
/****************************************************************/


/****************************************************************/
static void setChunkLength (SDX_Chunk *p_chunk, long p_length)
{
  Byte typ;
  Byte *arr;

  if ( ISIN (p_chunk->flags, SDX_FLAG_short) ) return;

  typ  = p_chunk->flags;         /* save flags */
  arr  = ((Byte *) p_chunk) + SDX_IDlen;

  p_length = BT_adaptL (p_length);
  memcpy (arr, &p_length, 4);
  p_chunk->flags = typ;          /* rebuild flags */

}/* end SetChunkLength */

/****************************************************************/
static long getChunkLength (SDX_Chunk *p_chunk)
{
  union TOL
  {
    long leng;
    struct
    {
       Byte flags;
       Byte dummy1, dummy2, dummy3;
    } longflags;
  } flagsORleng;

  Byte *arr;

  if ( ! p_chunk || ISIN (p_chunk->flags, SDX_FLAG_short))
    return 0;

  else
  {
    arr  = ((Byte *) p_chunk) + SDX_IDlen;
    memcpy (&(flagsORleng.leng), arr, 4);
    flagsORleng.longflags.flags = 0;

    return BT_adaptL (flagsORleng.leng);
  }/* end if */

}/* end getChunkLength */

/****************************************************************/
static long getDataLength (SDX_Chunk *p_chunk)
{
  long leng;

  if ( (DT (p_chunk->flags) == SDX_DT_numeric
     || DT (p_chunk->flags) == SDX_DT_float)
  && ! ISIN (p_chunk->flags, SDX_FLAG_array) )     return 0;                            /*#92*/

  if ( ISIN (p_chunk->flags, SDX_FLAG_short) )     return 3;

  if ( ISIN (p_chunk->flags, SDX_FLAG_compressed) )
  {
    long header;

    memcpy (&header, &p_chunk->data, CMP_HEADERLENGTH);
    memset (&header, 0, 1);

    return BT_adaptL (header);
  }

  /* regular chunk */
  leng = 0;
  memcpy ((Byte *)&leng + 1, (Byte *)p_chunk + SDX_IDlen + 1, 3);
  return BT_adaptL (leng);

}/* end getDataLength */

/****************************************************************/
static long getTotalLength (SDX_Chunk *p_chunk)
{
  long leng;

  if ( ISIN (p_chunk->flags, SDX_FLAG_short)) return SDX_HEADERLENGTH;

  leng = getChunkLength (p_chunk) + SDX_HEADERLENGTH;

  return leng;

}/* end getTotalLength */


/****************************************************************/
static void getShortData (SDX_Chunk *p_chunk, Byte *p_str)
{
  p_str [0] = p_chunk->length [0];
  p_str [1] = p_chunk->length [1];
  p_str [2] = p_chunk->length [2];

}/* end GetShortData */


/****************************************************************/
static short getArrayCount (SDX_Chunk *p_chunk)
{
  short count;

  if (! ISIN (p_chunk->flags, SDX_FLAG_array)) return 0;

  memcpy (&count, &p_chunk->data, 2);
  return BT_adaptS (count);

}/* end GetShortData */

/****************************************************************/
static void setShortData (SDX_Chunk *p_chunk, Byte *p_str, long p_length)
{
  p_chunk->length [0] = 0;
  p_chunk->length [1] = 0;
  p_chunk->length [2] = 0;

  switch (p_length) /* without any breaks! */
  {
    case 3: p_chunk->length [2] = p_str [2];
    case 2: p_chunk->length [1] = p_str [1];
    case 1: p_chunk->length [0] = p_str [0];
  }
} /* end SetShortData */

/****************************************************************/
static bool flagsOK (SDX_Chunk *p_chunk)                                                /*#92*/
{
  if ( DT(p_chunk->flags) == SDX_DT_inconsistent ) return false;
  if ( DT(p_chunk->flags) >  SDX_DT_implemented  ) return false;

  if ( ISIN(p_chunk->flags,  SDX_FLAG_reserved1) ) return false;

  if ( ISIN(p_chunk->flags,  SDX_FLAG_array )
     &&  DT(p_chunk->flags) == SDX_DT_structured
     ) return false;

  if ( ISIN(p_chunk->flags,  SDX_FLAG_short)
     &&  DT(p_chunk->flags) == SDX_DT_structured
     ) return false;

  if ( ISIN(p_chunk->flags,    SDX_FLAG_short)
     &&  DT(p_chunk->flags) == SDX_DT_float
     ) return false;

  return true;
} /*end- flagsOK */

/****************************************************************/
static void setDataType (SDX_obj *p_sdx)                                                /*#92*/
            /* returns dataType and encrypt flag of param structure */
{
  p_sdx->encrypt     = ISIN (p_sdx->currChunk->flags, SDX_FLAG_encrypted);
  p_sdx->dataType    = DT   (p_sdx->currChunk->flags);
}

/****************************************************************/
static long sdx_crypt (enum Tcrypt_mode p_mode, SDX_obj *p_sdx)                         /*#07*/
{
  long xchunkLength = 0;

  switch (p_mode)
  {
  case CM_ENCRYPT:
  case CM_DECRYPT:
    break;
  case CM_GET_BLOCKSIZE:
    return g_xoptions.encryptProc (p_mode, NULL, 0, NULL);
  default:
    return 0;
  }/*end-switch*/

  if (p_sdx == NULL) return 0;
  xchunkLength = getChunkLength (p_sdx->currChunk);

  if (!p_sdx->cryptkey) p_sdx->cryptkey = g_defkey;

  #if NATIVE_KEY
    if (p_sdx->cryptkey [0] > SDXE_KEYMARK)
    {
      BT_convert
        ( (Byte *)p_sdx->cryptkey
        , (Byte *)p_sdx->cryptkey
        , strlen (p_sdx->cryptkey)
        , g_xoptions.toNet
        );
      p_sdx->cryptkey [0] = SDXE_KEYMARK;  /* mark "converted to Net Format" */
    }
  #endif

  xchunkLength = g_xoptions.encryptProc
    ( p_mode
    , (Byte *)(p_sdx->currChunk) + SDX_HEADERLENGTH
    , xchunkLength
    , p_sdx->cryptkey
    );

  if (xchunkLength < 0)
  {
    p_sdx->ec = SDX_EC_crypterror;
    p_sdx->rc = SDX_RC_dataError;
    return xchunkLength;
  }

  switch (p_mode)
  {
  case CM_ENCRYPT:
  case CM_DECRYPT:
    if (! ISIN (p_sdx->currChunk->flags, SDX_FLAG_array))
      p_sdx->dataLength = xchunkLength;
      /* arrays: dataLength means length of array entry           */
      /*         total chunk data length = count * dataLength + 2 */
  }

  return xchunkLength;

} /*end- sdx_crypt */


/****************************************************************/
Byte *sdx_workBuffer (SDX_obj *p_sdx, long p_len)
{
  if (!p_sdx || !p_len) return NULL;
  if(p_sdx->magic != SDX_MAGIC)  goto newbuffer;

  if (p_len <= p_sdx->private_stuff->workbufflen)                                       /*#32*/
    return p_sdx->private_stuff->workbuffer;

  if (p_sdx->private_stuff->workbuffer)
    free (p_sdx->private_stuff->workbuffer);

newbuffer:
  /** which cretin had put in this code: ????
  p_sdx->dataType = SDX_OPT_CLEAR;
  SDX_init (p_sdx);
  *** end-cretin **/

  p_sdx->private_stuff->workbufflen = p_len + p_len / 2;
  return p_sdx->private_stuff->workbuffer
          = malloc (p_sdx->private_stuff->workbufflen);

}/* end-sdx_workBuffer */


/****************************************************************/
static long compress
  ( Byte *p_out        /* target (compressed including compr. header) */
  , Byte *p_in         /* input (uncompressed)                        */
  , long  p_inlen      /* in: uncompressed data length                */
  , Byte  p_ct         /* compression type                            */
  )
                       /* returns: >  0 compressed length     */
                       /*          == 0 could not compress    */
                       /*          <  0 error                 */
{
  Byte    *outbuff;
  long     header;
  long     outlen;
  int      level = 9, ws;
  int      rc = 0;

  if (p_inlen <= 0) return 0;

  ws = p_ct - 10 * SDX_CMPTYPE_DEF;
  if (ws >= 0 && ws <= 9)
  {
    level = ws;
    p_ct  = SDX_CMPTYPE_DEF;
  }

  header = BT_adaptL (p_inlen);
  memcpy (&header, &p_ct, 1);
  memcpy (p_out, &header, CMP_HEADERLENGTH);
  outbuff = p_out + CMP_HEADERLENGTH;

  switch (p_ct)
  {
    static bool s_further;           /*#30*/

    case SDX_CMPTYPE_RL1:
      outlen = RL1_compress (outbuff, p_in, p_inlen);
      if (p_inlen - outlen < CMP_THRESHOLD) outlen = -1;
      break;

#ifdef DEFLATE_IMPLEMENTED
    #pragma message ("DEFLATE_IMPLEMENTED")
    case SDX_CMPTYPE_DEF:
      outlen = p_inlen;
      rc = DEF_compress (outbuff, &outlen, p_in, p_inlen, level);
      if (rc == 0 && p_inlen - outlen < CMP_THRESHOLD) rc = 3;
      if (rc != 0) outlen = -1;
      if (rc == -99 && ! s_further)
      {
        s_further = true;
        printf (ERROR_NO_ZLIB);
      }
      break;
#endif

    default:
      return -CMP_RC_unknown_type;
  }

  if (outlen <= 0) return 0;

  return outlen + CMP_HEADERLENGTH;

} /*end- compress */

/****************************************************************/
static int decompress
  ( Byte *p_out     /* target (uncompressed)              */
  , long  p_outlen  /* length target buffer (maxlength)   */
  , Byte *p_in      /* input including compression header */
  , long  p_inlen   /* uncompressed data length           */
  )
                    /* returns return code SDX_EC...      */
{
  Byte    ct;
  bool    overflow = false;
  int     rc;
  long    outlen = 0;

  if (p_inlen <= 0) return SDX_RC_ok;

  ct = *p_in;

  p_in += CMP_HEADERLENGTH;

  if (p_outlen < p_inlen)
  {
    p_inlen = p_outlen;
    overflow = true;
  }

  switch (ct)
  {
    static bool further;

    case SDX_CMPTYPE_RL1:
      rc = RL1_decompress (p_out, p_in, p_inlen);
      break;

#ifdef DEFLATE_IMPLEMENTED
    case SDX_CMPTYPE_DEF:
      outlen = p_inlen;
      rc = DEF_decompress (p_out, &outlen, p_in, p_inlen);
      if (rc == -99 && ! further)
      {
        further = true;
        printf (ERROR_NO_ZLIB);
      }
      break;
#endif

    default:
      return CMP_RC_unknown_type;
  }

  if (overflow)
    return SDX_EC_overflow;
  else
    return rc;

} /*end- decompress */


/****************************************************************/
/*                                                              */
/*                         SDXF routines                        */
/*                                                              */
/****************************************************************/



/* Init; **************************************************/
void SDX_init (SDX_obj *p_sdx)

/* input:  container, dataType == processing type: SDX_OPT_OLD | _NEW | _CLEAR
   output: currChunk, chunkID, dataLength, remainingSize, dataType
           all others reset, except:
           maxLength
*/

{
  static bool s_initialized; /* no multithread conflict -- if it set early  */
                             /* in the main thread (or do a dummy-init)  #30*/
  ChunkID     id;

  BT_assertf (p_sdx != NULL, ERROR_noparam);
  p_sdx->function = "init";

  switch (p_sdx->dataType)
  {
    case SDX_OPT_NEW:
    case SDX_OPT_OLD:
      break;

    case SDX_OPT_CLEAR:
      p_sdx->magic         = 0;
      p_sdx->container     = NULL;
      p_sdx->bufferSize    = 0;
      p_sdx->cryptkey      = NULL;
      p_sdx->currChunk     = NULL;
      p_sdx->data          = NULL;
      p_sdx->dataLength    = 0;
      p_sdx->level         = 0;
      p_sdx->dataType      = 0;
      p_sdx->encrypt       = false;
      p_sdx->compression   = 00;
      p_sdx->chunkID       = SDX_blankID;
      p_sdx->remainingSize = 0;
      p_sdx->count         = 0;
      p_sdx->value         = 0;
      p_sdx->fvalue        = 0;
      p_sdx->ec            = SDX_EC_ok;
      p_sdx->rc            = SDX_RC_ok;

      p_sdx->private_stuff   = NULL;                                                    /*#32*/
      break;

    default:
      p_sdx->ec = SDX_EC_initError;
      p_sdx->rc = SDX_RC_parameterError;
      return;
  }

  if (! s_initialized)
  {
    s_initialized = true;                                                               /*#30*/

    /*  initial set globals  -- no multithread conflict */
    if (!g_xoptions.maxLevel)       g_xoptions.maxLevel      = DEF_MAX_LEVEL;
    if (!g_xoptions.max_array_dim)  g_xoptions.max_array_dim = DEF_MAX_ARRAY_DIM;
    if (g_defkey [0] == 0)          strncpy (g_defkey, DEF_KEY, SDX_MAXKEYLEN);

    if (g_xoptions.encryptBlocking)
      g_xoptions.encryptProc (CM_SET_BLOCKING, NULL, 0, NULL);

    if (g_xoptions.translation)
    {
      if (! g_xoptions.toNet && ! g_xoptions.toHost)                                    /*#96*/
      {
        bool loaded = false;

        if (g_xoptions.getTablesProc)
          loaded = g_xoptions.getTablesProc
                     (&g_xoptions.toNet, &g_xoptions.toHost);

        if (! loaded)
          TT_GetTransTables (&g_xoptions.toNet, &g_xoptions.toHost);

      } /* no tables defined */

      if       (g_xoptions.toNet && ! g_xoptions.toHost)
        g_xoptions.toHost = BT_invTab (g_xoptions.toNet);
      else if  (! g_xoptions.toNet && g_xoptions.toHost)
        g_xoptions.toNet  = BT_invTab (g_xoptions.toHost);

    } /* options.translation set */
  } /*end- initialized */

  if (p_sdx->magic != SDX_MAGIC)
  {
    p_sdx->magic = SDX_MAGIC;

    p_sdx->private_stuff = malloc (sizeof (*(p_sdx->private_stuff))                     /*#32*/
                            + g_xoptions.maxLevel * sizeof (*(p_sdx->private_stuff->stack)));

    p_sdx->private_stuff->iflags      = NIL;                                            /*#32*/
    p_sdx->private_stuff->oldChunkID  = SDX_blankID;                                    /*#32*/
    p_sdx->private_stuff->newChunk    = NULL;

												//    p_sdx->private_stuff->newChunk    = (SDX_Chunk *) ((Byte *)(p_sdx->currChunk)     /*#32*/
												//                                        + p_sdx->dataLength + SDX_HEADERLENGTH);
    p_sdx->private_stuff->workbuffer  = NULL;                                           /*#32*/
    p_sdx->private_stuff->workbufflen = 0;                                              /*#32*/
    p_sdx->arr_index                  = 0;

                        //    p_sdx->private_stuff->stack = malloc (g_xoptions.maxLevel * sizeof (*(p_sdx->private_stuff->stack)));

                                    #ifdef old                                          /*#32*/
                                        if ( p_sdx->private_stuff->stack == NULL ) /* obsolete by mymalloc */
                                        {
                                          p_sdx->ec    = SDX_EC_noMemory;
                                          p_sdx->rc    = SDX_RC_noMemory;
                                          return;
                                        }
                                    #endif
    memset (p_sdx->private_stuff->stack, 0,                                             /*#32*/
            g_xoptions.maxLevel * sizeof (*(p_sdx->private_stuff->stack)));


    if (p_sdx->dataType == SDX_OPT_NEW) p_sdx->cryptkey = NULL;

  }/* end-if magic */

  if (p_sdx->dataType == SDX_OPT_NEW)
  {
    if (!p_sdx->cryptkey)    p_sdx->cryptkey = g_defkey;

    if ( p_sdx->bufferSize < SDX_HEADERLENGTH )
    {
      p_sdx->ec = SDX_EC_error;
      p_sdx->rc = SDX_RC_parameterError;
      return;
    }

    if ( !p_sdx->container )
    {
      p_sdx->container = malloc (p_sdx->bufferSize);
      if ( !p_sdx->container )
      {
        p_sdx->ec = SDX_EC_noMemory;
        p_sdx->rc = SDX_RC_noMemory;
        return;
      }
    }

    p_sdx->currChunk     = NULL;
    p_sdx->data          = NULL;
    p_sdx->dataLength    = 0;
    p_sdx->level         = 0;
    p_sdx->dataType      = 0;
    p_sdx->encrypt       = false;
    p_sdx->compression   = 00;
    p_sdx->chunkID       = SDX_blankID;
    p_sdx->remainingSize = p_sdx->bufferSize;

    if (p_sdx->private_stuff)                                                           /*#32*/
    {
      p_sdx->private_stuff->newChunk  = NULL;
      p_sdx->private_stuff->iflags    = NIL;
      p_sdx->arr_index                = 0;
    }

  } /*end-then NEW */
  else
  {
    /* initialize param-struct for OLD chunk */
    if ( !p_sdx->container )
    {
      p_sdx->ec = SDX_EC_error;
      p_sdx->rc = SDX_RC_parameterError;
      return;
    }

    p_sdx->currChunk   = (SDX_Chunk *) p_sdx->container;
    p_sdx->data        = NULL;

    memcpy (&id, &p_sdx->currChunk->chunkID, SDX_IDlen);                                /*#95*/
    p_sdx->chunkID     = BT_adaptS (id);

    p_sdx->dataLength  = getChunkLength (p_sdx->currChunk);
    p_sdx->level       = 0;
    p_sdx->compression = 00;
    p_sdx->filler      = DEF_FILLER;
    p_sdx->encrypt     = ISIN (p_sdx->currChunk->flags, SDX_FLAG_encrypted);

    if (p_sdx->private_stuff)
    {
      p_sdx->private_stuff->iflags      = NIL;                                          /*#32*/
      p_sdx->private_stuff->oldChunkID  = SDX_blankID;                                  /*#32*/
      p_sdx->private_stuff->newChunk    = (SDX_Chunk *) ((Byte *)(p_sdx->currChunk)     /*#32*/
                                          + p_sdx->dataLength + SDX_HEADERLENGTH);
    }

    if (! flagsOK (p_sdx->currChunk))
    {
      p_sdx->ec = SDX_EC_not_consistent;
      p_sdx->rc = SDX_RC_dataError;
      return;
    }

    setDataType (p_sdx);
  } /*end-if OLD / NEW */

  /* common for NEW and OLD: */

  p_sdx->count       = 0;
  p_sdx->value       = 0;
  p_sdx->fvalue      = 0;
  p_sdx->ec          = SDX_EC_ok;
  p_sdx->rc          = SDX_RC_ok;

} /* end SDX_init */

/* Enter; ********************************************************/
void SDX_enter (SDX_obj *p_sdx)

/* input : (currChunk)
   output: chunkID, dataLength, dataType, currChunk, level
*/

{
  short lev;
  long  oldChunkLength;
  long  xchunkLength;
  ChunkID  id;

  BT_assertf (p_sdx != NULL, ERROR_noparam);
  if (p_sdx->rc > SDX_RC_warning) return;  /* fall thru */

  p_sdx->function = "enter";

  lev = p_sdx->level;

  if (p_sdx->magic != SDX_MAGIC)
  {
    p_sdx->ec = SDX_EC_magicError;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }
  if (p_sdx->currChunk == NULL)
  {
    p_sdx->ec = SDX_EC_paramMissing;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }

  if (! flagsOK (p_sdx->currChunk))
  {
    p_sdx->ec = SDX_EC_not_consistent;
    p_sdx->rc = SDX_RC_dataError;
    return;
  }
  if (DT (p_sdx->currChunk->flags) != SDX_DT_structured)                                /*#92*/
  {
    p_sdx->ec = SDX_EC_not_structured;
    p_sdx->rc = SDX_RC_illegalOperation;
    return;
  }
  if ( lev >= g_xoptions.maxLevel )
  {
    p_sdx->ec = SDX_EC_levelOvflw;
    p_sdx->rc = SDX_RC_dataError;
    return;
  }

  p_sdx->private_stuff->oldChunkID = SDX_blankID;
  p_sdx->arr_index                 = 0;

  xchunkLength = getChunkLength (p_sdx->currChunk);

  if ( xchunkLength == 0 )
  {
    p_sdx->ec = SDX_EC_end_of_data;
    p_sdx->rc = SDX_RC_warning;
    return;
  }

  if (ISIN (p_sdx->currChunk->flags, SDX_FLAG_encrypted) )
  {                /* ------------------------------  */
    long reallength = sdx_crypt (CM_DECRYPT, p_sdx);                                    /*#07*/
                   /* ------------------------------  */
    long diff       = xchunkLength - reallength;
    Byte *chunkdata = &p_sdx->currChunk->data;
    //BT_hexedit (0, (Byte *)(p_sdx->currChunk), 10, 0);       //test

    if (reallength < 0)
    {
      p_sdx->ec = SDX_EC_crypterror;
      p_sdx->rc = SDX_RC_dataError;
      return;
    }
                  /* ----------------------------------------------  */
    if (g_xoptions.encryptProc (CM_GET_BLOCKSIZE, NULL, 0, NULL) > 0)
                  /* ----------------------------------------------  */
    {
      memset (chunkdata + reallength, 0, diff);
      if (diff < SDX_HEADERLENGTH)
      {
        chunkdata [reallength] = (Byte)diff;
      }
      else
      {
        chunkdata [reallength + 2] = 0x40;  /* DT binary */
        chunkdata [reallength + 5] = (Byte)diff - SDX_HEADERLENGTH;
      }
    } /*end-if blocked ciphering */

    if (lev == 0)
    {
      setChunkLength (p_sdx->currChunk, reallength);
      xchunkLength = reallength;
    }

    EXCL (p_sdx->currChunk->flags, SDX_FLAG_encrypted);
    SDX_check (p_sdx);
    if (p_sdx->rc > 0) return;
  }/*end-if encrypted */

  p_sdx->level = lev + 1;


  if (ISIN (p_sdx->currChunk->flags, SDX_FLAG_compressed) )
  {
    long datalen = getDataLength (p_sdx->currChunk);
    Byte *wbuff  = sdx_workBuffer (p_sdx, datalen + LptrChunk + SDX_HEADERLENGTH);

    ((struct TptrChunk *) wbuff)->id  = SDX_blankID;
    ((struct TptrChunk *) wbuff)->ptr = p_sdx->currChunk;

    memcpy (wbuff + LptrChunk, p_sdx->currChunk, SDX_HEADERLENGTH);

    if (SDX_RC_ok <
      decompress
        ( wbuff + LptrChunk + SDX_HEADERLENGTH
        , datalen
        , &p_sdx->currChunk->data
        , datalen
        )
      )
    {
      p_sdx->ec = SDX_EC_comprerr;
      p_sdx->rc = SDX_RC_dataError;
      return;
    }

    p_sdx->currChunk   = (SDX_Chunk *)(wbuff + LptrChunk);
    xchunkLength = datalen;
    setChunkLength (p_sdx->currChunk, datalen);
    EXCL (p_sdx->currChunk->flags, SDX_FLAG_compressed);
    SDX_check (p_sdx);
    if (p_sdx->rc > 0) return;
  }/*end-if compressed  */

  p_sdx->private_stuff->stack [lev].chunk  = p_sdx->currChunk;
  p_sdx->private_stuff->stack [lev].rest   = xchunkLength;
  p_sdx->private_stuff->stack [lev].length = xchunkLength;

  /* jump to first chunk in structure: */

  p_sdx->currChunk = (SDX_Chunk *)((Byte *)(p_sdx->currChunk) + SDX_HEADERLENGTH);

  oldChunkLength = xchunkLength;
  xchunkLength   = getChunkLength (p_sdx->currChunk);

  if (xchunkLength > oldChunkLength - SDX_HEADERLENGTH)
  {
    p_sdx->ec = SDX_EC_not_consistent;
    p_sdx->rc = SDX_RC_dataError;
    return;
  }

  p_sdx->dataLength = getDataLength (p_sdx->currChunk);

  memcpy (&id, &p_sdx->currChunk->chunkID, SDX_IDlen);                                  /*#95*/
  p_sdx->chunkID  = BT_adaptS (id);

  setDataType (p_sdx);
  p_sdx->count = getArrayCount (p_sdx->currChunk);

  p_sdx->ec = SDX_EC_ok;
  p_sdx->rc = SDX_RC_ok;

} /* end SDX_enter */

/* Leave; **********************************************************/
void SDX_leave (SDX_obj *p_sdx)

/* output: chunkID, chunkLength, dataLength, dataType, currChunk, level
*/

{
  int     ix;
  bool    ignoreFlags = false, isOK;
  long    xchunkLength, diff = 0;
  ChunkID id;
  struct  T_stack *stackE;


  BT_assertf (p_sdx != NULL, ERROR_noparam);
  if (p_sdx->rc > SDX_RC_warning) return;  /* fall thru */

  p_sdx->function = "leave";

  if (p_sdx->magic != SDX_MAGIC)
  {
    p_sdx->ec = SDX_EC_magicError;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }
  if ( p_sdx->level <= 0 )
  {
    p_sdx->ec = SDX_EC_error;
    p_sdx->rc = SDX_RC_illegalOperation;
    return;
  }


  ix = p_sdx->level - 1;
  p_sdx->currChunk = (p_sdx->private_stuff->stack [ix]).chunk;

  if ( p_sdx->currChunk == NULL )
  {
    p_sdx->ec = SDX_EC_error;
    p_sdx->rc = SDX_RC_programError;
    return;
  }

  /* checking if curr chunk lies in decompressed work buffer */
  memcpy (&id, &p_sdx->currChunk->chunkID, SDX_IDlen);                                  /*#95*/
  if (id == SDX_blankID)                                                                /*#95*/
  {
    p_sdx->currChunk = ((struct TptrChunk *)p_sdx->currChunk)->ptr;
    memcpy (&id, &p_sdx->currChunk->chunkID, SDX_IDlen);                                /*#95*/
    ignoreFlags = true;
  }

  p_sdx->private_stuff->oldChunkID = SDX_blankID;
  p_sdx->arr_index                 = 0;

  p_sdx->chunkID    = BT_adaptS (id);                                                   /*#95*/
  xchunkLength      = getChunkLength (p_sdx->currChunk);
  p_sdx->dataType   = SDX_DT_structured;

  --(p_sdx->level);

  if (ignoreFlags)
  {
    SETDT (p_sdx->currChunk->flags, SDX_DT_structured);                                 /*#92*/
    p_sdx->dataLength = getDataLength (p_sdx->currChunk);
    p_sdx->ec         = SDX_EC_ok;
    p_sdx->rc         = SDX_RC_ok;
    return;
  }

  /* checking flags in create-structured-chunk mode */

  p_sdx->dataLength = getChunkLength (p_sdx->currChunk);                                /*#11*/
  isOK = true;

  if (ISIN (p_sdx->currChunk->flags, SDX_FLAG_compressed) )
  {
    long  wbufflen = xchunkLength + CMP_HEADERLENGTH;
    Byte *wbuff = sdx_workBuffer (p_sdx, wbufflen);

    EXCL (p_sdx->private_stuff->iflags, IFLAGS_compression_pending);
    printf (":6SDXF - do the compression\n");

    if (0 <
      (wbufflen
        = compress
            ( wbuff
            , &p_sdx->currChunk->data
            , xchunkLength
            , p_sdx->compression                                                        //R198
            )
      ))
    {
      diff = - (xchunkLength - wbufflen);
      memcpy (&p_sdx->currChunk->data, wbuff, wbufflen);
      setChunkLength (p_sdx->currChunk, wbufflen);                                      /*#11*/
      xchunkLength = wbufflen;                                                          /*#11*/
    }
    else
    {
      EXCL (p_sdx->currChunk->flags, SDX_FLAG_compressed);
      isOK = (wbufflen == 0);
      printf (":6SDXF - compression not successful / sensible. returned length=%d\n", wbufflen);
    }

  }/*end-if compression */

  if (ISIN (p_sdx->currChunk->flags, SDX_FLAG_encrypted) )
  {                  /* ---------------------------  */
    long cryptlength = sdx_crypt (CM_ENCRYPT, p_sdx);                                  /*#07(*/
                     /* ---------------------------  */
    if (cryptlength < 0)
    {
      p_sdx->ec = SDX_EC_crypterror;
      p_sdx->rc = SDX_RC_dataError;
      return;
    }

    diff += cryptlength - xchunkLength;
    EXCL (p_sdx->private_stuff->iflags, IFLAGS_encryption_pending);
  }

  p_sdx->remainingSize -= diff;
  p_sdx->private_stuff->newChunk
    = (SDX_Chunk *)((Byte *)(p_sdx->private_stuff->newChunk) + diff);

  stackE = &p_sdx->private_stuff->stack [p_sdx->level];
  stackE->length += diff;
  setChunkLength (stackE->chunk, stackE->length);                                   /*#11#07)*/

  if (p_sdx->level > 0 && DT(stackE->chunk->flags) == SDX_DT_inconsistent)
  {
    long leng = getChunkLength (stackE->chunk);
    stackE = &p_sdx->private_stuff->stack [p_sdx->level - 1];
    stackE->length += leng;
    setChunkLength (stackE->chunk, leng + getChunkLength (stackE->chunk));
  }


  SETDT (p_sdx->currChunk->flags, SDX_DT_structured);                                   /*#92*/

  if (isOK)
  {
    p_sdx->ec    = SDX_EC_ok;
    p_sdx->rc    = SDX_RC_ok;
  }
  else
  {
    p_sdx->ec    = SDX_EC_comprerr;
    p_sdx->rc    = SDX_RC_ok;
}

}/* end SDX_leave */

/* Next; ***********************************************************/
void SDX_next (SDX_obj *p_sdx)

/* input : (currChunk)
   output: chunkID, dataLength, currChunk, level, dataType, count
*/

{
  int     ix;
  long    totalLength, rest;
  ChunkID id;

  BT_assertf (p_sdx != NULL, ERROR_noparam);
  if (p_sdx->rc > SDX_RC_warning) return;  /* fall thru */

  p_sdx->function = "next";

  if (p_sdx->magic != SDX_MAGIC)
  {
    p_sdx->ec = SDX_EC_magicError;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }
  if ( p_sdx->level <= 0 )
  {
    p_sdx->ec = SDX_EC_error;
    p_sdx->rc = SDX_RC_illegalOperation;
    return;
  }

  ix = p_sdx->level - 1;
  p_sdx->count = 0;

  totalLength = getTotalLength (p_sdx->currChunk);

  (p_sdx->private_stuff->stack [ix]).rest -= totalLength;
  rest = p_sdx->private_stuff->stack [ix].rest;

  if (rest < 0)
  {
    p_sdx->ec = SDX_EC_error;
    p_sdx->rc = SDX_RC_dataError;
    return;
  }

  if (rest < SDX_HEADERLENGTH)
  {
    if (rest > 0)
    {
      Byte *len = (Byte *)(p_sdx->currChunk) + totalLength;
      if (rest != *len)
      {
        p_sdx->ec = SDX_EC_not_consistent;
        p_sdx->rc = SDX_RC_dataError;
        return;
      }
    }

    SDX_leave (p_sdx);
    if ( p_sdx->rc == SDX_RC_ok )
    {
      p_sdx->ec = SDX_EC_end_of_data;
      p_sdx->rc = SDX_RC_failed;
    }
    return;
  }

  p_sdx->currChunk = (SDX_Chunk *)((Byte *)(p_sdx->currChunk) + totalLength);

  if (! flagsOK (p_sdx->currChunk))
  {
    p_sdx->ec = SDX_EC_not_consistent;
    p_sdx->rc = SDX_RC_dataError;
    return;
  }

  memcpy (&id, &p_sdx->currChunk->chunkID, SDX_IDlen);                                  /*#95*/
  p_sdx->chunkID    = BT_adaptS (id);

  if (p_sdx->chunkID == SDX_blankID)
  {
    SDX_next (p_sdx);
    return;
  }

  p_sdx->dataLength = getDataLength (p_sdx->currChunk);
  p_sdx->encrypt    = ISIN (p_sdx->currChunk->flags, SDX_FLAG_encrypted);
  p_sdx->count      = getArrayCount (p_sdx->currChunk);

  setDataType (p_sdx);

  p_sdx->ec = SDX_EC_ok;
  p_sdx->rc = SDX_RC_ok;

} /*end SDX_next */

/****************************************************************/
void SDX_select (SDX_obj *p_sdx)

/* input:  chunkId,
   output: chunkLength, dataLength, dataType, currChunk, level
*/

{
  int        ix;
  long       totalLength;
  ChunkID    searchChunkID, currchunkID, id;
  SDX_Chunk *stackChunk, *nextChunk, *currChunk;
  long       xchunkLength = getChunkLength (p_sdx->currChunk);

  BT_assertf (p_sdx != NULL, ERROR_noparam);
  if (p_sdx->rc > SDX_RC_warning) return;  /* fall thru */

  p_sdx->function = "select";

  if (p_sdx->magic != SDX_MAGIC)
  {
    p_sdx->ec = SDX_EC_magicError;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }
  if ( p_sdx->level <= 0 )
  {
    p_sdx->ec = SDX_EC_error;
    p_sdx->rc = SDX_RC_illegalOperation;
    return;
  }

  ix = p_sdx->level - 1;
  stackChunk    = (p_sdx->private_stuff->stack [ix]).chunk;
  searchChunkID = p_sdx->chunkID;


  if ( searchChunkID != p_sdx->private_stuff->oldChunkID)
  {
    /* points to the first SDX_Chunk in structure: */
    nextChunk = (SDX_Chunk *)((Byte *)stackChunk + SDX_HEADERLENGTH);
    (p_sdx->private_stuff->stack [ix]).rest = getChunkLength (stackChunk); /* reset stack */
    p_sdx->private_stuff->oldChunkID       = searchChunkID;
  }
  else
  {
    currChunk    = p_sdx->currChunk;
    totalLength  = getTotalLength (currChunk);
    nextChunk    = (SDX_Chunk *)((Byte *)(currChunk) + totalLength);
  }
  /* end if */

  do /* search for chunk */
  {
    currChunk = nextChunk;

    if (! flagsOK (currChunk))
    {
      p_sdx->ec = SDX_EC_not_consistent;
      p_sdx->rc = SDX_RC_dataError;
      return;
    }

    totalLength = getTotalLength (currChunk);

    (p_sdx->private_stuff->stack [ix]).rest -= totalLength;

    if ( p_sdx->private_stuff->stack [ix].rest < 0 )
    {
      p_sdx->ec = SDX_EC_error;
      p_sdx->rc = SDX_RC_dataError;
      return;
    }

    if ( p_sdx->private_stuff->stack [ix].rest == 0 )
      nextChunk = NULL;
    else
      nextChunk = (SDX_Chunk *)((Byte *)(currChunk) + totalLength);

    memcpy (&id, &currChunk->chunkID, SDX_IDlen);                                       /*#95*/
    currchunkID = BT_adaptS (id);

    if (currchunkID == searchChunkID )
    {
      p_sdx->currChunk  = currChunk;
      xchunkLength      = getChunkLength (currChunk);
      p_sdx->dataLength = getDataLength (p_sdx->currChunk);
      p_sdx->count      = getArrayCount (p_sdx->currChunk);

      setDataType (p_sdx);

      p_sdx->ec = SDX_EC_ok;
      p_sdx->rc = SDX_RC_ok;

      if ( nextChunk == NULL )
        (p_sdx->private_stuff->stack [ix]).rest = getChunkLength (stackChunk); /* reset stack */

      return;
    } /*end-if*/

  } while ( nextChunk != NULL );
  /* not found */

  (p_sdx->private_stuff->stack [ix]).rest = getChunkLength (stackChunk); /* reset stack */
  p_sdx->private_stuff->oldChunkID= SDX_blankID;

  p_sdx->ec = SDX_EC_notFound;
  p_sdx->rc = SDX_RC_failed;

} /* end Select */

/* Extract **************************************************************/

static void extract_numeric      (SDX_obj*, Byte *source, long p_length);
static void extract_charbin      (SDX_obj*, Byte *source, long p_length);
static void extract_step_by_step (SDX_obj*);

/************************************************************************/
void SDX_extract (SDX_obj *p_sdx)

/* input : data, maxLength, (from next, enter, init: dataType), count
   output: dataLength, value, count
*/

{
  Byte   flags;
  long   xchunkLength;
  Byte  *uncr_data;

  /*----------  verify parameters  -------------------------*/

  BT_assertf (p_sdx != NULL, ERROR_noparam);
  if (p_sdx->rc > SDX_RC_warning) return;  /* fall thru */

  p_sdx->function = "extract";

  if (p_sdx->magic != SDX_MAGIC)
  {
    p_sdx->ec = SDX_EC_magicError;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }

  if ( (p_sdx->level     <  0   )
    || (p_sdx->currChunk  == NULL) )
  {
    p_sdx->ec = SDX_EC_error;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }

  if ( (p_sdx->dataType != SDX_DT_numeric) && (p_sdx->dataType != SDX_DT_float)
    && (p_sdx->data == NULL || p_sdx->maxLength <= 0 ) )
  {
    p_sdx->ec = SDX_EC_wrongLength;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }

  flags = p_sdx->currChunk->flags;

  if ( ISIN (flags, SDX_FLAG_array) && p_sdx->count == -1 )
  {
   /********************/
    extract_step_by_step (p_sdx);
   /********************/
    return;
  }

  xchunkLength = getChunkLength (p_sdx->currChunk);

  /* special extract for structured chunk: copy whole chunk as is: */
  if ( p_sdx->dataType == SDX_DT_structured )
  {
    long totallen = getChunkLength (p_sdx->currChunk) + SDX_HEADERLENGTH;

    if (totallen > p_sdx->maxLength)
    {
      p_sdx->ec = SDX_EC_dataCutted;
      p_sdx->rc = SDX_RC_failed;
      return;
    }

    memcpy (p_sdx->data, p_sdx->currChunk, totallen);

    p_sdx->dataLength = totallen;
    p_sdx->ec = SDX_EC_ok;
    p_sdx->rc = SDX_RC_ok;
    return;
  }/*end-if structured*/


  /*----------  decryption  --------------------------------*/
  if (ISIN (flags, SDX_FLAG_encrypted) )
  {
                  /*----------------------------*/
    xchunkLength = sdx_crypt (CM_DECRYPT, p_sdx);
                  /*----------------------------*/

    if (xchunkLength < 0)
    {
      p_sdx->ec = SDX_EC_crypterror;
      p_sdx->rc = SDX_RC_dataError;
      return;
    }

    EXCL (p_sdx->currChunk->flags, SDX_FLAG_encrypted);
  }

  /*----------  compression  --------------------------------*/
  if ( ISIN (flags, SDX_FLAG_compressed) )
  {
    int    error;
    long   maxLength;
    long   dataLength = getDataLength (p_sdx->currChunk);

    if ( ISIN (flags, SDX_FLAG_array) )
    {
      maxLength = p_sdx->count * p_sdx->dataLength;
      uncr_data = sdx_workBuffer (p_sdx, maxLength);
    }
    else
    {
      maxLength = p_sdx->maxLength;
      uncr_data = p_sdx->data;
    }
    error =
     /*--------------------------------------------------------------------*/
      decompress (uncr_data, maxLength, &p_sdx->currChunk->data, dataLength);
     /*--------------------------------------------------------------------*/
    if ( error > 0 )
    {
      switch (error)
      {
        case CMP_RC_unknown_type : p_sdx->ec = SDX_EC_unknown;   break;
        default:                   p_sdx->ec = SDX_EC_comprerr;
      }
      p_sdx->rc = SDX_RC_dataError;
      return;
    } /* end if */

    xchunkLength = (maxLength < dataLength) ? maxLength : dataLength;

  }
  else            /* not compressed  */
  {
    uncr_data  = &p_sdx->currChunk->data;
  }

  /*----------  processing data type-------------------------*/

  if ( ! ISIN (flags, SDX_FLAG_array) )
  {
    p_sdx->dataLength = xchunkLength;
    p_sdx->count      = 0;
  }

  switch ( p_sdx->dataType )
  {
  case SDX_DT_numeric :
  case SDX_DT_float :

    /**************/
    extract_numeric (p_sdx, uncr_data, xchunkLength);
    /**************/

    break;
  default:

    /**************/
    extract_charbin (p_sdx, uncr_data, xchunkLength);
    /**************/

  }/*end-switch*/

} /* end extract */

/********************************************************/
void extract_charbin (SDX_obj *p_sdx, Byte *p_source, long p_length)
{
  long   leng;
  Byte  *target;

  Byte   filler = p_sdx->filler;
  Byte   flags  = p_sdx->currChunk->flags;
  int    dt     = DT (flags);


  if ( ISIN (flags, SDX_FLAG_short) ) /* but not numeric */
  {
    target  = p_sdx->data;
    getShortData (p_sdx->currChunk, target);

    switch (p_sdx->dataType)
    {
      long ix, len;

    case SDX_DT_char:
      ix = ShortLength - 1;
      while (ix >= 0 && target [ix] == 0) --ix;
      len = ix + 1;
      ix  = p_sdx->maxLength;
      while (ix > len) target [--ix] = p_sdx->filler;
      p_sdx->dataLength = len;
      break;

    case SDX_DT_binary:
      ix = p_sdx->maxLength;
      while (ix > ShortLength) target [--ix] = NIL;
      break;

    default:
      p_sdx->ec = SDX_EC_wrongDataType;
      p_sdx->rc = SDX_RC_dataError;
      return;
    } /*end-switch dataType */

    leng = 3;
  }

  else if ( ISIN (flags, SDX_FLAG_array) )
  {
    short count, ix;
    long  dataLength;
    Byte *target;

    if (p_sdx->count <= 0)
    {
      p_sdx->ec = SDX_EC_dataCutted;
      p_sdx->rc = SDX_RC_parameterError;
      return;
    }

    memcpy (&count, p_source, 2);
    count      = BT_adaptS (count);
    dataLength = (p_length - ARRAYHEADERLENGTH) / count;
    p_sdx->ec  = SDX_EC_ok;

    if (p_sdx->dataLength < dataLength)
      p_sdx->ec    = SDX_EC_dataCutted;

    if (count > p_sdx->count)
    {
      count        = p_sdx->count;   /* ->count: input   */
      p_sdx->ec    = SDX_EC_dataCutted;
    }

    p_sdx->count = count;            /* ->count: output  */
    p_source    += 2;
    target       = p_sdx->data;

    switch (dt)
    {
      Byte *gap;

    case SDX_DT_char :
      for (ix = 0; ix < count; ++ix)
      {
        BT_convert (target, p_source, dataLength, g_xoptions.toHost);
        gap          = target + dataLength;
        target      += p_sdx->dataLength;
        p_source    += dataLength;
        while (gap < target) *(gap++) = filler;
      }/*end-for*/
      break;

    case SDX_DT_binary :
      for (ix = 0; ix < count; ++ix)
      {
        memcpy (target, p_source, dataLength);
        gap          = target + dataLength;
        target      += p_sdx->dataLength;
        p_source    += dataLength;
        while (gap < target) *(gap++) = DEF_FILLER;
      }/*end-for*/
      break;

    default:
      p_sdx->ec = SDX_EC_wrongDataType;
      p_sdx->rc = SDX_RC_dataError;
      return;
    } /*end-switch dataType */

    p_sdx->rc = SDX_RC_ok;
    return;
  }

  else  /* standard chunk (not short or array, but bin or char) */
  {
    memcpy (p_sdx->data, p_source, p_length);

  } /* end-if short, array, standard */

  /*-----------------  conversion  ---------------------*/
  {
    switch (dt)
    {
    case SDX_DT_char :                                                                  /*#92*/
      BT_convert (p_sdx->data, p_sdx->data, p_sdx->dataLength, g_xoptions.toHost);
      break;

    case SDX_DT_UTF8 :                                                                  /*#05*/
      if (g_xoptions.cvtUTF8Proc)
      {
        /*        Byte *worka = malloc (p_sdx->dataLength);                             /*#15*/
        bool rc;

        long outLength = p_sdx->maxLength;
        /*        memcpy (worka, p_sdx->data, p_sdx->dataLength);                       /*#15*/

        rc = g_xoptions.cvtUTF8Proc
              ( UTF8_FROM
              , p_sdx->data, &outLength          /* output */
              , p_sdx->data, p_sdx->dataLength   /* input  */
              );
             /*  (FROM_UTF8, p_sdx->data, &outLength, worka, p_sdx->dataLength);        /*#15*/
             /*        free (worka);                                                    /*#15*/
        if (rc)
        {
          p_sdx->dataLength = outLength;
        }
        else
        {
          p_sdx->ec = SDX_EC_malformed;
          p_sdx->rc = SDX_RC_dataError;
          return;
        }
      }
      break;
    } /*end-switch dataType */
  }

  /*--------------  fill up with filler  ------------*/

  target = (Byte *)(p_sdx->data) + p_sdx->dataLength;
  leng   = p_sdx->maxLength      - p_sdx->dataLength;

  if ( dt == SDX_DT_binary ) filler = DEF_FILLER;

  if ( leng > 0 )
    while (leng) target [--leng] = filler;

  if ( leng < 0 )
  {
    p_sdx->ec = SDX_EC_dataCutted;
    p_sdx->rc = SDX_RC_failed;
  }
  else
  {
    p_sdx->ec = SDX_EC_ok;
    p_sdx->rc = SDX_RC_ok;
  }
} /* end extract_charbin */

/********************************************************/
void extract_numeric (SDX_obj *p_sdx, Byte *p_source, long p_length)
{
  Byte   flags;
  long   xchunkLength;

  p_sdx->ec    = SDX_EC_ok;
  flags        = p_sdx->currChunk->flags;
  xchunkLength = getChunkLength (p_sdx->currChunk);

  if ( ISIN (flags, SDX_FLAG_array) )
  {
    short count;
    long  dataLength;

    if (p_sdx->count <= 0)
    {
      p_sdx->ec = SDX_EC_dataCutted;
      p_sdx->rc = SDX_RC_parameterError;
      return;
    }

    memcpy (&count, p_source, 2);
    count      = BT_adaptS (count);
    dataLength = (p_length - ARRAYHEADERLENGTH) / count;

    if (p_sdx->dataLength < dataLength && p_sdx->dataType == SDX_DT_numeric )
    {
      p_sdx->ec = SDX_EC_wrongLength;
      p_sdx->rc = SDX_RC_parameterError;
      return;
    }

    if (count > p_sdx->count)
    {
      count       = p_sdx->count;   /* ->count: input   */
      p_sdx->ec   = SDX_EC_dataCutted;
    }

    p_sdx->count  = count;            /* ->count: output  */
    p_source     += 2;

    switch (dataLength)  /* internal data length (idl) */
    {
      int     ix;
      short   workS;
      long    workL;
      float   workF;
      double  workD;

    case 1: /* idl = 1                                              */
      switch (p_sdx->dataLength) /* external data length */
      {
      case 1: /* signed char */
        memcpy (p_sdx->data, p_source, count);
        break;

      case 2: /* short */
        for (ix = 0; ix < count; ++ix)
        {
          workS = *(Small *)p_source;
          memcpy ( &(((short *)p_sdx->data) [ix]), &workS, 2);
          p_source += 1;
        }
        break;

      case 4: /* long */
        for (ix = 0; ix < count; ++ix)
        {
          workL = *(Small *)p_source;
          memcpy ( &(((long *)p_sdx->data) [ix]), &workL, 4);
          p_source += 1;
        }
        break;

      default:
        p_sdx->ec = SDX_EC_wrongLength;
        p_sdx->rc = SDX_RC_parameterError;
        return;
      }/*end-switch p_sdx->dataLength */
      break;

    case 2: /* idl = 2                                                */
      switch (p_sdx->dataLength) /* external data length */
      {
      case 2: /* short */
        for (ix = 0; ix < count; ++ix)
        {
          workS = BT_adaptS (*(short *)p_source);
          memcpy ( &(((short *)p_sdx->data) [ix]), &workS, 2);
          p_source += 2;
        }
        break;

      case 4:  /* long */
        for (ix = 0; ix < count; ++ix)
        {
          workL = BT_adaptS (*(short *)p_source);
          memcpy ( &(((long *)p_sdx->data) [ix]), &workL, 4);
          p_source += 2;
        }
        break;

      default:
        p_sdx->ec = SDX_EC_wrongLength;
        p_sdx->rc = SDX_RC_parameterError;
        return;
      }/*end-switch p_sdx->dataLength */
      break;

    case 4: /* idl = 4                                                   */
      switch (p_sdx->dataLength) /* external data length */
      {
      case 4: /* long, float */
        switch ( p_sdx->dataType )
        {
        case SDX_DT_numeric :
          for (ix = 0; ix < count; ++ix)
          {
            workL = BT_adaptL (*(long *)p_source);
            memcpy ( &(((long *)p_sdx->data) [ix]), &workL, 4);
            p_source += 4;
          }
          break;
        case SDX_DT_float :
          for (ix = 0; ix < count; ++ix)
          {
            workF = BT_adaptFtoHost (*(float *)p_source);
            memcpy ( &(((float *)p_sdx->data) [ix]), &workF, 4);
            p_source += 4;
            p_sdx->ec = get_float_ec ();
          }
          break;
        }
        break;

      case 8: /* double */
        for (ix = 0; ix < count; ++ix)
        {
          workD = BT_adaptFtoHost (*(float *)p_source);
          memcpy ( &(((double *)p_sdx->data) [ix]), &workD, 8);
          p_source += 4;
          p_sdx->ec = get_float_ec ();
        }
        break;

      default:
        p_sdx->ec = SDX_EC_wrongLength;
        p_sdx->rc = SDX_RC_parameterError;
        return;
      }/*end-switch p_sdx->dataLength */
      break;


    case 8: /* idl = 8                                                    */
      switch (p_sdx->dataLength) /* external data length */
      {
      case 4: /* float */
        for (ix = 0; ix < count; ++ix)
        {
          /* warning ok: "possible loss of data" : */
          workF = (float) BT_adaptDtoHost (*(double *)p_source);
          memcpy ( &(((float *)p_sdx->data) [ix]), &workF, 4);
          p_source += 8;
          p_sdx->ec = get_float_ec ();
        }
        break;

      case 8: /* double */
        for (ix = 0; ix < count; ++ix)
        {
          workD = BT_adaptDtoHost (*(double *)p_source);
          memcpy ( &(((double *)p_sdx->data) [ix]), &workD, 8);
          p_source += 8;
          p_sdx->ec = get_float_ec ();
        }
        break;

      default:
        p_sdx->ec = SDX_EC_wrongLength;
        p_sdx->rc = SDX_RC_parameterError;
        return;
      }/*end-switch p_sdx->dataLength */
      break;

    default:
      p_sdx->ec = SDX_EC_wrongLength;
      p_sdx->rc = SDX_RC_dataError;
      return;

    }/*end-switch data length */

  }/*end-then -------------------------------------------------- array */

  else if ( ISIN (flags, SDX_FLAG_short) )
  {
    long work = 0;
    getShortData (p_sdx->currChunk, (Byte *)(&work)+1);
    p_sdx->value      = BT_adaptL (work);
    p_sdx->dataLength = 4;
  }

  else if ( xchunkLength == 0 )
  {
    p_sdx->value      = 0;
    p_sdx->dataLength = 0;
  }

  else if ( xchunkLength == 2 )
  {
    short work = 0;
    memcpy (&work, p_source, 2);
    p_sdx->value      = BT_adaptS (work);
    p_sdx->dataLength = 2;

  }

  else if ( xchunkLength == 4 )
  {
    if (p_sdx->dataType == SDX_DT_numeric)
    {
      long work = 0;
      memcpy (&work, p_source, 4);
      p_sdx->value  = BT_adaptL (work);
    }
    else
    {
      float work = 0;
      memcpy (&work, p_source, 4);
      p_sdx->fvalue = BT_adaptFtoHost (work);
    }

    p_sdx->dataLength = 4;
  }

  else if ( xchunkLength == 8 )
  {
    double work = 0;

    memcpy (&work, p_source, 8);
    p_sdx->fvalue     = BT_adaptDtoHost (work);
    p_sdx->dataLength = 8;
  }

  if ( p_sdx->dataType == SDX_DT_numeric )  p_sdx->fvalue = p_sdx->value;

  /* ec is already set: ok or cutted: */
  p_sdx->rc = p_sdx->ec == SDX_EC_ok ? SDX_RC_ok : SDX_RC_warning;
  return;
}/*end extract_numeric */

/************************************************************************/
void extract_step_by_step (SDX_obj *p_sdx)

/* input : data, maxLength, (from next, enter, init: dataType), count
   output: dataLength, value, count
*/

{
  Byte   flags;
  short  count;
  long   dataLength;
  long   xchunkLength;
  Byte  *uncr_data;               // uncompressed array
  Byte  *curr_data;               // current data element

  flags = p_sdx->currChunk->flags;

  if ( p_sdx->arr_index == 0 )  // de-cryption and compression at
  {                                            // first access to chunck
    /*----------  decryption  --------------------------------*/
    if (ISIN (flags, SDX_FLAG_encrypted) )
    {
                    /*----------------------------*/
      xchunkLength = sdx_crypt (CM_DECRYPT, p_sdx);
                    /*----------------------------*/

      if (xchunkLength < 0)
      {
        p_sdx->ec = SDX_EC_crypterror;
        p_sdx->rc = SDX_RC_dataError;
        return;
      }

      EXCL (p_sdx->currChunk->flags, SDX_FLAG_encrypted);
    } //end-if encrypted

    /*----------  compression  --------------------------------*/
    if ( ISIN (flags, SDX_FLAG_compressed) )
    {
      int    error;
      long   maxLength;
      long   dataLength = getDataLength (p_sdx->currChunk);

      maxLength = g_xoptions.max_array_dim * p_sdx->dataLength;
      uncr_data = sdx_workBuffer (p_sdx, maxLength);

      error =
       /*--------------------------------------------------------------------*/
        decompress (uncr_data, maxLength, &p_sdx->currChunk->data, dataLength);
       /*--------------------------------------------------------------------*/
      if ( error > 0 )
      {
        switch (error)
        {
          case CMP_RC_unknown_type : p_sdx->ec = SDX_EC_unknown;   break;
          default:                   p_sdx->ec = SDX_EC_comprerr;
        }
        p_sdx->rc = SDX_RC_dataError;
        return;
      } /* end if */
    }
    else            /* not compressed  */
    {
      uncr_data  = &p_sdx->currChunk->data;
    } //end-if compressed

  }//end- first element


  if ( ISIN (flags, SDX_FLAG_compressed) )
  {
    uncr_data  = p_sdx->private_stuff->workbuffer;
  }
  else
  {
    uncr_data  = &p_sdx->currChunk->data;
  }

  memcpy (&count, uncr_data, 2);
  count  = BT_adaptS (count);

  if ( p_sdx->arr_index >= count )
  {
    p_sdx->ec = SDX_EC_end_of_data;
    p_sdx->rc = SDX_RC_warning;
    return;
  }

  dataLength = (getDataLength (p_sdx->currChunk) - ARRAYHEADERLENGTH) / count;
  curr_data  = uncr_data + (p_sdx->arr_index * dataLength) + ARRAYHEADERLENGTH;


  switch ( dataLength + 100 * p_sdx->dataType )
  {
    case 301:
      p_sdx->value = (long)*curr_data;
      break;
    case 302:
      {
        short work = 0;
        memcpy (&work, curr_data, 2);
        p_sdx->value = BT_adaptS (work);
      }
      break;
    case 304: ;
      {
        long work = 0;
        memcpy (&work, curr_data, 4);
        p_sdx->value  = BT_adaptL (work);
      }
      break;
    case 504: ;
      {
        float work = 0;
        memcpy (&work, curr_data, 4);
        p_sdx->fvalue = BT_adaptFtoHost (work);
      }
      break;
    case 508: ;
      {
        double work = 0;
        memcpy (&work, curr_data, 8);
        p_sdx->fvalue = BT_adaptDtoHost (work);
      }
      break;
    default:
      p_sdx->ec = SDX_EC_not_supported;
      p_sdx->rc = SDX_RC_failed;
      return;
  }

  ++ p_sdx->arr_index;

  if ( p_sdx->dataType == SDX_DT_numeric )  p_sdx->fvalue = p_sdx->value;

  p_sdx->ec = SDX_EC_ok;
  p_sdx->rc = SDX_RC_ok;

}/*end-extract_step_by_step  */



/*.Append ; *******************************************************/
void SDX_append (SDX_obj *p_sdx)

/* input : data, maxLength
   output: chunkID, dataLength
   update: currChunk, maxLength
*/

{
  Byte   *target, *source;
  SDX_Chunk  *targetChunk;
  ChunkID id;
  long    leng, xchunkLength;

  BT_assertf (p_sdx != NULL, ERROR_noparam);
  if (p_sdx->rc > SDX_RC_warning) return;  /* fall thru */

  p_sdx->function = "append";

  if (p_sdx->magic != SDX_MAGIC)
  {
    p_sdx->ec = SDX_EC_magicError;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }

  if ( p_sdx->remainingSize < SDX_HEADERLENGTH )
  {
    p_sdx->ec = SDX_EC_cont_too_small;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }

  if ( p_sdx->data == NULL)
  {
    p_sdx->ec = SDX_EC_data_missing;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }

  if (p_sdx->container == NULL)
  {
    p_sdx->ec = SDX_EC_initError;
    p_sdx->rc = SDX_RC_illegalOperation;
    return;
  }
  /* end if param */

  /*---- first chunk ? --------------------*/
  if ( p_sdx->level == 0 )
    p_sdx->currChunk = (SDX_Chunk *) p_sdx->container;
  else
    p_sdx->currChunk = p_sdx->private_stuff->newChunk;

  targetChunk = (SDX_Chunk *) p_sdx->data;           /* type casting */

  memcpy (&id, &targetChunk->chunkID, SDX_IDlen);                                       /*#95*/
  p_sdx->chunkID    = BT_adaptS (id);

  xchunkLength      = getChunkLength (targetChunk);
  p_sdx->dataLength = getDataLength  (targetChunk);

  /*----  enough place for new chunk ? -----*/

  if ( xchunkLength + SDX_HEADERLENGTH > p_sdx->remainingSize )
    { p_sdx->ec = SDX_EC_dataCutted; p_sdx->rc = SDX_RC_failed; return; }

  if ( p_sdx->level == 0 )
  {
    target = p_sdx->container;
  }
  else
  {
    struct  T_stack *stackE;
    stackE = & p_sdx->private_stuff->stack [p_sdx->level - 1];
    target = (Byte *)(stackE->chunk) + SDX_HEADERLENGTH + stackE->length;
  }

  leng = xchunkLength + SDX_HEADERLENGTH;
  source = p_sdx->data;

  /*----- move data ----*/
  memcpy (target, source, leng);

  if ( p_sdx->level > 0 )
  {
    struct  T_stack *stackE = & p_sdx->private_stuff->stack [p_sdx->level - 1];

    stackE->length += leng;
    setChunkLength (stackE->chunk, leng + getChunkLength (stackE->chunk));
  }

  setDataType (p_sdx);

  p_sdx->private_stuff->newChunk = (SDX_Chunk *)((Byte *)(p_sdx->currChunk) + leng);
  p_sdx->remainingSize -= leng;

  p_sdx->ec = SDX_EC_ok;
  p_sdx->rc = SDX_RC_ok;
} /* end Append */

/***********************************************************/

static long create_skalar             (SDX_obj*, Byte *target);
static long create_array              (SDX_obj*, Byte *target);
static long create_array_step_by_step (SDX_obj*, Byte *target);

/***********************************************************/
void SDX_create (SDX_obj *p_sdx)

/* input : chunkID, dataLength, data | value, dataType, count,
           compression, encrypt
   update: remainingSize
   output: currChunk
*/

{
  bool  encryption_required;
  bool  compression_required;
  Byte *target;
  int   ix;
  long  targetLength;
  long  xchunkLength;
  long  block_filler = 0;

  BT_assertf (p_sdx != NULL, ERROR_noparam);
  if (p_sdx->rc > SDX_RC_warning) return;  /* fall thru */

  p_sdx->function = "create";

  if (p_sdx->magic != SDX_MAGIC)
  {
    p_sdx->ec = SDX_EC_magicError;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }

  if ( p_sdx->remainingSize < SDX_HEADERLENGTH )
  {
    p_sdx->ec = SDX_EC_cont_too_small;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }

  if ( p_sdx->container == NULL)
  {
    p_sdx->ec = SDX_EC_error;
    p_sdx->rc = SDX_RC_illegalOperation;
    return;
  }

  if ( (p_sdx->dataType == SDX_DT_structured)
  && (  p_sdx->level >= g_xoptions.maxLevel) )
  {
    p_sdx->ec = SDX_EC_levelOvflw;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }

  if ( p_sdx->private_stuff->oldChunkID != p_sdx->chunkID )
  {
    p_sdx->private_stuff->oldChunkID = p_sdx->chunkID;
    p_sdx->arr_index  = 0;
  }

  encryption_required
    = ! ISIN (p_sdx->private_stuff->iflags, IFLAGS_encryption_pending)
      && p_sdx->encrypt;

  compression_required
    = ! ISIN (p_sdx->private_stuff->iflags, IFLAGS_compression_pending)
      && p_sdx->compression;

  if (ISIN (p_sdx->private_stuff->iflags, IFLAGS_encryption_pending) || p_sdx->encrypt)
    block_filler = sdx_crypt (CM_GET_BLOCKSIZE, NULL);

  /*---- first chunk ? --------------------*/
  if ( p_sdx->level == 0 )
    p_sdx->currChunk = (SDX_Chunk *) p_sdx->container;
  else if ( p_sdx->arr_index == 0 )
    p_sdx->currChunk = p_sdx->private_stuff->newChunk;

  if ( (p_sdx->currChunk == NULL) )
  {
    p_sdx->ec = SDX_EC_error;
    p_sdx->rc = SDX_RC_illegalOperation;
    return;
  }

  if ( p_sdx->dataType == SDX_DT_structured || (p_sdx->level == 0) )
  {
    /*------- new group chunk -------------*/
    ix                                      = p_sdx->level;
    p_sdx->private_stuff->stack [ix].chunk  = p_sdx->currChunk;
    p_sdx->private_stuff->stack [ix].length = 0;
  } /* end if structured  */


  p_sdx->currChunk->flags = 0;
  p_sdx->ec               = SDX_EC_ok;

  if (p_sdx->dataType == SDX_DT_structured)
  {
    SETDT (p_sdx->currChunk->flags, SDX_DT_inconsistent);                               /*#92*/
      /* DT structured will be set by function SDX_leave */
  }
  else if ( p_sdx->dataType >  SDX_DT_implemented )
  {
    p_sdx->ec = SDX_EC_wrongDataType;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }
  else
  {
    SETDT (p_sdx->currChunk->flags, p_sdx->dataType);                                   /*#92*/
  }

  {                                                                                     /*#95*/
    ChunkID id = BT_adaptS (p_sdx->chunkID);
    memcpy (&p_sdx->currChunk->chunkID, &id, SDX_IDlen);
  }

  if (p_sdx->dataType == SDX_DT_structured)
  {
    /*------- new group chunk -------------*/
    xchunkLength  = 0;
  }

  else if ( p_sdx->count == -1 ) /* array chunk to be constructed step by step */
  {
    Byte  *currtarget;
    short  workS;

    INCL (p_sdx->currChunk->flags, SDX_FLAG_array);
    target = &p_sdx->currChunk->data;

    if ( p_sdx->arr_index == 0 )
    {
      setChunkLength (p_sdx->currChunk, ARRAYHEADERLENGTH);

      if ( compression_required )
      {
        targetLength = p_sdx->dataLength * g_xoptions.max_array_dim + ARRAYHEADERLENGTH;
        target = sdx_workBuffer (p_sdx, targetLength + CMP_HEADERLENGTH);
      }
    }//end-if first array element

    switch ( p_sdx->dataType )
    {
    case SDX_DT_numeric:  p_sdx->dataLength = 4; break;
    case SDX_DT_float:    p_sdx->dataLength = 8; break;
    }

    currtarget = target + 2 + p_sdx->dataLength * p_sdx->arr_index;

                   /************************/
    xchunkLength = create_array_step_by_step (p_sdx, currtarget)
                   /************************/
      + getChunkLength (p_sdx->currChunk);

    workS = BT_adaptS ((short)(p_sdx->arr_index + 1));
    memcpy (target, &workS, ARRAYHEADERLENGTH);
  }

  else if ( p_sdx->count > 0 )   /* array chunk */
  {
    short workS;

    targetLength = p_sdx->dataLength * p_sdx->count + ARRAYHEADERLENGTH;

    /*----  enough place for new chunk ? -----*/
    if ( targetLength + SDX_HEADERLENGTH + block_filler > p_sdx->remainingSize )
    {
      p_sdx->ec = SDX_EC_dataCutted;
      p_sdx->rc = SDX_RC_failed;
      return;
      /* if encryption blocksize (block_filler) > 0: This could be refined:          */
      /* " if (tl + hl + block_filler) / block_filler * block_filler > remsize ..."  */
      /* avoiding a premature dataCutted error                                       */
    }

    INCL (p_sdx->currChunk->flags, SDX_FLAG_array);

    if ( compression_required )
      target = sdx_workBuffer (p_sdx, targetLength + CMP_HEADERLENGTH);
    else
      target = &p_sdx->currChunk->data;

    workS = BT_adaptS (p_sdx->count);
    memcpy (target, &workS, ARRAYHEADERLENGTH);

                   /***********/
    xchunkLength = create_array (p_sdx, target);
                   /***********/
  }

  else /* skalar chunk */
  {
    if ( p_sdx->dataType == SDX_DT_numeric || p_sdx->dataType == SDX_DT_float )
    {
      compression_required = false;
                     /************/
      xchunkLength = create_skalar (p_sdx, NULL);
                     /************/
    }
    else  /* non-numeric data types: */
    {
      targetLength
        = p_sdx->dataLength * (p_sdx->dataType == SDX_DT_UTF8 ? 2 : 1);
        /* improper workaround: the factor 2 is not true for asian      */
        /* languages. true length by mode UTF8_LENGTH in cvtUTF8proc ?  */
        /* (Request R158)  */

      /*----  enough place for new chunk ? -----*/
      if ( targetLength + SDX_HEADERLENGTH > p_sdx->remainingSize )
      {
        p_sdx->ec = SDX_EC_dataCutted;
        p_sdx->rc = SDX_RC_failed;
        return;
      }


      if ( compression_required )
        target = sdx_workBuffer (p_sdx, targetLength + CMP_HEADERLENGTH);
      else
        target = &p_sdx->currChunk->data;

                     /*************/
      xchunkLength = create_skalar (p_sdx, target);
                     /*************/
    }

  }/* end-if struct, array or skalar */

  setChunkLength (p_sdx->currChunk, xchunkLength);

  if (p_sdx->rc > SDX_RC_ok) return;


  /*----------  compression  -----------------------*/
  if (compression_required && p_sdx->dataType != SDX_DT_structured)
  {
    /* outData points to the work buffer! */
                  /********/
    long cmprlen = compress
                  /********/
      (&p_sdx->currChunk->data, target, xchunkLength, p_sdx->compression);

    if (cmprlen > 0)
    {
      INCL (p_sdx->currChunk->flags, SDX_FLAG_compressed);       /* ok */
      xchunkLength = cmprlen; /* cmprlen is shorter */
    }
    else if (cmprlen == 0)
    {
      memcpy (&p_sdx->currChunk->data, target, xchunkLength);
    }
    else
    {
        p_sdx->ec = SDX_EC_comprerr;
        p_sdx->rc = SDX_RC_programError;
        return;
    } /*end-switch compression ok or not */


  }/*end-if compressed */


  /*----------  encryption  -----------------------*/
  if (encryption_required)
  {
    INCL (p_sdx->currChunk->flags, SDX_FLAG_encrypted);

    if ( p_sdx->dataType == SDX_DT_structured )
      INCL (p_sdx->private_stuff->iflags, IFLAGS_encryption_pending);
    else
    {                                                                                   /*#07*/
                    /********/
      xchunkLength = sdx_crypt (CM_ENCRYPT, p_sdx);
                    /********/

      if (xchunkLength < 0)
      {
        p_sdx->ec = SDX_EC_crypterror;
        p_sdx->rc = SDX_RC_dataError;
        return;
      }
    }//end-if structured

  } /*end-if (encryption_required)*/

  setChunkLength (p_sdx->currChunk, xchunkLength);

  if ( p_sdx->count == -1 && p_sdx->arr_index > 0 )
  {
    struct  T_stack *stackE = & p_sdx->private_stuff->stack [p_sdx->level - 1];

    stackE->length += p_sdx->dataLength;
    setChunkLength (stackE->chunk, stackE->length);

    p_sdx->remainingSize -= p_sdx->dataLength;
  }
  else
  {
    xchunkLength += SDX_HEADERLENGTH;

    /*---------  adapting the stack  ----------------*/
    if ( p_sdx->level > 0 )
    {
      struct  T_stack *stackE = & p_sdx->private_stuff->stack [p_sdx->level - 1];

      stackE->length += xchunkLength;                                                  /*#07)*/
      setChunkLength (stackE->chunk, stackE->length);                               /*#11#07)*/
    } /* end if */

    if ( p_sdx->dataType == SDX_DT_structured )   ++(p_sdx->level);

    p_sdx->private_stuff->newChunk = (SDX_Chunk *)((Byte *)(p_sdx->currChunk) + xchunkLength);
    p_sdx->remainingSize -= xchunkLength;

    /*-------- compression of structured chunk -------*/
    if ( compression_required && p_sdx->dataType == SDX_DT_structured )
    {
      INCL (p_sdx->currChunk->flags, SDX_FLAG_compressed);
      INCL (p_sdx->private_stuff->iflags, IFLAGS_compression_pending);

    } /*end-if (compression_required)*/
  }//end-if construction of array step by step - or not

  if ( p_sdx->count == -1 )
  {
    if ( p_sdx->arr_index > 0 )
      p_sdx->private_stuff->newChunk =
        (SDX_Chunk *)((Byte *)(p_sdx->private_stuff->newChunk) + p_sdx->dataLength);

    ++p_sdx->arr_index;
  }

  /*------------  ok  -----------------------------*/
  p_sdx->rc    = (p_sdx->ec == SDX_EC_ok) ?  SDX_RC_ok : SDX_RC_warning;
  p_sdx->count = 0;

} /* end Create */

/***************************************************************************************/
long create_skalar (SDX_obj *p_sdx, Byte *p_target)
{
  long   xchunkLength = p_sdx->dataLength;
  long   workL;
  double workD;

  bool encryption_required
    = ! ISIN (p_sdx->private_stuff->iflags, IFLAGS_encryption_pending)
      && p_sdx->encrypt;


  /*--------- conversion of char-data  -------------*/
  switch (p_sdx->dataType)
  {
  case SDX_DT_numeric:
    workL = BT_adaptL (p_sdx->value);

    if ( p_sdx->value >= 0 && p_sdx->value <= 0x007FFFFF
        && ! encryption_required)
    {
      setShortData (p_sdx->currChunk, (Byte *)(&workL) + 1, 3);
      INCL (p_sdx->currChunk->flags, SDX_FLAG_short);
      return 0;
    }

    else if ( (unsigned long)p_sdx->value <= (unsigned long)0x00007FFF )
    { /* if encryption is required : no shorten */
      memcpy ((Byte *)&(p_sdx->currChunk->data), (Byte *)(&workL)+2, 2);
      xchunkLength = 2;
    }

    else
    { /* value negativ or absolute value too high */
      memcpy ((Byte *)&(p_sdx->currChunk->data), (Byte *)&workL, 4);
      xchunkLength = 4;
    } /*end-if value */

    return xchunkLength;

  case SDX_DT_float:
    workD = BT_adaptDtoNet (p_sdx->fvalue);
    memcpy ((Byte *)&(p_sdx->currChunk->data), (Byte *)&workD, 8);
    xchunkLength = 8;
    p_sdx->ec = get_float_ec (); /* for mainframe specific conversions: see float.h */
    return xchunkLength;

  case SDX_DT_char:
   /**********/
    BT_convert (p_target, p_sdx->data, p_sdx->dataLength, g_xoptions.toNet);
   /**********/
    break;

  case SDX_DT_binary:
    memcpy (p_target, p_sdx->data, p_sdx->dataLength);
    break;

  case SDX_DT_UTF8:                                                                     /*#05*/
    if ( g_xoptions.cvtUTF8Proc )
    {
      long outLength = 2 * xchunkLength;
                    /***********/
      if (g_xoptions.cvtUTF8Proc
                    /***********/
            (UTF8_TO, p_target, &outLength, p_sdx->data, xchunkLength))
      {
        xchunkLength = outLength;
      }
      else
      {
        p_sdx->ec = SDX_EC_malformed;
        p_sdx->rc = SDX_RC_dataError;
        return xchunkLength;
      }
    }/*end-if cvtUTF8Proc defined */

    else if ( p_target != p_sdx->data )
    {
      memcpy ( p_target, p_sdx->data, xchunkLength);
    }
    break;

  }/*end-switch dataType  (conversion) */

  /*------------- compression of data  -------------*/
  #ifdef CUTTING_TRAILING_BLANKS
  if (p_sdx->compression && p_sdx->dataType == SDX_DT_char)
  {
    Byte    *endbuffer;

    endbuffer = p_target + xchunkLength;
    while ( endbuffer >= p_target && *(--endbuffer) == (Byte)' ' );
    xchunkLength = endbuffer - p_target + 1;
  }
  #endif /* CUTTING_TRAILING_BLANKS */

  if ( xchunkLength > 0 ) setChunkLength (p_sdx->currChunk, xchunkLength);

  return xchunkLength;
} /* end create_skalar */

/***************************************************************************************/
static long create_array_step_by_step (SDX_obj *p_sdx, Byte *p_target)
{
  long   workL;
  float  workF;
  double workD;

  switch (p_sdx->dataType)
  {
  case SDX_DT_numeric:
    workL = BT_adaptL (p_sdx->value);
    memcpy (p_target, (Byte *)(&workL + 4 - p_sdx->dataLength), p_sdx->dataLength);
    return p_sdx->dataLength;

  case SDX_DT_float:
    switch ( p_sdx->dataLength )
    {
    case 4:
      workF = BT_adaptFtoNet ((float)p_sdx->fvalue);
      memcpy (p_target, (Byte *)(&workF), 4);
      break;
    case 8:
      workD = BT_adaptDtoNet (p_sdx->fvalue);
      memcpy (p_target, (Byte *)(&workD), 8);
      break;
    default:
      p_sdx->rc = SDX_RC_parameterError;
      p_sdx->ec = SDX_EC_wrongLength;
      return 0;
    }//end-switch datalength
    p_sdx->ec = get_float_ec (); /* for mainframe specific conversions: see float.h */
    return p_sdx->dataLength;

  case SDX_DT_char:
   /**********/
    BT_convert (p_target, p_sdx->data, p_sdx->dataLength, g_xoptions.toNet);
   /**********/
    return p_sdx->dataLength;

  case SDX_DT_binary:
    memcpy (p_target, p_sdx->data, p_sdx->dataLength);
    return p_sdx->dataLength;

  }/*end-switch dataType  (conversion) */

  return 0;
}
/***************************************************************************************/
static long create_array   (SDX_obj *p_sdx, Byte *p_target)
{
  int    ix;
  short  workS;
  long   workL;
  float  workF;
  double workD;
  short  count        = p_sdx->count;
  long   xchunkLength = count * p_sdx->dataLength + ARRAYHEADERLENGTH;

  p_target += ARRAYHEADERLENGTH;   /* arrayheader already set */

  switch (p_sdx->dataType)
  {
  case SDX_DT_numeric:
    switch (p_sdx->dataLength)
    {
    case 1:
      memcpy (p_target, p_sdx->data, count);
      break;

    case 2:
      for (ix = 0; ix < count; ++ix)
      {
        workS = BT_adaptS (((short *)p_sdx->data) [ix]);
        memcpy (p_target, &workS, 2);
        p_target += 2;
      }
      break;

    case 4:
      for (ix = 0; ix < count; ++ix)
      {
        workL = BT_adaptL (((long *)p_sdx->data) [ix]);
        memcpy (p_target, &workL, 4);
        p_target += 4;
      }
      break;

    default:
      p_sdx->ec    = SDX_EC_wrongLength;
      p_sdx->rc    = SDX_RC_parameterError;
      xchunkLength = 0;

    }/*end-switch data length */
    break;

  case SDX_DT_float:
    switch (p_sdx->dataLength)
    {
    case 4:
      for (ix = 0; ix < count; ++ix)
      {
        workF = BT_adaptFtoNet (((float *)p_sdx->data) [ix]);
        memcpy (p_target, &workF, 4);
        p_target += 4;
      }
      break;

    case 8:
      for (ix = 0; ix < count; ++ix)
      {
        workD = BT_adaptDtoNet (((double *)p_sdx->data) [ix]);
        memcpy (p_target, &workD, 8);
        p_target += 8;
      }
      p_sdx->ec = get_float_ec ();
      break;
    }/*end-switch data length */
    break;

  case SDX_DT_char:
    BT_convert (p_target, p_sdx->data, count * p_sdx->dataLength,
      g_xoptions.toNet);
    break;

  case SDX_DT_binary:
    memcpy (p_target, p_sdx->data, count * p_sdx->dataLength);
    break;

  default:
    p_sdx->ec    = SDX_EC_wrongDataType;
    p_sdx->rc    = SDX_RC_parameterError;
    xchunkLength = 0;
  }


  if ( xchunkLength > 0 ) setChunkLength (p_sdx->currChunk, xchunkLength);

  return xchunkLength;
} /* end create_array */

/***********************************************************/
void SDX_update (SDX_obj *p_sdx)
{
  /* input : chunkID, value | data                     */
  /* undocumented function: in-place-update of a chunk */

  long xchunkLength = getChunkLength (p_sdx->currChunk);

  BT_assertf (p_sdx != NULL, ERROR_noparam);
  if (p_sdx->rc > SDX_RC_warning) return;  /* fall thru */

  p_sdx->function = "update";

  if
  (  ! p_sdx->currChunk
  || ISIN (p_sdx->currChunk->flags, SDX_FLAG_encrypted)
  || ISIN (p_sdx->currChunk->flags, SDX_FLAG_compressed)
  )
  {
    p_sdx->rc = SDX_RC_parameterError;
    p_sdx->ec = SDX_EC_forbidden;
    return;
  }

  if (p_sdx->chunkID != SDX_blankID)
  {
    ChunkID id = BT_adaptS (p_sdx->chunkID);
    memcpy (&p_sdx->currChunk->chunkID, &id, SDX_IDlen);
  }

  if (DT (p_sdx->currChunk->flags) == SDX_DT_structured)                                /*#92*/
  {
    if (p_sdx->encrypt)
    {
      long leng = sdx_crypt (CM_ENCRYPT, p_sdx);

      if (leng < 0)
      {
        p_sdx->ec = SDX_EC_crypterror;
        p_sdx->rc = SDX_RC_dataError;
        return;
      }

      INCL (p_sdx->currChunk->flags, SDX_FLAG_encrypted);
      setChunkLength (p_sdx->currChunk, leng);
    }

    p_sdx->rc = SDX_RC_ok;
    p_sdx->ec = SDX_EC_ok;
    return;
  }

  if (p_sdx->dataType == SDX_DT_numeric)
  {
    long work = BT_adaptL (p_sdx->value);

    if (ISIN (p_sdx->currChunk->flags, SDX_FLAG_short))
    {
      if ( (unsigned long)p_sdx->value <= (unsigned long)0x007FFFFF)
        setShortData (p_sdx->currChunk, (Byte *)(&work) + 1, 3);
      else
        goto cutted;
    }
    else if (xchunkLength == 2)
    {
      if ( (unsigned long)p_sdx->value <= (unsigned long)0x00007FFF )
        memcpy ((Byte *)&(p_sdx->currChunk->data), (Byte *)&work, 2);
      else
        goto cutted;
    }
    else if (xchunkLength == 4)
      memcpy ((Byte *)&(p_sdx->currChunk->data), (Byte *)&work, 4);
    else
    {
      p_sdx->rc = SDX_RC_dataError;
      p_sdx->ec = SDX_EC_not_consistent;
      return;
    } /*end-if value */

    p_sdx->rc = SDX_RC_ok;
    p_sdx->ec = SDX_EC_ok;
    return;
  } /*end-if numeric */

  if (! p_sdx->data) goto ok;

  memcpy (&p_sdx->currChunk->data, p_sdx->data, xchunkLength);

ok:
  p_sdx->rc = SDX_RC_ok;
  p_sdx->ec = SDX_EC_ok;
  return;

cutted:
  p_sdx->rc = SDX_RC_dataError;
  p_sdx->ec = SDX_EC_dataCutted;

} /* end- SDX_update */

/***********************************************************/
void SDX_check (SDX_obj *p_sdx)

/* undocumented function: check chunk for consistency  */
{
  SDX_Chunk *wChunk, *currChunk;
  long   chunkLength, len;
  long   wlen;


  BT_assertf (p_sdx != NULL, ERROR_noparam);
  if (p_sdx->rc > SDX_RC_warning) return;  /* fall thru */

  p_sdx->function = "check";

  currChunk = p_sdx->currChunk;   /* save SDX_Chunk */

  if (currChunk == NULL)
  {
    p_sdx->ec = SDX_EC_paramMissing;
    p_sdx->rc = SDX_RC_parameterError;
    return;
  }

  if (! flagsOK (currChunk))
  {
    p_sdx->rc = SDX_RC_dataError;
    p_sdx->ec = SDX_EC_not_consistent;
    return;
  }

  chunkLength = getChunkLength (currChunk);

   if ( chunkLength > 0
    && DT (currChunk->flags) ==  SDX_DT_structured                                      /*#92*/
    && ! ISIN (currChunk->flags, SDX_FLAG_encrypted )
    && ! ISIN (currChunk->flags, SDX_FLAG_compressed)
  )
  {
    chunkLength  = getTotalLength (currChunk);

    wChunk = (SDX_Chunk *)((Byte *)currChunk + SDX_HEADERLENGTH);
    len = chunkLength - SDX_HEADERLENGTH;

    /* 1. loop: check only length consitency */
    while (len >= SDX_HEADERLENGTH)                                                     /*#07*/
    {
      wlen = getTotalLength (wChunk);
      len -= wlen;
      wChunk = (SDX_Chunk *)((Byte *)wChunk + wlen);
    } /* end-while len > 0 */

    if (len < 0
    || (len > 0 && ((Byte *)wChunk) [0] != len)                                         /*#07*/
       )
    {
      p_sdx->rc = SDX_RC_dataError;
      p_sdx->ec = SDX_EC_not_consistent;
      return;
    }

    wChunk = (SDX_Chunk *)((Byte *)currChunk + SDX_HEADERLENGTH);
    len = chunkLength - SDX_HEADERLENGTH;

    /* 2. loop: check also the inner chunks recursively */
    while (len >= SDX_HEADERLENGTH)
    {
      p_sdx->currChunk = wChunk;
      SDX_check (p_sdx);
      if ( p_sdx->rc > 0 )
      {
        p_sdx->rc = SDX_RC_dataError;
        p_sdx->ec = SDX_EC_not_consistent;
        return;
      }

      wlen   = getTotalLength (wChunk);
      len   -= wlen;
      wChunk = (SDX_Chunk *)((Byte *)wChunk + wlen);
    } /* end-while len > 0 */

  } /* end-if structured */

  p_sdx->currChunk = currChunk;     /* reload SDX_Chunk */
  p_sdx->rc        = SDX_RC_ok;
  p_sdx->ec        = SDX_EC_ok;

  return;

} /* end check */

/*************************************************************/
bool SDX_getTablesFromFile (Byte **p_toNet, Byte **p_toHost)
{
  Byte *toNet = NULL, *toHost = NULL;
  FILE *fh;

  fh = BT_altfopen (TABLES_ENV, TABLES_FILE, "r");
  if ((long)fh <= 0) return false;

  toNet = malloc (256);
  BT_assertf (toNet != NULL, ERROR_NO_MEM_TR_TABLE);

  BT_assertf (BT_readTable (toNet, fh), ERROR_TRANS_TABLE);

  toHost = malloc (256);
  BT_assertf (toHost != NULL, ERROR_NO_MEM_TR_TABLE);

  if (!BT_readTable (toHost, fh))
  {
    free (toHost);
    toHost = NULL;
  }/*end-if*/

  fclose (fh);
  *p_toNet  = toNet;
  *p_toHost = toHost;

  printf (TEXT_INIT);
  return true;

} /*end-SDX_getTablesFromFile */

/********* get options *************************************/
SDX_TOptions *SDX_getOptions ()
{
  if (g_xoptions.encryptBlocking)
    g_xoptions.encryptProc (CM_SET_BLOCKING, NULL, 0, NULL);                            /*#30*/

  return &g_xoptions;

} /* end- SDX_getOptions  */

/*----------------------------------------------------------*/
char *SDX_errtext (SDX_obj *p_sdx)
{
  char *str = (char *)sdx_workBuffer (p_sdx, SDX_ERRSTRLEN);

  return SDX_errtextB (p_sdx->rc, p_sdx->ec, str);

}/* end-SDX_errtext*/


/*----------------------------------------------------------*/
char *SDX_errtextB (short p_rc, short p_ec, char *p_buffer)
{
  sprintf (p_buffer, "SDXF/%02hd/%02hd -- ", p_rc, p_ec);

  switch (p_rc)
  {
    /*00*/ case SDX_RC_ok               : strcat (p_buffer, RC_TEXT_ok              ); break;
    /*01*/ case SDX_RC_failed           : strcat (p_buffer, RC_TEXT_failed          ); break;
    /*02*/ case SDX_RC_illegalOperation : strcat (p_buffer, RC_TEXT_illegalOperation); break;
    /*03*/ case SDX_RC_dataError        : strcat (p_buffer, RC_TEXT_dataError       ); break;
    /*04*/ case SDX_RC_parameterError   : strcat (p_buffer, RC_TEXT_parameterError  ); break;
    /*05*/ case SDX_RC_programError     : strcat (p_buffer, RC_TEXT_programError    ); break;
    default                             : strcat (p_buffer, RC_TEXT_other           );
  } /* end switch*/

  if (p_rc > 0) strcat (p_buffer, ": ");

  switch (p_ec)
  {
    /*00*/ case SDX_EC_ok               :                                              break;
    /*01*/ case SDX_EC_end_of_data      : strcat (p_buffer, EC_TEXT_end_of_data     ); break;
    /*02*/ case SDX_EC_notFound         : strcat (p_buffer, EC_TEXT_notFound        ); break;
    /*03*/ case SDX_EC_dataCutted       : strcat (p_buffer, EC_TEXT_dataCutted      ); break;
    /*04*/ case SDX_EC_overflow         : strcat (p_buffer, EC_TEXT_overflow        ); break;
    /*05*/ case SDX_EC_initError        : strcat (p_buffer, EC_TEXT_initError       ); break;
    /*06*/ case SDX_EC_comprerr         : strcat (p_buffer, EC_TEXT_comprerr        ); break;
    /*07*/ case SDX_EC_forbidden        : strcat (p_buffer, EC_TEXT_forbidden       ); break;
    /*08*/ case SDX_EC_unknown          : strcat (p_buffer, EC_TEXT_unknown         ); break;
    /*09*/ case SDX_EC_levelOvflw       : strcat (p_buffer, EC_TEXT_levelOvflw      ); break;
    /*10*/ case SDX_EC_paramMissing     : strcat (p_buffer, EC_TEXT_paramMissing    ); break;
    /*11*/ case SDX_EC_magicError       : strcat (p_buffer, EC_TEXT_magicError      ); break;
    /*12*/ case SDX_EC_not_consistent   : strcat (p_buffer, EC_TEXT_not_consistent  ); break;
    /*13*/ case SDX_EC_wrongDataType    : strcat (p_buffer, EC_TEXT_wrongDataType   ); break;
    /*14*/ case SDX_EC_noMemory         : strcat (p_buffer, EC_TEXT_noMemory        ); break;
    /*15*/ case SDX_EC_malformed        : strcat (p_buffer, EC_TEXT_malformed       ); break;
    /*16*/ case SDX_EC_crypterror       : strcat (p_buffer, EC_TEXT_crypterror      ); break;
    /*17*/ case SDX_EC_wrongLength      : strcat (p_buffer, EC_TEXT_wrongLength     ); break;
    /*18*/ case SDX_EC_FTP_inexact	    : strcat (p_buffer, EC_TEXT_FTP_inexact	    ); break;
    /*19*/ case SDX_EC_FTP_invalid	    : strcat (p_buffer, EC_TEXT_FTP_invalid	    ); break;
    /*20*/ case SDX_EC_FTP_underflow    : strcat (p_buffer, EC_TEXT_FTP_underflow	  ); break;
    /*21*/ case SDX_EC_FTP_overflow	    : strcat (p_buffer, EC_TEXT_FTP_overflow	  ); break;
    /*22*/ case SDX_EC_unknown_function	: strcat (p_buffer, EC_TEXT_unknown_function); break;
    /*23*/ case SDX_EC_cont_too_small 	: strcat (p_buffer, EC_TEXT_cont_too_small  ); break;
    /*24*/ case SDX_EC_not_supported  	: strcat (p_buffer, EC_TEXT_not_supported	  ); break;
    /*25*/ case SDX_EC_data_missing	    : strcat (p_buffer, EC_TEXT_data_missing	  ); break;
    /*26*/ case SDX_EC_not_structured   : strcat (p_buffer, EC_TEXT_not_structured  ); break;
    default                             : strcat (p_buffer, EC_TEXT_other           );
  } /* end switch*/

  return p_buffer;

}/* end-SDX_errtextB*/

/*----------------------------------------------------------*/
void  SDX_setKey (SDX_obj *p_sdx)                                                         //#34
{
  #define PASSLEN 100   /* only the first 100 chars of data are used */

  static Byte key [SDXE_SHA_HASHVALUE_LEN];
  char   passw [PASSLEN+1];
  long   passlen;
  SDX_TOptions *opts = SDX_getOptions ();

  if ( p_sdx->data == NULL ) return;

  passlen = strlen (p_sdx->data);
  if ( passlen > PASSLEN ) passlen = PASSLEN;

  BT_convert (passw, p_sdx->data, passlen, opts->toNet);

  SDXE_keyhash
  ( key                         /* out : the hash value [20]  */
  , SDXE_SHA_HASHVALUE_LEN      /* in  : length of output     */
  //, DEFAPPLKEY                  /* in  : the key              */                      /*#41*/
  //, DEFAPPLKEY_LEN              /* in  : length of key        */                      /*#41*/
  , p_sdx->data                 /* in  : the text             */
  , strlen (p_sdx->data)        /* in  : length of text       */
  , NULL                                                                                /*#41*/
  , 0                                                                                   /*#41*/
  );

  p_sdx->cryptkey = key;

}/* end-SDX_setKey*/

/********** end of source *******************/
