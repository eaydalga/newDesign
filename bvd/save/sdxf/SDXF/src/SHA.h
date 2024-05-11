/* doc-----------------------------------------------------------------------------
:Project.   SDXF: Structured Data Exchange Format
:Program.   SHA.h
:PrType.    header file
:Title.     SHA1 (Secure Hash Algorithm), algorithm by NIST
:Version.   
:Author.    
:Language.  ANSI-C
:Compiler.
:Created.   
---------------------------------------------------------------------------------*/ 
//  These functions calculates the hashvalue of messages 'mem' which
//  are 'length' bytes long, into 'hashvalue' which had to be 16 bytes
//  long (128 bits).
//
// the calculation is done in three steps with this 3 functions:

#ifndef _sha_h
#define _sha_h

#include "tools.h"

void sha_hash_init ();
void sha_hash_update (char *mem, unsigned long length);
void sha_hash_finish (unsigned long *hashvalue);

void sha_hash_mem(char *mem, unsigned long length, unsigned long *hashvalue);

#endif
