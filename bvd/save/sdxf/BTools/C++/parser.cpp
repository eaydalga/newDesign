/* doc-----------------------------------------------------------------------------
:Project.   common tools
:Program.   parser.c
:PrType.    function lib
:Title.     Parsing, analyzing and edit a list of arguments
:Version.
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI-C++
:Compiler.
:Created.   1995-11-23
:Changed.   2001-01-13 -- new design of parsing
:Changed.   2003-04    -- changed to C++ with OO: class CParser
:Changed.   2003-05-20 -- '//' as comment delimiter
:Note.      not multithread capable: do not use it in the server thread proc
------------------------------------------------------------------------------------*/

//#define xtest

#define DOUBLE_SLASH_AS_COMMENT

/*
:function.  parse
:todo.      new error te_option_K_but_no_keyword
*/
static char THIS_FILE[] = __FILE__;

bool parse_option_remove_parenthesis = true;

#define NIL 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lists.h"
#include "parser.h"

#define BACKSLASH  '`'  //'\\'

#ifdef trap // memory leaks:
    #ifdef _DEBUG
      #define MYDEBUG_NEW   new( _NORMAL_BLOCK, __FILE__, __LINE__)
    #else
      #define MYDEBUG_NEW
    #endif // _DEBUG

    #include "crtdbg.h"

    #ifdef _DEBUG
      #define new MYDEBUG_NEW
    #endif
#endif //end-trap

#ifndef xtest
  #define printf
#endif

enum
{
  PT_numeric              = 'N'
, PT_switch               = 'W'
, PT_toggle               = 'T'
, PT_name                 = 'X'
, PT_string               = 'S'
, NT_keyword              = 'K'
, NT_symbol               = 'Y'
, AT_keyword              = 'K'  /* keyword followed by a '='   */
, AT_name                 = 'N'  /* could be a key word or arg  */
, AT_string               = 'S'  /* cannot be a keyword         */
, OPT_keyword_requested   = 'K'
, OPT_last_parameter      = 'L'
, OPT_parameter_requested = 'R'
};

enum
{
  PRS_RC_program_error_1 = 101
, PRS_RC_program_error_2 = 102
, PRS_RC_program_error_3 = 103
, PRS_RC_program_error_4 = 104
};

/* options:                             */
enum
{
  OPTION_R   /*  0   R = required */
, OPTION_K   /*  1   K = keyword  */
, OPTION_L   /*  2   L = last     */
};

enum
{
  MASK_R = (1 << OPTION_R)
, MASK_K = (1 << OPTION_K)
, MASK_L = (1 << OPTION_L)
};

void PRS_dmy() {}  /* dummy for MVS */


/* help routines; */

static bool    isEqual (NameT, NameT);
static int     cleanup (int, char *);
static bool    isNumeric       (NameT);
static char   *namestr (LI_Node <NE>* p_ne);

#define THROW_ERROR(p_rc, p_token) {\
        rc = p_rc; strncpy (m_errtoken, p_token, ERRTOKLEN);    \
        throw this;}

/********************************************************************/
CParser::CParser (char *p_template)
                       /* input:  the descr. of the args */

