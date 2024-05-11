/* doc--------------------------------------------------------------------------
:Project.   SDXF
:Program.   transtab.h
:Title.     Define the Tabs for converting local to net and v.v.
:PrType.    Header File
:Version.   1.0
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI-C
:Compiler.
:Created.   1995-09-24 
-------------------------------------------------------------------------------*/
#ifndef transtab_h
#define transtab_h

#include "sysdef.h"

#define TT_GetTransTables TTGetTab


bool TT_GetTransTables (Byte **toNet, Byte **toHost);

#endif /*transtab_h*/

