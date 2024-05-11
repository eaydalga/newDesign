/* doc-----------------------------------------------------------------------------
:Project.   common tools
:Program.   parser.h
:PrType.    Header file
:Title.     Parsing, analysing and edit a list of arguments
:Version.
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI-C++
:Compiler.
:Created.   1995-11-23
:Changed.   2001-01-13 -- new design of parsing
:Note.      not multithread capable: do not use it in the server thread proc
------------------------------------------------------------------------------------*/

#ifdef comment

description of the Parse interface:

  usage: the parser will be invoked in 2 steps:
    1st: create an instance of the Parser Class CParser with a "Parser Template":

      CParser myparser (template);

    2nd: parse the arguments with the aid of this instance:

      int ok = myparser.parse ((long *)&container, arguments [, stringcontainer]);

    whereas:

  [...]       an optional item

  template := "[mult][options]parmtype[([keyword [=alias] ...] ; name [[=alias] ...]... )] ..."

  mult     :=  Multiplicator, numerical value > 0, default = 1
               (multiplicator for the following elements)

  options  :=  R = required    param must be specified
               K = keyword     the keyword must be given in the arguments if param is used
               L = last        rest of tokens without syntactical interpretation

  parmtype :=  N = numeric     digits (without sign) limited to long
               T = toggle      allows ON OFF YES NO (boolean, results in true/false)
               W = switch      = on (true) if keyword is given
               S = string      a word or "string" or 'string' or (string)
               X = Name [List] = 1 [2, 3, ...] like the position in
                                the list.

  (keyword; name) : an optional keyword, a list of names for parmtype X
                    The list of names starts after the char ';' 1)

  container  := array of long, for each entry in the template and
                in the order of the entries in the template.
                the container will be filled with...

                parmtype
                ------
                     N : the numeric value
                   W T : 0 (=false) or 1 (=true)
                   L S : a pointer to a zero-terminated string
                     X : the postion in the list

  arguments       := the string to #PINP_parse.
  stringcontainer := optional pointer to an object of class CStringContainer. 3)

  parser returns '0' for successful operation or '1' if there are no arguments
   (empty argument string or comment only)
  parser throws the exception CParser* if an error occurs. After the try block you
  can use for example:

    catch (CParser *err)
    {
      printf ("Parameter error: %s (rc=%d)\n", err->parserr(), err->rc);
      abort ();
    }

hints:

1.  option X: you can define aliases for the keyword and in the list of names,
    use '=' between the alias - for example:
    X(;A=B=C D=E F G=H)
    results:
    A, B & C generates index 1, D & E: 2, F: 3, G & H: 4

2.  these inputs are treated as comments:
    - a '#' in the 1st column (returns PRS_RC_empty)
    - the rest of line after a '#' is ignored, but not inside parenthesis or quotes.

3.  You can use the same parser object for to parse different arguments and the same
    template. But there is a problem with string arguments: the container entries of such
    arguments points to an area inside the parser object. This area will be overriden for
    every new call of the parser.
    You have to either:
    a) copy the argument strings in an area which is under your control. or:
    b) define a "String Container":

      CStringContainer myStrCont;
        and use this as a 3rd parameter in the parser call:

      int ok = myparser.parse ((long *)&container, arguments, &myStrCont);

      One String Container can be used for every call to parser, also for different
      Parser Objects.

4.  There are 3 little helpers as methods of the CParser class:

      char *CParser::join     (char **);        input:  the list of arguments as you got it
                                                        as the 2nd param of main()
                                                output: the concatenated argument string
      char *CParser::parmjoin (char **);        input:  same as for join()
                                                output: the concatenated argument string
                                                        without the 1st param (the program name)
      char *CParser::parserr  (void);           output: standard error text

#endif // comment



#ifndef parser_h
#define parser_h

const int MAX_ARGUMENTS = 2000;
const int ERRSTRLEN     = 100;
const int ERRTOKLEN     = 10;

#include "lists.h"

/********************************************************************************************/
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
const int PRS_NUMBER_rc                  = 14;

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
const int PRS_NUMBER_te                  = 11;

typedef LI_Node <char *> CStringNode;

/********************************************************************************************/
class CStringContainer : public LI_List <char *>
{
public:
  ~CStringContainer ();
};

/********************************************************************************************/
struct NameT
{
  char      *string;
  int        length;    /* of string */

  NameT () : string (NULL), length (0) {}
};

struct NE;

/********************************************************************************************/
struct TE    /* the template entry */
{
  LI_List <NE> nameList;
  bool         closed;
  bool         stringArg; /* for type X: should be xlate to index */
  short        c_index;   /* into container      */
  char         parmType;  /* .TXNBS              */
  char         options;   /* .LKR                */

};

/********************************************************************************************/
struct NE    /* the name entry */
{
  NameT         name;
  unsigned char type;      /* K=keyword, X=symbolic Name */
  short         l_index;   /* of postion in list         */
};

/********************************************************************************************/
struct AE  /* the argument entry */
{
  NameT         argument;
  bool          closed;
  unsigned char type;    /* 'K' = could be a keyword     */
                         /* 'S' = is definitely a string */
};

/********************************************************************************************/
class CParser
{
  CStringContainer *m_string_container;
  char              m_work [MAX_ARGUMENTS];
  char              m_errtoken [ERRTOKLEN+1];
  bool              m_opt_last_parm_given;
  char             *m_param;

public:
  int      rc;

  CParser (char *templat);        /* input:  the description of the args */
  ~CParser ();

  parse
    ( long *container             /* output: the edited arguments        */
    , char *arguments             /* input:  the list of arguments       */
    , CStringContainer * = NULL   /* input:  optional String Container   */
    );

  char *join     (char **);       /* input:  the list of arguments       */
  char *parmjoin (char **);       /* input:  the list of arguments       */
  char *parserr  (void);

protected:
  LI_List <TE> m_templList;
  LI_List <AE> m_argList;


private:
  LI_Node <NE> *newNE  (LI_Node <TE> *te, char *cursor, char type, int listIndex);
  LI_Node <TE> *newTE  (char  parmType, unsigned char options, int contIndex);
  int search_argument  (LI_Node <TE>* te, NE& ne, long *container);

};



#endif /*parser_h*/
