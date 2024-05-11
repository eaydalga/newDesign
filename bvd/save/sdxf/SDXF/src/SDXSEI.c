/* doc-----------------------------------------------------------------------------
:Project.   PINPI: Platform Independent Network Programming Interface
:Program.   SDXSEI.c
:PrType.    function lib
:Title.     Single Entry Interface for all SDXF calls
:Author.    Max Wildgrube
:Version.   
:Language.  ANSI-C
:Compiler.
:Created.   1997-03-21 
---------------------------------------------------------------------------------

Supported Interfaces:

  SDXFB - designed for Basic (called by the other Interfaces)
  SDXFC - designed for COBOL
  SDXFN - designed for Natural, (R) of Software-AG
  SDXFS - designed for SAS

---------------------------------------------------------------------------------*/ 

#define SDXSEI_version "SDXSEI build " __DATE__

/*************************************************/ 
static int g_mytrace = 0;       /*  trace on ?   */ 
/*************************************************/ 

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "tools.h"
#include "SDXtools.h"

#define TRACE   if(g_mytrace)
#define TRACE_P if(g_mytrace) printf   /* dummi= printf to avoid warnings ? */   
#define VB      ":4"
#define TR      ":6"
#define ET      ":7"


/* -------------------------  globals -------------------------  */ 

static SDX_PARAM    *g_param;               /* global ptr to param  */ 
static SDX_obj       sdxobj, *g_common_handle = &sdxobj;
static Byte         *g_data;
static SDX_obj      *g_sdx; 
static SDX_TOptions *g_sdxopts;
static int           g_first = 1;
static FILE         *g_file, *g_log_file;

static short         g_interf; /* controls the filler byte */
enum INTERF          { interf_none = 0, interf_C, interf_B };
 
/* sub function prototypes   */ 
/*          name                          function               */ 
/*          --------                      ---------------------- */ 
static void sn_version          (void);   /*00*/ 
static void sn_init             (void);   /*01*/ 
static void sn_new              (void);   /*02*/ 
static void sn_enter            (void);   /*03*/ 
static void sn_leave            (void);   /*04*/ 
static void sn_next             (void);   /*05*/ 
static void sn_select           (void);   /*06*/ 
static void sn_extract          (void);   /*07*/ 
static void sn_create           (void);   /*08*/ 
static void sn_append           (void);   /*09*/ 
static void sn_adaptshort       (void);   /*10*/ 
static void sn_adaptlong        (void);   /*11*/ 
static void sn_getbuffer        (void);   /*12*/ 
static void sn_readchunk        (void);   /*13*/ 
static void sn_writechunk       (void);   /*14*/ 
static void sn_fileopen         (void);   /*15*/ 
static void sn_fileclose        (void);   /*16*/ 
static void sn_set_key          (void);   /*17*/                                      //#34
static void sn_get_err_text     (void);   /*18*/ 
// static void sn_set_password     (void);   /*19*/
static void sn_set_log_file     (void);   /*19*/ 
static void sn_close_log_file   (void);   /*20*/ 

typedef void (*function) (void);

static struct Tfunclist {function fun; char *str;} funclist [] =
{
  /* 00 */   {sn_version          , "SDXF-version" }
  /* 01 */  ,{sn_init             , "sdx-init"     }
  /* 02 */  ,{sn_new              , "sdx-new"      }
  /* 03 */  ,{sn_enter            , "sdx-enter"    }
  /* 04 */  ,{sn_leave            , "sdx-leave"    }
  /* 05 */  ,{sn_next             , "sdx-next"     }
  /* 06 */  ,{sn_select           , "sdx-select"   }
  /* 07 */  ,{sn_extract          , "sdx-extract"  }
  /* 08 */  ,{sn_create           , "sdx-create"   }
  /* 09 */  ,{sn_append           , "sdx-append"   }
  /* 10 */  ,{sn_adaptshort       , "adaptshort"   }
  /* 11 */  ,{sn_adaptlong        , "adaptlong"    }
  /* 12 */  ,{sn_getbuffer        , "getbuffer"    }
  /* 13 */  ,{sn_readchunk        , "readChunk"    }
  /* 14 */  ,{sn_writechunk       , "writeChunk"   }
  /* 15 */  ,{sn_fileopen         , "file-open"    }
  /* 16 */  ,{sn_fileclose        , "file-close"   }
  /* 17 */  ,{sn_set_key          , "set-key"      }                                      //#34
  /* 18 */  ,{sn_get_err_text     , "get-err-text" }
  /* |  */ 
};/* A ------------V */ 
#define MAX_FUNC  ((sizeof funclist)/8-1)  


