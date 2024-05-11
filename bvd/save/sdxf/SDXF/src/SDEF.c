/* doc-----------------------------------------------------------------------------
:Project.   SDXF
:Program.   SDEF.c
:PrType.    main
:Title.     SDEF, Structured Data Editable Format
:Version.   
:Author.    Max Wildgrube
:Language.  ANSI-C
:Compiler.
:Created.   2001-01-20
:Usage.     convert from <infile> to <outfile> as <format> 
:                <compression type> <de/encryption key> NEPL=<number array elements per line>
:              format can be: SDXF or SDEF
:              compression types are: (only for convert as SDXF)
:                RL1
:                DEF (deflate)   
:              
---------------------------------------------------------------------------------*/ 
#define notSTEP_BY_STEP
#define LINELEN         200
#define BUFFLEN         128000
#define MAX_ARRAY_DIM   10000
#define LINE_THRESHOLD  240

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDXtools.h"
#include "parser.h"
#include "tools.h"


#define ARGTEMPL "RK(FROM) RK(TO) RKX(AS;SDXF SDEF) X(;RL1 DEF) K(KEY) KN(NEPL)"
//#define printf BT_printf

typedef struct
{
  char *infile;
  char *outfile;
  long  dir;
  long  compr;
  char *key;
  long  elements_per_line;
} TParm;

int  toSDXF   (TParm);
int  toSDEF   (TParm);
int  SDEF_out (SDX_handle, FILE*, long nepl);

static SDX_obj g_sdx;
static bool    g_error;

/**************************************************************************/ 
void usage (char *argv[])
{
  SDX_TOptions *opt = SDX_getOptions();

  if ( argv [1] )
    printf ("input was:\n   convert %s\n\n", BT_parmjoin(argv));

  printf 
    ( "usage: SDXconvert from <infile> to <outfile> as <format>\n" 
      "    (following parameters are optional:)\n"     
      "         <compression_type> <KEY de/encryption_key>\n"
      "         NEPL=<number of array elements per line>\n\n"
      "format can be: SDXF or SDEF\n"
      "compression types are: (only for convert to SDXF) RL1, DEF (deflate)\n"
    );
}

/**************************************************************************/ 
int main (int argc, char *argv[])
{
  static TParm  parm;
  int           rc;
  SDX_TOptions *opt = SDX_getOptions();

  printf 
    ( "convert: SDXF vs SDEF and v.v.\n"
      "Vers %s\n"
      "SDXF Version %s\n" 
      "by Max Wildgrube\n\n",
      __DATE__,
      opt->version
    );

//  BT_set_printf (BT_protocol);
//  BT_set_prompt (True); 
  
  rc = BT_parse ((long *)&parm, ARGTEMPL, BT_parmjoin(argv), NULL);
  if ( rc > 0 )
  {
    if ( rc > 1 ) printf ("convert parameter error (%s)\n", BT_parserr());
    usage (argv);
    return 1;
  }

  printf ("infile=%s\noutfile=%s\nas=%d (%s)\ncompr=%d\nkey=%s\narray elements per line=%d\n"
    , parm.infile
    , parm.outfile
    , parm.dir, parm.dir == 1 ? "SDXF" : "SDEF"
    , parm.compr
    , parm.key
    , parm.elements_per_line
    );

//  g_sdx.cryptkey = parm.key;
//  g_sdx.compression = (Byte) parm.compr;

  switch (parm.dir)
  {
  case 1:
    toSDXF (parm);
    break;
  case 2:
    toSDEF (parm);
    break;
  default:
    printf ("program error BT_parse\n");
    usage (argv);
    return 2;
  }

  if ( g_error ) printf ("some errors occurs\n");
  return 0;
}/*end- main */


