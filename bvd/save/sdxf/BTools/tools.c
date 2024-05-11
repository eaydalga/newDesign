/* doc-----------------------------------------------------------------------------
:Project.   common tools
:Program.   tools.c
:PrType.    function lib
:Title.     some common usable functions
:Version.   
:Author.    Max Wildgrube
:Language.  ANSI-C
:Compiler.
:Created.   
---------------------------------------------------------------------------------*/ 

/**+******************************************************************
**
**  BT_strZTerm   : Converts from NATURAL string to C string.
**                  (target and source can be the same buffer)
**
**  BT_strifill   : fills an ASCIIZ string with blanks
**                  C-string -->  NAT-string
**
**  BT_scanhex    : edits string of hex chiffres into a binary string (2->1)
**                  non-hex chiffre (0-F) will be overread 
**
**                  proto: 
**                  utlen = BT_scanhex (Byte *target, char *source, long maxlength)
**                  source = "ABC1.23" returns target = 0xABC123
**
**  BT_hexedit    : output (via printf) lines * 16 bytes in dump format.
**                  1st param should be the (relative) address of the 1st byte.
**                  proto: void BT_hexedit (int a, Byte *b, int lines);
**
**  BT_hexeditline: edits 16 bytes to a dump format.
**                  1st param should be the address of the 1st byte.
**                  returns a char * to an internal string, whichs is valid
**                  til next call of BT_hexeditline.
**                  proto: char *BT_hexeditline (int a, Byte *b);
**
**  BT_hexa       : edit up to 16 bytes of a binary field to hex digits
**                  returns a char * to an internal string, whichs is valid
**                  until next call of BT_hexa
**                  proto: char *BT_hexa (Byte *);
**  BT_hexan      : edit in a given length
**
**  BT_StrUpr     : tranform an ASCIIZ string into upper cases
**                  (with the aid of function toupper() ).
**                  The string will be transformed in place,
**                  nevertheless the pointer of this string is returned.
**                  proto: char *BT_StrUpr (char * psStr);
**
**  BT_invTab     : Inverts a table of size 256 (A -> A-1)
**                  in: const Byte *inTab
**                  returns: Byte *
**                  the memory for the out-tab will be allocated
**
**  BT_adaptD, BT_adaptS, BT_adaptL :
**                  depends from the processor architecture: made the swaps
**                  or not.
**
**  BT_printf :     with option "user defined function"
**                  set by ...
**
**  BT_set_printf : set alternate function for printf
**  BT_set_prompt : set prompt option for function BT_protocol
**  BT_protocol   : example for an alternate function: output on stdout
**                  with time-stamp and more handling.
**
**  BT_time_string: returns UNIX style date & time
**
**  BT_assertf    : tests a condition, if fails, printf a formatted
**                  string with replacements and exits program.
**                  in: bool, char *, ...
**
**  BT_striset    : like the WIN95 function _strset : fills a string
**                  with the specified character
**
**  BT_convert    : converts a char-buffer via a conversion table
**
**  BT_copymem    : copies a chunk of memory into another 
**
**  BT_malloc     : secure malloc
**  BT_free       : secure free
**
**-*****************************************************************/


#include <string.h>   /* str..., mem... */
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include "tools.h"

#undef  malloc 
#undef  free

/* implementation */

void TL_dmy() {}  /* dummy for MVS */

#define ML_INDICATOR ':'

/* globals: */
static char  g_filename [BT_LENFILENAME];      /* altfopen is _not_ multithread capable */ 
static bool  g_running       = true;           /* not a real multithread problem        */
static short g_message_level = BT_ML_STANDARD; /* not a real multithread problem        */
void         protocol_exit (void);

/**+********************************************************************
**
** Function Descr.   : BT_strZTerm
**                     Converts from NATURAL string to C string.
**                     target and source can be the same buffer
** Parameters        : char *    -O   target C string or NULL
**                     char *    -IO  NATURAL string
**                     long      -I   NATURAL string size
** Return value      : char *         C string
**
**-******************************************************************/
char *BT_strZTerm (char *p_target, char *p_source, long p_len)
{
  long len;

  /* Get string length: */
  len = p_len;
  while ( len-- > 0 && p_source [len] == ' ');

  if (len < p_len - 1) ++len;

  if (p_target)
  {
    memcpy(p_target, p_source, len);
    p_target [len] = 0;
    return p_target;
  }
  else
  {
    p_source [len] = 0;
    return p_source;
  }

} /*end- BT_strZTerm */

