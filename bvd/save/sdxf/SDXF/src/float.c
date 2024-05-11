/* doc----------------------------------------------------------------
:Project.   Structured Data eXchange Format (SDXF)
:Program.   float.c
:PrType.    Function Library
:Title.     conversion routines for floating point numbers
:Version.   0.3 
:Author.    Max Wildgrube
:Copyright. Max Wildgrube
:Language.  C (ANSI)
:Compiler.
:Note.      For Mainframes (MVS, BS2000) only
-----------------------------------------------------------------------
*/

/* realizing following functions:
 
  float  BT_adaptFtoHost (float);
  double BT_adaptDtoHost (double);
  float  BT_adaptFtoNet  (float);
  double BT_adaptDtoNet  (double);
*/

#include "SDXF.h"

#if defined (_SYS_MAINFRAME)

int get_float_ec ()
{
  return float_execption_flags;
}
#endif

/************************************************************/ 
#if defined (_SYS_MVS)

#include <_Ieee754.h>
 
int float_execption_flags;

/************************************************************/ 
void set_execption_flags (int p_rc)
{
  switch (p_rc)
  {
  case 0: float_execption_flags = 0; break; /* zero      */ 
  case 1: float_execption_flags = 3; break; /* underflow */ 
  case 2: float_execption_flags = 0; break; /* success   */ 
  case 3: float_execption_flags = 4; break; /* overflow  */ 
  }
}

/************************************************************/ 
float  BT_adaptFtoHost (float p_num)  
{
  float target;
  SDX_TOptions *opt = SDX_getOptions ();

  if (! opt->adaptFTPto390) return p_num;
  
  set_execption_flags 
    (__fp_btoh (&p_num, _FP_FLOAT, &target, _FP_FLOAT, _FP_BH_RN));
  
  return target;
}

/************************************************************/ 
double BT_adaptDtoHost (double p_num)  
{
  double target;
  SDX_TOptions *opt = SDX_getOptions ();

  if (! opt->adaptFTPto390) return p_num;
  
  set_execption_flags 
    (__fp_btoh (&p_num, _FP_DOUBLE, &target, _FP_DOUBLE, _FP_BH_RN));
  
  return target;
}

/************************************************************/ 
float  BT_adaptFtoNet  (float p_num)  
{
  float target;
  SDX_TOptions *opt = SDX_getOptions ();

  if (! opt->adaptFTPto390) return p_num;
  
  set_execption_flags 
    (__fp_htob (&p_num, _FP_FLOAT, &target, _FP_FLOAT, _FP_BH_RN));
  
  return target;
}

/************************************************************/ 
double BT_adaptDtoNet  (double p_num)  
{
  double target;
  SDX_TOptions *opt = SDX_getOptions ();

  if (! opt->adaptFTPto390) return p_num;
  
  set_execption_flags 
    (__fp_htob (&p_num, _FP_DOUBLE, &target, _FP_DOUBLE, _FP_BH_RN));
  
  return target;
}

/************************************************************/ 
/************************************************************/ 
#elif defined (_SYS_BS2000)

#include <ieee_390.h>

extern int float_execption_flags;

/************************************************************/ 
void set_execption_flags (void)
{
  switch (float_execption_flags)
  {
  case  0: float_execption_flags = 0; break; /* success   */ 
  case  1: float_execption_flags = 1; break; /* inexact   */ 
  case  2: float_execption_flags = 2; break; /* n.a.      */ 
  case  4: float_execption_flags = 3; break; /* underflow */ 
  case  8: float_execption_flags = 4; break; /* overflow  */ 
  case 16: float_execption_flags = 2; break; /* invalid   */ 
  }
}

/************************************************************/ 
float  BT_adaptFtoHost (float p_num)  
{
  float target;
  SDX_TOptions *opt = SDX_getOptions ();

  if (! opt->adaptFTPto390) return p_num;
  
  target = ieee2float (p_num);
  set_execption_flags ();
  
  return target;
}

/************************************************************/ 
float  BT_adaptFtoNet (float p_num)  
{
  float target;
  SDX_TOptions *opt = SDX_getOptions ();

  if (! opt->adaptFTPto390) return p_num;
  
  target = float2ieee (p_num);
  set_execption_flags ();
  
  return target;
}

/************************************************************/ 
double  BT_adaptDtoHost (double p_num)  
{
  double target;
  SDX_TOptions *opt = SDX_getOptions ();

  if (! opt->adaptFTPto390) return p_num;
  
  target = ieee2double (p_num);
  set_execption_flags ();
  
  return target;
}

/************************************************************/ 
double  BT_adaptDtoNet (double p_num)  
{
  double target;
  SDX_TOptions *opt = SDX_getOptions ();

  if (! opt->adaptFTPto390) return p_num;
  
  target = double2ieee (p_num);
  set_execption_flags ();
  
  return target;
}

#endif