/**************************************************************************/ 
int toSDXF (TParm p_parm)
{
  enum  { array_no, array_running, array_finished };

  FILE   *infile, *outfile;
  int     incnt = 0, outcnt = 0, line, array_count, array_status = array_no;
  char    str [LINELEN];
  ChunkID id, first_array_id = 0; 
  char    type;
  int     int_array    [MAX_ARRAY_DIM]; 
  double  double_array [MAX_ARRAY_DIM]; 


  if (! (infile = fopen(p_parm.infile, "rt")))
  {
    printf ("could not open infile %s\n", p_parm.infile);
    return 1;
  }

  if (! (outfile = SDX_fileOpen (p_parm.outfile, 'w')))
  {
    printf ("could not open outfile %s\n", p_parm.outfile);
    return 1;
  }

  g_sdx.container  = NULL;
  g_sdx.bufferSize = BUFFLEN;
  g_sdx.dataType   = SDX_OPT_NEW;
  SDX_init (&g_sdx);
  if (g_sdx.rc > SDX_RC_ok) {line = __LINE__; goto error;}
  
  g_sdx.compression = (Byte)p_parm.compr;

  if ( p_parm.key )
  { 
    g_sdx.data = p_parm.key;
    SDX_setKey (&g_sdx); 
    g_sdx.encrypt  = 1;
  }

  while (fgets (str, LINELEN, infile))
  {
    int len;

    len = strlen (str);

    if (str [len-1] == '\n') str [--len] = 0;
    ++incnt;
    
    if (len < 6) continue;
    
    sscanf (str, "%5hu%c", &id, &type);

    if ( array_status == array_running )
    {
      char *strp = strtok (str + 6, " ");

      while ( strp )
      {
        if ( array_count >= MAX_ARRAY_DIM ) 
        {
          printf ("too much array elements (more than %d) -- stop\n", MAX_ARRAY_DIM+1);
          abort();
        }

        switch ( type )
        {
        case '#':
          sscanf (strp, "%d", &int_array [array_count]);
          break;
        case '!':
          sscanf (strp, "%lE", &double_array [array_count]);
          break;
        }

        ++array_count;
        strp = strtok (NULL, " ");
      }//end-while tokens

    }//end-if running

    g_sdx.chunkID    = id;
    g_sdx.data       = NULL;
    g_sdx.dataLength = 0;

    switch (type)
    {
    case '(':
      g_sdx.dataType = SDX_DT_structured;
      break;

    case ')':
      SDX_leave (&g_sdx);
      if (g_sdx.rc > SDX_RC_ok) {line = __LINE__; goto error;}

      if (g_sdx.level == 0)
      {
        SDX_writeChunk (&g_sdx, outfile);
        ++outcnt;

        g_sdx.dataType   = SDX_OPT_NEW;
        SDX_init (&g_sdx);
        if (g_sdx.rc > SDX_RC_ok) {line = __LINE__; goto error;}
  
        g_sdx.compression = (Byte)p_parm.compr;
        if ( p_parm.key )
        {
          g_sdx.cryptkey = p_parm.key;
          g_sdx.encrypt  = 1;
        }

      }
      continue;

    case '\'':
      g_sdx.dataType = SDX_DT_char;
      g_sdx.data = (Byte *) str + 6;
      g_sdx.dataLength = strlen (g_sdx.data);
      break;

    case '/':
      g_sdx.dataType = SDX_DT_UTF8;
      g_sdx.data = (Byte *) str + 6;
      g_sdx.dataLength = strlen (g_sdx.data);
      break;

    case '#':
      g_sdx.dataType = SDX_DT_numeric;
      sscanf (str + 6, "%d", &g_sdx.value);
      break;

    case '!':
      g_sdx.dataType = SDX_DT_float;
      sscanf (str + 6, "%lE", &g_sdx.fvalue);
      break;

    case 'x':
      break;

    case '<':
      first_array_id = 0;
      array_status   = array_running;
      array_count    = 0;
      continue;

    case '>':
      g_sdx.chunkID = first_array_id;   
      array_status  = array_finished;
      break;

    default:
      continue;
    }/*end-switch*/ 

    if ( array_status == array_running )
    {
      if ( first_array_id != 0 && first_array_id != g_sdx.chunkID )
      {
        printf ("error in SDEF array sequence: different ChunkID's\n");
        printf ("line number is: %d \n", incnt);
        abort ();
      }
      else
        first_array_id = g_sdx.chunkID;

      continue;
    }//end-if

    if ( array_status == array_finished ) 
    {
#ifdef STEP_BY_STEP
      int i;

      for (i = 0; i < array_count; ++i)
      {
        switch ( g_sdx.dataType )
        {
        case SDX_DT_numeric:    
          g_sdx.dataLength = 4;
          g_sdx.value      = int_array [i];
          break;
        case SDX_DT_float:
          g_sdx.dataLength = 8;
          g_sdx.fvalue     = double_array [i];
          break;
        }

        g_sdx.count = -1;
       /**********/
        SDX_create (&g_sdx);
       /**********/
        if (g_sdx.rc > SDX_RC_ok) {line = __LINE__; goto error;}
      }//end-for

    }//end-if array finished

#else // NOT STEP_BY_STEP
      g_sdx.count = array_count;
                 
      switch ( g_sdx.dataType )
      {
      case SDX_DT_numeric:    
        g_sdx.dataLength = 4;
        g_sdx.data       = (Byte *)int_array;
        break;
      case SDX_DT_float:      
        g_sdx.dataLength = 8;
        g_sdx.data       = (Byte *)double_array;
        break;
      }
    }//end-if array finished
    

#endif // STEP_BY_STEP or NOT

    if ( array_status == array_finished ) 
    {
      array_status = array_no;   
      g_sdx.count  = 0;
    }
    else
    {
      SDX_create (&g_sdx);
      if (g_sdx.rc > SDX_RC_ok) {line = __LINE__; goto error;}
    }

  }/*end-while */ 

  printf ("%04d SDEF records read\n", incnt);
  printf ("%04d SDXF records wrote\n", outcnt);
  fclose (infile);
  SDX_fileClose (outfile);
  return 0;