/**+*****************************************************************
**
** Function Descr.   : BT_strifill
**                     Converts from C string to NATURAL string.
**                     source and target can be the same buffer
** Parameters        : p_target  -IO  NATURAL string
**                     p_source  -I   C string
**                     p_length  -I   NATURAL string size
** Return value      : char *         NATURAL string
** Functions used    :
**
**-******************************************************************/
char *BT_strifill (char *p_target, char *p_source, int p_length)
{
  int len;

  len = strlen ((char *)p_source);

  if (p_source != p_target) memcpy (p_target, p_source, len);

  for (; len < p_length; len++) *(p_target + len)=' ';

  return p_target;

} /*end- BT_strifill */

/**+*****************************************************************
**
** Function Descr.   : BT_StrUpr
**                     Converts an string to uppercase
** Parameters        : psStr     -IO  String
** Return value      : char *      String
** Functions used    :
**
**-*****************************************************************/
char *BT_StrUpr (char *p_sStr)
{
  char *psOrig = p_sStr;

  for (; *p_sStr; ++p_sStr) *p_sStr = (char)toupper(*p_sStr);

  return psOrig;

} /*end- BT_StrUpr */


/****************************************************************/
#define chars

#ifdef chars

/****************************************************************/
#define NILCHAR '.'
static Byte p(Byte c)
{
  if ((c < ' ') || (c > 0x7E)) return NILCHAR; else return c;
}

#endif /*chars*/

/****************************************************************/
char *BT_hexa (Byte *b)
{
  static char line [90];  //!!!MT
  int len;

  sprintf
    (  line
    , "%02X%02X%02X%02X %02X%02X%02X%02X "
      "%02X%02X%02X%02X %02X%02X%02X%02X"
    ,   b[ 0],    b[ 1],    b[ 2],    b[ 3]
    ,   b[ 4],    b[ 5],    b[ 6],    b[ 7]
    ,   b[ 8],    b[ 9],    b[10],    b[11]
    ,   b[12],    b[13],    b[14],    b[15]
    );

  len = strlen (b);
  if (len > 16) len = 16;
  len *= 2;

  if (len >  8) ++len;
  if (len > 16) ++len;
  if (len > 24) ++len;
  line [len] = 0;
  return line;

}/*end- BT_hexa */

/****************************************************************/
char *BT_hexan (Byte *b, int p_len)
{
  static char line [90]; //!!!MT

  sprintf
    (  line
    , "%02X%02X%02X%02X %02X%02X%02X%02X "
      "%02X%02X%02X%02X %02X%02X%02X%02X"
    ,   b[ 0],    b[ 1],    b[ 2],    b[ 3]
    ,   b[ 4],    b[ 5],    b[ 6],    b[ 7]
    ,   b[ 8],    b[ 9],    b[10],    b[11]
    ,   b[12],    b[13],    b[14],    b[15]
    );

  if (p_len > 16) p_len = 16;
  p_len *= 2;

  if (p_len >  8) ++p_len;
  if (p_len > 16) ++p_len;
  if (p_len > 24) ++p_len;
  line [p_len] = 0;
  return line;

}/*end- BT_hexan */

/****************************************************************/
char *BT_hexeditline (int a, Byte *b)
{

static char line [90]; //!!!MT
#ifdef chars

  sprintf (line, "%04X : %02X%02X%02X%02X %02X%02X%02X%02X"
                 " %02X%02X%02X%02X %02X%02X%02X%02X"
                 " >%c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c<",
          a,
          b[ 0],    b[ 1],    b[ 2],    b[ 3],
          b[ 4],    b[ 5],    b[ 6],    b[ 7],
          b[ 8],    b[ 9],    b[10],    b[11],
          b[12],    b[13],    b[14],    b[15],
        p(b[ 0]), p(b[ 1]), p(b[ 2]), p(b[ 3]),
        p(b[ 4]), p(b[ 5]), p(b[ 6]), p(b[ 7]),
        p(b[ 8]), p(b[ 9]), p(b[10]), p(b[11]),
        p(b[12]), p(b[13]), p(b[14]), p(b[15]));
#else
  sprintf (line, "%02X%02X%02X%02X %02X%02X%02X%02X  ",
        b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7]);