/******************************************************************/ 
/*
** NATURAL user exit: SDXFN
**
** usage: CALL 'SDXFN' PIN-PARM [DATA] 
*/ 
#ifdef SUPPORT_NATURAL
  NATFCT SDXFN NATARGDEF(nparm, parmptr, parmdec)
  {
    SDX_PARAM *param;
    Byte      *data;

    param = (SDX_PARAM *) parmptr [0];

    if (nparm >1) 
      data  = parmptr [1];   
    else
      data  = NULL;

    return sdxfC (param, data);
  }

#endif /* SUPPORT_NATURAL */

/**************************************************************/ 
DllExport void sdxfS (
                        SDX_handle *handle,       /*  1 F   4  internal                */ 
                        long       *value,        /*  2 F   4  binary value            */ 
                        double     *fvalue,       /*  3 D   8  floating binary value   */ /*#33*/
                        long       *length,       /*  4 F   4  data length             */ 
                        long       *bufferSize,   /*  5 F   4  size of SDXF container  */ 
                        short      *chunkID,      /*  6 H   2                          */ 
                        short      *dataType,     /*  7 H   2  SDXF data type          */ 
                        short      *count,        /*  8 H   2  array count             */ 
                        short      *function,     /*  9 H   2  one of f_               */ 
                        short      *rc,           /* 10 H   2  return code             */ 
                        short      *ec,           /* 11 H   2  extended rc             */ 
                        short      *level,        /* 12 H   2  SDXF struct level       */  /*#33*/
                        short      *do_encrypt,   /* 13 H   2  = 0 or 1                */  /*#33*/
                        short      *compr,        /* 14 H   2  compression method      */  /*#33*/
                        char       *option,       /* 15 C   1  option (SDX_open)       */  /*#33*/ 
                        char       *name,         /* 16 C  32  actual function name    */ 
                        // char       *passw,        /* 17 C  16  encryptkey (password)   */ //#34
                        char       *data          /* 18 C 200  char data               */  
                      )

{
  SDX_PARAM sdxpar;

  sdxpar.handle     = *handle      ;        
  sdxpar.value      = *value       ;    
  sdxpar.fvalue     = *fvalue      ;   
  sdxpar.length     = *length      ;   
  sdxpar.bufferSize = *bufferSize  ;
  sdxpar.chunkID    = *chunkID     ;  
  sdxpar.dataType   = *dataType    ; 
  sdxpar.count      = *count       ; 
  sdxpar.function   = *function    ; 
  sdxpar.rc         = *rc          ;       
  sdxpar.ec         = *ec          ;       
  sdxpar.level      = *level       ;    
  sdxpar.do_encrypt = *do_encrypt  ;
  sdxpar.compr      = *compr       ;    
  sdxpar.option     = *option      ;    
  BT_strZTerm (sdxpar.name,  name,  SDX_NAMLEN);
  // BT_strZTerm (sdxpar.passw, passw, SDXE_PASSLEN);                                 //#34

  /*************************/
     sdxfB (&sdxpar, data);
  /*************************/ 
  
  *handle      =  sdxpar.handle    ;        
  *value       =  sdxpar.value     ;    
  *fvalue      =  sdxpar.fvalue    ;   
  *length      =  sdxpar.length    ;   
  *bufferSize  =  sdxpar.bufferSize;
  *chunkID     =  sdxpar.chunkID   ;  
  *dataType    =  sdxpar.dataType  ; 
  *count       =  sdxpar.count     ; 
  *function    =  sdxpar.function  ; 
  *rc          =  sdxpar.rc        ;       
  *ec          =  sdxpar.ec        ;       
  *level       =  sdxpar.level     ;    
  *do_encrypt  =  sdxpar.do_encrypt;
  *compr       =  sdxpar.compr     ;    
  *option      =  sdxpar.option    ;    
  strncpy (name,  sdxpar.name,  SDX_NAMLEN);
  //strncpy (passw, sdxpar.passw, SDXE_PASSLEN);

}

