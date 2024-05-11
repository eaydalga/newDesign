/* doc-----------------------------------------------------------------------------
:Project.   common tools
:Program.   parser.c
:PrType.    function lib
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


/* 
:function.  BT_parse
:todo.      new error te_option_K_but_no_keyword 
*/ 

#define ERRTOKLEN 10

#define testx
#define test1x


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lists.h"
#include "parser.h"

#ifdef test
  #include "tools.h"
  #define printf BT_printf
  #define DEBUG  BT_printf  
#else
  #define DEBUG  
#endif

#define BACKSLASH '\\'


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

typedef struct NameT
  {
    char      *string;
    int        length;    /* of string */ 
  } NameT;
 

typedef struct TE  /* the template entry */ 
{
  LI_Node    node;
  LI_List    nameList;
  bool       closed; 
  bool       stringArg; /* for type X: should be xlate to index */
  short      c_index;   /* into container      */ 
  char       parmType;  /* .TXNBS              */ 
  Byte       options;   /* .LKR                */ 
} TE, *TEp;

typedef struct NE  /* the name entry */ 
{
  LI_Node    node;
  NameT      name;
  char       type;      /* K=keyword, X=symbolic Name */ 
  short      l_index;   /* of postion in list         */ 
} NE, *NEp;

typedef struct AE  /* the argument entry */ 
{
  LI_Node  node;
  NameT    argument;
  bool     closed; 
  Byte     type;      /* 'K' = could be a keyword     */ 
                      /* 'S' = is definitely a string */ 
} AE, *AEp;
 
static struct LI_List  g_templList;
static struct LI_List  g_argList;
static int             g_rc;
static char            g_errtoken [ERRTOKLEN];

void PRS_dmy() {}  /* dummy for MVS */ 


/* help routines; */ 

static bool    isEqual (NameT, NameT);
static int     cleanup (int, char *);
static TE     *newTE   (char parmType, Byte options, int contIndex);
static NE     *newNE   (TE *, char *cursor, char type, int listIndex);
static int     search_argument (TE*, NE*, long *container);
static bool    isNumeric       (NameT);
static char   *namestr         (NE* p_ne);