#endif /*chars*/
  return line;
}/*end- BT_hexeditline */

/****************************************************************/
void BT_hexedit (int p_a, Byte *p_b, int p_lines, const int p_mi)
{
  int  ii;
  char str [] = ":?%s\n";

  str [1] = '0' + p_mi;

  for (ii = 0; ii < p_lines; ++ii)
  {
    BT_printf (str, BT_hexeditline (p_a, p_b));
    p_a += 16;
    p_b += 16;
  }

}/*end- BT_hexedit */

/****************************************************************/
long BT_scanhex (Byte *p_target, char *p_source, long p_maxlength)
{
  int  is = 0, it = 0;
  bool left_nibble = true;
  char ttab [] = "0123456789ABCDEF";
  char *ptr;
  Byte b;

  while (p_source [is])
  {
    ptr = strchr (ttab, p_source [is]);
    if (ptr)
    {
      if (left_nibble)
        b = (ptr - ttab) << 4;
      else
      {
        b |= (Byte)(ptr - ttab);
        p_target [it] = b;
        ++it;
        if (it > p_maxlength) return -1;
      } /* end which nibble */ 
      left_nibble = ! left_nibble;

    } /*end- is BT_hexa chiffre */ 

    ++is;
  }

  if (left_nibble) p_target [it] = b; /* odd number of nibbles: 0 appended */ 

  return it;
}/*end- BT_scanhex */

/****************************************************************/

double BT_adaptD (double parm)
{
  double *ptr = &parm;

  union
  {
    double eight;
    struct
    {
      Byte byte [8];
    } b;
  } in, out;

  if (BT_adaptS(4711) == 4711) return parm;

  in.eight = parm;

  out.b.byte [0] = in.b.byte [7];
  out.b.byte [1] = in.b.byte [6];
  out.b.byte [2] = in.b.byte [5];
  out.b.byte [3] = in.b.byte [4];
  out.b.byte [4] = in.b.byte [3];
  out.b.byte [5] = in.b.byte [2];
  out.b.byte [6] = in.b.byte [1];
  out.b.byte [7] = in.b.byte [0];

  return out.eight;

}/*end- BT_adaptD */

float BT_adaptF (float parm)
{
  float *ptr = &parm;

  union
  {
    float four;
    struct
    {
      Byte byte [4];
    } b;
  } in, out;

  if (BT_adaptS(4711) == 4711) return parm;

  in.four = parm;

  out.b.byte [0] = in.b.byte [3];
  out.b.byte [1] = in.b.byte [2];
  out.b.byte [2] = in.b.byte [1];
  out.b.byte [3] = in.b.byte [0];

  return out.four;

}/*end- BT_adaptF */

long BT_adaptL (long parm)
{
  long *ptr = &parm;

  return (long)
     ( (((long)*((char *)ptr  ) & 0xFF) << 24)
     | (((long)*((char *)ptr+1) & 0xFF) << 16)
     | (((long)*((char *)ptr+2) & 0xFF) <<  8)
     | (((long)*((char *)ptr+3) & 0xFF)      )
     );

}/*end- BT_adaptL */

short BT_adaptS (short parm)
{
  short *ptr = &parm;

  return (short)
     ( (((short)*((char *)ptr  ) & 0xFF) << 8)
     | (((short)*((char *)ptr+1) & 0xFF)     )
     );

}/*end- BT_adaptS */


/****************************************************************/
Byte *BT_invTab (const Byte *p_inTab)
/**************************************
  build the Inversion of a 256-array
**************************************/
{
  int  ix;
  Byte *outTab;

  if (p_inTab  == NULL) return NULL;
  outTab = malloc (256);
  if (outTab == NULL) return NULL;

  for (ix=0; ix<256; ++ix) outTab [ix] = NIL;
  for (ix=0; ix<256; ++ix) outTab [p_inTab[ix]] = ix;

  return outTab;
}/*end- BT_invTab */