/**************************************************************/ 
DllExport long  sdxfC (SDX_PARAM *p_param, Byte *p_data)
{
  g_interf = interf_C;                /* C: The COBOL Interface */
  return sdxfB (p_param, p_data);
}

/**************************************************************/ 
DllExport long CC sdxfB (SDX_PARAM *p_param, Byte *p_data)
{
  Ulong ix;
  static char  *functionName; 
  static long   dummy, *dummy_area = &dummy;

  if ( ! g_interf )  g_interf = interf_B; /* B: The Basic Interface */
  g_sdxopts = SDX_getOptions ();                                       /*#13*/ 

  if (g_first)
  {
    g_first = 0;
    g_sdxopts->cvtUTF8Proc = UTF8_vsHost;  

    #define printf BT_printf
    /*   test:  BT_set_printf (tracelog);  */ 

    TRACE 
    {
      printf ("SDXSEI-I -- version %s \n", __DATE__);
    } /*END_TRACE*/ 

  } /*END first */ 

  if (!p_param)
  {
    printf ("SDXSEI-E -- severe program error: no param given!\n");
    return 999;
  }

  ix = p_param->function;
  if (ix > MAX_FUNC)
  {
    p_param->rc = SDX_RC_parameterError;
    p_param->ec = SDX_EC_unknown_function;

    printf ("SDXSEI-E -- function code %d is illegal!\n",ix);
    return 1;
  }

  g_param = p_param;
  g_data  = p_data;
  

  if (p_param->handle)
    g_sdx = p_param->handle;
  else
    p_param->handle = g_sdx = g_common_handle;

  if (g_sdx->rc > SDX_RC_warning && p_param->function != f_sdx_get_err_text)
    return g_sdx->rc;

  BT_strZTerm (NULL, p_param->name, SDX_NAMLEN);
  
   
  g_sdx->chunkID     = p_param->chunkID;
  g_sdx->data        = p_data;                                         /*#48*/ 
  g_sdx->dataLength  = p_param->length;
  g_sdx->maxLength   = p_param->length;
  g_sdx->bufferSize  = p_param->bufferSize;                            /*#72*/ 
  g_sdx->value       = p_param->value;
  g_sdx->fvalue      = p_param->fvalue;
  g_sdx->dataType    = p_param->dataType;
  g_sdx->count       = p_param->count;
  g_sdx->encrypt     = (Byte)p_param->do_encrypt;
  g_sdx->compression = (Byte)p_param->compr;
  g_sdx->filler      = g_interf == interf_C ? ' ' : NIL;
//  g_sdx->cryptkey    = p_param->passw;                               //#34
  
      /** function code as index for array funclist **/ 
  functionName = funclist [ix].str;

  TRACE_P
     ("SDXSEI-I -- running %s (%d), dt=%d\n", 
       functionName, p_param->function, p_param->dataType);

 /*****************************/ 
  (*(funclist [ix]).fun)();
 /*****************************/ 

  g_sdx               = p_param->handle;
  p_param->rc         = g_sdx->rc;
  p_param->ec         = g_sdx->ec;
  p_param->chunkID    = g_sdx->chunkID;
  p_param->length     = g_sdx->dataLength;
  p_param->bufferSize = g_sdx->remainingSize;
  p_param->value      = g_sdx->value;
  p_param->fvalue     = g_sdx->fvalue;
  p_param->level      = g_sdx->level;
  p_param->dataType   = g_sdx->dataType;
  p_param->count      = g_sdx->count;
  p_param->do_encrypt = g_sdx->encrypt;
  //p_param->option     = '0';  //test

  TRACE_P 
   ("SDXSEI-I -- returns rc=%d, ec=%d, dt=%d \n", 
     p_param->rc, p_param->ec, p_param->dataType);

  BT_strifill (p_param->name, functionName, SDXE_PASSLEN);

  return p_param->rc;
}
/************   and now the functions...   ***************/ 
/****************************************************************/ 
static void sn_version      (void)   /* 00 */ 
/****************************************************************/ 
{
  if (g_data)
    BT_strifill (g_data, g_sdxopts->version, SDX_NAMLEN);
  else
    BT_strifill (g_param->name, SDXSEI_version, SDX_NAMLEN);

  g_sdx->level  = 6;
  g_sdx->value  = 7;
  g_sdx->fvalue = 8.3;
  g_sdx->dataLength = 2;
  g_sdx->chunkID = 4711;

  g_sdx->rc = SDX_RC_ok;
  g_sdx->ec = SDX_EC_ok;
}