error:  
  printf ("convert error (%s): rc=%hd, ec=%hd, at %d\n",
          g_sdx.function, g_sdx.rc, g_sdx.ec, line);
  return 1;
}/*end- toSDXF */

/**************************************************************************/ 
int toSDEF (TParm p_parm)
{
  FILE   *infile, *outfile;
  int     incnt = 0, outcnt = 0, line;

  printf ("converting %s to SDEF file %s\n", p_parm.infile, p_parm.outfile);
  

  if (! (infile = SDX_fileOpen (p_parm.infile, 'r')))
  {
    printf ("could not open infile %s\n", p_parm.infile);
    return 1;
  }

  if (! (outfile = fopen(p_parm.outfile, "wt")))
  {
    printf ("could not open outfile %s\n", p_parm.outfile);
    return 1;
  }

  incnt  = 0;
  outcnt = 0;

  g_sdx.container  = NULL;
  g_sdx.bufferSize = BUFFLEN;
  g_sdx.dataType   = SDX_OPT_NEW;
  SDX_init (&g_sdx);
  if (g_sdx.rc > SDX_RC_ok) {line = __LINE__; goto error;}

  g_sdx.dataType   = SDX_DT_char;
  g_sdx.data       = "SDEF file created by SDXconvert";
  g_sdx.dataLength = strlen(g_sdx.data);
  g_sdx.chunkID    = 1;
  SDX_create (&g_sdx);
  SDEF_out   (&g_sdx, outfile, p_parm.elements_per_line);

  if ( p_parm.key )
  { 
    g_sdx.data = p_parm.key;
    SDX_setKey (&g_sdx); 
    g_sdx.encrypt  = 1;
  }

  while (SDX_readChunk (&g_sdx, infile))
  {
    ++incnt;
    for (;;)
    {
      if ( g_sdx.rc > SDX_RC_dataError ) {line = __LINE__; goto error;}

      switch (g_sdx.ec)
      {
        case SDX_EC_end_of_data :    /* end of group */ 
          SDEF_out (&g_sdx, outfile, 0);
          break;

        case SDX_EC_ok :    /* chunk */ 
          while (g_sdx.dataType == SDX_DT_structured) 
          {
            SDEF_out  (&g_sdx, outfile, 0);
            SDX_enter (&g_sdx);

            if (g_sdx.rc > SDX_RC_failed) 
            {
              SDEF_out  (&g_sdx, outfile, p_parm.elements_per_line);
              break; /*?? {line = __LINE__; goto error;} */ 
            }
            if (g_sdx.rc > SDX_RC_ok) continue;
          }
          SDEF_out (&g_sdx, outfile, p_parm.elements_per_line);
          break;

        default:
          if ( g_sdx.dataType == SDX_DT_structured ) {line = __LINE__; goto error;}
          SDEF_out (&g_sdx, outfile, p_parm.elements_per_line);
      } /* end-case*/ 

      if (g_sdx.level == 0) break;
      SDX_next (&g_sdx);

    }/*end-for */ 
  }/*end-while */ 

  if (g_sdx.rc > SDX_RC_failed) {line = __LINE__; goto error;}

  SDX_fileClose (infile);
  fclose (outfile);

  printf ("%04d SDXF records read\n", incnt);
  printf ("%04d SDEF records wrote\n", SDEF_out(NULL, NULL, 0));
  return 0;

error:  
  printf ("convert error (%s): rc=%hd, ec=%hd, at %d, Chunk# %d\n",
          g_sdx.function, g_sdx.rc, g_sdx.ec, line, incnt);
  return 1;
}/*end- toSDEF */