{
  int            contIndex, listIndex, status, error;
  long           mult;
  char          *cursor, *begkw;
  char           parmType;
  unsigned char  options;
  bool           eoString, readNext;

  m_param = NULL;
  LI_Node <NE> *neNode;
  LI_Node <TE> *teNode;

  /****************************************************/
  /*  1. parse template, create list of TE            */
  /*     results contIndex                            */
  /****************************************************/

  printf ("\n--- step 1 ---\n");

  m_templList.init ();
  m_argList.init   ();

  m_opt_last_parm_given = false;

  cursor      = p_template;
  contIndex   = -1;
  listIndex   = 0;

  eoString    = false;
  readNext    = true;
  status      = 0;           /* skip blanks */

  while (! eoString)
  {
    #ifdef test1
      printf("status=%d, char='%c'\n", status, *cursor);
    #endif

    switch (status)
    {
    case 0 :      /* skip blanks before options  */
      switch (*cursor)
      {
      case NIL :
        eoString = true;
        break;

      case ' ' :
        break;

      default:
        ++contIndex;
        readNext  = false;      /* hold char */
        options   = 0;
        parmType  = 0;
        mult      = 0;
        status    = 1;   /* new template */
      }/*end-switch (*cursor) */
      break;

    case 1 :      /* read options  */
      switch (*cursor)
      {
      case NIL :
      case ' ' :
        teNode = newTE (parmType, options, contIndex);
        status  = 6;
        break;    /* end of param */

      case '(' :
        teNode = newTE (parmType, options, contIndex);
        begkw   = cursor;
        status  = 2;
        break;

        /* store parmType */
      case PT_numeric :
      case PT_switch  :
      case PT_toggle  :
      case PT_name    :
      case PT_string  :
        parmType = *cursor;
        break;

        /* store option */
      case OPT_keyword_requested   : options |= MASK_K; break;
      case OPT_last_parameter      :
        m_opt_last_parm_given = true;
                                     options |= MASK_L; break;
      case OPT_parameter_requested : options |= MASK_R; break;

      case ')' :
        THROW_ERROR (PRS_RC_te_unexp_closing_parenthesis, cursor);

      default:
        if (*cursor >= '0' && *cursor <= '9'
        &&   options == 0  && parmType == 0)
          mult = 10 * mult + *cursor - '0';
        else
          THROW_ERROR (PRS_RC_te_illegal_option, cursor);
      }/*end-switch (*cursor) */
      break;

    case 2 :      /* skip blanks before keywords */
      switch (*cursor)
      {
      case ' ' :
        break;        /* skip blanks   */

      case ')' :
        status = 6;   /* end of param */
        break;

      case ';' :
        if (parmType != PT_name)
          THROW_ERROR (PRS_RC_te_unexp_nameList, cursor);

        listIndex = 1;
        status    = 4;   /* end of param */
        break;

      case '(' :
        THROW_ERROR (PRS_RC_te_unexp_opening_parenthesis, cursor);

      case NIL :
        THROW_ERROR (PRS_RC_te_no_closing_parenthesis, cursor);

      default:
        neNode = newNE (teNode, cursor, NT_keyword, 0);
        status = 3;  /* read keyword  */

      }/*end-switch (*cursor) */
      break;

    case 3 :      /* read keyword  */
      switch (*cursor)
      {
      case ' ' :
        status  = 7;
        break;      /* next list element */

      case '=' :    /* next keyword is alias */
        status  = 2;
        break;      /* next list element */

      case ')' :
        status = 6;
        break;

      case ';' :
        listIndex = 1;
        status    = 4;
        break;

      case '(' :
        THROW_ERROR (PRS_RC_te_unexp_opening_parenthesis, cursor);

      case NIL :
        THROW_ERROR (PRS_RC_te_no_closing_parenthesis, cursor);

      default:
        ++(neNode->info.name.length);
      }/*end-switch (*cursor) */
      break;

    case 4 :      /* overread blanks before Name */
      switch (*cursor)
      {
      case ' ' :
        break;

      case NIL :
        THROW_ERROR (PRS_RC_te_no_closing_parenthesis, cursor);

      default:

        neNode = newNE (teNode, cursor, NT_symbol, listIndex);
        status = 5;

      }/*end-switch (*cursor) */
      break;

    case 5 :      /* read Name */
      switch (*cursor)
      {
      case ' ' :
        ++listIndex;
        /*fallthru*/

      case '=' :     /* alias for Name */
        status = 4;
        break;

      case ')' :
        status = 6;
        break;

      case ';' :
        THROW_ERROR (PRS_RC_te_unexp_semicolon, cursor);

      case '(' :
        THROW_ERROR (PRS_RC_te_unexp_opening_parenthesis, cursor);

      case NIL :
        THROW_ERROR (PRS_RC_te_no_closing_parenthesis, cursor);

      default:
        ++(neNode->info.name.length);

      }/*end-switch (*cursor) */
      break;

    case 6:      /* next template */
      if (mult > 1)
      {
        --mult;
        ++contIndex;
        cursor = begkw;
        status = 1;
      }
      else
        status = 0;   /* end of param */

      readNext = false;
      break;

    case 7:      /* overread blanks after keyword */
      switch (*cursor)
      {
      case ' ' :
        break;

      case '=' :     /* next keyword is alias */
        status  = 2;
        break;      /* next list element */

      case ')' :
        status = 6;
        break;

      case ';' :
        listIndex = 1;
        status    = 4;
        break;

      case '(' :
        THROW_ERROR (PRS_RC_te_unexp_opening_parenthesis, cursor);

      case NIL :
        THROW_ERROR (PRS_RC_te_no_closing_parenthesis, cursor);

      default:
        THROW_ERROR (PRS_RC_te_only_one_keyword_allowed, cursor);

      }/*end-switch (*cursor) */
      break;

    default:
      THROW_ERROR (PRS_RC_program_error_1, NULL);
    }/*end-switch (status) */

    if (*cursor && readNext) ++cursor;
    readNext = true;
  }/*end-while (*cursor) */

  error = 0;
  switch (status)   /* end of template handling */
  {
    case 0 : if (!teNode) error = PRS_RC_te_empty;      break;
    case 1 : error = PRS_RC_te_no_keyword_after_option; break;
    case 2 :
    case 3 : error = PRS_RC_te_no_closing_parenthesis;  break;
    default: error = PRS_RC_program_error_3;
  }/*end-switch (status) */


  #ifdef xtest
      printf("\n---testausgabe templ List:\n");
      printf("last contIndex=%d\n", contIndex);
      teNode = m_templList.getFirst ();

      while (teNode)
      {
        printf ("c_index=%d, options=%1X, type =%c\n ",
          teNode->info.c_index, teNode->info.options, teNode->info.parmType);

        neNode = teNode->info.nameList.getFirst ();
        while (neNode)
        {
          printf ("   l_index=%d, type = %c, name=(%d)'%s'\n ",
            neNode->info.l_index, neNode->info.type, neNode->info.name.length, namestr (neNode));
          neNode = teNode->info.nameList.getNext ();
        }

        teNode = m_templList.getNext ();
      }
      printf ("---end-list\n");
  #endif  /*test*/

  if ( error ) THROW_ERROR (error, cursor);

}//end-Constructor