/****************************************************************/ 
static void sn_init         (void)   /* 01 */ 
/****************************************************************/ 
{
  if (!g_sdx)
    g_sdx = g_param->handle = g_common_handle;

  g_sdx->dataType = SDX_OPT_OLD;
  SDX_init (g_sdx);

  TRACE 
  {
    printf (TR "SDXSEI-T-init : chunkID: %hd; length= %d; dataType= %hd\n",
      g_sdx->chunkID, g_sdx->dataLength, g_sdx->dataType); 
  } /*end-TRACE*/ 
  
} /* end- sn_init        */ 

/****************************************************************/ 
static void sn_new          (void)   /* 02 */ 
/****************************************************************/ 
{

  if (!g_sdx)
    g_sdx = g_param->handle = g_common_handle;

  TRACE 
  {
    printf ("SDXSEI-T-new : length= %d; addr= %p\n", g_sdx->bufferSize, g_data);
  } /*end-TRACE*/ 

  g_sdx->container  = NULL;   /* old: g_data;                            #33*/
  g_sdx->bufferSize = g_param->bufferSize;                             /*#72*/ 
  g_sdx->dataType   = SDX_OPT_NEW;

  SDX_init (g_sdx);

} /* end- sn_new         */ 

/****************************************************************/ 
static void sn_enter        (void)   /* 03 */ 
/****************************************************************/ 
{
  if (g_sdx) 
  {
    SDX_enter (g_sdx);

    TRACE 
    {
      printf (ET "SDXSEI-T-enter : chunkID: %d; length= %d; dataType= %hd\n",
        g_sdx->chunkID, g_sdx->dataLength, g_sdx->dataType); 
    } /*end-TRACE*/ 
  }
  else
  {
    g_param->rc = SDX_RC_parameterError;
    g_param->ec = SDX_EC_paramMissing;
  }

} /* end- sn_enter       */ 

/****************************************************************/ 
static void sn_leave        (void)   /* 04 */ 
/****************************************************************/ 
{
  if (g_sdx) 
  {
    SDX_leave (g_sdx);

    TRACE 
    {
      printf (ET "SDXSEI-T-leave : chunkID: %hd; length= %d; dataType= %hd\n",
        g_sdx->chunkID, g_sdx->dataLength, g_sdx->dataType); 
    } /*end-TRACE*/ 
  }
  else
  {
    g_param->rc = SDX_RC_parameterError;
    g_param->ec = SDX_EC_paramMissing;
  }

} /* end- sn_leave       */ 

/****************************************************************/ 
static void sn_next         (void)   /* 05 */ 
/****************************************************************/ 
{
  if (g_sdx) 
  {
    SDX_next (g_sdx);

    TRACE 
    {
      printf (ET "SDXSEI-T-next : chunkID: %hd; length= %d; dataType= %hd\n",
        g_sdx->chunkID, g_sdx->dataLength, g_sdx->dataType); 
    } /*end-TRACE*/ 
  }
  else
  {
    g_param->rc = SDX_RC_parameterError;
    g_param->ec = SDX_EC_paramMissing;
  }

} /* end- sn_next        */ 

