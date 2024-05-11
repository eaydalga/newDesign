/* doc----------------------------------------------------------------
:Project.   common tools
:Program.   tools.h
:PrType.    header file
:Title.     some common usable functions
:Version.   0.2
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI-C
:Compiler.
----------------------------------------------------------------------*/
/**
**  tools.h : some common usable functions 
**
** The meanings of the message levels are:                    
**      ML            Level                                   */
#define BT_ML_ALWAYS    0  /* putout always                   */
#define BT_ML_QUIET     1  /* quiet mode (Log-File)           */
#define BT_ML_Q2        2  /* quiet mode (Dialog + Log-File)  */
#define BT_ML_STANDARD  3  /* short: S                        */
#define BT_ML_VERBOSE   4  /* short: V                        */
#define BT_ML_EXTENSIVE 5  /* short: E                        */
#define BT_ML_TRACE     6  /* short: T                        */
#define BT_ML_ET        7  /* exentensive Trace               */
/**                     9  /* "commented out" - never printed */

/**
** Overview: 
**
** BT_strZTerm    : Converts from NATURAL string to C string.
** BT_strifill    : Converts from C string to NATURAL string.
** BT_StrUpr      : Converts an string to uppercase
** BT_invTab      : generates the inverse of a table
** BT_adaptL      : returns the swapped version of a long, short and double
** , BT_adaptS      (in a non-swap environment these are only memcpy's) 
** , BT_adaptD      
** , BT_adaptF
** BT_hexedit     : output (via printf) lines * 16 bytes in dump format.
** BT_hexa        : edit up to 16 bytes of a binary field to hex digits
** BT_scanhex     : transforms hexa digits to there binary values 
** BT_assertf     : assert function with 2 or more params
** BT_altfopen    : tries to open a file via an environm. var or filename
** BT_getAltFileName : returns the filename used in the last recently
**                  call of BT_altfopen. 
** BT_printf	  	: replacement for printf : works like printf until ...
**  BT_set_printf : defines a replacement.
**  BT_set_1st_printf : defines a replacement, but only if there is none.
**  BT_protocol   : example for such a replacement.  
**  BT_set_prompt : set prompt option for function BT_protocol
**  BT_set_mess_level : set message level for function BT_printf
** BT_time_string : returns actual date & time in UNIX style 
** BT_striset     : fills a string with the specified character
** BT_convert     : converts a char-buffer via a conversion table  
** getlogin       : returns the userid = Win user for Win32
** BT_readTable   : loads a conversion table from file
** BT_copymem     : copies a chunk of memory into another (for non-C languages)
** BT_malloc      : replacement for malloc: abort program if no memory
** BT_free        : replacement for free: environment preserving pendant to BT_malloc
**/ 
#define BT_LENFILENAME     200
#define BT_LOG_LINE_LENGTH 200

#ifndef tools_h
#define tools_h

#include "sysdef.h"
#include <stdio.h>
#include <time.h>

#ifdef _SYS_UNIX
#  include <unistd.h>
#endif

						  /* prototypes */ 

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/*****************************************************************
** Function     : BT_printf, BT_set_printf, BT_set_mess_level
** Descr.       : replacement for printf : works like printf until
**                BT_set_printf is given with a procedure name: then 
**                this procedure will be called with the edited string
**                BT_set_mess_level: set message level for function BT_printf 
** Parameters   : BT_printf: same as printf
** Parameters   : BT_set_printf: TprintfProc *proc
** Parameters   : BT_set_mess_level: short message_level
**                whereas: typedef int TprintfProc (const char *);
**                this "proc" will be called when BT_printf is called,
**                param char* contains the edited (sprintf'ed) line to 
**                print. 
**                If line to print (message) begins with ':' (threshold indicator)
**                the next char is the threshold for this message: it will be 
**                printed if threshold is lower or equal message_level.
**                If message has no indicator, threshold is defaulted to 3.
** Return value : BT_printf: same as printf
** Return value : BT_set_printf: none
** Return value : BT_set_mess_level: none
**
**/