/**********************************************************************/ 
int BT_parse ( long *p_container  /* output: the edited arguments   */ 
          , char *p_templat         /* input:  the descr. of the args */ 
          , char *p_arguments       /* input:  the list of arguments  */ 
          , char *p_work            /* input:  work area              */ 
          )
{
  static char stringContainer [256];
  
  int        contIndex, listIndex, status, found, error, plevel, rc;
  long       value, mult;
  char      *cursor, rightquote, *rightcursor, *begkw;
  char       workchar, argType, parmType;
  Byte       options;
  bool       eoString, readNext, startWithParen;
  bool       opt_last_parm_given = false;

  struct NE *ne;
  struct TE *te = NULL;
  struct AE *ae = NULL;
  
  /*  overview:                                                 */ 

  /*  0. check arguments : empty?                               */ 
  /*  1. _parse__ template                                      */ 
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

  DEBUG ("\n--- step 0 ---\n");


  error = PRS_RC_ok;
  if (p_arguments)
  {
    if (p_arguments [0] == '*') error = PRS_RC_empty;
  }
  else
    error = PRS_RC_empty;
  
  if (error) return g_rc = error;

  if (! p_work) p_work = stringContainer;


  /****************************************************/ 
  /*  1. _parse__ template, create list of TE         */ 
  /*     results contIndex                            */ 
  /****************************************************/ 

  DEBUG ("\n--- step 1 ---\n");

  LI_init (&g_templList);
  LI_init (&g_argList);

  cursor      = p_templat;
  contIndex   = -1;
  listIndex   = 0; 

  eoString    = false;
  readNext    = true;
  status      = 0;           /* skip blanks */ 
    
  while (! eoString)
  {
    #ifdef test1
      BT_printf("status=%d, char='%c'\n", status, *cursor);
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
        te = newTE (parmType, options, contIndex);
        status  = 6;
        break;    /* end of param */ 

      case '(' : 
        te = newTE (parmType, options, contIndex);
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
        opt_last_parm_given = true;
                                     options |= MASK_L; break;
      case OPT_parameter_requested : options |= MASK_R; break;

      case ')' :
        return cleanup (PRS_RC_te_unexp_closing_parenthesis, cursor);

      default:
        if (*cursor >= '0' && *cursor <= '9' 
        &&   options == 0  && parmType == 0)
          mult = 10 * mult + *cursor - '0';
        else
          return cleanup (PRS_RC_te_illegal_option, cursor);
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
          return cleanup (PRS_RC_te_unexp_nameList, cursor);
        
        listIndex = 1;
        status    = 4;   /* end of param */ 
        break;   

      case '(' :
        return cleanup (PRS_RC_te_unexp_opening_parenthesis, cursor);
        
      case NIL :
        return cleanup (PRS_RC_te_no_closing_parenthesis, cursor);
        
      default:
        ne = newNE (te, cursor, NT_keyword, 0);
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
        return cleanup (PRS_RC_te_unexp_opening_parenthesis, cursor);

      case NIL :
        return cleanup (PRS_RC_te_no_closing_parenthesis, cursor);
        
      default:
        ++(ne->name.length);
      }/*end-switch (*cursor) */ 
      break;

    case 4 :      /* overread blanks before Name */ 
      switch (*cursor)
      {
      case ' ' :
        break;

      case NIL :
        return cleanup (PRS_RC_te_no_closing_parenthesis, cursor);
        
      default:

        ne = newNE (te, cursor, NT_symbol, listIndex);
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
        return cleanup (PRS_RC_te_unexp_semicolon, cursor);
        
      case '(' :
        return cleanup (PRS_RC_te_unexp_opening_parenthesis, cursor);

      case NIL :
        return cleanup (PRS_RC_te_no_closing_parenthesis, cursor);
        
      default:
        ++(ne->name.length);

      }/*end-switch (*cursor) */ 
      break;

    case 6:      /* next template */ 
      if (mult > 1)                                         /*#56*/ 
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

      case '=' :     /* next keyword is alias               %02*/ 
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
        return cleanup (PRS_RC_te_unexp_opening_parenthesis, cursor);

      case NIL :
        return cleanup (PRS_RC_te_no_closing_parenthesis, cursor);
        
      default:
        return cleanup (PRS_RC_te_only_one_keyword_allowed, cursor);

      }/*end-switch (*cursor) */ 
      break;   

    default:
      return cleanup (PRS_RC_program_error_1, NULL);
    }/*end-switch (status) */ 

    if (*cursor && readNext) ++cursor;
    readNext = true;
  }/*end-while (*cursor) */ 

  error = 0;
  switch (status)   /* end of template handling */ 
  {
    case 0 : if (!te) error = PRS_RC_te_empty;          break;
    case 1 : error = PRS_RC_te_no_keyword_after_option; break;
    case 2 : 
    case 3 : error = PRS_RC_te_no_closing_parenthesis;  break;
    default: error = PRS_RC_program_error_3;
  }/*end-switch (status) */ 


  #ifdef xtest
      printf("\n---testausgabe templ List:\n");
      printf("last contIndex=%d\n", contIndex);
      te = (struct TE *) LI_getFirst (&g_templList);
      
      while (te)
      {
        printf ("c_index=%d, options=%1X, type =%c\n ",
          te->c_index, te->options, te->parmType);

        ne = (struct NE *) LI_getFirst (&te->nameList);
        while (ne)
        {
          printf ("   l_index=%d, type = %c, name=(%d)'%s'\n ",
            ne->l_index, ne->type, ne->name.length, namestr (ne));
          ne =(struct NE *) LI_getNext (&te->nameList);
        }
     
        te = (struct TE *) LI_getNext (&g_templList);
      }
      printf ("---end-list\n");
  #endif  /*test*/ 

  if (error)  return cleanup (error, cursor);

  /****************************************************/ 
  /*  2. parse argstring, create list of AE           */ 
  /****************************************************/ 

  DEBUG ("--- step 2 ---\n");

  strcpy (p_work, p_arguments);  /*unsecure!*/ 
  cursor = p_work;

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
      switch (*cursor)
      {
      case '\n':         /* end of line */ 
      case 0x0D:         /* end of line */ 
      case ';':          /* comment */ 
        *cursor = NIL;   
        --cursor;        /* neutralize ++cursor   */ 
        break;

      case ' ' :   /* skip blanks, commas and tabs */ 
      case '\t':                                                   /*#30*/ 
      case ',' :                                                   /*#30*/ 
        break;

      case '\'':                                                   /*#47*/ 
      case '"':
        argType = AT_string;
        rightquote = *cursor;                                      /*%06*/ 
        ++cursor;  /* skip " */ 
        status  = 3;
        break;

      case '(':
        argType = AT_string;
        ++plevel;
        ++cursor;  /* skip ( */ 
        rightquote = ')'; 
        startWithParen = true;
        status = 5;
        break;

      case ')':
        return cleanup (PRS_RC_too_many_closing_parenthesis, cursor);

      default:
        startWithParen = false;
        status  = 2;
      }/* end switch cursor */ 

      if (status != 1)
      {
        ae = malloc (sizeof(*ae));
        LI_addTail (&g_argList, (LI_Node *)ae);

        ae->argument.string = cursor;
        ae->argument.length = 1;
        ae->type            = argType;
        ae->closed          = false;
        argType             = AT_name;  /* initial for next arg. */ 
      }/*end-if status 1 */ 

      ++cursor;
      break;

    /******/ 
    case 2 :                   /* read argument */ 
    /******/ 
      rightquote  = NIL;                                             /*%08*/ 
      rightcursor = NULL;                                            /*%08*/ 
      switch (*cursor)
      {
      case 0x0D:         /* end of line */ 
      case '\n':         /* end of line */ 
      case ';':
        *cursor = NIL;
        break;
      case '=':
        status = 6;
        break;
      case ',':
        ++cursor; 
        status  = 1; 
        break;
      case ' ':                                                    /*#88*/ 
      case '\t':                                                   /*#30*/ 
        status = 4; 
        break;
      case '(':
        ++plevel;
        status = 5;
        ++(ae->argument.length);
        ++cursor;
        break;
      case ')':
        return cleanup (PRS_RC_too_many_closing_parenthesis, cursor);
      default:
        ++(ae->argument.length);
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
          ++(ae->argument.length);
      }/*end-switch cursor */ 

      ++cursor;
      break;

    /******/ 
    case 4 :                   /* overread blanks before '=' */      /*#88*/ 
    /******/ 
      switch (*cursor)
      {
      case 0x0D:         /* end of line */ 
      case '\n':         /* end of line */ 
      case ';':
        *cursor = NIL;
        break;
      case '=':
        status  = 6; 
        break;
      case ' ':
      case '\t':                                                   /*#30*/ 
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
      case ';':
        return cleanup (PRS_RC_too_many_opening_parenthesis, cursor);
      case '(':
        ++plevel;
        ++(ae->argument.length);
        ++cursor;
        break;

      case ')':
        --plevel;
        ++cursor;

        if (plevel == 0) 
        {
          if (! startWithParen) ++(ae->argument.length);
          startWithParen = false;
          status = 2;
        }
        else if (plevel < 0)
          return cleanup (PRS_RC_too_many_closing_parenthesis, cursor);
        else
          ++(ae->argument.length);
        
        break;

      default:
        ++(ae->argument.length);
        ++cursor;
      }/*end-switch cursor */ 

      break;

    /******/ 
    case 6 :                   /* handle '=' */ 
    /******/ 
        ++cursor; 
        argType = AT_string;    
        if (!ae) return cleanup (PRS_RC_unexpected_equal_sign, cursor);
        ae->type = AT_keyword;
        status = 1;
        break;

    /******/ 
    default:
    /******/ 
      return cleanup (PRS_RC_program_error_2, NULL);
    }/*end-switch status */ 

  }/*end-while *cursor */ 

  if (plevel > 0)
    return cleanup (PRS_RC_too_many_opening_parenthesis, cursor);

  if (rightcursor) *rightcursor = NIL;

  if (LI_isEmpty (&g_argList)) 
    return cleanup (PRS_RC_empty, cursor);

  #ifdef xtest
    printf("\n---testausgabe arg List:\n(arguments='%s')\n",
      p_arguments);

    ae = (struct AE *) LI_getFirst (&g_argList);
    while (ae)
    {
      printf (" argument=%s^ \n",ae->argument.string );
      printf (" length  =%d  \n",ae->argument.length   );
      printf (" type    =%c  \n",ae->type     );
      ae = (struct AE *) LI_getNext (&g_argList);
    }
    printf ("---end-list\n");
  #endif  /*test*/ 


  /**************************************************************/ 
  /*  3.1 loop over argument list:                              */ 
  /*     only forced (=) keywords: for every =keywords in       */ 
  /*     argument list: search it in template/name list.        */ 
  /*     if not found: error "keyword unknown"                  */ 
  /**************************************************************/ 
  DEBUG ("\n--- step 3.1 ---\n");

  for (ae = (AE*) LI_getFirst (&g_argList); 
       ae; 
       ae = (AE*) LI_getNext (&g_argList)
    ) 
  {
    if (ae->type != AT_keyword) continue;

    found = false;
    for (te = (TE*) LI_getFirst (&g_templList); 
         te && ! found; 
         te = (TE*) LI_getNext (&g_templList)
        ) 
    {
      /* loop over keyword aliases */ 
      for (ne = (NE*) LI_getFirst (&te->nameList); 
           ne && ! found;
           ne = (NE*) LI_getNext (&te->nameList)
          )
      {
        if (ne->type != NT_keyword) continue;     /* only keywords */ 
        if (! (found = isEqual (ne->name, ae->argument) ))  continue;

      }/*next ne */ 
    }/*next te */ 

    if (! found && ! opt_last_parm_given) 
      return cleanup (PRS_RC_unknown_keyword, ae->argument.string);

  }/*next ae */ 

  /**************************************************************/ 
  /*  3.2 loop over template list:                              */ 
  /*     only keywords: search keywords in argument list.       */ 
  /*     if found: assign next argument to template element.    */ 
  /*     check requested keywords (error)                       */ 
  /**************************************************************/ 
  DEBUG ("\n--- step 3.2 ---\n");

  for (te = (TE*) LI_getFirst (&g_templList); 
       te; 
       te = (TE*) LI_getNext (&g_templList)
      ) 
  {
    /* loop over keyword aliases */ 
    found = false;
    for (ne = (NE*) LI_getFirst (&te->nameList); 
         ne && ! found;
         ne = (NE*) LI_getNext (&te->nameList)
        )
    {
      if (ne->type != NT_keyword) continue;     /* only keywords */ 

      rc = search_argument (te, ne, p_container);
      switch (rc)
      {
      case  0: continue;
      case -1: found = true; break;
      default: return cleanup (rc, namestr (ne));
      }
    }/*end-for NE */ 

    if (! found && (te->options & MASK_R) && (te->options & MASK_K)) 
      return cleanup (PRS_RC_keyword_param_requested, namestr (ne));

  } /*end-for TE */ 

  #ifdef test
      printf("\n---testausgabe templ List:\n");
      te = (struct TE *) LI_getFirst (&g_templList);
      
      while (te)
      {
        printf ("c_index=%d, options=%1X, type =%c, closed=%d\n ",
          te->c_index, te->options, te->parmType, te->closed);

        te = (struct TE *) LI_getNext (&g_templList);
      }
      printf ("---end-list\n");

  #endif /*test*/ 

  /**************************************************************/ 
  /*  4. loop over template list:                               */ 
  /*     check requested keywords (close TE)                    */ 
  /*     match only symbols to arguments                        */ 
  /*     check requested symbols (error)                        */ 
  /**************************************************************/ 
  DEBUG ("\n--- step 4 ---\n");

  for (te = (TE*) LI_getFirst (&g_templList); 
       te; 
       te = (TE*) LI_getNext (&g_templList)
      ) 
  {
    bool symbols_defined = false;

    if (te->closed)       continue;
    if (te->options & MASK_K) /* if keyword requested... */ 
    {
      te->closed = true;      /* keyword not found in arglist in step 3 */ 
      continue;
    }

    found = false;

    for (ne = (NE*) LI_getFirst (&te->nameList); 
         ne && ! found;
         ne = (NE*) LI_getNext (&te->nameList)
        )
    { 
      if (ne->type != NT_symbol) continue;
      symbols_defined = true;

      rc = search_argument (te, ne, p_container);
      switch (rc)
      {
      case  0: break;
      case -1: found = true; break;
      default: return cleanup (rc, namestr (ne));
      }

    }/*end-for next NE */ 

    if (symbols_defined && ! found)
    {
      if (te->options & MASK_R)
        return cleanup (PRS_RC_keyword_param_requested, namestr (ne)); 
      else
        te->closed = true;
    }
         
  } /*end-for TE */ 

  #ifdef test
      printf("\n---testausgabe templ List:\n");
      te = (struct TE *) LI_getFirst (&g_templList);
      
      while (te)
      {
        printf ("c_index=%d, options=%1X, type =%c, closed=%d\n ",
          te->c_index, te->options, te->parmType, te->closed);

        te = (struct TE *) LI_getNext (&g_templList);
      }
      printf ("---end-list\n");
      printf("\n---testausgabe argument List:\n");
      
      
      for (ae = (AE*) LI_getFirst (&g_argList);
           ae;
           ae = (AE*) LI_getNext (&g_argList))

      {
        printf ("arg=(%d)'%s', closed=%d\n ",
          ae->argument.length, ae->argument.string, ae->closed);
      }
      printf ("---end-list\n");

  #endif /*test*/ 


  /**************************************************************/ 
  /*  5. balancing remaining entries templates vs. arguments    */ 
  /*     (but not for booleans, they are handled in step 3)     */ 
  /*     (Toggles ar checked against proper values)             */ 
  /**************************************************************/ 

  DEBUG ("\n--- step 5 ---\n");

  ae = (AE*) LI_getFirst (&g_argList);
  te = (TE*) LI_getFirst (&g_templList);

  while (ae && te)
  {
    char *str;

    if (ae->closed)
    {
      ae = (AE*) LI_getNext (&g_argList);
      continue;
    }

    if (te->closed || te->parmType == PT_switch || (te->options & MASK_K) ) 
    {
      te = (TE*) LI_getNext (&g_templList);
      continue;
    }
    else if (te->parmType == PT_numeric)
    {
      if (! isNumeric (ae->argument))
      {
        te = (TE*) LI_getNext (&g_templList);
        continue;
      }
    }
    else if (te->parmType == PT_toggle) 
    {
      NameT toggels [4] = {{"ON",2}, {"OFF",3}, {"YES",3}, {"NO",2}};
      int ix = 0;

      while (ix < 4 && ! found)
        if (! (found = isEqual (toggels [ix], ae->argument))) ++ix;

      if (! found)
      {
        te = (TE *) LI_getNext (&g_templList);
        continue;
      }
    } /*end-if closed, parmtype Switch or Toggle */ 

    ae->closed    = true;
    te->closed    = true;
    te->stringArg = true;

    str = ae->argument.string;
    p_container [te->c_index] = (long) str; 
    
    if (te->options & (MASK_L)) 
    {
      ae = (AE*) LI_getLast (&g_argList);
      str = ae->argument.string;
      str [ae->argument.length] = NIL;
      ae =NULL;
      break;
    }

    str [ae->argument.length] = NIL;

    te = (TE *) LI_getNext (&g_templList);
    ae = (AE *) LI_getNext (&g_argList);

  }/*end-while ae and te */ 

  if (ae) return cleanup (PRS_RC_too_much_args, ae->argument.string);


  /****************************************************/ 
  /*  6. processing arguments (numerics, etc.?)       */ 
  /*     check templList for options = R              */ 
  /*     Conversions for option = X, N and T          */ 
  /*     NIL-Byte for string argument                 */ 
  /****************************************************/ 

  DEBUG ("--- step 6 ---\n");

  te = (TE*) LI_getFirst (&g_templList);
  while (te)
  {
    char *arg = (char *)p_container [te->c_index];

    if ( (! te->closed) && (te->options & MASK_R) )
      return cleanup (PRS_RC_too_few_args, arg);

    else if (!te->closed)
    {}

    else if (te->parmType == PT_numeric && te->stringArg)
    {
      unsigned int ix;

      value  = 0;
      for (ix = 0; ix < strlen (arg); ++ix )
      {
        workchar = arg [ix];

        if (workchar < '0' || workchar > '9')
          return cleanup (PRS_RC_numeral_expected, arg);

        value = 10 * value + workchar - '0';
      }/* next ix */ 

      p_container [te->c_index] = value;
    }/*end-if option N */ 

    else if ( te->parmType == PT_name 
              && arg
              && te->stringArg
            )
    {
      AE ae;
      ae.argument.string = arg;
      ae.argument.length = strlen (ae.argument.string);

      ne = (NE*) LI_getFirst (&te->nameList);
      found = false;

      while (ne && ! found)
      {
        if (ne->type != NT_symbol 
        || ! (found = isEqual (ne->name, ae.argument)))
          ne = (NE*) LI_getNext (&te->nameList);
      }/* end-while namelist */ 

      if (found)
        p_container [te->c_index] = ne->l_index;
      else
        return cleanup (PRS_RC_wrong_param_after_keyword, namestr (ne));
    }

    else if (te->parmType == PT_toggle && arg && te->stringArg)
    {
      if (toupper (arg [0]) == 'O')
      {
        if (toupper (arg [1]) == 'N')
          p_container [te->c_index] = 1;
        else if (toupper (arg [1]) == 'F')
          p_container [te->c_index] = 0;
        else
          return cleanup (PRS_RC_wrong_toggle_arg, arg);
      }
      else if (toupper (arg [0]) == 'Y')
      {
        p_container [te->c_index] = 1;
      }
      else if (toupper (arg [0]) == 'N')
      {
        p_container [te->c_index] = 0;
      }
      else 
      {
        return cleanup (PRS_RC_wrong_toggle_arg, arg);
      }

    }/*end-if  */ 

    te = (struct TE *) LI_getNext (&g_templList);
  }/*end-while te */ 


  DEBUG ("\n--- end parser ---\n");

  return cleanup (PRS_RC_ok, NULL);

}/*end-function BT_parse */ 

