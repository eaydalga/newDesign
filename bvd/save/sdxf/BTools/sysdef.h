/* doc----------------------------------------------------------------
:Project.   PINPI: Platform Independent Network Programming Interface
:Program.   sysdef.h
:PrType.    header file
:Title.     basic definitions
:Version.   0.2
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI-C
:Compiler.
----------------------------------------------------------------------*/

#ifndef sysdef_h
#define sysdef_h

//#define LIBTYPE " DLL"

#ifndef NO_SYSSPECS
#include "sysspecs.h"
#endif

/**** implementing dependend settings:  **/
#define AES_IMPLEMENTED
#define DEFLATE_IMPLEMENTED
/**** end-settings  **/

// #pragma message( "compiling for " _SYStext )

/* type definitions: */

#ifndef __cplusplus
  typedef unsigned int   bool;
  #define true  1
  #define false 0
#endif // C++

typedef unsigned char  Byte;
typedef   signed char  Small;
typedef unsigned short Ushort;
typedef unsigned long  Ulong;

typedef struct TIPADDR                                            /*#91 IPv6*/
{
  Ulong v4;
  Ulong x [3];

} Tipaddr;


/*   Macros:  */

#define ON    1
#define OFF   0

#define NIL   0x00

#define cEQUAL(x,y)     (0 == strcmp(x,y))
#define cNOT_EQUAL(x,y) (0 != strcmp(x,y))

#ifndef AI_FAMILY_FOR_LISTEN
  #define AI_FAMILY_FOR_LISTEN AF_UNSPEC;    /* Accept either IPv4 or IPv6  */
#endif

/*   multi-threading   */
#ifndef THREADPROCTYPE 
  #define THREADPROCTYPE void *
#endif

#ifndef  start_thread
  // start "thread" as a normal function:
  #define  start_thread(proc,handle) proc(handle)
#endif

#endif  /*sysdef_h*/   