/****************************************************************
** BT_printf     : with option "user defined function"
**                 set by ...
**
** BT_set_printf : set alternate function for printf
** BT_set_prompt : set prompt option for function BT_protocol
** BT_set_message_level : set message level for function BT_printf
** BT_get_message_level : returns global message level 
** BT_protocol   : example for an alternate function: output on stdout
**                 with time-stamp and 'more' handling.
****************************************************************/

static TPrintfProc *g_myPrintf = NULL;  //!!!MT
//static FILE        *g_loggfile = NULL;

/*-----------------------------------------------------------------*/
int BT_printf (const char *p_fmt, ...)
{
   va_list list;
   char    line [BT_LOG_LINE_LENGTH];
   char    mi;

   if ( p_fmt )
   {
     if ( p_fmt [0] == ML_INDICATOR )
     {
       mi = p_fmt [1];

       if ( mi >= '0' || mi <= '9' )
       {
         if ( mi > g_message_level + '0' ) return 0;
         p_fmt += 2;
       }
     }
     else
       mi = ' ';

     va_start (list, p_fmt);     /* Initialize variable arguments. */

     if ( g_myPrintf ) 
     {
       vsprintf (line, p_fmt, list);
       return g_myPrintf (line, mi);      
     }
     else
       return vprintf (p_fmt, list);
   }
   else
   {
     if ( g_myPrintf )
       return g_myPrintf (NULL, 0);
     else
       return 0;
   }

} /*end- BT_printf */

/*-----------------------------------------------------------------*/
void BT_set_printf (TPrintfProc *p_proc)
{
  g_myPrintf  = p_proc;    //!!!MT

} /*end- BT_set_printf */

/*-----------------------------------------------------------------*/
void BT_set_message_level  (short p_message_level)
{
  g_message_level = p_message_level;    //!!!MT
  BT_printf (":6BTools: Message Level set to %d\n", g_message_level);

} /*end- BT_set_mess_level */

/*-----------------------------------------------------------------*/
short BT_get_message_level ()
{
  return g_message_level;
} /*end- BT_set_mess_level */

/*-----------------------------------------------------------------*/
void BT_set_1st_printf (TPrintfProc *p_proc)
{
  if ( g_myPrintf ) return;    
  g_myPrintf  = p_proc;   //!!!MT  

} /*end- BT_set_printf */

/*-----------------------------------------------------------------*/
void BT_set_prompt (bool p_prompt)
{
  g_running = ! p_prompt;  //!!!MT

} /*end- BT_set_prompt */

/*-----------------------------------------------------------------*/
int BT_protocol     (const char *p_fmt, const char p_mi)
#define PRLINES 19
{
  static int  linecount     = 0;      //!!!MT
  static bool linecontinued = false;  //!!!MT
  static bool firstpass     = true;   //!!!MT
  int    ix, cnt;
  char   c;

  if ( firstpass )
  {
    firstpass = false;
    atexit (protocol_exit);
  }

  ix = strlen (p_fmt);

  if ( linecontinued )
    cnt = printf ("\n            %s", p_fmt);
  else
    if ( g_message_level >= BT_ML_TRACE )
      cnt = printf ("%s %c -- %s", BT_time_string (TS_date_time), p_mi, p_fmt);
    else
      cnt = printf ("%s -- %s", BT_time_string (TS_date_time), p_fmt);

  linecontinued = (p_fmt [ix-1] != '\n');

  ++linecount;

  if ( g_running ) return cnt;

  if ( linecount == PRLINES )
  {
    linecount = 0;
    printf ("----- more - ");
    c = getchar ();
    if ( c == 'r' ) g_running = true;
  }

  return cnt;

}/*end- BT_protocol */

/*-----------------------------------------------------------------*/
void protocol_exit (void)
{
  char   c;

  if (g_running) return;

  printf ("----- that's it - ");
  c = getchar ();

}/*end- protocol_exit  */