/******************************************************************* little Helper *****/
static char *toggleset [4]  = {"ON","OFF","YES","NO"};

void setToggle (NameT p_toggle [])
{
  for ( int ix = 0; ix < 4; ++ix )
  {
    p_toggle [ix].string = toggleset [ix];
    p_toggle [ix].length = strlen (p_toggle [ix].string);
  }

}//end-setToggle

/********************************************************************/
bool end_of_line (char *p_cursor)
{
  switch ( *p_cursor )
  {
  case '\n':         /* end of line */
  case 0x0D:         /* end of line */
  case '#':          /* comment */
    *p_cursor = NIL;
    break;

#ifdef DOUBLE_SLASH_AS_COMMENT
  case '/':          /* comment ? */
    if ( *(p_cursor + 1) == '/' )
      *p_cursor = NIL;   /* comment ! */
    break;
#endif
  }//end-switch

  return *p_cursor == NULL;
}//end- end_of_line

/********************************************************************/
int CParser::parse
  ( long             *p_container         /* output: the edited arguments        */
  , char             *p_arguments         /* input:  the list of arguments       */
  , CStringContainer *p_string_container  /* input:  optional String Container   */
  )
{
  int        status, found, error, plevel;
  long       value;
  char      *cursor, rightquote, *rightcursor;
  char       workchar, argType;
  bool       startWithParen;

  LI_Node <NE> *neNode;
  LI_Node <TE> *teNode;
  LI_Node <AE> *aeNode;

  /*  overview:                                                 */


  /*  0. check arguments : empty?                               */
  /*  2. _parse__ arguments                                     */
  /*  3. loop over template list:                               */
  /*     only keywords: search keywords in argument list.       */
  /*     if found: assign next argument to template element.    */
  /*     check requested keywords (error)                       */
  /*  4. loop over template list:                               */
  /*     check requested keywords (close TE)                    */
  /*     match only symbols to arguments                        */
  /*     check requested symbols (error)                        */
  /*  5. balancing remaining entries templates vs. arguments    */
  /*  6. processing arguments (numerics, etc.?)                 */


  /* let's go:                                        */


  /****************************************************/
  /*  0. check arguments : empty?                     */
  /****************************************************/

  printf ("\n--- step 0 ---\n");


  error = PRS_RC_ok;
  if (p_arguments)
  {
    if (p_arguments [0] == '*') error = PRS_RC_empty;
  }
  else
    error = PRS_RC_empty;

  if (error) return rc = error;

  NameT toggels [4];
  setToggle (toggels);

  /****************************************************/
  /*  2. _parse__ argstring, create list of AE           */
  /****************************************************/

  printf ("--- step 2 ---\n");

  strncpy (m_work, p_arguments, MAX_ARGUMENTS-1);  /*unsecure!*/
  cursor = m_work;

  rightquote  = 0;
  rightcursor = NULL;        /* position of right quote, to set nullbyte */
  argType     = AT_name;     /* assuming keyword, til we got more info */
  plevel      = 0;           /* parenthesis level */
  status      = 1;

  while (*cursor)
  {
    switch (status)
    {
    /******/
    case 1 :                   /* overread blanks */
    /******/
      if ( end_of_line (cursor) ) break;   //side-effect: *cursor set to NIL

      switch (*cursor)
      {
                                          #ifdef  obsolete
                                                case '\n':         /* end of line */
                                                case 0x0D:         /* end of line */
                                                case '#':          /* comment */
                                                  *cursor = NIL;
                                                  --cursor;        /* neutralize ++cursor   */
                                                  break;
                                          #endif // obsolete

      case ' ' :   /* skip blanks, commas and tabs */
      case '\t':
      case ',' :
        break;

      case '\'':
      case '"':
        argType = AT_string;
        rightquote = *cursor;
        ++cursor;  /* skip " */
        status  = 3;
        break;

      case '(':
        argType = AT_string;
        ++plevel;
        if (parse_option_remove_parenthesis) ++cursor;  /* skip ( */
        rightquote = ')';
        startWithParen = true;
        status = 5;
        break;

      case ')':
        THROW_ERROR (PRS_RC_too_many_closing_parenthesis, cursor);

      default:
        startWithParen = false;
        status  = 2;
      }/* end switch cursor */

      if (status != 1)
      {
        aeNode = new LI_Node <AE>;
        m_argList.addTail (aeNode);

        aeNode->info.argument.string = cursor;
        aeNode->info.argument.length = 1;
        aeNode->info.type            = argType;
        aeNode->info.closed          = false;
        argType                      = AT_name;  /* initial for next arg. */
      }/*end-if status 1 */

      ++cursor;
      break;

    /******/
    case 2 :                   /* read argument */
    /******/
      if ( end_of_line (cursor) ) break;   //side-effect: *cursor set to NIL

      rightquote  = NIL;                                             /*%08*/
      rightcursor = NULL;                                            /*%08*/

      switch (*cursor)
      {
                                          #ifdef  obsolete
                                            case 0x0D:         /* end of line */
                                            case '\n':         /* end of line */
                                            case '#':
                                              *cursor = NIL;
                                              break;
                                          #endif // obsolete
      case '=':
        status = 6;
        break;
      case ',':
        ++cursor;
        status  = 1;
        break;
      case ' ' :
      case '\t':
        status = 4;
        break;
      case '(':
        ++plevel;
        status = 5;
        ++(aeNode->info.argument.length);
        ++cursor;
        break;
      case ')':
        THROW_ERROR (PRS_RC_too_many_closing_parenthesis, cursor);
      default:
        ++(aeNode->info.argument.length);
        ++cursor;
      }/*end-switch cursor */

      break;

    /******/
    case 3 :                  /* read argument in quotes "" '' */
    /******/
      switch (*cursor)
      {
      case 0x0D:         /* end of line */
      case '\n':         /* end of line */
        *cursor = NIL;
        break;
      default:
        if (*cursor == rightquote && *(cursor-1) != BACKSLASH)
        {
          status  = 1;
          rightcursor = cursor;
        }
        else
          ++(aeNode->info.argument.length);
      }/*end-switch cursor */

      ++cursor;
      break;

    /******/
    case 4 :                   /* overread blanks before '=' */
    /******/
      if ( end_of_line (cursor) ) break;   //side-effect: *cursor set to NIL

      switch (*cursor)
      {
                                                #ifdef  obsolete
                                                      case 0x0D:         /* end of line */
                                                      case '\n':         /* end of line */
                                                      case '#':
                                                        *cursor = NIL;
                                                        break;
                                                #endif // obsolete
      case '=':
        status  = 6;
        break;
      case ' ' :
      case '\t':
        ++cursor;
        break;
      default:
        status  = 1;
      }/*end-switch cursor */

      break;

    /******/
    case 5 :                   /* read argument inside parenthesis */
    /******/
      switch (*cursor)
      {
      case 0x0D:         /* end of line */
      case '\n':         /* end of line */
            // '#' and '//' inside '(...)' allowed  --     case '#':
        THROW_ERROR (PRS_RC_too_many_opening_parenthesis, cursor);
      case '(':
        ++plevel;
        ++(aeNode->info.argument.length);
        ++cursor;
        break;

      case ')':
        --plevel;
        ++cursor;

        if (plevel == 0)
        {
          if (! startWithParen || ! parse_option_remove_parenthesis)
            ++(aeNode->info.argument.length);

          startWithParen = false;
          status = 2;
        }
        else if (plevel < 0)
          THROW_ERROR (PRS_RC_too_many_closing_parenthesis, cursor)
        else
          ++(aeNode->info.argument.length);

        break;

      default:
        ++(aeNode->info.argument.length);
        ++cursor;
      }/*end-switch cursor */

      break;

    /******/
    case 6 :                   /* handle '=' */
    /******/
        ++cursor;
        argType = AT_string;
        if (!aeNode) THROW_ERROR (PRS_RC_unexpected_equal_sign, cursor);
        aeNode->info.type = AT_keyword;
        status = 1;
        break;

    /******/
    default:
    /******/
      THROW_ERROR (PRS_RC_program_error_2, NULL);
    }/*end-switch status */

  }/*end-while *cursor */

  if (plevel > 0)
    THROW_ERROR (PRS_RC_too_many_opening_parenthesis, cursor);

  if (rightcursor) *rightcursor = NIL;

  if (m_argList.isEmpty ())
    return PRS_RC_empty;
    // THROW_ERROR (PRS_RC_empty, cursor);

  #ifdef xtest
    printf("\n---testausgabe arg List:\n(arguments='%s')\n",
      p_arguments);

    aeNode = m_argList.getFirst ();
    while (aeNode)
    {
      printf (" argument=%s^ \n",aeNode->info.argument.string);
      printf (" length  =%d  \n",aeNode->info.argument.length);
      printf (" type    =%c  \n",aeNode->info.type);
      aeNode = m_argList.getNext ();
    }
    printf ("---end-list\n");
  #endif  /*test*/


  /**************************************************************/
  /*  3.1 loop over argument list:                              */
  /*     only forced (=) keywords: for every =keywords in       */
  /*     argument list: search it in template/name list.        */
  /*     if not found: error "keyword unknown"                  */
  /**************************************************************/
  printf ("\n--- step 3.1 ---\n");

  for (aeNode = m_argList.getFirst ();
       aeNode;
       aeNode = m_argList.getNext ()
    )
  {
    if (aeNode->info.type != AT_keyword) continue;

    found = false;
    for (teNode = m_templList.getFirst ();
         teNode && ! found;
         teNode = m_templList.getNext ()
        )
    {
      /* loop over keyword aliases */

      for (neNode = teNode->info.nameList.getFirst ();
           neNode && ! found;
           neNode = teNode->info.nameList.getNext ()
          )
      {
        if (neNode->info.type != NT_keyword) continue;     /* only keywords */
        if (! (found = isEqual (neNode->info.name, aeNode->info.argument) ))  continue;

      }/*next ne */
    }/*next te */

    if (! found && ! m_opt_last_parm_given)
      THROW_ERROR (PRS_RC_unknown_keyword, aeNode->info.argument.string);

  }/*next ae */

  /**************************************************************/
  /*  3.2 loop over template list:                              */
  /*     only keywords: search keywords in argument list.       */
  /*     if found: assign next argument to template element.    */
  /*     check requested keywords (error)                       */
  /**************************************************************/
  printf ("\n--- step 3.2 ---\n");

  for (teNode = m_templList.getFirst ();
       teNode;
       teNode = m_templList.getNext ()
      )
  {
    /* loop over keyword aliases */
    found = false;
    for (neNode = teNode->info.nameList.getFirst ();
         neNode && ! found;
         neNode = teNode->info.nameList.getNext ()
        )
    {
      if (neNode->info.type != NT_keyword) continue;     /* only keywords */

      rc = search_argument (teNode, neNode->info, p_container);
      switch (rc)
      {
      case  0: continue;
      case -1: found = true; break;
      default: THROW_ERROR (rc, namestr (neNode));
      }
    }/*end-for NE */

    if (! found && (teNode->info.options & MASK_R) && (teNode->info.options & MASK_K))
      THROW_ERROR (PRS_RC_keyword_param_requested, namestr (teNode->info.nameList.getFirst ()));

  } /*end-for TE */

  #ifdef test
      printf("\n---testausgabe templ List:\n");
      te = (struct TE *) m_templList.getFirst ();

      while (te)
      {
        printf ("c_index=%d, options=%1X, type =%c, closed=%d\n ",
          te->c_index, te->options, te->parmType, te->closed);

        te = (struct TE *) m_templList.getNext (m_templList);
      }
      printf ("---end-list\n");

  #endif /*test*/

  /**************************************************************/
  /*  4. loop over template list:                               */
  /*     check requested keywords (close TE)                    */
  /*     match only symbols to arguments                        */
  /*     check requested symbols (error)                        */
  /**************************************************************/
  printf ("\n--- step 4 ---\n");

  for (teNode = m_templList.getFirst ();
       teNode;
       teNode = m_templList.getNext ()
      )
  {
    bool symbols_defined = false;

    if (teNode->info.closed)       continue;
    if (teNode->info.options & MASK_K) /* if keyword requested... */
    {
      teNode->info.closed = true;      /* keyword not found in arglist in step 3 */
      continue;
    }

    found = false;

    for (neNode = teNode->info.nameList.getFirst ();
         neNode && ! found;
         neNode = teNode->info.nameList.getNext ()
        )
    {
      if (neNode->info.type != NT_symbol) continue;
      symbols_defined = true;

      rc = search_argument (teNode, neNode->info, p_container);
      switch (rc)
      {
      case  0: break;
      case -1: found = true; break;
      default: THROW_ERROR (rc, namestr (neNode));
      }

    }/*end-for next NE */

    if (symbols_defined && ! found)
    {
      if (teNode->info.options & MASK_R)
        THROW_ERROR (PRS_RC_keyword_param_requested, namestr (neNode))
      else
        teNode->info.closed = true;
    }

  } /*end-for TE */

  #ifdef test
      printf("\n---testausgabe templ List:\n");
      te = (struct TE *) m_templList.getFirst ();

      while (te)
      {
        printf ("c_index=%d, options=%1X, type =%c, closed=%d\n ",
          te->c_index, te->options, te->parmType, te->closed);

        te = (struct TE *) m_templList.getNext ();
      }
      printf ("---end-list\n");
      printf("\n---testausgabe argument List:\n");


      for (ae = (AE *) m_argList.getFirst ();
           ae;
           ae = (AE *) m_argList.getNext ())

      {
        printf ("arg=(%d)'%s', closed=%d\n ",
          aeNode->info.argument.length, aeNode->info.argument.string, aeNode->info.closed);
      }
      printf ("---end-list\n");

  #endif /*test*/


  /**************************************************************/
  /*  5. balancing remaining entries templates vs. arguments    */
  /*     (but not for booleans, they are handled in step 3)     */
  /*     (Toggles ar checked against proper values)             */
  /**************************************************************/

  printf ("\n--- step 5 ---\n");

  aeNode = m_argList.getFirst ();
  teNode = m_templList.getFirst ();

  while (aeNode && teNode)
  {
    char *str;

    if (aeNode->info.closed)
    {
      aeNode = m_argList.getNext ();
      continue;
    }

    if (teNode->info.closed || teNode->info.parmType == PT_switch || (teNode->info.options & MASK_K) )
    {
      teNode = m_templList.getNext ();
      continue;
    }
    else if (teNode->info.parmType == PT_numeric)
    {
      if (! isNumeric (aeNode->info.argument))
      {
        teNode = m_templList.getNext ();
        continue;
      }
    }
    else if (teNode->info.parmType == PT_toggle)
    {
      int ix = 0;

      while (ix < 4 && ! found)
        if (! (found = isEqual (toggels [ix], aeNode->info.argument))) ++ix;

      if (! found)
      {
        teNode = m_templList.getNext ();
        continue;
      }
    } /*end-if closed, parmtype Switch or Toggle */

    aeNode->info.closed    = true;
    teNode->info.closed    = true;
    teNode->info.stringArg = true;

    str = aeNode->info.argument.string;
    p_container [teNode->info.c_index] = (long) str;

    if (teNode->info.options & (MASK_L))
    {
      aeNode  = m_argList.getLast ();
      str = aeNode->info.argument.string;
      str [aeNode->info.argument.length] = NIL;
      aeNode  = NULL;
      break;
    }

    str [aeNode->info.argument.length] = NIL;

    teNode = m_templList.getNext ();
    aeNode = m_argList.getNext ();

  }/*end-while ae and te */

  if (aeNode) THROW_ERROR (PRS_RC_too_much_args, aeNode->info.argument.string);


  /****************************************************/
  /*  6. processing arguments (numerics, etc.?)       */
  /*     check templList for options = R              */
  /*     Conversions for option = X, N and T          */
  /*     NIL-char for string argument                 */
  /****************************************************/

  printf ("--- step 6 ---\n");

  teNode = m_templList.getFirst ();
  while (teNode)
  {
    char *arg = (char *)p_container [teNode->info.c_index];

    if ( (! teNode->info.closed) && (teNode->info.options & MASK_R) )
      THROW_ERROR (PRS_RC_too_few_args, arg)

    else if (!teNode->info.closed)
    {}

    else if (teNode->info.parmType == PT_numeric && teNode->info.stringArg)
    {
      unsigned int ix;

      value  = 0;
      for (ix = 0; ix < strlen (arg); ++ix )
      {
        workchar = arg [ix];

        if (workchar < '0' || workchar > '9')
          THROW_ERROR (PRS_RC_numeral_expected, arg);

        value = 10 * value + workchar - '0';
      }/* next ix */

      p_container [teNode->info.c_index] = value;
    }/*end-if option N */

    else if ( teNode->info.parmType == PT_name
              && arg
              && teNode->info.stringArg
            )
    {
      AE ae;
      ae.argument.string = arg;
      ae.argument.length = strlen (ae.argument.string);

      neNode = teNode->info.nameList.getFirst ();
      found = false;

      while (neNode && ! found)
      {
        if (neNode->info.type != NT_symbol
        || ! (found = isEqual (neNode->info.name, ae.argument)))
          neNode = teNode->info.nameList.getNext ();
      }/* end-while namelist */

      if (found)
        p_container [teNode->info.c_index] = neNode->info.l_index;
      else
        THROW_ERROR (PRS_RC_wrong_param_after_keyword, namestr (neNode));
    }

    else if (teNode->info.parmType == PT_toggle && arg && teNode->info.stringArg)
    {
      if (toupper (arg [0]) == 'O')
      {
        if (toupper (arg [1]) == 'N')
          p_container [teNode->info.c_index] = 1;
        else if (toupper (arg [1]) == 'F')
          p_container [teNode->info.c_index] = 0;
        else
          THROW_ERROR (PRS_RC_wrong_toggle_arg, arg);
      }
      else if (toupper (arg [0]) == 'Y')
      {
        p_container [teNode->info.c_index] = 1;
      }
      else if (toupper (arg [0]) == 'N')
      {
        p_container [teNode->info.c_index] = 0;
      }
      else
      {
        THROW_ERROR (PRS_RC_wrong_toggle_arg, arg);
      }

    }/*end-if  */

    teNode = m_templList.getNext ();
  }/*end-while te */


  printf ("\n--- end parser ---\n");

  // cleanup argument list:

  LI_Node <AE> *ae, *ae1 = NULL;
  for (ae = m_argList.getFirst (); ae; ae = m_argList.getNext ())
  {
    if (ae1) delete ae1;
    ae1 = ae;
  } // next ae

  delete ae1;
  m_argList.init ();

  printf("\n---testausgabe templ List:\n");

  // reopens nodes in TE-List:

  teNode = m_templList.getFirst ();

  while (teNode)
  {
    #ifdef xtest
      if (teNode->info.parmType == 'S')
        printf ("S:%s\n", p_container [teNode->info.c_index]);
      else
        printf ("%c:%d\n", teNode->info.parmType, p_container [teNode->info.c_index]);
    #endif  /*test*/

    teNode->info.closed = false;
    teNode = m_templList.getNext ();
  }
  printf ("---end-list\n");

  if (p_string_container)
    for (teNode = m_templList.getFirst (); teNode; teNode = m_templList.getNext ())
    {
      if (teNode->info.parmType != 'S' || p_container [teNode->info.c_index] == 0) continue;

       char *str = new char [strlen ((char *)p_container [teNode->info.c_index])+3];
                                    // +3 instead of +1 for a little buffer
       strcpy (str, (char *)p_container [teNode->info.c_index]);
       CStringNode *sn = new CStringNode (str);
       p_string_container->addTail (sn);
       p_container [teNode->info.c_index] = (long)sn->info;
    }

  return PRS_RC_ok;

}/*end-function parse */

