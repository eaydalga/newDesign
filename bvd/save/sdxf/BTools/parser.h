/* doc-----------------------------------------------------------------------------
:Project.   common tools
:Program.   parser.h
:PrType.    Header file
:Title.     Parsing, analysing and edit a list of arguments
:Version.   
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI-C
:Compiler.
:Created.   1995-11-23
:Changed.   2001-01-13 -- new design of parsing
:Note.      not multithread capable: do not use it in the server thread proc
------------------------------------------------------------------------------------*/ 

/* function:   BT_parse


description of the BT_parse interface:

  usage: int ok = BT_parse ((long *)&container, templat, arguments, work_or_NULL);
 
  templat := "[mult][options]parmtype[([keyword [=alias] ...] ; name [[=alias] ...]... )] ..."

  mult    :=  Multiplicator, numerical value > 0, default = 1

  options :=  R = required    param must be specified 
              K = keyword     ...must be given if param is used
              L = last        rest of tokens without syntactical interpr.
  
  parmtype:=  N = numeric     digits (without sign) limited to long 
              T = toggle      allows ON OFF YES NO (boolean)
              W = switch      = TRUE if keyword is given
              S = string      a word or "string" or 'string' or (string)    
              X = Name [List] = 1 [2, 3, ...] like the position in
                                the list.  

  (keyword ...) an optional keyword
  (name ...)    a list of names for parmtype X
                The list of names starts after the char ';' 1)
   
  container := array of long, for each entry in the template and
               in the order of the entries in the template.
               the container will be filled with...

               parmtype 
               ------
                    N : the numeric value
                  W T : 0 (=false) or 1 (=true)
                  L S : a pointer to a zero-terminated string
                    X : the postion in the list (1-based)

    arguments := the string to parse.
    work_or_NULL := optional pointer to a work area.

    returns   :  see error codes in the following enum definition 

hints:

1): option X: you can define aliases for the keyword and in the list of names,
    use '=' between the alias - for example:
    X(;A=B=C D=E F G=H)
    results:
    A, B & C generates index 1, D & E: 2, F: 3, G & H: 4 

2): these inputs are treated as comments:
    - a '*' in the 1st column (returns PRS_RC_empty)
    - the rest of line after a ';' is ignored 
*/

#ifndef parser_h
#define parser_h

#include "sysdef.h"

/* prototypes:                 */

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
 
 DllExport char * BT_join     (char **);
 DllExport char * BT_parmjoin (char **);
 DllExport char * BT_parserr  (void); 
 
 DllExport int BT_parse
     ( long *container  /* output: the edited arguments        */ 
     , char *templat    /* input:  the description of the args */ 
     , char *arguments  /* input:  the list of arguments       */ 
     , char *work       /* input:  work area                   */ 
     ); 

#ifdef __cplusplus
}//end-extern-C
#endif /*__cplusplus*/


enum
{
    PRS_RC_ok                            =  0
  , PRS_RC_empty                         =  1
  , PRS_RC_keyword_param_requested       =  2
  , PRS_RC_too_much_args                 =  3
  , PRS_RC_too_few_args                  =  4
  , PRS_RC_numeral_expected              =  5
  , PRS_RC_wrong_toggle_arg              =  6
  , PRS_RC_no_value_after_keyword        =  7
  , PRS_RC_wrong_param_after_keyword     =  8
  , PRS_RC_param_needs_keyword           =  9
  , PRS_RC_too_many_opening_parenthesis  = 10
  , PRS_RC_too_many_closing_parenthesis  = 11
  , PRS_RC_unexpected_equal_sign         = 12 
  , PRS_RC_unknown_keyword               = 13
};
#define PRS_NUMBER_rc                      14

enum
{
    PRS_RC_te_base                       = 81
  , PRS_RC_te_illegal_option             = 81
  , PRS_RC_te_no_keyword_after_option    = 82
  , PRS_RC_te_no_closing_parenthesis     = 83
  , PRS_RC_te_unexp_closing_parenthesis  = 84
  , PRS_RC_te_unexp_opening_parenthesis  = 85
  , PRS_RC_te_empty                      = 86
  , PRS_RC_te_unexp_nameList             = 87 
  , PRS_RC_te_unexp_blank_after_keyword  = 88
  , PRS_RC_te_unexp_semicolon            = 89
  , PRS_RC_te_unexp_equal_sign           = 90
  , PRS_RC_te_only_one_keyword_allowed   = 91
};
#define PRS_NUMBER_te                     11


#endif /*parser_h*/
