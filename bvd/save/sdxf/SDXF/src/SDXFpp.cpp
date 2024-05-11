/* doc----------------------------------------------------------------
:Project.   SDXF: Structured Data Exchange Format
:Program.   SDXFpp.cpp
:Title.     class wrapper for SDXF functions
:PrType.    Function Lib
:Version.   
:Author.    Max Wildgrube
:Copyright. Max Wildgrube
:Language.  C++ (ANSI)
-----------------------------------------------------------------------*/


#include "SDXF.h"
#include "SDXtools.h"
#include <string.h>
#include <malloc.h>


///////////////////////////////////////////////////////////////
SDXF::~SDXF ()                // destructor
{
  if (mycont && cont != NULL) free (cont);
  mycont   = false;
}

///////////////////////////////////////////////////////////////
SDXF::SDXF ()                // dummy-constructor
{
  sdx.dataType  = SDX_OPT_CLEAR;
  SDX_init (&sdx);
  if (rc > SDX_RC_ok)  throw this;    

  cont   = NULL;
  mycont = false;
  ok     = true;
  rc     = sdx.rc;
  ec     = sdx.ec;

}

///////////////////////////////////////////////////////////////
SDXF::SDXF (Byte *p_cont)     // old container
{
  sdx.dataType  = SDX_OPT_OLD;
  sdx.container = p_cont;
  sdx.cryptkey  = NULL;

  SDX_init (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 
  cont     = p_cont;
  mycont   = false;

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;

  ok = true;
}

///////////////////////////////////////////////////////////////
SDXF::SDXF (long p_size)      // new container
{
  if (p_size == 0)
  {
    sdx.function = "constr new SDXF object: param size = 0";
    rc = SDX_RC_parameterError;
    ec = SDX_EC_paramMissing;
    throw this;                       //SDXC_Error (this);
  }
  
  cont = (Byte *) malloc (p_size);

  if (cont == NULL)
  {
    sdx.function = "constr new SDXF object: no memory";
    rc = SDX_RC_noMemory;
    ec = SDX_EC_noMemory;
    throw this;  
  }

  mycont = true;

  sdx.dataType   = SDX_OPT_NEW;
  sdx.container  = cont;
  sdx.bufferSize = p_size;

  SDX_init (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;  

  ok = true;
}

///////////////////////////////////////////////////////////////
SDXF::SDXF (Byte *p_cont, long p_size)      // new container
{
  if (p_size == 0)
  {
    sdx.function = "constr new SDXF object: param size = 0";
    rc = SDX_RC_parameterError;
    ec = SDX_EC_paramMissing;
    throw this;  
  }
  
  if (p_cont == NULL) 
    cont = (Byte *) malloc (p_size);
  else
    cont = p_cont;

  if (cont == NULL)
  {
    sdx.function = "constr new SDXF object: no memory";
    rc = SDX_RC_noMemory;
    ec = SDX_EC_noMemory;
    throw this;  
  }

  mycont = (p_cont == NULL);

  sdx.dataType   = SDX_OPT_NEW;
  sdx.container  = p_cont;
  sdx.bufferSize = p_size;
  
  SDX_init (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 
  cont     = p_cont;
  
  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;  

  ok = true;
}

// conventional interface:

///////////////////////////////////////////////////////////////
void SDXF::init (void)                   // old container
{
  sdx.dataType  = SDX_OPT_OLD;

  SDX_init (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;  

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::init (Byte *p_cont)             // old container
{
  if (p_cont != NULL)
  {
    if (mycont && cont != NULL) free (cont);
    cont   = p_cont;
    mycont = false;
  }
  else if (cont == NULL)
  {
    sdx.function = "init old SDXF object: no container";
    rc = SDX_RC_parameterError;
    ec = SDX_EC_paramMissing;
    throw this;  
  }

  sdx.dataType  = SDX_OPT_OLD;
  sdx.container = cont;
  
  SDX_init (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok) 
    throw this;  

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::init (Byte *p_cont, long p_size)  // new container
{
  if (p_size == 0)
  {
    sdx.function = "constr new SDXF object: param size = 0";
    rc           = SDX_RC_parameterError;
    ec           = SDX_EC_paramMissing;
    throw this;  
  }

  if (p_cont != NULL)
  {
    if (mycont && cont != NULL) free (cont);
    cont   = p_cont;
    mycont = false;
  }
  else if (cont == NULL)
  {
    sdx.function = "init old SDXF object: no container";
    rc           = SDX_RC_parameterError;
    ec           = SDX_EC_paramMissing;
    throw this;  
  }

  sdx.dataType   = SDX_OPT_NEW;
  sdx.container  = cont;
  sdx.bufferSize = p_size;

  SDX_init (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok) 
    throw this;  

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::init (long p_size)  // new container
{
  if (p_size == 0)
  {
    sdx.function = "constr new SDXF object: param size = 0";
    rc           = SDX_RC_parameterError;
    ec           = SDX_EC_paramMissing;
    throw this;  
  }

  if (mycont && cont != NULL) free (cont);

  cont = (Byte *) malloc (p_size);

  if (cont == NULL)
  {
    sdx.function = "constr new SDXF object: no memory";
    rc           = SDX_RC_noMemory;
    ec           = SDX_EC_noMemory;
    throw this;  
  }

  mycont = true;

  sdx.dataType   = SDX_OPT_NEW;
  sdx.container  = cont;
  sdx.bufferSize = p_size;

  SDX_init (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok) throw this;  

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::enter (void)
{
  SDX_enter (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_warning) throw this;

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::leave (void)
{
  SDX_leave (&sdx);

  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok) throw this; 

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::next (void)
{
  SDX_next (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_warning)  throw this;

  ok = (rc == SDX_RC_ok);
}

///////////////////////////////////////////////////////////////
long SDXF::extract  (void *p_data, long p_length)
{
  sdx.maxLength = p_length;
  sdx.data      = (Byte *)p_data;

  SDX_extract (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_warning)  throw this;


  ok = true;
  return sdx.dataLength;
}

#ifdef old
///////////////////////////////////////////////////////////////
long SDXF::extract  (char *p_data, long p_length)
{
  sdx.maxLength = p_length;
  sdx.data      = (Byte *)p_data;

  SDX_extract (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_warning)  throw this;


  ok = true;
  return sdx.dataLength;
}
#endif

///////////////////////////////////////////////////////////////
short SDXF::extract  (void *p_data, short p_length, short p_count)
{
  sdx.dataLength = p_length;
  sdx.count      = p_count;  
  sdx.data       = (Byte *)p_data;
  
  SDX_extract (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_warning)  throw this;

  ok = true;
  return sdx.count;
}

///////////////////////////////////////////////////////////////
long SDXF::extract  (void)
{
  sdx.maxLength = 0;
  sdx.data      = NULL;
  SDX_extract (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;

  ok = true;
  return sdx.value;
}

///////////////////////////////////////////////////////////////
void  SDXF::extract (long   *p_value)              // extract numeric
{
  sdx.maxLength = 0;
  sdx.data      = NULL;
  SDX_extract (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this; 

  *p_value = sdx.value;
  ok = true;
}

///////////////////////////////////////////////////////////////
void  SDXF::extract (double *p_fvalue)               // extract float
{
  sdx.maxLength = 0;
  sdx.data      = NULL;
  SDX_extract (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;     

  ok = true;
  *p_fvalue = sdx.fvalue;
}


///////////////////////////////////////////////////////////////
void SDXF::create (ChunkID p_id)                          // structured
{
  sdx.dataType = SDX_DT_structured;
  sdx.chunkID   = p_id; 

  SDX_create (&sdx);

  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;                     

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::create (ChunkID p_id, long p_value)       // numeric
{
  sdx.dataType   = SDX_DT_numeric;
  sdx.chunkID    = p_id; 
  sdx.value      = p_value;
  sdx.dataLength = 0;

  SDX_create (&sdx);

  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;                     

  ok = true;
}


///////////////////////////////////////////////////////////////
void SDXF::create (ChunkID p_id, Byte *p_data, long p_length) // binary
{
  sdx.dataType   = SDX_DT_binary;
  sdx.chunkID     = p_id; 
  sdx.data       = p_data;
  sdx.dataLength = p_length;

  SDX_create (&sdx);

  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;                     

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::create (ChunkID p_id, char *p_data)    // character
{
  sdx.dataType   = SDX_DT_char;
  sdx.chunkID     = p_id; 
  sdx.data       = (Byte *) p_data;
  sdx.dataLength = strlen (p_data);

  SDX_create (&sdx);

  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;                     

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::create  (ChunkID p_id, char *p_data, bool p_is_utf8)
 // 3rd parm=true: unicode character as UTF8 
{
  sdx.dataType   = p_is_utf8 ? SDX_DT_UTF8 : SDX_DT_char;
  sdx.chunkID    = p_id; 
  sdx.data       = (Byte *) p_data;
  sdx.dataLength = strlen (p_data);
  sdx.count      = 0;

  SDX_create (&sdx);

  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;  

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::create                                // array
            ( ChunkID p_id
            , void *p_data
            , short p_count
            , short p_length
            , SDX_DataType p_dt
            )
{
  sdx.dataType   = p_dt;
  sdx.chunkID    = p_id; 
  sdx.data       = (Byte *) p_data;
  sdx.dataLength = p_length;
  sdx.count      = p_count;

  SDX_create (&sdx);

  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;  

  ok = true;

}

///////////////////////////////////////////////////////////////
void SDXF::write (FILE *p_file)
{
  SDX_writeChunk (&sdx, p_file);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;                     

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::read (FILE *p_file)
{
  SDX_readChunk (&sdx, p_file);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;                     

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::set_compression (Byte p_comprtype)
{
  sdx.compression = p_comprtype;
}

///////////////////////////////////////////////////////////////
void SDXF::set_encryption  (Byte *p_cryptkey)
{
  if (p_cryptkey)
  {
    sdx.encrypt  = true;
    sdx.cryptkey = p_cryptkey;
  }
  else
  {
    sdx.encrypt  = false;
  }

}

// advanced interface:

///////////////////////////////////////////////////////////////
SDXC_CHUNK::SDXC_CHUNK (void)
{
  value  = 0;
  data   = NULL;
  length = 0;
  id     = 0;
  dt     = SDX_DT_inconsistent;
}
SDXC_CHUNK::SDXC_CHUNK (const ChunkID p_id)
{
  value  = 0;
  data   = NULL;
  length = 0;
  id     = p_id;
  dt     = SDX_DT_structured;
}

SDXC_CHUNK::SDXC_CHUNK (ChunkID p_id, long p_value)
{
  value  = p_value;
  data   = NULL;
  length = 0;
  id     = p_id;
  dt     = SDX_DT_numeric;
}

SDXC_CHUNK::SDXC_CHUNK (ChunkID p_id, char *p_data)
{
  value  = 0;
  data   = p_data;
  length = strlen (p_data); 
  id     = p_id;
  dt     = SDX_DT_char;
}

SDXC_CHUNK::SDXC_CHUNK (ChunkID p_id, char *p_data, long p_length)
{
  value  = 0;
  data   = p_data;
  length = p_length;
  id     = p_id;
  dt     = SDX_DT_char;
}

SDXC_CHUNK::SDXC_CHUNK (ChunkID p_id, Byte *p_data, long p_length)
{
  value  = 0;
  data   = (char *)p_data;
  length = p_length;
  id     = p_id;
  dt     = SDX_DT_binary;
}


///////////////////////////////////////////////////////////////
//void SDXF::operator >> (SDXC_CHUNK& p_chunk)  // alias for 'extract' 

void operator >> (SDXF& p_x, SDXC_CHUNK& p_chunk)  // alias for 'extract' 
{
  p_x.sdx.maxLength = p_chunk.length;
  p_x.sdx.data      = (Byte *)p_chunk.data;

  switch (p_x.sdx.dataType) 
  { 
  case SDX_DT_char:
  case SDX_DT_binary:
    if (p_x.sdx.maxLength == 0) 
    {
      p_x.rc = SDX_RC_parameterError;
      p_x.ec = SDX_EC_paramMissing;
      throw p_x;                   //SDXC_Error (&p_x);
    }

    if (p_x.sdx.data == NULL) p_x.sdx.data = (Byte *) malloc (p_x.sdx.maxLength);
  }
  p_chunk.data = (char *) p_x.sdx.data;

  SDX_extract (&p_x.sdx);

  p_x.length      = p_x.sdx.dataLength;
  p_x.dataType    = p_x.sdx.dataType; 
  p_chunk.value   = p_x.sdx.value;
  p_chunk.length  = p_x.length;

  p_x.id = p_chunk.id = p_x.sdx.chunkID;
  p_x.rc = p_x.sdx.rc;
  p_x.ec = p_x.sdx.ec;

  if (p_x.rc > SDX_RC_ok)  throw p_x;             // SDXC_Error (&p_x);

  p_x.ok = true;
}


///////////////////////////////////////////////////////////////
//void SDXF::operator << (SDXC_CHUNK p_chunk)    // alias for 'create' 
void operator << (SDXF& p_x, SDXC_CHUNK& p_chunk)  // alias for 'create' 
{
  p_x.sdx.dataType   = p_chunk.dt;
  p_x.sdx.chunkID    = p_chunk.id; 
  p_x.sdx.data       = (Byte *) p_chunk.data;
  p_x.sdx.value      = p_chunk.value;
  p_x.sdx.dataLength = p_chunk.length;

  SDX_create (&p_x.sdx);


  p_x.rc = p_x.sdx.rc;
  p_x.ec = p_x.sdx.ec;

  if (p_x.rc > SDX_RC_ok)  throw p_x;            // SDXC_Error (&p_x);

  p_x.dataType = p_x.sdx.dataType;
  p_x.length   = p_x.sdx.dataLength;
  p_x.ok       = true;
}


///////////////////////////////////////////////////////////////
bool SDXF::operator ++  (int)    // alias for 'next'    (returns success)
{
  SDX_next (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_warning) throw this;                     

  ok = (rc == SDX_RC_ok);
  return ok;
}

///////////////////////////////////////////////////////////////
void SDXF::operator ~  (void)    // alias for 'enter'
{
  SDX_enter (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_warning) throw this;                     

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::operator !  (void)    // alias for 'leave'
{
  SDX_leave (&sdx);

  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_warning) throw this;                     

  ok = true;
}
///////////////////////////////////////////////////////////////
void SDXF::operator >> (FILE *p_file)
{
  SDX_writeChunk (&sdx, p_file);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_warning) throw this;                     

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::operator << (FILE *p_file)
{
  SDX_readChunk (&sdx, p_file);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_warning) throw this;                     

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::operator () (void)             // old container
{
  sdx.dataType  = SDX_OPT_OLD;
  sdx.container = cont;
  
  SDX_init (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_warning) throw this;                     

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::operator () (Byte *p_cont)             // old container
{
  if (p_cont != NULL)
  {
    if (cont != NULL && mycont) free (cont);
    cont   = p_cont;
    mycont = false;
  }
                                  #ifdef old
                                    else if (cont == NULL)
                                    {
                                      sdx.function = "init old SDXF object: no container";
                                      rc = SDX_RC_parameterError;
                                      ec = SDX_EC_paramMissing;
                                      throw this;                     
                                    }
                                  #endif

  sdx.dataType  = SDX_OPT_OLD;
  sdx.container = cont;
  
  SDX_init (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_warning) throw this;                     

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::operator () (Byte *p_cont, long p_size)  // new container
{
  if (p_size == 0)
  {
    sdx.function = "constr new SDXF object: param size = 0";
    rc           = SDX_RC_parameterError;
    ec           = SDX_EC_paramMissing;
    throw this;                     
  }

  if (p_cont != NULL)
  {
    if (cont != NULL && mycont) free (cont);
    cont = p_cont;
    mycont = false;
  }
  else
  {
    if (cont == NULL)
    {
      sdx.function = "init old SDXF object: no container";
      rc           = SDX_RC_parameterError;
      ec           = SDX_EC_paramMissing;
      throw this;                     
    }
  }

  cont = p_cont;

  sdx.dataType   = SDX_OPT_NEW;
  sdx.container  = cont;
  sdx.bufferSize = p_size;

  SDX_init (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;                     

  ok = true;
}

///////////////////////////////////////////////////////////////
void SDXF::operator () (long p_size)  // new container
{
  if (p_size == 0)
  {
    sdx.function = "constr new SDXF object: param size = 0";
    rc           = SDX_RC_parameterError;
    ec           = SDX_EC_paramMissing;
    throw this;                     
  }

  if (cont != NULL && mycont) free (cont);

  cont = (Byte *) malloc (p_size);

  if (cont == NULL)
  {
    sdx.function = "constr new SDXF object: no memory";
    rc           = SDX_RC_noMemory;
    ec           = SDX_EC_noMemory;
    throw this;                     
  }

  mycont = true;

  sdx.dataType   = SDX_OPT_NEW;
  sdx.container  = cont;
  sdx.bufferSize = p_size;

  SDX_init (&sdx);

  length   = sdx.dataLength;
  dataType = sdx.dataType; 

  id = sdx.chunkID;
  rc = sdx.rc;
  ec = sdx.ec;

  if (rc > SDX_RC_ok)  throw this;                     

  ok = true;
}

//////////////////////////////////////////////////////////////
SDX_STREAM::SDX_STREAM (char *p_name, STREAM_mode p_mode = STREAM_old)
{
  char modetab [3] = {'r', 'w', 'a'};
  stream_handle     = SDX_fileOpen (p_name, modetab [p_mode]);
  success           = stream_handle != NULL;
}

SDX_STREAM::~SDX_STREAM ()                   // close stream  
{
  if ( stream_handle ) SDX_fileClose (stream_handle);
}

void SDX_STREAM::operator << (SDXF& p_sdx)
{
  success = stream_handle ? SDX_writeChunk (&p_sdx.sdx, stream_handle) : false;
}

void SDX_STREAM::operator >> (SDXF& p_sdx)
{
  success  = stream_handle ? SDX_readChunk (&p_sdx.sdx, stream_handle) : false;
  p_sdx.id = p_sdx.sdx.chunkID;
}



///////////////////////////////////////////////////////////////