/******************************************************************/
CStringContainer::~CStringContainer ()
{
  CStringNode *node;

  while (node = remHead ())
  {
    if ( node->m_del_info ) delete [] node->info;
    delete node;
  }
}//end- ~CStringContainer

/******************************************************************/
char * CParser::parmjoin (char **p_argv)
/* joining cmd line parameters without the 1st (prog name) */
{
  static int   plen  = 0;
  int ix, len;

  #define MAXARGS 30
  #ifdef _SYS_MVS
      #define FIRST 0
  #else
      #define FIRST 1
  #endif  /* _SYS_MVS */

  /* calculate memory */
  len = 0;
  for (ix = FIRST; ix < MAXARGS && p_argv[ix]; ++ix)
    len += strlen (p_argv[ix]) + 2;  /* + null + blank */

  if (len == 0) return NULL;

  if (plen < len)
  {
    if (m_param) free (m_param);
    m_param = (char *)malloc (len);
    plen  = len;
  }

  m_param [0] = 0;

  for (ix = FIRST; ix < MAXARGS && p_argv[ix]; ++ix)
  {
    strcat (m_param, " ");
    strcat (m_param, p_argv[ix]);
  }

  return m_param;

}/*end-parmjoin */

/******************************************************************/
char * CParser::join (char **p_argv)
/* joining cmd line parameters including the 1st (prog name) */
{
  static int   plen  = 0;
  int ix, len;

  #define MAXARGS 30

  /* calculate memory */
  len = 0;
  for (ix=0; ix < MAXARGS && p_argv[ix]; ++ix)
    len += strlen (p_argv[ix]) + 2;  /* + null + blank */

  if (len == 0) return NULL;

  #ifdef _SYS_MVS
    len += 10;     /* simulate 1.st param = *program */
  #endif  /* _SYS_MVS */

  if (plen < len)
  {
    if (m_param) free (m_param);
    m_param = (char *)malloc (len);
    plen  = len;
  }

  m_param [0] = 0;

  #ifdef _SYS_MVS
    strcat (_param, "*PROGRAM");
  #endif  /* _SYS_MVS */

  for (ix=0; ix < MAXARGS && p_argv[ix]; ++ix)
  {
    strcat (m_param, " ");
    strcat (m_param, p_argv[ix]);
  }

  return m_param;

}/*end-join */