/****************************************************************
**  BT_time_string: returns  date &/ time
****************************************************************/
char *BT_time_string (int p_type)
{
  static char str [30];    //!!!MT
  time_t     ltime;
  struct tm *tmtime;

  time( &ltime );
  tmtime = localtime (&ltime);

  switch (p_type)
  {
    case TS_date_time: strftime (str, 20, "%Y-%m-%d %H:%M:%S", tmtime); break;
    case TS_date     : strftime (str, 11, "%Y-%m-%d", tmtime);          break;
    case TS_time     : strftime (str,  9, "%H:%M:%S", tmtime);          break;
  }

  return str;
}/*end- BT_time_string */

/*************************************************************
**  BT_assertf : tests a condition, if fails, printf a
**     formatted string with replacements and exits program.
**     in: bool, char *, ...
*************************************************************/
void BT_assertf (bool p_cond, char *p_text, ...)
{
  va_list list;

  if (p_cond)  return;

  if ( p_text [0] == ML_INDICATOR ) p_text +=2;
  va_start (list, p_text);
  vprintf (p_text, list);

  exit(EXIT_FAILURE);
}/* end-func BT_assertf  */

/*************************************************************
** BT_striset : like the WIN95 function _strset : fills a
**     string with the specified character
*************************************************************/
void BT_striset (char *p_str, char p_char)
{
  int ix = 0;

  while (p_str [ix]) p_str[ix++] = p_char;
} /*end- BT_striset */

/****************************************************************
/** BT_convert : converts a char-buffer via a conversion table
**
/****************************************************************/
void BT_convert (Byte *p_out, Byte *p_in, long p_len, const Byte *p_tab)
{

  if (!p_len) return;
  if (!p_out) return;
  if (!p_in)  return;
  
  if (!p_tab)
  {
    if (p_out != p_in) memcpy (p_out, p_in, p_len);
    return;
  }

  while (p_len)
  {
    *p_out = p_tab [*p_in];
    ++p_out;
    ++p_in;
    --p_len;
  }

// oder so? :  while (p_len--) *(p_out++) = p_tab [*(p_in++)];

} /* end BT_convert */


/****************************************************************
** BT_altfopen       : tries to open file with 2.nd param as
**                     filename. if failed:
**                     tries to get the filename via the 1. param
**                     as environment variable. if envir. failed
**                     returns -1, else:
**                     tries to open file with value of environment
**                     variable. returns filehandle or NULL
** Parameters        : envirvarname   -I  char*
**                   : filename       -I  char*
**                   : open mode      -I  char*
** Return value      : FILE*
/****************************************************************/
FILE *BT_altfopen (char *p_env, char *p_file, char *p_mode)
{                                                 /* not thread-safe !!!MT  */ 
  FILE *fh;
  char *name;

  /*------------------ definition for BS2000 --------------------------*/
  #if defined _SYS_BS2000

  char envir [20] = "LINK=";

  g_filename [0]  = 0;
  name            = p_env;

  strcat (envir, p_env);
  fh = fopen (envir, p_mode);

  if (! fh)
  {
    if (p_file == NULL) return (FILE *)-1;

    fh = fopen (p_file, p_mode);
    name = p_file;
  }

  if (fh)
  {
    strncpy (g_filename, name, LENFILENAME);
    BT_printf (":4IO-TOOLS: file %s opened.\n", name);
  }

  /*------------------ definition for MVS --------------------------*/
  #elif defined _SYS_MVS

  char *envir    = p_env;
  char  dsn [60] = "DSN=";

  g_filename [0] = 0;
  name           = p_env;
  fh             = fopen (envir, p_mode);

  if (! fh)
  {
    if (p_file == NULL) return (FILE *)-1;

    name = p_file;
    strcat (dsn, p_file);
    fh = fopen (p_file, p_mode);
  }

  if (fh)
  {
    strncpy (g_filename, name, LENFILENAME);
    BT_printf (":4IO-TOOLS: file %s opened.\n", name);
  }


  /*--------------- definition for UNIX, Windows -------------------*/
  #else

  char *envir;

  g_filename [0] = 0;
  name           = p_file;

  if ((fh = fopen (p_file, "r"))) /* exists file in curr. dir.? */
  {
    fclose (fh);
    fh = fopen (p_file, p_mode);  /* yes: open in desired mode */
  }
  else
  {
    envir = getenv (p_env);       /* no: try content of envir var */  

    if (envir == NULL) return (FILE *)-1;

    fh = fopen (envir, p_mode);
    name = envir;
  }

  if (fh)
  {
    strncpy (g_filename, name, BT_LENFILENAME);
    BT_printf (":4IO-TOOLS: file %s opened.\n", name);
  }


  #endif
  /*--- common end ----*/

  return fh;

}/*end- BT_altfopen*/