/******************************************************************/ 
char * BT_parmjoin (char **p_argv)
/* joining cmd line parameters without the 1st (prog name) */ 
{
  static int   plen  = 0;
  static char *param = NULL;
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
    if (param) free (param);
    param = malloc (len);  
    plen  = len;
  }
  
  param [0] = 0;

  for (ix = FIRST; ix < MAXARGS && p_argv[ix]; ++ix)
  {
    strcat (param, " ");
    strcat (param, p_argv[ix]);
  }

  return param;

}/*end-BT_parmjoin */ 

/******************************************************************/ 
char * BT_join (char **p_argv)
/* joining cmd line parameters including the 1st (prog name) */ 
{
  static int   plen  = 0;   
  static char *param = NULL;
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
    if (param) free (param);
    param = malloc (len);  
    plen  = len;
  }
  
  param [0] = 0;

  #ifdef _SYS_MVS

    strcat (param, "*PROGRAM");

  #endif  /* _SYS_MVS */ 

  for (ix=0; ix < MAXARGS && p_argv[ix]; ++ix)
  {
    strcat (param, " ");
    strcat (param, p_argv[ix]);
  }

  return param;

}/*end-BT_join */ 

/******************************************************************/ 
char * BT_parserr (void)
{
  #define TEMPLEN 100
  static char temp [TEMPLEN];

  static char *argmess [] =
  {
     "ok"                             /*  0 */ 
    ,"empty"                          /*  1 */ 
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
    ,"empty"                          /* 86 */ 
    ,"unexpected nameList"            /* 87 */  
    ,"unexpected blank after keyword" /* 88 */ 
    ,"semicolon unexpected"           /* 89 */ 
    ,"equal sign unexpected"          /* 90 */ 
    ,"only one keyword allowed"       /* 91 */ 
  };

  if (g_rc >= 0 && g_rc < PRS_NUMBER_rc)
  {
    if (g_rc == PRS_RC_empty)
      strncpy (temp,  argmess [g_rc], TEMPLEN);
    else  
      sprintf (temp, "%s nearby '%s'", argmess [g_rc], g_errtoken);
  }
  else
  {
    g_rc -= PRS_RC_te_base;
    if (g_rc >= 0 && g_rc < PRS_NUMBER_te)
      sprintf (temp, "_parse__ error template: %s nearby '%s'", 
        temess [g_rc], g_errtoken);
    else
      sprintf (temp, "internal _parse__ error %d", g_rc + PRS_RC_te_base - 100);
  }

  return temp;

}/*end-BT_parserr */ 