/****************************************************************/ 
static void sn_select       (void)      /* 06 */ 
/****************************************************************/ 
{
  if (g_sdx) 
    SDX_select (g_sdx);
  else
  {
    g_param->rc = SDX_RC_parameterError;
    g_param->ec = SDX_EC_paramMissing;
  }
   
} /* end- sn_select      */ 

/****************************************************************/ 
static void sn_extract         (void)   /* 07 */ 
/****************************************************************/ 
{
  if (g_sdx) 
  {
    TRACE 
    {
      printf (ET "SDXSEI-T-extract : chunkID: %hd; length= %d; dataType= %hd\n",
        g_sdx->chunkID, g_sdx->dataLength, g_sdx->dataType); 
    } /*end-TRACE*/ 
  
    SDX_extract (g_sdx);
  }
  else
  {
    g_param->rc = SDX_RC_parameterError;
    g_param->ec = SDX_EC_paramMissing;
  }

} /* end- sn_extract        */ 

/****************************************************************/ 
static void sn_create       (void)   /* 08 */ 
/****************************************************************/ 
{
  if (g_sdx) 
  {
    TRACE 
    {
      printf (ET "SDXSEI-T-create: chunkID: %hd; length= %d; dataType= %hd;"
        " compr.type: %02d\n",
        g_sdx->chunkID, g_sdx->dataLength, g_sdx->dataType, g_sdx->compression); 
    } /*end-TRACE*/ 

    g_sdx->data = g_data;
    SDX_create (g_sdx);
  }
  else
  {
    g_param->rc = SDX_RC_parameterError;
    g_param->ec = SDX_EC_paramMissing;
  }

} /* end- sn_create      */ 

/****************************************************************/ 
static void sn_append       (void)   /* 09 */ 
/****************************************************************/ 
{
  if (g_sdx && g_data) 
  {
    TRACE 
    {
      printf (ET "SDXSEI-T-append: chunkID: %hd; length= %d; dataType= %hd\n",
        g_sdx->chunkID, g_sdx->dataLength, g_sdx->dataType); 
    } /*end-TRACE*/ 

    g_sdx->data = g_data;
    SDX_append (g_sdx);
  }
  else
  {
    g_param->rc = SDX_RC_parameterError;
    g_param->ec = SDX_EC_paramMissing;
  }

} /* end- sn_append      */ 

/****************************************************************/ 
static void sn_adaptshort   (void)   /* 10 */ 
/****************************************************************/ 
{
  g_param->value = BT_adaptS ((short)g_param->value);  // useful ???
} /* end- sn_adaptshort  */ 

/****************************************************************/ 
static void sn_adaptlong    (void)   /* 11 */ 
/****************************************************************/ 
{
  g_param->value = BT_adaptL (g_param->value); 
} /* end- sn_adaptlong   */ 

/****************************************************************/ 
static void sn_getbuffer    (void)   /* 12 */ 
/****************************************************************/ 
{
  if (g_sdx) 
  {
    TRACE 
    {
      printf ("SDXSEI-T-getbuffer : length= %d\n", g_param->length); 
    } /*end-TRACE*/ 

    g_sdx->container = malloc (g_param->length);
  }
  else
  {
    g_param->rc = SDX_RC_parameterError;
    g_param->ec = SDX_EC_paramMissing;
  }

} /* end- sn_getbuffer   */ 


/****************************************************************/ 
static void sn_readchunk     (void)   /* 13 */ 
/****************************************************************/ 
/* params in: g_file  -- file handle (internal use) 
              g_sdx   -- out off SDXF handle 
*/
{
  if (g_sdx) 
  {
    //????g_sdx->container = g_data;
    SDX_readChunk (g_sdx, g_file);

    TRACE 
    {
      printf (VB "SDXSEI-T-readchunk: chunkID: %hd; length= %d; dataType= %hd\n",
        g_sdx->chunkID, g_sdx->dataLength, g_sdx->dataType); 
    } /*end-TRACE*/ 

  }
  else
  {
    g_param->rc = SDX_RC_parameterError;
    g_param->ec = SDX_EC_paramMissing;
  }

}/* end-sn_readchunk */ 