/******************************************************************/
char * CParser::parserr (void)
{
  #define TEMPLEN 100
  static char temp [TEMPLEN];

  static char *argmess [] =
  {
     "ok"                             /*  0 */
    ,"no parameters given"            /*  1 */
    ,"keyword param expected"         /*  2 */
    ,"too much arguments"             /*  3 */
    ,"too few arguments"              /*  4 */
    ,"a numerical expected"           /*  5 */
    ,"wrong toggle argument"          /*  6 */
    ,"value after keyword expected"   /*  7 */
    ,"wrong param after keyword"      /*  8 */
    ,"param needs keyword"            /*  9 */
    ,"too many opening parenthesis"   /* 10 */
    ,"too many closing parenthesis"   /* 11 */
    ,"unexpected equal sign"          /* 12 */
    ,"unknown keyword"                /* 13 */
  };

  static char *temess [] =
  {
     "illegal option"                 /* 81 */
    ,"keyword after option expected"  /* 82 */
    ,"closing parenthesis expected"   /* 83 */
    ,"unexpected closing parenthesis" /* 84 */
    ,"unexpected opening parenthesis" /* 85 */
    ,"no templates given"             /* 86 */
    ,"unexpected nameList"            /* 87 */
    ,"unexpected blank after keyword" /* 88 */
    ,"semicolon unexpected"           /* 89 */
    ,"equal sign unexpected"          /* 90 */
    ,"only one keyword allowed"       /* 91 */
  };

  m_errtoken [ERRTOKLEN] = 0;

  if (rc >= 0 && rc < PRS_NUMBER_rc)
  {
    if (rc == PRS_RC_empty)
      strncpy (temp,  argmess [rc], TEMPLEN);
    else
      sprintf (temp, "%s nearby '%s'", argmess [rc], m_errtoken);
  }
  else
  {
    rc -= PRS_RC_te_base;
    if (rc >= 0 && rc < PRS_NUMBER_te)
      sprintf (temp, "parse template error: %s nearby '%s'",
        temess [rc], m_errtoken);
    else
      sprintf (temp, "internal parse error %d", rc + PRS_RC_te_base - 100);
  }

  return temp;

}/*end-parserr */

