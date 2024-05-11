/* doc----------------------------------------------------------------
:Project.   common tools
:               
:Program.   lists
:PrType.    header file
:Title.     tools for list processing
:Version.   1.0
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI-C
:Compiler.
:Created.   1998-10-17 
*/

#ifndef lists_h
#define lists_h

#include "sysdef.h"
#include <stdlib.h>

typedef struct LI_Node
{
  struct LI_Node *next, *prev;

} LI_Node;

typedef struct LI_List
{
  LI_Node  head;  
  LI_Node  tail; 
  LI_Node *current;

} LI_List;

typedef bool LI_DoProc   (LI_Node *);                   /*!!!typedefs*/ 
typedef bool LI_LessProc (LI_Node *, LI_Node *);        /*!!!typedefs*/ 
  /* returns True if 1st node is less than 2nd */ 

#ifdef _SYS_MAINFRAME
#define  LI_isEmpty    LIisEmpt    
#define  LI_sort       LIsort       
#define  LI_addHead    LIaddHed    
#define  LI_addTail    LIaddTel    
#define  LI_remove     LIremove     
#define  LI_addBefore  LIaddBef
#define  LI_addBehind  LIaddBeh
#define  LI_append     LIappend     
#define  LI_init       LIinit     
#define  LI_remHead    LIremHed  
#define  LI_remTail    LIremTel  
#define  LI_getFirst   LIgtFrst
#define  LI_getLast    LIgtLast 
#define  LI_getNext    LIgtNext 
#define  LI_getPrev    LIgtPrev 
#define  LI_doForward  LIdoForw
#define  LI_doBackward LIdoBack

#endif 

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

DllExport  bool LI_isEmpty    (LI_List *);
DllExport  bool LI_sort       (LI_List *, LI_LessProc *);
              /* returns False if list is empty */ 
DllExport  bool LI_addHead    (LI_List *, LI_Node *);
DllExport  bool LI_addTail    (LI_List *, LI_Node *);
DllExport  bool LI_remove     (LI_Node *);
DllExport  bool LI_addBefore  (LI_Node *, LI_Node *);
DllExport  bool LI_addBehind  (LI_Node *, LI_Node *);
DllExport  bool LI_append     (LI_List *, LI_List *);
           bool LI_setCurrent (LI_List *, LI_Node *);

DllExport  LI_List *LI_init   (LI_List *);
                  /* if param == NULL then memory for struct LI_List */ 
                  /*   will be allocated (and returned)              */   

DllExport  LI_Node *LI_remCurrent (LI_List *); /* and points to prev */ 
DllExport  LI_Node *LI_remHead    (LI_List *);
DllExport  LI_Node *LI_remTail    (LI_List *);  
/*  
    LI_rem... returns the adress of the removed Node
    if list is empty LI_rem... returns NULL
    remCurrent may be used inside a getNext loop
*/ 

DllExport  LI_Node *LI_getFirst (LI_List *);
DllExport  LI_Node *LI_getLast  (LI_List *);
DllExport  LI_Node *LI_getNext  (LI_List *);
DllExport  LI_Node *LI_getPrev  (LI_List *);
/*
    all LI_get... function return NULL if end of list was reached
*/ 

DllExport  LI_Node *LI_doForward  (LI_List *, LI_DoProc *);
DllExport  LI_Node *LI_doBackward (LI_List *, LI_DoProc *);
/* both do...ward functions returns NULL if all nodes results False
   by the DoProc function.
   They results the pointer of the first node which results True by 
   the DoProc function.
   This side effects are useful for searching.
*/

#ifdef __cplusplus
}//end-extern-C
#endif /*__cplusplus*/

#endif /*def lists_h */  