typedef   int   TPrintfProc          (const char *, char);        /*!!!typedefs*/ 
DllExport void  BT_set_printf        (TPrintfProc *proc);
DllExport void  BT_set_message_level (short message_level);
DllExport short BT_get_message_level ();
DllExport void  BT_set_1st_printf    (TPrintfProc *proc);
DllExport void  BT_set_prompt        (bool prompt);
DllExport int   BT_printf            (const char *fmt, ...);

/*****************************************************************
** Function     : BT_protocol
** Descr.       : ouput on stdout with time stamp and more-handling
** Parameters   : char*  - string edited by BT_printf
** Return value : int    - number of printed chars  
**/
DllExport int  BT_protocol    (const char *line, const char mi);

/*****************************************************************
** Function     : BT_adaptD, BT_adaptF, BT_adaptL, BT_adaptS
** Descr.       : returns the swapped version of a long or short
**                (in a non-swap environment these are only dummies) 
** Parameters   : parm    -I   long (or short) 
** Return value : long (or short) 
** Hint         : old names are swplong, swpshort, #defines for 
**                compatibility  
**/

DllExport  double BT_adaptD  (double parm);
DllExport  float  BT_adaptF  (float  parm);
DllExport  long   BT_adaptL  (long   parm);
DllExport  short  BT_adaptS  (short  parm);


/*****************************************************************
** Function     : BT_scanhex
** Descr.       : transforms hexa digits to there binary values:
**                2 digts becomes a byte
**                non-BT_hexa chiffres (0-F) will be overread 
**                a 0x0 will be appended for odd numbers of digits
** Parameters   : target    -O  Byte*
**                source    -I  char*
**                maxlength -I  of target
** Return value : length of translated chars in target, 
**                -1 if target too short 
** Example      : source = "ABC1.23" returns target = 0xABC123
**/
DllExport  long BT_scanhex (Byte *target, char *source, long maxlength);

/*****************************************************************
** Function     : BT_hexedit
** Descr.       : output (via printf) lines * 16 bytes in dump format.
**                1st param should be the address of the 1st byte.     
** Parameters   : a      -I  int  
**                b      -I  Byte 
**                lines  -I  int  
** Return value : none
**/
DllExport  void BT_hexedit (int a, Byte *b, int lines, const int mi);

/*****************************************************************
** Function     : BT_hexa 
** Descr.       : edit up to 16 bytes of a binary field to hex digits
** Parameters   : a      -I  int     start offset
**              : b      -I  ->Byte  source to edit  
**              : mi     -I  int     message indicator  
** Return value : char *  - edited string (in an internal buffer)
**/
DllExport char *BT_hexa  (Byte *);

/*****************************************************************
** Function     : BT_hexan 
** Descr.       : edit up to 16 bytes of a binary field to hex digits
**                in a given lengh
** Parameters   : b      -I  ->Byte 
**              : len    -I  int  
** Return value : char *  - edited string (in an internal buffer)
**/
DllExport char *BT_hexan (Byte *b, int len);

/*****************************************************************
** Function     : BT_assertf
** Descr.       : assert function with 2 or more params: the 2nd 
**                param is an error text, the 3rd and further are
**                replacements
**                var: a printf(2nd, ...);  will be done.                    
** Parameters   : cond  -I   bool  
**                text  -I   char*
**                repl  -I   char*, ...  
** Return value : none
**/
DllExport void BT_assertf (bool cond, char *text, ...);

/*****************************************************************
** Function     : BT_strZTerm
** Descr.       : Converts from NATURAL string to C string.
** Parameters   : char *    -O   target C string or NULL
**                char *    -IO  NATURAL string
**                long      -I   NATURAL string size
** Return value : char *         C string               
**/
DllExport  char *BT_strZTerm (char *target, char *source, long len);