#undef  printf
#define printf
/******************************************************************/
static bool isEqual (NameT p_sn1, NameT p_sn2)
{
  int ix;

  printf ("isEqual?: (%d)%s : (%d)%s === ",
      p_sn1.length, p_sn1.string, p_sn2.length, p_sn2.string);

  if (! p_sn1.string || ! p_sn2.string) return false;
  if (  p_sn1.length !=   p_sn2.length)
  {
    printf  ("no\n");
    return false;
  }

  for (ix=0; ix < p_sn2.length; ++ix)
    if (p_sn1.string [ix] != toupper (p_sn2.string [ix]))
    {
      printf  ("no\n");
      return false;
    }

    printf  ("yes\n");

  return true;

}/*end-isEqual */

/******************************************************************/
static bool isNumeric (NameT p_sn)
{
  int ix;


  if (! p_sn.string) return false;
  if (! p_sn.length) return false;

  for (ix=0; ix < p_sn.length; ++ix)
    if (p_sn.string [ix] < '0' || p_sn.string [ix] > '9') return false;

  return true;

}/*end-isNumeric */


/******************************************************************/
LI_Node <TE> * CParser::newTE (char p_parmType, unsigned char p_options, int p_contIndex)
{
  LI_Node <TE> *node = new LI_Node <TE>;

  node->info.nameList.init ();
  node->info.closed     = false;
  node->info.stringArg  = false;         /* for type X: should be xlate to index */
  node->info.c_index    = p_contIndex;   /* into container      */
  node->info.parmType   = p_parmType;    /* .TXNBS              */
  node->info.options    = p_options;     /* .LKR                */

  m_templList.addTail (node);

  return node;
}/*end-newTE */