/*****************************************************************
** BT_getAltFileName : returns the filename used in the last 
**                     recently call of BT_altfopen. This string 
**                     will be overrriden by the next call of 
**                     BT_altfopen!
** Parameters        : none
** Return value      : char*
*****************************************************************/
char *BT_getAltFileName (void)
{
   return g_filename;
}/*end- BT_getAltFileName*/

/*************************************************************/
bool BT_readTable (Byte *p_tab, FILE *p_fh)
 {
   #define TABLEWIDTH 80

   char line [TABLEWIDTH];
   int  i, j;
   unsigned int lv[16];

   if (!p_fh) return false;

   i=0; 
   for (;;)
   {
     if (fgets (line, TABLEWIDTH, p_fh) == NULL) return (i==256);

     switch ( line[0] ) 
     { 
       case '*'  :
       case ';'  :
       case 0x0A :
         break;
       default   : 
         j= sscanf(line, 
             "%2x %2x %2x %2x %2x %2x %2x %2x "
             "%2x %2x %2x %2x %2x %2x %2x %2x ",  
             &lv[0],  &lv[1],  &lv[2],  &lv[3],  &lv[4],  &lv[5],  
             &lv[6],  &lv[7],  &lv[8],  &lv[9],  &lv[10], &lv[11], 
             &lv[12], &lv[13], &lv[14], &lv[15] );

         if (j!=16) return false;

         for (j=0; j<16; ++j) p_tab[i+j] = (Byte)lv[j];

         i += 16;
         if (i==256) return true; 
     }/*end-switch '*', ... */ 
   }/*end-for lines, i*/ 

 } /*end-BT_readTable */ 

/*****************************************************************
** BT_copymem        : copies a chunk of memory into another 
** Parameters        : in: target-ptr, source-ptr, length
** Return value      : none
**/
DllExport void CC BT_copymem (Byte *p_to, Byte *p_from, long p_len)
{
  memcpy (p_to, p_from, p_len);
}

/*****************************************************************
** BT_malloc         : secure malloc
** Parameters        : in: long memsize, char* program, long line_number
** Return value      : ptr to allocated memory
**/
DllExport void *BT_malloc (long p_len, char *p_prog, long p_line)
{
  void *ptr;

  ptr = malloc (p_len);
  BT_printf (":7BT--malloc for %04x\n", ptr);   //test
  if (ptr) return ptr;

  BT_printf (":0Allocation of memory failed! Size: %d, in Module %s, at line %d\n",
    p_len, p_prog, p_line);
  BT_printf (":0Program aborted!\n");

  BT_printf (":7BT--malloc for '%s'\n", ptr);

  exit(EXIT_FAILURE);
}

/*****************************************************************
** BT_free           : secure free
** Parameters        : in: void* memory
** Return value      : none

This function is necessary as a pendant to BT_malloc in the Win32 DLL 
environment: memory allocated in a DLL environment must be freed also with
a DLL function (The same is valid for the user env.)
This is no problem under UNIX (Linux) with its .so libs.
**/
DllExport void BT_free (void *p_mem)
{
  BT_printf (":7BT--free for adr=%04x, '%s'\n", p_mem, p_mem);   //test
  #ifdef _DEBUG
    if (_CrtIsValidHeapPointer(p_mem))
      free (p_mem);
    else
      BT_printf (":7BT--free: could not free adr=%04x, '%s'\n", p_mem, p_mem);   //test
  #else
    free (p_mem);
  #endif

}


/********************************************************************************/
DllExport char *BT_alloc_str (const char *p_str)
{
  char *ptr = malloc (strlen (p_str) + 2);
  if (! ptr) return NULL;

  strcpy (ptr, p_str);   /*secure!*/ 
  BT_printf (":7BT--alloc-str for adr=%04x, '%s'\n", ptr, ptr);   //test
  return ptr;


}

/*---------------- end of source tools.c ----------*/
