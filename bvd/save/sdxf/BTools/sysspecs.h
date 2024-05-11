/* doc----------------------------------------------------------------
:Project.   PINPI: Platform Independent Network Programming Interface
:Program.   sysspecs.h
:PrType.    header file
:Title.     basic definitions
:Version.   0.3
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI-C
:Compiler.

------------------------------------------------------------------*/

#ifdef WIN32        /* no predefinition necessary for W95 (?) */
  #define _SYS_WIN
#endif

#ifdef  _SYS_WIN
  /*   multi-threading   */
  #define start_thread(f,p) _beginthread(f,0,p) == -1 ? errno : 0
  #define pthread_t
  #define thread_handle
  #define THREADPROCTYPE void


  #ifdef WIN32      /* only _SYS_WIN necessary for Windows 3.1 */
    #define _SYS_WIN32
  #else
    #define _SYS_WIN16
  #endif

  #ifdef _DLL
    #define DllImport	__declspec( dllimport )
    #define DllExport	__declspec( dllexport )
    #define CC __stdcall
  #endif

  #ifdef _CONSOLE
    #define _SYS_ASCII
  #else
    #define _SYS_ANSI
  #endif /* _CONSOLE */

#endif  /*_SYS_WIN */

#ifndef _DLL
  #define DllImport
  #define DllExport
  #define CC
#endif

#ifdef USE_SEI
  #undef  CC
  #define CC __stdcall
#endif
/* don't change following MVS stuff: */
#ifdef _SYS_MVS_IBM
  #define _SYS_MVS
#endif

#ifdef _SYS_MVS_INTERLINK
  #define _SYS_MVS
#endif

#ifdef _SYS_MVS
/*??  #include "adaspec.h" */
#endif
/* end-MVS stuff */


/* define sys text: */

#if   defined _SYS_WIN16
  #define _SYStext "Windows 3.1"

#elif defined _SYS_WIN32
  #ifdef _DLL
    #define _SYStext "Win32 (DLL)"
  #else
    #define _SYStext "Win32"
  #endif

#elif defined _SYS_UNIX
  #define _SYStext "Unix"
  #define _SYS_ASCII

#elif defined _SYS_LINUX
  #define _SYStext "Linux"
  #define _SYS_ASCII
  #define _SYS_UNIX

#elif defined _SYS_HPUX
  #define _SYStext "HP-UX"
  #define _SYS_ASCII
  #define _SYS_UNIX
  #define _ARCH_alignment_sensitive

#elif defined _SYS_BS2000
  #define _SYStext "BS2000"
  #define _SYS_MAINFRAME

#elif defined _SYS_MVS_IBM
  #define _SYStext "MVS (IBM tcp/ip)"
  #define _SYS_MAINFRAME

#elif defined _SYS_MVS_INTERLINK
  #define _SYStext "MVS (INTERLINK tcp/ip)"
  #define _SYS_MAINFRAME

#else
  #define _SYStext "unknown"
#endif

// #pragma message( "compiling for " _SYStext )

#ifndef _SYS_LINUX
  #define  FD_SETSIZE  1000 /* maximal number of sockets */
#endif

#ifdef _SYS_UNIX
  #define start_thread(f,p) pthread_create(&thread_handle,NULL,f,p)
  #include <pthread.h>
  #define getlogin(x) getenv("USER")
#endif

#ifdef _SYS_WIN32
  #define getlogin(x) getenv("USERNAME")
#endif