#undef  DEBUG
#define DEBUG
/******************************************************************/ 
static bool isEqual (NameT p_sn1, NameT p_sn2)
{
  int ix;

  DEBUG ("isEqual?: (%d)%s : (%d)%s === ",
      p_sn1.length, p_sn1.string, p_sn2.length, p_sn2.string);

  if (! p_sn1.string || ! p_sn2.string) return false;
  if (  p_sn1.length !=   p_sn2.length) 
  {
    DEBUG  ("no\n");
    return false;
  }

  for (ix=0; ix < p_sn2.length; ++ix)
    if (p_sn1.string [ix] != toupper (p_sn2.string [ix]))
    {
      DEBUG  ("no\n");
      return false;
    }

    DEBUG  ("yes\n");

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
static struct TE *newTE 
    ( 
      char  p_parmType
    , Byte  p_options 
    , int   p_contIndex
    )
{
  struct TE *te;

  te = malloc (sizeof(*te));
  if (! te) 
  {
    printf ("Parser: no more memory!\n");
    abort ();
  }

  LI_addTail (&g_templList, (LI_Node *)te);

  LI_init (&te->nameList);

  te->parmType  = p_parmType ? p_parmType : 'S';
  te->options   = p_options;
  te->c_index   = p_contIndex;
  te->closed    = false;
  te->stringArg = false;

  return te; 
}/*end-newTE */ 

/******************************************************************/ 
static NE *newNE 
  ( 
    TE   *p_te
  , char *p_cursor
  , char  p_type
  , int   p_listIndex
  )
{
  struct NE *ne;

  ne = malloc (sizeof(*ne));
  if (! ne) 
  {
    printf ("Parser: no more memory!\n");
    abort ();
  }
  
  ne->name.string = p_cursor;
  ne->name.length = 1;
  ne->l_index     = p_listIndex;
  ne->type        = p_type; 

  LI_addTail (&p_te->nameList, (LI_Node *)ne);
  return ne; 
}/*end-newNE */ 

/******************************************************************/ 
static int cleanup (int p_rc, char *p_entry)
{
  struct TE *te1;
  struct NE *ne1;
  AE        *ae1;
  LI_List   *nList; 

  g_rc = p_rc;
  g_errtoken [0] = 0;

  if (p_entry) 
    strncpy (g_errtoken, p_entry, ERRTOKLEN);
  else
    g_errtoken [0] = 0;
  
  te1 = NULL; 
  while (te1 = (TE *)LI_remHead (&g_templList)) 
  { 
    nList = &te1->nameList;
    while (ne1 = (NE *)LI_remHead (nList)) free (ne1); 
    free (te1); 
  };

  while (ae1 = (AE *)LI_remHead (&g_argList)) free (ae1);

  return g_rc;


}/*end-cleanup */ 

/******************************************************************/ 
char *namestr (NE* p_ne)
{
  static char str [100] = "?";
  int         len;
 
  if (! p_ne) return str;

  len = p_ne->name.length;
  strncpy (str, p_ne->name.string, len);
  str [len] = 0;
  return str;

} /*end- namestr */ 
 
/******************************************************************/ 
int search_argument (TE* p_te, NE* p_ne, long *p_container )
/*  returns: -1 found
              0 not found
             >0 error code 
*/ 
{
  AE*  ae;
  bool found = false;
  int  contIndex = -1;

  for (ae = (AE*) LI_getFirst (&g_argList); 
       ae && ! found; 
       ae = (AE*) LI_getNext (&g_argList)
    ) 
  {
    if (ae->closed || ae->type == AT_string) continue;
    if (! (found = isEqual (p_ne->name, ae->argument) ))  continue;

    /* we found a match between template keyword and an arg. token: */ 
    contIndex    = p_te->c_index;
    p_te->closed = true;
    ae->closed   = true;

    if (p_te->parmType == PT_switch)
    {
      p_container [contIndex] = true;          /* switch is on  */ 
      return -1;
    }

    if (p_ne->type == NT_keyword)
    { 
      if (! (ae = (AE*) LI_getNext (&g_argList)))  /* argument after keyword */ 
        return PRS_RC_no_value_after_keyword;
    }

    ae->closed              = true;
    p_te->stringArg         = true;
    p_container [contIndex] = (long)(ae->argument.string);

    /* keyword found. but TE points to the argument string    */ 
    /* analysis and correct setting of container in section 6 */ 

    if (p_te->options & MASK_L) 
    {    /* close remaining AE-nodes */ 
      for (ae = (AE*)ae->node.next; ae; ae = (AE*)ae->node.next)
        ae->closed = true;
    }
    else
    {
      char *str = ae->argument.string;
      str [ae->argument.length] = NIL;
    }/*end-if "Last Argument" or not */ 

  }/*end-for next AE */ 

  return found ? -1 : 0;
}/*end- search_argument */ 



/**** end *******/ 