/*****************************************************************
** Function     : BT_strifill
** Descr.       : Converts from C string to NATURAL string.
**                pcNatStr and psCstr can be the same buffer
** Parameters   : pcNatStr  -IO  NATURAL string
**                psCstr    -I   C string             
**                lNatSize  -I   NATURAL string size
** Return value : char*      NATURAL string               
**/
#ifdef _SYS_WIN16x
  char *_BT_strifill (char *target, char *source, int length);
  #define BT_strifill _BT_strifill
#else
  DllExport char *BT_strifill (char *target, char *source, int length); 
#endif 
  
/*****************************************************************
** Function     : BT_StrUpr
** Descr.       : Converts an string to uppercase
** Parameters   : psStr     -IO  String          
** Return value : char*      String               
**/
DllExport char *BT_StrUpr (char * sStr);

/*****************************************************************
** Function     : BT_adaptL       
** Descr.       : generates the inverse of a table
** Parameters   : intab     -I  char[256]          
** Return value : char*      String, length=256               
**/
DllExport  Byte *BT_invTab (const Byte *inTab);
																			  
/*****************************************************************
** Function     : BT_altfopen
** Descr.       : tries to open file with 2.nd param as 
**                filename. if failed:
**                tries to get the filename via the 1. param
**                as environment variable. if envir. failed 
**                returns -1, else:
**                tries to open file with value of environment
**                variable. returns filehandle or NULL
** Parameters   : envirvarname   -I  char* 
**              : filename       -I  char* 
**              : open mode      -I  char* 
** Return value : FILE*
**/
DllExport FILE *BT_altfopen (char *env, char *file, char *mode);

/*****************************************************************
** Function     : BT_getAltFileName
** Descr.       : returns the filename used in the last recently
**                call of BT_altfopen. This string will be overrriden
**                by the next call of BT_altfopen!
** Parameters   : none
** Return value : char* 
**/
DllExport char *BT_getAltFileName (void);

/*****************************************************************
** Function     : BT_time_string
** Descr.       : returns actual date & time in UNIX style 
** Parameters   : int type:                               */
#define TS_date_time 0      /* 'date time' returned            */
#define TS_date      1      /* 'date' returned: 'YYYY-MM-DD'   */
#define TS_time      2      /* 'time' returned: 'HH:MM:SS'     */
/*                     
** Return value : char*
**/
DllExport char *BT_time_string (int);

/*****************************************************************
** Function     : BT_striset
** Description  : like the WIN95 function _strset: 
**                fills a string with the specified character
**/
DllExport void BT_striset (char *str, char character);

/*****************************************************************
** Function     : BT_convert     
** Description  : converts a char-buffer via a conversion table
**/
DllExport void BT_convert (Byte *outstr, Byte *instr, long len, const Byte *tab);

/*****************************************************************
** Function     : getlogin
** Description  : returns the userid = GetUserName for Win32
** Hint         : no prefix BT_, it acts as a replacement for the UNIX function
**/
#ifdef _SYS_WIN_X
  DllExport char *getlogin (void);
#endif 

/*****************************************************************
** Function     : BT_readTable
** Description  : Loads a BT_convert table from File
**/
DllExport  bool BT_readTable (Byte *tab, FILE *fh);


/*****************************************************************
** Function     : BT_copymem     
** Description  : copies a chunk of memory into another 
**/

DllExport void CC BT_copymem (Byte *to, Byte *from, long len);

/*****************************************************************
** Function     : BT_malloc
** Description  : secure malloc
**/

DllExport void *BT_malloc (long len, char *prog, long line);
DllExport void  BT_free (void *mem);
DllExport char *BT_alloc_str (const char *str);

#define malloc(x) BT_malloc(x, __FILE__, __LINE__) 
#define free(x)   BT_free(x)

#ifdef __cplusplus
}//end-extern-C
#endif /*__cplusplus*/


#endif /*tools_h*/