/****************************************************************/ 
static void sn_writechunk    (void)   /* 14 */ 
/****************************************************************/ 
/* params in: g_file  -- file handle (internal use) 
              g_sdx   -- out off SDXF handle 
*/
{
  if (g_sdx) 
  {
    TRACE 
    {
      printf (VB "SDXSEI-T-writechunk : chunkID: %hd; length= %d; dataType= %hd\n",
        g_sdx->chunkID, g_sdx->dataLength, g_sdx->dataType); 
    } /*end-TRACE*/ 

    SDX_writeChunk (g_sdx, g_file);
  }
  else
  {
    g_param->rc = SDX_RC_parameterError;
    g_param->ec = SDX_EC_paramMissing;
  }

}/* end-sn_writechunk */ 

/****************************************************************/ 
static void sn_fileopen     (void)   /* 15 */ 
/****************************************************************/ 
/* params in: g_param->name   -- filename
              g_param->option -- open modus (r, w)
         out: g_file          -- file handle (internal use) 
*/
{
    char mode [3] = "xb";

    mode [0] = tolower (g_param->option);

    TRACE 
    {
      printf ("SDXSEI-T-fileopen %s; mode: %s\n", g_param->name, mode); 
    } /*end-TRACE*/ 

    g_file = fopen (g_param->name, mode);
    if (g_file)
    {
      g_sdx->rc = SDX_RC_ok;
      g_sdx->ec = SDX_EC_ok;
    }
    else
    {
      g_sdx->rc = SDX_RC_dataError;
      g_sdx->ec = SDX_EC_notFound;
    }

} /* end-sn_fileopen */ 

/****************************************************************/ 
static void sn_fileclose    (void)   /* 16 */ 
/****************************************************************/ 
/* params in: g_file  -- file handle (internal use) 
*/
{
  TRACE 
  {
    printf ("SDXSEI-T-fileclose\n"); 
  } /*end-TRACE*/ 

  if (g_file) fclose (g_file);
  g_file    = NULL;

  g_sdx->rc = SDX_RC_ok;
  g_sdx->ec = SDX_EC_ok;

} /* end-sn_fileclose */ 


/****************************************************************/ 
static void sn_set_key (void)    /* 17 */                                     //#34
/****************************************************************/ 
{
  if (g_sdx) 
  {
    TRACE 
    {
      printf ("SDXSEI-T-setKey : key: '%s'\n", g_sdx->data); 
    } /*end-TRACE*/ 
  
    g_sdx->data = g_data;
    if ( g_sdx->dataLength > 0 ) g_sdx->data [g_sdx->dataLength] = 0; 
    SDX_setKey (g_sdx);
    g_sdx->encrypt = true;
  }
  else
  {
    g_param->rc = SDX_RC_parameterError;
    g_param->ec = SDX_EC_paramMissing;
  }

  g_sdx->rc = SDX_RC_ok;
  g_sdx->ec = SDX_EC_ok;

} /* end-sn_set_encryption */ 


/****************************************************************/ 
static void sn_get_err_text     (void)   /* 18 */ 
/****************************************************************/ 
{
  if (! g_data) return;

  SDX_errtextB (g_param->rc, g_param->ec, g_data);
  g_sdx->dataLength = strlen (g_data);

  g_sdx->rc = SDX_RC_ok;
  g_sdx->ec = SDX_EC_ok;
  
}/* end- sn_get_err_text */ 

#ifdef obsolete                                                                          //#34
                          /****************************************************************/ 
                          static void sn_set_password     (void)   /* 19 */ 
                          /****************************************************************/ 
                          {
                            if (! g_data) return;
                            strncpy (g_param->handle->cryptkey, g_data, SDXE_PASSLEN); 

                            g_sdx->rc = SDX_RC_ok;
                            g_sdx->ec = SDX_EC_ok;
  
                          }/* end- sn_set_password */ 
#endif

/****   end- source   ********************************************/ 
