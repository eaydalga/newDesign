/* doc----------------------------------------------------------------
:Program.   SDXtools.h
:PrType.    header file
:Title.     Extended definitions for SDXF
:Contents.
:Version.   1.0
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI-C
:Compiler.
:Created.   1994-11-10

------------------------------------------------------------------*/
#ifndef SDXtools_h
#define SDXtools_h

#include "SDXF.h"
#include <stdio.h>

#ifdef _SYS_MAINFRAME
#define SDX_fileOpen          SXfilopen    
#define SDX_fileClose         SXfilclos
#define SDX_readChunk         SXrdChunk
#define SDX_writeChunk        SXwrChunk
#define SDX_free              SXfree
#define SDX_errtext           SXerrtxt
#define SDX_getTablesFromFile SXgtabff
#endif

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#define SDX_mode_read   'r'
#define SDX_mode_write  'w'
#define SDX_mode_append 'a'

DllExport FILE *SDX_fileOpen   (const char *filename, char modus); 
DllExport FILE *SDX_altfopen   (char *env, char *file, char mode);
                               /* modus is 'r'=read, 'w'=write or 'a'=append  */
DllExport void  SDX_fileClose  (FILE *file); 
DllExport bool  SDX_writeChunk (SDX_obj *sdx, FILE *file);
DllExport bool  SDX_readChunk  (SDX_obj *sdx, FILE *file);
DllExport void  SDX_free       (SDX_obj *sdx);  /* free container memory */ 
DllExport char *SDX_errtext    (SDX_obj *sdx);


#ifdef __cplusplus
///////////////////////////////////////////////////////////////////////////

typedef enum  
{
  STREAM_old
, STREAM_new
, STREAM_add
} STREAM_mode;

class DllExport SDX_STREAM
{
  public:
    SDX_STREAM (char *, STREAM_mode);    // open stream
    ~SDX_STREAM ();                      // close stream  
    void operator << (class SDXF&);      // write chunk to stream
    void operator >> (class SDXF&);      // read chunk from stream
    bool success;

  private:
    FILE *stream_handle;
};



}//end-extern-C
#endif /*__cplusplus*/

#endif /*SDXtools_h*/