/**************************************************************************/ 
int SDEF_out (SDX_handle p_sdx, FILE *p_file, long p_nepl)
{
  static int  outcnt;
  char        line [LINELEN];
  char       *strp;
  long        nepl_cnt;

  if (p_sdx == NULL) return outcnt;

  if ( p_sdx->count > 0 ) // array
  {
    if ( p_sdx->count >= MAX_ARRAY_DIM )
    {              
      sprintf (line, "88888? ERROR: too much array elements: %d. Limit is %d\n", 
        p_sdx->count, MAX_ARRAY_DIM);

      g_error = true;
      fputs (line, p_file); 
      ++outcnt;
      return outcnt;
    }

    sprintf (line, "99999<%d\n", p_sdx->count);
    fputs (line, p_file);   
    ++outcnt;

#ifdef STEP_BY_STEP
    {//begin step-by-step block
      int  i, arr_count = p_sdx->count;
      char dt;

      switch (p_sdx->dataType)
      {
      case SDX_DT_numeric:  dt = '#';  break;
      case SDX_DT_float:    dt = '!';  break;

      default:
        strcpy (line, "88888? ERROR: data type not supported for arrays");
        strcat (line , "\n");
        g_error = true;
        fputs (line, p_file);   
        ++outcnt;
        return outcnt;
      }//end-switch dataType

      sprintf (line, "%05hu%c", p_sdx->chunkID, dt);
      nepl_cnt = 0;

      for ( i = 0; i < arr_count; ++i )
      {
        ++nepl_cnt;
        if ( strlen (line) > LINE_THRESHOLD || (p_nepl > 0 && nepl_cnt > p_nepl) )
        {
          nepl_cnt = 1;
          strcat (line , "\n");
          fputs (line, p_file);   
          ++outcnt;
          sprintf (line, "%05hu%c", p_sdx->chunkID, dt);
        }

        p_sdx->count = -1;            // step-by-step indicator
        SDX_extract (p_sdx);
        if (p_sdx->rc > SDX_RC_ok) return outcnt;

        switch (p_sdx->dataType)
        {
        case SDX_DT_numeric:
          sprintf (line, "%s%d ", line, p_sdx->value);
          break;

        case SDX_DT_float:
          sprintf (line, "%s%G ", line, p_sdx->fvalue);
          break;

        default:
          strcpy (line, "88888? ERROR: data type not supported for arrays");
          g_error = true;
      }//end-switch dataType
      }//next i: array element
    }//end step-by-step block

#else // NOT STEP_BY_STEP
    switch (p_sdx->dataType)
    {
      int    i;
      int    int_array    [MAX_ARRAY_DIM]; 
      double double_array [MAX_ARRAY_DIM]; 

    case SDX_DT_numeric:
      p_sdx->data       = (Byte *)int_array;
      p_sdx->dataLength = 4;
      SDX_extract (p_sdx);
      if (p_sdx->rc > SDX_RC_ok) return outcnt;
  
      sprintf (line, "%05hu#", p_sdx->chunkID);
      nepl_cnt = 0;

      for (i=0; i < p_sdx->count; ++i)
      {
        ++nepl_cnt;
        if ( strlen (line) > LINE_THRESHOLD || (p_nepl > 0 && nepl_cnt > p_nepl) )
        {
          nepl_cnt = 1;
          strcat (line , "\n");
          fputs (line, p_file);   
          ++outcnt;
          sprintf (line, "%05hu#", p_sdx->chunkID);
        }
        sprintf (line, "%s%d ", line, int_array [i]);
      }//end-for array elements
      break;

    case SDX_DT_float:
      p_sdx->data       = (Byte *)double_array;
      p_sdx->dataLength = 8;
      SDX_extract (p_sdx);
      if (p_sdx->rc > SDX_RC_ok) return outcnt;

      sprintf (line, "%05hu!", p_sdx->chunkID);
      nepl_cnt = 0;

      for (i=0; i < p_sdx->count; ++i)
      {
        ++nepl_cnt;
        if ( strlen (line) > LINE_THRESHOLD || (p_nepl > 0 && nepl_cnt > p_nepl) )
        {
          nepl_cnt = 1;
          strcat (line , "\n");
          fputs (line, p_file);   
          ++outcnt;
          sprintf (line, "%05hu!", p_sdx->chunkID);
        }
        sprintf (line, "%s%G ", line, double_array [i]);
      }//end-for array elements
      break;

    default:
      strcpy (line, "88888? ERROR: data type not supported for arrays");
      g_error = true;
  }//end-switch dataType
#endif // STEP_BY_STEP or NOT

    strcat (line , "\n");
    fputs (line, p_file);   
    ++outcnt;

    sprintf (line, "99999>");
  }
  else  
  {
    if ( p_sdx->dataType != SDX_DT_structured )
    {
      p_sdx->data = line + 6;
      p_sdx->maxLength = LINELEN - 6;
      SDX_extract (p_sdx);
      if (p_sdx->rc > SDX_RC_ok) return outcnt;
    }

    if (p_sdx->rc > SDX_RC_failed)
    {
      sprintf (line, "%05hu? ERROR rc=%d ec=%d", p_sdx->chunkID, 
        p_sdx->rc, p_sdx->ec);
      g_error = true;
      if ( p_sdx->dataType != SDX_DT_structured ) p_sdx->rc = 0; 
    }
    else
      switch (p_sdx->dataType)
      {
      case SDX_DT_structured:
        sprintf (line, "%05hu%c", p_sdx->chunkID, 
          p_sdx->ec == SDX_EC_ok ? '(' : ')');
        break;
      case SDX_DT_char:
        sprintf (line, "%05hu'%s", p_sdx->chunkID, p_sdx->data);
        break;
      case SDX_DT_UTF8:
        sprintf (line, "%05hu/%s", p_sdx->chunkID, p_sdx->data);
        break;
      case SDX_DT_numeric:
        sprintf (line, "%05hu#%d", p_sdx->chunkID, p_sdx->value);
        break;
      case SDX_DT_float:
        sprintf (line, "%05hu!%G", p_sdx->chunkID, p_sdx->fvalue);
        break;
      case SDX_DT_binary:
        sprintf (line, "%05hux%s", p_sdx->chunkID, 
          BT_hexan (p_sdx->data, p_sdx->dataLength));
        break;
      default:
        strcpy (line, "88888'ERROR: unknown data type");
        g_error = true;
      }//end-switch
  }//end-if array or not
  
  do
  {
    strp = strstr (line, "\n");
    if (strp == NULL) break;
    *strp = '\\';
  } while (1);

  strcat (line , "\n");
  fputs (line, p_file); 
  ++outcnt;
  return outcnt;
} /*end- SDEF_out */
