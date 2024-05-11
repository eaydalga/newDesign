/* doc----------------------------------------------------------------
:Project.   Structured Data eXchange Format (SDXF)
:Program.   float.h
:PrType.    Header File
:Title.     conversion routines for floating point numbers
:Version.   0.3 
:Author.    Max Wildgrube
:Copyright. Max Wildgrube
:Language.  C (ANSI)
:Compiler.
-----------------------------------------------------------------------*/

#ifdef _SYS_MAINFRAME
  float  BT_adaptFtoHost (float);
  double BT_adaptDtoHost (double);
  float  BT_adaptFtoNet  (float);
  double BT_adaptDtoNet  (double);
  int    get_float_ec ();

#else
  #define BT_adaptFtoHost BT_adaptF
  #define BT_adaptDtoHost BT_adaptD
  #define BT_adaptFtoNet  BT_adaptF
  #define BT_adaptDtoNet  BT_adaptD
  #define get_float_ec() 0
#endif