/******************************************************************/
LI_Node <NE> * CParser::newNE (LI_Node <TE> *p_te, char *p_cursor, char  p_type, int p_listIndex)
{
  LI_Node <NE> *node = new LI_Node <NE>;

  node->info.name.string = p_cursor;
  node->info.name.length = 1;
  node->info.type        = p_type;        /* K=keyword, X=symbolic Name */
  node->info.l_index     = p_listIndex;   /* of postion in list         */

  p_te->info.nameList.addTail (node);

  return node;

}/*end-newNE */



/******************************************************************/
char *namestr (LI_Node <NE>* p_ne)
{
  static char str [100] = "?";
  int         len;

  if (! p_ne) return str;

  len = p_ne->info.name.length;
  strncpy (str, p_ne->info.name.string, len);
  str [len] = 0;
  return str;

} /*end- namestr */

/******************************************************************/
int CParser::search_argument (LI_Node <TE>* p_te, NE& p_ne, long *p_container)
/*  returns: -1 found
              0 not found
             >0 error code
*/
{
  LI_Node <AE>*  aeNode;
  bool found = false;
  int  contIndex = -1;

  for (aeNode =  m_argList.getFirst ();
       aeNode && ! found;
       aeNode = m_argList.getNext ()
    )
  {
    if (aeNode->info.closed || aeNode->info.type == AT_string)    continue;
    if (! (found = isEqual (p_ne.name, aeNode->info.argument) ))  continue;

    /* we found a match between template keyword and an arg. token: */
    contIndex           = p_te->info.c_index;
    p_te->info.closed   = true;
    aeNode->info.closed = true;

    if (p_te->info.parmType == PT_switch)
    {
      p_container [contIndex] = true;          /* switch is on  */
      return -1;
    }

    if (p_ne.type == NT_keyword)
    {
      if (! (aeNode = m_argList.getNext ()))  /* argument after keyword */
        return PRS_RC_no_value_after_keyword;
    }

    aeNode->info.closed     = true;
    p_te->info.stringArg    = true;
    p_container [contIndex] = (long)(aeNode->info.argument.string);

    /* keyword found. but TE points to the argument string    */
    /* analysis and correct setting of container in section 6 */

    if (p_te->info.options & MASK_L)
    {    /* close remaining AE-nodes */
      for (aeNode = aeNode->Next (); aeNode; aeNode = aeNode->Next ())
        aeNode->info.closed = true;
    }
    else
    {
      char *str = aeNode->info.argument.string;
      str [aeNode->info.argument.length] = NIL;
    }/*end-if "Last Argument" or not */

  }/*end-for next AE */

  return found ? -1 : 0;
}/*end- search_argument */

/******************************************************************/
CParser::~CParser ()
{
  if (m_param) free (m_param);

  // cleanup templ list

  LI_Node <TE> *te, *te1 = NULL;
  LI_Node <NE> *ne, *ne1;

  for (te = m_templList.getFirst (); te; te = m_templList.getNext ())
  {
    ne1 = NULL;
    for (ne = te->info.nameList.getFirst (); ne; ne = te->info.nameList.getNext ())
    {
      if (ne1) delete ne1;
      ne1 = ne;
    } // next ne
    delete ne1;

    if (te1) delete te1;
    te1 = te;
  } // next te
  delete te1;

  // cleanup argument list

  LI_Node <AE> *ae, *ae1 = NULL;
  for (ae = m_argList.getFirst (); ae; ae = m_argList.getNext ())
  {
    if (ae1) delete ae1;
    ae1 = ae;
  } // next ae
  delete ae1;

}// end-destructor



/**** end *******/
