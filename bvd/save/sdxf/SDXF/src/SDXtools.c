/* doc----------------------------------------------------------------
:Project.   SDXF: Structured Data Exchange Format
:Program.   SDXtools.c
:PrType.    function lib
:Title.     Additional Tools for SDXF
:Version.   1.0
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI-C
:Compiler.
:Created.   1994-11-10 
----------------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>

#include "SDXtools.h"
//???#include "tools.h"


void SDT_dmy() {}  /* dummy for MVS */ 


/*------------------------------------------------------------------*/ 
FILE *SDX_fileOpen (const char *p_filename, char p_modus)
/* params in: filename
              modus     ('r', 'w', 'a')
     returns: file handle
*/
{
    char mode [3] = "xb";

    mode [0] = tolower (p_modus);

    return fopen (p_filename, mode);

} /* end-SDX_fileOpen */ 

/*------------------------------------------------------------------*/ 
void SDX_fileClose (FILE *p_file)
/* params in: file handle returned by SDX_fileOpen
*/
{
  if ( p_file ) fclose (p_file);

} /* end-SDX_fileClose */ 

/*------------------------------------------------------------------*/ 
bool SDX_writeChunk (SDX_obj *p_sdx, FILE *p_file)
{
  size_t len;
    
  if ( !p_file )
  { 
    p_sdx->ec = SDX_EC_error; 
    p_sdx->rc = SDX_RC_illegalOperation; 
    return false;
  }

  p_sdx->dataType = SDX_OPT_OLD;
  SDX_init (p_sdx);

  len = p_sdx->dataLength + SDX_HEADERLENGTH;                          /*#75*/ 

  if ( fwrite (p_sdx->currChunk, 1, len, p_file) == len)
  {
    p_sdx->ec = SDX_EC_ok;
    p_sdx->rc = SDX_RC_ok;
    return true;
  }
  else
  {
    p_sdx->ec = SDX_EC_dataCutted;
    p_sdx->rc = SDX_RC_failed;
    return false;
  } /* END if */;

} /** END WriteChunk */ 

/*----------------------------------------------------------*/
bool SDX_readChunk (SDX_obj *p_sdx, FILE *p_file)
{
  long leng;
  int  ix; 
  char tempheader [SDX_HEADERLENGTH];

  if ( !p_file )
  { 
    p_sdx->ec = SDX_EC_error; 
    p_sdx->rc = SDX_RC_illegalOperation; 
    return false;
  }

  if (p_sdx->container)
    /* { p_sdx->ec = SDX_EC_error; p_sdx->rc = SDX_RC_parameterError;   return;}*/ 
    leng = fread (p_sdx->container, 1, SDX_HEADERLENGTH, p_file);
  else
    leng = fread (tempheader, 1, SDX_HEADERLENGTH, p_file);

  if (leng < SDX_HEADERLENGTH) 
    { p_sdx->ec = SDX_EC_end_of_data; p_sdx->rc = SDX_RC_failed; return false;}

  if (!p_sdx->container)      /* then allocate memory for container: */ 
  {    
    p_sdx->container = tempheader;
    p_sdx->dataType  = SDX_OPT_OLD;

    SDX_init (p_sdx);

    p_sdx->container = malloc (SDX_HEADERLENGTH + p_sdx->dataLength + 32);
    if (!p_sdx->container)
      { p_sdx->ec = SDX_EC_noMemory; p_sdx->rc = SDX_RC_noMemory; return false;}

    memcpy (p_sdx->container, tempheader, SDX_HEADERLENGTH);
    p_sdx->bufferSize = SDX_HEADERLENGTH + p_sdx->dataLength; 
  }

  p_sdx->dataType = SDX_OPT_OLD;
  SDX_init (p_sdx);

  if (p_sdx->dataLength > p_sdx->bufferSize - SDX_HEADERLENGTH)
    { p_sdx->ec = SDX_EC_dataCutted; p_sdx->rc = SDX_RC_dataError; return false;}

  leng = fread (p_sdx->container + SDX_HEADERLENGTH, 1, p_sdx->dataLength, p_file); 

  ix = ferror (p_file);
  ix = feof   (p_file);

  if       (leng < p_sdx->dataLength)
    { p_sdx->ec = SDX_EC_dataCutted; p_sdx->rc = SDX_RC_failed;    return false;}
  else if  (p_sdx->dataLength < leng)
    { p_sdx->ec = SDX_EC_error;      p_sdx->rc = SDX_RC_dataError; return false;}

  SDX_check (p_sdx);
  return p_sdx->rc == SDX_RC_ok;

} /* END ReadChunk */ 


/****************************************************************/
FILE *SDX_altfopen (char *p_env, char *p_file, char p_mode)
{                                     /* not thread-safe !!!MT  */ 
  FILE *fh;
  char *name;
  char *envir;

  name           = p_file;

  if ((fh = fopen (p_file, "r"))) /* exists file in curr. dir.? */
  {
    fclose (fh);
    fh = SDX_fileOpen (p_file, p_mode);  /* yes: open in desired mode */
  }
  else
  {
    envir = getenv (p_env);       /* no: try content of envir var */  

    if (envir == NULL) return (FILE *)-1;

    fh = SDX_fileOpen (envir, p_mode);
    name = envir;
  }

  return fh;

}/*end- SDX_altfopen*/

 


/*----------------------------------------------------------*/
void  SDX_free (SDX_obj *p_sdx)
{
  free (p_sdx->container);
  p_sdx->container = NULL;

} /* END SDX_free */ 


/** END SDXtools. **/ 
